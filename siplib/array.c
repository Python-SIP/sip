/*
 * This file implements the API for the array type.
 *
 * Copyright (c) 2013 Riverbank Computing Limited <info@riverbankcomputing.com>
 *
 * This file is part of SIP.
 *
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 *
 * This copy of SIP may also used under the terms of the GNU General Public
 * License v2 or v3 as published by the Free Software Foundation which can be
 * found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
 *
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <Python.h>

#include <stddef.h>
#include <string.h>

#include "sip.h"
#include "sipint.h"

#include "array.h"


/* The object data structure. */
typedef struct {
    PyObject_HEAD
    void *voidptr;
    SIP_SSIZE_T size;
    int rw;
} sipArrayObject;


/* The structure used to hold the results of a voidptr conversion. */
struct vp_values {
    void *voidptr;
    SIP_SSIZE_T size;
    int rw;
};


static int check_size(PyObject *self);
static int check_rw(PyObject *self);
static int check_index(PyObject *self, SIP_SSIZE_T idx);
#if PY_VERSION_HEX < 0x02060300
static SIP_SSIZE_T get_value_data(PyObject *value, void **value_ptr);
#endif
#if PY_VERSION_HEX < 0x02050000
static void fix_bounds(int size, int *left, int *right);
#endif
#if PY_VERSION_HEX >= 0x02050000
static void bad_key(PyObject *key);
#endif
static int check_slice_size(SIP_SSIZE_T size, SIP_SSIZE_T value_size);
static PyObject *make_voidptr(void *voidptr, SIP_SSIZE_T size, int rw);
static int vp_convertor(PyObject *arg, struct vp_values *vp);


#if defined(SIP_USE_PYCAPSULE)
/*
 * Implement ascapsule() for the type.
 */
static PyObject *sipArray_ascapsule(sipArrayObject *v, PyObject *arg)
{
    return PyCapsule_New(v->voidptr, NULL, NULL);
}
#endif


#if defined(SIP_SUPPORT_PYCOBJECT)
/*
 * Implement ascobject() for the type.
 */
static PyObject *sipArray_ascobject(sipArrayObject *v, PyObject *arg)
{
    return PyCObject_FromArray(v->voidptr, NULL);
}
#endif


/*
 * Implement asstring() for the type.
 */
static PyObject *sipArray_asstring(sipArrayObject *v, PyObject *args,
        PyObject *kw)
{
    static char *kwlist[] = {"size", NULL};

    SIP_SSIZE_T size = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kw,
#if PY_VERSION_HEX >= 0x02050000
            "|n:asstring",
#else
            "|i:asstring",
#endif
            kwlist, &size))
        return NULL;

    /* Use the current size if one wasn't explicitly given. */
    if (size < 0)
        size = v->size;

    if (size < 0)
    {
        PyErr_SetString(PyExc_ValueError,
                "a size must be given or the sip.voidptr object must have a size");
        return NULL;
    }

    return SIPBytes_FromStringAndSize(v->voidptr, size);
}


/*
 * Implement getsize() for the type.
 */
static PyObject *sipArray_getsize(sipArrayObject *v, PyObject *arg)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_FromSsize_t(v->size);
#elif PY_VERSION_HEX >= 0x02050000
    return PyInt_FromSsize_t(v->size);
#else
    return PyInt_FromLong(v->size);
#endif
}


/*
 * Implement setsize() for the type.
 */
