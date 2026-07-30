/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file implements the API for string convertors.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_string_convertors.h"

#include "sip_core.h"


/* Forward references. */
static char parse_string_as_encoded_char(PyObject *bytes, PyObject *obj);
static const char *parse_string_as_encoded_string(PyObject *bytes,
        PyObject **obj_p);
static void wchar_array_dtor(PyObject *mem);


/*
 * Convert a Python bytes object to a C char and raise an exception if there
 * was an error.
 */
char sip_bytes_as_char(PyObject *obj)
{
    Py_ssize_t asize;
    const char *cp = sip_bytes_as_char_array(obj, &asize);

    if (PyErr_Occurred())
        return '\0';

    if (asize != 1)
        PyErr_SetString(PyExc_TypeError,
                "a bytes-like object of length 1 expected");

    return *cp;
}


/*
 * Convert an optional Python bytes-like object as a C char array returning its
 * address and length.  An exception is raised if there was an error.
 */
const char *sip_bytes_as_char_array(PyObject *obj, Py_ssize_t *asize_p)
{
    PyErr_Clear();

    const char *cp;

    if (obj == Py_None)
    {
        cp = NULL;
        *asize_p = 0;
    }
    else if (PyBytes_Check(obj))
    {
        cp = PyBytes_AS_STRING(obj);
        *asize_p = PyBytes_GET_SIZE(obj);
    }
    else
    {
        Py_buffer view;

        if (PyObject_GetBuffer(obj, &view, PyBUF_SIMPLE) < 0)
            return NULL;

        cp = view.buf;
        *asize_p = view.len;

        PyBuffer_Release(&view);
    }

    return cp;
}


/*
 * Convert an optional Python bytes-like object to a C string and raise an
 * exception if there was an error.
 */
const char *sip_bytes_as_string(PyObject *obj)
{
    Py_ssize_t asize;

    return sip_bytes_as_char_array(obj, &asize);
}


/*
 * Convert a Python bytes-like or string object to an ASCII encoded C char and
 * raise an exception if there was an error.
 */
char sip_string_as_ascii_char(PyObject *obj)
{
    char ch = parse_string_as_encoded_char(PyUnicode_AsASCIIString(obj), obj);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_LENGTH(obj) != 1)
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or ASCII string of length 1 expected");

        return '\0';
    }

    return ch;
}


/*
 * Convert an optional Python bytes-like or string object to an ASCII encoded C
 * string and raise an The object is updated with the one that owns the encoded
 * string.
 */
const char *sip_string_as_ascii_string(PyObject **obj_p)
{
    const char *cp = parse_string_as_encoded_string(
            PyUnicode_AsASCIIString(*obj_p), obj_p);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(*obj_p))
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or ASCII string expected");

        return NULL;
    }

    return cp;
}


/*
 * Convert a Python bytes-like or string object to a Latin-1 encoded C char and
 * raise an exception if there was an error.
 */
char sip_string_as_latin1_char(PyObject *obj)
{
    char ch = parse_string_as_encoded_char(PyUnicode_AsLatin1String(obj), obj);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_LENGTH(obj) != 1)
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or Latin-1 string of length 1 expected");

        return '\0';
    }

    return ch;
}


/*
 * Convert an optional Python bytes-like or string object to a Latin-1 encoded
 * C string and raise an exception if there was an error.  The object is
 * updated with the one that owns the encoded string.
 */
const char *sip_string_as_latin1_string(PyObject **obj_p)
{
    const char *cp = parse_string_as_encoded_string(
            PyUnicode_AsLatin1String(*obj_p), obj_p);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(*obj_p))
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or Latin-1 string expected");

        return NULL;
    }

    return cp;
}


/*
 * Convert a Python bytes-like or string object to a UTF-8 encoded C char and
 * raise an exception if there was an error.
 */
char sip_string_as_utf8_char(PyObject *obj)
{
    char ch = parse_string_as_encoded_char(PyUnicode_AsUTF8String(obj), obj);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_LENGTH(obj) != 1)
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or UTF-8 string of length 1 expected");

        return '\0';
    }

    return ch;
}


/*
 * Convert an optional Python bytes-like or string object to a UTF-8 encoded C
 * string and raise an exception if there was an error.  The object is updated
 * with the one that owns the encoded string.
 */
