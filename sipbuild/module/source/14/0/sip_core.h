/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the core sip module internal interfaces.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_CORE_H
#define _SIP_CORE_H

#include <Python.h>

#include <stdint.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

#undef  TRUE
#define TRUE        1

#undef  FALSE
#define FALSE       0


/* Macros to access the flags of a type specification. */
#define sipTypeSpecIsClass(ts)      (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_CLASS)
#define sipTypeSpecIsEnum(ts)       (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_ENUM)
#define sipTypeSpecIsException(ts)  (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_EXCEPTION)
#define sipTypeSpecIsExternal(ts)   (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_EXTERNAL)
#define sipTypeSpecIsMapped(ts)     (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_MAPPED)
#define sipTypeSpecIsNamespace(ts)  (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_NAMESPACE)
#if defined(SIP_CONFIGURATION_CustomEnums)
#define sipTypeSpecIsScopedEnum(ts) (((ts)->flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_SCOPED_ENUM)
#endif

#define sipTypeSpecAllowNone(ts)        ((ts)->flags & SIP_TYPE_ALLOW_NONE)
#define sipTypeSpecCallSuperInit(ts)    ((ts)->flags & SIP_TYPE_SUPER_INIT)
#define sipTypeSpecHasSCC(ts)           ((ts)->flags & SIP_TYPE_SCC)
#define sipTypeSpecIsAbstract(ts)       ((ts)->flags & SIP_TYPE_ABSTRACT)
#define sipTypeSpecNeedsUserState(ts)   ((ts)->flags & SIP_TYPE_USER_STATE)


/*
 * The SIP ABI implementation.
 */
extern const sipABISpec sip_abi;


/*
 * These are part of the SIP API but are also used within the SIP module.
 */
int sip_api_convert_from_slice_object(PyObject *slice, Py_ssize_t length,
        Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
        Py_ssize_t *slicelength);
int sip_api_deprecated(const char *classname, const char *method,
        const char *message);
void sip_api_free(void *mem);
void *sip_api_get_address(sipSimpleWrapper *sw);
void *sip_api_get_cpp_ptr(sipModuleState *ms, PyObject *w_inst,
        sipTypeID type_id);
int sip_api_keep_reference(sipModuleState *ms, PyObject *w_inst, int key,
        PyObject *obj);
void *sip_api_malloc(size_t nbytes);


/*
 * These are not part of the SIP API but are used within the SIP module.
 */
int sip_add_attrs_to_type(sipModuleState *ms, PyTypeObject *py_type,
        const sipAttrGroupSpec *attrs, const sipTypeSpec *ts);
void sip_add_to_parent(sipWrapper *self, sipWrapper *owner);
int sip_append_py_object_to_list(sipSipModuleState *sms, PyObject **listp,
        PyObject *object);
void *sip_cast_cpp_ptr(void *ptr, PyTypeObject *src_type,
        PyTypeObject *target_type);
int sip_check_pointer(void *ptr, PyObject *w_inst);
PyTypeObject *sip_create_mapped_type(sipSipModuleState *sms,
        const sipModuleSpec *wmd, const sipMappedTypeSpec *mtd,
        PyObject *w_mod_dict);
int sip_dict_set_and_discard(PyObject *dict, const char *name, PyObject *obj);
void *sip_get_complex_cpp_ptr(sipModuleState *ms, PyObject *w_inst,
        sipTypeID type_id);
void *sip_get_cpp_ptr(PyObject *w_inst, PyTypeObject *target_type);
void *sip_get_final_address(sipSipModuleState *sms, PyTypeObject *py_type,
        void *cpp);
sipConvertFromFunc sip_get_from_convertor(PyTypeObject *py_type,
        const sipTypeSpec *td);
int sip_get_local_py_type(sipModuleState *ms, sipTypeNr type_nr,
        PyTypeObject **py_type_p);
PyTypeObject *sip_get_py_type(sipModuleState *ms, sipTypeID type_id,
        PyObject **def_mod_p);
PyObject *sip_get_type_detail(sipModuleState *ms, sipTypeID type_id,
        const sipTypeSpec **ts_p, PyTypeObject **py_type_p);
PyObject *sip_get_type_name(const sipTypeSpec *ts);
const sipTypeSpec *sip_get_type_spec(sipModuleState *ms, sipTypeID type_id,
        PyObject **def_mod_p);
const sipTypeSpec *sip_get_type_spec_from_wt(sipWrapperType *wt);
int sip_is_subtype(sipModuleState *ms, PyTypeObject *py_type,
        sipTypeID type_id);
void sip_isolate_wrapper(sipModuleState *ms, sipSimpleWrapper *sw);
void sip_raise_no_convert_from(const sipTypeSpec *td);
void sip_remove_from_parent(sipWrapper *self);
int sip_set_qualname(sipModuleState *ms, const sipTypeSpec *ts,
        PyObject *py_type);
void sip_transfer_back(sipSipModuleState *sms, PyObject *self);
void sip_transfer_to(sipSipModuleState *sms, PyObject *self, PyObject *owner);
sipTypeID sip_type_scope(sipModuleState *ms, sipTypeID type_id);

#ifdef __cplusplus
}
#endif

#endif
