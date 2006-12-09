/*
 * The main header file for SIP.
 *
 * @BS_LICENSE@
 */


#ifndef SIP_H
#define SIP_H

#include <stdio.h>
#include <sys/types.h>


#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE    1
#define FALSE   0


#define DEFAULT_OFILE_EXT   ".o"    /* Default object file extension. */

#define MAX_NR_ARGS         20      /* Max. nr. args. to a function or template. */


/* For convenience. */

#define classBaseName(cd)   ((cd) -> iff -> name -> text)
#define classFQCName(cd)    ((cd) -> iff -> fqcname)


/* Handle module flags. */

#define MOD_HAS_DELAYED_DTORS   0x0001  /* It has a class with a delayed dtor. */

#define hasDelayedDtors(m)  ((m) -> modflags & MOD_HAS_DELAYED_DTORS)
#define setHasDelayedDtors(m)   ((m) -> modflags |= MOD_HAS_DELAYED_DTORS)


/* Handle section flags. */

#define SECT_IS_PUBLIC      0x0001  /* It is public. */
#define SECT_IS_PROT        0x0002  /* It is protected. */
#define SECT_IS_PRIVATE     0x0004  /* It is private. */
#define SECT_IS_SLOT        0x0008  /* It is a slot. */
#define SECT_IS_SIGNAL      0x0010  /* It is a signal. */
#define SECT_MASK           0x001f  /* The mask of all flags. */


/* Handle class flags. */

#define CLASS_HAS_ENUMS     0x00010000  /* It has enums. */
#define CLASS_HAS_SIGSLOTS  0x00020000  /* It has signals or slots. */
#define CLASS_IS_ABSTRACT   0x00040000  /* It is an abstract class. */
#define CLASS_HAS_SHADOW    0x00080000  /* It is has a shadow class. */
#define CLASS_IS_OPAQUE     0x00100000  /* It is opaque. */
#define CLASS_HAS_VAR_HANDLERS  0x00200000  /* It has variable handlers. */
#define CLASS_DTOR_RELEASE_GIL  0x00400000  /* The dtor releases the GIL. */
#define CLASS_IS_PROTECTED  0x00800000  /* It is protected. */
#define CLASS_IS_PROTECTED_SAV  0x01000000  /* It is protected (saved). */
#define CLASS_IS_RENAMED    0x02000000  /* It has a different Python name. */
#define CLASS_IS_INCOMPLETE 0x04000000  /* The specification is incomplete. */
#define CLASS_CAN_CREATE    0x08000000  /* It has usable ctors. */
#define CLASS_IS_EXTERNAL   0x10000000  /* It is external. */
#define CLASS_IS_DELAYED_DTOR   0x20000000  /* The dtor is delayed. */
#define CLASS_NO_DEFAULT_CTORS  0x40000000  /* Don't create default ctors. */

#define hasEnums(cd)        ((cd) -> classflags & CLASS_HAS_ENUMS)
#define setHasEnums(cd)     ((cd) -> classflags |= CLASS_HAS_ENUMS)
#define hasSigSlots(cd)     ((cd) -> classflags & CLASS_HAS_SIGSLOTS)
#define setHasSigSlots(cd)  ((cd) -> classflags |= CLASS_HAS_SIGSLOTS)
#define isAbstractClass(cd) ((cd) -> classflags & CLASS_IS_ABSTRACT)
#define setIsAbstractClass(cd)  ((cd) -> classflags |= CLASS_IS_ABSTRACT)
#define hasShadow(cd)       ((cd) -> classflags & CLASS_HAS_SHADOW)
#define setHasShadow(cd)    ((cd) -> classflags |= CLASS_HAS_SHADOW)
#define resetHasShadow(cd)  ((cd) -> classflags &= ~CLASS_HAS_SHADOW)
#define isOpaque(cd)        ((cd) -> classflags & CLASS_IS_OPAQUE)
#define setIsOpaque(cd)     ((cd) -> classflags |= CLASS_IS_OPAQUE)
#define hasVarHandlers(cd)  ((cd) -> classflags & CLASS_HAS_VAR_HANDLERS)
#define setHasVarHandlers(cd)   ((cd) -> classflags |= CLASS_HAS_VAR_HANDLERS)
#define isProtectedClass(cd)    ((cd) -> classflags & CLASS_IS_PROTECTED)
#define setIsProtectedClass(cd) ((cd) -> classflags |= CLASS_IS_PROTECTED)
#define resetIsProtectedClass(cd)   ((cd) -> classflags &= ~CLASS_IS_PROTECTED)
#define wasProtectedClass(cd)   ((cd) -> classflags & CLASS_IS_PROTECTED_SAV)
#define setWasProtectedClass(cd)    ((cd) -> classflags |= CLASS_IS_PROTECTED_SAV)
#define resetWasProtectedClass(cd)  ((cd) -> classflags &= ~CLASS_IS_PROTECTED_SAV)
#define isReleaseGILDtor(c) ((cd) -> classflags & CLASS_DTOR_RELEASE_GIL)
#define setIsReleaseGILDtor(c)  ((cd) -> classflags |= CLASS_DTOR_RELEASE_GIL)
#define isRenamedClass(cd)  ((cd) -> classflags & CLASS_IS_RENAMED)
#define setIsRenamedClass(cd)   ((cd) -> classflags |= CLASS_IS_RENAMED)
#define isIncomplete(cd)    ((cd) -> classflags & CLASS_IS_INCOMPLETE)
#define setIsIncomplete(cd) ((cd) -> classflags |= CLASS_IS_INCOMPLETE)
#define canCreate(cd)       ((cd) -> classflags & CLASS_CAN_CREATE)
#define setCanCreate(cd)    ((cd) -> classflags |= CLASS_CAN_CREATE)
#define resetCanCreate(cd)  ((cd) -> classflags &= ~CLASS_CAN_CREATE)
#define isExternal(cd)      ((cd) -> classflags & CLASS_IS_EXTERNAL)
#define setIsExternal(cd)   ((cd) -> classflags |= CLASS_IS_EXTERNAL)
#define isDelayedDtor(cd)   ((cd) -> classflags & CLASS_IS_DELAYED_DTOR)
#define setIsDelayedDtor(cd)    ((cd) -> classflags |= CLASS_IS_DELAYED_DTOR)
#define noDefaultCtors(cd)  ((cd) -> classflags & CLASS_NO_DEFAULT_CTORS)
#define setNoDefaultCtors(cd)   ((cd) -> classflags |= CLASS_NO_DEFAULT_CTORS)

