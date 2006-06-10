/*
 * The SIP module interface.
 *
 * @BS_LICENSE@
 */


#ifndef _SIP_H
#define	_SIP_H


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
#define	SIP_VERSION		0x@RM_HEXVERSION@
#define	SIP_VERSION_STR		"@RM_LATEST@"


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
 * 3.2  Added sip_api_export_symbol() and sip_api_import_symbol().
 *
 * 3.1  Added sip_api_add_mapped_type_instance().
 *
 * 3.0  Moved the Qt support out of the sip module and into PyQt.  This is
 *      such a dramatic change that there is no point in attempting to maintain
 *      backwards compatibility.
 *
 * 2.0	Added the td_flags field to the sipTypeDef structure.
 *	Added the first_child, sibling_next, sibling_prev and parent fields to
 *	the sipWrapper structure.
 *	Added the td_traverse and td_clear fields to the sipTypeDef structure.
 *	Added the em_api_minor field to the sipExportedModuleDef structure.
 *	Added sip_api_bad_operator_arg().
 *	Added sip_api_wrapper_check().
 *
 * 1.1	Added support for __pos__ and __abs__.
 *
 * 1.0	Removed all deprecated parts of the API.
 *	Removed the td_proxy field from the sipTypeDef structure.
 *	Removed the create proxy function from the 'q' and 'y' format
 *	characters to sip_api_parse_args().
 *	Removed sip_api_emit_to_slot().
 *	Reworked the enum related structures.
 *
 * 0.2	Added the 'H' format character to sip_api_parse_args().
 *
 * 0.1	Added sip_api_add_class_instance().
 *	Added the 't' format character to sip_api_parse_args().
 *	Deprecated the 'J' and 'K' format characters to sip_api_parse_result().
 *
 * 0.0	Original version.
 */
#define	SIP_API_MAJOR_NR	3
#define	SIP_API_MINOR_NR	2


/* Some compatibility stuff to help with handwritten code for SIP v3. */
#if !defined(ANY)
#define ANY     void
#endif


/*
 * The mask that can be passed to sipTrace().
 */
#define	SIP_TRACE_CATCHERS	0x0001
#define	SIP_TRACE_CTORS		0x0002
#define	SIP_TRACE_DTORS		0x0004
#define	SIP_TRACE_INITS		0x0008
#define	SIP_TRACE_DEALLOCS	0x0010
#define	SIP_TRACE_METHODS	0x0020


/*
 * Hide some thread dependent stuff.
 */
#ifdef WITH_THREAD
typedef PyGILState_STATE sip_gilstate_t;
#define	SIP_RELEASE_GIL(gs)	PyGILState_Release(gs);
#define	SIP_BLOCK_THREADS	{PyGILState_STATE sipGIL = PyGILState_Ensure();
#define	SIP_UNBLOCK_THREADS	PyGILState_Release(sipGIL);}
#else
typedef int sip_gilstate_t;
#define	SIP_RELEASE_GIL(gs)
#define	SIP_BLOCK_THREADS
#define	SIP_UNBLOCK_THREADS
#endif


/*
 * The metatype for a wrapper type.
 */
typedef struct _sipWrapperType {
	PyHeapTypeObject super;		/* The super-metatype. */
	struct _sipTypeDef *type;	/* The additional type information. */
	struct _sipInitExtenderDef *iextend;	/* The list of init extenders. */
} sipWrapperType;


/*
 * A C/C++ object wrapped as a Python object.
 */
typedef struct _sipWrapper {
	PyObject_HEAD
	PyObject *user;			/* For the user to use. */
	union {
		void *cppPtr;		/* C/C++ object pointer. */
		void *(*afPtr)();	/* Access function. */
	} u;
	int flags;			/* Object flags. */
	PyObject *dict;			/* The instance dictionary. */
	struct _sipPySig *pySigList;	/* Python signal list (complex). */
	struct _sipWrapper *next;	/* Next object at this address. */
	struct _sipWrapper *first_child;	/* First child object. */
	struct _sipWrapper *sibling_next;	/* Next sibling. */
	struct _sipWrapper *sibling_prev;	/* Previous sibling. */
	struct _sipWrapper *parent;	/* Owning object. */
} sipWrapper;


