/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the wrapped variable support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_VARIABLE_H
#define _SIP_VARIABLE_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

PyObject *sip_variable_get(sipModuleState *ms, PyObject *instance,
        const sipAttrSpec *attr_spec, PyTypeObject *binding_type);
int sip_variable_set(sipModuleState *ms, PyObject *instance, PyObject *value,
        const sipAttrSpec *attr_spec, PyTypeObject *binding_type);

#ifdef __cplusplus
}
#endif

#endif
