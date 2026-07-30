/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the voidptr type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_VOIDPTR_H
#define _SIP_VOIDPTR_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

PyObject *sip_api_convert_from_const_void_ptr(sipModuleState *ms,
        const void *val);
PyObject *sip_api_convert_from_const_void_ptr_and_size(sipModuleState *ms,
        const void *val, Py_ssize_t size);
PyObject *sip_api_convert_from_void_ptr(sipModuleState *ms, void *val);
PyObject *sip_api_convert_from_void_ptr_and_size(sipModuleState *ms, void *val,
        Py_ssize_t size);
void *sip_api_convert_to_void_ptr(PyObject *obj);

PyObject *sip_convert_from_const_void_ptr(sipSipModuleState *sms,
        const void *val);
PyObject *sip_convert_from_void_ptr(sipSipModuleState *sms, void *val);
int sip_void_ptr_init(PyObject *module, sipSipModuleState *sms);

#ifdef __cplusplus
}
#endif

#endif
