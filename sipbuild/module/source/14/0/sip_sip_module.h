/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The sip module interface.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_SIP_MODULE_H
#define _SIP_SIP_MODULE_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"
#include "sip_object_map.h"
#include "sip_wrapped_module.h"


#ifdef __cplusplus
extern "C" {
#endif

/* The sip module's state. */
struct sipSipModuleStateImpl {
    /* The sip.array type object. */
    PyTypeObject *array_type;

    /* The sip.callable type object. */
    PyTypeObject *callable_type;

#if defined(SIP_CONFIGURATION_CustomEnums)
    /* The custom enum type object. */
    PyTypeObject *custom_enum_type;
#endif

#if defined(SIP_CONFIGURATION_PyEnums)
    /* The builtin int type object. */
    PyObject *builtin_int_type;

    /* The builtin object type object. */
    PyObject *builtin_object_type;
#endif

    /* The empty tuple. */
    PyObject *empty_tuple;

    /* The enum.Enum type object. */
    PyObject *enum_enum_type;

    /* The enum.IntEnum type object. */
    PyObject *enum_int_enum_type;

#if defined(SIP_CONFIGURATION_PyEnums)
    /* The enum.Flag type object. */
    PyObject *enum_flag_type;

    /* The enum.IntFlag type object. */
    PyObject *enum_int_flag_type;
#endif

    /* The interpreter view. */
    PyInterpreterView *interpreter_view;

    /* The method descriptor type object. */
    PyTypeObject *method_descr_type;

    /*
     * The list of weak references to registered modules.  The list will only
     * ever be appended to and existing elements will never be modified.  This
     * means that indexes into this list (eg. in an absolute type ID) will
     * always be able to be safely checked to see if they are still valid.  In
     * addition accessing an element will always produce a strong reference to
     * a module object.
     */
    PyObject *module_list;

    /* The sip.modulewrapper type object. */
    PyTypeObject *module_wrapper_type;

#if defined(Py_GIL_DISABLED)
    /*
     * This is the main mutex mainly used to protect the object map and when
     * modifying a wrapped object's position in its hierachy.
     */
    PyMutex mutex;
#endif

    /* The object map. */
    sipObjectMap object_map;

    /* The sip.simplewrapper type object. */
    PyTypeObject *simple_wrapper_type;

    /* The trace mask. */
    unsigned trace_mask;

    /* The variable descriptor type object. */
    PyTypeObject *variable_descr_type;

    /* The sip.voidptr type object. */
    PyTypeObject *void_ptr_type;

    /* The sip.wrapper type object. */
    PyTypeObject *wrapper_type;

    /* The sip.wrappertype type object. */
    PyTypeObject *wrapper_type_type;
};


sipSipModuleState *sip_get_sip_module_state_from_type(PyTypeObject *type);
int sip_sip_module_clear(sipSipModuleState *sms);
void sip_sip_module_free(sipSipModuleState *sms);
int sip_sip_module_init(sipSipModuleState *sms, PyObject *smod);
int sip_sip_module_traverse(sipSipModuleState *sms, visitproc visit,
        void *arg);


/*
 * Return the sip module's state.
 */
static inline sipSipModuleState *sip_get_sip_module_state(PyObject *smod)
{
#if _SIP_MODULE_SHARED
    sipSipModuleState *sms = (sipSipModuleState *)PyModule_GetState(smod);

    assert(sms != NULL);

    return sms;
#else
    /* The module is actually the wrapped module. */
    sipModuleState *ms = sip_get_module_state(smod);

    return ms->sip_module_state;
#endif
}

#ifdef __cplusplus
}
#endif

#endif
