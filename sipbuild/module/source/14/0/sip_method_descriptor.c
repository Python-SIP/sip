/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the method descriptor type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_method_descriptor.h"

#include "sip_attribute.h"
#include "sip_callable.h"
#include "sip_docstrings.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_wrapper_type.h"


/******************************************************************************
 * We don't use the similar Python descriptor because it doesn't support a
 * method having static and non-static overloads, and we handle mixins via a
 * delegate.
 *****************************************************************************/


/*
 * The object data structure.
 */
typedef struct {
    PyObject_HEAD

    /* The callable specification. */
    const sipAttrSpec *attr_spec;

    /* A strong reference to the defining type. */
    PyObject *defining_type;

    /* The type specification of the defining type if it is extendable. */
    const sipTypeSpec *extending_ts;

    /* A strong reference to the implementation module. */
    PyObject *impl_mod;
} MethodDescr;


/* Forward declarations of slot functions. */
static int MethodDescr_clear(MethodDescr *self);
static void MethodDescr_dealloc(MethodDescr *self);
static PyObject *MethodDescr_descr_get(MethodDescr *self, PyObject *obj,
        PyObject *type);
static PyObject *MethodDescr_get_doc(MethodDescr *self, void *closure);
static int MethodDescr_traverse(MethodDescr *self, visitproc visit, void *arg);


/*
 * The type slots.
 */
static PyGetSetDef MethodDescr_getset[] = {
    {"__doc__", (getter)MethodDescr_get_doc},
    {0}
};

static PySlot MethodDescr_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".methoddescriptor"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (MethodDescr)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_DISALLOW_INSTANTIATION |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_HAVE_GC),
    PySlot_STATIC_DATA(Py_tp_getset, MethodDescr_getset),
    PySlot_FUNC(Py_tp_clear, MethodDescr_clear),
    PySlot_FUNC(Py_tp_dealloc, MethodDescr_dealloc),
    PySlot_FUNC(Py_tp_descr_get, MethodDescr_descr_get),
    PySlot_FUNC(Py_tp_traverse, MethodDescr_traverse),
    PySlot_END
};


/* Forward declarations. */
static MethodDescr *alloc_method_descr(sipSipModuleState *sms);


/*
 * Return a new method descriptor for the given method.
 */
PyObject *sipMethodDescr_New(sipSipModuleState *sms,
        const sipAttrSpec *attr_spec, PyObject *impl_mod,
        PyObject *defining_type, const sipTypeSpec *extending_ts)
{
    MethodDescr *descr = alloc_method_descr(sms);

    if (descr != NULL)
    {
        descr->attr_spec = attr_spec;
        descr->defining_type = Py_NewRef(defining_type);
        descr->extending_ts = extending_ts;
        descr->impl_mod = Py_NewRef(impl_mod);
    }

    return (PyObject *)descr;
}


/*
 * The descriptor's descriptor get slot.
 */
static PyObject *MethodDescr_descr_get(MethodDescr *self, PyObject *obj,
        PyObject *type)
{
    sipModuleState *ms = sip_get_module_state(self->impl_mod);
    sipSipModuleState *sms = ms->sip_module_state;

    /* Get the mixin index, if any. */
    Py_ssize_t mixin_i = sip_get_mixin_index(sms, type, self->defining_type);

    /* Determine what the callable is bound to (ie. its self object). */
    PyObject *bind;

    if (obj == NULL)
    {
        /* The argument parser must work out that 'self' is the type object. */
        bind = mixin_i >= 0 ? self->defining_type : type;
    }
    else
    {
        /*
         * The argument parser must work out that 'self' is the instance
         * object.
         */
        bind = mixin_i >= 0 ? sip_get_mixin_instance(obj, mixin_i) : obj;
    }

    return sipCallable_New(sms, self->attr_spec, self->impl_mod, bind,
            self->extending_ts);
}


/*
 * The descriptor's __doc__ getter.
 */
static PyObject *MethodDescr_get_doc(MethodDescr *self,
        void *Py_UNUSED(closure))
{
    return sip_get_callable_docstring(sip_get_module_state(self->impl_mod),
            self->attr_spec, self->extending_ts);
}


/*
 * The descriptor's traverse slot.
 */
static int MethodDescr_traverse(MethodDescr *self, visitproc visit, void *arg)
{
    Py_VISIT(Py_TYPE(self));
    Py_VISIT(self->defining_type);
    Py_VISIT(self->impl_mod);

    return 0;
}


/*
 * The descriptor's clear slot.
 */
static int MethodDescr_clear(MethodDescr *self)
{
    Py_CLEAR(self->defining_type);
    Py_CLEAR(self->impl_mod);

    return 0;
}


/*
 * The descriptor's dealloc slot.
 */
static void MethodDescr_dealloc(MethodDescr *self)
{
    PyObject_GC_UnTrack((PyObject *)self);
    MethodDescr_clear(self);
    PyTypeObject *type = Py_TYPE(self);
    type->tp_free(self);
    Py_DECREF(type);
}


/*
 * Initialise the method descriptor.
 */
int sip_method_descr_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, MethodDescr_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_END
    };

    sms->method_descr_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->method_descr_type == NULL)
        return -1;

    return 0;
}


/*
 * Allocate a new method descriptor for a wrapper type.
 */
static MethodDescr *alloc_method_descr(sipSipModuleState *sms)
{
    return (MethodDescr *)PyType_GenericAlloc(sms->method_descr_type, 0);
}
