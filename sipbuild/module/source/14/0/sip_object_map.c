/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This module implements a hash table class for mapping C/C++ addresses to the
 * corresponding wrapped Python object.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stdint.h>
#include <string.h>

#include "sip_object_map.h"

#include "sip_core.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper_type.h"


#define hash_1(k,s) (((uintptr_t)(k)) % (s))
#define hash_2(k,s) ((s) - 2 - (hash_1((k),(s)) % ((s) - 2)))


/* Prime numbers to use as hash table sizes. */
static const uintptr_t hash_primes[] = {
    521,        1031,       2053,       4099,
    8209,       16411,      32771,      65537,      131101,     262147,
    524309,     1048583,    2097169,    4194319,    8388617,    16777259,
    33554467,   67108879,   134217757,  268435459,  536870923,  1073741827,
    2147483659U,0
};


/* Forward declarations. */
static void add_aliases(sipModuleState *ms, sipSimpleWrapper *obj, void *addr,
        const sipTypeSpec *ts);
static void add_object(sipModuleState *ms, sipSimpleWrapper *obj, void *addr);
static sipHashEntry *find_hash_entry(sipObjectMap *om, void *key);
static sipHashEntry *new_hash_table(uintptr_t size);
static void remove_aliases(sipModuleState *ms, sipSimpleWrapper *obj,
        void *addr, const sipTypeSpec *ts);
static int remove_object(sipModuleState *ms, sipSimpleWrapper *obj,
        void *addr);
static void reorganise_map(sipObjectMap *om);


/*
 * Initialise an object map.
 */
int sip_om_init(sipObjectMap *om)
{
    om->prime_idx = 0;
    om->unused = om->size = hash_primes[om->prime_idx];
    om->stale = 0;

    if ((om->hash_array = new_hash_table(om->size)) == NULL)
        return -1;

    return 0;
}


/*
 * Finalise an object map.
 */
void sip_om_finalise(sipObjectMap *om)
{
    if (om->hash_array != NULL)
        sip_api_free(om->hash_array);
}


/*
 * Allocate and initialise a new hash table.  Return NULL on error.
 */
static sipHashEntry *new_hash_table(uintptr_t size)
{
    size_t nbytes;
    sipHashEntry *hashtab;

    nbytes = sizeof (sipHashEntry) * size;

    if ((hashtab = (sipHashEntry *)sip_api_malloc(nbytes)) != NULL)
        memset(hashtab, 0, nbytes);

    return hashtab;
}


/*
 * Return a pointer to the hash entry that is used, or should be used, for the
 * given C/C++ address.
 */
static sipHashEntry *find_hash_entry(sipObjectMap *om, void *key)
{
    uintptr_t hash, inc;
    void *hek;

    hash = hash_1(key, om->size);
    inc = hash_2(key, om->size);

    while ((hek = om->hash_array[hash].key) != NULL && hek != key)
        hash = (hash + inc) % om->size;

    return &om->hash_array[hash];
}


/*
 * Return a strong reference to the wrapped Python object of a specific type
 * for a C/C++ address or NULL if it wasn't found.  The main mutex must be
 * locked.
 */
PyObject *sip_om_find_object(sipObjectMap *om, void *key, PyTypeObject *w_type)
{
    sipHashEntry *he = find_hash_entry(om, key);
    sipSimpleWrapper *sw = he->first;

    /* Go through each wrapped object at this address. */
    while (sw != NULL)
    {
        PyObject *unaliased = (PyObject *)(sipIsAlias(sw) ? sw->data : sw);

        /*
         * If the reference count is 0 then it is in the process of being
         * deleted, so ignore it.  It's not completely clear how this can
         * happen (but it can) because it implies that the garbage collection
         * code is being re-entered (and there are guards in place to prevent
         * this).
         */
        if (Py_REFCNT(unaliased) == 0)
            goto next_object;

        /* Ignore it if the C/C++ address is no longer valid. */
        if (sip_api_get_address((sipSimpleWrapper *)unaliased) == NULL)
            goto next_object;

        /*
         * If this wrapped object is of the given type, or a sub-type of it,
         * then we assume it is the same C++ object.
         */
        if (PyObject_TypeCheck(unaliased, w_type))
            return Py_NewRef(unaliased);

next_object:
        sw = sw->next;
    }

    return NULL;
}


/*
 * Add a C/C++ address and the corresponding wrapped Python object to the map.
 * The main mutex must be locked.
 */
void sip_om_add_object(sipModuleState *ms, sipSimpleWrapper *obj)
{
    /* Add the object. */
    add_object(ms, obj, obj->data);

    /* Add any aliases. */
    add_aliases(ms, obj, obj->data,
            sip_get_type_spec_from_wt(
                    (sipWrapperType *)Py_TYPE((PyObject *)obj)));
}


/*
 * Add an alias for any address that is different when cast to a super-type.
 */
