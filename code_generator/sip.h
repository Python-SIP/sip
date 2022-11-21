/*
 * The main header file for SIP.
 *
 * Copyright (c) 2022 Riverbank Computing Limited <info@riverbankcomputing.com>
 *
 * This file is part of SIP.
 *
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 *
 * This copy of SIP may also used under the terms of the GNU General Public
 * License v2 or v3 as published by the Free Software Foundation which can be
 * found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
 *
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef SIP_H
#define SIP_H

#include <stdio.h>
#include <sys/types.h>


/* Use symbolic names for (significant) ABI versions. */

#define ABI_13_4    0x0d04
#define ABI_13_1    0x0d01
#define ABI_13_0    0x0d00
#define ABI_12_11   0x0c0b
#define ABI_12_9    0x0c09
#define ABI_12_8    0x0c08


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
#define MAX_NR_DEREFS       5       /* Max. nr. type derefences. */


/* For convenience. */

#define classBaseName(cd)   scopedNameTail((cd)->iff->fqcname)
#define classFQCName(cd)    ((cd)->iff->fqcname)

/* Return TRUE if the ABI supports sip.array. */
#define abiSupportsArray()  (abiVersion >= ABI_13_4 || (abiVersion >= ABI_12_11 && abiVersion < ABI_13_0))

/* Return the Python scope corresponding to a C/C++ scope. */
#define pyScope(c)          ((c) != NULL && isHiddenNamespace(c) ? NULL : (c))

/* Control how scopes should be stripped. */
#define STRIP_NONE      0       /* This must be 0. */
#define STRIP_GLOBAL    (-1)    /* This must be -ve. */


/* Handle module flags. */

#define MOD_HAS_DELAYED_DTORS   0x0001  /* It has a class with a delayed dtor. */
#define MOD_IS_UNUSED           0x0002  /* This flag is unused. */
#define MOD_IS_COMPOSITE        0x0004  /* It is a composite module. */
#define MOD_IS_TRANSFORMED      0x0008  /* It's types have been transformed. */
#define MOD_USE_ARG_NAMES       0x0010  /* Use real argument names. */
#define MOD_USE_LIMITED_API     0x0020  /* Use the limited API. */
#define MOD_ALL_RAISE_PY_EXC    0x0040  /* All callable raise a Python exception. */
#define MOD_SUPER_INIT_NO       0x0080  /* Don't call super().__init__(). */
#define MOD_SUPER_INIT_YES      0x0100  /* Call super().__init__(). */
#define MOD_SUPER_INIT_UNDEF    0x0000  /* Calling super().__init__() is undefined. */
#define MOD_SUPER_INIT_MASK     0x0180  /* The mask for the above flags. */
#define MOD_PY_SSIZE_T_CLEAN    0x0200  /* #define PY_SSIZE_T_CLEAN. */

#define hasDelayedDtors(m)  ((m)->modflags & MOD_HAS_DELAYED_DTORS)
#define setHasDelayedDtors(m)   ((m)->modflags |= MOD_HAS_DELAYED_DTORS)
#define isComposite(m)      ((m)->modflags & MOD_IS_COMPOSITE)
#define setIsComposite(m)   ((m)->modflags |= MOD_IS_COMPOSITE)
#define setIsTransformed(m) ((m)->modflags |= MOD_IS_TRANSFORMED)
#define isTransformed(m)    ((m)->modflags & MOD_IS_TRANSFORMED)
#define setUseArgNames(m)   ((m)->modflags |= MOD_USE_ARG_NAMES)
#define useArgNames(m)      ((m)->modflags & MOD_USE_ARG_NAMES)
#define setUseLimitedAPI(m) ((m)->modflags |= MOD_USE_LIMITED_API)
#define useLimitedAPI(m)    ((m)->modflags & MOD_USE_LIMITED_API)
#define setAllRaisePyException(m)   ((m)->modflags |= MOD_ALL_RAISE_PY_EXC)
#define allRaisePyException(m)  ((m)->modflags & MOD_ALL_RAISE_PY_EXC)
#define setCallSuperInitNo(m)   ((m)->modflags = ((m)->modflags & ~MOD_SUPER_INIT_MASK) | MOD_SUPER_INIT_NO)
#define setCallSuperInitYes(m)  ((m)->modflags = ((m)->modflags & ~MOD_SUPER_INIT_MASK) | MOD_SUPER_INIT_YES)
#define isCallSuperInitYes(m)   (((m)->modflags & MOD_SUPER_INIT_MASK) == MOD_SUPER_INIT_YES)
#define isCallSuperInitUndefined(m) (((m)->modflags & MOD_SUPER_INIT_MASK) == MOD_SUPER_INIT_UNDEF)
#define setPY_SSIZE_T_CLEAN(m)  ((m)->modflags |= MOD_PY_SSIZE_T_CLEAN)
#define isPY_SSIZE_T_CLEAN(m)   ((m)->modflags & MOD_PY_SSIZE_T_CLEAN)


/* Handle section flags. */

#define SECT_IS_PUBLIC      0x01        /* It is public. */
#define SECT_IS_PROT        0x02        /* It is protected. */
#define SECT_IS_PRIVATE     0x04        /* It is private. */
#define SECT_IS_SLOT        0x08        /* It is a slot. */
#define SECT_IS_SIGNAL      0x10        /* It is a signal. */
#define SECT_MASK           0x1f        /* The mask of all flags. */


/* Handle class flags.  These are combined with the section flags. */

#define CLASS_HAS_SIGSLOTS  0x00000200  /* It has signals or slots. */
#define CLASS_IS_ABSTRACT   0x00000400  /* It is an abstract class. */
#define CLASS_HAS_SHADOW    0x00000800  /* It is has a shadow class. */
#define CLASS_IS_OPAQUE     0x00001000  /* It is opaque. */
#define CLASS_HAS_VAR_HANDLERS  0x00002000  /* It has variable handlers. */
#define CLASS_DTOR_RELEASE_GIL  0x00004000  /* The dtor releases the GIL. */
#define CLASS_IS_PROTECTED  0x00008000  /* It is protected. */
#define CLASS_IS_PROTECTED_SAV  0x00010000  /* It is protected (saved). */
#define CLASS_IS_INCOMPLETE 0x00020000  /* The specification is incomplete. */
#define CLASS_CAN_CREATE    0x00040000  /* It has usable ctors. */
#define CLASS_IS_EXTERNAL   0x00080000  /* It is external. */
#define CLASS_IS_DELAYED_DTOR   0x00100000  /* The dtor is delayed. */
#define CLASS_NO_DEFAULT_CTORS  0x00200000  /* Don't create default ctors. */
#define CLASS_QOBJECT_SUB   0x00400000  /* It is derived from QObject. */
#define CLASS_DTOR_HOLD_GIL 0x00800000  /* The dtor holds the GIL. */
#define CLASS_ARRAY_HELPER  0x01000000  /* Generate an array helper. */
#define CLASS_NO_QMETAOBJECT    0x02000000  /* It has no QMetaObject. */
#define CLASS_IS_DEPRECATED 0x04000000  /* It is deprecated. */
#define CLASS_CANNOT_COPY   0x08000000  /* It cannot be copied. */
#define CLASS_CANNOT_ASSIGN 0x10000000  /* It cannot be assigned. */
#define CLASS_ALLOW_NONE    0x20000000  /* The class will handle None. */
#define CLASS_HAS_NONLAZY   0x40000000  /* The class has non-lazy methods. */

