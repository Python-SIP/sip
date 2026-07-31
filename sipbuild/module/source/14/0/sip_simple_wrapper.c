/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * The implementation of the sip simple wrapper type.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include <stddef.h>

#include "sip_simple_wrapper.h"

#include "sip_attribute.h"
#include "sip_core.h"
#include "sip_iterators.h"
#include "sip_object_map.h"
#include "sip_parsers.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"
#include "sip_wrapper.h"
#include "sip_wrapper_type.h"


/*
 * The type's getters and setters.
 */
static PyObject *SimpleWrapper_get_dict(sipSimpleWrapper *self, void *closure);
static int SimpleWrapper_set_dict(sipSimpleWrapper *self, PyObject *value,
        void *closure);

static PyGetSetDef SimpleWrapper_getset[] = {
    {"__dict__", (getter)SimpleWrapper_get_dict, (setter)SimpleWrapper_set_dict},
    {0}
};


/*
 * The type's members.
 */
static PyMemberDef SimpleWrapper_members[] = {
    {"__dictoffset__", Py_T_PYSSIZET, offsetof(sipSimpleWrapper, dict), Py_READONLY},
    {0}
};


/*
 * The type's methods.
 */
static PyObject *SimpleWrapper_dir(sipSimpleWrapper *self, PyObject *args);

static PyMethodDef SimpleWrapper_methods[] = {
    {"__dir__", (PyCFunction)SimpleWrapper_dir, METH_NOARGS, NULL},
    {0}
};


/* Forward declarations of slot functions. */
static int SimpleWrapper_clear(PyObject *self);
static void SimpleWrapper_dealloc(PyObject *self);
static PyObject *SimpleWrapper_new(PyTypeObject *cls, PyObject *args,
        PyObject *kwds);
static int SimpleWrapper_traverse(PyObject *self, visitproc visit, void *arg);


/*
 * The type slots.
 */
static PySlot SimpleWrapper_slots[] = {
    PySlot_STATIC_DATA(Py_tp_name, _SIP_TYPE_NAME_PREFIX ".simplewrapper"),
    PySlot_SIZE(Py_tp_basicsize, sizeof (sipSimpleWrapper)),
    PySlot_UINT64(Py_tp_flags,
            Py_TPFLAGS_DEFAULT |
            Py_TPFLAGS_BASETYPE |
            Py_TPFLAGS_HAVE_GC),
    PySlot_STATIC_DATA(Py_tp_getset, SimpleWrapper_getset),
    PySlot_STATIC_DATA(Py_tp_members, SimpleWrapper_members),
    PySlot_STATIC_DATA(Py_tp_methods, SimpleWrapper_methods),
    PySlot_FUNC(Py_tp_clear, SimpleWrapper_clear),
    PySlot_FUNC(Py_tp_dealloc, SimpleWrapper_dealloc),
    PySlot_FUNC(Py_tp_new, SimpleWrapper_new),
    PySlot_FUNC(Py_tp_traverse, SimpleWrapper_traverse),
    PySlot_END
};


/* The closure used when iterating over init extenders. */
typedef struct {
    PyObject **p_state_p;
    PyObject *self;
    PyObject *const *args;
    Py_ssize_t nr_args;
    PyObject *kwd_names;
    PyObject **unused_p;
    PyObject **owner_p;
    void *cpp;
} InitClosure;


/* Remaining forward declarations. */
static int call_super_init(PyTypeObject *super_type, PyObject *self,
        PyObject *kwds);
static sipFinalFunc find_finalisation(sipModuleState *ms,
        const sipClassTypeSpec *cts, PyObject **final_mod_p);
static const sipClassTypeSpec *get_class_type_spec_from_self(PyObject *self);
static void *init_extend(sipModuleState *ms, PyObject **p_state_p,
        PyObject *self, PyObject *const *args, Py_ssize_t nr_args,
        PyObject *kwd_names, PyObject **unused_p, PyObject **owner_p,
        const sipTypeSpec *extending_ts);
static int init_extender_handler(sipModuleState *x_ms,
        const sipInitExtenderSpec *x_init_extender_spec, void *closure);
