/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This is the implementation of the sip module wrapper type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_module_wrapper.h"

#include "sip_attribute.h"
#include "sip_core.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"


/* Forward declarations of slot functions. */
static PyObject *ModuleWrapper_dir(PyObject *self, PyObject *args);
static PyObject *ModuleWrapper_getattro(PyObject *self, PyObject *name);
static int ModuleWrapper_setattro(PyObject *self, PyObject *name,
        PyObject *value);


/*
 * The type slots.
 */
static PyMethodDef ModuleWrapper_methods[] = {
    {"__dir__", ModuleWrapper_dir, METH_NOARGS, NULL},
    {0}
};

static PySlot ModuleWrapper_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_MODULE_FQ_NAME ".modulewrapper"),
    PySlot_UINT64(Py_tp_flags, Py_TPFLAGS_DEFAULT),
    PySlot_STATIC_DATA(Py_tp_methods, ModuleWrapper_methods),
    PySlot_FUNC(Py_tp_getattro, ModuleWrapper_getattro),
    PySlot_FUNC(Py_tp_setattro, ModuleWrapper_setattro),
    PySlot_END
};


/*
 * The module __dir__() implementation.
 */
static PyObject *ModuleWrapper_dir(PyObject *self, PyObject *Py_UNUSED(args))
{
    PyObject *mod_dict = PyModule_GetDict(self);
    if (mod_dict == NULL)
        return NULL;

    PyObject *attr_dict = PyDict_Copy(mod_dict);
    if (attr_dict == NULL)
        return NULL;

    PyObject *dir = sip_dir_of_module(self, attr_dict);

    Py_DECREF(attr_dict);

    return dir;
}


/*
 * The module getattro slot.
 */
static PyObject *ModuleWrapper_getattro(PyObject *self, PyObject *name)
{
    PyObject *mod_dict = PyModule_GetDict(self);
    if (mod_dict == NULL)
        return NULL;

    sipModuleState *ms = sip_get_module_state(self);

    return sip_mod_con_getattro(ms, self, name, mod_dict,
            ms->module_spec->attrs_static_variables,
            ms->module_spec->attrs_types, NULL);
}


/*
 * The module setattro slot.
 */
static int ModuleWrapper_setattro(PyObject *self, PyObject *name,
        PyObject *value)
{
    sipModuleState *ms = sip_get_module_state(self);

    return sip_mod_con_setattro(ms, self, name, value,
            ms->module_spec->attrs_static_variables, NULL);
}


/*
 * Initialise the type.
 */
int sip_module_wrapper_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, ModuleWrapper_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_DATA(Py_tp_bases, &PyModule_Type),
        PySlot_END
    };

    sms->module_wrapper_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->module_wrapper_type == NULL)
        return -1;

    if (PyModule_AddType(module, sms->module_wrapper_type) < 0)
        return -1;

    return 0;
}