#define hasSigSlots(cd)     ((cd)->classflags & CLASS_HAS_SIGSLOTS)
#define setHasSigSlots(cd)  ((cd)->classflags |= CLASS_HAS_SIGSLOTS)
#define isAbstractClass(cd) ((cd)->classflags & CLASS_IS_ABSTRACT)
#define setIsAbstractClass(cd)  ((cd)->classflags |= CLASS_IS_ABSTRACT)
#define hasShadow(cd)       ((cd)->classflags & CLASS_HAS_SHADOW)
#define setHasShadow(cd)    ((cd)->classflags |= CLASS_HAS_SHADOW)
#define resetHasShadow(cd)  ((cd)->classflags &= ~CLASS_HAS_SHADOW)
#define isOpaque(cd)        ((cd)->classflags & CLASS_IS_OPAQUE)
#define setIsOpaque(cd)     ((cd)->classflags |= CLASS_IS_OPAQUE)
#define hasVarHandlers(cd)  ((cd)->classflags & CLASS_HAS_VAR_HANDLERS)
#define setHasVarHandlers(cd)   ((cd)->classflags |= CLASS_HAS_VAR_HANDLERS)
#define isProtectedClass(cd)    ((cd)->classflags & CLASS_IS_PROTECTED)
#define setIsProtectedClass(cd) ((cd)->classflags |= CLASS_IS_PROTECTED)
#define resetIsProtectedClass(cd)   ((cd)->classflags &= ~CLASS_IS_PROTECTED)
#define wasProtectedClass(cd)   ((cd)->classflags & CLASS_IS_PROTECTED_SAV)
#define setWasProtectedClass(cd)    ((cd)->classflags |= CLASS_IS_PROTECTED_SAV)
#define resetWasProtectedClass(cd)  ((cd)->classflags &= ~CLASS_IS_PROTECTED_SAV)
#define isReleaseGILDtor(cd)    ((cd)->classflags & CLASS_DTOR_RELEASE_GIL)
#define setIsReleaseGILDtor(cd) ((cd)->classflags |= CLASS_DTOR_RELEASE_GIL)
#define isIncomplete(cd)    ((cd)->classflags & CLASS_IS_INCOMPLETE)
#define setIsIncomplete(cd) ((cd)->classflags |= CLASS_IS_INCOMPLETE)
#define canCreate(cd)       ((cd)->classflags & CLASS_CAN_CREATE)
#define setCanCreate(cd)    ((cd)->classflags |= CLASS_CAN_CREATE)
#define resetCanCreate(cd)  ((cd)->classflags &= ~CLASS_CAN_CREATE)
#define isExternal(cd)      ((cd)->classflags & CLASS_IS_EXTERNAL)
#define setIsExternal(cd)   ((cd)->classflags |= CLASS_IS_EXTERNAL)
#define isDelayedDtor(cd)   ((cd)->classflags & CLASS_IS_DELAYED_DTOR)
#define setIsDelayedDtor(cd)    ((cd)->classflags |= CLASS_IS_DELAYED_DTOR)
#define noDefaultCtors(cd)  ((cd)->classflags & CLASS_NO_DEFAULT_CTORS)
#define setNoDefaultCtors(cd)   ((cd)->classflags |= CLASS_NO_DEFAULT_CTORS)
#define isQObjectSubClass(cd)   ((cd)->classflags & CLASS_QOBJECT_SUB)
#define setIsQObjectSubClass(cd)    ((cd)->classflags |= CLASS_QOBJECT_SUB)
#define isHoldGILDtor(cd)   ((cd)->classflags & CLASS_DTOR_HOLD_GIL)
#define setIsHoldGILDtor(cd) ((cd)->classflags |= CLASS_DTOR_HOLD_GIL)
#define arrayHelper(cd)     ((cd)->classflags & CLASS_ARRAY_HELPER)
#define setArrayHelper(cd)  ((cd)->classflags |= CLASS_ARRAY_HELPER)
#define noPyQtQMetaObject(cd)   ((cd)->classflags & CLASS_NO_QMETAOBJECT)
#define setPyQtNoQMetaObject(cd)    ((cd)->classflags |= CLASS_NO_QMETAOBJECT)
#define isDeprecatedClass(cd)   ((cd)->classflags & CLASS_IS_DEPRECATED)
#define setIsDeprecatedClass(cd)    ((cd)->classflags |= CLASS_IS_DEPRECATED)
#define cannotCopy(cd)      ((cd)->classflags & CLASS_CANNOT_COPY)
#define setCannotCopy(cd)   ((cd)->classflags |= CLASS_CANNOT_COPY)
#define cannotAssign(cd)    ((cd)->classflags & CLASS_CANNOT_ASSIGN)
#define setCannotAssign(cd) ((cd)->classflags |= CLASS_CANNOT_ASSIGN)
#define classHandlesNone(cd)    ((cd)->classflags & CLASS_ALLOW_NONE)
#define setClassHandlesNone(cd) ((cd)->classflags |= CLASS_ALLOW_NONE)
#define hasNonlazyMethod(cd)    ((cd)->classflags & CLASS_HAS_NONLAZY)
#define setHasNonlazyMethod(cd) ((cd)->classflags |= CLASS_HAS_NONLAZY)

#define isPublicDtor(cd)    ((cd)->classflags & SECT_IS_PUBLIC)
#define setIsPublicDtor(cd) ((cd)->classflags |= SECT_IS_PUBLIC)
#define isProtectedDtor(cd) ((cd)->classflags & SECT_IS_PROT)
#define isPrivateDtor(cd)   ((cd)->classflags & SECT_IS_PRIVATE)

#define isDtor(cd)          ((cd)->classflags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE))


/* Handle the second group of class flags. */

#define CLASS2_MIXIN        0x01        /* The class is a mixin. */
#define CLASS2_EXPORT_DERIVED   0x02    /* Export the derived class declaration. */
#define CLASS2_HIDDEN_NS    0x04        /* The namespace is hidden. */
#define CLASS2_USE_TMPL_NAME    0x08    /* Use the template name. */
#define CLASS2_NEEDS_SHADOW 0x10        /* The class needs a shadow class. */
#define CLASS2_COPY_HELPER  0x20        /* Generate a copy helper. */
#define CLASS2_UNION        0x40        /* The class is a union. */

#define isMixin(cd)         ((cd)->classflags2 & CLASS2_MIXIN)
#define setMixin(cd)        ((cd)->classflags2 |= CLASS2_MIXIN)
#define isExportDerived(cd) ((cd)->classflags2 & CLASS2_EXPORT_DERIVED)
#define setExportDerived(cd)    ((cd)->classflags2 |= CLASS2_EXPORT_DERIVED)
#define isHiddenNamespace(cd)   ((cd)->classflags2 & CLASS2_HIDDEN_NS)
#define setHiddenNamespace(cd)  ((cd)->classflags2 |= CLASS2_HIDDEN_NS)
#define useTemplateName(cd) ((cd)->classflags2 & CLASS2_USE_TMPL_NAME)
#define setUseTemplateName(cd)  ((cd)->classflags2 |= CLASS2_USE_TMPL_NAME)
#define needsShadow(cd)      ((cd)->classflags2 & CLASS2_NEEDS_SHADOW)
#define setNeedsShadow(cd)   ((cd)->classflags2 |= CLASS2_NEEDS_SHADOW)
#define copyHelper(cd)      ((cd)->classflags2 & CLASS2_COPY_HELPER)
#define setCopyHelper(cd)   ((cd)->classflags2 |= CLASS2_COPY_HELPER)
#define isUnion(cd)         ((cd)->classflags2 & CLASS2_UNION)
#define setUnion(cd)        ((cd)->classflags2 |= CLASS2_UNION)


/* Handle ctor flags.  These are combined with the section flags. */

#define CTOR_RELEASE_GIL    0x00000100  /* The ctor releases the GIL. */
#define CTOR_EXPLICIT       0x00000200  /* The ctor is explicit. */
#define CTOR_CAST           0x00000400  /* The ctor is a cast. */
#define CTOR_HOLD_GIL       0x00000800  /* The ctor holds the GIL. */
#define CTOR_XFERRED        0x00001000  /* Ownership is transferred. */
#define CTOR_IS_DEPRECATED  0x00002000  /* The ctor is deprecated. */
#define CTOR_RAISES_PY_EXC  0x00004000  /* It raises a Python exception. */

#define isPublicCtor(c)     ((c)->ctorflags & SECT_IS_PUBLIC)
#define setIsPublicCtor(c)  ((c)->ctorflags |= SECT_IS_PUBLIC)
#define isProtectedCtor(c)  ((c)->ctorflags & SECT_IS_PROT)
#define setIsProtectedCtor(c)   ((c)->ctorflags |= SECT_IS_PROT)
#define isPrivateCtor(c)    ((c)->ctorflags & SECT_IS_PRIVATE)
#define setIsPrivateCtor(c) ((c)->ctorflags |= SECT_IS_PRIVATE)