static int vectorcall_create(PyObject *args, PyObject *kwargs,
        PyObject **small_argv, Py_ssize_t *argv_len_p, PyObject ***argv_p,
        Py_ssize_t *nr_pos_args_p, PyObject **kw_names_p);
static void vectorcall_dispose(PyObject **small_argv, PyObject **argv,
        Py_ssize_t argv_len, PyObject *kw_names);


/*
 * The simple wrapper clear slot.
 */
static int SimpleWrapper_clear(PyObject *self)
{
    sipWrapperType *wt = (sipWrapperType *)Py_TYPE(self);
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;
    int vret = 0;

    /*
     * Call any handwritten clear code.  Note that this can be called after the
     * the C/C++ instance has been destroyed (because we can be called by
     * sipWrapper_dealloc()).  This feels wrong but we retain this historical
     * behaviour as it doesn't seem to have caused problems in the wild.
     */
    sipClearFunc clear = ((const sipClassTypeSpec *)sip_get_type_spec_from_wt(wt))->clear;

    if (clear != NULL)
        vret = clear(sw->data);

    Py_CLEAR(sw->dict);
    Py_CLEAR(sw->extra_refs);
    Py_CLEAR(sw->mixin_main);
    Py_CLEAR(sw->mixins);
    Py_CLEAR(sw->user);

    /* Handle any children if the type supports the concept. */
    if (wt->is_wrapper)
    {
        sipWrapper *w = (sipWrapper *)self;

        /* Avoid a compiler warning when the GIL is enabled. */
#if defined(Py_GIL_DISABLED)
        sipSipModuleState *sms = sip_get_module_state(wt->defining_module)->sip_module_state;
#endif

        Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

        /* Detach any children (which will be owned by C/C++). */
        while (w->first_child != NULL)
            sip_remove_from_parent(w->first_child);

        Py_END_CRITICAL_SECTION();
    }

    return vret;
}


/*
 * The simple wrapper dealloc slot.
 */
static void SimpleWrapper_dealloc(PyObject *self)
{
    PyObject_GC_UnTrack(self);

    /*
     * Remove the object from the map and call the C/C++ dtor if we own the
     * instance.
     */
    sipWrapperType *wt = (sipWrapperType *)Py_TYPE(self);
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;
    sipModuleState *ms = sip_get_module_state(wt->defining_module);

    /* Invoke any event handlers. */
    PyObject *mods = ms->sip_module_state->module_list;
    Py_ssize_t i;

    for (i = 0; i < PyList_GET_SIZE(mods); i++)
    {
        PyObject *ehs_mod;

        if (PyWeakref_GetRef(PyList_GET_ITEM(mods, i), &ehs_mod) < 0)
            break;

        if (ehs_mod == NULL)
            continue;

        sipModuleState *ehs_ms = sip_get_module_state(ehs_mod);

        if (ehs_ms->event_handlers != NULL)
        {
            const sipEventHandlerSpec *ehs = ehs_ms->event_handlers;

            while (ehs->event_type != sipEventNone)
            {
                if (ehs->event_type == sipEventCollectingWrapper && sip_is_subtype(ehs_ms, (PyTypeObject *)wt, ehs->type_id))
                {
                    sipCollectingWrapperEventHandler handler = (sipCollectingWrapperEventHandler)ehs->handler;

                    handler(ehs_ms, ehs->type_id, sw);
                }

                ehs++;
            }
        }

        Py_DECREF(ehs_mod);
    }

    /*
     * Remove the object from the map before calling the class specific dealloc
     * code.  This code calls the C++ dtor and may result in further calls that
     * pass the instance as an argument.  If this is still in the map then it's
     * reference count would be increased (to one) and bad things happen when
     * it drops back to zero again.  (An example is PyQt events generated
     * during the dtor call being passed to an event filter implemented in
     * Python.)  By removing it from the map first we ensure that a new Python
     * object is created.
     */
    Py_BEGIN_CRITICAL_SECTION_MUTEX(&ms->sip_module_state->mutex);
    sip_om_remove_object(ms, sw);
    Py_END_CRITICAL_SECTION();

    sipDeallocFunc dealloc = ((const sipClassTypeSpec *)sip_get_type_spec_from_wt(wt))->dealloc;

    if (dealloc != NULL)
        dealloc(sw);

    /*
     * Now that the C++ object no longer exists (as far as we are concerned) we
     * can tidy up the Python object.
     */
    SimpleWrapper_clear(self);

    PyTypeObject *py_type = (PyTypeObject *)wt;
    py_type->tp_free(self);
    Py_DECREF(py_type);
}


