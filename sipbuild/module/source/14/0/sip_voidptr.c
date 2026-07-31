/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file implements the API for the voidptr type.
 *
 * Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stddef.h>
#include <string.h>

#include "sip_voidptr.h"

#include "sip_array.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"


/* The object data structure. */
typedef struct {
    PyObject_HEAD
    void *voidptr;
    Py_ssize_t size;
    int rw;
} VoidPtr;


/* Forward declarations of methods. */
static PyObject *VoidPtr_asarray(VoidPtr *v, PyObject *args, PyObject *kw);
static PyObject *VoidPtr_ascapsule(VoidPtr *v, PyObject *arg);
static PyObject *VoidPtr_asstring(VoidPtr *v, PyObject *args, PyObject *kw);
static PyObject *VoidPtr_getsize(VoidPtr *v, PyObject *arg);
static PyObject *VoidPtr_setsize(VoidPtr *v, PyObject *arg);
static PyObject *VoidPtr_getwriteable(VoidPtr *v, PyObject *arg);
static PyObject *VoidPtr_setwriteable(VoidPtr *v, PyObject *arg);


/* Forward declarations of slot functions. */
static int VoidPtr_ass_subscript(PyObject *self, PyObject *key,
        PyObject *value);
static int VoidPtr_bool(PyObject *self);
static void VoidPtr_dealloc(PyObject *self);
static int VoidPtr_getbuffer(PyObject *self, Py_buffer *view, int flags);
static PyObject *VoidPtr_int(PyObject *self);
static PyObject *VoidPtr_item(PyObject *self, Py_ssize_t idx);
static Py_ssize_t VoidPtr_length(PyObject *self);
static PyObject *VoidPtr_new(PyTypeObject *cls, PyObject *args, PyObject *kw);
static PyObject *VoidPtr_subscript(PyObject *self, PyObject *key);
static int VoidPtr_traverse(PyObject *self, visitproc visit, void *arg);


/*
 * The type slots.  Note that we don't use the
 * METH_METHOD|METH_FASTCALL|METH_KEYWORDS calling convention to get the
 * defining class because there is no public argument parsing support.
 */
static PyMethodDef VoidPtr_Methods[] = {
    {"asarray", (PyCFunction)VoidPtr_asarray, METH_VARARGS|METH_KEYWORDS, NULL},
    {"ascapsule", (PyCFunction)VoidPtr_ascapsule, METH_NOARGS, NULL},
    {"asstring", (PyCFunction)VoidPtr_asstring, METH_VARARGS|METH_KEYWORDS, NULL},
    {"getsize", (PyCFunction)VoidPtr_getsize, METH_NOARGS, NULL},
    {"setsize", (PyCFunction)VoidPtr_setsize, METH_O, NULL},
    {"getwriteable", (PyCFunction)VoidPtr_getwriteable, METH_NOARGS, NULL},
    {"setwriteable", (PyCFunction)VoidPtr_setwriteable, METH_O, NULL},
    {0}
};

static PySlot VoidPtr_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".voidptr"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (VoidPtr)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_DISALLOW_INSTANTIATION |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_HAVE_GC),
    PySlot_STATIC_DATA(Py_tp_methods, VoidPtr_Methods),
    PySlot_FUNC(Py_bf_getbuffer, VoidPtr_getbuffer),
    PySlot_FUNC(Py_mp_ass_subscript, VoidPtr_ass_subscript),
    PySlot_FUNC(Py_mp_length, VoidPtr_length),
    PySlot_FUNC(Py_mp_subscript, VoidPtr_subscript),
    PySlot_FUNC(Py_nb_bool, VoidPtr_bool),
    PySlot_FUNC(Py_nb_int, VoidPtr_int),
    PySlot_FUNC(Py_sq_item, VoidPtr_item),
    PySlot_FUNC(Py_sq_length, VoidPtr_length),
    PySlot_FUNC(Py_tp_dealloc, VoidPtr_dealloc),
    PySlot_FUNC(Py_tp_new, VoidPtr_new),
    PySlot_FUNC(Py_tp_traverse, VoidPtr_traverse),
    PySlot_END
};


