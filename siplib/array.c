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
    void *cpp;
    const sipTypeDef *td;
    const char *format;
    size_t stride;
    SIP_SSIZE_T len;
    int readonly;
} sipArrayObject;


static int check_writable(sipArrayObject *array);
static int check_index(sipArrayObject *array, SIP_SSIZE_T idx);
static void *get_value(sipArrayObject *array, PyObject *value);
static void *get_slice(sipArrayObject *array, PyObject *value,
        SIP_SSIZE_T len);
#if PY_VERSION_HEX < 0x02050000
static void fix_bounds(int len, int *left, int *right);
#endif
#if PY_VERSION_HEX >= 0x02050000
static void bad_key(PyObject *key);
#endif
static void *element(sipArrayObject *array, SIP_SSIZE_T idx);
static PyObject *make_array(void *cpp, const sipTypeDef *td,
        const char *format, size_t stride, SIP_SSIZE_T len, int readonly);


/*
 * Implement len() for the type.
 */
static SIP_SSIZE_T sipArray_length(PyObject *self)
{
    return ((sipArrayObject *)self)->len;
}


/*
 * Implement sequence item sub-script for the type.
 */
static PyObject *sipArray_item(PyObject *self, SIP_SSIZE_T idx)
{
    sipArrayObject *array = (sipArrayObject *)self;
    PyObject *py_item;
    void *data;

    if (check_index(array, idx) < 0)
        return NULL;

    data = element(array, idx);

    if (array->td != NULL)
    {
        py_item = sip_api_convert_from_type(data, array->td, NULL);
    }
    else
    {
        switch (*array->format)
        {
        case 'H':
            py_item = PyLong_FromUnsignedLong(*(unsigned short *)data);
            break;

        case 'I':
            py_item = PyLong_FromUnsignedLong(*(unsigned int *)data);
            break;

        default:
            py_item = NULL;
        }
    }

    return py_item;
}


#if PY_VERSION_HEX < 0x02050000
/*
 * Implement sequence slice sub-script for the type.
 */
static PyObject *sipArray_slice(PyObject *self, int left, int right)
{
    sipArrayObject *array = (sipArrayObject *)self;

    fix_bounds(array->len, &left, &right);

    if (left == right)
        left = right = 0;

    return make_array(element(array, left), array->td, array->format,
            array->stride, right - left, array->readonly);
}


/*
 * Implement sequence assignment item sub-script for the type.
 */
static int sipArray_ass_item(PyObject *self, int idx, PyObject *value)
{
    sipArrayObject *array = (sipArrayObject *)self;
    void *value_cpp;

    if (check_writable(array) < 0 || check_index(array, idx) < 0)
        return -1;

    if ((value_cpp = get_value(array, value)) == NULL)
        return -1;

    memmove(element(array, idx), value_cpp, array->stride);

    return 0;
}


/*
 * Implement sequence assignment slice sub-script for the type.
 */
static int sipArray_ass_slice(PyObject *self, int left, int right,
        PyObject *value)
{
    sipArrayObject *array = (sipArrayObject *)self;
    void *value_cpp;

    if (check_writable(array) < 0)
        return -1;

    fix_bounds(array->len, &left, &right);

    if ((value_cpp = get_slice(array, value, right - left)) == NULL)
        return -1;

    memmove(element(array, left), value_cpp, (right - left) * array->stride);

    return 0;
}
#endif


/* The sequence methods data structure. */
static PySequenceMethods sipArray_SequenceMethods = {
    sipArray_length,        /* sq_length */
    0,                      /* sq_concat */
    0,                      /* sq_repeat */
    sipArray_item,          /* sq_item */
#if PY_VERSION_HEX < 0x02050000
    sipArray_slice,         /* sq_slice */
    sipArray_ass_item,      /* sq_ass_item */
    sipArray_ass_slice,     /* sq_ass_slice */
#endif
};


#if PY_VERSION_HEX >= 0x02050000
/*
 * Implement mapping sub-script for the type.
 */
