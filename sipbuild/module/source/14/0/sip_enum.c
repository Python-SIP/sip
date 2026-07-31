/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file implements the enum support for all styles of enum.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_enum.h"

#include "sip.h"
#include "sip_core.h"
#include "sip_int_convertors.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"


#if defined(SIP_CONFIGURATION_CustomEnums)

/* Forward declarations of slot functions. */
static PyObject *EnumType_getattro(PyObject *self, PyObject *name);
static int EnumType_traverse(PyObject *self, visitproc visit, void *arg);


/*
 * The type slots.  We inherit everything from the standard Python metatype and
 * the size of the type object created is increased to accomodate the extra
 * information we associate with a named enum type.
 */
static PySlot EnumType_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".enumtype"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (sipEnumTypeImpl)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_IMMUTABLETYPE |
            Py_TPFLAGS_HAVE_GC |
            Py_TPFLAGS_TYPE_SUBCLASS),
    PySlot_FUNC(Py_tp_getattro, EnumType_getattro),
    PySlot_FUNC(Py_tp_traverse, EnumType_traverse),
    PySlot_END
};


/*
 * The custom enum type's getattro slot.
 */
static PyObject *EnumType_getattro(PyObject *self, PyObject *name)
{
    /*
     * Try a generic lookup first.  This has the side effect of checking the
     * type of the name object.
     */
    PyObject *res = PyObject_GenericGetAttr(self, name);
    if (res != NULL)
        return res;

    if (!PyErr_ExceptionMatches(PyExc_AttributeError))
        return NULL;

    PyErr_Clear();

    /* Get the member name. */
    const char *name_s = PyUnicode_AsUTF8(name);
    if (name_s == NULL)
        return NULL;

    const sipEnumTypeSpec *ets = ((sipEnumTypeImpl *)self)->spec;
    const sipEnumMemberSpec *member = ets->members;

    while (member->name != NULL)
    {
        if (strcmp(member->name, name_s) == 0)
            return PyObject_CallFunction(self, "(i)", member->value.int_t);

        member++;
    }

    PyErr_Format(PyExc_AttributeError, "enum %N has no member '%s'",
            (PyTypeObject *)self, name_s);

    return NULL;
}


/*
 * The custom enum type's traverse slot.
 */
static int EnumType_traverse(PyObject *self, visitproc visit, void *arg)
{
    Py_VISIT(Py_TYPE(self));

    return 0;
}

#endif


/* Forward declarations. */
static int add_operator_attrs(sipModuleState *ms, PyObject *enum_obj,
        const sipEnumTypeSpec *ets);
#if defined(SIP_CONFIGURATION_CustomEnums)
static PyTypeObject *create_custom_enum_type(sipModuleState *ms,
        const sipEnumTypeSpec *ets, PyObject *name);
#endif
static PyObject *create_dict_with_module(sipModuleState *ms,
        const char *key_name);
static PyTypeObject *create_py_enum_type(sipModuleState *ms,
        const sipEnumTypeSpec *ets, PyObject *name);
static void enum_expected(PyObject *obj, PyObject *py_type);
static int init_enum_module_types(sipSipModuleState *sms);
#if defined(SIP_CONFIGURATION_PyEnums)
static PyObject *get_builtin_type(PyObject *builtins, const char *name);
static PyObject *missing(PyObject *cls, PyObject *value, int int_enum);
static PyObject *missing_enum(PyObject *cls, PyObject *value);
static PyObject *missing_int_enum(PyObject *cls, PyObject *value);
#endif
static void raise_internal_error(sipTypeID type_id);


#if defined(SIP_CONFIGURATION_PyEnums)
/*
 * Return a non-zero value if an object is a sub-class of enum.Flag.
 */
int sip_api_is_enum_flag(sipModuleState *ms, PyObject *obj)
{
    return (PyObject_IsSubclass(obj, ms->sip_module_state->enum_flag_type) == 1);
}
#endif


