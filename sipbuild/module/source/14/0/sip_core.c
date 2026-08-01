/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The core sip module code.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "sip_core.h"

#include "sip.h"
#include "sip_array.h"
#include "sip_attribute.h"
#include "sip_docstrings.h"
#include "sip_enum.h"
#include "sip_helpers.h"
#include "sip_int_convertors.h"
#include "sip_method_descriptor.h"
#include "sip_parsers.h"
#include "sip_simple_wrapper.h"
#include "sip_sip_module.h"
#include "sip_string_convertors.h"
#include "sip_variable_descriptor.h"
#include "sip_voidptr.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper.h"
#include "sip_wrapper_type.h"


static void sip_api_abstract_method(const char *classname, const char *method);
static void sip_api_call_hook(const char *hookname);
static void *sip_api_cast_to_target_type(sipModuleState *ms, sipTypeID type_id,
        void *cpp, const sipClassTypeSpec *target_cts);
static int sip_api_convert_to_enum(sipModuleState *ms, PyObject *obj,
        void *addr, sipTypeID type_id);
static int sip_api_enable_autoconversion(sipModuleState *ms,
        PyTypeObject *py_type, int enable);
static void sip_api_export_symbols(sipModuleState *ms,
        const sipSymbolSpec *const symbols);
static sipTypeID sip_api_find_type(sipModuleState *ms, const char *type);
static PyInterpreterView *sip_api_get_interpreter_view(sipModuleState *ms);
static PyTypeObject *sip_api_get_py_type_ref(sipModuleState *ms,
        sipTypeID type_id);
static PyTypeObject *sip_api_get_simple_wrapper_type(sipModuleState *ms);
static int sip_api_get_state(PyObject *transferObj);
static PyObject *sip_api_get_type_user_object(sipWrapperType *wt);
static PyObject *sip_api_get_user_object(sipSimpleWrapper *sw);
static PyTypeObject *sip_api_get_void_ptr_type(sipModuleState *ms);
static PyTypeObject *sip_api_get_wrapper_type(sipModuleState *ms);
static PyTypeObject *sip_api_get_wrapper_type_type(sipModuleState *ms);
static void *sip_api_import_symbol(sipModuleState *ms, const char *name);
static void sip_api_instance_destroyed(sipModuleState *ms,
        sipSimpleWrapper **self_p);
static int sip_api_is_derived_class(sipSimpleWrapper *sw);
static int sip_api_is_owned_by_python(sipSimpleWrapper *sw);
static bool sip_api_is_target_type(sipModuleState *ms, sipTypeID type_id,
        const sipClassTypeSpec *target_cts);
static int sip_api_is_user_type(sipWrapperType *wt);
static sipExceptionHandler sip_api_next_exception_handler(sipModuleState *ms,
        PyObject **handler_mod_p, sipModuleState **handler_ms_p,
        Py_ssize_t *state_p);
static void sip_api_raise_type_exception(sipModuleState *ms, sipTypeID type_id,
        void *ptr);
static void sip_api_raise_unknown_exception(PyObject **p_state_p);
static int sip_api_register_event_handlers(sipModuleState *ms,
        const sipEventHandlerSpec *const handlers);
static int sip_api_register_py_type(sipModuleState *ms, PyTypeObject *py_type);
static const char *sip_api_resolve_typedef(sipModuleState *ms,
        const char *name);
static bool sip_api_self_is_arg(sipModuleState *ms, PyObject *self,
        sipTypeID type_id);
static void sip_api_set_type_user_object(sipWrapperType *wt, PyObject *data);
static void sip_api_set_user_object(sipSimpleWrapper *sw, PyObject *user);
static void sip_api_trace(sipModuleState *ms, unsigned mask, const char *fmt,
        ...);
static void sip_api_transfer_back(sipModuleState *ms, PyObject *self);
static void sip_api_transfer_to(sipModuleState *ms, PyObject *self,
        PyObject *owner);
static sipTypeID sip_api_type_scope(sipModuleState *ms, sipTypeID type_id);
static void sip_api_visit_wrappers(sipModuleState *ms,
        sipWrapperVisitorFunc visitor, void *closure);


/*
 * The immutable data structure that implements the SIP ABI.
 */
const sipABISpec sip_abi = {
    sizeof (sipModuleState),

    /*
     * The following are part of the public ABI.
     */
    sip_api_bad_catcher_result,
    sip_api_bad_length_for_slice,
    sip_api_build_result,
    sip_api_call_method,
    sip_api_convert_from_sequence_index,
    sip_api_can_convert_to_type,
    sip_api_convert_to_type,
    sip_api_convert_to_type_us,
    sip_api_force_convert_to_type,
    sip_api_force_convert_to_type_us,
    sip_api_release_type,
    sip_api_release_type_us,
    sip_api_convert_from_type,
    sip_api_convert_from_new_type,
    sip_api_convert_from_enum,
    sip_api_get_state,
    sip_api_free,
    sip_api_get_py_object_ref,
    sip_api_get_py_type_ref,
    sip_api_get_simple_wrapper_type,
    sip_api_get_void_ptr_type,
    sip_api_get_wrapper_type,
    sip_api_get_wrapper_type_type,
    sip_api_malloc,
    sip_api_trace,
    sip_api_transfer_back,
    sip_api_transfer_to,
    sip_api_long_as_unsigned_long,
    sip_api_convert_from_void_ptr,
    sip_api_convert_from_const_void_ptr,
    sip_api_convert_from_void_ptr_and_size,
    sip_api_convert_from_const_void_ptr_and_size,
    sip_api_convert_to_void_ptr,
    sip_api_export_symbols,
    sip_api_import_symbol,
    sip_api_find_type,
    sip_api_register_py_type,
    sip_api_type_scope,
    sip_api_resolve_typedef,
    sip_api_bad_callable_arg,
    sip_api_get_address,
    sip_api_enable_autoconversion,
    sip_api_convert_from_new_py_type,
    sip_api_convert_to_typed_array,
    sip_api_convert_to_array,
    sip_api_get_interpreter_view,
    sip_api_set_type_user_object,
    sip_api_get_type_user_object,
    sip_api_get_method,
    sip_api_from_method,
    sip_api_get_c_function,
    sip_api_get_date,
    sip_api_from_date,
    sip_api_get_date_time,
    sip_api_from_date_time,
    sip_api_get_time,
    sip_api_from_time,
    sip_api_is_user_type,
    sip_api_unicode_new,
    sip_api_unicode_write,
    sip_api_unicode_data,
    sip_api_get_user_object,
    sip_api_set_user_object,
    sip_api_instance_destroyed,
    sip_api_is_owned_by_python,
    sip_api_enable_gc,
    sip_api_object_dump,
    sip_api_register_event_handlers,
    sip_api_convert_to_enum,
    sip_api_convert_to_bool,
    sip_api_long_as_char,
    sip_api_long_as_signed_char,
    sip_api_long_as_unsigned_char,
    sip_api_long_as_short,
    sip_api_long_as_unsigned_short,
    sip_api_long_as_int,
    sip_api_long_as_unsigned_int,
    sip_api_long_as_long,
    sip_api_long_as_long_long,
    sip_api_long_as_unsigned_long_long,
    sip_api_convert_from_slice_object,
    sip_api_long_as_size_t,
    sip_api_visit_wrappers,
#if defined(SIP_CONFIGURATION_PyEnums)
    sip_api_is_enum_flag,
#else
    NULL,
#endif
    sip_api_py_type_dict_ref,
    sip_api_get_frame_ref,
    sip_api_get_module_user_state,
    sip_api_set_module_user_state,
    /*
     * The following are not part of the public ABI.
     */
    sip_api_parse_args,
    sip_api_parse_args_c,
    sip_api_parse_ctor_args,
    sip_api_abstract_method,
    sip_api_get_cpp_ptr,
    sip_api_is_py_method,
    sip_api_is_target_type,
    sip_api_cast_to_target_type,
    sip_api_call_hook,
    sip_api_raise_unknown_exception,
    sip_api_raise_type_exception,
    sip_api_set_parser_error,
    sip_api_deprecated,
    sip_api_keep_reference,
    sip_api_add_exception,
    sip_api_parse_result,
    sip_api_call_error_handler,
    sip_api_call_procedure_method,
    sip_api_init_slot_impl,
    sip_api_is_derived_class,
    sip_api_next_exception_handler,
    sip_api_self_is_arg,
    sip_api_module_clear,
    sip_api_module_exec,
    sip_api_module_free,
    sip_api_module_traverse,
};


/* Forward references. */
static int add_slot_dispatchers(sipModuleState *ms, PyTypeObject *py_type,
        const sipClassTypeSpec *cts);
static void call_py_dtor(sipModuleState *ms, sipSimpleWrapper *self);
static int compare_typedef_name(const void *key, const void *el);
static PyTypeObject *create_class_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipClassTypeSpec *ctd);
static PyTypeObject *create_container_type(sipModuleState *ms,
        sipTypeID type_id, const sipTypeSpec *ts, initproc tp_init,
        sipGetBufferFunc bf_getbuffer, sipReleaseBufferFunc bf_releasebuffer,
        const sipAttrGroupSpec *attrs, PyObject *bases,
        PyTypeObject *metatype);
static PyTypeObject *create_exception_type(sipModuleState *ms,
        const sipExceptionTypeSpec *ets);