/*
 * The simple wrapper get buffer slot.
 */
int SimpleWrapper_getbuffer(PyObject *self, Py_buffer *buf, int flags)
{
    const sipClassTypeSpec *cts = get_class_type_spec_from_self(self);

    return cts->getbuffer(self, ((sipSimpleWrapper *)self)->data, buf, flags);
}


/*
 * The simple wrapper new slot.
 */
static PyObject *SimpleWrapper_new(PyTypeObject *cls,
        PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwds))
{
    sipSipModuleState *sms = sip_get_sip_module_state_from_type(cls);

    /* Check the base types are not being used directly. */
    if (cls == sms->simple_wrapper_type || cls == sms->wrapper_type)
    {
        PyErr_Format(PyExc_TypeError, "%s cannot be instantiated",
                cls->tp_name);
        return NULL;
    }

    sipWrapperType *wt = (sipWrapperType *)cls;
    const sipTypeSpec *ts = sip_get_type_spec_from_wt(wt);

    /* See if it is a mapped type. */
    if (sipTypeSpecIsMapped(ts))
    {
        PyErr_Format(PyExc_TypeError,
                "%s represents a mapped type and cannot be instantiated",
                cls->tp_name);

        return NULL;
    }

    /* See if it is a namespace. */
    if (sipTypeSpecIsNamespace(ts))
    {
        PyErr_Format(PyExc_TypeError,
                "%s represents a C++ namespace and cannot be instantiated",
                cls->tp_name);

        return NULL;
    }

    /*
     * See if it cannot be instantiated or sub-classed from Python, eg. it's an
     * opaque class.  Some restrictions might be overcome with better SIP
     * support.
     */
    if (((sipClassTypeSpec *)ts)->init == NULL)
    {
        PyErr_Format(PyExc_TypeError,
                "%s cannot be instantiated or sub-classed", cls->tp_name);

        return NULL;
    }

    /* See if it is an abstract type. */
    if (sipTypeSpecIsAbstract(ts) && !wt->user_type)
    {
        PyErr_Format(PyExc_TypeError,
                "%s wraps a C++ abstract class and cannot be instantiated",
                cls->tp_name);

        return NULL;
    }

    /* Call the standard super-type new. */
    return PyBaseObject_Type.tp_new(cls, sms->empty_tuple, NULL);
}


/*
 * The simple wrapper release buffer slot.
 */
void SimpleWrapper_releasebuffer(PyObject *self, Py_buffer *buf)
{
    const sipClassTypeSpec *cts = get_class_type_spec_from_self(self);

    cts->releasebuffer(self, ((sipSimpleWrapper *)self)->data, buf);
}


/*
 * The simple wrapper traverse slot.
 */
static int SimpleWrapper_traverse(PyObject *self, visitproc visit,
        void *arg)
{
    sipWrapperType *wt = (sipWrapperType *)Py_TYPE(self);
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    Py_VISIT(Py_TYPE(self));

    /* Call any handwritten traverse code. */
    sipModuleState *ms = (sipModuleState *)PyModule_GetState_DuringGC(
            wt->defining_module);

    if (ms != NULL)
    {
        // Note that we don't use sip_get_type_spec_from_wt() because we have
        // to use PyModule_GetState_DuringGC().
        PyObject *def_mod;
        const sipClassTypeSpec *cts = (const sipClassTypeSpec *)sip_get_type_spec(ms, wt->type_id, &def_mod);

        if (def_mod != NULL)
        {
            if (cts->traverse != NULL)
            {
                int vret = cts->traverse(sw->data, visit, arg);

                if (vret != 0)
                {
                    Py_DECREF(def_mod);
                    return vret;
                }
            }

            Py_DECREF(def_mod);
        }
    }

    Py_VISIT(sw->dict);
    Py_VISIT(sw->extra_refs);
    Py_VISIT(sw->mixin_main);
    Py_VISIT(sw->mixins);
    Py_VISIT(sw->user);

    /* Handle any children if the type supports the concept. */
    if (wt->is_wrapper)
    {
        /* Note that we don't lock the main mutex. */

        sipWrapper *w = ((sipWrapper *)sw)->first_child;

        while (w != NULL)
        {
            /*
             * We don't traverse if the wrapper is a child of itself.  We do
             * this so that wrapped objects returned by virtual methods with
             * the /Factory/ don't have those objects collected.  This then
             * means that plugins implemented in Python have a chance of
             * working.
             */
            if (w != (sipWrapper *)sw)
            {
                int vret = visit((PyObject *)w, arg);

                if (vret != 0)
                    return vret;
            }

            w = w->sibling_next;
        }
    }

    return 0;
}


