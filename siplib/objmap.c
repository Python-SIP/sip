/*
 * This module implements a hash table class for mapping C/C++ addresses to the
 * corresponding wrapped Python object.
 *
 * @BS_LICENSE@
 */


#include <string.h>

#include "sip.h"
#include "sipint.h"


#define hash_1(k,s) (((unsigned long)(k)) % (s))
#define hash_2(k,s) ((s) - 2 - (hash_1((k),(s)) % ((s) - 2)))


/* Prime numbers to use as hash table sizes. */
static unsigned long hash_primes[] = {
    521,        1031,       2053,       4099,
    8209,       16411,      32771,      65537,      131101,     262147,
    524309,     1048583,    2097169,    4194319,    8388617,    16777259,
    33554467,   67108879,   134217757,  268435459,  536870923,  1073741827,
    2147483659U,0
};


static sipHashEntry *newHashTable(unsigned long);
static sipHashEntry *findHashEntry(sipObjectMap *,void *);
static void reorganiseMap(sipObjectMap *om);


/*
 * Initialise an object map.
 */
void sipOMInit(sipObjectMap *om)
{
    om -> primeIdx = 0;
    om -> unused = om -> size = hash_primes[om -> primeIdx];
    om -> stale = 0;
    om -> hash_array = newHashTable(om -> size);
}


/*
 * Finalise an object map.
 */
void sipOMFinalise(sipObjectMap *om)
{
    sip_api_free(om -> hash_array);
}


/*
 * Allocate and initialise a new hash table.
 */
static sipHashEntry *newHashTable(unsigned long size)
{
    size_t nbytes;
    sipHashEntry *hashtab;

    nbytes = sizeof (sipHashEntry) * size;

    if ((hashtab = (sipHashEntry *)sip_api_malloc(nbytes)) != NULL)
        memset(hashtab,0,nbytes);

    return hashtab;
}


/*
 * Return a pointer to the hash entry that is used, or should be used, for the
 * given C/C++ address.
 */
static sipHashEntry *findHashEntry(sipObjectMap *om,void *key)
{
    unsigned long hash, inc;
    void *hek;

    hash = hash_1(key,om -> size);
    inc = hash_2(key,om -> size);

    while ((hek = om -> hash_array[hash].key) != NULL && hek != key)
        hash = (hash + inc) % om -> size;

    return &om -> hash_array[hash];
}


/*
 * Return the wrapped Python object of a specific type for a C/C++ address or
 * NULL if it wasn't found.
 */
sipWrapper *sipOMFindObject(sipObjectMap *om, void *key, sipWrapperType *type)
{
    sipHashEntry *he = findHashEntry(om, key);
    sipWrapper *w;

    /* Go through each wrapped object at this address. */
    for (w = he->first; w != NULL; w = w->next)
    {
        /*
         * If this wrapped object is of the given type, or a sub-type of it,
         * then we assume it is the same C++ object.
         */
        if (PyObject_TypeCheck(w, (PyTypeObject *)type))
            return w;
    }

    return NULL;
}


/*
 * Add a C/C++ address and the corresponding wrapped Python object to the map.
 */
void sipOMAddObject(sipObjectMap *om,sipWrapper *val)
{
    sipHashEntry *he = findHashEntry(om,val -> u.cppPtr);

    /*
     * If the bucket is in use then we appear to have several objects at the
     * same address.
     */
    if (he -> first != NULL)
    {
        /*
         * This can happen for three reasons.  A variable of one class can be
         * declared at the start of another class.  Therefore there are two
         * objects, of different classes, with the same address.  The second
         * reason is that the old C/C++ object has been deleted by C/C++ but we
         * didn't get to find out for some reason, and a new C/C++ instance has
         * been created at the same address.  The third reason is if we are in
         * the process of deleting a Python object but the C++ object gets
         * wrapped again because the C++ dtor called a method that has been
         * re-implemented in Python.  The absence of the SIP_SHARE_MAP flag
         * tells us that a new C++ instance has just been created and so we
         * know the second reason is the correct one so we mark the old
         * pointers as invalid and reuse the entry.  Otherwise we just add this
         * one to the existing list of objects at this address.
         */
        if (!(val -> flags & SIP_SHARE_MAP))
        {
            sipWrapper *w = he->first;

            he->first = NULL;

            while (w != NULL)
            {
                sipWrapper *next = w->next;

                /* We are removing it from the map here. */
                sipSetNotInMap(w);
                sip_api_common_dtor(w);

                w = next;
            }
        }

        val -> next = he -> first;
        he -> first = val;

        return;
    }

    /* See if the bucket was unused or stale. */
    if (he -> key == NULL)
    {
        he -> key = val -> u.cppPtr;
        om -> unused--;
    }
    else
        om -> stale--;

    /* Add the rest of the new value. */
    he -> first = val;
    val -> next = NULL;

    reorganiseMap(om);
}


/*
 * Reorganise a map if it is running short of space.
 */
static void reorganiseMap(sipObjectMap *om)
{
    unsigned long old_size, i;
    sipHashEntry *ohe, *old_tab;

    /* Don't bother if it still has more than 12% available. */
    if (om -> unused > om -> size >> 3)
        return;

    /*
     * If reorganising (ie. making the stale buckets unused) using the same
     * sized table would make 25% available then do that.  Otherwise use a
     * bigger table (if possible).
     */
    if (om -> unused + om -> stale < om -> size >> 2 && hash_primes[om -> primeIdx + 1] != 0)
        om -> primeIdx++;

    old_size = om -> size;
    old_tab = om -> hash_array;

    om -> unused = om -> size = hash_primes[om -> primeIdx];
    om -> stale = 0;
    om -> hash_array = newHashTable(om -> size);

    /* Transfer the entries from the old table to the new one. */
    ohe = old_tab;

    for (i = 0; i < old_size; ++i)
    {
        if (ohe -> key != NULL && ohe -> first != NULL)
        {
            *findHashEntry(om,ohe -> key) = *ohe;
            om -> unused--;
        }

        ++ohe;
    }

    sip_api_free(old_tab);
}


/*
 * Remove a C/C++ object from the table.  Return 0 if it was removed
 * successfully.
 */
int sipOMRemoveObject(sipObjectMap *om,sipWrapper *val)
{
    sipHashEntry *he = findHashEntry(om,val -> u.cppPtr);
    sipWrapper **wp;

    for (wp = &he -> first; *wp != NULL; wp = &(*wp) -> next)
        if (*wp == val)
        {
            *wp = val -> next;

            /*
             * If the bucket is now empty then count it as stale.
             * Note that we do not NULL the key and count it as
             * unused because that might throw out the search for
             * another entry that wanted to go here, found it
             * already occupied, and was put somewhere else.  In
             * other words, searches must be repeatable until we
             * reorganise the table.
             */
            if (he -> first == NULL)
                om -> stale++;

            return 0;
        }

    return -1;
}
