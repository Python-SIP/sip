/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file implements the API for the argument parsers support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "sip_parsers.h"

#include "sip.h"
#include "sip_array.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_int_convertors.h"
#include "sip_object_map.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_string_convertors.h"
#include "sip_voidptr.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper.h"
#include "sip_wrapper_type.h"


/*
 * These are the format flags supported by argument parsers.
 */
#define FMT_AP_DEREF            0x01    /* The pointer will be dereferenced. */
#define FMT_AP_TRANSFER         0x02    /* Implement /Transfer/. */
#define FMT_AP_TRANSFER_BACK    0x04    /* Implement /TransferBack/. */
#define FMT_AP_NO_CONVERTORS    0x08    /* Suppress any convertors. */
#define FMT_AP_TRANSFER_THIS    0x10    /* Support for /TransferThis/. */


/*
 * These are the format flags supported by result parsers.  Deprecated values
 * have a _DEPR suffix.
 */
#define FMT_RP_DEREF            0x01    /* The pointer will be dereferenced. */
#define FMT_RP_FACTORY          0x02    /* /Factory/ or /TransferBack/. */
#define FMT_RP_MAKE_COPY        0x04    /* Return a copy of the value. */
#define FMT_RP_NO_STATE_DEPR    0x04    /* Don't return the C/C++ state. */


/*
 * The different reasons for failing to parse an overload.  These include
 * internal (i.e. non-user) errors.
 */
typedef enum {
    Ok, Unbound, TooFew, TooMany, UnknownKeyword, Duplicate, WrongType, Raised,
    Exception, Overflow, Missing
} sipParseFailureReason;


/*
 * The description of a failure to parse an overload because of a user error.
 */
typedef struct {
    sipParseFailureReason reason;   /* The reason for the failure. */
    const char *type_hint;          /* The overload's type hint. */
    const char *detail_str;         /* The detail if a string. */
    PyObject *detail_obj;           /* The detail if a Python object. */
    int arg_nr;                     /* The wrong positional argument. */
    const char *arg_name;           /* The wrong keyword argument. */
    int overflow_arg_nr;            /* The overflowed positional argument. */
    const char *overflow_arg_name;  /* The overflowed keyword argument. */
} sipParseFailure;


/* Forward references. */
static void add_failure(PyObject **p_state_p, sipParseFailure *failure);
static PyObject *bad_type_str(int arg_nr, PyObject *arg);
static PyObject *build_object(sipModuleState *ms, PyObject *tup,
        const char *fmt, va_list va);
static PyObject *call_method(sipModuleState *ms, PyObject *method,
        const char *fmt, va_list va);
static int convert_from_sequence(sipModuleState *ms, PyObject *seq,
        sipTypeID type_id, void **array, Py_ssize_t *nr_elem);
static int convert_subclass(sipSipModuleState *sms, PyObject **def_mod_p,
        PyTypeObject **py_type_p, const sipTypeSpec **ts_p, void **cpp_p);
static int convert_subclass_pass(sipSipModuleState *sms, PyObject **def_mod_p,
        PyTypeObject **py_type_p, const sipTypeSpec **ts_p, void **cpp_p);
static PyObject *convert_to_sequence(sipModuleState *ms, void *array,
        Py_ssize_t nr_elem, sipTypeID type_id);
static sipSimpleWrapper *deref_mixin(sipSimpleWrapper *sw);
static const char *detail_from_failure(PyObject *failure_obj,
        PyObject **detail_p);
static void failure_dtor(PyObject *capsule);
static sipVirtErrorHandlerFunc find_error_handler(sipModuleState *ms,
        const char *error_handler);
static PyObject *get_py_object(sipSipModuleState *sms, void *cppPtr,
        PyTypeObject *w_type);
static PyObject *get_self_from_args(PyObject *const *args,
        Py_ssize_t nr_pos_args, PyTypeObject *py_type);
static void handle_failed_int_conversion(sipParseFailure *pf, PyObject *arg);
static void handle_failed_type_conversion(sipParseFailure *pf, PyObject *arg);
static void parse_pass_1(sipModuleState *ms, PyObject *arg,
        const sipArgParserFormatSpec *fmt_spec, void **fmt_params,
        sipParseFailure *failure);
static void parse_pass_2(sipModuleState *ms, PyObject *self, PyObject *arg,
        const sipArgParserFormatSpec *fmt_spec, void **fmt_params,
        sipParseFailure *failure);
static int parse_result(sipModuleState *ms, PyObject *method, PyObject *res,
        sipSimpleWrapper *py_self, const char *fmt, va_list va);
static bool parse_vc_kwd_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **unused_p,
        PyObject **self_p, sipTypeID self_type_id, void **self_cpp_p,
        const char *type_hint);
static void raise_no_convert_to(PyObject *py, const sipTypeSpec *ts);
static void transfer(sipSipModuleState *sms, PyObject *py,
        PyObject *transferObj);
static int user_state_is_valid(const sipTypeSpec *ts, void **user_statep);


/*
 * Adds the current exception to the current list of exceptions (if it is a
 * user exception) or replace the current list of exceptions.
 */
void sip_api_add_exception(sipErrorState es, PyObject **p_state_p,
        const char *type_hint)
{
    assert(*p_state_p == NULL);

    if (es == sipErrorContinue)
    {
        sipParseFailure failure = {.type_hint = type_hint};

        /* Get the value of the exception. */
        failure.detail_obj = PyErr_GetRaisedException();
        failure.reason = Exception;

        add_failure(p_state_p, &failure);

        if (failure.reason == Raised)
        {
            Py_XDECREF(failure.detail_obj);
            es = sipErrorFail;
        }
    }

    if (es == sipErrorFail)
        sip_api_set_parser_error(p_state_p);
}


/*
 * Adds a failure about an argument with an incorrect type to the current list
 * of exceptions.
 */
sipErrorState sip_api_bad_callable_arg(int arg_nr, PyObject *arg)
{
    PyObject *detail = bad_type_str(arg_nr + 1, arg);

    if (detail == NULL)
        return sipErrorFail;

    PyErr_SetObject(PyExc_TypeError, detail);
    Py_DECREF(detail);

    return sipErrorContinue;
}


/*
 * Report a Python member function with an unexpected result.
 */
void sip_api_bad_catcher_result(PyObject *method)
{
    /*
     * This is part of the public API so we make no assumptions about the
     * method object.
     */
    if (!PyMethod_Check(method) ||
        PyMethod_GET_FUNCTION(method) == NULL ||
        !PyFunction_Check(PyMethod_GET_FUNCTION(method)) ||
        PyMethod_GET_SELF(method) == NULL)
    {
        PyErr_SetString(PyExc_TypeError,
                "invalid argument to sipBadCatcherResult()");
        return;
    }

    PyObject *mname = ((PyFunctionObject *)PyMethod_GET_FUNCTION(method))->func_name;

    /*
     * Get the current exception object if there is one.  Its string
     * representation will be used as the detail of a new exception.
     */
    PyObject *exc = PyErr_GetRaisedException();

    if (exc != NULL)
    {
        PyErr_Format((PyObject *)Py_TYPE(exc),
                "invalid result from %s.%U(), %S",
                Py_TYPE(PyMethod_GET_SELF(method))->tp_name, mname, exc);
        Py_DECREF(exc);
    }
    else
    {
        PyErr_Format(PyExc_TypeError, "invalid result from %s.%U()",
                Py_TYPE(PyMethod_GET_SELF(method))->tp_name, mname);
    }
}


/*
 * Build a result object based on a format string.
 */
PyObject *sip_api_build_result(sipModuleState *ms, int *is_err_p,
        const char *fmt, ...)
{
    va_list va;

    va_start(va,fmt);

    /* Basic validation of the format string. */

    int badfmt = FALSE;
    PyObject *res = NULL;
    Py_ssize_t tupsz;

    if (*fmt == '(')
    {
        char *ep;

        if ((ep = strchr(fmt,')')) == NULL || ep[1] != '\0')
            badfmt = TRUE;
        else
            tupsz = (Py_ssize_t)(ep - fmt - 1);
    }
    else if (strlen(fmt) == 1)
    {
        tupsz = -1;
    }
    else
    {
        badfmt = TRUE;
    }

    if (badfmt)
    {
        PyErr_Format(PyExc_SystemError,
                "sipBuildResult(): invalid format string \"%s\"",fmt);
    }
    else if (tupsz < 0 || (res = PyTuple_New(tupsz)) != NULL)
    {
        res = build_object(ms, res, fmt, va);
    }

    va_end(va);

    if (res == NULL && is_err_p != NULL)
        *is_err_p = TRUE;

    return res;
}


/*
 * Call a virtual error handler.  This is called with the GIL and from the
 * thread that raised the error.
 */
void sip_api_call_error_handler(sipModuleState *ms, const char *error_handler,
        sipSimpleWrapper *sw, PyThreadStateToken *tst)
{
    sipModuleState *handler_ms;
    sipVirtErrorHandlerFunc handler;

    if (error_handler == NULL)
    {
        handler = NULL;
    }
    else
    {
        /*
         * In the legacy ABI versions a handler defined in the current module
         * will always be used in preference to a handler with the same name
         * defined in another module.  We check the current module for the
         * handler before checking other modules to mimic this behaviour.
         */
        handler = find_error_handler(ms, error_handler);

        if (handler != NULL)
        {
            handler_ms = ms;
        }
        else
        {
            PyObject *module_list = ms->sip_module_state->module_list;
            Py_ssize_t i;

            for (i = 0; i < PyList_GET_SIZE(module_list); i++)
            {
                PyObject *mod;

                if (PyWeakref_GetRef(PyList_GET_ITEM(module_list, i), &mod) < 0)
                    break;

                if (mod == NULL || mod == ms->wrapped_module)
                    continue;

                handler_ms = sip_get_module_state(mod);
                handler = find_error_handler(handler_ms, error_handler);

                if (handler != NULL)
                    break;
            }
        }
    }

    if (handler != NULL)
    {
        handler(handler_ms, deref_mixin(sw), tst);
    }
    else
    {
        /*
         * Note that PyErr_WriteUnraisable() would be more appropriate but any
         * advantages aren't enough to break backwards compatibility.
         */
        PyErr_Print();
    }
}


/*
 * If an error handler is defined in a module then return it.
 */
static sipVirtErrorHandlerFunc find_error_handler(sipModuleState *ms,
        const char *error_handler)
{
    const sipVirtErrorHandlerSpec *veh = ms->module_spec->virt_error_handlers;
    if (veh == NULL)
        return NULL;

    while (veh->name != NULL)
    {
        if (strcmp(veh->name, error_handler) == 0)
            return veh->handler;

        veh++;
    }

    return NULL;
}


/*
 * Call the Python re-implementation of a C++ virtual.
 */
PyObject *sip_api_call_method(sipModuleState *ms, int *is_err_p,
        PyObject *method, const char *fmt, ...)
{
    PyObject *res;
    va_list va;

    va_start(va, fmt);
    res = call_method(ms, method, fmt, va);
    va_end(va);

    if (res == NULL && is_err_p != NULL)
        *is_err_p = TRUE;

    return res;
}


/*
 * Call the Python re-implementation of a C++ virtual that does not return a
 * value and handle the result.
 */