/*
 * The __dict__ getter.
 */
static PyObject *SimpleWrapper_get_dict(sipSimpleWrapper *self,
        void *Py_UNUSED(closure))
{
    /* Create the dictionary if needed. */
    if (self->dict == NULL)
        self->dict = PyDict_New();

    return Py_XNewRef(self->dict);
}


/*
 * The __dict__ setter.
 */
static int SimpleWrapper_set_dict(sipSimpleWrapper *self, PyObject *value,
        void *Py_UNUSED(closure))
{
    /* Check that any new value really is a dictionary. */
    if (value != NULL && !PyDict_Check(value))
    {
        PyErr_Format(PyExc_TypeError,
                "__dict__ must be set to a dictionary, not a %T", value);
        return -1;
    }

    Py_CLEAR(self->dict);
    self->dict = Py_XNewRef(value);

    return 0;
}


/*
 * The __dir__() implementation.
 */
static PyObject *SimpleWrapper_dir(sipSimpleWrapper *self,
        PyObject *Py_UNUSED(args))
{
    /* Get the instance attributes. */
    PyObject *attr_dict = (self->dict != NULL ?
            PyDict_Copy(self->dict) : PyDict_New());
    if (attr_dict == NULL)
        return NULL;

    PyObject *dir = sip_dir_of_wt((sipWrapperType *)Py_TYPE(self), attr_dict);
    Py_DECREF(attr_dict);

    return dir;
}


/*
 * The implementation of the tp_init slot for wrappers.  We don't implement
 * this as part of SimpleWrapper because we want to be called for each wrapped
 * type in the MRO so that keyword arguments are extracted in the correct
 * order.
 */
