/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the method descriptor type.
 *
 * Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_METHOD_DESCRIPTOR_H
#define _SIP_METHOD_DESCRIPTOR_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

PyObject *sipMethodDescr_New(sipSipModuleState *sms,
        const sipAttrSpec *attr_spec, PyObject *impl_mod,
        PyObject *defining_type, const sipTypeSpec *extending_ts);
int sip_method_descr_init(PyObject *module, sipSipModuleState *sms);

#ifdef __cplusplus
}
#endif

#endif
