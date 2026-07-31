/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The sip module implementation.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <assert.h>

#include "sip.h"

#include "sip_array.h"
#include "sip_callable.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_method_descriptor.h"
#include "sip_module_wrapper.h"
#include "sip_object_map.h"
#include "sip_sip_module.h"
#include "sip_sip_module_methods.h"
#include "sip_variable_descriptor.h"
#include "sip_voidptr.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper.h"
#include "sip_wrapper_type.h"


#if _SIP_MODULE_SHARED

/* Forward declarations specific to a standalone sip module. */
static const sipABISpec *bootstrap(int abi_major);
static int module_clear(PyObject *module);
static int module_exec(PyObject *module);
static void module_free(void *module_ptr);
static int module_traverse(PyObject *module, visitproc visit, void *arg);


/* The standalone sip module definition. */
PyABIInfo_VAR(abi_info);

static PySlot module_slots[] = {
    PySlot_STATIC_DATA(Py_mod_name, _SIP_MODULE_FQ_NAME),
    PySlot_STATIC_DATA(Py_mod_abi, &abi_info),
    PySlot_STATIC_DATA(Py_mod_doc, PyDoc_STR("Bindings related utilities")),
    PySlot_FUNC(Py_mod_exec, module_exec),
    PySlot_DATA(Py_mod_gil, Py_MOD_GIL_NOT_USED),
    PySlot_STATIC_DATA(Py_mod_methods, sipSipModuleMethods),
    PySlot_DATA(Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED),
    PySlot_FUNC(Py_mod_state_clear, module_clear),
    PySlot_FUNC(Py_mod_state_free, module_free),
    PySlot_SIZE(Py_mod_state_size, sizeof (sipSipModuleState)),
    PySlot_FUNC(Py_mod_state_traverse, module_traverse),
    PySlot_END
};


/*
 * The standalone sip module initialisation function.
 */
PyMODEXPORT_FUNC _SIP_MODULE_ENTRY(void)
{
    return module_slots;
}


/*
 * This should only be needed until
 * https://github.com/pypa/distutils/issues/387 is resolved.
 */
#ifdef _WIN32
PyMODINIT_FUNC _SIP_LEGACY_MODULE_ENTRY(void)
{
    PyErr_SetString(PyExc_NotImplementedError, "legacy init not supported");
    return NULL;
}
#endif


/*
 * Implement the exec phase of the module initialisation.
 */
static int module_exec(PyObject *smod)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);

    /* Initialise the module. */
    if (sip_sip_module_init(sms, smod) < 0)
        return -1;

    /* Publish the first stage bootstrap function. */
    PyObject *api_obj = PyCapsule_New((void *)bootstrap, "_C_BOOTSTRAP", NULL);

    int rc = PyModule_AddObjectRef(smod, "_C_BOOTSTRAP", api_obj);
    Py_XDECREF(api_obj);

    return rc;
}


/*
 * Implement the standalone module clear slot.
 */
static int module_clear(PyObject *smod)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);

    return sip_sip_module_clear(sms);
}


/*
 * Implement the standalone module free slot.
 */
static void module_free(void *smod_ptr)
{
    sipSipModuleState *sms = sip_get_sip_module_state((PyObject *)smod_ptr);

    sip_sip_module_free(sms);
}


/*
 * Implement the standalone module traverse slot.
 */
static int module_traverse(PyObject *smod, visitproc visit, void *arg)
{
    sipSipModuleState *sms = (sipSipModuleState *)PyModule_GetState_DuringGC(
            smod);

    /* It's not clear what value should be returned here. */
    if (sms == NULL)
        return 0;

    return sip_sip_module_traverse(sms, visit, arg);
}


/*
 * The first stage bootstrap function.
 */
static const sipABISpec *bootstrap(int abi_major)
{
    if (abi_major != 14)
    {
        PyErr_Format(PyExc_RuntimeError,
                "the sip module implements ABI v14 but the requesting module requires ABI v%d",
                abi_major);
        return NULL;
    }

    return &sip_abi;
}
#endif


/*
 * Implement the module initialisation support.
 */
