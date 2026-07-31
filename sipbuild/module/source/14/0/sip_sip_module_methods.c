/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the sip module's methods.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>
#include <stdio.h>

#include "sip_sip_module_methods.h"

#include "sip.h"
#include "sip_core.h"
#include "sip_enum.h"
#include "sip_object_map.h"
#include "sip_parsers.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper.h"
#include "sip_wrapper_type.h"


/* Forward declarations of method implementations. */
static PyObject *meth_assign(PyObject *mod, PyObject *args);
static PyObject *meth_delete(PyObject *mod, PyObject *arg);
static PyObject *meth_dump(PyObject *mod, PyObject *arg);
static PyObject *meth_enableautoconversion(PyObject *mod, PyObject *args);
static PyObject *meth_isdeleted(PyObject *mod, PyObject *args);
static PyObject *meth_ispycreated(PyObject *mod, PyObject *args);
static PyObject *meth_ispyowned(PyObject *mod, PyObject *args);
static PyObject *meth_setdeleted(PyObject *mod, PyObject *args);
static PyObject *meth_settracemask(PyObject *mod, PyObject *args);
static PyObject *meth_transferback(PyObject *mod, PyObject *args);
static PyObject *meth_transferto(PyObject *mod, PyObject *args);
static PyObject *meth_wrapinstance(PyObject *mod, PyObject *args);
static PyObject *meth_unwrapinstance(PyObject *mod, PyObject *args);


PyMethodDef sipSipModuleMethods[] = {
    {"assign", meth_assign, METH_VARARGS, NULL},
    {"delete", meth_delete, METH_VARARGS, NULL},
    {"dump", meth_dump, METH_O, NULL},
    {"enableautoconversion", meth_enableautoconversion, METH_VARARGS, NULL},
    {"isdeleted", meth_isdeleted, METH_VARARGS, NULL},
    {"ispycreated", meth_ispycreated, METH_VARARGS, NULL},
    {"ispyowned", meth_ispyowned, METH_VARARGS, NULL},
    {"setdeleted", meth_setdeleted, METH_VARARGS, NULL},
    {"settracemask", meth_settracemask, METH_VARARGS, NULL},
    {"transferback", meth_transferback, METH_VARARGS, NULL},
    {"transferto", meth_transferto, METH_VARARGS, NULL},
    {"wrapinstance", meth_wrapinstance, METH_VARARGS, NULL},
    {"unwrapinstance", meth_unwrapinstance, METH_VARARGS, NULL},
    {0}
};


/* Forward declarations. */
static void clear_wrapper(sipSipModuleState *sms, PyObject *w_inst);
static void print_object(const char *label, PyObject *obj);


/*
 * Invoke the assignment operator for a C++ instance.
 */
static PyObject *meth_assign(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *dst, *src;

    if (!PyArg_ParseTuple(args, "O!O!:assign", sms->simple_wrapper_type, &dst, sms->simple_wrapper_type, &src))
        return NULL;

    /* Get the assignment helper. */
    sipAssignFunc assign_helper;
    PyTypeObject *dst_type = Py_TYPE(dst);
    const sipTypeSpec *dst_ts = sip_get_type_spec_from_wt(
            (sipWrapperType *)dst_type);

    if (sipTypeSpecIsMapped(dst_ts))
        assign_helper = ((const sipMappedTypeSpec *)dst_ts)->assign;
    else
        assign_helper = ((const sipClassTypeSpec *)dst_ts)->assign;

    if (assign_helper == NULL)
    {
        PyErr_SetString(PyExc_TypeError,
                "argument 1 of assign() does not support assignment");
        return NULL;
    }

    /* Check the types are compatible. */
    PyTypeObject *src_type = Py_TYPE(src);

    if (src_type == dst_type)
    {
        dst_type = NULL;
    }
    else if (!PyType_IsSubtype(src_type, dst_type))
    {
        PyErr_SetString(PyExc_TypeError,
                "type of argument 1 of assign() must be a super-type of type of argument 2");
        return NULL;
    }

    /* Get the addresses. */
    void *dst_addr, *src_addr;

    if ((dst_addr = sip_get_cpp_ptr(dst, NULL)) == NULL)
        return NULL;

    if ((src_addr = sip_get_cpp_ptr(src, dst_type)) == NULL)
        return NULL;

    /* Do the assignment. */
    assign_helper(dst_addr, 0, src_addr);

    Py_RETURN_NONE;
}