/*
 * Create the Python type object for a wrapped enum.
 */
PyTypeObject *sip_create_enum_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipEnumTypeSpec *ets)
{
    /* Get the enum types if we haven't done so already. */
    if (init_enum_module_types(ms->sip_module_state) < 0)
        return NULL;

    PyObject *name = sip_get_type_name(&ets->base);
    if (name == NULL)
        return NULL;

    PyTypeObject *enum_type;

#if defined(SIP_CONFIGURATION_PyEnums)
    enum_type = create_py_enum_type(ms, ets, name);
#endif

#if defined(SIP_CONFIGURATION_CustomEnums)
    if (sipTypeSpecIsScopedEnum((const sipTypeSpec *)ets))
        enum_type = create_py_enum_type(ms, ets, name);
    else
        enum_type = create_custom_enum_type(ms, ets, name);
#endif

    Py_DECREF(name);

    if (enum_type == NULL)
        return NULL;

    if (sip_set_qualname(ms, (const sipTypeSpec *)ets, (PyObject *)enum_type) < 0)
    {
        Py_DECREF(enum_type);
        return NULL;
    }

    return enum_type;
}


/*
 * Create a Python object for a member of a named enum.
 */
PyObject *sip_api_convert_from_enum(sipModuleState *ms, void *addr,
        sipTypeID type_id)
{
    assert(sipTypeIsEnum(type_id));

    const sipEnumTypeSpec *ets;
    PyObject *py_type;

    PyObject *def_mod = sip_get_type_detail(ms, type_id,
            (const sipTypeSpec **)&ets, (PyTypeObject **)&py_type);
    if (def_mod == NULL)
        return NULL;

    PyObject *res;

    switch (ets->cpp_base_type)
    {
        case sipType_byte:
            res = PyObject_CallFunction(py_type, "(b)", *(char *)addr);
            break;

        case sipType_sbyte:
            res = PyObject_CallFunction(py_type, "(b)", *(signed char *)addr);
            break;

        case sipType_ubyte:
            res = PyObject_CallFunction(py_type, "(B)",
                    *(unsigned char *)addr);
            break;

        case sipType_short:
            res = PyObject_CallFunction(py_type, "(h)", *(short *)addr);
            break;

        case sipType_ushort:
            res = PyObject_CallFunction(py_type, "(H)",
                    *(unsigned short *)addr);
            break;

        case sipType_int:
            res = PyObject_CallFunction(py_type, "(i)", *(int *)addr);
            break;

        case sipType_uint:
            res = PyObject_CallFunction(py_type, "(I)", *(unsigned *)addr);
            break;

        case sipType_long:
            res = PyObject_CallFunction(py_type, "(l)", *(long *)addr);
            break;

        case sipType_ulong:
            res = PyObject_CallFunction(py_type, "(k)",
                    *(unsigned long *)addr);
            break;

        case sipType_longlong:
            res = PyObject_CallFunction(py_type, "(L)", *(long long *)addr);
            break;

        case sipType_ulonglong:
            res = PyObject_CallFunction(py_type, "(K)",
                    *(unsigned long long *)addr);
            break;

        default:
            raise_internal_error(ets->cpp_base_type);
            res = NULL;
    }

    Py_DECREF(def_mod);

    return res;
}


/*
 * Implement the conversion from a Python object implementing an enum to a
 * member value.
 */
