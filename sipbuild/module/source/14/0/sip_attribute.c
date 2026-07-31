/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The support for attributes.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stdlib.h>
#include <string.h>

#include "sip_attribute.h"

#include "sip_callable.h"
#include "sip_iterators.h"
#include "sip_method_descriptor.h"
#include "sip_sip_module.h"
#include "sip_variable_descriptor.h"
#include "sip_variable.h"
#include "sip_wrapper_type.h"


/* The closure used when iterating to get attribute names. */
typedef struct {
    PyObject *attr_dict;
} DirClosure;


/* The closure used when iterating to get attribute values. */
typedef struct {
    sipModuleState *ms;
    const sipAttrSpec *attr_spec;
} GetClosure;


/* Forward declarations. */
static int add_attr_group_to_dir(PyObject *dir,
        const sipAttrGroupSpec *const attr_group);
static int compare_attribute(const void *key, const void *el);
static PyObject *create_callable(sipModuleState *ms,
        const sipAttrSpec *attr_spec);
static PyObject *create_property(sipModuleState *ms,
        const sipAttrSpec *attr_spec);
static int dir_handler(sipModuleState *x_ms, const sipAttrSpec *x_attr_spec,
        void *closure);
static PyObject *get_dir(sipModuleState *ms, PyObject *attr_dict,
        const sipAttrGroupSpec *const attrs_static_variables,
        const sipAttrGroupSpec *const attrs_types,
        const sipTypeSpec *extending_ts);
static int get_extender_attribute(sipModuleState *ms,
        const sipTypeSpec *extending_ts, const char *name,
        sipModuleState **x_ms_p, const sipAttrSpec **x_attr_spec_p);
static int get_extender_attribute_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure);
static PyObject *get_py_type(sipModuleState *ms, const sipAttrSpec *attr_spec);


/*
 * Return the __dir__ of a module.
 */
PyObject *sip_dir_of_module(PyObject *mod, PyObject *attr_dict)
{
    sipModuleState *ms = sip_get_module_state(mod);

    return get_dir(ms, attr_dict, ms->module_spec->attrs_static_variables,
            ms->module_spec->attrs_types, NULL);
}


/*
 * Return the __dir__ of a wrapper type.
 */
PyObject *sip_dir_of_wt(sipWrapperType *wt, PyObject *attr_dict)
{
    /* Add the class dicts of any MRO types. */
    PyObject *mro = ((PyTypeObject *)wt)->tp_mro;
    assert(PyTuple_CheckExact(mro));

    Py_ssize_t mro_i = PyTuple_GET_SIZE(mro);

    while (mro_i-- > 0)
    {
        PyObject *cls_dict = ((PyTypeObject *)PyTuple_GET_ITEM(mro, mro_i))->tp_dict;

        if (cls_dict != NULL && PyDict_Update(attr_dict, cls_dict) < 0)
            return NULL;
    }

    /* Get the attribute groups. */
    const sipTypeSpec *ts = sip_get_type_spec_from_wt(wt);
    const sipAttrGroupSpec *attrs_static_variables, *attrs_types;

    if (sipTypeSpecIsMapped(ts))
    {
        attrs_static_variables = NULL;
        attrs_types = ((const sipMappedTypeSpec *)ts)->attrs_types;
    }
    else
    {
        attrs_static_variables = ((const sipClassTypeSpec *)ts)->attrs_static_variables;
        attrs_types = ((const sipClassTypeSpec *)ts)->attrs_types;
    }

    return get_dir(sip_get_module_state(wt->defining_module), attr_dict,
            attrs_static_variables, attrs_types,
            sipTypeSpecIsNamespace(ts) ? ts : NULL);
}


/*
 * Return a new reference to the Python object for an attribute according to
 * its specification.
 */
PyObject *sip_get_attribute_for_spec(sipModuleState *ms,
        PyObject *defining_object, const sipAttrSpec *attr_spec,
        const sipTypeSpec *extending_ts)
{
    switch (attr_spec->name[0])
    {
    case 'c':
    case 'e':
        return sipMethodDescr_New(ms->sip_module_state, attr_spec,
                ms->wrapped_module, defining_object, extending_ts);

    case 'i':
        return sipVariableDescr_New(ms->sip_module_state, attr_spec,
                defining_object);

    case 'm':
        return sipCallable_New(ms->sip_module_state, attr_spec,
                ms->wrapped_module, NULL, NULL);

    case 'p':
        return create_property(ms, attr_spec);

    case 's':
        return sipMethodDescr_New(ms->sip_module_state, attr_spec,
                ms->wrapped_module, defining_object, NULL);

    case 't':
        return get_py_type(ms, attr_spec);
    }

    /* This should never happen. */
    return NULL;
}


/*
 * Return the attribute specification for a name or NULL if there was none.
 */