static PyTypeObject *create_mapped_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipMappedTypeSpec *mts);
static PyTypeObject *find_registered_py_type(sipSipModuleState *sms,
        const char *name, PyObject **def_mod_p);
static PyObject *get_doc(PyObject *self, void *closure);
static bool is_a_wrapped_type(sipModuleState *ms, PyTypeObject *py_type);
static const sipSymbolSpec *lookup_symbol(sipModuleState *ms,
        const char *name);
static PyObject *pickle_type(PyObject *self, PyTypeObject *defining_class,
        PyObject *const *args, Py_ssize_t nargs, PyObject *kwd_args);
static int set_reduce(PyTypeObject *py_type);
static PyObject *resolve_type_id(sipModuleState *ms, sipTypeID type_id,
        sipTypeNr *def_type_nr_p);


/*
 * Return a borrowed reference to the Python type object that implements simple
 * wrapped instances (ie. those that do not support object ownership).
 */
static PyTypeObject *sip_api_get_simple_wrapper_type(sipModuleState *ms)
{
    return ms->sip_module_state->simple_wrapper_type;
}


/*
 * Return a borrowed reference to the Python type object that implements
 * wrapped instances.
 */
static PyTypeObject *sip_api_get_wrapper_type(sipModuleState *ms)
{
    return ms->sip_module_state->wrapper_type;
}


/*
 * Return a borrowed reference to the Python type object that is the metatype
 * for wrapped types.
 */
static PyTypeObject *sip_api_get_wrapper_type_type(sipModuleState *ms)
{
    return ms->sip_module_state->wrapper_type_type;
}


/*
 * Return a borrowed reference to the Python type object for the voidptr type.
 */
static PyTypeObject *sip_api_get_void_ptr_type(sipModuleState *ms)
{
    return ms->sip_module_state->void_ptr_type;
}


/*
 * Set a dictionary item and discard the reference to the item even if there
 * was an error.
 */
int sip_dict_set_and_discard(PyObject *dict, const char *name, PyObject *obj)
{
    if (obj == NULL)
        return -1;

    int rc = PyDict_SetItemString(dict, name, obj);

    Py_DECREF(obj);

    return rc;
}


/*
 * Return the current interpreter view.  To determine if the interpreter is
 * usable then call PyInterpreterGuard_FromView().
 */
static PyInterpreterView *sip_api_get_interpreter_view(sipModuleState *ms)
{
    return ms->sip_module_state->interpreter_view;
}


/*
 * Display a printf() style message to stderr according to the current trace
 * mask.
 */
static void sip_api_trace(sipModuleState *ms, unsigned mask, const char *fmt,
        ...)
{
    va_list ap;

    va_start(ap,fmt);

    if (ms->sip_module_state->trace_mask & mask)
        vfprintf(stderr, fmt, ap);

    va_end(ap);
}


/*
 * Convert a Python object implementing an enum to a member value.  An
 * exception is raised if there was an error.
 */
static int sip_api_convert_to_enum(sipModuleState *ms, PyObject *obj,
        void *addr, sipTypeID type_id)
{
    return sip_enum_convert_to_enum(ms, obj, addr, type_id, TRUE);
}


/*
 * Register the given Python type.
 */
static int sip_api_register_py_type(sipModuleState *ms, PyTypeObject *type)
{
    return sip_append_py_object_to_list(ms->sip_module_state,
            &ms->registered_py_types, (PyObject *)type);
}


/*
 * Return a borrowed reference to the registered type with the given name and a
 * strong reference to the defining module.  An exception is raised if it
 * couldn't be found.
 */
static PyTypeObject *find_registered_py_type(sipSipModuleState *sms,
        const char *name, PyObject **def_mod_p)
{
    PyObject *mods = sms->module_list;
    Py_ssize_t i;

    *def_mod_p = NULL;

    for (i = 0; i < PyList_GET_SIZE(mods); i++)
    {
        PyObject *mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, i), &mod) < 0)
            return NULL;

        if (mod == NULL)
            continue;

        sipModuleState *ms = sip_get_module_state(mod);

        if (ms->registered_py_types != NULL)
        {
            Py_ssize_t rpt_i;

            /* The list can only ever grow so we don't need to protect it. */
            for (rpt_i = 0; rpt_i < PyList_GET_SIZE(ms->registered_py_types); rpt_i++)
            {
                PyTypeObject *type = (PyTypeObject *)PyList_GET_ITEM(
                        ms->registered_py_types, rpt_i);

                if (strcmp(type->tp_name, name) == 0)
                {
                    *def_mod_p = mod;
                    return type;
                }
            }
        }

        Py_DECREF(mod);
    }

    PyErr_Format(PyExc_RuntimeError, "%s is not a registered type", name);

    return NULL;
}


/*
 * Keep an extra reference to an object (which may be NULL if the object was
 * optional).
 */
int sip_api_keep_reference(sipModuleState *ms, PyObject *w_inst, int key,
        PyObject *obj)
{
    /* Get a pointer to the dict of extra references. */
    PyObject **extra_refs_p = (w_inst != NULL ?
            &((sipSimpleWrapper *)w_inst)->extra_refs :
            &ms->extra_refs);

    /* Create the dict if it doesn't already exist. */
    Py_BEGIN_CRITICAL_SECTION_MUTEX(&ms->sip_module_state->mutex);

    if (*extra_refs_p == NULL)
        *extra_refs_p = PyDict_New();

    Py_END_CRITICAL_SECTION();

    if (*extra_refs_p == NULL)
        return -1;

    PyObject *key_obj = PyLong_FromLong(key);
    if (key_obj == NULL)
        return -1;

    /* Map NULL onto None and we want to replace any existing reference. */
    if (obj == NULL)
        obj = Py_None;

    int rc = PyDict_SetItem(*extra_refs_p, key_obj, obj);
    Py_DECREF(key_obj);

    return rc;
}


/*
 * A wrapper around the Python memory allocater that will raise an exception if
 * the allocation fails.
 */
void *sip_api_malloc(size_t nbytes)
{
    void *mem;

    if ((mem = PyMem_RawMalloc(nbytes)) == NULL)
        PyErr_NoMemory();

    return mem;
}


/*
 * A wrapper around the Python memory de-allocater.
 */
void sip_api_free(void *mem)
{
    PyMem_RawFree(mem);
}


/*
 * Carry out actions common to all dtors.
 */
static void sip_api_instance_destroyed(sipModuleState *ms,
        sipSimpleWrapper **self_p)
{
    sipSimpleWrapper *self = *self_p;

    assert(self != NULL);

    PyThreadStateToken *tst = PyThreadState_EnsureFromView(
            ms->sip_module_state->interpreter_view);

    if (tst != NULL)
    {
        /* We may be tidying up after an exception so preserve it. */
        PyObject *exc = PyErr_GetRaisedException();
        call_py_dtor(ms, self);
        PyErr_SetRaisedException(exc);

        Py_BEGIN_CRITICAL_SECTION_MUTEX(&ms->sip_module_state->mutex);
        sip_isolate_wrapper(ms, self);
        Py_END_CRITICAL_SECTION();

        /*
         * It is now safe to remove the link from the wrapper to the C/C++
         * instance.
         */
        self->data = NULL;

        *self_p = NULL;

        PyThreadState_Release(tst);
    }
}


/*
 * Isolate a wrapper, ie. clear any relationships with other wrappers.  This
 * may mean it gets garbage collected.  The main mutex must be locked.
 */
void sip_isolate_wrapper(sipModuleState *ms, sipSimpleWrapper *sw)
{
    sip_om_remove_object(ms, sw);

    /*
     * If C/C++ has a reference (and therefore no parent) then remove it.
     * Otherwise remove the object from any parent.
     */
    if (sipCppHasRef(sw))
    {
        sipResetCppHasRef(sw);
        Py_DECREF((PyObject *)sw);
    }
    else if (((sipWrapperType *)Py_TYPE((PyObject *)sw))->is_wrapper)
    {
        sip_remove_from_parent((sipWrapper *)sw);
    }
}


/*
 * Call self.__dtor__() if it is implemented.
 */
static void call_py_dtor(sipModuleState *ms, sipSimpleWrapper *self)
{
    PyThreadStateToken *tst;
    char pymc = 0;

    PyObject *method = sip_api_is_py_method(ms, &tst, &pymc, &self, NULL,
            "__dtor__");

    if (method != NULL)
    {
        PyObject *res = PyObject_CallObject(method, NULL);

        Py_DECREF(method);

        /* Discard any result. */
        Py_XDECREF(res);

        /* Handle any error the best we can. */
        if (PyErr_Occurred())
            PyErr_Print();

        PyThreadState_Release(tst);
    }
}


/*
 * Add a wrapper to it's parent owner.  The wrapper must not currently have a
 * parent and, therefore, no siblings.  The main mutex must be locked.
 */
void sip_add_to_parent(sipWrapper *self, sipWrapper *owner)
{
    if (owner->first_child != NULL)
    {
        self->sibling_next = owner->first_child;
        owner->first_child->sibling_prev = self;
    }

    /*
     * The owner holds a real reference so that the cyclic garbage collector
     * works properly.
     */
    owner->first_child = (sipWrapper *)Py_NewRef(self);
    self->parent = owner;
}


/*
 * Remove a wrapper from it's parent if it has one.  The main mutex must be
 * locked.
 */
