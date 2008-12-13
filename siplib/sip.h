/*
 * The SIP module interface.
 *
 * @BS_LICENSE@
 */


#ifndef _SIP_H
#define _SIP_H


/*
 * This gets round a problem with Qt's moc and Python v2.3.  Strictly speaking
 * it's a Qt problem but later versions of Python include a fix for it so we
 * might as well too.
 */
#undef slots


#include <Python.h>

/*
 * There is a mis-feature somewhere with the Borland compiler.  This works
 * around it.
 */
#if defined(__BORLANDC__)
#include <rpc.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Sanity check on the Python version. */
#if PY_VERSION_HEX < 0x02030000
#error "This version of SIP requires Python v2.3 or later"
#endif


/*
 * Define the SIP version number.
 */
#define SIP_VERSION         0x@RM_HEXVERSION@
#define SIP_VERSION_STR     "@RM_LATEST@"


/*
 * Define the current API version number.  SIP must handle modules with the
 * same major number and with the same or earlier minor number.  Whenever data
 * structure elements are added they must be appended and the minor number
 * incremented.  Whenever data structure elements are removed or the order
 * changed then the major number must be incremented and the minor number set
 * to 0.
 *
 * History:
 *
 * 4.0  Removed all the previously deprecated parts of the API.
 *      Added the sipSimpleWrapper_Type, sipWrapper_Type, sipWrapperType_Type
 *      and sipVoidPtr_Type type objects.
 *      Added sip_api_register_py_type().
 *      Replaced sip_api_add_class_instance() and
 *      sip_api_add_mapped_type_instance() with sip_api_add_type_instance().
 *      Replaced sip_api_raise_class_exception() and
 *      sip_api_raise_sub_class_exception() with
 *      sip_api_raise_type_exception().
 *      Replaced sip_api_is_exact_wrapped_type() with a macro.
 *      Deprecated sipWrapper_Check().
 *
 * 3.8  Added sip_api_register_meta_type() and sip_api_deprecated().
 *      Added qt_register_meta_type() to the Qt support API.
 *      The C/C++ names of enums and types are now always defined in the
 *      relevant structures and don't default to the Python name.
 *      Added the 'XE' format characters to sip_api_parse_args().
 *
 * 3.7  Added sip_api_convert_from_const_void_ptr(),
 *      sip_api_convert_from_void_ptr_and_size() and
 *      sip_api_convert_from_const_void_ptr_and_size().
 *      Added the 'g' and 'G' format characters (to replace the now deprecated
 *      'a' and 'A' format characters) to sip_api_build_result(),
 *      sip_api_call_method() and sip_api_parse_result().
 *      Added the 'k' and 'K' format characters (to replace the now deprecated
 *      'a' and 'A' format characters) to sip_api_parse_args().
 *      Added sip_api_invoke_slot().
 *      Added sip_api_parse_type().
 *      Added sip_api_is_exact_wrapped_type().
 *      Added sip_api_assign_instance().
 *      Added sip_api_assign_mapped_type().
 *      Added the td_assign and td_qt fields to the sipTypeDef structure.
 *      Added the mt_assign field to the sipMappedType structure.
 *
 * 3.6  Added the 'g' format character to sip_api_parse_args().
 *
 * 3.5  Added the td_pickle field to the sipTypeDef structure.
 *      Added sip_api_transfer_break().
 *
 * 3.4  Added qt_find_connection() to the Qt support API.
 *      Added sip_api_string_as_char(), sip_api_unicode_as_wchar(),
 *      sip_api_unicode_as_wstring(), sip_api_find_class(),
 *      sip_api_find_named_enum() and sip_api_parse_signature().
 *      Added the 'A', 'w' and 'x' format characters to sip_api_parse_args(),
 *      sip_api_parse_result(), sip_api_build_result() and
 *      sip_api_call_method().
 *
 * 3.3  Added sip_api_register_int_types().
 *
 * 3.2  Added sip_api_export_symbol() and sip_api_import_symbol().
 *
 * 3.1  Added sip_api_add_mapped_type_instance().
 *
 * 3.0  Moved the Qt support out of the sip module and into PyQt.  This is
 *      such a dramatic change that there is no point in attempting to maintain
 *      backwards compatibility.
 *
 * 2.0  Added the td_flags field to the sipTypeDef structure.
 *      Added the first_child, sibling_next, sibling_prev and parent fields to
 *      the sipWrapper structure.
 *      Added the td_traverse and td_clear fields to the sipTypeDef structure.
 *      Added the em_api_minor field to the sipExportedModuleDef structure.
 *      Added sip_api_bad_operator_arg().
 *      Added sip_api_wrapper_check().
 *
 * 1.1  Added support for __pos__ and __abs__.
 *
 * 1.0  Removed all deprecated parts of the API.
 *      Removed the td_proxy field from the sipTypeDef structure.
 *      Removed the create proxy function from the 'q' and 'y' format
 *      characters to sip_api_parse_args().
 *      Removed sip_api_emit_to_slot().
 *      Reworked the enum related structures.
 *
 * 0.2  Added the 'H' format character to sip_api_parse_args().
 *
 * 0.1  Added sip_api_add_class_instance().
 *      Added the 't' format character to sip_api_parse_args().
 *      Deprecated the 'J' and 'K' format characters to sip_api_parse_result().
 *
 * 0.0  Original version.
 */
#define SIP_API_MAJOR_NR    4
#define SIP_API_MINOR_NR    0


