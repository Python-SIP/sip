/*
 * The transitional Python bindings for the C parts of the sip code generator.
 *
 * Copyright (c) 2022 Riverbank Computing Limited <info@riverbankcomputing.com>
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


#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if !defined(Py_LIMITED_API)
#define Py_LIMITED_API      0x03070000
#endif

#include <Python.h>

#include "sip.h"


/* Globals - see sip.h for their meanings. */
unsigned sipVersion;
const char *sipVersionStr;
unsigned abiVersion;
char *sipName;

/* Support for fatal error handling. */
static char error_text[1000];
static PyObject *exception_type;
static void raise_exception(void);

/* Forward declarations. */
static PyObject *py_set_globals(PyObject *self, PyObject *args);
static PyObject *py_py2c(PyObject *self, PyObject *args);
static PyObject *py_generateCode(PyObject *self, PyObject *args);

static int fs_convertor(PyObject *obj, char **fsp);
static int sipSpec_convertor(PyObject *obj, sipSpec **ptp);
static int stringList_convertor(PyObject *obj, stringList **slp);
static PyObject *stringList_convert_from(stringList *sl);
static int extend_stringList(stringList **slp, PyObject *py_list, int no_dups);


/*
 * The sipbuild.code_generator module initialisation function.
 */
PyMODINIT_FUNC PyInit_code_generator(void)
{
    static PyMethodDef methods[] = {
        {"set_globals", py_set_globals, METH_VARARGS, NULL},
        {"py2c", py_py2c, METH_VARARGS, NULL},
        {"generateCode", py_generateCode, METH_VARARGS, NULL},
        {NULL, NULL, 0, NULL},
    };

    static PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "sipbuild.code_generator",  /* m_name */
        NULL,                       /* m_doc */
        -1,                         /* m_size */
        methods,                    /* m_methods */
        NULL,                       /* m_reload */
        NULL,                       /* m_traverse */
        NULL,                       /* m_clear */
        NULL,                       /* m_free */
    };

    return PyModule_Create(&module_def);
}


/*
 * Set the globals.
 */
static PyObject *py_set_globals(PyObject *self, PyObject *args)
{
    unsigned abi_major, abi_minor;

    if (!PyArg_ParseTuple(args, "IzIIzO",
            &sipVersion,
            &sipVersionStr,
            &abi_major,
            &abi_minor,
            &sipName,
            &exception_type))
        return NULL;

    abiVersion = (abi_major << 8) | abi_minor;

    if (sipName != NULL && sipName[0] == '\0')
        sipName = NULL;

    Py_INCREF(exception_type);

    Py_RETURN_NONE;
}


/*
 * Wrapper around py2c().
 */
static PyObject *py_py2c(PyObject *self, PyObject *args)
{
    PyObject *spec;
    const char *encoding;
    extern sipSpec *py2c(PyObject *, const char *);

    if (!PyArg_ParseTuple(args, "Os",
            &spec,
            &encoding))
        return NULL;

    return PyCapsule_New(py2c(spec, encoding), NULL, NULL);
}


/*
 * Wrapper around generateCode().
 */
static PyObject *py_generateCode(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    char *codeDir, *srcSuffix;
    const char *api_header;
    int exceptions, tracing, releaseGIL, parts, docs, py_debug;
    stringList *versions, *xfeatures, *sources;

    if (!PyArg_ParseTuple(args, "O&O&O&pppiO&O&pp",
            sipSpec_convertor, &pt,
            fs_convertor, &codeDir,
            fs_convertor, &srcSuffix,
            &exceptions,
            &tracing,
            &releaseGIL,
            &parts,
            stringList_convertor, &versions,
            stringList_convertor, &xfeatures,
            &docs,
            &py_debug))
        return NULL;

    sources = generateCode(pt, codeDir, srcSuffix, exceptions, tracing,
            releaseGIL, parts, versions, xfeatures, docs, py_debug,
            &api_header);

    if (sources == NULL)
    {
        raise_exception();
        return NULL;
    }

    return Py_BuildValue("(sN)", api_header, stringList_convert_from(sources));
}