void sip_api_call_procedure_method(sipModuleState *ms,
        PyThreadStateToken *tst, const char *error_handler,
        sipSimpleWrapper *py_self, PyObject *method, const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    PyObject *res = call_method(ms, method, fmt, va);
    va_end(va);

    if (res != NULL)
    {
        Py_DECREF(res);

        if (res != Py_None)
        {
            sip_api_bad_catcher_result(method);
            res = NULL;
        }
    }

    Py_DECREF(method);

    if (res == NULL)
        sip_api_call_error_handler(ms, error_handler, py_self, tst);

    PyThreadState_Release(tst);
}


/*
 * Convert a new C/C++ instance to a Python instance of a specific Python type.
 */
PyObject *sip_api_convert_from_new_py_type(sipModuleState *ms, void *cpp,
        PyTypeObject *py_type, sipWrapper *owner, sipSimpleWrapper **self_p,
        const char *fmt, ...)
{
    PyObject *args, *res;
    va_list va;

    va_start(va, fmt);

    if ((args = PyTuple_New(strlen(fmt))) != NULL && build_object(ms, args, fmt, va) != NULL)
    {
        /*
         * The type of 'owner' should be 'PyObject *' to be consistent with the
         * type of transfer objects.  We stick with the bad design to support
         * legacy code.
         */
        res = sip_wrap_instance(ms, cpp, py_type, args, (PyObject *)owner,
                (self_p != NULL ? SIP_DERIVED_CLASS : 0));

        /* Initialise the rest of an instance of a derived class. */
        if (self_p != NULL)
            *self_p = (sipSimpleWrapper *)res;
    }
    else
    {
        res = NULL;
    }

    Py_XDECREF(args);

    va_end(va);

    return res;
}


/*
 * Return a strong reference to the object that wraps a C/C++ pointer.
 */
PyObject *sip_api_get_py_object_ref(sipModuleState *ms, void *cppPtr,
        sipTypeID type_id)
{
    PyObject *def_mod;
    PyTypeObject *py_type = sip_get_py_type(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return NULL;

    PyObject *obj = get_py_object(ms->sip_module_state, cppPtr, py_type);
    Py_DECREF(def_mod);

    return obj;
}


/*
 * Report a callable with invalid argument types.
 */
void sip_no_callable(PyObject *p_state, const char *scope, const char *name)
{
    const char *sep = ".";

    if (scope == NULL)
        scope = ++sep;

    if (p_state == NULL)
    {
        /*
         * If we have got this far without trying a parse then there must be no
         * overloads.
         */
        PyErr_Format(PyExc_TypeError, "%s%s%s() has no non-private overloads",
                scope, sep, name);
        return;
    }

    if (p_state != Py_None)
    {
        assert(PyList_Check(p_state));

        PyObject *exc;

        /* There is an entry for each overload that was tried. */
        if (PyList_GET_SIZE(p_state) == 1)
        {
            PyObject *detail;
            const char *type_hint = detail_from_failure(
                    PyList_GET_ITEM(p_state, 0), &detail);

            if (detail != NULL)
            {
                if (type_hint != NULL)
                    exc = PyUnicode_FromFormat("%s%s%s: %U", scope, sep,
                            type_hint, detail);
                else
                    exc = PyUnicode_FromFormat("%s%s%s(): %U", scope, sep,
                            name, detail);

                Py_DECREF(detail);
            }
            else
            {
                exc = NULL;
            }
        }
        else
        {
            static const char *summary = "arguments did not match any overloaded call:";

            exc = PyUnicode_FromFormat("%s%s%s(): %s", scope, sep, name,
                    summary);

            Py_ssize_t i;

            for (i = 0; i < PyList_GET_SIZE(p_state); ++i)
            {
                PyObject *detail, *failure;
                const char *type_hint = detail_from_failure(
                            PyList_GET_ITEM(p_state, i), &detail);

                if (detail != NULL)
                {
                    if (type_hint != NULL)
                        failure = PyUnicode_FromFormat("\n  %s: %U", type_hint,
                                detail);
                    else
                        failure = PyUnicode_FromFormat("\n  overload %zd: %U",
                                i + 1, detail);

                    Py_DECREF(detail);

                    if (failure != NULL)
                        PyUnicode_AppendAndDel(&exc, failure);
                }
                else
                {
                    failure = NULL;
                }

                if (failure == NULL)
                {
                    Py_XDECREF(exc);
                    exc = NULL;
                    break;
                }
            }
        }

        if (exc != NULL)
        {
            PyErr_SetObject(PyExc_TypeError, exc);
            Py_DECREF(exc);
        }
    }

    Py_DECREF(p_state);
}


/*
 * Parse a result object based on a format string.  This will consume the
 * reference to the Python re-implementation and release the current thread
 * state.
 */
int sip_api_parse_result(sipModuleState *ms, PyThreadStateToken *tst,
        const char *error_handler, sipSimpleWrapper *sw, PyObject *method,
        PyObject *res, const char *fmt, ...)
{
    int rc;

    if (res != NULL)
    {
        va_list va;

        va_start(va, fmt);
        rc = parse_result(ms, method, res, deref_mixin(sw), fmt, va);
        va_end(va);

        Py_DECREF(res);
    }
    else
    {
        rc = -1;
    }

    Py_DECREF(method);

    if (rc < 0)
        sip_api_call_error_handler(ms, error_handler, sw, tst);

    PyThreadState_Release(tst);

    return rc;
}


/*
 * sip_api_release_type_us() without user state support.
 */
void sip_api_release_type(sipModuleState *ms, void *cpp, sipTypeID type_id,
        int state)
{
    sip_api_release_type_us(ms, cpp, type_id, state, NULL);
}


/*
 * Release a possibly temporary C/C++ instance created by a type convertor.
 */
void sip_api_release_type_us(sipModuleState *ms, void *cpp, sipTypeID type_id,
        int state, void *user_state)
{
    /* See if there is something to release. */
    if (state & SIP_TEMPORARY)
    {
        PyObject *def_mod;
        const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);

        if (def_mod != NULL)
        {
            sip_release(cpp, ts, state, user_state);
            Py_DECREF(def_mod);
        }
    }
}


/*
 * Release an instance.  The type specification must be protected by the
 * caller.
 */
void sip_release(void *addr, const sipTypeSpec *ts, int state,
        void *user_state)
{
    if (sipTypeSpecIsClass(ts))
    {
        sipReleaseFunc rel = ((const sipClassTypeSpec *)ts)->release;

        /*
         * If there is no release function then it must be a C structure and we
         * can just free it.
         */
        if (rel == NULL)
            sip_api_free(addr);
        else
            rel(addr, state);
    }
    else if (sipTypeSpecIsMapped(ts))
    {
        sipReleaseUSFunc rel = ((const sipMappedTypeSpec *)ts)->release;

        if (rel != NULL)
            rel(addr, state, user_state);
    }
}


/*
 * Convert a C/C++ instance to a Python instance.
 */
PyObject *sip_api_convert_from_type(sipModuleState *ms, void *cpp,
        sipTypeID type_id, PyObject *transferObj)
{
    /* Handle None. */
    if (cpp == NULL)
        Py_RETURN_NONE;

    assert(sipTypeIsClass(type_id) || sipTypeIsMapped(type_id));

    const sipTypeSpec *ts;
    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, &ts, &py_type);
    if (def_mod == NULL)
        return NULL;

    sipSipModuleState *sms = ms->sip_module_state;

    if ((cpp = sip_get_final_address(sms, py_type, cpp)) == NULL)
        goto gc_def_mod;

    sipConvertFromFunc cfrom = sip_get_from_convertor(py_type, ts);

    if (cfrom != NULL)
    {
        Py_DECREF(py_type);
        return cfrom(ms, cpp, transferObj);
    }

    if (sipTypeSpecIsMapped(ts))
    {
        sip_raise_no_convert_from(ts);
        goto gc_def_mod;
    }

    /*
     * See if we have already wrapped it.  Invoking sub-class code can be
     * expensive so we check the cache first, even though the sub-class code
     * might perform a down-cast.
     */
    PyObject *py;

    if ((py = get_py_object(sms, cpp, py_type)) == NULL && sipTypeSpecHasSCC(ts))
    {
        void *orig_cpp = cpp;
        const sipTypeSpec *orig_ts = ts;

        /* Apply the sub-class convertor. */
        if (convert_subclass(sms, &def_mod, &py_type, &ts, &cpp) < 0)
            goto gc_def_mod;

        /*
         * If the sub-class convertor has done something then check the cache
         * again using the modified values.
         */
        if (cpp != orig_cpp || ts != orig_ts)
        {
            py = get_py_object(sms, cpp, py_type);

            /* Use the module state for the updated Python type. */
            ms = sip_get_module_state(
                    ((sipWrapperType *)py_type)->defining_module);
        }
    }

    if (py == NULL && (py = sip_wrap_instance(ms, cpp, py_type, NULL, NULL, SIP_SHARE_MAP)) == NULL)
        goto gc_def_mod;

    /* Handle any ownership transfer. */
    if (transferObj != NULL)
        transfer(sms, py, transferObj);

    Py_DECREF(def_mod);
    return py;

gc_def_mod:
    Py_DECREF(def_mod);
    return NULL;
}


/*
 * sip_api_force_convert_to_type_us() without user state support.
 */
void *sip_api_force_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        int *iserrp)
{
    return sip_api_force_convert_to_type_us(ms, pyObj, type_id, transferObj,
            flags, statep, NULL, iserrp);
}


/*
 * Convert a Python object to a C/C++ pointer and raise an exception if it
 * can't be done.
 */
void *sip_api_force_convert_to_type_us(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        void **user_statep, int *iserrp)
{
    /* Don't even try if there has already been an error. */
    if (*iserrp)
        return NULL;

    /* See if the object's type can be converted. */
    if (!sip_api_can_convert_to_type(ms, pyObj, type_id, flags))
    {
        PyObject *def_mod;
        const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);

        if (def_mod != NULL)
        {
            if (sipTypeSpecIsMapped(ts))
                raise_no_convert_to(pyObj, ts);
            else
                PyErr_Format(PyExc_TypeError, "%s cannot be converted to %s",
                        Py_TYPE(pyObj)->tp_name, ts->tp_name);

            if (statep != NULL)
                *statep = 0;

            Py_DECREF(def_mod);
        }

        *iserrp = TRUE;
        return NULL;
    }

    /* Do the conversion. */
    return sip_api_convert_to_type_us(ms, pyObj, type_id, transferObj, flags,
            statep, user_statep, iserrp);
}


/*
 * Return a strong reference to any Python reimplementation of a C/C++ virtual
 * function.  If one was found then a thread state is attached and the Python
 * API can be called.
 */