int sip_enum_convert_to_enum(sipModuleState *ms, PyObject *obj, void *addr,
        sipTypeID type_id, int unconstrained)
{
    assert(sipTypeIsEnum(type_id));

    const sipEnumTypeSpec *ets;
    PyObject *py_type;

    PyObject *def_mod = sip_get_type_detail(ms, type_id,
            (const sipTypeSpec **)&ets, (PyTypeObject **)&py_type);
    if (def_mod == NULL)
        return -1;

#if defined(SIP_CONFIGURATION_CustomEnums)
    if (sipTypeSpecIsEnum((const sipTypeSpec *)ets))
    {
        int val;

        if (PyObject_TypeCheck((PyObject *)Py_TYPE(obj), ms->sip_module_state->custom_enum_type))
        {
            if (!PyObject_TypeCheck(obj, (PyTypeObject *)py_type))
            {
                enum_expected(obj, py_type);
                goto gc_def_mod;
            }

            /* This will never overflow. */
            val = sip_api_long_as_int(obj);
        }
        else if (unconstrained && PyLong_Check(obj))
        {
            val = sip_api_long_as_int(obj);
        }
        else
        {
            enum_expected(obj, py_type);
            goto gc_def_mod;
        }

        *(int *)addr = val;

        Py_DECREF(def_mod);

        return 0;
    }
#endif

    if (PyObject_IsInstance(obj, py_type) <= 0)
    {
        enum_expected(obj, py_type);
        goto gc_def_mod;
    }

    /* Get the value from the object. */
    PyObject *value_s = PyUnicode_InternFromString("value");

    if (value_s == NULL)
        goto gc_def_mod;

    PyObject *val_obj = PyObject_GetAttr(obj, value_s);
    Py_DECREF(value_s);

    if (val_obj == NULL)
        goto gc_def_mod;

    /* Convert the value. */
    PyErr_Clear();

    switch (ets->cpp_base_type)
    {
        case sipType_byte:
            *(char *)addr = sip_api_long_as_char(val_obj);
            break;

        case sipType_sbyte:
            *(signed char *)addr = sip_api_long_as_signed_char(val_obj);
            break;

        case sipType_ubyte:
            *(unsigned char *)addr = sip_api_long_as_unsigned_char(val_obj);
            break;

        case sipType_short:
            *(short *)addr = sip_api_long_as_short(val_obj);
            break;

        case sipType_ushort:
            *(unsigned short *)addr = sip_api_long_as_unsigned_short(val_obj);
            break;

        case sipType_int:
            *(int *)addr = sip_api_long_as_int(val_obj);
            break;

        case sipType_uint:
            *(unsigned *)addr = sip_api_long_as_unsigned_int(val_obj);
            break;

        case sipType_long:
            *(long *)addr = sip_api_long_as_long(val_obj);
            break;

        case sipType_ulong:
            *(unsigned long *)addr = sip_api_long_as_unsigned_long(val_obj);
            break;

        case sipType_longlong:
            *(long long *)addr = sip_api_long_as_long_long(val_obj);
            break;

        case sipType_ulonglong:
            *(unsigned long long *)addr = sip_api_long_as_unsigned_long_long(
                    val_obj);
            break;

        default:
            raise_internal_error(ets->cpp_base_type);
    }

    Py_DECREF(val_obj);
    Py_DECREF(def_mod);

    return PyErr_Occurred() ? -1 : 0;

gc_def_mod:
    Py_DECREF(def_mod);
    return -1;
}


/*
 * Initialise the enum support.  A negative value is returned (and an exception
 * set) if there was an error.
 */
int sip_enum_init(PyObject *mod, sipSipModuleState *sms)
{
#if defined(SIP_CONFIGURATION_PyEnums)
    sms->builtin_int_type = NULL;
    sms->builtin_object_type = NULL;
#endif

    sms->enum_enum_type = NULL;
    sms->enum_int_enum_type = NULL;
#if defined(SIP_CONFIGURATION_PyEnums)
    sms->enum_flag_type = NULL;
    sms->enum_int_flag_type = NULL;
#endif

#if defined(SIP_CONFIGURATION_CustomEnums)
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, EnumType_slots),
        PySlot_DATA(Py_tp_module, mod),
        PySlot_DATA(Py_tp_bases, &PyType_Type),
        PySlot_END
    };

    sms->custom_enum_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->custom_enum_type == NULL)
        return -1;
