/*
 * SIP library code.
 *
 * @BS_LICENSE@
 */


#include <Python.h>

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "sip.h"
#include "sipint.h"


/*
 * Forward declarations for the objects and functions that make up the public
 * and private SIP API.
 */
static sipWrapperType sipWrapper_Type;
static PyTypeObject sipWrapperType_Type;
static PyTypeObject sipEnumType_Type;
static PyTypeObject sipVoidPtr_Type;

static void sip_api_bad_catcher_result(PyObject *method);
static void sip_api_bad_length_for_slice(SIP_SSIZE_T seqlen,
        SIP_SSIZE_T slicelen);
static PyObject *sip_api_build_result(int *isErr, const char *fmt, ...);
static PyObject *sip_api_call_method(int *isErr, PyObject *method,
        const char *fmt, ...);
static SIP_SSIZE_T sip_api_convert_from_sequence_index(SIP_SSIZE_T idx,
        SIP_SSIZE_T len);
static int sip_api_can_convert_to_type(PyObject *pyObj, const sipTypeDef *td,
        int flags);
static void *sip_api_convert_to_type(PyObject *pyObj, const sipTypeDef *td,
        PyObject *transferObj, int flags, int *statep, int *iserrp);
static void *sip_api_force_convert_to_type(PyObject *pyObj,
        const sipTypeDef *td, PyObject *transferObj, int flags, int *statep,
        int *iserrp);
static int sip_api_can_convert_to_enum(PyObject *pyObj, const sipTypeDef *td);
static void sip_api_release_type(void *cpp, const sipTypeDef *td, int state);
static PyObject *sip_api_convert_from_new_type(void *cpp, const sipTypeDef *td,
        PyObject *transferObj);
static int sip_api_get_state(PyObject *transferObj);
static PyObject *sip_api_get_pyobject(void *cppPtr, const sipTypeDef *td);
static sipWrapperType *sip_api_map_int_to_class(int typeInt,
        const sipIntTypeClassMap *map, int maplen);
static sipWrapperType *sip_api_map_string_to_class(const char *typeString,
        const sipStringTypeClassMap *map, int maplen);
static int sip_api_parse_result(int *isErr, PyObject *method, PyObject *res,
        const char *fmt, ...);
static void sip_api_trace(unsigned mask,const char *fmt,...);
static void sip_api_transfer_back(PyObject *self);
static void sip_api_transfer_to(PyObject *self, PyObject *owner);
static int sip_api_export_module(sipExportedModuleDef *client,
        unsigned api_major, unsigned api_minor, void *unused);
static int sip_api_init_module(sipExportedModuleDef *client,
        PyObject *mod_dict);
static int sip_api_parse_args(int *argsParsedp, PyObject *sipArgs,
        const char *fmt, ...);
static int sip_api_parse_pair(int *argsParsedp, PyObject *sipArg0,
        PyObject *sipArg1, const char *fmt, ...);
static void *sip_api_convert_to_void_ptr(PyObject *obj);
static void sip_api_no_function(int argsParsed, const char *func);
static void sip_api_no_method(int argsParsed, const char *classname,
        const char *method);
static void sip_api_abstract_method(const char *classname, const char *method);
static void sip_api_bad_class(const char *classname);
static void *sip_api_get_complex_cpp_ptr(sipSimpleWrapper *sw);
static PyObject *sip_api_is_py_method(sip_gilstate_t *gil, char *pymc,
        sipSimpleWrapper *sipSelf, const char *cname, const char *mname);
static void sip_api_call_hook(const char *hookname);
static void sip_api_raise_unknown_exception(void);
static void sip_api_raise_type_exception(const sipTypeDef *td, void *ptr);
static int sip_api_add_type_instance(PyObject *dict, const char *name,
        void *cppPtr, const sipTypeDef *td);
static void sip_api_bad_operator_arg(PyObject *self, PyObject *arg,
        sipPySlotType st);
static PyObject *sip_api_pyslot_extend(sipExportedModuleDef *mod,
        sipPySlotType st, const sipTypeDef *td, PyObject *arg0,
        PyObject *arg1);
static void sip_api_add_delayed_dtor(sipSimpleWrapper *w);
static unsigned long sip_api_long_as_unsigned_long(PyObject *o);
static int sip_api_export_symbol(const char *name, void *sym);
static void *sip_api_import_symbol(const char *name);
static const sipTypeDef *sip_api_find_type(const char *type);
static sipWrapperType *sip_api_find_class(const char *type);
static const sipMappedType *sip_api_find_mapped_type(const char *type);
static PyTypeObject *sip_api_find_named_enum(const char *type);
static char sip_api_bytes_as_char(PyObject *obj);
static const char *sip_api_bytes_as_string(PyObject *obj);
static char sip_api_string_as_ascii_char(PyObject *obj);
static const char *sip_api_string_as_ascii_string(PyObject **obj);
static char sip_api_string_as_latin1_char(PyObject *obj);
static const char *sip_api_string_as_latin1_string(PyObject **obj);
static char sip_api_string_as_utf8_char(PyObject *obj);
static const char *sip_api_string_as_utf8_string(PyObject **obj);
#if defined(HAVE_WCHAR_H)
static wchar_t sip_api_unicode_as_wchar(PyObject *obj);
static wchar_t *sip_api_unicode_as_wstring(PyObject *obj);
#else
static int sip_api_unicode_as_wchar(PyObject *obj);
static int *sip_api_unicode_as_wstring(PyObject *obj);
#endif
static void sip_api_transfer_break(PyObject *self);
static PyObject *sip_api_convert_from_void_ptr(void *val);
static PyObject *sip_api_convert_from_const_void_ptr(const void *val);
static PyObject *sip_api_convert_from_void_ptr_and_size(void *val,
        SIP_SSIZE_T size);
static PyObject *sip_api_convert_from_const_void_ptr_and_size(const void *val,
        SIP_SSIZE_T size);
static int sip_api_deprecated(const char *classname, const char *method);
static int sip_api_register_py_type(PyTypeObject *supertype);
static PyObject *sip_api_convert_from_enum(int eval, const sipTypeDef *td);
static const sipTypeDef *sip_api_type_from_py_type_object(PyTypeObject *py_type);
static const sipTypeDef *sip_api_type_scope(const sipTypeDef *td);
static const char *sip_api_resolve_typedef(const char *name);
static int sip_api_register_attribute_getter(const sipTypeDef *td,
        sipAttrGetterFunc getter);
static void sip_api_clear_any_slot_reference(sipSlot *slot);
static int sip_api_visit_slot(sipSlot *slot, visitproc visit, void *arg);
static void sip_api_keep_reference(PyObject *self, int key, PyObject *obj);


/*
 * The data structure that represents the SIP API.
 */
static const sipAPIDef sip_api = {
    /* This must be first. */
    sip_api_export_module,
    /*
     * The following are part of the public API.
     */
    (PyTypeObject *)&sipSimpleWrapper_Type,
    (PyTypeObject *)&sipWrapper_Type,
    &sipWrapperType_Type,
    &sipVoidPtr_Type,

    sip_api_bad_catcher_result,
    sip_api_bad_length_for_slice,
    sip_api_build_result,
    sip_api_call_method,
    sip_api_connect_rx,
    sip_api_convert_from_sequence_index,
    sip_api_can_convert_to_type,
    sip_api_convert_to_type,
    sip_api_force_convert_to_type,
    sip_api_can_convert_to_enum,
    sip_api_release_type,
    sip_api_convert_from_type,
    sip_api_convert_from_new_type,
    sip_api_convert_from_enum,
    sip_api_get_state,
    sip_api_disconnect_rx,
    sip_api_free,
    sip_api_get_pyobject,
    sip_api_malloc,
    sip_api_parse_result,
    sip_api_trace,
    sip_api_transfer_back,
    sip_api_transfer_to,
    sip_api_transfer_break,
    sip_api_long_as_unsigned_long,
    sip_api_convert_from_void_ptr,
    sip_api_convert_from_const_void_ptr,
    sip_api_convert_from_void_ptr_and_size,
    sip_api_convert_from_const_void_ptr_and_size,
    sip_api_convert_to_void_ptr,
    sip_api_export_symbol,
    sip_api_import_symbol,
    sip_api_find_type,
    sip_api_register_py_type,
    sip_api_type_from_py_type_object,
    sip_api_type_scope,
    sip_api_resolve_typedef,
    sip_api_register_attribute_getter,
    sip_api_is_api_enabled,
    /*
     * The following are deprecated parts of the public API.
     */
    sip_api_find_named_enum,
    sip_api_find_mapped_type,
    sip_api_find_class,
    sip_api_map_int_to_class,
    sip_api_map_string_to_class,
    /*
     * The following may be used by Qt support code but by no other handwritten
     * code.
     */
    sip_api_free_sipslot,
    sip_api_same_slot,
    sip_api_convert_rx,
    sip_api_invoke_slot,
    sip_api_save_slot,
    sip_api_clear_any_slot_reference,
    sip_api_visit_slot,
    /*
     * The following are not part of the public API.
     */
    sip_api_init_module,
    sip_api_parse_args,
    sip_api_parse_pair,
    sip_api_common_dtor,
    sip_api_no_function,
    sip_api_no_method,
    sip_api_abstract_method,
    sip_api_bad_class,
    sip_api_get_cpp_ptr,
    sip_api_get_complex_cpp_ptr,
    sip_api_is_py_method,
    sip_api_call_hook,
    sip_api_start_thread,
    sip_api_end_thread,
    sip_api_raise_unknown_exception,
    sip_api_raise_type_exception,
    sip_api_add_type_instance,
    sip_api_bad_operator_arg,
    sip_api_pyslot_extend,
    sip_api_add_delayed_dtor,
    sip_api_bytes_as_char,
    sip_api_bytes_as_string,
    sip_api_string_as_ascii_char,
    sip_api_string_as_ascii_string,
    sip_api_string_as_latin1_char,
    sip_api_string_as_latin1_string,
    sip_api_string_as_utf8_char,
    sip_api_string_as_utf8_string,
    sip_api_unicode_as_wchar,
    sip_api_unicode_as_wstring,
    sip_api_deprecated,
    sip_api_keep_reference
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
#define FORMAT_NO_CONVERTORS    0x08    /* Suppress any convertors. */
#define FORMAT_TRANSFER_THIS    0x10    /* Support for /TransferThis/. */


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


/*
 * An entry in the linked list of attribute getters.
 */
typedef struct _sipAttrGetter {
    PyTypeObject *type;             /* The Python type being handled. */
    sipAttrGetterFunc getter;       /* The getter. */
    struct _sipAttrGetter *next;    /* The next in the list. */
} sipAttrGetter;


/*****************************************************************************
 * The structures to support a Python type to hold a named enum.
 *****************************************************************************/

/*
 * The type data structure.  We inherit everything from the standard Python
 * metatype and the size of the type object created is increased to accomodate
 * the extra information we associate with a named enum type.
 */
static PyTypeObject sipEnumType_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "sip.enumtype",         /* tp_name */
    sizeof (sipEnumTypeObject), /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_reserved (Python v3), tp_compare (Python v2) */
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
};


sipQtAPI *sipQtSupport = NULL;
sipTypeDef *sipQObjectType;


/*
 * Various strings as Python objects created as and when needed.
 */
static PyObject *licenseName = NULL;
static PyObject *licenseeName = NULL;
static PyObject *typeName = NULL;
static PyObject *timestampName = NULL;
static PyObject *signatureName = NULL;

static sipObjectMap cppPyMap;           /* The C/C++ to Python map. */
static sipExportedModuleDef *moduleList = NULL; /* List of registered modules. */
static unsigned traceMask = 0;          /* The current trace mask. */

static sipTypeDef *currentType = NULL;  /* The type being created. */

static PyObject *type_unpickler;        /* The type unpickler function. */
static PyObject *enum_unpickler;        /* The enum unpickler function. */
static sipSymbol *sipSymbolList = NULL; /* The list of published symbols. */
static sipAttrGetter *sipAttrGetters = NULL;  /* The list of attribute getters. */
static sipPyObject *sipRegisteredPyTypes = NULL;    /* Registered Python types. */
static PyInterpreterState *sipInterpreter = NULL;   /* The interpreter. */


static void addClassSlots(sipWrapperType *wt, sipClassTypeDef *ctd);
static void addTypeSlots(PyTypeObject *to, PyNumberMethods *nb,
        PySequenceMethods *sq, PyMappingMethods *mp, sipPySlotDef *slots);
static void *findSlot(PyObject *self, sipPySlotType st);
static void *findSlotInType(sipPySlotDef *psd, sipPySlotType st);
static int objobjargprocSlot(PyObject *self, PyObject *arg1, PyObject *arg2,
        sipPySlotType st);
static int ssizeobjargprocSlot(PyObject *self, SIP_SSIZE_T arg1,
        PyObject *arg2, sipPySlotType st);
static PyObject *buildObject(PyObject *tup, const char *fmt, va_list va);
static int parsePass1(sipSimpleWrapper **selfp, int *selfargp,
        int *argsParsedp, PyObject *sipArgs, const char *fmt, va_list va);
static int parsePass2(sipSimpleWrapper *self, int selfarg, int nrargs,
        PyObject *sipArgs, const char *fmt, va_list va);
static int isQObject(PyObject *obj);
static int canConvertFromSequence(PyObject *seq, const sipTypeDef *td);
static int convertFromSequence(PyObject *seq, const sipTypeDef *td,
        void **array, SIP_SSIZE_T *nr_elem);
static PyObject *convertToSequence(void *array, SIP_SSIZE_T nr_elem,
        const sipTypeDef *td);
static int getSelfFromArgs(sipTypeDef *td, PyObject *args, int argnr,
        sipSimpleWrapper **selfp);
static PyObject *createEnumMember(sipTypeDef *td, sipEnumMemberDef *enm);
static int compareTypedefName(const void *key, const void *el);
static int checkPointer(void *ptr);
static void *cast_cpp_ptr(void *ptr, PyTypeObject *src_type,
        const sipTypeDef *dst_type);
static void badArgs(int argsParsed, const char *scope, const char *method);
static void finalise(void);
static PyObject *getDefaultBases(void);
static PyObject *getScopeDict(sipTypeDef *td, PyObject *mod_dict,
        sipExportedModuleDef *client);
static PyObject *createContainerType(sipContainerDef *cod, sipTypeDef *td,
        PyObject *bases, PyObject *metatype, PyObject *mod_dict,
        sipExportedModuleDef *client);
static int createClassType(sipExportedModuleDef *client, sipClassTypeDef *ctd,
        PyObject *mod_dict);
static int createMappedType(sipExportedModuleDef *client,
        sipMappedTypeDef *mtd, PyObject *mod_dict);
static sipExportedModuleDef *getModule(PyObject *mname_obj);
static PyObject *pickle_type(PyObject *obj, PyObject *);
static PyObject *unpickle_type(PyObject *, PyObject *args);
static PyObject *pickle_enum(PyObject *obj, PyObject *);
static PyObject *unpickle_enum(PyObject *, PyObject *args);
static int setReduce(PyTypeObject *type, PyMethodDef *pickler);
static int createEnumType(sipExportedModuleDef *client, sipEnumTypeDef *etd,
        PyObject *mod_dict);
static PyObject *createTypeDict(PyObject *mname);
static sipExportedModuleDef *getTypeModule(const sipEncodedTypeDef *enc,
        sipExportedModuleDef *em);
static sipTypeDef *getGeneratedType(const sipEncodedTypeDef *enc,
        sipExportedModuleDef *em);
static const sipTypeDef *convertSubClass(const sipTypeDef *td, void **cppPtr);
static void *getPtrTypeDef(sipSimpleWrapper *self,
        const sipClassTypeDef **ctd);
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
static int addTypeInstances(PyObject *dict, sipTypeInstanceDef *ti);
static int addSingleTypeInstance(PyObject *dict, const char *name,
        void *cppPtr, const sipTypeDef *td, int initflags);
static int addLicense(PyObject *dict, sipLicenseDef *lc);
static PyObject *cast(PyObject *self, PyObject *args);
static PyObject *callDtor(PyObject *self, PyObject *args);
static PyObject *dumpWrapper(PyObject *self, PyObject *args);
static PyObject *isDeleted(PyObject *self, PyObject *args);
static PyObject *isPyOwned(PyObject *self, PyObject *args);
static PyObject *setDeleted(PyObject *self, PyObject *args);
static PyObject *setTraceMask(PyObject *self, PyObject *args);
static PyObject *wrapInstance(PyObject *self, PyObject *args);
static PyObject *unwrapInstance(PyObject *self, PyObject *args);
static PyObject *transferBack(PyObject *self, PyObject *args);
static PyObject *transferTo(PyObject *self, PyObject *args);
static void print_object(const char *label, PyObject *obj);
static void addToParent(sipWrapper *self, sipWrapper *owner);
static void removeFromParent(sipWrapper *self);
static void release(void *addr, const sipTypeDef *td, int state);
static void callPyDtor(sipSimpleWrapper *self);
static int parseBytes_AsCharArray(PyObject *obj, const char **ap,
        SIP_SSIZE_T *aszp);
static int parseBytes_AsChar(PyObject *obj, char *ap);
static int parseBytes_AsString(PyObject *obj, const char **ap);
static int parseString_AsASCIIChar(PyObject *obj, char *ap);
static PyObject *parseString_AsASCIIString(PyObject *obj, const char **ap);
static int parseString_AsLatin1Char(PyObject *obj, char *ap);
static PyObject *parseString_AsLatin1String(PyObject *obj, const char **ap);
static int parseString_AsUTF8Char(PyObject *obj, char *ap);
static PyObject *parseString_AsUTF8String(PyObject *obj, const char **ap);
static int parseString_AsEncodedChar(PyObject *bytes, PyObject *obj, char *ap);
static PyObject *parseString_AsEncodedString(PyObject *bytes, PyObject *obj,
        const char **ap);
#if defined(HAVE_WCHAR_H)
static int parseWCharArray(PyObject *obj, wchar_t **ap, SIP_SSIZE_T *aszp);
static int convertToWCharArray(PyObject *obj, wchar_t **ap, SIP_SSIZE_T *aszp);
static int parseWChar(PyObject *obj, wchar_t *ap);
static int convertToWChar(PyObject *obj, wchar_t *ap);
static int parseWCharString(PyObject *obj, wchar_t **ap);
static int convertToWCharString(PyObject *obj, wchar_t **ap);
#else
static void raiseNoWChar();
#endif
static void *getComplexCppPtr(sipSimpleWrapper *w, const sipTypeDef *td);
static PyObject *make_voidptr(void *voidptr, SIP_SSIZE_T size, int rw);
static PyObject *findPyType(const char *name);
static int addPyObjectToList(sipPyObject **head, PyObject *object);
static PyObject *getDictFromObject(PyObject *obj);
static void forgetObject(sipSimpleWrapper *sw);
static int add_lazy_container_attrs(sipTypeDef *td, sipContainerDef *cod,
        PyObject *dict);
static int add_lazy_attrs(sipTypeDef *td);
static int add_all_lazy_attrs(sipTypeDef *td);
static int objectify(const char *s, PyObject **objp);


/*
 * The Python module initialisation function.
 */
#if PY_MAJOR_VERSION >= 3
#define SIP_MODULE_ENTRY        PyInit_sip
#define SIP_MODULE_TYPE         PyObject *
#define SIP_MODULE_DISCARD(m)   Py_DECREF(m)
#define SIP_FATAL(s)            return NULL
#define SIP_MODULE_RETURN(m)    return (m)
#else
#define SIP_MODULE_ENTRY        initsip
#define SIP_MODULE_TYPE         void
#define SIP_MODULE_DISCARD(m)
#define SIP_FATAL(s)            Py_FatalError(s)
#define SIP_MODULE_RETURN(m)
#endif

#if defined(SIP_STATIC_MODULE)
SIP_MODULE_TYPE SIP_MODULE_ENTRY(void)
#else
PyMODINIT_FUNC SIP_MODULE_ENTRY(void)
#endif
{
    static PyMethodDef methods[] = {
        {"cast", cast, METH_VARARGS, NULL},
        {"delete", callDtor, METH_VARARGS, NULL},
        {"dump", dumpWrapper, METH_VARARGS, NULL},
        {"getapi", sipGetAPI, METH_VARARGS, NULL},
        {"isdeleted", isDeleted, METH_VARARGS, NULL},
        {"ispyowned", isPyOwned, METH_VARARGS, NULL},
        {"setapi", sipSetAPI, METH_VARARGS, NULL},
        {"setdeleted", setDeleted, METH_VARARGS, NULL},
        {"settracemask", setTraceMask, METH_VARARGS, NULL},
        {"transferback", transferBack, METH_VARARGS, NULL},
        {"transferto", transferTo, METH_VARARGS, NULL},
        {"wrapinstance", wrapInstance, METH_VARARGS, NULL},
        {"unwrapinstance", unwrapInstance, METH_VARARGS, NULL},
        {"_unpickle_type", unpickle_type, METH_VARARGS, NULL},
        {"_unpickle_enum", unpickle_enum, METH_VARARGS, NULL},
        {NULL, NULL, 0, NULL}
    };

#if PY_MAJOR_VERSION >= 3
    static PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "sip",                  /* m_name */
        NULL,                   /* m_doc */
        -1,                     /* m_size */
        methods,                /* m_methods */
        NULL,                   /* m_reload */
        NULL,                   /* m_traverse */
        NULL,                   /* m_clear */
        NULL,                   /* m_free */
    };
#endif

    int rc;
    PyObject *mod, *mod_dict, *obj;

#ifdef WITH_THREAD
    PyEval_InitThreads();