PyObject *sip_api_is_py_method(sipModuleState *ms, PyThreadStateToken **tst_p,
        char *pymc, sipSimpleWrapper **self_p, const char *cname,
        const char *mname)
{
    sipSipModuleState *sms = ms->sip_module_state;

    /*
     * This is the most common case (where there is no Python reimplementation)
     * so we take a fast shortcut.
     */
    if (*pymc != 0)
        return NULL;

    /* We might still have C++ going after the interpreter has gone. */
    if ((*tst_p = PyThreadState_EnsureFromView(sms->interpreter_view)) == NULL)
        return NULL;

    /* Only read this when we the thread state is attached. */
    sipSimpleWrapper *self = *self_p;

    /*
     * It's possible that the Python object has been deleted but the underlying
     * C++ instance is still working and trying to handle virtual functions.
     * Alternatively, an instance has started handling virtual functions before
     * its ctor has returned.  In either case say there is no Python
     * reimplementation.
     */
    if (self != NULL)
        self = deref_mixin(self);

    if (self == NULL)
        goto release;

    /*
     * It's possible that the object's type's tp_mro is NULL.  A possible
     * circumstance is when a type has been created dynamically and the only
     * reference to it is the single instance of the type which is in the
     * process of being garbage collected.
     */
    PyTypeObject *cls = Py_TYPE(self);
    PyObject *mro = cls->tp_mro;

    if (mro == NULL)
        goto release;

    /* Get any reimplementation. */

    PyObject *mname_obj = PyUnicode_FromString(mname);
    if (mname_obj == NULL)
        goto release;

    /*
     * We don't use PyObject_GetAttr() because that might find the generated
     * C function before a reimplementation defined in a mixin (ie. later in
     * the MRO).
     */
    if (self->dict != NULL)
    {
        /* Check the instance dictionary in case it has been monkey patched. */
        PyObject *reimp;
        if (PyDict_GetItemRef(self->dict, mname_obj, &reimp) < 0)
        {
            Py_DECREF(mname_obj);
            goto release;
        }

        if (reimp != NULL)
        {
            if (PyCallable_Check(reimp))
            {
                Py_DECREF(mname_obj);
                return reimp;
            }

            Py_DECREF(reimp);
        }
    }

    assert(PyTuple_Check(mro));

    PyObject *reimp = NULL;
    Py_ssize_t i;

    for (i = 0; i < PyTuple_GET_SIZE(mro); ++i)
    {
        cls = (PyTypeObject *)PyTuple_GET_ITEM(mro, i);

        /*
         * Check any possible reimplementation is not the wrapped C++ method or
         * a default special method implementation.
         */
        if (cls->tp_dict != NULL)
        {
            PyObject *cls_attr;
            if (PyDict_GetItemRef(cls->tp_dict, mname_obj, &cls_attr) < 0)
            {
                Py_DECREF(mname_obj);
                goto release;
            }

            if (cls_attr != NULL)
            {
                PyTypeObject *attr_type = Py_TYPE(cls_attr);

                if (attr_type != sms->method_descr_type && attr_type != &PyWrapperDescr_Type)
                {
                    reimp = cls_attr;
                    break;
                }

                Py_DECREF(cls_attr);
            }
        }
    }

    Py_DECREF(mname_obj);

    if (reimp != NULL)
    {
        /*
         * Emulate the behaviour of a descriptor to make sure we return a bound
         * method.  If we don't know what it is assume that an appropriate
         * exception will be raised later on.
         */
        if (PyMethod_Check(reimp))
        {
            /* It's already a method but make sure it is bound. */
            if (PyMethod_GET_SELF(reimp) == NULL)
            {
                Py_SETREF(reimp,
                        PyMethod_New(PyMethod_GET_FUNCTION(reimp),
                        (PyObject *)self));
            }
        }
        else if (PyFunction_Check(reimp))
        {
            Py_SETREF(reimp, PyMethod_New(reimp, (PyObject *)self));
        }
        else if (Py_TYPE(reimp)->tp_descr_get)
        {
            /* It is a descriptor, so assume it will do the right thing. */
            Py_SETREF(reimp,
                    Py_TYPE(reimp)->tp_descr_get(reimp, (PyObject *)self,
                            (PyObject *)cls));
        }
    }
    else
    {
        /* Use the fast track in future. */
        *pymc = 1;

        if (cname != NULL)
        {
            /* Note that this will only be raised once per method. */
            PyErr_Format(PyExc_NotImplementedError,
                    "%s.%s() is abstract and must be overridden", cname,
                    mname);
            PyErr_Print();
        }

        PyThreadState_Release(*tst_p);
    }

    return reimp;

release:
    PyThreadState_Release(*tst_p);
    return NULL;
}


/*
 * Add a parse failure to the current list of exceptions.
 */
static void add_failure(PyObject **p_state_p, sipParseFailure *failure)
{
    sipParseFailure *failure_copy;
    PyObject *failure_obj;

    /* Create the list if necessary. */
    if (*p_state_p == NULL && (*p_state_p = PyList_New(0)) == NULL)
    {
        failure->reason = Raised;
        return;
    }

    /*
     * Make a copy of the failure, convert it to a Python object and add it to
     * the list.  We do it this way to make it as lightweight as possible.
     */
    if ((failure_copy = sip_api_malloc(sizeof (sipParseFailure))) == NULL)
    {
        failure->reason = Raised;
        return;
    }

    *failure_copy = *failure;

    if ((failure_obj = PyCapsule_New(failure_copy, NULL, failure_dtor)) == NULL)
    {
        sip_api_free(failure_copy);
        failure->reason = Raised;
        return;
    }

    /* Ownership of any detail object is now with the wrapped failure. */
    failure->detail_obj = NULL;

    if (PyList_Append(*p_state_p, failure_obj) < 0)
    {
        Py_DECREF(failure_obj);
        failure->reason = Raised;
        return;
    }

    Py_DECREF(failure_obj);
}


/*
 * Return a string as a Python object that describes an argument with an
 * unexpected type.
 */
static PyObject *bad_type_str(int arg_nr, PyObject *arg)
{
    return PyUnicode_FromFormat("argument %d has unexpected type %T", arg_nr,
            arg);
}


/*
 * Get the values off the stack and put them into an object.
 */
static PyObject *build_object(sipModuleState *ms, PyObject *obj,
        const char *fmt, va_list va)
{
    /*
     * The format string has already been checked that it is properly formed if
     * it is enclosed in parenthesis.
     */
    char term_ch;

    if (*fmt == '(')
    {
        term_ch = ')';
        ++fmt;
    }
    else
        term_ch = '\0';

    char ch;
    int i = 0;

    while ((ch = *fmt++) != term_ch)
    {
        PyObject *el;

        switch (ch)
        {
        case 'g':
            {
                char *s = va_arg(va, char *);
                Py_ssize_t l = va_arg(va, Py_ssize_t);

                if (s != NULL)
                {
                    el = PyBytes_FromStringAndSize(s, l);
                }
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'G':
            {
                wchar_t *s = va_arg(va, wchar_t *);
                Py_ssize_t l = va_arg(va, Py_ssize_t);

                if (s != NULL)
                    el = PyUnicode_FromWideChar(s, l);
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'b':
            el = PyBool_FromLong(va_arg(va, int));
            break;

        case 'c':
            {
                char c = va_arg(va, int);

                el = PyBytes_FromStringAndSize(&c, 1);
            }

            break;

        case 'a':
            {
                char c = va_arg(va, int);

                el = PyUnicode_FromStringAndSize(&c, 1);
            }

            break;

        case 'w':
            {
                wchar_t c = va_arg(va, int);

                el = PyUnicode_FromWideChar(&c, 1);
            }

            break;

        case 'F':
            {
                void *addr = va_arg(va, void *);
                sipTypeID type_id = va_arg(va, sipTypeID);

                el = sip_api_convert_from_enum(ms, addr, type_id);
            }

            break;

        case 'd':
        case 'f':
            el = PyFloat_FromDouble(va_arg(va, double));
            break;

        case 'e':
        case 'h':
        case 'i':
        case 'L':
            el = PyLong_FromLong(va_arg(va, int));
            break;

        case 'l':
            el = PyLong_FromLong(va_arg(va, long));
            break;

        case 'm':
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned long));
            break;

        case 'n':
            el = PyLong_FromLongLong(va_arg(va, long long));
            break;

        case 'o':
            el = PyLong_FromUnsignedLongLong(va_arg(va, unsigned long long));
            break;

        case 's':
            {
                char *s = va_arg(va, char *);

                if (s != NULL)
                {
                    el = PyBytes_FromString(s);
                }
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'A':
            {
                char *s = va_arg(va, char *);

                if (s != NULL)
                {
                    el = PyUnicode_FromString(s);
                }
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'x':
            {
                wchar_t *s = va_arg(va, wchar_t *);

                if (s != NULL)
                    el = PyUnicode_FromWideChar(s, (Py_ssize_t)wcslen(s));
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 't':
        case 'u':
        case 'M':
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned));
            break;

        case '=':
            el = PyLong_FromSize_t(va_arg(va, size_t));
            break;

        case 'N':
            {
                void *p = va_arg(va, void *);
                sipTypeID type_id = va_arg(va, sipTypeID);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_new_type(ms, p, type_id, xfer);
            }

            break;

        case 'D':
            {
                void *p = va_arg(va, void *);
                const sipTypeID type_id = va_arg(va, sipTypeID);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_type(ms, p, type_id, xfer);
            }

            break;

        case 'r':
            {
                void *p = va_arg(va, void *);
                Py_ssize_t l = va_arg(va, Py_ssize_t);
                sipTypeID type_id = va_arg(va, sipTypeID);

                el = convert_to_sequence(ms, p, l, type_id);
            }

            break;

        case 'R':
            el = va_arg(va, PyObject *);
            break;

        case 'S':
            el = Py_NewRef(va_arg(va, PyObject *));
            break;

        case 'V':
            el = sip_convert_from_void_ptr(ms->sip_module_state,
                    va_arg(va, void *));
            break;

        case 'z':
            {
                const char *name = va_arg(va, const char *);
                void *p = va_arg(va, void *);

                if (p == NULL)
                    el = Py_NewRef(Py_None);
                else
                    el = PyCapsule_New(p, name, NULL);
            }

            break;

        default:
            PyErr_Format(PyExc_SystemError,
                    "build_object(): invalid format character '%c'", ch);
            el = NULL;
        }

        if (el == NULL)
        {
            Py_XDECREF(obj);
            return NULL;
        }

        if (obj == NULL)
            return el;

        PyTuple_SET_ITEM(obj, i, el);
        i++;
    }

    return obj;
}


/*
 * Call a method and return the result.
 */
static PyObject *call_method(sipModuleState *ms, PyObject *method,
        const char *fmt, va_list va)
{
    PyObject *args, *res;

    if ((args = PyTuple_New(strlen(fmt))) == NULL)
        return NULL;

    if (build_object(ms, args, fmt, va) != NULL)
        res = PyObject_CallObject(method, args);
    else
        res = NULL;

    Py_DECREF(args);

    return res;
}


/*
 * Check to see if a Python object can be converted to a type.
 */
int sip_api_can_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, int flags)
{
    assert(sipTypeIsClass(type_id) || sipTypeIsMapped(type_id));

    const sipTypeSpec *ts;
    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, &ts, &py_type);
    if (def_mod == NULL)
        return FALSE;

    int ok;

    if (pyObj == Py_None)
    {
        /* If the type explicitly handles None then ignore the flags. */
        if (sipTypeSpecAllowNone(ts))
            ok = TRUE;
        else
            ok = ((flags & SIP_NOT_NONE) == 0);
    }
    else
    {
        sipConvertToFunc cto;

        if (sipTypeSpecIsClass(ts))
        {
            cto = ((const sipClassTypeSpec *)ts)->cto;

            if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
                ok = PyObject_TypeCheck(pyObj, py_type);
            else
                ok = cto(ms, pyObj, NULL, NULL, NULL, NULL);
        }
        else
        {
            if ((cto = ((const sipMappedTypeSpec *)ts)->cto) != NULL)
                ok = cto(ms, pyObj, NULL, NULL, NULL, NULL);
            else
                ok = FALSE;
        }
    }

    Py_DECREF(def_mod);

    return ok;
}