#endif

    return 0;
}


/*
 * Return a non-zero value if an object is a sub-class of enum.Enum.
 */
int sip_enum_is_enum(sipSipModuleState *sms, PyObject *obj)
{
    if (init_enum_module_types(sms) < 0)
        return 0;

    return (PyObject_IsSubclass(obj, sms->enum_enum_type) == 1);
}


#if defined(SIP_CONFIGURATION_CustomEnums)
/*
 * Create a custom enum type.
 */
static PyTypeObject *create_custom_enum_type(sipModuleState *ms,
        const sipEnumTypeSpec *ets, PyObject *name)
{
    PyObject *bases = PyTuple_Pack(1, (PyObject *)&PyLong_Type);
    if (bases == NULL)
        return NULL;

    PyObject *dict = create_dict_with_module(ms, "__module__");
    if (dict == NULL)
    {
        Py_DECREF(bases);
        return NULL;
    }

    PyObject *args = PyTuple_Pack(3, name, bases, dict);

    Py_DECREF(bases);
    Py_DECREF(dict);

    if (args == NULL)
        return NULL;

    PyObject *enum_obj = PyObject_Call(
            (PyObject *)ms->sip_module_state->custom_enum_type, args, NULL);

    Py_DECREF(args);

    if (enum_obj == NULL)
        return NULL;

    ((sipEnumTypeImpl *)enum_obj)->spec = ets;

    if (add_operator_attrs(ms, enum_obj, ets) < 0)
    {
        Py_DECREF(enum_obj);
        return NULL;
    }

    return (PyTypeObject *)enum_obj;
}
#endif


/*
 * Create a Python enum type.
 */