#define isPublicDtor(cd)    ((cd) -> classflags & SECT_IS_PUBLIC)
#define setIsPublicDtor(cd) ((cd) -> classflags |= SECT_IS_PUBLIC)
#define isProtectedDtor(cd) ((cd) -> classflags & SECT_IS_PROT)
#define isPrivateDtor(cd)   ((cd) -> classflags & SECT_IS_PRIVATE)

#define isDtor(cd)      ((cd) -> classflags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE))


/* Handle ctor flags. */

#define CTOR_RELEASE_GIL    0x00010000  /* The ctor releases the GIL. */
#define CTOR_EXPLICIT       0x00020000  /* The ctor is explicit. */
#define CTOR_CAST           0x00040000  /* The ctor is a cast. */

#define isPublicCtor(c)     ((c) -> ctorflags & SECT_IS_PUBLIC)
#define setIsPublicCtor(c)  ((c) -> ctorflags |= SECT_IS_PUBLIC)
#define isProtectedCtor(c)  ((c) -> ctorflags & SECT_IS_PROT)
#define setIsProtectedCtor(c)   ((c) -> ctorflags |= SECT_IS_PROT)
#define isPrivateCtor(c)    ((c) -> ctorflags & SECT_IS_PRIVATE)
#define setIsPrivateCtor(c) ((c) -> ctorflags |= SECT_IS_PRIVATE)

#define isReleaseGILCtor(c) ((c) -> ctorflags & CTOR_RELEASE_GIL)
#define setIsReleaseGILCtor(c)  ((c) -> ctorflags |= CTOR_RELEASE_GIL)
#define isExplicitCtor(c)   ((c) -> ctorflags & CTOR_EXPLICIT)
#define setIsExplicitCtor(c)    ((c) -> ctorflags |= CTOR_EXPLICIT)
#define isCastCtor(c)       ((c) -> ctorflags & CTOR_CAST)


/* Handle member flags. */

#define MEMBR_NUMERIC       0x0001      /* It is a numeric slot. */

#define isNumeric(m)        ((m) -> memberflags & MEMBR_NUMERIC)
#define setIsNumeric(m)     ((m) -> memberflags |= MEMBR_NUMERIC)


/* Handle enum flags. */

#define ENUM_WAS_PROT       0x00010000  /* It was defined as protected. */
#define ENUM_IS_RENAMED     0x00020000  /* It has been renamed. */

#define isProtectedEnum(e)  ((e) -> enumflags & SECT_IS_PROT)
#define setIsProtectedEnum(e)   ((e) -> enumflags |= SECT_IS_PROT)
#define resetIsProtectedEnum(e) ((e) -> enumflags &= ~SECT_IS_PROT)

#define wasProtectedEnum(e) ((e) -> enumflags & ENUM_WAS_PROT)
#define setWasProtectedEnum(e)  ((e) -> enumflags |= ENUM_WAS_PROT)
#define resetWasProtectedEnum(e)    ((e) -> enumflags &= ~ENUM_WAS_PROT)
#define isRenamedEnum(e)    ((e) -> enumflags & ENUM_IS_RENAMED)
#define setIsRenamedEnum(e) ((e) -> enumflags |= ENUM_IS_RENAMED)


/* Handle hierarchy flags. */

#define HIER_IS_DUPLICATE   0x0001      /* It is a super class duplicate. */
#define HIER_HAS_DUPLICATE  0x0002      /* It has a super class duplicate. */

#define isDuplicateSuper(m) ((m) -> mroflags & HIER_IS_DUPLICATE)
#define setIsDuplicateSuper(m)  ((m) -> mroflags |= HIER_IS_DUPLICATE)
#define hasDuplicateSuper(m)    ((m) -> mroflags & HIER_HAS_DUPLICATE)
#define setHasDuplicateSuper(m) ((m) -> mroflags |= HIER_HAS_DUPLICATE)


/* Handle overload flags. */