/*
 * Some convenient function pointers.
 */
typedef void *(*sipInitFunc)(sipWrapper *,PyObject *,sipWrapper **,int *);
typedef int (*sipTraverseFunc)(void *, visitproc, void *);
typedef int (*sipClearFunc)(void *);
typedef int (*sipBufferFunc)(PyObject *,void *,int,void **);
typedef int (*sipSegCountFunc)(PyObject *,void *,int *);
typedef void (*sipDeallocFunc)(sipWrapper *);
typedef void *(*sipCastFunc)(void *,sipWrapperType *);
typedef sipWrapperType *(*sipSubClassConvertFunc)(void **);
typedef void *(*sipForceConvertToFunc)(PyObject *,int *);
typedef int (*sipConvertToFunc)(PyObject *,void **,int *,PyObject *);
typedef PyObject *(*sipConvertFromFunc)(void *,PyObject *);
typedef int (*sipVirtHandlerFunc)(void *,PyObject *,...);
typedef int (*sipEmitFunc)(sipWrapper *,PyObject *);
typedef void (*sipReleaseFunc)(void *, int);


/*
 * The information describing an encoded class ID.
 */
typedef struct _sipEncodedClassDef {
	unsigned sc_class:16;		/* The class number. */
	unsigned sc_module:8;		/* The module number (255 for this one). */
	unsigned sc_flag:1;		/* A context specific flag. */
} sipEncodedClassDef;


/*
 * The information describing an enum member.
 */
typedef struct _sipEnumMemberDef {
	const char *em_name;		/* The member name. */
	int em_val;			/* The member value. */
	int em_enum;			/* The member enum, -ve if anonymous. */
} sipEnumMemberDef;


/*
 * The information describing a named enum.
 */
typedef struct _sipEnumDef {
	const char *e_name;		/* The Python name of the enum. */
	const char *e_cname;		/* The C/C++ name of the enum, NULL if the same as the Python name. */
	int e_scope;			/* The scoping type. */
	struct _sipPySlotDef *e_pyslots;	/* The Python slots. */
} sipEnumDef;


/*
 * The information describing static instances.
 */
typedef struct _sipInstancesDef {
	struct _sipClassInstanceDef *id_class;	/* The classes. */
	struct _sipVoidPtrInstanceDef *id_voidp;	/* The void *. */
	struct _sipCharInstanceDef *id_char;	/* The chars. */
	struct _sipStringInstanceDef *id_string;	/* The strings. */
	struct _sipIntInstanceDef *id_int;	/* The ints. */
	struct _sipLongInstanceDef *id_long;	/* The longs. */
	struct _sipUnsignedLongInstanceDef *id_ulong;	/* The unsigned longs. */
	struct _sipLongLongInstanceDef *id_llong;	/* The long longs. */
	struct _sipUnsignedLongLongInstanceDef *id_ullong;	/* The unsigned long longs. */
	struct _sipDoubleInstanceDef *id_double;	/* The doubles. */
	struct _sipEnumInstanceDef *id_enum;	/* The enums. */
} sipInstancesDef;


/*
 * The information describing a type initialiser extender.
 */
typedef struct _sipInitExtenderDef {
	sipInitFunc ie_extender;	/* The extender function. */
	sipEncodedClassDef ie_class;	/* The class being extended. */
	struct _sipInitExtenderDef *ie_next;	/* The next extender for this class. */
} sipInitExtenderDef;


/*
 * The information describing a sub-class convertor.
 */
typedef struct _sipSubClassConvertorDef {
	sipSubClassConvertFunc scc_convertor;	/* The convertor. */
	sipEncodedClassDef scc_base;	/* The encoded base type. */
	sipWrapperType *scc_basetype;	/* The base type. */
} sipSubClassConvertorDef;


/*
 * The different Python slot types.
 */