static PyObject *sipArray_setsize(sipArrayObject *v, PyObject *arg)
{
    SIP_SSIZE_T size;

#if PY_MAJOR_VERSION >= 3
    size = PyLong_AsSsize_t(arg);
#elif PY_VERSION_HEX >= 0x02050000
    size = PyInt_AsSsize_t(arg);
#else
    size = (int)PyInt_AsLong(arg);
#endif

    if (PyErr_Occurred())
        return NULL;

    v->size = size;

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Implement getwriteable() for the type.
 */
static PyObject *sipArray_getwriteable(sipArrayObject *v, PyObject *arg)
{
    return PyBool_FromLong(v->rw);
}


/*
 * Implement setwriteable() for the type.
 */
static PyObject *sipArray_setwriteable(sipArrayObject *v, PyObject *arg)
{
    int rw;

    rw = (int)SIPLong_AsLong(arg);

    if (PyErr_Occurred())
        return NULL;

    v->rw = rw;

    Py_INCREF(Py_None);
    return Py_None;
}


/* The methods data structure. */
static PyMethodDef sipArray_Methods[] = {
#if defined(SIP_USE_PYCAPSULE)
    {"ascapsule", (PyCFunction)sipArray_ascapsule, METH_NOARGS, NULL},
#endif
#if defined(SIP_SUPPORT_PYCOBJECT)
    {"ascobject", (PyCFunction)sipArray_ascobject, METH_NOARGS, NULL},
#endif
    {"asstring", (PyCFunction)sipArray_asstring, METH_KEYWORDS, NULL},
    {"getsize", (PyCFunction)sipArray_getsize, METH_NOARGS, NULL},
    {"setsize", (PyCFunction)sipArray_setsize, METH_O, NULL},
    {"getwriteable", (PyCFunction)sipArray_getwriteable, METH_NOARGS, NULL},
    {"setwriteable", (PyCFunction)sipArray_setwriteable, METH_O, NULL},
    {NULL}
};


/*
 * Implement int() for the type.
 */
static PyObject *sipArray_int(PyObject *self)
{
    return PyLong_FromVoidPtr(((sipArrayObject *)self)->voidptr);
}


#if PY_MAJOR_VERSION < 3
/*
 * Implement hex() for the type.
 */
static PyObject *sipArray_hex(PyObject *self)
{
    char buf[2 + 16 + 1];

    PyOS_snprintf(buf, sizeof (buf), "0x%.*lx", (int)(sizeof (void *) * 2),
            (unsigned long)((sipArrayObject *)self)->voidptr);

    return PyString_FromString(buf);
}
#endif


/* The number methods data structure. */
static PyNumberMethods sipArray_NumberMethods = {
    0,                      /* nb_add */
    0,                      /* nb_subtract */
    0,                      /* nb_multiply */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_divide */
#endif
    0,                      /* nb_remainder */
    0,                      /* nb_divmod */
    0,                      /* nb_power */
    0,                      /* nb_negative */
    0,                      /* nb_positive */
    0,                      /* nb_absolute */
    0,                      /* nb_bool (Python v3), nb_nonzero (Python v2) */
    0,                      /* nb_invert */
    0,                      /* nb_lshift */
    0,                      /* nb_rshift */
    0,                      /* nb_and */
    0,                      /* nb_xor */
    0,                      /* nb_or */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_coerce */
#endif
    sipArray_int,         /* nb_int */
    0,                      /* nb_reserved (Python v3), nb_long (Python v2) */
    0,                      /* nb_float */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_oct */
    sipArray_hex,         /* nb_hex */
#endif
    0,                      /* nb_inplace_add */
    0,                      /* nb_inplace_subtract */
    0,                      /* nb_inplace_multiply */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_inplace_divide */
#endif
    0,                      /* nb_inplace_remainder */
    0,                      /* nb_inplace_power */
    0,                      /* nb_inplace_lshift */
    0,                      /* nb_inplace_rshift */
    0,                      /* nb_inplace_and */
    0,                      /* nb_inplace_xor */
    0,                      /* nb_inplace_or */
    0,                      /* nb_floor_divide */
    0,                      /* nb_true_divide */
    0,                      /* nb_inplace_floor_divide */
    0,                      /* nb_inplace_true_divide */
#if PY_VERSION_HEX >= 0x02050000
    0                       /* nb_index */
#endif
};


/*
 * Implement len() for the type.
 */
static SIP_SSIZE_T sipArray_length(PyObject *self)
{
    if (check_size(self) < 0)
        return -1;

    return ((sipArrayObject *)self)->size;
}


/*
 * Implement sequence item sub-script for the type.
 */
static PyObject *sipArray_item(PyObject *self, SIP_SSIZE_T idx)
{
    if (check_size(self) < 0 || check_index(self, idx) < 0)
        return NULL;

    return SIPBytes_FromStringAndSize(
            (char *)((sipArrayObject *)self)->voidptr + idx, 1);
}


#if PY_VERSION_HEX < 0x02050000
/*
 * Implement sequence slice sub-script for the type.
 */
static PyObject *sipArray_slice(PyObject *self, int left, int right)
{
    sipArrayObject *v;

    if (check_size(self) < 0)
        return NULL;

    v = (sipArrayObject *)self;

    fix_bounds(v->size, &left, &right);

    if (left == right)
        left = right = 0;

    return make_voidptr((char *)(v->voidptr) + left, right - left, v->rw);
}


/*
 * Implement sequence assignment item sub-script for the type.
 */
static int sipArray_ass_item(PyObject *self, int idx, PyObject *value)
{
    int value_size;
    void *value_ptr;

    if (check_rw(self) < 0 || check_size(self) < 0 || check_index(self, idx) < 0)
        return -1;

    if ((value_size = get_value_data(value, &value_ptr)) < 0)
        return -1;

    if (value_size != 1)
    {
        PyErr_SetString(PyExc_TypeError,
                "right operand must be a single byte");

        return -1;
    }

    ((char *)((sipArrayObject *)self)->voidptr)[idx] = *(char *)value_ptr;

    return 0;
}


/*
 * Implement sequence assignment slice sub-script for the type.
 */
static int sipArray_ass_slice(PyObject *self, int left, int right,
        PyObject *value)
{
    sipArrayObject *v;
    int value_size;
    void *value_ptr;

    if (check_rw(self) < 0 || check_size(self) < 0)
        return -1;

    if ((value_size = get_value_data(value, &value_ptr)) < 0)
        return -1;

    v = (sipArrayObject *)self;

    fix_bounds(v->size, &left, &right);

    if (check_slice_size(right - left, value_size) < 0)
        return -1;

    memmove((char *)(v->voidptr) + left, value_ptr, right - left);

    return 0;
}
#endif


/* The sequence methods data structure. */
static PySequenceMethods sipArray_SequenceMethods = {
    sipArray_length,      /* sq_length */
    0,                      /* sq_concat */
    0,                      /* sq_repeat */
    sipArray_item,        /* sq_item */
#if PY_VERSION_HEX < 0x02050000
    sipArray_slice,       /* sq_slice */
    sipArray_ass_item,    /* sq_ass_item */
    sipArray_ass_slice,   /* sq_ass_slice */
#endif
};


#if PY_VERSION_HEX >= 0x02050000
/*
 * Implement mapping sub-script for the type.
 */
static PyObject *sipArray_subscript(PyObject *self, PyObject *key)
{
    sipArrayObject *v;

    if (check_size(self) < 0)
        return NULL;

    v = (sipArrayObject *)self;

    if (PyIndex_Check(key))
    {
        Py_ssize_t idx = PyNumber_AsSsize_t(key, PyExc_IndexError);

        if (idx == -1 && PyErr_Occurred())
            return NULL;

        if (idx < 0)
            idx += v->size;

        return sipArray_item(self, idx);
    }

    if (PySlice_Check(key))
    {
        Py_ssize_t start, stop, step, slicelength;

        if (sipConvertFromSliceObject(key, v->size, &start, &stop, &step, &slicelength) < 0)
            return NULL;

        if (step != 1)
        {
            PyErr_SetNone(PyExc_NotImplementedError);
            return NULL;
        }

        return make_voidptr((char *)v->voidptr + start, slicelength, v->rw);
    }

    bad_key(key);

    return NULL;
}


/*
 * Implement mapping assignment sub-script for the type.
 */
static int sipArray_ass_subscript(PyObject *self, PyObject *key,
        PyObject *value)
{
    sipArrayObject *v;
    Py_ssize_t start, size;
#if PY_VERSION_HEX >= 0x02060300
    Py_buffer value_view;
#else
    Py_ssize_t value_size;
    void *value_ptr;
#endif

    if (check_rw(self) < 0 || check_size(self) < 0)
        return -1;

    v = (sipArrayObject *)self;

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

        if (sipConvertFromSliceObject(key, v->size, &start, &stop, &step, &size) < 0)
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

#if PY_VERSION_HEX >= 0x02060300
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
#else
    if ((value_size = get_value_data(value, &value_ptr)) < 0)
        return -1;

    if (check_slice_size(size, value_size) < 0)
        return -1;

    memmove((char *)v->voidptr + start, value_ptr, size);
#endif

    return 0;
}


/* The mapping methods data structure. */
static PyMappingMethods sipArray_MappingMethods = {
    sipArray_length,      /* mp_length */
    sipArray_subscript,   /* mp_subscript */
    sipArray_ass_subscript,   /* mp_ass_subscript */
};
#endif


#if PY_VERSION_HEX >= 0x02060300
/*
 * The buffer implementation for Python v2.6.3 and later.
 */
static int sipArray_getbuffer(PyObject *self, Py_buffer *buf, int flags)
{
    sipArrayObject *v;

    if (check_size(self) < 0)
        return -1;

    v = (sipArrayObject *)self;

    return PyBuffer_FillInfo(buf, self, v->voidptr, v->size, !v->rw, flags);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The read buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getreadbuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    sipArrayObject *v;

    if (seg != 0)
    {
        PyErr_SetString(PyExc_SystemError, "invalid buffer segment");
        return -1;
    }

    if (check_size(self) < 0)
        return -1;

    v = (sipArrayObject *)self;

    *ptr = v->voidptr;

    return v->size;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The write buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getwritebuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    if (((sipArrayObject *)self)->rw)
        return sipArray_getreadbuffer(self, seg, ptr);

    PyErr_SetString(PyExc_TypeError, "sip.voidptr object is not writeable");
    return -1;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The segment count implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getsegcount(PyObject *self, SIP_SSIZE_T *lenp)
{
    SIP_SSIZE_T segs, len;

    len = ((sipArrayObject *)self)->size;
    segs = (len < 0 ? 0 : 1);

    if (lenp != NULL)
        *lenp = len;

    return segs;
}
#endif


/* The buffer methods data structure. */
static PyBufferProcs sipArray_BufferProcs = {
#if PY_MAJOR_VERSION >= 3
    sipArray_getbuffer,   /* bf_getbuffer */
    0                       /* bf_releasebuffer */
#else
    sipArray_getreadbuffer,   /* bf_getreadbuffer */
    sipArray_getwritebuffer,  /* bf_getwritebuffer */
    sipArray_getsegcount, /* bf_getsegcount */
#if PY_VERSION_HEX >= 0x02050000
    (charbufferproc)sipArray_getreadbuffer,   /* bf_getcharbuffer */
#if PY_VERSION_HEX >= 0x02060300
    sipArray_getbuffer,   /* bf_getbuffer */
    0                       /* bf_releasebuffer */
#endif
#else
    (getcharbufferproc)sipArray_getreadbuffer /* bf_getcharbuffer */
#endif
#endif
};


/*
 * Implement __new__ for the type.
 */
static PyObject *sipArray_new(PyTypeObject *subtype, PyObject *args,
        PyObject *kw)
{
    static char *kwlist[] = {"address", "size", "writeable", NULL};

    struct vp_values vp_conversion;
    SIP_SSIZE_T size = -1;
    int rw = -1;
    PyObject *obj;

    if (!PyArg_ParseTupleAndKeywords(args, kw,
#if PY_VERSION_HEX >= 0x02050000
            "O&|ni:voidptr",
#else
            "O&|ii:voidptr",
#endif
            kwlist, vp_convertor, &vp_conversion, &size, &rw))
        return NULL;

    /* Use the explicit size if one was given. */
    if (size >= 0)
        vp_conversion.size = size;

    /* Use the explicit writeable flag if one was given. */
    if (rw >= 0)
        vp_conversion.rw = rw;

    /* Create the instance. */
    if ((obj = subtype->tp_alloc(subtype, 0)) == NULL)
        return NULL;

    /* Save the values. */
    ((sipArrayObject *)obj)->voidptr = vp_conversion.voidptr;
    ((sipArrayObject *)obj)->size = vp_conversion.size;
    ((sipArrayObject *)obj)->rw = vp_conversion.rw;

    return obj;
}


/* The type data structure. */
PyTypeObject sipArray_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "sip.voidptr",          /* tp_name */
    sizeof (sipArrayObject),  /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_reserved (Python v3), tp_compare (Python v2) */
    0,                      /* tp_repr */
    &sipArray_NumberMethods,  /* tp_as_number */
    &sipArray_SequenceMethods,    /* tp_as_sequence */
#if PY_VERSION_HEX >= 0x02050000
    &sipArray_MappingMethods, /* tp_as_mapping */
#else
    0,                      /* tp_as_mapping */
#endif
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    &sipArray_BufferProcs,    /* tp_as_buffer */
#if defined(Py_TPFLAGS_HAVE_NEWBUFFER)
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_NEWBUFFER,   /* tp_flags */
#else
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
#endif
    0,                      /* tp_doc */
    0,                      /* tp_traverse */
    0,                      /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    sipArray_Methods,     /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    0,                      /* tp_init */
    0,                      /* tp_alloc */
    sipArray_new,         /* tp_new */
};


/*
 * A convenience function to convert a C/C++ void pointer from a Python object.
 */
void *sip_api_convert_array(PyObject *obj)
{
    struct vp_values vp;

    if (obj == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "sip.voidptr is NULL");
        return NULL;
    }

    if (vp_convertor(obj, &vp))
        return vp.voidptr;

    return PyLong_AsVoidPtr(obj);
}


