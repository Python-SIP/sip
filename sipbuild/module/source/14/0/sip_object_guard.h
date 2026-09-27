/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the object guard support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_OBJECT_GUARD_H
#define _SIP_OBJECT_GUARD_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif


sipObjectGuard *sip_api_object_guard_new(sipModuleState *ms, PyObject *obj);
sipModuleState *sip_api_object_guard_get_module_state(sipObjectGuard *guard);
PyObject *sip_api_object_guard_get_ref(sipObjectGuard *guard,
        PyThreadStateToken **tst_p);
void sip_api_object_guard_release(sipObjectGuard *guard);
int sip_api_object_guard_clear(sipModuleState *ms, sipObjectGuard *guard);
int sip_api_object_guard_free(sipModuleState *ms, sipObjectGuard *guard);
int sip_api_object_guard_traverse(sipModuleState *ms, sipObjectGuard *guard,
        visitproc visit, void *arg);


#ifdef __cplusplus
}
#endif

#endif