typedef enum {
	str_slot,			/* __str__ */
	int_slot,			/* __int__ */
	long_slot,			/* __long__ */
	float_slot,			/* __float__ */
	len_slot,			/* __len__ */
	contains_slot,			/* __contains__ */
	add_slot,			/* __add__ for number */
	concat_slot,			/* __add__ for sequence types */
	sub_slot,			/* __sub__ */
	mul_slot,			/* __mul__ for number types */
	repeat_slot,			/* __mul__ for sequence types */
	div_slot,			/* __div__ */
	mod_slot,			/* __mod__ */
	and_slot,			/* __and__ */
	or_slot,			/* __or__ */
	xor_slot,			/* __xor__ */
	lshift_slot,			/* __lshift__ */
	rshift_slot,			/* __rshift__ */
	iadd_slot,			/* __iadd__ for number types */
	iconcat_slot,			/* __iadd__ for sequence types */
	isub_slot,			/* __isub__ */
	imul_slot,			/* __imul__ for number types */
	irepeat_slot,			/* __imul__ for sequence types */
	idiv_slot,			/* __idiv__ */
	imod_slot,			/* __imod__ */
	iand_slot,			/* __iand__ */
	ior_slot,			/* __ior__ */
	ixor_slot,			/* __ixor__ */
	ilshift_slot,			/* __ilshift__ */
	irshift_slot,			/* __irshift__ */
	invert_slot,			/* __invert__ */
	call_slot,			/* __call__ */
	getitem_slot,			/* __getitem__ */
	setitem_slot,			/* __setitem__ */
	delitem_slot,			/* __delitem__ */
	lt_slot,			/* __lt__ */
	le_slot,			/* __le__ */
	eq_slot,			/* __eq__ */
	ne_slot,			/* __ne__ */
	gt_slot,			/* __gt__ */
	ge_slot,			/* __ge__ */
	cmp_slot,			/* __cmp__ */
	nonzero_slot,			/* __nonzero__ */
	neg_slot,			/* __neg__ */
	repr_slot,			/* __repr__ */
	hash_slot,			/* __hash__ */
	pos_slot,			/* __pos__ */
	abs_slot			/* __abs__ */
} sipPySlotType;


/*
 * The information describing a Python slot function.
 */
typedef struct _sipPySlotDef {
	void *psd_func;			/* The function. */
	sipPySlotType psd_type;		/* The type. */
} sipPySlotDef;


/*
 * The information describing a Python slot extender.
 */
typedef struct _sipPySlotExtenderDef {
	void *pse_func;			/* The function. */
	sipPySlotType pse_type;		/* The type. */
	sipEncodedClassDef pse_class;	/* The encoded class. */
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
	sstring_sat
} sipSigArgType;


/*
 * The information describing a typedef.
 */
typedef struct _sipTypedefDef {
	const char *tdd_name;		/* The typedef name. */
	sipSigArgType tdd_type;		/* The typedef type. */
	const char *tdd_type_name;	/* The type name for enum_sat, class_sat and mtype_sat. */
	const char *tdd_mod_name;	/* The defining module, NULL if the current one. */
} sipTypedefDef;


/*
 * The information describing a type.
 */
typedef struct _sipTypeDef {
	struct _sipExportedModuleDef *td_module;	/* The module. */
	int td_flags;			/* Type flags, see the sipType*() macros. */
	const char *td_name;		/* The Python name of the type. */
	const char *td_cname;		/* The C/C++ name of the type, NULL if the same as the Python name. */
	sipEncodedClassDef td_scope;	/* The scoping type. */
	sipEncodedClassDef *td_supers;	/* The super-types. */
	sipPySlotDef *td_pyslots;	/* The table of Python slots. */
	int td_nrmethods;		/* The number of lazy methods. */
	PyMethodDef *td_methods;	/* The table of lazy methods. */
	int td_nrenummembers;		/* The number of lazy enum members. */
	sipEnumMemberDef *td_enummembers;	/* The table of lazy enum members. */
	PyMethodDef *td_variables;	/* The variable table. */
	sipInitFunc td_init;		/* The initialisation function. */
	sipTraverseFunc td_traverse;	/* The traverse function. */
	sipClearFunc td_clear;		/* The clear function. */
	sipBufferFunc td_readbuffer;	/* The read buffer function. */
	sipBufferFunc td_writebuffer;	/* The write buffer function. */
	sipSegCountFunc td_segcount;	/* The segment count function. */
	sipBufferFunc td_charbuffer;	/* The char buffer function. */
	sipDeallocFunc td_dealloc;	/* The deallocation function. */
	sipCastFunc td_cast;		/* The cast function, 0 if a C struct. */
	sipReleaseFunc td_release;	/* The release function. */
	sipForceConvertToFunc td_fcto;	/* The force convert to function, 0 if a C++ namespace. */
	sipConvertToFunc td_cto;	/* The convert to function. */
	struct _sipQtSignal *td_emit;	/* Emit table for Qt signals. */
	sipInstancesDef td_instances;	/* The static instances. */
	struct _sipTypeDef *td_nsextender;	/* The next namespace extender. */
} sipTypeDef;