/*
 * Convert a new C/C++ instance to a Python instance.
 */
PyObject *sip_api_convert_from_new_type(sipModuleState *ms, void *cpp,
        sipTypeID type_id, PyObject *transferObj)
{
    /* Handle None. */
    if (cpp == NULL)
        Py_RETURN_NONE;

    const sipTypeSpec *ts;
    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, &ts, &py_type);
    if (def_mod == NULL)
        return NULL;

    sipSipModuleState *sms = ms->sip_module_state;

    if ((cpp = sip_get_final_address(sms, py_type, cpp)) == NULL)
        goto gc_def_mod;

    sipConvertFromFunc cfrom = sip_get_from_convertor(py_type, ts);

    if (cfrom != NULL)
    {
        PyObject *res = cfrom(ms, cpp, transferObj);

        if (res != NULL)
        {
            /*
             * We no longer need the C/C++ instance so we release it (unless
             * its ownership is transferred).  This means this call is
             * semantically equivalent to the case where we are wrapping a
             * class.
             */
            if (transferObj == NULL || transferObj == Py_None)
                sip_release(cpp, ts, 0, NULL);
        }

        Py_DECREF(def_mod);
        return res;
    }

    if (sipTypeSpecIsMapped(ts))
    {
        sip_raise_no_convert_from(ts);
        goto gc_def_mod;
    }

    /* Apply any sub-class convertor. */
    if (sipTypeSpecHasSCC(ts) && convert_subclass(sms, &def_mod, &py_type, &ts, &cpp) < 0)
        goto gc_def_mod;

    /* Handle any ownership transfer. */
    PyObject *owner;

    if (transferObj == NULL || transferObj == Py_None)
        owner = NULL;
    else
        owner = transferObj;

    PyObject *res = sip_wrap_instance(ms, cpp, py_type, NULL, owner,
            (owner == NULL ? SIP_PY_OWNED : 0));

    Py_DECREF(def_mod);
    return res;

gc_def_mod:
    Py_DECREF(def_mod);
    return NULL;
}


/*
 * Convert a Python sequence to an array that has already "passed"
 * PySequence_Check().  Return TRUE if the conversion was successful.
 */
static int convert_from_sequence(sipModuleState *ms, PyObject *seq,
        sipTypeID type_id, void **array, Py_ssize_t *nr_elem)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return FALSE;

    sipArrayFunc array_helper;
    sipAssignFunc assign_helper;

    /* Get the type's helpers. */
    if (sipTypeSpecIsMapped(ts))
    {
        array_helper = ((const sipMappedTypeSpec *)ts)->array;
        assign_helper = ((const sipMappedTypeSpec *)ts)->assign;
    }
    else
    {
        array_helper = ((const sipClassTypeSpec *)ts)->array;
        assign_helper = ((const sipClassTypeSpec *)ts)->assign;
    }

    assert(array_helper != NULL);
    assert(assign_helper != NULL);

    /*
     * Create the memory for the array of values.  Note that this will leak if
     * there is an error.
     */
    Py_ssize_t size = PySequence_Size(seq);
    assert(size >= 0);

    void *array_mem = array_helper(size);
    int iserr = 0;
    Py_ssize_t i;

    for (i = 0; i < size; ++i)
    {
        PyObject *val_obj;
        void *val;

        if ((val_obj = PySequence_GetItem(seq, i)) == NULL)
            goto gc_def_mod;

        val = sip_api_force_convert_to_type_us(ms, val_obj, type_id, NULL,
                SIP_NO_CONVERTORS|SIP_NOT_NONE, NULL, NULL, &iserr);

        Py_DECREF(val_obj);

        if (iserr)
            goto gc_def_mod;

        assign_helper(array_mem, i, val);
    }

    *array = array_mem;
    *nr_elem = size;

    Py_DECREF(def_mod);

    return TRUE;

gc_def_mod:
    Py_DECREF(def_mod);

    return FALSE;
}


/*
 * Call any sub-class convertors for a type returning an updated Python type
 * object and type definition corresponding to the sub-type, and possibly
 * modifying the C++ address (in the case of multiple inheritence).
 */
static int convert_subclass(sipSipModuleState *sms, PyObject **def_mod_p,
        PyTypeObject **py_type_p, const sipTypeSpec **ts_p, void **cpp_p)
{
    /* Handle the trivial case. */
    if (*cpp_p == NULL)
        return 0;

    /* Try the conversions until told to stop. */
    int rc;

    while ((rc = convert_subclass_pass(sms, def_mod_p, py_type_p, ts_p, cpp_p)) > 0)
        ;

    return rc;
}


/*
 * Do a single pass through the available convertors.
 */
static int convert_subclass_pass(sipSipModuleState *sms, PyObject **def_mod_p,
        PyTypeObject **py_type_p, const sipTypeSpec **ts_p, void **cpp_p)
{
    PyTypeObject *py_type = *py_type_p;

    /*
     * The order in which we go through the modules means that sub-class
     * convertors will be invoked for more specific types first.
     */
    Py_ssize_t i = PyList_GET_SIZE(sms->module_list);

    while (i-- > 0)
    {
        PyObject *mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(sms->module_list, i), &mod) < 0)
            return -1;

        if (mod == NULL)
            continue;

        sipModuleState *ms = sip_get_module_state(mod);
        const sipSubClassConvertorSpec *scc = ms->module_spec->convertors;

        if (scc == NULL)
        {
            Py_DECREF(mod);
            continue;
        }

        while (scc->convertor != NULL)
        {
            PyObject *base_mod;
            PyTypeObject *base_py_type = sip_get_py_type(ms, scc->base_id,
                    &base_mod);
            if (base_mod == NULL)
            {
                Py_DECREF(mod);
                return -1;
            }

            /*
             * The base type is the "root" class that may have a number of
             * convertors each handling a "branch" of the derived tree of
             * classes.  The "root" normally implements the base function that
             * provides the RTTI used by the convertors and is re-implemented
             * by derived classes.  We therefore see if the target type is a
             * sub-class of the root, ie. see if the convertor might be able to
             * convert the target type to something more specific.
             */
            if (PyType_IsSubtype(py_type, base_py_type))
            {
                void *cpp = sip_cast_cpp_ptr(*cpp_p, py_type, base_py_type);

                sipTypeID sub_id = (*scc->convertor)(&cpp);
                if (sub_id != sipType_Invalid)
                {
                    const sipTypeSpec *sub_ts;
                    PyTypeObject *sub_py_type;
                    PyObject *sub_mod = sip_get_type_detail(ms, sub_id,
                            &sub_ts, &sub_py_type);

                    if (sub_mod == NULL)
                    {
                        Py_DECREF(base_mod);
                        Py_DECREF(mod);
                        return -1;
                    }

                    /*
                     * We are only interested in types that are not
                     * super-classes of the target.  This happens either
                     * because it is in an earlier convertor than the one that
                     * handles the type or it is in a later convertor that
                     * handles a different branch of the hierarchy.  Either
                     * way, the ordering of the modules ensures that there will
                     * be no more than one and that it will be the right one.
                     */
                    if (!PyType_IsSubtype(py_type, sub_py_type))
                    {
                        *py_type_p = sub_py_type;
                        *ts_p = sub_ts;
                        *cpp_p = cpp;

                        if (*def_mod_p != sub_mod)
                            Py_SETREF(*def_mod_p, Py_NewRef(sub_mod));

                        /*
                         * Finally we allow the convertor to return a type that
                         * is apparently unrelated to the current convertor.
                         * This causes the whole process to be restarted with
                         * the new values.
                         */
                        int is_subtype = PyType_IsSubtype(sub_py_type,
                                base_py_type);

                        Py_DECREF(sub_mod);
                        Py_DECREF(base_mod);
                        Py_DECREF(mod);

                        return !is_subtype;
                    }

                    Py_DECREF(sub_mod);
                }
            }

            Py_DECREF(base_mod);
            scc++;
        }

        Py_DECREF(mod);
    }

    /*
     * We haven't found the exact type, so return the most specific type that
     * it must be.  This can happen legitimately if the wrapped library is
     * returning an internal class that is down-cast to a more generic class.
     * Also we want this function to be safe when a class doesn't have any
     * convertors.
     */
    return 0;
}


/*
 * Convert an array of a type to a Python sequence.
 */
static PyObject *convert_to_sequence(sipModuleState *ms, void *array,
        Py_ssize_t nr_elem, sipTypeID type_id)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return NULL;

    sipCopyFunc copy_helper;

    /* Get the type's copy helper. */
    if (sipTypeSpecIsMapped(ts))
        copy_helper = ((const sipMappedTypeSpec *)ts)->copy;
    else
        copy_helper = ((const sipClassTypeSpec *)ts)->copy;

    assert(copy_helper != NULL);

    PyObject *seq = PyTuple_New(nr_elem);
    if (seq == NULL)
        goto gc_def_mod;

    Py_ssize_t i;

    for (i = 0; i < nr_elem; ++i)
    {
        void *el = copy_helper(array, i);
        PyObject *el_obj = sip_api_convert_from_new_type(ms, el, type_id,
                NULL);

        if (el_obj == NULL)
        {
            sip_release(el, ts, 0, NULL);
            Py_DECREF(seq);
            goto gc_def_mod;
        }

        PyTuple_SET_ITEM(seq, i, el_obj);
    }

    Py_DECREF(def_mod);

    return seq;

gc_def_mod:
    Py_DECREF(def_mod);
    return NULL;
}


/*
 * sip_api_convert_to_type_us() without user state support.
 */
void *sip_api_convert_to_type(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        int *iserrp)
{
    return sip_api_convert_to_type_us(ms, pyObj, type_id, transferObj, flags,
            statep, NULL, iserrp);
}


/*
 * Convert a Python object to a C/C++ pointer, assuming a previous call to
 * sip_api_can_convert_to_type() has been successful.  Allow ownership to be
 * transferred and any type convertors to be disabled.
 */
void *sip_api_convert_to_type_us(sipModuleState *ms, PyObject *pyObj,
        sipTypeID type_id, PyObject *transferObj, int flags, int *statep,
        void **user_statep, int *iserrp)
{
    assert(sipTypeIsClass(type_id) || sipTypeIsMapped(type_id));

    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return NULL;

    void *cpp = NULL;
    int state = 0;

    /* Don't convert if there has already been an error. */
    if (!*iserrp)
    {
        /* Do the conversion. */
        if (pyObj == Py_None && !sipTypeSpecAllowNone(ts))
        {
            cpp = NULL;
        }
        else
        {
            sipConvertToFunc cto;

            if (sipTypeSpecIsClass(ts))
            {
                cto = ((const sipClassTypeSpec *)ts)->cto;

                if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
                {
                    cpp = sip_api_get_cpp_ptr(ms, pyObj, type_id);

                    if (cpp == NULL)
                    {
                        *iserrp = TRUE;
                    }
                    else if (transferObj != NULL)
                    {
                        transfer(ms->sip_module_state, pyObj, transferObj);
                    }
                }
                else if (user_state_is_valid(ts, user_statep))
                {
                    state = cto(ms, pyObj, &cpp, iserrp, transferObj,
                            user_statep);
                }
            }
            else if ((cto = ((const sipMappedTypeSpec *)ts)->cto) != NULL)
            {
                if (user_state_is_valid(ts, user_statep))
                    state = cto(ms, pyObj, &cpp, iserrp, transferObj,
                            user_statep);
            }
            else
            {
                raise_no_convert_to(pyObj, ts);
            }
        }
    }

    if (statep != NULL)
        *statep = state;

    Py_DECREF(def_mod);

    return cpp;
}