const sipAttrSpec *sip_get_attribute_spec(const char *name,
        const sipAttrGroupSpec *attrs)
{
    if (attrs == NULL)
        return NULL;

    return (const sipAttrSpec *)bsearch((const void *)name,
            (const void *)attrs->attrs, attrs->nr_attrs, sizeof (sipAttrSpec),
            compare_attribute);
}


/*
 * The getattro handler for modules and containers.
 */
PyObject *sip_mod_con_getattro(sipModuleState *ms, PyObject *self,
        PyObject *name, PyObject *attr_dict,
        const sipAttrGroupSpec *const attrs_static_variables,
        const sipAttrGroupSpec *const attrs_types,
        const sipTypeSpec *extending_ts)
{
    const char *utf8_name = PyUnicode_AsUTF8(name);
    const sipAttrSpec *attr_spec;

    /*
     * The behaviour of static variables is that of a data descriptor and they
     * take precedence over any attributes set by the user.
     */
    attr_spec = sip_get_attribute_spec(utf8_name, attrs_static_variables);

    if (attr_spec != NULL)
        return sip_variable_get(ms, self, attr_spec, NULL);

    /* Get any extension attribute. */
    const sipAttrSpec *x_attr_spec = NULL;
    sipModuleState *x_ms = NULL;

    if (extending_ts != NULL)
    {
        /* See if the extension is a static variable. */
        if (get_extender_attribute(ms, extending_ts, utf8_name, &x_ms, &x_attr_spec) < 0)
            return NULL;

        if (x_attr_spec != NULL && sipAttrIsStaticVariable(x_attr_spec))
            return sip_variable_get(x_ms, self, x_attr_spec, NULL);
    }

    /*
     * Revert to the super-class behaviour.  This will pick up any objects
     * already created from wrapped attribute specifications.
     */
    PyObject *attr = Py_TYPE(self)->tp_base->tp_getattro(self, name);
    if (attr != NULL)
        return attr;

    if (!PyErr_ExceptionMatches(PyExc_AttributeError))
        return NULL;

    /* See if there is a wrapped attribute (which should be a type). */
    attr_spec = sip_get_attribute_spec(utf8_name, attrs_types);

    if (attr_spec != NULL)
    {
        /* Clear the super-class AttributeError. */
        PyErr_Clear();

        attr = get_py_type(ms, attr_spec);
        if (attr == NULL)
            return NULL;

        /* Save it in the dict. */
        if (PyDict_SetItem(attr_dict, name, attr) < 0)
        {
            Py_DECREF(attr);
            return NULL;
        }

        return attr;
    }

    /* See if the type has been extended. */
    if (x_attr_spec != NULL)
    {
        /* Clear the super-class AttributeError. */
        PyErr_Clear();

        attr = sip_get_attribute_for_spec(x_ms, self, x_attr_spec,
                extending_ts);
        if (attr == NULL)
            return NULL;

        descrgetfunc getter = Py_TYPE(attr)->tp_descr_get;
        if (getter != NULL)
            Py_SETREF(attr, getter(attr, NULL, self));

        return attr;
    }

    /*
     * The exception from the super-class should still be in place if no
     * attribute was found.
     */
    return NULL;
}


/*
 * The setattro handler for modules and containers.
 */
int sip_mod_con_setattro(sipModuleState *ms, PyObject *self, PyObject *name,
        PyObject *value, const sipAttrGroupSpec *const attrs_static_variables,
        const sipTypeSpec *extending_ts)
{
    const char *utf8_name = PyUnicode_AsUTF8(name);

    /*
     * Note that we can't use a real descriptor for class (ie. static)
     * variables because while the type object will look for a data descriptor
     * in the type's dictionary going a get it doesn't when doing a set.
     * Instead it just overwrites the descriptor.
     */

    const sipAttrSpec *attr_spec = sip_get_attribute_spec(utf8_name,
            attrs_static_variables);

    if (attr_spec != NULL)
        return sip_variable_set(ms, self, value, attr_spec, NULL);

    /* See if there is an extension. */
    if (extending_ts != NULL)
    {
        const sipAttrSpec *x_attr_spec;
        sipModuleState *x_ms;

        if (get_extender_attribute(ms, extending_ts, utf8_name, &x_ms, &x_attr_spec) < 0)
            return -1;

        if (x_attr_spec != NULL && sipAttrIsStaticVariable(x_attr_spec))
            return sip_variable_set(x_ms, self, value, x_attr_spec, NULL);
    }

    return Py_TYPE(self)->tp_base->tp_setattro(self, name, value);
}


/*
 * Add a group of attributes to a dir dictionary.
 */
static int add_attr_group_to_dir(PyObject *dir,
        const sipAttrGroupSpec *const attr_group)
{
    /* Handle the trivial case. */
    if (attr_group == NULL)
        return 0;

    Py_ssize_t attr_i = 0;

    while (attr_i < attr_group->nr_attrs)
    {
        const sipAttrSpec *attr_spec = &attr_group->attrs[attr_i++];

        /* We are only interested in the name and not the value. */
        if (PyDict_SetItemString(dir, sipAttrGetName(attr_spec), Py_None) < 0)
            return -1;
    }

    return 0;
}