#define OVER_IS_VIRTUAL     0x00010000  /* It is virtual. */
#define OVER_IS_ABSTRACT    0x00020000  /* It is abstract. */
#define OVER_IS_CONST       0x00040000  /* It is a const function. */
#define OVER_IS_STATIC      0x00080000  /* It is a static function. */
#define OVER_IS_AUTOGEN     0x00100000  /* It is auto-generated. */
#define OVER_IS_NEW_THREAD  0x00200000  /* It is in a new thread. */
#define OVER_IS_FACTORY     0x00400000  /* It is a factory method. */
#define OVER_IS_FACTORY     0x00400000  /* It is a factory method. */
#define OVER_XFERRED_BACK   0x00800000  /* Ownership is transferred back. */
#define OVER_RELEASE_GIL    0x01000000  /* The function releases the GIL. */
#define OVER_IS_VIRTUAL_REIMP   0x02000000  /* It is a re-implementation of a virtual. */
#define OVER_DONT_DEREF_SELF    0x04000000  /* For comparison operators, don't dereference self. */

#define isPublic(o)         ((o) -> overflags & SECT_IS_PUBLIC)
#define setIsPublic(o)      ((o) -> overflags |= SECT_IS_PUBLIC)
#define isProtected(o)      ((o) -> overflags & SECT_IS_PROT)
#define setIsProtected(o)   ((o) -> overflags |= SECT_IS_PROT)
#define isPrivate(o)        ((o) -> overflags & SECT_IS_PRIVATE)
#define setIsPrivate(o)     ((o) -> overflags |= SECT_IS_PRIVATE)
#define isSlot(o)           ((o) -> overflags & SECT_IS_SLOT)
#define setIsSlot(o)        ((o) -> overflags |= SECT_IS_SLOT)
#define isSignal(o)         ((o) -> overflags & SECT_IS_SIGNAL)
#define setIsSignal(o)      ((o) -> overflags |= SECT_IS_SIGNAL)

#define isVirtual(o)        ((o) -> overflags & OVER_IS_VIRTUAL)
#define setIsVirtual(o)     ((o) -> overflags |= OVER_IS_VIRTUAL)
#define isAbstract(o)       ((o) -> overflags & OVER_IS_ABSTRACT)
#define setIsAbstract(o)    ((o) -> overflags |= OVER_IS_ABSTRACT)
#define isConst(o)          ((o) -> overflags & OVER_IS_CONST)
#define setIsConst(o)       ((o) -> overflags |= OVER_IS_CONST)
#define isStatic(o)         ((o) -> overflags & OVER_IS_STATIC)
#define setIsStatic(o)      ((o) -> overflags |= OVER_IS_STATIC)
#define isAutoGen(o)        ((o) -> overflags & OVER_IS_AUTOGEN)
#define setIsAutoGen(o)     ((o) -> overflags |= OVER_IS_AUTOGEN)
#define resetIsAutoGen(o)   ((o) -> overflags &= ~OVER_IS_AUTOGEN)
#define isNewThread(o)      ((o) -> overflags & OVER_IS_NEW_THREAD)
#define setIsNewThread(o)   ((o) -> overflags |= OVER_IS_NEW_THREAD)
#define isFactory(o)        ((o) -> overflags & OVER_IS_FACTORY)
#define setIsFactory(o)     ((o) -> overflags |= OVER_IS_FACTORY)
#define isResultTransferredBack(o)  ((o) -> overflags & OVER_XFERRED_BACK)
#define setIsResultTransferredBack(o)   ((o) -> overflags |= OVER_XFERRED_BACK)
#define isReleaseGIL(o)     ((o) -> overflags & OVER_RELEASE_GIL)
#define setIsReleaseGIL(o)  ((o) -> overflags |= OVER_RELEASE_GIL)
#define isVirtualReimp(o)   ((o) -> overflags & OVER_IS_VIRTUAL_REIMP)
#define setIsVirtualReimp(o)    ((o) -> overflags |= OVER_IS_VIRTUAL_REIMP)
#define dontDerefSelf(o)    ((o) -> overflags & OVER_DONT_DEREF_SELF)
#define setDontDerefSelf(o) ((o) -> overflags |= OVER_DONT_DEREF_SELF)


/* Handle variable flags. */

#define VAR_IS_STATIC       0x01    /* It is a static variable. */
#define VAR_NEEDS_HANDLER   0x02    /* It the variable needs a handler. */

#define isStaticVar(v)      ((v) -> varflags & VAR_IS_STATIC)
#define setIsStaticVar(v)   ((v) -> varflags |= VAR_IS_STATIC)
#define needsHandler(v)     ((v) -> varflags & VAR_NEEDS_HANDLER)
#define setNeedsHandler(v)  ((v) -> varflags |= VAR_NEEDS_HANDLER)


/* Handle argument flags. */

#define ARG_IS_REF      0x0001  /* It is a reference. */
#define ARG_IS_CONST        0x0002  /* It is a const. */
#define ARG_XFERRED     0x0004  /* Ownership is transferred. */
#define ARG_THIS_XFERRED    0x0008  /* Ownership of this is transferred. */
#define ARG_XFERRED_BACK    0x0010  /* Ownership is transferred back. */
#define ARG_ARRAY       0x0020  /* Used as an array. */
#define ARG_ARRAY_SIZE      0x0040  /* Used as an array size. */
#define ARG_ALLOW_NONE      0x0080  /* Allow None as a value. */
#define ARG_GET_WRAPPER     0x0100  /* Get the wrapper object. */
#define ARG_IN          0x0200  /* It passes an argument. */
#define ARG_OUT         0x0400  /* It returns a result. */
#define ARG_CONSTRAINED     0x0800  /* Suppress type conversion. */

