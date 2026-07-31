/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the sip simple wrapper type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_SIMPLE_WRAPPER_H
#define _SIP_SIMPLE_WRAPPER_H

#include <Python.h>

#include "sip.h"
#include "sip_decls.h"


#ifdef __cplusplus
extern "C" {
#endif

/* These are held in flags. */
#define SIP_PY_OWNED        0x0010  /* If owned by Python. */
#define SIP_SHARE_MAP       0x0020  /* Allow the map slot to be shared. */
#define SIP_CPP_HAS_REF     0x0040  /* If C/C++ has a reference. */
#define SIP_POSSIBLE_PROXY  0x0080  /* If there might be a proxy slot. */
#define SIP_ALIAS           0x0100  /* If it is an alias. */
#define SIP_CREATED         0x0200  /* If the C/C++ object has been created. */

#define sipIsDerived(sw)    ((sw)->flags & SIP_DERIVED_CLASS)
#define sipIsPyOwned(sw)    ((sw)->flags & SIP_PY_OWNED)
#define sipSetPyOwned(sw)   ((sw)->flags |= SIP_PY_OWNED)
#define sipResetPyOwned(sw) ((sw)->flags &= ~SIP_PY_OWNED)
#define sipCppHasRef(sw)    ((sw)->flags & SIP_CPP_HAS_REF)
#define sipSetCppHasRef(sw) ((sw)->flags |= SIP_CPP_HAS_REF)
#define sipResetCppHasRef(sw)   ((sw)->flags &= ~SIP_CPP_HAS_REF)
#define sipPossibleProxy(sw)    ((sw)->flags & SIP_POSSIBLE_PROXY)
#define sipSetPossibleProxy(sw) ((sw)->flags |= SIP_POSSIBLE_PROXY)
#define sipIsAlias(sw)      ((sw)->flags & SIP_ALIAS)
#define sipWasCreated(sw)   ((sw)->flags & SIP_CREATED)


/*
 * The type of a simple C/C++ wrapper object.
 */
struct sipSimpleWrapperImpl {
    PyObject_HEAD

    /* The data, ie. a pointer to the C/C++ object. */
    void *data;

    /* The instance dictionary. */
    PyObject *dict;

    /* The optional dictionary of extra references using an int key. */
    PyObject *extra_refs;

    /* Object flags. */
    unsigned flags;

    /* The main instance if this is a mixin. */
    sipSimpleWrapper *mixin_main;

    /* The optional list of mixin instances. */
    PyObject *mixins;

    /* Next object at this address. */
    sipSimpleWrapper *next;

    /* For the user to use. */
    PyObject *user;
};


int SimpleWrapper_getbuffer(PyObject *self, Py_buffer *buf, int flags);
void SimpleWrapper_releasebuffer(PyObject *self, Py_buffer *buf);

int sip_api_init_slot_impl(PyObject *self, PyObject *args, PyObject *kwds,
        void *module_token, sipTypeID type_id);
Py_ssize_t sip_get_mixin_index(sipSipModuleState *sms, PyObject *py_type,
        PyObject *defining_type);
PyObject *sip_get_mixin_instance(PyObject *main_object, Py_ssize_t mixin_i);
int sip_simple_wrapper_init(PyObject *module, sipSipModuleState *sms);
PyObject *sip_wrap_instance(sipModuleState *ms, void *cpp,
        PyTypeObject *py_type, PyObject *args, PyObject *owner, int flags);

#ifdef __cplusplus
}
#endif

#endif