/*
 * The information describing an external type.
 */
typedef struct _sipExternalTypeDef {
	int et_nr;			/* The index into the type table. */
	const char *et_name;		/* The name of the type. */
} sipExternalTypeDef;


/*
 * The information describing a mapped class.
 */
typedef struct _sipMappedType {
	const char *mt_name;		/* The corresponding C++ definition. */
	sipReleaseFunc mt_release;	/* The release function. */
	sipForceConvertToFunc mt_fcto;	/* The force convert to function. */
	sipConvertToFunc mt_cto;	/* The convert to function. */
	sipConvertFromFunc mt_cfrom;	/* The convert from function. */
} sipMappedType;


/*
 * Defines an entry in the module specific list of delayed dtor calls.
 */
typedef struct _sipDelayedDtor {
	void *dd_ptr;			/* The C/C++ instance. */
	const char *dd_name;		/* The class name. */
	int dd_isderived;		/* Non-zero if dd_ptr is a derived class instance. */
	struct _sipDelayedDtor *dd_next;	/* Next in the list. */
} sipDelayedDtor;


/*
 * The information describing an imported module.
 */
typedef struct _sipImportedModuleDef {
	char *im_name;			/* The module name. */
	int im_version;			/* The required version. */
	struct _sipExportedModuleDef *im_module;	/* The imported module. */
} sipImportedModuleDef;


/*
 * The main client module structure.
 */
typedef struct _sipExportedModuleDef {
	struct _sipExportedModuleDef *em_next;	/* The next in the list. */
	unsigned em_api_minor;		/* The SIP API minor version number. */
	char *em_name;			/* The module name. */
	PyObject *em_nameobj;		/* The module name as an object. */
	int em_version;			/* The module version. */
	sipImportedModuleDef *em_imports;	/* The imported modules. */
	struct _sipQtAPI *em_qt_api;	/* The optional Qt support API. */
	int em_nrtypes;			/* The number of types. */
	struct _sipWrapperType **em_types;	/* The table of type types. */
	sipExternalTypeDef *em_external;	/* The table of external types. */
	sipMappedType **em_mappedtypes;	/* The table of mapped types. */
	int em_nrenums;			/* The number of enums. */
	PyTypeObject **em_enums;	/* The table of enum types. */
	sipEnumDef *em_enumdefs;	/* The table of enum type data. */
	int em_nrenummembers;		/* The number of members in global enums. */
	sipEnumMemberDef *em_enummembers;	/* The table of members in global enums. */
	sipTypedefDef *em_typedefs;	/* The table of typedefs. */
	sipVirtHandlerFunc *em_virthandlers;	/* The table of virtual handlers. */
	sipSubClassConvertorDef *em_convertors;	/* The sub-class convertors. */
	sipInstancesDef em_instances;	/* The static instances. */
	struct _sipLicenseDef *em_license;	/* The license. */
	PyObject **em_exceptions;	/* The table of exception types. */
	sipPySlotExtenderDef *em_slotextend;	/* The table of Python slot extenders. */
	sipInitExtenderDef *em_initextend;	/* The table of initialiser extenders. */
	void (*em_delayeddtors)(const sipDelayedDtor *);	/* The delayed dtor handler. */
	sipDelayedDtor *em_ddlist;	/* The list of delayed dtors. */
} sipExportedModuleDef;


/*
 * The information describing a license to be added to a dictionary.
 */
typedef struct _sipLicenseDef {
	char *lc_type;			/* The type of license. */
	char *lc_licensee;		/* The licensee. */
	char *lc_timestamp;		/* The timestamp. */
	char *lc_signature;		/* The signature. */
} sipLicenseDef;


/*
 * The information describing a void pointer instance to be added to a
 * dictionary.
 */