#define isReleaseGILCtor(c) ((c)->ctorflags & CTOR_RELEASE_GIL)
#define setIsReleaseGILCtor(c)  ((c)->ctorflags |= CTOR_RELEASE_GIL)
#define isExplicitCtor(c)   ((c)->ctorflags & CTOR_EXPLICIT)
#define setIsExplicitCtor(c)    ((c)->ctorflags |= CTOR_EXPLICIT)
#define isCastCtor(c)       ((c)->ctorflags & CTOR_CAST)
#define isHoldGILCtor(c)    ((c)->ctorflags & CTOR_HOLD_GIL)
#define setIsHoldGILCtor(c) ((c)->ctorflags |= CTOR_HOLD_GIL)
#define isResultTransferredCtor(c)  ((c)->ctorflags & CTOR_XFERRED)
#define setIsResultTransferredCtor(c)   ((c)->ctorflags |= CTOR_XFERRED)
#define isDeprecatedCtor(c) ((c)->ctorflags & CTOR_IS_DEPRECATED)
#define setIsDeprecatedCtor(c)  ((c)->ctorflags |= CTOR_IS_DEPRECATED)
#define raisesPyExceptionCtor(c)    ((c)->ctorflags & CTOR_RAISES_PY_EXC)
#define setRaisesPyExceptionCtor(c) ((c)->ctorflags |= CTOR_RAISES_PY_EXC)


/* Handle member flags. */

#define MEMBR_NUMERIC       0x0001      /* It is a numeric slot. */
#define MEMBR_SEQUENCE      0x0002      /* It is a sequnce slot. */
#define MEMBR_NO_ARG_PARSER 0x0004      /* Don't generate an argument parser. */
#define MEMBR_KEYWORD_ARGS  0x0008      /* It allows keyword arguments. */
#define MEMBR_HAS_PROTECTED 0x0010      /* It has a protected overload. */

#define isNumeric(m)        ((m)->memberflags & MEMBR_NUMERIC)
#define setIsNumeric(m)     ((m)->memberflags |= MEMBR_NUMERIC)
#define isSequence(m)       ((m)->memberflags & MEMBR_SEQUENCE)
#define setIsSequence(m)    ((m)->memberflags |= MEMBR_SEQUENCE)
#define noArgParser(m)      ((m)->memberflags & MEMBR_NO_ARG_PARSER)
#define setNoArgParser(m)   ((m)->memberflags |= MEMBR_NO_ARG_PARSER)
#define useKeywordArgs(m)   ((m)->memberflags & MEMBR_KEYWORD_ARGS)
#define setUseKeywordArgs(m)    ((m)->memberflags |= MEMBR_KEYWORD_ARGS)
#define hasProtected(m)     ((m)->memberflags & MEMBR_HAS_PROTECTED)
#define setHasProtected(m)  ((m)->memberflags |= MEMBR_HAS_PROTECTED)


/* Handle enum flags.  These are combined with the section flags. */

#define ENUM_WAS_PROT       0x00000100  /* It was defined as protected. */
#define ENUM_NO_SCOPE       0x00000200  /* Omit the member scopes. */
#define ENUM_NEEDS_ENUM     0x00000400  /* The module needs it. */
#define ENUM_SCOPED         0x00000800  /* A C++0x11 scoped enum. */
#define ENUM_MASK           0x00007000  /* The enum base type mask. */
#define ENUM_ENUM           0x00000000  /* The base type is Enum. */
#define ENUM_FLAG           0x00001000  /* The base type is Flag. */
#define ENUM_INT_ENUM       0x00002000  /* The base type is IntEnum. */
#define ENUM_INT_FLAG       0x00003000  /* The base type is IntFlag. */
#define ENUM_UINT_ENUM      0x00004000  /* The base type is IntEnum with unsigned members. */

#define isProtectedEnum(e)  ((e)->enumflags & SECT_IS_PROT)
#define setIsProtectedEnum(e)   ((e)->enumflags |= SECT_IS_PROT)
#define resetIsProtectedEnum(e) ((e)->enumflags &= ~SECT_IS_PROT)

#define wasProtectedEnum(e) ((e)->enumflags & ENUM_WAS_PROT)
#define setWasProtectedEnum(e)  ((e)->enumflags |= ENUM_WAS_PROT)
#define resetWasProtectedEnum(e)    ((e)->enumflags &= ~ENUM_WAS_PROT)
#define isNoScope(e)        ((e)->enumflags & ENUM_NO_SCOPE)
#define setIsNoScope(e)     ((e)->enumflags |= ENUM_NO_SCOPE)
#define needsEnum(e)        ((e)->enumflags & ENUM_NEEDS_ENUM)
#define setNeedsEnum(e)     ((e)->enumflags |= ENUM_NEEDS_ENUM)
#define isScopedEnum(e)     ((e)->enumflags & ENUM_SCOPED)
#define setIsScopedEnum(e)  ((e)->enumflags |= ENUM_SCOPED)

#define isEnumEnum(e)       (((e)->enumflags & ENUM_MASK) == ENUM_ENUM)
#define setIsEnumEnum(e)    ((e)->enumflags |= ENUM_ENUM)
#define isEnumFlag(e)       (((e)->enumflags & ENUM_MASK) == ENUM_FLAG)
#define setIsEnumFlag(e)    ((e)->enumflags |= ENUM_FLAG)
#define isEnumIntEnum(e)    (((e)->enumflags & ENUM_MASK) == ENUM_INT_ENUM)
#define setIsEnumIntEnum(e) ((e)->enumflags |= ENUM_INT_ENUM)
#define isEnumIntFlag(e)    (((e)->enumflags & ENUM_MASK) == ENUM_INT_FLAG)
#define setIsEnumIntFlag(e) ((e)->enumflags |= ENUM_INT_FLAG)
#define isEnumUIntEnum(e)   (((e)->enumflags & ENUM_MASK) == ENUM_UINT_ENUM)
#define setIsEnumUIntEnum(e)    ((e)->enumflags |= ENUM_UINT_ENUM)


/* Handle overload flags.  These are combined with the section flags. */

#define OVER_IS_VIRTUAL     0x00000100  /* It is virtual. */
#define OVER_IS_ABSTRACT    0x00000200  /* It is abstract. */
#define OVER_IS_CONST       0x00000400  /* It is a const function. */
#define OVER_IS_STATIC      0x00000800  /* It is a static function. */
#define OVER_IS_AUTOGEN     0x00001000  /* It is auto-generated. */
#define OVER_IS_NEW_THREAD  0x00002000  /* It is in a new thread. */
#define OVER_IS_FACTORY     0x00004000  /* It is a factory method. */
#define OVER_XFERRED_BACK   0x00008000  /* Ownership is transferred back. */
#define OVER_XFERRED        0x00010000  /* Ownership is transferred. */
#define OVER_IS_VIRTUAL_REIMP   0x00020000  /* It is a re-implementation of a virtual. */
#define OVER_DONT_DEREF_SELF    0x00040000  /* For comparison operators, don't dereference self. */
#define OVER_HOLD_GIL       0x00080000  /* The function holds the GIL. */
#define OVER_RELEASE_GIL    0x00100000  /* The function releases the GIL. */
#define OVER_THIS_XFERRED   0x00200000  /* Ownership of this is transferred. */
#define OVER_IS_GLOBAL      0x00400000  /* It is a global operator. */
#define OVER_IS_COMPLEMENTARY   0x00800000  /* It is a complementary operator. */
#define OVER_IS_DEPRECATED  0x01000000  /* It is deprecated. */
#define OVER_REALLY_PROT    0x02000000  /* It really is protected. */
#define OVER_IS_DELATTR     0x04000000  /* It is __delattr__. */
#define OVER_RAISES_PY_EXC  0x08000000  /* It raises a Python exception. */
#define OVER_NO_ERROR_HANDLER   0x10000000  /* It doesn't use a virtual error handler. */
#define OVER_ABORT_ON_EXC   0x20000000  /* It aborts on an exception. */
#define OVER_IS_FINAL       0x40000000  /* It is a final method. */
#define OVER_IS_REFLECTED   0x80000000  /* It is a reflected slot. */