int sip_api_init_slot_impl(PyObject *self, PyObject *args, PyObject *kwargs,
        void *module_token, sipTypeID type_id)
{
    /* Get the module state. */
    PyTypeObject *self_type = Py_TYPE(self);

    PyObject *mod = PyType_GetModuleByToken(self_type, module_token);
    if (mod == NULL)
        return -1;

    sipModuleState *ms = sip_get_module_state(mod);
    sipSipModuleState *sms = ms->sip_module_state;

    /* Get the type and its specification. */
    const sipClassTypeSpec *cts;
    PyTypeObject *py_type;
    PyObject *def_mod = sip_get_type_detail(ms, type_id,
            (const sipTypeSpec **)&cts, &py_type);
    if (def_mod == NULL)
        return -1;

    PyObject *final_mod = Py_NewRef(def_mod);

    /*
     * Find our position in the MRO and if there are any other wrapper types
     * earlier than us (in which case we are being used as a mixin).
     */
    Py_ssize_t mixin_i = sip_get_mixin_index(sms, (PyObject *)self_type,
            (PyObject *)py_type);

    /*
     * If we are a mixin then create a Python object for it, otherwise use
     * self.
     */
    PyObject *inst;

    if (mixin_i >= 0)
    {
        inst = py_type->tp_new(py_type, sms->empty_tuple, NULL);
        if (inst == NULL)
            goto gc_def_mod;

        /* Add the mixin instance to the main object. */
        ((sipSimpleWrapper *)inst)->mixin_main = (sipSimpleWrapper *)Py_NewRef(self);

        if (((sipSimpleWrapper *)self)->mixins == NULL)
            if ((((sipSimpleWrapper *)self)->mixins = PyList_New(0)) == NULL)
                goto gc_inst;

        assert(PyList_Size(((sipSimpleWrapper *)self)->mixins) == mixin_i);

        if (PyList_Append(((sipSimpleWrapper *)self)->mixins, inst) < 0)
            goto gc_inst;
    }
    else
    {
        /*
         * Check that there isn't already a C/C++ instance.  This can happen if
         * sipConvertFromNewPyType() has been called with a user-defined Python
         * type that has an __init__ implementation that calls it's super-type
         * __init__.  If so we just ignore the call.
         */
        if (((sipSimpleWrapper *)self)->data != NULL)
        {
            Py_DECREF(py_type);
            return 0;
        }

        inst = Py_NewRef(self);
    }

    /* See if we are interested in any unused keyword arguments. */
    sipFinalFunc final_func = find_finalisation(ms, cts, &final_mod);
    PyObject *unused = NULL;
    PyObject **unused_p = NULL;

    if (sipTypeSpecCallSuperInit(&cts->base) || final_func != NULL)
        unused_p = &unused;

    /* Call the C++ ctor. */
    void *cpp;
    PyObject *owner = NULL;

    /* Convert the traditional arguments to vectorcall style. */
#define SMALL_ARGV 16
    PyObject *small_argv[SMALL_ARGV];
    Py_ssize_t argv_len = SMALL_ARGV;

    PyObject *p_state = NULL;
    PyObject **argv, *kw_names;
    Py_ssize_t nr_pos_args;

    if (vectorcall_create(args, kwargs, small_argv, &argv_len, &argv, &nr_pos_args, &kw_names) < 0)
        goto gc_inst;

    cpp = cts->init(ms, &p_state, inst, argv, nr_pos_args, kw_names, unused_p,
            &owner);

    vectorcall_dispose(small_argv, argv, argv_len, kw_names);

    int flags = 0;

    if (cpp != NULL)
    {
        flags |= SIP_DERIVED_CLASS;
    }
    else if (p_state == Py_None)
    {
        /*
         * The C++ ctor must have raised an exception which has been translated
         * to a Python exception.
         */
        Py_DECREF(p_state);
        goto gc_inst;
    }
    else
    {
        cpp = init_extend(ms, &p_state, inst, argv, nr_pos_args, kw_names,
                unused_p, &owner, &cts->base);

        if (cpp == NULL)
        {
            sip_no_callable(p_state, NULL, cts->base.tp_name);
            goto gc_inst;
        }
    }

    /* Complete the basic initialisation. */
    ((sipSimpleWrapper *)inst)->data = cpp;
    ((sipSimpleWrapper *)inst)->flags = flags | SIP_CREATED;
    Py_DECREF(inst);

    if (mixin_i < 0)
    {
        if (owner == NULL)
        {
            ((sipSimpleWrapper *)self)->flags |= SIP_PY_OWNED;
        }
        else if ((PyObject *)owner == Py_None)
        {
            /* This is the hack that means that C++ owns the new instance. */
            ((sipSimpleWrapper *)self)->flags |= SIP_CPP_HAS_REF;
            Py_INCREF(self);
            owner = NULL;
        }

        Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

        /* Handle any owner if the type supports the concept. */
        if (((sipWrapperType *)self_type)->is_wrapper)
        {
            /*
             * The application may be doing something very unadvisable (like
             * calling __init__() for a second time), so make sure we don't
             * already have a parent.
             */
            sip_remove_from_parent((sipWrapper *)self);

            if (owner != NULL)
            {
                assert(PyObject_TypeCheck(owner, sms->wrapper_type));

                sip_add_to_parent((sipWrapper *)self, (sipWrapper *)owner);
            }
        }

        sip_om_add_object(ms, (sipSimpleWrapper *)self);

        Py_END_CRITICAL_SECTION();

        /* Call any finalisation code. */
        if (final_func != NULL)
        {
            PyObject *new_unused = NULL, **new_unused_p;

            if (unused == NULL || unused != kwargs)
            {
                /*
                 * There are no unused arguments or we have already created a
                 * dict containing the unused sub-set, so there is no need to
                 * create another.
                 */
                new_unused_p = NULL;
            }
            else
            {
                /*
                 * All of the keyword arguments are unused, so if some of them
                 * are now going to be used then a new dict will be needed.
                 */
                new_unused_p = &new_unused;
            }
            
            if (final_func(self, cpp, unused, new_unused_p) < 0)
            {
                Py_XDECREF(unused);
                goto gc_def_mod;
            }

            if (new_unused != NULL)
            {
                Py_DECREF(unused);
                unused = new_unused;
            }
        }
    }

    /* See if we should call the equivalent of super().__init__(). */
    if (sipTypeSpecCallSuperInit(&cts->base))
    {
        /* Find this type's position in the MRO. */
        Py_ssize_t mro_i;
        PyObject *mro = self_type->tp_mro;

        for (mro_i = 0; mro_i < PyTuple_GET_SIZE(mro); mro_i++)
            if (PyTuple_GET_ITEM(mro, mro_i) == (PyObject *)py_type)
                break;

        assert(mro_i < PyTuple_GET_SIZE(mro));

        /* Move to the next significant type in the MRO. */
        mro_i++;
        PyTypeObject *super_type = (PyTypeObject *)PyTuple_GET_ITEM(mro,
                mro_i);

        /* Skip wrapper. */
        if (super_type == sms->wrapper_type)
        {
            mro_i++;
            super_type = (PyTypeObject *)PyTuple_GET_ITEM(mro, mro_i);
        }

        /* Skip simplewrapper. */
        if (super_type == sms->simple_wrapper_type)
        {
            mro_i++;
            super_type = (PyTypeObject *)PyTuple_GET_ITEM(mro, mro_i);
        }

        int rc = call_super_init(super_type, self, unused);

        Py_XDECREF(unused);

        if (rc < 0)
            goto gc_def_mod;
    }

    Py_DECREF(final_mod);
    Py_DECREF(def_mod);

    return 0;

gc_inst:
    Py_DECREF(inst);

gc_def_mod:
    Py_DECREF(final_mod);
    Py_DECREF(def_mod);

    return -1;
}