/* Some compatibility stuff to help with handwritten code for SIP v3. */
#if !defined(ANY)
#define ANY     void
#endif


/* Some Python compatibility stuff. */
#if PY_VERSION_HEX >= 0x02050000
#define SIP_SSIZE_T         Py_ssize_t
#else
#define SIP_SSIZE_T         int
#endif


/*
 * The mask that can be passed to sipTrace().
 */
#define SIP_TRACE_CATCHERS  0x0001
#define SIP_TRACE_CTORS     0x0002
#define SIP_TRACE_DTORS     0x0004
#define SIP_TRACE_INITS     0x0008
#define SIP_TRACE_DEALLOCS  0x0010
#define SIP_TRACE_METHODS   0x0020


/*
 * Hide some thread dependent stuff.
 */
#ifdef WITH_THREAD
typedef PyGILState_STATE sip_gilstate_t;
#define SIP_RELEASE_GIL(gs) PyGILState_Release(gs);
#define SIP_BLOCK_THREADS   {PyGILState_STATE sipGIL = PyGILState_Ensure();
#define SIP_UNBLOCK_THREADS PyGILState_Release(sipGIL);}
#else
typedef int sip_gilstate_t;
#define SIP_RELEASE_GIL(gs)
#define SIP_BLOCK_THREADS
#define SIP_UNBLOCK_THREADS
#endif


/*
 * The meta-type of a wrapper type.
 */
typedef struct _sipWrapperType {
    /*
     * The super-metatype.  This must be first in the structure so that it can
     * be cast to a PyTypeObject *.
     */
    PyHeapTypeObject super;

    /* The additional type information. */
    struct _sipTypeDef *type;

    /* The list of init extenders. */
    struct _sipInitExtenderDef *iextend;

    /* The optional PyQt defined Q_OBJECT information. */
    void *qt_qobject;
} sipWrapperType;


/*
 * The type of a simple C/C++ wrapper object.
 */
typedef struct _sipSimpleWrapper {
    PyObject_HEAD

    union {
        /* C/C++ object pointer. */
        void *cppPtr;

        /* Access function. */
        void *(*afPtr)();
    } u;

    /* Object flags. */
    int flags;

    /* For the user to use. */
    PyObject *user;

    /* The instance dictionary. */
    PyObject *dict;

    /* Next object at this address. */
    struct _sipSimpleWrapper *next;
} sipSimpleWrapper;


/*
 * The type of a C/C++ wrapper object that supports parent/child relationships.
 */
typedef struct _sipWrapper {
    /* The super-type. */
    sipSimpleWrapper super;

    /* Python signal list (complex). */
    struct _sipPySig *pySigList;

    /* First child object. */
    struct _sipWrapper *first_child;

    /* Next sibling. */
    struct _sipWrapper *sibling_next;

    /* Previous sibling. */
    struct _sipWrapper *sibling_prev;

    /* Owning object. */
    struct _sipWrapper *parent;
} sipWrapper;


/*
 * Some convenient function pointers.
 */
typedef void *(*sipInitFunc)(sipSimpleWrapper *, PyObject *, sipWrapper **,
        int *);
typedef int (*sipTraverseFunc)(void *, visitproc, void *);
typedef int (*sipClearFunc)(void *);
typedef SIP_SSIZE_T (*sipBufferFunc)(PyObject *, void *, SIP_SSIZE_T, void **);
typedef SIP_SSIZE_T (*sipSegCountFunc)(PyObject *, void *, SIP_SSIZE_T *);
typedef void (*sipDeallocFunc)(sipSimpleWrapper *);
typedef void *(*sipCastFunc)(void *, struct _sipTypeDef *);
typedef sipWrapperType *(*sipSubClassConvertFunc)(void **);
typedef int (*sipConvertToFunc)(PyObject *, void **, int *, PyObject *);
typedef PyObject *(*sipConvertFromFunc)(void *, PyObject *);
typedef int (*sipVirtHandlerFunc)(void *, PyObject *, ...);
typedef int (*sipEmitFunc)(sipWrapper *, PyObject *);
typedef void (*sipReleaseFunc)(void *, int);
typedef PyObject *(*sipPickleFunc)(void *);
typedef void (*sipAssignFunc)(void *, const void *);


/*
 * The information describing an encoded class ID.
 */
typedef struct _sipEncodedClassDef {
    /* The class number. */
    unsigned sc_class:16;

    /* The module number (255 for this one). */
    unsigned sc_module:8;

    /* A context specific flag. */
    unsigned sc_flag:1;
} sipEncodedClassDef;


/*
 * The information describing an enum member.
 */
typedef struct _sipEnumMemberDef {
    /* The member name. */
    const char *em_name;

    /* The member value. */
    int em_val;

    /* The member enum, -ve if anonymous. */
    int em_enum;
} sipEnumMemberDef;


/*
 * The information describing a named enum.
 */
typedef struct _sipEnumDef {
    /* The Python name of the enum. */
    int e_name;

    /* The C/C++ name of the enum. */
    int e_cname;

    /* The scoping type, -1 if it is defined at the module level. */
    int e_scope;

    /* The Python slots. */
    struct _sipPySlotDef *e_pyslots;
} sipEnumDef;


/*
 * The information describing static instances.
 */
