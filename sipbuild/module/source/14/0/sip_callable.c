/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the callable type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stddef.h>

#include "sip_callable.h"

#include "sip_attribute.h"
#include "sip_docstrings.h"
#include "sip_iterators.h"
#include "sip_parsers.h"
#include "sip_sip_module.h"


/*
 * The object data structure.
 */
typedef struct {
    PyObject_HEAD

    /* The callable specification. */
    const sipAttrSpec *attr_spec;

    /* The type specification of the containing type if it is extendable. */
    const sipTypeSpec *extending_ts;

    /* A strong reference to the implementation module. */
    PyObject *impl_mod;

    /* A strong reference to the optional self object. */
    PyObject *self;

    /* The vectorcall implementation. */
    vectorcallfunc vectorcall;
} CallableObject;


/* Forward declarations of slot functions. */
static int Callable_clear(CallableObject *self);
static void Callable_dealloc(CallableObject *self);
static PyObject *Callable_get_doc(CallableObject *self, void *closure);
static int Callable_traverse(CallableObject *self, visitproc visit, void *arg);
static PyObject *Callable_vectorcall(CallableObject *self,
        PyObject *const *args, size_t nargsf, PyObject *kwd_names);


/*
 * The type slots.
 */
static PyMemberDef Callable_members[] = {
    {"__vectorcalloffset__", Py_T_PYSSIZET, offsetof(CallableObject, vectorcall), Py_READONLY},
    {0}
};

static PyGetSetDef Callable_getset[] = {
    {"__doc__", (getter)Callable_get_doc},
    {0}
};

static PySlot Callable_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_MODULE_FQ_NAME ".callable"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (CallableObject)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_DISALLOW_INSTANTIATION |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_HAVE_GC |
            Py_TPFLAGS_HAVE_VECTORCALL),
    PySlot_STATIC_DATA(Py_tp_getset, Callable_getset),
    PySlot_STATIC_DATA(Py_tp_members, Callable_members),
    PySlot_FUNC(Py_tp_call, PyVectorcall_Call),
    PySlot_FUNC(Py_tp_clear, Callable_clear),
    PySlot_FUNC(Py_tp_dealloc, Callable_dealloc),
    PySlot_FUNC(Py_tp_traverse, Callable_traverse),
    PySlot_END
};


/* The closure used when iterating over attributes looking for a callable. */
typedef struct {
    PyObject **p_state_p;
    PyObject *self;
    PyObject *const *args;
    Py_ssize_t nr_args;
    PyObject *kwd_names;
    PyObject *result;
} CallClosure;


/* Remaining forward declarations. */
static PyObject *call_extender(sipModuleState *ms, PyObject **p_state_p,
        PyObject *self, PyObject *const *args, Py_ssize_t nr_args,
        PyObject *kwd_names, const sipTypeSpec *extending_ts,
        const char *name);
static int call_extender_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure);


/*
 * Return a new callable.
 */
PyObject *sipCallable_New(sipSipModuleState *sms, const sipAttrSpec *attr_spec,
        PyObject *impl_mod, PyObject *self, const sipTypeSpec *extending_ts)
{
    assert(sipAttrIsClassCallable(attr_spec) || \
           sipAttrIsModuleCallable(attr_spec) || \
           sipAttrIsSlot(attr_spec) || \
           sipAttrIsExtendableSlot(attr_spec));

    CallableObject *callable = (CallableObject *)PyType_GenericAlloc(
            sms->callable_type, 0);

    if (callable != NULL)
    {
        callable->attr_spec = attr_spec;
        callable->impl_mod = Py_NewRef(impl_mod);
        callable->self = Py_XNewRef(self);
        callable->extending_ts = extending_ts;
        callable->vectorcall = (vectorcallfunc)Callable_vectorcall;
    }

    return (PyObject *)callable;
}


/*
 * The callable's clear slot.
 */
static int Callable_clear(CallableObject *self)
{
    Py_CLEAR(self->impl_mod);
    Py_CLEAR(self->self);

    return 0;
}


/*
 * The callable's dealloc slot.
 */