typedef struct _sipVoidPtrInstanceDef {
	char *vi_name;			/* The void pointer name. */
	void *vi_val;			/* The void pointer value. */
} sipVoidPtrInstanceDef;


/*
 * The information describing a char instance to be added to a dictionary.
 */
typedef struct _sipCharInstanceDef {
	char *ci_name;			/* The char name. */
	char ci_val;			/* The char value. */
} sipCharInstanceDef;


/*
 * The information describing a string instance to be added to a dictionary.
 */
typedef struct _sipStringInstanceDef {
	char *si_name;			/* The string name. */
	char *si_val;			/* The string value. */
} sipStringInstanceDef;


/*
 * The information describing an int instance to be added to a dictionary.
 */
typedef struct _sipIntInstanceDef {
	char *ii_name;			/* The int name. */
	int ii_val;			/* The int value. */
} sipIntInstanceDef;


/*
 * The information describing a long instance to be added to a dictionary.
 */
typedef struct _sipLongInstanceDef {
	char *li_name;			/* The long name. */
	long li_val;			/* The long value. */
} sipLongInstanceDef;


/*
 * The information describing an unsigned long instance to be added to a
 * dictionary.
 */
typedef struct _sipUnsignedLongInstanceDef {
	char *uli_name;			/* The unsigned long name. */
	unsigned long uli_val;		/* The unsigned long value. */
} sipUnsignedLongInstanceDef;


/*
 * The information describing a long long instance to be added to a dictionary.
 */
typedef struct _sipLongLongInstanceDef {
	char *lli_name;			/* The long long name. */
#if defined(HAVE_LONG_LONG)
	PY_LONG_LONG lli_val;		/* The long long value. */
#else
	long lli_val;
#endif
} sipLongLongInstanceDef;


/*
 * The information describing an unsigned long long instance to be added to a
 * dictionary.
 */
typedef struct _sipUnsignedLongLongInstanceDef {
	char *ulli_name;		/* The unsigned long long name. */
#if defined(HAVE_LONG_LONG)
	unsigned PY_LONG_LONG ulli_val;	/* The unsigned long long value. */
#else
	unsigned long ulli_val;
#endif
} sipUnsignedLongLongInstanceDef;


/*
 * The information describing a double instance to be added to a dictionary.
 */
typedef struct _sipDoubleInstanceDef {
	char *di_name;			/* The double name. */
	double di_val;			/* The double value. */
} sipDoubleInstanceDef;


/*
 * The information describing a class instance to be added to a dictionary.
 */
typedef struct _sipClassInstanceDef {
	char *ci_name;			/* The class instance name. */
	void *ci_ptr;			/* The actual instance. */
	struct _sipWrapperType **ci_type;	/* A pointer to the Python type. */
	int ci_flags;			/* The wrapping flags. */
} sipClassInstanceDef;


/*
 * The information describing an enum instance to be added to a dictionary.
 */
typedef struct _sipEnumInstanceDef {
	char *ei_name;			/* The enum instance name. */
	int ei_val;			/* The enum value. */
	PyTypeObject **ei_type;		/* A pointer to the Python type. */
} sipEnumInstanceDef;


/*
 * Define a mapping between a wrapped type identified by a string and the
 * corresponding Python type.
 */
typedef struct _sipStringTypeClassMap {
	char *typeString;		/* The type as a string. */
	struct _sipWrapperType **pyType;	/* A pointer to the Python type. */
} sipStringTypeClassMap;


/*
 * Define a mapping between a wrapped type identified by an integer and the
 * corresponding Python type.
 */
typedef struct _sipIntTypeClassMap {
	int typeInt;			/* The type as an integer. */
	struct _sipWrapperType **pyType;	/* A pointer to the Python type. */
} sipIntTypeClassMap;


/*
 * A Python method's component parts.  This allows us to re-create the method
 * without changing the reference counts of the components.
 */
typedef struct _sipPyMethod {
	PyObject *mfunc;		/* The function. */
	PyObject *mself;		/* Self if it is a bound method. */
	PyObject *mclass;		/* The class. */
} sipPyMethod;


/*
 * Cache a reference to a Python member function.
 */
typedef struct _sipMethodCache {
	int mcflags;			/* Method cache flags. */
	sipPyMethod pyMethod;		/* The method. */
} sipMethodCache;