#define isReference(a)      ((a) -> argflags & ARG_IS_REF)
#define setIsReference(a)   ((a) -> argflags |= ARG_IS_REF)
#define resetIsReference(a) ((a) -> argflags &= ~ARG_IS_REF)
#define isConstArg(a)       ((a) -> argflags & ARG_IS_CONST)
#define setIsConstArg(a)    ((a) -> argflags |= ARG_IS_CONST)
#define resetIsConstArg(a)  ((a) -> argflags &= ~ARG_IS_CONST)
#define isTransferred(a)    ((a) -> argflags & ARG_XFERRED)
#define setIsTransferred(a) ((a) -> argflags |= ARG_XFERRED)
#define isThisTransferred(a)    ((a) -> argflags & ARG_THIS_XFERRED)
#define setIsThisTransferred(a) ((a) -> argflags |= ARG_THIS_XFERRED)
#define isTransferredBack(a)    ((a) -> argflags & ARG_XFERRED_BACK)
#define setIsTransferredBack(a) ((a) -> argflags |= ARG_XFERRED_BACK)
#define isArray(a)      ((a) -> argflags & ARG_ARRAY)
#define setArray(a)     ((a) -> argflags |= ARG_ARRAY)
#define isArraySize(a)      ((a) -> argflags & ARG_ARRAY_SIZE)
#define setArraySize(a)     ((a) -> argflags |= ARG_ARRAY_SIZE)
#define isAllowNone(a)      ((a) -> argflags & ARG_ALLOW_NONE)
#define setAllowNone(a)     ((a) -> argflags |= ARG_ALLOW_NONE)
#define isGetWrapper(a)     ((a) -> argflags & ARG_GET_WRAPPER)
#define setGetWrapper(a)    ((a) -> argflags |= ARG_GET_WRAPPER)
#define isInArg(a)      ((a) -> argflags & ARG_IN)
#define setIsInArg(a)       ((a) -> argflags |= ARG_IN)
#define isOutArg(a)     ((a) -> argflags & ARG_OUT)
#define setIsOutArg(a)      ((a) -> argflags |= ARG_OUT)
#define isConstrained(a)    ((a) -> argflags & ARG_CONSTRAINED)
#define setIsConstrained(a) ((a) -> argflags |= ARG_CONSTRAINED)
#define resetIsConstrained(a)   ((a) -> argflags &= ~ARG_CONSTRAINED)


/* Handle name flags. */

#define NAME_IS_USED        0x01    /* It is used in the main module. */
#define NAME_IS_CLASS       0x02    /* It is the name of a class. */

#define isUsedName(n)       ((n) -> nameflags & NAME_IS_USED)
#define setIsUsedName(n)    ((n) -> nameflags |= NAME_IS_USED)
#define resetIsUsedName(n)  ((n) -> nameflags &= ~NAME_IS_USED)
#define isClassName(n)      ((n) -> nameflags & NAME_IS_CLASS)
#define setIsClassName(n)   ((n) -> nameflags |= NAME_IS_CLASS)
#define resetIsClassName(n) ((n) -> nameflags &= ~NAME_IS_CLASS)


/* Handle virtual handler flags. */

#define VH_IS_DUPLICATE     0x01    /* It is a duplicate. */
#define VH_TRANSFERS        0x02    /* It transfers ownership of the result. */

#define isDuplicateVH(vh)   ((vh) -> vhflags & VH_IS_DUPLICATE)
#define setIsDuplicateVH(vh)    ((vh) -> vhflags |= VH_IS_DUPLICATE)
#define resetIsDuplicateVH(vh)  ((vh) -> vhflags &= ~VH_IS_DUPLICATE)
#define isTransferVH(vh)    ((vh) -> vhflags & VH_TRANSFERS)
#define setIsTransferVH(vh) ((vh) -> vhflags |= VH_TRANSFERS)


/* Slot types. */

typedef enum {
    str_slot,
    unicode_slot,
    int_slot,
    long_slot,
    float_slot,
    len_slot,
    contains_slot,
    add_slot,
    concat_slot,
    sub_slot,
    mul_slot,
    repeat_slot,
    div_slot,
    mod_slot,
    and_slot,
    or_slot,
    xor_slot,
    lshift_slot,
    rshift_slot,
    iadd_slot,
    iconcat_slot,
    isub_slot,
    imul_slot,
    irepeat_slot,
    idiv_slot,
    imod_slot,
    iand_slot,
    ior_slot,
    ixor_slot,
    ilshift_slot,
    irshift_slot,
    invert_slot,
    call_slot,
    getitem_slot,
    setitem_slot,
    delitem_slot,
    lt_slot,
    le_slot,
    eq_slot,
    ne_slot,
    gt_slot,
    ge_slot,
    cmp_slot,
    nonzero_slot,
    neg_slot,
    pos_slot,
    abs_slot,
    repr_slot,
    hash_slot,
    no_slot
} slotType;


/*
 * Argument types.  Always add new ones at the end because the numeric values
 * can appear in generated code.
 */
typedef enum {
    no_type,
    defined_type,
    class_type,
    struct_type,
    void_type,
    enum_type,
    template_type,
    signal_type,
    slot_type,
    rxcon_type,
    rxdis_type,
    slotcon_type,
    slotdis_type,
    ustring_type,
    string_type,
    short_type,
    ushort_type,
    cint_type,
    int_type,
    uint_type,
    long_type,
    ulong_type,
    float_type,
    cfloat_type,
    double_type,
    cdouble_type,
    bool_type,
    mapped_type,
    pyobject_type,
    pytuple_type,
    pylist_type,
    pydict_type,
    pycallable_type,
    pyslice_type,
    qobject_type,
    function_type,
    pytype_type,
    ellipsis_type,
    longlong_type,
    ulonglong_type,
    anyslot_type,
    cbool_type,
    sstring_type
} argType;