static void Callable_dealloc(CallableObject *self)
{
    PyObject_GC_UnTrack((PyObject *)self);
    Callable_clear(self);
    PyTypeObject *type = Py_TYPE(self);
    type->tp_free(self);
    Py_DECREF(type);
}


/*
 * The callable's __doc__ getter.
 */
static PyObject *Callable_get_doc(CallableObject *self,
        void *Py_UNUSED(closure))
{
    return sip_get_callable_docstring(sip_get_module_state(self->impl_mod),
            self->attr_spec, self->extending_ts);
}


/*
 * The callable's traverse slot.
 */
static int Callable_traverse(CallableObject *self, visitproc visit, void *arg)
{
    Py_VISIT(Py_TYPE(self));
    Py_VISIT(self->impl_mod);
    Py_VISIT(self->self);

    return 0;
}


/*
 * The callable's vectorcall slot.
 */
static PyObject *Callable_vectorcall(CallableObject *self,
        PyObject *const *args, size_t nargsf, PyObject *kwd_names)
{
    sipModuleState *ms = sip_get_module_state(self->impl_mod);
    PyObject *p_state = NULL;
    Py_ssize_t nr_args = PyVectorcall_NARGS(nargsf);
    const sipAttrSpec *attr_spec = self->attr_spec;
    PyObject *res;

    /*
     * If we have a callable implementation then invoke it.  Otherwise there is
     * no implementation and we are a slot extender dispatcher.
     */
    if (attr_spec->spec.callable != NULL)
        res = attr_spec->spec.callable->callable_impl(ms, &p_state, self->self,
                args, nr_args, kwd_names);
    else
        res = NULL;

    /*
     * If we haven't found a matching signature (and there has been no error)
     * and the type is extendable then look for other possible implementations.
     */
    if (res == NULL && p_state != Py_None && self->extending_ts != NULL)
        res = call_extender(ms, &p_state, self->self, args, nr_args, kwd_names,
                self->extending_ts, sipAttrGetName(attr_spec));

    if (res != NULL)
        return res;

    /*
     * A failed slot call should return NotImplemented in order to trigger the
     * standard message.
     */
    if (p_state != NULL && p_state != Py_None && sipAttrIsExtendableSlot(attr_spec))
    {
        Py_DECREF(p_state);
        PyErr_Clear();
        Py_RETURN_NOTIMPLEMENTED;
    }

    sip_no_callable(p_state,
            self->self != NULL ? Py_TYPE(self->self)->tp_name : NULL,
            sipAttrGetName(attr_spec));

    return NULL;
}


/*
 * Initialise the callable type.
 */
int sip_callable_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, Callable_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_END
    };

    sms->callable_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->callable_type == NULL)
        return -1;

    return 0;
}


/*
 * Call any extensions to a callable until a result is obtained, an error
 * occurs or no appropriate extender was found.
 */
static PyObject *call_extender(sipModuleState *ms, PyObject **p_state_p,
        PyObject *self, PyObject *const *args, Py_ssize_t nr_args,
        PyObject *kwd_names, const sipTypeSpec *extending_ts, const char *name)
{
    CallClosure call_closure = {
        .p_state_p = p_state_p,
        .self = self,
        .args = args,
        .nr_args = nr_args,
        .kwd_names = kwd_names,
    };

    int state = sip_iterate_over_attributes(ms, extending_ts, name,
            call_extender_handler, &call_closure);

    /* The caller uses the parser state to determine if there was an error. */
    if (state < 0)
        sip_api_set_parser_error(p_state_p);

    return state > 0 ? call_closure.result : NULL;
}


/*
 * The attribute iterator handler that calls a callable.
 */
static int call_extender_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure)
{
    CallClosure *cc = (CallClosure *)closure;

    cc->result = x_attr_spec->spec.callable->callable_impl(x_ms, cc->p_state_p,
            cc->self, cc->args, cc->nr_args, cc->kwd_names);

    /* See if there was a result. */
    if (cc->result != NULL)
        return 1;

    /* Stop if there was an error. */
    return *cc->p_state_p == Py_None ? -1 : 0;
}