/* The structure used to hold the results of a voidptr conversion. */
struct vp_values {
    void *voidptr;
    Py_ssize_t size;
    int rw;
};


/* Forward declarations. */
static int check_size(PyObject *self);
static int check_rw(PyObject *self);
static int check_index(PyObject *self, Py_ssize_t idx);
static void bad_key(PyObject *key);
static int check_slice_size(Py_ssize_t size, Py_ssize_t value_size);
static PyObject *create_voidptr(sipSipModuleState *sms, void *voidptr,
        Py_ssize_t size, int rw);
static int vp_convertor(PyObject *arg, struct vp_values *vp);
static Py_ssize_t get_size_from_arg(VoidPtr *v, Py_ssize_t size);


/*
 * Implement ascapsule() for the type.
 */
static PyObject *VoidPtr_ascapsule(VoidPtr *v, PyObject *arg)
{
    (void)arg;

    return PyCapsule_New(v->voidptr, NULL, NULL);
}


/*
 * Implement asarray() for the type.
 */
static PyObject *VoidPtr_asarray(VoidPtr *v, PyObject *args, PyObject *kw)
{
    static char *const kwlist[] = {"size", NULL};

    Py_ssize_t size = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "|n:asarray", kwlist, &size))
        return NULL;

    if ((size = get_size_from_arg(v, size)) < 0)
        return NULL;

    return sip_array_from_bytes(
            sip_get_sip_module_state_from_type(Py_TYPE((PyObject *)v)),
            v->voidptr, size, v->rw);
}


/*
 * Implement asstring() for the type.
 */
static PyObject *VoidPtr_asstring(VoidPtr *v, PyObject *args, PyObject *kw)
{
    static char *const kwlist[] = {"size", NULL};

    Py_ssize_t size = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "|n:asstring", kwlist, &size))
        return NULL;

    if ((size = get_size_from_arg(v, size)) < 0)
        return NULL;

    return PyBytes_FromStringAndSize(v->voidptr, size);
}


/*
 * Implement getsize() for the type.
 */
static PyObject *VoidPtr_getsize(VoidPtr *v, PyObject *arg)
{
    (void)arg;

    return PyLong_FromSsize_t(v->size);
}


/*
 * Implement setsize() for the type.
 */
static PyObject *VoidPtr_setsize(VoidPtr *v, PyObject *arg)
{
    Py_ssize_t size = PyLong_AsSsize_t(arg);

    if (PyErr_Occurred())
        return NULL;

    v->size = size;

    Py_RETURN_NONE;
}


/*
 * Implement getwriteable() for the type.
 */
static PyObject *VoidPtr_getwriteable(VoidPtr *v, PyObject *arg)
{
    (void)arg;

    return PyBool_FromLong(v->rw);
}


/*
 * Implement setwriteable() for the type.
 */
static PyObject *VoidPtr_setwriteable(VoidPtr *v, PyObject *arg)
{
    int rw;

    if ((rw = PyObject_IsTrue(arg)) < 0)
        return NULL;

    v->rw = rw;

    Py_RETURN_NONE;
}


/*
 * Implement bool() for the type.
 */
static int VoidPtr_bool(PyObject *self)
{
    return (((VoidPtr *)self)->voidptr != NULL);
}


/*
 * Implement int() for the type.
 */
static PyObject *VoidPtr_int(PyObject *self)
{
    return PyLong_FromVoidPtr(((VoidPtr *)self)->voidptr);
}


/*
 * Implement len() for the type.
 */
static Py_ssize_t VoidPtr_length(PyObject *self)
{
    if (check_size(self) < 0)
        return -1;

    return ((VoidPtr *)self)->size;
}


/*
 * Implement sequence item sub-script for the type.
 */
