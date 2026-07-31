/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This is the implementation of the wrapped variable support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stdbool.h>

#include "sip_variable.h"

#include "sip_attribute.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_int_convertors.h"
#include "sip_parsers.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_string_convertors.h"
#include "sip_voidptr.h"


/*
 * Note that (like the legacy ABIs) we don't support capsule variables.
 * However we could do so relatively easily.  To do so would require a new
 * category of type IDs dedicated to capsules where the module number and type
 * number would identify a sipTypedefSpec from which the name of the capsule
 * can be found.
 */


/* Forward declarations. */
static int get_extra_reference(sipModuleState *ms, PyObject *instance, int key,
        PyObject **ref_p);
static void *get_instance_address(const sipAttrSpec *attr_spec,
        PyObject *instance, PyTypeObject *binding_type);
static int get_optional_instance_address(const sipAttrSpec *attr_spec,
        PyObject *instance, PyTypeObject *binding_type, void **addr_p);
static void *get_variable_address(const sipAttrSpec *v_spec,
        PyObject *instance, PyTypeObject *binding_type);


/*
 * Get the value of a variable.
 */
PyObject *sip_variable_get(sipModuleState *ms, PyObject *instance,
        const sipAttrSpec *attr_spec, PyTypeObject *binding_type)
{
    if (instance != NULL && Py_TYPE(instance) == ms->sip_module_state->module_wrapper_type)
        instance = NULL;

    const sipVariableSpec *v_spec = attr_spec->spec.variable;

    if (v_spec->get_code != NULL)
    {
        void *cpp;

        if (get_optional_instance_address(attr_spec, instance, binding_type, &cpp) < 0)
            return NULL;

        return v_spec->get_code(ms, cpp, (PyObject *)binding_type);
    }

    void *addr;

    if (v_spec->flags & SIP_WV_LITERAL)
        addr = NULL;
    else if ((addr = get_variable_address(attr_spec, instance, binding_type)) == NULL)
        return NULL;

    switch (v_spec->type_id)
    {
        case sipType_bool:
            return PyBool_FromLong(
                    addr != NULL ? *(bool *)addr : v_spec->value.bool_t);

        case sipType_byte:
            return PyLong_FromLong(
                    addr != NULL ? *(char *)addr : v_spec->value.byte_t);

        case sipType_sbyte:
            return PyLong_FromLong(
                    addr != NULL ? *(signed char *)addr : v_spec->value.sbyte_t);

        case sipType_ubyte:
            return PyLong_FromUnsignedLong(
                    addr != NULL ? *(unsigned char *)addr : v_spec->value.ubyte_t);

        case sipType_short:
            return PyLong_FromLong(
                    addr != NULL ? *(short *)addr : v_spec->value.short_t);

        case sipType_ushort:
            return PyLong_FromUnsignedLong(
                    addr != NULL ? *(unsigned short *)addr : v_spec->value.ushort_t);

        case sipType_int:
            return PyLong_FromLong(
                    addr != NULL ? *(int *)addr : v_spec->value.int_t);

        case sipType_uint:
            return PyLong_FromUnsignedLong(
                    addr != NULL ? *(unsigned *)addr : v_spec->value.uint_t);

        case sipType_long:
            return PyLong_FromLong(
                    addr != NULL ? *(long *)addr : v_spec->value.long_t);

        case sipType_ulong:
            return PyLong_FromUnsignedLong(
                    addr != NULL ? *(unsigned long *)addr : v_spec->value.ulong_t);

        case sipType_longlong:
            return PyLong_FromLongLong(
                    addr != NULL ? *(long long *)addr : v_spec->value.longlong_t);

        case sipType_ulonglong:
            return PyLong_FromUnsignedLongLong(
                    addr != NULL ?  *(unsigned long long *)addr : v_spec->value.ulonglong_t);

        case sipType_Py_hash_t:
            return PyLong_FromLongLong((long long)*(Py_hash_t *)addr);

        case sipType_Py_ssize_t:
            return PyLong_FromSsize_t(*(Py_ssize_t *)addr);

        case sipType_size_t:
            return PyLong_FromSize_t(*(size_t *)addr);

        case sipType_float:
            return PyFloat_FromDouble(*(float *)addr);

        case sipType_double:
            return PyFloat_FromDouble(*(double *)addr);

        case sipType_char:
        case sipType_schar:
        case sipType_uchar:
            return PyBytes_FromStringAndSize((char *)addr, 1);

        case sipType_char_ascii:
            return PyUnicode_DecodeASCII((char *)addr, 1, SIP_NULLPTR);

        case sipType_char_latin1:
            return PyUnicode_DecodeLatin1((char *)addr, 1, SIP_NULLPTR);

        case sipType_char_utf8:
            return PyUnicode_DecodeUTF8((char *)addr, 1, SIP_NULLPTR);

        case sipType_wchar:
            return PyUnicode_FromWideChar((wchar_t *)addr, 1);

        case sipType_str:
        case sipType_sstr:
        case sipType_ustr:
        {
            const char *c_value = *(char **)addr;

            if (c_value == SIP_NULLPTR)
                return Py_NewRef(Py_None);

            return PyBytes_FromString(c_value);
        }

        case sipType_str_ascii:
        {
            const char *c_value = *(char **)addr;

            if (c_value == SIP_NULLPTR)
                return Py_NewRef(Py_None);

            return PyUnicode_DecodeASCII(c_value, strlen(c_value),
                    SIP_NULLPTR);
        }

        case sipType_str_latin1:
        {
            const char *c_value = *(char **)addr;

            if (c_value == SIP_NULLPTR)
                return Py_NewRef(Py_None);

            return PyUnicode_DecodeLatin1(c_value, strlen(c_value),
                    SIP_NULLPTR);
        }

        case sipType_str_utf8:
        {
            const char *c_value = *(char **)addr;

            if (c_value == SIP_NULLPTR)
                return Py_NewRef(Py_None);

            return PyUnicode_DecodeUTF8(c_value, strlen(c_value), NULL);
        }

        case sipType_wstr:
        {
            const wchar_t *c_value = *(wchar_t **)addr;

            if (c_value == SIP_NULLPTR)
                return Py_NewRef(Py_None);

            return PyUnicode_FromWideChar(c_value,
                    (Py_ssize_t)wcslen(c_value));
        }

        case sipType_voidptr:
            return sip_convert_from_void_ptr(ms->sip_module_state,
                    *(void **)addr);

        case sipType_voidptr_const:
            return sip_convert_from_const_void_ptr(ms->sip_module_state,
                    *(const void **)addr);

        case sipType_pyobject:
        case sipType_pytuple:
        case sipType_pylist:
        case sipType_pydict:
        case sipType_pycallable:
        case sipType_pyslice:
        case sipType_pytype:
        case sipType_pybuffer:
        {
            /*
             * Note that this is the historical behaviour and is probably
             * inconsistent with what the parsers do.
             */
            PyObject *c_value = *(PyObject **)addr;

            if (c_value == NULL)
                c_value = Py_None;

            return Py_NewRef(c_value);
        }
    }

    if (sipTypeIsEnum(v_spec->type_id))
        return sip_api_convert_from_enum(ms, addr, v_spec->type_id);

    /* If there is no key then the variable is a pointer to a wrapped type. */
    if (v_spec->key == 0)
        return sip_api_convert_from_type(ms, *(void **)addr, v_spec->type_id,
                NULL);

    /*
     * The variable is a wrapped type so first see if it has already been
     * wrapped.
     */
    PyObject *obj;
    if (get_extra_reference(ms, instance, v_spec->key, &obj) != 0)
        return obj;

    obj = sip_api_convert_from_type(ms, addr, v_spec->type_id, NULL);
    if (obj == NULL)
        return NULL;

    /*
     * Keep a reference to the object that wraps the variable in the instance
     * containing the variable (or the module if the variable is static).  This
     * means the same object will be returned in subsequent gets.
     */
    if (sip_api_keep_reference(ms, instance, v_spec->key, obj) < 0)
    {
        Py_DECREF(obj);
        return NULL;
    }

    /*
     * Also keep a reference to the instance containing the variable in the
     * object that wraps the variable.  This prevents the memory implementing
     * the variable from being freed.
     */
    if (sip_api_keep_reference(ms, obj, v_spec->key, instance) < 0)
    {
        Py_DECREF(obj);
        return NULL;
    }

    return obj;
}


