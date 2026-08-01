/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the wrapped module support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_wrapped_module.h"

#include "sip_attribute.h"
#include "sip_callable.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_sip_module.h"
#include "sip_sip_module_methods.h"


/* Forward references. */
static int add_license(PyObject *mod, const sipLicenseSpec *lic_spec);
static int import_module(const sipImportedModuleSpec *ims,
        sipImportedModule *im);
static void module_clear(sipModuleState *ms);


/* The wrapped module's clear slot. */
int sip_api_module_clear(PyObject *mod)
{
    module_clear(sip_get_module_state(mod));

    return 0;
}


/* The wrapped module's free slot. */
void sip_api_module_free(void *mod_ptr)
{
    sipModuleState *ms = sip_get_module_state((PyObject *)mod_ptr);

    /* Invoke any module collection handlers. */
    if (ms->event_handlers != NULL)
    {
        const sipEventHandlerSpec *ehs = ms->event_handlers;

        while (ehs->event_type != sipEventNone)
        {
            if (ehs->event_type == sipEventCollectingModule)
            {
                sipCollectingModuleEventHandler handler = (sipCollectingModuleEventHandler)ehs->handler;

                handler();
            }

            ehs++;
        }
    }

    /* Clear all the Python references. */
    module_clear(ms);

    /* Free any user state. */
    if (ms->user_state_free != NULL)
        ms->user_state_free(ms, ms->user_state);

    /* Free the additional memory related to type implementations. */
    if (ms->type_impls != NULL)
        PyMem_Free(ms->type_impls);

    /* Free the additional memory related to imported modules. */
    if (ms->imported_modules != NULL)
    {
        sipModuleNr mi;

        for (mi = 0; mi < ms->module_spec->nr_import_specs; mi++)
            if (ms->imported_modules[mi].type_nr_map != NULL)
                PyMem_Free(ms->imported_modules[mi].type_nr_map);

        PyMem_Free(ms->imported_modules);
    }

#if !_SIP_MODULE_SHARED
    sip_sip_module_free(ms->sip_module_state);
    sip_api_free(ms->sip_module_state);
#endif
}


/*
 * The execute phase of a wrapped module initialisation.  Note that we don't
 * tidy up on error because the free slot will be called anyway and so long as
 * it doesn't make any assumptions about the state of things then everything
 * will be correct.
 */
