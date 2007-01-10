/*
 * SIP library code.
 *
 * @BS_LICENSE@
 */


#include <Python.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "sip.h"
#include "sipint.h"


/*
 * These are the functions that make up the public and private SIP API.
 */
static void sip_api_bad_catcher_result(PyObject *method);
static void sip_api_bad_length_for_slice(SIP_SSIZE_T seqlen,
        SIP_SSIZE_T slicelen);
static PyObject *sip_api_build_result(int *isErr, const char *fmt, ...);
static PyObject *sip_api_call_method(int *isErr, PyObject *method,
        const char *fmt, ...);
static PyObject *sip_api_class_name(PyObject *self);
static SIP_SSIZE_T sip_api_convert_from_sequence_index(SIP_SSIZE_T idx,
        SIP_SSIZE_T len);
static int sip_api_can_convert_to_instance(PyObject *pyObj,
        sipWrapperType *type, int flags);
static int sip_api_can_convert_to_mapped_type(PyObject *pyObj,
        const sipMappedType *mt, int flags);
static void *sip_api_convert_to_instance(PyObject *pyObj, sipWrapperType *type,
        PyObject *transferObj, int flags, int *statep, int *iserrp);
static void *sip_api_convert_to_mapped_type(PyObject *pyObj,
        const sipMappedType *mt, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
static void *sip_api_force_convert_to_instance(PyObject *pyObj,
        sipWrapperType *type, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
static void *sip_api_force_convert_to_mapped_type(PyObject *pyObj,
        const sipMappedType *mt, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
static void sip_api_release_instance(void *cpp, sipWrapperType *type,
        int state);
static void sip_api_release_mapped_type(void *cpp, const sipMappedType *mt,
        int state);
static PyObject *sip_api_convert_from_new_instance(void *cpp,
        sipWrapperType *type, PyObject *transferObj);
static PyObject *sip_api_convert_from_mapped_type(void *cpp,
        const sipMappedType *mt, PyObject *transferObj);
static void *sip_api_convert_to_cpp(PyObject *sipSelf, sipWrapperType *type,
        int *iserrp);
static int sip_api_get_state(PyObject *transferObj);
static const sipMappedType *sip_api_find_mapped_type(const char *type);
static PyObject *sip_api_get_wrapper(void *cppPtr, sipWrapperType *type);
static sipWrapperType *sip_api_map_int_to_class(int typeInt,
        const sipIntTypeClassMap *map, int maplen);
static sipWrapperType *sip_api_map_string_to_class(const char *typeString,
        const sipStringTypeClassMap *map, int maplen);
static int sip_api_parse_result(int *isErr, PyObject *method, PyObject *res,
        const char *fmt, ...);
static void sip_api_trace(unsigned mask,const char *fmt,...);
static void sip_api_transfer(PyObject *self, int toCpp);
static void sip_api_transfer_back(PyObject *self);
static void sip_api_transfer_to(PyObject *self, PyObject *owner);
static int sip_api_export_module(sipExportedModuleDef *client,
        unsigned api_major, unsigned api_minor, PyObject *mod_dict);
static int sip_api_parse_args(int *argsParsedp, PyObject *sipArgs,
        const char *fmt, ...);
static int sip_api_parse_pair(int *argsParsedp, PyObject *sipArg0,
        PyObject *sipArg1, const char *fmt, ...);
static void sip_api_common_ctor(sipMethodCache *cache, int nrmeths);
static void sip_api_common_dtor(sipWrapper *sipSelf);
static void *sip_api_convert_to_void_ptr(PyObject *obj);
static void sip_api_no_function(int argsParsed, const char *func);
static void sip_api_no_method(int argsParsed, const char *classname,
        const char *method);
static void sip_api_abstract_method(const char *classname, const char *method);
static void sip_api_bad_class(const char *classname);
static void sip_api_bad_set_type(const char *classname, const char *var);
static void *sip_api_get_complex_cpp_ptr(sipWrapper *w);
static PyObject *sip_api_is_py_method(sip_gilstate_t *gil,
        sipMethodCache *pymc, sipWrapper *sipSelf, char *cname, char *mname);
static void sip_api_call_hook(const char *hookname);
static void sip_api_raise_unknown_exception(void);
static void sip_api_raise_class_exception(sipWrapperType *type, void *ptr);
static void sip_api_raise_sub_class_exception(sipWrapperType *type, void *ptr);
static int sip_api_add_class_instance(PyObject *dict, const char *name,
        void *cppPtr, sipWrapperType *wt);
static int sip_api_add_mapped_type_instance(PyObject *dict, const char *name,
        void *cppPtr, const sipMappedType *mt);
static int sip_api_add_enum_instance(PyObject *dict, const char *name,
        int value, PyTypeObject *type);
static void sip_api_bad_operator_arg(PyObject *self, PyObject *arg,
        sipPySlotType st);
static PyObject *sip_api_pyslot_extend(sipExportedModuleDef *mod,
        sipPySlotType st, sipWrapperType *type, PyObject *arg0,
        PyObject *arg1);
static void sip_api_add_delayed_dtor(sipWrapper *w);
static unsigned long sip_api_long_as_unsigned_long(PyObject *o);
static int sip_api_export_symbol(const char *name, void *sym);
static void *sip_api_import_symbol(const char *name);
static int sip_api_register_int_types(PyObject *args);


/*
 * The data structure that represents the SIP API.
 */
static const sipAPIDef sip_api = {
    /* This must be first. */
    sip_api_export_module,
    /*
     * The following are part of the public API.
     */
    sip_api_bad_catcher_result,
    sip_api_bad_length_for_slice,
    sip_api_build_result,
    sip_api_call_method,
    sip_api_class_name,
    sip_api_connect_rx,
    sip_api_convert_from_sequence_index,
    sip_api_can_convert_to_instance,
    sip_api_can_convert_to_mapped_type,
    sip_api_convert_to_instance,
    sip_api_convert_to_mapped_type,
    sip_api_force_convert_to_instance,
    sip_api_force_convert_to_mapped_type,
    sip_api_release_instance,
    sip_api_release_mapped_type,
    sip_api_convert_from_instance,
    sip_api_convert_from_new_instance,
    sip_api_convert_from_mapped_type,
    sip_api_convert_to_cpp,
    sip_api_get_state,
    sip_api_find_mapped_type,
    sip_api_disconnect_rx,
    sip_api_emit_signal,
    sip_api_free,
    sip_api_get_sender,
    sip_api_get_wrapper,
    sip_api_malloc,
    sip_api_map_int_to_class,
    sip_api_map_string_to_class,
    sip_api_parse_result,
    sip_api_trace,
    sip_api_transfer,
    sip_api_transfer_back,
    sip_api_transfer_to,
    sip_api_wrapper_check,
    sip_api_long_as_unsigned_long,
    /*
     * The following may be used by Qt support code but by no other handwritten
     * code.
     */
    sip_api_convert_from_named_enum,
    sip_api_convert_from_void_ptr,
    sip_api_free_connection,
    sip_api_emit_to_slot,
    sip_api_same_connection,
    sip_api_convert_rx,
    /*
     * The following are not part of the public API.
     */
    sip_api_parse_args,
    sip_api_parse_pair,
    sip_api_common_ctor,
    sip_api_common_dtor,
    sip_api_convert_to_void_ptr,
    sip_api_no_function,
    sip_api_no_method,
    sip_api_abstract_method,
    sip_api_bad_class,
    sip_api_bad_set_type,
    sip_api_get_cpp_ptr,
    sip_api_get_complex_cpp_ptr,
    sip_api_is_py_method,
    sip_api_call_hook,
    sip_api_start_thread,
    sip_api_end_thread,
    sip_api_raise_unknown_exception,
    sip_api_raise_class_exception,
    sip_api_raise_sub_class_exception,
    sip_api_add_class_instance,
    sip_api_add_enum_instance,
    sip_api_bad_operator_arg,
    sip_api_pyslot_extend,
    sip_api_add_delayed_dtor,
    sip_api_add_mapped_type_instance,
    /*
     * The following are part of the public API.
     */
    sip_api_export_symbol,
    sip_api_import_symbol,
    /*
     * The following may be used by Qt support code but by no other handwritten
     * code.
     */
    sip_api_register_int_types,
};


#define PARSE_OK                0x00000000      /* Parse is Ok so far. */
#define PARSE_MANY              0x10000000      /* Too many arguments. */
#define PARSE_FEW               0x20000000      /* Too few arguments. */
#define PARSE_TYPE              0x30000000      /* Argument with a bad type. */
#define PARSE_UNBOUND           0x40000000      /* Unbound method. */
#define PARSE_FORMAT            0x50000000      /* Bad format character. */
#define PARSE_RAISED            0x60000000      /* Exception already raised. */
#define PARSE_STICKY            0x80000000      /* The error sticks. */
#define PARSE_MASK              0xf0000000

/*
 * Note that some of the following flags safely share values because they
 * cannot be used at the same time.
 */
#define FORMAT_DEREF            0x01    /* The pointer will be dereferenced. */
#define FORMAT_FACTORY          0x02    /* Implement /Factory/ in a VH. */
#define FORMAT_TRANSFER         0x02    /* Implement /Transfer/. */
#define FORMAT_NO_STATE         0x04    /* Don't return the C/C++ state. */
#define FORMAT_TRANSFER_BACK    0x04    /* Implement /TransferBack/. */
#define FORMAT_GET_WRAPPER      0x08    /* Implement /GetWrapper/. */
#define FORMAT_NO_CONVERTORS    0x10    /* Suppress any convertors. */

#define SIP_MC_FOUND            0x01    /* If we have looked for the method. */
#define SIP_MC_ISMETH           0x02    /* If we looked and there was one. */

#define sipFoundMethod(m)       ((m)->mcflags & SIP_MC_FOUND)
#define sipSetFoundMethod(m)    ((m)->mcflags |= SIP_MC_FOUND)
#define sipIsMethod(m)          ((m)->mcflags & SIP_MC_ISMETH)
#define sipSetIsMethod(m)       ((m)->mcflags |= SIP_MC_ISMETH)


/*
 * An entry in a linked list of name/symbol pairs.
 */
typedef struct _sipSymbol {
    const char *name;           /* The name. */
    void *symbol;               /* The symbol. */
    struct _sipSymbol *next;    /* The next in the list. */
} sipSymbol;


/*
 * An entry in a linked list of Python objects.
 */
typedef struct _sipPyObject {
    PyObject *object;           /* The Python object. */
    struct _sipPyObject *next;  /* The next in the list. */
} sipPyObject;


static PyTypeObject sipWrapperType_Type;
static sipWrapperType sipWrapper_Type;
static PyTypeObject sipVoidPtr_Type;

PyInterpreterState *sipInterpreter = NULL;
sipQtAPI *sipQtSupport = NULL;
sipWrapperType *sipQObjectClass;
sipPyObject *sipRegisteredIntTypes = NULL;
sipSymbol *sipSymbolList = NULL;


/*
 * Various strings as Python objects created as and when needed.
 */
static PyObject *licenseName = NULL;
static PyObject *licenseeName = NULL;
static PyObject *typeName = NULL;
static PyObject *timestampName = NULL;
static PyObject *signatureName = NULL;

static sipObjectMap cppPyMap;           /* The C/C++ to Python map. */
static sipExportedModuleDef *clientList = NULL; /* List of registered clients. */
static unsigned traceMask = 0;          /* The current trace mask. */

static sipTypeDef *currentType = NULL;  /* The type being created. */


static void addSlots(sipWrapperType *wt, sipTypeDef *td);
static void initSlots(PyTypeObject *to, PyNumberMethods *nb,
        PySequenceMethods *sq, PyMappingMethods *mp, sipPySlotDef *slots,
        int force);
static void *findSlot(PyObject *self, sipPySlotType st);
static void *findSlotInType(sipTypeDef *td, sipPySlotType st);
static int objobjargprocSlot(PyObject *self, PyObject *arg1, PyObject *arg2,
        sipPySlotType st);
static int ssizeobjargprocSlot(PyObject *self, SIP_SSIZE_T arg1,
        PyObject *arg2, sipPySlotType st);
static PyObject *buildObject(PyObject *tup, const char *fmt, va_list va);
static int parsePass1(sipWrapper **selfp, int *selfargp, int *argsParsedp,
        PyObject *sipArgs, const char *fmt, va_list va);
static int parsePass2(sipWrapper *self, int selfarg, int nrargs,
        PyObject *sipArgs, const char *fmt, va_list va);
static int getSelfFromArgs(sipWrapperType *type, PyObject *args, int argnr,
        sipWrapper **selfp);
static PyObject *createEnumMember(sipTypeDef *td, sipEnumMemberDef *enm);
static PyObject *handleGetLazyAttr(PyObject *nameobj, sipWrapperType *wt,
        sipWrapper *w);
static int handleSetLazyAttr(PyObject *nameobj, PyObject *valobj,
        sipWrapperType *wt, sipWrapper *w);
static int getNonStaticVariables(sipWrapperType *wt, sipWrapper *w,
        PyObject **ndict);
static void findLazyAttr(sipWrapperType *wt, char *name, PyMethodDef **pmdp,
        sipEnumMemberDef **enmp, PyMethodDef **vmdp, sipTypeDef **in);
static int compareMethodName(const void *key, const void *el);
static int compareEnumMemberName(const void *key, const void *el);
static int checkPointer(void *ptr);
static void badArgs(int argsParsed, const char *classname, const char *method);
static void finalise(void);
static sipWrapperType *createType(sipExportedModuleDef *client,
        sipTypeDef *type, PyObject *mod_dict);
static PyTypeObject *createEnum(sipExportedModuleDef *client, sipEnumDef *ed,
        PyObject *mod_dict);
static const char *getBaseName(const char *name);
static PyObject *getBaseNameObject(const char *name);
static PyObject *createTypeDict(PyObject *mname);
static sipExportedModuleDef *getClassModule(sipEncodedClassDef *enc,
        sipExportedModuleDef *em);
static sipWrapperType *getClassType(sipEncodedClassDef *enc,
        sipExportedModuleDef *em);
static sipWrapperType *convertSubClass(sipWrapperType *type, void **cppPtr);
static void *getPtrTypeDef(sipWrapper *self, sipTypeDef **td);
static int addInstances(PyObject *dict, sipInstancesDef *id);
static int addVoidPtrInstances(PyObject *dict, sipVoidPtrInstanceDef *vi);
static int addCharInstances(PyObject *dict, sipCharInstanceDef *ci);
static int addStringInstances(PyObject *dict, sipStringInstanceDef *si);
static int addIntInstances(PyObject *dict, sipIntInstanceDef *ii);
static int addLongInstances(PyObject *dict, sipLongInstanceDef *li);
static int addUnsignedLongInstances(PyObject *dict,
        sipUnsignedLongInstanceDef *uli);
static int addLongLongInstances(PyObject *dict, sipLongLongInstanceDef *lli);
static int addUnsignedLongLongInstances(PyObject *dict,
        sipUnsignedLongLongInstanceDef *ulli);
static int addDoubleInstances(PyObject *dict, sipDoubleInstanceDef *di);
static int addEnumInstances(PyObject *dict, sipEnumInstanceDef *ei);
static int addSingleEnumInstance(PyObject *dict, const char *name, int value,
        PyTypeObject *type);
static int addClassInstances(PyObject *dict, sipClassInstanceDef *ci);
static int addSingleClassInstance(PyObject *dict, const char *name,
        void *cppPtr, sipWrapperType *wt, int initflags);
static int addLicense(PyObject *dict, sipLicenseDef *lc);
static PyObject *cast(PyObject *self, PyObject *args);
static PyObject *callDtor(PyObject *self, PyObject *args);
static PyObject *isDeleted(PyObject *self, PyObject *args);
static PyObject *setDeleted(PyObject *self, PyObject *args);
static PyObject *setTraceMask(PyObject *self, PyObject *args);
static PyObject *wrapInstance(PyObject *self, PyObject *args);
static PyObject *unwrapInstance(PyObject *self, PyObject *args);
static PyObject *transfer(PyObject *self, PyObject *args);
static PyObject *transferBack(PyObject *self, PyObject *args);
static PyObject *transferTo(PyObject *self, PyObject *args);
static int sipWrapperType_Check(PyObject *op);
static void addToParent(sipWrapper *self, sipWrapper *owner);
static void removeFromParent(sipWrapper *self);
static int findClassArg(sipExportedModuleDef *emd, const char *name,
        size_t len, sipSigArg *at, int indir);
static int findMtypeArg(sipMappedType **mttab, const char *name, size_t len,
        sipSigArg *at, int indir);
static int findEnumArg(sipExportedModuleDef *emd, const char *name, size_t len,
        sipSigArg *at, int indir);
static int sameScopedName(const char *pyname, const char *name, size_t len);
static int nameEq(const char *with, const char *name, size_t len);
static int isExactWrappedType(sipWrapperType *wt);
static void release(void *addr, sipTypeDef *td, int state);
static void callPyDtor(sipWrapper *self);


/*
 * The Python module initialisation function.
 */
#if defined(SIP_STATIC_MODULE)
void initsip(void)
#else
PyMODINIT_FUNC initsip(void)
#endif
{
    static PyMethodDef methods[] = {
        {"cast", cast, METH_VARARGS, NULL},
        {"delete", callDtor, METH_VARARGS, NULL},
        {"isdeleted", isDeleted, METH_VARARGS, NULL},
        {"setdeleted", setDeleted, METH_VARARGS, NULL},
        {"settracemask", setTraceMask, METH_VARARGS, NULL},
        {"transfer", transfer, METH_VARARGS, NULL},
        {"transferback", transferBack, METH_VARARGS, NULL},
        {"transferto", transferTo, METH_VARARGS, NULL},
        {"wrapinstance", wrapInstance, METH_VARARGS, NULL},
        {"unwrapinstance", unwrapInstance, METH_VARARGS, NULL},
        {NULL, NULL, 0, NULL}
    };

    int rc;
    PyObject *mod, *mod_dict, *obj;

#ifdef WITH_THREAD
    PyEval_InitThreads();
#endif

    /* Initialise the types. */
    sipWrapperType_Type.tp_base = &PyType_Type;

    if (PyType_Ready(&sipWrapperType_Type) < 0)
        Py_FatalError("sip: Failed to initialise sip.wrappertype type");

    if (PyType_Ready((PyTypeObject *)&sipWrapper_Type) < 0)
        Py_FatalError("sip: Failed to initialise sip.wrapper type");

    if (PyType_Ready(&sipVoidPtr_Type) < 0)
        Py_FatalError("sip: Failed to initialise sip.voidptr type");

    mod = Py_InitModule("sip", methods);
    mod_dict = PyModule_GetDict(mod);

    /* Publish the SIP API. */
    if ((obj = PyCObject_FromVoidPtr((void *)&sip_api, NULL)) == NULL)
        Py_FatalError("sip: Failed to create _C_API object");

    rc = PyDict_SetItemString(mod_dict, "_C_API", obj);
    Py_DECREF(obj);

    if (rc < 0)
        Py_FatalError("sip: Failed to add _C_API object to module dictionary");

    /* Add the SIP version number, but don't worry about errors. */
    if ((obj = PyInt_FromLong(SIP_VERSION)) != NULL)
    {
        PyDict_SetItemString(mod_dict, "SIP_VERSION", obj);
        Py_DECREF(obj);
    }

    if ((obj = PyString_FromString(SIP_VERSION_STR)) != NULL)
    {
        PyDict_SetItemString(mod_dict, "SIP_VERSION_STR", obj);
        Py_DECREF(obj);
    }

    /* Add the type objects, but don't worry about errors. */
    PyDict_SetItemString(mod_dict, "wrappertype", (PyObject *)&sipWrapperType_Type);
    PyDict_SetItemString(mod_dict, "wrapper", (PyObject *)&sipWrapper_Type);
    PyDict_SetItemString(mod_dict, "voidptr", (PyObject *)&sipVoidPtr_Type);

    /* Initialise the module if it hasn't already been done. */
    if (sipInterpreter == NULL)
    {
        Py_AtExit(finalise);

        /* Initialise the object map. */
        sipOMInit(&cppPyMap);

        sipQtSupport = NULL;

        /*
         * Get the current interpreter.  This will be shared between all
         * threads.
         */
        sipInterpreter = PyThreadState_Get()->interp;
    }
}


/*
 * Display a printf() style message to stderr according to the current trace
 * mask.
 */
static void sip_api_trace(unsigned mask, const char *fmt, ...)
{
    va_list ap;

    va_start(ap,fmt);

    if (mask & traceMask)
        vfprintf(stderr, fmt, ap);

    va_end(ap);
}


/*
 * Set the trace mask.
 */
static PyObject *setTraceMask(PyObject *self, PyObject *args)
{
    unsigned new_mask;

    if (PyArg_ParseTuple(args, "I:settracemask", &new_mask))
    {
        traceMask = new_mask;

        Py_INCREF(Py_None);
        return Py_None;
    }

    return NULL;
}


/*
 * Transfer the ownership of an instance to C/C++.
 */
static PyObject *transferTo(PyObject *self, PyObject *args)
{
    PyObject *w, *owner;

    if (PyArg_ParseTuple(args, "O!O:transferto", &sipWrapper_Type, &w, &owner))
    {
        if (owner == Py_None)
            owner = NULL;
        else if (!sip_api_wrapper_check(owner))
        {
            PyErr_Format(PyExc_TypeError, "transferto() argument 2 must be sip.wrapper, not %s", owner->ob_type->tp_name);
            return NULL;
        }

        sip_api_transfer_to(w, owner);

        Py_INCREF(Py_None);
        return Py_None;
    }

    return NULL;
}


/*
 * Transfer the ownership of an instance to Python.
 */
static PyObject *transferBack(PyObject *self, PyObject *args)
{
    PyObject *w;

    if (PyArg_ParseTuple(args, "O!:transferback", &sipWrapper_Type, &w))
    {
        sip_api_transfer_back(w);

        Py_INCREF(Py_None);
        return Py_None;
    }

    return NULL;
}


/*
 * Transfer the ownership of an instance.  This is deprecated.
 */
static PyObject *transfer(PyObject *self, PyObject *args)
{
    PyObject *w;
    int toCpp;

    if (PyArg_ParseTuple(args, "O!i:transfer", &sipWrapper_Type, &w, &toCpp))
    {
        if (toCpp)
            sip_api_transfer_to(w, NULL);
        else
            sip_api_transfer_back(w);

        Py_INCREF(Py_None);
        return Py_None;
    }

    return NULL;
}


/*
 * Cast an instance to one of it's sub or super-classes by returning a new
 * Python object with the superclass type wrapping the same C++ instance.
 */
static PyObject *cast(PyObject *self, PyObject *args)
{
    sipWrapper *w;
    sipWrapperType *wt, *type;
    void *addr;
    PyTypeObject *ft, *tt;

    if (!PyArg_ParseTuple(args, "O!O!:cast", &sipWrapper_Type, &w, &sipWrapperType_Type, &wt))
        return NULL;

    ft = ((PyObject *)w)->ob_type;
    tt = (PyTypeObject *)wt;

    if (ft == tt || PyType_IsSubtype(tt, ft))
        type = NULL;
    else if (PyType_IsSubtype(ft, tt))
        type = wt;
    else
    {
        PyErr_SetString(PyExc_TypeError, "argument 1 of sip.cast() must be an instance of a sub or super-type of argument 2");
        return NULL;
    }

    if ((addr = sip_api_get_cpp_ptr(w, type)) == NULL)
        return NULL;

    /*
     * We don't put this new object into the map so that the original object is
     * always found.  It would also totally confuse the map logic.
     */
    return sipWrapSimpleInstance(addr, wt, NULL, (w->flags | SIP_NOT_IN_MAP) & ~SIP_PY_OWNED);
}


/*
 * Call an instance's dtor.
 */
static PyObject *callDtor(PyObject *self, PyObject *args)
{
    sipWrapper *w;
    void *addr;
    sipTypeDef *td;

    if (!PyArg_ParseTuple(args, "O!:delete", &sipWrapper_Type, &w))
        return NULL;

    addr = getPtrTypeDef(w, &td);

    if (checkPointer(addr) < 0)
        return NULL;

    /*
     * Transfer ownership to C++ so we don't try to release it again when the
     * Python object is garbage collected.
     */
    removeFromParent(w);
    sipResetPyOwned(w);

    release(addr, td, w->flags);

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Check if an instance still exists without raising an exception.
 */
static PyObject *isDeleted(PyObject *self, PyObject *args)
{
    sipWrapper *w;
    PyObject *res;

    if (!PyArg_ParseTuple(args, "O!:isdeleted", &sipWrapper_Type, &w))
        return NULL;

    res = (sipGetAddress(w) == NULL ? Py_True : Py_False);

    Py_INCREF(res);
    return res;
}


/*
 * Mark an instance as having been deleted.
 */
static PyObject *setDeleted(PyObject *self, PyObject *args)
{
    sipWrapper *w;

    if (!PyArg_ParseTuple(args, "O!:setdeleted", &sipWrapper_Type, &w))
        return NULL;

    /*
     * Transfer ownership to C++ so we don't try to release it when the Python
     * object is garbage collected.
     */
    removeFromParent(w);
    sipResetPyOwned(w);

    w->u.cppPtr = NULL;

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Unwrap an instance.
 */
static PyObject *unwrapInstance(PyObject *self, PyObject *args)
{
    sipWrapper *w;

    if (PyArg_ParseTuple(args, "O!:unwrapinstance", &sipWrapper_Type, &w))
    {
        void *addr;

        /*
         * We just get the pointer but don't try and cast it (which isn't
         * needed and wouldn't work with the way casts are currently
         * implemented if we are unwrapping something derived from a wrapped
         * class).
         */
        if ((addr = sip_api_get_cpp_ptr(w, NULL)) == NULL)
            return NULL;

        return PyLong_FromVoidPtr(addr);
    }

    return NULL;
}


/*
 * Wrap an instance.
 */
static PyObject *wrapInstance(PyObject *self, PyObject *args)
{
    unsigned long addr;
    sipWrapperType *wt;

    if (PyArg_ParseTuple(args, "kO!:wrapinstance", &addr, &sipWrapperType_Type, &wt))
        return sip_api_convert_from_instance((void *)addr, wt, NULL);

    return NULL;
}


/*
 * Register a client module.  A negative value is returned and an exception
 * raised if there was an error.  Not normally needed by handwritten code.
 */
static int sip_api_export_module(sipExportedModuleDef *client,
        unsigned api_major, unsigned api_minor, PyObject *mod_dict)
{
    sipExportedModuleDef *em;
    sipImportedModuleDef *im;
    sipSubClassConvertorDef *scc;
    sipWrapperType **mw;
    sipEnumMemberDef *emd;
    sipInitExtenderDef *ie;
    int i;

    /* Check that we can support it. */

    if (api_major != SIP_API_MAJOR_NR || api_minor > SIP_API_MINOR_NR)
    {
#if SIP_API_MINOR_NR > 0
        PyErr_Format(PyExc_RuntimeError, "the sip module supports API v%d.0 to v%d.%d but the %s module requires API v%d.%d", SIP_API_MAJOR_NR, SIP_API_MAJOR_NR, SIP_API_MINOR_NR, client->em_name, api_major,api_minor);
#else
        PyErr_Format(PyExc_RuntimeError, "the sip module supports API v%d.0 but the %s module requires API v%d.%d", SIP_API_MAJOR_NR, client->em_name, api_major,api_minor);
#endif

        return -1;
    }

    /* Convert the module name to an object. */
    if ((client->em_nameobj = PyString_FromString(client->em_name)) == NULL)
        return -1;

    for (em = clientList; em != NULL; em = em->em_next)
    {
        /* SIP clients must have unique names. */
        if (strcmp(em->em_name, client->em_name) == 0)
        {
            PyErr_Format(PyExc_RuntimeError, "the sip module has already registered a module called %s", client->em_name);

            return -1;
        }

        /* Only one module can claim to wrap QObject. */
        if (em->em_qt_api != NULL && client->em_qt_api != NULL)
        {
            PyErr_Format(PyExc_RuntimeError, "the %s and %s modules both wrap the QObject class", client->em_name, em->em_name);

            return -1;
        }
    }

    /* Import any required modules. */
    if ((im = client->em_imports) != NULL)
    {
        while (im->im_name != NULL)
        {
            PyObject *mod;

            if ((mod = PyImport_ImportModule(im->im_name)) == NULL)
                return -1;

            for (em = clientList; em != NULL; em = em->em_next)
                if (strcmp(em->em_name, im->im_name) == 0)
                    break;

            if (em == NULL)
            {
                PyErr_Format(PyExc_RuntimeError, "the %s module failed to register with the sip module", im->im_name);

                return -1;
            }

            /* Check the versions are compatible. */
            if (im->im_version >= 0 || em->em_version >= 0)
                if (im->im_version != em->em_version)
                {
                    PyErr_Format(PyExc_RuntimeError, "the %s module is version %d but the %s module requires version %d", em->em_name, em->em_version, client->em_name, im->im_version);

                    return -1;
                }

            /* Save the imported module. */
            im->im_module = em;

            ++im;
        }
    }

    /* Create the module's classes. */
    if ((mw = client->em_types) != NULL)
        for (i = 0; i < client->em_nrtypes; ++i, ++mw)
        {
            sipTypeDef *td = (sipTypeDef *)*mw;

            /* Skip external classes. */
            if (td == NULL)
                continue;

            /* See if this is a namespace extender. */
            if (td->td_name == NULL)
            {
                sipTypeDef **last;
                sipWrapperType *wt = getClassType(&td->td_scope, client);

                /* Append this type to the real one. */
                last = &wt->type->td_nsextender;

                while (*last != NULL)
                    last = &(*last)->td_nsextender;

                *last = td;

                /*
                 * Set this so that the extender's original
                 * module can be found.
                 */
                td->td_module = client;

                /*
                 * Save the real namespace type so that it is
                 * the correct scope for any enums or classes
                 * defined in this module.
                 */
                *mw = wt;
            }
            else if ((*mw = createType(client, td, mod_dict)) == NULL)
                return -1;
        }

    /* Set any Qt support API. */
    if (client->em_qt_api != NULL)
    {
        sipQtSupport = client->em_qt_api;
        sipQObjectClass = *sipQtSupport->qt_qobject;
    }

    /* Append any initialiser extenders to the relevant classes. */
    if ((ie = client->em_initextend) != NULL)
        while (ie->ie_extender != NULL)
        {
            sipWrapperType *wt = getClassType(&ie->ie_class, client);

            ie->ie_next = wt->iextend;
            wt->iextend = ie;

            ++ie;
        }

    /* Set the base class object for any sub-class convertors. */
    if ((scc = client->em_convertors) != NULL)
        while (scc->scc_convertor != NULL)
        {
            scc->scc_basetype = getClassType(&scc->scc_base, client);

            ++scc;
        }

    /* Create the module's enums. */
    if (client->em_nrenums != 0)
    {
        if ((client->em_enums = sip_api_malloc(client->em_nrenums * sizeof (PyTypeObject *))) == NULL)
            return -1;

        for (i = 0; i < client->em_nrenums; ++i)
            if ((client->em_enums[i] = createEnum(client, &client->em_enumdefs[i], mod_dict)) == NULL)
                return -1;
    }

    for (emd = client->em_enummembers, i = 0; i < client->em_nrenummembers; ++i, ++emd)
    {
        PyObject *mo;

        if ((mo = sip_api_convert_from_named_enum(emd->em_val, client->em_enums[emd->em_enum])) == NULL)
            return -1;

        if (PyDict_SetItemString(mod_dict, emd->em_name, mo) < 0)
            return -1;

        Py_DECREF(mo);
    }


    /*
     * Add any class static instances.  We need to do this once all types are
     * fully formed because of potential interdependencies.
     */
    if ((mw = client->em_types) != NULL)
        for (i = 0; i < client->em_nrtypes; ++i)
        {
            sipWrapperType *wt;

            if ((wt = *mw++) != NULL && addInstances(((PyTypeObject *)wt)->tp_dict, &wt->type->td_instances) < 0)
                return -1;
        }

    /* Add any global static instances. */
    if (addInstances(mod_dict, &client->em_instances) < 0)
        return -1;

    /* Add any license. */
    if (client->em_license != NULL && addLicense(mod_dict, client->em_license) < 0)
        return -1;

    /* See if the new module satisfies any outstanding external types. */
    for (em = clientList; em != NULL; em = em->em_next)
    {
        sipExternalTypeDef *etd;

        if (em->em_external == NULL)
            continue;

        for (etd = em->em_external; etd->et_nr >= 0; ++etd)
        {
            if (etd->et_name == NULL)
                continue;

            mw = client->em_types;

            for (i = 0; i < client->em_nrtypes; ++i)
            {
                sipWrapperType *wt;
                const char *tname;

                if ((wt = *mw++) == NULL)
                    continue;

                tname = strchr(wt->type->td_name, '.') + 1;

                if (strcmp(etd->et_name, tname) == 0)
                {
                    em->em_types[etd->et_nr] = wt;
                    etd->et_name = NULL;

                    break;
                }
            }
        }
    }

    /* Add to the list of client modules. */
    client->em_next = clientList;
    clientList = client;

    return 0;
}


/*
 * Called by the interpreter to do any final clearing up, just in case the
 * interpreter will re-start.
 */
static void finalise(void)
{
    sipExportedModuleDef *em;

    /* Mark the Python API as unavailable. */
    sipInterpreter = NULL;

    /* Handle any delayed dtors. */
    for (em = clientList; em != NULL; em = em->em_next)
        if (em->em_ddlist != NULL)
        {
            em->em_delayeddtors(em->em_ddlist);

            /* Free the list. */
            do
            {
                sipDelayedDtor *dd = em->em_ddlist;

                em->em_ddlist = dd->dd_next;
                sip_api_free(dd);
            }
            while (em->em_ddlist != NULL);
        }

    licenseName = NULL;
    licenseeName = NULL;
    typeName = NULL;
    timestampName = NULL;
    signatureName = NULL;

    /* Release all memory we've allocated directly. */
    sipOMFinalise(&cppPyMap);

    /* Re-initialise those globals that (might) need it. */
    clientList = NULL;
}


/*
 * Add a wrapped C/C++ pointer to the list of delayed dtors.
 */
static void sip_api_add_delayed_dtor(sipWrapper *w)
{
    void *ptr;
    sipTypeDef *td;
    sipExportedModuleDef *em;

    if ((ptr = getPtrTypeDef(w, &td)) == NULL)
        return;

    /* Find the defining module. */
    for (em = clientList; em != NULL; em = em->em_next)
    {
        int i;

        for (i = 0; i < em->em_nrtypes; ++i)
            if (em->em_types[i] != NULL && em->em_types[i]->type == td)
            {
                sipDelayedDtor *dd;

                if ((dd = sip_api_malloc(sizeof (sipDelayedDtor))) == NULL)
                    return;

                /* Add to the list. */
                dd->dd_ptr = ptr;
                dd->dd_name = getBaseName(td->td_name);
                dd->dd_isderived = sipIsDerived(w);
                dd->dd_next = em->em_ddlist;

                em->em_ddlist = dd;

                return;
            }
    }
}


/*
 * A wrapper around the Python memory allocater that will raise an exception if
 * if the allocation fails.
 */
void *sip_api_malloc(size_t nbytes)
{
    void *mem;

    if ((mem = PyMem_Malloc(nbytes)) == NULL)
        PyErr_NoMemory();

    return mem;
}


/*
 * A wrapper around the Python memory de-allocater.
 */
void sip_api_free(void *mem)
{
    PyMem_Free(mem);
}


/*
 * Extend a Python slot by looking in other modules to see if there is an
 * extender function that can handle the arguments.
 */
static PyObject *sip_api_pyslot_extend(sipExportedModuleDef *mod,
                       sipPySlotType st, sipWrapperType *type,
                       PyObject *arg0, PyObject *arg1)
{
    sipExportedModuleDef *em;

    /* Go through each module. */
    for (em = clientList; em != NULL; em = em->em_next)
    {
        sipPySlotExtenderDef *ex;

        /* Skip the module that couldn't handle the arguments. */
        if (em == mod)
            continue;

        /* Skip if the module doesn't have any extenders. */
        if (em->em_slotextend == NULL)
            continue;

        /* Go through each extender. */
        for (ex = em->em_slotextend; ex->pse_func != NULL; ++ex)
        {
            PyObject *res;

            /* Skip if not the right slot type. */
            if (ex->pse_type != st)
                continue;

            /* Check against the type if one was given. */
            if (type != NULL && type != getClassType(&ex->pse_class, NULL))
                continue;

            PyErr_Clear();

            res = ((binaryfunc)ex->pse_func)(arg0, arg1);

            if (res != Py_NotImplemented)
                return res;
        }
    }

    /* The arguments couldn't handled anywhere. */
    PyErr_Clear();

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}


/*
 * Call the Python re-implementation of a C++ virtual.
 */
static PyObject *sip_api_call_method(int *isErr, PyObject *method,
        const char *fmt, ...)
{
    PyObject *args, *res;
    va_list va;

    va_start(va,fmt);

    if ((args = PyTuple_New(strlen(fmt))) != NULL && buildObject(args,fmt,va) != NULL)
        res = PyEval_CallObject(method,args);
    else
    {
        res = NULL;

        if (isErr != NULL)
            *isErr = TRUE;
    }

    Py_XDECREF(args);

    va_end(va);

    return res;
}


/*
 * Build a result object based on a format string.
 */
static PyObject *sip_api_build_result(int *isErr, const char *fmt, ...)
{
    PyObject *res = NULL;
    int badfmt, tupsz;
    va_list va;

    va_start(va,fmt);

    /* Basic validation of the format string. */

    badfmt = FALSE;

    if (*fmt == '(')
    {
        char *ep;

        if ((ep = strchr(fmt,')')) == NULL || ep[1] != '\0')
            badfmt = TRUE;
        else
            tupsz = ep - fmt - 1;
    }
    else if (strlen(fmt) == 1)
        tupsz = -1;
    else
        badfmt = TRUE;

    if (badfmt)
        PyErr_Format(PyExc_SystemError,"sipBuildResult(): invalid format string \"%s\"",fmt);
    else if (tupsz < 0 || (res = PyTuple_New(tupsz)) != NULL)
        res = buildObject(res,fmt,va);

    va_end(va);

    if (res == NULL && isErr != NULL)
        *isErr = TRUE;

    return res;
}


/*
 * Get the values off the stack and put them into an object.
 */
static PyObject *buildObject(PyObject *obj, const char *fmt, va_list va)
{
    char ch, termch;
    int i;

    /*
     * The format string has already been checked that it is properly
     * formed if it is enclosed in parenthesis.
     */
    if (*fmt == '(')
    {
        termch = ')';
        ++fmt;
    }
    else
        termch = '\0';

    i = 0;

    while ((ch = *fmt++) != termch)
    {
        PyObject *el;

        switch (ch)
        {
        case 'a':
            {
                char *s;
                int l;

                s = va_arg(va,char *);
                l = va_arg(va,int);

                if (s != NULL)
                    el = PyString_FromStringAndSize(s,l);
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'b':
            el = PyBool_FromLong(va_arg(va,int));
            break;

        case 'c':
            {
                char c = va_arg(va, int);

                el = PyString_FromStringAndSize(&c,1);
            }

            break;

        case 'e':
            el = PyInt_FromLong(va_arg(va,int));
            break;

        case 'E':
            {
                int ev = va_arg(va, int);
                PyTypeObject *et = va_arg(va, PyTypeObject *);

                el = sip_api_convert_from_named_enum(ev, et);
            }

            break;

        case 'd':
        case 'f':
            el = PyFloat_FromDouble(va_arg(va,double));
            break;

        case 'h':
        case 'i':
            el = PyInt_FromLong(va_arg(va,int));
            break;

        case 'l':
            el = PyLong_FromLong(va_arg(va,long));
            break;

        case 'm':
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned long));
            break;

        case 'n':
#if defined(HAVE_LONG_LONG)
            el = PyLong_FromLongLong(va_arg(va, PY_LONG_LONG));
#else
            el = PyLong_FromLong(va_arg(va, long));
#endif
            break;

        case 'o':
#if defined(HAVE_LONG_LONG)
            el = PyLong_FromUnsignedLongLong(va_arg(va, unsigned PY_LONG_LONG));
#else
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned long));
#endif
            break;

        case 's':
            {
                char *s = va_arg(va,char *);

                if (s != NULL)
                    el = PyString_FromString(s);
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 't':
        case 'u':
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned));
            break;

        case 'B':
            {
                void *p = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va, sipWrapperType *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_new_instance(p, wt, xfer);
            }

            break;

        case 'C':
            {
                void *p = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va, sipWrapperType *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_instance(p, wt, xfer);
            }

            break;

        case 'D':
            {
                void *p = va_arg(va, void *);
                const sipMappedType *mt = va_arg(va, const sipMappedType *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_mapped_type(p, mt, xfer);
            }

            break;

        case 'M':
        case 'O':
            {
                void *sipCpp = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va,sipWrapperType *);

                el = sip_api_convert_from_instance(sipCpp,wt,NULL);
            }

            break;

        case 'N':
        case 'P':
            {
                void *sipCpp = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va,sipWrapperType *);

                el = sip_api_convert_from_new_instance(sipCpp,wt,NULL);
            }

            break;

        case 'R':
            el = va_arg(va,PyObject *);
            break;

        case 'S':
            el = va_arg(va,PyObject *);
            Py_INCREF(el);
            break;

        case 'T':
            {
                void *sipCpp = va_arg(va,void *);
                sipConvertFromFunc func = va_arg(va,sipConvertFromFunc);

                el = func(sipCpp, NULL);
            }

            break;

        case 'V':
            el = sip_api_convert_from_void_ptr(va_arg(va,void *));
            break;

        default:
            PyErr_Format(PyExc_SystemError,"buildObject(): invalid format character '%c'",ch);
            el = NULL;
        }

        if (el == NULL)
        {
            Py_XDECREF(obj);
            return NULL;
        }

        if (obj == NULL)
            return el;

        PyTuple_SET_ITEM(obj,i,el);
        ++i;
    }

    return obj;
}


