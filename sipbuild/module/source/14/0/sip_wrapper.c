/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the sip wrapper type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_wrapper.h"

#include "sip_sip_module.h"


/*
 * The type slots.  Note that the slots are implemented by simplewrapper which
 * will deal with the extra requirements of this type.  It is done this way
 * because the correct type can only be determined at run-time.
 */
static PySlot Wrapper_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".wrapper"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (sipWrapper)),
    PySlot_UINT64(Py_tp_flags, Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE),
    PySlot_END
};


/*
 * Initialise the wrapper type.
 */
int sip_wrapper_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, Wrapper_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_DATA(Py_tp_metaclass, sms->wrapper_type_type),
        PySlot_DATA(Py_tp_bases, sms->simple_wrapper_type),
        PySlot_END
    };

    sms->wrapper_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->wrapper_type == NULL)
        return -1;

    if (PyModule_AddType(module, sms->wrapper_type) < 0)
        return -1;

    return 0;
}