void sip_remove_from_parent(sipWrapper *self)
{
    if (self->parent != NULL)
    {
        if (self->parent->first_child == self)
            self->parent->first_child = self->sibling_next;

        if (self->sibling_next != NULL)
            self->sibling_next->sibling_prev = self->sibling_prev;

        if (self->sibling_prev != NULL)
            self->sibling_prev->sibling_next = self->sibling_next;

        self->parent = NULL;
        self->sibling_next = NULL;
        self->sibling_prev = NULL;

        /*
         * We must do this last, after all the pointers are correct, because
         * this is used by the clear slot.
         */
        Py_DECREF((PyObject *)self);
    }
}


/*
 * Create a container type and return a strong reference to it.
 */
static PyTypeObject *create_container_type(sipModuleState *ms,
        sipTypeID type_id, const sipTypeSpec *ts, initproc tp_init,
        sipGetBufferFunc bf_getbuffer, sipReleaseBufferFunc bf_releasebuffer,
        const sipAttrGroupSpec *attrs, PyObject *bases, PyTypeObject *metatype)
{
    static PyGetSetDef getters_setters[] = {
        {"__doc__", get_doc},
        {0}
    };

    PyType_Slot slots[] = {
        {Py_tp_getset, getters_setters},
        {0},
        {0},
        {0},
        {0}
    };

    Py_ssize_t slot_index = 1;

    if (tp_init != NULL)
    {
        slots[slot_index].slot = Py_tp_init;
        slots[slot_index].pfunc = tp_init;
        slot_index++;
    }

    if (bf_getbuffer != NULL)
    {
        slots[slot_index].slot = Py_bf_getbuffer;
        slots[slot_index].pfunc = SimpleWrapper_getbuffer;
        slot_index++;
    }

    if (bf_releasebuffer != NULL)
    {
        slots[slot_index].slot = Py_bf_releasebuffer;
        slots[slot_index].pfunc = SimpleWrapper_releasebuffer;
        slot_index++;
    }

    PyType_Spec spec = {
        .name = ts->tp_name,
        .basicsize = 0,
        .flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,
        .slots = (PyType_Slot *)slots,
    };

    PyTypeObject *w_type = (PyTypeObject *)PyType_FromMetaclass(metatype,
            ms->wrapped_module, &spec, bases);

    if (w_type == NULL)
        return NULL;

    /* Configure the type. */
    sipWrapperType *wt = (sipWrapperType *)w_type;

    wt->is_wrapper = PyType_IsSubtype(w_type,
            ms->sip_module_state->wrapper_type);
    wt->defining_module = Py_NewRef(ms->wrapped_module);
    wt->type_id = type_id;

    /* Add the attributes. */
    if (sip_add_attrs_to_type(ms, w_type, attrs, ts) < 0)
    {
        Py_DECREF(w_type);
        return NULL;
    }

    return w_type;
}


/*
 * The __doc__ getter.
 */
static PyObject *get_doc(PyObject *self, void *Py_UNUSED(closure))
{
    return sip_get_class_docstring((sipWrapperType *)Py_TYPE(self));
}


/*
 * Add a group of attributes to a type.
 */
int sip_add_attrs_to_type(sipModuleState *ms, PyTypeObject *py_type,
        const sipAttrGroupSpec *attrs, const sipTypeSpec *ts)
{
    if (attrs != NULL)
    {
        Py_ssize_t attr_i = 0;

        while (attr_i < attrs->nr_attrs)
        {
            const sipAttrSpec *attr_spec = &attrs->attrs[attr_i++];

            PyObject *attr = sip_get_attribute_for_spec(ms,
                    (PyObject *)py_type, attr_spec,
                    sipTypeSpecIsNamespace(ts) || sipAttrIsExtendableSlot(attr_spec)? ts : NULL);

            /*
             * We need to use PyObject_SetAttrString() for slots.  For other
             * attributes we update the type's dict directly as a minor
             * optimisation.
             */
            int rc;

            if (sipAttrIsSlot(attr_spec) || sipAttrIsExtendableSlot(attr_spec))
            {
                if (attr == NULL)
                    return -1;

                rc = PyObject_SetAttrString((PyObject *)py_type,
                        sipAttrGetName(attr_spec), attr);

                Py_DECREF(attr);
            }
            else
            {
                rc = sip_dict_set_and_discard(py_type->tp_dict,
                        sipAttrGetName(attr_spec), attr);
            }

            if (rc < 0)
                return -1;
        }
    }

    return 0;
}


/*
 * Return a strong reference to the name of a type.
 */
PyObject *sip_get_type_name(const sipTypeSpec *ts)
{
    const char *name = strrchr(ts->tp_name, '.');
    if (name != NULL)
        name++;
    else
        name = ts->tp_name;

    return PyUnicode_FromString(name);
}


/*
 * Set __qualname__ for a type.
 */
int sip_set_qualname(sipModuleState *ms, const sipTypeSpec *ts,
        PyObject *py_type)
{
    /* It only needs setting if there is a scope. */
    if (ts->scope_id == sipType_Invalid)
        return 0;

    PyObject *dunder_qualname = PyUnicode_InternFromString("__qualname__");
    if (dunder_qualname == NULL)
        return -1;

    /* Get the __qualname__ of the scope (which may be a mapped type). */
    PyTypeObject *scope_py_type;
    PyObject *scope_mod = sip_get_type_detail(ms, ts->scope_id, NULL,
            &scope_py_type);
    if (scope_mod == NULL)
        goto gc_dunder_qualname;

    assert(scope_py_type != NULL);

    PyObject *scope_qualname = PyObject_GetAttr((PyObject *)scope_py_type,
            dunder_qualname);

    Py_DECREF(scope_mod);

    if (scope_qualname == NULL)
        goto gc_dunder_qualname;

    /* Get the basename of the type. */
    PyObject *name = sip_get_type_name(ts);
    if (name == NULL)
    {
        Py_DECREF(scope_qualname);
        goto gc_dunder_qualname;
    }

    PyObject *qualname = PyUnicode_FromFormat("%S.%S", scope_qualname, name);
    Py_DECREF(name);
    Py_DECREF(scope_qualname);

    if (qualname == NULL)
        goto gc_dunder_qualname;

    int rc = PyObject_SetAttr(py_type, dunder_qualname, qualname);
    Py_DECREF(dunder_qualname);
    Py_DECREF(qualname);

    return rc;

gc_dunder_qualname:
    Py_DECREF(dunder_qualname);
    return -1;
}


/*
 * Add the dispatchers for extendable slots that don't have a local
 * implementation.
 */
static int add_slot_dispatchers(sipModuleState *ms, PyTypeObject *py_type,
        const sipClassTypeSpec *cts)
{
    /* These must be alphabetical order. */
    static const sipAttrSpec dispatcher_attrs[] = {
        {.name = "e__add__"},
        {.name = "e__and__"},
        {.name = "e__eq__"},
        {.name = "e__floordiv__"},
        {.name = "e__ge__"},
        {.name = "e__gt__"},
        {.name = "e__iadd__"},
        {.name = "e__iand__"},
        {.name = "e__ifloordiv__"},
        {.name = "e__ilshift__"},
        {.name = "e__imatmul__"},
        {.name = "e__imod__"},
        {.name = "e__imul__"},
        {.name = "e__ior__"},
        {.name = "e__irshift__"},
        {.name = "e__isub__"},
        {.name = "e__itruediv__"},
        {.name = "e__ixor__"},
        {.name = "e__le__"},
        {.name = "e__lshift__"},
        {.name = "e__lt__"},
        {.name = "e__matmul__"},
        {.name = "e__mod__"},
        {.name = "e__mul__"},
        {.name = "e__ne__"},
        {.name = "e__or__"},
        {.name = "e__rshift__"},
        {.name = "e__sub__"},
        {.name = "e__truediv__"},
        {.name = "e__xor__"},
        {0}
    };

    /*
     * Find the first extendable slot that the class has an implementation for,
     * if any.
     */
    Py_ssize_t cls_idx = 0;

    if (cts->attrs != NULL)
    {
        while (cls_idx < cts->attrs->nr_attrs)
        {
            if (sipAttrIsExtendableSlot(&cts->attrs->attrs[cls_idx]))
                break;

            cls_idx++;
        }
    }
    else
    {
        cls_idx = -1;
    }

    /*
     * Step through all the extendable slots and the class attributes.  If we
     * find a slot that is missing from the class then add a dispatcher for it.
     */
    const sipAttrSpec *attr_spec;

    for (attr_spec = dispatcher_attrs; attr_spec->name != NULL; attr_spec++)
    {
        int cmp = -1;

        /* Step through the attributes until we are level or after the slot. */
        while (cls_idx >= 0 && cls_idx < cts->attrs->nr_attrs)
        {
            cmp = strcmp(sipAttrGetName(attr_spec),
                    sipAttrGetName(&cts->attrs->attrs[cls_idx]));

            if (cmp <= 0)
                break;

            /*
             * The slot is later so move to the next class attribute and try
             * again.
             */
            cls_idx++;
        }

        if (cmp == 0)
        {
            cls_idx++;
            continue;
        }

        PyObject *attr = sipMethodDescr_New(ms->sip_module_state, attr_spec,
                ms->wrapped_module, (PyObject *)py_type, &cts->base);

        if (attr == NULL)
            return -1;

        int rc = PyObject_SetAttrString((PyObject *)py_type,
                sipAttrGetName(attr_spec), attr);

        Py_DECREF(attr);

        if (rc < 0)
            return -1;
    }

    return 0;
}


/*
 * Create a class type object.
 */