/*
 * Return the mixin index of a defining type for a Python type or -1 if the
 * defining type is not a mixin.
 */
Py_ssize_t sip_get_mixin_index(sipSipModuleState *sms, PyObject *py_type,
        PyObject *defining_type)
{
    /* There is nothing to do if the Python type is the defining type. */
    if (py_type != defining_type)
    {
        PyObject *bases = ((PyTypeObject *)py_type)->tp_bases;
        Py_ssize_t base_i = 0;
        Py_ssize_t mixin_i = -1;

        while (base_i < PyTuple_GET_SIZE(bases))
        {
            PyObject *base_type = PyTuple_GET_ITEM(bases, base_i);
            if (base_type == defining_type)
                return mixin_i;

            if (PyType_IsSubtype((PyTypeObject *)base_type, sms->simple_wrapper_type) && !((sipWrapperType *)base_type)->user_type)
                mixin_i++;

            base_i++;
        }
    }

    return -1;
}


/*
 * Return the mixin instance of a main object at a given index.
 */
PyObject *sip_get_mixin_instance(PyObject *main_object, Py_ssize_t mixin_i)
{
    assert(((sipSimpleWrapper *)main_object)->mixins != NULL &&
            mixin_i < PyList_Size(((sipSimpleWrapper *)main_object)->mixins));

    return PyList_GET_ITEM(((sipSimpleWrapper *)main_object)->mixins, mixin_i);
}


/*
 * Initialise the simple wrapper type.
 */
int sip_simple_wrapper_init(PyObject *module, sipSipModuleState *sms)
{
    PySlot all_slots[] = {
        PySlot_STATIC_DATA(Py_slot_subslots, SimpleWrapper_slots),
        PySlot_DATA(Py_tp_module, module),
        PySlot_DATA(Py_tp_metaclass, sms->wrapper_type_type),
        PySlot_END
    };

    sms->simple_wrapper_type = (PyTypeObject *)PyType_FromSlots(all_slots);

    if (sms->simple_wrapper_type == NULL)
        return -1;

    if (PyModule_AddType(module, sms->simple_wrapper_type) < 0)
        return -1;

    return 0;
}


/*
 * Convert a new C/C++ pointer to a Python instance.
 */