int sip_sip_module_init(sipSipModuleState *sms, PyObject *smod)
{
    sms->module_list = NULL;
#if defined(Py_GIL_DISABLED)
    sms->mutex = (PyMutex){0};
#endif

    /* Add the constants. */
    long abi_version = (SIP_ABI_MAJOR_VERSION << 16) +
            (SIP_ABI_MINOR_VERSION << 8) +
            SIP_MODULE_PATCH_VERSION;

    if (PyModule_AddIntConstant(smod, "SIP_ABI_VERSION", abi_version) < 0 ||
        PyModule_AddIntMacro(smod, SIP_VERSION) < 0 ||
        PyModule_AddStringMacro(smod, SIP_VERSION_STR) < 0)
        return -1;

    /* Initialise the types. */
    if (sip_wrapper_type_init(smod, sms) < 0 ||
        sip_simple_wrapper_init(smod, sms) < 0 ||
        sip_wrapper_init(smod, sms) < 0 ||
        sip_module_wrapper_init(smod, sms) < 0 ||
        sip_method_descr_init(smod, sms) < 0 ||
        sip_callable_init(smod, sms) < 0 ||
        sip_variable_descr_init(smod, sms) < 0 ||
        sip_enum_init(smod, sms) < 0 ||
        sip_void_ptr_init(smod, sms) < 0 ||
        sip_array_init(smod, sms) < 0)
        return -1;

    /* This will always be needed. */
    sms->empty_tuple = Py_GetConstant(Py_CONSTANT_EMPTY_TUPLE);

    /* Initialise the object map. */
    if (sip_om_init(&sms->object_map) < 0)
        return -1;

    /* Get the current interpreter view. */
    if ((sms->interpreter_view = PyInterpreterView_FromCurrent()) == NULL)
        return -1;

    return 0;
}


/*
 * Implement the module clear support.
 */
int sip_sip_module_clear(sipSipModuleState *sms)
{
    Py_CLEAR(sms->array_type);
    Py_CLEAR(sms->callable_type);
#if defined(SIP_CONFIGURATION_PyEnums)
    Py_CLEAR(sms->builtin_int_type);
    Py_CLEAR(sms->builtin_object_type);
#endif
#if defined(SIP_CONFIGURATION_CustomEnums)
    Py_CLEAR(sms->custom_enum_type);
#endif
    Py_CLEAR(sms->empty_tuple);
#if defined(SIP_CONFIGURATION_CustomEnums)
    Py_CLEAR(sms->enum_enum_type);
    Py_CLEAR(sms->enum_int_enum_type);
#endif
#if defined(SIP_CONFIGURATION_PyEnums)
    Py_CLEAR(sms->enum_enum_type);
    Py_CLEAR(sms->enum_int_enum_type);
    Py_CLEAR(sms->enum_flag_type);
    Py_CLEAR(sms->enum_int_flag_type);
#endif
    Py_CLEAR(sms->method_descr_type);
    Py_CLEAR(sms->module_wrapper_type);
    Py_CLEAR(sms->simple_wrapper_type);
    Py_CLEAR(sms->variable_descr_type);
    Py_CLEAR(sms->void_ptr_type);
    Py_CLEAR(sms->wrapper_type);
    Py_CLEAR(sms->wrapper_type_type);

    Py_CLEAR(sms->module_list);

    return 0;
}


/*
 * Implement the module free support.
 */
void sip_sip_module_free(sipSipModuleState *sms)
{
    sip_sip_module_clear(sms);
    sip_om_finalise(&sms->object_map);
    PyInterpreterView_Close(sms->interpreter_view);
}


/*
 * Implement the module traverse support.
 */
int sip_sip_module_traverse(sipSipModuleState *sms, visitproc visit, void *arg)
{
    Py_VISIT(sms->array_type);
    Py_VISIT(sms->callable_type);
#if defined(SIP_CONFIGURATION_PyEnums)
    Py_VISIT(sms->builtin_int_type);
    Py_VISIT(sms->builtin_object_type);
#endif
#if defined(SIP_CONFIGURATION_CustomEnums)
    Py_VISIT(sms->custom_enum_type);
#endif
    Py_VISIT(sms->empty_tuple);
    Py_VISIT(sms->enum_enum_type);
    Py_VISIT(sms->enum_int_enum_type);
#if defined(SIP_CONFIGURATION_PyEnums)
    Py_VISIT(sms->enum_flag_type);
    Py_VISIT(sms->enum_int_flag_type);
#endif
    Py_VISIT(sms->method_descr_type);
    Py_VISIT(sms->module_wrapper_type);
    Py_VISIT(sms->simple_wrapper_type);
    Py_VISIT(sms->variable_descr_type);
    Py_VISIT(sms->void_ptr_type);
    Py_VISIT(sms->wrapper_type);
    Py_VISIT(sms->wrapper_type_type);

    Py_VISIT(sms->module_list);

    return 0;
}


/*
 * Return the state for the sip module from a type that was created by the sip
 * module or NULL (and an exception raised) if the type isn't known to sip.
 * This should never be the case for generated code but might happen if the
 * user passes in an incorrect value via the API.
 */
sipSipModuleState *sip_get_sip_module_state_from_type(PyTypeObject *type)
{
#if _SIP_MODULE_SHARED
    PyObject *smod = PyType_GetModuleByToken(type, module_slots);
    if (smod == NULL)
        return NULL;

    return sip_get_sip_module_state(smod);
#else
    extern PySlot _SIP_MODULE_SLOTS[];

    PyObject *mod = PyType_GetModuleByToken(type, _SIP_MODULE_SLOTS);
    if (mod == NULL)
        return NULL;

    return sip_get_module_state(mod)->sip_module_state;
#endif
}