static void add_aliases(sipModuleState *ms, sipSimpleWrapper *obj, void *addr,
        const sipTypeSpec *ts)
{
    const sipClassTypeSpec *cts = (const sipClassTypeSpec *)ts;
    const sipTypeID *supers = cts->supers;

    /* See if there are any super-classes. */
    if (supers != NULL)
    {
        sipTypeID sup_type_id = *supers++;

        PyObject *sup_mod;
        const sipTypeSpec *sup_ts = sip_get_type_spec(ms, sup_type_id,
                &sup_mod);
        assert(sup_mod != NULL);

        /* Recurse up the hierachy for the first super-class. */
        add_aliases(sip_get_module_state(sup_mod), obj, addr, sup_ts);
        Py_DECREF(sup_mod);

        /*
         * We only check for aliases for subsequent super-classes because the
         * first one can never need one.
         */
        sipWrapperType *wt = (sipWrapperType *)Py_TYPE((PyObject *)obj);
        sipCastFunc cast = ((const sipClassTypeSpec *)sip_get_type_spec_from_wt(
                wt))->cast;

        while (!sipTypeIDIsSentinel(sup_type_id))
        {
            sup_type_id = *supers++;

            sup_ts = sip_get_type_spec(ms, sup_type_id, &sup_mod);
            assert(sup_mod != NULL);

            /* Recurse up the hierachy for the remaining super-classes. */
            sipModuleState *sup_ms = sip_get_module_state(sup_mod);
            add_aliases(sup_ms, obj, addr, sup_ts);

            void *sup_addr = cast(sup_ms, addr,
                    (const sipClassTypeSpec *)sup_ts);

            if (sup_addr != addr)
            {
                sipSimpleWrapper *alias = sip_api_malloc(
                        sizeof (sipSimpleWrapper));

                /* Note that we silently ignore errors. */
                if (alias != NULL)
                {
                    /*
                     * An alias is basically a bit-wise copy of the Python
                     * object but only to ensure the fields we are subverting
                     * are in the right place.  An alias should never be passed
                     * to the Python API.
                     */
                    *alias = *obj;

                    alias->flags = (obj->flags & SIP_SHARE_MAP) | SIP_ALIAS;
                    alias->data = obj;
                    alias->next = NULL;

                    add_object(ms, alias, sup_addr);
                }
            }

            Py_DECREF(sup_mod);
        }
    }
}


/*
 * Add a wrapper (which may be an alias) to the map.
 */
static void add_object(sipModuleState *ms, sipSimpleWrapper *obj, void *addr)
{
    sipSipModuleState *sms = ms->sip_module_state;
    sipHashEntry *he = find_hash_entry(&sms->object_map, addr);

    /*
     * If the bucket is in use then we appear to have several objects at the
     * same address.
     */
    if (he->first != NULL)
    {
        /*
         * This can happen for four reasons.  A variable of one class can be
         * declared at the start of another class.  Therefore there are two
         * objects, of different classes, with the same address.  The second
         * reason is that the old C/C++ object has been deleted by C/C++ but we
         * didn't get to find out for some reason, and a new C/C++ instance has
         * been created at the same address.  The third reason is if we are in
         * the process of deleting a Python object but the C++ object gets
         * wrapped again because the C++ dtor called a method that has been
         * re-implemented in Python.  The fourth reason is that the user has
         * called sipConvertFromNewType() for a C/C++ object that isn't
         * actually new.  The presence of the SIP_SHARE_MAP flag (set by
         * sipConvertFromType()) means that we are OK with this and we just add
         * this one to the list of existing objects at this address.
         * Otherwise there is no right way of handling the situation so (for
         * historical reasons) we choose to isolate the wrapper and let it die
         * a natural death.
         */
        if (!(obj->flags & SIP_SHARE_MAP))
        {
            sipSimpleWrapper *sw = he->first;

            he->first = NULL;

            while (sw != NULL)
            {
                sipSimpleWrapper *next = sw->next;

                if (sipIsAlias(sw))
                    sip_api_free(sw);
                else
                    sip_isolate_wrapper(ms, sw);

                sw = next;
            }
        }

        obj->next = he->first;
        he->first = obj;

        return;
    }

    /* See if the bucket was unused or stale. */
    if (he->key == NULL)
    {
        he->key = addr;
        sms->object_map.unused--;
    }
    else
    {
        sms->object_map.stale--;
    }

    /* Add the rest of the new value. */
    he->first = obj;
    obj->next = NULL;

    reorganise_map(&sms->object_map);
}


/*
 * Reorganise a map if it is running short of space.  An inability to increase
 * the size of the map isn't considered an error.
 */
