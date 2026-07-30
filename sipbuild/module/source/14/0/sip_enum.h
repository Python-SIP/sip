/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the enum support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_ENUM_H
#define _SIP_ENUM_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined(SIP_CONFIGURATION_CustomEnums)
/*
 * The meta-type of a custom enum type.
 */
typedef struct {
    /*
     * The super-metatype.  This must be first in the structure so that it can
     * be cast to a PyTypeObject *.
     */
    PyHeapTypeObject super;

    /* The enum specification. */
    const sipEnumTypeSpec *spec;
} sipEnumTypeImpl;
#endif


/* These are part of the module API. */
PyObject *sip_api_convert_from_enum(sipModuleState *ms, void *addr,
        sipTypeID type_id);
#if defined(SIP_CONFIGURATION_PyEnums)
int sip_api_is_enum_flag(sipModuleState *ms, PyObject *obj);
#endif


PyTypeObject *sip_create_enum_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipEnumTypeSpec *ets);
int sip_enum_convert_to_enum(sipModuleState *ms, PyObject *obj, void *addr,
        sipTypeID type_id, int unconstrained);
int sip_enum_init(PyObject *mod, sipSipModuleState *sms);
int sip_enum_is_enum(sipSipModuleState *sms, PyObject *obj);


#ifdef __cplusplus
}
#endif

#endif