typedef struct _sipInstancesDef {
    /* The types. */
    struct _sipTypeInstanceDef *id_type;

    /* The void *. */
    struct _sipVoidPtrInstanceDef *id_voidp;

    /* The chars. */
    struct _sipCharInstanceDef *id_char;

    /* The strings. */
    struct _sipStringInstanceDef *id_string;

    /* The ints. */
    struct _sipIntInstanceDef *id_int;

    /* The longs. */
    struct _sipLongInstanceDef *id_long;

    /* The unsigned longs. */
    struct _sipUnsignedLongInstanceDef *id_ulong;

    /* The long longs. */
    struct _sipLongLongInstanceDef *id_llong;

    /* The unsigned long longs. */
    struct _sipUnsignedLongLongInstanceDef *id_ullong;

    /* The doubles. */
    struct _sipDoubleInstanceDef *id_double;

    /* The enums. */
    struct _sipEnumInstanceDef *id_enum;
} sipInstancesDef;


/*
 * The information describing a type initialiser extender.
 */
typedef struct _sipInitExtenderDef {
    /* The extender function. */
    sipInitFunc ie_extender;

    /* The class being extended. */
    sipEncodedClassDef ie_class;

    /* The next extender for this class. */
    struct _sipInitExtenderDef *ie_next;
} sipInitExtenderDef;


/*
 * The information describing a sub-class convertor.
 */
typedef struct _sipSubClassConvertorDef {
    /* The convertor. */
    sipSubClassConvertFunc scc_convertor;

    /* The encoded base type. */
    sipEncodedClassDef scc_base;

    /* The base type. */
    struct _sipTypeDef *scc_basetype;
} sipSubClassConvertorDef;


/*
 * The different Python slot types.
 */
typedef enum {
    str_slot,           /* __str__ */
    int_slot,           /* __int__ */
    long_slot,          /* __long__ */
    float_slot,         /* __float__ */
    len_slot,           /* __len__ */
    contains_slot,      /* __contains__ */
    add_slot,           /* __add__ for number */
    concat_slot,        /* __add__ for sequence types */
    sub_slot,           /* __sub__ */
    mul_slot,           /* __mul__ for number types */
    repeat_slot,        /* __mul__ for sequence types */
    div_slot,           /* __div__ */
    mod_slot,           /* __mod__ */
    and_slot,           /* __and__ */
    or_slot,            /* __or__ */
    xor_slot,           /* __xor__ */
    lshift_slot,        /* __lshift__ */
    rshift_slot,        /* __rshift__ */
    iadd_slot,          /* __iadd__ for number types */
    iconcat_slot,       /* __iadd__ for sequence types */
    isub_slot,          /* __isub__ */
    imul_slot,          /* __imul__ for number types */
    irepeat_slot,       /* __imul__ for sequence types */
    idiv_slot,          /* __idiv__ */
    imod_slot,          /* __imod__ */
    iand_slot,          /* __iand__ */
    ior_slot,           /* __ior__ */
    ixor_slot,          /* __ixor__ */
    ilshift_slot,       /* __ilshift__ */
    irshift_slot,       /* __irshift__ */
    invert_slot,        /* __invert__ */
    call_slot,          /* __call__ */
    getitem_slot,       /* __getitem__ */
    setitem_slot,       /* __setitem__ */
    delitem_slot,       /* __delitem__ */
    lt_slot,            /* __lt__ */
    le_slot,            /* __le__ */
    eq_slot,            /* __eq__ */
    ne_slot,            /* __ne__ */
    gt_slot,            /* __gt__ */
    ge_slot,            /* __ge__ */
    cmp_slot,           /* __cmp__ */
    nonzero_slot,       /* __nonzero__ */
    neg_slot,           /* __neg__ */
    repr_slot,          /* __repr__ */
    hash_slot,          /* __hash__ */
    pos_slot,           /* __pos__ */
    abs_slot            /* __abs__ */
} sipPySlotType;


/*
 * The information describing a Python slot function.
 */
typedef struct _sipPySlotDef {
    /* The function. */
    void *psd_func;

    /* The type. */
    sipPySlotType psd_type;
} sipPySlotDef;


/*
 * The information describing a Python slot extender.
 */
typedef struct _sipPySlotExtenderDef {
    /* The function. */
    void *pse_func;

    /* The type. */
    sipPySlotType pse_type;

    /* The encoded class. */
    sipEncodedClassDef pse_class;
} sipPySlotExtenderDef;


/*
 * This enumerates the different dynamic signal argument types.
 */
typedef enum {
    unknown_sat,
    char_sat,
    uchar_sat,
    string_sat,
    ustring_sat,
    short_sat,
    ushort_sat,
    int_sat,
    uint_sat,
    long_sat,
    ulong_sat,
    longlong_sat,
    ulonglong_sat,
    float_sat,
    double_sat,
    enum_sat,
    bool_sat,
    void_sat,
    class_sat,
    classp_sat,
    mtype_sat,
    mtypep_sat,
    qvariant_sat,
    qvariantp_sat,
    pyobject_sat,
    schar_sat,
    sstring_sat,
    wchar_sat,
    wstring_sat
} sipSigArgType;


/*
 * The information describing a typedef.
 */
typedef struct _sipTypedefDef {
    /* The typedef name. */
    const char *tdd_name;

    /* The typedef type. */
    sipSigArgType tdd_type;

    /* The type name for enum_sat, class_sat and mtype_sat. */
    const char *tdd_type_name;

    /* The defining module, NULL if the current one. */
    const char *tdd_mod_name;
} sipTypedefDef;


/*
 * The information describing a type, either a C++ class (or C struct), a C++
 * namespace, or a mapped type.
 */