static PyTypeObject *create_py_enum_type(sipModuleState *ms,
        const sipEnumTypeSpec *ets, PyObject *name)
{
    sipSipModuleState *sms = ms->sip_module_state;

    /* Create a dict of the members. */
    PyObject *members = PyDict_New();
    if (members == NULL)
        goto ret_err;

    const sipEnumMemberSpec *member = ets->members;

    while (member->name != NULL)
    {
        PyObject *value_obj;

        switch (ets->cpp_base_type)
        {
            case sipType_bool:
                value_obj = PyBool_FromLong(member->value.bool_t);
                break;

            case sipType_byte:
                value_obj = PyLong_FromLong(member->value.byte_t);
                break;

            case sipType_sbyte:
                value_obj = PyLong_FromLong(member->value.sbyte_t);
                break;

            case sipType_ubyte:
                value_obj = PyLong_FromUnsignedLong(member->value.ubyte_t);
                break;

            case sipType_short:
                value_obj = PyLong_FromLong(member->value.short_t);
                break;

            case sipType_ushort:
                value_obj = PyLong_FromUnsignedLong(member->value.ushort_t);
                break;

            case sipType_int:
                value_obj = PyLong_FromLong(member->value.int_t);
                break;

            case sipType_uint:
                value_obj = PyLong_FromUnsignedLong(member->value.uint_t);
                break;

            case sipType_long:
                value_obj = PyLong_FromLong(member->value.long_t);
                break;

            case sipType_ulong:
                value_obj = PyLong_FromUnsignedLong(member->value.ulong_t);
                break;

            case sipType_longlong:
                value_obj = PyLong_FromLongLong(member->value.longlong_t);
                break;

            case sipType_ulonglong:
                value_obj = PyLong_FromUnsignedLongLong(
                        member->value.ulonglong_t);
                break;

            default:
                raise_internal_error(ets->cpp_base_type);
                value_obj = NULL;
        }

        if (sip_dict_set_and_discard(members, member->name, value_obj) < 0)
            goto rel_members;

        member++;
    }

    PyObject *args = PyTuple_Pack(2, name, members);
    if (args == NULL)
        goto rel_members;

    PyObject *kw_args = create_dict_with_module(ms, "module");
    if (kw_args == NULL)
        goto rel_args;

    PyObject *enum_factory;

#if defined(SIP_CONFIGURATION_PyEnums)
    PyMethodDef *missing_md = NULL;

    if (ets->py_base_type == SIP_ENUM_INT_FLAG)
    {
        enum_factory = sms->enum_int_flag_type;
    }
    else if (ets->py_base_type == SIP_ENUM_FLAG)
    {
        enum_factory = sms->enum_flag_type;
    }
    else if (ets->py_base_type == SIP_ENUM_INT_ENUM || ets->py_base_type == SIP_ENUM_UINT_ENUM)
    {
        static PyMethodDef missing_int_enum_md = {
            "_missing_", missing_int_enum, METH_O|METH_CLASS, NULL
        };

        enum_factory = sms->enum_int_enum_type;
        missing_md = &missing_int_enum_md;
    }
    else
    {
        static PyMethodDef missing_enum_md = {
            "_missing_", missing_enum, METH_O|METH_CLASS, NULL
        };

        enum_factory = sms->enum_enum_type;
        missing_md = &missing_enum_md;
    }
#endif

#if defined(SIP_CONFIGURATION_CustomEnums)
    enum_factory = sms->enum_int_enum_type;
#endif

    PyObject *enum_obj = PyObject_Call(enum_factory, args, kw_args);
    if (enum_obj == NULL)
        goto rel_kw_args;

    Py_DECREF(kw_args);
    Py_DECREF(args);
    Py_DECREF(members);

#if defined(SIP_CONFIGURATION_PyEnums)
    /* Inject _missing_. */
    if (missing_md != NULL)
    {
        PyObject *missing_cfunc;

        if ((missing_cfunc = PyCFunction_New(missing_md, enum_obj)) == NULL)
        {
            Py_DECREF(enum_obj);
            return NULL;
        }

        PyObject *sunder_missing = PyUnicode_InternFromString("_missing_");

        if (sunder_missing == NULL)
        {
            Py_DECREF(missing_cfunc);
            Py_DECREF(enum_obj);
            return NULL;
        }

        int rc = PyObject_SetAttr(enum_obj, sunder_missing, missing_cfunc);
        Py_DECREF(sunder_missing);
        Py_DECREF(missing_cfunc);

        if (rc < 0)
        {
            Py_DECREF(enum_obj);
            return NULL;
        }
    }

    if (add_operator_attrs(ms, enum_obj, ets) < 0)
    {
        Py_DECREF(enum_obj);
        return NULL;
    }
#endif

    return (PyTypeObject *)enum_obj;

    /* Unwind on errors. */

rel_kw_args:
    Py_DECREF(kw_args);

rel_args:
    Py_DECREF(args);

rel_members:
    Py_DECREF(members);

ret_err:
    return NULL;
}


/*
 * Add any Python slots that implement C++ operators to an enum type.
 */
static int add_operator_attrs(sipModuleState *ms, PyObject *enum_obj,
        const sipEnumTypeSpec *ets)
{
    return sip_add_attrs_to_type(ms, (PyTypeObject *)enum_obj, ets->attrs,
            &ets->base);
}


/*
 * Create a dict containing the module name using a configurable key name.
 */
static PyObject *create_dict_with_module(sipModuleState *ms,
        const char *key_name)
{
    PyObject *dict = PyDict_New();
    if (dict == NULL)
        return NULL;

    PyObject *key_obj = PyUnicode_InternFromString(key_name);
    if (key_obj == NULL)
        goto gc_dict;

    PyObject *module_name = PyModule_GetNameObject(ms->wrapped_module);
    if (module_name == NULL)
    {
        Py_DECREF(key_obj);
        goto gc_dict;
    }

    int rc = PyDict_SetItem(dict, key_obj, module_name);
    Py_DECREF(key_obj);
    Py_DECREF(module_name);

    if (rc < 0)
        goto gc_dict;

    return dict;

gc_dict:
    Py_DECREF(dict);
    return NULL;
}


