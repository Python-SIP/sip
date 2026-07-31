/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the argument parsers support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_PARSERS_H
#define _SIP_PARSERS_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

void sip_api_add_exception(sipErrorState es, PyObject **p_state_p,
        const char *type_hint);
sipErrorState sip_api_bad_callable_arg(int arg_nr, PyObject *arg);
void sip_api_bad_catcher_result(PyObject *method);
PyObject *sip_api_build_result(sipModuleState *ms, int *is_err_p,
        const char *fmt, ...);
void sip_api_call_error_handler(sipModuleState *ms, const char *error_handler,
        sipSimpleWrapper *sw, PyThreadStateToken *tst);
PyObject *sip_api_call_method(sipModuleState *ms, int *is_err_p,
        PyObject *method, const char *fmt, ...);
void sip_api_call_procedure_method(sipModuleState *ms,
        PyThreadStateToken *tst, const char *error_handler,
        sipSimpleWrapper *py_self, PyObject *method, const char *fmt, ...);
int sip_api_can_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, int flags);
PyObject *sip_api_convert_from_new_py_type(sipModuleState *ms, void *cpp,
        PyTypeObject *py_type, sipWrapper *owner, sipSimpleWrapper **self_p,
        const char *fmt, ...);
PyObject *sip_api_convert_from_new_type(sipModuleState *ms, void *cpp,
        sipTypeID type_id, PyObject *transferObj);
PyObject *sip_api_convert_from_type(sipModuleState *ms, void *cpp,
        sipTypeID type_id, PyObject *transferObj);
void *sip_api_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
void *sip_api_convert_to_type_us(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        void **user_statep, int *iserrp);
void *sip_api_force_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
void *sip_api_force_convert_to_type_us(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        void **user_statep, int *iserrp);
PyObject *sip_api_get_py_object_ref(sipModuleState *ms, void *cppPtr,
        sipTypeID type_id);
PyObject *sip_api_is_py_method(sipModuleState *ms, PyThreadStateToken **tst_p,
        char *pymc, sipSimpleWrapper **self_p, const char *cname,
        const char *mname);
bool sip_api_parse_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **self_p,
        sipTypeID self_type_id, void **self_cpp_p);
bool sip_api_parse_args_c(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **self_p,
        sipTypeID self_type_id, void **self_cpp_p, const char *type_hint);
bool sip_api_parse_ctor_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject *self,
        PyObject **unused_p);
int sip_api_parse_result(sipModuleState *ms, PyThreadStateToken *tst,
        const char *error_handler, sipSimpleWrapper *sw, PyObject *method,
        PyObject *res, const char *fmt, ...);
void sip_api_release_type(sipModuleState *ms, void *cpp, sipTypeID type_id,
        int state);
void sip_api_release_type_us(sipModuleState *ms, void *cpp, sipTypeID type_id,
        int state, void *user_state);
void sip_api_set_parser_error(PyObject **p_state_p);

int sip_copy_wrapped_type(sipModuleState *ms, sipTypeID type_id, void *dst,
        void *src);
void sip_no_callable(PyObject *p_state, const char *scope, const char *name);
void sip_release(void *addr, const sipTypeSpec *td, int state,
        void *user_state);

#ifdef __cplusplus
}
#endif

#endif