/*
 * Convert a callable argument to a filesystem name.
 */
static int fs_convertor(PyObject *obj, char **fsp)
{
    PyObject *bytes;
    char *fs;

    if (obj == Py_None)
    {
        *fsp = NULL;
        return 1;
    }

    if ((bytes = PyUnicode_EncodeFSDefault(obj)) == NULL)
        return 0;

    if ((fs = PyBytes_AsString(bytes)) == NULL)
    {
        Py_DECREF(bytes);
        return 0;
    }

    *fsp = sipStrdup(fs);

    Py_DECREF(bytes);

    return 1;
}


/*
 * Convert a callable argument to a sipSpec.
 */
static int sipSpec_convertor(PyObject *obj, sipSpec **ptp)
{
    if (!PyCapsule_CheckExact(obj))
    {
        PyErr_SetString(PyExc_TypeError, "parse tree expected");
        return 0;
    }

    if ((*ptp = (sipSpec *)PyCapsule_GetPointer(obj, NULL)) == NULL)
        return 0;

    return 1;
}


/*
 * Convert a callable argument to a stringList.
 */
static int stringList_convertor(PyObject *obj, stringList **slp)
{
    *slp = NULL;

    if (obj == Py_None)
        return 1;

    return extend_stringList(slp, obj, 0);
}


/*
 * Convert a stringList to a Python list of strings.
 */
static PyObject *stringList_convert_from(stringList *sl)
{
    PyObject *pyl;

    if ((pyl = PyList_New(0)) == NULL)
        return NULL;

    while (sl != NULL)
    {
        PyObject *s;

        if ((s = PyUnicode_DecodeLocale(sl->s, NULL)) == NULL)
        {
            Py_DECREF(pyl);
            return NULL;
        }

        if (PyList_Append(pyl, s) < 0)
        {
            Py_DECREF(s);
            Py_DECREF(pyl);
            return NULL;
        }

        Py_DECREF(s);

        sl = sl->next;
    }

    return pyl;
}


/*
 * Extend a stringList by the contents fo a Python list of strings.
 */
static int extend_stringList(stringList **slp, PyObject *py_list, int no_dups)
{
    Py_ssize_t i, size;

    if ((size = PyList_Size(py_list)) < 0)
        return 0;

    for (i = 0; i < size; ++i)
    {
        const char *el_s;
        PyObject *el = PyUnicode_EncodeLocale(PyList_GetItem(py_list, i),
                NULL);

        if (el == NULL)
            return 0;

        if ((el_s = PyBytes_AsString(el)) == NULL)
        {
            Py_DECREF(el);
            return 0;
        }

        if (no_dups)
        {
            stringList *sl;

            for (sl = *slp; sl != NULL; sl = sl->next)
                if (strcmp(sl->s, el_s) == 0)
                    break;

            if (sl != NULL)
                continue;
        }

        appendString(slp, sipStrdup(el_s));
        Py_DECREF(el);
    }

    return 1;
}


/*
 * Display a one line error message describing a fatal error.
 */
int error(const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(error_text);
    size_t room = sizeof (error_text) - used - 1;

    va_start(ap,fmt);
    vsnprintf(&error_text[used], room, fmt, ap);
    va_end(ap);

    return -1;
}


/*
 * Append to the current error message.
 */
void errorAppend(const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(error_text);
    size_t room = sizeof (error_text) - used - 1;

    va_start(ap, fmt);
    vsnprintf(&error_text[used], room, fmt, ap);
    va_end(ap);
}


/*
 * Raise an exception if needed.
 */
static void raise_exception()
{
    PyErr_SetString(exception_type, error_text);

    /*
     * The error text buffer may be used more than once as the Python exception
     * could be ignored.
     */
    error_text[0] = '\0';
}