/*
 * A slot (in the Qt, rather than Python, sense).
 */
typedef struct _sipSlot {
	char *name;			/* Name if a Qt or Python signal. */
	PyObject *pyobj;		/* Signal or Qt slot object. */
	sipPyMethod meth;		/* Python slot method, pyobj is NULL. */
	PyObject *weakSlot;		/* A weak reference to the slot. */
} sipSlot;


/*
 * A receiver of a Python signal.
 */
typedef struct _sipPySigRx {
	sipSlot rx;			/* The receiver. */
	struct _sipPySigRx *next;	/* Next in the list. */
} sipPySigRx;


/*
 * A Python signal.
 */
typedef struct _sipPySig {
	char *name;			/* The name of the signal. */
	sipPySigRx *rxlist;		/* The list of receivers. */
	struct _sipPySig *next;		/* Next in the list. */
} sipPySig;


/*
 * This defines a single dynamic signal argument type.
 */
typedef struct _sipSigArg {
	sipSigArgType atype;		/* The type. */

	union {
		sipWrapperType *wt;	/* The Python type for classes. */
		sipMappedType *mt;	/* The data for mapped types. */
		PyTypeObject *et;	/* The Python type for named enums. */
	} u;
} sipSigArg;


/*
 * A parsed signal signature.
 */
typedef struct _sipSignature {
	int sg_nrargs;			/* The number of arguments. */
	sipSigArg *sg_args;		/* The parsed arguments (heap). */
	char *sg_signature;		/* The unparsed signature (heap). */
	struct _sipSignature *sg_next;	/* The next in the list. */
} sipSignature;


/*
 * A connection to a universal slot.
 */
typedef struct _sipSlotConnection {
	void *sc_transmitter;		/* The transmitter QObject. */
	const sipSignature *sc_signature;	/* The parsed signature. */
	sipSlot sc_slot;		/* The slot. */
} sipSlotConnection;


/*
 * Maps the name of a Qt signal to a wrapper function to emit it.
 */
typedef struct _sipQtSignal {
	char *st_name;			/* The signal name. */
	sipEmitFunc st_emitfunc;	/* The emitter function. */
} sipQtSignal;


/*
 * The API exported by the SIP module, ie. pointers to all the data and
 * functions that can be used by generated code.
 */
