/*
 * The transitional Python bindings for the C parts of the sip code generator.
 *
 * Copyright (c) 2019 Riverbank Computing Limited <info@riverbankcomputing.com>
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
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <Python.h>

#include "sip.h"


/* Globals - see sip.h for their meanings. */
unsigned sipVersion;
const char *sipVersionStr;
unsigned abiMajor;
unsigned abiMinor;
stringList *includeDirList;

/* Support for fatal error handling. */
#define NO_EXCEPTION        0           /* No exception has been set. */
#define EXCEPTION_SET       1           /* An exception has been set. */
#define EXCEPTION_NEEDED    2           /* An exception needs to be set. */

static char error_text[1000];
static jmp_buf on_fatal_error;
static PyObject *exception_type;
static void raise_exception(int action);

/* Forward declarations. */
static PyObject *py_set_globals(PyObject *self, PyObject *args);
static PyObject *py_parse(PyObject *self, PyObject *args);
static PyObject *py_generateCode(PyObject *self, PyObject *args);
static PyObject *py_generateExtracts(PyObject *self, PyObject *args);
static PyObject *py_generateAPI(PyObject *self, PyObject *args);
static PyObject *py_generateXML(PyObject *self, PyObject *args);
static PyObject *py_generateTypeHints(PyObject *self, PyObject *args);

static int fs_convertor(PyObject *obj, char **fsp);
static int sipSpec_convertor(PyObject *obj, sipSpec **ptp);
static int stringList_convertor(PyObject *obj, stringList **slp);
static PyObject *stringList_convert_from(stringList *sl);
static int extend_stringList(stringList **slp, PyObject *py_list);
static void exception_set(void);


/*
 * The sipbuild.code_generator module initialisation function.
 */