static PyTypeObject *create_class_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipClassTypeSpec *cts)
{
    sipSipModuleState *sms = ms->sip_module_state;

    PyObject *bases;

    if (cts->supers == NULL)
    {
        if (cts->supertype == NULL)
        {
            bases = sipTypeSpecIsNamespace(&cts->base) ?
                Py_NewRef(sms->simple_wrapper_type) :
                Py_NewRef(sms->wrapper_type);
        }
        else if (*cts->supertype == '\0')
        {
            bases = Py_NewRef(sms->simple_wrapper_type);
        }
        else
        {
            PyObject *sup_mod;
            PyTypeObject *sup = find_registered_py_type(sms, cts->supertype,
                    &sup_mod);
            if (sup_mod == NULL)
                return NULL;

            if (!PyType_IsSubtype(sup, sms->simple_wrapper_type))
            {
                Py_DECREF(sup_mod);

                PyErr_Format(PyExc_TypeError, "'%s' is not a sub-type of '%s'",
                        cts->supertype, sms->simple_wrapper_type->tp_name);
                return NULL;
            }

            bases = Py_NewRef(sup);
            Py_DECREF(sup_mod);
        }
    }
    else if (sipTypeIDIsSentinel(cts->supers[0]))
    {
        /* There is only one super-type. */
        PyObject *base_mod;
        bases = (PyObject *)sip_get_py_type(ms, cts->supers[0], &base_mod);
        if (base_mod == NULL)
            return NULL;

        Py_INCREF(bases);
        Py_DECREF(base_mod);
    }
    else
    {
        const sipTypeID *supers;
        Py_ssize_t nr_supers = 1;

        for (supers = cts->supers; !sipTypeIDIsSentinel(*supers); supers++)
            nr_supers++;

        if ((bases = PyTuple_New(nr_supers)) == NULL)
            return NULL;

        Py_ssize_t i;

        for (i = 0; i < nr_supers; i++)
        {
            PyObject *sup_mod;
            PyTypeObject *sup_py_type = sip_get_py_type(ms, cts->supers[i],
                    &sup_mod);
            if (sup_mod == NULL)
            {
                Py_DECREF(bases);
                return NULL;
            }

            PyTuple_SET_ITEM(bases, i, Py_NewRef(sup_py_type));
            Py_DECREF(sup_mod);
        }
    }

    /*
     * Use the explicit meta-type if there is one, otherwise use the meta-type
     * of the first super-type.
     */
    PyTypeObject *metatype;

    if (cts->metatype != NULL)
    {
        PyObject *meta_mod;
        metatype = find_registered_py_type(sms, cts->metatype, &meta_mod);
        if (meta_mod == NULL)
        {
            Py_DECREF(bases);
            return NULL;
        }

        if (!PyType_IsSubtype(metatype, sms->wrapper_type_type))
        {
            Py_DECREF(meta_mod);
            Py_DECREF(bases);

            PyErr_Format(PyExc_TypeError, "'%s' is not a sub-type of '%s'",
                    cts->metatype, sms->wrapper_type_type->tp_name);
            return NULL;
        }

        Py_INCREF(metatype);
        Py_DECREF(meta_mod);
    }
    else
    {
        PyObject *first = PyTuple_CheckExact(bases) ? PyTuple_GET_ITEM(bases, 0) : bases;
        metatype = (PyTypeObject *)Py_NewRef(Py_TYPE(first));
    }

    sipTypeID type_id = SIP_TYPE_ID_TYPE_CLASS | SIP_TYPE_ID_LOCAL_MODULE | type_nr;

    PyTypeObject *py_type = create_container_type(ms, type_id, &cts->base,
            cts->init_slot, cts->getbuffer, cts->releasebuffer, cts->attrs,
            bases, metatype);

    Py_DECREF(metatype);
    Py_DECREF(bases);

    if (py_type == NULL)
        return NULL;

    /* Add the extendable slot dispatchers. */
    if (add_slot_dispatchers(ms, py_type, cts) < 0)
    {
        Py_DECREF(py_type);
        return NULL;
    }

    /* Set __qualname__. */
    if (sip_set_qualname(ms, &cts->base, (PyObject *)py_type) < 0)
    {
        Py_DECREF(py_type);
        return NULL;
    }


    /* Handle the pickle function. */
    if (cts->pickle != NULL && set_reduce(py_type) < 0)
    {
        Py_DECREF(py_type);
        return NULL;
    }

    /* Invoke any event handlers. */
    PyObject *mods = sms->module_list;
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(mods); i++)
    {
        PyObject *ehs_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, i), &ehs_mod) < 0)
        {
            Py_DECREF(py_type);
            return NULL;
        }

        if (ehs_mod == NULL)
            continue;

        sipModuleState *ehs_ms = sip_get_module_state(ehs_mod);

        if (ehs_ms->event_handlers != NULL)
        {
            const sipEventHandlerSpec *ehs = ehs_ms->event_handlers;

            while (ehs->event_type != sipEventNone)
            {
                if (ehs->event_type == sipEventFinalisingType && sip_is_subtype(ehs_ms, py_type, ehs->type_id))
                {
                    sipFinalisingTypeEventHandler handler = (sipFinalisingTypeEventHandler)ehs->handler;

                    if (handler(ehs_ms, ehs->type_id, py_type) < 0)
                    {
                        Py_DECREF(ehs_mod);
                        Py_DECREF(py_type);
                        return NULL;
                    }
                }

                ehs++;
            }
        }

        Py_DECREF(ehs_mod);
    }

    return py_type;
}


/*
 * Create an exception type object.
 */
static PyTypeObject *create_exception_type(sipModuleState *ms,
        const sipExceptionTypeSpec *ets)
{
    PyObject *base, *base_mod = NULL;

    switch (ets->base_type_id)
    {
        /* Convert the standard exceptions. */
        case sipType_BaseException: base = PyExc_BaseException; break;
        case sipType_BaseExceptionGroup: base = PyExc_BaseExceptionGroup; break;
        case sipType_Exception: base = PyExc_Exception; break;
        case sipType_ArithmeticError: base = PyExc_ArithmeticError; break;
        case sipType_AssertionError: base = PyExc_AssertionError; break;
        case sipType_AttributeError: base = PyExc_AttributeError; break;
        case sipType_BlockingIOError: base = PyExc_BlockingIOError; break;
        case sipType_BrokenPipeError: base = PyExc_BrokenPipeError; break;
        case sipType_BufferError: base = PyExc_BufferError; break;
        case sipType_ChildProcessError: base = PyExc_ChildProcessError; break;
        case sipType_ConnectionAbortedError: base = PyExc_ConnectionAbortedError; break;
        case sipType_ConnectionError: base = PyExc_ConnectionError; break;
        case sipType_ConnectionRefusedError: base = PyExc_ConnectionRefusedError; break;
        case sipType_ConnectionResetError: base = PyExc_ConnectionResetError; break;
        case sipType_EOFError: base = PyExc_EOFError; break;
        case sipType_FileExistsError: base = PyExc_FileExistsError; break;
        case sipType_FileNotFoundError: base = PyExc_FileNotFoundError; break;
        case sipType_FloatingPointError: base = PyExc_FloatingPointError; break;
        case sipType_GeneratorExit: base = PyExc_GeneratorExit; break;
        case sipType_ImportCycleError: base = PyExc_ImportCycleError; break;
        case sipType_ImportError: base = PyExc_ImportError; break;
        case sipType_IndentationError: base = PyExc_IndentationError; break;
        case sipType_IndexError: base = PyExc_IndexError; break;
        case sipType_InterruptedError: base = PyExc_InterruptedError; break;
        case sipType_IsADirectoryError: base = PyExc_IsADirectoryError; break;
        case sipType_KeyError: base = PyExc_KeyError; break;
        case sipType_KeyboardInterrupt: base = PyExc_KeyboardInterrupt; break;
        case sipType_LookupError: base = PyExc_LookupError; break;
        case sipType_MemoryError: base = PyExc_MemoryError; break;
        case sipType_ModuleNotFoundError: base = PyExc_ModuleNotFoundError; break;
        case sipType_NameError: base = PyExc_NameError; break;
        case sipType_NotADirectoryError: base = PyExc_NotADirectoryError; break;
        case sipType_NotImplementedError: base = PyExc_NotImplementedError; break;
        case sipType_OSError: base = PyExc_OSError; break;
        case sipType_OverflowError: base = PyExc_OverflowError; break;
        case sipType_PermissionError: base = PyExc_PermissionError; break;
        case sipType_ProcessLookupError: base = PyExc_ProcessLookupError; break;
        case sipType_PythonFinalizationError: base = PyExc_PythonFinalizationError; break;
        case sipType_RecursionError: base = PyExc_RecursionError; break;
        case sipType_ReferenceError: base = PyExc_ReferenceError; break;
        case sipType_RuntimeError: base = PyExc_RuntimeError; break;
        case sipType_StopAsyncIteration: base = PyExc_StopAsyncIteration; break;
        case sipType_StopIteration: base = PyExc_StopIteration; break;
        case sipType_SyntaxError: base = PyExc_SyntaxError; break;
        case sipType_SystemError: base = PyExc_SystemError; break;
        case sipType_SystemExit: base = PyExc_SystemExit; break;
        case sipType_TabError: base = PyExc_TabError; break;
        case sipType_TimeoutError: base = PyExc_TimeoutError; break;
        case sipType_TypeError: base = PyExc_TypeError; break;
        case sipType_UnboundLocalError: base = PyExc_UnboundLocalError; break;
        case sipType_UnicodeDecodeError: base = PyExc_UnicodeDecodeError; break;
        case sipType_UnicodeEncodeError: base = PyExc_UnicodeEncodeError; break;
        case sipType_UnicodeError: base = PyExc_UnicodeError; break;
        case sipType_UnicodeTranslateError: base = PyExc_UnicodeTranslateError; break;
        case sipType_ValueError: base = PyExc_ValueError; break;
        case sipType_ZeroDivisionError: base = PyExc_ZeroDivisionError; break;

        /* Convert the standard warnings. */
        case sipType_Warning: base = PyExc_Warning; break;
        case sipType_BytesWarning: base = PyExc_BytesWarning; break;
        case sipType_DeprecationWarning: base = PyExc_DeprecationWarning; break;
        case sipType_EncodingWarning: base = PyExc_EncodingWarning; break;
        case sipType_FutureWarning: base = PyExc_FutureWarning; break;
        case sipType_ImportWarning: base = PyExc_ImportWarning; break;
        case sipType_PendingDeprecationWarning: base = PyExc_PendingDeprecationWarning; break;
        case sipType_ResourceWarning: base = PyExc_ResourceWarning; break;
        case sipType_RuntimeWarning: base = PyExc_RuntimeWarning; break;
        case sipType_SyntaxWarning: base = PyExc_SyntaxWarning; break;
        case sipType_UnicodeWarning: base = PyExc_UnicodeWarning; break;
        case sipType_UserWarning: base = PyExc_UserWarning; break;

        default:
        {
            base = (PyObject *)sip_get_py_type(ms, ets->base_type_id,
                    &base_mod);
            if (base_mod == NULL)
                return NULL;
        }
    }

    PyObject *exc_type = PyErr_NewException(ets->base.tp_name, base, NULL);

    Py_XDECREF(base_mod);

    return (PyTypeObject *)exc_type;
}