/*
 * Call an instance's dtor.
 */
static PyObject *meth_delete(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *w_inst;

    if (!PyArg_ParseTuple(args, "O!:delete", sms->simple_wrapper_type, &w_inst))
        return NULL;

    sipSimpleWrapper *sw = (sipSimpleWrapper *)w_inst;

    if (sip_check_pointer(sw->data, w_inst) < 0)
        return NULL;

    clear_wrapper(sms, w_inst);

    sip_release(sw->data,
            sip_get_type_spec_from_wt((sipWrapperType *)Py_TYPE(w_inst)),
            sw->flags, NULL);

    Py_RETURN_NONE;
}


/*
 * Dump various bits of potentially useful information to stdout.  Note that we
 * use the same calling convention as sys.getrefcount() so that it has the
 * same caveat regarding the reference count.
 */
static PyObject *meth_dump(PyObject *smod, PyObject *arg)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);

    if (!PyObject_TypeCheck(arg, sms->simple_wrapper_type))
    {
        PyErr_Format(PyExc_TypeError,
                "dump() argument 1 must be " _SIP_MODULE_FQ_NAME ".simplewrapper, not %s",
                Py_TYPE(arg)->tp_name);
        return NULL;
    }

    sipSimpleWrapper *sw = (sipSimpleWrapper *)arg;

    print_object(NULL, (PyObject *)sw);

    printf("    Reference count: %" PY_FORMAT_SIZE_T "d\n", Py_REFCNT(sw));
    printf("    Address of wrapped object: %p\n", sip_api_get_address(sw));
    printf("    Created by: %s\n", (sipIsDerived(sw) ? "Python" : "C/C++"));
    printf("    To be destroyed by: %s\n", (sipIsPyOwned(sw) ? "Python" : "C/C++"));

    if (((sipWrapperType *)Py_TYPE(sw))->is_wrapper)
    {
        sipWrapper *w = (sipWrapper *)sw;

        print_object("Parent wrapper", (PyObject *)w->parent);
        print_object("Next sibling wrapper", (PyObject *)w->sibling_next);
        print_object("Previous sibling wrapper", (PyObject *)w->sibling_prev);
        print_object("First child wrapper", (PyObject *)w->first_child);
    }

    Py_RETURN_NONE;
}


/*
 * Enable or disable auto-conversion of a wrapped type.
 */
static PyObject *meth_enableautoconversion(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    sipWrapperType *wt;
    int enable;

    if (!PyArg_ParseTuple(args, "O!i:enableautoconversion", sms->wrapper_type_type, &wt, &enable))
        return NULL;

    PyObject *was_enabled = (wt->autoconversion_disabled ? Py_False : Py_True);
    wt->autoconversion_disabled = !enable;

    return Py_NewRef(was_enabled);
}


/*
 * Check if an instance still exists without raising an exception.
 */
static PyObject *meth_isdeleted(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    sipSimpleWrapper *sw;

    if (!PyArg_ParseTuple(args, "O!:isdeleted", sms->simple_wrapper_type, &sw))
        return NULL;

    PyObject *res = (sip_api_get_address(sw) == NULL ? Py_True : Py_False);

    return Py_NewRef(res);
}


/*
 * Check if an instance was created by Python.
 */
static PyObject *meth_ispycreated(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    sipSimpleWrapper *sw;

    if (!PyArg_ParseTuple(args, "O!:ispycreated", sms->simple_wrapper_type, &sw))
        return NULL;

    PyObject *res = (sipIsDerived(sw) ? Py_True : Py_False);

    return Py_NewRef(res);
}


/*
 * Check if an instance is owned by Python or C/C++.
 */
