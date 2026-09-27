/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This defines the helpers for handwritten code that are not needed otherwise.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_HELPERS_H
#define _SIP_HELPERS_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

void sip_api_bad_length_for_slice(Py_ssize_t seqlen, Py_ssize_t slicelen);
PyObject *sip_api_convert_from_enum(sipModuleState *ms, int eval,
        sipTypeID type_id);
Py_ssize_t sip_api_convert_from_sequence_index(Py_ssize_t idx, Py_ssize_t len);
int sip_api_convert_to_enum(sipModuleState *ms, PyObject *obj,
        sipTypeID type_id);
int sip_api_enable_gc(int enable);
PyObject *sip_api_from_date(const sipDateDef *date);
PyObject *sip_api_from_date_time(const sipDateDef *date,
        const sipTimeDef *time);
PyObject *sip_api_from_method(const sipMethodDef *method);
PyObject *sip_api_from_time(const sipTimeDef *time);
sipAssignFunc sip_api_get_assignment_function(sipModuleState *ms,
        sipTypeID type_id);
int sip_api_get_c_function(PyObject *obj, sipCFunctionDef *c_function);
int sip_api_get_date(PyObject *obj, sipDateDef *date);
int sip_api_get_date_time(PyObject *obj, sipDateDef *date, sipTimeDef *time);
PyFrameObject *sip_api_get_frame_ref(int depth);
int sip_api_get_method(PyObject *obj, sipMethodDef *method);
sipModuleState *sip_api_get_module_state(sipModuleState *ms, void *token);
sipModuleState *sip_api_get_module_state_by_type(void *token,
        PyTypeObject *py_type);
int sip_api_get_time(PyObject *obj, sipTimeDef *time);
sipTypeID sip_api_make_absolute(sipModuleState *ms, sipTypeID type_id);
void sip_api_object_dump(PyObject *obj);
const char *sip_api_py_type_name(const PyTypeObject *py_type);
PyObject *sip_api_py_type_dict_ref(PyTypeObject *py_type);
sipTypeID sip_api_type_from_py_type_object(sipModuleState *ms,
        PyTypeObject *py_type);
const char *sip_api_type_name(sipModuleState *ms, sipTypeID type_id);
const void *sip_api_type_plugin_data(sipModuleState *ms, uint32_t plugin_id,
        sipTypeID type_id);
void *sip_api_unicode_data(PyObject *obj, int *char_size, Py_ssize_t *len);
PyObject *sip_api_unicode_new(Py_ssize_t len, unsigned maxchar, int *kind,
        void **data);
void sip_api_unicode_write(int kind, void *data, int index, unsigned value);

#ifdef __cplusplus
}
#endif

#endif