/*
 * Create a mapped type object.
 */
static PyTypeObject *create_mapped_type(sipModuleState *ms, sipTypeNr type_nr,
        const sipMappedTypeSpec *mts)
{
    sipSipModuleState *sms = ms->sip_module_state;

    return create_container_type(ms,
            SIP_TYPE_ID_TYPE_MAPPED | SIP_TYPE_ID_LOCAL_MODULE | type_nr,
            &mts->base, NULL, NULL, NULL, mts->attrs,
            (PyObject *)sms->simple_wrapper_type, sms->wrapper_type_type);
}


/*
 * The type pickler, ie. the implementation of __reduce__.
 */
static PyObject *pickle_type(PyObject *self, PyTypeObject *defining_class,
        PyObject *const *Py_UNUSED(args), Py_ssize_t Py_UNUSED(nargs),
        PyObject *Py_UNUSED(kwd_args))
{
    const sipTypeSpec *ts = sip_get_type_spec_from_wt(
            (sipWrapperType *)defining_class);

    /*
     * Ask the handwritten pickle code for the tuple of arguments that will
     * recreate the object.
     */
    PyObject *init_args = ((const sipClassTypeSpec *)ts)->pickle(
            sip_get_cpp_ptr(self, defining_class));
    if (init_args == NULL)
        return NULL;

    if (!PyTuple_Check(init_args))
    {
        PyErr_Format(PyExc_TypeError,
                "%%PickleCode for type %s did not return a tuple",
                defining_class->tp_name);
        return NULL;
    }

    return Py_BuildValue("ON", Py_TYPE(self), init_args);
}


/*
 * Set the __reduce__ method for a type.
 */
static int set_reduce(PyTypeObject *py_type)
{
    static PyMethodDef pickler = {
        "_pickle_type", (PyCFunction)pickle_type, METH_METHOD|METH_FASTCALL|METH_KEYWORDS, NULL
    };

    /* Create the method descripter. */
    PyObject *descr = PyDescr_NewMethod(py_type, &pickler);
    if (descr == NULL)
        return -1;

    PyObject *reduce_s = PyUnicode_InternFromString("__reduce__");
    if (reduce_s == NULL)
    {
        Py_DECREF(descr);
        return -1;
    }

    /* Set the method. */
    int rc = PyObject_SetAttr((PyObject *)py_type, reduce_s, descr);
    Py_DECREF(reduce_s);
    Py_DECREF(descr);

    return rc;
}


/*
 * Return the type ID corresponding to the scope of the given wrapped type.
 */
static sipTypeID sip_api_type_scope(sipModuleState *ms, sipTypeID type_id)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return sipType_Invalid;

    sipTypeID scope_id = ts->scope_id;

    Py_DECREF(def_mod);

    return scope_id;
}


/*
 * Report an abstract method called with an unbound self.
 */
static void sip_api_abstract_method(const char *classname, const char *method)
{
    PyErr_Format(PyExc_TypeError,
            "%s.%s() is abstract and cannot be called as an unbound method",
            classname, method);
}


/*
 * Report a deprecated class or method with an optional message.
 */
int sip_api_deprecated(const char *classname, const char *method,
        const char *message)
{
    size_t bufsize = 100 + (message != NULL ? strlen(message) : 0);
    char *buf = (char *)sip_api_malloc(bufsize * sizeof(char));
    int written = 0;

    if (classname == NULL)
        written = PyOS_snprintf(buf, bufsize, "%s() is deprecated", method);
    else if (method == NULL)
        written = PyOS_snprintf(buf, bufsize, "%s constructor is deprecated",
				classname);
    else
        written = PyOS_snprintf(buf, bufsize, "%s.%s() is deprecated",
                classname, method);

    if (message != NULL)
        PyOS_snprintf(buf+written, bufsize-written, ": %s", message);

    int res = PyErr_WarnEx(PyExc_DeprecationWarning, buf, 1);
    sip_api_free(buf); 

    return res;
}


/*
 * Transfer ownership of a class instance to Python from C/C++.
 */
static void sip_api_transfer_back(sipModuleState *ms, PyObject *self)
{
    /* Note that we can't assume self is a SIP generated type. */
    if (self != NULL && PyObject_TypeCheck(self, ms->sip_module_state->wrapper_type))
        sip_transfer_back(ms->sip_module_state, self);
}


/*
 * Implement the transfer of ownership of a class instance to Python from
 * C/C++.
 */
void sip_transfer_back(sipSipModuleState *sms, PyObject *self)
{
    /* self's type is known to be valid at this point. */
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    if (sipCppHasRef(sw))
    {
        sipResetCppHasRef(sw);
        Py_DECREF(self);
    }
    else
    {
        Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);
        sip_remove_from_parent((sipWrapper *)sw);
        Py_END_CRITICAL_SECTION();
    }

    sipSetPyOwned(sw);
}


/*
 * Transfer ownership of a class instance to C/C++ from Python.
 */
static void sip_api_transfer_to(sipModuleState *ms, PyObject *self,
        PyObject *owner)
{
    sip_transfer_to(ms->sip_module_state, self, owner);
}


/*
 * Implement the transfer of ownership of a class instance to C/C++ from
 * Python.
 */
void sip_transfer_to(sipSipModuleState *sms, PyObject *self,
        PyObject *owner)
{
    /*
     * Note that we can't assume self is a SIP generated type.  There is a
     * legitimate case where we try to transfer a PyObject that may not be a
     * SIP generated class.  The virtual handler code calls this function to
     * keep the C/C++ instance alive when it gets rid of the Python object
     * returned by the Python method.  A class may have handwritten code that
     * converts a regular Python type - so we can't assume that we can simply
     * cast to sipWrapper.
     */

    if (self == NULL || !PyObject_TypeCheck(self, sms->wrapper_type))
        return;

    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

    if (owner == NULL)
    {
        /* There is no owner. */

        if (sipCppHasRef(sw))
        {
            sipResetCppHasRef(sw);
        }
        else
        {
            Py_INCREF(self);
            sip_remove_from_parent((sipWrapper *)sw);
            sipResetPyOwned(sw);
        }

        Py_DECREF(self);
    }
    else if (owner == Py_None)
    {
        /*
         * The owner is a C++ instance and not a Python object (ie. there is no
         * parent) so there is an explicit extra reference to keep this Python
         * object alive.  Note that there is no way to specify this from a .sip
         * file - it is useful when embedding in C/C++ applications.
         */

        if (!sipCppHasRef(sw))
        {
            Py_INCREF(self);
            sip_remove_from_parent((sipWrapper *)sw);
            sipResetPyOwned(sw);

            sipSetCppHasRef(sw);
        }
    }
    else if (PyObject_TypeCheck(owner, sms->wrapper_type))
    {
        /*
         * The owner is a Python object (ie. the C++ instance that the Python
         * object wraps).
         */

        if (sipCppHasRef(sw))
        {
            sipResetCppHasRef(sw);
        }
        else
        {
            Py_INCREF(self);
            sip_remove_from_parent((sipWrapper *)sw);
            sipResetPyOwned(sw);
        }

        sip_add_to_parent((sipWrapper *)sw, (sipWrapper *)owner);

        Py_DECREF(self);
    }

    Py_END_CRITICAL_SECTION();
}


/*
 * Return the address of the C/C++ instance.
 */
void *sip_api_get_address(sipSimpleWrapper *sw)
{
    return sw->data;
}


/*
 * Get the C/C++ pointer for a complex object and cast it to the required type.
 */
