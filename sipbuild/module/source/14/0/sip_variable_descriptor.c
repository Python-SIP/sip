/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the variable descriptor type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_variable_descriptor.h"

#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_variable.h"
#include "sip_wrapper_type.h"


/******************************************************************************
 * We don't use the similar Python descriptor because of the mixin support.
 *****************************************************************************/


/*
 * The object data structure.
 */
typedef struct {
    PyObject_HEAD

    /* The wrapped variable specification. */
    const sipAttrSpec *attr_spec;

    /* A strong reference to the defining type. */
    PyObject *defining_type;
} VariableDescr;


/* Forward declarations of slot functions. */
static int VariableDescr_clear(VariableDescr *self);
static void VariableDescr_dealloc(VariableDescr *self);
static PyObject *VariableDescr_descr_get(VariableDescr *self, PyObject *obj,
        PyObject *type);
static int VariableDescr_descr_set(VariableDescr *self, PyObject *obj,
        PyObject *value);
static int VariableDescr_traverse(VariableDescr *self, visitproc visit,
        void *arg);


/*
 * The type slots.
 */
static PySlot VariableDescr_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".variabledescriptor"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (VariableDescr)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_DISALLOW_INSTANTIATION |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_HAVE_GC),
    PySlot_FUNC(Py_tp_clear, VariableDescr_clear),
    PySlot_FUNC(Py_tp_dealloc, VariableDescr_dealloc),
    PySlot_FUNC(Py_tp_descr_get, VariableDescr_descr_get),
    PySlot_FUNC(Py_tp_descr_set, VariableDescr_descr_set),
    PySlot_FUNC(Py_tp_traverse, VariableDescr_traverse),
    PySlot_END
};


/* Forward declarations. */
static VariableDescr *alloc_variable_descr(sipSipModuleState *sms);


/*
 * Return a new method descriptor for the given getter/setter.
 */
PyObject *sipVariableDescr_New(sipSipModuleState *sms,
        const sipAttrSpec *attr_spec, PyObject *defining_type)
{
    VariableDescr *descr = alloc_variable_descr(sms);

    if (descr != NULL)
    {
        descr->attr_spec = attr_spec;
        descr->defining_type = Py_NewRef(defining_type);
    }

    return (PyObject *)descr;
}


/*
 * The descriptor's descriptor get slot.
 */
static PyObject *VariableDescr_descr_get(VariableDescr *self, PyObject *obj,
        PyObject *type)
{
    PyObject *defining_mod = ((sipWrapperType *)self->defining_type)->defining_module;
    sipModuleState *ms = sip_get_module_state(defining_mod);
    sipSipModuleState *sms = ms->sip_module_state;

    Py_ssize_t mixin_i = sip_get_mixin_index(sms, type, self->defining_type);

    if (mixin_i >= 0)
    {
        if (obj != NULL)
            obj = sip_get_mixin_instance(obj, mixin_i);

        type = self->defining_type;
    }

    return sip_variable_get(ms, obj, self->attr_spec, (PyTypeObject *)type);
}


/*
 * The descriptor's descriptor set slot.
 */
static int VariableDescr_descr_set(VariableDescr *self, PyObject *obj,
        PyObject *value)
{
    PyObject *defining_mod = ((sipWrapperType *)self->defining_type)->defining_module;
    sipModuleState *ms = sip_get_module_state(defining_mod);
    sipSipModuleState *sms = ms->sip_module_state;

    PyObject *type;

    if (obj != NULL)
    {
        type = (PyObject *)Py_TYPE(obj);

        Py_ssize_t mixin_i = sip_get_mixin_index(sms, type,
                self->defining_type);

        if (mixin_i >= 0)
        {
            obj = sip_get_mixin_instance(obj, mixin_i);
            type = self->defining_type;
        }
    }
    else
    {
        type = self->defining_type;
    }

    return sip_variable_set(ms, obj, value, self->attr_spec,
            (PyTypeObject *)type);
}


/*
 * The descriptor's traverse slot.
 */
static int VariableDescr_traverse(VariableDescr *self, visitproc visit,
        void *arg)
{
    Py_VISIT(Py_TYPE(self));
    Py_VISIT(self->defining_type);

    return 0;
}


/*
 * The descriptor's clear slot.
 */
static int VariableDescr_clear(VariableDescr *self)
{
    Py_CLEAR(self->defining_type);

    return 0;
}


/*
 * The descriptor's dealloc slot.
 */
static void VariableDescr_dealloc(VariableDescr *self)
{
    PyObject_GC_UnTrack((PyObject *)self);
    VariableDescr_clear(self);
    PyTypeObject *type = Py_TYPE(self);
    type->tp_free(self);
    Py_DECREF(type);
}


/*
 * Initialise the variable descriptor.
 */
int sip_variable_descr_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, VariableDescr_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_END
    };

    sms->variable_descr_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->variable_descr_type == NULL)
        return -1;

    return 0;
}


/*
 * Allocate a new variable descriptor for a wrapper type.
 */
static VariableDescr *alloc_variable_descr(sipSipModuleState *sms)
{
    return (VariableDescr *)PyType_GenericAlloc(sms->variable_descr_type, 0);
}