/*
 * Convert a C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_array(void *val)
{
    return make_voidptr(val, -1, TRUE);
}


/*
 * Convert a C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_const_array(const void *val)
{
    return make_voidptr((void *)val, -1, FALSE);
}


/*
 * Convert a sized C/C++ void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_array_and_size(void *val, SIP_SSIZE_T size)
{
    return make_voidptr(val, size, TRUE);
}


/*
 * Convert a sized C/C++ const void pointer to a sip.voidptr object.
 */
PyObject *sip_api_convert_from_const_array_and_size(const void *val,
        SIP_SSIZE_T size)
{
    return make_voidptr((void *)val, size, FALSE);
}


/*
 * Check that a void pointer has an explicit size and raise an exception if it
 * hasn't.
 */
static int check_size(PyObject *self)
{
    if (((sipArrayObject *)self)->size >= 0)
        return 0;

    PyErr_SetString(PyExc_IndexError,
            "sip.voidptr object has an unknown size");

    return -1;
}


/*
 * Check that a void pointer is writable.
 */
static int check_rw(PyObject *self)
{
    if (((sipArrayObject *)self)->rw)
        return 0;

    PyErr_SetString(PyExc_TypeError,
            "cannot modify a read-only sip.voidptr object");

    return -1;
}


/*
 * Check that an index is valid for a void pointer.
 */
