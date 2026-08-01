/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the wrapped module support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_WRAPPED_MODULE_H
#define _SIP_WRAPPED_MODULE_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * An imported wrapped module.  Note that we don't refer to it as a state to
 * avoid confusion with the Python module state.
 */
typedef struct _sipImportedModule {
    /* A strong reference to the module. */
    PyObject *module;

    /*
     * An array on the heap mapping contextual type numbers to defining type
     * numbers.
     */
    sipTypeNr *type_nr_map;
} sipImportedModule;


/*
 * The implementation of a type.
 */
typedef union {
    /*
     * If the type is external then this is the cached absolute type ID of the
     * actual type.  This is set only when needed and will be sipType_Invalid
     * until then.
     */
    sipTypeID external_type_id;

    /*
     * The Python type object that implements the type.  It may be NULL if the
     * generated type doesn't have a Python implementation (ie. some mapped
     * types).
     */
    PyTypeObject *py_type;
} sipTypeImpl;


/*
 * The implementation of a wrapped module's state.
 */
struct sipModuleStateImpl {
    /* The array of event handlers. */
    const sipEventHandlerSpec *event_handlers;

    /* The optional dictionary of extra references using an int key. */
    PyObject *extra_refs;

    /* The array of imported modules. */
    sipImportedModule *imported_modules;

    /* The specification of the wrapped module. */
    const sipModuleSpec *module_spec;

    /* The list of registered Python type objects. */
    PyObject *registered_py_types;

#if _SIP_MODULE_SHARED
    /*
     * A strong reference to the sip module.  The value isn't used but we keep
     * the reference to ensure it isn't garbage collected.
     */
    PyObject *sip_module;
#endif

    /* The module state of the sip module. */
    sipSipModuleState *sip_module_state;

    /* The exported symbols. */
    const sipSymbolSpec *symbols;

    /* The array of type implementations accessed using the type ID. */
    sipTypeImpl *type_impls;

    /* The user-supplied additional state. */
    void *user_state;

    /* The user-supplied state clear function. */
    sipModuleUserStateClearFunc user_state_clear;

    /* The user-supplied state free function. */
    sipModuleUserStateFreeFunc user_state_free;

    /* The user-supplied state traverse function. */
    sipModuleUserStateTraverseFunc user_state_traverse;

    /* A borrowed reference to the wrapped module. */
    PyObject *wrapped_module;
};


void *sip_api_get_module_user_state(sipModuleState *ms);
int sip_api_module_clear(PyObject *mod);
void sip_api_module_free(void *mod_ptr);
int sip_api_module_traverse(PyObject *mod, visitproc visit, void *arg);
void sip_api_set_module_user_state(sipModuleState *ms, void *user_state,
        sipModuleUserStateClearFunc user_state_clear,
        sipModuleUserStateFreeFunc user_state_free,
        sipModuleUserStateTraverseFunc user_state_traverse);

sipModuleState *sip_get_module_state(PyObject *mod);

#ifdef __cplusplus
}
#endif

#endif
