/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This implements the helpers for handwritten code that are not needed
 * otherwise.  Backwards compatibility is more important than modern design.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>
#include <datetime.h>

#include <string.h>

#include "sip_helpers.h"

#include "sip.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper_type.h"


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
 * Create a Python object for a member of a named enum which is assumed to have
 * the default underlying type (ie. int).
 */
PyObject *sip_api_convert_from_enum(sipModuleState *ms, int eval,
        sipTypeID type_id)
{
    return sip_api_convert_from_based_enum(ms, &eval, type_id);
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
 * Convert a Python object implementing an enum to a member value.  An
 * exception is raised if there was an error.  The enum is assumed to have the
 * default underlying type (ie. int).
 */
int sip_api_convert_to_enum(sipModuleState *ms, PyObject *obj,
        sipTypeID type_id)
{
    int eval;

    sip_enum_convert_to_based_enum(ms, obj, &eval, type_id, TRUE);

    return eval;
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
 * Return the assignment helper function for a type or NULL if it doesn't have
 * one.
 */
sipAssignFunc sip_api_get_assignment_function(sipModuleState *ms,
        sipTypeID type_id)
{
    PyObject *def_mod;
    const sipTypeSpec *spec = sip_get_type_spec(ms, type_id, &def_mod);
    if (spec == NULL)
        return NULL;

    Py_DECREF(def_mod);

    if (sipTypeIsClass(type_id))
        return ((const sipClassTypeSpec *)spec)->assign;

    if (sipTypeIsMapped(type_id))
        return ((const sipMappedTypeSpec *)spec)->assign;

    return NULL;
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
 * Return the module state for the module with a particular token.
 */
sipModuleState *sip_api_get_module_state(sipModuleState *ms, void *token)
{
    PyObject *mods = ms->sip_module_state->module_list;
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(mods); i++)
    {
        PyObject *mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, i), &mod) < 0)
            return NULL;

        if (mod == NULL)
            continue;

        void *mod_token;
        if (PyModule_GetToken(mod, &mod_token) == 0 && mod_token == token)
        {
            sipModuleState *mod_state = sip_get_module_state(mod);

            /*
             * It should be Ok to release the reference as the module should be
             * referenced by the calling module's import chain.
             */
            Py_DECREF(mod);

            return mod_state;
        }

        Py_DECREF(mod);
    }

    PyErr_SetString(PyExc_RuntimeError, "the token isn't recognised");

    return NULL;
}


/*
 * Return the module state from a Python type.
 */
sipModuleState *sip_api_get_module_state_by_type(void *token,
        PyTypeObject *py_type)
{
    PyObject *mod = PyType_GetModuleByToken(py_type, token);
    if (!mod)
        return NULL;

    sipModuleState *sipMS = (sipModuleState *)PyModule_GetState(mod);

    /*
     * It should be Ok to release the reference as the module should be
     * referenced by the calling module's import chain.
     */
    Py_DECREF(mod);

    return sipMS;
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
 * Return the absolute type ID of a (possibly) relative type ID.
 */
sipTypeID sip_api_make_absolute(sipModuleState *ms, sipTypeID type_id)
{
    /* Handle the trivial cases. */
    if (sipTypeIDIsAbsolute(type_id) || sipTypeIDIsPOD(type_id))
        return type_id;

    sipModuleState *def_ms;
    sipTypeNr def_type_nr;

    if (sipTypeIDIsLocalModule(type_id))
    {
        def_ms = ms;
        def_type_nr = sipTypeIDTypeNr(type_id);
    }
    else
    {
        sipImportedModule *im = &ms->imported_modules[sipTypeIDModuleNr(type_id)];

        def_ms = sip_get_module_state(im->module);
        def_type_nr = im->type_nr_map[sipTypeIDTypeNr(type_id)];
    }

    return SIP_TYPE_ID_ABSOLUTE | (type_id & SIP_TYPE_ID_TYPE_MASK) |
            (def_ms->module_nr << 16) | def_type_nr;
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
 * Get the unqualified name of a Python type.
 */
const char *sip_api_py_type_name(const PyTypeObject *py_type)
{
    /* We allow any Python type, not just wrapper types. */
    const char *name = strrchr(py_type->tp_name, '.');

    return name != NULL ? name + 1 : py_type->tp_name;
}


/*
 * A thin wrapper around PyType_GetDict() (on behalf of the limited API).
 */
PyObject *sip_api_py_type_dict_ref(PyTypeObject *py_type)
{
    return PyType_GetDict(py_type);
}


/*
 * Return the absolute type ID of a Python type or sipType_Invalid if it
 * doesn't have a type specification.
 */
sipTypeID sip_api_type_from_py_type_object(sipModuleState *ms,
        PyTypeObject *py_type)
{
    sipSipModuleState *sms = ms->sip_module_state;

    if (PyObject_TypeCheck((PyObject *)py_type, sms->wrapper_type_type))
        return ((sipWrapperType *)py_type)->type_id;

#if defined(SIP_CONFIGURATION_CustomEnums)
    if (PyObject_TypeCheck((PyObject *)py_type, sms->custom_enum_type))
        return ((sipEnumTypeImpl *)py_type)->type_id;
#endif

    if (sip_enum_is_enum(sms, (PyObject *)py_type))
    {
        PyObject *dunder_sip = PyUnicode_InternFromString("__sip__");
        if (dunder_sip == NULL)
            return sipType_Invalid;

        PyObject *type_id_obj = PyObject_GetAttr((PyObject *)py_type,
                dunder_sip);

        Py_DECREF(dunder_sip);

        if (type_id_obj == NULL)
            return sipType_Invalid;

        sipTypeID type_id;

        if (PyLong_AsUInt32(type_id_obj, &type_id) < 0)
            type_id = sipType_Invalid;

        Py_DECREF(type_id_obj);

        return type_id;
    }

    return sipType_Invalid;
}


/*
 * Return the C/C++ name of a wrapped type.
 */
const char *sip_api_type_name(sipModuleState *ms, sipTypeID type_id)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);

    if (def_mod == NULL)
        return NULL;

    Py_DECREF(def_mod);

    return ts->cpp_name;
}


/*
 * Return a pointer to the immutable plugin-specific data for a type.
 */
const void *sip_api_type_plugin_data(sipModuleState *ms, uint32_t plugin_id,
        sipTypeID type_id)
{
    if (!sipTypeIsClass(type_id))
        return NULL;

    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec( ms, type_id, &def_mod);

    if (def_mod == NULL)
        return NULL;

    Py_DECREF(def_mod);

    const sipPluginDataSpec *pds = ((const sipClassTypeSpec *)ts)->plugins_data;
    if (pds == NULL)
        return NULL;

    while (pds->data != NULL)
    {
        if (pds->plugin_id == plugin_id)
            return pds->data;

        pds++;
    }

    return NULL;
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