int sip_api_module_exec(PyObject *mod, const sipModuleSpec *m_spec)
{
    /* Check that we can support it. */
    if (m_spec->abi_major != SIP_ABI_MAJOR_VERSION || m_spec->abi_minor > SIP_ABI_MINOR_VERSION)
    {
#if SIP_ABI_MINOR_VERSION > 0
        PyErr_Format(PyExc_RuntimeError,
                "the sip module implements ABI v%d.0 to v%d.%d but the %s module requires ABI v%d.%d",
                SIP_ABI_MAJOR_VERSION, SIP_ABI_MAJOR_VERSION,
                SIP_ABI_MINOR_VERSION, PyModule_GetName(mod),
                m_spec->abi_major, m_spec->abi_minor);
#else
        PyErr_Format(PyExc_RuntimeError,
                "the sip module implements ABI v%d.0 but the %s module requires ABI v%d.%d",
                SIP_ABI_MAJOR_VERSION, PyModule_GetName(mod),
                m_spec->abi_major, m_spec->abi_minor);
#endif

        return -1;
    }

    if (m_spec->sip_configuration != SIP_CONFIGURATION)
    {
        PyErr_Format(PyExc_RuntimeError,
                "the sip module has a configuration of 0x%04x but the %s module requires 0x%04x",
                SIP_CONFIGURATION, PyModule_GetName(mod),
                m_spec->sip_configuration);

        return -1;
    }

    sipModuleState *ms = sip_get_module_state(mod);

#if _SIP_MODULE_SHARED
    ms->sip_module = PyImport_ImportModule(_SIP_MODULE_FQ_NAME);
    if (ms->sip_module == NULL)
        return -1;

    ms->sip_module_state = sip_get_sip_module_state(ms->sip_module);
#else
    ms->sip_module_state = sip_api_malloc(sizeof (sipSipModuleState));

    if (sip_sip_module_init(ms->sip_module_state, mod) < 0)
        return -1;
#endif
    ms->wrapped_module = mod;
    ms->module_spec = m_spec;

    sipSipModuleState *sms = ms->sip_module_state;

    /* Update the new module's super-type. */
    PyObject *class_s = PyUnicode_InternFromString("__class__");
    if (class_s == NULL)
        return -1;

    if (PyObject_SetAttr(mod, class_s, (PyObject *)sms->module_wrapper_type) < 0)
    {
        Py_DECREF(class_s);
        return -1;
    }

    Py_DECREF(class_s);

    /* Add the SIP version number. */
    if (PyModule_AddIntMacro(mod, SIP_VERSION) < 0)
        return -1;

    if (PyModule_AddStringMacro(mod, SIP_VERSION_STR) < 0)
        return -1;

    /* Add the SIP ABI version number. */
    const long abi_version = (SIP_ABI_MAJOR_VERSION << 16) +
            (SIP_ABI_MINOR_VERSION << 8) +
            SIP_MODULE_PATCH_VERSION;

    if (PyModule_AddIntConstant(mod, "SIP_ABI_VERSION", abi_version) < 0)
        return -1;

#if !_SIP_MODULE_SHARED
    /* Add the sip module's methods. */
    if (PyModule_AddFunctions(mod, sipSipModuleMethods) < 0)
        return -1;
#endif

    /* Add the non-type attributes. */
    if (m_spec->attrs != NULL)
    {
        Py_ssize_t attr_i = 0;

        while (attr_i < m_spec->attrs->nr_attrs)
        {
            const sipAttrSpec *attr_spec = &m_spec->attrs->attrs[attr_i++];

            if (PyModule_Add(mod, sipAttrGetName(attr_spec), sip_get_attribute_for_spec(ms, mod, attr_spec, NULL)) < 0)
                return -1;
        }
    }

    /* Allocate the space for any type implementations. */
    if (m_spec->nr_type_specs > 0 && (ms->type_impls = PyMem_Calloc(m_spec->nr_type_specs, sizeof (sipTypeImpl))) == NULL)
        return -1;

    /* Import any required wrapped modules. */
    if (m_spec->nr_import_specs > 0)
    {
        if ((ms->imported_modules = PyMem_Calloc(m_spec->nr_import_specs, sizeof (sipImportedModule))) == NULL)
            return -1;

        sipModuleNr mi;

        for (mi = 0; mi < m_spec->nr_import_specs; mi++)
            if (import_module(&m_spec->import_specs[mi], &ms->imported_modules[mi]) < 0)
                return -1;
    }

    /* Add any license. */
    if (m_spec->license != NULL && add_license(mod, m_spec->license) < 0)
        return -1;

    /* Now the module is complete, add it to the list of wrapped modules. */
    PyObject *weak_mod = PyWeakref_NewRef(mod, NULL);
    if (weak_mod == NULL)
        return -1;

    int rc = sip_append_py_object_to_list(sms, &sms->module_list, weak_mod);
    Py_DECREF(weak_mod);

    return rc;
}


/* The wrapped module's traverse slot. */
int sip_api_module_traverse(PyObject *mod, visitproc visit, void *arg)
{
    sipModuleState *ms = (sipModuleState *)PyModule_GetState_DuringGC(mod);
    if (ms == NULL)
        return 0;

    /* The module initialisation may not have completed successfully. */
    if (ms->module_spec != NULL)
    {
        const sipModuleSpec *m_spec = ms->module_spec;

        /* Visit the types. */
        sipTypeNr ti;

        for (ti = 0; ti < m_spec->nr_type_specs; ti++)
            if (!sipTypeSpecIsExternal(m_spec->type_specs[ti]))
                Py_VISIT(ms->type_impls[ti].py_type);

        /* Visit the imported modules. */
        sipModuleNr mi;

        for (mi = 0; mi < m_spec->nr_import_specs; mi++)
            Py_VISIT(ms->imported_modules[mi].module);
    }

    Py_VISIT(ms->extra_refs);
    Py_VISIT(ms->registered_py_types);
#if _SIP_MODULE_SHARED
    Py_VISIT(ms->sip_module);
#else
    sip_sip_module_traverse(ms->sip_module_state, visit, arg);
#endif

    /* Visit any user state. */
    if (ms->user_state_traverse != NULL)
        ms->user_state_traverse(ms, ms->user_state, visit, arg);

    return 0;
}


/*
 * Return a wrapped module's state.
 */
sipModuleState *sip_get_module_state(PyObject *mod)
{
    sipModuleState *ms = (sipModuleState *)PyModule_GetState(mod);

    /* Something has gone seriously wrong if there is no state. */
    if (ms == NULL)
        Py_FatalError("wrapped module has no state");

    return ms;
}