/*
 * Return the main instance for an object if it is a mixin.
 */
static sipSimpleWrapper *deref_mixin(sipSimpleWrapper *sw)
{
    return sw->mixin_main != NULL ? sw->mixin_main : sw;
}


/*
 * Return the type hint of the failed callable and a string/unicode object that
 * describes the failure.
 */
static const char *detail_from_failure(PyObject *failure_obj,
        PyObject **detail_p)
{
    sipParseFailure *failure;
    PyObject *detail;

    failure = (sipParseFailure *)PyCapsule_GetPointer(failure_obj, NULL);

    switch (failure->reason)
    {
    case Unbound:
        detail = PyUnicode_FromFormat(
                "first argument of unbound method must have type %s",
                failure->detail_str);
        break;

    case TooFew:
        detail = PyUnicode_FromString("not enough positional arguments");
        break;

    case TooMany:
        detail = PyUnicode_FromString("too many arguments");
        break;

    case Missing:
        detail = PyUnicode_FromFormat("%s argument is missing",
                failure->detail_str);
        break;

    case UnknownKeyword:
        detail = PyUnicode_FromFormat("%U is not a valid keyword argument",
                failure->detail_obj);
        break;

    case Duplicate:
        detail = PyUnicode_FromFormat(
                "%s has already been given as a positional argument",
                failure->detail_str);
        break;

    case WrongType:
        if (failure->arg_nr >= 0)
            detail = bad_type_str(failure->arg_nr, failure->detail_obj);
        else
            detail = PyUnicode_FromFormat(
                    "argument %s has unexpected type %T", failure->arg_name,
                    failure->detail_obj);

        break;

    case Exception:
        detail = failure->detail_obj;

        if (detail)
        {
            Py_INCREF(detail);
            break;
        }

        /* Drop through. */

    default:
        detail = PyUnicode_FromString("unknown reason");
    }

    *detail_p = detail;

    return failure->type_hint;
}


/*
 * The dtor for parse failure wrapped in a Python object.
 */
static void failure_dtor(PyObject *capsule)
{
    sipParseFailure *failure = (sipParseFailure *)PyCapsule_GetPointer(capsule, NULL);

    Py_XDECREF(failure->detail_obj);

    sip_api_free(failure);
}


/*
 * Implement the conversion of a C/C++ pointer to the object that wraps it.
 */
static PyObject *get_py_object(sipSipModuleState *sms, void *cppPtr,
        PyTypeObject *w_type)
{
    PyObject *obj;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);
    obj = sip_om_find_object(&sms->object_map, cppPtr, w_type);
    Py_END_CRITICAL_SECTION();

    return obj;
}


/*
 * Get "self" from the argument array for a method called as
 * Class.Method(self, ...) rather than self.Method(...).
 */
static PyObject *get_self_from_args(PyObject *const *args,
        Py_ssize_t nr_pos_args, PyTypeObject *py_type)
{
    if (nr_pos_args == 0)
        return NULL;

    PyObject *self = args[0];

    if (PyObject_TypeCheck(self, py_type))
        return self;

    return NULL;
}


/*
 * Called after a failed conversion of an integer.
 */
static void handle_failed_int_conversion(sipParseFailure *pf, PyObject *arg)
{
    assert(pf->reason == Ok || pf->reason == Overflow);

    PyObject *exc = PyErr_GetRaisedException();

    if (PyErr_GivenExceptionMatches(exc, PyExc_OverflowError))
    {
        /* Remove any previous overflow exception. */
        Py_XDECREF(pf->detail_obj);

        pf->reason = Overflow;
        pf->overflow_arg_nr = pf->arg_nr;
        pf->overflow_arg_name = pf->arg_name;
        pf->detail_obj = Py_NewRef(exc);
    }
    else
    {
        handle_failed_type_conversion(pf, arg);
    }

    PyErr_SetRaisedException(exc);
}


/*
 * Called after a failed conversion of a type.
 */
static void handle_failed_type_conversion(sipParseFailure *pf, PyObject *arg)
{
    pf->reason = WrongType;
    pf->detail_obj = Py_NewRef(arg);
}


/*
 * Set the parser state to indicate that there has been an error.
 */
void sip_api_set_parser_error(PyObject **p_state_p)
{
    PyObject *p_state = *p_state_p;

    Py_XDECREF(p_state);
    p_state = Py_NewRef(Py_None);

    *p_state_p = p_state;
}


/*
 * Parse the positional and/or keyword vectorcall arguments to a C++ ctor
 * without any side effects.  Return false if there was an error (an exception
 * will have been raised and the parser state will be Py_None) or if the
 * arguments didn't match (and an explanation will be added to the parser state
 * list, creating it if necessary).  Return true if the arguments matched (and
 * the parser state will be NULL).
 */
bool sip_api_parse_ctor_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject *self,
        PyObject **unused_p)
{
    return parse_vc_kwd_args(ms, p_state_p, args, nr_pos_args, kwd_names,
            ap_spec, params, unused_p, &self, sipType_Invalid, NULL,
            ap_spec->type_hint);
}


/*
 * Parse the positional and/or keyword vectorcall arguments to a C/C++ function
 * without any side effects.  Return false if there was an error (an exception
 * will have been raised and the parser state will be Py_None) or if the
 * arguments didn't match (and an explanation will be added to the parser state
 * list, creating it if necessary).  Return true if the arguments matched (and
 * the parser state will be NULL).
 */
bool sip_api_parse_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **self_p,
        sipTypeID self_type_id, void **self_cpp_p)
{
    return parse_vc_kwd_args(ms, p_state_p, args, nr_pos_args, kwd_names,
            ap_spec, params, NULL, self_p, self_type_id, self_cpp_p,
            ap_spec->type_hint);
}


/*
 * This is identical to sip_api_parse_args() except that the type hint is an
 * additional parameter rather than being passed in the sipArgParserSpec
 * structure.  It is used by C bindings because the C language doesn't (yet)
 * fully support constant expressions as standard.
 */
bool sip_api_parse_args_c(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **self_p,
        sipTypeID self_type_id, void **self_cpp_p, const char *type_hint)
{
    return parse_vc_kwd_args(ms, p_state_p, args, nr_pos_args, kwd_names,
            ap_spec, params, NULL, self_p, self_type_id, self_cpp_p,
            type_hint);
}


/*
 * Parse the positional and/or keyword vectorcall arguments to a C/C++ callable
 * without any side effects.
 */