typedef struct _sipTypeDef {
    /* The version number. */
    int td_version;

    /* The module. */
    struct _sipExportedModuleDef *td_module;

    /* Type flags, see the sipType*() macros. */
    int td_flags;

    /* The C/C++ name of the type. */
    int td_cname;

    /* The Python name of the type. */
    int td_name;

    /* The Python type object. */
    sipWrapperType *td_wrapper_type;

    /*
     * The meta-type name, -1 to use the meta-type of the first super-type
     * (normally sipWrapperType).
     */
    int td_metatype;

    /* The super-type name, -1 to use sipWrapper. */
    int td_supertype;

    /*
     * The scoping type or the namespace this is extending if it is a namespace
     * extender.
     */
    sipEncodedClassDef td_scope;

    /* The super-types. */
    sipEncodedClassDef *td_supers;

    /* The table of Python slots. */
    sipPySlotDef *td_pyslots;

    /* The number of lazy methods. */
    int td_nrmethods;

    /* The table of lazy methods. */
    PyMethodDef *td_methods;

    /* The number of lazy enum members. */
    int td_nrenummembers;

    /* The table of lazy enum members. */
    sipEnumMemberDef *td_enummembers;

    /* The variable table. */
    PyMethodDef *td_variables;

    /* The initialisation function. */
    sipInitFunc td_init;

    /* The traverse function. */
    sipTraverseFunc td_traverse;

    /* The clear function. */
    sipClearFunc td_clear;

    /* The read buffer function. */
    sipBufferFunc td_readbuffer;

    /* The write buffer function. */
    sipBufferFunc td_writebuffer;

    /* The segment count function. */
    sipSegCountFunc td_segcount;

    /* The char buffer function. */
    sipBufferFunc td_charbuffer;

    /* The deallocation function. */
    sipDeallocFunc td_dealloc;

    /* The assignment function. */
    sipAssignFunc td_assign;

    /* The release function. */
    sipReleaseFunc td_release;

    /* The cast function, 0 if a C struct. */
    sipCastFunc td_cast;

    /* The convert to function. */
    sipConvertToFunc td_cto;

    /* The convert from function (mapped types only). */
    sipConvertFromFunc td_cfrom;

    /* The static instances. */
    sipInstancesDef td_instances;

    /* The next namespace extender. */
    struct _sipTypeDef *td_nsextender;

    /* The pickle function. */
    sipPickleFunc td_pickle;

    /* Emit table for Qt signals. */
    struct _sipQtSignal *td_emit;

    /* The optional PyQt defined information. */
    const void *td_qt;
} sipTypeDef;


/*
 * The information describing an external type.
 */
typedef struct _sipExternalTypeDef {
    /* The index into the type table. */
    int et_nr;

    /* The name of the type. */
    const char *et_name;
} sipExternalTypeDef;


/*
 * The information describing a mapped class.  This (and anything that uses it)
 * is deprecated.
 */
typedef sipTypeDef sipMappedType;


/*
 * Defines an entry in the module specific list of delayed dtor calls.
 */
typedef struct _sipDelayedDtor {
    /* The C/C++ instance. */
    void *dd_ptr;

    /* The class name. */
    const char *dd_name;

    /* Non-zero if dd_ptr is a derived class instance. */
    int dd_isderived;

    /* Next in the list. */
    struct _sipDelayedDtor *dd_next;
} sipDelayedDtor;


/*
 * The information describing an imported module.
 */
typedef struct _sipImportedModuleDef {
    /* The module name. */
    const char *im_name;

    /* The required version. */
    int im_version;

    /* The imported module. */
    struct _sipExportedModuleDef *im_module;
} sipImportedModuleDef;


/*
 * The main client module structure.
 */
typedef struct _sipExportedModuleDef {
    /* The next in the list. */
    struct _sipExportedModuleDef *em_next;

    /* The SIP API minor version number. */
    unsigned em_api_minor;

    /* The module name. */
    int em_name;

    /* The module name as an object. */
    PyObject *em_nameobj;

    /* The module version. */
    int em_version;

    /* The string pool. */
    const char *em_strings;

    /* The imported modules. */
    sipImportedModuleDef *em_imports;

    /* The optional Qt support API. */
    struct _sipQtAPI *em_qt_api;

    /* The number of types. */
    int em_nrtypes;

    /* The table of type types. */
    sipTypeDef **em_types;

    /* The table of external types. */
    sipExternalTypeDef *em_external;

    /* The number of mapped types. */
    int em_nrmappedtypes;

    /* The table of mapped types. */
    sipTypeDef **em_mappedtypes;

    /* The number of enums. */
    int em_nrenums;

    /* The table of enum types. */
    PyTypeObject **em_enums;

    /* The table of enum type data. */
    sipEnumDef *em_enumdefs;

    /* The number of members in global enums. */
    int em_nrenummembers;

    /* The table of members in global enums. */
    sipEnumMemberDef *em_enummembers;

    /* The table of typedefs. */
    sipTypedefDef *em_typedefs;

    /* The table of virtual handlers. */
    sipVirtHandlerFunc *em_virthandlers;

    /* The sub-class convertors. */
    sipSubClassConvertorDef *em_convertors;

    /* The static instances. */
    sipInstancesDef em_instances;

    /* The license. */
    struct _sipLicenseDef *em_license;

    /* The table of exception types. */
    PyObject **em_exceptions;

    /* The table of Python slot extenders. */
    sipPySlotExtenderDef *em_slotextend;

    /* The table of initialiser extenders. */
    sipInitExtenderDef *em_initextend;

    /* The delayed dtor handler. */
    void (*em_delayeddtors)(const sipDelayedDtor *);

    /* The list of delayed dtors. */
    sipDelayedDtor *em_ddlist;
} sipExportedModuleDef;


