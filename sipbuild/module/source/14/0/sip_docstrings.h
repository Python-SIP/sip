/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the docstrings support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_DOCSTRINGS_H
#define _SIP_DOCSTRINGS_H

#include <Python.h>

#include "sip.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper_type.h"


#ifdef __cplusplus
extern "C" {
#endif

PyObject *sip_get_callable_docstring(sipModuleState *ms,
        const sipAttrSpec *attr_spec, const sipTypeSpec *extending_ts);
PyObject *sip_get_class_docstring(sipWrapperType *wt);

#ifdef __cplusplus
}
#endif

#endif
