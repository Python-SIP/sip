/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the docstrings support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_docstrings.h"

#include "sip_attribute.h"
#include "sip_iterators.h"


/* The closure used when iterating over extender docstrings. */
typedef struct {
    PyObject **doc_p;
    PyObject *newline;
} ExtensionsClosure;


/* Forward declarations. */
static int append_docs(PyObject **doc_p, PyObject *newline,
        const sipDocSpec *ds);
static int append_text(PyObject **doc_p, PyObject *newline, const char *text,
        bool *new_overload_p);
static int callable_extensions_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure);
static int init_extensions_handler(sipModuleState *x_ms,
        const sipInitExtenderSpec *x_init_extender_spec, void *closure);


/*
 * Return the docstring for a callable.
 */
PyObject *sip_get_callable_docstring(sipModuleState *ms,
        const sipAttrSpec *attr_spec, const sipTypeSpec *extending_ts)
{
    const sipDocSpec *docs = attr_spec->spec.callable->docs;

    if (docs == NULL)
        Py_RETURN_NONE;

    PyObject *newline = PyUnicode_InternFromString("\n");
    if (newline == NULL)
        return NULL;

    PyObject *doc = NULL;

    if (append_docs(&doc, newline, docs) < 0)
        goto release;

    /* Append any docstrings from extenders. */
    if (extending_ts != NULL)
    {
        ExtensionsClosure closure = {
            .doc_p = &doc,
            .newline = newline,
        };

        if (sip_iterate_over_attributes(ms, extending_ts, sipAttrGetName(attr_spec), callable_extensions_handler, &closure) < 0)
            goto release;
    }

    Py_DECREF(newline);

    return doc;

release:
    Py_DECREF(newline);
    Py_XDECREF(doc);

    return NULL;
}


/*
 * Return the docstring for a class.
 */
PyObject *sip_get_class_docstring(sipWrapperType *wt)
{
    const sipTypeSpec *ts = sip_get_type_spec_from_wt(wt);

    if (!sipTypeSpecIsClass(ts))
        Py_RETURN_NONE;

    const sipClassTypeSpec *cts = (const sipClassTypeSpec *)ts;
    const sipDocSpec *ctor_docs = cts->docs;;

    if (cts->docstring == NULL && ctor_docs == NULL)
        Py_RETURN_NONE;

    PyObject *newline = PyUnicode_InternFromString("\n");
    if (newline == NULL)
        return NULL;

    PyObject *doc = NULL;

    /* See if we need and have any leading ctor documentation. */
    if (ctor_docs != NULL && (cts->docstring == NULL || cts->docstring[0] == 'p'))
    {
        if (append_docs(&doc, newline, ctor_docs) < 0)
            goto release;

        ctor_docs = NULL;
    }

    /* Add any class docstring. */
    if (cts->docstring != NULL)
    {
        bool new_overload = true;

        if (append_text(&doc, newline, cts->docstring + 1, &new_overload) < 0)
            goto release;
    }

    /* See if we need and have any trailing ctor documentation. */
    if (ctor_docs != NULL && (cts->docstring == NULL || cts->docstring[0] == 'a'))
    {
        if (append_docs(&doc, newline, ctor_docs) < 0)
            goto release;
    }

    /* Append docstrings from any init extenders. */
    ExtensionsClosure closure = {
        .doc_p = &doc,
        .newline = newline,
    };

    if (sip_iterate_over_init_extenders(sip_get_module_state(wt->defining_module), ts, init_extensions_handler, &closure) < 0)
        goto release;

    Py_DECREF(newline);

    return doc;

release:
    Py_DECREF(newline);
    Py_XDECREF(doc);
    return NULL;
}


/*
 * Append the docstring from a document specification.
 */
static int append_docs(PyObject **doc_p, PyObject *newline,
        const sipDocSpec *ds)
{
    while (ds->type_hint != NULL || ds->docstring != NULL)
    {
        /*
         * A blank line is inserted between sections if there is a docstring.
         * Otherwise we don't use blank lines if we only have type hints.  This
         * may look a little odd in some cases but normally everything has a
         * docstring or nothing has a docstring.
         */
        bool new_overload = (ds->docstring != NULL);

        /*
         * Get the type hint as a Python object if there is one and it is
         * needed.
         */
        const char *type_hint;

        if (ds->type_hint != NULL && (ds->docstring == NULL || ds->docstring[0] != 'd'))
            type_hint = ds->type_hint;
        else
            type_hint = NULL;

        /* See if the type hint goes first. */
        if (type_hint != NULL && (ds->docstring == NULL || ds->docstring[0] == 'p'))
        {
            if (append_text(doc_p, newline, type_hint, &new_overload) < 0)
                return -1;

            type_hint = NULL;
        }

        /* Any docstring will go next. */
        if (ds->docstring != NULL)
        {
            if (append_text(doc_p, newline, ds->docstring + 1, &new_overload) < 0)
                return -1;
        }

        /* If we still have a type hint then it goes last. */
        if (type_hint != NULL)
        {
            if (append_text(doc_p, newline, type_hint, &new_overload) < 0)
                return -1;
        }

        ds++;
    }

    return 0;
}


/*
 * Update a partial docstring with a new line.
 */
static int append_text(PyObject **doc_p, PyObject *newline, const char *text,
        bool *new_overload_p)
{
    if (*doc_p != NULL)
    {
        /* Existing text doesn't have a trailing newline. */
        PyUnicode_Append(doc_p, newline);
        if (*doc_p == NULL)
            return -1;

        /* See if an overload separator is needed. */
        if (*new_overload_p)
        {
            PyUnicode_Append(doc_p, newline);
            if (*doc_p == NULL)
                return -1;
        }
    }

    *new_overload_p = false;

    PyObject *text_obj = PyUnicode_FromString(text);
    if (text_obj == NULL)
        return -1;

    if (*doc_p == NULL)
    {
        *doc_p = text_obj;
    }
    else
    {
        PyUnicode_AppendAndDel(doc_p, text_obj);
        if (*doc_p == NULL)
            return -1;
    }

    return 0;
}


/*
 * The attribute iterator handler that appends an extension docstring.
 */
//static int callable_extensions_handler(sipModuleState *Py_UNUSED(x_ms),
static int callable_extensions_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure)
{
    ExtensionsClosure *ec = (ExtensionsClosure *)closure;

    const sipDocSpec *docs = x_attr_spec->spec.callable->docs;
    if (docs == NULL)
        return 0;

    return append_docs(ec->doc_p, ec->newline, docs);
}


/*
 * The init extender iterator handler that appends an extension docstring.
 */
static int init_extensions_handler(sipModuleState *Py_UNUSED(x_ms),
        const sipInitExtenderSpec *x_init_extender_spec, void *closure)
{
    ExtensionsClosure *ec = (ExtensionsClosure *)closure;

    const sipDocSpec *docs = x_init_extender_spec->docs;
    if (docs == NULL)
        return 0;

    return append_docs(ec->doc_p, ec->newline, docs);
}