/*
 * The information describing a license to be added to a dictionary.
 */
typedef struct _sipLicenseDef {
    /* The type of license. */
    const char *lc_type;

    /* The licensee. */
    const char *lc_licensee;

    /* The timestamp. */
    const char *lc_timestamp;

    /* The signature. */
    const char *lc_signature;
} sipLicenseDef;


/*
 * The information describing a void pointer instance to be added to a
 * dictionary.
 */
typedef struct _sipVoidPtrInstanceDef {
    /* The void pointer name. */
    const char *vi_name;

    /* The void pointer value. */
    void *vi_val;
} sipVoidPtrInstanceDef;


/*
 * The information describing a char instance to be added to a dictionary.
 */
typedef struct _sipCharInstanceDef {
    /* The char name. */
    const char *ci_name;

    /* The char value. */
    char ci_val;
} sipCharInstanceDef;


/*
 * The information describing a string instance to be added to a dictionary.
 */
typedef struct _sipStringInstanceDef {
    /* The string name. */
    const char *si_name;

    /* The string value. */
    const char *si_val;
} sipStringInstanceDef;


/*
 * The information describing an int instance to be added to a dictionary.
 */
typedef struct _sipIntInstanceDef {
    /* The int name. */
    const char *ii_name;

    /* The int value. */
    int ii_val;
} sipIntInstanceDef;


/*
 * The information describing a long instance to be added to a dictionary.
 */
typedef struct _sipLongInstanceDef {
    /* The long name. */
    const char *li_name;

    /* The long value. */
    long li_val;
} sipLongInstanceDef;


/*
 * The information describing an unsigned long instance to be added to a
 * dictionary.
 */
typedef struct _sipUnsignedLongInstanceDef {
    /* The unsigned long name. */
    const char *uli_name;

    /* The unsigned long value. */
    unsigned long uli_val;
} sipUnsignedLongInstanceDef;


/*
 * The information describing a long long instance to be added to a dictionary.
 */
typedef struct _sipLongLongInstanceDef {
    /* The long long name. */
    const char *lli_name;

    /* The long long value. */
#if defined(HAVE_LONG_LONG)
    PY_LONG_LONG lli_val;
#else
    long lli_val;
#endif
} sipLongLongInstanceDef;


/*
 * The information describing an unsigned long long instance to be added to a
 * dictionary.
 */
typedef struct _sipUnsignedLongLongInstanceDef {
    /* The unsigned long long name. */
    const char *ulli_name;

    /* The unsigned long long value. */
#if defined(HAVE_LONG_LONG)
    unsigned PY_LONG_LONG ulli_val;
#else
    unsigned long ulli_val;
#endif
} sipUnsignedLongLongInstanceDef;


/*
 * The information describing a double instance to be added to a dictionary.
 */
typedef struct _sipDoubleInstanceDef {
    /* The double name. */
    const char *di_name;

    /* The double value. */
    double di_val;
} sipDoubleInstanceDef;


/*
 * The information describing a type instance to be added to a dictionary.
 */
typedef struct _sipTypeInstanceDef {
    /* The type instance name. */
    const char *ti_name;

    /* The actual instance. */
    void *ti_ptr;

    /* A pointer to the Python type. */
    struct _sipTypeDef *ti_type;

    /* The wrapping flags. */
    int ti_flags;
} sipTypeInstanceDef;


/*
 * The information describing an enum instance to be added to a dictionary.
 */
typedef struct _sipEnumInstanceDef {
    /* The enum instance name. */
    const char *ei_name;

    /* The enum value. */
    int ei_val;

    /* A pointer to the Python type. */
    PyTypeObject **ei_type;
} sipEnumInstanceDef;


/*
 * Define a mapping between a wrapped type identified by a string and the
 * corresponding Python type.
 */
typedef struct _sipStringTypeClassMap {
    /* The type as a string. */
    const char *typeString;

    /* A pointer to the Python type. */
    struct _sipWrapperType **pyType;
} sipStringTypeClassMap;


/*
 * Define a mapping between a wrapped type identified by an integer and the
 * corresponding Python type.
 */
typedef struct _sipIntTypeClassMap {
    /* The type as an integer. */
    int typeInt;

    /* A pointer to the Python type. */
    struct _sipWrapperType **pyType;
} sipIntTypeClassMap;


/*
 * A Python method's component parts.  This allows us to re-create the method
 * without changing the reference counts of the components.
 */
typedef struct _sipPyMethod {
    /* The function. */
    PyObject *mfunc;

    /* Self if it is a bound method. */
    PyObject *mself;

    /* The class. */
    PyObject *mclass;
} sipPyMethod;


/*
 * Cache a reference to a Python member function.
 */
typedef struct _sipMethodCache {
    /* Method cache flags. */
    int mcflags;

    /* The method. */
    sipPyMethod pyMethod;
} sipMethodCache;


/*
 * A slot (in the Qt, rather than Python, sense).
 */
typedef struct _sipSlot {
    /* Name if a Qt or Python signal. */
    char *name;

    /* Signal or Qt slot object. */
    PyObject *pyobj;

    /* Python slot method, pyobj is NULL. */
    sipPyMethod meth;

    /* A weak reference to the slot, Py_True if pyobj has an extra reference. */
    PyObject *weakSlot;
} sipSlot;


/*
 * An entry in a linked list of slots.
 */