/*
 * Raise an exception when failing to convert an enum because of its type.
 */
static void enum_expected(PyObject *obj, PyObject *py_type)
{
    PyErr_Format(PyExc_TypeError, "a member of enum %N is expected not %T",
            (PyTypeObject *)py_type, obj);
}


/*
 * Initialise the required types from the standard library enum module.  Return
 * a negative value and raise an exception if there is an error.
 */
static int init_enum_module_types(sipSipModuleState *sms)
{
    /* Check if it has already been done. */
    if (sms->enum_enum_type != NULL)
        return 0;

#if defined(SIP_CONFIGURATION_PyEnums)
    /* Get the builtin types. */
    PyObject *builtins = PyEval_GetFrameBuiltins();
    if (builtins == NULL)
        return -1;

    sms->builtin_int_type = get_builtin_type(builtins, "int");
    sms->builtin_object_type = get_builtin_type(builtins, "object");

    Py_DECREF(builtins);

    if (sms->builtin_int_type == NULL || sms->builtin_object_type == NULL)
    {
        Py_CLEAR(sms->builtin_int_type);
        Py_CLEAR(sms->builtin_object_type);

        return -1;
    }
#endif

    /* Get the enum types. */
    PyObject *enum_module = PyImport_ImportModule("enum");
    if (enum_module == NULL)
        return -1;

    sms->enum_enum_type = PyObject_GetAttrString(enum_module, "Enum");
    sms->enum_int_enum_type = PyObject_GetAttrString(enum_module, "IntEnum");
#if defined(SIP_CONFIGURATION_PyEnums)
    sms->enum_flag_type = PyObject_GetAttrString(enum_module, "Flag");
    sms->enum_int_flag_type = PyObject_GetAttrString(enum_module, "IntFlag");
#endif

    Py_DECREF(enum_module);

    if (sms->enum_enum_type == NULL || sms->enum_int_enum_type == NULL
#if defined(SIP_CONFIGURATION_PyEnums)
        || sms->enum_flag_type == NULL || sms->enum_int_flag_type == NULL
#endif
        )
    {
        Py_CLEAR(sms->enum_enum_type);
        Py_CLEAR(sms->enum_int_enum_type);
#if defined(SIP_CONFIGURATION_PyEnums)
        Py_CLEAR(sms->enum_flag_type);
        Py_CLEAR(sms->enum_int_flag_type);
#endif

        return -1;
    }

    return 0;
}


#if defined(SIP_CONFIGURATION_PyEnums)
/*
 * Return a string reference to a named builtin type.
 */
static PyObject *get_builtin_type(PyObject *builtins, const char *name)
{
    PyObject *type;
    int rc = PyDict_GetItemStringRef(builtins, name, &type);
    if (rc > 0)
        return type;

    /* Make sure there is an exception. */
    if (rc == 0)
        PyErr_Format(PyExc_SystemError, "unable to find builtin %s type",
                name);

    return NULL;
}


/*
 * The bulk of the implementation of _missing_ that handles missing members.
 */