static int check_index(PyObject *self, SIP_SSIZE_T idx)
{
    if (idx >= 0 && idx < ((sipArrayObject *)self)->size)
        return 0;

    PyErr_SetString(PyExc_IndexError, "index out of bounds");

    return -1;
}


#if PY_VERSION_HEX < 0x02060300
/*
 * Get the address and size of the data from a value that supports the buffer
 * interface.
 */
static SIP_SSIZE_T get_value_data(PyObject *value, void **value_ptr)
{
    PyBufferProcs *bf = Py_TYPE(value)->tp_as_buffer;

    if (bf == NULL || bf->bf_getreadbuffer == NULL || bf->bf_getsegcount == NULL)
    {
        PyErr_Format(PyExc_TypeError,
                "'%s' does not support the buffer interface",
                Py_TYPE(value)->tp_name);

        return -1;
    }

    if ((*bf->bf_getsegcount)(value, NULL) != 1)
    {
        PyErr_SetString(PyExc_TypeError,
                "single-segment buffer object expected");

        return -1;
    }

    return (*bf->bf_getreadbuffer)(value, 0, value_ptr);
}
#endif


#if PY_VERSION_HEX < 0x02050000
/*
 * Fix the bounds of a slice in the same way that the Python buffer object
 * does.
 */