static void reorganise_map(sipObjectMap *om)
{
    /* Don't bother if it still has more than 12% available. */
    if (om->unused > om->size >> 3)
        return;

    /*
     * If reorganising (ie. making the stale buckets unused) using the same
     * sized table would make 25% available then do that.  Otherwise use a
     * bigger table (if possible).
     */
    int new_prime_idx = om->prime_idx;

    if (om->unused + om->stale < om->size >> 2 && hash_primes[om->prime_idx + 1] != 0)
        new_prime_idx++;

    /* Allocate the new hash array. */
    uintptr_t new_size = hash_primes[new_prime_idx];
    sipHashEntry *new_hash_array = new_hash_table(new_size);
    if (new_hash_array == NULL)
        return;

    /*
     * Nothing can fail now so transfer the entries from the old array to the
     * new one and update the metadata.
     */
    sipHashEntry *old_hash_array = om->hash_array;
    om->hash_array = new_hash_array;

    uintptr_t old_size = om->size;
    om->unused = om->size = new_size;

    om->prime_idx = new_prime_idx;
    om->stale = 0;

    uintptr_t i;

    for (i = 0; i < old_size; i++)
    {
        if (old_hash_array->key != NULL && old_hash_array->first != NULL)
        {
            *find_hash_entry(om, old_hash_array->key) = *old_hash_array;
            om->unused--;
        }

        old_hash_array++;
    }

    sip_api_free(old_hash_array);
}


/*
 * Remove a C/C++ object from the table.  Return 0 if it was removed
 * successfully.  The main mutex must be locked.
 */
int sip_om_remove_object(sipModuleState *ms, sipSimpleWrapper *obj)
{
    /* Remove any aliases. */
    remove_aliases(ms, obj, obj->data,
            sip_get_type_spec_from_wt(
                    (sipWrapperType *)Py_TYPE((PyObject *)obj)));

    /* Remove the object. */
    return remove_object(ms, obj, obj->data);
}


/*
 * Remove an alias for any address that is different when cast to a super-type.
 */
static void remove_aliases(sipModuleState *ms, sipSimpleWrapper *obj,
        void *addr, const sipTypeSpec *ts)
{
    const sipClassTypeSpec *cts = (const sipClassTypeSpec *)ts;
    const sipTypeID *supers = cts->supers;

    /* See if there are any super-classes. */
    if (supers != NULL)
    {
        sipTypeID sup_type_id = *supers++;

        PyObject *sup_mod;
        const sipTypeSpec *sup_ts = sip_get_type_spec(ms, sup_type_id,
                &sup_mod);
        assert(sup_mod != NULL);

        /* Recurse up the hierachy for the first super-class. */
        remove_aliases(sip_get_module_state(sup_mod), obj, addr, sup_ts);
        Py_DECREF(sup_mod);

        /*
         * We only check for aliases for subsequent super-classes because the
         * first one can never need one.
         */
        sipWrapperType *wt = (sipWrapperType *)Py_TYPE((PyObject *)obj);
        sipCastFunc cast = ((const sipClassTypeSpec *)sip_get_type_spec_from_wt(
                wt))->cast;

        while (!sipTypeIDIsSentinel(sup_type_id))
        {
            sup_type_id = *supers++;

            sup_ts = sip_get_type_spec(ms, sup_type_id, &sup_mod);
            assert(sup_mod != NULL);

            sipModuleState *sup_ms = sip_get_module_state(sup_mod);

            /* Recurse up the hierachy for the remaining super-classes. */
            remove_aliases(sup_ms, obj, addr, sup_ts);

            void *sup_addr = cast(sup_ms, addr,
                    (const sipClassTypeSpec *)sup_ts);

            if (sup_addr != addr)
                remove_object(ms, obj, sup_addr);

            Py_DECREF(sup_mod);
        }
    }
}


/*
 * Remove a wrapper from the map.
 */
static int remove_object(sipModuleState *ms, sipSimpleWrapper *obj, void *addr)
{
    sipSipModuleState *sms = ms->sip_module_state;
    sipHashEntry *he = find_hash_entry(&sms->object_map, addr);
    sipSimpleWrapper **sw_p;

    for (sw_p = &he->first; *sw_p != NULL; sw_p = &(*sw_p)->next)
    {
        sipSimpleWrapper *sw = *sw_p;
        sipSimpleWrapper *next = sw->next;
        int do_remove;

        if (sipIsAlias(sw))
        {
            if (sw->data == obj)
            {
                sip_api_free(sw);
                do_remove = TRUE;
            }
            else
            {
                do_remove = FALSE;
            }
        }
        else
        {
            do_remove = (sw == obj);
        }

        if (do_remove)
        {
            *sw_p = next;

            /*
             * If the bucket is now empty then count it as stale.  Note that we
             * do not NULL the key and count it as unused because that might
             * throw out the search for another entry that wanted to go here,
             * found it already occupied, and was put somewhere else.  In other
             * words, searches must be repeatable until we reorganise the
             * table.
             */
            if (he->first == NULL)
                sms->object_map.stale++;

            return 0;
        }
    }

    return -1;
}


/*
 * Call a visitor function for every wrapped object.  The main mutex must be
 * locked.
 */
void sip_om_visit_wrappers(sipObjectMap *om, sipWrapperVisitorFunc visitor,
        void *closure)
{
    const sipHashEntry *he;
    uintptr_t i;

    for (he = om->hash_array, i = 0; i < om->size; ++i, ++he)
    {
        if (he->key != NULL)
        {
            sipSimpleWrapper *sw;

            for (sw = he->first; sw != NULL; sw = sw->next)
                visitor(sw, closure);
        }
    }
}