/*
 * The bsearch() helper function for searching an attributes table.
 */
static int compare_attribute(const void *key, const void *el)
{
    return strcmp((const char *)key, sipAttrGetName((const sipAttrSpec *)el));
}


/*
 * Return a callable or Py_None if there isn't one.
 */
static PyObject *create_callable(sipModuleState *ms,
        const sipAttrSpec *attr_spec)
{
    if (attr_spec == NULL)
        return Py_NewRef(Py_None);

    return sipCallable_New(ms->sip_module_state, attr_spec, ms->wrapped_module,
            NULL, NULL);
}


/*
 * Create and return a Python property.
 */
static PyObject *create_property(sipModuleState *ms,
        const sipAttrSpec *attr_spec)
{
    const sipPropertySpec *ps = attr_spec->spec.property;
    PyObject *prop, *fget, *fset, *doc;

    prop = fget = fset = doc = NULL;

    if ((fget = create_callable(ms, ps->getter)) == NULL)
        goto done;

    if ((fset = create_callable(ms, ps->setter)) == NULL)
        goto done;

    if (ps->docstring == NULL)
    {
        doc = Py_NewRef(Py_None);
    }
    else if ((doc = PyUnicode_FromString(ps->docstring)) == NULL)
    {
        goto done;
    }

    prop = PyObject_CallFunctionObjArgs((PyObject *)&PyProperty_Type, fget,
            fset, Py_None, doc, NULL);

done:
    Py_XDECREF(fget);
    Py_XDECREF(fset);
    Py_XDECREF(doc);

    return prop;
}


/*
 * The dir handler that adds an attribute name to a dictionary.
 */
static int dir_handler(sipModuleState *Py_UNUSED(x_ms),
        const sipAttrSpec *x_attr_spec, void *closure)
{
    DirClosure *dc = (DirClosure *)closure;

    if (PyDict_SetItemString(dc->attr_dict, sipAttrGetName(x_attr_spec), Py_None) < 0)
        return -1;

    /* Continue iterating. */
    return 0;
}


/*
 * The __dir__ handler for modules and containers.
 */
static PyObject *get_dir(sipModuleState *ms, PyObject *attr_dict,
        const sipAttrGroupSpec *const attrs_static_variables,
        const sipAttrGroupSpec *const attrs_types,
        const sipTypeSpec *extending_ts)
{
    if (add_attr_group_to_dir(attr_dict, attrs_static_variables) < 0)
        return NULL;

    if (add_attr_group_to_dir(attr_dict, attrs_types) < 0)
        return NULL;

    if (extending_ts != NULL)
    {
        DirClosure dir_closure = {.attr_dict = attr_dict};

        if (sip_iterate_over_attributes(ms, extending_ts, NULL, dir_handler, &dir_closure) < 0)
            return NULL;
    }

    return PyDict_Keys(attr_dict);
}


/*
 * Return an attribute specification that extends a type.  Returns -1 if there
 * was an error, otherwise the specification (if there is one) and the defining
 * module state are returned via pointers.
 */
static int get_extender_attribute(sipModuleState *ms,
        const sipTypeSpec *extending_ts, const char *name,
        sipModuleState **x_ms_p, const sipAttrSpec **x_attr_spec_p)
{
    GetClosure get_closure = {0};

    if (sip_iterate_over_attributes(ms, extending_ts, name, get_extender_attribute_handler, &get_closure) < 0)
        return -1;

    *x_ms_p = get_closure.ms;
    *x_attr_spec_p = get_closure.attr_spec;

    return 0;
}


/*
 * The attribute handler that returns an attribute extension.
 */
static int get_extender_attribute_handler(sipModuleState *x_ms,
        const sipAttrSpec *x_attr_spec, void *closure)
{
    GetClosure *gc = (GetClosure *)closure;

    gc->ms = x_ms;
    gc->attr_spec = x_attr_spec;

    /*
     * There is no need to iterate further.  Note that we don't check that
     * there aren't any other extenders for the same name.  This isn't a
     * problem for callables but other type of attribute should be unique.
     */
    return 1;
}


/*
 * Return a strong reference to the Python type object for a wrapped type
 * attribute.
 */
static PyObject *get_py_type(sipModuleState *ms, const sipAttrSpec *attr_spec)
{
    /*
     * Note that the type may have been created some time ago (using a type ID
     * from generated code) and this is just the first time it has been
     * accessed as an attribute.
     */
    PyTypeObject *py_type;

    if (sip_get_local_py_type(ms, attr_spec->spec.type_nr, &py_type) < 0)
        return NULL;

    /*
     * The type would be NULL for mapped types with no attributes but that
     * should never happen in this context.
     */
    assert(py_type != NULL);

    return Py_NewRef(py_type);
}
