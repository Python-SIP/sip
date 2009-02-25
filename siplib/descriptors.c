/*
 * The implementation of the different descriptors.
 *
 * @BS_LICENSE@
 */


#include <Python.h>

#include "sip.h"
#include "sipint.h"


/*****************************************************************************
 * A method descriptor.  We don't use the similar Python descriptor because it
 * doesn't support a method having static and non-static overloads.
 *****************************************************************************/


/* Forward declarations of slots. */
static PyObject *sipMethodDescr_descr_get(PyObject *self, PyObject *obj,
        PyObject *type);


/*
 * The object data structure.
 */
typedef struct _sipMethodDescr {
    PyObject_HEAD

    /* The method definition. */
    PyMethodDef *pmd;
} sipMethodDescr;


/*
 * The type data structure.
 */
PyTypeObject sipMethodDescr_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                      /* ob_size */
    "sip.methoddescriptor", /* tp_name */
    sizeof (sipMethodDescr),    /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_compare */
    0,                      /* tp_repr */
    0,                      /* tp_as_number */
    0,                      /* tp_as_sequence */
    0,                      /* tp_as_mapping */
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    0,                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,     /* tp_flags */
    0,                      /* tp_doc */
    0,                      /* tp_traverse */
    0,                      /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    sipMethodDescr_descr_get,   /* tp_descr_get */
};


/*
 * Return a new method descriptor for the given method.
 */
PyObject *sipMethodDescr_New(PyMethodDef *pmd)
{
    PyObject *descr = PyType_GenericAlloc(&sipMethodDescr_Type, 0);

    if (descr != NULL)
        ((sipMethodDescr *)descr)->pmd = pmd;

    return descr;
}


/*
 * The descriptor's descriptor get slot.
 */
static PyObject *sipMethodDescr_descr_get(PyObject *self, PyObject *obj,
        PyObject *type)
{
    sipMethodDescr *md = (sipMethodDescr *)self;

    if (obj == Py_None)
        obj = NULL;

    return PyCFunction_New(md->pmd, obj);
}


/*****************************************************************************
 * A variable descriptor.  We don't use the similar Python descriptor because
 * it doesn't support static variables.
 *****************************************************************************/


/* Forward declarations of slots. */
static PyObject *sipVariableDescr_descr_get(PyObject *self, PyObject *obj,
        PyObject *type);


/*
 * The object data structure.
 */
typedef struct _sipVariableDescr {
    PyObject_HEAD

    /* The getter/setter definition. */
    sipVariableDef *vd;

    /* The generated type definition. */
    const sipClassTypeDef *ctd;
} sipVariableDescr;


/*
 * The type data structure.
 */
PyTypeObject sipVariableDescr_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                      /* ob_size */
    "sip.variabledescriptor",   /* tp_name */
    sizeof (sipVariableDescr),  /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_compare */
    0,                      /* tp_repr */
    0,                      /* tp_as_number */
    0,                      /* tp_as_sequence */
    0,                      /* tp_as_mapping */
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    0,                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,     /* tp_flags */
    0,                      /* tp_doc */
    0,                      /* tp_traverse */
    0,                      /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    sipVariableDescr_descr_get, /* tp_descr_get */
};


/* Forward declarations. */
static void raise_nonstatic_exception(sipVariableDescr *vd);


/*
 * Return a new method descriptor for the given getter/setter.
 */
PyObject *sipVariableDescr_New(sipVariableDef *vd, sipClassTypeDef *ctd)
{
    PyObject *descr = PyType_GenericAlloc(&sipVariableDescr_Type, 0);

    if (descr != NULL)
    {
        ((sipVariableDescr *)descr)->vd = vd;
        ((sipVariableDescr *)descr)->ctd = ctd;
    }

    return descr;
}


/*
 * The descriptor's descriptor get slot.
 */
static PyObject *sipVariableDescr_descr_get(PyObject *self, PyObject *obj,
        PyObject *type)
{
    sipVariableDescr *vd = (sipVariableDescr *)self;
    void *cpp;

    if (vd->vd->vd_is_static)
    {
        cpp = NULL;
    }
    else
    {
        /* Check that access was via an instance. */
        if (obj == NULL || obj == Py_None)
        {
            raise_nonstatic_exception(vd);
            return NULL;
        }

        /* Get the C++ instance. */
        if ((cpp = sip_api_get_cpp_ptr((sipSimpleWrapper *)obj, vd->ctd)) == NULL)
            return NULL;
    }

    return vd->vd->vd_getter(cpp);
}


/*
 * Raise an exception about accessing a non-static variable.
 */
static void raise_nonstatic_exception(sipVariableDescr *vd)
{
    PyErr_Format(PyExc_AttributeError, "'%s.%s' is an instance variable",
            sipPyNameOfClass(vd->ctd), vd->vd->vd_name);

}