void *sip_get_complex_cpp_ptr(sipModuleState *ms, PyObject *w_inst,
        sipTypeID type_id)
{
    sipSimpleWrapper *sw = (sipSimpleWrapper *)w_inst;

    if (!sipIsDerived(sw))
    {
        PyErr_SetString(PyExc_RuntimeError,
                "no access to protected functions or signals for objects not created from Python");

        return NULL;
    }

    return sip_api_get_cpp_ptr(ms, w_inst, type_id);
}


/*
 * Get the C/C++ pointer from a wrapper and cast it to the required type.
 */
void *sip_api_get_cpp_ptr(sipModuleState *ms, PyObject *w_inst,
        sipTypeID type_id)
{
    PyObject *def_mod;
    PyTypeObject *py_type = sip_get_py_type(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return NULL;

    void *cpp = sip_get_cpp_ptr(w_inst, py_type);

    Py_DECREF(def_mod);

    return cpp;
}


/*
 * Implement the getting of the C/C++ pointer from a wrapper and optionally
 * cast it to the required type.
 */
void *sip_get_cpp_ptr(PyObject *w_inst, PyTypeObject *target_type)
{
    void *ptr = sip_api_get_address((sipSimpleWrapper *)w_inst);

    if (sip_check_pointer(ptr, w_inst) < 0)
        return NULL;

    if (target_type != NULL)
    {
        if (PyObject_TypeCheck(w_inst, target_type))
            ptr = sip_cast_cpp_ptr(ptr, Py_TYPE(w_inst), target_type);
        else
            ptr = NULL;

        if (ptr == NULL)
            PyErr_Format(PyExc_TypeError, "could not convert '%s' to '%s'",
                    Py_TYPE(w_inst)->tp_name, target_type->tp_name);
    }

    return ptr;
}


/*
 * Cast a C/C++ pointer from a source type to a destination type.
 */
void *sip_cast_cpp_ptr(void *ptr, PyTypeObject *src_type,
        PyTypeObject *target_type)
{
    sipWrapperType *src_wt = (sipWrapperType *)src_type;
    sipWrapperType *target_wt = (sipWrapperType *)target_type;

    sipCastFunc cast = ((const sipClassTypeSpec *)sip_get_type_spec_from_wt(
            src_wt))->cast;

    /* C structures and base classes don't have cast functions. */
    if (cast != NULL)
    {
        sipModuleState *src_ms = sip_get_module_state(src_wt->defining_module);
        const sipTypeSpec *target_ts = sip_get_type_spec_from_wt(target_wt);

        ptr = cast(src_ms, ptr, (const sipClassTypeSpec *)target_ts);
    }

    return ptr;
}


/*
 * Check that a pointer is non-NULL.
 */
int sip_check_pointer(void *ptr, PyObject *w_inst)
{
    if (ptr == NULL)
    {
        sipSimpleWrapper *sw = (sipSimpleWrapper *)w_inst;

        PyErr_Format(PyExc_RuntimeError, (sipWasCreated(sw) ?
                        "wrapped C/C++ object of type %s has been deleted" :
                        "super-class __init__() of type %s was never called"),
                Py_TYPE(w_inst)->tp_name);
        return -1;
    }

    return 0;
}


/*
 * Return TRUE if an object is owned by Python.
 */
static int sip_api_is_owned_by_python(sipSimpleWrapper *sw)
{
    return sipIsPyOwned(sw);
}


/*
 * Return TRUE if the type of a C++ instance is a derived class.
 */
static int sip_api_is_derived_class(sipSimpleWrapper *sw)
{
    return sipIsDerived(sw);
}


/*
 * Return a borrowed reference to the user defined object from a wrapped
 * instance.
 */
static PyObject *sip_api_get_user_object(sipSimpleWrapper *sw)
{
    return sw->user;
}


/*
 * Set the user defined object in a wrapped instance.  This steals a reference
 * to the object.
 */
static void sip_api_set_user_object(sipSimpleWrapper *sw, PyObject *user)
{
    /*
     * Note that there are multiple issues with the current implementation:
     * - the getter should return a strong reference
     * - only one object can be stored (there may be use cases where different
     *   modules each want to set an object).
     * We choose not to change the implementation for the moment and wait until
     * we have specific use cases to inform the design.
     */
    sw->user = user;
}


/*
 * Implement the normal transfer policy for the result of %ConvertToTypeCode,
 * ie. it is temporary unless it is being transferred from Python.
 */
int sip_api_get_state(PyObject *transferObj)
{
    return (transferObj == NULL || transferObj == Py_None) ? SIP_TEMPORARY : 0;
}


/*
 * The bsearch() helper function for searching a type definitions table.
 */
static int compare_type_spec(const void *key, const void *el)
{
    const char *s1 = (const char *)key;
    const char *s2 = (*(const sipTypeSpec **)el)->cpp_name;
    char ch1, ch2;

    /*
     * Compare while ignoring spaces so that we don't impose a rigorous naming
     * standard.  This only really affects template-based mapped types.
     */
    do
    {
        while ((ch1 = *s1++) == ' ')
            ;

        while ((ch2 = *s2++) == ' ')
            ;

        /* We might be looking for a pointer or a reference. */
        if ((ch1 == '*' || ch1 == '&' || ch1 == '\0') && ch2 == '\0')
            return 0;
    }
    while (ch1 == ch2);

    return (ch1 < ch2 ? -1 : 1);
}


/*
 * Return the type ID for a named type.
 */
static sipTypeID sip_api_find_type(sipModuleState *ms, const char *type)
{
    PyObject *module_list = ms->sip_module_state->module_list;
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(module_list); i++)
    {
        PyObject *mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(module_list, i), &mod) < 0)
            break;

        if (mod == NULL)
            continue;

        const sipModuleSpec *m_spec = sip_get_module_state(mod)->module_spec;

        /* Everything from here uses const specifications. */
        Py_DECREF(mod);

        const sipTypeSpec *const *ts_p = (const sipTypeSpec *const *)bsearch(
                (const void *)type, (const void *)m_spec->type_specs,
                m_spec->nr_type_specs, sizeof (const sipTypeSpec *),
                compare_type_spec);

        if (ts_p != NULL)
        {
            const sipTypeSpec *ts = *ts_p;

            if (sipTypeSpecIsExternal(ts))
                continue;

            /* Determine the type number. */
            sipTypeID type_nr = (sipTypeID)(ts_p - m_spec->type_specs);

            /* Determine the type of the type. */
            sipTypeID type_type;

            if (sipTypeSpecIsClass(ts) || sipTypeSpecIsNamespace(ts))
                type_type = SIP_TYPE_ID_TYPE_CLASS;
            else if (sipTypeSpecIsMapped(ts))
                type_type = SIP_TYPE_ID_TYPE_MAPPED;
            else
                type_type = SIP_TYPE_ID_TYPE_ENUM;

            /*
             * Return an absolute ID of a generated type.  Absolute types mean
             * that a type that this module knows nothing about can still be
             * referenced.
             */
            return type_type | SIP_TYPE_ID_ABSOLUTE | (sipTypeID)(i << 16) | type_nr;
        }
    }

    return sipType_Invalid;
}


/*
 * Call a hook.
 */
static void sip_api_call_hook(const char *hookname)
{
    /* Get the function hook. */
    PyObject *hook = PyImport_ImportModuleAttrString("builtins", hookname);
    if (hook == NULL)
        return;

    /* Call the hook and discard any result. */
    PyObject *res = PyObject_CallObject(hook, NULL);
    Py_XDECREF(res);
 
    Py_DECREF(hook);
}


/*
 * Raise an unknown exception.
 */
static void sip_api_raise_unknown_exception(PyObject **p_state_p)
{
    PyErr_SetObject(PyExc_Exception, PyUnicode_InternFromString("unknown"));
    sip_api_set_parser_error(p_state_p);
}


/*
 * Raise an exception implemented as a type.
 */
static void sip_api_raise_type_exception(sipModuleState *ms, sipTypeID type_id,
        void *ptr)
{
    PyObject *def_mod;
    PyTypeObject *py_type = sip_get_py_type(ms, type_id, &def_mod);
    if (def_mod == NULL)
        return;

    PyObject *self = sip_wrap_instance(ms, ptr, py_type, NULL, NULL,
            SIP_PY_OWNED);
    PyErr_SetObject((PyObject *)py_type, self);
    Py_XDECREF(self);

    Py_DECREF(def_mod);
}


/*
 * Return true if a type is a target type.
 */
static bool sip_api_is_target_type(sipModuleState *ms, sipTypeID type_id,
        const sipClassTypeSpec *target_cts)
{
    assert(sipTypeIsClass(type_id));

    PyObject *def_mod;
    const sipClassTypeSpec *cts = (const sipClassTypeSpec *)sip_get_type_spec(
            ms, type_id, &def_mod);
    if (def_mod == NULL)
        return false;

    bool matches = (cts == target_cts);

    Py_DECREF(def_mod);

    return matches;
}


/*
 * Return a C/C++ pointer of a target type cast from a supertype.
 */
static void *sip_api_cast_to_target_type(sipModuleState *ms, sipTypeID type_id,
        void *cpp, const sipClassTypeSpec *target_cts)
{
    assert(sipTypeIsClass(type_id));

    PyObject *def_mod;
    const sipClassTypeSpec *cts = (const sipClassTypeSpec *)sip_get_type_spec(
            ms, type_id, &def_mod);
    if (def_mod == NULL)
        return NULL;

    void *target_cpp = cts->cast(sip_get_module_state(def_mod), cpp,
            target_cts);

    Py_DECREF(def_mod);

    return target_cpp;
}