static PyObject *sipArray_subscript(PyObject *self, PyObject *key)
{
    sipArrayObject *array = (sipArrayObject *)self;

    if (PyIndex_Check(key))
    {
        Py_ssize_t idx = PyNumber_AsSsize_t(key, PyExc_IndexError);

        if (idx == -1 && PyErr_Occurred())
            return NULL;

        if (idx < 0)
            idx += array->len;

        return sipArray_item(self, idx);
    }

    if (PySlice_Check(key))
    {
        Py_ssize_t start, stop, step, slicelength;

        if (sipConvertFromSliceObject(key, array->len, &start, &stop, &step, &slicelength) < 0)
            return NULL;

        if (step != 1)
        {
            PyErr_SetNone(PyExc_NotImplementedError);
            return NULL;
        }

        return make_array(element(array->cpp, start), array->td, array->format,
                array->stride, slicelength, array->readonly);
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
    sipArrayObject *array = (sipArrayObject *)self;
    SIP_SSIZE_T start, len;
    void *value_cpp;

    if (check_writable(array) < 0)
        return -1;

    if (PyIndex_Check(key))
    {
        start = PyNumber_AsSsize_t(key, PyExc_IndexError);

        if (start == -1 && PyErr_Occurred())
            return -1;

        if (start < 0)
            start += array->len;

        if (check_index(array, start) < 0)
            return -1;

        if ((value_cpp = get_value(array, value)) == NULL)
            return -1;

        len = 1;
    }
    else if (PySlice_Check(key))
    {
        Py_ssize_t stop, step;

        if (sipConvertFromSliceObject(key, array->len, &start, &stop, &step, &len) < 0)
            return -1;

        if (step != 1)
        {
            PyErr_SetNone(PyExc_NotImplementedError);
            return -1;
        }

        if ((value_cpp = get_slice(array, value, len)) == NULL)
            return -1;
    }
    else
    {
        bad_key(key);

        return -1;
    }

    memmove(element(array, start), value_cpp, len * array->stride);

    return 0;
}


/* The mapping methods data structure. */
static PyMappingMethods sipArray_MappingMethods = {
    sipArray_length,        /* mp_length */
    sipArray_subscript,     /* mp_subscript */
    sipArray_ass_subscript, /* mp_ass_subscript */
};
#endif


#if PY_VERSION_HEX >= 0x02060300
/*
 * The buffer implementation for Python v2.6.3 and later.
 */
static int sipArray_getbuffer(PyObject *self, Py_buffer *view, int flags)
{
    sipArrayObject *array = (sipArrayObject *)self;

    if (view == NULL)
        return 0;

    if (((flags & PyBUF_WRITABLE) == PyBUF_WRITABLE) && array->readonly)
    {
        PyErr_SetString(PyExc_BufferError, "Object is not writable.");
        return -1;
    }

    view->obj = self;
    Py_INCREF(self);

    view->buf = array->cpp;
    view->len = array->len;
    view->readonly = array->readonly;
    view->itemsize = array->stride;

    view->format = NULL;
    if ((flags & PyBUF_FORMAT) == PyBUF_FORMAT)
        view->format = array->format;

    view->ndim = 1;

    view->shape = NULL;
    if ((flags & PyBUF_ND) == PyBUF_ND)
        view->shape = &view->len;

    view->strides = NULL;
    if ((flags & PyBUF_STRIDES) == PyBUF_STRIDES)
        view->strides = &view->itemsize;

    view->suboffsets = NULL;
    view->internal = NULL;

    return 0;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The read buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getreadbuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    sipArrayObject *array = (sipArrayObject *)self;

    if (seg != 0)
    {
        PyErr_SetString(PyExc_SystemError, "invalid buffer segment");
        return -1;
    }

    *ptr = array->cpp;

    return array->len;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The write buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getwritebuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    if (check_writable((sipArrayObject *)self) < 0)
        return -1;

    return sipArray_getreadbuffer(self, seg, ptr);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The segment count implementation for Python v2.
 */
static SIP_SSIZE_T sipArray_getsegcount(PyObject *self, SIP_SSIZE_T *lenp)
{
    SIP_SSIZE_T segs, len;

    len = ((sipArrayObject *)self)->len;
    segs = (len < 0 ? 0 : 1);

    if (lenp != NULL)
        *lenp = len;

    return segs;
}
#endif


/* The buffer methods data structure. */
static PyBufferProcs sipArray_BufferProcs = {
#if PY_MAJOR_VERSION >= 3
    sipArray_getbuffer,     /* bf_getbuffer */
    0                       /* bf_releasebuffer */
#else
    sipArray_getreadbuffer, /* bf_getreadbuffer */
    sipArray_getwritebuffer,    /* bf_getwritebuffer */
    sipArray_getsegcount,   /* bf_getsegcount */
#if PY_VERSION_HEX >= 0x02050000
    (charbufferproc)sipArray_getreadbuffer, /* bf_getcharbuffer */
#if PY_VERSION_HEX >= 0x02060300
    sipArray_getbuffer,     /* bf_getbuffer */
    0                       /* bf_releasebuffer */
#endif
#else
    (getcharbufferproc)sipArray_getreadbuffer   /* bf_getcharbuffer */
#endif
#endif
};


/* The type data structure. */
PyTypeObject sipArray_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "sip.array",            /* tp_name */
    sizeof (sipArrayObject),    /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_reserved (Python v3), tp_compare (Python v2) */
    0,                      /* tp_repr */
    0,                      /* tp_as_number */
    &sipArray_SequenceMethods,  /* tp_as_sequence */
#if PY_VERSION_HEX >= 0x02050000
    &sipArray_MappingMethods,   /* tp_as_mapping */
#else
    0,                      /* tp_as_mapping */
#endif
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    &sipArray_BufferProcs,  /* tp_as_buffer */
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
    0,                      /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    0,                      /* tp_init */
    0,                      /* tp_alloc */
    0,                      /* tp_new */
};


/*
 * Check that an array is writable.
 */
static int check_writable(sipArrayObject *array)
{
    if (array->readonly)
    {
        PyErr_SetString(PyExc_TypeError, "sip.array object is read-only");
        return -1;
    }

    return 0;
}


/*
 * Check that an index is valid for an array.
 */
static int check_index(sipArrayObject *array, SIP_SSIZE_T idx)
{
    if (idx >= 0 && idx < array->len)
        return 0;

    PyErr_SetString(PyExc_IndexError, "index out of bounds");

    return -1;
}


#if PY_VERSION_HEX < 0x02050000
/*
 * Fix the bounds of a slice in the same way that the Python buffer object
 * does.
 */
static void fix_bounds(int len, int *left, int *right)
{
    if (*left < 0)
        *left = 0;
    else if (*left > len)
        *left = len;

    if (*right < *left)
        *right = *left;
    else if (*right > len)
        *right = len;
}
#endif


#if PY_VERSION_HEX >= 0x02050000
/*
 * Raise an exception about a bad sub-script key.
 */
static void bad_key(PyObject *key)
{
    PyErr_Format(PyExc_TypeError,
            "cannot index a sip.array object using '%s'",
            Py_TYPE(key)->tp_name);
}
#endif


/*
 * Get the address of an element of an array.
 */
static void *element(sipArrayObject *array, SIP_SSIZE_T idx)
{
    return (unsigned char *)(array->cpp) + idx * array->stride;
}


/*
 * Get the address of a value that will be copied to an array.
 */
static void *get_value(sipArrayObject *array, PyObject *value)
{
    static union {
        unsigned short u_short;
        unsigned int u_int;
    } static_data;

    void *data;

    if (array->td != NULL)
    {
        int iserr = FALSE;

        data = sip_api_force_convert_to_type(value, array->td, NULL,
                SIP_NOT_NONE|SIP_NO_CONVERTORS, NULL, &iserr);
    }
    else
    {
        PyErr_Clear();

        switch (*array->format)
        {
        case 'H':
            static_data.u_short = sipLongAsUnsignedLong(value);
            data = &static_data.u_short;
            break;

        case 'I':
            static_data.u_int = sipLongAsUnsignedLong(value);
            data = &static_data.u_int;
            break;

        default:
            data = NULL;
        }

        if (PyErr_Occurred())
            data = NULL;
    }

    return data;
}


/*
 * Get the address of an value that will be copied to an array slice.
 */
static void *get_slice(sipArrayObject *array, PyObject *value, SIP_SSIZE_T len)
{
    sipArrayObject *other = (sipArrayObject *)value;

    if (!PyObject_IsInstance(value, (PyObject *)&sipArray_Type) || array->td != other->td || strcmp(array->format, other->format) != 0)
    {
        const char *type;

        if (array->td != NULL)
        {
            type = sipTypeName(array->td);
        }
        else
        {
            switch (*array->format)
            {
            case 'H':
                type = "unsigned short";
                break;

            case 'I':
                type = "unsigned int";
                break;

            default:
                type = "";
            }
        }

        PyErr_Format(PyExc_TypeError,
                "can only assign another array of %s to the slice", type);

        return NULL;
    }

    if (other->len != len)
    {
        PyErr_Format(PyExc_TypeError,
#if PY_VERSION_HEX >= 0x02050000
                "the array being assigned must have length %zd",
#else
                "the array being assigned must have length %d",
#endif
                len);

        return NULL;
    }

    if (other->stride == array->stride)
    {
        PyErr_Format(PyExc_TypeError,
#if PY_VERSION_HEX >= 0x02050000
                "the array being assigned must have stride %zu",
                array->stride);
#else
                "the array being assigned must have stride %ld",
                (unsigned long)array->stride);
#endif

        return NULL;
    }

    return other->cpp;
}


