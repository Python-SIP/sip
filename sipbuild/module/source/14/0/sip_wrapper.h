/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the sip wrapper type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_WRAPPER_H
#define _SIP_WRAPPER_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"
#include "sip_simple_wrapper.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The type of a C/C++ wrapper object that supports parent/child relationships.
 * A parent holds a strong reference to each of its children.
 */
struct sipWrapperImpl {
    /* The super-type. */
    sipSimpleWrapper super;

    /* First child object. */
    sipWrapper *first_child;

    /* Next sibling. */
    sipWrapper *sibling_next;

    /* Previous sibling. */
    sipWrapper *sibling_prev;

    /* Owning object. */
    sipWrapper *parent;
};


int sip_wrapper_init(PyObject *module, sipSipModuleState *sms);

#ifdef __cplusplus
}
#endif

#endif
