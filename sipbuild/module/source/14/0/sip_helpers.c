/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This implements the helpers for handwritten code that are not needed
 * otherwise.  Backwards compatibility is more important than modern design.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>
#include <datetime.h>

#include "sip_helpers.h"

#include "sip.h"
#include "sip_core.h"


/*
 * Report a sequence length that does not match the length of a slice.
 */
void sip_api_bad_length_for_slice(Py_ssize_t seqlen, Py_ssize_t slicelen)
{
    PyErr_Format(PyExc_ValueError,
            "attempt to assign sequence of size %zd to slice of size %zd",
            seqlen, slicelen);
}


/*
 * Convert a sequence index.  Return the index or a negative value if there was
 * an error.
 */
Py_ssize_t sip_api_convert_from_sequence_index(Py_ssize_t idx, Py_ssize_t len)
{
    /* Negative indices start from the other end. */
    if (idx < 0)
        idx = len + idx;

    if (idx < 0 || idx >= len)
    {
        PyErr_Format(PyExc_IndexError, "sequence index out of range");
        return -1;
    }

    return idx;
}


/*
 * Enable or disable the garbage collector.  Return the previous state or -1 if
 * there was an error.
 */
int sip_api_enable_gc(int enable)
{
    static PyObject *enable_func = NULL, *disable_func, *isenabled_func;
    PyObject *result;
    int was_enabled;

    /*
     * This may be -ve in the highly unusual event that a previous call failed.
     */
    if (enable < 0)
        return -1;

    /* Get the functions if we haven't already got them. */
    if (enable_func == NULL)
    {
        PyObject *gc_module;

        if ((gc_module = PyImport_ImportModule("gc")) == NULL)
            return -1;

        if ((enable_func = PyObject_GetAttrString(gc_module, "enable")) == NULL)
        {
            Py_DECREF(gc_module);
            return -1;
        }

        if ((disable_func = PyObject_GetAttrString(gc_module, "disable")) == NULL)
        {
            Py_DECREF(enable_func);
            Py_DECREF(gc_module);
            return -1;
        }

        if ((isenabled_func = PyObject_GetAttrString(gc_module, "isenabled")) == NULL)
        {
            Py_DECREF(disable_func);
            Py_DECREF(enable_func);
            Py_DECREF(gc_module);
            return -1;
        }

        Py_DECREF(gc_module);
    }

    /* Get the current state. */
    if ((result = PyObject_CallObject(isenabled_func, NULL)) == NULL)
        return -1;

    was_enabled = PyObject_IsTrue(result);
    Py_DECREF(result);

    if (was_enabled < 0)
        return -1;

    /* See if the state needs changing. */
    if (!was_enabled != !enable)
    {
        /* Enable or disable as required. */
        result = PyObject_CallObject((enable ? enable_func : disable_func),
                NULL);

        Py_XDECREF(result);

        if (result != Py_None)
            return -1;
    }

    return was_enabled;
}


/*
 * Create a date from its component parts.
 */
PyObject *sip_api_from_date(const sipDateDef *date)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return NULL;

    return PyDate_FromDate(date->pd_year, date->pd_month, date->pd_day);
}


/*
 * Create a datetime from its component parts.
 */
PyObject *sip_api_from_date_time(const sipDateDef *date,
        const sipTimeDef *time)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return NULL;

    return PyDateTime_FromDateAndTime(date->pd_year, date->pd_month,
            date->pd_day, time->pt_hour, time->pt_minute, time->pt_second,
            time->pt_microsecond);
}


/*
 * Create a method from its component parts.
 */
PyObject *sip_api_from_method(const sipMethodDef *method)
{
    return PyMethod_New(method->pm_function, method->pm_self);
}


/*
 * Create a time from its component parts.
 */
PyObject *sip_api_from_time(const sipTimeDef *time)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return NULL;

    return PyTime_FromTime(time->pt_hour, time->pt_minute, time->pt_second,
            time->pt_microsecond);
}


/*
 * Check an object is a C function and return TRUE and its component parts if
 * it is.
 */
int sip_api_get_c_function(PyObject *obj, sipCFunctionDef *c_function)
{
    if (!PyCFunction_Check(obj))
        return FALSE;

    if (c_function != NULL)
    {
        c_function->cf_function = ((PyCFunctionObject *)obj)->m_ml;
        c_function->cf_self = PyCFunction_GET_SELF(obj);
    }

    return TRUE;
}


/*
 * Check an object is a date and return 1 and its component parts if it is.  -1
 * is returned and an exception set if there was an error (probably attempting
 * to use this with sub-interpreters).
 */
int sip_api_get_date(PyObject *obj, sipDateDef *date)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return -1;

    if (!PyDate_Check(obj))
        return 0;

    if (date != NULL)
    {
        date->pd_year = PyDateTime_GET_YEAR(obj);
        date->pd_month = PyDateTime_GET_MONTH(obj);
        date->pd_day = PyDateTime_GET_DAY(obj);
    }

    return 1;
}