const char *sip_string_as_utf8_string(PyObject **obj_p)
{
    const char *cp = parse_string_as_encoded_string(
            PyUnicode_AsUTF8String(*obj_p), obj_p);

    if (PyErr_Occurred())
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(*obj_p))
            PyErr_SetString(PyExc_TypeError,
                    "a bytes-like object or UTF-8 string expected");

        return NULL;
    }

    return cp;
}


/*
 * Convert a Python string object to a wchar_t character.
 */
wchar_t sip_string_as_wchar(PyObject *obj)
{
    if (!PyUnicode_Check(obj) || PyUnicode_GET_LENGTH(obj) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "a string of length 1 expected");
        return L'\0';
    }

    wchar_t wch;

    if (PyUnicode_AsWideChar(obj, &wch, 1) != 1)
        return L'\0';

    /*
     * This might have been set coming in to this call but it didn't matter
     * until now.
     */
    PyErr_Clear();

    return wch;
}


/*
 * Convert an optional Python string object as a C wchar_t array returning its
 * address and length.  An exception is raised if there was an error.
 */
wchar_t *sip_string_as_wchar_array(PyObject **obj_p, Py_ssize_t *asize_p)
{
    wchar_t *wcp;
    Py_ssize_t asize;
    PyObject *obj = *obj_p;

    PyErr_Clear();

    if (obj == Py_None)
    {
        wcp = NULL;
        asize = 0;

        /* The extra reference as owner. */
        Py_INCREF(obj);
    }
    else if (PyUnicode_Check(obj))
    {
        /*
         * If the size isn't requested then this will check for embedded L'\0'
         * characters.
         */
        wcp = PyUnicode_AsWideCharString(obj, asize_p != NULL ? &asize : NULL);
        if (wcp == NULL)
            return NULL;

        /*
         * Wrap the array in a capsule so that it can be garbage collected when
         * no longer needed.
         */
        PyObject *mem = PyCapsule_New((void *)wcp, NULL, wchar_array_dtor);
        if (mem == NULL)
        {
            PyMem_Free(wcp);
            return NULL;
        }

        *obj_p = mem;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "a string expected");
        return NULL;
    }

    if (asize_p != NULL)
        *asize_p = asize;

    return wcp;
}


/*
 * Convert an optional Python string object to a wide character array and raise
 * an exception if there was an error.  The object is updated with the one that
 * owns the array.
 */
wchar_t *sip_string_as_wstring(PyObject **obj_p)
{
    return sip_string_as_wchar_array(obj_p, NULL);
}


/*
 * Parse an encoded character and return it.
 */
static char parse_string_as_encoded_char(PyObject *bytes, PyObject *obj)
{
    if (bytes == NULL)
    {
        /* Don't try anything else if there was an encoding error. */
        if (PyUnicode_Check(obj))
            return '\0';

        return sip_bytes_as_char(obj);
    }

    if (PyBytes_GET_SIZE(bytes) != 1)
    {
        PyErr_SetString(PyExc_TypeError,
                "a decoded value of length 1 expected");

        Py_DECREF(bytes);
        return '\0';
    }

    char ch = *PyBytes_AS_STRING(bytes);
    Py_DECREF(bytes);

    /*
     * This might have been set coming in to the outer call but it didn't
     * matter until now.
     */
    PyErr_Clear();

    return ch;
}


/*
 * Parse an optional encoded string and return it and a reference to the object
 * that owns the encoded string.
 */
static const char *parse_string_as_encoded_string(PyObject *bytes,
        PyObject **obj_p)
{
    if (bytes == NULL)
    {
        PyObject *obj = *obj_p;

        /* Don't try anything else if there was an encoding error. */
        if (PyUnicode_Check(obj))
            return NULL;

        /* This will take care of the None/NULL case. */
        const char *cp = sip_bytes_as_string(obj);

        if (PyErr_Occurred())
            return NULL;

        /* The extra reference as owner. */
        Py_INCREF(obj);

        return cp;
    }

    *obj_p = bytes;

    /*
     * This might have been set coming in to the outer call but it didn't
     * matter until now.
     */
    PyErr_Clear();

    return PyBytes_AS_STRING(bytes);
}


/*
 * Garbage collect the wchar_t array in a capsule.
 */
static void wchar_array_dtor(PyObject *mem)
{
    PyMem_Free(PyCapsule_GetPointer(mem, NULL));
}