static PyObject *VoidPtr_item(PyObject *self, Py_ssize_t idx)
{
    if (check_size(self) < 0 || check_index(self, idx) < 0)
        return NULL;

    return PyBytes_FromStringAndSize(
            (char *)((VoidPtr *)self)->voidptr + idx, 1);
}


/*
 * Implement mapping sub-script for the type.
 */
static PyObject *VoidPtr_subscript(PyObject *self, PyObject *key)
{
    VoidPtr *v;

    if (check_size(self) < 0)
        return NULL;

    v = (VoidPtr *)self;

    if (PyIndex_Check(key))
    {
        Py_ssize_t idx = PyNumber_AsSsize_t(key, PyExc_IndexError);

        if (idx == -1 && PyErr_Occurred())
            return NULL;

        if (idx < 0)
            idx += v->size;

        return VoidPtr_item(self, idx);
    }

    if (PySlice_Check(key))
    {
        Py_ssize_t start, stop, step, slicelength;

        if (sip_api_convert_from_slice_object(key, v->size, &start, &stop, &step, &slicelength) < 0)
            return NULL;

        if (step != 1)
        {
            PyErr_SetNone(PyExc_NotImplementedError);
            return NULL;
        }

        return create_voidptr(
                sip_get_sip_module_state_from_type(Py_TYPE(self)),
                (char *)v->voidptr + start, slicelength, v->rw);
    }

    bad_key(key);

    return NULL;
}


/*
 * Implement mapping assignment sub-script for the type.
 */
static int VoidPtr_ass_subscript(PyObject *self, PyObject *key,
        PyObject *value)
{
    VoidPtr *v;
    Py_ssize_t start, size;
    Py_buffer value_view;

    if (check_rw(self) < 0 || check_size(self) < 0)
        return -1;

    v = (VoidPtr *)self;

    if (PyIndex_Check(key))
    {
        start = PyNumber_AsSsize_t(key, PyExc_IndexError);

        if (start == -1 && PyErr_Occurred())
            return -1;

        if (start < 0)
            start += v->size;

        if (check_index(self, start) < 0)
            return -1;

        size = 1;
    }
    else if (PySlice_Check(key))
    {
        Py_ssize_t stop, step;

        if (sip_api_convert_from_slice_object(key, v->size, &start, &stop, &step, &size) < 0)
            return -1;

        if (step != 1)
        {
            PyErr_SetNone(PyExc_NotImplementedError);
            return -1;
        }
    }
    else
    {
        bad_key(key);

        return -1;
    }

    if (PyObject_GetBuffer(value, &value_view, PyBUF_CONTIG_RO) < 0)
        return -1;

    /* We could allow any item size... */
    if (value_view.itemsize != 1)
    {
        PyErr_Format(PyExc_TypeError, "'%s' must have an item size of 1",
                Py_TYPE(value_view.obj)->tp_name);

        PyBuffer_Release(&value_view);
        return -1;
    }

    if (check_slice_size(size, value_view.len) < 0)
    {
        PyBuffer_Release(&value_view);
        return -1;
    }

    memmove((char *)v->voidptr + start, value_view.buf, size);

    PyBuffer_Release(&value_view);

    return 0;
}


/*
 * The buffer implementation for Python v2.6.3 and later.
 */
static int VoidPtr_getbuffer(PyObject *self, Py_buffer *buf, int flags)
{
    VoidPtr *v;

    if (check_size(self) < 0)
        return -1;

    v = (VoidPtr *)self;

    return PyBuffer_FillInfo(buf, self, v->voidptr, v->size, !v->rw, flags);
}


/*
 * Implement __new__ for the type.
 */