PyObject *sip_wrap_instance(sipModuleState *ms, void *cpp,
        PyTypeObject *py_type, PyObject *args, PyObject *owner, int flags)
{
    if (cpp == NULL)
        Py_RETURN_NONE;

    sipSipModuleState *sms = ms->sip_module_state;

    if (args == NULL)
        args = sms->empty_tuple;

    PyObject *self = py_type->tp_new(py_type, args, NULL);
    if (self == NULL)
        return NULL;

    ((sipSimpleWrapper *)self)->data = cpp;
    ((sipSimpleWrapper *)self)->flags = flags | SIP_CREATED;

    Py_BEGIN_CRITICAL_SECTION_MUTEX(&sms->mutex);

    sip_om_add_object(ms, ((sipSimpleWrapper *)self));

    if (owner != NULL && ((sipWrapperType *)py_type)->is_wrapper)
    {
        assert(PyObject_TypeCheck(owner, sms->wrapper_type));
        sip_add_to_parent((sipWrapper *)self, (sipWrapper *)owner);
    }

    Py_END_CRITICAL_SECTION();

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
                if (ehs->event_type == sipEventWrappedInstance && sip_is_subtype(ehs_ms, py_type, ehs->type_id))
                {
                    sipWrappedInstanceEventHandler handler = (sipWrappedInstanceEventHandler)ehs->handler;

                    handler(ehs_ms, ehs->type_id, cpp);
                }

                ehs++;
            }
        }

        Py_DECREF(ehs_mod);
    }

    return self;
}


/*
 * Call the equivalent of super().__init__() for an instance.
 */
static int call_super_init(PyTypeObject *super_type, PyObject *self,
        PyObject *kwds)
{
    /* Get the super type's __init__ method. */
    PyObject *dunder_init = PyUnicode_InternFromString("__init__");
    if (dunder_init == NULL)
        return -1;

    PyObject *init = PyObject_GetAttr((PyObject *)super_type, dunder_init);

    Py_DECREF(dunder_init);

    if (init == NULL)
        return -1;

    /* Create the tuple of positional arguments. */
    PyObject *init_args = PyTuple_Pack(1, self);
    if (init_args == NULL)
    {
        Py_DECREF(init);
        return -1;
    }

    /* Call the method. */
    PyObject *init_res = PyObject_Call(init, init_args, kwds);
    Py_DECREF(init_args);
    Py_DECREF(init);
    Py_XDECREF(init_res);

    return (init_res != NULL) ? 0 : -1;
}


/*
 * Find any finalisation function for a class, searching its super-classes if
 * necessary.
 */
static sipFinalFunc find_finalisation(sipModuleState *ms,
        const sipClassTypeSpec *cts, PyObject **final_mod_p)
{
    if (cts->final != NULL)
        return cts->final;

    const sipTypeID *supers = cts->supers;

    if (supers != NULL)
    {
        sipTypeID sup_type_id;

        do
        {
            sup_type_id = *supers++;

            PyObject *sup_mod;
            const sipTypeSpec *sup_ts = sip_get_type_spec(ms, sup_type_id,
                    &sup_mod);
            assert(sup_mod != NULL);

            sipFinalFunc func = find_finalisation(
                    sip_get_module_state(sup_mod),
                    (const sipClassTypeSpec *)sup_ts, final_mod_p);

            if (func != NULL)
            {
                Py_SETREF(*final_mod_p, sup_mod);
                return func;
            }

            Py_DECREF(sup_mod);
        }
        while (!sipTypeIDIsSentinel(sup_type_id));
    }

    return NULL;
}


/*
 * Return the class type specification for a wrapper.
 */
static const sipClassTypeSpec *get_class_type_spec_from_self(PyObject *self)
{
    return (const sipClassTypeSpec *)sip_get_type_spec_from_wt(
            (sipWrapperType *)Py_TYPE(self));
}


/*
 * Invoke any init extensions to a type until a result is obtained, an error
 * occurs or no appropriate extender was found.
 */