typedef struct _sipAPIDef {
	/*
	 * This must be the first entry and it's signature must not change so
	 * that version number mismatches can be detected and reported.
	 */
	int (*api_export_module)(sipExportedModuleDef *client,unsigned api_major,unsigned api_minor,PyObject *mod_dict);
	/*
	 * The following are part of the public API.
	 */
	void (*api_bad_catcher_result)(PyObject *method);
	void (*api_bad_length_for_slice)(int seqlen,int slicelen);
	PyObject *(*api_build_result)(int *isErr, const char *fmt, ...);
	PyObject *(*api_call_method)(int *isErr, PyObject *method, const char *fmt, ...);
	PyObject *(*api_class_name)(PyObject *self);
	PyObject *(*api_connect_rx)(PyObject *txObj,const char *sig,PyObject *rxObj,const char *slot, int type);
	int (*api_convert_from_sequence_index)(int idx,int len);
	int (*api_can_convert_to_instance)(PyObject *pyObj, sipWrapperType *type, int flags);
	int (*api_can_convert_to_mapped_type)(PyObject *pyObj, const sipMappedType *mt, int flags);
	void *(*api_convert_to_instance)(PyObject *pyObj, sipWrapperType *type, PyObject *transferObj, int flags, int *statep, int *iserrp);
	void *(*api_convert_to_mapped_type)(PyObject *pyObj, const sipMappedType *mt, PyObject *transferObj, int flags, int *statep, int *iserrp);
	void *(*api_force_convert_to_instance)(PyObject *pyObj, sipWrapperType *type, PyObject *transferObj, int flags, int *statep, int *iserrp);
	void *(*api_force_convert_to_mapped_type)(PyObject *pyObj, const sipMappedType *mt, PyObject *transferObj, int flags, int *statep, int *iserrp);
	void (*api_release_instance)(void *cpp, sipWrapperType *type, int state);
	void (*api_release_mapped_type)(void *cpp, const sipMappedType *mt, int state);
	PyObject *(*api_convert_from_instance)(void *cpp, sipWrapperType *type, PyObject *transferObj);
	PyObject *(*api_convert_from_new_instance)(void *cpp, sipWrapperType *type, PyObject *transferObj);
	PyObject *(*api_convert_from_mapped_type)(void *cpp, const sipMappedType *mt, PyObject *transferObj);
	void *(*api_convert_to_cpp)(PyObject *sipSelf,sipWrapperType *type,int *iserrp);
	int (*api_get_state)(PyObject *transferObj);
	const sipMappedType *(*api_find_mapped_type)(const char *type);
	PyObject *(*api_disconnect_rx)(PyObject *txObj,const char *sig,PyObject *rxObj,const char *slot);
	int (*api_emit_signal)(PyObject *self,const char *sig,PyObject *sigargs);
	void (*api_free)(void *mem);
	PyObject *(*api_get_sender)();
	PyObject *(*api_get_wrapper)(void *cppPtr,sipWrapperType *type);
	void *(*api_malloc)(size_t nbytes);
	sipWrapperType *(*api_map_int_to_class)(int typeInt,const sipIntTypeClassMap *map,int maplen);
	sipWrapperType *(*api_map_string_to_class)(const char *typeString,const sipStringTypeClassMap *map,int maplen);
	int (*api_parse_result)(int *isErr, PyObject *method, PyObject *res, const char *fmt, ...);
	void (*api_trace)(unsigned mask,const char *fmt,...);
	void (*api_transfer)(PyObject *self,int toCpp);
	void (*api_transfer_back)(PyObject *self);
	void (*api_transfer_to)(PyObject *self,PyObject *owner);
	int (*api_wrapper_check)(PyObject *o);
	unsigned long (*api_long_as_unsigned_long)(PyObject *o);
	PyObject *(*api_convert_from_named_enum)(int eval, PyTypeObject *et);
	/*
	 * The following may be used by Qt support code but no other
	 * handwritten code.
	 */
	PyObject *(*api_convert_from_void_ptr)(void *val);
	void (*api_free_connection)(sipSlotConnection *conn);
	int (*api_emit_to_slot)(sipSlot *slot, PyObject *sigargs);
	int (*api_same_connection)(sipSlotConnection *conn, void *tx, const char *sig, PyObject *rxObj, const char *slot);
	void *(*api_convert_rx)(sipWrapper *txSelf, const char *sigargs, PyObject *rxObj, const char *slot, const char **memberp);
	/*
	 * The following are not part of the public API.
	 */
	int (*api_parse_args)(int *argsParsedp, PyObject *sipArgs,
			const char *fmt, ...);
	int (*api_parse_pair)(int *argsParsedp,PyObject *arg0,PyObject *arg1,char *fmt,...);
	void (*api_common_ctor)(sipMethodCache *cache,int nrmeths);
	void (*api_common_dtor)(sipWrapper *sipSelf);
	void *(*api_convert_to_void_ptr)(PyObject *obj);
	void (*api_no_function)(int argsParsed, const char *func);
	void (*api_no_method)(int argsParsed, const char *classname, const char *method);
	void (*api_abstract_method)(const char *classname, const char *method);
	void (*api_bad_class)(const char *classname);
	void (*api_bad_set_type)(const char *classname,const char *var);
	void *(*api_get_cpp_ptr)(sipWrapper *w,sipWrapperType *type);
	void *(*api_get_complex_cpp_ptr)(sipWrapper *w);
	PyObject *(*api_is_py_method)(sip_gilstate_t *gil,sipMethodCache *pymc,sipWrapper *sipSelf,char *cname,char *mname);
	void (*api_call_hook)(char *hookname);
	void (*api_start_thread)(void);
	void (*api_end_thread)(void);
	void (*api_raise_unknown_exception)(void);
	void (*api_raise_class_exception)(sipWrapperType *type,void *ptr);
	void (*api_raise_sub_class_exception)(sipWrapperType *type,void *ptr);
	int (*api_add_class_instance)(PyObject *dict,char *name,void *cppPtr,sipWrapperType *wt);
	int (*api_add_enum_instance)(PyObject *dict,char *name,int value,PyTypeObject *type);
	void (*api_bad_operator_arg)(PyObject *self, PyObject *arg, sipPySlotType st);
	PyObject *(*api_pyslot_extend)(sipExportedModuleDef *mod, sipPySlotType st, sipWrapperType *type, PyObject *arg0, PyObject *arg1);
	void (*api_add_delayed_dtor)(sipWrapper *w);
	int (*api_add_mapped_type_instance)(PyObject *dict, char *name,
			void *cppPtr, const sipMappedType *mt);
	/*
	 * The following are part of the public API.
	 */
	int (*api_export_symbol)(const char *name, void *sym);
	void *(*api_import_symbol)(const char *name);
} sipAPIDef;