#define isPublic(o)         ((o)->overflags & SECT_IS_PUBLIC)
#define setIsPublic(o)      ((o)->overflags |= SECT_IS_PUBLIC)
#define isProtected(o)      ((o)->overflags & SECT_IS_PROT)
#define setIsProtected(o)   ((o)->overflags |= SECT_IS_PROT)
#define isPrivate(o)        ((o)->overflags & SECT_IS_PRIVATE)
#define setIsPrivate(o)     ((o)->overflags |= SECT_IS_PRIVATE)
#define isSlot(o)           ((o)->overflags & SECT_IS_SLOT)
#define setIsSlot(o)        ((o)->overflags |= SECT_IS_SLOT)
#define resetIsSlot(o)      ((o)->overflags &= ~SECT_IS_SLOT)
#define isSignal(o)         ((o)->overflags & SECT_IS_SIGNAL)
#define setIsSignal(o)      ((o)->overflags |= SECT_IS_SIGNAL)
#define resetIsSignal(o)    ((o)->overflags &= ~SECT_IS_SIGNAL)

#define isVirtual(o)        ((o)->overflags & OVER_IS_VIRTUAL)
#define setIsVirtual(o)     ((o)->overflags |= OVER_IS_VIRTUAL)
#define resetIsVirtual(o)   ((o)->overflags &= ~OVER_IS_VIRTUAL)
#define isAbstract(o)       ((o)->overflags & OVER_IS_ABSTRACT)
#define setIsAbstract(o)    ((o)->overflags |= OVER_IS_ABSTRACT)
#define isConst(o)          ((o)->overflags & OVER_IS_CONST)
#define setIsConst(o)       ((o)->overflags |= OVER_IS_CONST)
#define isStatic(o)         ((o)->overflags & OVER_IS_STATIC)
#define setIsStatic(o)      ((o)->overflags |= OVER_IS_STATIC)
#define isAutoGen(o)        ((o)->overflags & OVER_IS_AUTOGEN)
#define setIsAutoGen(o)     ((o)->overflags |= OVER_IS_AUTOGEN)
#define resetIsAutoGen(o)   ((o)->overflags &= ~OVER_IS_AUTOGEN)
#define isNewThread(o)      ((o)->overflags & OVER_IS_NEW_THREAD)
#define setIsNewThread(o)   ((o)->overflags |= OVER_IS_NEW_THREAD)
#define isFactory(o)        ((o)->overflags & OVER_IS_FACTORY)
#define setIsFactory(o)     ((o)->overflags |= OVER_IS_FACTORY)
#define isResultTransferredBack(o)  ((o)->overflags & OVER_XFERRED_BACK)
#define setIsResultTransferredBack(o)   ((o)->overflags |= OVER_XFERRED_BACK)
#define isResultTransferred(o)  ((o)->overflags & OVER_XFERRED)
#define setIsResultTransferred(o)   ((o)->overflags |= OVER_XFERRED)
#define isVirtualReimp(o)   ((o)->overflags & OVER_IS_VIRTUAL_REIMP)
#define setIsVirtualReimp(o)    ((o)->overflags |= OVER_IS_VIRTUAL_REIMP)
#define dontDerefSelf(o)    ((o)->overflags & OVER_DONT_DEREF_SELF)
#define setDontDerefSelf(o) ((o)->overflags |= OVER_DONT_DEREF_SELF)
#define isHoldGIL(o)        ((o)->overflags & OVER_HOLD_GIL)
#define setIsHoldGIL(o)     ((o)->overflags |= OVER_HOLD_GIL)
#define isReleaseGIL(o)     ((o)->overflags & OVER_RELEASE_GIL)
#define setIsReleaseGIL(o)  ((o)->overflags |= OVER_RELEASE_GIL)
#define isThisTransferredMeth(o)    ((o)->overflags & OVER_THIS_XFERRED)
#define setIsThisTransferredMeth(o) ((o)->overflags |= OVER_THIS_XFERRED)
#define isGlobal(o)         ((o)->overflags & OVER_IS_GLOBAL)
#define setIsGlobal(o)      ((o)->overflags |= OVER_IS_GLOBAL)
#define isComplementary(o)  ((o)->overflags & OVER_IS_COMPLEMENTARY)
#define setIsComplementary(o)   ((o)->overflags |= OVER_IS_COMPLEMENTARY)
#define isDeprecated(o)     ((o)->overflags & OVER_IS_DEPRECATED)
#define setIsDeprecated(o)  ((o)->overflags |= OVER_IS_DEPRECATED)
#define isReallyProtected(o)    ((o)->overflags & OVER_REALLY_PROT)
#define setIsReallyProtected(o) ((o)->overflags |= OVER_REALLY_PROT)
#define isDelattr(o)        ((o)->overflags & OVER_IS_DELATTR)
#define setIsDelattr(o)     ((o)->overflags |= OVER_IS_DELATTR)
#define raisesPyException(o)    ((o)->overflags & OVER_RAISES_PY_EXC)
#define setRaisesPyException(o) ((o)->overflags |= OVER_RAISES_PY_EXC)
#define noErrorHandler(o)   ((o)->overflags & OVER_NO_ERROR_HANDLER)
#define setNoErrorHandler(o)    ((o)->overflags |= OVER_NO_ERROR_HANDLER)
#define abortOnException(o)     ((o)->overflags & OVER_ABORT_ON_EXC)
#define setAbortOnException(o)  ((o)->overflags |= OVER_ABORT_ON_EXC)
#define isFinal(o)          ((o)->overflags & OVER_IS_FINAL)
#define setIsFinal(o)       ((o)->overflags |= OVER_IS_FINAL)
#define isReflected(o)      ((o)->overflags & OVER_IS_REFLECTED)
#define setIsReflected(o)   ((o)->overflags |= OVER_IS_REFLECTED)


/* Handle variable flags. */

#define VAR_IS_STATIC       0x01    /* It is a static variable. */
#define VAR_NEEDS_HANDLER   0x02    /* The variable needs a handler. */
#define VAR_NO_SETTER       0x04    /* The variable has no setter. */

#define isStaticVar(v)      ((v)->varflags & VAR_IS_STATIC)
#define setIsStaticVar(v)   ((v)->varflags |= VAR_IS_STATIC)
#define needsHandler(v)     ((v)->varflags & VAR_NEEDS_HANDLER)
#define setNeedsHandler(v)  ((v)->varflags |= VAR_NEEDS_HANDLER)
#define noSetter(v)         ((v)->varflags & VAR_NO_SETTER)
#define setNoSetter(v)      ((v)->varflags |= VAR_NO_SETTER)


/* Handle argument flags. */

#define ARG_IS_REF          0x00000001  /* It is a reference. */
#define ARG_IS_CONST        0x00000002  /* It is a const. */
#define ARG_XFERRED         0x00000004  /* Ownership is transferred. */
#define ARG_THIS_XFERRED    0x00000008  /* Ownership of this is transferred. */
#define ARG_XFERRED_BACK    0x00000010  /* Ownership is transferred back. */
#define ARG_ARRAY           0x00000020  /* Used as an array. */
#define ARG_ARRAY_SIZE      0x00000040  /* Used as an array size. */
#define ARG_ALLOW_NONE      0x00000080  /* Allow None as a value. */
#define ARG_GET_WRAPPER     0x00000100  /* Get the wrapper object. */
#define ARG_IN              0x00000200  /* It passes an argument. */
#define ARG_OUT             0x00000400  /* It returns a result. */
#define ARG_CONSTRAINED     0x00000800  /* Suppress type conversion. */
#define ARG_SINGLE_SHOT     0x00001000  /* The slot is only ever fired once. */
#define ARG_RESULT_SIZE     0x00002000  /* It defines the result size. */
#define ARG_KEEP_REF        0x00004000  /* Keep a reference. */
#define ARG_NO_COPY         0x00008000  /* Disable copying of const refs. */
#define ARG_DISALLOW_NONE   0x00010000  /* Disallow None as a value. */