static bool parse_vc_kwd_args(sipModuleState *ms, PyObject **p_state_p,
        PyObject *const *args, Py_ssize_t nr_pos_args, PyObject *kwd_names,
        const sipArgParserSpec *ap_spec, void **params, PyObject **unused_p,
        PyObject **self_p, sipTypeID self_type_id, void **self_cpp_p,
        const char *type_hint)
{
    /* Previous second pass errors stop subsequent parses. */
    if (*p_state_p != NULL && *p_state_p == Py_None)
        return false;

    sipSipModuleState *sms = ms->sip_module_state;

    sipParseFailure failure = {
        .reason = Ok,
        .type_hint = type_hint,
    };

    /* Handle the "self" argument. */
    PyObject *self = *self_p;

    switch (ap_spec->overload_type)
    {
        case sipOverloadMethod:
        case sipOverloadProtectedMethod:
        {
            bool got_self = false;

            if (self != NULL)
            {
                /*
                 * We will be expecting an enum if we are parsing the arguments
                 * to an enum slot.
                 */
                if (sipTypeIsEnum(self_type_id))
                {
                    if (sip_enum_convert_to_enum(ms, self, (void *)self_cpp_p, self_type_id, FALSE) < 0)
                    {
                        handle_failed_type_conversion(&failure, self);
                        goto failed_setup;
                    }

                    got_self = true;
                }
                else if (PyObject_TypeCheck(self, sms->simple_wrapper_type))
                {
                    got_self = true;
                }
            }

            if (!got_self)
            {
                PyObject *def_mod;
                PyTypeObject *py_type = sip_get_py_type(ms, self_type_id,
                        &def_mod);
                if (def_mod == NULL)
                {
                    failure.reason = Raised;
                    goto failed_setup;
                }

                self = get_self_from_args(args, nr_pos_args, py_type);

                if (self == NULL)
                {
                    failure.reason = Unbound;
                    failure.detail_str = py_type->tp_name;
                    Py_DECREF(def_mod);
                    goto failed_setup;
                }

                /* The call was cls.method(self, ...). */
                *self_p = self;
                args++;
                nr_pos_args--;

                Py_DECREF(def_mod);
            }

            break;
        }

        case sipOverloadStaticMethod:
        {
            /*
             * If the call was self.method(...) rather than cls.method(...)
             * then get cls from self.
             */
            if (PyObject_TypeCheck(self, sms->wrapper_type))
                *self_p = (PyObject *)Py_TYPE(self);

            /* This is for pass 2. */
            self = NULL;

            break;
        }

        case sipOverloadOther:
            /* Nothing to do. */
            break;
    }

    /* Get the total number of arguments given. */
    Py_ssize_t nr_args = nr_pos_args;

    if (kwd_names != NULL)
    {
        assert(PyTuple_Check(kwd_names));
        nr_args += PyTuple_GET_SIZE(kwd_names);
    }

    if (nr_args < ap_spec->nr_required)
    {
        failure.reason = TooFew;
        goto failed_setup;
    }

    /*
     * Populate an array where each element is the format of the corresponding
     * argument.  A format will be NULL if the argument is an unused keyword
     * argument.  The array is on the stack unless there are many arguments in
     * which case the array is on the heap.
     */
#define SMALL_ARGV  16
    const sipArgParserFormatSpec *small_arg_fmts[SMALL_ARGV];
    const sipArgParserFormatSpec **arg_fmts;

    if (nr_args < SMALL_ARGV)
    {
        arg_fmts = small_arg_fmts;
    }
    else if ((arg_fmts = sip_api_malloc(nr_args * sizeof (sipArgParserFormatSpec *))) == NULL)
    {
        failure.reason = Raised;
        goto failed_setup;
    }

    memset(arg_fmts, 0, nr_args * sizeof (sipArgParserFormatSpec *));

    /* Set the formats of keyword arguments. */
    Py_ssize_t arg_nr, fmt_nr;

    for (arg_nr = nr_pos_args; arg_nr < nr_args; arg_nr++)
    {
        PyObject *kwd_name = PyTuple_GET_ITEM(kwd_names, arg_nr - nr_pos_args);

        for (fmt_nr = 0; fmt_nr < ap_spec->nr_formats; fmt_nr++)
        {
            const sipArgParserFormatSpec *fmt_spec = &ap_spec->formats[fmt_nr];

            if (fmt_spec->kw_name != NULL && PyUnicode_CompareWithASCIIString(kwd_name, fmt_spec->kw_name) == 0)
            {
                arg_fmts[fmt_nr] = fmt_spec;
                break;
            }
        }

        /*
         * If there was no corresponding format and we are not collecting
         * unused arguments then fail.
         */
        if (fmt_nr == ap_spec->nr_formats && unused_p == NULL)
        {
            failure.reason = UnknownKeyword;
            failure.detail_obj = Py_NewRef(kwd_name);
            goto release;
        }
    }

    /* Set the formats of positional arguments. */
    for (arg_nr = 0; arg_nr < nr_pos_args; arg_nr++)
    {
        if (arg_nr == ap_spec->nr_formats)
        {
            failure.reason = TooMany;
            goto release;
        }

        const sipArgParserFormatSpec *fmt_spec = &ap_spec->formats[arg_nr];

        /*
         * If there is already a format then it must also be a keyword
         * argument.
         */
        if (arg_fmts[arg_nr] != NULL)
        {
            failure.reason = Duplicate;
            failure.detail_str = fmt_spec->kw_name;
            goto release;
        }

        arg_fmts[arg_nr] = fmt_spec;

        if (fmt_spec->type_id == sipType_ellipsis)
            break;
    }

    /* Check all required arguments have a format. */
    for (fmt_nr = 0; fmt_nr < ap_spec->nr_required; fmt_nr++)
        if (arg_fmts[fmt_nr] == NULL)
        {
            failure.reason = TooFew;
            goto release;
        }

    /*
     * Pass 1 checks the argument type and does cheap conversions with no side
     * effects.
     */
    for (arg_nr = 0; arg_nr < nr_args; arg_nr++)
    {
        const sipArgParserFormatSpec *fmt_spec = arg_fmts[arg_nr];

        if (arg_fmts[arg_nr] == NULL)
            continue;

        if (fmt_spec->kw_name != NULL)
        {
            failure.arg_nr = -1;
            failure.arg_name = fmt_spec->kw_name;
        }
        else
        {
            failure.arg_nr = (int)arg_nr;
            failure.arg_name = NULL;
        }

        parse_pass_1(ms, args[arg_nr], fmt_spec, &params[fmt_spec->param_base],
                &failure);

        /*
         * We continue parsing on overflow as that implies the type is correct
         * but the conversion has failed (which is a pass 2 error).
         */
        if (failure.reason != Ok && failure.reason != Overflow)
            goto release;
    }

    /* Handle an argument overflowing. */
    if (failure.reason == Overflow)
    {
        /*
         * We have successfully parsed the signature but one of the arguments
         * has been found to overflow.  Raise an appropriate exception and
         * ensure we don't parse any subsequent overloads.
         */
        if (failure.overflow_arg_nr >= 0)
        {
            PyErr_Format(PyExc_OverflowError, "argument %d overflowed: %S",
                    failure.overflow_arg_nr, failure.detail_obj);
        }
        else
        {
            PyErr_Format(PyExc_OverflowError, "argument '%s' overflowed: %S",
                    failure.overflow_arg_name, failure.detail_obj);
        }

        /* The overflow exception has now been raised. */
        failure.reason = Raised;

        goto release;
    }

    /* The arguments match so remove any previous failed parses. */
    Py_XDECREF(*p_state_p);
    *p_state_p = NULL;

    /* Get the C/C++ address. */
    if (ap_spec->overload_type == sipOverloadMethod)
    {
        /* Enums are handled in the first pass. */
        if (!sipTypeIsEnum(self_type_id))
        {
            if ((*self_cpp_p = sip_api_get_cpp_ptr(ms, self, self_type_id)) == NULL)
            {
                failure.reason = Raised;
                goto release;
            }
        }
    }
    else if (ap_spec->overload_type == sipOverloadProtectedMethod)
    {
        if ((*self_cpp_p = sip_get_complex_cpp_ptr(ms, self, self_type_id)) == NULL)
        {
            failure.reason = Raised;
            goto release;
        }
    }

    /* Pass 2 does the remaining conversions. */
    for (arg_nr = 0; arg_nr < nr_args; arg_nr++)
    {
        const sipArgParserFormatSpec *fmt_spec = arg_fmts[arg_nr];

        if (fmt_spec == NULL)
            continue;

        void **fmt_params = &params[fmt_spec->param_base];

        /* Handle any ellipsis argument. */
        if (fmt_spec->type_id == sipType_ellipsis)
        {
            PyObject *var_args;

            /* Create a tuple for any remaining arguments. */
            if ((var_args = PyTuple_New(nr_pos_args - arg_nr)) == NULL)
            {
                failure.reason = Raised;
                goto release;
            }

            Py_ssize_t v = 0;

            while (arg_nr < nr_pos_args)
            {
                PyTuple_SET_ITEM(var_args, v, Py_NewRef(args[arg_nr]));

                arg_nr++;
                v++;
            }

            /* Return the tuple. */
            *(PyObject **)*fmt_params = var_args;

            break;
        }

        /* Do the outstanding conversions. */
        parse_pass_2(ms, self, args[arg_nr], fmt_spec, fmt_params, &failure);

        if (failure.reason != Ok)
            goto release;
    }

    /* Handle any unused keyword arguments. */
    if (unused_p != NULL)
    {
        PyObject *unused_dict = *unused_p;

        for (arg_nr = nr_pos_args; arg_nr < nr_args; arg_nr++)
            if (arg_fmts[arg_nr] == NULL)
            {
                /*
                 * Add it to the dictionary of unused arguments creating it if
                 * necessary.  Note that if the unused arguments are actually
                 * used by a later overload then the parse will incorrectly
                 * succeed.  This should be picked up (perhaps with a
                 * misleading exception) so long as the code that handles the
                 * unused arguments checks that it can handle them all.
                 */
                if (unused_dict == NULL && (*unused_p = unused_dict = PyDict_New()) == NULL)
                {
                    failure.reason = Raised;
                    goto release;
                }

                if (PyDict_SetItem(unused_dict, PyTuple_GET_ITEM(kwd_names, arg_nr - nr_pos_args), args[arg_nr]) < 0)
                {
                    failure.reason = Raised;
                    goto release;
                }
            }
    }

    if (arg_fmts != small_arg_fmts)
        sip_api_free(arg_fmts);

    return true;

release:
    if (arg_fmts != small_arg_fmts)
        sip_api_free(arg_fmts);

failed_setup:
    if (failure.reason != Raised)
        add_failure(p_state_p, &failure);

    if (failure.reason == Raised)
    {
        Py_XDECREF(failure.detail_obj);

        /*
         * Discard any previous errors and flag that the exception we want the
         * user to see has been raised.
         */
        Py_XDECREF(*p_state_p);
        *p_state_p = Py_NewRef(Py_None);
    }

    return false;
}


/*
 * First pass of the argument parse, converting those that can be done so
 * without any side effects.
 */