/* Value types. */

typedef enum {
    qchar_value,
    string_value,
    numeric_value,
    real_value,
    scoped_value,
    fcall_value
} valueType;


/* Version types. */

typedef enum {
    time_qualifier,
    platform_qualifier,
    feature_qualifier
} qualType;


/* Interface file types. */

typedef enum {
    exception_iface,
    mappedtype_iface,
    namespace_iface,
    class_iface
} ifaceFileType;


/* A software license. */

typedef struct {
    char            *type;      /* The license type. */
    char            *licensee;  /* The licensee. */
    char            *timestamp; /* The timestamp. */
    char            *sig;       /* The signature. */
} licenseDef;


/* A version qualifier. */

typedef struct _qualDef {
    char            *name;      /* The qualifier name. */
    qualType        qtype;      /* The qualifier type. */
    struct _moduleDef   *module;    /* The defining module. */
    int         line;       /* Timeline if it is a time. */
    int         order;      /* Order if it is a time. */
    struct _qualDef *next;          /* Next in the list. */
} qualDef;


/* A scoped name. */

typedef struct _scopedNameDef {
    char            *name;      /* The name. */
    struct _scopedNameDef   *next;      /* Next in the scope list. */
} scopedNameDef;


/* A name. */

typedef struct _nameDef {
    int         nameflags;  /* The name flags. */
    struct _moduleDef   *module;    /* The main module. */
    char            *text;      /* The text of the name. */
    struct _nameDef     *next;      /* Next in the list. */
} nameDef;


/*
 * A node in the tree of classes used to determine the order in which the
 * classes need to be created.
 */

typedef struct _nodeDef {
    int         ordered;    /* Set if in order. */
    struct _classDef    *cd;        /* The class. */
    struct _nodeDef     *parent;    /* The parent. */
    struct _nodeDef     *child;     /* The first child. */
    struct _nodeDef     *next;      /* The next sibling. */
} nodeDef;


/* A module definition. */

typedef struct _moduleDef {
    char            *fullname;  /* The full module name. */
    char            *name;      /* The module base name. */
    int         version;    /* The module version. */
    int         modflags;   /* The module flags. */
    int         modulenr;   /* The module number. */
    char            *file;      /* The filename. */
    qualDef         *qualifiers;    /* The list of qualifiers. */
    nodeDef         root;       /* Root of class tree. */
    int         nrtimelines;    /* The nr. of timelines. */
    int         nrclasses;  /* The nr. of classes. */
    int         nrexceptions;   /* The nr. of exceptions. */
    int         nrmappedtypes;  /* The nr. of mapped types. */
    int         nrenums;    /* The nr. of named enums. */
    int         nrtypedefs; /* The nr. of typedefs. */
    int         nrvirthandlers; /* The nr. of virtual handlers. */
    struct _virtHandlerDef  *virthandlers;  /* The virtual handlers. */
    licenseDef      *license;   /* The software license. */
    struct _moduleListDef   *allimports;    /* The list of all imports. */
    struct _moduleListDef   *imports;   /* The list of direct imports. */
    struct _moduleDef   *next;      /* Next in the list. */
} moduleDef;


/* An entry in a linked module list. */

typedef struct _moduleListDef {
    moduleDef       *module;    /* The module itself. */
    struct _moduleListDef   *next;      /* The next in the list. */
} moduleListDef;


/* A literal code block. */

typedef struct _codeBlock {
    char            *frag;      /* The code itself. */
    char            *filename;  /* The original file. */
    int         linenr;     /* The line in the file. */
    struct _codeBlock   *next;      /* Next in the list. */
} codeBlock;


/* The arguments to a throw specifier. */

typedef struct _throwArgs {
    int         nrArgs;     /* The number of arguments. */
    struct _exceptionDef    *args[MAX_NR_ARGS]; /* The arguments. */
} throwArgs;


/* An exception. */

typedef struct _exceptionDef {
    int         exceptionnr;    /* The exception number. */
    struct _ifaceFileDef    *iff;       /* The interface file. */
    char            *pyname;    /* The exception Python name. */
    struct _classDef    *cd;        /* The exception class. */
    char            *bibase;    /* The builtin base exception. */
    struct _exceptionDef    *base;      /* The defined base exception. */
    codeBlock       *hdrcode;   /* Optional header code. */
    codeBlock       *raisecode; /* Raise exception code. */
    struct _exceptionDef    *next;      /* The next in the list. */
} exceptionDef;


/* A value. */

typedef struct _valueDef {
    valueType vtype;            /* The type. */
    char vunop;                 /* Any unary operator. */
    char vbinop;                /* Any binary operator. */
    union {
        char vqchar;            /* Quoted character value. */
        long vnum;              /* Numeric value. */
        double vreal;           /* Real value. */
        char *vstr;             /* String value. */
        scopedNameDef *vscp;    /* Scoped value. */
        struct _fcallDef *fcd;  /* Function call. */
    } u;
    struct _valueDef *next;     /* Next in the expression. */
} valueDef;


