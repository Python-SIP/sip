/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the sip wrapper type type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_WRAPPER_TYPE_H
#define _SIP_WRAPPER_TYPE_H

#include <Python.h>

#include "sip.h"
#include "sip_core.h"
#include "sip_decls.h"
#include "sip_wrapped_module.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The meta-type of a wrapper type.
 */
struct sipWrapperTypeImpl {
    /*
     * The super-metatype.  This must be first in the structure so that it can
     * be cast to a PyTypeObject *.
     */
    PyHeapTypeObject super;

    /* A strong reference to the defining module. */
    PyObject *defining_module;

    /*
     * For the user to use.  This is a strong reference.
     */
    PyObject *user_data;

    /* The type ID in the context of the defining module. */
    sipTypeID type_id;

    /* Set if autoconversion of the type is disabled. */
    bool autoconversion_disabled;

    /*
     * Set if the type is a sub-type of wrapper rather than simple wrapper.
     * This can only be used if we know we have a SIP generated type.
     */
    bool is_wrapper;

    /* Set if the type is a user implemented Python sub-class. */
    bool user_type;
};


int sip_wrapper_type_init(PyObject *module, sipSipModuleState *sms);

#ifdef __cplusplus
}
#endif

#endif