#endif

    /* Initialise the types. */
    sipWrapperType_Type.tp_base = &PyType_Type;

    if (PyType_Ready(&sipWrapperType_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.wrappertype type");

    if (PyType_Ready((PyTypeObject *)&sipSimpleWrapper_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.simplewrapper type");

    if (sip_api_register_py_type((PyTypeObject *)&sipSimpleWrapper_Type) < 0)
        SIP_FATAL("sip: Failed to register sip.simplewrapper type");

#if PY_VERSION_HEX >= 0x02050000
    sipWrapper_Type.super.ht_type.tp_base = (PyTypeObject *)&sipSimpleWrapper_Type;
#else
    sipWrapper_Type.super.type.tp_base = (PyTypeObject *)&sipSimpleWrapper_Type;
#endif

    if (PyType_Ready((PyTypeObject *)&sipWrapper_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.wrapper type");

    if (PyType_Ready(&sipMethodDescr_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.methoddescriptor type");

    if (PyType_Ready(&sipVariableDescr_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.variabledescriptor type");

    sipEnumType_Type.tp_base = &PyType_Type;

    if (PyType_Ready(&sipEnumType_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.enumtype type");

    if (PyType_Ready(&sipVoidPtr_Type) < 0)
        SIP_FATAL("sip: Failed to initialise sip.voidptr type");

#if PY_MAJOR_VERSION >= 3
    mod = PyModule_Create(&module_def);
#else
    mod = Py_InitModule("sip", methods);
#endif

    if (mod == NULL)
        SIP_FATAL("sip: Failed to intialise sip module");

    mod_dict = PyModule_GetDict(mod);

    /* Get a reference to the pickle helpers. */
    type_unpickler = PyDict_GetItemString(mod_dict, "_unpickle_type");
    enum_unpickler = PyDict_GetItemString(mod_dict, "_unpickle_enum");

    if (type_unpickler == NULL || enum_unpickler == NULL)
    {
        SIP_MODULE_DISCARD(mod);
        SIP_FATAL("sip: Failed to get pickle helpers");
    }

    /* Publish the SIP API. */
    if ((obj = PyCObject_FromVoidPtr((void *)&sip_api, NULL)) == NULL)
    {
        SIP_MODULE_DISCARD(mod);
        SIP_FATAL("sip: Failed to create _C_API object");
    }

    rc = PyDict_SetItemString(mod_dict, "_C_API", obj);
    Py_DECREF(obj);

    if (rc < 0)
    {
        SIP_MODULE_DISCARD(mod);
        SIP_FATAL("sip: Failed to add _C_API object to module dictionary");
    }

    /* Add the SIP version number, but don't worry about errors. */
#if PY_MAJOR_VERSION >= 3
    obj = PyLong_FromLong(SIP_VERSION);
#else
    obj = PyInt_FromLong(SIP_VERSION);
#endif

    if (obj != NULL)
    {
        PyDict_SetItemString(mod_dict, "SIP_VERSION", obj);
        Py_DECREF(obj);
    }

#if PY_MAJOR_VERSION >= 3
    obj = PyUnicode_FromString(SIP_VERSION_STR);
#else
    obj = PyString_FromString(SIP_VERSION_STR);
#endif

    if (obj != NULL)
    {
        PyDict_SetItemString(mod_dict, "SIP_VERSION_STR", obj);
        Py_DECREF(obj);
    }

    /* Add the type objects, but don't worry about errors. */
    PyDict_SetItemString(mod_dict, "wrappertype",
            (PyObject *)&sipWrapperType_Type);
    PyDict_SetItemString(mod_dict, "simplewrapper",
            (PyObject *)&sipSimpleWrapper_Type);
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

    SIP_MODULE_RETURN(mod);
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
 * Dump various bits of potentially useful information to stdout.
 */
static PyObject *dumpWrapper(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;

    if (PyArg_ParseTuple(args, "O!:dump", &sipSimpleWrapper_Type, &sw))
    {
        print_object(NULL, (PyObject *)sw);

#if PY_VERSION_HEX >= 0x02050000
        printf("    Reference count: %" PY_FORMAT_SIZE_T "d\n", Py_REFCNT(sw));
#else
        printf("    Reference count: %d\n", Py_REFCNT(sw));
#endif
        printf("    Address of wrapped object: %p\n", sipGetAddress(sw));
        printf("    To be destroyed by: %s\n", (sipIsPyOwned(sw) ? "Python" : "C/C++"));
        printf("    Derived class?: %s\n", (sipIsDerived(sw) ? "yes" : "no"));

        if (PyObject_TypeCheck((PyObject *)sw, (PyTypeObject *)&sipWrapper_Type))
        {
            sipWrapper *w = (sipWrapper *)sw;

            print_object("Parent wrapper", (PyObject *)w->parent);
            print_object("Next sibling wrapper", (PyObject *)w->sibling_next);
            print_object("Previous sibling wrapper",
                    (PyObject *)w->sibling_prev);
            print_object("First child wrapper", (PyObject *)w->first_child);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    return NULL;
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
        else if (PyObject_TypeCheck(owner, (PyTypeObject *)&sipWrapper_Type))
        {
            PyErr_Format(PyExc_TypeError, "transferto() argument 2 must be sip.wrapper, not %s", Py_TYPE(owner)->tp_name);
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
 * Cast an instance to one of it's sub or super-classes by returning a new
 * Python object with the superclass type wrapping the same C++ instance.
 */
static PyObject *cast(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;
    sipWrapperType *wt, *type;
    void *addr;
    PyTypeObject *ft, *tt;

    if (!PyArg_ParseTuple(args, "O!O!:cast", &sipSimpleWrapper_Type, &sw, &sipWrapperType_Type, &wt))
        return NULL;

    ft = Py_TYPE(sw);
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

    if ((addr = sip_api_get_cpp_ptr(sw, type->type)) == NULL)
        return NULL;

    /*
     * We don't put this new object into the map so that the original object is
     * always found.  It would also totally confuse the map logic.
     */
    return sipWrapSimpleInstance(addr, wt->type, NULL, (sw->flags | SIP_NOT_IN_MAP) & ~SIP_PY_OWNED);
}


/*
 * Call an instance's dtor.
 */
static PyObject *callDtor(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;
    void *addr;
    const sipClassTypeDef *ctd;

    if (!PyArg_ParseTuple(args, "O!:delete", &sipSimpleWrapper_Type, &sw))
        return NULL;

    addr = getPtrTypeDef(sw, &ctd);

    if (checkPointer(addr) < 0)
        return NULL;

    if (PyObject_TypeCheck((PyObject *)sw, (PyTypeObject *)&sipWrapper_Type))
    {
        /*
         * Transfer ownership to C++ so we don't try to release it again when
         * the Python object is garbage collected.
         */
        removeFromParent((sipWrapper *)sw);
        sipResetPyOwned(sw);
    }

    release(addr, (const sipTypeDef *)ctd, sw->flags);

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Check if an instance still exists without raising an exception.
 */
static PyObject *isDeleted(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;
    PyObject *res;

    if (!PyArg_ParseTuple(args, "O!:isdeleted", &sipSimpleWrapper_Type, &sw))
        return NULL;

    res = (sipGetAddress(sw) == NULL ? Py_True : Py_False);

    Py_INCREF(res);
    return res;
}


/*
 * Check if an instance is owned by Python or C/C++.
 */
static PyObject *isPyOwned(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;
    PyObject *res;

    if (!PyArg_ParseTuple(args, "O!:ispyowned", &sipSimpleWrapper_Type, &sw))
        return NULL;

    res = (sipIsPyOwned(sw) ? Py_True : Py_False);

    Py_INCREF(res);
    return res;
}


/*
 * Mark an instance as having been deleted.
 */
static PyObject *setDeleted(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;

    if (!PyArg_ParseTuple(args, "O!:setdeleted", &sipSimpleWrapper_Type, &sw))
        return NULL;

    if (PyObject_TypeCheck((PyObject *)sw, (PyTypeObject *)&sipWrapper_Type))
    {
        /*
         * Transfer ownership to C++ so we don't try to release it when the
         * Python object is garbage collected.
         */
        removeFromParent((sipWrapper *)sw);
        sipResetPyOwned(sw);
    }

    sw->u.cppPtr = NULL;

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Unwrap an instance.
 */
static PyObject *unwrapInstance(PyObject *self, PyObject *args)
{
    sipSimpleWrapper *sw;

    if (PyArg_ParseTuple(args, "O!:unwrapinstance", &sipSimpleWrapper_Type, &sw))
    {
        void *addr;

        /*
         * We just get the pointer but don't try and cast it (which isn't
         * needed and wouldn't work with the way casts are currently
         * implemented if we are unwrapping something derived from a wrapped
         * class).
         */
        if ((addr = sip_api_get_cpp_ptr(sw, NULL)) == NULL)
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
        return sip_api_convert_from_type((void *)addr, wt->type, NULL);

    return NULL;
}


/*
 * Register a client module.  A negative value is returned and an exception
 * raised if there was an error.
 */
static int sip_api_export_module(sipExportedModuleDef *client,
        unsigned api_major, unsigned api_minor, void *unused)
{
    sipExportedModuleDef *em;
    const char *full_name = sipNameOfModule(client);

    /* Check that we can support it. */

    if (api_major != SIP_API_MAJOR_NR || api_minor > SIP_API_MINOR_NR)
    {
#if SIP_API_MINOR_NR > 0
        PyErr_Format(PyExc_RuntimeError,
                "the sip module implements API v%d.0 to v%d.%d but the %s module requires API v%d.%d",
                SIP_API_MAJOR_NR, SIP_API_MAJOR_NR, SIP_API_MINOR_NR,
                full_name, api_major, api_minor);
#else
        PyErr_Format(PyExc_RuntimeError,
                "the sip module implements API v%d.0 but the %s module requires API v%d.%d",
                SIP_API_MAJOR_NR, full_name, api_major, api_minor);
#endif

        return -1;
    }

    /* Import any required modules. */
    if (client->em_imports != NULL)
    {
        sipImportedModuleDef *im = client->em_imports;

        while (im->im_name != NULL)
        {
            PyObject *mod;

            if ((mod = PyImport_ImportModule(im->im_name)) == NULL)
                return -1;

            for (em = moduleList; em != NULL; em = em->em_next)
                if (strcmp(sipNameOfModule(em), im->im_name) == 0)
                    break;

            if (em == NULL)
            {
                PyErr_Format(PyExc_RuntimeError,
                        "the %s module failed to register with the sip module",
                        im->im_name);

                return -1;
            }

            /* Check the versions are compatible. */
            if (im->im_version >= 0 || em->em_version >= 0)
                if (im->im_version != em->em_version)
                {
                    PyErr_Format(PyExc_RuntimeError,
                            "the %s module is version %d but the %s module requires version %d",
                            sipNameOfModule(em), em->em_version, full_name,
                            im->im_version);

                    return -1;
                }

            /* Save the imported module. */
            im->im_module = em;

            ++im;
        }
    }

    for (em = moduleList; em != NULL; em = em->em_next)
    {
        /* SIP clients must have unique names. */
        if (strcmp(sipNameOfModule(em), full_name) == 0)
        {
            PyErr_Format(PyExc_RuntimeError,
                    "the sip module has already registered a module called %s",
                    full_name);

            return -1;
        }

        /* Only one module can claim to wrap QObject. */
        if (em->em_qt_api != NULL && client->em_qt_api != NULL)
        {
            PyErr_Format(PyExc_RuntimeError,
                    "the %s and %s modules both wrap the QObject class",
                    full_name, sipNameOfModule(em));

            return -1;
        }
    }

    /* Convert the module name to an object. */
#if PY_MAJOR_VERSION >= 3
    client->em_nameobj = PyUnicode_FromString(full_name);
#else
    client->em_nameobj = PyString_FromString(full_name);
#endif

    if (client->em_nameobj == NULL)
        return -1;

    /* Add it to the list of client modules. */
    client->em_next = moduleList;
    moduleList = client;

    return 0;
}


/*
 * Initialise the contents of a client module.  By this time anything that
 * this depends on should have been initialised.  A negative value is returned
 * and an exception raised if there was an error.
 */
static int sip_api_init_module(sipExportedModuleDef *client,
        PyObject *mod_dict)
{
    sipExportedModuleDef *em;
    sipEnumMemberDef *emd;
    int i;

    /* Handle any API. */
    if (sipInitAPI(client, mod_dict) < 0)
        return -1;

    /* Create the module's types. */
    for (i = 0; i < client->em_nrtypes; ++i)
    {
        sipTypeDef *td = client->em_types[i];

        /* Skip external classes. */
        if (td == NULL)
             continue;

        /* Skip if already initialised. */
        if (td->td_module != NULL)
            continue;

        /* If it is a stub then just set the module so we can get its name. */
        if (sipTypeIsStub(td))
        {
            td->td_module = client;
            continue;
        }

        if (sipTypeIsEnum(td))
        {
            sipEnumTypeDef *etd = (sipEnumTypeDef *)td;

            if (td->td_version < 0 || sipIsRangeEnabled(client, td->td_version))
                if (createEnumType(client, etd, mod_dict) < 0)
                    return -1;

            /*
             * Register the enum pickler for scoped enums (unscoped, ie. those
             * not nested, don't need special treatment).
             */
            if (etd->etd_scope >= 0)
            {
                static PyMethodDef md = {
                    "_pickle_enum", pickle_enum, METH_NOARGS, NULL
                };

                if (setReduce(sipTypeAsPyTypeObject(td), &md) < 0)
                    return -1;
            }
        }
        else if (sipTypeIsMapped(td))
        {
            sipMappedTypeDef *mtd = (sipMappedTypeDef *)td;

            /* If there is a name then we need a namespace. */
            if (mtd->mtd_container.cod_name >= 0)
            {
                if (createMappedType(client, mtd, mod_dict) < 0)
                    return -1;
            }
            else
            {
                td->td_module = client;
            }
        }
        else
        {
            sipClassTypeDef *ctd = (sipClassTypeDef *)td;

            /* See if this is a namespace extender. */
            if (ctd->ctd_container.cod_name < 0)
            {
                sipTypeDef *real_nspace;
                sipClassTypeDef **last;

                ctd->ctd_base.td_module = client;

                real_nspace = getGeneratedType(&ctd->ctd_container.cod_scope,
                        client);

                /* Append this type to the real one. */
                last = &((sipClassTypeDef *)real_nspace)->ctd_nsextender;

                while (*last != NULL)
                    last = &(*last)->ctd_nsextender;

                *last = ctd;

                /*
                 * Save the real namespace type so that it is the correct scope
                 * for any enums or classes defined in this module.
                 */
                client->em_types[i] = real_nspace;
            }
            else if (createClassType(client, ctd, mod_dict) < 0)
                return -1;
        }
    }

    /* Set any Qt support API. */
    if (client->em_qt_api != NULL)
    {
        sipQtSupport = client->em_qt_api;
        sipQObjectType = *sipQtSupport->qt_qobject;
    }

    /* Append any initialiser extenders to the relevant classes. */
    if (client->em_initextend != NULL)
    {
        sipInitExtenderDef *ie = client->em_initextend;

        while (ie->ie_extender != NULL)
        {
            sipTypeDef *td = getGeneratedType(&ie->ie_class, client);
            int enabled;

            if (ie->ie_api_range < 0)
                enabled = TRUE;
            else
                enabled = sipIsRangeEnabled(td->td_module, ie->ie_api_range);

            if (enabled)
            {
                sipWrapperType *wt = (sipWrapperType *)sipTypeAsPyTypeObject(td);

                ie->ie_next = wt->iextend;
                wt->iextend = ie;
            }

            ++ie;
        }
    }

    /* Set the base class object for any sub-class convertors. */
    if (client->em_convertors != NULL)
    {
        sipSubClassConvertorDef *scc = client->em_convertors;

        while (scc->scc_convertor != NULL)
        {
            scc->scc_basetype = getGeneratedType(&scc->scc_base, client);

            ++scc;
        }
    }

    /* Create the module's enum members. */
    for (emd = client->em_enummembers, i = 0; i < client->em_nrenummembers; ++i, ++emd)
    {
        PyObject *mo;

        if ((mo = sip_api_convert_from_enum(emd->em_val, client->em_types[emd->em_enum])) == NULL)
            return -1;

        if (PyDict_SetItemString(mod_dict, emd->em_name, mo) < 0)
            return -1;

        Py_DECREF(mo);
    }


    /*
     * Add any class static instances.  We need to do this once all types are
     * fully formed because of potential interdependencies.
     */
    for (i = 0; i < client->em_nrtypes; ++i)
    {
        sipTypeDef *td = client->em_types[i];

        if (td != NULL && !sipTypeIsStub(td) && sipTypeIsClass(td))
            if (addInstances((sipTypeAsPyTypeObject(td))->tp_dict, &((sipClassTypeDef *)td)->ctd_container.cod_instances) < 0)
                return -1;
    }

    /* Add any global static instances. */
    if (addInstances(mod_dict, &client->em_instances) < 0)
        return -1;

    /* Add any license. */
    if (client->em_license != NULL && addLicense(mod_dict, client->em_license) < 0)
        return -1;

    /* See if the new module satisfies any outstanding external types. */
    for (em = moduleList; em != NULL; em = em->em_next)
    {
        sipExternalTypeDef *etd;

        if (em == client || em->em_external == NULL)
            continue;

        for (etd = em->em_external; etd->et_nr >= 0; ++etd)
        {
            if (etd->et_name == NULL)
                continue;

            for (i = 0; i < client->em_nrtypes; ++i)
            {
                sipTypeDef *td = client->em_types[i];

                if (td != NULL && !sipTypeIsStub(td) && sipTypeIsClass(td))
                {
                    const char *pyname = sipPyNameOfContainer(
                            &((sipClassTypeDef *)td)->ctd_container, td);

                    if (strcmp(etd->et_name, pyname) == 0)
                    {
                        em->em_types[etd->et_nr] = td;
                        etd->et_name = NULL;

                        break;
                    }
                }
            }
        }
    }

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
    for (em = moduleList; em != NULL; em = em->em_next)
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
    moduleList = NULL;
}


/*
 * Register the given Python type.
 */
static int sip_api_register_py_type(PyTypeObject *type)
{
    return addPyObjectToList(&sipRegisteredPyTypes, (PyObject *)type);
}


/*
 * Find the registered type with the given name.  Raise an exception if it
 * couldn't be found.
 */
static PyObject *findPyType(const char *name)
{
    sipPyObject *po;

    for (po = sipRegisteredPyTypes; po != NULL; po = po->next)
    {
        PyObject *type = po->object;

        if (strcmp(((PyTypeObject *)type)->tp_name, name) == 0)
            return type;
    }

    PyErr_Format(PyExc_RuntimeError, "%s is not a registered type", name);

    return NULL;
}


/*
 * Add a wrapped C/C++ pointer to the list of delayed dtors.
 */
static void sip_api_add_delayed_dtor(sipSimpleWrapper *sw)
{
    void *ptr;
    const sipClassTypeDef *ctd;
    sipExportedModuleDef *em;

    if ((ptr = getPtrTypeDef(sw, &ctd)) == NULL)
        return;

    /* Find the defining module. */
    for (em = moduleList; em != NULL; em = em->em_next)
    {
        int i;

        for (i = 0; i < em->em_nrtypes; ++i)
            if (em->em_types[i] == (const sipTypeDef *)ctd)
            {
                sipDelayedDtor *dd;

                if ((dd = sip_api_malloc(sizeof (sipDelayedDtor))) == NULL)
                    return;

                /* Add to the list. */
                dd->dd_ptr = ptr;
                dd->dd_name = sipPyNameOfContainer(&ctd->ctd_container,
                        (sipTypeDef *)ctd);
                dd->dd_isderived = sipIsDerived(sw);
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
        sipPySlotType st, const sipTypeDef *td, PyObject *arg0,
        PyObject *arg1)
{
    sipExportedModuleDef *em;

    /* Go through each module. */
    for (em = moduleList; em != NULL; em = em->em_next)
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
            if (td != NULL && td != getGeneratedType(&ex->pse_class, NULL))
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
     * The format string has already been checked that it is properly formed if
     * it is enclosed in parenthesis.
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
        case 'g':
            {
                char *s;
                SIP_SSIZE_T l;

                s = va_arg(va, char *);
                l = va_arg(va, SIP_SSIZE_T);

                if (s != NULL)
#if PY_MAJOR_VERSION >= 3
                    el = PyBytes_FromStringAndSize(s, l);
#else
                    el = PyString_FromStringAndSize(s, l);
#endif
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'G':
#if defined(HAVE_WCHAR_H)
            {
                wchar_t *s;
                SIP_SSIZE_T l;

                s = va_arg(va, wchar_t *);
                l = va_arg(va, SIP_SSIZE_T);

                if (s != NULL)
                    el = PyUnicode_FromWideChar(s, l);
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }
#else
            raiseNoWChar();
            el = NULL;
#endif

            break;

        case 'b':
            el = PyBool_FromLong(va_arg(va,int));
            break;

        case 'c':
            {
                char c = va_arg(va, int);

#if PY_MAJOR_VERSION >= 3
                el = PyBytes_FromStringAndSize(&c, 1);
#else
                el = PyString_FromStringAndSize(&c, 1);
#endif
            }

            break;

        case 'a':
            {
                char c = va_arg(va, int);

#if PY_MAJOR_VERSION >= 3
                el = PyUnicode_FromStringAndSize(&c, 1);
#else
                el = PyString_FromStringAndSize(&c, 1);
#endif
            }

            break;

        case 'w':
#if defined(HAVE_WCHAR_H)
            {
                wchar_t c = va_arg(va, int);

                el = PyUnicode_FromWideChar(&c, 1);
            }
#else
            raiseNoWChar();
            el = NULL;
#endif

            break;

        case 'E':
            {
                /* This is deprecated. */

                int ev = va_arg(va, int);
                PyTypeObject *et = va_arg(va, PyTypeObject *);

                el = sip_api_convert_from_enum(ev,
                        ((const sipEnumTypeObject *)et)->type);
            }

            break;

        case 'F':
            {
                int ev = va_arg(va, int);
                const sipTypeDef *td = va_arg(va, const sipTypeDef *);

                el = sip_api_convert_from_enum(ev, td);
            }

            break;

        case 'd':
        case 'f':
            el = PyFloat_FromDouble(va_arg(va,double));
            break;

        case 'e':
        case 'h':
        case 'i':
#if PY_MAJOR_VERSION >= 3
            el = PyLong_FromLong(va_arg(va, int));
#else
            el = PyInt_FromLong(va_arg(va, int));
#endif
            break;

        case 'l':
            el = PyLong_FromLong(va_arg(va, long));
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
                char *s = va_arg(va, char *);

                if (s != NULL)
#if PY_MAJOR_VERSION >= 3
                    el = PyBytes_FromString(s);
#else
                    el = PyString_FromString(s);
#endif
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
#if PY_MAJOR_VERSION >= 3
                    el = PyUnicode_FromString(s);
#else
                    el = PyString_FromString(s);
#endif
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }

            break;

        case 'x':
#if defined(HAVE_WCHAR_H)
            {
                wchar_t *s = va_arg(va, wchar_t *);

                if (s != NULL)
                    el = PyUnicode_FromWideChar(s, (SIP_SSIZE_T)wcslen(s));
                else
                {
                    Py_INCREF(Py_None);
                    el = Py_None;
                }
            }
#else
            raiseNoWChar();
            el = NULL;
#endif

            break;

        case 't':
        case 'u':
            el = PyLong_FromUnsignedLong(va_arg(va, unsigned));
            break;

        case 'B':
            {
                /* This is deprecated. */

                void *p = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va, sipWrapperType *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_new_type(p, wt->type, xfer);
            }

            break;

        case 'N':
            {
                void *p = va_arg(va, void *);
                const sipTypeDef *td = va_arg(va, const sipTypeDef *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_new_type(p, td, xfer);
            }

            break;

        case 'C':
            {
                /* This is deprecated. */

                void *p = va_arg(va,void *);
                sipWrapperType *wt = va_arg(va, sipWrapperType *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_type(p, wt->type, xfer);
            }

            break;

        case 'D':
            {
                void *p = va_arg(va, void *);
                const sipTypeDef *td = va_arg(va, const sipTypeDef *);
                PyObject *xfer = va_arg(va, PyObject *);

                el = sip_api_convert_from_type(p, td, xfer);
            }

            break;

        case 'r':
            {
                void *p = va_arg(va, void *);
                SIP_SSIZE_T l = va_arg(va, SIP_SSIZE_T);
                const sipTypeDef *td = va_arg(va, const sipTypeDef *);

                el = convertToSequence(p, l, td);
            }

            break;

        case 'R':
            el = va_arg(va,PyObject *);
            break;

        case 'S':
            el = va_arg(va,PyObject *);
            Py_INCREF(el);
            break;

        case 'V':
            el = sip_api_convert_from_void_ptr(va_arg(va, void *));
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
    sipSimpleWrapper *self = NULL;
    va_list va;

    va_start(va,fmt);

    /* Get self if it is provided. */
    if (*fmt == 'S')
    {
        self = va_arg(va, sipSimpleWrapper *);
        ++fmt;
    }

    /* Basic validation of the format string. */
    if (*fmt == '(')
    {
        char ch;
        const char *cp = ++fmt;

        tupsz = 0;

        while ((ch = *cp++) != ')')
        {
            if (ch == '\0')
            {
                PyErr_Format(PyExc_SystemError, "sipParseResult(): invalid format string \"%s\"", fmt - 1);
                rc = -1;

                break;
            }

            /*
             * Some format characters have a sub-format so skip the character
             * and count the sub-format character next time round.
             */
            if (strchr("CD", ch) == NULL)
                ++tupsz;
        }

        if (rc == 0)
            if (!PyTuple_Check(res) || PyTuple_GET_SIZE(res) != tupsz)
            {
                sip_api_bad_catcher_result(method);
                rc = -1;
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
            case 'g':
                {
                    const char **p = va_arg(va, const char **);
                    SIP_SSIZE_T *szp = va_arg(va, SIP_SSIZE_T *);

                    if (parseBytes_AsCharArray(arg, p, szp) < 0)
                        invalid = TRUE;
                }

                break;

            case 'G':
#if defined(HAVE_WCHAR_H)
                {
                    wchar_t **p = va_arg(va, wchar_t **);
                    SIP_SSIZE_T *szp = va_arg(va, SIP_SSIZE_T *);

                    if (parseWCharArray(arg, p, szp) < 0)
                        invalid = TRUE;
                }
#else
                raiseNoWChar();
                invalid = TRUE;
#endif

                break;

            case 'b':
                {
#if PY_MAJOR_VERSION >= 3
                    int v = PyLong_AsLong(arg);
#else
                    int v = PyInt_AsLong(arg);
#endif

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        sipSetBool(va_arg(va, void *), v);
                }

                break;

            case 'c':
                {
                    char *p = va_arg(va, char *);

                    if (parseBytes_AsChar(arg, p) < 0)
                        invalid = TRUE;
                }

                break;

            case 'a':
                {
                    char *p = va_arg(va, char *);
                    int enc;

                    switch (*fmt++)
                    {
                    case 'A':
                        enc = parseString_AsASCIIChar(arg, p);
                        break;

                    case 'L':
                        enc = parseString_AsLatin1Char(arg, p);
                        break;

                    case '8':
                        enc = parseString_AsUTF8Char(arg, p);
                        break;

                    default:
                        enc = -1;
                    }

                    if (enc < 0)
                        invalid = TRUE;
                }

                break;

            case 'w':
#if defined(HAVE_WCHAR_H)
                {
                    wchar_t *p = va_arg(va, wchar_t *);

                    if (parseWChar(arg, p) < 0)
                        invalid = TRUE;
                }
#else
                raiseNoWChar();
                invalid = TRUE;
#endif

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

            case 'E':
                {
                    /* This is deprecated. */

                    PyTypeObject *et = va_arg(va, PyTypeObject *);
                    int *p = va_arg(va, int *);

                    if (sip_api_can_convert_to_enum(arg, ((sipEnumTypeObject *)et)->type))
#if PY_MAJOR_VERSION >= 3
                        *p = PyLong_AsLong(arg);
#else
                        *p = PyInt_AsLong(arg);
#endif
                    else
                        invalid = TRUE;
                }

                break;

            case 'F':
                {
                    sipTypeDef *td = va_arg(va, sipTypeDef *);
                    int *p = va_arg(va, int *);

                    if (sip_api_can_convert_to_enum(arg, td))
#if PY_MAJOR_VERSION >= 3
                        *p = PyLong_AsLong(arg);
#else
                        *p = PyInt_AsLong(arg);
#endif
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
#if PY_MAJOR_VERSION >= 3
                    short v = PyLong_AsLong(arg);
#else
                    short v = PyInt_AsLong(arg);
#endif

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va, short *) = v;
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

            case 'e':
            case 'i':
                {
#if PY_MAJOR_VERSION >= 3
                    int v = PyLong_AsLong(arg);
#else
                    int v = PyInt_AsLong(arg);
#endif

                    if (PyErr_Occurred())
                        invalid = TRUE;
                    else
                        *va_arg(va, int *) = v;
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
                    /* This is deprecated. */

                    const char **p = va_arg(va, const char **);

                    if (parseBytes_AsString(arg, p) < 0)
                        invalid = TRUE;
                }

                break;

            case 'A':
                {
                    int key = va_arg(va, int);
                    const char **p = va_arg(va, const char **);
                    PyObject *keep;

                    switch (*fmt++)
                    {
                    case 'A':
                        keep = parseString_AsASCIIString(arg, p);
                        break;

                    case 'L':
                        keep = parseString_AsLatin1String(arg, p);
                        break;

                    case '8':
                        keep = parseString_AsUTF8String(arg, p);
                        break;

                    default:
                        keep = NULL;
                    }

                    if (keep == NULL)
                        invalid = TRUE;
                    else
                        sip_api_keep_reference((PyObject *)self, key, keep);
                }

                break;

            case 'B':
                {
                    int key = va_arg(va, int);
                    const char **p = va_arg(va, const char **);

                    if (parseBytes_AsString(arg, p) < 0)
                        invalid = TRUE;
                    else
                    {
                        Py_INCREF(arg);
                        sip_api_keep_reference((PyObject *)self, key, arg);
                    }
                }

                break;

            case 'x':
#if defined(HAVE_WCHAR_H)
                {
                    wchar_t **p = va_arg(va, wchar_t **);

                    if (parseWCharString(arg, p) < 0)
                        invalid = TRUE;
                }
#else
                raiseNoWChar();
                invalid = TRUE;
#endif

                break;

            case 'C':
                {
                    /* This is deprecated. */

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

                        *cpp = sip_api_force_convert_to_type(arg, type->type, (flags & FORMAT_FACTORY ? arg : NULL), (flags & FORMAT_DEREF ? SIP_NOT_NONE : 0), state, &iserr);

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
                        const sipTypeDef *td;
                        void **cpp;
                        int *state;

                        td = va_arg(va, const sipTypeDef *);

                        if (flags & FORMAT_NO_STATE)
                            state = NULL;
                        else
                            state = va_arg(va, int *);

                        cpp = va_arg(va, void **);

                        *cpp = sip_api_force_convert_to_type(arg, td, (flags & FORMAT_FACTORY ? arg : NULL), (flags & FORMAT_DEREF ? SIP_NOT_NONE : 0), state, &iserr);

                        if (iserr)
                            invalid = TRUE;
                    }
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
    int no_tmp_tuple, valid, nrargs, selfarg;
    sipSimpleWrapper *self;
    PyObject *single_arg;
    va_list va;

    /* Previous sticky errors stop subsequent parses. */
    if (*argsParsedp & PARSE_STICKY)
        return 0;

    /*
     * See if we are parsing a single argument.  In current versions we are
     * told explicitly by the first character of the format string.  In earlier
     * versions we guessed (sometimes wrongly).
     */
    if (*fmt == '1')
    {
        ++fmt;
        no_tmp_tuple = FALSE;
    }
    else
        no_tmp_tuple = PyTuple_Check(sipArgs);

    if (no_tmp_tuple)
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
     * The first pass checks all the types and does conversions that are cheap
     * and have no side effects.
     */
    va_start(va,fmt);
    valid = parsePass1(&self,&selfarg,&nrargs,sipArgs,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        int pvalid, pnrargs;

        /*
         * Use this error if there was no previous error, or if we have parsed
         * more arguments this time, or if the previous error was that there
         * were too many arguments.
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
     * The second pass does any remaining conversions now that we know we have
     * the right signature.
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
    sipSimpleWrapper *self;
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
     * The first pass checks all the types and does conversions that are cheap
     * and have no side effects.
     */
    va_start(va,fmt);
    valid = parsePass1(&self,&selfarg,&nrargs,args,fmt,va);
    va_end(va);

    if (valid != PARSE_OK)
    {
        int pvalid, pnrargs;

        /*
         * Use this error if there was no previous error, or if we have parsed
         * more arguments this time, or if the previous error was that there
         * were too many arguments.
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
     * The second pass does any remaining conversions now that we know we have
     * the right signature.
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
static int parsePass1(sipSimpleWrapper **selfp, int *selfargp,
        int *argsParsedp, PyObject *sipArgs, const char *fmt, va_list va)
{
    int valid, compulsory, nrargs, argnr, nrparsed;

    valid = PARSE_OK;
    nrargs = *argsParsedp;
    nrparsed = 0;
    compulsory = TRUE;
    argnr = 0;

    /*
     * Handle those format characters that deal with the "self" argument.  They
     * will always be the first one.
     */
    *selfp = NULL;
    *selfargp = FALSE;

    switch (*fmt++)
    {
    case 'B':
    case 'p':
        {
            PyObject *self;
            sipTypeDef *td;

            self = *va_arg(va,PyObject **);
            td = va_arg(va,sipTypeDef *);
            va_arg(va,void **);

            if (self == NULL)
            {
                if ((valid = getSelfFromArgs(td, sipArgs, argnr, selfp)) != PARSE_OK)
                    break;

                *selfargp = TRUE;
                ++nrparsed;
                ++argnr;
            }
            else
                *selfp = (sipSimpleWrapper *)self;

            break;
        }

    case 'C':
        *selfp = (sipSimpleWrapper *)va_arg(va,PyObject *);
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
             * It is an error if we are still expecting compulsory arguments
             * unless the current argument is an ellipsis.
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

        case '@':
            /* Implement /GetWrapper/. */
            *va_arg(va, PyObject **) = arg;

            /* Process the same argument next time round. */
            --argnr;
            --nrparsed;

            break;

        case 's':
            {
                /* String from a Python bytes or None. */

                const char **p = va_arg(va, const char **);

                if (parseBytes_AsString(arg, p) < 0)
                    valid = PARSE_TYPE;

                break;
            }

        case 'A':
            {
                /* String from a Python string or None. */

                PyObject **keep = va_arg(va, PyObject **);
                const char **p = va_arg(va, const char **);
                PyObject *s;
                int enc = 0;

                switch (*fmt++)
                {
                case 'A':
                    s = parseString_AsASCIIString(arg, p);
                    break;

                case 'L':
                    s = parseString_AsLatin1String(arg, p);
                    break;

                case '8':
                    s = parseString_AsUTF8String(arg, p);
                    break;

                default:
                    enc = -1;
                }

                if (enc < 0)
                    valid = PARSE_FORMAT;
                else if (s == NULL)
                    valid = PARSE_TYPE;
                else
                    *keep = s;

                break;
            }

        case 'x':
#if defined(HAVE_WCHAR_H)
            {
                /* Wide string or None. */

                wchar_t **p = va_arg(va, wchar_t **);

                if (parseWCharString(arg, p) < 0)
                    valid = PARSE_TYPE;

                break;
            }
#else
            raiseNoWChar();
            valid = PARSE_RAISED;
            break;
#endif

        case 'U':
            {
                /* Slot name or callable, return the name or callable. */

                char **sname = va_arg(va, char **);
                PyObject **scall = va_arg(va, PyObject **);

                *sname = NULL;
                *scall = NULL;

#if PY_MAJOR_VERSION >= 3
                if (PyBytes_Check(arg))
#else
                if (PyString_Check(arg))
#endif
                {
#if PY_MAJOR_VERSION >= 3
                    char *s = PyBytes_AS_STRING(arg);
#else
                    char *s = PyString_AS_STRING(arg);
#endif

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

#if PY_MAJOR_VERSION >= 3
                if (PyBytes_Check(arg))
#else
                if (PyString_Check(arg))
#endif
                {
#if PY_MAJOR_VERSION >= 3
                    char *s = PyBytes_AS_STRING(arg);
#else
                    char *s = PyString_AS_STRING(arg);
#endif

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

#if PY_MAJOR_VERSION >= 3
                if (PyBytes_Check(arg))
#else
                if (PyString_Check(arg))
#endif
                {
#if PY_MAJOR_VERSION >= 3
                    char *s = PyBytes_AS_STRING(arg);
#else
                    char *s = PyString_AS_STRING(arg);
#endif

                    if (*s == '2' || *s == '9')
                        *va_arg(va,char **) = s;
                    else
                        valid = PARSE_TYPE;
                }
                else
                    valid = PARSE_TYPE;

                break;
            }

        case 'r':
            {
                /* Sequence of class or mapped type instances. */

                const sipTypeDef *td;

                td = va_arg(va, const sipTypeDef *);
                va_arg(va, void **);
                va_arg(va, SIP_SSIZE_T *);

                if (!canConvertFromSequence(arg, td))
                    valid = PARSE_TYPE;

                break;
            }

        case 'J':
            {
                /* Class or mapped type instance. */

                if (*fmt == '\0')
                    valid = PARSE_FORMAT;
                else
                {
                    int flags = *fmt++ - '0';
                    const sipTypeDef *td;
                    int iflgs = 0;

                    td = va_arg(va, const sipTypeDef *);
                    va_arg(va, void **);

                    if (flags & FORMAT_DEREF)
                        iflgs |= SIP_NOT_NONE;

                    if (flags & FORMAT_TRANSFER_THIS)
                        va_arg(va,PyObject **);

                    if (flags & FORMAT_NO_CONVERTORS)
                        iflgs |= SIP_NO_CONVERTORS;
                    else
                        va_arg(va, int *);

                    if (!sip_api_can_convert_to_type(arg, td, iflgs))
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
                /* Python object of any type with a sub-format. */

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

                if (isQObject(arg))
                    *va_arg(va,PyObject **) = arg;
                else
                    valid = PARSE_TYPE;

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

                if (!isQObject(arg))
                    valid = PARSE_TYPE;

                break;
            }

        case 'Q':
            {
                /* Qt receiver to disconnect. */

                va_arg(va,char *);
                va_arg(va,void **);
                va_arg(va,const char **);

                if (!isQObject(arg))
                    valid = PARSE_TYPE;

                break;
            }

        case 'g':
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

        case 'k':
            {
                /* Char array or None. */

                const char **p = va_arg(va, const char **);
                SIP_SSIZE_T *szp = va_arg(va, SIP_SSIZE_T *);

                if (parseBytes_AsCharArray(arg, p, szp) < 0)
                    valid = PARSE_TYPE;

                break;
            }

        case 'K':
#if defined(HAVE_WCHAR_H)
            {
                /* Wide char array or None. */

                wchar_t **p = va_arg(va, wchar_t **);
                SIP_SSIZE_T *szp = va_arg(va, SIP_SSIZE_T *);

                if (parseWCharArray(arg, p, szp) < 0)
                    valid = PARSE_TYPE;

                break;
            }
#else
            raiseNoWChar();
            valid = PARSE_RAISED;
            break
#endif

        case 'c':
            {
                /* Character from a Python bytes. */

                char *p = va_arg(va, char *);

                if (parseBytes_AsChar(arg, p) < 0)
                    valid = PARSE_TYPE;

                break;
            }

        case 'a':
            {
                /* Character from a Python string. */

                char *p = va_arg(va, char *);
                int enc;

                switch (*fmt++)
                {
                case 'A':
                    enc = parseString_AsASCIIChar(arg, p);
                    break;

                case 'L':
                    enc = parseString_AsLatin1Char(arg, p);
                    break;

                case '8':
                    enc = parseString_AsUTF8Char(arg, p);
                    break;

                default:
                    valid = PARSE_FORMAT;
                    enc = 0;
                }

                if (enc < 0)
                    valid = PARSE_TYPE;

                break;
            }

        case 'w':
#if defined(HAVE_WCHAR_H)
            {
                /* Wide character. */

                wchar_t *p = va_arg(va, wchar_t *);

                if (parseWChar(arg, p) < 0)
                    valid = PARSE_TYPE;

                break;
            }
#else
            raiseNoWChar();
            valid = PARSE_RAISED;
            break
#endif

        case 'b':
            {
                /* Bool. */

#if PY_MAJOR_VERSION >= 3
                int v = PyLong_AsLong(arg);
#else
                int v = PyInt_AsLong(arg);
#endif

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    sipSetBool(va_arg(va, void *), v);

                break;
            }

        case 'E':
            {
                /* Named enum or integer. */

                sipTypeDef *td = va_arg(va, sipTypeDef *);

                va_arg(va, int *);

                if (!sip_api_can_convert_to_enum(arg, td))
                    valid = PARSE_TYPE;
            }

            break;

        case 'e':
        case 'i':
            {
                /* Integer or anonymous enum. */

#if PY_MAJOR_VERSION >= 3
                int v = PyLong_AsLong(arg);
#else
                int v = PyInt_AsLong(arg);
#endif

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va, int *) = v;

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

#if PY_MAJOR_VERSION >= 3
                short v = PyLong_AsLong(arg);
#else
                short v = PyInt_AsLong(arg);
#endif

                if (PyErr_Occurred())
                    valid = PARSE_TYPE;
                else
                    *va_arg(va, short *) = v;

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
                /* Constrained types. */

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

#if PY_MAJOR_VERSION >= 3
                        if (PyLong_Check(arg))
                            *va_arg(va, int *) = PyLong_AS_LONG(arg);
#else
                        if (PyInt_Check(arg))
                            *va_arg(va, int *) = PyInt_AS_LONG(arg);
#endif
                        else
                            valid = PARSE_TYPE;

                        break;
                    }

                case 'E':
                    {
                        /* Named enum. */

                        sipTypeDef *td = va_arg(va, sipTypeDef *);

                        va_arg(va, int *);

                        if (!PyObject_TypeCheck(arg, sipTypeAsPyTypeObject(td)))
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
                /* An ellipsis matches everything and ends the parse. */
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
static int parsePass2(sipSimpleWrapper *self, int selfarg, int nrargs,
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
             * The address of a C++ instance when calling one of its public
             * methods.
             */

            const sipTypeDef *td;
            void **p;

            *va_arg(va, PyObject **) = (PyObject *)self;
            td = va_arg(va, const sipTypeDef *);
            p = va_arg(va, void **);

            if ((*p = sip_api_get_cpp_ptr(self, td)) == NULL)
                valid = PARSE_RAISED;

            break;
        }

    case 'p':
        {
            /*
             * The address of a C++ instance when calling one of its protected
             * methods.
             */

            const sipTypeDef *td;
            void **p;

            *va_arg(va, PyObject **) = (PyObject *)self;
            td = va_arg(va, const sipTypeDef *);
            p = va_arg(va, void **);

            if ((*p = getComplexCppPtr(self, td)) == NULL)
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
         * Do the outstanding conversions.  For most types it has already been
         * done, so we are just skipping the parameters.
         */
        switch (ch)
        {
        case '@':
            /* Implement /GetWrapper/. */
            va_arg(va, PyObject **);

            /* Process the same argument next time round. */
            --a;

            break;

        case 'q':
            {
                /* Qt receiver to connect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                if ((*rx = sip_api_convert_rx((sipWrapper *)self, sig, arg, *slot, slot, 0)) == NULL)
                    valid = PARSE_RAISED;

                break;
            }

        case 'Q':
            {
                /* Qt receiver to disconnect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                *rx = sipGetRx(self, sig, arg, *slot, slot);
                break;
            }

        case 'g':
            {
                /* Python single shot slot to connect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                if ((*rx = sip_api_convert_rx((sipWrapper *)self, sig, arg, NULL, slot, SIP_SINGLE_SHOT)) == NULL)
                    valid = PARSE_RAISED;

                break;
            }

        case 'y':
            {
                /* Python slot to connect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                if ((*rx = sip_api_convert_rx((sipWrapper *)self, sig, arg, NULL, slot, 0)) == NULL)
                    valid = PARSE_RAISED;

                break;
            }

        case 'Y':
            {
                /* Python slot to disconnect. */

                char *sig = va_arg(va,char *);
                void **rx = va_arg(va,void **);
                const char **slot = va_arg(va,const char **);

                *rx = sipGetRx(self, sig, arg, NULL, slot);
                break;
            }

        case 'r':
            {
                /* Sequence of class or mapped type instances. */

                const sipTypeDef *td;
                void **array;
                SIP_SSIZE_T *nr_elem;

                td = va_arg(va, const sipTypeDef *);
                array = va_arg(va, void **);
                nr_elem = va_arg(va, SIP_SSIZE_T *);

                if (!convertFromSequence(arg, td, array, nr_elem))
                    valid = PARSE_RAISED;

                break;
            }

        case 'J':
            {
                /* Class or mapped type instance. */

                int flags = *fmt++ - '0';
                const sipTypeDef *td;
                void **p;
                int iflgs = 0;
                int iserr = FALSE;
                int *state;
                PyObject *xfer, **owner;

                td = va_arg(va, const sipTypeDef *);
                p = va_arg(va, void **);

                if (flags & FORMAT_TRANSFER)
                    xfer = (self ? (PyObject *)self : arg);
                else if (flags & FORMAT_TRANSFER_BACK)
                    xfer = Py_None;
                else
                    xfer = NULL;

                if (flags & FORMAT_DEREF)
                    iflgs |= SIP_NOT_NONE;

                if (flags & FORMAT_TRANSFER_THIS)
                    owner = va_arg(va, PyObject **);

                if (flags & FORMAT_NO_CONVERTORS)
                {
                    iflgs |= SIP_NO_CONVERTORS;
                    state = NULL;
                }
                else
                    state = va_arg(va, int *);

                *p = sip_api_convert_to_type(arg, td, xfer, iflgs, state, &iserr);

                if (iserr)
                    valid = PARSE_RAISED;

                if (flags & FORMAT_TRANSFER_THIS && *p != NULL)
                    *owner = arg;

                break;
            }

        case 'P':
            {
                /* Python object of any type with a sub-format. */

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
                /* Constrained types. */

                switch (*fmt++)
                {
                case 'E':
                    {
                        /* Named enum. */

                        int *p;

                        va_arg(va, sipTypeDef *);
                        p = va_arg(va, int *);

#if PY_MAJOR_VERSION >= 3
                        *p = PyLong_AsLong(arg);
#else
                        *p = PyInt_AsLong(arg);
#endif

                        break;
                    }

                default:
                    va_arg(va,void *);
                }

                break;
            }


        case 'E':
            {
                /* Named enum. */

                int *p;

                va_arg(va, sipTypeDef *);
                p = va_arg(va, int *);

#if PY_MAJOR_VERSION >= 3
                *p = PyLong_AsLong(arg);
#else
                *p = PyInt_AsLong(arg);
#endif

                break;
            }

        /*
         * These need special handling because they have a sub-format
         * character.
         */
        case 'A':
            va_arg(va, void *);

            /* Drop through. */

        case 'a':
            va_arg(va, void *);
            fmt++;
            break;

        /*
         * Every other argument is a pointer and only differ in how many there
         * are.
         */
        case 'N':
        case 'T':
        case 'k':
        case 'K':
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
 * Return TRUE if an object is a QObject.
 */
static int isQObject(PyObject *obj)
{
    return (sipQtSupport != NULL && PyObject_TypeCheck(obj, sipTypeAsPyTypeObject(sipQObjectType)));
}


/*
 * See if a Python object is a sequence of a particular type.
 */
static int canConvertFromSequence(PyObject *seq, const sipTypeDef *td)
{
    SIP_SSIZE_T i, size = PySequence_Size(seq);

    if (size < 0)
        return FALSE;

    for (i = 0; i < size; ++i)
    {
        int ok;
        PyObject *val_obj;

        if ((val_obj = PySequence_GetItem(seq, i)) == NULL)
            return FALSE;

        ok = sip_api_can_convert_to_type(val_obj, td,
                SIP_NO_CONVERTORS|SIP_NOT_NONE);

        Py_DECREF(val_obj);

        if (!ok)
            return FALSE;
    }

    return TRUE;
}


/*
 * Convert a Python sequence to an array that has already "passed"
 * canConvertFromSequence().  Return TRUE if the conversion was successful.
 */
static int convertFromSequence(PyObject *seq, const sipTypeDef *td,
        void **array, SIP_SSIZE_T *nr_elem)
{
    int iserr = 0;
    SIP_SSIZE_T i, size = PySequence_Size(seq);
    sipArrayFunc array_helper;
    sipAssignFunc assign_helper;
    void *array_mem;

    /* Get the type's helpers. */
    if (sipTypeIsMapped(td))
    {
        array_helper = ((const sipMappedTypeDef *)td)->mtd_array;
        assign_helper = ((const sipMappedTypeDef *)td)->mtd_assign;
    }
    else
    {
        array_helper = ((const sipClassTypeDef *)td)->ctd_array;
        assign_helper = ((const sipClassTypeDef *)td)->ctd_assign;
    }

    assert(array_helper != NULL);
    assert(assign_helper != NULL);

    /*
     * Create the memory for the array of values.  Note that this will leak if
     * there is an error.
     */
    array_mem = array_helper(size);

    for (i = 0; i < size; ++i)
    {
        PyObject *val_obj;
        void *val;

        if ((val_obj = PySequence_GetItem(seq, i)) == NULL)
            return FALSE;

        val = sip_api_convert_to_type(val_obj, td, NULL,
                SIP_NO_CONVERTORS|SIP_NOT_NONE, NULL, &iserr);

        Py_DECREF(val_obj);

        if (iserr)
            return FALSE;

        assign_helper(array_mem, i, val);
    }

    *array = array_mem;
    *nr_elem = size;

    return TRUE;
}


/*
 * Convert an array of a type to a Python sequence.
 */
static PyObject *convertToSequence(void *array, SIP_SSIZE_T nr_elem,
        const sipTypeDef *td)
{
    SIP_SSIZE_T i;
    PyObject *seq;
    sipCopyFunc copy_helper;

    /* Get the type's copy helper. */
    if (sipTypeIsMapped(td))
        copy_helper = ((const sipMappedTypeDef *)td)->mtd_copy;
    else
        copy_helper = ((const sipClassTypeDef *)td)->ctd_copy;

    assert(copy_helper != NULL);

    if ((seq = PyTuple_New(nr_elem)) == NULL)
        return NULL;

    for (i = 0; i < nr_elem; ++i)
    {
        void *el = copy_helper(array, i);
        PyObject *el_obj = sip_api_convert_from_new_type(el, td, NULL);

        if (el_obj == NULL)
        {
            release(el, td, 0);
            Py_DECREF(seq);
        }

        PyTuple_SET_ITEM(seq, i, el_obj);
    }

    return seq;
}


/*
 * Carry out actions common to all dtors.
 */
void sip_api_common_dtor(sipSimpleWrapper *sipSelf)
{
    if (sipSelf != NULL && sipInterpreter != NULL)
    {
        PyObject *xtype, *xvalue, *xtb;

        SIP_BLOCK_THREADS

        /* We may be tidying up after an exception so preserve it. */
        PyErr_Fetch(&xtype, &xvalue, &xtb);
        callPyDtor(sipSelf);
        PyErr_Restore(xtype, xvalue, xtb);

        if (!sipNotInMap(sipSelf))
            sipOMRemoveObject(&cppPyMap, sipSelf);

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
        else if (PyObject_TypeCheck((PyObject *)sipSelf, (PyTypeObject *)&sipWrapper_Type))
            removeFromParent((sipWrapper *)sipSelf);

        SIP_UNBLOCK_THREADS
    }
}


/*
 * Call self.__dtor__() if it is implemented.
 */
static void callPyDtor(sipSimpleWrapper *self)
{
    sip_gilstate_t sipGILState;
    char pymc = 0;
    PyObject *meth;

    meth = sip_api_is_py_method(&sipGILState, &pymc, self, NULL, "__dtor__");

    if (meth != NULL)
    {
        PyObject *res = sip_api_call_method(0, meth, "", NULL);

        Py_DECREF(meth);

        /* Discard any result. */
        Py_XDECREF(res);

        /* Handle any error the best we can. */
        if (PyErr_Occurred())
            PyErr_Print();

        SIP_RELEASE_GIL(sipGILState);
    }
}


/*
 * Add a wrapper to it's parent owner.  The wrapper must not currently have a
 * parent and, therefore, no siblings.
 */
static void addToParent(sipWrapper *self, sipWrapper *owner)
{
    if (owner->first_child != NULL)
    {
        self->sibling_next = owner->first_child;
        owner->first_child->sibling_prev = self;
    }

    owner->first_child = self;
    self->parent = owner;

    /*
     * The owner holds a real reference so that the cyclic garbage collector
     * works properly.
     */
    Py_INCREF((sipSimpleWrapper *)self);
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
         * We must do this last, after all the pointers are correct, because
         * this is used by the clear slot.
         */
        Py_DECREF((sipSimpleWrapper *)self);
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
 * Return a tuple of the base classes of a type that has no explicit
 * super-type.
 */
static PyObject *getDefaultBases(void)
{
    static PyObject *default_bases = NULL;

    /* Only do this once. */
    if (default_bases == NULL)
    {
#if PY_VERSION_HEX >= 0x02040000
        default_bases = PyTuple_Pack(1, (PyObject *)&sipWrapper_Type);
#else
        default_bases = Py_BuildValue("(O)", &sipWrapper_Type);
#endif

        if (default_bases == NULL)
            return NULL;
    }

    Py_INCREF(default_bases);

    return default_bases;
}


/*
 * Return the dictionary of a type.
 */
static PyObject *getScopeDict(sipTypeDef *td, PyObject *mod_dict,
        sipExportedModuleDef *client)
{
    /*
     * Initialise the scoping type if necessary.  It will always be in the
     * same module if it needs doing.
     */
    if (sipTypeIsMapped(td))
    {
        if (createMappedType(client, (sipMappedTypeDef *)td, mod_dict) < 0)
            return NULL;

        /* Check that the mapped type can act as a container. */
        assert(sipTypeAsPyTypeObject(td) != NULL);
    }
    else
    {
        if (createClassType(client, (sipClassTypeDef *)td, mod_dict) < 0)
            return NULL;
    }

    return (sipTypeAsPyTypeObject(td))->tp_dict;
}


/*
 * Create a container type and return a borrowed reference to it.
 */
static PyObject *createContainerType(sipContainerDef *cod, sipTypeDef *td,
        PyObject *bases, PyObject *metatype, PyObject *mod_dict,
        sipExportedModuleDef *client)
{
    PyObject *py_type, *scope_dict, *typedict, *name, *args;

    /* Get the dictionary to place the type in. */
    if (cod->cod_scope.sc_flag)
    {
        scope_dict = mod_dict;
    }
    else if ((scope_dict = getScopeDict(getGeneratedType(&cod->cod_scope, client), mod_dict, client)) == NULL)
        goto reterr;

    /* Create the type dictionary. */
    if ((typedict = createTypeDict(client->em_nameobj)) == NULL)
        goto reterr;

    /* Create an object corresponding to the type name. */
#if PY_MAJOR_VERSION >= 3
    name = PyUnicode_FromString(sipPyNameOfContainer(cod, td));
#else
    name = PyString_FromString(sipPyNameOfContainer(cod, td));
#endif

    if (name == NULL)
        goto reldict;

    /* Create the type by calling the metatype. */
#if PY_VERSION_HEX >= 0x02040000
    args = PyTuple_Pack(3, name, bases, typedict);
#else
    args = Py_BuildValue("OOO", name, bases, typedict);
#endif

    if (args == NULL)
        goto relname;

    /* Pass the type via the back door. */
    currentType = td;

    if ((py_type = PyObject_Call(metatype, args, NULL)) == NULL)
        goto relargs;

    /* Add the type to the "parent" dictionary. */
    if (PyDict_SetItem(scope_dict, name, py_type) < 0)
        goto reltype;

    Py_DECREF(args);
    Py_DECREF(name);
    Py_DECREF(typedict);

    return py_type;

    /* Unwind on error. */

reltype:
    Py_DECREF(py_type);

relargs:
    Py_DECREF(args);

relname:
    Py_DECREF(name);

reldict:
    Py_DECREF(typedict);

reterr:
    currentType = NULL;
    return NULL;
}


/*
 * Create a single class type object.
 */
static int createClassType(sipExportedModuleDef *client, sipClassTypeDef *ctd,
        PyObject *mod_dict)
{
    PyObject *bases, *metatype, *py_type;
    sipEncodedTypeDef *sup;

    /* Handle the trivial case where we have already been initialised. */
    if (ctd->ctd_base.td_module != NULL)
        return 0;

    /* Set this up now to gain access to the string pool. */
    ctd->ctd_base.td_module = client;

    /* Create the tuple of super-types. */
    if ((sup = ctd->ctd_supers) == NULL)
    {
        if (ctd->ctd_supertype < 0)
        {
            bases = getDefaultBases();
        }
        else
        {
            PyObject *supertype;
            const char *supertype_name = sipNameFromPool(client,
                    ctd->ctd_supertype);

            if ((supertype = findPyType(supertype_name)) == NULL)
                goto reterr;

#if PY_VERSION_HEX >= 0x02040000
            bases = PyTuple_Pack(1, supertype);
#else
            bases = Py_BuildValue("(O)", supertype);
#endif
        }

        if (bases == NULL)
            goto reterr;
    }
    else
    {
        int i, nrsupers = 0;

        do
            ++nrsupers;
        while (!sup++->sc_flag);

        if ((bases = PyTuple_New(nrsupers)) == NULL)
            goto reterr;

        for (sup = ctd->ctd_supers, i = 0; i < nrsupers; ++i, ++sup)
        {
            PyObject *st;
            sipTypeDef *sup_td = getGeneratedType(sup, client);

            /*
             * Initialise the super-class if necessary.  It will always be in
             * the same module if it needs doing.
             */
            if (createClassType(client, (sipClassTypeDef *)sup_td, mod_dict) < 0)
                goto relbases;

            st = (PyObject *)sipTypeAsPyTypeObject(sup_td);

            Py_INCREF(st);
            PyTuple_SET_ITEM(bases, i, st);
        }
    }

    /*
     * Use the explicit meta-type if there is one, otherwise use the meta-type
     * of the first super-type.
     */
    if (ctd->ctd_metatype >= 0)
    {
        const char *metatype_name = sipNameFromPool(client, ctd->ctd_metatype);

        if ((metatype = findPyType(metatype_name)) == NULL)
            goto relbases;
    }
    else
        metatype = (PyObject *)Py_TYPE(PyTuple_GET_ITEM(bases, 0));

    if ((py_type = createContainerType(&ctd->ctd_container, (sipTypeDef *)ctd, bases, metatype, mod_dict, client)) == NULL)
        goto relbases;

    /* Handle the pickle function. */
    if (ctd->ctd_pickle != NULL)
    {
        static PyMethodDef md = {
            "_pickle_type", pickle_type, METH_NOARGS, NULL
        };

        if (setReduce((PyTypeObject *)py_type, &md) < 0)
            goto reltype;
    }

    /* We can now release our references. */
    Py_DECREF(bases);

    return 0;

    /* Unwind after an error. */

reltype:
    Py_DECREF(py_type);

relbases:
    Py_DECREF(bases);

reterr:
    ctd->ctd_base.td_module = NULL;
    return -1;
}


/*
 * Create a single mapped type object.
 */
static int createMappedType(sipExportedModuleDef *client,
        sipMappedTypeDef *mtd, PyObject *mod_dict)
{
    PyObject *bases;

    /* Handle the trivial case where we have already been initialised. */
    if (mtd->mtd_base.td_module != NULL)
        return 0;

    /* Set this up now to gain access to the string pool. */
    mtd->mtd_base.td_module = client;

    /* Create the tuple of super-types. */
    if ((bases = getDefaultBases()) == NULL)
        goto reterr;

    if (createContainerType(&mtd->mtd_container, (sipTypeDef *)mtd, bases, (PyObject *)&sipWrapperType_Type, mod_dict, client) == NULL)
        goto relbases;

    /* We can now release our references. */
    Py_DECREF(bases);

    return 0;

    /* Unwind after an error. */

relbases:
    Py_DECREF(bases);

reterr:
    mtd->mtd_base.td_module = NULL;
    return -1;
}


/*
 * Return the module definition for a named module.
 */
static sipExportedModuleDef *getModule(PyObject *mname_obj)
{
    PyObject *mod;
    sipExportedModuleDef *em;

    /* Make sure the module is imported. */
    if ((mod = PyImport_Import(mname_obj)) == NULL)
        return NULL;

    /* Find the module definition. */
    for (em = moduleList; em != NULL; em = em->em_next)
#if PY_MAJOR_VERSION >= 3
        if (PyUnicode_Compare(mname_obj, em->em_nameobj) == 0)
#else
        if (strcmp(PyString_AS_STRING(mname_obj), sipNameOfModule(em)) == 0)
#endif
            break;

    Py_DECREF(mod);

    if (em == NULL)
    {
#if PY_MAJOR_VERSION >= 3
        PyErr_Format(PyExc_SystemError, "unable to find to find module: %U",
                mname_obj);
#else
        PyErr_Format(PyExc_SystemError, "unable to find to find module: %s",
                PyString_AS_STRING(mname_obj));
#endif
    }

    return em;
}


/*
 * The type unpickler.
 */
static PyObject *unpickle_type(PyObject *ignore, PyObject *args)
{
    PyObject *mname_obj, *init_args;
    const char *tname;
    sipExportedModuleDef *em;
    int i;

    if (!PyArg_ParseTuple(args, "SsO!:_unpickle_type", &mname_obj, &tname, &PyTuple_Type, &init_args))
        return NULL;

    /* Get the module definition. */
    if ((em = getModule(mname_obj)) == NULL)
        return NULL;

    /* Find the class type object. */
    for (i = 0; i < em->em_nrtypes; ++i)
    {
        sipTypeDef *td = em->em_types[i];

        if (td != NULL && !sipTypeIsStub(td) && sipTypeIsClass(td))
        {
            const char *pyname = sipPyNameOfContainer(
                    &((sipClassTypeDef *)td)->ctd_container, td);

            if (strcmp(pyname, tname) == 0)
                return PyObject_CallObject((PyObject *)sipTypeAsPyTypeObject(td), init_args);
        }
    }

    PyErr_Format(PyExc_SystemError, "unable to find to find type: %s", tname);

    return NULL;
}


/*
 * The type pickler.
 */
static PyObject *pickle_type(PyObject *obj, PyObject *ignore)
{
    sipExportedModuleDef *em;

    /* Find the type definition and defining module. */
    for (em = moduleList; em != NULL; em = em->em_next)
    {
        int i;

        for (i = 0; i < em->em_nrtypes; ++i)
        {
            sipTypeDef *td = em->em_types[i];

            if (td != NULL && !sipTypeIsStub(td) && sipTypeIsClass(td))
                if (sipTypeAsPyTypeObject(td) == Py_TYPE(obj))
                {
                    PyObject *init_args;
                    sipClassTypeDef *ctd = (sipClassTypeDef *)td;
                    const char *pyname = sipPyNameOfContainer(&ctd->ctd_container, td);

                    /*
                     * Ask the handwritten pickle code for the tuple of
                     * arguments that will recreate the object.
                     */
                    init_args = ctd->ctd_pickle(sip_api_get_cpp_ptr((sipSimpleWrapper *)obj, NULL));

                    if (!PyTuple_Check(init_args))
                    {
                        PyErr_Format(PyExc_TypeError,
                                "%%PickleCode for type %s.%s did not return a tuple",
                                sipNameOfModule(em), pyname);

                        return NULL;
                    }

                    return Py_BuildValue("O(OsN)", type_unpickler,
                            em->em_nameobj, pyname, init_args);
                }
        }
    }

    /* We should never get here. */
    PyErr_Format(PyExc_SystemError, "attempt to pickle unknown type '%s'",
            Py_TYPE(obj)->tp_name);

    return NULL;
}


/*
 * The enum unpickler.
 */
static PyObject *unpickle_enum(PyObject *ignore, PyObject *args)
{
    PyObject *mname_obj, *evalue_obj;
    const char *ename;
    sipExportedModuleDef *em;
    int i;

    if (!PyArg_ParseTuple(args, "SsO:_unpickle_enum", &mname_obj, &ename, &evalue_obj))
        return NULL;

    /* Get the module definition. */
    if ((em = getModule(mname_obj)) == NULL)
        return NULL;

    /* Find the enum type object. */
    for (i = 0; i < em->em_nrtypes; ++i)
    {
        sipTypeDef *td = em->em_types[i];

        if (td != NULL && !sipTypeIsStub(td) && sipTypeIsEnum(td))
            if (strcmp(sipPyNameOfEnum((sipEnumTypeDef *)td), ename) == 0)
                return PyObject_CallFunctionObjArgs((PyObject *)sipTypeAsPyTypeObject(td), evalue_obj, NULL);
    }

    PyErr_Format(PyExc_SystemError, "unable to find to find enum: %s", ename);

    return NULL;
}


/*
 * The enum pickler.
 */
static PyObject *pickle_enum(PyObject *obj, PyObject *ignore)
{
    sipTypeDef *td = ((sipEnumTypeObject *)Py_TYPE(obj))->type;

    return Py_BuildValue("O(Osi)", enum_unpickler, td->td_module->em_nameobj,
            sipPyNameOfEnum((sipEnumTypeDef *)td),
#if PY_MAJOR_VERSION >= 3
            (int)PyLong_AS_LONG(obj)
#else
            (int)PyInt_AS_LONG(obj)
#endif
            );
}


/*
 * Set the __reduce__method for a type.
 */
static int setReduce(PyTypeObject *type, PyMethodDef *pickler)
{
    static PyObject *rstr = NULL;
    PyObject *descr;
    int rc;

    if (objectify("__reduce__", &rstr) < 0)
        return -1;

    /* Create the method descripter. */
    if ((descr = PyDescr_NewMethod(type, pickler)) == NULL)
        return -1;

    /*
     * Save the method.  Note that we don't use PyObject_SetAttr() as we want
     * to bypass any lazy attribute loading (which may not be safe yet).
     */
    rc = PyType_Type.tp_setattro((PyObject *)type, rstr, descr);

    Py_DECREF(descr);

    return rc;
}


/*
 * Create an enum type object.
 */
static int createEnumType(sipExportedModuleDef *client, sipEnumTypeDef *etd,
        PyObject *mod_dict)
{
    static PyObject *bases = NULL;
    PyObject *name, *typedict, *args, *dict;
    PyTypeObject *py_type;

    etd->etd_base.td_module = client;

    /* Get the dictionary into which the type will be placed. */
    if (etd->etd_scope < 0)
        dict = mod_dict;
    else if ((dict = getScopeDict(client->em_types[etd->etd_scope], mod_dict, client)) == NULL)
        goto reterr;

    /* Create the base type tuple if it hasn't already been done. */
    if (bases == NULL)
    {
#if PY_MAJOR_VERSION >= 3
        bases = PyTuple_Pack(1, (PyObject *)&PyLong_Type);
#elif PY_VERSION_HEX >= 0x02040000
        bases = PyTuple_Pack(1, (PyObject *)&PyInt_Type);
#else
        bases = Py_BuildValue("(O)", &PyInt_Type);
#endif

        if (bases == NULL)
            goto reterr;
    }

    /* Create an object corresponding to the type name. */
#if PY_MAJOR_VERSION >= 3
    name = PyUnicode_FromString(sipPyNameOfEnum(etd));
#else
    name = PyString_FromString(sipPyNameOfEnum(etd));
#endif

    if (name == NULL)
        goto reterr;

    /* Create the type dictionary. */
    if ((typedict = createTypeDict(client->em_nameobj)) == NULL)
        goto relname;

    /* Create the type by calling the metatype. */
#if PY_VERSION_HEX >= 0x02040000
    args = PyTuple_Pack(3, name, bases, typedict);
#else
    args = Py_BuildValue("OOO", name, bases, typedict);
#endif

    Py_DECREF(typedict);

    if (args == NULL)
        goto relname;

    py_type = (PyTypeObject *)PyObject_Call((PyObject *)&sipEnumType_Type, args, NULL);

    Py_DECREF(args);

    if (py_type == NULL)
        goto relname;

    /*
     * Set the links between the Python type object and the generated type
     * structure.
     */
    ((sipEnumTypeObject *)py_type)->type = &etd->etd_base;
    etd->etd_base.u.td_py_type = py_type;

    /* Initialise any slots. */
    if (etd->etd_pyslots != NULL)
        addTypeSlots(py_type, py_type->tp_as_number, py_type->tp_as_sequence,
                py_type->tp_as_mapping, etd->etd_pyslots);

    /* Add the type to the "parent" dictionary. */
    if (PyDict_SetItem(dict, name, (PyObject *)py_type) < 0)
    {
        Py_DECREF((PyObject *)py_type);
        goto relname;
    }

    /* We can now release our remaining references. */
    Py_DECREF(name);

    return 0;

    /* Unwind after an error. */

relname:
    Py_DECREF(name);

reterr:
    etd->etd_base.td_module = client;
    return -1;
}


/*
 * Create a type dictionary for dynamic type being created in the module with
 * the specified name.
 */
static PyObject *createTypeDict(PyObject *mname)
{
    static PyObject *mstr = NULL;
    PyObject *dict;

    if (objectify("__module__", &mstr) < 0)
        return NULL;

    /* Create the dictionary. */
    if ((dict = PyDict_New()) == NULL)
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
 * Convert an ASCII string to a Python object if it hasn't already been done.
 */
static int objectify(const char *s, PyObject **objp)
{
    if (*objp == NULL)
    {
#if PY_MAJOR_VERSION >= 3
        *objp = PyUnicode_FromString(s);
#else
        *objp = PyString_FromString(s);
#endif

        if (*objp == NULL)
            return -1;
    }

    return 0;
}


/*
 * Add a set of static instances to a dictionary.
 */
static int addInstances(PyObject *dict, sipInstancesDef *id)
{
    if (id->id_type != NULL && addTypeInstances(dict, id->id_type) < 0)
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

    return 0;
}


/*
 * Get "self" from the argument tuple for a method called as
 * Class.Method(self, ...) rather than self.Method(...).
 */
static int getSelfFromArgs(sipTypeDef *td, PyObject *args, int argnr,
        sipSimpleWrapper **selfp)
{
    PyObject *self;

    /* Get self from the argument tuple. */

    if (argnr >= PyTuple_GET_SIZE(args))
        return PARSE_UNBOUND;

    self = PyTuple_GET_ITEM(args, argnr);

    if (!PyObject_TypeCheck(self, sipTypeAsPyTypeObject(td)))
        return PARSE_UNBOUND;

    *selfp = (sipSimpleWrapper *)self;

    return PARSE_OK;
}


/*
 * Populate a container's type dictionary.
 */
static int add_lazy_container_attrs(sipTypeDef *td, sipContainerDef *cod,
        PyObject *dict)
{
    int i;
    PyMethodDef *pmd;
    sipEnumMemberDef *enm;
    sipVariableDef *vd;

    /* Do the methods. */
    pmd = cod->cod_methods;

    for (i = 0; i < cod->cod_nrmethods; ++i)
    {
        int rc;
        PyObject *descr;

        if ((descr = sipMethodDescr_New(pmd)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, pmd->ml_name, descr);

        Py_DECREF(descr);

        if (rc < 0)
            return -1;

        ++pmd;
    }

    /* Do the enum members. */
    enm = cod->cod_enummembers;

    for (i = 0; i < cod->cod_nrenummembers; ++i)
    {
        int rc;
        PyObject *val;

        if ((val = createEnumMember(td, enm)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, enm->em_name, val);

        Py_DECREF(val);

        if (rc < 0)
            return -1;

        ++enm;
    }

    /* Do the variables. */
    vd = cod->cod_variables;

    for (i = 0; i < cod->cod_nrvariables; ++i)
    {
        int rc;
        PyObject *descr;

        if ((descr = sipVariableDescr_New(vd, td, cod)) == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, vd->vd_name, descr);

        Py_DECREF(descr);

        if (rc < 0)
            return -1;

        ++vd;
    }

    return 0;
}


/*
 * Populate a type dictionary with all lazy attributes if it hasn't already
 * been done.
 */
static int add_lazy_attrs(sipTypeDef *td)
{
    sipWrapperType *wt = (sipWrapperType *)sipTypeAsPyTypeObject(td);
    PyObject *dict;
    sipAttrGetter *ag;

    /* Handle the trivial case. */
    if (wt->dict_complete)
        return 0;

    dict = ((PyTypeObject *)wt)->tp_dict;

    if (sipTypeIsMapped(td))
    {
        if (add_lazy_container_attrs(td, &((sipMappedTypeDef *)td)->mtd_container, dict) < 0)
            return -1;
    }
    else
    {
        sipClassTypeDef *nsx;

        /* Search the possible linked list of namespace extenders. */
        for (nsx = (sipClassTypeDef *)td; nsx != NULL; nsx = nsx->ctd_nsextender)
            if (add_lazy_container_attrs((sipTypeDef *)nsx, &nsx->ctd_container, dict) < 0)
                return -1;
    }

    /*
     * Get any lazy attributes from registered getters.  This must be done last
     * to allow any existing attributes to be replaced.
     */
    for (ag = sipAttrGetters; ag != NULL; ag = ag->next)
        if (ag->type == NULL || PyType_IsSubtype((PyTypeObject *)wt, ag->type))
            if (ag->getter(td, dict) < 0)
                return -1;

    wt->dict_complete = TRUE;

    return 0;
}


/*
 * Populate the type dictionary and all its super-types.
 */
static int add_all_lazy_attrs(sipTypeDef *td)
{
    if (td == NULL)
        return 0;

    if (add_lazy_attrs(td) < 0)
        return -1;

    if (sipTypeIsClass(td))
    {
        sipClassTypeDef *ctd = (sipClassTypeDef *)td;
        sipEncodedTypeDef *sup;

        if ((sup = ctd->ctd_supers) != NULL)
            do
            {
                sipTypeDef *sup_td = getGeneratedType(sup, td->td_module);

                if (add_all_lazy_attrs(sup_td) < 0)
                    return -1;
            }
            while (!sup++->sc_flag);
    }

    return 0;
}


/*
 * Return the generated type structure corresponding to the given Python type
 * object.
 */
static const sipTypeDef *sip_api_type_from_py_type_object(PyTypeObject *py_type)
{
    if (PyObject_TypeCheck((PyObject *)py_type, &sipWrapperType_Type))
        return ((sipWrapperType *)py_type)->type;

    if (PyObject_TypeCheck((PyObject *)py_type, &sipEnumType_Type))
        return ((sipEnumTypeObject *)py_type)->type;

    return NULL;
}


/*
 * Return the generated type structure corresponding to the scope of the given
 * type.
 */
static const sipTypeDef *sip_api_type_scope(const sipTypeDef *td)
{
    if (sipTypeIsEnum(td))
    {
        const sipEnumTypeDef *etd = (const sipEnumTypeDef *)td;

        if (etd->etd_scope >= 0)
            return td->td_module->em_types[etd->etd_scope];
    }
    else
    {
        const sipContainerDef *cod;

        if (sipTypeIsMapped(td))
            cod = &((const sipMappedTypeDef *)td)->mtd_container;
        else
            cod = &((const sipClassTypeDef *)td)->ctd_container;

        if (!cod->cod_scope.sc_flag)
            return getGeneratedType(&cod->cod_scope, td->td_module);
    }

    return NULL;
}


/*
 * Return TRUE if an object can be converted to a named enum.
 */
static int sip_api_can_convert_to_enum(PyObject *obj, const sipTypeDef *td)
{
    assert(sipTypeIsEnum(td));

    /* If the object is an enum then it must be the right enum. */
    if (PyObject_TypeCheck((PyObject *)Py_TYPE(obj), &sipEnumType_Type))
        return (PyObject_TypeCheck(obj, sipTypeAsPyTypeObject(td)));

#if PY_MAJOR_VERSION >= 3
    return PyLong_Check(obj);
#else
    return PyInt_Check(obj);
#endif
}


/*
 * Create a Python object for an enum member.
 */
static PyObject *createEnumMember(sipTypeDef *td, sipEnumMemberDef *enm)
{
    if (enm->em_enum < 0)
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(enm->em_val);
#else
        return PyInt_FromLong(enm->em_val);
#endif

    return sip_api_convert_from_enum(enm->em_val,
            td->td_module->em_types[enm->em_enum]);
}


/*
 * Create a Python object for a member of a named enum.
 */
static PyObject *sip_api_convert_from_enum(int eval, const sipTypeDef *td)
{
    assert(sipTypeIsEnum(td));

    return PyObject_CallFunction((PyObject *)sipTypeAsPyTypeObject(td), "(i)",
            eval);
}


/*
 * Register a getter for unknown attributes.
 */
static int sip_api_register_attribute_getter(const sipTypeDef *td,
        sipAttrGetterFunc getter)
{
    sipAttrGetter *ag = sip_api_malloc(sizeof (sipAttrGetter));

    if (ag == NULL)
        return -1;

    ag->type = sipTypeAsPyTypeObject(td);
    ag->getter = getter;
    ag->next = sipAttrGetters;

    sipAttrGetters = ag;

    return 0;
}


/*
 * Report a function with invalid argument types.
 */
static void sip_api_no_function(int argsParsed, const char *func)
{
    badArgs(argsParsed, NULL, func);
}


/*
 * Report a method/function/signal with invalid argument types.
 */
static void sip_api_no_method(int argsParsed, const char *classname, const char *method)
{
    badArgs(argsParsed, classname, method);
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
 * Report a deprecated class or method.
 */
static int sip_api_deprecated(const char *classname, const char *method)
{
    char buf[100];

    if (classname == NULL)
        PyOS_snprintf(buf, sizeof (buf), "%s() is deprecated", method);
    else if (method == NULL)
        PyOS_snprintf(buf, sizeof (buf), "%s constructor is deprecated",
                classname);
    else
        PyOS_snprintf(buf, sizeof (buf), "%s.%s() is deprecated", classname,
                method);

#if PY_VERSION_HEX >= 0x02050000
    return PyErr_WarnEx(PyExc_DeprecationWarning, buf, 1);
#else
    return PyErr_Warn(PyExc_DeprecationWarning, buf);
#endif
}


/*
 * Handle error reporting for bad arguments to various things.
 */
static void badArgs(int argsParsed, const char *scope, const char *method)
{
    const char *sep;
    int nrparsed = argsParsed & ~PARSE_MASK;

    if (scope != NULL)
        sep = ".";
    else
    {
        scope = "";
        sep = "";
    }

    switch (argsParsed & PARSE_MASK)
    {
    case PARSE_FEW:
        PyErr_Format(PyExc_TypeError,
                "insufficient number of arguments to %s%s%s()", scope, sep,
                method);
        break;

    case PARSE_MANY:
        PyErr_Format(PyExc_TypeError,
                "too many arguments to %s%s%s(), %d at most expected", scope,
                sep, method, nrparsed);
        break;

    case PARSE_TYPE:
        PyErr_Format(PyExc_TypeError,
                "argument %d of %s%s%s() has an invalid type", nrparsed + 1,
                scope, sep, method);
        break;

    case PARSE_FORMAT:
        PyErr_Format(PyExc_TypeError,
                "invalid format to sipParseArgs() from %s%s%s()", scope,
                sep, method);
        break;

    case PARSE_UNBOUND:
        PyErr_Format(PyExc_TypeError,
                "first argument of unbound method %s%s%s() must be a %s instance",
                scope, sep, method, scope);
        break;

    case PARSE_RAISED:
        /* It has already been taken care of. */
        break;

    case PARSE_OK:
        /* This is raised by a private re-implementation. */
        PyErr_Format(PyExc_AttributeError, "%s%s%s is a private method", scope,
                sep, method);
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
        PyErr_Format(PyExc_TypeError,
                "cannot concatenate '%s' and '%s' objects",
                Py_TYPE(self)->tp_name, Py_TYPE(arg)->tp_name);
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
        PyErr_Format(PyExc_TypeError,
                "unsupported operand type(s) for %s: '%s' and '%s'", sn,
                Py_TYPE(self)->tp_name, Py_TYPE(arg)->tp_name);
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
    PyErr_Format(PyExc_TypeError,
            "cannot convert Python object to an instance of %s", classname);
}


/*
 * Report a Python member function with an unexpected return type.
 */
static void sip_api_bad_catcher_result(PyObject *method)
{
    PyObject *mname;

    /*
     * This is part of the public API so we make no assumptions about the
     * method object.
     */
    if (!PyMethod_Check(method) ||
        PyMethod_GET_FUNCTION(method) == NULL ||
        !PyFunction_Check(PyMethod_GET_FUNCTION(method)) ||
        PyMethod_GET_SELF(method) == NULL)
    {
        PyErr_Format(PyExc_TypeError,
                "invalid argument to sipBadCatcherResult()");
        return;
    }

    mname = ((PyFunctionObject *)PyMethod_GET_FUNCTION(method))->func_name;

#if PY_MAJOR_VERSION >= 3
    PyErr_Format(PyExc_TypeError, "invalid result type from %s.%U()",
            Py_TYPE(PyMethod_GET_SELF(method))->tp_name, mname);
#else
    PyErr_Format(PyExc_TypeError, "invalid result type from %s.%s()",
            Py_TYPE(PyMethod_GET_SELF(method))->tp_name,
            PyString_AsString(mname));
#endif
}


/*
 * Transfer ownership of a class instance to Python from C/C++.
 */
static void sip_api_transfer_back(PyObject *self)
{
    if (self != NULL && PyObject_TypeCheck(self, (PyTypeObject *)&sipWrapper_Type))
    {
        sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

        if (sipCppHasRef(sw))
        {
            sipResetCppHasRef(sw);
            Py_DECREF(sw);
        }
        else
            removeFromParent((sipWrapper *)sw);

        sipSetPyOwned(sw);
    }
}


/*
 * Break the association of a C++ owned Python object with any parent.
 */
static void sip_api_transfer_break(PyObject *self)
{
    if (self != NULL && PyObject_TypeCheck(self, (PyTypeObject *)&sipWrapper_Type))
    {
        sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

        if (sipCppHasRef(sw))
        {
            sipResetCppHasRef(sw);
            Py_DECREF(sw);
        }
        else
            removeFromParent((sipWrapper *)sw);
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
    if (self != NULL &&
            PyObject_TypeCheck(self, (PyTypeObject *)&sipWrapper_Type) &&
            (owner == NULL ||
                    PyObject_TypeCheck(owner, (PyTypeObject *)&sipWrapper_Type)))
    {
        sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

        /*
         * Keep the object alive while we do the transfer.  If C++ has a
         * reference then there is no need to increment it, just reset the flag
         * and the following decrement will bring everything back to the way it
         * should be.
         */
        if (sipCppHasRef(sw))
            sipResetCppHasRef(sw);
        else
        {
            Py_INCREF(sw);
            removeFromParent((sipWrapper *)sw);
        }

        if (owner != NULL)
            addToParent((sipWrapper *)sw, (sipWrapper *)owner);

        Py_DECREF(sw);

        sipResetPyOwned(sw);
    }
}


/*
 * Add a license to a dictionary.
 */
static int addLicense(PyObject *dict,sipLicenseDef *lc)
{
    int rc;
    PyObject *ldict, *proxy, *o;

    /* Convert the strings we use to objects if not already done. */

    if (objectify("__license__", &licenseName) < 0)
        return -1;

    if (objectify("Licensee", &licenseeName) < 0)
        return -1;

    if (objectify("Type", &typeName) < 0)
        return -1;

    if (objectify("Timestamp", &timestampName) < 0)
        return -1;

    if (objectify("Signature", &signatureName) < 0)
        return -1;

    /* We use a dictionary to hold the license information. */
    if ((ldict = PyDict_New()) == NULL)
        return -1;

    /* The license type is compulsory, the rest are optional. */
    if (lc->lc_type == NULL)
        goto deldict;

#if PY_MAJOR_VERSION >= 3
    o = PyUnicode_FromString(lc->lc_type);
#else
    o = PyString_FromString(lc->lc_type);
#endif

    if (o == NULL)
        goto deldict;

    rc = PyDict_SetItem(ldict,typeName,o);
    Py_DECREF(o);

    if (rc < 0)
        goto deldict;

    if (lc->lc_licensee != NULL)
    {
#if PY_MAJOR_VERSION >= 3
        o = PyUnicode_FromString(lc->lc_licensee);
#else
        o = PyString_FromString(lc->lc_licensee);
#endif

        if (o == NULL)
            goto deldict;

        rc = PyDict_SetItem(ldict,licenseeName,o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lc->lc_timestamp != NULL)
    {
#if PY_MAJOR_VERSION >= 3
        o = PyUnicode_FromString(lc->lc_timestamp);
#else
        o = PyString_FromString(lc->lc_timestamp);
#endif

        if (o == NULL)
            goto deldict;

        rc = PyDict_SetItem(ldict,timestampName,o);
        Py_DECREF(o);

        if (rc < 0)
            goto deldict;
    }

    if (lc->lc_signature != NULL)
    {
#if PY_MAJOR_VERSION >= 3
        o = PyUnicode_FromString(lc->lc_signature);
#else
        o = PyString_FromString(lc->lc_signature);
#endif

        if (o == NULL)
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

    rc = PyDict_SetItem(dict, licenseName, proxy);
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
static int addCharInstances(PyObject *dict, sipCharInstanceDef *ci)
{
    while (ci->ci_name != NULL)
    {
        int rc;
        PyObject *w;

        switch (ci->ci_encoding)
        {
        case 'A':
            w = PyUnicode_DecodeASCII(&ci->ci_val, 1, NULL);
            break;

        case 'L':
            w = PyUnicode_DecodeLatin1(&ci->ci_val, 1, NULL);
            break;

        case '8':
#if PY_MAJOR_VERSION >= 3
            w = PyUnicode_FromStringAndSize(&ci->ci_val, 1);
#else
            w = PyUnicode_DecodeUTF8(&ci->ci_val, 1, NULL);
#endif
            break;

        default:
#if PY_MAJOR_VERSION >= 3
            w = PyBytes_FromStringAndSize(&ci->ci_val, 1);
#else
            w = PyString_FromStringAndSize(&ci->ci_val, 1);
#endif
        }

        if (w == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, ci->ci_name, w);
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
static int addStringInstances(PyObject *dict, sipStringInstanceDef *si)
{
    while (si->si_name != NULL)
    {
        int rc;
        PyObject *w;

        switch (si->si_encoding)
        {
        case 'A':
            w = PyUnicode_DecodeASCII(si->si_val, strlen(si->si_val), NULL);
            break;

        case 'L':
            w = PyUnicode_DecodeLatin1(si->si_val, strlen(si->si_val), NULL);
            break;

        case '8':
#if PY_MAJOR_VERSION >= 3
            w = PyUnicode_FromString(si->si_val);
#else
            w = PyUnicode_DecodeUTF8(si->si_val, strlen(si->si_val), NULL);
#endif
            break;

        default:
#if PY_MAJOR_VERSION >= 3
            w = PyBytes_FromString(si->si_val);
#else
            w = PyString_FromString(si->si_val);
#endif
        }

        if (w == NULL)
            return -1;

        rc = PyDict_SetItemString(dict, si->si_name, w);
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

#if PY_MAJOR_VERSION >= 3
        w = PyLong_FromLong(ii->ii_val);
#else
        w = PyInt_FromLong(ii->ii_val);
#endif

        if (w == NULL)
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
 * Wrap a set of type instances and add them to a dictionary.
 */
static int addTypeInstances(PyObject *dict, sipTypeInstanceDef *ti)
{
    while (ti->ti_name != NULL)
    {
        if (addSingleTypeInstance(dict, ti->ti_name, ti->ti_ptr, *ti->ti_type, ti->ti_flags) < 0)
            return -1;

        ++ti;
    }

    return 0;
}


/*
 * Wrap a single type instance and add it to a dictionary.
 */
static int addSingleTypeInstance(PyObject *dict, const char *name,
        void *cppPtr, const sipTypeDef *td, int initflags)
{
    int rc;
    PyObject *obj;

    if (sipTypeIsClass(td))
    {
        obj = sipWrapSimpleInstance(cppPtr, td, NULL, initflags);
    }
    else if (sipTypeIsEnum(td))
    {
        obj = sip_api_convert_from_enum(*(int *)cppPtr, td);
    }
    else
    {
        assert(sipTypeIsMapped(td));

        obj = ((const sipMappedTypeDef *)td)->mtd_cfrom(cppPtr, NULL);
    }

    if (obj == NULL)
        return -1;

    rc = PyDict_SetItemString(dict, name, obj);
    Py_DECREF(obj);

    return rc;
}


/*
 * Convert a type instance and add it to a dictionary.
 */
static int sip_api_add_type_instance(PyObject *dict, const char *name,
        void *cppPtr, const sipTypeDef *td)
{
    return addSingleTypeInstance(getDictFromObject(dict), name, cppPtr, td, 0);
}


/*
 * Return the instance dictionary for an object if it is a wrapped type.
 * Otherwise assume that it is a module dictionary.
 */
static PyObject *getDictFromObject(PyObject *obj)
{
    if (PyObject_TypeCheck(obj, (PyTypeObject *)&sipWrapperType_Type))
        obj = ((PyTypeObject *)obj)->tp_dict;

    return obj;
}


/*
 * Return a Python reimplementation corresponding to a C/C++ virtual function,
 * if any.  If one was found then the Python lock is acquired.
 */
static PyObject *sip_api_is_py_method(sip_gilstate_t *gil, char *pymc,
        sipSimpleWrapper *sipSelf, const char *cname, const char *mname)
{
    PyObject *mname_obj, *reimp, *meth;
    PyTypeObject *cls;

    /*
     * This is the most common case (where there is no Python reimplementation)
     * so we take a fast shortcut without acquiring the GIL.
     */
    if (*pymc != 0)
        return NULL;

    /* We might still have C++ going after the interpreter has gone. */
    if (sipInterpreter == NULL)
        return NULL;

    /*
     * It's possible that the Python object has been deleted but the underlying
     * C++ instance is still working and trying to handle virtual functions.
     * Alternatively, an instance has started handling virtual functions before
     * its ctor has returned.  In either case say there is no Python
     * reimplementation.
     */
    if (sipSelf == NULL)
        return NULL;

    /* Get any reimplementation. */

#if PY_MAJOR_VERSION >= 3
    mname_obj = PyUnicode_FromString(mname);
#else
    mname_obj = PyString_FromString(mname);
#endif

    if (mname_obj == NULL)
        return NULL;

#ifdef WITH_THREAD
    *gil = PyGILState_Ensure();
#endif

    /*
     * We don't use PyObject_GetAttr() because that might find the generated
     * C function before a reimplementation defined in a mixin (ie. later in
     * the MRO).
     */

    if (sipSelf->dict != NULL)
        /* Check the instance dictionary in case it has been monkey patched. */
        reimp = PyDict_GetItem(sipSelf->dict, mname_obj);
    else
        reimp = NULL;

    cls = Py_TYPE(sipSelf);

    if (reimp == NULL)
    {
        SIP_SSIZE_T i;
        PyObject *mro;

        mro = cls->tp_mro;
        assert(PyTuple_Check(mro));

        for (i = 0; i < PyTuple_GET_SIZE(mro); ++i)
        {
            cls = (PyTypeObject *)PyTuple_GET_ITEM(mro, i);

            if (cls->tp_dict != NULL)
            {
                reimp = PyDict_GetItem(cls->tp_dict, mname_obj);

                if (reimp != NULL)
                    break;
            }
        }
    }

    Py_DECREF(mname_obj);

    /* Check any reimplementation is Python code. */
    if (reimp != NULL && Py_TYPE(reimp) == &PyFunction_Type)
    {
#if PY_MAJOR_VERSION >= 3
        meth = PyMethod_New(reimp, (PyObject *)sipSelf);
#else
        meth = PyMethod_New(reimp, (PyObject *)sipSelf, (PyObject *)cls);
#endif
    }
    else
    {
        meth = NULL;
    }

    if (meth == NULL)
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

#ifdef WITH_THREAD
        PyGILState_Release(*gil);
#endif
    }

    return meth;
}


/*
 * Convert a C/C++ pointer to the object that wraps it.
 */
static PyObject *sip_api_get_pyobject(void *cppPtr, const sipTypeDef *td)
{
    return (PyObject *)sipOMFindObject(&cppPyMap, cppPtr, td);
}


/*
 * Return the C/C++ pointer from a wrapper without any checks.
 */
void *sipGetAddress(sipSimpleWrapper *sw)
{
    if (sipIsAccessFunc(sw))
        return (*sw->u.afPtr)();

    if (sipIsIndirect(sw))
        return *((void **)sw->u.cppPtr);

    return sw->u.cppPtr;
}


/*
 * Get the C/C++ pointer for a complex object.  Note that not casting the C++
 * pointer is a bug.  However this is only ever called by PyQt3 signal emitter
 * code and PyQt doesn't contain anything that multiply inherits from QObject.
 */
static void *sip_api_get_complex_cpp_ptr(sipSimpleWrapper *sw)
{
    return getComplexCppPtr(sw, NULL);
}


/*
 * Get the C/C++ pointer for a complex object and optionally cast it to the
 * required type.
 */
static void *getComplexCppPtr(sipSimpleWrapper *sw, const sipTypeDef *td)
{
    if (!sipIsDerived(sw))
    {
        PyErr_SetString(PyExc_RuntimeError,
                "no access to protected functions or signals for objects not created from Python");

        return NULL;
    }

    return sip_api_get_cpp_ptr(sw, td);
}


/*
 * Get the C/C++ pointer from a wrapper and optionally cast it to the required
 * type.
 */
void *sip_api_get_cpp_ptr(sipSimpleWrapper *sw, const sipTypeDef *td)
{
    void *ptr = sipGetAddress(sw);

    if (checkPointer(ptr) < 0)
        return NULL;

    if (td != NULL)
        ptr = cast_cpp_ptr(ptr, Py_TYPE(sw), td);

    return ptr;
}


/*
 * Cast a C/C++ pointer from a source type to a destination type.
 */
static void *cast_cpp_ptr(void *ptr, PyTypeObject *src_type,
        const sipTypeDef *dst_type)
{
    sipCastFunc cast = ((const sipClassTypeDef *)((sipWrapperType *)src_type)->type)->ctd_cast;

    /* C structures don't have cast functions. */
    if (cast != NULL)
        ptr = (*cast)(ptr, dst_type);

    return ptr;
}


/*
 * Check that a pointer is non-NULL.
 */
static int checkPointer(void *ptr)
{
    if (ptr == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,
                "underlying C/C++ object has been deleted");
        return -1;
    }

    return 0;
}


/*
 * Keep an extra reference to an object.
 */
static void sip_api_keep_reference(PyObject *self, int key, PyObject *obj)
{
    PyObject *dict, *key_obj;

    /*
     * If there isn't a "self" to keep the extra reference for later garbage
     * collection then just take a reference and let it leak.  This could
     * happen, for example, if virtuals were still being called while Python
     * was shutting down.
     */
    if (self == NULL)
    {
        Py_XINCREF(obj);
        return;
    }

    /* Create the extra references dictionary if needed. */
    if ((dict = ((sipSimpleWrapper *)self)->extra_refs) == NULL)
    {
        if ((dict = PyDict_New()) == NULL)
            return;

        ((sipSimpleWrapper *)self)->extra_refs = dict;
    }

#if PY_MAJOR_VERSION >= 3
    key_obj = PyLong_FromLong(key);
#else
    key_obj = PyInt_FromLong(key);
#endif

    if (key_obj != NULL)
    {
        /* This can happen if the argument was optional. */
        if (obj == NULL)
            obj = Py_None;

        PyDict_SetItem(dict, key_obj, obj);
        Py_DECREF(key_obj);
    }
}


/*
 * Check to see if a Python object can be converted to a type.
 */
static int sip_api_can_convert_to_type(PyObject *pyObj, const sipTypeDef *td,
        int flags)
{
    int ok;

    assert(sipTypeIsClass(td) || sipTypeIsMapped(td));

    /* None is handled outside the type checkers. */
    if (pyObj == Py_None)
    {
        /* If the type explicitly handles None then ignore the flags. */
        if (sipTypeAllowNone(td))
            ok = TRUE;
        else
            ok = ((flags & SIP_NOT_NONE) == 0);
    }
    else
    {
        sipConvertToFunc cto;

        if (sipTypeIsClass(td))
            cto = ((const sipClassTypeDef *)td)->ctd_cto;
        else
            cto = ((const sipMappedTypeDef *)td)->mtd_cto;

        if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
        {
            if (sipTypeIsMapped(td))
                ok = FALSE;
            else
                ok = PyObject_TypeCheck(pyObj, sipTypeAsPyTypeObject(td));
        }
        else
            ok = cto(pyObj, NULL, NULL, NULL);
    }

    return ok;
}


/*
 * Convert a Python object to a C/C++ pointer, assuming a previous call to
 * sip_api_can_convert_to_type() has been successful.  Allow ownership to be
 * transferred and any type convertors to be disabled.
 */
static void *sip_api_convert_to_type(PyObject *pyObj, const sipTypeDef *td,
        PyObject *transferObj, int flags, int *statep, int *iserrp)
{
    void *cpp = NULL;
    int state = 0;

    assert(sipTypeIsClass(td) || sipTypeIsMapped(td));

    /* Don't convert if there has already been an error. */
    if (!*iserrp)
    {
        /* Do the conversion. */
        if (pyObj == Py_None && !sipTypeAllowNone(td))
            cpp = NULL;
        else
        {
            sipConvertToFunc cto;

            if (sipTypeIsClass(td))
                cto = ((const sipClassTypeDef *)td)->ctd_cto;
            else
                cto = ((const sipMappedTypeDef *)td)->mtd_cto;

            if (cto == NULL || (flags & SIP_NO_CONVERTORS) != 0)
            {
                if ((cpp = sip_api_get_cpp_ptr((sipSimpleWrapper *)pyObj, td)) == NULL)
                    *iserrp = TRUE;
                else if (transferObj != NULL)
                {
                    if (transferObj == Py_None)
                        sip_api_transfer_back(pyObj);
                    else
                        sip_api_transfer_to(pyObj, transferObj);
                }
            }
            else
                state = cto(pyObj, &cpp, iserrp, transferObj);
        }
    }

    if (statep != NULL)
        *statep = state;

    return cpp;
}


/*
 * Convert a Python object to a C/C++ pointer and raise an exception if it
 * can't be done.
 */
static void *sip_api_force_convert_to_type(PyObject *pyObj,
        const sipTypeDef *td, PyObject *transferObj, int flags, int *statep,
        int *iserrp)
{
    /* Don't even try if there has already been an error. */
    if (*iserrp)
        return NULL;

    /* See if the object's type can be converted. */
    if (!sip_api_can_convert_to_type(pyObj, td, flags))
    {
        if (sipTypeIsMapped(td))
            PyErr_Format(PyExc_TypeError,
                    "%s cannot be converted to a C/C++ %s in this context",
                    Py_TYPE(pyObj)->tp_name, sipTypeName(td));
        else
            PyErr_Format(PyExc_TypeError,
                    "%s cannot be converted to %s.%s in this context",
                    Py_TYPE(pyObj)->tp_name, sipNameOfModule(td->td_module),
                    sipPyNameOfContainer(&((const sipClassTypeDef *)td)->ctd_container, td));

        if (statep != NULL)
            *statep = 0;

        *iserrp = TRUE;
        return NULL;
    }

    /* Do the conversion. */
    return sip_api_convert_to_type(pyObj, td, transferObj, flags, statep,
            iserrp);
}


/*
 * Release a possibly temporary C/C++ instance created by a type convertor.
 */
static void sip_api_release_type(void *cpp, const sipTypeDef *td, int state)
{
    /* See if there is something to release. */
    if (state & SIP_TEMPORARY)
        release(cpp, td, state);
}


/*
 * Release an instance.
 */
static void release(void *addr, const sipTypeDef *td, int state)
{
    sipReleaseFunc rel;

    if (sipTypeIsClass(td))
    {
        rel = ((const sipClassTypeDef *)td)->ctd_release;

        /*
         * If there is no release function then it must be a C structure and we
         * can just free it.
         */
        if (rel == NULL)
            sip_api_free(addr);
    }
    else if (sipTypeIsMapped(td))
        rel = ((const sipMappedTypeDef *)td)->mtd_release;
    else
        rel = NULL;

    if (rel != NULL)
        rel(addr, state);
}


/*
 * Convert a C/C++ instance to a Python instance.
 */
PyObject *sip_api_convert_from_type(void *cpp, const sipTypeDef *td,
        PyObject *transferObj)
{
    PyObject *py;

    assert(sipTypeIsClass(td) || sipTypeIsMapped(td));

    /* Handle None. */
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if (sipTypeIsMapped(td))
        return ((const sipMappedTypeDef *)td)->mtd_cfrom(cpp, transferObj);

    /* Apply any sub-class convertor. */
    if (sipTypeHasSCC(td))
        td = convertSubClass(td, &cpp);

    /* See if we have already wrapped it. */
    if ((py = sip_api_get_pyobject(cpp, td)) != NULL)
        Py_INCREF(py);
    else if ((py = sipWrapSimpleInstance(cpp, td, NULL, SIP_SHARE_MAP)) == NULL)
        return NULL;

    /* Handle any ownership transfer. */
    if (transferObj != NULL)
    {
        if (transferObj == Py_None)
            sip_api_transfer_back(py);
        else
            sip_api_transfer_to(py, transferObj);
    }

    return py;
}


/*
 * Convert a new C/C++ instance to a Python instance.
 */
static PyObject *sip_api_convert_from_new_type(void *cpp, const sipTypeDef *td,
        PyObject *transferObj)
{
    sipWrapper *owner;

    /* Handle None. */
    if (cpp == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if (sipTypeIsMapped(td))
    {
        PyObject *res = ((const sipMappedTypeDef *)td)->mtd_cfrom(cpp,
                transferObj);

        if (res != NULL)
        {
            /*
             * We no longer need the C/C++ instance so we release it (unless
             * its ownership is transferred).  This means this call is
             * semantically equivalent to the case where the type is a wrapped
             * class.
             */
            if (transferObj == NULL || transferObj == Py_None)
                release(cpp, td, 0);
        }

        return res;
    }

    assert(sipTypeIsClass(td));

    /* Apply any sub-class convertor. */
    if (sipTypeHasSCC(td))
        td = convertSubClass(td, &cpp);

    /* Handle any ownership transfer. */
    if (transferObj == NULL || transferObj == Py_None)
        owner = NULL;
    else
        owner = (sipWrapper *)transferObj;

    return sipWrapSimpleInstance(cpp, td, owner, (owner == NULL ? SIP_PY_OWNED : 0));
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
 * This is set by sip_api_find_type() before calling bsearch() on the types
 * table for the module.  This is a hack that works around the problem of
 * unresolved externally defined types.
 */
static sipExportedModuleDef *module_searched;


/*
 * The bsearch() helper function for searching the types table.
 */
static int compareTypeDef(const void *key, const void *el)
{
    const char *s1 = (const char *)key;
    const char *s2 = NULL;
    const sipTypeDef *td;
    char ch1, ch2;

    /* Allow for unresolved externally defined types. */
    td = *(const sipTypeDef **)el;

    if (td != NULL)
        s2 = sipTypeName(td);
    else
    {
        sipExternalTypeDef *etd = module_searched->em_external;

        assert(etd != NULL);

        /* Find which external type it is. */
        while (etd->et_nr >= 0)
        {
            const sipTypeDef **tdp = &module_searched->em_types[etd->et_nr];

            if (tdp == (const sipTypeDef **)el)
            {
                s2 = etd->et_name;
                break;
            }

            ++etd;
        }

        assert(s2 != NULL);
    }

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

        /* We might be looking for a pointer. */
        if ((ch1 == '*' || ch1 == '\0') && ch2 == '\0')
            return 0;
    }
    while (ch1 == ch2);

    return (ch1 < ch2 ? -1 : 1);
}


/*
 * Return the type structure for a particular type.
 */
static const sipTypeDef *sip_api_find_type(const char *type)
{
    sipExportedModuleDef *em;

    for (em = moduleList; em != NULL; em = em->em_next)
    {
        sipTypeDef **tdp;

        /* The backdoor to the comparison helper. */
        module_searched = em;

        tdp = (sipTypeDef **)bsearch((const void *)type,
                (const void *)em->em_types, em->em_nrtypes,
                sizeof (sipTypeDef *), compareTypeDef);

        if (tdp != NULL)
        {
            /*
             * Note that this will be NULL for unresolved externally defined
             * types.
             */
            return *tdp;
        }
    }

    return NULL;
}


/*
 * Return the mapped type structure for a particular mapped type.  This is
 * deprecated.
 */
static const sipMappedType *sip_api_find_mapped_type(const char *type)
{
    const sipTypeDef *td = sip_api_find_type(type);

    if (td != NULL && sipTypeIsMapped(td))
        return (const sipMappedType *)td;

    return NULL;
}


/*
 * Return the type structure for a particular class.  This is deprecated.
 */
static sipWrapperType *sip_api_find_class(const char *type)
{
    const sipTypeDef *td = sip_api_find_type(type);

    if (td != NULL && sipTypeIsClass(td))
        return (sipWrapperType *)sipTypeAsPyTypeObject(td);

    return NULL;
}


/*
 * Return the type structure for a particular named enum.  This is deprecated.
 */
static PyTypeObject *sip_api_find_named_enum(const char *type)
{
    const sipTypeDef *td = sip_api_find_type(type);

    if (td != NULL && sipTypeIsEnum(td))
        return sipTypeAsPyTypeObject(td);

    return NULL;
}


/*
 * Save the components of a Python method.
 */
void sipSaveMethod(sipPyMethod *pm, PyObject *meth)
{
    pm->mfunc = PyMethod_GET_FUNCTION(meth);
    pm->mself = PyMethod_GET_SELF(meth);
#if PY_MAJOR_VERSION < 3
    pm->mclass = PyMethod_GET_CLASS(meth);
#endif
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
    if ((mod = PyDict_GetItemString(dictofmods, "__builtin__")) == NULL)
        return;
 
    /* Get it's dictionary. */
    if ((dict = PyModule_GetDict(mod)) == NULL)
        return;
 
    /* Get the function hook. */
    if ((hook = PyDict_GetItemString(dict, hookname)) == NULL)
        return;
 
    /* Call the hook and discard any result. */
    res = PyObject_CallObject(hook, NULL);
 
    Py_XDECREF(res);
}


/*
 * Call any sub-class convertors for a given type returning a pointer to the
 * sub-type object, and possibly modifying the C++ address (in the case of
 * multiple inheritence).
 */
static const sipTypeDef *convertSubClass(const sipTypeDef *td, void **cppPtr)
{
    PyTypeObject *py_type = sipTypeAsPyTypeObject(td);
    sipExportedModuleDef *em;

    if (*cppPtr == NULL)
        return NULL;

    /*
     * Note that this code depends on the fact that a module appears in the
     * list of modules before any module it imports, ie. sub-class convertors
     * will be invoked for more specific types first.
     */
    for (em = moduleList; em != NULL; em = em->em_next)
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
            if (PyType_IsSubtype(py_type, sipTypeAsPyTypeObject(scc->scc_basetype)))
            {
                void *ptr;
                const sipTypeDef *subtype;

                ptr = cast_cpp_ptr(*cppPtr, py_type, scc->scc_basetype);
                subtype = (*scc->scc_convertor)(&ptr);

                /*
                 * We are only interested in types that are not super-classes
                 * of the target.  This happens either because it is in an
                 * earlier convertor than the one that handles the type or it
                 * is in a later convertor that handles a different branch of
                 * the hierarchy.  Either way, the ordering of the modules
                 * ensures that there will be no more than one and that it will
                 * be the right one.
                 */
                if (subtype != NULL && !PyType_IsSubtype(py_type, sipTypeAsPyTypeObject(subtype)))
                {
                    *cppPtr = ptr;
                    return subtype;
                }
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
    return td;
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
 * recognised.  This is deprecated.
 */
static sipWrapperType *sip_api_map_string_to_class(const char *typeString,
        const sipStringTypeClassMap *map, int maplen)
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
 * recognised.  This is deprecated.
 */
static sipWrapperType *sip_api_map_int_to_class(int typeInt,
        const sipIntTypeClassMap *map, int maplen)
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

    objectify("unknown", &mobj);

    PyErr_SetObject(PyExc_Exception, mobj);

    SIP_UNBLOCK_THREADS
}


/*
 * Raise an exception implemented as a type.  Make no assumptions about the
 * GIL.
 */
static void sip_api_raise_type_exception(const sipTypeDef *td, void *ptr)
{
    PyObject *self;

    assert(sipTypeIsClass(td));

    SIP_BLOCK_THREADS

    self = sipWrapSimpleInstance(ptr, td, NULL, SIP_PY_OWNED);

    PyErr_SetObject((PyObject *)sipTypeAsPyTypeObject(td), self);

    Py_XDECREF(self);

    SIP_UNBLOCK_THREADS
}


/*
 * Return the module of an encoded type.
 */
static sipExportedModuleDef *getTypeModule(const sipEncodedTypeDef *enc,
        sipExportedModuleDef *em)
{
    if (enc->sc_module != 255)
        em = em->em_imports[enc->sc_module].im_module;

    return em;
}


/*
 * Return the generated type structure of an encoded type.
 */
static sipTypeDef *getGeneratedType(const sipEncodedTypeDef *enc,
        sipExportedModuleDef *em)
{
    return getTypeModule(enc, em)->em_types[enc->sc_type];
}


/*
 * Find a particular slot function for a type.
 */
static void *findSlot(PyObject *self, sipPySlotType st)
{
    void *slot;
    PyTypeObject *py_type = Py_TYPE(self);

    /* See if it is a wrapper. */
    if (PyObject_TypeCheck((PyObject *)py_type, &sipWrapperType_Type))
    {
        sipClassTypeDef *ctd;

        ctd = (sipClassTypeDef *)((sipWrapperType *)(py_type))->type;

        if (ctd->ctd_pyslots != NULL)
            slot = findSlotInType(ctd->ctd_pyslots, st);
        else
            slot = NULL;

        if (slot == NULL)
        {
            sipEncodedTypeDef *sup;

            /* Search any super-types. */
            if ((sup = ctd->ctd_supers) != NULL)
            {
                sipClassTypeDef *sup_ctd;

                do
                {
                    sup_ctd = (sipClassTypeDef *)getGeneratedType(sup,
                            ctd->ctd_base.td_module);

                    if (sup_ctd->ctd_pyslots != NULL)
                        slot = findSlotInType(sup_ctd->ctd_pyslots, st);
                }
                while (slot == NULL && !sup++->sc_flag);
            }
        }
    }
    else
    {
        sipEnumTypeDef *etd;

        /* If it is not a wrapper then it must be an enum. */
        assert(PyObject_TypeCheck((PyObject *)py_type, &sipEnumType_Type));

        etd = (sipEnumTypeDef *)((sipEnumTypeObject *)(py_type))->type;

        assert(etd->etd_pyslots != NULL);

        slot = findSlotInType(etd->etd_pyslots, st);
    }

    return slot;
}


/*
 * Find a particular slot function in a particular type.
 */
static void *findSlotInType(sipPySlotDef *psd, sipPySlotType st)
{
    while (psd->psd_func != NULL)
    {
        if (psd->psd_type == st)
            return psd->psd_func;

        ++psd;
    }

    return NULL;
}


/*
 * Return the C/C++ address and the generated class structure for a wrapper.
 */
static void *getPtrTypeDef(sipSimpleWrapper *self, const sipClassTypeDef **ctd)
{
    *ctd = (const sipClassTypeDef *)((sipWrapperType *)Py_TYPE(self))->type;

    return (sipNotInMap(self) ? NULL : self->u.cppPtr);
}


/*
 * Handle an objobjargproc slot.
 */
static int objobjargprocSlot(PyObject *self, PyObject *arg1, PyObject *arg2,
        sipPySlotType st)
{
    int (*f)(PyObject *, PyObject *);
    int res;

    f = (int (*)(PyObject *, PyObject *))findSlot(self, st);

    if (f != NULL)
    {
        PyObject *args;

        /*
         * Slot handlers require a single PyObject *.  The second argument is
         * optional.
         */
        if (arg2 == NULL)
        {
            args = arg1;
            Py_INCREF(args);
        }
        else
        {
#if PY_VERSION_HEX >= 0x02040000
            args = PyTuple_Pack(2, arg1, arg2);
#else
            args = Py_BuildValue("(OO)", arg1, arg2);
#endif

            if (args == NULL)
                return -1;
        }

        res = f(self, args);
        Py_DECREF(args);
    }
    else
    {
        PyErr_SetNone(PyExc_NotImplementedError);
        res = -1;
    }

    return res;
}


/*
 * Handle an ssizeobjargproc slot.
 */
static int ssizeobjargprocSlot(PyObject *self, SIP_SSIZE_T arg1,
        PyObject *arg2, sipPySlotType st)
{
    int (*f)(PyObject *, PyObject *);
    int res;

    f = (int (*)(PyObject *, PyObject *))findSlot(self, st);

    if (f != NULL)
    {
        PyObject *args;

        /*
         * Slot handlers require a single PyObject *.  The second argument is
         * optional.
         */
        if (arg2 == NULL)
#if PY_MAJOR_VERSION >= 3
            args = PyLong_FromSsize_t(arg1);
#elif PY_VERSION_HEX >= 0x02050000
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

        res = f(self, args);
        Py_DECREF(args);
    }
    else
    {
        PyErr_SetNone(PyExc_NotImplementedError);
        res = -1;
    }

    return res;
}


/*****************************************************************************
 * The functions, data types and structures to support a Python type to hold a
 * void *.
 *****************************************************************************/

/* The object data structure. */
typedef struct {
    PyObject_HEAD
    void *voidptr;
    SIP_SSIZE_T size;
    int rw;
} sipVoidPtrObject;


/* The structure used to hold the results of a voidptr conversion. */
struct vp_values {
    void *voidptr;
    SIP_SSIZE_T size;
    int rw;
};


/*
 * Convert a Python object to the values needed to create a voidptr.
 */
static int vp_convertor(PyObject *arg, struct vp_values *vp)
{
    void *ptr;
    SIP_SSIZE_T size = -1;
    int rw = TRUE;

    if (arg == Py_None)
        ptr = NULL;
    else if (PyCObject_Check(arg))
        ptr = PyCObject_AsVoidPtr(arg);
    else if (PyObject_TypeCheck(arg, &sipVoidPtr_Type))
    {
        ptr = ((sipVoidPtrObject *)arg)->voidptr;
        size = ((sipVoidPtrObject *)arg)->size;
        rw = ((sipVoidPtrObject *)arg)->rw;
    }
    else
    {
#if PY_MAJOR_VERSION >= 3
        ptr = PyLong_AsVoidPtr(arg);
#else
        ptr = (void *)PyInt_AsLong(arg);
#endif

        if (PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "a single integer, CObject, None or another voidptr is required");
            return 0;
        }
    }

    vp->voidptr = ptr;
    vp->size = size;
    vp->rw = rw;

    return 1;
}


/*
 * Implement __new__ for the type.
 */
static PyObject *sipVoidPtr_new(PyTypeObject *subtype, PyObject *args,
        PyObject *kw)
{
    static char *kwlist[] = {"address", "size", "writeable", NULL};

    struct vp_values vp_conversion;
    SIP_SSIZE_T size = -1;
    int rw = -1;
    PyObject *obj;

    if (!PyArg_ParseTupleAndKeywords(args, kw,
#if PY_VERSION_HEX >= 0x02050000
            "O&|ni:voidptr",
#else
            "O&|ii:voidptr",
#endif
            kwlist, vp_convertor, &vp_conversion, &size, &rw))
        return NULL;

    /* Use the explicit size if one was given. */
    if (size >= 0)
        vp_conversion.size = size;

    /* Use the explicit writeable flag if one was given. */
    if (rw >= 0)
        vp_conversion.rw = rw;

    /* Create the instance. */
    if ((obj = subtype->tp_alloc(subtype, 0)) == NULL)
        return NULL;

    /* Save the values. */
    ((sipVoidPtrObject *)obj)->voidptr = vp_conversion.voidptr;
    ((sipVoidPtrObject *)obj)->size = vp_conversion.size;
    ((sipVoidPtrObject *)obj)->rw = vp_conversion.rw;

    return obj;
}


#if PY_MAJOR_VERSION >= 3
/*
 * The read buffer implementation for Python v3.
 */
static int sipVoidPtr_getbuffer(PyObject *self, Py_buffer *buf, int flags)
{
    sipVoidPtrObject *v = (sipVoidPtrObject *)self;

    return PyBuffer_FillInfo(buf, self, v->voidptr, v->size, !v->rw, flags);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The read buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipVoidPtr_getbuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    SIP_SSIZE_T size = ((sipVoidPtrObject *)self)->size;

    if (size < 0 || seg != 0)
    {
        PyErr_SetString(PyExc_SystemError, "invalid buffer segment");
        return -1;
    }

    *ptr = ((sipVoidPtrObject *)self)->voidptr;

    return size;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The write buffer implementation for Python v2.
 */
static SIP_SSIZE_T sipVoidPtr_getwritebuffer(PyObject *self, SIP_SSIZE_T seg,
        void **ptr)
{
    if (((sipVoidPtrObject *)self)->rw)
        return sipVoidPtr_getbuffer(self, seg, ptr);

    PyErr_SetString(PyExc_TypeError, "the sip.voidptr is not writeable");
    return -1;
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The segment count implementation for Python v2.
 */
static SIP_SSIZE_T sipVoidPtr_getsegcount(PyObject *self, SIP_SSIZE_T *lenp)
{
    SIP_SSIZE_T segs, len;

    len = ((sipVoidPtrObject *)self)->size;
    segs = (len < 0 ? 0 : 1);

    if (lenp != NULL)
        *lenp = len;

    return segs;
}
#endif


/*
 * Implement int() for the type.
 */
static PyObject *sipVoidPtr_int(sipVoidPtrObject *v)
{
    return PyLong_FromVoidPtr(v->voidptr);
}


#if PY_MAJOR_VERSION < 3
/*
 * Implement hex() for the type.
 */
static PyObject *sipVoidPtr_hex(sipVoidPtrObject *v)
{
    char buf[2 + 16 + 1];

    PyOS_snprintf(buf, sizeof (buf), "0x%.*lx", (int)(sizeof (void *) * 2),
            (unsigned long)v->voidptr);

    return PyString_FromString(buf);
}
#endif


/*
 * Implement ascobject() for the type.
 */
static PyObject *sipVoidPtr_ascobject(sipVoidPtrObject *v, PyObject *arg)
{
    return PyCObject_FromVoidPtr(v->voidptr, NULL);
}


/*
 * Implement asstring() for the type.
 */
static PyObject *sipVoidPtr_asstring(sipVoidPtrObject *v, PyObject *args,
        PyObject *kw)
{
    static char *kwlist[] = {"size", NULL};

    SIP_SSIZE_T size = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kw,
#if PY_VERSION_HEX >= 0x02050000
            "|n:asstring",
#else
            "|i:asstring",
#endif
            kwlist, &size))
        return NULL;

    /* Use the current size if one wasn't explicitly given. */
    if (size < 0)
        size = v->size;

    if (size < 0)
    {
        PyErr_SetString(PyExc_ValueError,
                "a size must be given or the sip.voidptr must have a size");
        return NULL;
    }

#if PY_MAJOR_VERSION >= 3
    return PyBytes_FromStringAndSize(v->voidptr, size);
#else
    return PyString_FromStringAndSize(v->voidptr, size);
#endif
}


/*
 * Implement getsize() for the type.
 */
static PyObject *sipVoidPtr_getsize(sipVoidPtrObject *v, PyObject *arg)
{
#if PY_MAJOR_VERSION >= 3
    return PyLong_FromSsize_t(v->size);
#elif PY_VERSION_HEX >= 0x02050000
    return PyInt_FromSsize_t(v->size);
#else
    return PyInt_FromLong(v->size);
#endif
}


/*
 * Implement setsize() for the type.
 */
static PyObject *sipVoidPtr_setsize(sipVoidPtrObject *v, PyObject *arg)
{
    SIP_SSIZE_T size;

#if PY_MAJOR_VERSION >= 3
    size = PyLong_AsSsize_t(arg);
#elif PY_VERSION_HEX >= 0x02050000
    size = PyInt_AsSsize_t(arg);
#else
    size = (int)PyInt_AsLong(arg);
#endif

    if (PyErr_Occurred())
        return NULL;

    v->size = size;

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Implement getwriteable() for the type.
 */
static PyObject *sipVoidPtr_getwriteable(sipVoidPtrObject *v, PyObject *arg)
{
    return PyBool_FromLong(v->rw);
}


/*
 * Implement setwriteable() for the type.
 */
static PyObject *sipVoidPtr_setwriteable(sipVoidPtrObject *v, PyObject *arg)
{
    int rw;

#if PY_MAJOR_VERSION >= 3
    rw = (int)PyLong_AsLong(arg);
#else
    rw = (int)PyInt_AsLong(arg);
#endif

    if (PyErr_Occurred())
        return NULL;

    v->rw = rw;

    Py_INCREF(Py_None);
    return Py_None;
}


/* The methods data structure. */
static PyMethodDef sipVoidPtr_Methods[] = {
    {"ascobject", (PyCFunction)sipVoidPtr_ascobject, METH_NOARGS, NULL},
    {"asstring", (PyCFunction)sipVoidPtr_asstring, METH_KEYWORDS, NULL},
    {"getsize", (PyCFunction)sipVoidPtr_getsize, METH_NOARGS, NULL},
    {"setsize", (PyCFunction)sipVoidPtr_setsize, METH_O, NULL},
    {"getwriteable", (PyCFunction)sipVoidPtr_getwriteable, METH_NOARGS, NULL},
    {"setwriteable", (PyCFunction)sipVoidPtr_setwriteable, METH_O, NULL},
    {NULL}
};


/* The number methods data structure. */
static PyNumberMethods sipVoidPtr_NumberMethods = {
    0,                      /* nb_add */
    0,                      /* nb_subtract */
    0,                      /* nb_multiply */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_divide */
#endif
    0,                      /* nb_remainder */
    0,                      /* nb_divmod */
    0,                      /* nb_power */
    0,                      /* nb_negative */
    0,                      /* nb_positive */
    0,                      /* nb_absolute */
    0,                      /* nb_bool (Python v3), nb_nonzero (Python v2) */
    0,                      /* nb_invert */
    0,                      /* nb_lshift */
    0,                      /* nb_rshift */
    0,                      /* nb_and */
    0,                      /* nb_xor */
    0,                      /* nb_or */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_coerce */
#endif
    (unaryfunc)sipVoidPtr_int,  /* nb_int */
    0,                      /* nb_reserved (Python v3), nb_long (Python v2) */
    0,                      /* nb_float */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_oct */
    (unaryfunc)sipVoidPtr_hex,  /* nb_hex */
#endif
    0,                      /* nb_inplace_add */
    0,                      /* nb_inplace_subtract */
    0,                      /* nb_inplace_multiply */
#if PY_MAJOR_VERSION < 3
    0,                      /* nb_inplace_divide */
#endif
    0,                      /* nb_inplace_remainder */
    0,                      /* nb_inplace_power */
    0,                      /* nb_inplace_lshift */
    0,                      /* nb_inplace_rshift */
    0,                      /* nb_inplace_and */
    0,                      /* nb_inplace_xor */
    0,                      /* nb_inplace_or */
    0,                      /* nb_floor_divide */
    0,                      /* nb_true_divide */
    0,                      /* nb_inplace_floor_divide */
    0,                      /* nb_inplace_true_divide */
#if PY_VERSION_HEX >= 0x02050000
    0                       /* nb_index */
#endif
};


/* The buffer methods data structure. */
static PyBufferProcs sipVoidPtr_BufferProcs = {
    sipVoidPtr_getbuffer,
#if PY_MAJOR_VERSION >= 3
    NULL,
#else
    sipVoidPtr_getwritebuffer,
    sipVoidPtr_getsegcount,
#if PY_VERSION_HEX >= 0x02050000
    (charbufferproc)sipVoidPtr_getbuffer
#else
    (getcharbufferproc)sipVoidPtr_getbuffer
#endif
#endif
};


/* The type data structure. */
static PyTypeObject sipVoidPtr_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "sip.voidptr",          /* tp_name */
    sizeof (sipVoidPtrObject),  /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_reserved (Python v3), tp_compare (Python v2) */
    0,                      /* tp_repr */
    &sipVoidPtr_NumberMethods,  /* tp_as_number */
    0,                      /* tp_as_sequence */
    0,                      /* tp_as_mapping */
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    0,                      /* tp_getattro */
    0,                      /* tp_setattro */
    &sipVoidPtr_BufferProcs,    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    0,                      /* tp_doc */
    0,                      /* tp_traverse */
    0,                      /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    sipVoidPtr_Methods,     /* tp_methods */
    0,                      /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    0,                      /* tp_init */
    0,                      /* tp_alloc */
    sipVoidPtr_new,         /* tp_new */
};


/*
 * A convenience function to convert a C/C++ void pointer from a Python object.
 */
static void *sip_api_convert_to_void_ptr(PyObject *obj)
{
    if (obj == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "sip.voidptr is NULL");
        return NULL;
    }

    if (obj == Py_None)
        return NULL;

    if (PyObject_TypeCheck(obj, &sipVoidPtr_Type))
        return ((sipVoidPtrObject *)obj)->voidptr;

    if (PyCObject_Check(obj))
        return PyCObject_AsVoidPtr(obj);

#if PY_MAJOR_VERSION >= 3
    return PyLong_AsVoidPtr(obj);
#else
    return (void *)PyInt_AsLong(obj);
#endif
}


/*
 * Convert a C/C++ void pointer to a sip.voidptr object.
 */
static PyObject *sip_api_convert_from_void_ptr(void *val)
{
    return make_voidptr(val, -1, TRUE);
}


/*
 * Convert a C/C++ void pointer to a sip.voidptr object.
 */
static PyObject *sip_api_convert_from_const_void_ptr(const void *val)
{
    return make_voidptr((void *)val, -1, FALSE);
}


/*
 * Convert a sized C/C++ void pointer to a sip.voidptr object.
 */
static PyObject *sip_api_convert_from_void_ptr_and_size(void *val,
        SIP_SSIZE_T size)
{
    return make_voidptr(val, size, TRUE);
}


/*
 * Convert a sized C/C++ const void pointer to a sip.voidptr object.
 */
static PyObject *sip_api_convert_from_const_void_ptr_and_size(const void *val,
        SIP_SSIZE_T size)
{
    return make_voidptr((void *)val, size, FALSE);
}


/*
 * Do the work of converting a void pointer.
 */
static PyObject *make_voidptr(void *voidptr, SIP_SSIZE_T size, int rw)
{
    sipVoidPtrObject *self;

    if (voidptr == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    if ((self = PyObject_NEW(sipVoidPtrObject, &sipVoidPtr_Type)) == NULL)
        return NULL;

    self->voidptr = voidptr;
    self->size = size;
    self->rw = rw;

    return (PyObject *)self;
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
     * Consume any extra type specific information and use it to initialise the
     * slots.  This only happens for directly wrapped classes (and not
     * programmer written sub-classes).  This must be done in the alloc
     * function because it is the only place we can break out of the default
     * new() function before PyType_Ready() is called.
     */
    if (currentType != NULL)
    {
        ((sipWrapperType *)o)->type = currentType;

        if (sipTypeIsClass(currentType))
            addClassSlots((sipWrapperType *)o, (sipClassTypeDef *)currentType);

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
         * We allow the class to use this as a meta-type without being
         * derived from a class that uses it.  This allows mixin classes that
         * need their own meta-type to work so long as their meta-type is
         * derived from this meta-type.  This condition is indicated by the
         * pointer to the generated type structure being NULL.
         */
        if (sc != NULL && PyObject_TypeCheck((PyObject *)sc, (PyTypeObject *)&sipWrapperType_Type))
            self->type = ((sipWrapperType *)sc)->type;
    }
    else
    {
        /*
         * We must be a generated type so remember the type object in the
         * generated type structure.
         */
        assert(self->type->u.td_py_type == NULL);

        self->type->u.td_py_type = (PyTypeObject *)self;
    }

    return 0;
}


/*
 * The type getattro slot.
 */
static PyObject *sipWrapperType_getattro(PyObject *self, PyObject *name)
{
    if (add_all_lazy_attrs(((sipWrapperType *)self)->type) < 0)
        return NULL;

    return PyType_Type.tp_getattro(self, name);
}


/*
 * The type setattro slot.
 */
static int sipWrapperType_setattro(PyObject *self, PyObject *name,
        PyObject *value)
{
    if (add_all_lazy_attrs(((sipWrapperType *)self)->type) < 0)
        return -1;

    return PyType_Type.tp_setattro(self, name, value);
}


/*
 * The type data structure.  We inherit everything from the standard Python
 * metatype except the init and getattro methods and the size of the type
 * object created is increased to accomodate the extra information we associate
 * with a wrapped type.
 */
static PyTypeObject sipWrapperType_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "sip.wrappertype",      /* tp_name */
    sizeof (sipWrapperType),    /* tp_basicsize */
    0,                      /* tp_itemsize */
    0,                      /* tp_dealloc */
    0,                      /* tp_print */
    0,                      /* tp_getattr */
    0,                      /* tp_setattr */
    0,                      /* tp_reserved (Python v3), tp_compare (Python v2) */
    0,                      /* tp_repr */
    0,                      /* tp_as_number */
    0,                      /* tp_as_sequence */
    0,                      /* tp_as_mapping */
    0,                      /* tp_hash */
    0,                      /* tp_call */
    0,                      /* tp_str */
    sipWrapperType_getattro,    /* tp_getattro */
    sipWrapperType_setattro,    /* tp_setattro */
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
    (initproc)sipWrapperType_init,  /* tp_init */
    sipWrapperType_alloc,   /* tp_alloc */
    0,                      /* tp_new */
    0,                      /* tp_free */
};


/*****************************************************************************
 * The Python type that is the super-type for all C++ wrapper types.
 *****************************************************************************/

/*
 * The instance new slot.
 */
static PyObject *sipSimpleWrapper_new(sipWrapperType *wt, PyObject *args,
        PyObject *kwds)
{
    static PyObject *noargs = NULL;
    sipTypeDef *td = wt->type;
    sipContainerDef *cod;

    if (sipTypeIsMapped(td))
        cod = &((sipMappedTypeDef *)td)->mtd_container;
    else
        cod = &((sipClassTypeDef *)td)->ctd_container;

    /* We need an empty tuple for an empty argument list. */
    if (noargs == NULL)
    {
        noargs = PyTuple_New(0);

        if (noargs == NULL)
            return NULL;
    }

    /* Check the base types are not being used directly. */
    if (wt == &sipSimpleWrapper_Type || wt == &sipWrapper_Type)
    {
        PyErr_Format(PyExc_TypeError,
                "the %s.%s type cannot be instantiated or sub-classed",
                sipNameOfModule(td->td_module),
                sipPyNameOfContainer(cod, td));

        return NULL;
    }

    /* See if it is a mapped type. */
    if (sipTypeIsMapped(td))
    {
        PyErr_Format(PyExc_TypeError,
                "%s.%s represents a mapped type and cannot be instantiated",
                sipNameOfModule(td->td_module),
                sipPyNameOfContainer(cod, td));

        return NULL;
    }

    /* See if it is a namespace. */
    if (sipTypeIsNamespace(td))
    {
        PyErr_Format(PyExc_TypeError,
                "%s.%s represents a C++ namespace and cannot be instantiated",
                sipNameOfModule(td->td_module),
                sipPyNameOfContainer(cod, td));

        return NULL;
    }

    /*
     * See if the object is being created explicitly rather than being wrapped.
     */
    if (sipGetPending(NULL, NULL) == NULL)
    {
        /*
         * See if it cannot be instantiated or sub-classed from Python, eg.
         * it's an opaque class.  Some restrictions might be overcome with
         * better SIP support.
         */
        if (((sipClassTypeDef *)td)->ctd_init == NULL)
        {
            PyErr_Format(PyExc_TypeError,
                    "%s.%s cannot be instantiated or sub-classed",
                    sipNameOfModule(td->td_module),
                    sipPyNameOfContainer(cod, td));

            return NULL;
        }

        /* See if it is an abstract type. */
        if (sipTypeIsAbstract(td) && sipIsExactWrappedType(wt))
        {
            PyErr_Format(PyExc_TypeError,
                    "%s.%s represents a C++ abstract class and cannot be instantiated",
                    sipNameOfModule(td->td_module),
                    sipPyNameOfContainer(cod, td));

            return NULL;
        }
    }

    /* Call the standard super-type new. */
    return PyBaseObject_Type.tp_new((PyTypeObject *)wt, noargs, NULL);
}


/*
 * The instance init slot.
 */
static int sipSimpleWrapper_init(sipSimpleWrapper *self, PyObject *args,
        PyObject *kwds)
{
    void *sipNew;
    int sipFlags;
    sipWrapper *owner;
    sipWrapperType *wt = (sipWrapperType *)Py_TYPE(self);
    sipTypeDef *td = wt->type;
    sipClassTypeDef *ctd = (sipClassTypeDef *)td;

    /* Check there is no existing C++ instance waiting to be wrapped. */
    if ((sipNew = sipGetPending(&owner, &sipFlags)) == NULL)
    {
        int argsparsed = 0;

        /* Call the C++ ctor. */
        owner = NULL;

        if ((sipNew = ctd->ctd_init(self, args, (PyObject **)&owner, &argsparsed)) != NULL)
            sipFlags = SIP_DERIVED_CLASS;
        else
        {
            int pstate = argsparsed & PARSE_MASK;
            sipInitExtenderDef *ie = wt->iextend;

            /*
             * If the parse was successful, and no exception has been raised,
             * but no C/C++ object was created then we assume that handwritten
             * code decided after the parse that it didn't want to handle the
             * signature.
             */
            if (pstate == PARSE_OK && !PyErr_Occurred())
                pstate = argsparsed = PARSE_TYPE;

            /*
             * While we just have signature errors, try any initialiser
             * extenders.
             */
            while (ie != NULL && (pstate == PARSE_MANY || pstate == PARSE_FEW || pstate == PARSE_TYPE))
            {
                argsparsed = 0;

                if ((sipNew = ie->ie_extender(self, args, (PyObject **)&owner, &argsparsed)) != NULL)
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

                badArgs(argsparsed, sipNameOfModule(td->td_module),
                        sipPyNameOfContainer(&ctd->ctd_container, td));
                return -1;
            }

            sipFlags = 0;
        }

        if (owner == NULL)
            sipFlags |= SIP_PY_OWNED;
        else if ((PyObject *)owner == Py_None)
        {
            /* This is the hack that means that C++ owns the new instance. */
            sipFlags |= SIP_CPP_HAS_REF;
            Py_INCREF(self);
            owner = NULL;
        }
    }

    /*
     * If there is an owner then we assume that the wrapper supports the
     * concept.
     */
    if (owner != NULL)
    {
        assert(PyObject_TypeCheck((PyObject *)self, (PyTypeObject *)&sipWrapper_Type));
        addToParent((sipWrapper *)self, (sipWrapper *)owner);
    }

    self->u.cppPtr = sipNew;
    self->flags = sipFlags;

    if (!sipNotInMap(self))
        sipOMAddObject(&cppPyMap,self);

    if (kwds != NULL && PyDict_Check(kwds) && PyDict_Size(kwds) != 0)
    {
        static int got_kw_handler = FALSE;
        static int (*kw_handler)(PyObject *, void *, PyObject *);

        // Get the keyword handler if necessary.  Note that the C/C++
        // instance will leak if there is any error.
        if (!got_kw_handler)
        {
            kw_handler = sip_api_import_symbol("pyqt_kw_handler");
            got_kw_handler = TRUE;
        }

        if (kw_handler == NULL || !isQObject(self))
        {
            PyErr_Format(PyExc_TypeError,
                    "%s.%s does not support keyword arguments",
                    sipNameOfModule(td->td_module),
                    sipPyNameOfContainer(&ctd->ctd_container, td));

            return -1;
        }

        if (kw_handler(self, sipNew, kwds) < 0)
            return -1;
    }

    return 0;
}


/*
 * The instance traverse slot.
 */
static int sipSimpleWrapper_traverse(sipSimpleWrapper *self, visitproc visit,
        void *arg)
{
    int vret;
    void *ptr;
    const sipClassTypeDef *ctd;

    /* Call the nearest handwritten traverse code in the class hierachy. */
    if ((ptr = getPtrTypeDef(self, &ctd)) != NULL)
    {
        const sipClassTypeDef *sup_ctd = ctd;

        if (ctd->ctd_traverse == NULL)
        {
            sipEncodedTypeDef *sup;

            if ((sup = ctd->ctd_supers) != NULL)
                do
                    sup_ctd = (sipClassTypeDef *)getGeneratedType(sup, ctd->ctd_base.td_module);
                while (sup_ctd->ctd_traverse == NULL && !sup++->sc_flag);
        }

        if (sup_ctd->ctd_traverse != NULL)
            if ((vret = sup_ctd->ctd_traverse(ptr, visit, arg)) != 0)
                return vret;
    }

    if (self->dict != NULL)
        if ((vret = visit(self->dict, arg)) != 0)
            return vret;

    if (self->extra_refs != NULL)
        if ((vret = visit(self->extra_refs, arg)) != 0)
            return vret;

    if (self->user != NULL)
        if ((vret = visit(self->user, arg)) != 0)
            return vret;

    return 0;
}


/*
 * The instance clear slot.
 */
static int sipSimpleWrapper_clear(sipSimpleWrapper *self)
{
    int vret = 0;
    void *ptr;
    const sipClassTypeDef *ctd;
    PyObject *tmp;

    /* Call the nearest handwritten clear code in the class hierachy. */
    if ((ptr = getPtrTypeDef(self, &ctd)) != NULL)
    {
        const sipClassTypeDef *sup_ctd = ctd;

        if (ctd->ctd_clear == NULL)
        {
            sipEncodedTypeDef *sup;

            if ((sup = ctd->ctd_supers) != NULL)
                do
                    sup_ctd = (sipClassTypeDef *)getGeneratedType(sup, ctd->ctd_base.td_module);
                while (sup_ctd->ctd_clear == NULL && !sup++->sc_flag);
        }

        if (sup_ctd->ctd_clear != NULL)
            vret = sup_ctd->ctd_clear(ptr);
    }

    /* Remove the instance dictionary. */
    tmp = self->dict;
    self->dict = NULL;
    Py_XDECREF(tmp);

    /* Remove any extra references dictionary. */
    tmp = self->extra_refs;
    self->extra_refs = NULL;
    Py_XDECREF(tmp);

    /* Remove any user object. */
    tmp = self->user;
    self->user = NULL;
    Py_XDECREF(tmp);

    return vret;
}


#if PY_MAJOR_VERSION >= 3
/*
 * The instance get buffer slot for Python v3.
 */
static int sipSimpleWrapper_getbuffer(sipSimpleWrapper *self, Py_buffer *buf,
        int flags)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return -1;

    return ctd->ctd_getbuffer((PyObject *)self, ptr, buf, flags);
}
#endif


#if PY_MAJOR_VERSION >= 3
/*
 * The instance release buffer slot for Python v3.
 */
static void sipSimpleWrapper_releasebuffer(sipSimpleWrapper *self,
        Py_buffer *buf)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return -1;

    return ctd->ctd_releasebuffer((PyObject *)self, ptr, buf);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The instance read buffer slot for Python v2.
 */
static SIP_SSIZE_T sipSimpleWrapper_getreadbuffer(sipSimpleWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return -1;

    return ctd->ctd_readbuffer((PyObject *)self, ptr, segment, ptrptr);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The instance write buffer slot for Python v2.
 */
static SIP_SSIZE_T sipSimpleWrapper_getwritebuffer(sipSimpleWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return -1;

    return ctd->ctd_writebuffer((PyObject *)self, ptr, segment, ptrptr);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The instance segment count slot for Python v2.
 */
static SIP_SSIZE_T sipSimpleWrapper_getsegcount(sipSimpleWrapper *self,
        SIP_SSIZE_T *lenp)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return 0;

    return ctd->ctd_segcount((PyObject *)self, ptr, lenp);
}
#endif


#if PY_MAJOR_VERSION < 3
/*
 * The instance char buffer slot for Python v2.
 */
static SIP_SSIZE_T sipSimpleWrapper_getcharbuffer(sipSimpleWrapper *self,
        SIP_SSIZE_T segment, void **ptrptr)
{
    void *ptr;
    const sipClassTypeDef *ctd;

    if ((ptr = getPtrTypeDef(self, &ctd)) == NULL)
        return -1;

    return ctd->ctd_charbuffer((PyObject *)self, ptr, segment, ptrptr);
}
#endif


/*
 * The instance dealloc slot.
 */
static void sipSimpleWrapper_dealloc(sipSimpleWrapper *self)
{
    forgetObject(self);

    /*
     * Now that the C++ object no longer exists we can tidy up the Python
     * object.  We used to do this first but that meant lambda slots were
     * removed too soon (if they were connected to QObject.destroyed()).
     */
    sipSimpleWrapper_clear(self);

    /* Call the standard super-type dealloc. */
    PyBaseObject_Type.tp_dealloc((PyObject *)self);
}


/*
 * The type call slot.  Note that keyword arguments aren't supported.
 */
static PyObject *slot_call(PyObject *self,PyObject *args,PyObject *kw)
{
    PyObject *(*f)(PyObject *,PyObject *);

    f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self, call_slot);

    assert(f != NULL);

    return f(self,args);
}


/*
 * The sequence type item slot.
 */
static PyObject *slot_sq_item(PyObject *self, SIP_SSIZE_T n)
{
    PyObject *(*f)(PyObject *,PyObject *);
    PyObject *arg, *res;

#if PY_MAJOR_VERSION >= 3
    arg = PyLong_FromSsize_t(n);
#elif PY_VERSION_HEX >= 0x02050000
    arg = PyInt_FromSsize_t(n);
#else
    arg = PyInt_FromLong(n);
#endif

    if (arg == NULL)
        return NULL;

    f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self, getitem_slot);

    assert(f != NULL);

    res = f(self,arg);

    Py_DECREF(arg);

    return res;
}


/*
 * The mapping type assign subscript slot.
 */
static int slot_mp_ass_subscript(PyObject *self, PyObject *key,
        PyObject *value)
{
    return objobjargprocSlot(self, key, value,
            (value != NULL ? setitem_slot : delitem_slot));
}


/*
 * The sequence type assign item slot.
 */
static int slot_sq_ass_item(PyObject *self, SIP_SSIZE_T i, PyObject *o)
{
    return ssizeobjargprocSlot(self, i, o,
            (o != NULL ? setitem_slot : delitem_slot));
}


/*
 * The type rich compare slot.
 */
static PyObject *slot_richcompare(PyObject *self, PyObject *arg, int op)
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
    if ((f = (PyObject *(*)(PyObject *,PyObject *))findSlot(self, st)) == NULL)
    {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return f(self, arg);
}


/*
 * The instance getattro slot.
 */
static PyObject *sipSimpleWrapper_getattro(PyObject *self, PyObject *name)
{
    if (add_all_lazy_attrs(((sipWrapperType *)Py_TYPE(self))->type) < 0)
        return NULL;

    return PyObject_GenericGetAttr(self, name);
}


/*
 * The instance setattro slot.
 */
static int sipSimpleWrapper_setattro(PyObject *self, PyObject *name,
        PyObject *value)
{
    if (add_all_lazy_attrs(((sipWrapperType *)Py_TYPE(self))->type) < 0)
        return -1;

    return PyObject_GenericSetAttr(self, name, value);
}


/*
 * The __dict__ getter.
 */
static PyObject *sipSimpleWrapper_get_dict(PyObject *self, void *closure)
{
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    /* Create the dictionary if needed. */
    if (sw->dict == NULL)
    {
        sw->dict = PyDict_New();

        if (sw->dict == NULL)
            return NULL;
    }

    Py_INCREF(sw->dict);
    return sw->dict;
}


/*
 * The __dict__ setter.
 */
static int sipSimpleWrapper_set_dict(PyObject *self, PyObject *value,
        void *closure)
{
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    /* Check that any new value really is a dictionary. */
    if (value != NULL && !PyDict_Check(value))
    {
        PyErr_Format(PyExc_TypeError,
                "__dict__ must be set to a dictionary, not a '%s'",
                Py_TYPE(value)->tp_name);
        return -1;
    }

    Py_XDECREF(sw->dict);
    
    Py_XINCREF(value);
    sw->dict = value;

    return 0;
}


/*
 * The table of getters and setters.
 */
static PyGetSetDef sipSimpleWrapper_getset[] = {
    {(char *)"__dict__", sipSimpleWrapper_get_dict, sipSimpleWrapper_set_dict,
            NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};


/*
 * The type data structure.  Note that we pretend to be a mapping object and a
 * sequence object at the same time.  Python will choose one over another,
 * depending on the context, but we implement as much as we can and don't make
 * assumptions about which Python will choose.
 */
sipWrapperType sipSimpleWrapper_Type = {
#if !defined(STACKLESS)
    {
#endif
        {
            PyVarObject_HEAD_INIT(&sipWrapperType_Type, 0)
            "sip.simplewrapper",    /* tp_name */
            sizeof (sipSimpleWrapper),  /* tp_basicsize */
            0,              /* tp_itemsize */
            (destructor)sipSimpleWrapper_dealloc,   /* tp_dealloc */
            0,              /* tp_print */
            0,              /* tp_getattr */
            0,              /* tp_setattr */
            0,              /* tp_reserved (Python v3), tp_compare (Python v2) */
            0,              /* tp_repr */
            0,              /* tp_as_number */
            0,              /* tp_as_sequence */
            0,              /* tp_as_mapping */
            0,              /* tp_hash */
            0,              /* tp_call */
            0,              /* tp_str */
            sipSimpleWrapper_getattro,  /* tp_getattro */
            sipSimpleWrapper_setattro,  /* tp_setattro */
            0,              /* tp_as_buffer */
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,  /* tp_flags */
            0,              /* tp_doc */
            (traverseproc)sipSimpleWrapper_traverse,    /* tp_traverse */
            (inquiry)sipSimpleWrapper_clear,    /* tp_clear */
            0,              /* tp_richcompare */
            0,              /* tp_weaklistoffset */
            0,              /* tp_iter */
            0,              /* tp_iternext */
            0,              /* tp_methods */
            0,              /* tp_members */
            sipSimpleWrapper_getset,    /* tp_getset */
            0,              /* tp_base */
            0,              /* tp_dict */
            0,              /* tp_descr_get */
            0,              /* tp_descr_set */
            offsetof(sipSimpleWrapper, dict),   /* tp_dictoffset */
            (initproc)sipSimpleWrapper_init,    /* tp_init */
            0,              /* tp_alloc */
            (newfunc)sipSimpleWrapper_new,  /* tp_new */
            0,              /* tp_free */
        },
#if !defined(STACKLESS)
    },
#endif
    0,
    0
};


/*****************************************************************************
 * The Python type that is the super-type for all C++ wrapper types that
 * support parent/child relationships.
 *****************************************************************************/

/*
 * The instance clear slot.
 */
static int sipWrapper_clear(sipWrapper *self)
{
    int vret;
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;

    vret = sipSimpleWrapper_clear(sw);

    /* Remove any slots connected via a proxy. */
    if (sipQtSupport != NULL && sipIsPyOwned(sw) && sipPossibleProxy(sw))
    {
        void *tx = sipGetAddress(sw);

        if (tx != NULL)
        {
            sipSlot *slot;
            void *context = NULL;

            while ((slot = sipQtSupport->qt_find_sipslot(tx, &context)) != NULL)
            {
                sip_api_clear_any_slot_reference(slot);

                if (context == NULL)
                    break;
            }
        }
    }

    /* Detach children (which will be owned by C/C++). */
    while ((sw = (sipSimpleWrapper *)self->first_child) != NULL)
    {
        /*
         * Although this object is being garbage collected it doesn't follow
         * that it's children should be.  So we make sure that the child stays
         * alive and remember we have done so.
         */
        Py_INCREF(sw);
        sipSetCppHasRef(sw);

        removeFromParent(self->first_child);
    }

    return vret;
}


/*
 * The instance dealloc slot.
 */
static void sipWrapper_dealloc(sipWrapper *self)
{
    /*
     * We can't simply call the super-type because things have to be done in a
     * certain order.  The first thing is to get rid of the wrapped instance.
     */
    forgetObject((sipSimpleWrapper *)self);

    sipWrapper_clear(self);

    /* Skip the super-type's dealloc. */
    PyBaseObject_Type.tp_dealloc((PyObject *)self);
}


/*
 * The instance traverse slot.
 */
static int sipWrapper_traverse(sipWrapper *self, visitproc visit, void *arg)
{
    int vret;
    sipSimpleWrapper *sw = (sipSimpleWrapper *)self;
    sipWrapper *w;

    if ((vret = sipSimpleWrapper_traverse(sw, visit, arg)) != 0)
        return vret;

    /* This should be handwritten code in PyQt. */
    if (sipQtSupport != NULL && sipIsPyOwned(sw))
    {
        void *tx = sipGetAddress(sw);

        if (tx != NULL)
        {
            sipSlot *slot;
            void *context = NULL;

            while ((slot = sipQtSupport->qt_find_sipslot(tx, &context)) != NULL)
            {
                if ((vret = sip_api_visit_slot(slot, visit, arg)) != 0)
                    return vret;

                if (context == NULL)
                    break;
            }
        }
    }

    for (w = self->first_child; w != NULL; w = w->sibling_next)
    {
        /*
         * We don't traverse if the wrapper is a child of itself.  We do this
         * so that wrapped objects returned by virtual methods with the
         * /Factory/ don't have those objects collected.  This then means that
         * plugins implemented in Python have a chance of working.
         */
        if (w != self)
            if ((vret = visit((PyObject *)w, arg)) != 0)
                return vret;
    }

    return 0;
}


/*
 * The type data structure.
 */
static sipWrapperType sipWrapper_Type = {
#if !defined(STACKLESS)
    {
#endif
        {
            PyVarObject_HEAD_INIT(&sipWrapperType_Type, 0)
            "sip.wrapper",  /* tp_name */
            sizeof (sipWrapper),    /* tp_basicsize */
            0,              /* tp_itemsize */
            (destructor)sipWrapper_dealloc, /* tp_dealloc */
            0,              /* tp_print */
            0,              /* tp_getattr */
            0,              /* tp_setattr */
            0,              /* tp_reserved (Python v3), tp_compare (Python v2) */
            0,              /* tp_repr */
            0,              /* tp_as_number */
            0,              /* tp_as_sequence */
            0,              /* tp_as_mapping */
            0,              /* tp_hash */
            0,              /* tp_call */
            0,              /* tp_str */
            0,              /* tp_getattro */
            0,              /* tp_setattro */
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
            0,              /* tp_dictoffset */
            0,              /* tp_init */
            0,              /* tp_alloc */
            0,              /* tp_new */
            0,              /* tp_free */
        },
#if !defined(STACKLESS)
    },
#endif
    0,
    0
};


/*
 * Add the slots for a class type and all its super-types.
 */
static void addClassSlots(sipWrapperType *wt, sipClassTypeDef *ctd)
{
    /* Add the buffer interface. */
#if PY_MAJOR_VERSION >= 3
    if (ctd->ctd_getbuffer != NULL)
        wt->super.as_buffer.bf_getbuffer = (getbufferproc)sipSimpleWrapper_getbuffer;

    if (ctd->ctd_releasebuffer != NULL)
        wt->super.as_buffer.bf_releasebuffer = (releasebufferproc)sipSimpleWrapper_releasebuffer;
#else
    if (ctd->ctd_readbuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getreadbuffer = (readbufferproc)sipSimpleWrapper_getreadbuffer;
#else
        wt->super.as_buffer.bf_getreadbuffer = (getreadbufferproc)sipSimpleWrapper_getreadbuffer;
#endif

    if (ctd->ctd_writebuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getwritebuffer = (writebufferproc)sipSimpleWrapper_getwritebuffer;
#else
        wt->super.as_buffer.bf_getwritebuffer = (getwritebufferproc)sipSimpleWrapper_getwritebuffer;
#endif

    if (ctd->ctd_segcount != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getsegcount = (segcountproc)sipSimpleWrapper_getsegcount;
#else
        wt->super.as_buffer.bf_getsegcount = (getsegcountproc)sipSimpleWrapper_getsegcount;
#endif

    if (ctd->ctd_charbuffer != NULL)
#if PY_VERSION_HEX >= 0x02050000
        wt->super.as_buffer.bf_getcharbuffer = (charbufferproc)sipSimpleWrapper_getcharbuffer;
#else
        wt->super.as_buffer.bf_getcharbuffer = (getcharbufferproc)sipSimpleWrapper_getcharbuffer;
#endif
#endif

    /* Add the slots for this type. */
    if (ctd->ctd_pyslots != NULL)
        addTypeSlots((PyTypeObject *)wt, &wt->super.as_number,
                &wt->super.as_sequence, &wt->super.as_mapping,
                ctd->ctd_pyslots);
}


/*
 * Add the slot handler for each slot present in the type.
 */
static void addTypeSlots(PyTypeObject *to, PyNumberMethods *nb,
        PySequenceMethods *sq, PyMappingMethods *mp, sipPySlotDef *slots)
{
    void *f;

    while ((f = slots->psd_func) != NULL)
        switch (slots++->psd_type)
        {
        case str_slot:
            to->tp_str = (reprfunc)f;
            break;

        case int_slot:
            if (nb != NULL)
                nb->nb_int = (unaryfunc)f;
            break;

#if PY_MAJOR_VERSION < 3
        case long_slot:
            if (nb != NULL)
                nb->nb_long = (unaryfunc)f;
            break;
#endif

        case float_slot:
            if (nb != NULL)
                nb->nb_float = (unaryfunc)f;
            break;

        case len_slot:
            if (mp != NULL)
#if PY_VERSION_HEX >= 0x02050000
                mp->mp_length = (lenfunc)f;
#else
                mp->mp_length = (inquiry)f;
#endif
            if (sq != NULL)
#if PY_VERSION_HEX >= 0x02050000
                sq->sq_length = (lenfunc)f;
#else
                sq->sq_length = (inquiry)f;
#endif
            break;

        case contains_slot:
            if (sq != NULL)
                sq->sq_contains = (objobjproc)f;
            break;

        case add_slot:
            if (nb != NULL)
                nb->nb_add = (binaryfunc)f;
            break;

        case concat_slot:
            if (sq != NULL)
                sq->sq_concat = (binaryfunc)f;
            break;

        case sub_slot:
            if (nb != NULL)
                nb->nb_subtract = (binaryfunc)f;
            break;

        case mul_slot:
            if (nb != NULL)
                nb->nb_multiply = (binaryfunc)f;
            break;

        case repeat_slot:
            if (sq != NULL)
#if PY_VERSION_HEX >= 0x02050000
                sq->sq_repeat = (ssizeargfunc)f;
#else
                sq->sq_repeat = (intargfunc)f;
#endif
            break;

        case div_slot:
            if (nb != NULL)
            {
                nb->nb_true_divide = (binaryfunc)f;
#if PY_MAJOR_VERSION < 3
                nb->nb_divide = (binaryfunc)f;
#endif
            }
            break;

        case mod_slot:
            if (nb != NULL)
                nb->nb_remainder = (binaryfunc)f;
            break;

        case floordiv_slot:
            if (nb != NULL)
                nb->nb_floor_divide = (binaryfunc)f;
            break;

        case truediv_slot:
            if (nb != NULL)
                nb->nb_true_divide = (binaryfunc)f;
            break;

        case and_slot:
            if (nb != NULL)
                nb->nb_and = (binaryfunc)f;
            break;

        case or_slot:
            if (nb != NULL)
                nb->nb_or = (binaryfunc)f;
            break;

        case xor_slot:
            if (nb != NULL)
                nb->nb_xor = (binaryfunc)f;
            break;

        case lshift_slot:
            if (nb != NULL)
                nb->nb_lshift = (binaryfunc)f;
            break;

        case rshift_slot:
            if (nb != NULL)
                nb->nb_rshift = (binaryfunc)f;
            break;

        case iadd_slot:
            if (nb != NULL)
                nb->nb_inplace_add = (binaryfunc)f;
            break;

        case iconcat_slot:
            if (sq != NULL)
                sq->sq_inplace_concat = (binaryfunc)f;
            break;

        case isub_slot:
            if (nb != NULL)
                nb->nb_inplace_subtract = (binaryfunc)f;
            break;

        case imul_slot:
            if (nb != NULL)
                nb->nb_inplace_multiply = (binaryfunc)f;
            break;

        case irepeat_slot:
            if (sq != NULL)
#if PY_VERSION_HEX >= 0x02050000
                sq->sq_inplace_repeat = (ssizeargfunc)f;
#else
                sq->sq_inplace_repeat = (intargfunc)f;
#endif
            break;

        case idiv_slot:
            if (nb != NULL)
            {
                nb->nb_inplace_true_divide = (binaryfunc)f;
#if PY_MAJOR_VERSION < 3
                nb->nb_inplace_divide = (binaryfunc)f;
#endif
            }
            break;

        case imod_slot:
            if (nb != NULL)
                nb->nb_inplace_remainder = (binaryfunc)f;
            break;

        case ifloordiv_slot:
            if (nb != NULL)
                nb->nb_inplace_floor_divide = (binaryfunc)f;
            break;

        case itruediv_slot:
            if (nb != NULL)
                nb->nb_inplace_true_divide = (binaryfunc)f;
            break;

        case iand_slot:
            if (nb != NULL)
                nb->nb_inplace_and = (binaryfunc)f;
            break;

        case ior_slot:
            if (nb != NULL)
                nb->nb_inplace_or = (binaryfunc)f;
            break;

        case ixor_slot:
            if (nb != NULL)
                nb->nb_inplace_xor = (binaryfunc)f;
            break;

        case ilshift_slot:
            if (nb != NULL)
                nb->nb_inplace_lshift = (binaryfunc)f;
            break;

        case irshift_slot:
            if (nb != NULL)
                nb->nb_inplace_rshift = (binaryfunc)f;
            break;

        case invert_slot:
            if (nb != NULL)
                nb->nb_invert = (unaryfunc)f;
            break;

        case call_slot:
            to->tp_call = slot_call;
            break;

        case getitem_slot:
            if (mp != NULL)
                mp->mp_subscript = (binaryfunc)f;
            if (sq != NULL)
                sq->sq_item = slot_sq_item;
            break;

        case setitem_slot:
        case delitem_slot:
            if (mp != NULL)
                mp->mp_ass_subscript = slot_mp_ass_subscript;
            if (sq != NULL)
                sq->sq_ass_item = slot_sq_ass_item;
            break;

        case lt_slot:
        case le_slot:
        case eq_slot:
        case ne_slot:
        case gt_slot:
        case ge_slot:
            to->tp_richcompare = slot_richcompare;
            break;

#if PY_MAJOR_VERSION < 3
        case cmp_slot:
            to->tp_compare = (cmpfunc)f;
            break;
#endif

        case bool_slot:
            if (nb != NULL)
#if PY_MAJOR_VERSION >= 3
                nb->nb_bool = (inquiry)f;
#else
                nb->nb_nonzero = (inquiry)f;
#endif
            break;

        case neg_slot:
            if (nb != NULL)
                nb->nb_negative = (unaryfunc)f;
            break;

        case repr_slot:
            to->tp_repr = (reprfunc)f;
            break;

        case hash_slot:
            to->tp_hash = (hashfunc)f;
            break;

        case pos_slot:
            if (nb != NULL)
                nb->nb_positive = (unaryfunc)f;
            break;

        case abs_slot:
            if (nb != NULL)
                nb->nb_absolute = (unaryfunc)f;
            break;

#if PY_VERSION_HEX >= 0x02050000
        case index_slot:
            if (nb != NULL)
                nb->nb_index = (unaryfunc)f;
            break;
#endif

        case iter_slot:
            to->tp_iter = (getiterfunc)f;
            break;

        case next_slot:
            to->tp_iternext = (iternextfunc)f;
            break;
        }
}


/*
 * Remove the object from the map and call the C/C++ dtor if we own the
 * instance.
 */
static void forgetObject(sipSimpleWrapper *sw)
{
    const sipClassTypeDef *ctd;

    /*
     * This is needed because we release the GIL when calling a C++ dtor.
     * Without it the cyclic garbage collector can be invoked from another
     * thread resulting in a crash.
     */
    PyObject_GC_UnTrack((PyObject *)sw);

    if (getPtrTypeDef(sw, &ctd) != NULL)
    {
        /*
         * Remove the object from the map before calling the class specific
         * dealloc code.  This code calls the C++ dtor and may result in
         * further calls that pass the instance as an argument.  If this is
         * still in the map then it's reference count would be increased (to
         * one) and bad things happen when it drops back to zero again.  (An
         * example is PyQt events generated during the dtor call being passed
         * to an event filter implemented in Python.)  By removing it from the
         * map first we ensure that a new Python object is created.
         */
        sipOMRemoveObject(&cppPyMap, sw);

        /* Call the C++ dtor if there is one. */
        if (ctd->ctd_dealloc != NULL)
            ctd->ctd_dealloc(sw);
    }
}


/*
 * If the given name is that of a typedef then the corresponding type is
 * returned.
 */
static const char *sip_api_resolve_typedef(const char *name)
{
    const sipExportedModuleDef *em;

    /*
     * Note that if the same name is defined as more than one type (which is
     * possible if more than one completely independent modules are being
     * used) then we might pick the wrong one.
     */
    for (em = moduleList; em != NULL; em = em->em_next)
    {
        if (em->em_nrtypedefs > 0)
        {
            sipTypedefDef *tdd;

            tdd = (sipTypedefDef *)bsearch(name, em->em_typedefs,
                    em->em_nrtypedefs, sizeof (sipTypedefDef),
                    compareTypedefName);

            if (tdd != NULL)
                return tdd->tdd_type_name;
        }
    }

    return NULL;
}


/*
 * The bsearch() helper function for searching a sorted typedef table.
 */
static int compareTypedefName(const void *key, const void *el)
{
    return strcmp((const char *)key, ((const sipTypedefDef *)el)->tdd_name);
}


/*
 * Add the given Python object to the given list.  Return 0 if there was no
 * error.
 */
static int addPyObjectToList(sipPyObject **head, PyObject *object)
{
    sipPyObject *po;

    if ((po = sip_api_malloc(sizeof (sipPyObject))) == NULL)
        return -1;

    po->object = object;
    po->next = *head;

    *head = po;

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


/*
 * Visit a slot connected to an object for the cyclic garbage collector.  This
 * is only called externally by PyQt3.
 */
static int sip_api_visit_slot(sipSlot *slot, visitproc visit, void *arg)
{
    /* See if the slot has an extra reference. */
    if (slot->weakSlot == Py_True && slot->pyobj != Py_None)
        return visit(slot->pyobj, arg);

    return 0;
}


/*
 * Clear a slot if it has an extra reference to keep it alive.  This is only
 * called externally by PyQt3.
 */
static void sip_api_clear_any_slot_reference(sipSlot *slot)
{
    if (slot->weakSlot == Py_True)
    {
        PyObject *xref = slot->pyobj;

        /*
         * Replace the slot with None.  We don't use NULL as this has another
         * meaning.
         */
        Py_INCREF(Py_None);
        slot->pyobj = Py_None;

        Py_DECREF(xref);
    }
}


/*
 * Convert a Python object to a character and raise an exception if there was
 * an error.
 */
static char sip_api_bytes_as_char(PyObject *obj)
{
    char ch;

    if (parseBytes_AsChar(obj, &ch) < 0)
    {
        PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                "bytes of length 1 expected not '%s'",
#else
                "string of length 1 expected not '%s'",
#endif
                Py_TYPE(obj)->tp_name);

        return '\0';
    }

    return ch;
}


/*
 * Convert a Python object to a string and raise an exception if there was
 * an error.
 */
static const char *sip_api_bytes_as_string(PyObject *obj)
{
    const char *a;

    if (parseBytes_AsString(obj, &a) < 0)
    {
        PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                "bytes expected not '%s'",
#else
                "string expected not '%s'",
#endif
                Py_TYPE(obj)->tp_name);

        return NULL;
    }

    return a;
}


/*
 * Convert a Python ASCII string object to a character and raise an exception
 * if there was an error.
 */
static char sip_api_string_as_ascii_char(PyObject *obj)
{
    char ch;

    if (parseString_AsASCIIChar(obj, &ch) < 0)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_SIZE(obj) != 1)
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or ASCII string of length 1 expected not '%s'",
#else
                    "string or ASCII unicode of length 1 expected not '%s'",
#endif
                    Py_TYPE(obj)->tp_name);

        return '\0';
    }

    return ch;
}


/*
 * Parse an ASCII character and return it.
 */
static int parseString_AsASCIIChar(PyObject *obj, char *ap)
{
    return parseString_AsEncodedChar(PyUnicode_AsASCIIString(obj), obj, ap);
}


/*
 * Convert a Python Latin-1 string object to a character and raise an exception
 * if there was an error.
 */
static char sip_api_string_as_latin1_char(PyObject *obj)
{
    char ch;

    if (parseString_AsLatin1Char(obj, &ch) < 0)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_SIZE(obj) != 1)
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or Latin-1 string of length 1 expected not '%s'",
#else
                    "string or Latin-1 unicode of length 1 expected not '%s'",
#endif
                    Py_TYPE(obj)->tp_name);

        return '\0';
    }

    return ch;
}


/*
 * Parse a Latin-1 character and return it.
 */
static int parseString_AsLatin1Char(PyObject *obj, char *ap)
{
    return parseString_AsEncodedChar(PyUnicode_AsLatin1String(obj), obj, ap);
}


/*
 * Convert a Python UTF-8 string object to a character and raise an exception
 * if there was an error.
 */
static char sip_api_string_as_utf8_char(PyObject *obj)
{
    char ch;

    if (parseString_AsUTF8Char(obj, &ch) < 0)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(obj) || PyUnicode_GET_SIZE(obj) != 1)
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or UTF-8 string of length 1 expected not '%s'",
#else
                    "string or UTF-8 unicode of length 1 expected not '%s'",
#endif
                    Py_TYPE(obj)->tp_name);

        return '\0';
    }

    return ch;
}


/*
 * Parse a UTF-8 character and return it.
 */
static int parseString_AsUTF8Char(PyObject *obj, char *ap)
{
    return parseString_AsEncodedChar(PyUnicode_AsUTF8String(obj), obj, ap);
}


/*
 * Parse an encoded character and return it.
 */
static int parseString_AsEncodedChar(PyObject *bytes, PyObject *obj, char *ap)
{
    SIP_SSIZE_T size;

    if (bytes == NULL)
    {
        PyErr_Clear();

        return parseBytes_AsChar(obj, ap);
    }

#if PY_MAJOR_VERSION >= 3
    size = PyBytes_GET_SIZE(bytes);
#else
    size = PyString_GET_SIZE(bytes);
#endif

    if (size != 1)
    {
        Py_DECREF(bytes);
        return -1;
    }

#if PY_MAJOR_VERSION >= 3
    *ap = *PyBytes_AS_STRING(bytes);
#else
    *ap = *PyString_AS_STRING(bytes);
#endif

    Py_DECREF(bytes);

    return 0;
}


/*
 * Convert a Python ASCII string object to a string and raise an exception if
 * there was an error.  The object is updated with the one that owns the
 * string.  Note that None is considered an error.
 */
static const char *sip_api_string_as_ascii_string(PyObject **obj)
{
    PyObject *s = *obj;
    const char *a;

    if (s == Py_None || (*obj = parseString_AsASCIIString(s, &a)) == NULL)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(s))
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or ASCII string expected not '%s'",
#else
                    "string or ASCII unicode expected not '%s'",
#endif
                    Py_TYPE(s)->tp_name);

        return NULL;
    }

    return a;
}


/*
 * Parse an ASCII string and return it and a new reference to the object that
 * owns the string.
 */
static PyObject *parseString_AsASCIIString(PyObject *obj, const char **ap)
{
    return parseString_AsEncodedString(PyUnicode_AsASCIIString(obj), obj, ap);
}


/*
 * Convert a Python Latin-1 string object to a string and raise an exception if
 * there was an error.  The object is updated with the one that owns the
 * string.  Note that None is considered an error.
 */
static const char *sip_api_string_as_latin1_string(PyObject **obj)
{
    PyObject *s = *obj;
    const char *a;

    if (s == Py_None || (*obj = parseString_AsLatin1String(s, &a)) == NULL)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(s))
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or Latin-1 string expected not '%s'",
#else
                    "string or Latin-1 unicode expected not '%s'",
#endif
                    Py_TYPE(s)->tp_name);

        return NULL;
    }

    return a;
}


/*
 * Parse a Latin-1 string and return it and a new reference to the object that
 * owns the string.
 */
static PyObject *parseString_AsLatin1String(PyObject *obj, const char **ap)
{
    return parseString_AsEncodedString(PyUnicode_AsLatin1String(obj), obj, ap);
}


/*
 * Convert a Python UTF-8 string object to a string and raise an exception if
 * there was an error.  The object is updated with the one that owns the
 * string.  Note that None is considered an error.
 */
static const char *sip_api_string_as_utf8_string(PyObject **obj)
{
    PyObject *s = *obj;
    const char *a;

    if (s == Py_None || (*obj = parseString_AsUTF8String(s, &a)) == NULL)
    {
        /* Use the exception set if it was an encoding error. */
        if (!PyUnicode_Check(s))
            PyErr_Format(PyExc_TypeError,
#if PY_MAJOR_VERSION >= 3
                    "bytes or UTF-8 string expected not '%s'",
#else
                    "string or UTF-8 unicode expected not '%s'",
#endif
                    Py_TYPE(s)->tp_name);

        return NULL;
    }

    return a;
}


/*
 * Parse a UTF-8 string and return it and a new reference to the object that
 * owns the string.
 */
static PyObject *parseString_AsUTF8String(PyObject *obj, const char **ap)
{
    return parseString_AsEncodedString(PyUnicode_AsUTF8String(obj), obj, ap);
}


/*
 * Parse an encoded string and return it and a new reference to the object that
 * owns the string.
 */
static PyObject *parseString_AsEncodedString(PyObject *bytes, PyObject *obj,
        const char **ap)
{
    if (bytes != NULL)
    {
#if PY_MAJOR_VERSION >= 3
        *ap = PyBytes_AS_STRING(bytes);
#else
        *ap = PyString_AS_STRING(bytes);
#endif

        return bytes;
    }

    PyErr_Clear();

    if (parseBytes_AsString(obj, ap) < 0)
        return NULL;

    Py_INCREF(obj);

    return obj;
}


/*
 * Parse a character array and return it's address and length.
 */
static int parseBytes_AsCharArray(PyObject *obj, const char **ap,
        SIP_SSIZE_T *aszp)
{
    if (obj == Py_None)
    {
        *ap = NULL;
        *aszp = 0;
    }
#if PY_MAJOR_VERSION >= 3
    else if (PyBytes_Check(obj))
    {
        *ap = PyBytes_AS_STRING(obj);
        *aszp = PyBytes_GET_SIZE(obj);
    }
#else
    else if (PyString_Check(obj))
    {
        *ap = PyString_AS_STRING(obj);
        *aszp = PyString_GET_SIZE(obj);
    }
#endif
    else if (PyObject_AsCharBuffer(obj, ap, aszp) < 0)
        return -1;

    return 0;
}


/*
 * Parse a character and return it.
 */
static int parseBytes_AsChar(PyObject *obj, char *ap)
{
    const char *chp;
    SIP_SSIZE_T sz;

#if PY_MAJOR_VERSION >= 3
    if (PyBytes_Check(obj))
    {
        chp = PyBytes_AS_STRING(obj);
        sz = PyBytes_GET_SIZE(obj);
    }
#else
    if (PyString_Check(obj))
    {
        chp = PyString_AS_STRING(obj);
        sz = PyString_GET_SIZE(obj);
    }
#endif
    else if (PyObject_AsCharBuffer(obj, &chp, &sz) < 0)
        return -1;

    if (sz != 1)
        return -1;

    *ap = *chp;

    return 0;
}


/*
 * Parse a character string and return it.
 */
static int parseBytes_AsString(PyObject *obj, const char **ap)
{
    SIP_SSIZE_T sz;

    return parseBytes_AsCharArray(obj, ap, &sz);
}


#if defined(HAVE_WCHAR_H)
/*
 * Convert a Python object to a wide character.
 */
static wchar_t sip_api_unicode_as_wchar(PyObject *obj)
{
    wchar_t ch;

    if (parseWChar(obj, &ch) < 0)
    {
        PyErr_Format(PyExc_ValueError,
#if PY_MAJOR_VERSION >= 3
                "string"
#else
                "unicode string"
#endif
                " of length 1 expected, not %s", Py_TYPE(obj)->tp_name);

        return L'\0';
    }

    return ch;
}


/*
 * Convert a Python object to a wide character string on the heap.
 */
static wchar_t *sip_api_unicode_as_wstring(PyObject *obj)
{
    wchar_t *p;

    if (parseWCharString(obj, &p) < 0)
    {
        PyErr_Format(PyExc_ValueError,
#if PY_MAJOR_VERSION >= 3
                "string"
#else
                "unicode string"
#endif
                " expected, not %s", Py_TYPE(obj)->tp_name);

        return NULL;
    }

    return p;
}


/*
 * Parse a wide character array and return it's address and length.
 */
static int parseWCharArray(PyObject *obj, wchar_t **ap, SIP_SSIZE_T *aszp)
{
    if (obj == Py_None)
    {
        *ap = NULL;
        *aszp = 0;

        return 0;
    }

    if (PyUnicode_Check(obj))
        return convertToWCharArray(obj, ap, aszp);

#if PY_MAJOR_VERSION < 3
    if (PyString_Check(obj))
    {
        int rc;
        PyObject *uobj;

        if ((uobj = PyUnicode_FromObject(obj)) == NULL)
            return -1;

        rc = convertToWCharArray(uobj, ap, aszp);
        Py_DECREF(uobj);

        return rc;
    }
#endif

    return -1;
}


/*
 * Convert a Unicode object to a wide character array and return it's address
 * and length.
 */
static int convertToWCharArray(PyObject *obj, wchar_t **ap, SIP_SSIZE_T *aszp)
{
    SIP_SSIZE_T ulen;
    wchar_t *wc;

    ulen = PyUnicode_GET_SIZE(obj);

    if ((wc = sip_api_malloc(ulen * sizeof (wchar_t))) == NULL)
        return -1;

    ulen = PyUnicode_AsWideChar((PyUnicodeObject *)obj, wc, ulen);

    if (ulen < 0)
    {
        sip_api_free(wc);
        return -1;
    }

    *ap = wc;
    *aszp = ulen;

    return 0;
}


/*
 * Parse a wide character and return it.
 */
static int parseWChar(PyObject *obj, wchar_t *ap)
{
    if (PyUnicode_Check(obj))
        return convertToWChar(obj, ap);

#if PY_MAJOR_VERSION < 3
    if (PyString_Check(obj))
    {
        int rc;
        PyObject *uobj;

        if ((uobj = PyUnicode_FromObject(obj)) == NULL)
            return -1;

        rc = convertToWChar(uobj, ap);
        Py_DECREF(uobj);

        return rc;
    }
#endif

    return -1;
}


/*
 * Convert a Unicode object to a wide character and return it.
 */
static int convertToWChar(PyObject *obj, wchar_t *ap)
{
    if (PyUnicode_GET_SIZE(obj) != 1)
        return -1;

    if (PyUnicode_AsWideChar((PyUnicodeObject *)obj, ap, 1) != 1)
        return -1;

    return 0;
}


/*
 * Parse a wide character string and return a copy on the heap.
 */
static int parseWCharString(PyObject *obj, wchar_t **ap)
{
    if (obj == Py_None)
    {
        *ap = NULL;

        return 0;
    }

    if (PyUnicode_Check(obj))
        return convertToWCharString(obj, ap);

#if PY_MAJOR_VERSION < 3
    if (PyString_Check(obj))
    {
        int rc;
        PyObject *uobj;

        if ((uobj = PyUnicode_FromObject(obj)) == NULL)
            return -1;

        rc = convertToWCharString(uobj, ap);
        Py_DECREF(uobj);

        return rc;
    }
#endif

    return -1;
}


/*
 * Convert a Unicode object to a wide character string and return a copy on
 * the heap.
 */
static int convertToWCharString(PyObject *obj, wchar_t **ap)
{
    SIP_SSIZE_T ulen;
    wchar_t *wc;

    ulen = PyUnicode_GET_SIZE(obj);

    if ((wc = sip_api_malloc((ulen + 1) * sizeof (wchar_t))) == NULL)
        return -1;

    ulen = PyUnicode_AsWideChar((PyUnicodeObject *)obj, wc, ulen);

    if (ulen < 0)
    {
        sip_api_free(wc);
        return -1;
    }

    wc[ulen] = L'\0';

    *ap = wc;

    return 0;
}

#else

/*
 * Convert a Python object to a wide character.
 */
static int sip_api_unicode_as_wchar(PyObject *obj)
{
    raiseNoWChar();

    return 0;
}


/*
 * Convert a Python object to a wide character.
 */
static int *sip_api_unicode_as_wstring(PyObject *obj)
{
    raiseNoWChar();

    return NULL;
}


/*
 * Report the need for absent wide character support.
 */
static void raiseNoWChar()
{
    PyErr_SetString(PyExc_SystemError, "sip built without wchar_t support");
}

#endif
