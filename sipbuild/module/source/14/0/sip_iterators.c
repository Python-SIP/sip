/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The support for iterators.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <string.h>

#include "sip_iterators.h"

#include "sip_attribute.h"
#include "sip_parsers.h"
#include "sip_sip_module.h"


/*
 * Iterate over the (virtual) list of attributes that extend a type and invoke
 * a handler to perform some action.  The value returned by the handler
 * determines if the iteration continues.
 */
int sip_iterate_over_attributes(sipModuleState *ms,
        const sipTypeSpec *extending_ts, const char *name,
        sipIterAttrFunc handler, void *closure)
{
    sipSipModuleState *sms = ms->sip_module_state;
    PyObject *mods = sms->module_list;

    /* Shortcut the trivial case where there is only one module. */
    if (PyList_GET_SIZE(mods) == 1)
        return 0;

    /*
     * Iterate of the list of modules allowing for the fact that a module may
     * be removed at any time.
     */
    Py_ssize_t x_i;

    for (x_i = 0; x_i < PyList_GET_SIZE(mods); x_i++)
    {
        PyObject *x_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, x_i), &x_mod) < 0)
            return -1;

        if (x_mod == NULL)
            continue;

        /* Don't search the originating module. */
        if (x_mod == ms->wrapped_module)
            goto gc_mod;

        sipModuleState *x_ms = sip_get_module_state(x_mod);

        /* Skip if the module doesn't have any extenders. */
        const sipExtenderSpec *extenders = x_ms->module_spec->extenders;
        if (extenders == NULL)
            goto gc_mod;

        while (extenders->extending_type_id != sipType_Invalid)
        {
            /* See if this extender extends the type. */
            PyObject *def_mod;
            const sipTypeSpec *ts = sip_get_type_spec(x_ms,
                    extenders->extending_type_id, &def_mod);
            if (def_mod == NULL)
                goto gc_mod;

            if (ts == extending_ts)
            {
                /*
                 * Potentially loop through all attributes, but if we have a
                 * name we will look for it and stop if it was found.
                 */
                Py_ssize_t i = 0;

                while (i < extenders->attrs->nr_attrs)
                {
                    const sipAttrSpec *x_attr_spec;

                    if (name != NULL)
                    {
                        x_attr_spec = sip_get_attribute_spec(name,
                                extenders->attrs);

                        if (x_attr_spec == NULL)
                            break;
                    }
                    else
                    {
                        x_attr_spec = &extenders->attrs->attrs[i++];
                    }

                    /*
                     * Invoke the handler.  A non-zero result means we stop
                     * iterating.
                     */
                    int state = handler(x_ms, x_attr_spec, closure);

                    if (state != 0)
                    {
                        Py_DECREF(def_mod);
                        Py_DECREF(x_mod);
                        return state;
                    }

                    if (name != NULL)
                        break;
                }
            }

            Py_DECREF(def_mod);
            extenders++;
        }

gc_mod:
        Py_DECREF(x_mod);
    }

    return 0;
}


/*
 * Iterate over the (virtual) list of init extenders that extend a type and
 * invoke a handler to perform some action.  The value returned by the handler
 * determines if the iteration continues.
 */
int sip_iterate_over_init_extenders(sipModuleState *ms,
        const sipTypeSpec *extending_ts, sipIterInitFunc handler,
        void *closure)
{
    sipSipModuleState *sms = ms->sip_module_state;
    PyObject *mods = sms->module_list;

    /* Shortcut the trivial case where there is only one module. */
    if (PyList_GET_SIZE(mods) == 1)
        return 0;

    /*
     * Iterate of the list of modules allowing for the fact that a module may
     * be removed at any time.
     */
    Py_ssize_t x_i;

    for (x_i = 0; x_i < PyList_GET_SIZE(mods); x_i++)
    {
        PyObject *x_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, x_i), &x_mod) < 0)
            return -1;

        if (x_mod == NULL)
            continue;

        /* Don't search the originating module. */
        if (x_mod == ms->wrapped_module)
            goto gc_mod;

        sipModuleState *x_ms = sip_get_module_state(x_mod);

        /* Skip if the module doesn't have any init extenders. */
        const sipInitExtenderSpec *extenders = x_ms->module_spec->init_extenders;
        if (extenders == NULL)
            goto gc_mod;

        while (extenders->extending_type_id != sipType_Invalid)
        {
            /* See if this extender extends the type. */
            PyObject *def_mod;
            const sipTypeSpec *ts = sip_get_type_spec(x_ms,
                    extenders->extending_type_id, &def_mod);
            if (def_mod == NULL)
                goto gc_mod;

            if (ts == extending_ts)
            {
                /*
                 * Invoke the handler.  A non-zero result means we stop
                 * iterating.
                 */
                int state = handler(x_ms, extenders, closure);

                if (state != 0)
                {
                    Py_DECREF(def_mod);
                    Py_DECREF(x_mod);
                    return state;
                }
            }

            Py_DECREF(def_mod);
            extenders++;
        }

gc_mod:
        Py_DECREF(x_mod);
    }

    return 0;
}
