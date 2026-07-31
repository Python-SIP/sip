/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This is the implementation of the sip wrapper type type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_wrapper_type.h"

#include "sip_attribute.h"
#include "sip_core.h"
#include "sip_docstrings.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"


/* Forward declarations of slot functions. */
static int WrapperType_clear(sipWrapperType *self);
static void WrapperType_dealloc(sipWrapperType *self);
static PyObject *WrapperType_dir(sipWrapperType *self, PyObject *args);
static PyObject *WrapperType_getattro(sipWrapperType *self, PyObject *name);
static PyObject *WrapperType_get_doc(sipWrapperType *self, void *closure);
static int WrapperType_init(sipWrapperType *self, PyObject *args,
        PyObject *kwds);
static int WrapperType_setattro(sipWrapperType *self, PyObject *name,
        PyObject *value);
static int WrapperType_traverse(sipWrapperType *self, visitproc visit,
        void *arg);


/*
 * The type slots.
 */
static PyGetSetDef WrapperType_getset[] = {
    {"__doc__", (getter)WrapperType_get_doc},
    {0}
};

static PyMethodDef WrapperType_methods[] = {
    {"__dir__", (PyCFunction)WrapperType_dir, METH_NOARGS, NULL},
    {0}
};

static PySlot WrapperType_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".wrappertype"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (sipWrapperType)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_BASETYPE |
            Py_TPFLAGS_HAVE_GC |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_TYPE_SUBCLASS),
    PySlot_STATIC_DATA(Py_tp_getset, WrapperType_getset),
    PySlot_STATIC_DATA(Py_tp_methods, WrapperType_methods),
    PySlot_FUNC(Py_tp_clear, WrapperType_clear),
    PySlot_FUNC(Py_tp_dealloc, WrapperType_dealloc),
    PySlot_FUNC(Py_tp_getattro, WrapperType_getattro),
    PySlot_FUNC(Py_tp_init, WrapperType_init),
    PySlot_FUNC(Py_tp_setattro, WrapperType_setattro),
    PySlot_FUNC(Py_tp_traverse, WrapperType_traverse),
    PySlot_END
};


/*
 * The metatype clear slot.
 */
static int WrapperType_clear(sipWrapperType *self)
{
    Py_CLEAR(self->defining_module);
    Py_CLEAR(self->user_data);

    return 0;
}


/*
 * The metatype dealloc slot.
 */
static void WrapperType_dealloc(sipWrapperType *self)
{
    PyObject_GC_UnTrack((PyObject *)self);

    WrapperType_clear(self);

    PyTypeObject *type = Py_TYPE(self);
    type->tp_free(self);
    Py_DECREF(type);
}


/*
 * The metatype __dir__() implementation.
 */
static PyObject *WrapperType_dir(sipWrapperType *self,
        PyObject *Py_UNUSED(args))
{
    PyObject *attr_dict = PyDict_New();
    if (attr_dict == NULL)
        return NULL;

    PyObject *dir = sip_dir_of_wt(self, attr_dict);

    Py_DECREF(attr_dict);

    return dir;
}


/*
 * The metatype getattro slot.
 */
static PyObject *WrapperType_getattro(sipWrapperType *self, PyObject *name)
{
    /* Python itself may make calls along the MRO. */
    if (self->defining_module == NULL)
        return PyType_Type.tp_getattro((PyObject *)self, name);

    const sipTypeSpec *ts = sip_get_type_spec_from_wt(self);
    const sipAttrGroupSpec *attrs_static_variables, *attrs_types;

    if (sipTypeSpecIsMapped(ts))
    {
        attrs_static_variables = NULL;
        attrs_types = ((const sipMappedTypeSpec *)ts)->attrs_types;
    }
    else
    {
        attrs_static_variables = ((const sipClassTypeSpec *)ts)->attrs_static_variables;
        attrs_types = ((const sipClassTypeSpec *)ts)->attrs_types;
    }

    return sip_mod_con_getattro(sip_get_module_state(self->defining_module),
            (PyObject *)self, name, ((PyTypeObject *)self)->tp_dict,
            attrs_static_variables, attrs_types,
            sipTypeSpecIsNamespace(ts) ? ts : NULL);
}


/*
 * The metatype __doc__ getter.
 */
static PyObject *WrapperType_get_doc(sipWrapperType *self,
        void *Py_UNUSED(closure))
{
    return sip_get_class_docstring(self);
}


/*
 * The metatype init slot.  Note that this is *not* called for wrapped types
 * (because they are created using PyType_FromMetaclass()) but is called for
 * Python sub-classes.
 */
static int WrapperType_init(sipWrapperType *self, PyObject *args,
        PyObject *kwds)
{
    /* Call the standard super-metatype init. */
    if (PyType_Type.tp_init((PyObject *)self, args, kwds) < 0)
        return -1;

    /*
     * Disallow this being used as a meta-type for anything other than a
     * wrapped class.
     */
    sipSipModuleState *sms = sip_get_sip_module_state_from_type(
            (PyTypeObject *)self);
    PyTypeObject *base = ((PyTypeObject *)self)->tp_base;

    if (sms == NULL || base == NULL || !PyObject_TypeCheck((PyObject *)base, sms->wrapper_type_type))
    {
        PyErr_SetString(PyExc_TypeError,
                _SIP_MODULE_FQ_NAME ".wrappertype can only be used as the "
                "metatype for wrapped classes");
        return -1;
    }

    /* Inherit from the base class. */
    self->defining_module = Py_XNewRef(
            ((sipWrapperType *)base)->defining_module);
    self->is_wrapper = ((sipWrapperType *)base)->is_wrapper;
    self->type_id = ((sipWrapperType *)base)->type_id;

    /* Disallow sub-classing directly from simplewrapper or wrapper. */
    if (self->defining_module == NULL)
    {
        PyErr_Format(PyExc_TypeError,
                "Python classes cannot sub-class directly from %s",
                base->tp_name);
        return -1;
    }

    self->user_type = TRUE;

    return 0;
}


/*
 * The metatype setattro slot.
 */
static int WrapperType_setattro(sipWrapperType *self, PyObject *name,
        PyObject *value)
{
    /* Python itself may make calls along the MRO. */
    if (self->defining_module == NULL)
        return PyType_Type.tp_setattro((PyObject *)self, name, value);

    const sipTypeSpec *ts = sip_get_type_spec_from_wt(self);
    const sipAttrGroupSpec *attrs_static_variables;

    if (sipTypeSpecIsMapped(ts))
        attrs_static_variables = NULL;
    else
        attrs_static_variables = ((const sipClassTypeSpec *)ts)->attrs_static_variables;

    return sip_mod_con_setattro(sip_get_module_state(self->defining_module),
            (PyObject *)self, name, value, attrs_static_variables,
            sipTypeSpecIsNamespace(ts) ? ts : NULL);
}


/*
 * The metatype traverse slot.
 */
static int WrapperType_traverse(sipWrapperType *self, visitproc visit,
        void *arg)
{
    Py_VISIT(Py_TYPE(self));

    Py_VISIT(self->defining_module);
    Py_VISIT(self->user_data);

    return 0;
}


/*
 * Initialise the metatype.
 */
int sip_wrapper_type_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, WrapperType_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_DATA(Py_tp_bases, &PyType_Type),
        PySlot_END
    };

    sms->wrapper_type_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->wrapper_type_type == NULL)
        return -1;

    if (PyModule_AddType(module, sms->wrapper_type_type) < 0)
        return -1;

    return 0;
}