static void *init_extend(sipModuleState *ms, PyObject **p_state_p,
        PyObject *self, PyObject *const *args, Py_ssize_t nr_args,
        PyObject *kwd_names, PyObject **unused_p, PyObject **owner_p,
        const sipTypeSpec *extending_ts)
{
    InitClosure init_closure = {
        .p_state_p = p_state_p,
        .self = self,
        .args = args,
        .nr_args = nr_args,
        .kwd_names = kwd_names,
        .unused_p = unused_p,
        .owner_p = owner_p,
    };

    int state = sip_iterate_over_init_extenders(ms, extending_ts,
            init_extender_handler, &init_closure);

    /* The caller uses the parser state to determine if there was an error. */
    if (state < 0)
        sip_api_set_parser_error(p_state_p);

    return state > 0 ? init_closure.cpp : NULL;
}


/*
 * The init extender iterator handler.
 */
static int init_extender_handler(sipModuleState *x_ms,
        const sipInitExtenderSpec *x_init_extender_spec, void *closure)
{
    InitClosure *ic = (InitClosure *)closure;

    ic->cpp = x_init_extender_spec->extender(x_ms, ic->p_state_p, ic->self,
            ic->args, ic->nr_args, ic->kwd_names, ic->unused_p, ic->owner_p);

    /* See if there was a result. */
    if (ic->cpp != NULL)
        return 1;

    /* Stop if there was an error. */
    return *ic->p_state_p == Py_None ? -1 : 0;
}


/*
 * Convert traditional arguments to vectorcall style.  This steals its approach
 * from the Python internals.
 */
static int vectorcall_create(PyObject *args, PyObject *kwargs,
        PyObject **small_argv, Py_ssize_t *argv_len_p, PyObject ***argv_p,
        Py_ssize_t *nr_pos_args_p, PyObject **kw_names_p)
{
    Py_ssize_t nr_pos_args = (args == NULL ? 0 : PyTuple_Size(args));
    if (nr_pos_args < 0)
        return -1;

    Py_ssize_t nr_kwd_args = (kwargs == NULL ? 0 : PyDict_Size(kwargs));
    if (nr_kwd_args < 0)
        return -1;

    /* Minimise the memory allocations for most cases. */
    PyObject **argv;
    Py_ssize_t argv_len = nr_pos_args + nr_kwd_args;

    if (argv_len <= *argv_len_p)
    {
        argv = small_argv;
    }
    else
    {
        argv = sip_api_malloc(argv_len * sizeof (PyObject *));
        if (argv == NULL)
            return -1;
    }

    *argv_len_p = argv_len;

    Py_ssize_t i = 0;

    for (i = 0; i < nr_pos_args; i++)
        argv[i] = Py_NewRef(PyTuple_GET_ITEM(args, i));

    PyObject *kw_names;
    unsigned long names_are_strings = Py_TPFLAGS_UNICODE_SUBCLASS;

    if (nr_kwd_args == 0)
    {
        kw_names = NULL;
    }
    else
    {
        if ((kw_names = PyTuple_New(nr_kwd_args)) == NULL)
        {
            vectorcall_dispose(small_argv, argv, nr_pos_args, NULL);
            return -1;
        }

        Py_ssize_t pos = 0;
        PyObject *key, *value;
        i = 0;

        /*
         * The dict is local to this thread so there is no need to protect it.
         */
        while (PyDict_Next(kwargs, &pos, &key, &value))
        {
            names_are_strings &= Py_TYPE(key)->tp_flags;
            PyTuple_SET_ITEM(kw_names, i, Py_NewRef(key));
            argv[nr_pos_args + i] = Py_NewRef(value);
            i++;
        }
    }

    if (names_are_strings)
    {
        *argv_p = argv;
        *nr_pos_args_p = nr_pos_args;

        if (kw_names_p != NULL)
            *kw_names_p = kw_names;

        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "keywords must be strings");

    vectorcall_dispose(small_argv, argv, argv_len, kw_names);

    return -1;
}


/*
 * Dispose of the resources allocated by vectorcall_create().
 */
static void vectorcall_dispose(PyObject **small_argv, PyObject **argv,
        Py_ssize_t argv_len, PyObject *kw_names)
{
    Py_ssize_t i;

    for (i = 0; i < argv_len; i++)
        Py_DECREF(argv[i]);

    if (argv != small_argv)
        sip_api_free(argv);

    Py_XDECREF(kw_names);
}