/* A member function argument (or result). */

typedef struct {
    argType atype;              /* The type. */
    char *name;                 /* The name. */
    int argflags;               /* The argument flags. */
    int nrderefs;               /* Nr. of dereferences. */
    valueDef *defval;           /* The default value. */
    union {
        struct _signatureDef *sa;       /* If it is a function. */
        struct _templateDef *td;        /* If it is a template. */
        struct _scopedNameDef *snd;     /* If it is a defined type. */
        struct _classDef *cd;           /* If it is a class. */
        struct _enumDef *ed;            /* If it is an enum. */
        struct _scopedNameDef *sname;   /* If it is a struct. */
        struct _mappedTypeDef *mtd;     /* If it is a mapped type. */
    } u;
} argDef;



/* An entry in a linked argument list. */
typedef struct _argList {
    argDef          arg;        /* The argument itself. */
    struct _argList     *next;      /* Next in the list. */
} argList;


/* A function call. */

typedef struct _fcallDef {
    argDef          type;       /* The type. */
    int         nrArgs;     /* The number of arguments. */
    struct _valueDef    *args[MAX_NR_ARGS]; /* The arguments. */
} fcallDef;


/* An interface file definition. */

typedef struct _ifaceFileDef {
    nameDef         *name;      /* The name. */
    ifaceFileType       type;       /* Interface file type. */
    scopedNameDef       *fqcname;   /* The fully qualified C++ name. */
    moduleDef       *module;    /* The owning module. */
    struct _ifaceFileList   *used;      /* Interface files used. */
    struct _ifaceFileDef    *next;      /* Next in the list. */
} ifaceFileDef;


/* An entry in a linked interface file list. */

typedef struct _ifaceFileList {
    ifaceFileDef        *iff;       /* The interface file itself. */
    int         header;     /* If needed in the .h file. */
    struct _ifaceFileList   *next;      /* Next in the list. */
} ifaceFileList;


/* A mapped type. */

typedef struct _mappedTypeDef {
    argDef          type;       /* The type being mapped. */
    int         mappednr;   /* The mapped type number. */
    ifaceFileDef        *iff;       /* The interface file. */
    codeBlock       *hdrcode;   /* Header code. */
    codeBlock       *convfromcode;  /* Convert from C++ code. */
    codeBlock       *convtocode;    /* Convert to C++ code. */
    struct _mappedTypeDef   *next;      /* Next in the list. */
} mappedTypeDef;


/* A function signature. */

typedef struct _signatureDef {
    argDef          result;     /* The result. */
    int         nrArgs;     /* The number of arguments. */
    argDef          args[MAX_NR_ARGS];  /* The arguments. */
} signatureDef;


/* A list of function signatures. */

typedef struct _signatureList {
    struct _signatureDef    *sd;        /* The signature. */
    struct _signatureList   *next;      /* Next in the list. */
} signatureList;


/* A template type. */

typedef struct _templateDef {
    scopedNameDef       *fqname;    /* The name. */
    signatureDef        types;      /* The types. */
} templateDef;


/* A list of virtual handlers. */

typedef struct _virtHandlerDef {
    int         virthandlernr;  /* The nr. of the virtual handler. */
    int         vhflags;    /* The virtual handler flags. */
    signatureDef        *pysig;     /* The Python signature. */
    signatureDef        *cppsig;    /* The C++ signature. */
    struct _moduleDef   *module;    /* The defining module. */
    codeBlock       *virtcode;  /* Virtual handler code. */
    struct _virtHandlerDef  *next;      /* Next in the list. */
} virtHandlerDef;


/* A typedef definition. */

typedef struct _typedefDef {
    scopedNameDef       *fqname;    /* The fully qualified name. */
    struct _classDef    *ecd;       /* The enclosing class. */
    moduleDef       *module;    /* The owning module. */
    argDef          type;       /* The actual type. */
    struct _typedefDef  *next;      /* Next in the list. */
} typedefDef;


/* A variable definition. */

typedef struct _varDef {
    nameDef         *pyname;    /* The variable Python name. */
    scopedNameDef       *fqcname;   /* The fully qualified C/C++ name. */
    struct _classDef    *ecd;       /* The enclosing class. */
    moduleDef       *module;    /* The owning module. */
    int         varflags;   /* The variable flags. */
    argDef          type;       /* The actual type. */
    codeBlock       *accessfunc;    /* The access function. */
    codeBlock       *getcode;   /* The get code. */
    codeBlock       *setcode;   /* The set code. */
    struct _varDef      *next;      /* Next in the list. */
} varDef;


/* An overloaded member function definition. */

typedef struct _overDef {
    char            *cppname;   /* The C++ name. */
    int         overflags;  /* The overload flags. */
    struct _memberDef   *common;    /* Common parts. */
    signatureDef        pysig;      /* The Python signature. */
    signatureDef        *cppsig;    /* The C++ signature. */
    throwArgs       *exceptions;    /* The exceptions. */
    codeBlock       *methodcode;    /* Method code. */
    virtHandlerDef      *virthandler;   /* The virtual handler. */
    char            *prehook;   /* The pre-hook name. */
    char            *posthook;  /* The post-hook name. */
    struct _overDef     *next;      /* Next in the list. */
} overDef;


/* An overloaded constructor definition. */