PyMODINIT_FUNC PyInit_code_generator(void)
{
    static PyMethodDef methods[] = {
        {"set_globals", py_set_globals, METH_VARARGS, NULL},
        {"parse", py_parse, METH_VARARGS, NULL},
        {"generateCode", py_generateCode, METH_VARARGS, NULL},
        {"generateExtracts", py_generateExtracts, METH_VARARGS, NULL},
        {"generateAPI", py_generateAPI, METH_VARARGS, NULL},
        {"generateXML", py_generateXML, METH_VARARGS, NULL},
        {"generateTypeHints", py_generateTypeHints, METH_VARARGS, NULL},
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
    if (!PyArg_ParseTuple(args, "IsIIOO&",
            &sipVersion,
            &sipVersionStr,
            &abiMajor,
            &abiMinor,
            &exception_type,
            stringList_convertor, &includeDirList))
        return NULL;

    Py_INCREF(exception_type);

    Py_RETURN_NONE;
}


/*
 * Wrapper around parse().
 */
static PyObject *py_parse(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    FILE *file;
    char *filename;
    stringList *versions, *backstops, *xfeatures, *sip_files;
    int strict, protHack, action;

    if (!PyArg_ParseTuple(args, "O&pO&O&O&p",
            fs_convertor, &filename,
            &strict,
            stringList_convertor, &versions,
            stringList_convertor, &backstops,
            stringList_convertor, &xfeatures,
            &protHack))
        return NULL;

    pt = sipMalloc(sizeof (sipSpec));

    if (filename != NULL)
    {
        file = NULL;
    }
    else
    {
        file = stdin;
        filename = "stdin";
    }

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    sip_files = NULL;

    parse(pt, file, filename, strict, &versions, backstops, &xfeatures,
            protHack, &sip_files);

    transform(pt, strict);

    return Py_BuildValue("(NsiNNN)", PyCapsule_New(pt, NULL, NULL),
            pt->module->fullname->text,
            (isComposite(pt->module) || useLimitedAPI(pt->module)),
            stringList_convert_from(sip_files),
            stringList_convert_from(versions),
            stringList_convert_from(xfeatures));
}


/*
 * Wrapper around generateCode().
 */
static PyObject *py_generateCode(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    char *codeDir, *srcSuffix, *sipName;
    const char *api_header;
    int exceptions, tracing, releaseGIL, parts, docs, py_debug, action;
    stringList *versions, *xfeatures, *sources;

    if (!PyArg_ParseTuple(args, "O&O&O&pppiO&O&ppz",
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
            &py_debug,
            &sipName))
        return NULL;

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    if (sipName != NULL && sipName[0] == '\0')
        sipName = NULL;

    sources = generateCode(pt, codeDir, srcSuffix, exceptions, tracing,
            releaseGIL, parts, versions, xfeatures, docs, py_debug, sipName,
            &api_header);

    return Py_BuildValue("(sN)", api_header, stringList_convert_from(sources));
}


/*
 * Wrapper around generateExtracts().
 */
static PyObject *py_generateExtracts(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    stringList *extracts;
    int action;

    if (!PyArg_ParseTuple(args, "O&O&",
            sipSpec_convertor, &pt,
            stringList_convertor, &extracts))
        return NULL;

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    generateExtracts(pt, extracts);

    Py_RETURN_NONE;
}


/*
 * Wrapper around generateAPI().
 */
static PyObject *py_generateAPI(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    char *apiFile;
    int action;

    if (!PyArg_ParseTuple(args, "O&O&",
            sipSpec_convertor, &pt,
            fs_convertor, &apiFile))
        return NULL;

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    generateAPI(pt, pt->module, apiFile);

    Py_RETURN_NONE;
}


/*
 * Wrapper around generateXML().
 */
static PyObject *py_generateXML(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    char *xmlFile;
    int action;

    if (!PyArg_ParseTuple(args, "O&O&",
            sipSpec_convertor, &pt,
            fs_convertor, &xmlFile))
        return NULL;

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    generateXML(pt, pt->module, xmlFile);

    Py_RETURN_NONE;
}


/*
 * Wrapper around generateTypeHints().
 */
static PyObject *py_generateTypeHints(PyObject *self, PyObject *args)
{
    sipSpec *pt;
    char *pyiFile;
    int action;

    if (!PyArg_ParseTuple(args, "O&O&",
            sipSpec_convertor, &pt,
            fs_convertor, &pyiFile))
        return NULL;

    if ((action = setjmp(on_fatal_error)) != NO_EXCEPTION)
    {
        raise_exception(action);
        return NULL;
    }

    generateTypeHints(pt, pt->module, pyiFile);

    Py_RETURN_NONE;
}


/*
 * Convert a callable argument to a filesystem name.
 */
static int fs_convertor(PyObject *obj, char **fsp)
{
    PyObject *bytes;

    if (obj == Py_None)
    {
        *fsp = NULL;
        return 1;
    }

    if ((bytes = PyUnicode_EncodeFSDefault(obj)) == NULL)
        return 0;

    /* Leak the bytes object rather than strdup() its contents. */
    *fsp = PyBytes_AS_STRING(bytes);

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

    if (!PyList_Check(obj))
    {
        PyErr_SetString(PyExc_TypeError, "list of str expected");
        return 0;
    }

    return extend_stringList(slp, obj);
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
static int extend_stringList(stringList **slp, PyObject *py_list)
{
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(py_list); ++i)
    {
        PyObject *el = PyUnicode_EncodeLocale(PyList_GET_ITEM(py_list, i),
                NULL);

        if (el == NULL)
            return 0;

        appendString(slp, sipStrdup(PyBytes_AS_STRING(el)));
    }

    return 1;
}


/*
 * Display a one line error message describing a fatal error.  This does not
 * return.
 */
void fatal(const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(error_text);
    size_t room = sizeof (error_text) - used - 1;

    va_start(ap,fmt);
    vsnprintf(&error_text[used], room, fmt, ap);
    va_end(ap);

    /* Raise an exception. */
    longjmp(on_fatal_error, EXCEPTION_NEEDED);
}


/*
 * Append to the current error message.
 */
void fatalAppend(const char *fmt, ...)
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
static void raise_exception(int action)
{
    if (action == EXCEPTION_NEEDED)
        PyErr_SetString(exception_type, error_text);

    /*
     * The error text buffer may be used more than once as the Python exception
     * could be ignored.
     */
    error_text[0] = '\0';
}


/*
 * Return to the Python interpreter after an exception has been set.
 */
static void exception_set(void)
{
    longjmp(on_fatal_error, EXCEPTION_SET);
}


/*
 * Display a warning message.
 */
void warning(Warning w, const char *fmt, ...)
{
    static char warning_text[1000];

    va_list ap;
    size_t used = strlen(warning_text);
    size_t room = sizeof (warning_text) - used - 1;

    va_start(ap, fmt);
    vsnprintf(&warning_text[used], room, fmt, ap);
    va_end(ap);

    if (strchr(fmt, '\n') != NULL)
    {
        int ret;

        ret = PyErr_WarnEx(
                (w == DeprecationWarning ? PyExc_FutureWarning :
                        PyExc_UserWarning),
                warning_text, 1);

        warning_text[0] = '\0';

        if (ret < 0)
            exception_set();
    }
}


/*
 * Get the configuration of a set of bindings and update the list of tags and
 * disabled features.
 */
void get_bindings_configuration(const char *sip_file, stringList **tags,
        stringList **disabled)
{
    static PyObject *get_bindings_configuration = NULL;

    PyObject *res, *py_tags, *py_disabled;

    /* Get the Python helper. */
    if (get_bindings_configuration == NULL)
    {
        PyObject *mod;

        if ((mod = PyImport_ImportModule("sipbuild.helpers")) == NULL)
            exception_set();

        get_bindings_configuration = PyObject_GetAttrString(mod,
                "get_bindings_configuration");
        Py_DECREF(mod);

        if (get_bindings_configuration == NULL)
            exception_set();
    }

    /* Call the helper. */
    res = PyObject_CallFunction(get_bindings_configuration, "IIsN", abiMajor,
            abiMinor, sip_file, stringList_convert_from(includeDirList));

    if (res == NULL)
        exception_set();

    /* The result should be a 2-tuple of lists of strings. */
    assert(PyTuple_Check(res));
    assert(PyTuple_GET_SIZE(res) == 2);

    py_tags = PyTuple_GET_ITEM(res, 0);
    assert(PyList_Check(py_tags));

    if (!extend_stringList(tags, py_tags))
    {
        Py_DECREF(res);
        exception_set();
    }

    py_disabled = PyTuple_GET_ITEM(res, 1);
    assert(PyList_Check(py_disabled));

    if (!extend_stringList(disabled, py_disabled))
    {
        Py_DECREF(res);
        exception_set();
    }

    /* Tidy up. */
    Py_DECREF(res);
}