static PyObject *VoidPtr_new(PyTypeObject *cls, PyObject *args, PyObject *kw)
{
    static char *const kwlist[] = {"address", "size", "writeable", NULL};

    struct vp_values vp_conversion;
    Py_ssize_t size = -1;
    int rw = -1;
    PyObject *obj;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "O&|ni:voidptr", kwlist, vp_convertor, &vp_conversion, &size, &rw))
        return NULL;

    /* Use the explicit size if one was given. */
    if (size >= 0)
        vp_conversion.size = size;

    /* Use the explicit writeable flag if one was given. */
    if (rw >= 0)
        vp_conversion.rw = rw;

    /* Create the instance. */
    if ((obj = cls->tp_alloc(cls, 0)) == NULL)
        return NULL;

    /* Save the values. */
    ((VoidPtr *)obj)->voidptr = vp_conversion.voidptr;
    ((VoidPtr *)obj)->size = vp_conversion.size;
    ((VoidPtr *)obj)->rw = vp_conversion.rw;

    return obj;
}


/*
 * The void pointer's traverse slot.
 */
static int VoidPtr_traverse(PyObject *self, visitproc visit, void *arg)
{
    Py_VISIT(Py_TYPE(self));

    return 0;
}


/*
 * The void pointer's dealloc slot.
 */
static void VoidPtr_dealloc(PyObject *self)
{
    PyObject_GC_UnTrack(self);
    PyTypeObject *type = Py_TYPE(self);
    type->tp_free(self);
    Py_DECREF(type);
}


/*
 * A convenience function to convert a C/C++ void pointer from a Python object.
 */
void *sip_api_convert_to_void_ptr(PyObject *obj)
{
    PyErr_Clear();

    if (obj == NULL)
    {
        PyErr_SetString(PyExc_TypeError,
                _SIP_MODULE_FQ_NAME ".voidptr is NULL");
        return NULL;
    }

    struct vp_values vp;

    if (vp_convertor(obj, &vp))
        return vp.voidptr;

    return PyLong_AsVoidPtr(obj);
}


/*
 * Convert a C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_void_ptr(sipModuleState *ms, void *val)
{
    return sip_convert_from_void_ptr(ms->sip_module_state, val);
}


/*
 * Implement the conversion of a C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_convert_from_void_ptr(sipSipModuleState *sms, void *val)
{
    return create_voidptr(sms, val, -1, TRUE);
}


/*
 * Convert a C/C++ const void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_const_void_ptr(sipModuleState *ms,
        const void *val)
{
    return sip_convert_from_const_void_ptr(ms->sip_module_state, val);
}


/*
 * Implement the conversion of a C/C++ const void pointer to a sip.voidptr
 * object.
 */
PyObject *sip_convert_from_const_void_ptr(sipSipModuleState *sms,
        const void *val)
{
    return create_voidptr(sms, (void *)val, -1, FALSE);
}


/*
 * Convert a sized C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_void_ptr_and_size(sipModuleState *ms, void *val,
        Py_ssize_t size)
{
    return create_voidptr(ms->sip_module_state, val, size, TRUE);
}


/*
 * Convert a sized C/C++ const void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_const_void_ptr_and_size(sipModuleState *ms,
        const void *val, Py_ssize_t size)
{
    return create_voidptr(ms->sip_module_state, (void *)val, size, FALSE);
}


/*
 * Initialise the void pointer type.
 */
int sip_void_ptr_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, VoidPtr_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_END
    };

    sms->void_ptr_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->void_ptr_type == NULL)
        return -1;

    if (PyModule_AddType(module, sms->void_ptr_type) < 0)
        return -1;

    return 0;
}


/*
 * Check that a void pointer has an explicit size and raise an exception if it
 * hasn't.
 */
static int check_size(PyObject *self)
{
    if (((VoidPtr *)self)->size >= 0)
        return 0;

    PyErr_SetString(PyExc_IndexError,
            _SIP_MODULE_FQ_NAME ".voidptr object has an unknown size");

    return -1;
}


/*
 * Check that a void pointer is writable.
 */
