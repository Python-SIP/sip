/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file implements the API for the argument parsers support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#include <Python.h>

#include "sip_object_guard.h"

#include "sip_core.h"
#include "sip_sip_module.h"
#include "sip_wrapped_module.h"


/*
 * The opaque object guard structure.
 */
struct sipObjectGuardImpl
{
    /* The interpreter view that owns the object. */
    PyInterpreterView *interp_view;

    /*
     * The module state of the module that provided the context in which the
     * guard (and not the object) was created.  It is only valid if the object
     * is valid.
     */
    sipModuleState *module_state;

    /* A strong reference to the object. */
    PyObject *obj;
};


/*
 * Create a new guard for an object.  Returns NULL with an exception raised if
 * there was an error.
 */
sipObjectGuard *sip_api_object_guard_new(sipModuleState *ms, PyObject *obj)
{
    sipObjectGuard *guard = (sipObjectGuard *)sip_api_malloc(
            sizeof (sipObjectGuard));
    if (guard == NULL)
        return NULL;

    guard->interp_view = ms->sip_module_state->interpreter_view;
    guard->module_state = ms;
    guard->obj = Py_NewRef(obj);

    return guard;
}


/*
 * Return the module state for the module that provided the context when the
 * guard was created.  It must only ever be called after a succesful call to
 * sipObjectGuard_GetRef().
 */
sipModuleState *sip_api_object_guard_get_module_state(sipObjectGuard *guard)
{
    return guard->module_state;
}


/*
 * Return a new reference to a guarded object and a PyThreadStateToken for the
 * (now attached) thread state.  The token must be passed to
 * PyThreadState_Release() once the reference to the guarded object has been
 * released.  NULL is returned (and no exception raised) if the object is no
 * longer available.  This may be called without an attached thread state.
 */
PyObject *sip_api_object_guard_get_ref(sipObjectGuard *guard,
        PyThreadStateToken **tst_p)
{
    if ((*tst_p = PyThreadState_EnsureFromView(guard->interp_view)) == NULL)
        return NULL;

    /* The object may been garbage collected. */
    PyObject *obj = Py_XNewRef(guard->obj);
    if (!obj)
    {
        PyThreadState_Release(*tst_p);
        *tst_p = NULL;
    }

    return obj;
}


/*
 * Release a guarded object (and the guard itself) which must be owned by the
 * current interpreter.
 */
void sip_api_object_guard_release(sipObjectGuard *guard)
{
    Py_XDECREF(guard->obj);
    sip_api_free(guard);
}


/*
 * Clear the guarded object if it is owned by the current interpreter.  This is
 * normally called by the 'clear' function installed with
 * sipSetModuleUserState().  Note that by doing this we are tying the lifecycle
 * of the object to that of the module context in which it was created rather
 * than the interpreter context in which it is valid.
 */
int sip_api_object_guard_clear(sipModuleState *ms, sipObjectGuard *guard)
{
    if (guard->interp_view == ms->sip_module_state->interpreter_view)
    {
        guard->module_state = NULL;
        Py_CLEAR(guard->obj);
    }

    return 0;
}


/*
 * Free the guard if it is owned by the current interpreter.  Returns a
 * non-zero value if it was freed.  This is normally called by the 'free'
 * function installed with sipSetModuleUserState().
 */
int sip_api_object_guard_free(sipModuleState *ms, sipObjectGuard *guard)
{
    if (guard->interp_view == ms->sip_module_state->interpreter_view)
    {
        sip_api_object_guard_release(guard);
        return 1;
    }

    return 0;
}


/*
 * Traverse the guarded object object if it is owned by the current
 * interpreter.  This is normally called by the 'traverse' function installed
 * with sipSetModuleUserState().
 */
int sip_api_object_guard_traverse(sipModuleState *ms, sipObjectGuard *guard,
        visitproc visit, void *arg)
{
    if (guard->interp_view == ms->sip_module_state->interpreter_view)
        Py_VISIT(guard->obj);

    return 0;
}