static PyObject *meth_ispyowned(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    sipSimpleWrapper *sw;

    if (!PyArg_ParseTuple(args, "O!:ispyowned", sms->simple_wrapper_type, &sw))
        return NULL;

    PyObject *res = (sipIsPyOwned(sw) ? Py_True : Py_False);

    return Py_NewRef(res);
}


/*
 * Mark an instance as having been deleted.
 */
static PyObject *meth_setdeleted(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *w_inst;

    if (!PyArg_ParseTuple(args, "O!:setdeleted", sms->simple_wrapper_type, &w_inst))
        return NULL;

    clear_wrapper(sms, w_inst);

    Py_RETURN_NONE;
}


/*
 * Set the trace mask.
 */
static PyObject *meth_settracemask(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    unsigned new_mask;

    if (!PyArg_ParseTuple(args, "I:settracemask", &new_mask))
        return NULL;

    sms->trace_mask = new_mask;

    Py_RETURN_NONE;
}


/*
 * Transfer the ownership of an instance to Python.
 */
static PyObject *meth_transferback(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *w;

    if (!PyArg_ParseTuple(args, "O!:transferback", sms->wrapper_type, &w))
        return NULL;

    sip_transfer_back(sms, w);

    Py_RETURN_NONE;
}


/*
 * Transfer the ownership of an instance to C/C++.
 */
static PyObject *meth_transferto(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *w, *owner;

    if (!PyArg_ParseTuple(args, "O!O:transferto", sms->wrapper_type, &w, &owner))
        return NULL;

    if (owner == Py_None)
    {
        /*
         * Note that the Python API is different to the C API when the owner is
         * None.
         */
        owner = NULL;
    }
    else if (!((sipWrapperType *)Py_TYPE(owner))->is_wrapper)
    {
        PyErr_Format(PyExc_TypeError,
                "transferto() argument 2 must be " _SIP_MODULE_FQ_NAME ".wrapper, not %s",
                Py_TYPE(owner)->tp_name);
        return NULL;
    }

    sip_transfer_to(sms, w, owner);

    Py_RETURN_NONE;
}


/*
 * Unwrap an instance.
 */
static PyObject *meth_unwrapinstance(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    PyObject *w_inst;

    if (!PyArg_ParseTuple(args, "O!:unwrapinstance", sms->simple_wrapper_type, &w_inst))
        return NULL;

    /*
     * We just get the pointer but don't try and cast it (which isn't needed
     * and wouldn't work with the way casts are currently implemented if we are
     * unwrapping something derived from a wrapped class).
     */
    void *addr = sip_get_cpp_ptr(w_inst, NULL);
    if (addr == NULL)
        return NULL;

    return PyLong_FromVoidPtr(addr);
}


/*
 * Wrap an instance.
 */
static PyObject *meth_wrapinstance(PyObject *smod, PyObject *args)
{
    sipSipModuleState *sms = sip_get_sip_module_state(smod);
    unsigned long long addr;
    sipWrapperType *wt;

    if (!PyArg_ParseTuple(args, "KO!:wrapinstance", &addr, sms->wrapper_type_type, &wt))
        return NULL;

    return sip_api_convert_from_type(sip_get_module_state(wt->defining_module),
            (void *)addr, wt->type_id, NULL);
}


/*
 * Clear a simple wrapper.
 */
static void clear_wrapper(sipSipModuleState *sms, PyObject *w_inst)
{
    sipWrapperType *wt = (sipWrapperType *)Py_TYPE(w_inst);
    sipSimpleWrapper *sw = (sipSimpleWrapper *)w_inst;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

    if (wt->is_wrapper)
        sip_remove_from_parent((sipWrapper *)sw);

    /*
     * Transfer ownership to C++ so we don't try to release it when the
     * Python object is garbage collected.
     */
    sipResetPyOwned(sw);

    sip_om_remove_object(sip_get_module_state(wt->defining_module), sw);

    Py_END_CRITICAL_SECTION();
}


/*
 * Write a reference to a wrapper to stdout.
 */
static void print_object(const char *label, PyObject *obj)
{
    if (label != NULL)
        printf("    %s: ", label);

    if (obj != NULL)
        PyObject_Print(obj, stdout, 0);
    else
        printf("NULL");

    printf("\n");
}