/*
 * Parse a result object based on a format string.
 */
static int sip_api_parse_result(int *isErr, PyObject *method, PyObject *res,
        const char *fmt, ...)
{
    int tupsz, rc = 0;
    va_list va;

    va_start(va,fmt);

    /* Basic validation of the format string. */

    if (*fmt == '(')
    {
        char *ep;

        if ((ep = strchr(fmt,')')) == NULL || ep[1] != '\0')
        {
            PyErr_Format(PyExc_SystemError, "sipParseResult(): invalid format string \"%s\"", fmt);
            rc = -1;
        }
        else
        {
            tupsz = ep - ++fmt;

            if (tupsz >= 0 && (!PyTuple_Check(res) || PyTuple_GET_SIZE(res) != tupsz))
            {
                sip_api_bad_catcher_result(method);
                rc = -1;
            }
        }
    }
    else
        tupsz = -1;

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
                arg = PyTuple_GET_ITEM(res,i);
                ++i;
            }
            else
                arg = res;

            switch (ch)
            {
            case 'a':
                {
                    char **p = va_arg(va,char **);
                    int *szp = va_arg(va,int *);

                    if (arg == Py_None)
                    {
                        *p = NULL;
                        *szp = 0;
                    }
                    else if (PyString_Check(arg))
                    {
                        *p = PyString_AS_STRING(arg);
                        *szp = PyString_GET_SIZE(arg);
                    }
                    else
                        invalid = TRUE;
                }

                break;

            case 'b':
                {
                    int v = PyInt_AsLong(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        sipSetBool(va_arg(va,void *),v);
                }

                break;

            case 'c':
                {
                    if (PyString_Check(arg) && PyString_GET_SIZE(arg) == 1)
                        *va_arg(va,char *) = *PyString_AS_STRING(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case 'd':
                {
                    double v = PyFloat_AsDouble(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,double *) = v;
                }

                break;

            case 'e':
                {
                    int v = PyInt_AsLong(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,int *) = v;
                }

                break;

            case 'E':
                {
                    PyTypeObject *et = va_arg(va, PyTypeObject *);
                    int *p = va_arg(va, int *);

                    if (PyObject_TypeCheck(arg, et))
                        *p = PyInt_AsLong(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case 'f':
                {
                    float v = PyFloat_AsDouble(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,float *) = v;
                }

                break;

            case 'h':
                {
                    short v = PyInt_AsLong(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,short *) = v;
                }

                break;

            case 't':
                {
                    unsigned short v = sip_api_long_as_unsigned_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,unsigned short *) = v;
                }

                break;

            case 'i':
                {
                    int v = PyInt_AsLong(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,int *) = v;
                }

                break;

            case 'u':
                {
                    unsigned v = sip_api_long_as_unsigned_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,unsigned *) = v;
                }

                break;

            case 'l':
                {
                    long v = PyLong_AsLong(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,long *) = v;
                }

                break;

            case 'm':
                {
                    unsigned long v = sip_api_long_as_unsigned_long(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va, unsigned long *) = v;
                }

                break;

            case 'n':
                {
#if defined(HAVE_LONG_LONG)
                    PY_LONG_LONG v = PyLong_AsLongLong(arg);
#else
                    long v = PyLong_AsLong(arg);
#endif

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
#if defined(HAVE_LONG_LONG)
                        *va_arg(va, PY_LONG_LONG *) = v;
#else
                        *va_arg(va, long *) = v;
#endif
                }

                break;

            case 'o':
                {
#if defined(HAVE_LONG_LONG)
                    unsigned PY_LONG_LONG v = PyLong_AsUnsignedLongLong(arg);
#else
                    unsigned long v = PyLong_AsUnsignedLong(arg);
#endif

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
#if defined(HAVE_LONG_LONG)
                        *va_arg(va, unsigned PY_LONG_LONG *) = v;
#else
                        *va_arg(va, unsigned long *) = v;
#endif
                }

                break;

            case 's':
                {
                    char **p = va_arg(va,char **);

                    if (arg == Py_None)
                        *p = NULL;
                    else if (PyString_Check(arg))
                        *p = PyString_AS_STRING(arg);
                    else
                        invalid = TRUE;
                }

                break;

            case 'C':
                {
                    if (*fmt == '\0')
                        invalid = TRUE;
                    else
                    {
                        int flags = *fmt++ - '0';
                        int iserr = FALSE;
                        sipWrapperType *type;
                        void **cpp;
                        int *state;

                        type = va_arg(va, sipWrapperType *);

                        if (flags & FORMAT_NO_STATE)
                            state = NULL;
                        else
                            state = va_arg(va, int *);

                        cpp = va_arg(va, void **);

                        *cpp = sip_api_force_convert_to_instance(arg, type, (flags & FORMAT_FACTORY ? arg : NULL), (flags & FORMAT_DEREF ? SIP_NOT_NONE : 0), state, &iserr);

                        if (iserr)
                            invalid = TRUE;
                    }
                }

                break;

            case 'D':
                {
                    if (*fmt == '\0')
                        invalid = TRUE;
                    else
                    {
                        int flags = *fmt++ - '0';
                        int iserr = FALSE;
                        const sipMappedType *mt;
                        void **cpp;
                        int *state;

                        mt = va_arg(va, const sipMappedType *);

                        if (flags & FORMAT_NO_STATE)
                            state = NULL;
                        else
                            state = va_arg(va, int *);

                        cpp = va_arg(va, void **);

                        *cpp = sip_api_force_convert_to_mapped_type(arg, mt, (flags & FORMAT_FACTORY ? arg : NULL), (flags & FORMAT_DEREF ? SIP_NOT_NONE : 0), state, &iserr);

                        if (iserr)
                            invalid = TRUE;
                    }
                }

                break;

            case 'L':
                {
                    sipForceConvertToFunc func = va_arg(va,sipForceConvertToFunc);
                    void **sipCpp = va_arg(va,void **);
                    int iserr = FALSE;

                    *sipCpp = func(arg,&iserr);

                    if (iserr)
                        invalid = TRUE;
                }

                break;

            case 'M':
                {
                    sipForceConvertToFunc func = va_arg(va,sipForceConvertToFunc);
                    void **sipCpp = va_arg(va,void **);
                    int iserr = FALSE;

                    *sipCpp = func(arg,&iserr);

                    if (iserr || *sipCpp == NULL)
                        invalid = TRUE;
                }

                break;

            case 'N':
                {
                    PyTypeObject *type = va_arg(va,PyTypeObject *);
                    PyObject **p = va_arg(va,PyObject **);

                    if (arg == Py_None || PyObject_TypeCheck(arg,type))
                    {
                        Py_INCREF(arg);
                        *p = arg;
                    }
                    else
                        invalid = TRUE;
                }

                break;

            case 'O':
                Py_INCREF(arg);
                *va_arg(va,PyObject **) = arg;
                break;

            case 'T':
                {
                    PyTypeObject *type = va_arg(va,PyTypeObject *);
                    PyObject **p = va_arg(va,PyObject **);

                    if (PyObject_TypeCheck(arg,type))
                    {
                        Py_INCREF(arg);
                        *p = arg;
                    }
                    else
                        invalid = TRUE;
                }

                break;

            case 'V':
                {
                    void *v = sip_api_convert_to_void_ptr(arg);

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va,void **) = v;
                }

                break;

            case 'Z':
                if (arg != Py_None)
                    invalid = TRUE;

                break;

            default:
                PyErr_Format(PyExc_SystemError,"sipParseResult(): invalid format character '%c'",ch);
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

    va_end(va);

    if (isErr != NULL && rc < 0)
        *isErr = TRUE;

    return rc;
}


/*
 * A thin wrapper around PyLong_AsUnsignedLong() that works around a bug in
 * Python versions prior to v2.4 where an integer (or a named enum) causes an
 * error.
 */
static unsigned long sip_api_long_as_unsigned_long(PyObject *o)
{
#if PY_VERSION_HEX < 0x02040000
    if (o != NULL && !PyLong_Check(o) && PyInt_Check(o))
    {
        long v = PyInt_AsLong(o);

        if (v < 0)
        {
            PyErr_SetString(PyExc_OverflowError,
            "can't convert negative value to unsigned long");

            return (unsigned long)-1;
        }

        return v;
    }
#endif

    return PyLong_AsUnsignedLong(o);
}


/*
 * Parse the arguments to a C/C++ function without any side effects.
 */
static int sip_api_parse_args(int *argsParsedp, PyObject *sipArgs,
        const char *fmt, ...)
{
    int valid, nrargs, selfarg;
    sipWrapper *self;
    PyObject *single_arg;
    va_list va;

    /* Previous sticky errors stop subsequent parses. */
    if (*argsParsedp & PARSE_STICKY)
        return 0;

    /* See if we are parsing a tuple or a single argument. */
    if (PyTuple_Check(sipArgs))
    {
        Py_INCREF(sipArgs);
        nrargs = PyTuple_GET_SIZE(sipArgs);
    }
    else if ((single_arg = PyTuple_New(1)) != NULL)
    {
        Py_INCREF(sipArgs);
        PyTuple_SET_ITEM(single_arg,0,sipArgs);

        sipArgs = single_arg;
        nrargs = 1;
    }
    else
        return 0;

    /*
     * The first pass checks all the types and does conversions that are
     * cheap and have no side effects.
     */
    va_start(va,fmt);
    valid = parsePass1(&self,&selfarg,&nrargs,sipArgs,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        int pvalid, pnrargs;

        /*
         * Use this error if there was no previous error, or if we
         * have parsed more arguments this time, or if the previous
         * error was that there were too many arguments.
         */
        pvalid = (*argsParsedp & PARSE_MASK);
        pnrargs = (*argsParsedp & ~PARSE_MASK);

        if (pvalid == PARSE_OK || pnrargs < nrargs ||
            (pnrargs == nrargs && pvalid == PARSE_MANY))
            *argsParsedp = valid | nrargs;

        Py_DECREF(sipArgs);

        return 0;
    }

    /*
     * The second pass does any remaining conversions now that we know we
     * have the right signature.
     */
    va_start(va,fmt);
    valid = parsePass2(self,selfarg,nrargs,sipArgs,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        *argsParsedp = valid | PARSE_STICKY;

        Py_DECREF(sipArgs);

        return 0;
    }

    *argsParsedp = nrargs;

    Py_DECREF(sipArgs);

    return 1;
}


/*
 * Parse a pair of arguments to a C/C++ function without any side effects.
 */
static int sip_api_parse_pair(int *argsParsedp, PyObject *sipArg0,
        PyObject *sipArg1, const char *fmt, ...)
{
    int valid, nrargs, selfarg;
    sipWrapper *self;
    PyObject *args;
    va_list va;

    /* Previous sticky errors stop subsequent parses. */
    if (*argsParsedp & PARSE_STICKY)
        return 0;

    if ((args = PyTuple_New(2)) == NULL)
        return 0;

    Py_INCREF(sipArg0);
    PyTuple_SET_ITEM(args, 0, sipArg0);

    Py_INCREF(sipArg1);
    PyTuple_SET_ITEM(args, 1, sipArg1);

    nrargs = 2;

    /*
     * The first pass checks all the types and does conversions that are
     * cheap and have no side effects.
     */
    va_start(va,fmt);
    valid = parsePass1(&self,&selfarg,&nrargs,args,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        int pvalid, pnrargs;

        /*
         * Use this error if there was no previous error, or if we
         * have parsed more arguments this time, or if the previous
         * error was that there were too many arguments.
         */
        pvalid = (*argsParsedp & PARSE_MASK);
        pnrargs = (*argsParsedp & ~PARSE_MASK);

        if (pvalid == PARSE_OK || pnrargs < nrargs ||
            (pnrargs == nrargs && pvalid == PARSE_MANY))
            *argsParsedp = valid | nrargs;

        Py_DECREF(args);

        return 0;
    }

    /*
     * The second pass does any remaining conversions now that we know we
     * have the right signature.
     */
    va_start(va,fmt);
    valid = parsePass2(self,selfarg,nrargs,args,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        *argsParsedp = valid | PARSE_STICKY;

        Py_DECREF(args);

        return 0;
    }

    *argsParsedp = nrargs;

    Py_DECREF(args);

    return 1;
}


/*
 * First pass of the argument parse, converting those that can be done so
 * without any side effects.  Return PARSE_OK if the arguments matched.
 */
static int parsePass1(sipWrapper **selfp, int *selfargp, int *argsParsedp,
        PyObject *sipArgs, const char *fmt, va_list va)
{
    int valid, compulsory, nrargs, argnr, nrparsed;

    valid = PARSE_OK;
    nrargs = *argsParsedp;
    nrparsed = 0;
    compulsory = TRUE;
    argnr = 0;

    /*
     * Handle those format characters that deal with the "self" argument.
     * They will always be the first one.
     */
    *selfp = NULL;
    *selfargp = FALSE;

    switch (*fmt++)
    {
    case 'B':
    case 'p':
        {
            PyObject *self;
            sipWrapperType *type;

            self = *va_arg(va,PyObject **);
            type = va_arg(va,sipWrapperType *);
            va_arg(va,void **);

            if (self == NULL)
            {
                if ((valid = getSelfFromArgs(type,sipArgs,argnr,selfp)) != PARSE_OK)
                    break;

                *selfargp = TRUE;
                ++nrparsed;
                ++argnr;
            }
            else
                *selfp = (sipWrapper *)self;

            break;
        }

    case 'C':
        *selfp = (sipWrapper *)va_arg(va,PyObject *);
        break;

    default:
        --fmt;
    }

    /* Now handle the remaining arguments. */
    while (valid == PARSE_OK)
    {
        char ch;
        PyObject *arg;

        PyErr_Clear();

        /* See if the following arguments are optional. */
        if ((ch = *fmt++) == '|')
        {
            compulsory = FALSE;
            ch = *fmt++;
        }

        /* See if we don't expect anything else. */

        if (ch == '\0')
        {
            /* Invalid if there are still arguments. */
            if (argnr < nrargs)
                valid = PARSE_MANY;

            break;
        }

        /* See if we have run out of arguments. */

        if (argnr == nrargs)
        {
            /*
             * It is an error if we are still expecting compulsory
             * arguments unless the current argume is an ellipsis.
             */
            if (ch != 'W' && ch != '\0' && compulsory)
                valid = PARSE_FEW;

            break;
        }

        /* Get the next argument. */
        arg = PyTuple_GET_ITEM(sipArgs,argnr);
        ++argnr;

        switch (ch)
        {
        case 'W':
            /* Ellipsis. */
            break;

        case 's':
            {
                /* String or None. */

                char **p = va_arg(va,char **);

                if (arg == Py_None)
                    *p = NULL;
                else if (PyString_Check(arg))
                    *p = PyString_AS_STRING(arg);
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'U':
            {
                /*
                 * Slot name or callable, return the name or
                 * callable.
                 */

                char **sname = va_arg(va, char **);
                PyObject **scall = va_arg(va, PyObject **);

                *sname = NULL;
                *scall = NULL;

                if (PyString_Check(arg))
                {
                    char *s = PyString_AS_STRING(arg);

                    if (*s == '1' || *s == '2' || *s == '9')
                        *sname = s;
                    else
                        valid = PARSE_TYPE;
                }
                else if (PyCallable_Check(arg))
                    *scall = arg;
                else if (arg != Py_None)
                    valid = PARSE_TYPE;

                break;
            }

        case 'S':
            {
                /* Slot name, return the name. */

                if (PyString_Check(arg))
                {
                    char *s = PyString_AS_STRING(arg);

                    if (*s == '1' || *s == '2' || *s == '9')
                        *va_arg(va,char **) = s;
                    else
                        valid = PARSE_TYPE;
                }
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'G':
            {
                /* Signal name, return the name. */

                if (PyString_Check(arg))
                {
                    char *s = PyString_AS_STRING(arg);

                    if (*s == '2' || *s == '9')
                        *va_arg(va,char **) = s;
                    else
                        valid = PARSE_TYPE;
                }
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'J':
            {
                /* Class instance. */

                if (*fmt == '\0')
                    valid = PARSE_FORMAT;
                else
                {
                    int flags = *fmt++ - '0';
                    sipWrapperType *type;
                    int iflgs = 0;

                    type = va_arg(va,sipWrapperType *);
                    va_arg(va,void **);

                    if (flags & FORMAT_DEREF)
                        iflgs |= SIP_NOT_NONE;

                    if (flags & FORMAT_GET_WRAPPER)
                        va_arg(va,PyObject **);

                    if (flags & FORMAT_NO_CONVERTORS)
                        iflgs |= SIP_NO_CONVERTORS;
                    else
                        va_arg(va, int *);

                    if (!sip_api_can_convert_to_instance(arg, type, iflgs))
                        valid = PARSE_TYPE;
                }

                break;
            }

        case 'M':
            {
                /* Mapped type instance. */

                if (*fmt == '\0')
                    valid = PARSE_FORMAT;
                else
                {
                    int flags = *fmt++ - '0';
                    sipMappedType *mt;
                    int iflgs = 0;

                    mt = va_arg(va, sipMappedType *);
                    va_arg(va, void **);
                    va_arg(va, int *);

                    if (flags & FORMAT_DEREF)
                        iflgs |= SIP_NOT_NONE;

                    if (!sip_api_can_convert_to_mapped_type(arg, mt, iflgs))
                        valid = PARSE_TYPE;
                }

                break;
            }


        case 'N':
            {
                /* Python object of given type or None. */

                PyTypeObject *type = va_arg(va,PyTypeObject *);
                PyObject **p = va_arg(va,PyObject **);

                if (arg == Py_None || PyObject_TypeCheck(arg,type))
                    *p = arg;
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'P':
            {
                /*
                 * Python object of any type with a
                 * sub-format.
                 */

                *va_arg(va,PyObject **) = arg;

                /* Skip the sub-format. */
                if (*fmt++ == '\0')
                    valid = PARSE_FORMAT;

                break;
            }

        case 'T':
            {
                /* Python object of given type. */

                PyTypeObject *type = va_arg(va,PyTypeObject *);
                PyObject **p = va_arg(va,PyObject **);

                if (PyObject_TypeCheck(arg,type))
                    *p = arg;
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'R':
            {
                /* Sub-class of QObject. */

                if (sipQtSupport == NULL || !PyObject_TypeCheck(arg, (PyTypeObject *)sipQObjectClass))
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,PyObject **) = arg;

                break;
            }

        case 'F':
            {
                /* Python callable object. */
 
                if (PyCallable_Check(arg))
                    *va_arg(va,PyObject **) = arg;
                else
                    valid = PARSE_TYPE;
 
                break;
            }

        case 'H':
            {
                /* Python callable object or None. */
 
                if (arg == Py_None || PyCallable_Check(arg))
                    *va_arg(va,PyObject **) = arg;
                else
                    valid = PARSE_TYPE;
 
                break;
            }

        case 'q':
            {
                /* Qt receiver to connect. */

                va_arg(va,char *);
                va_arg(va,void **);
                va_arg(va,const char **);

                if (sipQtSupport == NULL || !PyObject_TypeCheck(arg, (PyTypeObject *)sipQObjectClass))
                    valid = PARSE_TYPE;

                break;
            }

        case 'Q':
            {
                /* Qt receiver to disconnect. */

                va_arg(va,char *);
                va_arg(va,void **);
                va_arg(va,const char **);

                if (sipQtSupport == NULL || !PyObject_TypeCheck(arg, (PyTypeObject *)sipQObjectClass))
                    valid = PARSE_TYPE;

                break;
            }

        case 'y':
            {
                /* Python slot to connect. */

                va_arg(va,char *);
                va_arg(va,void **);
                va_arg(va,const char **);

                if (sipQtSupport == NULL || !PyCallable_Check(arg))
                    valid = PARSE_TYPE;

                break;
            }

        case 'Y':
            {
                /* Python slot to disconnect. */

                va_arg(va,char *);
                va_arg(va,void **);
                va_arg(va,const char **);

                if (sipQtSupport == NULL || !PyCallable_Check(arg))
                    valid = PARSE_TYPE;

                break;
            }

        case 'a':
            {
                /* Byte array or None. */

                char **p = va_arg(va,char **);
                int *szp = va_arg(va,int *);

                if (arg == Py_None)
                {
                    *p = NULL;
                    *szp = 0;
                }
                else if (PyString_Check(arg))
                {
                    *p = PyString_AS_STRING(arg);
                    *szp = PyString_GET_SIZE(arg);
                }
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'c':
            {
                /* Character. */

                if (PyString_Check(arg) && PyString_GET_SIZE(arg) == 1)
                    *va_arg(va,char *) = *PyString_AS_STRING(arg);
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'b':
            {
                /* Bool. */

                int v = PyInt_AsLong(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    sipSetBool(va_arg(va,void *),v);

                break;
            }

        case 'e':
            {
                /* Anonymous enum. */

                int v = PyInt_AsLong(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,int *) = v;

                break;
            }

        case 'E':
            {
                /* Named enum. */

                PyTypeObject *et = va_arg(va, PyTypeObject *);

                va_arg(va, int *);

                if (!PyObject_TypeCheck(arg, et))
                    valid = PARSE_TYPE;
            }

            break;

        case 'i':
            {
                /* Integer. */

                int v = PyInt_AsLong(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,int *) = v;

                break;
            }

        case 'u':
            {
                /* Unsigned integer. */

                unsigned v = sip_api_long_as_unsigned_long(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va, unsigned *) = v;

                break;
            }

        case 'h':
            {
                /* Short integer. */

                short v = PyInt_AsLong(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,short *) = v;

                break;
            }

        case 't':
            {
                /* Unsigned short integer. */

                unsigned short v = sip_api_long_as_unsigned_long(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va, unsigned short *) = v;

                break;
            }

        case 'l':
            {
                /* Long integer. */

                long v = PyLong_AsLong(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,long *) = v;

                break;
            }

        case 'm':
            {
                /* Unsigned long integer. */

                unsigned long v = sip_api_long_as_unsigned_long(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va, unsigned long *) = v;

                break;
            }

        case 'n':
            {
                /* Long long integer. */

#if defined(HAVE_LONG_LONG)
                PY_LONG_LONG v = PyLong_AsLongLong(arg);
#else
                long v = PyLong_AsLong(arg);
#endif

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
#if defined(HAVE_LONG_LONG)
                    *va_arg(va, PY_LONG_LONG *) = v;
#else
                    *va_arg(va, long *) = v;
#endif

                break;
            }

        case 'o':
            {
                /* Unsigned long long integer. */

#if defined(HAVE_LONG_LONG)
                unsigned PY_LONG_LONG v = PyLong_AsUnsignedLongLong(arg);
#else
                unsigned long v = PyLong_AsUnsignedLong(arg);
#endif

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
#if defined(HAVE_LONG_LONG)
                    *va_arg(va, unsigned PY_LONG_LONG *) = v;
#else
                    *va_arg(va, unsigned long *) = v;
#endif

                break;
            }

        case 'f':
            {
                /* Float. */

                double v = PyFloat_AsDouble(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,float *) = (float)v;

                break;
            }

        case 'X':
            {
                /* Constrained (ie. exact) types. */

                switch (*fmt++)
                {
                case 'b':
                    {
                        /* Boolean. */

                        if (PyBool_Check(arg))
                            sipSetBool(va_arg(va,void *),(arg == Py_True));
                        else
                            valid = PARSE_TYPE;

                        break;
                    }

                case 'd':
                    {
                        /* Double float. */

                        if (PyFloat_Check(arg))
                            *va_arg(va,double *) = PyFloat_AS_DOUBLE(arg);
                        else
                            valid = PARSE_TYPE;

                        break;
                    }

                case 'f':
                    {
                        /* Float. */

                        if (PyFloat_Check(arg))
                            *va_arg(va,float *) = (float)PyFloat_AS_DOUBLE(arg);
                        else
                            valid = PARSE_TYPE;

                        break;
                    }

                case 'i':
                    {
                        /* Integer. */

                        if (PyInt_Check(arg))
                            *va_arg(va,int *) = PyInt_AS_LONG(arg);
                        else
                            valid = PARSE_TYPE;

                        break;
                    }

                default:
                    valid = PARSE_FORMAT;
                }

                break;
            }

        case 'd':
            {
                /* Double float. */

                double v = PyFloat_AsDouble(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,double *) = v;

                break;
            }

        case 'v':
            {
                /* Void pointer. */

                void *v = sip_api_convert_to_void_ptr(arg);

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va,void **) = v;

                break;
            }

        default:
            valid = PARSE_FORMAT;
        }

        if (valid == PARSE_OK)
        {
            if (ch == 'W')
            {
                /*
                 * An ellipsis matches everything and ends the
                 * parse.
                 */
                nrparsed = nrargs;
                break;
            }

            ++nrparsed;
        }
    }

    *argsParsedp = nrparsed;

    return valid;
}


/*
 * Second pass of the argument parse, converting the remaining ones that might
 * have side effects.  Return PARSE_OK if there was no error.
 */
static int parsePass2(sipWrapper *self, int selfarg, int nrargs,
        PyObject *sipArgs, const char *fmt, va_list va)
{
    int a, valid;

    valid = PARSE_OK;

    /* Handle the converions of "self" first. */
    switch (*fmt++)
    {
    case 'B':
        {
            /*
             * The address of a C++ instance when calling one of
             * its public methods.
             */

            sipWrapperType *type;
            void **p;

            *va_arg(va,PyObject **) = (PyObject *)self;
            type = va_arg(va,sipWrapperType *);
            p = va_arg(va,void **);

            if ((*p = sip_api_get_cpp_ptr(self,type)) == NULL)
                valid = PARSE_RAISED;

            break;
        }

    case 'p':
        {
            /*
             * The address of a C++ instance when calling one of
             * its protected methods.
             */

            void **p;

            *va_arg(va,PyObject **) = (PyObject *)self;
            va_arg(va,sipWrapperType *);
            p = va_arg(va,void **);

            if ((*p = sip_api_get_complex_cpp_ptr(self)) == NULL)
                valid = PARSE_RAISED;

            break;
        }

    case 'C':
        va_arg(va,PyObject *);
        break;

    default:
        --fmt;
    }

    for (a = (selfarg ? 1 : 0); a < nrargs && *fmt != 'W' && valid == PARSE_OK; ++a)
    {
        char ch;
        PyObject *arg = PyTuple_GET_ITEM(sipArgs,a);

        /* Skip the optional character. */
        if ((ch = *fmt++) == '|')
            ch = *fmt++;

        /*
         * Do the outstanding conversions.  For most types it has
         * already been done, so we are just skipping the parameters.
         */
        switch (ch)
        {
        case 'q':
            {
                /* Qt receiver to connect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                if ((*rx = sip_api_convert_rx(self,sig,arg,*slot,slot)) == NULL)
                    valid = PARSE_RAISED;

                break;
            }

        case 'Q':
            {
                /* Qt receiver to disconnect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                *rx = sipGetRx(self,sig,arg,*slot,slot);
                break;
            }

        case 'y':
            {
                /* Python slot to connect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                if ((*rx = sip_api_convert_rx(self,sig,arg,NULL,slot)) == NULL)
                    valid = PARSE_RAISED;

                break;
            }

        case 'Y':
            {
                /* Python slot to disconnect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                *rx = sipGetRx(self,sig,arg,NULL,slot);
                break;
            }

        case 'J':
            {
                /* Class instance. */

                int flags = *fmt++ - '0';
                sipWrapperType *type;
                void **p;
                int iflgs = 0;
                int iserr = FALSE;
                int *state;
                PyObject *xfer, **wrapper;

                type = va_arg(va,sipWrapperType *);
                p = va_arg(va,void **);

                if (flags & FORMAT_TRANSFER)
                    xfer = (self ? (PyObject *)self : arg);
                else if (flags & FORMAT_TRANSFER_BACK)
                    xfer = Py_None;
                else
                    xfer = NULL;

                if (flags & FORMAT_DEREF)
                    iflgs |= SIP_NOT_NONE;

                if (flags & FORMAT_GET_WRAPPER)
                    wrapper = va_arg(va, PyObject **);
                else
                    wrapper = NULL;

                if (flags & FORMAT_NO_CONVERTORS)
                {
                    iflgs |= SIP_NO_CONVERTORS;
                    state = NULL;
                }
                else
                    state = va_arg(va, int *);

                *p = sip_api_convert_to_instance(arg, type, xfer, iflgs, state, &iserr);

                if (iserr)
                    valid = PARSE_RAISED;

                if (wrapper != NULL)
                    *wrapper = (*p != NULL ? arg : NULL);

                break;
            }

        case 'M':
            {
                /* Mapped type instance. */

                int flags = *fmt++ - '0';
                sipMappedType *mt;
                void **p;
                int iflgs = 0;
                int iserr = FALSE;
                int *state;
                PyObject *xfer;

                mt = va_arg(va, sipMappedType *);
                p = va_arg(va, void **);
                state = va_arg(va, int *);

                if (flags & FORMAT_TRANSFER)
                    xfer = (self ? (PyObject *)self : arg);
                else if (flags & FORMAT_TRANSFER_BACK)
                    xfer = Py_None;
                else
                    xfer = NULL;

                if (flags & FORMAT_DEREF)
                    iflgs |= SIP_NOT_NONE;

                *p = sip_api_convert_to_mapped_type(arg, mt, xfer, iflgs, state, &iserr);

                if (iserr)
                    valid = PARSE_RAISED;

                break;
            }

        case 'P':
            {
                /*
                 * Python object of any type with a
                 * sub-format.
                 */

                PyObject **p = va_arg(va,PyObject **);
                int flags = *fmt++ - '0';

                if (flags & FORMAT_TRANSFER)
                {
                    Py_XINCREF(*p);
                }
                else if (flags & FORMAT_TRANSFER_BACK)
                {
                    Py_XDECREF(*p);
                }

                break;
            }

        case 'X':
            {
                /* Constrained (ie. exact) type. */

                ++fmt;
                va_arg(va,void *);

                break;
            }


        case 'E':
            {
                /* Named enum. */

                int *p;

                va_arg(va, PyTypeObject *);
                p = va_arg(va, int *);

                *p = PyInt_AsLong(arg);

                break;
            }

        /*
         * Every other argument is a pointer and only differ in how
         * many there are.
         */
        case 'N':
        case 'T':
        case 'a':
            va_arg(va,void *);

            /* Drop through. */

        default:
            va_arg(va,void *);
        }
    }

    /* Handle any ellipsis argument. */
    if (*fmt == 'W' && valid == PARSE_OK)
    {
        PyObject *al;

        /* Create a tuple for any remaining arguments. */
        if ((al = PyTuple_New(nrargs - a)) != NULL)
        {
            int da = 0;

            while (a < nrargs)
            {
                PyObject *arg = PyTuple_GET_ITEM(sipArgs,a);

                /* Add the remaining argument to the tuple. */
                Py_INCREF(arg);
                PyTuple_SET_ITEM(al, da, arg);

                ++a;
                ++da;
            }

            /* Return the tuple. */
            *va_arg(va, PyObject **) = al;
        }
        else
            valid = PARSE_RAISED;
    }

    return valid;
}


/*
 * Carry out actions common to all ctors.
 */
static void sip_api_common_ctor(sipMethodCache *cache,int nrmeths)
{
    /* This is thread safe. */
    while (nrmeths-- > 0)
        cache++->mcflags = 0;
}


/*
 * Carry out actions common to all dtors.
 */
static void sip_api_common_dtor(sipWrapper *sipSelf)
{
    if (sipSelf != NULL && sipInterpreter != NULL)
    {
        SIP_BLOCK_THREADS

        callPyDtor(sipSelf);

        if (!sipNotInMap(sipSelf))
            sipOMRemoveObject(&cppPyMap,sipSelf);

        /* This no longer points to anything useful. */
        sipSelf->u.cppPtr = NULL;

        /*
         * If C/C++ has a reference (and therefore no parent) then remove it.
         * Otherwise remove the object from any parent.
         */
        if (sipCppHasRef(sipSelf))
        {
            sipResetCppHasRef(sipSelf);
            Py_DECREF(sipSelf);
        }
        else
            removeFromParent(sipSelf);

        SIP_UNBLOCK_THREADS
    }
}


/*
 * Call self.__dtor__() if it is implemented.
 */
static void callPyDtor(sipWrapper *self)
{
    sip_gilstate_t sipGILState;
    sipMethodCache pymc;
    PyObject *meth;

    /* No need to cache the method, it will only be called once. */
    pymc.mcflags = 0;
    meth = sip_api_is_py_method(&sipGILState, &pymc, self, NULL, "__dtor__");

    if (meth != NULL)
    {
        PyObject *res = sip_api_call_method(0, meth, "", NULL);

        Py_DECREF(meth);

        /* Discard any result. */
        Py_XDECREF(res);

        SIP_RELEASE_GIL(sipGILState);
    }
}


/*
 * Add a wrapper to it's parent owner if it has one.  The wrapper must not
 * currently have a parent and, therefore, no siblings.
 */
static void addToParent(sipWrapper *self, sipWrapper *owner)
{
    if (owner != NULL)
    {
        if (owner->first_child != NULL)
        {
            self->sibling_next = owner->first_child;
            owner->first_child->sibling_prev = self;
        }

        owner->first_child = self;
        self->parent = owner;

        /*
         * The owner holds a real reference so that the cyclic garbage
         * collector works properly.
         */
        Py_INCREF(self);
    }
}


/*
 * Remove a wrapper from it's parent if it has one.
 */
static void removeFromParent(sipWrapper *self)
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
         * We must do this last, after all the pointers are correct,
         * because this is used by the clear slot.
         */
        Py_DECREF(self);
    }
}


/*
 * Convert a sequence index.  Return the index or a negative value if there was
 * an error.
 */
static SIP_SSIZE_T sip_api_convert_from_sequence_index(SIP_SSIZE_T idx,
        SIP_SSIZE_T len)
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
 * Create and return a single type object.
 */
static sipWrapperType *createType(sipExportedModuleDef *client,
                  sipTypeDef *type, PyObject *mod_dict)
{
    PyObject *name, *bases, *typedict, *args, *dict;
    sipEncodedClassDef *sup;
    sipWrapperType *wt;

    /* Create an object corresponding to the type name. */
    if ((name = getBaseNameObject(type->td_name)) == NULL)
        goto reterr;

    /* Create the tuple of super types. */
    if ((sup = type->td_supers) == NULL)
    {
        static PyObject *nobases = NULL;

        if (nobases == NULL && (nobases = Py_BuildValue("(O)",&sipWrapper_Type)) == NULL)
            goto relname;

        Py_INCREF(nobases);
        bases = nobases;
    }
    else
    {
        int i, nrsupers = 0;

        do
            ++nrsupers;
        while (!sup++->sc_flag);

        if ((bases = PyTuple_New(nrsupers)) == NULL)
            goto relname;

        for (sup = type->td_supers, i = 0; i < nrsupers; ++i, ++sup)
        {
            PyObject *st = (PyObject *)getClassType(sup, client);

            Py_INCREF(st);
            PyTuple_SET_ITEM(bases,i,st);
        }
    }

    /* Create the type dictionary. */
    if ((typedict = createTypeDict(client->em_nameobj)) == NULL)
        goto relbases;

    /* Initialise the rest of the type and pass it via the back door. */
    type->td_module = client;
    currentType = type;

    /* Create the type by calling the metatype. */
    if ((args = Py_BuildValue("OOO",name,bases,typedict)) == NULL)
        goto reldict;

    if ((wt = (sipWrapperType *)PyObject_Call((PyObject *)&sipWrapperType_Type,args,NULL)) == NULL)
        goto relargs;

    /* Get the dictionary into which the type will be placed. */
    if (type->td_scope.sc_flag)
        dict = mod_dict;
    else
        dict = ((PyTypeObject *)getClassType(&type->td_scope, client))->tp_dict;

    /* Add the type to the "parent" dictionary. */
    if (PyDict_SetItem(dict,name,(PyObject *)wt) < 0)
        goto reltype;

    /* We can now release our references. */
    Py_DECREF(args);
    Py_DECREF(typedict);
    Py_DECREF(bases);
    Py_DECREF(name);

    return wt;

    /* Unwind after an error. */

reltype:
    Py_DECREF((PyObject *)wt);

relargs:
    Py_DECREF(args);

reldict:
    Py_DECREF(typedict);

relbases:
    Py_DECREF(bases);

relname:
    Py_DECREF(name);

reterr:
    return NULL;
}


/*
 * Create and return an enum type object.
 */
static PyTypeObject *createEnum(sipExportedModuleDef *client, sipEnumDef *ed,
                PyObject *mod_dict)
{
    static PyObject *bases = NULL;
    PyObject *name, *typedict, *args, *dict;
    PyTypeObject *et;

    /* Get the module and dictionary into which the type will be placed. */
    if (ed->e_scope < 0)
        dict = mod_dict;
    else
        dict = ((PyTypeObject *)client->em_types[ed->e_scope])->tp_dict;

    /* Create the base type tuple if it hasn't already been done. */
    if (bases == NULL && (bases = Py_BuildValue("(O)",&PyInt_Type)) == NULL)
        goto reterr;

    /* Create an object corresponding to the type name. */
    if ((name = getBaseNameObject(ed->e_name)) == NULL)
        goto reterr;

    /* Create the type dictionary. */
    if ((typedict = createTypeDict(client->em_nameobj)) == NULL)
        goto relname;

    /* Create the type by calling the metatype. */
    if ((args = Py_BuildValue("OOO",name,bases,typedict)) == NULL)
        goto reldict;

    if ((et = (PyTypeObject *)PyObject_Call((PyObject *)&PyType_Type,args,NULL)) == NULL)
        goto relargs;

    /* Initialise any slots. */
    if (ed->e_pyslots != NULL)
        initSlots(et, et->tp_as_number, et->tp_as_sequence, et->tp_as_mapping, ed->e_pyslots, TRUE);

    /* Add the type to the "parent" dictionary. */
    if (PyDict_SetItem(dict,name,(PyObject *)et) < 0)
        goto reltype;

    /* We can now release our references. */
    Py_DECREF(args);
    Py_DECREF(typedict);
    Py_DECREF(name);

    return et;

    /* Unwind after an error. */

reltype:
    Py_DECREF((PyObject *)et);

relargs:
    Py_DECREF(args);

reldict:
    Py_DECREF(typedict);

relname:
    Py_DECREF(name);

reterr:
    return NULL;
}


/*
 * Return a pointer to the basename of a Python "pathname".
 */
static const char *getBaseName(const char *name)
{
    const char *bn;

    if ((bn = strrchr(name, '.')) != NULL)
        ++bn;
    else
        bn = name;

    return bn;
}


/*
 * Create a Python object corresponding to the basename of a Python "pathname".
 */
static PyObject *getBaseNameObject(const char *name)
{
    return PyString_FromString(getBaseName(name));
}


/*
 * Create a type dictionary for dynamic type being created in the module with
 * the specified name.
 */
static PyObject *createTypeDict(PyObject *mname)
{
    static PyObject *proto = NULL;
    static PyObject *mstr = NULL;
    PyObject *dict;

    /* Create a prototype dictionary. */
    if (proto == NULL)
    {
        if ((proto = PyDict_New()) == NULL)
            return NULL;

        /*
         * These tell pickle that SIP generated classes can't be
         * pickled.
         */
        if (PyDict_SetItemString(proto, "__reduce_ex__", Py_None) < 0 ||
            PyDict_SetItemString(proto, "__reduce__", Py_None) < 0)
        {
            Py_DECREF(proto);
            proto = NULL;

            return NULL;
        }
    }

    /* Create an object for "__module__". */
    if (mstr == NULL && (mstr = PyString_FromString("__module__")) == NULL)
        return NULL;

    if ((dict = PyDict_Copy(proto)) == NULL)
        return NULL;

    /* We need to set the module name as an attribute for dynamic types. */
    if (PyDict_SetItem(dict, mstr, mname) < 0)
    {
        Py_DECREF(dict);
        return NULL;
    }

    return dict;
}


/*
 * Add a set of static instances to a dictionary.
 */
static int addInstances(PyObject *dict,sipInstancesDef *id)
{
    if (id->id_class != NULL && addClassInstances(dict,id->id_class) < 0)
        return -1;

    if (id->id_voidp != NULL && addVoidPtrInstances(dict,id->id_voidp) < 0)
        return -1;

    if (id->id_char != NULL && addCharInstances(dict,id->id_char) < 0)
        return -1;

    if (id->id_string != NULL && addStringInstances(dict,id->id_string) < 0)
        return -1;

    if (id->id_int != NULL && addIntInstances(dict, id->id_int) < 0)
        return -1;

    if (id->id_long != NULL && addLongInstances(dict,id->id_long) < 0)
        return -1;

    if (id->id_ulong != NULL && addUnsignedLongInstances(dict, id->id_ulong) < 0)
        return -1;

    if (id->id_llong != NULL && addLongLongInstances(dict, id->id_llong) < 0)
        return -1;

    if (id->id_ullong != NULL && addUnsignedLongLongInstances(dict, id->id_ullong) < 0)
        return -1;

    if (id->id_double != NULL && addDoubleInstances(dict,id->id_double) < 0)
        return -1;

    if (id->id_enum != NULL && addEnumInstances(dict,id->id_enum) < 0)
        return -1;

    return 0;
}


/*
 * Get "self" from the argument tuple for a method called as
 * Class.Method(self, ...) rather than self.Method(...).
 */
static int getSelfFromArgs(sipWrapperType *type, PyObject *args, int argnr,
        sipWrapper **selfp)
{
    PyObject *self;

    /* Get self from the argument tuple. */

    if (argnr >= PyTuple_GET_SIZE(args))
        return PARSE_UNBOUND;

    self = PyTuple_GET_ITEM(args, argnr);

    if (!PyObject_TypeCheck(self, (PyTypeObject *)type))
        return PARSE_UNBOUND;

    *selfp = (sipWrapper *)self;

    return PARSE_OK;
}


/*
 * Handle the result of a call to the class/instance setattro methods.
 */
static int handleSetLazyAttr(PyObject *nameobj,PyObject *valobj,
                 sipWrapperType *wt,sipWrapper *w)
{
    char *name;
    PyMethodDef *pmd, *vmd;
    sipEnumMemberDef *enm;

    /* See if it was a lazy attribute. */
    if ((name = PyString_AsString(nameobj)) == NULL)
        return -1;

    pmd = NULL;
    enm = NULL;
    vmd = NULL;

    findLazyAttr(wt,name,&pmd,&enm,&vmd,NULL);

    if (vmd != NULL)
    {
        if (valobj == NULL)
        {
            PyErr_Format(PyExc_ValueError,"%s.%s cannot be deleted",wt->type->td_name,name);

            return -1;
        }

        if ((vmd->ml_flags & METH_STATIC) != 0 || w != NULL)
        {
            PyObject *res;

            if ((res = (*vmd->ml_meth)((PyObject *)w,valobj)) == NULL)
                return -1;

            /* Ignore the result (which should be Py_None). */
            Py_DECREF(res);

            return 0;
        }

        PyErr_SetObject(PyExc_AttributeError,nameobj);

        return -1;
    }

    /* It isn't a variable. */
    return 1;
}


/*
 * Handle the result of a call to the class/instance getattro methods.
 */
static PyObject *handleGetLazyAttr(PyObject *nameobj,sipWrapperType *wt,
                   sipWrapper *w)
{
    char *name;
    PyMethodDef *pmd, *vmd;
    sipEnumMemberDef *enm;
    sipTypeDef *in;

    /* If it was an error, propagate it. */
    if (!PyErr_ExceptionMatches(PyExc_AttributeError))
        return NULL;

    PyErr_Clear();

    /* See if it was a lazy attribute. */
    if ((name = PyString_AsString(nameobj)) == NULL)
        return NULL;

    pmd = NULL;
    enm = NULL;
    vmd = NULL;

    findLazyAttr(wt,name,&pmd,&enm,&vmd,&in);

    if (pmd != NULL)
        return PyCFunction_New(pmd,(PyObject *)w);

    if (enm != NULL)
    {
        PyObject *attr;

        /*
         * Convert the value to an object.  Note that we cannot cache
         * it in the type dictionary because a sub-type might have a
         * lazy attribute of the same name.  In this case (because we
         * call the standard getattro code first) this one would be
         * wrongly found in preference to the one in the sub-class.
         * The example in PyQt is QScrollView::ResizePolicy and
         * QListView::WidthMode both having a member called Manual.
         * One way around this might be to cache them in a separate
         * dictionary and search that before doing the binary search
         * through the lazy enum table.
         */
        if ((attr = createEnumMember(in, enm)) == NULL)
            return NULL;

        return attr;
    }

    if (vmd != NULL)
        if ((vmd->ml_flags & METH_STATIC) != 0 || w != NULL)
            return (*vmd->ml_meth)((PyObject *)w,NULL);

    PyErr_SetObject(PyExc_AttributeError,nameobj);

    return NULL;
}


/*
 * Create a Python object for an enum member.
 */
static PyObject *createEnumMember(sipTypeDef *td, sipEnumMemberDef *enm)
{
    if (enm->em_enum < 0)
        return PyInt_FromLong(enm->em_val);

    return sip_api_convert_from_named_enum(enm->em_val, td->td_module->em_enums[enm->em_enum]);
}


/*
 * Create a Python object for a member of a named enum.
 */
PyObject *sip_api_convert_from_named_enum(int eval, PyTypeObject *et)
{
    PyObject *args, *mo;

    if ((args = Py_BuildValue("(i)", eval)) == NULL)
        return NULL;

    mo = PyObject_Call((PyObject *)et, args, NULL);

    Py_DECREF(args);

    return mo;
}


/*
 * Find definition for a lazy class attribute.
 */
static void findLazyAttr(sipWrapperType *wt,char *name,PyMethodDef **pmdp,
             sipEnumMemberDef **enmp,PyMethodDef **vmdp,
             sipTypeDef **in)
{
    sipTypeDef *td, *nsx;
    sipEncodedClassDef *sup;

    /* The base type doesn't have any type information. */
    if ((td = wt->type) == NULL)
        return;

    /* Search the possible linked list of namespace extenders. */
    nsx = td;

    do
    {
        /* Try the methods. */
        if (nsx->td_nrmethods > 0 &&
            (*pmdp = (PyMethodDef *)bsearch(name, nsx->td_methods, nsx->td_nrmethods, sizeof (PyMethodDef), compareMethodName)) != NULL)
            return;

        /* Try the enum members. */
        if (nsx->td_nrenummembers > 0 &&
            (*enmp = (sipEnumMemberDef *)bsearch(name, nsx->td_enummembers, nsx->td_nrenummembers, sizeof (sipEnumMemberDef), compareEnumMemberName)) != NULL)
        {
            if (in != NULL)
                *in = nsx;

            return;
        }

        /* Try the variables.  Note, these aren't sorted. */
        if (nsx->td_variables != NULL)
        {
            PyMethodDef *md;

            for (md = nsx->td_variables; md->ml_name != NULL; ++md)
                if (strcmp(name, md->ml_name) == 0)
                {
                    *vmdp = md;
                    return;
                }
        }

        nsx = nsx->td_nsextender;
    }
    while (nsx != NULL);

    /* Check the base classes. */
    if ((sup = td->td_supers) != NULL)
        do
        {
            findLazyAttr(getClassType(sup, td->td_module), name, pmdp, enmp, vmdp, in);

            if (*pmdp != NULL || *enmp != NULL || *vmdp != NULL)
                break;
        }
        while (!sup++->sc_flag);
}


/*
 * The bsearch() helper function for searching a sorted method table.
 */
static int compareMethodName(const void *key,const void *el)
{
    return strcmp((const char *)key,((const PyMethodDef *)el)->ml_name);
}


/*
 * The bsearch() helper function for searching a sorted enum member table.
 */
static int compareEnumMemberName(const void *key,const void *el)
{
    return strcmp((const char *)key,((const sipEnumMemberDef *)el)->em_name);
}


/*
 * Report a function with invalid argument types.
 */
static void sip_api_no_function(int argsParsed, const char *func)
{
    badArgs(argsParsed,NULL,func);
}


/*
 * Report a method/function/signal with invalid argument types.
 */
static void sip_api_no_method(int argsParsed, const char *classname, const char *method)
{
    badArgs(argsParsed,classname,method);
}


/*
 * Report an abstract method called with an unbound self.
 */
static void sip_api_abstract_method(const char *classname, const char *method)
{
    PyErr_Format(PyExc_TypeError,"%s.%s() is abstract and cannot be called as an unbound method", classname, method);
}


/*
 * Handle error reporting for bad arguments to various things.
 */
static void badArgs(int argsParsed, const char *classname, const char *method)
{
    char *sep;
    int nrparsed = argsParsed & ~PARSE_MASK;

    if (classname != NULL)
        sep = ".";
    else
    {
        classname = "";
        sep = "";
    }

    switch (argsParsed & PARSE_MASK)
    {
    case PARSE_FEW:
        PyErr_Format(PyExc_TypeError,"insufficient number of arguments to %s%s%s()",classname,sep,method);
        break;

    case PARSE_MANY:
        PyErr_Format(PyExc_TypeError,"too many arguments to %s%s%s(), %d at most expected",classname,sep,method,nrparsed);
        break;

    case PARSE_TYPE:
        PyErr_Format(PyExc_TypeError,"argument %d of %s%s%s() has an invalid type",nrparsed + 1,classname,sep,method);
        break;

    case PARSE_FORMAT:
        PyErr_Format(PyExc_TypeError,"invalid format to sipParseArgs() from %s%s%s()",classname,sep,method);
        break;

    case PARSE_UNBOUND:
        PyErr_Format(PyExc_TypeError,"first argument of unbound method %s%s%s() must be a %s instance",classname,sep,method,classname);
        break;

    case PARSE_RAISED:
        /* It has already been taken care of. */

        break;

    case PARSE_OK:
        /* This is raised by a private re-implementation. */
        PyErr_Format(PyExc_AttributeError,"%s%s%s is a private method",classname,sep,method);
        break;
    }
}


/*
 * Report a bad operator argument.  Only a small subset of operators need to
 * be handled (those that don't return Py_NotImplemented).
 */
static void sip_api_bad_operator_arg(PyObject *self, PyObject *arg,
                     sipPySlotType st)
{
    const char *sn = NULL;

    /* Try and get the text to match a Python exception. */

    switch (st)
    {
    case concat_slot:
    case iconcat_slot:
        PyErr_Format(PyExc_TypeError, "cannot concatenate '%s' and '%s' objects", self->ob_type->tp_name, arg->ob_type->tp_name);
        break;

    case repeat_slot:
        sn = "*";
        break;

    case irepeat_slot:
        sn = "*=";
        break;

    default:
        sn = "unknown";
    }

    if (sn != NULL)
        PyErr_Format(PyExc_TypeError, "unsupported operand type(s) for %s: '%s' and '%s'", sn, self->ob_type->tp_name, arg->ob_type->tp_name);
}


/*
 * Report a sequence length that does not match the length of a slice.
 */
static void sip_api_bad_length_for_slice(SIP_SSIZE_T seqlen,
        SIP_SSIZE_T slicelen)
{
    PyErr_Format(PyExc_ValueError,
#if PY_VERSION_HEX >= 0x02050000
            "attempt to assign sequence of size %zd to slice of size %zd",
#else
            "attempt to assign sequence of size %d to slice of size %d",
#endif
            seqlen, slicelen);
}


/*
 * Report a Python object that cannot be converted to a particular class.
 */
static void sip_api_bad_class(const char *classname)
{
    PyErr_Format(PyExc_TypeError,"cannot convert Python object to an instance of %s",classname);
}


/*
 * Report a Python class variable with an unexpected type.
 */
static void sip_api_bad_set_type(const char *classname,const char *var)
{
    PyErr_Format(PyExc_TypeError,"invalid type for variable %s.%s",classname,var);
}


/*
 * Report a Python member function with an unexpected return type.
 */
static void sip_api_bad_catcher_result(PyObject *method)
{
    const char *cname;
    char *mname;

    /*
     * This is part of the public API so we make no assumptions about the
     * method object.
     */
    if (!PyMethod_Check(method) ||
        PyMethod_GET_FUNCTION(method) == NULL ||
        !PyFunction_Check(PyMethod_GET_FUNCTION(method)) ||
        PyMethod_GET_SELF(method) == NULL)
    {
        PyErr_Format(PyExc_TypeError,"invalid argument to sipBadCatcherResult()");
        return;
    }

    mname = PyString_AsString(((PyFunctionObject *)PyMethod_GET_FUNCTION(method))->func_name);

    if (mname == NULL)
        return;

    cname = PyMethod_GET_SELF(method)->ob_type->tp_name;

    PyErr_Format(PyExc_TypeError,"invalid result type from %s.%s()",cname,mname);
}


/*
 * Return the name of the class corresponding to a wrapper object.  This comes
 * with a reference.
 */
static PyObject *sip_api_class_name(PyObject *self)
{
    return PyString_FromString(self->ob_type->tp_name);
}


/*
 * Return non-zero if the object is a C++ instance wrapper.
 */
int sip_api_wrapper_check(PyObject *o)
{
    return PyObject_TypeCheck(o,(PyTypeObject *)&sipWrapper_Type);
}


/*
 * Return non-zero if the object is a C++ instance wrapper type.
 */
static int sipWrapperType_Check(PyObject *op)
{
    return PyObject_TypeCheck(op,(PyTypeObject *)&sipWrapperType_Type);
}


/*
 * Transfer ownership of a class instance to Python from C/C++.
 */
static void sip_api_transfer_back(PyObject *self)
{
    if (self != NULL && sip_api_wrapper_check(self))
    {
        sipWrapper *w = (sipWrapper *)self;

        if (sipCppHasRef(w))
        {
            sipResetCppHasRef(w);
            Py_DECREF(w);
        }
        else
            removeFromParent(w);

        sipSetPyOwned(w);
    }
}


/*
 * Transfer ownership of a class instance to C/C++ from Python.
 */
static void sip_api_transfer_to(PyObject *self, PyObject *owner)
{
    /*
     * There is a legitimate case where we try to transfer a PyObject that
     * may not be a SIP generated class.  The virtual handler code calls
     * this function to keep the C/C++ instance alive when it gets rid of
     * the Python object returned by the Python method.  A class may have
     * handwritten code that converts a regular Python type - so we can't
     * assume that we can simply cast to sipWrapper.
     */
    if (self != NULL && sip_api_wrapper_check(self) && (owner == NULL || sip_api_wrapper_check(owner)))
    {
        sipWrapper *w = (sipWrapper *)self;

        /* Keep the object alive while we do the transfer. */
        Py_INCREF(self);

        removeFromParent(w);
        addToParent(w, (sipWrapper *)owner);

        Py_DECREF(self);

        sipResetPyOwned(w);
    }
}


/*
 * Transfer ownership of a class instance from Python to C/C++, or vice versa.
 * This is deprecated.
 */
static void sip_api_transfer(PyObject *self, int toCpp)
{
    if (toCpp)
        sip_api_transfer_to(self, self);
    else
        sip_api_transfer_back(self);
}


/*
 * Add a license to a dictionary.
 */
static int addLicense(PyObject *dict,sipLicenseDef *lc)
{
    int rc;
    PyObject *ldict, *proxy, *o;

    /* Convert the strings we use to objects if not already done. */

    if (licenseName == NULL && (licenseName = PyString_FromString("__license__")) == NULL)
        return -1;

    if (licenseeName == NULL && (licenseeName = PyString_FromString("Licensee")) == NULL)
        return -1;

    if (typeName == NULL && (typeName = PyString_FromString("Type")) == NULL)
        return -1;

    if (timestampName == NULL && (timestampName = PyString_FromString("Timestamp")) == NULL)
        return -1;

    if (signatureName == NULL && (signatureName = PyString_FromString("Signature")) == NULL)
        return -1;

    /* We use a dictionary to hold the license information. */
    if ((ldict = PyDict_New()) == NULL)
        return -1;

    /* The license type is compulsory, the rest are optional. */
    if (lc->lc_type == NULL || (o = PyString_FromString(lc->lc_type)) == NULL)
        goto deldict;

    rc = PyDict_SetItem(ldict,typeName,o);
    Py_DECREF(o);

    if (rc < 0)
        goto deldict;

    if (lc->lc_licensee != NULL)
    {
        if ((o = PyString_FromString(lc->lc_licensee)) == NULL)
            goto deldict;

        rc = PyDict_SetItem(ldict,licenseeName,o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lc->lc_timestamp != NULL)
    {
        if ((o = PyString_FromString(lc->lc_timestamp)) == NULL)
            goto deldict;

        rc = PyDict_SetItem(ldict,timestampName,o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lc->lc_signature != NULL)
    {
        if ((o = PyString_FromString(lc->lc_signature)) == NULL)
            goto deldict;

        rc = PyDict_SetItem(ldict,signatureName,o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    /* Create a read-only proxy. */
    if ((proxy = PyDictProxy_New(ldict)) == NULL)
        goto deldict;

    Py_DECREF(ldict);

    rc = PyDict_SetItem(dict,licenseName,proxy);
    Py_DECREF(proxy);

    return rc;

deldict:
    Py_DECREF(ldict);

    return -1;
}


/*
 * Add the void pointer instances to a dictionary.
 */
static int addVoidPtrInstances(PyObject *dict,sipVoidPtrInstanceDef *vi)
{
    while (vi->vi_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = sip_api_convert_from_void_ptr(vi->vi_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict,vi->vi_name,w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++vi;
    }

    return 0;
}


/*
 * Add the char instances to a dictionary.
 */
static int addCharInstances(PyObject *dict,sipCharInstanceDef *ci)
{
    while (ci->ci_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyString_FromStringAndSize(&ci->ci_val,1)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict,ci->ci_name,w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++ci;
    }

    return 0;
}


/*
 * Add the string instances to a dictionary.
 */
static int addStringInstances(PyObject *dict,sipStringInstanceDef *si)
{
    while (si->si_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyString_FromString(si->si_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict,si->si_name,w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++si;
    }

    return 0;
}


/*
 * Add the int instances to a dictionary.
 */
static int addIntInstances(PyObject *dict, sipIntInstanceDef *ii)
{
    while (ii->ii_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyInt_FromLong(ii->ii_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, ii->ii_name, w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++ii;
    }

    return 0;
}


/*
 * Add the long instances to a dictionary.
 */
static int addLongInstances(PyObject *dict,sipLongInstanceDef *li)
{
    while (li->li_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyLong_FromLong(li->li_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict,li->li_name,w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++li;
    }

    return 0;
}


/*
 * Add the unsigned long instances to a dictionary.
 */
static int addUnsignedLongInstances(PyObject *dict, sipUnsignedLongInstanceDef *uli)
{
    while (uli->uli_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyLong_FromUnsignedLong(uli->uli_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, uli->uli_name, w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++uli;
    }

    return 0;
}


/*
 * Add the long long instances to a dictionary.
 */
static int addLongLongInstances(PyObject *dict, sipLongLongInstanceDef *lli)
{
    while (lli->lli_name != NULL)
    {
        int rc;
        PyObject *w;

#if defined(HAVE_LONG_LONG)
        if ((w = PyLong_FromLongLong(lli->lli_val)) == NULL)
#else
        if ((w = PyLong_FromLong(lli->lli_val)) == NULL)
#endif
            return -1;

        rc = PyDict_SetItemString(dict, lli->lli_name, w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++lli;
    }

    return 0;
}


/*
 * Add the unsigned long long instances to a dictionary.
 */
static int addUnsignedLongLongInstances(PyObject *dict, sipUnsignedLongLongInstanceDef *ulli)
{
    while (ulli->ulli_name != NULL)
    {
        int rc;
        PyObject *w;

#if defined(HAVE_LONG_LONG)
        if ((w = PyLong_FromUnsignedLongLong(ulli->ulli_val)) == NULL)
#else
        if ((w = PyLong_FromUnsignedLong(ulli->ulli_val)) == NULL)
#endif
            return -1;

        rc = PyDict_SetItemString(dict, ulli->ulli_name, w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++ulli;
    }

    return 0;
}


/*
 * Add the double instances to a dictionary.
 */
static int addDoubleInstances(PyObject *dict,sipDoubleInstanceDef *di)
{
    while (di->di_name != NULL)
    {
        int rc;
        PyObject *w;

        if ((w = PyFloat_FromDouble(di->di_val)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict,di->di_name,w);
        Py_DECREF(w);

        if (rc < 0)
            return -1;

        ++di;
    }

    return 0;
}


/*
 * Wrap a set of enum instances and add them to a dictionary.
 */
static int addEnumInstances(PyObject *dict, sipEnumInstanceDef *ei)
{
    while (ei->ei_name != NULL)
    {
        if (addSingleEnumInstance(dict, ei->ei_name, ei->ei_val, *ei->ei_type) < 0)
            return -1;

        ++ei;
    }

    return 0;
}


/*
 * Wrap a single enum instance and add it to a dictionary.
 */
static int addSingleEnumInstance(PyObject *dict, const char *name, int value,
        PyTypeObject *type)
{
    int rc;
    PyObject *w;

    if ((w = sip_api_convert_from_named_enum(value, type)) == NULL)
        return -1;

    rc = PyDict_SetItemString(dict, name, w);
    Py_DECREF(w);

    return rc;
}


/*
 * Wrap an enum instance and add it to a dictionary.
 */
static int sip_api_add_enum_instance(PyObject *dict, const char *name,
        int value, PyTypeObject *type)
{
    /* If this is a wrapped type then get the type dictionary. */
    if (sipWrapperType_Check(dict))
        dict = ((PyTypeObject *)dict)->tp_dict;

    return addSingleEnumInstance(dict, name, value, type);
}


/*
 * Wrap a set of class instances and add them to a dictionary.
 */
static int addClassInstances(PyObject *dict, sipClassInstanceDef *ci)
{
    while (ci->ci_name != NULL)
    {
        if (addSingleClassInstance(dict, ci->ci_name, ci->ci_ptr, *ci->ci_type, ci->ci_flags) < 0)
            return -1;

        ++ci;
    }

    return 0;
}


/*
 * Wrap a single class instance and add it to a dictionary.
 */
static int addSingleClassInstance(PyObject *dict, const char *name,
        void *cppPtr, sipWrapperType *wt, int initflags)
{
    int rc;
    PyObject *w;

    if ((w = sipWrapSimpleInstance(cppPtr,wt,NULL,initflags)) == NULL)
        return -1;

    rc = PyDict_SetItemString(dict,name,w);
    Py_DECREF(w);

    return rc;
}


/*
 * Wrap a class instance and add it to a dictionary.
 */
static int sip_api_add_class_instance(PyObject *dict, const char *name,
        void *cppPtr, sipWrapperType *wt)
{
    /* If this is a wrapped type then get the type dictionary. */
    if (sipWrapperType_Check(dict))
        dict = ((PyTypeObject *)dict)->tp_dict;

    return addSingleClassInstance(dict, name, cppPtr, wt, 0);
}


/*
 * Wrap a mapped type instance and add it to a dictionary.
 */
static int sip_api_add_mapped_type_instance(PyObject *dict, const char *name,
        void *cppPtr, const sipMappedType *mt)
{
    int rc;
    PyObject *w;

    /* If this is a wrapped type then get the type dictionary. */
    if (sipWrapperType_Check(dict))
        dict = ((PyTypeObject *)dict)->tp_dict;

    if ((w = mt->mt_cfrom(cppPtr, NULL)) == NULL)
        return -1;

    rc = PyDict_SetItemString(dict, name, w);
    Py_DECREF(w);

    return rc;
}


/*
 * Get the C/C++ pointer for a complex object.
 */
static void *sip_api_get_complex_cpp_ptr(sipWrapper *w)
{
    if (!sipIsDerived(w))
    {
        PyErr_SetString(PyExc_RuntimeError,"no access to protected functions or signals for objects not created from Python");

        return NULL;
    }

    if (checkPointer(w->u.cppPtr) < 0)
        return NULL;

    return w->u.cppPtr;
}


/*
 * Return the Python member function corresponding to a C/C++ virtual function,
 * if any.  If one was found then the Python lock is acquired.
 */
static PyObject *sip_api_is_py_method(sip_gilstate_t *gil,sipMethodCache *pymc,
                      sipWrapper *sipSelf,char *cname,
                      char *mname)
{
    /* We might still have C++ going after the interpreter has gone. */
    if (sipInterpreter == NULL)
        return NULL;

    /*
     * It's possible that the Python object has been deleted but the
     * underlying (complex) C/C++ instance is still working and trying to
     * handle virtual functions.  Or an instance has started handling
     * virtual functions before its ctor has returned.  In either case say
     * there is no Python method.
     */
    if (sipSelf == NULL)
        return NULL;

#ifdef WITH_THREAD
    *gil = PyGILState_Ensure();
#endif

    /* See if we have already looked for the Python method. */
    if (!sipFoundMethod(pymc))
    {
        PyObject *methobj;

        /*
         * Using PyMethod_Check() rather than PyCallable_Check() has
         * the added benefits of ensuring the (common) case of there
         * being no Python method is handled as a direct call to C/C++
         * (rather than converted to Python and then back to C/C++) and
         * makes sure that abstract virtuals are trapped.
         */
        if ((methobj = PyObject_GetAttrString((PyObject *)sipSelf,mname)) != NULL)
        {
            if (PyMethod_Check(methobj))
            {
                sipSetIsMethod(pymc);
                sipSaveMethod(&pymc->pyMethod,methobj);
            }

            Py_DECREF(methobj);
        }

        PyErr_Clear();

        sipSetFoundMethod(pymc);
    }
    else if (sipIsMethod(pymc))
        PyErr_Clear();

    if (sipIsMethod(pymc))
        return PyMethod_New(pymc->pyMethod.mfunc,pymc->pyMethod.mself,pymc->pyMethod.mclass);

    if (cname != NULL)
        PyErr_Format(PyExc_NotImplementedError,"%s.%s() is abstract and must be overridden",cname,mname);

#ifdef WITH_THREAD
    PyGILState_Release(*gil);
#endif

    return NULL;
}


/*
 * Convert a C/C++ pointer to the object that wraps it.
 */
static PyObject *sip_api_get_wrapper(void *cppPtr,sipWrapperType *type)
{
    return (PyObject *)sipOMFindObject(&cppPyMap,cppPtr,type);
}


/*
 * Return the C/C++ pointer from a wrapper without any checks.
 */
void *sipGetAddress(sipWrapper *w)
{
    if (sipIsAccessFunc(w))
        return (*w->u.afPtr)();

    if (sipIsIndirect(w))
        return *((void **)w->u.cppPtr);

    return w->u.cppPtr;
}


/*
 * Get the C/C++ pointer from a wrapper and optionally cast it to the required
 * type.
 */
void *sip_api_get_cpp_ptr(sipWrapper *w,sipWrapperType *type)
{
    void *ptr = sipGetAddress(w);

    if (checkPointer(ptr) < 0)
        return NULL;

    if (type != NULL)
    {
        sipCastFunc cast;

        cast = ((sipWrapperType *)w->ob_type)->type->td_cast;

        /* C structures don't have cast functions. */
        if (cast != NULL)
            ptr = (*cast)(ptr,type);
    }

    return ptr;
}


/*
 * Check that a pointer is non-NULL.
 */
static int checkPointer(void *ptr)
{
    if (ptr == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,"underlying C/C++ object has been deleted");
        return -1;
    }

    return 0;
}


/*
 * Check to see if a Python object can be converted to a wrapped type.
 */
static int sip_api_can_convert_to_instance(PyObject *pyObj,
        sipWrapperType *type, int flags)
{
    int ok;
    sipConvertToFunc cto = type->type->td_cto;

    /* None is handled outside the type checkers. */
    if (pyObj == Py_None)
        ok = ((flags & SIP_NOT_NONE) == 0);
    else if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
        ok = PyObject_TypeCheck(pyObj, (PyTypeObject *)type);
    else
        ok = cto(pyObj, NULL, NULL, NULL);

    return ok;
}


/*
 * Check to see if a Python object can be converted to a mapped type.
 */
static int sip_api_can_convert_to_mapped_type(PyObject *pyObj,
                          const sipMappedType *mt,
                          int flags)
{
    int ok;

    /* None is handled outside the type checkers. */
    if (pyObj == Py_None)
        ok = ((flags & SIP_NOT_NONE) == 0);
    else
        ok = mt->mt_cto(pyObj, NULL, NULL, NULL);

    return ok;
}


/*
 * Convert a Python object to a C/C++ pointer, assuming a previous call to
 * sip_api_can_convert_to_instance() has been successful.  Allow ownership to
 * be transferred and any type convertors to be disabled.
 */
static void *sip_api_convert_to_instance(PyObject *pyObj, sipWrapperType *type,
                     PyObject *transferObj, int flags,
                     int *statep, int *iserrp)
{
    void *cpp = NULL;
    int state = 0;
    sipConvertToFunc cto = type->type->td_cto;

    /* Don't convert if there has already been an error. */
    if (!*iserrp)
    {
        /* Do the conversion. */
        if (pyObj == Py_None)
            cpp = NULL;
        else if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
        {
            if ((cpp = sip_api_get_cpp_ptr((sipWrapper *)pyObj, type)) == NULL)
                *iserrp = TRUE;
            else if (transferObj != NULL)
                if (transferObj == Py_None)
                    sip_api_transfer_back(pyObj);
                else
                    sip_api_transfer_to(pyObj, transferObj);
        }
        else
            state = cto(pyObj, &cpp, iserrp, transferObj);
    }

    if (statep != NULL)
        *statep = state;

    return cpp;
}


/*
 * Convert a Python object to a C/C++ pointer, assuming a previous call to
 * sip_api_can_convert_to_mapped_type() has been successful.  Allow ownership
 * to be transferred.
 */
static void *sip_api_convert_to_mapped_type(PyObject *pyObj,
                        const sipMappedType *mt,
                        PyObject *transferObj, int flags,
                        int *statep, int *iserrp)
{
    void *cpp = NULL;
    int state = 0;

    /* Don't convert if there has already been an error. */
    if (!*iserrp)
        if (pyObj == Py_None)
            cpp = NULL;
        else
            state = mt->mt_cto(pyObj, &cpp, iserrp, transferObj);

    if (statep != NULL)
        *statep = state;

    return cpp;
}


/*
 * Convert a Python object to a C/C++ pointer and raise an exception if it
 * can't be done.
 */
static void *sip_api_force_convert_to_instance(PyObject *pyObj,
                           sipWrapperType *type,
                           PyObject *transferObj,
                           int flags, int *statep,
                           int *iserrp)
{
    /* Don't even try if there has already been an error. */
    if (*iserrp)
        return NULL;

    /* See if the object's type can be converted. */
    if (!sip_api_can_convert_to_instance(pyObj, type, flags))
    {
        PyErr_Format(PyExc_TypeError, "%s cannot be converted to %s in this context", pyObj->ob_type->tp_name, type->type->td_name);

        if (statep != NULL)
            *statep = 0;

        *iserrp = TRUE;
        return NULL;
    }

    /* Do the conversion. */
    return sip_api_convert_to_instance(pyObj, type, transferObj, flags, statep, iserrp);
}


/*
 * Convert a Python object to a C/C++ pointer and raise an exception if it
 * can't be done.
 */
static void *sip_api_force_convert_to_mapped_type(PyObject *pyObj,
                          const sipMappedType *mt,
                          PyObject *transferObj,
                          int flags, int *statep,
                          int *iserrp)
{
    /* Don't even try if there has already been an error. */
    if (*iserrp)
        return NULL;

    /* See if the object's type can be converted. */
    if (!sip_api_can_convert_to_mapped_type(pyObj, mt, flags))
    {
        PyErr_Format(PyExc_TypeError, "%s cannot be converted to %s in this context", pyObj->ob_type->tp_name, mt->mt_name);

        if (statep != NULL)
            *statep = 0;

        *iserrp = TRUE;
        return NULL;
    }

    /* Do the conversion. */
    return sip_api_convert_to_mapped_type(pyObj, mt, transferObj, flags, statep, iserrp);
}


/*
 * Release a possibly temporary instance created by a type convertor.
 */
static void sip_api_release_instance(void *cpp, sipWrapperType *type, int state)
{
    /* See if there is something to release. */
    if (state & SIP_TEMPORARY)
        release(cpp, type->type, state);
}


/*
 * Release an instance.
 */
static void release(void *addr, sipTypeDef *td, int state)
{
    sipReleaseFunc rel = td->td_release;

    /*
     * If there is no release function then it must be a C structure and we can
     * just free it.
     */
    if (rel == NULL)
        sip_api_free(addr);
    else
        rel(addr, state);
}


/*
 * Release a possibly temporary mapped type created by a type convertor.
 */
static void sip_api_release_mapped_type(void *cpp, const sipMappedType *mt,
                    int state)
{
    /* See if there is something to release. */
    if (state & SIP_TEMPORARY)
    {
        sipReleaseFunc rel = mt->mt_release;

        /*
         * If there is no release function then it must be a C
         * structure and we can just free it.
         */
        if (rel == NULL)
            sip_api_free(cpp);
        else
            rel(cpp, state);
    }
}


/*
 * Convert a C/C++ instance to a Python instance.
 */
PyObject *sip_api_convert_from_instance(void *cpp, sipWrapperType *type,
                    PyObject *transferObj)
{
    PyObject *py;

    /* Handle None. */
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    /* Apply any sub-class convertor. */
    if (sipTypeHasSCC(type))
        type = convertSubClass(type, &cpp);

    /* See if we have already wrapped it. */
    if ((py = sip_api_get_wrapper(cpp, type)) != NULL)
        Py_INCREF(py);
    else if ((py = sipWrapSimpleInstance(cpp, type, NULL, SIP_SHARE_MAP)) == NULL)
        return NULL;

    /* Handle any ownership transfer. */
    if (transferObj != NULL)
        if (transferObj == Py_None)
            sip_api_transfer_back(py);
        else
            sip_api_transfer_to(py, transferObj);

    return py;
}


/*
 * Convert a new C/C++ instance to a Python instance.
 */
static PyObject *sip_api_convert_from_new_instance(void *cpp,
                           sipWrapperType *type,
                           PyObject *transferObj)
{
    sipWrapper *owner;

    /* Handle None. */
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    /* Apply any sub-class convertor. */
    if (sipTypeHasSCC(type))
        type = convertSubClass(type, &cpp);

    /* Handle any ownership transfer. */
    if (transferObj != NULL && transferObj != Py_None)
        owner = (sipWrapper *)transferObj;
    else
        owner = NULL;

    return sipWrapSimpleInstance(cpp, type, owner, (owner == NULL ? SIP_PY_OWNED : 0));
}


/*
 * Convert a C/C++ instance implemented as a mapped type to a Python object.
 */
static PyObject *sip_api_convert_from_mapped_type(void *cpp,
                          const sipMappedType *mt,
                          PyObject *transferObj)
{
    /* Handle None. */
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return mt->mt_cfrom(cpp, transferObj);
}


/*
 * Convert a Python instance of a class to a C/C++ object pointer, checking
 * that the instance's class is derived from a given base type.
 */
static void *sip_api_convert_to_cpp(PyObject *sipSelf,sipWrapperType *type,
                    int *iserrp)
{
    return sip_api_convert_to_instance(sipSelf, type, NULL, SIP_NO_CONVERTORS, NULL, iserrp);
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
 * Return the mapped type structure for a particular mapped type.
 */
static const sipMappedType *sip_api_find_mapped_type(const char *type)
{
    sipExportedModuleDef *em;

    for (em = clientList; em != NULL; em = em->em_next)
    {
        sipMappedType **mtypes, *mt;

        if ((mtypes = em->em_mappedtypes) == NULL)
            continue;

        while ((mt = *mtypes++) != NULL)
        {
            const char *s1 = mt->mt_name, *s2 = type;

            /*
             * Compare while ignoring spaces so that we don't
             * impose a rigorous naming standard.
             */
            do
            {
                while (*s1 == ' ')
                    ++s1;

                while (*s2 == ' ')
                    ++s2;

                if (*s1 == '\0' && *s2 == '\0')
                    return mt;
            }
            while (*s1++ == *s2++);
        }
    }

    return NULL;
}


/*
 * Save the components of a Python method.
 */
void sipSaveMethod(sipPyMethod *pm,PyObject *meth)
{
    pm->mfunc = PyMethod_GET_FUNCTION(meth);
    pm->mself = PyMethod_GET_SELF(meth);
    pm->mclass = PyMethod_GET_CLASS(meth);
}


/*
 * Call a hook.
 */
static void sip_api_call_hook(const char *hookname)
{
    PyObject *dictofmods, *mod, *dict, *hook, *res;
 
    /* Get the dictionary of modules. */
    if ((dictofmods = PyImport_GetModuleDict()) == NULL)
        return;
 
    /* Get the __builtin__ module. */
    if ((mod = PyDict_GetItemString(dictofmods,"__builtin__")) == NULL)
        return;
 
    /* Get it's dictionary. */
    if ((dict = PyModule_GetDict(mod)) == NULL)
        return;
 
    /* Get the function hook. */
    if ((hook = PyDict_GetItemString(dict,hookname)) == NULL)
        return;
 
    /* Call the hook and discard any result. */
    res = PyObject_CallObject(hook,NULL);
 
    Py_XDECREF(res);
}


/*
 * Call any sub-class convertors for a given type returning a pointer to the
 * sub-type object, and possibly modifying the C++ address (in the case of
 * multiple inheritence).
 */
static sipWrapperType *convertSubClass(sipWrapperType *type, void **cppPtr)
{
    sipExportedModuleDef *em;

    if (*cppPtr == NULL)
        return NULL;

    for (em = clientList; em != NULL; em = em->em_next)
    {
        sipSubClassConvertorDef *scc;

        if ((scc = em->em_convertors) == NULL)
            continue;

        while (scc->scc_convertor != NULL)
        {
            /*
             * The base type is the "root" class that may have a number of
             * convertors each handling a "branch" of the derived tree of
             * classes.  The "root" normally implements the base function that
             * provides the RTTI used by the convertors and is re-implemented
             * by derived classes.  We therefore see if the target type is a
             * sub-class of the root, ie. see if the convertor might be able to
             * convert the target type to something more specific.
             */
            if (PyType_IsSubtype((PyTypeObject *)type, (PyTypeObject *)scc->scc_basetype))
            {
                sipWrapperType *subtype;

                if ((subtype = (*scc->scc_convertor)(cppPtr)) != NULL)
                    return subtype;
            }

            ++scc;
        }
    }

    /*
     * We haven't found the exact type, so return the most specific type that
     * it must be.  This can happen legitimately if the wrapped library is
     * returning an internal class that is down-cast to a more generic class.
     * Also we want this function to be safe when a class doesn't have any
     * convertors.
     */
    return type;
}


/*
 * The bsearch() helper function for searching a sorted string map table.
 */
static int compareStringMapEntry(const void *key,const void *el)
{
    return strcmp((const char *)key,((const sipStringTypeClassMap *)el)->typeString);
}


/*
 * A convenience function for %ConvertToSubClassCode for types represented as a
 * string.  Returns the Python class object or NULL if the type wasn't
 * recognised.
 */
static sipWrapperType *sip_api_map_string_to_class(const char *typeString,
                         const sipStringTypeClassMap *map,
                         int maplen)
{
    sipStringTypeClassMap *me;

    me = (sipStringTypeClassMap *)bsearch((const void *)typeString,
                          (const void *)map,maplen,
                          sizeof (sipStringTypeClassMap),
                          compareStringMapEntry);

        return ((me != NULL) ? *me->pyType : NULL);
}


/*
 * The bsearch() helper function for searching a sorted integer map table.
 */
static int compareIntMapEntry(const void *keyp,const void *el)
{
    int key = *(int *)keyp;

    if (key > ((const sipIntTypeClassMap *)el)->typeInt)
        return 1;

    if (key < ((const sipIntTypeClassMap *)el)->typeInt)
        return -1;

    return 0;
}


/*
 * A convenience function for %ConvertToSubClassCode for types represented as
 * an integer.  Returns the Python class object or NULL if the type wasn't
 * recognised.
 */
static sipWrapperType *sip_api_map_int_to_class(int typeInt,
                          const sipIntTypeClassMap *map,
                          int maplen)
{
    sipIntTypeClassMap *me;

    me = (sipIntTypeClassMap *)bsearch((const void *)&typeInt,
                       (const void *)map,maplen,
                       sizeof (sipIntTypeClassMap),
                       compareIntMapEntry);

        return ((me != NULL) ? *me->pyType : NULL);
}


/*
 * Raise an unknown exception.  Make no assumptions about the GIL.
 */
static void sip_api_raise_unknown_exception(void)
{
    static PyObject *mobj = NULL;

    SIP_BLOCK_THREADS

    if (mobj == NULL)
        mobj = PyString_FromString("unknown");

    PyErr_SetObject(PyExc_Exception, mobj);

    SIP_UNBLOCK_THREADS
}


/*
 * Raise an exception implemented as a class.  Make no assumptions about the
 * GIL.
 */
static void sip_api_raise_class_exception(sipWrapperType *type,void *ptr)
{
        PyObject *self;

    SIP_BLOCK_THREADS

        self = sipWrapSimpleInstance(ptr,type,NULL,SIP_PY_OWNED);

        PyErr_SetObject((PyObject *)type,self);

        Py_XDECREF(self);

    SIP_UNBLOCK_THREADS
}


/*
 * Raise an exception implemented as a class or sub-class.  Make no assumptions
 * about the GIL.
 */
static void sip_api_raise_sub_class_exception(sipWrapperType *type,void *ptr)
{
        PyObject *self;

    SIP_BLOCK_THREADS

        self = sipWrapSimpleInstance(ptr,type,NULL,SIP_PY_OWNED);

        PyErr_SetObject((PyObject *)type,self);

        Py_XDECREF(self);

    SIP_UNBLOCK_THREADS
}


/*
 * Return the module of an encoded class.
 */
static sipExportedModuleDef *getClassModule(sipEncodedClassDef *enc,
                        sipExportedModuleDef *em)
{
    if (enc->sc_module != 255)
        em = em->em_imports[enc->sc_module].im_module;

    return em;
}


/*
 * Return the type of an encoded class.
 */
static sipWrapperType *getClassType(sipEncodedClassDef *enc,
                    sipExportedModuleDef *em)
{
    return getClassModule(enc, em)->em_types[enc->sc_class];
}


/*
 * Find a particular slot function for a wrapper.
 */
static void *findSlot(PyObject *self,sipPySlotType st)
{
    sipTypeDef *td = ((sipWrapperType *)(self->ob_type))->type;
    sipEncodedClassDef *sup;
    void *slot;

    /* Check the immediate type. */
    if ((slot = findSlotInType(td, st)) != NULL)
        return slot;

    /* Check the super-types, if there are any. */
    if ((sup = td->td_supers) != NULL)
        do
            if ((slot = findSlotInType(getClassType(sup, td->td_module)->type, st)) != NULL)
                return slot;
        while (!sup++->sc_flag);

    /* This should never happen. */
    return NULL;
}


/*
 * Find a particular slot function in a type.
 */
static void *findSlotInType(sipTypeDef *td, sipPySlotType st)
{
    sipPySlotDef *psd;

    if ((psd = td->td_pyslots) != NULL)
        while (psd->psd_func != NULL)
        {
            if (psd->psd_type == st)
                return psd->psd_func;

            ++psd;
        }

    return NULL;
}


/*
 * Return the C/C++ address and the basic type information for a wrapper.
 */
static void *getPtrTypeDef(sipWrapper *self, sipTypeDef **td)
{
    *td = ((sipWrapperType *)self->ob_type)->type;

    return (sipNotInMap(self) ? NULL : self->u.cppPtr);
}


/*
 * Handle an objobjargproc slot.
 */
static int objobjargprocSlot(PyObject *self,PyObject *arg1,PyObject *arg2,
                 sipPySlotType st)
{
    int (*f)(PyObject *,PyObject *);
    PyObject *args;
    int res;

    /*
     * Slot handlers require a single PyObject *.  The second argument is
     * optional.
     */
    if (arg2 == NULL)
        args = arg1;
    else if (PyTuple_Check(arg1))
    {
        int i;

        /*
         * It's already a tuple so we need to copy it and append the
         * value.
         */
        if ((args = PyTuple_New(PyTuple_GET_SIZE(arg1) + 1)) == NULL)
            return -1;

        for (i = 0; i < PyTuple_GET_SIZE(arg1); ++i)
        {
            PyObject *o = PyTuple_GET_ITEM(arg1,i);

            PyTuple_SET_ITEM(args,i,o);
            Py_INCREF(o);
        }

        PyTuple_SET_ITEM(args,i,arg2);
        Py_INCREF(arg2);
    }
    else if ((args = Py_BuildValue("(OO)",arg1,arg2)) == NULL)
        return -1;

    f = (int (*)(PyObject *,PyObject *))findSlot(self,st);

    res = f(self,args);

    if (arg2 != NULL)
    {
        Py_DECREF(args);
    }

    return res;
}


/*
 * Handle an ssizeobjargproc slot.
 */
static int ssizeobjargprocSlot(PyObject *self, SIP_SSIZE_T arg1,
        PyObject *arg2, sipPySlotType st)
{
    int (*f)(PyObject *,PyObject *);
    PyObject *args;
    int res;

    /*
     * Slot handlers require a single PyObject *.  The second argument is
     * optional.
     */
    if (arg2 == NULL)
#if PY_VERSION_HEX >= 0x02050000
        args = PyInt_FromSsize_t(arg1);
#else
        args = PyInt_FromLong(arg1);
#endif
    else
#if PY_VERSION_HEX >= 0x02050000
        args = Py_BuildValue("(nO)", arg1, arg2);
#else
        args = Py_BuildValue("(iO)", arg1, arg2);
#endif

    if (args == NULL)
        return -1;

    f = (int (*)(PyObject *,PyObject *))findSlot(self,st);

    res = f(self,args);

    Py_DECREF(args);

    return res;
}


/*****************************************************************************
 * The functions, data types and structures to support a Python type to hold a
 * void * that can be converted to an integer.
 *****************************************************************************/

/* The object data structure. */
typedef struct {
    PyObject_HEAD
    void *voidptr;
} sipVoidPtr;


/*
 * Implement __new__ for the type.
 */
static PyObject *sipVoidPtr_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    void *ptr;
    int nargs, bad;

    /* We don't support keyword arguments. */
    if (kwds != NULL)
    {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return NULL;
    }

    /* Get the single argument. */
    if ((nargs = PyTuple_Size(args)) < 0)
        return NULL;

    bad = FALSE;

    if (nargs == 1)
    {
        PyObject *arg = PyTuple_GET_ITEM(args, 0);

        if (arg == Py_None)
            ptr = NULL;
        else if (PyCObject_Check(arg))
            ptr = PyCObject_AsVoidPtr(arg);
        else if (arg->ob_type == &sipVoidPtr_Type)
        {
            /*
             * The type is immutable so just return the argument.
             */
            Py_INCREF(arg);
            return arg;
        }
        else
        {
            ptr = (void *)PyInt_AsLong(arg);

            if (PyErr_Occurred())
                bad = TRUE;
        }
    }
    else
        bad = TRUE;

    if (bad)
    {
        PyErr_SetString(PyExc_TypeError, "a single integer, CObject, None or another voidptr is required");
        return NULL;
    }

    /* Create the instance. */
    if ((obj = subtype->tp_alloc(subtype, 0)) == NULL)
        return NULL;

    /* Save the pointer. */
    ((sipVoidPtr *)obj)->voidptr = ptr;

    return obj;
}


/*
 * Implement int() for the type.
 */
static PyObject *sipVoidPtr_int(sipVoidPtr *v)
{
    return PyInt_FromLong((long)v->voidptr);
}


/*
 * Implement hex() for the type.
 */
static PyObject *sipVoidPtr_hex(sipVoidPtr *v)
{
    char buf[2 + 16 + 1];

    PyOS_snprintf(buf, sizeof (buf), "0x%.*lx", (int)(sizeof (void *) * 2), (unsigned long)v->voidptr);

    return PyString_FromString(buf);
}


/*
 * Implement ascobject() for the type.
 */
static PyObject *sipVoidPtr_ascobject(sipVoidPtr *v, PyObject *arg)
{
    return PyCObject_FromVoidPtr(v->voidptr, NULL);
}


/*
 * Implement asstring() for the type.
 */
static PyObject *sipVoidPtr_asstring(sipVoidPtr *v,PyObject *arg)
{
    long nbytes = PyInt_AsLong(arg);

    if (PyErr_Occurred())
        return NULL;

    return PyString_FromStringAndSize(v->voidptr,nbytes);
}


/* The methods data structure. */
static PyMethodDef sipVoidPtr_Methods[] = {
    {"ascobject", (PyCFunction)sipVoidPtr_ascobject, METH_NOARGS, NULL},
    {"asstring", (PyCFunction)sipVoidPtr_asstring, METH_O, NULL},
    {NULL}
};


/* The number methods data structure. */
PyNumberMethods sipVoidPtr_NumberMethods = {
    0,                      /* nb_add */
    0,                      /* nb_subtract */
    0,                      /* nb_multiply */
    0,                      /* nb_divide */
    0,                      /* nb_remainder */
    0,                      /* nb_divmod */
    0,                      /* nb_power */
    0,                      /* nb_negative */
    0,                      /* nb_positive */
    0,                      /* nb_absolute */
    0,                      /* nb_nonzero */
    0,                      /* nb_invert */
    0,                      /* nb_lshift */
    0,                      /* nb_rshift */
    0,                      /* nb_and */
    0,                      /* nb_xor */
    0,                      /* nb_or */
    0,                      /* nb_coerce */
    (unaryfunc)sipVoidPtr_int,          /* nb_int */
    0,                      /* nb_long */
    0,                      /* nb_float */
    0,                      /* nb_oct */
    (unaryfunc)sipVoidPtr_hex,          /* nb_hex */
};


/* The type data structure. */
static PyTypeObject sipVoidPtr_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                      /* ob_size */
    "sip.voidptr",                  /* tp_name */
    sizeof (sipVoidPtr),                /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_compare */
    0,                      /* tp_repr */
    &sipVoidPtr_NumberMethods,          /* tp_as_number */
    0,                      /* tp_as_sequence */
    0,                      /* tp_as_mapping */
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    0,                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    0,                      /* tp_doc */
    0,                      /* tp_traverse */
    0,                      /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    sipVoidPtr_Methods,             /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    0,                      /* tp_init */
    0,                      /* tp_alloc */
    sipVoidPtr_new,                 /* tp_new */
};


/*
 * A convenience function to convert a C/C++ void pointer from a Python object.
 */
static void *sip_api_convert_to_void_ptr(PyObject *obj)
{
    if (obj == NULL)
    {
        PyErr_SetString(PyExc_TypeError,"sip.voidptr is NULL");
        return NULL;
    }

    if (obj == Py_None)
        return NULL;

    /* Save a conversion if it's not a sub-type. */
    if (obj->ob_type == &sipVoidPtr_Type)
        return ((sipVoidPtr *)obj)->voidptr;

    return (void *)PyInt_AsLong(obj);
}


/*
 * A convenience function to convert a C/C++ void pointer to a Python object.
 */
PyObject *sip_api_convert_from_void_ptr(void *val)
{
    sipVoidPtr *self;

    if (val == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if ((self = PyObject_NEW(sipVoidPtr,&sipVoidPtr_Type)) == NULL)
        return NULL;

    self->voidptr = val;

    return (PyObject *)self;
}


/*
 * Return TRUE if a type is a wrapped type, rather than a sub-type implemented
 * in Python or the super-type.
 */
static int isExactWrappedType(sipWrapperType *wt)
{
    char *name;

    /*
     * We check by comparing the actual type name with the name used to create
     * the original wrapped type.
     */
#if PY_VERSION_HEX >= 0x02050000
    if ((name = PyString_AsString(wt->super.ht_name)) == NULL)
#else
    if ((name = PyString_AsString(wt->super.name)) == NULL)
#endif
        return FALSE;

    return (strcmp(name, getBaseName(wt->type->td_name)) == 0);
}


/*****************************************************************************
 * The Python metatype for a C++ wrapper type.
 *****************************************************************************/

/*
 * The type alloc slot.
 */
static PyObject *sipWrapperType_alloc(PyTypeObject *self, SIP_SSIZE_T nitems)
{
    PyObject *o;

    /* Call the standard super-metatype alloc. */
    if ((o = PyType_Type.tp_alloc(self, nitems)) == NULL)
        return NULL;

    /*
     * Consume any extra type specific information and use it to initialise
     * the slots.  This only happens for directly wrapped classes (and not
     * programmer written sub-classes).  This must be done in the alloc
     * function because it is the only place we can break out of the
     * default new() function before PyType_Ready() is called.
     */
    if (currentType != NULL)
    {
        ((sipWrapperType *)o)->type = currentType;
        addSlots((sipWrapperType *)o, currentType);
        currentType = NULL;
    }

    return o;
}


/*
 * The type init slot.
 */
static int sipWrapperType_init(sipWrapperType *self, PyObject *args,
        PyObject *kwds)
{
    /* Call the standard super-metatype init. */
    if (PyType_Type.tp_init((PyObject *)self, args, kwds) < 0)
        return -1;

    /*
     * If we don't yet have any extra type specific information (because we are
     * a programmer defined sub-class) then get it from the (first) super-type.
     */
    if (self->type == NULL)
    {
        PyTypeObject *sc = ((PyTypeObject *)self)->tp_base;

        /*
         * Make sure that the type is derived from sip.wrapper.  It
         * might not if the type specifies sip.wrappertype as the
         * __metaclass__.
         */
        if (sc == NULL || !sipWrapperType_Check((PyObject *)sc))
        {
            PyErr_Format(PyExc_TypeError,
                    "type %s must be derived from sip.wrapper",
                    ((PyTypeObject *)self)->tp_name);

            return -1;
        }

        self->type = ((sipWrapperType *)sc)->type;
    }

    return 0;
}


/*
 * The type getattro slot.
 */
static PyObject *sipWrapperType_getattro(PyObject *obj,PyObject *name)
{
    char *nm;
    PyObject *attr;
    sipWrapperType *wt = (sipWrapperType *)obj;

    /*
     * If we are getting the type dictionary for a base wrapped type then we
     * don't want the super-metatype to handle it.
     */
    if ((nm = PyString_AsString(name)) == NULL)
        return NULL;

    if (strcmp(nm, "__dict__") == 0)
    {
        int i;
        sipTypeDef *td;
        sipEnumMemberDef *enm;
        PyObject *dict;
        PyMethodDef *pmd;

        dict = ((PyTypeObject *)wt)->tp_dict;

        /* The base type doesn't have any type information. */
        if ((td = wt->type) == NULL || !isExactWrappedType(wt))
        {
            Py_INCREF(dict);
            return dict;
        }

        /*
         * We can't cache the methods or variables so we need to make a
         * temporary copy of the type dictionary and return that (so
         * that it will get garbage collected immediately afterwards).
         */
        if ((dict = PyDict_Copy(dict)) == NULL)
            return NULL;

        /* Search the possible linked list of namespace extenders. */
        do
        {
            /*
             * Add the type's lazy enums.  It doesn't matter if
             * they are already there.
             */
            enm = td->td_enummembers;

            for (i = 0; i < td->td_nrenummembers; ++i)
            {
                int rc;
                PyObject *val;

                if ((val = createEnumMember(td, enm)) == NULL)
                    return NULL;

                rc = PyDict_SetItemString(dict, enm->em_name, val);

                Py_DECREF(val);

                if (rc < 0)
                    return NULL;

                ++enm;
            }

            /* Do the methods. */
            pmd = td->td_methods;

            for (i = 0; i < td->td_nrmethods; ++i)
            {
                int rc;
                PyObject *meth;

                if ((meth = PyCFunction_New(pmd, NULL)) == NULL)
                {
                    Py_DECREF(dict);
                    return NULL;
                }

                rc = PyDict_SetItemString(dict, pmd->ml_name, meth);

                Py_DECREF(meth);

                if (rc < 0)
                {
                    Py_DECREF(dict);
                    return NULL;
                }

                ++pmd;
            }

            /* Do the static variables. */
            if ((pmd = td->td_variables) != NULL)
                while (pmd->ml_name != NULL)
                {
                    if ((pmd->ml_flags & METH_STATIC) != 0)
                    {
                        int rc;
                        PyObject *val;

                        if ((val = (*pmd->ml_meth)(NULL, NULL)) == NULL)
                        {
                            Py_DECREF(dict);
                            return NULL;
                        }

                        rc = PyDict_SetItemString(dict, pmd->ml_name, val);

                        Py_DECREF(val);

                        if (rc < 0)
                        {
                            Py_DECREF(dict);
                            return NULL;
                        }
                    }

                    ++pmd;
                }

            td = td->td_nsextender;
        }
        while (td != NULL);

        return dict;
    }

    /* Now try the super-metatype's method. */
    if ((attr = PyType_Type.tp_getattro(obj,name)) != NULL)
        return attr;

    return handleGetLazyAttr(name,wt,NULL);
}


/*
 * The type setattro slot.
 */
static int sipWrapperType_setattro(PyObject *obj,PyObject *name,PyObject *value)
{
    int rc;

    rc = handleSetLazyAttr(name,value,(sipWrapperType *)obj,NULL);

    if (rc <= 0)
        return rc;

    /* Try the super-type's method last. */
    return PyType_Type.tp_setattro(obj,name,value);
}


/*
 * The type data structure.  We inherit everything from the standard Python
 * metatype except the init and getattro methods and the size of the type
 * object created is increased to accomodate the extra information we associate
 * with a wrapped type.
 */
static PyTypeObject sipWrapperType_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                      /* ob_size */
    "sip.wrappertype",              /* tp_name */
    sizeof (sipWrapperType),            /* tp_basicsize */
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
    sipWrapperType_getattro,            /* tp_getattro */
    sipWrapperType_setattro,            /* tp_setattro */
    0,                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
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
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)sipWrapperType_init,          /* tp_init */
    sipWrapperType_alloc,               /* tp_alloc */
    0,                      /* tp_new */
    0,                      /* tp_free */
};


/*****************************************************************************
 * The Python type that is the super-type for all C++ wrapper types.
 *****************************************************************************/

/*
 * The instance new slot.
 */
static PyObject *sipWrapper_new(sipWrapperType *wt,PyObject *args,PyObject *kwds)
{
    /* See if it is a namespace. */
    if (wt->type->td_fcto == NULL)
    {
        PyErr_Format(PyExc_TypeError, "%s represents a C++ namespace that cannot be instantiated", wt->type->td_name);

        return NULL;
    }

    /*
     * See if the object is being created explicitly rather than being
     * wrapped.
     */
    if (sipGetPending(NULL, NULL) == NULL)
    {
        /*
         * See if it cannot be instantiated or sub-classed from Python,
         * eg. it's an opaque class.  Some restrictions might be
         * overcome with better SIP support.
         */
        if (wt->type->td_init == NULL)
        {
            PyErr_Format(PyExc_TypeError,"%s represents a C++ class that cannot be instantiated or sub-classed", wt->type->td_name);

            return NULL;
        }

        /* See if it is an abstract type. */
        if (sipTypeIsAbstract(wt) && strcmp(strchr(wt->type->td_name, '.') + 1, ((PyTypeObject *)wt)->tp_name) == 0)
        {
            PyErr_Format(PyExc_TypeError, "%s represents a C++ abstract class and cannot be instantiated", wt->type->td_name);

            return NULL;
        }
    }

    /* Call the standard super-type new. */
    return PyBaseObject_Type.tp_new((PyTypeObject *)wt, args, kwds);
}


/*
 * The instance init slot.
 */
static int sipWrapper_init(sipWrapper *self,PyObject *args,PyObject *kwds)
{
    void *sipNew;
    int sipFlags;
    sipWrapper *owner;

    if (self->ob_type == (PyTypeObject *)&sipWrapper_Type)
    {
        PyErr_SetString(PyExc_TypeError,"the sip.wrapper type cannot be instantiated");
        return -1;
    }

    if (kwds != NULL)
    {
        PyErr_SetString(PyExc_TypeError,"keyword arguments are not supported");
        return -1;
    }

    /*
     * Create the instance dictionary if it hasn't already been done.  I think
     * that it will have been created if an __init__ method of a sub-class
     * creates instance attributes before calling it's super-class __init__
     * method (ie. this one).
     */
    if (self->dict == NULL && (self->dict = PyDict_New()) == NULL)
        return -1;

    /* Check there is no existing C++ instance waiting to be wrapped. */
    if ((sipNew = sipGetPending(&owner, &sipFlags)) == NULL)
    {
        int argsparsed = 0;
        sipWrapperType *wt = (sipWrapperType *)self->ob_type;

        /* Call the C++ ctor. */
        owner = NULL;

        if ((sipNew = wt->type->td_init(self, args, &owner, &argsparsed)) != NULL)
            sipFlags = SIP_DERIVED_CLASS;
        else
        {
            int pstate = argsparsed & PARSE_MASK;
            sipInitExtenderDef *ie = wt->iextend;

            /*
             * While we just have signature errors, try any initialiser
             * extenders.
             */
            while (ie != NULL && (pstate == PARSE_MANY || pstate == PARSE_FEW || pstate == PARSE_TYPE))
            {
                argsparsed = 0;

                if ((sipNew = ie->ie_extender(self, args, &owner, &argsparsed)) != NULL)
                    break;

                pstate = argsparsed & PARSE_MASK;
                ie = ie->ie_next;
            }

            if (sipNew == NULL)
            {
            	/*
            	 * If the arguments were parsed without error then assume an
            	 * exception has already been raised for why the instance
            	 * wasn't created.
            	 */
                if (pstate == PARSE_OK)
                    argsparsed = PARSE_RAISED;

                badArgs(argsparsed, NULL, getBaseName(wt->type->td_name));
                return -1;
            }

            sipFlags = 0;
        }

        if (owner == NULL)
            sipFlags |= SIP_PY_OWNED;
    }

    addToParent(self, owner);

    self->u.cppPtr = sipNew;
    self->flags = sipFlags;

    if (!sipNotInMap(self))
        sipOMAddObject(&cppPyMap,self);

    return 0;
}


/*
 * The instance traverse slot.
 */
static int sipWrapper_traverse(sipWrapper *self, visitproc visit, void *arg)
{
    int vret;
    void *ptr;
    sipTypeDef *td;
    sipWrapper *w;

    /* Call the nearest handwritten traverse code in the class hierachy. */
    if ((ptr = getPtrTypeDef(self, &td)) != NULL)
    {
        sipTypeDef *ctd = td;

        if (td->td_traverse == NULL)
        {
            sipEncodedClassDef *sup;

            if ((sup = td->td_supers) != NULL)
                do
                    ctd = getClassType(sup, td->td_module)->type;
                while (ctd->td_traverse == NULL && !sup++->sc_flag);
        }

        if (ctd->td_traverse != NULL)
            if ((vret = ctd->td_traverse(ptr, visit, arg)) != 0)
                return vret;
    }

    if (self->user != NULL)
        if ((vret = visit(self->user, arg)) != 0)
            return vret;

    if (self->dict != NULL)
        if ((vret = visit(self->dict, arg)) != 0)
            return vret;

    for (w = self->first_child; w != NULL; w = w->sibling_next)
    {
        /*
         * We don't traverse if the wrapper is a child of itself.  We
         * do this so that wrapped objects returned by virtual methods
         * with the /Factory/ don't have those objects collected.  This
         * then means that plugins implemented in Python have a chance
         * of working.
         */
        if (w != self)
            if ((vret = visit((PyObject *)w, arg)) != 0)
                return vret;
    }

    return 0;
}


/*
 * The instance clear slot.
 */
static int sipWrapper_clear(sipWrapper *self)
{
    int vret = 0;
    void *ptr;
    sipTypeDef *td;
    PyObject *tmp;

    /* Call the nearest handwritten clear code in the class hierachy. */
    if ((ptr = getPtrTypeDef(self, &td)) != NULL)
    {
        sipTypeDef *ctd = td;

        if (td->td_clear == NULL)
        {
            sipEncodedClassDef *sup;

            if ((sup = td->td_supers) != NULL)
                do
                    ctd = getClassType(sup, td->td_module)->type;
                while (ctd->td_clear == NULL && !sup++->sc_flag);
        }

        if (ctd->td_clear != NULL)
            vret = ctd->td_clear(ptr);
    }

    /* Remove any user object. */
    tmp = self->user;
    self->user = NULL;
    Py_XDECREF(tmp);

    /* Remove the instance dictionary. */
    tmp = self->dict;
    self->dict = NULL;
    Py_XDECREF(tmp);

    /* Detach children (which will be owned by C/C++. */
    while (self->first_child != NULL)
    {
        /*
         * Although this object is being garbage collected it doesn't follow
         * that it's children should be.  So we make sure that the child stays
         * alive and remember we have done so.
         */
        Py_INCREF(self->first_child);
        sipSetCppHasRef(self->first_child);

        removeFromParent(self->first_child);
    }

    return vret;
}


/*
 * The instance read buffer slot.
 */
static SIP_SSIZE_T sipWrapper_getreadbuffer(sipWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    sipTypeDef *td;

    if ((ptr = getPtrTypeDef(self, &td)) == NULL)
        return -1;

    return td->td_readbuffer((PyObject *)self, ptr, segment, ptrptr);
}


/*
 * The instance write buffer slot.
 */
static SIP_SSIZE_T sipWrapper_getwritebuffer(sipWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    sipTypeDef *td;

    if ((ptr = getPtrTypeDef(self, &td)) == NULL)
        return -1;

    return td->td_writebuffer((PyObject *)self, ptr, segment, ptrptr);
}


/*
 * The instance segment count slot.
 */
static SIP_SSIZE_T sipWrapper_getsegcount(sipWrapper *self, SIP_SSIZE_T *lenp)
{
    void *ptr;
    sipTypeDef *td;

    if ((ptr = getPtrTypeDef(self, &td)) == NULL)
        return 0;

    return td->td_segcount((PyObject *)self, ptr, lenp);
}


/*
 * The instance char buffer slot.
 */
static SIP_SSIZE_T sipWrapper_getcharbuffer(sipWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    sipTypeDef *td;

    if ((ptr = getPtrTypeDef(self, &td)) == NULL)
        return -1;

    return td->td_charbuffer((PyObject *)self, ptr, segment, ptrptr);
}


/*
 * The instance dealloc slot.
 */
static void sipWrapper_dealloc(sipWrapper *self)
{
    sipTypeDef *td;

    sipWrapper_clear(self);

    if (getPtrTypeDef(self, &td) != NULL)
    {
        /*
         * Remove the object from the map before calling the class
         * specific dealloc code.  This code calls the C++ dtor and
         * may result in further calls that pass the instance as an
         * argument.  If this is still in the map then it's reference
         * count would be increased (to one) and bad things happen
         * when it drops back to zero again.  (An example is PyQt
         * events generated during the dtor call being passed to an
         * event filter implemented in Python.)  By removing it from
         * the map first we ensure that a new Python object is
         * created.
         */
        sipOMRemoveObject(&cppPyMap, self);

        /* Call the C++ dtor if there is one. */
        if (td->td_dealloc != NULL)
            td->td_dealloc(self);
    }

    while (self->pySigList != NULL)
    {
        sipPySig *ps;
        sipPySigRx *psrx;

        /* Take this one out of the list. */
        ps = self->pySigList;
        self->pySigList = ps->next;

        while ((psrx = ps->rxlist) != NULL)
        {
            ps->rxlist = psrx->next;
            sipFreePySigRx(psrx);
        }

        sip_api_free(ps->name);
        sip_api_free(ps);
    }

    /* Call the standard super-type dealloc. */
    PyBaseObject_Type.tp_dealloc((PyObject *)self);
}


/*
 * The instance call slot.  Note that keyword arguments aren't supported.
 */
static PyObject *sipWrapper_call(PyObject *self,PyObject *args,PyObject *kw)
{
    PyObject *(*f)(PyObject *,PyObject *);

    f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self,call_slot);

    return f(self,args);
}


/*
 * The sequence instance item slot.
 */
static PyObject *sipWrapper_sq_item(PyObject *self, SIP_SSIZE_T n)
{
    PyObject *(*f)(PyObject *,PyObject *);
    PyObject *arg, *res;

#if PY_VERSION_HEX >= 0x02050000
    if ((arg = PyInt_FromSsize_t(n)) == NULL)
#else
    if ((arg = PyInt_FromLong(n)) == NULL)
#endif
        return NULL;

    f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self,getitem_slot);

    res = f(self,arg);

    Py_DECREF(arg);

    return res;
}


/*
 * The mapping instance assign subscript slot.
 */
static int sipWrapper_mp_ass_subscript(PyObject *self,PyObject *key,
                       PyObject *value)
{
    return objobjargprocSlot(self,key,value,(value != NULL ? setitem_slot : delitem_slot));
}


/*
 * The sequence instance assign item slot.
 */
static int sipWrapper_sq_ass_item(PyObject *self, SIP_SSIZE_T i, PyObject *o)
{
    return ssizeobjargprocSlot(self, i, o, (o != NULL ? setitem_slot : delitem_slot));
}


/*
 * The instance rich compare slot.
 */
static PyObject *sipWrapper_richcompare(PyObject *self,PyObject *arg,int op)
{
    PyObject *(*f)(PyObject *,PyObject *);
    sipPySlotType st;

    /* Convert the operation to a slot type. */
    switch (op)
    {
    case Py_LT:
        st = lt_slot;
        break;

    case Py_LE:
        st = le_slot;
        break;

    case Py_EQ:
        st = eq_slot;
        break;

    case Py_NE:
        st = ne_slot;
        break;

    case Py_GT:
        st = gt_slot;
        break;

    case Py_GE:
        st = ge_slot;
        break;
    }

    /* It might not exist if not all the above have been implemented. */
    if ((f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self,st)) == NULL)
    {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return f(self,arg);
}


/*
 * The instance getattro slot.
 */
static PyObject *sipWrapper_getattro(PyObject *obj,PyObject *name)
{
    char *nm;
    PyObject *attr;
    sipWrapperType *wt = (sipWrapperType *)obj->ob_type;
    sipWrapper *w = (sipWrapper *)obj;

    /*
     * If we are getting the instance dictionary of a base wrapper type
     * then we don't want the metatype to handle it.
     */
    if ((nm = PyString_AsString(name)) == NULL)
        return NULL;

    if (strcmp(nm, "__dict__") == 0)
    {
        PyObject *tmpdict = NULL;

        if (isExactWrappedType(wt) && getNonStaticVariables(wt, w, &tmpdict) < 0)
        {
            Py_XDECREF(tmpdict);
            return NULL;
        }

        /*
         * If a copy of the instance dictionary wasn't created then
         * just return the original.  Note that Python doesn't want a
         * proxy.
         */
        if (tmpdict == NULL)
            if ((tmpdict = w->dict) == NULL)
                tmpdict = PyDict_New();
            else
                Py_INCREF(tmpdict);

        return tmpdict;
    }

    /* Try the super-type's method first. */
    if ((attr = PyBaseObject_Type.tp_getattro(obj,name)) != NULL)
        return attr;

    return handleGetLazyAttr(name,wt,w);
}


/*
 * Add the values of all non-static variables to a dictionary (first making a
 * copy of the dictionary if needed).
 */
static int getNonStaticVariables(sipWrapperType *wt,sipWrapper *w,
                 PyObject **ndict)
{
    PyMethodDef *pmd;

    if ((pmd = wt->type->td_variables) != NULL)
        while (pmd->ml_name != NULL)
        {
            if ((pmd->ml_flags & METH_STATIC) == 0)
            {
                int rc;
                PyObject *val, *dict;

                /*
                 * Create a copy of the original dictionary if
                 * it hasn't already been done.
                 */
                if ((dict = *ndict) == NULL)
                {
                    if ((dict = PyDict_Copy(w->dict)) == NULL)
                        return -1;

                    *ndict = dict;
                }

                if ((val = (*pmd->ml_meth)((PyObject *)w,NULL)) == NULL)
                    return -1;

                rc = PyDict_SetItemString(dict,pmd->ml_name,val);

                Py_DECREF(val);

                if (rc < 0)
                    return -1;
            }

            ++pmd;
        }

    return 0;
}


/*
 * The instance setattro slot.
 */
static int sipWrapper_setattro(PyObject *obj,PyObject *name,PyObject *value)
{
    int rc;

    rc = handleSetLazyAttr(name,value,(sipWrapperType *)obj->ob_type,(sipWrapper *)obj);

    if (rc <= 0)
        return rc;

    /* Try the super-type's method last. */
    return PyBaseObject_Type.tp_setattro(obj,name,value);
}


/*
 * The type data structure.  Note that we pretend to be a mapping object and a
 * sequence object at the same time.  Python will choose one over another,
 * depending on the context, but we implement as much as we can and don't make
 * assumptions about which Python will choose.
 */
static sipWrapperType sipWrapper_Type = {
    {
        {
            PyObject_HEAD_INIT(&sipWrapperType_Type)
            0,              /* ob_size */
            "sip.wrapper",          /* tp_name */
            sizeof (sipWrapper),        /* tp_basicsize */
            0,              /* tp_itemsize */
            (destructor)sipWrapper_dealloc, /* tp_dealloc */
            0,              /* tp_print */
            0,              /* tp_getattr */
            0,              /* tp_setattr */
            0,              /* tp_compare */
            0,              /* tp_repr */
            0,              /* tp_as_number */
            0,              /* tp_as_sequence */
            0,              /* tp_as_mapping */
            0,              /* tp_hash */
            0,              /* tp_call */
            0,              /* tp_str */
            sipWrapper_getattro,        /* tp_getattro */
            sipWrapper_setattro,        /* tp_setattro */
            0,              /* tp_as_buffer */
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,  /* tp_flags */
            0,              /* tp_doc */
            (traverseproc)sipWrapper_traverse,  /* tp_traverse */
            (inquiry)sipWrapper_clear,  /* tp_clear */
            0,              /* tp_richcompare */
            0,              /* tp_weaklistoffset */
            0,              /* tp_iter */
            0,              /* tp_iternext */
            0,              /* tp_methods */
            0,              /* tp_members */
            0,              /* tp_getset */
            0,              /* tp_base */
            0,              /* tp_dict */
            0,              /* tp_descr_get */
            0,              /* tp_descr_set */
            offsetof(sipWrapper,dict),  /* tp_dictoffset */
            (initproc)sipWrapper_init,  /* tp_init */
            0,              /* tp_alloc */
            (newfunc)sipWrapper_new,    /* tp_new */
            0,              /* tp_free */
        },
    },
    0,
    0
};


/*
 * Add the slots for a type and all its super-types.
 */
static void addSlots(sipWrapperType *wt, sipTypeDef *td)
{
    sipEncodedClassDef *sup;

    /* Add the buffer interface. */
    if (td->td_readbuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getreadbuffer = (readbufferproc)sipWrapper_getreadbuffer;
#else
        wt->super.as_buffer.bf_getreadbuffer = (getreadbufferproc)sipWrapper_getreadbuffer;
#endif

    if (td->td_writebuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getwritebuffer = (writebufferproc)sipWrapper_getwritebuffer;
#else
        wt->super.as_buffer.bf_getwritebuffer = (getwritebufferproc)sipWrapper_getwritebuffer;
#endif

    if (td->td_segcount != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getsegcount = (segcountproc)sipWrapper_getsegcount;
#else
        wt->super.as_buffer.bf_getsegcount = (getsegcountproc)sipWrapper_getsegcount;
#endif

    if (td->td_charbuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getcharbuffer = (charbufferproc)sipWrapper_getcharbuffer;
#else
        wt->super.as_buffer.bf_getcharbuffer = (getcharbufferproc)sipWrapper_getcharbuffer;
#endif

    /* Add the slots for this type. */
    if (td->td_pyslots != NULL)
        initSlots((PyTypeObject *)wt, &wt->super.as_number, &wt->super.as_sequence, &wt->super.as_mapping, td->td_pyslots, FALSE);

    /* Recurse through any super-types. */
    if ((sup = td->td_supers) != NULL)
        do
            addSlots(wt, getClassType(sup, td->td_module)->type);
        while (!sup++->sc_flag);
}


/*
 * Add the slot handler for each slot present in the type, optionally replacing
 * any that have already been defined.
 */
static void initSlots(PyTypeObject *to, PyNumberMethods *nb, PySequenceMethods *sq, PyMappingMethods *mp, sipPySlotDef *slots, int force)
{
    void *f;

    while ((f = slots->psd_func) != NULL)
        switch (slots++->psd_type)
        {
        case str_slot:
            if (force || to->tp_str == NULL)
                to->tp_str = (reprfunc)f;
            break;

        case int_slot:
            if (nb != NULL)
                if (force || nb->nb_int == NULL)
                    nb->nb_int = (unaryfunc)f;
            break;

        case long_slot:
            if (nb != NULL)
                if (force || nb->nb_long == NULL)
                    nb->nb_long = (unaryfunc)f;
            break;

        case float_slot:
            if (nb != NULL)
                if (force || nb->nb_float == NULL)
                    nb->nb_float = (unaryfunc)f;
            break;

        case len_slot:
            if (mp != NULL)
                if (force || mp->mp_length == NULL)
#if PY_VERSION_HEX >= 0x02050000
                    mp->mp_length = (lenfunc)f;
#else
                    mp->mp_length = (inquiry)f;
#endif
            if (sq != NULL)
                if (force || sq->sq_length == NULL)
#if PY_VERSION_HEX >= 0x02050000
                    sq->sq_length = (lenfunc)f;
#else
                    sq->sq_length = (inquiry)f;
#endif
            break;

        case contains_slot:
            if (sq != NULL)
                if (force || sq->sq_contains == NULL)
                    sq->sq_contains = (objobjproc)f;
            break;

        case add_slot:
            if (nb != NULL)
                if (force || nb->nb_add == NULL)
                    nb->nb_add = (binaryfunc)f;
            break;

        case concat_slot:
            if (sq != NULL)
                if (force || sq->sq_concat == NULL)
                    sq->sq_concat = (binaryfunc)f;
            break;

        case sub_slot:
            if (nb != NULL)
                if (force || nb->nb_subtract == NULL)
                    nb->nb_subtract = (binaryfunc)f;
            break;

        case mul_slot:
            if (nb != NULL)
                if (force || nb->nb_multiply == NULL)
                    nb->nb_multiply = (binaryfunc)f;
            break;

        case repeat_slot:
            if (sq != NULL)
                if (force || sq->sq_repeat == NULL)
#if PY_VERSION_HEX >= 0x02050000
                    sq->sq_repeat = (ssizeargfunc)f;
#else
                    sq->sq_repeat = (intargfunc)f;
#endif
            break;

        case div_slot:
            if (nb != NULL)
            {
                if (force || nb->nb_divide == NULL)
                    nb->nb_divide = (binaryfunc)f;

                if (force || nb->nb_true_divide == NULL)
                    nb->nb_true_divide = (binaryfunc)f;
            }
            break;

        case mod_slot:
            if (nb != NULL)
                if (force || nb->nb_remainder == NULL)
                    nb->nb_remainder = (binaryfunc)f;
            break;

        case and_slot:
            if (nb != NULL)
                if (force || nb->nb_and == NULL)
                    nb->nb_and = (binaryfunc)f;
            break;

        case or_slot:
            if (nb != NULL)
                if (force || nb->nb_or == NULL)
                    nb->nb_or = (binaryfunc)f;
            break;

        case xor_slot:
            if (nb != NULL)
                if (force || nb->nb_xor == NULL)
                    nb->nb_xor = (binaryfunc)f;
            break;

        case lshift_slot:
            if (nb != NULL)
                if (force || nb->nb_lshift == NULL)
                    nb->nb_lshift = (binaryfunc)f;
            break;

        case rshift_slot:
            if (nb != NULL)
                if (force || nb->nb_rshift == NULL)
                    nb->nb_rshift = (binaryfunc)f;
            break;

        case iadd_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_add == NULL)
                    nb->nb_inplace_add = (binaryfunc)f;
            break;

        case iconcat_slot:
            if (sq != NULL)
                if (force || sq->sq_inplace_concat == NULL)
                    sq->sq_inplace_concat = (binaryfunc)f;
            break;

        case isub_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_subtract == NULL)
                    nb->nb_inplace_subtract = (binaryfunc)f;
            break;

        case imul_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_multiply == NULL)
                    nb->nb_inplace_multiply = (binaryfunc)f;
            break;

        case irepeat_slot:
            if (sq != NULL)
                if (force || sq->sq_inplace_repeat == NULL)
#if PY_VERSION_HEX >= 0x02050000
                    sq->sq_inplace_repeat = (ssizeargfunc)f;
#else
                    sq->sq_inplace_repeat = (intargfunc)f;
#endif
            break;

        case idiv_slot:
            if (nb != NULL)
            {
                if (force || nb->nb_inplace_divide == NULL)
                    nb->nb_inplace_divide = (binaryfunc)f;

                if (force || nb->nb_inplace_true_divide == NULL)
                    nb->nb_inplace_true_divide = (binaryfunc)f;
            }
            break;

        case imod_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_remainder == NULL)
                    nb->nb_inplace_remainder = (binaryfunc)f;
            break;

        case iand_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_and == NULL)
                    nb->nb_inplace_and = (binaryfunc)f;
            break;

        case ior_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_or == NULL)
                    nb->nb_inplace_or = (binaryfunc)f;
            break;

        case ixor_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_xor == NULL)
                    nb->nb_inplace_xor = (binaryfunc)f;
            break;

        case ilshift_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_lshift == NULL)
                    nb->nb_inplace_lshift = (binaryfunc)f;
            break;

        case irshift_slot:
            if (nb != NULL)
                if (force || nb->nb_inplace_rshift == NULL)
                    nb->nb_inplace_rshift = (binaryfunc)f;
            break;

        case invert_slot:
            if (nb != NULL)
                if (force || nb->nb_invert == NULL)
                    nb->nb_invert = (unaryfunc)f;
            break;

        case call_slot:
            if (force || to->tp_call == NULL)
                to->tp_call = sipWrapper_call;
            break;

        case getitem_slot:
            if (mp != NULL)
                if (force || mp->mp_subscript == NULL)
                    mp->mp_subscript = (binaryfunc)f;
            if (sq != NULL)
                if (force || sq->sq_item == NULL)
                    sq->sq_item = sipWrapper_sq_item;
            break;

        case setitem_slot:
        case delitem_slot:
            if (mp != NULL)
                if (force || mp->mp_ass_subscript == NULL)
                    mp->mp_ass_subscript = sipWrapper_mp_ass_subscript;
            if (sq != NULL)
                if (force || sq->sq_ass_item == NULL)
                    sq->sq_ass_item = sipWrapper_sq_ass_item;
            break;

        case lt_slot:
        case le_slot:
        case eq_slot:
        case ne_slot:
        case gt_slot:
        case ge_slot:
            if (force || to->tp_richcompare == NULL)
                to->tp_richcompare = sipWrapper_richcompare;
            break;

        case cmp_slot:
            if (force || to->tp_compare == NULL)
                to->tp_compare = (cmpfunc)f;
            break;

        case nonzero_slot:
            if (nb != NULL)
                if (force || nb->nb_nonzero == NULL)
                    nb->nb_nonzero = (inquiry)f;
            break;

        case neg_slot:
            if (nb != NULL)
                if (force || nb->nb_negative == NULL)
                    nb->nb_negative = (unaryfunc)f;
            break;

        case repr_slot:
            if (force || to->tp_repr == NULL)
                to->tp_repr = (reprfunc)f;
            break;

        case hash_slot:
            if (force || to->tp_hash == NULL)
                to->tp_hash = (hashfunc)f;
            break;

        case pos_slot:
            if (nb != NULL)
                if (force || nb->nb_positive == NULL)
                    nb->nb_positive = (unaryfunc)f;
            break;

        case abs_slot:
            if (nb != NULL)
                if (force || nb->nb_absolute == NULL)
                    nb->nb_absolute = (unaryfunc)f;
            break;
        }
}


/*
 * Search for a named class and return TRUE and the necessary information to
 * create an instance of it if it was found.
 */
static int findClassArg(sipExportedModuleDef *emd, const char *name,
            size_t len, sipSigArg *at, int indir)
{
    int i;
    sipWrapperType **wtp = emd->em_types;

    for (i = 0; i < emd->em_nrtypes; ++i)
    {
        sipWrapperType *wt;

        if ((wt = *wtp++) == NULL)
            continue;

        if (wt->type->td_cname != NULL)
        {
            if (!nameEq(wt->type->td_cname, name, len))
                continue;
        }
        else if (!sameScopedName(wt->type->td_name, name, len))
            continue;

        if (indir == 0)
            at->atype = class_sat;
        else if (indir == 1)
            at->atype = classp_sat;
        else
            at->atype = unknown_sat;

        at->u.wt = wt;

        return TRUE;
    }

    return FALSE;
}


/*
 * Search for a mapped type and return TRUE and the necessary information to
 * create an instance of it if it was found.
 */
static int findMtypeArg(sipMappedType **mttab, const char *name, size_t len,
            sipSigArg *at, int indir)
{
    sipMappedType *mt;

    while ((mt = *mttab++) != NULL)
        if (nameEq(mt->mt_name, name, len))
        {
            if (indir == 0)
                at->atype = mtype_sat;
            else if (indir == 1)
                at->atype = mtypep_sat;
            else
                at->atype = unknown_sat;

            at->u.mt = mt;

            return TRUE;
        }

    return FALSE;
}


/*
 * Search for a named enum and return TRUE and the necessary information to
 * create an instance of it if it was found.
 */
static int findEnumArg(sipExportedModuleDef *emd, const char *name, size_t len,
               sipSigArg *at, int indir)
{
    int i;
    sipEnumDef *ed;

    for (ed = emd->em_enumdefs, i = 0; i < emd->em_nrenums; ++i, ++ed)
    {
        if (ed->e_cname != NULL)
        {
            if (!nameEq(ed->e_cname, name, len))
                continue;
        }
        else if (!sameScopedName(ed->e_name, name, len))
            continue;

        if (indir == 0)
            at->atype = enum_sat;
        else
            at->atype = unknown_sat;

        at->u.et = emd->em_enums[i];

        return TRUE;
    }

    return FALSE;
}


/*
 * Search for a named type and the necessary information to create an instance
 * of it.
 */
void sipFindSigArgType(const char *name, size_t len, sipSigArg *at, int indir)
{
    sipExportedModuleDef *em;
    sipPyObject *po;

    at->atype = unknown_sat;

    for (em = clientList; em != NULL; em = em->em_next)
    {
        sipTypedefDef *tdd;

        /* Search for a typedef. */
        if ((tdd = em->em_typedefs) != NULL)
            while (tdd->tdd_name != NULL)
            {
                if (nameEq(tdd->tdd_name, name, len))
                {
                    sipExportedModuleDef *tem;
                    const char *tn;
                    size_t tnlen;

                    at->atype = tdd->tdd_type;

                    /* Done with the simple cases. */
                    if ((tn = tdd->tdd_type_name) == NULL)
                        return;

                    /*
                     * Find the module that this class, mapped type or enum is
                     * defined in.
                     */
                    if (tdd->tdd_mod_name == NULL)
                        tem = em;
                    else
                        for (tem = clientList; tem != NULL; tem = tem->em_next)
                            if (strcmp(tem->em_name, tdd->tdd_mod_name) == 0)
                                break;

                    tnlen = strlen(tn);

                    switch (tdd->tdd_type)
                    {
                    case class_sat:
                        findClassArg(tem, tn, tnlen, at, indir);
                        break;

                    case mtype_sat:
                        findMtypeArg(tem->em_mappedtypes, tn, tnlen, at, indir);
                        break;

                    case enum_sat:
                        findEnumArg(tem, tn, tnlen, at, indir);
                        break;
                    }

                    /* We should have found it by now. */
                    return;
                }

                ++tdd;
            }

        /* Search for a class. */
        if (em->em_types != NULL && findClassArg(em, name, len, at, indir))
            return;

        /* Search for a mapped type. */
        if (em->em_mappedtypes != NULL && findMtypeArg(em->em_mappedtypes, name, len, at, indir))
            return;

        /* Search for an enum. */
        if (em->em_enums != NULL && findEnumArg(em, name, len, at, indir))
            return;
    }

    /* Search for a dynamically registered int type. */
    for (po = sipRegisteredIntTypes; po != NULL; po = po->next)
    {
        int i;

        for (i = 0; i < PyTuple_GET_SIZE(po->object); ++i)
        {
            char *int_nm = PyString_AsString(PyTuple_GET_ITEM(po->object, i));

            if (int_nm == NULL)
                continue;

            if (nameEq(int_nm, name, len))
            {
                at->atype = int_sat;
                return;
            }
        }
    }
}


/*
 * Compare a '\0' terminated string with the first len characters of a second
 * and return a non-zero value if they are equal.
 */
static int nameEq(const char *with, const char *name, size_t len)
{
    return (strlen(with) == len && strncmp(with, name, len) == 0);
}


/*
 * Return TRUE if a Python scoped name and a fixed length C++ scoped name
 * match.
 */
static int sameScopedName(const char *pyname, const char *name, size_t len)
{
    char ch;

    /* Skip the module name from the Python name. */
    pyname = strchr(pyname, '.') + 1;

    while ((ch = *pyname++) != '\0' && len)
        if (ch == '.')
        {
            if (len < 2 || name[0] != ':' || name[1] != ':')
                return FALSE;

            name += 2;
            len -= 2;
        }
        else if (ch == name[0])
        {
            ++name;
            --len;
        }
        else
            return FALSE;

    return (ch == '\0' && len == 0);
}


/*
 * Register a Python tuple of type names that will be interpreted as ints if
 * they are seen as signal arguments.
 */
static int sip_api_register_int_types(PyObject *args)
{
    sipPyObject *po;
    int bad_args = FALSE;

    /* Raise an exception if the arguments are bad. */
    if (PyTuple_Check(args))
    {
        int i;

        for (i = 0; i < PyTuple_GET_SIZE(args); ++i)
            if (!PyString_Check(PyTuple_GET_ITEM(args, i)))
            {
                bad_args = TRUE;
                break;
            }
    }
    else
        bad_args = TRUE;

    if (bad_args)
    {
        PyErr_SetString(PyExc_TypeError, "all arguments must be strings");
        return -1;
    }

    if ((po = sip_api_malloc(sizeof (sipPyObject))) == NULL)
        return -1;

    Py_INCREF(args);

    po->object = args;
    po->next = sipRegisteredIntTypes;

    sipRegisteredIntTypes = po;

    return 0;
}


/*
 * Register a symbol with a name.  A negative value is returned if the name was
 * already registered.
 */
static int sip_api_export_symbol(const char *name, void *sym)
{
    sipSymbol *ss;

    if (sip_api_import_symbol(name) != NULL)
        return -1;

    if ((ss = sip_api_malloc(sizeof (sipSymbol))) == NULL)
        return -1;

    ss->name = name;
    ss->symbol = sym;
    ss->next = sipSymbolList;

    sipSymbolList = ss;

    return 0;
}


/*
 * Return the symbol registered with the given name.  NULL is returned if the
 * name was not registered.
 */
static void *sip_api_import_symbol(const char *name)
{
    sipSymbol *ss;

    for (ss = sipSymbolList; ss != NULL; ss = ss->next)
        if (strcmp(ss->name, name) == 0)
            return ss->symbol;

    return NULL;
}