typedef struct _ctorDef {
    int         ctorflags;  /* The ctor flags. */
    signatureDef        pysig;      /* The Python signature. */
    signatureDef        *cppsig;    /* The C++ signature, NULL if /NoDerived/. */
    throwArgs       *exceptions;    /* The exceptions. */
    codeBlock       *methodcode;    /* Method code. */
    char            *prehook;   /* The pre-hook name. */
    char            *posthook;  /* The post-hook name. */
    struct _ctorDef     *next;      /* Next in the list. */
} ctorDef;


/* An enumerated type member definition. */

typedef struct _enumMemberDef {
    nameDef         *pyname;    /* The Python name. */
    char            *cname;     /* The C/C++ name. */
    struct _enumDef     *ed;        /* The enclosing enum. */
    struct _enumMemberDef   *next;      /* Next in the list. */
} enumMemberDef;


/* An enumerated type definition. */

typedef struct _enumDef {
    int         enumflags;  /* The enum flags. */
    scopedNameDef       *fqcname;   /* The name (may be NULL). */
    nameDef         *pyname;    /* The Python name (may be NULL). */
    int         enumnr;     /* The enum number. */
    struct _classDef    *ecd;       /* The enclosing class. */
    struct _classDef    *pcd;       /* The publishing class. */
    moduleDef       *module;    /* The owning module. */
    enumMemberDef       *members;   /* The list of members. */
    struct _memberDef   *slots;     /* The list of slots. */
    struct _overDef     *overs;     /* The list of slot overloads. */
    struct _enumDef     *next;      /* Next in the list. */
} enumDef;


/* An member function definition. */

typedef struct _memberDef {
    nameDef         *pyname;    /* The Python name. */
    int         memberflags;    /* The member flags. */
    slotType        slot;       /* The slot type. */
    moduleDef       *module;    /* The owning module. */
    struct _memberDef   *next;      /* Next in the list. */
} memberDef;


/* A list of visible member functions. */

typedef struct _visibleList {
    memberDef       *m;     /* The member definition. */
    struct _classDef    *cd;        /* The class. */
    struct _visibleList *next;      /* Next in the list. */
} visibleList;


/* An entry in a linked class list. */

typedef struct _classList {
    struct _classDef    *cd;        /* The class itself. */
    struct _classList   *next;      /* Next in the list. */
} classList;


/* A virtual overload definition. */

typedef struct _virtOverDef {
    overDef         o;      /* The overload. */
    struct _classDef    *scope;     /* The overload scope. */
    struct _virtOverDef *next;      /* Next in the list. */
} virtOverDef;


/* A class that appears in a class's hierarchy. */

typedef struct _mroDef {
    struct _classDef    *cd;        /* The class. */
    int         mroflags;       /* The hierarchy flags. */
    struct _mroDef      *next;      /* The next in the list. */
} mroDef;


/* A class definition. */

typedef struct _classDef {
    int         classflags; /* The class flags. */
    int         classnr;    /* The class number. */
    char            *pyname;    /* The Python name. */
    ifaceFileDef        *iff;       /* The interface file. */
    struct _classDef    *ecd;       /* The enclosing scope. */
    struct _classDef    *real;      /* The real class if this is a proxy or extender. */
    nodeDef         *node;      /* Position in class tree. */
    classList       *supers;    /* The parent classes. */
    mroDef          *mro;       /* The super-class hierarchy. */
    templateDef     *td;        /* The instantiated template. */
    ctorDef         *ctors;     /* The constructors. */
    ctorDef         *defctor;   /* The default ctor. */
    codeBlock       *dealloccode;   /* Handwritten dealloc code. */
    codeBlock       *dtorcode;  /* Handwritten dtor code. */
    throwArgs       *dtorexceptions;    /* The dtor exceptions. */
    memberDef       *members;   /* The member functions. */
    overDef         *overs;     /* The overloads. */
    argList         *casts;     /* The operator casts. */
    virtOverDef     *vmembers;  /* The virtual members. */
    visibleList     *visible;   /* The visible members. */
    codeBlock       *cppcode;   /* Class C++ code. */
    codeBlock       *hdrcode;   /* Class header code. */
    codeBlock       *convtosubcode; /* Convert to sub C++ code. */
    struct _classDef    *subbase;   /* Sub-class base class. */
    codeBlock       *convtocode;    /* Convert to C++ code. */
    codeBlock       *travcode;  /* Traverse code. */
    codeBlock       *clearcode; /* Clear code. */
    codeBlock       *readbufcode;   /* Read buffer code. */
    codeBlock       *writebufcode;  /* Write buffer code. */
    codeBlock       *segcountcode;  /* Segment count code. */
    codeBlock       *charbufcode;   /* Character buffer code. */
    struct _classDef    *next;      /* Next in the list. */
} classDef;


/* A class template definition. */

typedef struct _classTmplDef {
    signatureDef        sig;        /* The template arguments. */
    classDef        *cd;        /* The class itself. */
    struct _classTmplDef    *next;      /* The next in the list. */
} classTmplDef;


/* A mapped type template definition. */

typedef struct _mappedTypeTmplDef {
    signatureDef        sig;        /* The template arguments. */
    mappedTypeDef       *mt;        /* The mapped type itself. */
    struct _mappedTypeTmplDef   *next;  /* The next in the list. */
} mappedTypeTmplDef;


/* The parse tree corresponding to the specification file. */

