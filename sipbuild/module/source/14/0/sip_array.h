/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the array type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_ARRAY_H
#define _SIP_ARRAY_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

PyObject *sip_api_convert_to_array(sipModuleState *ms, void *data,
        const char *format, Py_ssize_t len, int flags);
PyObject *sip_api_convert_to_typed_array(sipModuleState *ms, void *data,
        sipTypeID type_id, const char *format, size_t stride, Py_ssize_t len,
        int flags);

int sip_array_can_convert(sipModuleState *ms, PyObject *obj,
        sipTypeID type_id);
void sip_array_convert(PyObject *obj, void **data, Py_ssize_t *size);
int sip_array_init(PyObject *module, sipSipModuleState *sms);
PyObject *sip_array_from_bytes(sipSipModuleState *sms, void *data,
        Py_ssize_t size, int rw);

#ifdef __cplusplus
}
#endif

#endif