static int check_rw(PyObject *self)
{
    if (((VoidPtr *)self)->rw)
        return 0;

    PyErr_SetString(PyExc_TypeError,
            "cannot modify a read-only " _SIP_MODULE_FQ_NAME ".voidptr object");

    return -1;
}


/*
 * Check that an index is valid for a void pointer.
 */
static int check_index(PyObject *self, Py_ssize_t idx)
{
    if (idx >= 0 && idx < ((VoidPtr *)self)->size)
        return 0;

    PyErr_SetString(PyExc_IndexError, "index out of bounds");

    return -1;
}


/*
 * Raise an exception about a bad sub-script key.
 */
static void bad_key(PyObject *key)
{
    PyErr_Format(PyExc_TypeError,
            "cannot index a " _SIP_MODULE_FQ_NAME ".voidptr object using '%s'",
            Py_TYPE(key)->tp_name);
}


/*
 * Check that the size of a value is the same as the size of the slice it is
 * replacing.
 */
static int check_slice_size(Py_ssize_t size, Py_ssize_t value_size)
{
    if (value_size == size)
        return 0;

    PyErr_SetString(PyExc_ValueError,
            "cannot modify the size of a " _SIP_MODULE_FQ_NAME ".voidptr object");

    return -1;
}


/*
 * Do the work of converting a void pointer.
 */
static PyObject *create_voidptr(sipSipModuleState *sms, void *voidptr,
        Py_ssize_t size, int rw)
{
    PyErr_Clear();

    if (voidptr == NULL)
        Py_RETURN_NONE;

    VoidPtr *void_ptr = (VoidPtr *)PyType_GenericAlloc(sms->void_ptr_type, 0);

    if (void_ptr == NULL)
        return NULL;

    void_ptr->voidptr = voidptr;
    void_ptr->size = size;
    void_ptr->rw = rw;

    return (PyObject *)void_ptr;
}


/*
 * Convert a Python object to the values needed to create a voidptr.
 */
static int vp_convertor(PyObject *arg, struct vp_values *vp)
{
    sipSipModuleState *sms = sip_get_sip_module_state_from_type(Py_TYPE(arg));
    if (sms == NULL)
        PyErr_Clear();

    void *ptr;
    Py_ssize_t size = -1;
    int rw = TRUE;

    if (arg == Py_None)
    {
        ptr = NULL;
    }
    else if (PyCapsule_CheckExact(arg))
    {
        ptr = PyCapsule_GetPointer(arg, NULL);
    }
    else if (sms != NULL && PyObject_TypeCheck(arg, sms->void_ptr_type))
    {
        ptr = ((VoidPtr *)arg)->voidptr;
        size = ((VoidPtr *)arg)->size;
        rw = ((VoidPtr *)arg)->rw;
    }
    else if (PyObject_CheckBuffer(arg))
    {
        Py_buffer view;

        if (PyObject_GetBuffer(arg, &view, PyBUF_SIMPLE) < 0)
            return 0;

        ptr = view.buf;
        size = view.len;
        rw = !view.readonly;

        PyBuffer_Release(&view);
    }
    else
    {
        PyErr_Clear();
        ptr = PyLong_AsVoidPtr(arg);

        if (PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "a single integer, Capsule, None, bytes-like object or another " _SIP_MODULE_FQ_NAME ".voidptr object is required");
            return 0;
        }
    }

    vp->voidptr = ptr;
    vp->size = size;
    vp->rw = rw;

    return 1;
}


/*
 * Get a size possibly supplied as an argument, otherwise get it from the
 * object.  Raise an exception if there was no size specified.
 */
static Py_ssize_t get_size_from_arg(VoidPtr *v, Py_ssize_t size)
{
    /* Use the current size if one wasn't explicitly given. */
    if (size < 0)
        size = v->size;

    if (size < 0)
        PyErr_SetString(PyExc_ValueError,
                "a size must be given or the " _SIP_MODULE_FQ_NAME ".voidptr object must have a size");

    return size;
}