/*
 * Check an object is a datetime and return 1 and its component parts if it
 * is.  -1 is returned and an exception set if there was an error (probably
 * attempting to use this with sub-interpreters).
 */
int sip_api_get_date_time(PyObject *obj, sipDateDef *date, sipTimeDef *time)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return -1;

    if (!PyDateTime_Check(obj))
        return 0;

    if (date != NULL)
    {
        date->pd_year = PyDateTime_GET_YEAR(obj);
        date->pd_month = PyDateTime_GET_MONTH(obj);
        date->pd_day = PyDateTime_GET_DAY(obj);
    }

    if (time != NULL)
    {
        time->pt_hour = PyDateTime_DATE_GET_HOUR(obj);
        time->pt_minute = PyDateTime_DATE_GET_MINUTE(obj);
        time->pt_second = PyDateTime_DATE_GET_SECOND(obj);
        time->pt_microsecond = PyDateTime_DATE_GET_MICROSECOND(obj);
    }

    return 1;
}


/*
 * Return a strong reference to a frame from the execution stack.
 */
PyFrameObject *sip_api_get_frame_ref(int depth)
{
#if defined(PYPY_VERSION)
    /* PyPy only supports a depth of 0. */
    return NULL;
#else
    PyFrameObject *frame = (PyFrameObject *)Py_XNewRef(PyEval_GetFrame());

    while (frame != NULL && depth > 0)
    {
        PyFrameObject *back_frame = PyFrame_GetBack(frame);
        Py_DECREF(frame);
        frame = back_frame;

        --depth;
    }

    return frame;
#endif
}


/*
 * Check an object is a method and return TRUE and its component parts if it
 * is.
 */
int sip_api_get_method(PyObject *obj, sipMethodDef *method)
{
    if (!PyMethod_Check(obj))
        return FALSE;

    if (method != NULL)
    {
        method->pm_self = PyMethod_GET_SELF(obj);
        method->pm_function = PyMethod_GET_FUNCTION(obj);
    }

    return TRUE;
}


/*
 * Check an object is a time and return TRUE and its component parts if it is.
 * -1 is returned and an exception set if there was an error (probably
 * attempting to use this with sub-interpreters).
 */
int sip_api_get_time(PyObject *obj, sipTimeDef *time)
{
    PyDateTime_IMPORT;
    if (PyErr_Occurred())
        return -1;

    if (!PyTime_Check(obj))
        return 0;

    if (time != NULL)
    {
        time->pt_hour = PyDateTime_TIME_GET_HOUR(obj);
        time->pt_minute = PyDateTime_TIME_GET_MINUTE(obj);
        time->pt_second = PyDateTime_TIME_GET_SECOND(obj);
        time->pt_microsecond = PyDateTime_TIME_GET_MICROSECOND(obj);
    }

    return 1;
}


/*
 * A thin wrapper around PyObject_Dump() usually used when debugging with the
 * limited API.
 */
void sip_api_object_dump(PyObject *obj)
{
    PyObject_Dump(obj);
}


/*
 * A thin wrapper around PyType_GetDict() (on behalf of the limited API).
 */
PyObject *sip_api_py_type_dict_ref(PyTypeObject *py_type)
{
    return PyType_GetDict(py_type);
}


/*
 * Get the address of the contents of a Unicode object, the character size and
 * the length.
 */
void *sip_api_unicode_data(PyObject *obj, int *char_size, Py_ssize_t *len)
{
    void *data;

    /* Assume there will be an error. */
    *char_size = -1;

    if (PyUnicode_READY(obj) < 0)
        return NULL;

    *len = PyUnicode_GET_LENGTH(obj);

    switch (PyUnicode_KIND(obj))
    {
    case PyUnicode_1BYTE_KIND:
        *char_size = 1;
        data = PyUnicode_1BYTE_DATA(obj);
        break;

    case PyUnicode_2BYTE_KIND:
        *char_size = 2;
        data = PyUnicode_2BYTE_DATA(obj);
        break;

    case PyUnicode_4BYTE_KIND:
        *char_size = 4;
        data = PyUnicode_4BYTE_DATA(obj);
        break;

    default:
        data = NULL;
    }

    return data;
}


/*
 * Create a new Unicode object and return the character size and buffer.
 */
PyObject *sip_api_unicode_new(Py_ssize_t len, unsigned maxchar, int *kind,
        void **data)
{
    PyObject *obj;

    if ((obj = PyUnicode_New(len, maxchar)) != NULL)
    {
        *kind = PyUnicode_KIND(obj);
        *data = PyUnicode_DATA(obj);
    }

    return obj;
}


/*
 * Update a new Unicode object with a new character.
 */
void sip_api_unicode_write(int kind, void *data, int index, unsigned value)
{
    PyUnicode_WRITE(kind, data, index, value);
}