/*
 * Set the value of a variable.
 */
int sip_variable_set(sipModuleState *ms, PyObject *instance, PyObject *value,
        const sipAttrSpec *attr_spec, PyTypeObject *binding_type)
{
    if (value == NULL)
    {
        PyErr_Format(PyExc_AttributeError, "'%s' cannot be deleted",
                sipAttrGetName(attr_spec));
        return -1;
    }

    if (instance != NULL && Py_TYPE(instance) == ms->sip_module_state->module_wrapper_type)
        instance = NULL;

    const sipVariableSpec *v_spec = attr_spec->spec.variable;

    if (v_spec->set_code != NULL)
    {
        void *cpp;

        if (get_optional_instance_address(attr_spec, instance, binding_type, &cpp) < 0)
            return -1;

        return v_spec->set_code(ms, cpp, value, (PyObject *)binding_type);
    }

    if (v_spec->flags & (SIP_WV_RO|SIP_WV_LITERAL))
    {
        PyErr_Format(PyExc_ValueError,
                "'%s' is a constant and cannot be modified",
                sipAttrGetName(attr_spec));
        return -1;
    }

    void *addr = get_variable_address(attr_spec, instance, binding_type);
    if (addr == NULL)
        return -1;

    switch (v_spec->type_id)
    {
        case sipType_byte:
        {
            char c_value = sip_api_long_as_char(value);

            if (PyErr_Occurred())
                return -1;

            *(char *)addr = c_value;

            return 0;
        }

        case sipType_sbyte:
        {
            signed char c_value = sip_api_long_as_signed_char(value);

            if (PyErr_Occurred())
                return -1;

            *(signed char *)addr = c_value;

            return 0;
        }

        case sipType_ubyte:
        {
            unsigned char c_value = sip_api_long_as_unsigned_char(value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned char *)addr = c_value;

            return 0;
        }

        case sipType_short:
        {
            short c_value = sip_api_long_as_short(value);

            if (PyErr_Occurred())
                return -1;

            *(short *)addr = c_value;

            return 0;
        }

        case sipType_ushort:
        {
            unsigned short c_value = sip_api_long_as_unsigned_short(value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned short *)addr = c_value;

            return 0;
        }

        case sipType_int:
        {
            int c_value = sip_api_long_as_int(value);

            if (PyErr_Occurred())
                return -1;

            *(int *)addr = c_value;

            return 0;
        }

        case sipType_uint:
        {
            unsigned c_value = sip_api_long_as_unsigned_int(value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned *)addr = c_value;

            return 0;
        }

        case sipType_long:
        {
            long c_value = sip_api_long_as_long(value);

            if (PyErr_Occurred())
                return -1;

            *(long *)addr = c_value;

            return 0;
        }

        case sipType_ulong:
        {
            unsigned long c_value = sip_api_long_as_unsigned_long(value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned long *)addr = c_value;

            return 0;
        }

        case sipType_longlong:
        {
            long long c_value = sip_api_long_as_long_long(value);

            if (PyErr_Occurred())
                return -1;

            *(long long *)addr = c_value;

            return 0;
        }

        case sipType_ulonglong:
        {
            unsigned long long c_value = sip_api_long_as_unsigned_long_long(
                    value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned long long *)addr = c_value;

            return 0;
        }


        case sipType_Py_hash_t:
        {
            Py_hash_t c_value = sip_api_long_as_long(value);

            if (PyErr_Occurred())
                return -1;

            *(Py_hash_t *)addr = c_value;

            return 0;
        }

        case sipType_Py_ssize_t:
        {
            Py_ssize_t c_value = sip_api_long_as_long(value);

            if (PyErr_Occurred())
                return -1;

            *(Py_ssize_t *)addr = c_value;

            return 0;
        }

        case sipType_size_t:
        {
            size_t c_value = sip_api_long_as_size_t(value);

            if (PyErr_Occurred())
                return -1;

            *(size_t *)addr = c_value;

            return 0;
        }

        case sipType_float:
        {
            float c_value = (float)PyFloat_AsDouble(value);

            if (PyErr_Occurred())
                return -1;

            *(float *)addr = c_value;

            return 0;
        }

        case sipType_double:
        {
            double c_value = PyFloat_AsDouble(value);

            if (PyErr_Occurred())
                return -1;

            *(double *)addr = c_value;

            return 0;
        }

        case sipType_char:
        {
            char c_value = sip_bytes_as_char(value);

            if (PyErr_Occurred())
                return -1;

            *(char *)addr = c_value;

            return 0;
        }

        case sipType_char_ascii:
        {
            char c_value = sip_string_as_ascii_char(value);

            if (PyErr_Occurred())
                return -1;

            *(char *)addr = c_value;

            return 0;
        }

        case sipType_char_latin1:
        {
            char c_value = sip_string_as_latin1_char(value);

            if (PyErr_Occurred())
                return -1;

            *(char *)addr = c_value;

            return 0;
        }

        case sipType_char_utf8:
        {
            char c_value = sip_string_as_utf8_char(value);

            if (PyErr_Occurred())
                return -1;

            *(char *)addr = c_value;

            return 0;
        }

        case sipType_schar:
        {
            signed char c_value = (signed char)sip_bytes_as_char(value);

            if (PyErr_Occurred())
                return -1;

            *(signed char *)addr = c_value;

            return 0;
        }

        case sipType_uchar:
        {
            unsigned char c_value = (unsigned char)sip_bytes_as_char(value);

            if (PyErr_Occurred())
                return -1;

            *(unsigned char *)addr = c_value;

            return 0;
        }

        case sipType_wchar:
        {
            wchar_t c_value = sip_string_as_wchar(value);

            if (PyErr_Occurred())
                return -1;

            *(wchar_t *)addr = c_value;

            return 0;
        }

        case sipType_str:
        {
            const char *c_value = sip_bytes_as_string(value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const char **)addr = c_value;

            return 0;
        }

        case sipType_str_ascii:
        {
            const char *c_value = sip_string_as_ascii_string(&value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const char **)addr = c_value;

            return 0;
        }

        case sipType_str_latin1:
        {
            const char *c_value = sip_string_as_latin1_string(&value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const char **)addr = c_value;

            return 0;
        }

        case sipType_str_utf8:
        {
            const char *c_value = sip_string_as_utf8_string(&value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const char **)addr = c_value;

            return 0;
        }

        case sipType_sstr:
        {
            const signed char *c_value = (const signed char *)sip_bytes_as_string(value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const signed char **)addr = c_value;

            return 0;
        }

        case sipType_ustr:
        {
            const unsigned char *c_value = (const unsigned char *)sip_bytes_as_string(value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(const unsigned char **)addr = c_value;

            return 0;
        }

        case sipType_wstr:
        {
            wchar_t *c_value = sip_string_as_wstring(&value);

            if (PyErr_Occurred())
                return -1;

            if (sip_api_keep_reference(ms, instance, v_spec->key, value) < 0)
                return -1;

            *(wchar_t **)addr = c_value;

            return 0;
        }

        case sipType_bool:
        {
            bool c_value = sip_api_convert_to_bool(value);

            if (PyErr_Occurred())
                return -1;

            *(bool *)addr = c_value;

            return 0;
        }

        case sipType_voidptr:
        case sipType_voidptr_const:
        {
            void *c_value = sip_api_convert_to_void_ptr(value);

            if (PyErr_Occurred())
                return -1;

            *(void **)addr = c_value;

            return 0;
        }

        case sipType_pyobject:
        case sipType_pytuple:
        case sipType_pylist:
        case sipType_pydict:
        case sipType_pycallable:
        case sipType_pyslice:
        case sipType_pytype:
        case sipType_pybuffer:
        {
            /*
             * Note that this is the historical behaviour and is probably
             * inconsistent with what the parsers do.
             */
            Py_INCREF(value);

            Py_XDECREF(*(PyObject **)addr);
            *(PyObject **)addr = value;

            return 0;
        }
    }

    if (sipTypeIsEnum(v_spec->type_id))
        return sip_enum_convert_to_enum(ms, value, addr, v_spec->type_id,
                FALSE);

    int is_err = 0, state;
    void *user_state = NULL;
    void *cpp = sip_api_force_convert_to_type_us(ms, value, v_spec->type_id,
            NULL, 0, &state, &user_state, &is_err);

    if (is_err)
        return -1;

    /*
     * If there is no key then the value is a pointer to a wrapped type.  In
     * which case just save the pointer.
     */
    if (v_spec->key == 0)
    {
        /*
         * Note that any user state will leak.  Types that require user states
         * aren't good candidates for attributes.
         */
        *(void **)addr = cpp;
        return 0;
    }

    /* Copy the value. */
    int rc = sip_copy_wrapped_type(ms, v_spec->type_id, addr, cpp);
    sip_api_release_type_us(ms, cpp, v_spec->type_id, state, user_state);

    return rc;
}


/*
 * Get a strong reference to an object that has been kept as an extra
 * reference.  Return -1 on error, 0 if there was no object and 1 if there was
 * an object.
 */
static int get_extra_reference(sipModuleState *ms, PyObject *instance, int key,
        PyObject **ref_p)
{
    /* Get the relevant extra references dict if there is one. */
    PyObject *extra_refs = (instance != NULL ?
            ((sipSimpleWrapper *)instance)->extra_refs :
            ms->extra_refs);

    if (extra_refs == NULL)
    {
        *ref_p = NULL;
        return 0;
    }

    PyObject *key_obj = PyLong_FromLong(key);
    if (key_obj == NULL)
        return -1;

    int rc = PyDict_GetItemRef(extra_refs, key_obj, ref_p);
    Py_DECREF(key_obj);

    return rc;
}


/*
 * Return the C/C++ address of a possible instance.
 */
static int get_optional_instance_address(const sipAttrSpec *attr_spec,
        PyObject *instance, PyTypeObject *binding_type, void **addr_p)
{
    if (sipAttrIsStaticVariable(attr_spec))
        *addr_p = NULL;
    else if ((*addr_p = get_instance_address(attr_spec, instance, binding_type)) == NULL)
        return -1;

    return 0;
}


/*
 * Return the C/C++ address of an instance.
 */
static void *get_instance_address(const sipAttrSpec *attr_spec,
        PyObject *instance, PyTypeObject *binding_type)
{
    assert(sipAttrIsInstanceVariable(attr_spec));

    if (instance == NULL || instance == Py_None)
    {
        PyErr_Format(PyExc_AttributeError, "%s.%s is an instance attribute",
                ((PyTypeObject *)binding_type)->tp_name,
                sipAttrGetName(attr_spec));
        return NULL;
    }

    return sip_get_cpp_ptr(instance, binding_type);
}


/*
 * Return the C/C++ address of a variable.
 */
static void *get_variable_address(const sipAttrSpec *attr_spec,
        PyObject *instance, PyTypeObject *binding_type)
{
    const sipVariableSpec *v_spec = attr_spec->spec.variable;

    if (sipAttrIsStaticVariable(attr_spec))
        return v_spec->value.ptr_t;

    void *instance_addr = get_instance_address(attr_spec, instance,
            binding_type);
    if (instance_addr == NULL)
        return NULL;

    return ((sipVariableAddrGetFunc)v_spec->value.ptr_t)(instance_addr);
}