/*
 * Return a strong reference to the defining module of a type ID and the
 * defining type number.
 */
static PyObject *resolve_type_id(sipModuleState *ms, sipTypeID type_id,
        sipTypeNr *def_type_nr_p)
{
    if (!sipTypeIsClass(type_id) && !sipTypeIsMapped(type_id) && !sipTypeIsEnum(type_id) && !sipTypeIsException(type_id))
    {
        PyErr_Format(PyExc_TypeError,
                "type ID %0x does not refer to a wrapped type", type_id);
        return NULL;
    }

    /* Resolve external type IDs. */
    if (sipTypeIDIsExternal(type_id))
    {
        sipTypeNr type_nr = sipTypeIDTypeNr(type_id);

        /* The actual type ID is cached in the Python type. */
        type_id = ms->type_impls[type_nr].external_type_id;
        if (type_id == sipType_Invalid)
        {
            const sipTypeSpec *ts = ms->module_spec->type_specs[type_nr];
            assert(sipTypeSpecIsExternal(ts));

            if ((type_id = sip_api_find_type(ms, ts->cpp_name)) == sipType_Invalid)
            {
                PyErr_Format(PyExc_TypeError,
                        "type '%s' is defined in a module that hasn't been imported yet",
                        ts->cpp_name);
                return NULL;
            }

            ms->type_impls[type_nr].external_type_id = type_id;
        }
    }

    PyObject *def_mod;
    sipTypeNr def_type_nr = sipTypeIDTypeNr(type_id);

    if (sipTypeIDIsAbsolute(type_id))
    {
        if (PyWeakref_GetRef(PyList_GET_ITEM(ms->sip_module_state->module_list, sipTypeIDModuleNr(type_id)), &def_mod) < 0)
            return NULL;

        if (def_mod == NULL)
        {
            PyErr_Format(PyExc_TypeError,
                    "type ID %0x refers to a type in a module that is no longer imported",
                    type_id);
            return NULL;
        }
    }
    else if (sipTypeIDIsLocalModule(type_id))
    {
        def_mod = Py_NewRef(ms->wrapped_module);
    }
    else
    {
        sipImportedModule *im = &ms->imported_modules[sipTypeIDModuleNr(type_id)];

        def_mod = Py_NewRef(im->module);
        def_type_nr = im->type_nr_map[def_type_nr];
    }

    *def_type_nr_p = def_type_nr;

    return def_mod;
}


/*
 * Return a strong reference to the defining module of a type ID and
 * (optionally) the type specification and the Python type (if the type ID
 * corresponds to a class, or NULL if it doesn't).
 */
PyObject *sip_get_type_detail(sipModuleState *ms, sipTypeID type_id,
        const sipTypeSpec **ts_p, PyTypeObject **py_type_p)
{
    sipTypeNr def_type_nr;
    PyObject *def_mod = resolve_type_id(ms, type_id, &def_type_nr);
    if (def_mod == NULL)
        return NULL;

    sipModuleState *def_ms = sip_get_module_state(def_mod);

    /*
     * Note that we are called by traverse slots and so must not do anything
     * that might have side effects (like create a new type object).  However a
     * traverse slot will never ask for a type object so the following test
     * will always fail in that context, so everything is Ok.
     */
    if (py_type_p != NULL)
    {
        if (sip_get_local_py_type(def_ms, def_type_nr, py_type_p) < 0)
        {
            Py_DECREF(def_mod);
            return NULL;
        }
    }

    if (ts_p != NULL)
        *ts_p = def_ms->module_spec->type_specs[def_type_nr];

    return def_mod;
}


/*
 * Return a borrowed reference to the Python type that implements a wrapped
 * class and a strong reference to the defining module.
 */
PyTypeObject *sip_get_py_type(sipModuleState *ms, sipTypeID type_id,
        PyObject **def_mod_p)
{
    if (!sipTypeIsClass(type_id))
    {
        PyErr_SetString(PyExc_TypeError,
                "type does not refer to a wrapped class");
        return NULL;
    }

    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, NULL, &py_type);

    if (def_mod == NULL)
        return NULL;

    *def_mod_p = def_mod;

    return py_type;
}


/*
 * Return a strong reference to the Python type that implements a wrapped type.
 */
PyTypeObject *sip_api_get_py_type_ref(sipModuleState *ms, sipTypeID type_id)
{
    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, NULL, &py_type);

    if (def_mod == NULL)
        return NULL;

    Py_INCREF(py_type);
    Py_DECREF(def_mod);

    return py_type;
}


/*
 * Return the type specification for a type ID and a strong reference to the
 * defining module.
 */
const sipTypeSpec *sip_get_type_spec(sipModuleState *ms, sipTypeID type_id,
        PyObject **def_mod_p)
{
    const sipTypeSpec *ts;
    PyObject *def_mod = sip_get_type_detail(ms, type_id, &ts, NULL);
    if (def_mod == NULL)
        return NULL;

    *def_mod_p = def_mod;

    return ts;
}


/*
 * Return the type specification for a wrapper type.
 */
const sipTypeSpec *sip_get_type_spec_from_wt(sipWrapperType *wt)
{
    PyObject *def_mod;
    const sipTypeSpec *ts = sip_get_type_spec(
            sip_get_module_state(wt->defining_module), wt->type_id, &def_mod);
    assert(def_mod != NULL);

    /*
     * The wrapper type should be already protected (ie. there is a strong
     * reference to it somewhere) so there is no need to keep the defining
     * module reference.
     */
    Py_DECREF(def_mod);

    return ts;
}


/*
 * Return a borrowed reference to the Python type object for a type number in
 * the current module, creating it if necessary.  This is where new type
 * objects are created from the corresponding type numbers.
 */
int sip_get_local_py_type(sipModuleState *ms, sipTypeNr type_nr,
        PyTypeObject **py_type_p)
{
    PyTypeObject *py_type = ms->type_impls[type_nr].py_type;

    if (py_type == NULL)
    {
        const sipTypeSpec *ts = ms->module_spec->type_specs[type_nr];

        if (sipTypeSpecIsEnum(ts)
#if defined(SIP_CONFIGURATION_CustomEnums)
            || sipTypeSpecIsScopedEnum(ts)
#endif
            )
        {
            py_type = sip_create_enum_type(ms, type_nr,
                    (const sipEnumTypeSpec *)ts);

            if (py_type == NULL)
                return -1;
        }
        else if (sipTypeSpecIsMapped(ts))
        {
            if (ts->tp_name != NULL)
            {
                py_type = create_mapped_type(ms, type_nr,
                        (const sipMappedTypeSpec *)ts);

                if (py_type == NULL)
                    return -1;
            }
        }
        else if (sipTypeSpecIsException(ts))
        {
            py_type = create_exception_type(ms,
                    (const sipExceptionTypeSpec *)ts);

            if (py_type == NULL)
                return -1;
        }
        else
        {
            py_type = create_class_type(ms, type_nr,
                    (const sipClassTypeSpec *)ts);

            if (py_type == NULL)
                return -1;
        }

        ms->type_impls[type_nr].py_type = py_type;
    }

    *py_type_p = py_type;

    return 0;
}


/*
 * Return true if self was passed as an argument (ie. it doesn't have the type
 * expected of self).
 */
static bool sip_api_self_is_arg(sipModuleState *ms, PyObject *self,
        sipTypeID type_id)
{
    PyObject *def_mod;
    PyTypeObject *py_type = sip_get_py_type(ms, type_id, &def_mod);
    assert(def_mod != NULL);
    assert(py_type != NULL);

    bool was_arg = (!PyObject_TypeCheck(self, py_type) || sipIsDerived((sipSimpleWrapper *)self));

    Py_DECREF(def_mod);

    return was_arg;
}


/*
 * If the given name is that of a typedef then the name of the corresponding
 * type is returned.  If not then the original name is returned.
 */
static const char *sip_api_resolve_typedef(sipModuleState *ms,
        const char *name)
{
    PyObject *module_list = ms->sip_module_state->module_list;
    Py_ssize_t i;

    /*
     * Note that if the same name is defined as more than one type (which is
     * possible if more than one completely independent modules are being
     * used) then we might pick the wrong one.
     */
    for (i = 0; i < PyList_GET_SIZE(module_list); i++)
    {
        PyObject *mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(module_list, i), &mod) < 0)
            break;

        if (mod == NULL)
            continue;

        const sipModuleSpec *m_spec = sip_get_module_state(mod)->module_spec;

        /* Everything from here uses const specifications. */
        Py_DECREF(mod);

        if (m_spec->nr_typedefs > 0)
        {
            const sipTypedefSpec *tds = (const sipTypedefSpec *)bsearch(name,
                    m_spec->typedefs, m_spec->nr_typedefs,
                    sizeof (sipTypedefSpec), compare_typedef_name);

            if (tds != NULL)
                return tds->type_name;
        }
    }

    return name;
}


/*
 * The bsearch() helper function for searching a sorted typedef table.
 */
static int compare_typedef_name(const void *key, const void *el)
{
    return strcmp((const char *)key, ((const sipTypedefSpec *)el)->name);
}


/*
 * Add a Python type object to a list.  Return 0 if there was no error.
 */