static PyObject *missing(PyObject *cls, PyObject *value, int int_enum)
{
    sipSipModuleState *sms = sip_get_sip_module_state_from_type(
            (PyTypeObject *)cls);
    PyObject *sip_missing, *member, *value_str;
    int rc;

    PyObject *sunder_sip_missing = PyUnicode_InternFromString("_sip_missing_");

    if (sunder_sip_missing == NULL)
        return NULL;

    /* Get the dict of previously missing members. */
    if ((sip_missing = PyObject_GetAttr(cls, sunder_sip_missing)) != NULL)
    {
        Py_DECREF(sunder_sip_missing);

        rc = PyDict_GetItemRef(sip_missing, value, &member);

        if (member != NULL)
        {
            Py_DECREF(sip_missing);

            /* Return the already missing member. */
            return member;
        }

        if (rc < 0)
        {
            Py_DECREF(sip_missing);
            return NULL;
        }
    }
    else if (PyErr_ExceptionMatches(PyExc_AttributeError))
    {
        PyErr_Clear();

        /* Create the dict and save it in the class. */
        if ((sip_missing = PyDict_New()) == NULL)
        {
            Py_DECREF(sunder_sip_missing);
            return NULL;
        }

        rc = PyObject_SetAttr(cls, sunder_sip_missing, sip_missing);
        Py_DECREF(sunder_sip_missing);

        if (rc < 0)
        {
            Py_DECREF(sip_missing);
            return NULL;
        }
    }
    else
    {
        Py_DECREF(sunder_sip_missing);

        /* The exception is unexpected. */
        return NULL;
    }

    /* Create a member for the missing value. */
    PyObject *dunder_new = PyUnicode_InternFromString("__new__");

    if (dunder_new == NULL)
    {
        Py_DECREF(sip_missing);
        return NULL;
    }

    if (int_enum)
        member = PyObject_CallMethodObjArgs(sms->builtin_int_type, dunder_new, cls, value,
                NULL);
    else
        member = PyObject_CallMethodObjArgs(sms->builtin_object_type, dunder_new, cls,
                NULL);

    Py_DECREF(dunder_new);

    if (member == NULL)
    {
        Py_DECREF(sip_missing);
        return NULL;
    }

    /* Set the member's attributes. */
    if ((value_str = PyObject_Str(value)) == NULL)
    {
        Py_DECREF(member);
        Py_DECREF(sip_missing);
        return NULL;
    }

    PyObject *sunder_name = PyUnicode_InternFromString("_name_");

    if (sunder_name == NULL)
    {
        Py_DECREF(value_str);
        Py_DECREF(member);
        Py_DECREF(sip_missing);
        return NULL;
    }

    rc = PyObject_SetAttr(member, sunder_name, value_str);
    Py_DECREF(sunder_name);
    Py_DECREF(value_str);

    if (rc < 0)
    {
        Py_DECREF(member);
        Py_DECREF(sip_missing);
        return NULL;
    }

    PyObject *sunder_value = PyUnicode_InternFromString("_value_");

    if (sunder_value == NULL)
    {
        Py_DECREF(member);
        Py_DECREF(sip_missing);
    }

    rc = PyObject_SetAttr(member, sunder_value, value);
    Py_DECREF(sunder_value);

    if (rc < 0)
    {
        Py_DECREF(member);
        Py_DECREF(sip_missing);
        return NULL;
    }

    /* Save the member so that it is a singleton. */
    rc = PyDict_SetItem(sip_missing, value, member);
    Py_DECREF(sip_missing);

    if (rc < 0)
    {
        Py_DECREF(member);
        return NULL;
    }

    return member;
}


/*
 * The replacement implementation of _missing_ that handles missing members in
 * Enums.
 */
static PyObject *missing_enum(PyObject *cls, PyObject *value)
{
    PyObject *member;

    Py_BEGIN_CRITICAL_SECTION(cls);
    member = missing(cls, value, FALSE);
    Py_END_CRITICAL_SECTION();

    return member;
}


/*
 * The replacment implementation of _missing_ that handles missing members in
 * IntEnums.
 */
static PyObject *missing_int_enum(PyObject *cls, PyObject *value)
{
    PyObject *member;

    Py_BEGIN_CRITICAL_SECTION(cls);
    member = missing(cls, value, TRUE);
    Py_END_CRITICAL_SECTION();

    return member;
}
#endif


/*
 * Raise an exception relating to an invalid type ID.
 */
static void raise_internal_error(sipTypeID type_id)
{
    PyErr_Format(PyExc_SystemError, "unsupported enum type ID: 0x%04x",
            type_id);
}