typedef struct _sipSlotList {
    /* The receiver. */
    sipSlot rx;

    /* Next in the list. */
    struct _sipSlotList *next;
} sipSlotList;


/*
 * A Python signal.
 */
typedef struct _sipPySig {
    /* The name of the signal. */
    char *name;

    /* The list of receivers. */
    sipSlotList *rxlist;

    /* Next in the list. */
    struct _sipPySig *next;
} sipPySig;


/*
 * This defines a single dynamic signal argument type.
 */
typedef struct _sipSigArg {
    /* The type. */
    sipSigArgType atype;

    union {
        /* The Python type for classes. */
        sipWrapperType *wt;

        /* The data for mapped types. */
        sipTypeDef *mt;

        /* The Python type for named enums. */
        PyTypeObject *et;
    } u;
} sipSigArg;


/*
 * A parsed signal signature.
 */
typedef struct _sipSignature {
    /* The number of arguments. */
    int sg_nrargs;

    /* The parsed arguments (heap). */
    sipSigArg *sg_args;

    /* The unparsed signature (heap). */
    char *sg_signature;

    /* The next in the list. */
    struct _sipSignature *sg_next;
} sipSignature;


/*
 * A connection to a universal slot.
 */
typedef struct _sipSlotConnection {
    /* The transmitter QObject. */
    void *sc_transmitter;

    /* The parsed signature. */
    const sipSignature *sc_signature;

    /* The slot. */
    sipSlot sc_slot;
} sipSlotConnection;


/*
 * Maps the name of a Qt signal to a wrapper function to emit it.
 */
typedef struct _sipQtSignal {
    /* The signal name. */
    const char *st_name;

    /* The emitter function. */
    sipEmitFunc st_emitfunc;
} sipQtSignal;


/*
 * The API exported by the SIP module, ie. pointers to all the data and
 * functions that can be used by generated code.
 */