int sip_append_py_object_to_list(sipSipModuleState *sms, PyObject **listp,
        PyObject *object)
{
    int rc;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

    PyObject *list = *listp;

    if (list != NULL)
    {
        rc = PyList_Append(list, object);
    }
    else if ((list = PyList_New(1)) != NULL)
    {
        PyList_SET_ITEM(list, 0, Py_NewRef(object));
        *listp = list;
        rc = 0;
    }
    else
    {
        rc = -1;
    }

    Py_END_CRITICAL_SECTION();

    return rc;
}


/*
 * Register an array of exported symbols.
 */
static void sip_api_export_symbols(sipModuleState *ms,
        const sipSymbolSpec *const symbols)
{
    ms->symbols = symbols;
}


/*
 * Return the symbol registered with the given name.  NULL is returned (but no
 * exception set) if the name was not registered.
 */
static void *sip_api_import_symbol(sipModuleState *ms, const char *name)
{
    /* First check this module. */
    const sipSymbolSpec *ss = lookup_symbol(ms, name);

    /*
     * If it is not local then check the modules that this module imports.
     * This guarantees the module defining the symbol won't be garbage
     * collected while we are using it and we provide some protection against
     * modules accidentally using the same name for different symbols.
     */
    if (ss == NULL && ms->imported_modules != NULL)
    {
        sipModuleNr mi;

        for (mi = 0; mi < ms->module_spec->nr_import_specs; mi++)
        {
            sipModuleState *ims = sip_get_module_state(
                    ms->imported_modules[mi].module);

            if ((ss = lookup_symbol(ims, name)) != NULL)
                break;
        }
    }

    if (ss == NULL)
        return NULL;

    return ss->symbol;
}


/*
 * Lookup a symbol name in a module.
 */
static const sipSymbolSpec *lookup_symbol(sipModuleState *ms, const char *name)
{
    /*
     * It's possible that another thread could replace the symbol table while
     * we are doing this but as symbols should be static it just means an old
     * one would be returned.
     */
    const sipSymbolSpec *ss = ms->symbols;

    if (ss != NULL)
    {
        while (ss->name != NULL)
        {
            if (strcmp(ss->name, name) == 0)
                return ss;

            ss++;
        }
    }

    return NULL;
}


/*
 * Return the function that converts a C++ instance to a Python object.
 */
sipConvertFromFunc sip_get_from_convertor(PyTypeObject *w_type,
        const sipTypeSpec *ts)
{
    if (sipTypeSpecIsMapped(ts))
        return ((const sipMappedTypeSpec *)ts)->cfrom;

    assert(sipTypeSpecIsClass(ts) && w_type != NULL);

    sipWrapperType *wt = (sipWrapperType *)w_type;

    if (wt->autoconversion_disabled)
        return NULL;

    return ((const sipClassTypeSpec *)ts)->cfrom;
}


/*
 * Enable or disable the auto-conversion.  Returns the previous enabled state.
 */
static int sip_api_enable_autoconversion(sipModuleState *ms,
        PyTypeObject *py_type, int enable)
{
    if (!is_a_wrapped_type(ms, py_type))
        return -1;

    sipWrapperType *wt = (sipWrapperType *)py_type;

    int was_enabled = !wt->autoconversion_disabled;
    wt->autoconversion_disabled = !enable;

    return was_enabled;
}


/*
 * Get the final C/C++ address.  This is called before we wrap a C/C++ instance
 * (either a mapped type, a C struct or a C++ class) to allow the bindings to
 * replace it with another instance (eg. a proxy).  NULL is returned and an
 * exception raised if there was an error.
 */
void *sip_get_final_address(sipSipModuleState *sms, PyTypeObject *py_type,
        void *cpp)
{
    /* Invoke any event handlers. */
    PyObject *mods = sms->module_list;
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(mods); i++)
    {
        PyObject *ehs_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, i), &ehs_mod) < 0)
            return NULL;

        if (ehs_mod == NULL)
            continue;

        sipModuleState *ehs_ms = sip_get_module_state(ehs_mod);

        if (ehs_ms->event_handlers != NULL)
        {
            const sipEventHandlerSpec *ehs = ehs_ms->event_handlers;

            while (ehs->event_type != sipEventNone)
            {
                if (ehs->event_type == sipEventFinalisingAddress && sip_is_subtype(ehs_ms, py_type, ehs->type_id))
                {
                    sipFinalisingAddressEventHandler handler = (sipFinalisingAddressEventHandler)ehs->handler;

                    if ((cpp = handler(ehs_ms, ehs->type_id, cpp)) == NULL)
                    {
                        Py_DECREF(ehs_mod);
                        return NULL;
                    }
                }

                ehs++;
            }
        }

        Py_DECREF(ehs_mod);
    }

    return cpp;
}


/*
 * Set the user-specific type data.
 */
static void sip_api_set_type_user_object(sipWrapperType *wt, PyObject *data)
{
    /*
     * Note that there are similar issues to those with the user object stored
     * in a wrapped instance.  Likewise we choose to wait for specific use
     * cases to inform a better implementation.
     */
    Py_BEGIN_CRITICAL_SECTION(wt);
    Py_XSETREF(wt->user_data, Py_XNewRef(data));
    Py_END_CRITICAL_SECTION();
}


/*
 * Get the user-specific type data.
 */
static PyObject *sip_api_get_type_user_object(sipWrapperType *wt)
{
    return Py_XNewRef(wt->user_data);
}


/*
 * See if a type is user defined.
 */
static int sip_api_is_user_type(sipWrapperType *wt)
{
    return wt->user_type;
}


/*
 * Register a module's event handlers.
 */
static int sip_api_register_event_handlers(sipModuleState *ms,
        const sipEventHandlerSpec *const handlers)
{
    assert(handlers != NULL);

    /* Check the handlers are valid. */
    Py_ssize_t i;

    for (i = 0; handlers[i].event_type != sipEventNone; i++)
    {
        const sipEventHandlerSpec *ehs = &handlers[i];

        if (ehs->event_type == sipEventCollectingModule)
            continue;

        sipTypeID type_id = ehs->type_id;

        if (!sipTypeIDIsLocalModule(type_id) || !(sipTypeIsClass(type_id) || sipTypeIsMapped(type_id)))
        {
            PyErr_SetString(PyExc_SystemError,
                    "an event handler can only be registered for a class or "
                    "mapped type by the defining module");
            return -1;
        }
    }

    ms->event_handlers = handlers;

    return 0;
}


/*
 * Returns TRUE if a type is a sub-type of a wrapped type.
 */
int sip_is_subtype(sipModuleState *ms, PyTypeObject *py_type,
        sipTypeID type_id)
{
    /*
     * We know that the type ID is local so we go directly to its
     * implementation.  If there isn't one then the type being checked can't be
     * a sub-type.
     */
    PyTypeObject *base_type = ms->type_impls[sipTypeIDTypeNr(type_id)].py_type;
    if (base_type == NULL)
        return FALSE;

    return PyType_IsSubtype(py_type, base_type);
}


/*
 * Unpack a slice object.
 */
int sip_api_convert_from_slice_object(PyObject *slice, Py_ssize_t length,
        Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step,
        Py_ssize_t *slicelength)
{
    if (PySlice_Unpack(slice, start, stop, step) < 0)
        return -1;

    *slicelength = PySlice_AdjustIndices(length, start, stop, *step);

    return 0;
}


/*
 * Call a visitor function for every wrapped object.
 */
static void sip_api_visit_wrappers(sipModuleState *ms,
        sipWrapperVisitorFunc visitor, void *closure)
{
    sipSipModuleState *sms = ms->sip_module_state;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);
    sip_om_visit_wrappers(&sms->object_map, visitor, closure);
    Py_END_CRITICAL_SECTION();
}


/*
 * Raise an exception when there is no mapped type converter to convert from
 * C/C++ to Python.
 */
void sip_raise_no_convert_from(const sipTypeSpec *td)
{
    PyErr_Format(PyExc_TypeError, "%s cannot be converted to a Python object",
            td->cpp_name);
}


/*
 * Return the next exception handler and a strong reference to its defining
 * module.  The order is undefined.
 */
static sipExceptionHandler sip_api_next_exception_handler(sipModuleState *ms,
        PyObject **handler_mod_p, sipModuleState **handler_ms_p,
        Py_ssize_t *state_p)
{
    PyObject *module_list = ms->sip_module_state->module_list;
    Py_ssize_t i;

    for (i = *state_p; i < PyList_GET_SIZE(module_list); i++)
    {
        PyObject *handler_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(module_list, i), &handler_mod) < 0)
            break;

        if (handler_mod == NULL)
            continue;

        sipModuleState *handler_ms = sip_get_module_state(handler_mod);
        sipExceptionHandler eh = handler_ms->module_spec->exception_handler;

        if (eh != NULL)
        {
            *handler_mod_p = handler_mod;
            *handler_ms_p = handler_ms;
            *state_p = i;

            return eh;
        }

        Py_DECREF(handler_mod);
    }

    return NULL;
}


/*
 * Check if a type object is a wrapped type object and raise an exception if it
 * isn't.
 */
static bool is_a_wrapped_type(sipModuleState *ms, PyTypeObject *py_type)
{
    if (PyObject_TypeCheck((PyObject *)py_type, ms->sip_module_state->wrapper_type_type))
        return true;

    PyErr_Format(PyExc_TypeError, "'%s' is not a wrapped type",
            py_type->tp_name);

    return false;
}
