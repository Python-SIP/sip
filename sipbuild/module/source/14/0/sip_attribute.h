/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the attribute support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_ATTRIBUTE_H
#define _SIP_ATTRIBUTE_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Macros to extract the name and type of an attribute. */
#define sipAttrGetName(a)               ((a)->name + 1)
#define sipAttrIsClassCallable(a)       ((a)->name[0] == 'c')
#define sipAttrIsExtendableSlot(a)      ((a)->name[0] == 'e')
#define sipAttrIsInstanceVariable(a)    ((a)->name[0] == 'i')
#define sipAttrIsModuleCallable(a)      ((a)->name[0] == 'm')
#define sipAttrIsSlot(a)                ((a)->name[0] == 's')
#define sipAttrIsStaticVariable(a)      ((a)->name[0] == 'v')
#define sipAttrIsType(a)                ((a)->name[0] == 't')


PyObject *sip_get_attribute_for_spec(sipModuleState *ms, PyObject *self,
        const sipAttrSpec *attr_spec, const sipTypeSpec *extending_ts);
const sipAttrSpec *sip_get_attribute_spec(const char *name,
        const sipAttrGroupSpec *attrs);
PyObject *sip_dir_of_module(PyObject *mod, PyObject *attr_dict);
PyObject *sip_dir_of_wt(sipWrapperType *wt, PyObject *attr_dict);
PyObject *sip_mod_con_getattro(sipModuleState *ms, PyObject *self,
        PyObject *name, PyObject *attr_dict,
        const sipAttrGroupSpec *const attrs_static_variables,
        const sipAttrGroupSpec *const attrs_types,
        const sipTypeSpec *extending_ts);
int sip_mod_con_setattro(sipModuleState *ms, PyObject *self, PyObject *name,
        PyObject *value, const sipAttrGroupSpec *const attrs_static_variables,
        const sipTypeSpec *extending_ts);

#ifdef __cplusplus
}
#endif

#endif