typedef struct _sipAPIDef {
    /*
     * This must be the first entry and it's signature must not change so that
     * version number mismatches can be detected and reported.
     */
    int (*api_export_module)(sipExportedModuleDef *client, unsigned api_major,
            unsigned api_minor, PyObject *mod_dict);

    /*
     * The following are part of the public API.
     */
    PyTypeObject *api_simplewrapper_type;
    PyTypeObject *api_wrapper_type;
    PyTypeObject *api_wrappertype_type;
    PyTypeObject *api_voidptr_type;

    void (*api_bad_catcher_result)(PyObject *method);
    void (*api_bad_length_for_slice)(SIP_SSIZE_T seqlen, SIP_SSIZE_T slicelen);
    PyObject *(*api_build_result)(int *isErr, const char *fmt, ...);
    PyObject *(*api_call_method)(int *isErr, PyObject *method, const char *fmt,
            ...);
    PyObject *(*api_connect_rx)(PyObject *txObj, const char *sig,
            PyObject *rxObj, const char *slot, int type);
    SIP_SSIZE_T (*api_convert_from_sequence_index)(SIP_SSIZE_T idx,
            SIP_SSIZE_T len);
    int (*api_can_convert_to_instance)(PyObject *pyObj, sipWrapperType *type,
            int flags);
    int (*api_can_convert_to_mapped_type)(PyObject *pyObj,
            const sipMappedType *mt, int flags);
    void *(*api_convert_to_instance)(PyObject *pyObj, sipWrapperType *type,
            PyObject *transferObj, int flags, int *statep, int *iserrp);
    void *(*api_convert_to_mapped_type)(PyObject *pyObj,
            const sipMappedType *mt, PyObject *transferObj, int flags,
            int *statep, int *iserrp);
    void *(*api_force_convert_to_instance)(PyObject *pyObj,
            sipWrapperType *type, PyObject *transferObj, int flags,
            int *statep, int *iserrp);
    void *(*api_force_convert_to_mapped_type)(PyObject *pyObj,
            const sipMappedType *mt, PyObject *transferObj, int flags,
            int *statep, int *iserrp);
    void (*api_release_instance)(void *cpp, sipWrapperType *type, int state);
    void (*api_release_mapped_type)(void *cpp, const sipMappedType *mt,
            int state);
    PyObject *(*api_convert_from_instance)(void *cpp, sipWrapperType *type,
            PyObject *transferObj);
    PyObject *(*api_convert_from_new_instance)(void *cpp, sipWrapperType *type,
            PyObject *transferObj);
    PyObject *(*api_convert_from_mapped_type)(void *cpp,
            const sipMappedType *mt, PyObject *transferObj);
    int (*api_get_state)(PyObject *transferObj);
    const sipMappedType *(*api_find_mapped_type)(const char *type);
    PyObject *(*api_disconnect_rx)(PyObject *txObj, const char *sig,
            PyObject *rxObj, const char *slot);
    int (*api_emit_signal)(PyObject *self, const char *sig, PyObject *sigargs);
    void (*api_free)(void *mem);
    PyObject *(*api_get_sender)();
    PyObject *(*api_get_wrapper)(void *cppPtr, sipWrapperType *type);
    void *(*api_malloc)(size_t nbytes);
    sipWrapperType *(*api_map_int_to_class)(int typeInt,
            const sipIntTypeClassMap *map, int maplen);
    sipWrapperType *(*api_map_string_to_class)(const char *typeString,
            const sipStringTypeClassMap *map, int maplen);
    int (*api_parse_result)(int *isErr, PyObject *method, PyObject *res,
            const char *fmt, ...);
    void (*api_trace)(unsigned mask, const char *fmt, ...);
    void (*api_transfer_back)(PyObject *self);
    void (*api_transfer_to)(PyObject *self, PyObject *owner);
    void (*api_transfer_break)(PyObject *self);
    unsigned long (*api_long_as_unsigned_long)(PyObject *o);
    PyObject *(*api_convert_from_named_enum)(int eval, PyTypeObject *et);
    PyObject *(*api_convert_from_void_ptr)(void *val);
    PyObject *(*api_convert_from_const_void_ptr)(const void *val);
    PyObject *(*api_convert_from_void_ptr_and_size)(void *val,
            SIP_SSIZE_T size);
    PyObject *(*api_convert_from_const_void_ptr_and_size)(const void *val,
            SIP_SSIZE_T size);
    void *(*api_convert_to_void_ptr)(PyObject *obj);
    int (*api_export_symbol)(const char *name, void *sym);
    void *(*api_import_symbol)(const char *name);
    sipWrapperType *(*api_find_class)(const char *type);
    PyTypeObject *(*api_find_named_enum)(const char *type);
    int (*api_register_py_type)(PyTypeObject *type);

    /*
     * The following may be used by Qt support code but no other handwritten
     * code.
     */
    void (*api_free_connection)(sipSlotConnection *conn);
    int (*api_emit_to_slot)(const sipSlot *slot, PyObject *sigargs);
    int (*api_same_connection)(sipSlotConnection *conn, void *tx,
            const char *sig, PyObject *rxObj, const char *slot);
    void *(*api_convert_rx)(sipWrapper *txSelf, const char *sigargs,
            PyObject *rxObj, const char *slot, const char **memberp);
    int (*api_register_int_types)(PyObject *args);
    sipSignature *(*api_parse_signature)(const char *sig);
    PyObject *(*api_invoke_slot)(const sipSlot *slot, PyObject *sigargs);
    void (*api_parse_type)(const char *type, sipSigArg *arg);
    int (*api_assign_type)(void *dst, const void *src, sipTypeDef *td);

    /*
     * The following are not part of the public API.
     */
    int (*api_parse_args)(int *argsParsedp, PyObject *sipArgs,
            const char *fmt, ...);
    int (*api_parse_pair)(int *argsParsedp, PyObject *arg0, PyObject *arg1,
            const char *fmt, ...);
    void (*api_common_ctor)(sipMethodCache *cache, int nrmeths);
    void (*api_common_dtor)(sipSimpleWrapper *sipSelf);
    void (*api_no_function)(int argsParsed, const char *func);
    void (*api_no_method)(int argsParsed, const char *classname,
            const char *method);
    void (*api_abstract_method)(const char *classname, const char *method);
    void (*api_bad_class)(const char *classname);
    void (*api_bad_set_type)(const char *classname, const char *var);
    void *(*api_get_cpp_ptr)(sipSimpleWrapper *w, sipTypeDef *td);
    void *(*api_get_complex_cpp_ptr)(sipSimpleWrapper *w);
    PyObject *(*api_is_py_method)(sip_gilstate_t *gil, sipMethodCache *pymc,
            sipSimpleWrapper *sipSelf, const char *cname, const char *mname);
    void (*api_call_hook)(const char *hookname);
    void (*api_start_thread)(void);
    void (*api_end_thread)(void);
    void (*api_raise_unknown_exception)(void);
    void (*api_raise_type_exception)(sipTypeDef *td, void *ptr);
    int (*api_add_type_instance)(PyObject *dict, const char *name,
            void *cppPtr, sipTypeDef *td);
    int (*api_add_enum_instance)(PyObject *dict, const char *name, int value,
            PyTypeObject *type);
    void (*api_bad_operator_arg)(PyObject *self, PyObject *arg,
            sipPySlotType st);
    PyObject *(*api_pyslot_extend)(sipExportedModuleDef *mod, sipPySlotType st,
            sipTypeDef *type, PyObject *arg0, PyObject *arg1);
    void (*api_add_delayed_dtor)(sipSimpleWrapper *w);
    char (*api_string_as_char)(PyObject *obj);
#if defined(HAVE_WCHAR_H)
    wchar_t (*api_unicode_as_wchar)(PyObject *obj);
    wchar_t *(*api_unicode_as_wstring)(PyObject *obj);
#else
    int (*api_unicode_as_wchar)(PyObject *obj);
    int *(*api_unicode_as_wstring)(PyObject *obj);
#endif
    void (*api_register_qt_metatype)(int type, struct _sipTypeDef *td);
    int (*api_deprecated)(const char *classname, const char *method);
} sipAPIDef;


/*
 * The API implementing the optional Qt support.
 */
typedef struct _sipQtAPI {
    struct _sipTypeDef **qt_qobject;
    int (*qt_is_qt_signal)(void *, const char *);
    void *(*qt_create_universal_signal_shortcut)(void *, const char *,
            const char **);
    void *(*qt_create_universal_signal)(void *, const struct _sipSignature *);
    void *(*qt_find_universal_signal_shortcut)(void *, const char *,
            const char **);
    void *(*qt_find_universal_signal)(void *, const struct _sipSignature *);
    int (*qt_emit_signal_shortcut)(void *, const char *, PyObject *);
    int (*qt_emit_signal)(void *, const struct _sipSignature *, PyObject *);
    void *(*qt_create_universal_slot)(struct _sipWrapper *,
            struct _sipSlotConnection *, const char **);
    void (*qt_destroy_universal_slot)(void *);
    void *(*qt_find_slot)(void *, const char *, PyObject *, const char *,
            const char **);
    int (*qt_connect)(void *, const char *, void *, const char *, int);
    int (*qt_disconnect)(void *, const char *, void *, const char *);
    int (*qt_signals_blocked)(void *);
    const void *(*qt_get_sender)();
    void (*qt_forget_sender)();
    int (*qt_same_name)(const char *, const char *);
    sipSlotConnection *(*qt_find_connection)(void *, void **);
    void *(*qt_create_universal_slot_ex)(struct _sipWrapper *,
            struct _sipSlotConnection *, const char **, int);
    void (*qt_register_meta_type)(int type, struct _sipTypeDef *td);
} sipQtAPI;