static void fix_bounds(int size, int *left, int *right)
{
    if (*left < 0)
        *left = 0;
    else if (*left > size)
        *left = size;

    if (*right < *left)
        *right = *left;
    else if (*right > size)
        *right = size;
}
#endif


#if PY_VERSION_HEX >= 0x02050000
/*
 * Raise an exception about a bad sub-script key.
 */
static void bad_key(PyObject *key)
{
    PyErr_Format(PyExc_TypeError,
            "cannot index a sip.voidptr object using '%s'",
            Py_TYPE(key)->tp_name);
}
#endif


/*
 * Check that the size of a value is the same as the size of the slice it is
 * replacing.
 */
static int check_slice_size(SIP_SSIZE_T size, SIP_SSIZE_T value_size)
{
    if (value_size == size)
        return 0;

    PyErr_SetString(PyExc_ValueError,
            "cannot modify the size of a sip.voidptr object");

    return -1;
}


/*
 * Do the work of converting a void pointer.
 */
static PyObject *make_voidptr(void *voidptr, SIP_SSIZE_T size, int rw)
{
    sipArrayObject *self;

    if (voidptr == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if ((self = PyObject_NEW(sipArrayObject, &sipArray_Type)) == NULL)
        return NULL;

    self->voidptr = voidptr;
    self->size = size;
    self->rw = rw;

    return (PyObject *)self;
}


/*
 * Convert a Python object to the values needed to create a voidptr.
 */
static int vp_convertor(PyObject *arg, struct vp_values *vp)
{
    void *ptr;
    SIP_SSIZE_T size = -1;
    int rw = TRUE;

    if (arg == Py_None)
        ptr = NULL;
#if defined(SIP_USE_PYCAPSULE)
    else if (PyCapsule_CheckExact(arg))
        ptr = PyCapsule_GetPointer(arg, NULL);
#endif
#if defined(SIP_SUPPORT_PYCOBJECT)
    else if (PyCObject_Check(arg))
        ptr = PyCObject_AsVoidPtr(arg);
#endif
    else if (PyObject_TypeCheck(arg, &sipArray_Type))
    {
        ptr = ((sipArrayObject *)arg)->voidptr;
        size = ((sipArrayObject *)arg)->size;
        rw = ((sipArrayObject *)arg)->rw;
    }
#if PY_VERSION_HEX >= 0x02060300
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
#endif
#if PY_VERSION_HEX < 0x03000000
    else if (PyObject_AsReadBuffer(arg, &ptr, &size) >= 0)
    {
        rw = (Py_TYPE(arg)->tp_as_buffer->bf_getwritebuffer != NULL);
    }
#endif
    else
    {
        PyErr_Clear();
        ptr = PyLong_AsVoidPtr(arg);

        if (PyErr_Occurred())
        {
#if PY_VERSION_HEX >= 0x03010000
            PyErr_SetString(PyExc_TypeError, "a single integer, CObject, None, buffer protocol implementor or another sip.voidptr object is required");
#else
            PyErr_SetString(PyExc_TypeError, "a single integer, Capsule, CObject, None, buffer protocol implementor or another sip.voidptr object is required");
#endif
            return 0;
        }
    }

    vp->voidptr = ptr;
    vp->size = size;
    vp->rw = rw;

    return 1;
}


/*
 * Wrap an array of instances of a defined type.
 */
PyObject *sip_api_convert_to_typed_array(void *cpp, const sipTypeDef *td,
        size_t stride, SIP_SSIZE_T len, int rw)
{
    PyErr_SetString(PyExc_TypeError, "arrays not yet implemented");
    return NULL;
}