typedef struct {
    moduleDef       *module;    /* This module. */
    moduleDef       *modules;   /* The list of modules. */
    nameDef         *namecache; /* The name cache. */
    ifaceFileDef        *ifacefiles;    /* The list of interface files. */
    classDef        *classes;   /* The list of classes. */
    classTmplDef        *classtemplates;    /* The list of class templates. */
    classDef        *proxies;   /* The list of proxy classes. */
    exceptionDef        *exceptions;    /* The list of exceptions. */
    mappedTypeDef       *mappedtypes;   /* The mapped types. */
    mappedTypeTmplDef   *mappedtypetemplates;   /* The list of mapped type templates. */
    int         qobjclass;  /* QObject class, -1 if none. */
    enumDef         *enums;     /* List of enums. */
    varDef          *vars;      /* List of variables. */
    memberDef       *othfuncs;  /* List of other functions. */
    overDef         *overs;     /* Global overloads. */
    typedefDef      *typedefs;  /* List of typedefs. */
    codeBlock       *copying;   /* Software license. */
    codeBlock       *exphdrcode;    /* Exported header code. */
    codeBlock       *hdrcode;   /* Header code. */
    codeBlock       *cppcode;   /* Global C++ code. */
    codeBlock       *docs;      /* Documentation. */
    codeBlock       *preinitcode;   /* Pre-initialisation code. */
    codeBlock       *postinitcode;  /* Post-initialisation code. */
    codeBlock       *unitcode;  /* Compileation unit code. */
    ifaceFileList       *used;      /* Interface files used. */
    int         sigslots;   /* Set if signals or slots are used. */
    int         genc;       /* Set if we are generating C code. */
    struct _stringList  *options;   /* The list of options. */
} sipSpec;


/* A list of strings. */

typedef struct _stringList {
    const char      *s;     /* The string. */
    struct _stringList  *next;      /* The next in the list. */
} stringList;


/* File specific context information for the parser. */

typedef struct _parserContext {
    int         ifdepth;    /* The depth of nested if's. */
    moduleDef       *prevmod;   /* The previous module. */
} parserContext;


extern char *sipVersion;        /* The version of SIP. */
extern stringList *includeDirList;  /* The include directory list for SIP files. */


void parse(sipSpec *,FILE *,char *,stringList *,stringList *);
void parserEOF(char *,parserContext *);
void transform(sipSpec *);
void generateCode(sipSpec *,char *,char *,char *,char *,int,int,int,int,stringList *);
void generateAPI(sipSpec *pt, const char *apiFile);
void generateXML(sipSpec *pt, const char *xmlFile);
void generateExpression(valueDef *vd, FILE *fp);
void warning(char *,...);
void fatal(char *,...);
void fatalScopedName(scopedNameDef *);
void setInputFile(FILE *,char *,parserContext *,int);
void *sipMalloc(size_t);
char *sipStrdup(char *);
char *concat(const char *, ...);
void append(char **,char *);
ifaceFileList *addToUsedList(ifaceFileList **, ifaceFileDef *);
int excludedFeature(stringList *,qualDef *);
int sameSignature(signatureDef *,signatureDef *,int);
int sameTemplateSignature(signatureDef *sd1, signatureDef *sd2, int deep);
int sameScopedName(scopedNameDef *,scopedNameDef *);
int sameBaseType(argDef *,argDef *);
char *scopedNameTail(scopedNameDef *);
scopedNameDef *text2scopePart(char *);
scopedNameDef *copyScopedName(scopedNameDef *);
void appendScopedName(scopedNameDef **,scopedNameDef *);
void freeScopedName(scopedNameDef *);
void appendToClassList(classList **,classDef *);
void prcode(FILE *fp, const char *fmt, ...);
void prOverloadName(FILE *fp, overDef *od);
void prScopedPythonName(FILE *fp, classDef *scope, const char *pyname);
void prOverloadDecl(FILE *fp, overDef *od, int defval);
int isIntReturnSlot(memberDef *md);
int isLongReturnSlot(memberDef *md);
int isVoidReturnSlot(memberDef *md);
int isNumberSlot(memberDef *md);
int isRichCompareSlot(memberDef *md);
mappedTypeDef *allocMappedType(argDef *type);
void appendString(stringList **headp, const char *s);
void appendTypeStrings(scopedNameDef *ename, signatureDef *patt, signatureDef *src, signatureDef *known, scopedNameDef **names, scopedNameDef **values);
codeBlock *templateCode(sipSpec *pt, ifaceFileList **used, codeBlock *ocb, scopedNameDef *names, scopedNameDef *values);
ifaceFileDef *findIfaceFile(sipSpec *pt, moduleDef *mod, scopedNameDef *fqname, ifaceFileType iftype, argDef *ad);
int optNoEmitters(sipSpec *pt);
int optRegisterTypes(sipSpec *pt);
void yywarning(char *);


/* These are only here because bison publically references them. */

/* Represent a set of option flags. */

#define MAX_NR_FLAGS    5

typedef enum {
    bool_flag,
    string_flag,
    name_flag,
    opt_name_flag,
    integer_flag
} flagType;

typedef struct {
    char        *fname;         /* The flag name. */
    flagType    ftype;          /* The flag type. */
    union {                 /* The flag value. */
        char    *sval;          /* A string value. */
        long    ival;           /* An integer value. */
    } fvalue;
} optFlag;

typedef struct {
    int     nrFlags;        /* The number of flags. */
    optFlag     flags[MAX_NR_FLAGS];    /* Each flag. */
} optFlags;

#endif