/*
 * These are flags that can be passed to sipCanConvertToInstance(),
 * sipConvertToInstance() and sipForceConvertToInstance().
 */
#define SIP_NOT_NONE        0x01    /* Disallow None. */
#define SIP_NO_CONVERTORS   0x02    /* Disable any type convertors. */


/*
 * These are the state flags returned by %ConvertToTypeCode.  Note that these
 * share the same "namespace" as the flags below.
 */
#define SIP_TEMPORARY       0x0001  /* A temporary instance. */
#define SIP_DERIVED_CLASS   0x0002  /* The instance is derived. */


/*
 * These flags are specific to the Qt support API.
 */
#define SIP_SINGLE_SHOT     0x01    /* The connection is single shot. */


/*
 * Useful macros, not part of the public API.
 */
#define SIP_PY_OWNED        0x0004  /* Owned by Python. */
#define SIP_INDIRECT        0x0008  /* If there is a level of indirection. */
#define SIP_ACCFUNC         0x0010  /* If there is an access function. */
#define SIP_NOT_IN_MAP      0x0020  /* If Python object not in the map. */
#define SIP_SHARE_MAP       0x0040  /* If the map slot might be occupied. */
#define SIP_CPP_HAS_REF     0x0080  /* If C/C++ has a reference. */
#define SIP_POSSIBLE_PROXY  0x0100  /* If there might be a proxy slot. */

#define sipIsPyOwned(w)     ((w)->flags & SIP_PY_OWNED)
#define sipSetPyOwned(w)    ((w)->flags |= SIP_PY_OWNED)
#define sipResetPyOwned(w)  ((w)->flags &= ~SIP_PY_OWNED)
#define sipIsDerived(w)     ((w)->flags & SIP_DERIVED_CLASS)
#define sipIsIndirect(w)    ((w)->flags & SIP_INDIRECT)
#define sipIsAccessFunc(w)  ((w)->flags & SIP_ACCFUNC)
#define sipNotInMap(w)      ((w)->flags & SIP_NOT_IN_MAP)
#define sipSetNotInMap(w)   ((w)->flags |= SIP_NOT_IN_MAP)
#define sipCppHasRef(w)     ((w)->flags & SIP_CPP_HAS_REF)
#define sipSetCppHasRef(w)  ((w)->flags |= SIP_CPP_HAS_REF)
#define sipResetCppHasRef(w)    ((w)->flags &= ~SIP_CPP_HAS_REF)
#define sipPossibleProxy(w) ((w)->flags & SIP_POSSIBLE_PROXY)
#define sipSetPossibleProxy(w)  ((w)->flags |= SIP_POSSIBLE_PROXY)


#define SIP_TYPE_TYPE_MASK  0x0007  /* The type type mask. */
#define SIP_TYPE_CLASS      0x0000  /* If the type is a C++ class. */
#define SIP_TYPE_NAMESPACE  0x0001  /* If the type is a C++ namespace. */
#define SIP_TYPE_MAPPED     0x0002  /* If the type is a mapped type. */
#define SIP_TYPE_ABSTRACT   0x0008  /* If the type is abstract. */
#define SIP_TYPE_SCC        0x0010  /* If the type is subject to sub-class convertors. */
#define SIP_TYPE_FLAGS_SHIFT    8   /* The user type flags shift. */
#define SIP_TYPE_FLAGS_MASK 0x0f00  /* The user type flags mask. */


/*
 * The following are part of the public API.
 */
#define sipTypeIsClass(td)  (((td)->td_flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_CLASS)
#define sipTypeIsNamespace(td)  (((td)->td_flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_NAMESPACE)
#define sipTypeIsMapped(td) (((td)->td_flags & SIP_TYPE_TYPE_MASK) == SIP_TYPE_MAPPED)
#define sipTypeFlags(td)    (((td)->td_flags & SIP_TYPE_FLAGS_MASK) >> SIP_TYPE_FLAGS_SHIFT)
#define sipTypePyTypeObject(td) ((td)->td_wrapper_type)

#define sipIsExactWrappedType(wt)   (sipTypePyTypeObject((wt)->type) == (wt))

#define sipConvertFromSliceObject(o,len,start,stop,step,slen) \
        PySlice_GetIndicesEx((PySliceObject *)(o), (len), (start), (stop), \
                (step), (slen))


/*
 * The following are deprecated parts of the public API.
 */
#define sipClassName(w)     PyString_FromString((w)->ob_type->tp_name)


/*
 * The following are not part of the public API.
 */
#define sipTypeIsAbstract(td)   ((td)->td_flags & SIP_TYPE_ABSTRACT)
#define sipTypeHasSCC(td)   ((td)->td_flags & SIP_TYPE_SCC)


#ifdef __cplusplus
}
#endif


#endif