static void parse_pass_1(sipModuleState *ms, PyObject *arg,
        const sipArgParserFormatSpec *fmt_spec, void **fmt_params,
        sipParseFailure *failure)
{
    bool allow_none = fmt_spec->flags & SIP_APFF_ALLOW_NONE;
    bool is_array = fmt_spec->flags & SIP_APFF_IS_ARRAY;
    bool is_constrained = fmt_spec->flags & SIP_APFF_CONSTRAINED;

    /* Skip any /GetWrapper/ parameter. */
    if (fmt_spec->flags & SIP_APFF_GET_WRAPPER)
        fmt_params++;

    bool type_conversion_failed = false;
    bool int_conversion_failed = false;
    PyErr_Clear();

    switch (fmt_spec->type_id)
    {
        case sipType_ellipsis:
        case sipType_pyobject:
            break;

        case sipType_char:
        {
            if (is_array)
            {
                if (allow_none || arg != Py_None)
                {
                    const char **cp_p = (const char **)*fmt_params++;

                    *cp_p = sip_bytes_as_char_array(arg,
                            (Py_ssize_t *)*fmt_params);
                    type_conversion_failed = PyErr_Occurred();
                }
                else
                {
                    type_conversion_failed = true;
                }
            }
            else
            {
                *(char *)*fmt_params = sip_bytes_as_char(arg);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_str:
        {
            if (allow_none || arg != Py_None)
            {
                *(const char **)*fmt_params = sip_bytes_as_string(arg);
                type_conversion_failed = PyErr_Occurred();
            }
            else
            {
                type_conversion_failed = true;
            }

            break;
        }

        case sipType_wchar:
        {
            if (is_array)
            {
                if (allow_none || arg != Py_None)
                {
                    PyObject *keep = arg;
                    Py_ssize_t asize;
                    wchar_t *wcp = sip_string_as_wchar_array(&keep, &asize);

                    if (PyErr_Occurred())
                    {
                        type_conversion_failed = true;
                    }
                    else
                    {
                        *(PyObject **)*fmt_params++ = keep;
                        *(wchar_t **)*fmt_params++ = wcp;
                        *(Py_ssize_t *)*fmt_params = asize;
                    }
                }
                else
                {
                    type_conversion_failed = true;
                }
            }
            else
            {
                *(wchar_t *)*fmt_params = sip_string_as_wchar(arg);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_wstr:
        {
            if (allow_none || arg != Py_None)
            {
                PyObject *keep = arg;
                wchar_t *wcp = sip_string_as_wstring(&keep);

                if (PyErr_Occurred())
                {
                    type_conversion_failed = true;
                }
                else
                {
                    *(PyObject **)*fmt_params++ = keep;
                    *(wchar_t **)*fmt_params = wcp;
                }
            }
            else
            {
                type_conversion_failed = true;
            }

            break;
        }

        case sipType_str_ascii:
        case sipType_str_latin1:
        case sipType_str_utf8:
        case sipType_char_ascii:
        case sipType_char_latin1:
        case sipType_char_utf8:
        {
            type_conversion_failed = \
                (!allow_none && arg == Py_None) || \
                (arg != Py_None && !PyUnicode_Check(arg) && !PyBytes_Check(arg) && !PyObject_CheckBuffer(arg));

            break;
        }

        case sipType_pydict:
        {
            if (!((allow_none && arg == Py_None) || PyObject_TypeCheck(arg, &PyDict_Type)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pylist:
        {
            if (!((allow_none && arg == Py_None) || PyObject_TypeCheck(arg, &PyList_Type)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pyslice:
        {
            if (!((allow_none && arg == Py_None) || PyObject_TypeCheck(arg, &PySlice_Type)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pytuple:
        {
            if (!((allow_none && arg == Py_None) || PyObject_TypeCheck(arg, &PyTuple_Type)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pytype:
        {
            if (!((allow_none && arg == Py_None) || PyObject_TypeCheck(arg, &PyType_Type)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pycallable:
        {
            if (!((allow_none && arg == Py_None) || PyCallable_Check(arg)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pybuffer:
        {
            if (!((allow_none && arg == Py_None) || PyObject_CheckBuffer(arg)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_pyenum:
        {
            if (!((allow_none && arg == Py_None) || sip_enum_is_enum(ms->sip_module_state, arg)))
                type_conversion_failed = true;
            else
                *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_bool:
        {
            if (is_constrained)
            {
                if (PyBool_Check(arg))
                    *(bool *)*fmt_params = (arg == Py_True);
                else
                    type_conversion_failed = true;
            }
            else
            {
                *(bool *)*fmt_params = sip_api_convert_to_bool(arg);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_int:
        {
            if (is_constrained && !PyLong_Check(arg))
            {
                type_conversion_failed = true;
            }
            else
            {
                *(int *)*fmt_params = sip_api_long_as_int(arg);
                int_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_uint:
        {
            *(unsigned *)*fmt_params = sip_api_long_as_unsigned_int(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_size_t:
        {
            *(size_t *)*fmt_params = sip_api_long_as_size_t(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_byte:
        {
            *(char *)*fmt_params = sip_api_long_as_char(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_sbyte:
        {
            *(signed char *)*fmt_params = sip_api_long_as_signed_char(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_ubyte:
        {
            *(unsigned char *)*fmt_params = sip_api_long_as_unsigned_char(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_short:
        {
            *(signed short *)*fmt_params = sip_api_long_as_short(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_ushort:
        {
            *(unsigned short *)*fmt_params = sip_api_long_as_unsigned_short(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_long:
        {
            *(long *)*fmt_params = sip_api_long_as_long(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_ulong:
        {
            *(unsigned long *)*fmt_params = sip_api_long_as_unsigned_long(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_longlong:
        {
            *(long long *)*fmt_params = sip_api_long_as_long_long(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_ulonglong:
        {
            *(unsigned long long *)*fmt_params = sip_api_long_as_unsigned_long_long(arg);
            int_conversion_failed = PyErr_Occurred();

            break;
        }

        case sipType_float:
        {
            if (is_constrained)
            {
                if (PyFloat_Check(arg))
                    *(float *)*fmt_params = (float)PyFloat_AS_DOUBLE(arg);
                else
                    type_conversion_failed = true;
            }
            else
            {
                *(float *)*fmt_params = (float)PyFloat_AsDouble(arg);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_double:
        {
            if (is_constrained)
            {
                if (PyFloat_Check(arg))
                    *(double *)*fmt_params = PyFloat_AS_DOUBLE(arg);
                else
                    type_conversion_failed = true;
            }
            else
            {
                *(double *)*fmt_params = PyFloat_AsDouble(arg);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        case sipType_voidptr:
        {
            if (allow_none || arg != Py_None)
            {
                *(void **)*fmt_params = sip_api_convert_to_void_ptr(arg);
                type_conversion_failed = PyErr_Occurred();
            }
            else
            {
                type_conversion_failed = true;
            }

            break;
        }

        case sipType_pycapsule:
        {
            const char *name = (const char *)*fmt_params++;
            void **v_p = (void **)*fmt_params;

            if (allow_none && arg == Py_None)
            {
                *v_p = NULL;
            }
            else
            {
                *v_p = PyCapsule_GetPointer(arg, name);
                type_conversion_failed = PyErr_Occurred();
            }

            break;
        }

        default:
        {
            sipTypeID type_id = fmt_spec->type_id;

            if (sipTypeIsEnum(type_id))
            {
                if (sip_enum_convert_to_enum(ms, arg, *fmt_params, type_id, is_constrained ? FALSE : TRUE) < 0)
                    type_conversion_failed = true;
            }
            else if (is_array)
            {
                if (sipTypeIsClass(type_id))
                {
                    if (!sip_array_can_convert(ms, arg, type_id) && !PySequence_Check(arg))
                        type_conversion_failed = true;
                }
                else
                {
                    assert(sipTypeIsMapped(type_id));

                    if (!PySequence_Check(arg))
                        type_conversion_failed = true;
                }
            }
            else
            {
                int flags = 0;

                if (!allow_none)
                    flags |= SIP_NOT_NONE;

                if (is_constrained)
                    flags |= SIP_NO_CONVERTORS;

                if (!sip_api_can_convert_to_type(ms, arg, type_id, flags))
                    type_conversion_failed = true;
            }
        }
    }

    if (type_conversion_failed)
        handle_failed_type_conversion(failure, arg);
    else if (int_conversion_failed)
        handle_failed_int_conversion(failure, arg);
}


/*
 * Second pass of the argument parse, converting the remaining ones that might
 * have side effects.  Return true if there was no error.
 */
static void parse_pass_2(sipModuleState *ms, PyObject *self, PyObject *arg,
        const sipArgParserFormatSpec *fmt_spec, void **fmt_params,
        sipParseFailure *failure)
{
    uint16_t transfer = fmt_spec->flags & SIP_APFF_TRANSFER_MASK;

    /* Handle /GetWrapper/. */
    if (fmt_spec->flags & SIP_APFF_GET_WRAPPER)
        *(PyObject **)*fmt_params++ = arg;

    switch (fmt_spec->type_id)
    {
        case sipType_pyobject:
        {
            if (transfer == SIP_APFF_TRANSFER)
                Py_XINCREF(arg);
            else if (transfer == SIP_APFF_TRANSFER_BACK)
                Py_XDECREF(arg);

            *(PyObject **)*fmt_params = arg;

            break;
        }

        case sipType_str_ascii:
        {
            PyObject **keep_p = (PyObject **)*fmt_params++;

            *keep_p = arg;

            *(const char **)*fmt_params = sip_string_as_ascii_string(keep_p);

            if (PyErr_Occurred())
                failure->reason = Raised;

            break;
        }

        case sipType_str_latin1:
        {
            PyObject **keep_p = (PyObject **)*fmt_params++;

            *keep_p = arg;

            *(const char **)*fmt_params = sip_string_as_latin1_string(keep_p);

            if (PyErr_Occurred())
                failure->reason = Raised;

            break;
        }

        case sipType_str_utf8:
        {
            PyObject **keep_p = (PyObject **)*fmt_params++;

            *keep_p = arg;

            *(const char **)*fmt_params = sip_string_as_utf8_string(keep_p);

            if (PyErr_Occurred())
                failure->reason = Raised;

            break;
        }

        case sipType_char_ascii:
        {
            *(char *)*fmt_params = sip_string_as_ascii_char(arg);

            if (PyErr_Occurred())
                failure->reason = Raised;
        }

        case sipType_char_latin1:
        {
            *(char *)*fmt_params = sip_string_as_latin1_char(arg);

            if (PyErr_Occurred())
                failure->reason = Raised;
        }

        case sipType_char_utf8:
        {
            *(char *)*fmt_params = sip_string_as_utf8_char(arg);

            if (PyErr_Occurred())
                failure->reason = Raised;
        }

        default:
        {
            sipTypeID type_id = fmt_spec->type_id;
            bool allow_none = fmt_spec->flags & SIP_APFF_ALLOW_NONE;
            bool is_array = fmt_spec->flags & SIP_APFF_IS_ARRAY;
            bool is_constrained = fmt_spec->flags & SIP_APFF_CONSTRAINED;

            if (is_array)
            {
                if (sipTypeIsClass(type_id))
                {
                    void **array = (void **)*fmt_params++;
                    Py_ssize_t *nr_elem_p = (Py_ssize_t *)*fmt_params++;
                    int *is_temp_p = (int *)*fmt_params;

                    if (sip_array_can_convert(ms, arg, type_id))
                    {
                        sip_array_convert(arg, array, nr_elem_p);
                        *is_temp_p = FALSE;
                    }
                    else if (convert_from_sequence(ms, arg, type_id, array, nr_elem_p))
                    {
                        /*
                         * Note that this will leak if there is a subsequent
                         * error.
                         */
                        *is_temp_p = TRUE;
                    }
                    else
                    {
                        failure->reason = Raised;
                    }
                }
                else if (sipTypeIsMapped(type_id))
                {
                    void **array = (void **)*fmt_params++;
                    Py_ssize_t *nr_elem_p = (Py_ssize_t *)*fmt_params;

                    if (!convert_from_sequence(ms, arg, type_id, array, nr_elem_p))
                        failure->reason = Raised;
                }
            }
            else if (sipTypeIsClass(type_id) || sipTypeIsMapped(type_id))
            {
                int flags = 0;
                int *state_p;
                PyObject *xfer, **owner;
                void **user_state_p;

                void **p = (void **)*fmt_params++;

                if (transfer == SIP_APFF_TRANSFER)
                    xfer = ((self != NULL) ? self : arg);
                else if (transfer == SIP_APFF_TRANSFER_BACK)
                    xfer = Py_None;
                else
                    xfer = NULL;

                if (!allow_none)
                    flags |= SIP_NOT_NONE;

                if (transfer == SIP_APFF_TRANSFER_THIS)
                    owner = (PyObject **)*fmt_params++;
                else
                    owner = NULL;

                if (is_constrained)
                {
                    flags |= SIP_NO_CONVERTORS;
                    state_p = NULL;
                }
                else
                {
                    state_p = (int *)*fmt_params++;
                }

                PyObject *def_mod;
                const sipTypeSpec *ts = sip_get_type_spec(ms, type_id,
                        &def_mod);
                if (def_mod == NULL)
                {
                    failure->reason = Raised;
                    break;
                }

                if (sipTypeSpecNeedsUserState(ts))
                    user_state_p = (void **)*fmt_params++;
                else
                    user_state_p = NULL;

                int is_err = FALSE;

                *p = sip_api_convert_to_type_us(ms, arg, type_id, xfer,
                            flags, state_p, user_state_p, &is_err);

                if (is_err)
                    failure->reason = Raised;
                else if (owner != NULL && *p != NULL)
                    *owner = arg;

                Py_DECREF(def_mod);
            }
        }
    }
}


/*
 * Do the main work of parsing a result object based on a format string.
 */
static int parse_result(sipModuleState *ms, PyObject *method, PyObject *res,
        sipSimpleWrapper *py_self, const char *fmt, va_list va)
{
    /* We rely on PyErr_Occurred(). */
    PyErr_Clear();

    /* Basic validation of the format string. */
    Py_ssize_t tupsz;
    int rc = 0;

    if (*fmt == '(')
    {
        char ch;
        const char *cp = ++fmt;
        int sub_format = FALSE;

        tupsz = 0;

        while ((ch = *cp++) != ')')
        {
            if (ch == '\0')
            {
                PyErr_Format(PyExc_ValueError,
                        "sipParseResult(): invalid format string \"%s\"",
                        fmt - 1);
                rc = -1;

                break;
            }

            if (sub_format)
            {
                sub_format = FALSE;
            }
            else
            {
                tupsz++;

                /* Some format characters have a sub-format. */
                if (strchr("aAHDC", ch) != NULL)
                    sub_format = TRUE;
            }
        }

        if (rc == 0)
            if (!PyTuple_Check(res) || PyTuple_GET_SIZE(res) != tupsz)
            {
                sip_api_bad_catcher_result(method);
                rc = -1;
            }
    }
    else
    {
        tupsz = -1;
    }

    if (rc == 0)
    {
        char ch;
        int i = 0;

        while ((ch = *fmt++) != '\0' && ch != ')' && rc == 0)
        {
            PyObject *arg;
            int invalid = FALSE;

            if (tupsz > 0)
            {
                arg = PyTuple_GET_ITEM(res, i);
                i++;
            }
            else
            {
                arg = res;
            }

            switch (ch)
            {
            case 'g':
                {
                    const char **p = va_arg(va, const char **);
                    Py_ssize_t *szp = va_arg(va, Py_ssize_t *);

                    Py_ssize_t asize;
                    const char *cp = sip_bytes_as_char_array(arg, &asize);

                    if (PyErr_Occurred())
                    {
                        invalid = TRUE;
                    }
                    else
                    {
                        *p = cp;
                        *szp = asize;
                    }
                }

                break;

            case 'G':
                {
                    int key = va_arg(va, int);
                    wchar_t **p = va_arg(va, wchar_t **);
                    Py_ssize_t *szp = va_arg(va, Py_ssize_t *);

                    PyObject *keep = arg;
                    Py_ssize_t asize;

                    wchar_t *wcp = sip_string_as_wchar_array(&keep, &asize);

                    if (PyErr_Occurred() || sip_api_keep_reference(ms, (PyObject *)py_self, key, keep) < 0)
                    {
                        invalid = TRUE;
                    }
                    else
                    {
                        *p = wcp;
                        *szp = asize;
                    }
                }

                break;

            case 'b':
                {
                    bool *p = va_arg(va, bool *);

                    bool v = sip_api_convert_to_bool(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'c':
                {
                    char *p = va_arg(va, char *);

                    char ch = sip_bytes_as_char(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = ch;
                }

                break;

            case 'a':
                {
                    char *p = va_arg(va, char *);
                    char ch;

                    switch (*fmt++)
                    {
                    case 'A':
                        ch = sip_string_as_ascii_char(arg);
                        break;

                    case 'L':
                        ch = sip_string_as_latin1_char(arg);
                        break;

                    case '8':
                        ch = sip_string_as_utf8_char(arg);
                        break;

                    default:
                        ch = '\0';
                        PyErr_SetString(PyExc_ValueError,
                                "sipParseResult(): invalid sub-format character to 'a'");
                    }

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = ch;
                }

                break;

            case 'w':
                {
                    wchar_t *p = va_arg(va, wchar_t *);

                    wchar_t wch = sip_string_as_wchar(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = wch;
                }

                break;

            case 'd':
                {
                    double *p = va_arg(va, double *);
                    double v = PyFloat_AsDouble(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'F':
                {
                    sipTypeID type_id = va_arg(va, sipTypeID);
                    void *p = va_arg(va, void *);

                    if (sip_enum_convert_to_enum(ms, arg, p, type_id, TRUE) < 0)
                        invalid = TRUE;
                }

                break;

            case 'f':
                {
                    float *p = va_arg(va, float *);
                    float v = (float)PyFloat_AsDouble(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'I':
                {
                    char *p = va_arg(va, char *);
                    char v = sip_api_long_as_char(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'L':
                {
                    signed char *p = va_arg(va, signed char *);
                    signed char v = sip_api_long_as_signed_char(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'M':
                {
                    unsigned char *p = va_arg(va, unsigned char *);
                    unsigned char v = sip_api_long_as_unsigned_char(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'h':
                {
                    signed short *p = va_arg(va, signed short *);
                    signed short v = sip_api_long_as_short(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 't':
                {
                    unsigned short *p = va_arg(va, unsigned short *);
                    unsigned short v = sip_api_long_as_unsigned_short(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'e':
                {
                    int *p = va_arg(va, int *);
                    int v = sip_api_long_as_int(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'i':
                {
                    int *p = va_arg(va, int *);
                    int v = sip_api_long_as_int(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'u':
                {
                    unsigned *p = va_arg(va, unsigned *);
                    unsigned v = sip_api_long_as_unsigned_int(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case '=':
                {
                    size_t *p = va_arg(va, size_t *);
                    size_t v = sip_api_long_as_size_t(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'l':
                {
                    long *p = va_arg(va, long *);
                    long v = sip_api_long_as_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'm':
                {
                    unsigned long *p = va_arg(va, unsigned long *);
                    unsigned long v = sip_api_long_as_unsigned_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'n':
                {
                    long long *p = va_arg(va, long long *);
                    long long v = sip_api_long_as_long_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'o':
                {
                    unsigned long long *p = va_arg(va, unsigned long long *);
                    unsigned long long v = sip_api_long_as_unsigned_long_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'A':
                {
                    int key = va_arg(va, int);
                    const char **p = va_arg(va, const char **);

                    PyObject *keep = arg;
                    const char *cp;

                    switch (*fmt++)
                    {
                    case 'A':
                        cp = sip_string_as_ascii_string(&keep);
                        break;

                    case 'L':
                        cp = sip_string_as_latin1_string(&keep);
                        break;

                    case '8':
                        cp = sip_string_as_utf8_string(&keep);
                        break;

                    default:
                        cp = NULL;
                        PyErr_SetString(PyExc_ValueError,
                                "sipParseResult(): invalid sub-format character to 'A'");
                    }

                    if (PyErr_Occurred() || sip_api_keep_reference(ms, (PyObject *)py_self, key, keep) < 0)
                        invalid = TRUE;
                    else
                        *p = cp;
                }

                break;

            case 'B':
                {
                    int key = va_arg(va, int);
                    const char **p = va_arg(va, const char **);

                    const char *cp = sip_bytes_as_string(arg);

                    if (PyErr_Occurred() || sip_api_keep_reference(ms, (PyObject *)py_self, key, arg) < 0)
                        invalid = TRUE;
                    else
                        *p = cp;
                }

                break;

            case 'x':
                {
                    int key = va_arg(va, int);
                    wchar_t **p = va_arg(va, wchar_t **);

                    PyObject *keep = arg;

                    wchar_t *wcp = sip_string_as_wstring(&keep);

                    if (PyErr_Occurred() || sip_api_keep_reference(ms, (PyObject *)py_self, key, keep) < 0)
                        invalid = TRUE;
                    else
                        *p = wcp;
                }

                break;

            case 'H':
                {
                    if (*fmt == '\0')
                    {
                        invalid = TRUE;
                    }
                    else
                    {
                        sipTypeID type_id = va_arg(va, sipTypeID);
                        void *cpp = va_arg(va, void **);

                        int flags = *fmt++ - '0';
                        int iserr = FALSE, state;
                        void *user_state;

                        void *val = sip_api_force_convert_to_type_us(ms, arg,
                                type_id,
                                (flags & FMT_RP_FACTORY ? arg : NULL),
                                (flags & FMT_RP_DEREF ? SIP_NOT_NONE : 0),
                                &state,
                                (flags & FMT_RP_MAKE_COPY ? &user_state : NULL),
                                &iserr);

                        if (iserr)
                        {
                            invalid = TRUE;
                        }
                        else if (flags & FMT_RP_MAKE_COPY)
                        {
                            /*
                             * Note that I don't know why cpp would ever be
                             * NULL.
                             */
                            int rc;
                            if (cpp != NULL)
                                rc = sip_copy_wrapped_type(ms, type_id, cpp,
                                        val);
                            else
                                rc = 0;

                            sip_api_release_type_us(ms, val, type_id, state,
                                    user_state);

                            if (rc < 0)
                                break;
                        }
                        else if (cpp != NULL)
                        {
                            *(void **)cpp = val;
                        }
                    }
                }

                break;

            case 'N':
                {
                    PyTypeObject *type = va_arg(va, PyTypeObject *);
                    PyObject **p = va_arg(va, PyObject **);

                    if (arg == Py_None || PyObject_TypeCheck(arg, type))
                    {
                        if (p != NULL)
                        {
                            Py_INCREF(arg);
                            *p = arg;
                        }
                    }
                    else
                    {
                        invalid = TRUE;
                    }
                }

                break;

            case 'O':
                {
                    PyObject **p = va_arg(va, PyObject **);

                    if (p != NULL)
                    {
                        Py_INCREF(arg);
                        *p = arg;
                    }
                }

                break;

            case 'T':
                {
                    PyTypeObject *type = va_arg(va, PyTypeObject *);
                    PyObject **p = va_arg(va, PyObject **);

                    if (PyObject_TypeCheck(arg, type))
                    {
                        if (p != NULL)
                        {
                            Py_INCREF(arg);
                            *p = arg;
                        }
                    }
                    else
                    {
                        invalid = TRUE;
                    }
                }

                break;

            case 'V':
                {
                    void *v = sip_api_convert_to_void_ptr(arg);
                    void **p = va_arg(va, void **);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *p = v;
                }

                break;

            case 'z':
                {
                    const char *name = va_arg(va, const char *);
                    void **p = va_arg(va, void **);

                    if (arg == Py_None)
                    {
                        if (p != NULL)
                            *p = NULL;
                    }
                    else
                    {
                        void *v = PyCapsule_GetPointer(arg, name);

                        if (PyErr_Occurred())
                            invalid = TRUE;
                        else
                            *p = v;
                    }
                }

                break;

            case 'Z':
                if (arg != Py_None)
                    invalid = TRUE;

                break;

            case '!':
                {
                    PyObject **p = va_arg(va, PyObject **);

                    if (PyObject_CheckBuffer(arg))
                        *p = Py_NewRef(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case '$':
                {
                    PyObject **p = va_arg(va, PyObject **);

                    if (arg == Py_None || PyObject_CheckBuffer(arg))
                        *p = Py_NewRef(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case '&':
                {
                    PyObject **p = va_arg(va, PyObject **);

                    if (sip_enum_is_enum(ms->sip_module_state, arg))
                        *p = Py_NewRef(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case '^':
                {
                    PyObject **p = va_arg(va, PyObject **);

                    if (arg == Py_None || sip_enum_is_enum(ms->sip_module_state, arg))
                        *p = Py_NewRef(arg);
                    else
                        invalid = TRUE;
                }

                break;

            default:
                PyErr_Format(PyExc_ValueError,
                        "sipParseResult(): invalid format character '%c'", ch);
                rc = -1;
            }

            if (invalid)
            {
                sip_api_bad_catcher_result(method);
                rc = -1;
                break;
            }
        }
    }

    return rc;
}


/*
 * Copy an instance of a wrapped type.
 */
int sip_copy_wrapped_type(sipModuleState *ms, sipTypeID type_id, void *dst,
        void *src)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return -1;

    sipAssignFunc assign_helper;

    if (sipTypeSpecIsMapped(ts))
        assign_helper = ((const sipMappedTypeSpec *)ts)->assign;
    else
        assign_helper = ((const sipClassTypeSpec *)ts)->assign;

    assert(assign_helper != NULL);

    assign_helper(dst, 0, src);

    Py_DECREF(def_mod);

    return 0;
}


/*
 * Raise an exception when there is no mapped type converter to convert to
 * C/C++ from Python.
 */
static void raise_no_convert_to(PyObject *py, const sipTypeSpec *ts)
{
    PyErr_Format(PyExc_TypeError, "%s cannot be converted to %s",
            Py_TYPE(py)->tp_name, ts->cpp_name);
}


/*
 * Handle a transfer object.
 */
static void transfer(sipSipModuleState *sms, PyObject *py,
        PyObject *transferObj)
{
    if (transferObj == Py_None)
        sip_transfer_back(sms, py);
    else
        sip_transfer_to(sms, py, transferObj);
}


/*
 * Check that a user state pointer has been provided if the type requires it.
 * This is most likely a problem with handwritten code.
 */
static int user_state_is_valid(const sipTypeSpec *ts, void **user_statep)
{
    if (sipTypeSpecNeedsUserState(ts) && user_statep == NULL)
    {
        PyErr_Format(PyExc_RuntimeError,
                "%s requires user state but none is provided", ts->cpp_name);

        return FALSE;
    }

    return TRUE;
}