/*
 * Do the work of creating an array.
 */
static PyObject *make_array(void *cpp, const sipTypeDef *td,
        const char *format, size_t stride, SIP_SSIZE_T len, int readonly)
{
    sipArrayObject *array;

    if ((array = PyObject_NEW(sipArrayObject, &sipArray_Type)) == NULL)
        return NULL;

    array->cpp = cpp;
    array->td = td;
    array->format = format;
    array->stride = stride;
    array->len = len;
    array->readonly = readonly;

    return (PyObject *)array;
}


/*
 * Wrap an array of instances of a fundamental type.  At the moment format must
 * be either "H" (unsigned short) or "I" (unsigned int).
 */
PyObject *sip_api_convert_to_array(void *cpp, const char *format,
        SIP_SSIZE_T len, int readonly)
{
    size_t stride;

    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    switch (*format)
    {
    case 'H':
        stride = sizeof (unsigned short);
        break;

    case 'I':
        stride = sizeof (unsigned int);
        break;

    default:
        stride = 0;
    }

    assert(stride > 0);
    assert(len >= 0);

    return make_array(cpp, NULL, format, stride, len, readonly);
}


/*
 * Wrap an array of instances of a defined type.
 */
PyObject *sip_api_convert_to_typed_array(void *cpp, const sipTypeDef *td,
        const char *format, size_t stride, SIP_SSIZE_T len, int readonly)
{
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    assert(stride > 0);
    assert(len >= 0);

    return make_array(cpp, td, format, stride, len, readonly);
}