#define isReference(a)      ((a)->argflags & ARG_IS_REF)
#define setIsReference(a)   ((a)->argflags |= ARG_IS_REF)
#define resetIsReference(a) ((a)->argflags &= ~ARG_IS_REF)
#define isConstArg(a)       ((a)->argflags & ARG_IS_CONST)
#define setIsConstArg(a)    ((a)->argflags |= ARG_IS_CONST)
#define resetIsConstArg(a)  ((a)->argflags &= ~ARG_IS_CONST)
#define isTransferred(a)    ((a)->argflags & ARG_XFERRED)
#define setIsTransferred(a) ((a)->argflags |= ARG_XFERRED)
#define resetIsTransferred(a)   ((a)->argflags &= ~ARG_XFERRED)
#define isThisTransferred(a)    ((a)->argflags & ARG_THIS_XFERRED)
#define setIsThisTransferred(a) ((a)->argflags |= ARG_THIS_XFERRED)
#define isTransferredBack(a)    ((a)->argflags & ARG_XFERRED_BACK)
#define setIsTransferredBack(a) ((a)->argflags |= ARG_XFERRED_BACK)
#define isArray(a)          ((a)->argflags & ARG_ARRAY)
#define setArray(a)         ((a)->argflags |= ARG_ARRAY)
#define isArraySize(a)      ((a)->argflags & ARG_ARRAY_SIZE)
#define setArraySize(a)     ((a)->argflags |= ARG_ARRAY_SIZE)
#define isAllowNone(a)      ((a)->argflags & ARG_ALLOW_NONE)
#define setAllowNone(a)     ((a)->argflags |= ARG_ALLOW_NONE)
#define isGetWrapper(a)     ((a)->argflags & ARG_GET_WRAPPER)
#define setGetWrapper(a)    ((a)->argflags |= ARG_GET_WRAPPER)
#define isInArg(a)          ((a)->argflags & ARG_IN)
#define setIsInArg(a)       ((a)->argflags |= ARG_IN)
#define isOutArg(a)         ((a)->argflags & ARG_OUT)
#define setIsOutArg(a)      ((a)->argflags |= ARG_OUT)
#define isConstrained(a)    ((a)->argflags & ARG_CONSTRAINED)
#define setIsConstrained(a) ((a)->argflags |= ARG_CONSTRAINED)
#define resetIsConstrained(a)   ((a)->argflags &= ~ARG_CONSTRAINED)
#define isSingleShot(a)     ((a)->argflags & ARG_SINGLE_SHOT)
#define isResultSize(a)     ((a)->argflags & ARG_RESULT_SIZE)
#define setResultSize(a)    ((a)->argflags |= ARG_RESULT_SIZE)
#define keepReference(a)    ((a)->argflags & ARG_KEEP_REF)
#define setKeepReference(a) ((a)->argflags |= ARG_KEEP_REF)
#define noCopy(a)           ((a)->argflags & ARG_NO_COPY)
#define setNoCopy(a)        ((a)->argflags |= ARG_NO_COPY)
#define isDisallowNone(a)   ((a)->argflags & ARG_DISALLOW_NONE)
#define setDisallowNone(a)  ((a)->argflags |= ARG_DISALLOW_NONE)


/* Handle name flags. */

#define NAME_IS_USED        0x01    /* It is used in the main module. */
#define NAME_IS_SUBSTR      0x02    /* It is a substring of another. */

#define isUsedName(n)       ((n)->nameflags & NAME_IS_USED)
#define setIsUsedName(n)    ((n)->nameflags |= NAME_IS_USED)
#define resetIsUsedName(n)  ((n)->nameflags &= ~NAME_IS_USED)
#define isSubstring(n)      ((n)->nameflags & NAME_IS_SUBSTR)
#define setIsSubstring(n)   ((n)->nameflags |= NAME_IS_SUBSTR)


/* Handle virtual handler flags. */

#define VH_TRANSFERS        0x01    /* It transfers ownership of the result. */
#define VH_ABORT_ON_EXC     0x02    /* It aborts on an exception. */

#define isTransferVH(vh)    ((vh)->vhflags & VH_TRANSFERS)
#define setIsTransferVH(vh) ((vh)->vhflags |= VH_TRANSFERS)
#define abortOnExceptionVH(vh)  ((vh)->vhflags & VH_ABORT_ON_EXC)
#define setAbortOnExceptionVH(vh)   ((vh)->vhflags |= VH_ABORT_ON_EXC)


/* Handle mapped type flags. */

#define MT_NO_RELEASE       0x01    /* Do not generate a release function. */
#define MT_ALLOW_NONE       0x02    /* The mapped type will handle None. */
#define MT_USER_STATE       0x04    /* The convertors need user state. */
#define MT_NO_ASSIGN_OP     0x08    /* No assignment operator. */
#define MT_NO_COPY_CTOR     0x10    /* No copy ctor. */
#define MT_NO_DEFAULT_CTOR  0x20    /* No default ctor. */

#define noRelease(mt)       ((mt)->mtflags & MT_NO_RELEASE)
#define setNoRelease(mt)    ((mt)->mtflags |= MT_NO_RELEASE)
#define handlesNone(mt)     ((mt)->mtflags & MT_ALLOW_NONE)
#define setHandlesNone(mt)  ((mt)->mtflags |= MT_ALLOW_NONE)
#define needsUserState(mt)  ((mt)->mtflags & MT_USER_STATE)
#define setNeedsUserState(mt)   ((mt)->mtflags |= MT_USER_STATE)
#define noAssignOp(mt)      ((mt)->mtflags & MT_NO_ASSIGN_OP)
#define setNoAssignOp(mt)   ((mt)->mtflags |= MT_NO_ASSIGN_OP)
#define noCopyCtor(mt)      ((mt)->mtflags & MT_NO_COPY_CTOR)
#define setNoCopyCtor(mt)   ((mt)->mtflags |= MT_NO_COPY_CTOR)
#define noDefaultCtor(mt)   ((mt)->mtflags & MT_NO_DEFAULT_CTOR)
#define setNoDefaultCtor(mt)    ((mt)->mtflags |= MT_NO_DEFAULT_CTOR)


/* Handle typedef flags. */

#define TD_NO_TYPE_NAME     0x01    /* Do not use the typedef name. */

#define noTypeName(td)      ((td)->tdflags & TD_NO_TYPE_NAME)
#define setNoTypeName(td)   ((td)->tdflags |= TD_NO_TYPE_NAME)


/* Warning categories. */
typedef enum {
    ParserWarning,
    DeprecationWarning
} Warning;


/* Docstring formatting. */
typedef enum {
    raw,
    deindented
} Format;


/* Docstring signature positioning. */
typedef enum {
    discarded,
    prepended,
    appended
} Signature;


/* Levels of keyword argument support. */
typedef enum {
    NoKwArgs = 0,
    AllKwArgs,
    OptionalKwArgs
} KwArgs;


/* Slot types. */
typedef enum {
    str_slot,
    int_slot,
    float_slot,
    len_slot,
    contains_slot,
    add_slot,
    concat_slot,
    sub_slot,
    mul_slot,
    repeat_slot,
    div_slot,       /* No longer used. */
    mod_slot,
    floordiv_slot,
    truediv_slot,
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
    idiv_slot,      /* No longer used. */
    imod_slot,
    ifloordiv_slot,
    itruediv_slot,
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
    bool_slot,
    neg_slot,
    pos_slot,
    abs_slot,
    repr_slot,
    hash_slot,
    index_slot,
    iter_slot,
    next_slot,
    setattr_slot,
    delattr_slot,       /* This is local to the parser. */
    matmul_slot,
    imatmul_slot,
    await_slot,
    aiter_slot,
    anext_slot,
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
    signal_type_unused,
    slot_type_unused,
    rxcon_type_unused,
    rxdis_type_unused,
    slotcon_type_unused,
    slotdis_type_unused,
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
    qobject_type_unused,
    function_type,
    pytype_type,
    ellipsis_type,
    longlong_type,
    ulonglong_type,
    anyslot_type_unused,
    cbool_type,
    sstring_type,
    wstring_type,
    fake_void_type,
    ssize_type,
    ascii_string_type,
    latin1_string_type,
    utf8_string_type,
    byte_type,
    sbyte_type,
    ubyte_type,
    capsule_type,
    pybuffer_type,
    size_type,
    pyenum_type,
    union_type,
    hash_type
} argType;


