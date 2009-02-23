/*
 * The implementation of the different descriptors.
 *
 * @BS_LICENSE@
 */


#include <Python.h>

#include "sip.h"
#include "sipint.h"


/*****************************************************************************
 * A method descriptor.
 *****************************************************************************/


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
 * Return a new method descriptor for the given method.  We don't use the
 * Python descriptor because it doesn't support a method having static and
 * non-static overloads.
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