/*
 * Return the additional module state supplied by the user.
 */
void *sip_api_get_module_user_state(sipModuleState *ms)
{
    return ms->user_state;
}


/*
 * Set the additional module state from the user.
 */
void sip_api_set_module_user_state(sipModuleState *ms, void *user_state,
        sipModuleUserStateClearFunc user_state_clear,
        sipModuleUserStateFreeFunc user_state_free,
        sipModuleUserStateTraverseFunc user_state_traverse)
{
    ms->user_state = user_state;
    ms->user_state_clear = user_state_clear;
    ms->user_state_free = user_state_free;
    ms->user_state_traverse = user_state_traverse;
}


/*
 * Add a license to a module.
 */
static int add_license(PyObject *mod, const sipLicenseSpec *lic_spec)
{
    /* The license type is compulsory, the rest are optional. */
    if (lic_spec->type == NULL)
        return -1;

    /* We use a dictionary to hold the license information. */
    PyObject *lic_dict = PyDict_New();
    if (lic_dict == NULL)
        return -1;

    PyObject *o;

    if ((o = PyUnicode_FromString(lic_spec->type)) == NULL)
        goto deldict;

    int rc = PyDict_SetItemString(lic_dict, "Type", o);
    Py_DECREF(o);

    if (rc < 0)
        goto deldict;

    if (lic_spec->licensee != NULL)
    {
        if ((o = PyUnicode_FromString(lic_spec->licensee)) == NULL)
            goto deldict;

        rc = PyDict_SetItemString(lic_dict, "Licensee", o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lic_spec->timestamp != NULL)
    {
        if ((o = PyUnicode_FromString(lic_spec->timestamp)) == NULL)
            goto deldict;

        rc = PyDict_SetItemString(lic_dict, "Timestamp", o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lic_spec->signature != NULL)
    {
        if ((o = PyUnicode_FromString(lic_spec->signature)) == NULL)
            goto deldict;

        rc = PyDict_SetItemString(lic_dict, "Signature", o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    /* Create and save a read-only proxy. */
    rc = PyModule_Add(mod, "__license__", PyDictProxy_New(lic_dict));
    Py_DECREF(lic_dict);

    return rc;

deldict:
    Py_DECREF(lic_dict);
    return -1;
}


/*
 * Import a module that the module being created requires.
 */
static int import_module(const sipImportedModuleSpec *ims,
        sipImportedModule *im)
{
    /* Import the module. */
    if ((im->module = PyImport_ImportModule(ims->name)) == NULL)
        return -1;

    /* Populate the type number map. */
    if (ims->nr_types > 0)
    {
        if ((im->type_nr_map = PyMem_Calloc(ims->nr_types, sizeof (sipTypeNr))) == NULL)
            return -1;

        sipModuleState *ms = sip_get_module_state(im->module);
        sipTypeNr ti, iti;

        /* We rely on both tables being in the same order. */
        for (iti = ti = 0; ti < ims->nr_types; ti++)
        {
            const char *type_name = ims->type_names[ti];

            while (strcmp(type_name, ms->module_spec->type_specs[iti]->cpp_name) != 0)
                if (++iti >= ms->module_spec->nr_type_specs)
                {
                    PyErr_Format(PyExc_RuntimeError,
                            "unable to find type '%s' in module %s", type_name,
                            ims->name);
                    return -1;
                }

            im->type_nr_map[ti] = iti;
        }
    }

    return 0;
}


/* Clear a wrapped module's Python references. */
static void module_clear(sipModuleState *ms)
{
    /* Clear any user state. */
    if (ms->user_state_clear != NULL)
        ms->user_state_clear(ms, ms->user_state);

    const sipModuleSpec *m_spec = ms->module_spec;

    /* Clear the wrapped types. */
    sipTypeNr ti;

    for (ti = 0; ti < m_spec->nr_type_specs; ti++)
        if (!sipTypeSpecIsExternal(m_spec->type_specs[ti]))
            Py_CLEAR(ms->type_impls[ti].py_type);

    /* Clear the imported modules. */
    sipModuleNr mi;

    for (mi = 0; mi < m_spec->nr_import_specs; mi++)
        Py_CLEAR(ms->imported_modules[mi].module);

    Py_CLEAR(ms->extra_refs);
    Py_CLEAR(ms->registered_py_types);
#if _SIP_MODULE_SHARED
    Py_CLEAR(ms->sip_module);
#else
    sip_sip_module_clear(ms->sip_module_state);
#endif
}