/*
 * The API implementing the optional Qt support.
 */
typedef struct _sipQtAPI {
	struct _sipWrapperType **qt_qobject;	/* The QObject type. */
	int (*qt_is_qt_signal)(void *, const char *);
	void *(*qt_create_universal_signal_shortcut)(void *, const char *, const char **);
	void *(*qt_create_universal_signal)(void *, const struct _sipSignature *);
	void *(*qt_find_universal_signal_shortcut)(void *, const char *, const char **);
	void *(*qt_find_universal_signal)(void *, const struct _sipSignature *);
	int (*qt_emit_signal_shortcut)(void *, const char *, PyObject *);
	int (*qt_emit_signal)(void *, const struct _sipSignature *, PyObject *);
	void *(*qt_create_universal_slot)(struct _sipWrapper *, struct _sipSlotConnection *, const char **);
	void (*qt_destroy_universal_slot)(void *);
	void *(*qt_find_slot)(void *, const char *, PyObject *, const char *, const char **);
	int (*qt_connect)(void *, const char *, void *, const char *, int);
	int (*qt_disconnect)(void *, const char *, void *, const char *);
	int (*qt_signals_blocked)(void *);
	const void *(*qt_get_sender)();
	void (*qt_forget_sender)();
	int (*qt_same_name)(const char *, const char *);
} sipQtAPI;


/*
 * These are flags that can be passed to sipCanConvertToInstance(),
 * sipConvertToInstance() and sipForceConvertToInstance().
 */
#define	SIP_NOT_NONE		0x01	/* Disallow None. */
#define	SIP_NO_CONVERTORS	0x02	/* Disable any type convertors. */


/*
 * These are the state flags returned by %ConvertToTypeCode.  Note that these
 * share the same "namespace" as the flags below.
 */
#define	SIP_TEMPORARY		0x01	/* A temporary instance. */
#define	SIP_DERIVED_CLASS	0x02	/* The instance is derived. */


/*
 * Useful macros, not part of the public API.
 */
#define	SIP_PY_OWNED		0x04	/* Owned by Python. */
#define	SIP_INDIRECT		0x08	/* If there is a level of indirection. */
#define	SIP_ACCFUNC		0x10	/* If there is an access function. */
#define	SIP_NOT_IN_MAP		0x20	/* If Python object not in the map. */
#define	SIP_SHARE_MAP		0x40	/* If the map slot might be occupied. */

#define	sipIsPyOwned(w)		((w) -> flags & SIP_PY_OWNED)
#define	sipSetPyOwned(w)	((w) -> flags |= SIP_PY_OWNED)
#define	sipResetPyOwned(w)	((w) -> flags &= ~SIP_PY_OWNED)
#define	sipIsDerived(w)		((w) -> flags & SIP_DERIVED_CLASS)
#define	sipIsIndirect(w)	((w) -> flags & SIP_INDIRECT)
#define	sipIsAccessFunc(w)	((w) -> flags & SIP_ACCFUNC)
#define	sipNotInMap(w)		((w) -> flags & SIP_NOT_IN_MAP)


#define	SIP_TYPE_ABSTRACT	0x01	/* If the type is abstract. */
#define	SIP_TYPE_SCC		0x02	/* If the type is subject to sub-class convertors. */

#define	sipTypeIsAbstract(wt)	((wt)->type->td_flags & SIP_TYPE_ABSTRACT)
#define	sipTypeHasSCC(wt)	((wt)->type->td_flags & SIP_TYPE_SCC)


#ifdef __cplusplus
}
#endif


#endif