/* Value types. */
typedef enum {
    qchar_value,
    string_value,
    numeric_value,
    real_value,
    scoped_value,
    fcall_value,
    empty_value
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


/* Type hint parse status. */
typedef enum {
    needs_parsing,
    being_parsed,
    parsed
} typeHintParseStatus;


/* Type hint node type. */
typedef enum {
    typing_node,
    class_node,
    enum_node,
    other_node
} typeHintNodeType;


/* A location in a .sip source file. */
typedef struct {
    int linenr;                         /* The line number. */
    const char *name;                   /* The filename. */
} sourceLocation;


/* A software license. */
typedef struct {
    const char *type;                   /* The license type. */
    const char *licensee;               /* The licensee. */
    const char *timestamp;              /* The timestamp. */
    const char *sig;                    /* The signature. */
} licenseDef;


/* A version qualifier. */
typedef struct _qualDef {
    const char *name;                   /* The qualifier name. */
    qualType qtype;                     /* The qualifier type. */
    struct _moduleDef *module;          /* The defining module. */
    int line;                           /* Timeline if it is a time. */
    unsigned order;                     /* Order if it is a time. */
    int default_enabled;                /* Enabled by default. */
    struct _qualDef *next;              /* Next in the list. */
} qualDef;


/* A scoped name. */
typedef struct _scopedNameDef {
    char *name;                         /* The name. */
    struct _scopedNameDef *next;        /* Next in the scope list. */
} scopedNameDef;


/* A name. */
typedef struct _nameDef {
    int nameflags;                      /* The name flags. */
    const char *text;                   /* The text of the name. */
    size_t len;                         /* The length of the name. */
    size_t offset;                      /* The offset in the string pool. */
    struct _nameDef *next;              /* Next in the list. */
} nameDef;


/* A literal code block. */
typedef struct _codeBlock {
    char *frag;                         /* The code itself. */
    const char *filename;               /* The original file. */
    int linenr;                         /* The line in the file. */
} codeBlock;


/* A list of literal code blocks. */
typedef struct _codeBlockList {
    codeBlock *block;                   /* The code block. */
    struct _codeBlockList *next;        /* The next in the list. */
} codeBlockList;


/* The arguments to a throw specifier. */
typedef struct _throwArgs {
    int nrArgs;                         /* The number of arguments, -ve if noexcept. */
    struct _exceptionDef *args[MAX_NR_ARGS];    /* The arguments. */
} throwArgs;


/* An exception. */
typedef struct _exceptionDef {
    int exceptionnr;                    /* The exception number. */
    int needed;                         /* The module needs it. */
    struct _ifaceFileDef *iff;          /* The interface file. */
    const char *pyname;                 /* The exception Python name. */
    struct _classDef *cd;               /* The exception class. */
    char *bibase;                       /* The builtin base exception. */
    struct _exceptionDef *base;         /* The defined base exception. */
    codeBlockList *raisecode;           /* Raise exception code. */
    struct _exceptionDef *next;         /* The next in the list. */
} exceptionDef;


/* A value. */
typedef struct _valueDef {
    valueType vtype;                    /* The type. */
    char vunop;                         /* Any unary operator. */
    char vbinop;                        /* Any binary operator. */
    scopedNameDef *cast;                /* Any cast. */
    union {
        char vqchar;                    /* Quoted character value. */
        long vnum;                      /* Numeric value. */
        double vreal;                   /* Real value. */
        char *vstr;                     /* String value. */
        scopedNameDef *vscp;            /* Scoped value. */
        struct _fcallDef *fcd;          /* Function call. */
    } u;
    struct _valueDef *next;             /* Next in the expression. */
} valueDef;


/* A member function argument (or result). */
typedef struct {
    argType atype;                      /* The type. */
    nameDef *name;                      /* The name. */
    struct _typeHintDef *typehint_in;   /* The PEP 484 input type hint. */
    struct _typeHintDef *typehint_out;  /* The PEP 484 output type hint. */
    const char *typehint_value;         /* The type hint value. */
    int argflags;                       /* The argument flags. */
    int nrderefs;                       /* Nr. of dereferences. */
    int derefs[MAX_NR_DEREFS];          /* The const for each dereference. */
    valueDef *defval;                   /* The default value. */
    int scopes_stripped;                /* Nr. of scopes to be stripped. */
    int key;                            /* The optional /KeepReference/ key. */
    struct _typedefDef *original_type;  /* The original type if typedef'd. */
    union {
        struct _signatureDef *sa;       /* If it is a function. */
        struct _templateDef *td;        /* If it is a template. */
        struct _scopedNameDef *snd;     /* If it is a defined type. */
        struct _classDef *cd;           /* If it is a class. */
        struct _enumDef *ed;            /* If it is an enum. */
        struct _scopedNameDef *sname;   /* If it is a struct. */
        struct _mappedTypeDef *mtd;     /* If it is a mapped type. */
        struct _scopedNameDef *cap;     /* If it is a capsule. */
    } u;
} argDef;


/* An entry in a linked argument list. */
typedef struct _argList {
    argDef arg;                         /* The argument itself. */
    struct _argList *next;              /* Next in the list. */
} argList;


/* A function call. */
typedef struct _fcallDef {
    argDef type;                        /* The type. */
    int nrArgs;                         /* The number of arguments. */
    struct _valueDef *args[MAX_NR_ARGS];    /* The arguments. */
} fcallDef;


/* A virtual error handler. */
typedef struct _virtErrorHandler {
    const char *name;                   /* The name of the handler. */
    codeBlockList *code;                /* The handler code. */
    struct _moduleDef *mod;             /* The defining module. */
    int index;                          /* The index within the module. */
    struct _virtErrorHandler *next;     /* The next in the list. */
} virtErrorHandler;


/* A parsed PEP 484 compliant type hint. */
typedef struct _typeHintDef {
    typeHintParseStatus status;         /* The state of the type hint parse. */
    char *raw_hint;                     /* The raw hint. */
    struct _typeHintNodeDef *root;      /* The root of parsed nodes. */
} typeHintDef;


/* A node of a parsed type hint. */
typedef struct _typeHintNodeDef {
    typeHintNodeType type;              /* The type of the node. */
    union {
        const char *name;               /* For typing objects and others. */
        struct _classDef *cd;           /* For class nodes. */
        struct _enumDef *ed;            /* For enum nodes. */
    } u;
    struct _typeHintNodeDef *children;  /* The list of children. */
    struct _typeHintNodeDef *next;      /* The next sibling. */
} typeHintNodeDef;


/* An explicit docstring. */
typedef struct _docstringDef {
    Signature signature;                /* How the signature should be positioned. */
    char *text;                         /* The text of the docstring. */
} docstringDef;


/* A module definition. */
typedef struct _moduleDef {
    nameDef *fullname;                  /* The full module name. */
    const char *name;                   /* The module base name. */
    docstringDef *docstring;            /* The docstring. */
    int modflags;                       /* The module flags. */
    KwArgs kwargs;                      /* The style of keyword argument support. */
    struct _memberDef *othfuncs;        /* List of other functions. */
    struct _overDef *overs;             /* Global overloads. */
    Format defdocstringfmt;             /* The default docstring format. */
    Signature defdocstringsig;          /* The default docstring signature. */
    argType encoding;                   /* The default string encoding. */
    struct _exceptionDef *defexception; /* The default exception. */
    codeBlockList *hdrcode;             /* Header code. */
    codeBlockList *cppcode;             /* Global C++ code. */
    codeBlockList *copying;             /* Software license. */
    codeBlockList *preinitcode;         /* Pre-initialisation code. */
    codeBlockList *initcode;            /* Initialisation code. */
    codeBlockList *postinitcode;        /* Post-initialisation code. */
    codeBlockList *unitcode;            /* Compilation unit code. */
    codeBlockList *unitpostinccode;     /* Compilation unit post-include code. */
    codeBlockList *typehintcode;        /* Type hint code. */
    const char *virt_error_handler;     /* The virtual error handler. */
    int parts;                          /* The number of parts generated. */
    const char *file;                   /* The filename. */
    qualDef *qualifiers;                /* The list of qualifiers. */
    argDef *needed_types;               /* The array of needed types. */
    int nr_needed_types;                /* The number of needed types. */
    int nrtimelines;                    /* The nr. of timelines. */
    int nrexceptions;                   /* The nr. of exceptions. */
    int nrtypedefs;                     /* The nr. of typedefs. */
    int nrvirterrorhandlers;            /* The nr. of virtual error handlers. */
    int next_key;                       /* The next key to allocate. */
    licenseDef *license;                /* The software license. */
    struct _classDef *proxies;          /* The list of proxy classes. */
    struct _moduleDef *container;       /* The container module, if any. */
    struct _ifaceFileList *used;        /* Interface files used. */
    struct _moduleListDef *allimports;  /* The list of all imports. */
    struct _moduleListDef *imports;     /* The list of direct imports. */
    struct _autoPyNameDef *autopyname;  /* The Python naming rules. */
    struct _moduleDef *next;            /* Next in the list. */
} moduleDef;


/* An entry in a linked module list. */
typedef struct _moduleListDef {
    moduleDef *module;                  /* The module itself. */
    struct _moduleListDef *next;        /* The next in the list. */
} moduleListDef;


/* An interface file definition. */
typedef struct _ifaceFileDef {
    nameDef *name;                      /* The name. */
    int needed;                         /* The main module needs it. */
    ifaceFileType type;                 /* Interface file type. */
    int ifacenr;                        /* The index into the types table. */
    scopedNameDef *fqcname;             /* The fully qualified C++ name. */
    moduleDef *module;                  /* The owning module. */
    codeBlockList *hdrcode;             /* Header code. */
    const char *file_extension;         /* The optional file extension. */
    struct _ifaceFileList *used;        /* Interface files used. */
    struct _ifaceFileDef *next;         /* Next in the list. */
} ifaceFileDef;


/* An entry in a linked interface file list. */

typedef struct _ifaceFileList {
    ifaceFileDef *iff;                  /* The interface file itself. */
    struct _ifaceFileList *next;        /* Next in the list. */
} ifaceFileList;


/* A mapped type. */
typedef struct _mappedTypeDef {
    int mtflags;                        /* The mapped type flags. */
    argDef type;                        /* The type being mapped. */
    nameDef *pyname;                    /* The Python name. */
    nameDef *cname;                     /* The C/C++ name. */
    typeHintDef *typehint_in;           /* The PEP 484 input type hint. */
    typeHintDef *typehint_out;          /* The PEP 484 output type hint. */
    const char *typehint_value;         /* The type hint value. */
    int pyqt_flags;                     /* The PyQt specific flags. */
    ifaceFileDef *iff;                  /* The interface file. */
    struct _memberDef *members;         /* The static member functions. */
    struct _overDef *overs;             /* The static overloads. */
    codeBlockList *instancecode;        /* Create instance code. */
    codeBlockList *typecode;            /* Type code. */
    codeBlockList *convfromcode;        /* Convert from C++ code. */
    codeBlockList *convtocode;          /* Convert to C++ code. */
    codeBlockList *releasecode;         /* Release code. */
    struct _mappedTypeDef *real;        /* The original definition. */
    struct _mappedTypeDef *next;        /* Next in the list. */
} mappedTypeDef;


/* A function signature. */
typedef struct _signatureDef {
    argDef result;                      /* The result. */
    int nrArgs;                         /* The number of arguments. */
    argDef args[MAX_NR_ARGS];           /* The arguments. */
} signatureDef;


/* A list of function signatures. */
typedef struct _signatureList {
    struct _signatureDef *sd;           /* The signature. */
    struct _signatureList *next;        /* Next in the list. */
} signatureList;


/* A template type. */
typedef struct _templateDef {
    scopedNameDef *fqname;              /* The name. */
    signatureDef types;                 /* The types. */
} templateDef;


/* A list of virtual handlers. */
typedef struct _virtHandlerDef {
    int virthandlernr;                  /* The nr. of the virtual handler. */
    int vhflags;                        /* The virtual handler flags. */
    signatureDef *pysig;                /* The Python signature. */
    signatureDef *cppsig;               /* The C++ signature. */
    codeBlockList *virtcode;            /* Virtual handler code. */
    virtErrorHandler *veh;              /* The virtual error handler. */
    struct _virtHandlerDef *next;       /* Next in the list. */
} virtHandlerDef;


/* A typedef definition. */
typedef struct _typedefDef {
    int tdflags;                        /* The typedef flags. */
    scopedNameDef *fqname;              /* The fully qualified name. */
    struct _classDef *ecd;              /* The enclosing class. */
    moduleDef *module;                  /* The owning module. */
    argDef type;                        /* The actual type. */
    struct _typedefDef *next;           /* Next in the list. */
} typedefDef;


/* A variable definition. */
typedef struct _varDef {
    scopedNameDef *fqcname;             /* The fully qualified C/C++ name. */
    nameDef *pyname;                    /* The variable Python name. */
    int no_typehint;                    /* The type hint will be suppressed. */
    struct _classDef *ecd;              /* The enclosing class. */
    moduleDef *module;                  /* The owning module. */
    int varflags;                       /* The variable flags. */
    argDef type;                        /* The actual type. */
    codeBlockList *accessfunc;          /* The access function. */
    codeBlockList *getcode;             /* The get code. */
    codeBlockList *setcode;             /* The set code. */
    struct _varDef *next;               /* Next in the list. */
} varDef;


/* A property definition. */
typedef struct _propertyDef {
    nameDef *name;                      /* The property name. */
    docstringDef *docstring;            /* The docstring. */
    const char *get;                    /* The name of the getter method. */
    const char *set;                    /* The name of the setter method. */
    struct _propertyDef *next;          /* Next in the list. */
} propertyDef;


/* An overloaded member function definition. */
typedef struct _overDef {
    sourceLocation sloc;                /* The source location. */
    const char *cppname;                /* The C++ name. */
    docstringDef *docstring;            /* The docstring. */
    int overflags;                      /* The overload flags. */
    int no_typehint;                    /* The type hint will be suppressed. */
    int pyqt_signal_hack;               /* The PyQt signal hack. */
    KwArgs kwargs;                      /* The keyword argument support. */
    struct _memberDef *common;          /* Common parts. */
    signatureDef pysig;                 /* The Python signature. */
    signatureDef *cppsig;               /* The C++ signature. */
    throwArgs *exceptions;              /* The exceptions. */
    codeBlockList *methodcode;          /* Method code. */
    codeBlockList *premethodcode;       /* Code to insert before the method code. */
    codeBlockList *virtcallcode;        /* Virtual call code. */
    codeBlockList *virtcode;            /* Virtual handler code. */
    char *prehook;                      /* The pre-hook name. */
    char *posthook;                     /* The post-hook name. */
    const char *virt_error_handler;     /* The virtual error handler. */
    struct _overDef *next;              /* Next in the list. */
} overDef;


/* An overloaded constructor definition. */
typedef struct _ctorDef {
    docstringDef *docstring;            /* The docstring. */
    int ctorflags;                      /* The ctor flags. */
    int no_typehint;                    /* The type hint will be suppressed. */
    KwArgs kwargs;                      /* The keyword argument support. */
    signatureDef pysig;                 /* The Python signature. */
    signatureDef *cppsig;               /* The C++ signature, NULL if /NoDerived/. */
    throwArgs *exceptions;              /* The exceptions. */
    codeBlockList *methodcode;          /* Method code. */
    codeBlockList *premethodcode;       /* Code to insert before the method code. */
    char *prehook;                      /* The pre-hook name. */
    char *posthook;                     /* The post-hook name. */
    struct _ctorDef *next;              /* Next in the list. */
} ctorDef;


/* An enumerated type member definition. */
typedef struct _enumMemberDef {
    nameDef *pyname;                    /* The Python name. */
    int no_typehint;                    /* The type hint will be suppressed. */
    char *cname;                        /* The C/C++ name. */
    struct _enumDef *ed;                /* The enclosing enum. */
    struct _enumMemberDef *next;        /* Next in the list. */
} enumMemberDef;


/* An enumerated type definition. */
typedef struct _enumDef {
    int enumflags;                      /* The enum flags. */
    scopedNameDef *fqcname;             /* The C/C++ name (may be NULL). */
    nameDef *cname;                     /* The C/C++ name (may be NULL). */
    nameDef *pyname;                    /* The Python name (may be NULL). */
    int no_typehint;                    /* The type hint will be suppressed. */
    int enumnr;                         /* The enum number. */
    int enum_idx;                       /* The enum index within the module. */
    struct _classDef *ecd;              /* The enclosing class, if any. */
    struct _mappedTypeDef *emtd;        /* The enclosing mapped type, if any. */
    moduleDef *module;                  /* The owning module. */
    enumMemberDef *members;             /* The list of members. */
    struct _memberDef *slots;           /* The list of slots. */
    struct _overDef *overs;             /* The list of slot overloads. */
    struct _enumDef *next;              /* Next in the list. */
} enumDef;


/* An member function definition. */
typedef struct _memberDef {
    nameDef *pyname;                    /* The Python name. */
    int memberflags;                    /* The member flags. */
    int membernr;                       /* The index in the method table. */
    slotType slot;                      /* The slot type. */
    moduleDef *module;                  /* The owning module. */
    struct _ifaceFileDef *ns_scope;     /* The scope if it has been moved. */
    struct _memberDef *next;            /* Next in the list. */
} memberDef;


/* A list of visible member functions. */
typedef struct _visibleList {
    memberDef *m;                       /* The member definition. */
    struct _classDef *cd;               /* The class. */
    struct _visibleList *next;          /* Next in the list. */
} visibleList;


/* An entry in a linked class list. */
typedef struct _classList {
    struct _classDef *cd;               /* The class itself. */
    struct _classList *next;            /* Next in the list. */
} classList;


/* A virtual overload definition. */
typedef struct _virtOverDef {
    overDef *od;                        /* The overload. */
    virtHandlerDef *virthandler;        /* The virtual handler. */
    struct _virtOverDef *next;          /* Next in the list. */
} virtOverDef;


/* A class that appears in a class's hierarchy. */
typedef struct _mroDef {
    struct _classDef *cd;               /* The class. */
    struct _mroDef *next;               /* The next in the list. */
} mroDef;


/* A class definition. */
typedef struct _classDef {
    docstringDef *docstring;            /* The class docstring. */
    unsigned classflags;                /* The class flags. */
    unsigned classflags2;               /* The class flags, part 2. */
    int pyqt_flags;                     /* The PyQt specific flags. */
    struct _stringList *pyqt_flags_enums;   /* The names of the flagged enum. */
    const char *pyqt_interface;         /* The Qt interface name. */
    nameDef *pyname;                    /* The Python name. */
    int no_typehint;                    /* The type hint will be suppressed. */
    ifaceFileDef *iff;                  /* The interface file. */
    struct _classDef *ecd;              /* The enclosing scope. */
    struct _classDef *real;             /* The real class if this is a proxy or extender. */
    classList *supers;                  /* The parent classes. */
    mroDef *mro;                        /* The super-class hierarchy. */
    nameDef *metatype;                  /* The meta-type. */
    nameDef *supertype;                 /* The super-type. */
    templateDef *td;                    /* The instantiated template. */
    ctorDef *ctors;                     /* The constructors. */
    ctorDef *defctor;                   /* The default ctor. */
    codeBlockList *dealloccode;         /* Handwritten dealloc code. */
    codeBlockList *dtorcode;            /* Handwritten dtor code. */
    throwArgs *dtorexceptions;          /* The dtor exceptions. */
    memberDef *members;                 /* The member functions. */
    overDef *overs;                     /* The overloads. */
    argList *casts;                     /* The operator casts. */
    virtOverDef *vmembers;              /* The virtual members. */
    visibleList *visible;               /* The visible members. */
    codeBlockList *cppcode;             /* Class C++ code. */
    codeBlockList *convtosubcode;       /* Convert to sub C++ code. */
    struct _classDef *subbase;          /* Sub-class base class. */
    codeBlockList *instancecode;        /* Create instance code. */
    codeBlockList *convtocode;          /* Convert to C++ code. */
    codeBlockList *convfromcode;        /* Convert from C++ code. */
    codeBlockList *travcode;            /* Traverse code. */
    codeBlockList *clearcode;           /* Clear code. */
    codeBlockList *getbufcode;          /* Get buffer code. */
    codeBlockList *releasebufcode;      /* Release buffer code. */
    codeBlockList *picklecode;          /* Pickle code. */
    codeBlockList *finalcode;           /* Finalisation code. */
    codeBlockList *typehintcode;        /* Type hint code. */
    propertyDef *properties;            /* The properties. */
    const char *virt_error_handler;     /* The virtual error handler. */
    typeHintDef *typehint_in;           /* The PEP 484 input type hint. */
    typeHintDef *typehint_out;          /* The PEP 484 output type hint. */
    const char *typehint_value;         /* The type hint value. */
    struct _classDef *next;             /* Next in the list. */
} classDef;


/* A rule for automatic Python naming. */
typedef struct _autoPyNameDef {
    const char *remove_leading;         /* Leading string to remove. */
    struct _autoPyNameDef *next;        /* The next in the list. */
} autoPyNameDef;


/* The parse tree corresponding to the specification file. */
typedef struct {
    moduleDef *module;                  /* The module being generated. */
    nameDef *namecache;                 /* The name cache. */
    ifaceFileDef *ifacefiles;           /* The list of interface files. */
    classDef *classes;                  /* The list of classes. */
    exceptionDef *exceptions;           /* The list of exceptions. */
    mappedTypeDef *mappedtypes;         /* The mapped types. */
    enumDef *enums;                     /* List of enums. */
    varDef *vars;                       /* List of variables. */
    typedefDef *typedefs;               /* List of typedefs. */
    int nrvirthandlers;                 /* The number of virtual handlers. */
    virtHandlerDef *virthandlers;       /* The virtual handlers. */
    virtErrorHandler *errorhandlers;    /* The list of virtual error handlers. */
    codeBlockList *exphdrcode;          /* Exported header code. */
    codeBlockList *exptypehintcode;     /* Exported type hint code. */
    classDef *qobject_cd;               /* QObject class, NULL if none. */
    int genc;                           /* Set if we are generating C code. */
    struct _stringList *plugins;        /* The list of plugins. */
    struct _extractDef *extracts;       /* The list of extracts. */
} sipSpec;


/* A list of strings. */
typedef struct _stringList {
    const char *s;                      /* The string. */
    struct _stringList *next;           /* The next in the list. */
} stringList;


extern unsigned sipVersion;             /* The version of SIP. */
extern const char *sipVersionStr;       /* The version of SIP as a string. */
extern char *sipName;                   /* The name of the sip module. */
extern unsigned abiVersion;             /* The encoded ABI version number. */


void get_bindings_configuration(const char *sip_file, stringList **tags,
        stringList **disabled);
stringList *generateCode(sipSpec *, char *, const char *, int, int, int, int,
        stringList *needed_qualifiers, stringList *, int, int,
        const char **api_header);
void generateExpression(valueDef *vd, int in_str, FILE *fp);
int error(const char *fmt, ...);
void errorAppend(const char *fmt, ...);
void *sipMalloc(size_t n);
void *sipCalloc(size_t nr, size_t n);
char *sipStrdup(const char *);
char *concat(const char *, ...);
void append(char **, const char *);
int compareScopedNames(scopedNameDef *snd1, scopedNameDef *snd2);
char *scopedNameTail(scopedNameDef *);
void prcode(FILE *fp, const char *fmt, ...);
void prCopying(FILE *fp, moduleDef *mod, const char *comment);
void prDefaultValue(argDef *ad, FILE *fp);
void prScopedPythonName(FILE *fp, classDef *scope, const char *pyname);
int isNumberSlot(memberDef *md);
void appendString(stringList **headp, const char *s);
int pluginPyQt5(sipSpec *pt);
int pluginPyQt6(sipSpec *pt);
memberDef *findMethod(classDef *cd, const char *name);
void pyiCtor(sipSpec *pt, moduleDef *mod, classDef *cd, ctorDef *ct, FILE *fp);
void pyiOverload(sipSpec *pt, moduleDef *mod, overDef *od, int is_method,
        FILE *fp);
scopedNameDef *removeGlobalScope(scopedNameDef *snd);
void pyiTypeHint(sipSpec *pt, typeHintDef *thd, moduleDef *mod, int out,
        FILE *fp);
void generateBaseType(ifaceFileDef *scope, argDef *ad, int use_typename,
        int strip, FILE *fp);
void normaliseArgs(signatureDef *sd);
void restoreArgs(signatureDef *sd);

#endif
