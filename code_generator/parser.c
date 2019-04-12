/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_API = 258,
     TK_AUTOPYNAME = 259,
     TK_DEFDOCSTRFMT = 260,
     TK_DEFDOCSTRSIG = 261,
     TK_DEFENCODING = 262,
     TK_PLUGIN = 263,
     TK_VIRTERRORHANDLER = 264,
     TK_EXPTYPEHINTCODE = 265,
     TK_TYPEHINTCODE = 266,
     TK_DOCSTRING = 267,
     TK_EXTRACT = 268,
     TK_ACCESSCODE = 269,
     TK_GETCODE = 270,
     TK_SETCODE = 271,
     TK_PREINITCODE = 272,
     TK_INITCODE = 273,
     TK_POSTINITCODE = 274,
     TK_FINALCODE = 275,
     TK_UNITCODE = 276,
     TK_UNITPOSTINCLUDECODE = 277,
     TK_MODCODE = 278,
     TK_TYPECODE = 279,
     TK_COPYING = 280,
     TK_MAPPEDTYPE = 281,
     TK_CODELINE = 282,
     TK_IF = 283,
     TK_END = 284,
     TK_NAME_VALUE = 285,
     TK_PATH_VALUE = 286,
     TK_STRING_VALUE = 287,
     TK_VIRTUALCATCHERCODE = 288,
     TK_TRAVERSECODE = 289,
     TK_CLEARCODE = 290,
     TK_GETBUFFERCODE = 291,
     TK_RELEASEBUFFERCODE = 292,
     TK_READBUFFERCODE = 293,
     TK_WRITEBUFFERCODE = 294,
     TK_SEGCOUNTCODE = 295,
     TK_CHARBUFFERCODE = 296,
     TK_PICKLECODE = 297,
     TK_VIRTUALCALLCODE = 298,
     TK_METHODCODE = 299,
     TK_PREMETHODCODE = 300,
     TK_INSTANCECODE = 301,
     TK_FROMTYPE = 302,
     TK_TOTYPE = 303,
     TK_TOSUBCLASS = 304,
     TK_INCLUDE = 305,
     TK_IMPORT = 306,
     TK_EXPHEADERCODE = 307,
     TK_MODHEADERCODE = 308,
     TK_TYPEHEADERCODE = 309,
     TK_MODULE = 310,
     TK_COMPOMODULE = 311,
     TK_CLASS = 312,
     TK_STRUCT = 313,
     TK_PUBLIC = 314,
     TK_PROTECTED = 315,
     TK_PRIVATE = 316,
     TK_SIGNALS = 317,
     TK_SIGNAL_METHOD = 318,
     TK_SLOTS = 319,
     TK_SLOT_METHOD = 320,
     TK_BOOL = 321,
     TK_SHORT = 322,
     TK_INT = 323,
     TK_LONG = 324,
     TK_FLOAT = 325,
     TK_DOUBLE = 326,
     TK_CHAR = 327,
     TK_WCHAR_T = 328,
     TK_VOID = 329,
     TK_PYOBJECT = 330,
     TK_PYTUPLE = 331,
     TK_PYLIST = 332,
     TK_PYDICT = 333,
     TK_PYCALLABLE = 334,
     TK_PYSLICE = 335,
     TK_PYTYPE = 336,
     TK_PYBUFFER = 337,
     TK_VIRTUAL = 338,
     TK_ENUM = 339,
     TK_SIGNED = 340,
     TK_UNSIGNED = 341,
     TK_SCOPE = 342,
     TK_LOGICAL_OR = 343,
     TK_CONST = 344,
     TK_STATIC = 345,
     TK_SIPSIGNAL = 346,
     TK_SIPSLOT = 347,
     TK_SIPRXCON = 348,
     TK_PYSSIZET = 349,
     TK_SIZET = 350,
     TK_NUMBER_VALUE = 351,
     TK_REAL_VALUE = 352,
     TK_TYPEDEF = 353,
     TK_NAMESPACE = 354,
     TK_TIMELINE = 355,
     TK_PLATFORMS = 356,
     TK_FEATURE = 357,
     TK_LICENSE = 358,
     TK_QCHAR_VALUE = 359,
     TK_TRUE_VALUE = 360,
     TK_FALSE_VALUE = 361,
     TK_NULL_VALUE = 362,
     TK_OPERATOR = 363,
     TK_THROW = 364,
     TK_EXCEPTION = 365,
     TK_RAISECODE = 366,
     TK_EXPLICIT = 367,
     TK_TEMPLATE = 368,
     TK_FINAL = 369,
     TK_ELLIPSIS = 370,
     TK_DEFMETATYPE = 371,
     TK_DEFSUPERTYPE = 372,
     TK_PROPERTY = 373,
     TK_HIDE_NS = 374,
     TK_FORMAT = 375,
     TK_GET = 376,
     TK_ID = 377,
     TK_KWARGS = 378,
     TK_LANGUAGE = 379,
     TK_LICENSEE = 380,
     TK_NAME = 381,
     TK_OPTIONAL = 382,
     TK_ORDER = 383,
     TK_REMOVELEADING = 384,
     TK_SET = 385,
     TK_SIGNATURE = 386,
     TK_TIMESTAMP = 387,
     TK_TYPE = 388,
     TK_USEARGNAMES = 389,
     TK_USELIMITEDAPI = 390,
     TK_ALLRAISEPYEXC = 391,
     TK_CALLSUPERINIT = 392,
     TK_DEFERRORHANDLER = 393,
     TK_VERSION = 394
   };
#endif
/* Tokens.  */
#define TK_API 258
#define TK_AUTOPYNAME 259
#define TK_DEFDOCSTRFMT 260
#define TK_DEFDOCSTRSIG 261
#define TK_DEFENCODING 262
#define TK_PLUGIN 263
#define TK_VIRTERRORHANDLER 264
#define TK_EXPTYPEHINTCODE 265
#define TK_TYPEHINTCODE 266
#define TK_DOCSTRING 267
#define TK_EXTRACT 268
#define TK_ACCESSCODE 269
#define TK_GETCODE 270
#define TK_SETCODE 271
#define TK_PREINITCODE 272
#define TK_INITCODE 273
#define TK_POSTINITCODE 274
#define TK_FINALCODE 275
#define TK_UNITCODE 276
#define TK_UNITPOSTINCLUDECODE 277
#define TK_MODCODE 278
#define TK_TYPECODE 279
#define TK_COPYING 280
#define TK_MAPPEDTYPE 281
#define TK_CODELINE 282
#define TK_IF 283
#define TK_END 284
#define TK_NAME_VALUE 285
#define TK_PATH_VALUE 286
#define TK_STRING_VALUE 287
#define TK_VIRTUALCATCHERCODE 288
#define TK_TRAVERSECODE 289
#define TK_CLEARCODE 290
#define TK_GETBUFFERCODE 291
#define TK_RELEASEBUFFERCODE 292
#define TK_READBUFFERCODE 293
#define TK_WRITEBUFFERCODE 294
#define TK_SEGCOUNTCODE 295
#define TK_CHARBUFFERCODE 296
#define TK_PICKLECODE 297
#define TK_VIRTUALCALLCODE 298
#define TK_METHODCODE 299
#define TK_PREMETHODCODE 300
#define TK_INSTANCECODE 301
#define TK_FROMTYPE 302
#define TK_TOTYPE 303
#define TK_TOSUBCLASS 304
#define TK_INCLUDE 305
#define TK_IMPORT 306
#define TK_EXPHEADERCODE 307
#define TK_MODHEADERCODE 308
#define TK_TYPEHEADERCODE 309
#define TK_MODULE 310
#define TK_COMPOMODULE 311
#define TK_CLASS 312
#define TK_STRUCT 313
#define TK_PUBLIC 314
#define TK_PROTECTED 315
#define TK_PRIVATE 316
#define TK_SIGNALS 317
#define TK_SIGNAL_METHOD 318
#define TK_SLOTS 319
#define TK_SLOT_METHOD 320
#define TK_BOOL 321
#define TK_SHORT 322
#define TK_INT 323
#define TK_LONG 324
#define TK_FLOAT 325
#define TK_DOUBLE 326
#define TK_CHAR 327
#define TK_WCHAR_T 328
#define TK_VOID 329
#define TK_PYOBJECT 330
#define TK_PYTUPLE 331
#define TK_PYLIST 332
#define TK_PYDICT 333
#define TK_PYCALLABLE 334
#define TK_PYSLICE 335
#define TK_PYTYPE 336
#define TK_PYBUFFER 337
#define TK_VIRTUAL 338
#define TK_ENUM 339
#define TK_SIGNED 340
#define TK_UNSIGNED 341
#define TK_SCOPE 342
#define TK_LOGICAL_OR 343
#define TK_CONST 344
#define TK_STATIC 345
#define TK_SIPSIGNAL 346
#define TK_SIPSLOT 347
#define TK_SIPRXCON 348
#define TK_PYSSIZET 349
#define TK_SIZET 350
#define TK_NUMBER_VALUE 351
#define TK_REAL_VALUE 352
#define TK_TYPEDEF 353
#define TK_NAMESPACE 354
#define TK_TIMELINE 355
#define TK_PLATFORMS 356
#define TK_FEATURE 357
#define TK_LICENSE 358
#define TK_QCHAR_VALUE 359
#define TK_TRUE_VALUE 360
#define TK_FALSE_VALUE 361
#define TK_NULL_VALUE 362
#define TK_OPERATOR 363
#define TK_THROW 364
#define TK_EXCEPTION 365
#define TK_RAISECODE 366
#define TK_EXPLICIT 367
#define TK_TEMPLATE 368
#define TK_FINAL 369
#define TK_ELLIPSIS 370
#define TK_DEFMETATYPE 371
#define TK_DEFSUPERTYPE 372
#define TK_PROPERTY 373
#define TK_HIDE_NS 374
#define TK_FORMAT 375
#define TK_GET 376
#define TK_ID 377
#define TK_KWARGS 378
#define TK_LANGUAGE 379
#define TK_LICENSEE 380
#define TK_NAME 381
#define TK_OPTIONAL 382
#define TK_ORDER 383
#define TK_REMOVELEADING 384
#define TK_SET 385
#define TK_SIGNATURE 386
#define TK_TIMESTAMP 387
#define TK_TYPE 388
#define TK_USEARGNAMES 389
#define TK_USELIMITEDAPI 390
#define TK_ALLRAISEPYEXC 391
#define TK_CALLSUPERINIT 392
#define TK_DEFERRORHANDLER 393
#define TK_VERSION 394




/* Copy the first part of user declarations.  */
#line 19 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sip.h"


#define MAX_NESTED_IF       10
#define MAX_NESTED_SCOPE    10

#define inMainModule()      (currentSpec->module == currentModule || currentModule->container != NULL)


static sipSpec *currentSpec;            /* The current spec being parsed. */
static int strictParse;                 /* Set if the platform is enforced. */
static stringList *backstops;           /* The list of backstops. */
static stringList *neededQualifiers;    /* The list of required qualifiers. */
static stringList *excludedQualifiers;  /* The list of excluded qualifiers. */
static moduleDef *currentModule;        /* The current module being parsed. */
static mappedTypeDef *currentMappedType;    /* The current mapped type. */
static enumDef *currentEnum;            /* The current enum being parsed. */
static int sectionFlags;                /* The current section flags. */
static int currentIsVirt;               /* Set if the callable is virtual. */
static int currentCtorIsExplicit;       /* Set if the ctor is explicit. */
static int currentIsStatic;             /* Set if the current is static. */
static int currentIsSignal;             /* Set if the current is Q_SIGNAL. */
static int currentIsSlot;               /* Set if the current is Q_SLOT. */
static int currentIsTemplate;           /* Set if the current is a template. */
static char *previousFile;              /* The file just parsed. */
static parserContext currentContext;    /* The current context. */
static int stackPtr;                    /* The stack pointer. */
static int skipStack[MAX_NESTED_IF];    /* Stack of skip flags. */
static classDef *scopeStack[MAX_NESTED_SCOPE];  /* The scope stack. */
static int sectFlagsStack[MAX_NESTED_SCOPE];    /* The section flags stack. */
static int currentScopeIdx;             /* The scope stack index. */
static unsigned currentTimelineOrder;   /* The current timeline order. */
static classList *currentSupers;        /* The current super-class list. */
static platformDef *currentPlatforms;   /* The current platforms list. */
static platformDef *platformStack[MAX_NESTED_IF];   /* Stack of platforms. */
static KwArgs defaultKwArgs;            /* The default keyword arguments support. */
static int makeProtPublic;              /* Treat protected items as public. */
static int parsingCSignature;           /* An explicit C/C++ signature is being parsed. */


static const char *getPythonName(moduleDef *mod, optFlags *optflgs,
        const char *cname);
static classDef *findClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname, int tmpl_arg);
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff,
        int tmpl_arg);
static classDef *newClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *snd,
        const char *virt_error_handler, typeHintDef *typehint_in,
        typeHintDef *typehint_out, const char *typehint_value);
static void finishClass(sipSpec *, moduleDef *, classDef *, optFlags *);
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new);
static mappedTypeDef *newMappedType(sipSpec *,argDef *, optFlags *);
static enumDef *newEnum(sipSpec *pt, moduleDef *mod, mappedTypeDef *mt_scope,
        char *name, optFlags *of, int flags, int isscoped);
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod,
        classDef *scope, scopedNameDef *fqname, classTmplDef *tcd,
        templateDef *td, const char *pyname, int use_template_name,
        docstringDef *docstring);
static void newTypedef(sipSpec *, moduleDef *, char *, argDef *, optFlags *,
        docstringDef *);
static void newVar(sipSpec *pt, moduleDef *mod, char *name, int isstatic,
        argDef *type, optFlags *of, codeBlock *acode, codeBlock *gcode,
        codeBlock *scode, int section);
static void newCtor(moduleDef *, char *, int, signatureDef *, optFlags *,
                    codeBlock *, throwArgs *, signatureDef *, int,
                    docstringDef *, codeBlock *);
static void newFunction(sipSpec *, moduleDef *, classDef *, ifaceFileDef *,
        mappedTypeDef *, int, int, int, int, int, char *, signatureDef *, int,
        int, optFlags *, codeBlock *, codeBlock *, codeBlock *, throwArgs *,
        signatureDef *, docstringDef *, int, codeBlock *);
static optFlag *findOptFlag(optFlags *flgs, const char *name);
static optFlag *getOptFlag(optFlags *flgs, const char *name, flagType ft);
static memberDef *findFunction(sipSpec *, moduleDef *, classDef *,
        ifaceFileDef *, mappedTypeDef *, const char *, int, int, int);
static void checkAttributes(sipSpec *, moduleDef *, classDef *,
        mappedTypeDef *, const char *, int);
static void newModule(FILE *fp, const char *filename);
static moduleDef *allocModule();
static void parseFile(FILE *fp, const char *name, moduleDef *prevmod,
        int optional);
static void handleEOF(void);
static void handleEOM(void);
static qualDef *findQualifier(const char *name);
static const char *getInt(const char *cp, int *ip);
static scopedNameDef *text2scopedName(ifaceFileDef *scope, char *text);
static scopedNameDef *scopeScopedName(ifaceFileDef *scope,
        scopedNameDef *name);
static void pushScope(classDef *);
static void popScope(void);
static classDef *currentScope(void);
static void newQualifier(moduleDef *, int, unsigned, int, const char *,
        qualType);
static qualDef *allocQualifier(moduleDef *, int, unsigned, int, const char *,
        qualType);
static void newImport(const char *filename);
static int timePeriod(const char *lname, const char *uname);
static int platOrFeature(char *name, int optnot);
static void addPlatform(qualDef *qd);
static int notSkipping(void);
static void getHooks(optFlags *,char **,char **);
static int getTransfer(optFlags *optflgs);
static int getReleaseGIL(optFlags *optflgs);
static int getHoldGIL(optFlags *optflgs);
static int getDeprecated(optFlags *optflgs);
static int getAllowNone(optFlags *optflgs);
static int getDisallowNone(optFlags *optflgs);
static const char *getVirtErrorHandler(optFlags *optflgs);
static const char *getDocType(optFlags *optflgs);
static const char *getTypeHintValue(optFlags *optflgs);
static void getTypeHints(optFlags *optflgs, typeHintDef **in,
        typeHintDef **out);
static int getNoTypeHint(optFlags *optflgs);
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd,
        templateDef *td, classDef *ncd, scopedNameDef *type_names,
        scopedNameDef *type_values);
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td,
        classDef *ncd, scopedNameDef *type_names, scopedNameDef *type_values);
static int search_back(const char *end, const char *start, const char *target);
static char *type2string(argDef *ad);
static char *scopedNameToString(scopedNameDef *name);
static void addUsedFromCode(sipSpec *pt, ifaceFileList **used, const char *sname);
static int sameName(scopedNameDef *snd, const char *sname);
static int stringFind(stringList *sl, const char *s);
static void setModuleName(sipSpec *pt, moduleDef *mod, const char *fullname);
static int foundInScope(scopedNameDef *fq_name, scopedNameDef *rel_name);
static void defineClass(scopedNameDef *snd, classList *supers, optFlags *of);
static classDef *completeClass(scopedNameDef *snd, optFlags *of, int has_def);
static memberDef *instantiateTemplateMethods(memberDef *tmd, moduleDef *mod);
static void instantiateTemplateEnums(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static void instantiateTemplateVars(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static void instantiateTemplateTypedefs(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, scopedNameDef *type_names,
        scopedNameDef *type_values);
static overDef *instantiateTemplateOverloads(sipSpec *pt, overDef *tod,
        memberDef *tmethods, memberDef *methods, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static void resolveAnyTypedef(sipSpec *pt, argDef *ad);
static void addTypedef(sipSpec *pt, typedefDef *tdd);
static void addVariable(sipSpec *pt, varDef *vd);
static void applyTypeFlags(moduleDef *mod, argDef *ad, optFlags *flags);
static Format convertFormat(const char *format);
static Signature convertSignature(const char *signature);
static argType convertEncoding(const char *encoding);
static apiVersionRangeDef *getAPIRange(optFlags *optflgs);
static apiVersionRangeDef *convertAPIRange(moduleDef *mod, nameDef *name,
        int from, int to);
static char *convertFeaturedString(char *fs);
static KwArgs keywordArgs(moduleDef *mod, optFlags *optflgs, signatureDef *sd,
        int need_name);
static char *strip(char *s);
static int isEnabledFeature(const char *name);
static void addProperty(sipSpec *pt, moduleDef *mod, classDef *cd,
        const char *name, const char *get, const char *set,
        docstringDef *docstring);
static moduleDef *configureModule(sipSpec *pt, moduleDef *module,
        const char *filename, const char *name, int c_module, KwArgs kwargs,
        int use_arg_names, int use_limited_api, int call_super_init,
        int all_raise_py_exc, const char *def_error_handler,
        docstringDef *docstring);
static void addAutoPyName(moduleDef *mod, const char *remove_leading);
static KwArgs convertKwArgs(const char *kwargs);
static void checkAnnos(optFlags *annos, const char *valid[]);
static void checkNoAnnos(optFlags *annos, const char *msg);
static void appendCodeBlock(codeBlockList **headp, codeBlock *cb);
static void handleKeepReference(optFlags *optflgs, argDef *ad, moduleDef *mod);
static void mappedTypeAnnos(mappedTypeDef *mtd, optFlags *optflgs);
static void add_new_deref(argDef *new, argDef *orig, int isconst);
static void add_derefs(argDef *dst, argDef *src);
static int isBackstop(qualDef *qd);
static void checkEllipsis(signatureDef *sd);
static scopedNameDef *fullyQualifiedName(scopedNameDef *snd);


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 203 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
{
    char            qchar;
    char            *text;
    long            number;
    double          real;
    argDef          memArg;
    signatureDef    signature;
    signatureDef    *optsignature;
    throwArgs       *throwlist;
    codeBlock       *codeb;
    docstringDef    *docstr;
    valueDef        value;
    valueDef        *valp;
    optFlags        optflags;
    optFlag         flag;
    scopedNameDef   *scpvalp;
    fcallDef        fcall;
    int             boolean;
    exceptionDef    exceptionbase;
    classDef        *klass;
    apiCfg          api;
    autoPyNameCfg   autopyname;
    compModuleCfg   compmodule;
    defDocstringFmtCfg  defdocstringfmt;
    defDocstringSigCfg  defdocstringsig;
    defEncodingCfg  defencoding;
    defMetatypeCfg  defmetatype;
    defSupertypeCfg defsupertype;
    hiddenNsCfg     hiddenns;
    exceptionCfg    exception;
    docstringCfg    docstring;
    extractCfg      extract;
    featureCfg      feature;
    licenseCfg      license;
    importCfg       import;
    includeCfg      include;
    moduleCfg       module;
    pluginCfg       plugin;
    propertyCfg     property;
    variableCfg     variable;
    vehCfg          veh;
    int             token;
}
/* Line 193 of yacc.c.  */
#line 602 "/Users/phil/hg/sip/code_generator/parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 615 "/Users/phil/hg/sip/code_generator/parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1471

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  162
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  241
/* YYNRULES -- Number of rules.  */
#define YYNRULES  562
/* YYNRULES -- Number of states.  */
#define YYNSTATES  973

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   394

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   147,     2,     2,     2,   160,   152,     2,
     140,   141,   150,   149,   142,   148,     2,   151,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   157,   146,
     155,   143,   156,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   158,     2,   159,   161,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   144,   153,   145,   154,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    84,    86,    88,    90,    92,    94,    96,    98,
     100,   103,   105,   109,   111,   115,   119,   122,   124,   128,
     130,   134,   138,   141,   143,   147,   149,   153,   157,   160,
     162,   166,   168,   172,   176,   180,   182,   186,   188,   192,
     196,   201,   203,   207,   211,   215,   221,   222,   226,   231,
     233,   236,   238,   240,   242,   244,   247,   248,   254,   255,
     262,   267,   269,   272,   274,   276,   278,   280,   283,   286,
     288,   290,   292,   307,   308,   314,   315,   319,   321,   324,
     325,   331,   333,   336,   338,   341,   343,   347,   349,   353,
     357,   358,   364,   366,   369,   371,   372,   378,   380,   383,
     387,   392,   394,   398,   400,   403,   404,   406,   410,   412,
     416,   420,   424,   428,   432,   435,   437,   441,   443,   447,
     451,   454,   456,   460,   462,   466,   470,   473,   475,   479,
     481,   485,   489,   493,   495,   499,   501,   505,   509,   510,
     515,   517,   520,   522,   524,   526,   530,   531,   534,   538,
     540,   544,   548,   552,   556,   560,   564,   568,   572,   576,
     577,   582,   584,   587,   589,   591,   593,   595,   597,   599,
     600,   602,   605,   607,   611,   613,   617,   621,   625,   628,
     630,   634,   636,   640,   644,   645,   648,   649,   652,   653,
     656,   659,   662,   665,   668,   671,   674,   677,   680,   683,
     686,   689,   692,   695,   698,   701,   704,   707,   710,   713,
     716,   719,   722,   725,   728,   731,   734,   738,   740,   744,
     748,   752,   753,   755,   759,   761,   765,   769,   773,   774,
     776,   780,   782,   786,   788,   792,   796,   800,   803,   805,
     808,   809,   819,   820,   822,   824,   825,   827,   828,   830,
     832,   835,   837,   839,   844,   845,   847,   848,   851,   852,
     855,   857,   861,   863,   865,   867,   869,   871,   873,   874,
     876,   878,   880,   882,   884,   886,   890,   891,   895,   898,
     900,   902,   906,   908,   910,   912,   914,   919,   922,   924,
     926,   928,   930,   932,   934,   935,   937,   941,   948,   961,
     962,   963,   972,   973,   977,   982,   983,   984,   993,   994,
     997,   999,  1003,  1006,  1007,  1009,  1011,  1013,  1014,  1018,
    1019,  1021,  1024,  1026,  1028,  1030,  1032,  1034,  1036,  1038,
    1040,  1042,  1044,  1046,  1048,  1050,  1052,  1054,  1056,  1058,
    1060,  1062,  1064,  1066,  1068,  1070,  1072,  1074,  1076,  1078,
    1080,  1083,  1086,  1089,  1093,  1097,  1101,  1104,  1108,  1112,
    1114,  1118,  1122,  1126,  1130,  1131,  1136,  1138,  1141,  1143,
    1145,  1147,  1149,  1151,  1152,  1154,  1155,  1159,  1161,  1173,
    1174,  1178,  1180,  1192,  1193,  1194,  1201,  1202,  1203,  1211,
    1229,  1237,  1255,  1272,  1274,  1276,  1278,  1280,  1282,  1284,
    1286,  1288,  1291,  1294,  1297,  1300,  1303,  1306,  1309,  1312,
    1315,  1318,  1322,  1326,  1328,  1331,  1334,  1336,  1339,  1342,
    1345,  1347,  1350,  1351,  1353,  1354,  1356,  1357,  1360,  1361,
    1365,  1367,  1371,  1373,  1377,  1379,  1385,  1387,  1389,  1390,
    1393,  1394,  1397,  1398,  1401,  1402,  1405,  1407,  1408,  1410,
    1414,  1419,  1424,  1428,  1431,  1432,  1436,  1437,  1441,  1443,
    1444,  1448,  1450,  1452,  1454,  1455,  1459,  1461,  1470,  1471,
    1475,  1477,  1480,  1482,  1484,  1487,  1490,  1493,  1498,  1502,
    1506,  1507,  1509,  1510,  1514,  1517,  1519,  1524,  1527,  1530,
    1532,  1534,  1537,  1539,  1541,  1544,  1547,  1551,  1553,  1555,
    1557,  1560,  1563,  1565,  1567,  1569,  1571,  1573,  1575,  1577,
    1579,  1581,  1583,  1585,  1587,  1589,  1591,  1593,  1597,  1598,
    1603,  1604,  1606
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     163,     0,    -1,   164,    -1,   163,   164,    -1,    -1,   165,
     166,    -1,   249,    -1,   242,    -1,   180,    -1,   270,    -1,
     259,    -1,   263,    -1,   188,    -1,   217,    -1,   209,    -1,
     213,    -1,   226,    -1,   168,    -1,   172,    -1,   176,    -1,
     230,    -1,   234,    -1,   238,    -1,   271,    -1,   272,    -1,
     285,    -1,   287,    -1,   288,    -1,   289,    -1,   290,    -1,
     291,    -1,   292,    -1,   293,    -1,   304,    -1,   197,    -1,
     199,    -1,   184,    -1,   167,    -1,   221,    -1,   225,    -1,
     205,    -1,   332,    -1,   338,    -1,   335,    -1,   191,    -1,
     331,    -1,   310,    -1,   367,    -1,   391,    -1,   273,    -1,
       5,   169,    -1,    32,    -1,   140,   170,   141,    -1,   171,
      -1,   170,   142,   171,    -1,   126,   143,    32,    -1,     6,
     173,    -1,    32,    -1,   140,   174,   141,    -1,   175,    -1,
     174,   142,   175,    -1,   126,   143,    32,    -1,     7,   177,
      -1,    32,    -1,   140,   178,   141,    -1,   179,    -1,   178,
     142,   179,    -1,   126,   143,    32,    -1,     8,   181,    -1,
      30,    -1,   140,   182,   141,    -1,   183,    -1,   182,   142,
     183,    -1,   126,   143,    30,    -1,     9,   185,   308,    -1,
      30,    -1,   140,   186,   141,    -1,   187,    -1,   186,   142,
     187,    -1,   126,   143,    30,    -1,     3,   140,   189,   141,
      -1,   190,    -1,   189,   142,   190,    -1,   126,   143,   355,
      -1,   139,   143,    96,    -1,   110,   325,   192,   372,   193,
      -1,    -1,   140,   325,   141,    -1,   144,   194,   145,   146,
      -1,   195,    -1,   194,   195,    -1,   221,    -1,   225,    -1,
     196,    -1,   273,    -1,   111,   308,    -1,    -1,    26,   399,
     372,   198,   201,    -1,    -1,   337,    26,   399,   372,   200,
     201,    -1,   144,   202,   145,   146,    -1,   203,    -1,   202,
     203,    -1,   221,    -1,   225,    -1,   273,    -1,   286,    -1,
      47,   308,    -1,    48,   308,    -1,   282,    -1,   310,    -1,
     204,    -1,    90,   395,    30,   140,   380,   141,   369,   401,
     372,   365,   146,   303,   378,   377,    -1,    -1,    99,    30,
     206,   207,   146,    -1,    -1,   144,   208,   145,    -1,   167,
      -1,   208,   167,    -1,    -1,   101,   210,   144,   211,   145,
      -1,   212,    -1,   211,   212,    -1,    30,    -1,   102,   214,
      -1,    30,    -1,   140,   215,   141,    -1,   216,    -1,   215,
     142,   216,    -1,   126,   143,   355,    -1,    -1,   100,   218,
     144,   219,   145,    -1,   220,    -1,   219,   220,    -1,    30,
      -1,    -1,    28,   140,   222,   224,   141,    -1,    30,    -1,
     147,    30,    -1,   223,    88,    30,    -1,   223,    88,   147,
      30,    -1,   223,    -1,   313,   148,   313,    -1,    29,    -1,
     103,   227,    -1,    -1,    32,    -1,   140,   228,   141,    -1,
     229,    -1,   228,   142,   229,    -1,   133,   143,    32,    -1,
     125,   143,    32,    -1,   131,   143,    32,    -1,   132,   143,
      32,    -1,   116,   231,    -1,   257,    -1,   140,   232,   141,
      -1,   233,    -1,   232,   142,   233,    -1,   126,   143,   257,
      -1,   117,   235,    -1,   257,    -1,   140,   236,   141,    -1,
     237,    -1,   236,   142,   237,    -1,   126,   143,   257,    -1,
     119,   239,    -1,   325,    -1,   140,   240,   141,    -1,   241,
      -1,   240,   142,   241,    -1,   126,   143,   325,    -1,    56,
     243,   246,    -1,   257,    -1,   140,   244,   141,    -1,   245,
      -1,   244,   142,   245,    -1,   126,   143,   257,    -1,    -1,
     144,   247,   145,   146,    -1,   248,    -1,   247,   248,    -1,
     221,    -1,   225,    -1,   299,    -1,    55,   250,   254,    -1,
      -1,   257,   251,    -1,   140,   252,   141,    -1,   253,    -1,
     252,   142,   253,    -1,   123,   143,    32,    -1,   124,   143,
      32,    -1,   126,   143,   257,    -1,   134,   143,   328,    -1,
     135,   143,   328,    -1,   136,   143,   328,    -1,   137,   143,
     328,    -1,   138,   143,    30,    -1,    -1,   144,   255,   145,
     146,    -1,   256,    -1,   255,   256,    -1,   221,    -1,   225,
      -1,   295,    -1,   299,    -1,    30,    -1,    31,    -1,    -1,
      96,    -1,    50,   260,    -1,    31,    -1,   140,   261,   141,
      -1,   262,    -1,   261,   142,   262,    -1,   126,   143,    31,
      -1,   127,   143,   328,    -1,    51,   264,    -1,    31,    -1,
     140,   265,   141,    -1,   266,    -1,   265,   142,   266,    -1,
     126,   143,    31,    -1,    -1,    14,   308,    -1,    -1,    15,
     308,    -1,    -1,    16,   308,    -1,    25,   308,    -1,    52,
     308,    -1,    53,   308,    -1,    54,   308,    -1,    34,   308,
      -1,    35,   308,    -1,    36,   308,    -1,    37,   308,    -1,
      38,   308,    -1,    39,   308,    -1,    40,   308,    -1,    41,
     308,    -1,    46,   308,    -1,    42,   308,    -1,    20,   308,
      -1,    23,   308,    -1,    24,   308,    -1,    17,   308,    -1,
      18,   308,    -1,    19,   308,    -1,    21,   308,    -1,    22,
     308,    -1,    10,   308,    -1,    11,   308,    -1,    11,   308,
      -1,     4,   296,    -1,   140,   297,   141,    -1,   298,    -1,
     297,   142,   298,    -1,   129,   143,    32,    -1,    12,   300,
     308,    -1,    -1,    32,    -1,   140,   301,   141,    -1,   302,
      -1,   301,   142,   302,    -1,   120,   143,    32,    -1,   131,
     143,    32,    -1,    -1,   299,    -1,    13,   305,   308,    -1,
      30,    -1,   140,   306,   141,    -1,   307,    -1,   306,   142,
     307,    -1,   122,   143,    30,    -1,   128,   143,    96,    -1,
     309,    29,    -1,    27,    -1,   309,    27,    -1,    -1,    84,
     312,   313,   372,   311,   144,   314,   145,   146,    -1,    -1,
      57,    -1,    58,    -1,    -1,    30,    -1,    -1,   315,    -1,
     316,    -1,   315,   316,    -1,   221,    -1,   225,    -1,    30,
     318,   372,   317,    -1,    -1,   142,    -1,    -1,   143,   323,
      -1,    -1,   143,   320,    -1,   323,    -1,   320,   321,   323,
      -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,   152,
      -1,   153,    -1,    -1,   147,    -1,   154,    -1,   148,    -1,
     149,    -1,   150,    -1,   152,    -1,   324,   322,   329,    -1,
      -1,   140,   325,   141,    -1,    87,   326,    -1,   326,    -1,
     327,    -1,   326,    87,   327,    -1,    30,    -1,   105,    -1,
     106,    -1,   325,    -1,   399,   140,   330,   141,    -1,   144,
     145,    -1,    97,    -1,    96,    -1,   328,    -1,   107,    -1,
      32,    -1,   104,    -1,    -1,   320,    -1,   330,   142,   320,
      -1,    98,   395,    30,   372,   146,   303,    -1,    98,   395,
     140,   150,    30,   141,   140,   400,   141,   372,   146,   303,
      -1,    -1,    -1,    58,   325,   333,   341,   372,   334,   345,
     146,    -1,    -1,   337,   336,   338,    -1,   113,   155,   400,
     156,    -1,    -1,    -1,    57,   325,   339,   341,   372,   340,
     345,   146,    -1,    -1,   157,   342,    -1,   343,    -1,   342,
     142,   343,    -1,   344,   325,    -1,    -1,    59,    -1,    60,
      -1,    61,    -1,    -1,   144,   346,   145,    -1,    -1,   347,
      -1,   346,   347,    -1,   221,    -1,   225,    -1,   205,    -1,
     332,    -1,   338,    -1,   335,    -1,   191,    -1,   331,    -1,
     310,    -1,   348,    -1,   299,    -1,   286,    -1,   273,    -1,
     274,    -1,   275,    -1,   276,    -1,   277,    -1,   278,    -1,
     279,    -1,   280,    -1,   281,    -1,   282,    -1,   283,    -1,
     284,    -1,   294,    -1,   360,    -1,   357,    -1,   383,    -1,
      49,   308,    -1,    48,   308,    -1,    47,   308,    -1,    59,
     356,   157,    -1,    60,   356,   157,    -1,    61,   356,   157,
      -1,    62,   157,    -1,   118,   349,   352,    -1,   140,   350,
     141,    -1,   351,    -1,   350,   142,   351,    -1,   121,   143,
      30,    -1,   126,   143,   355,    -1,   130,   143,    30,    -1,
      -1,   144,   353,   145,   146,    -1,   354,    -1,   353,   354,
      -1,   221,    -1,   225,    -1,   299,    -1,    30,    -1,    32,
      -1,    -1,    64,    -1,    -1,    83,   358,   359,    -1,   359,
      -1,   154,    30,   140,   141,   401,   371,   372,   146,   378,
     377,   379,    -1,    -1,   112,   361,   362,    -1,   362,    -1,
      30,   140,   380,   141,   401,   372,   363,   146,   303,   378,
     377,    -1,    -1,    -1,   158,   364,   140,   380,   141,   159,
      -1,    -1,    -1,   158,   366,   395,   140,   380,   141,   159,
      -1,   395,    30,   140,   380,   141,   369,   370,   401,   371,
     372,   365,   146,   303,   378,   377,   379,   376,    -1,   395,
     108,   143,   140,   395,   141,   146,    -1,   395,   108,   368,
     140,   380,   141,   369,   370,   401,   371,   372,   365,   146,
     378,   377,   379,   376,    -1,   108,   395,   140,   380,   141,
     369,   370,   401,   371,   372,   365,   146,   378,   377,   379,
     376,    -1,   149,    -1,   148,    -1,   150,    -1,   151,    -1,
     160,    -1,   152,    -1,   153,    -1,   161,    -1,   155,   155,
      -1,   156,   156,    -1,   149,   143,    -1,   148,   143,    -1,
     150,   143,    -1,   151,   143,    -1,   160,   143,    -1,   152,
     143,    -1,   153,   143,    -1,   161,   143,    -1,   155,   155,
     143,    -1,   156,   156,   143,    -1,   154,    -1,   140,   141,
      -1,   158,   159,    -1,   155,    -1,   155,   143,    -1,   143,
     143,    -1,   147,   143,    -1,   156,    -1,   156,   143,    -1,
      -1,    89,    -1,    -1,   114,    -1,    -1,   143,    96,    -1,
      -1,   151,   373,   151,    -1,   374,    -1,   373,   142,   374,
      -1,    30,    -1,    30,   143,   375,    -1,   257,    -1,    30,
     157,   258,   148,   258,    -1,    32,    -1,    96,    -1,    -1,
      43,   308,    -1,    -1,    44,   308,    -1,    -1,    45,   308,
      -1,    -1,    33,   308,    -1,   381,    -1,    -1,   382,    -1,
     381,   142,   382,    -1,    91,   313,   372,   319,    -1,    92,
     313,   372,   319,    -1,    93,   313,   372,    -1,   396,   319,
      -1,    -1,    63,   384,   386,    -1,    -1,    65,   385,   386,
      -1,   386,    -1,    -1,    90,   387,   388,    -1,   388,    -1,
     389,    -1,   391,    -1,    -1,    83,   390,   367,    -1,   367,
      -1,   395,    30,   372,   392,   146,   267,   268,   269,    -1,
      -1,   144,   393,   145,    -1,   394,    -1,   393,   394,    -1,
     221,    -1,   225,    -1,    14,   308,    -1,    15,   308,    -1,
      16,   308,    -1,    89,   399,   398,   397,    -1,   399,   398,
     397,    -1,   395,   313,   372,    -1,    -1,   152,    -1,    -1,
     398,   150,    89,    -1,   398,   150,    -1,   325,    -1,   325,
     155,   400,   156,    -1,    58,   325,    -1,    86,    67,    -1,
      67,    -1,    86,    -1,    86,    68,    -1,    68,    -1,    69,
      -1,    86,    69,    -1,    69,    69,    -1,    86,    69,    69,
      -1,    70,    -1,    71,    -1,    66,    -1,    85,    72,    -1,
      86,    72,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    82,    -1,    94,    -1,    95,    -1,   115,
      -1,   395,    -1,   400,   142,   395,    -1,    -1,   109,   140,
     402,   141,    -1,    -1,   325,    -1,   402,   142,   325,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   553,   553,   554,   557,   557,   576,   577,   578,   579,
     580,   581,   582,   583,   584,   585,   586,   587,   588,   589,
     590,   591,   592,   593,   594,   595,   596,   597,   598,   599,
     600,   601,   602,   603,   604,   605,   606,   607,   610,   611,
     612,   613,   614,   615,   616,   617,   618,   619,   620,   621,
     634,   640,   645,   650,   651,   661,   668,   674,   679,   684,
     685,   695,   702,   711,   716,   721,   722,   732,   739,   750,
     755,   760,   761,   771,   778,   807,   812,   817,   818,   828,
     835,   861,   862,   873,   879,   887,   934,   938,  1045,  1050,
    1051,  1062,  1065,  1068,  1082,  1098,  1103,  1103,  1126,  1126,
    1193,  1207,  1208,  1211,  1212,  1213,  1217,  1221,  1230,  1239,
    1248,  1249,  1252,  1267,  1267,  1304,  1305,  1308,  1309,  1312,
    1312,  1341,  1342,  1345,  1351,  1357,  1362,  1367,  1368,  1378,
    1385,  1385,  1411,  1412,  1415,  1421,  1421,  1440,  1443,  1446,
    1449,  1454,  1455,  1460,  1468,  1484,  1492,  1498,  1503,  1504,
    1517,  1525,  1533,  1541,  1551,  1562,  1567,  1572,  1573,  1583,
    1590,  1601,  1606,  1611,  1612,  1622,  1629,  1641,  1646,  1651,
    1652,  1662,  1669,  1687,  1692,  1697,  1698,  1708,  1715,  1719,
    1724,  1725,  1735,  1738,  1741,  1755,  1768,  1768,  1778,  1783,
    1784,  1801,  1813,  1831,  1843,  1855,  1867,  1879,  1891,  1905,
    1909,  1914,  1915,  1925,  1928,  1931,  1934,  1948,  1949,  1965,
    1968,  1971,  1980,  1986,  1991,  1992,  2003,  2009,  2017,  2023,
    2028,  2033,  2034,  2044,  2051,  2054,  2059,  2062,  2067,  2070,
    2075,  2081,  2087,  2093,  2098,  2103,  2108,  2113,  2118,  2123,
    2128,  2133,  2138,  2143,  2148,  2153,  2159,  2164,  2170,  2176,
    2182,  2188,  2194,  2200,  2206,  2211,  2217,  2222,  2223,  2233,
    2240,  2324,  2328,  2334,  2339,  2340,  2351,  2357,  2365,  2368,
    2371,  2380,  2386,  2391,  2392,  2403,  2409,  2420,  2423,  2424,
    2434,  2434,  2458,  2461,  2464,  2469,  2472,  2477,  2478,  2481,
    2482,  2485,  2486,  2487,  2526,  2527,  2530,  2531,  2534,  2537,
    2542,  2543,  2561,  2564,  2567,  2570,  2573,  2576,  2581,  2584,
    2587,  2590,  2593,  2596,  2599,  2604,  2619,  2622,  2627,  2633,
    2636,  2637,  2645,  2650,  2653,  2658,  2667,  2677,  2680,  2684,
    2688,  2692,  2696,  2700,  2706,  2711,  2717,  2735,  2757,  2796,
    2802,  2796,  2846,  2846,  2872,  2877,  2883,  2877,  2923,  2924,
    2927,  2928,  2931,  2983,  2986,  2989,  2992,  2997,  3000,  3005,
    3006,  3007,  3010,  3011,  3012,  3013,  3014,  3015,  3016,  3017,
    3018,  3019,  3020,  3031,  3035,  3039,  3050,  3061,  3072,  3083,
    3094,  3105,  3116,  3127,  3138,  3149,  3160,  3171,  3172,  3173,
    3174,  3185,  3196,  3207,  3214,  3221,  3228,  3237,  3250,  3255,
    3256,  3268,  3275,  3282,  3291,  3295,  3300,  3301,  3311,  3314,
    3317,  3331,  3332,  3335,  3338,  3344,  3344,  3345,  3348,  3414,
    3414,  3415,  3418,  3464,  3467,  3467,  3478,  3481,  3481,  3493,
    3511,  3531,  3575,  3656,  3657,  3658,  3659,  3660,  3661,  3662,
    3663,  3664,  3665,  3666,  3667,  3668,  3669,  3670,  3671,  3672,
    3673,  3674,  3675,  3676,  3677,  3678,  3679,  3680,  3681,  3682,
    3683,  3684,  3687,  3690,  3695,  3698,  3703,  3706,  3714,  3717,
    3723,  3727,  3739,  3743,  3749,  3753,  3776,  3780,  3786,  3789,
    3794,  3797,  3802,  3805,  3810,  3813,  3818,  3855,  3860,  3866,
    3889,  3901,  3913,  3932,  3939,  3939,  3940,  3940,  3941,  3945,
    3945,  3946,  3950,  3951,  3955,  3955,  3956,  3959,  4014,  4020,
    4025,  4026,  4038,  4041,  4044,  4059,  4074,  4091,  4096,  4110,
    4220,  4223,  4231,  4234,  4237,  4242,  4250,  4261,  4276,  4280,
    4284,  4288,  4292,  4296,  4300,  4304,  4308,  4312,  4316,  4320,
    4324,  4328,  4332,  4336,  4340,  4344,  4348,  4352,  4356,  4360,
    4364,  4368,  4372,  4376,  4380,  4384,  4390,  4396,  4412,  4415,
    4423,  4429,  4436
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TK_API", "TK_AUTOPYNAME",
  "TK_DEFDOCSTRFMT", "TK_DEFDOCSTRSIG", "TK_DEFENCODING", "TK_PLUGIN",
  "TK_VIRTERRORHANDLER", "TK_EXPTYPEHINTCODE", "TK_TYPEHINTCODE",
  "TK_DOCSTRING", "TK_EXTRACT", "TK_ACCESSCODE", "TK_GETCODE",
  "TK_SETCODE", "TK_PREINITCODE", "TK_INITCODE", "TK_POSTINITCODE",
  "TK_FINALCODE", "TK_UNITCODE", "TK_UNITPOSTINCLUDECODE", "TK_MODCODE",
  "TK_TYPECODE", "TK_COPYING", "TK_MAPPEDTYPE", "TK_CODELINE", "TK_IF",
  "TK_END", "TK_NAME_VALUE", "TK_PATH_VALUE", "TK_STRING_VALUE",
  "TK_VIRTUALCATCHERCODE", "TK_TRAVERSECODE", "TK_CLEARCODE",
  "TK_GETBUFFERCODE", "TK_RELEASEBUFFERCODE", "TK_READBUFFERCODE",
  "TK_WRITEBUFFERCODE", "TK_SEGCOUNTCODE", "TK_CHARBUFFERCODE",
  "TK_PICKLECODE", "TK_VIRTUALCALLCODE", "TK_METHODCODE",
  "TK_PREMETHODCODE", "TK_INSTANCECODE", "TK_FROMTYPE", "TK_TOTYPE",
  "TK_TOSUBCLASS", "TK_INCLUDE", "TK_IMPORT", "TK_EXPHEADERCODE",
  "TK_MODHEADERCODE", "TK_TYPEHEADERCODE", "TK_MODULE", "TK_COMPOMODULE",
  "TK_CLASS", "TK_STRUCT", "TK_PUBLIC", "TK_PROTECTED", "TK_PRIVATE",
  "TK_SIGNALS", "TK_SIGNAL_METHOD", "TK_SLOTS", "TK_SLOT_METHOD",
  "TK_BOOL", "TK_SHORT", "TK_INT", "TK_LONG", "TK_FLOAT", "TK_DOUBLE",
  "TK_CHAR", "TK_WCHAR_T", "TK_VOID", "TK_PYOBJECT", "TK_PYTUPLE",
  "TK_PYLIST", "TK_PYDICT", "TK_PYCALLABLE", "TK_PYSLICE", "TK_PYTYPE",
  "TK_PYBUFFER", "TK_VIRTUAL", "TK_ENUM", "TK_SIGNED", "TK_UNSIGNED",
  "TK_SCOPE", "TK_LOGICAL_OR", "TK_CONST", "TK_STATIC", "TK_SIPSIGNAL",
  "TK_SIPSLOT", "TK_SIPRXCON", "TK_PYSSIZET", "TK_SIZET",
  "TK_NUMBER_VALUE", "TK_REAL_VALUE", "TK_TYPEDEF", "TK_NAMESPACE",
  "TK_TIMELINE", "TK_PLATFORMS", "TK_FEATURE", "TK_LICENSE",
  "TK_QCHAR_VALUE", "TK_TRUE_VALUE", "TK_FALSE_VALUE", "TK_NULL_VALUE",
  "TK_OPERATOR", "TK_THROW", "TK_EXCEPTION", "TK_RAISECODE", "TK_EXPLICIT",
  "TK_TEMPLATE", "TK_FINAL", "TK_ELLIPSIS", "TK_DEFMETATYPE",
  "TK_DEFSUPERTYPE", "TK_PROPERTY", "TK_HIDE_NS", "TK_FORMAT", "TK_GET",
  "TK_ID", "TK_KWARGS", "TK_LANGUAGE", "TK_LICENSEE", "TK_NAME",
  "TK_OPTIONAL", "TK_ORDER", "TK_REMOVELEADING", "TK_SET", "TK_SIGNATURE",
  "TK_TIMESTAMP", "TK_TYPE", "TK_USEARGNAMES", "TK_USELIMITEDAPI",
  "TK_ALLRAISEPYEXC", "TK_CALLSUPERINIT", "TK_DEFERRORHANDLER",
  "TK_VERSION", "'('", "')'", "','", "'='", "'{'", "'}'", "';'", "'!'",
  "'-'", "'+'", "'*'", "'/'", "'&'", "'|'", "'~'", "'<'", "'>'", "':'",
  "'['", "']'", "'%'", "'^'", "$accept", "specification", "statement",
  "@1", "modstatement", "nsstatement", "defdocstringfmt",
  "defdocstringfmt_args", "defdocstringfmt_arg_list",
  "defdocstringfmt_arg", "defdocstringsig", "defdocstringsig_args",
  "defdocstringsig_arg_list", "defdocstringsig_arg", "defencoding",
  "defencoding_args", "defencoding_arg_list", "defencoding_arg", "plugin",
  "plugin_args", "plugin_arg_list", "plugin_arg", "virterrorhandler",
  "veh_args", "veh_arg_list", "veh_arg", "api", "api_arg_list", "api_arg",
  "exception", "baseexception", "exception_body",
  "exception_body_directives", "exception_body_directive", "raisecode",
  "mappedtype", "@2", "mappedtypetmpl", "@3", "mtdefinition", "mtbody",
  "mtline", "mtfunction", "namespace", "@4", "optnsbody", "nsbody",
  "platforms", "@5", "platformlist", "platform", "feature", "feature_args",
  "feature_arg_list", "feature_arg", "timeline", "@6", "qualifierlist",
  "qualifiername", "ifstart", "@7", "oredqualifiers", "qualifiers",
  "ifend", "license", "license_args", "license_arg_list", "license_arg",
  "defmetatype", "defmetatype_args", "defmetatype_arg_list",
  "defmetatype_arg", "defsupertype", "defsupertype_args",
  "defsupertype_arg_list", "defsupertype_arg", "hiddenns", "hiddenns_args",
  "hiddenns_arg_list", "hiddenns_arg", "compmodule", "compmodule_args",
  "compmodule_arg_list", "compmodule_arg", "compmodule_body",
  "compmodule_body_directives", "compmodule_body_directive", "module",
  "module_args", "@8", "module_arg_list", "module_arg", "module_body",
  "module_body_directives", "module_body_directive", "dottedname",
  "optnumber", "include", "include_args", "include_arg_list",
  "include_arg", "import", "import_args", "import_arg_list", "import_arg",
  "optaccesscode", "optgetcode", "optsetcode", "copying", "exphdrcode",
  "modhdrcode", "typehdrcode", "travcode", "clearcode", "getbufcode",
  "releasebufcode", "readbufcode", "writebufcode", "segcountcode",
  "charbufcode", "instancecode", "picklecode", "finalcode", "modcode",
  "typecode", "preinitcode", "initcode", "postinitcode", "unitcode",
  "unitpostinccode", "exptypehintcode", "modtypehintcode",
  "classtypehintcode", "autopyname", "autopyname_args",
  "autopyname_arg_list", "autopyname_arg", "docstring", "docstring_args",
  "docstring_arg_list", "docstring_arg", "optdocstring", "extract",
  "extract_args", "extract_arg_list", "extract_arg", "codeblock",
  "codelines", "enum", "@9", "optenumkey", "optname", "optenumbody",
  "enumbody", "enumline", "optcomma", "optenumassign", "optassign", "expr",
  "binop", "optunop", "value", "optcast", "scopedname", "scopednamehead",
  "scopepart", "bool_value", "simplevalue", "exprlist", "typedef",
  "struct", "@10", "@11", "classtmpl", "@12", "template", "class", "@13",
  "@14", "superclasses", "superlist", "superclass", "class_access",
  "optclassbody", "classbody", "classline", "property", "property_args",
  "property_arg_list", "property_arg", "property_body",
  "property_body_directives", "property_body_directive", "name_or_string",
  "optslot", "dtor", "@15", "dtor_decl", "ctor", "@16", "simplector",
  "optctorsig", "@17", "optsig", "@18", "function", "operatorname",
  "optconst", "optfinal", "optabstract", "optflags", "flaglist", "flag",
  "flagvalue", "virtualcallcode", "methodcode", "premethodcode",
  "virtualcatchercode", "arglist", "rawarglist", "argvalue", "varmember",
  "@19", "@20", "simple_varmem", "@21", "varmem", "member", "@22",
  "variable", "variable_body", "variable_body_directives",
  "variable_body_directive", "cpptype", "argtype", "optref", "deref",
  "basetype", "cpptypelist", "optexceptions", "exceptionlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
      40,    41,    44,    61,   123,   125,    59,    33,    45,    43,
      42,    47,    38,   124,   126,    60,    62,    58,    91,    93,
      37,    94
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   162,   163,   163,   165,   164,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     168,   169,   169,   170,   170,   171,   172,   173,   173,   174,
     174,   175,   176,   177,   177,   178,   178,   179,   180,   181,
     181,   182,   182,   183,   184,   185,   185,   186,   186,   187,
     188,   189,   189,   190,   190,   191,   192,   192,   193,   194,
     194,   195,   195,   195,   195,   196,   198,   197,   200,   199,
     201,   202,   202,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   204,   206,   205,   207,   207,   208,   208,   210,
     209,   211,   211,   212,   213,   214,   214,   215,   215,   216,
     218,   217,   219,   219,   220,   222,   221,   223,   223,   223,
     223,   224,   224,   225,   226,   227,   227,   227,   228,   228,
     229,   229,   229,   229,   230,   231,   231,   232,   232,   233,
     234,   235,   235,   236,   236,   237,   238,   239,   239,   240,
     240,   241,   242,   243,   243,   244,   244,   245,   246,   246,
     247,   247,   248,   248,   248,   249,   251,   250,   250,   252,
     252,   253,   253,   253,   253,   253,   253,   253,   253,   254,
     254,   255,   255,   256,   256,   256,   256,   257,   257,   258,
     258,   259,   260,   260,   261,   261,   262,   262,   263,   264,
     264,   265,   265,   266,   267,   267,   268,   268,   269,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   297,   298,
     299,   300,   300,   300,   301,   301,   302,   302,   303,   303,
     304,   305,   305,   306,   306,   307,   307,   308,   309,   309,
     311,   310,   312,   312,   312,   313,   313,   314,   314,   315,
     315,   316,   316,   316,   317,   317,   318,   318,   319,   319,
     320,   320,   321,   321,   321,   321,   321,   321,   322,   322,
     322,   322,   322,   322,   322,   323,   324,   324,   325,   325,
     326,   326,   327,   328,   328,   329,   329,   329,   329,   329,
     329,   329,   329,   329,   330,   330,   330,   331,   331,   333,
     334,   332,   336,   335,   337,   339,   340,   338,   341,   341,
     342,   342,   343,   344,   344,   344,   344,   345,   345,   346,
     346,   346,   347,   347,   347,   347,   347,   347,   347,   347,
     347,   347,   347,   347,   347,   347,   347,   347,   347,   347,
     347,   347,   347,   347,   347,   347,   347,   347,   347,   347,
     347,   347,   347,   347,   347,   347,   347,   348,   349,   350,
     350,   351,   351,   351,   352,   352,   353,   353,   354,   354,
     354,   355,   355,   356,   356,   358,   357,   357,   359,   361,
     360,   360,   362,   363,   364,   363,   365,   366,   365,   367,
     367,   367,   367,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   369,   369,   370,   370,   371,   371,   372,   372,
     373,   373,   374,   374,   375,   375,   375,   375,   376,   376,
     377,   377,   378,   378,   379,   379,   380,   381,   381,   381,
     382,   382,   382,   382,   384,   383,   385,   383,   383,   387,
     386,   386,   388,   388,   390,   389,   389,   391,   392,   392,
     393,   393,   394,   394,   394,   394,   394,   395,   395,   396,
     397,   397,   398,   398,   398,   399,   399,   399,   399,   399,
     399,   399,   399,   399,   399,   399,   399,   399,   399,   399,
     399,   399,   399,   399,   399,   399,   399,   399,   399,   399,
     399,   399,   399,   399,   399,   399,   400,   400,   401,   401,
     402,   402,   402
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     1,     3,     3,     2,     1,     3,     1,
       3,     3,     2,     1,     3,     1,     3,     3,     2,     1,
       3,     1,     3,     3,     3,     1,     3,     1,     3,     3,
       4,     1,     3,     3,     3,     5,     0,     3,     4,     1,
       2,     1,     1,     1,     1,     2,     0,     5,     0,     6,
       4,     1,     2,     1,     1,     1,     1,     2,     2,     1,
       1,     1,    14,     0,     5,     0,     3,     1,     2,     0,
       5,     1,     2,     1,     2,     1,     3,     1,     3,     3,
       0,     5,     1,     2,     1,     0,     5,     1,     2,     3,
       4,     1,     3,     1,     2,     0,     1,     3,     1,     3,
       3,     3,     3,     3,     2,     1,     3,     1,     3,     3,
       2,     1,     3,     1,     3,     3,     2,     1,     3,     1,
       3,     3,     3,     1,     3,     1,     3,     3,     0,     4,
       1,     2,     1,     1,     1,     3,     0,     2,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     0,
       4,     1,     2,     1,     1,     1,     1,     1,     1,     0,
       1,     2,     1,     3,     1,     3,     3,     3,     2,     1,
       3,     1,     3,     3,     0,     2,     0,     2,     0,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     1,     3,     3,
       3,     0,     1,     3,     1,     3,     3,     3,     0,     1,
       3,     1,     3,     1,     3,     3,     3,     2,     1,     2,
       0,     9,     0,     1,     1,     0,     1,     0,     1,     1,
       2,     1,     1,     4,     0,     1,     0,     2,     0,     2,
       1,     3,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     1,     1,     1,     1,     3,     0,     3,     2,     1,
       1,     3,     1,     1,     1,     1,     4,     2,     1,     1,
       1,     1,     1,     1,     0,     1,     3,     6,    12,     0,
       0,     8,     0,     3,     4,     0,     0,     8,     0,     2,
       1,     3,     2,     0,     1,     1,     1,     0,     3,     0,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     3,     3,     3,     2,     3,     3,     1,
       3,     3,     3,     3,     0,     4,     1,     2,     1,     1,
       1,     1,     1,     0,     1,     0,     3,     1,    11,     0,
       3,     1,    11,     0,     0,     6,     0,     0,     7,    17,
       7,    17,    16,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     1,     2,     2,     1,     2,     2,     2,
       1,     2,     0,     1,     0,     1,     0,     2,     0,     3,
       1,     3,     1,     3,     1,     5,     1,     1,     0,     2,
       0,     2,     0,     2,     0,     2,     1,     0,     1,     3,
       4,     4,     3,     2,     0,     3,     0,     3,     1,     0,
       3,     1,     1,     1,     0,     3,     1,     8,     0,     3,
       1,     2,     1,     1,     2,     2,     2,     4,     3,     3,
       0,     1,     0,     3,     2,     1,     4,     2,     2,     1,
       1,     2,     1,     1,     2,     2,     3,     1,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     0,     4,
       0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     4,     2,     0,     1,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   143,   322,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   539,   529,   532,   533,   537,
     538,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   552,   282,     0,   530,     0,     0,   553,   554,     0,
       0,   130,   119,     0,   145,     0,     0,     0,   555,     0,
       0,     0,     5,    37,    17,    18,    19,     8,    36,    12,
      44,    34,    35,    40,    14,    15,    13,    38,    39,    16,
      20,    21,    22,     7,     6,    10,    11,     9,    23,    24,
      49,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      46,   525,   319,   320,    45,    41,    43,   342,    42,    47,
      48,     0,   522,     0,    51,     0,    50,    57,     0,    56,
      63,     0,    62,    69,     0,    68,    75,     0,     0,   278,
     252,     0,   253,   271,     0,     0,   247,   248,   249,   250,
     251,   245,   230,     0,   468,   135,   212,     0,   211,   219,
       0,   218,   231,   232,   233,   207,   208,     0,   199,   186,
       0,   178,   173,   345,   339,   535,   283,   284,   285,   540,
     528,   531,   534,   541,   318,   522,     0,   113,     0,     0,
     125,     0,   124,   146,     0,   144,     0,    86,     0,     0,
     154,   155,     0,   160,   161,     0,   166,   167,     0,     0,
       0,     0,   468,     0,   520,     0,     0,     0,    81,     0,
       0,    53,     0,     0,    59,     0,     0,    65,     0,     0,
      71,     0,     0,    77,    74,   279,   277,     0,     0,     0,
     273,   270,   527,     0,    96,   285,     0,     0,     0,   214,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   189,     0,   185,   187,     0,     0,   175,     0,
     172,   348,   348,   286,   468,   536,   520,   468,     0,   115,
       0,     0,     0,     0,   127,     0,     0,     0,     0,     0,
     148,   487,     0,   468,   556,     0,     0,     0,   157,     0,
       0,   163,     0,     0,   169,     0,   321,   468,   343,   487,
     508,     0,     0,     0,   434,   433,   435,   436,   438,   439,
     453,   456,   460,     0,   437,   440,     0,   524,   521,   518,
       0,     0,    80,     0,     0,    52,     0,     0,    58,     0,
       0,    64,     0,     0,    70,     0,     0,    76,     0,     0,
       0,   272,     0,   472,     0,   470,     0,   137,     0,   141,
       0,     0,     0,     0,   213,     0,     0,   220,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   188,     0,     0,
     261,   203,   204,     0,   201,   205,   206,     0,   174,     0,
     182,   183,     0,   180,   184,   353,   468,   468,   280,   517,
       0,     0,     0,     0,   134,     0,   132,   123,     0,   121,
       0,   126,     0,     0,     0,     0,     0,   147,     0,   285,
     285,   285,     0,   486,   488,   285,   298,     0,     0,     0,
     344,     0,   156,     0,     0,   162,     0,     0,   168,     0,
     526,    98,     0,     0,     0,   454,     0,   458,   459,   444,
     443,   445,   446,   448,   449,   457,   441,   461,   442,   455,
     447,   450,   487,   523,   411,   412,    83,    84,    82,    55,
      54,    61,    60,    67,    66,    73,    72,    79,    78,   275,
     276,   274,     0,     0,   469,     0,    97,   138,     0,   136,
     285,   216,   323,   324,   217,   215,   223,   222,   191,   192,
     193,   194,   195,   196,   197,   198,   190,     0,   255,   262,
       0,     0,     0,   202,   177,   176,     0,   181,   354,   355,
     356,   349,   350,     0,   346,   340,     0,   268,     0,   117,
       0,   342,   114,   131,   133,   120,   122,   129,   128,   151,
     152,   153,   150,   149,   468,   468,   468,   462,     0,   468,
     316,   493,    87,     0,    85,   557,   159,   158,   165,   164,
     171,   170,     0,   462,     0,     0,     0,   512,   513,     0,
     510,   224,     0,   451,   452,     0,   207,   476,   477,   474,
     473,   471,     0,     0,     0,     0,     0,     0,   101,   111,
     103,   104,   105,   109,   106,   110,   139,     0,   142,     0,
       0,   257,     0,     0,     0,   264,   260,   200,   179,   353,
     352,   357,   357,   287,   269,   337,     0,   116,   118,   298,
     298,   492,   463,   464,   489,   519,     0,   299,   300,   308,
       0,     0,    89,    93,    91,    92,    94,    99,   464,   514,
     515,   516,   509,   511,     0,   226,     0,   462,   209,   246,
     242,   107,   108,     0,     0,   102,   140,     0,   256,     0,
       0,     0,   263,     0,   351,   359,     0,     0,   296,   291,
     292,     0,   288,   289,     0,   490,   491,   465,   558,     0,
     302,   303,   304,   305,   306,   307,   316,   309,   311,   312,
     313,   314,   310,     0,    95,     0,    90,   558,   225,     0,
     228,   430,   464,   210,     0,     0,   100,   259,   258,   266,
     267,   265,     0,     0,   322,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   413,   413,   413,
       0,   494,   496,   504,   499,   419,     0,     0,   368,   364,
     362,   363,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   373,   386,   372,   370,   369,   365,
     367,   366,     0,   360,   371,   388,   417,   387,   421,   506,
     389,   498,   501,   502,   503,   347,   341,   316,   468,     0,
     290,     0,     0,   466,   317,   301,   332,   329,   328,   333,
     331,     0,   325,   330,   315,     0,    88,   466,   227,     0,
     507,   558,   209,   487,   254,   244,   487,   234,   235,   236,
     237,   238,   239,   240,   241,   243,   392,   391,   390,   414,
       0,     0,     0,   396,     0,     0,     0,     0,     0,     0,
       0,   404,     0,   358,   361,   297,   294,   281,   468,   560,
       0,   468,   327,   316,   468,   229,   466,   475,     0,     0,
     393,   394,   395,   504,   495,   497,   416,   505,     0,   500,
       0,   420,     0,     0,     0,     0,   399,     0,   397,     0,
     295,   293,     0,   561,     0,   467,   426,   335,     0,   426,
     468,   462,   558,     0,     0,     0,     0,   398,     0,   408,
     409,   410,     0,   406,   558,   268,   559,     0,   427,     0,
     326,   316,     0,   426,   558,   468,   401,   402,   403,   400,
       0,   407,   466,   338,   562,     0,   482,   336,   268,     0,
     468,   423,   405,   468,     0,     0,   480,   482,   482,   426,
     424,     0,     0,   487,   483,     0,   484,   480,   480,     0,
       0,   268,   482,     0,   481,     0,   478,   484,   484,   268,
     487,   482,   480,     0,   485,     0,   432,   478,   478,   482,
       0,   480,   484,   428,   479,   429,   431,   480,     0,   422,
     418,   112,   425
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,    72,    73,    74,   126,   220,   221,
      75,   129,   223,   224,    76,   132,   226,   227,    77,   135,
     229,   230,    78,   138,   232,   233,    79,   217,   218,    80,
     293,   554,   631,   632,   633,    81,   356,    82,   562,   486,
     587,   588,   589,    83,   279,   403,   530,    84,   189,   408,
     409,    85,   192,   283,   284,    86,   188,   405,   406,    87,
     245,   359,   360,    88,    89,   195,   289,   290,    90,   200,
     297,   298,    91,   203,   300,   301,    92,   206,   303,   304,
      93,   171,   267,   268,   270,   392,   393,    94,   168,   265,
     261,   262,   264,   383,   384,   169,   704,    95,   158,   248,
     249,    96,   161,   251,   252,   645,   700,   800,    97,    98,
      99,   100,   743,   744,   745,   746,   747,   748,   749,   750,
     593,   752,   753,   101,   594,   102,   103,   104,   105,   106,
     107,   108,   755,   385,   508,   600,   601,   614,   511,   604,
     605,   615,   109,   145,   239,   240,   140,   141,   110,   526,
     178,   274,   671,   672,   673,   871,   778,   551,   627,   686,
     693,   628,   629,   111,   112,   113,   494,   794,   878,   114,
     115,   272,   612,   116,   211,   531,   118,   271,   611,   396,
     521,   522,   523,   666,   762,   763,   764,   831,   865,   866,
     868,   892,   893,   466,   820,   765,   826,   766,   767,   829,
     768,   931,   940,   899,   915,   769,   326,   623,   678,   841,
     244,   354,   355,   580,   956,   936,   926,   946,   422,   423,
     424,   770,   824,   825,   771,   828,   772,   773,   827,   774,
     444,   569,   570,   121,   426,   329,   214,   122,   295,   783,
     874
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -822
static const yytype_int16 yypact[] =
{
    -822,   119,  -822,  1013,  -822,  -822,    -1,    14,    34,    45,
      22,    64,   160,   160,    70,   160,   160,   160,   160,   160,
     160,   160,   667,    85,  -822,  -822,    40,    43,   160,   160,
     160,    48,    50,    65,    65,  -822,  -822,  -822,   122,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,   194,   150,   342,   244,   667,  -822,  -822,  1356,
     246,  -822,  -822,    73,    76,  1356,    65,   123,  -822,    54,
      57,    37,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,   127,   214,  -822,  -822,  -822,  -822,   279,  -822,  -822,
    -822,    90,  -822,   102,  -822,   181,  -822,  -822,   183,  -822,
    -822,   199,  -822,  -822,   207,  -822,  -822,   234,   160,  -822,
    -822,   115,  -822,  -822,   105,   160,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,    65,   179,  -822,  -822,   144,  -822,  -822,
     248,  -822,  -822,  -822,  -822,  -822,  -822,   379,   218,  -822,
     254,   251,  -822,  -822,     3,  -822,  -822,  -822,   374,  -822,
    -822,  -822,   303,  -822,   214,  -822,    74,  -822,   264,   268,
    -822,   295,  -822,  -822,   148,  -822,   293,   296,  1356,   316,
    -822,  -822,   334,  -822,  -822,   339,  -822,  -822,  1356,   244,
     667,   410,    86,   443,    13,   330,   343,   205,  -822,   345,
     215,  -822,   348,   223,  -822,   357,   235,  -822,   361,   245,
    -822,   363,   256,  -822,  -822,  -822,  -822,   375,   393,   275,
    -822,  -822,  -822,   507,  -822,    20,   395,   397,   278,  -822,
     398,   283,  -822,   399,   400,   401,   402,   404,   405,   406,
     407,   286,  -822,   228,  -822,  -822,   411,   297,  -822,   236,
    -822,   403,   403,  -822,   179,  -822,    13,   179,   408,   412,
     525,   527,   416,   299,  -822,   418,   419,   420,   421,   328,
    -822,  1142,    65,   179,  -822,   -21,   423,   336,  -822,   424,
     338,  -822,   425,   340,  -822,    82,  -822,   179,  -822,  1142,
     429,   433,   -26,   432,   434,   435,   436,   437,   438,   439,
    -822,   100,   110,   417,   441,   442,   448,   500,  -822,  -822,
     229,   504,  -822,   102,   573,  -822,   181,   574,  -822,   183,
     575,  -822,   199,   578,  -822,   207,   579,  -822,   234,   580,
     515,  -822,   105,   469,    -5,  -822,   470,   465,   585,   528,
     477,   471,   589,   230,  -822,   144,   590,  -822,   248,   591,
     593,   453,   230,   230,   230,   230,   592,  -822,   379,   486,
      78,  -822,  -822,    31,  -822,  -822,  -822,   453,  -822,   254,
    -822,  -822,    25,  -822,  -822,   290,   179,   179,  -822,  -822,
     482,   599,  1089,   484,  -822,    19,  -822,  -822,    33,  -822,
     229,  -822,   295,   601,   603,   604,   606,  -822,   148,   374,
     374,   374,   498,   499,  -822,   374,   497,   501,   502,  1356,
    -822,   453,  -822,   316,   453,  -822,   334,    65,  -822,   339,
    -822,  -822,   503,   313,   505,  -822,  1356,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,   509,  -822,   512,  -822,
    -822,  -822,  1142,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,   204,   507,  -822,   291,  -822,  -822,    21,  -822,
     374,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,   514,  -822,  -822,
      51,   160,   510,  -822,  -822,  -822,   516,  -822,  -822,  -822,
    -822,   506,  -822,    65,  -822,  -822,   517,   633,   519,  -822,
     377,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,   179,   179,   179,   569,  1142,   179,
     523,  -822,  -822,    84,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,   470,   569,   160,   160,   160,  -822,  -822,    41,
    -822,   650,   524,  -822,  -822,   526,   511,  -822,  -822,  -822,
    -822,  -822,   160,   160,   160,   160,  1356,    44,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,   636,  -822,   529,
     352,  -822,   530,   531,   354,  -822,  -822,  -822,  -822,   290,
    -822,   532,   532,   355,  -822,  -822,   535,  -822,  -822,   497,
     497,  -822,  -822,   556,  -822,  -822,    65,   241,  -822,   219,
     160,    47,  -822,  -822,  -822,  -822,  -822,  -822,   556,  -822,
    -822,  -822,  -822,  -822,   160,   656,   533,   569,   581,  -822,
    -822,  -822,  -822,   648,   534,  -822,  -822,   649,  -822,   514,
     651,   652,  -822,    51,  -822,   899,   539,   541,   545,  -822,
    -822,   546,   355,  -822,  1356,  -822,  -822,  -822,   583,   549,
    -822,  -822,  -822,  -822,  -822,  -822,   523,  -822,  -822,  -822,
    -822,  -822,  -822,   217,  -822,   547,  -822,   583,  -822,   160,
     678,  -822,   556,  -822,   548,   555,  -822,  -822,  -822,  -822,
    -822,  -822,   160,   160,   558,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   637,   637,   637,
     559,  -822,  -822,   561,  -822,  -822,   577,   688,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,   772,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,   523,   179,   576,
    -822,   367,   584,   587,  -822,  -822,  -822,  -822,  -822,  -822,
    -822,   582,     5,  -822,  -822,   586,  -822,   587,  -822,   160,
    -822,   583,   581,  1142,  -822,  -822,  1142,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
     562,   563,   564,  -822,  1195,  1195,   596,  1303,  1249,   693,
     137,   607,   588,  -822,  -822,  -822,   613,  -822,   179,    65,
     635,   179,  -822,   261,   179,  -822,   587,  -822,   616,   617,
    -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,   113,  -822,
     558,  -822,   620,   621,   622,   370,  -822,   236,  -822,   619,
    -822,  -822,   623,  -822,   382,  -822,   608,   241,   384,   608,
     179,   569,   583,   627,   738,   229,   740,  -822,   137,  -822,
    -822,  -822,    36,  -822,   583,   633,  -822,    65,  -822,   625,
    -822,   523,   626,   608,   583,   179,  -822,  -822,  -822,  -822,
     628,  -822,   587,  -822,  -822,  1356,   728,   241,   633,   629,
     179,   618,  -822,   179,   638,   160,   733,   728,   728,   608,
    -822,   639,   641,  1142,  -822,   160,   746,   733,   733,   642,
     653,   633,   728,   640,  -822,   160,   747,   746,   746,   633,
    1142,   728,   733,   630,  -822,   160,  -822,   747,   747,   728,
     654,   733,   746,  -822,  -822,  -822,  -822,   733,   632,  -822,
    -822,  -822,  -822
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -822,  -822,   793,  -822,  -822,  -358,  -822,  -822,  -822,   461,
    -822,  -822,  -822,   459,  -822,  -822,  -822,   457,  -822,  -822,
    -822,   458,  -822,  -822,  -822,   456,  -822,  -822,   472,  -583,
    -822,  -822,  -822,   185,  -822,  -822,  -822,  -822,  -822,   255,
    -822,   237,  -822,  -569,  -822,  -822,  -822,  -822,  -822,  -822,
     414,  -822,  -822,  -822,   415,  -822,  -822,  -822,   431,  -243,
    -822,  -822,  -822,  -235,  -822,  -822,  -822,   445,  -822,  -822,
    -822,   390,  -822,  -822,  -822,   392,  -822,  -822,  -822,   426,
    -822,  -822,  -822,   475,  -822,  -822,   468,  -822,  -822,  -822,
    -822,   491,  -822,  -822,   489,    -8,    72,  -822,  -822,  -822,
     513,  -822,  -822,  -822,   508,  -822,  -822,  -822,  -822,  -822,
    -822,  -456,  -822,  -822,  -822,  -822,  -822,  -822,  -822,  -822,
    -566,  -822,  -822,  -822,  -550,  -822,  -822,  -822,  -822,  -822,
    -822,  -822,  -822,  -822,  -822,  -822,   216,  -233,  -822,  -822,
     220,  -672,  -822,  -822,  -822,   536,   -13,  -822,  -454,  -822,
    -822,  -218,  -822,  -822,   209,  -822,  -822,   -99,  -754,  -822,
    -822,  -593,  -822,   -24,   822,   670,  -334,  -822,  -822,  -547,
    -543,  -822,  -822,  -542,  -822,   886,  -197,  -822,  -822,   624,
    -822,   282,  -822,   280,  -822,   131,  -822,  -822,  -822,     6,
    -822,  -822,     7,  -388,  -198,  -822,  -822,    69,  -822,  -822,
      68,  -822,  -822,  -773,  -822,    -2,  -822,  -538,  -533,  -739,
    -191,  -822,   422,  -822,  -425,  -707,  -611,  -821,  -296,  -822,
     350,  -822,  -822,  -822,  -290,  -822,    79,  -822,  -822,    16,
    -822,  -822,   331,   -47,  -822,   644,   716,   -11,  -185,  -665,
    -822
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -528
static const yytype_int16 yytable[] =
{
     142,   119,   146,   147,   148,   149,   150,   151,   152,   173,
     174,   154,   186,   442,   308,   162,   163,   164,   196,   120,
     381,   310,   537,   305,   172,   638,   390,   361,   382,   592,
     386,   595,   797,  -527,   391,   379,   394,   380,   501,   502,
     503,   504,   197,   380,   529,   185,   124,   207,   380,   404,
     357,   596,   133,    23,    24,   564,   565,   566,   844,    23,
      24,   201,   204,   407,    23,    24,   127,    25,   582,    23,
      24,   156,    23,    24,   159,    23,    24,   130,   165,   166,
     165,   166,   738,   398,   165,   166,   400,   165,   166,   877,
     583,   584,   585,   785,   136,    25,   739,   636,    30,   751,
     143,    30,   428,   190,   277,   697,   902,   880,   193,   702,
     509,  -527,    23,    24,   446,   754,   441,   447,   758,     4,
     212,   429,   759,   760,    55,   234,   957,   958,    52,   242,
     919,   592,   241,   595,   586,   430,   846,   483,    30,   123,
     381,   970,   235,   883,   236,  -525,   484,   917,   382,   390,
     386,   294,    55,  -527,   125,  -527,   939,   391,   630,   394,
     208,   294,   134,   327,   533,   328,   575,   358,   597,   801,
     516,   602,   618,   923,   128,   636,   512,   205,   535,   738,
     157,   910,   603,   160,   835,   131,   642,   139,   167,   654,
     170,   175,   695,   739,   199,   630,   751,   202,   213,   307,
     567,   544,   545,   546,   137,   524,   525,   549,   568,   742,
     144,   757,   754,   191,   278,   758,   194,   905,   510,   759,
     760,   213,   179,   913,   429,   155,   309,   237,   215,   912,
     947,   948,   379,   238,   576,   166,   577,   243,   440,   920,
     380,   216,   590,   455,   425,   962,   927,    25,   380,   786,
     591,   176,   177,   457,   969,   456,    23,    24,   862,   464,
     971,   465,   425,   863,    23,    24,   458,   864,   427,   951,
     246,   247,   598,   285,    25,   153,   187,   959,   198,   286,
     287,   288,   208,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
     578,   209,    53,    54,    55,   210,   742,   219,   757,   222,
     634,    57,    58,   787,   788,   582,   937,   938,   635,    23,
      24,   789,   492,   493,   790,   225,   567,   564,   565,   566,
     243,   952,    68,   228,   568,   492,   493,   583,   584,   585,
     961,    23,    24,   904,   590,    30,   332,   333,   967,   518,
     519,   520,   591,   619,   620,   621,   335,   336,   625,   793,
     231,   791,   263,   500,   338,   339,   687,   688,   689,   690,
     669,   691,   275,   692,   250,    52,   341,   342,   670,   514,
     266,   586,   555,    23,    24,   668,   344,   345,   634,   680,
     681,   682,   683,   684,   685,   269,   635,   347,   348,   572,
     119,   626,  -334,  -334,   273,    23,    24,    25,   280,   180,
     181,   182,   281,   560,   183,   425,   351,   352,   120,   364,
     365,   282,   740,   556,   367,   368,   558,   377,   378,   669,
     741,    30,   756,   291,    33,    34,   292,   670,   388,   389,
     411,   412,   296,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
     299,    52,    53,    54,    55,   302,    56,    33,   761,   417,
     418,    57,    58,   330,   579,    59,    60,   432,   433,   435,
     436,   438,   439,   165,   166,    65,   331,    66,   334,   781,
      67,   337,    68,   658,   659,   662,   663,   907,   606,   610,
     340,   425,   253,   254,   343,   255,   346,   848,   838,   429,
     849,   887,   888,   256,   257,   258,   259,   260,   349,   740,
     675,   676,   617,   896,   897,   900,   901,   741,   119,   756,
     821,   822,   965,   966,   854,   855,   350,   353,   362,   653,
     363,   366,   369,   370,   371,   372,   120,   373,   374,   375,
     376,   639,   640,   641,   387,   404,   402,   407,   401,   410,
     395,   413,   414,   415,   416,   761,   431,   434,   437,   649,
     650,   651,   652,   443,   445,   448,   459,   449,   450,   451,
     452,   453,   454,   311,   460,   461,   312,   836,   462,   463,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     467,   323,   679,   324,   325,   469,   471,   473,   475,   477,
     479,   480,   482,  -286,   485,   487,   488,   694,   489,   490,
     491,   496,   505,   498,   889,   499,   507,   294,   527,   528,
     532,   698,   890,   539,   891,   540,   541,   943,   542,   547,
     550,   548,   552,   599,   563,   380,   553,   872,   609,   889,
     876,   571,   573,   879,   960,   574,   607,   890,   622,   891,
     616,   613,   608,   626,   644,   646,   656,   647,   648,   792,
     677,   699,   657,   660,   661,   674,   665,   703,   705,   701,
     706,   707,   795,   709,   710,   775,   798,   776,   777,   903,
     784,   779,   782,   796,   799,   803,   802,    25,   806,   804,
     805,   819,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   921,  -415,   823,   830,   832,   850,
     851,   852,   837,   860,   839,   153,   843,   842,   869,   929,
     840,   875,   932,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
     737,   867,    53,    54,    55,   870,   425,   881,   882,   425,
     894,    57,    58,   884,   885,   886,   898,   309,   906,   895,
     908,   916,   918,   925,   922,   928,   930,   935,   933,   945,
     858,   953,    68,   712,   380,   941,   845,   942,   949,   963,
     955,   972,   713,   950,     5,   968,   582,   470,   472,   474,
      23,    24,   714,   476,   478,   468,   715,   716,   717,   718,
     719,   720,   721,   722,   723,   873,   696,   637,   583,   724,
     725,   726,   536,   557,   655,   857,    30,   538,   559,    33,
      34,   727,   728,   729,   730,   731,   534,   732,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,   733,    52,    53,    54,    55,
     517,    56,   734,   543,   515,   561,    57,    58,   924,   506,
      59,    60,   513,   914,   847,   708,   497,   184,   495,   306,
      65,   780,    66,   711,   735,    67,   425,    68,   481,   117,
     736,   664,   667,   834,   909,   856,   397,   861,   624,   911,
     643,   276,     0,   425,     0,   581,     0,   859,     0,     0,
     712,   380,   934,     0,     0,     0,     0,   833,     0,   713,
     399,     0,   944,   582,     0,     0,   737,    23,    24,   714,
       0,     0,   954,   715,   716,   717,   718,   719,   720,   721,
     722,   723,   964,     0,     0,   583,   724,   725,   726,     0,
       0,     0,     0,    30,     0,     0,    33,    34,   727,   728,
     729,   730,   731,     0,   732,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   733,    52,    53,    54,    55,     0,    56,   734,
       0,     0,     0,    57,    58,     0,     0,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,    65,     0,    66,
       0,   735,    67,     0,    68,     0,     6,   736,     7,     8,
       9,    10,    11,    12,    13,     0,    14,     0,     0,     0,
      15,    16,    17,     0,    18,    19,    20,     0,    21,    22,
       0,    23,    24,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   737,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,     0,     0,     0,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,     0,    52,    53,    54,
      55,     0,    56,     0,     0,     0,     0,    57,    58,     0,
       0,    59,    60,    61,    62,    63,    64,    23,    24,    25,
       0,    65,     0,    66,     0,     0,    67,     0,    68,    69,
      70,     0,    71,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,     0,    33,    34,     0,     0,
       0,     0,     0,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    25,    52,    53,    54,    55,     0,    56,     0,
       0,     0,     0,    57,    58,     0,     0,    59,    60,     0,
       0,     0,     0,     0,     0,     0,     0,    65,     0,    66,
     153,     0,    67,     0,    68,     0,     0,     0,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    25,     0,    53,    54,    55,
       0,    56,     0,   419,   420,   421,    57,    58,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,     0,     0,     0,    68,     0,     0,
       0,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,   853,    25,
      53,    54,    55,     0,    56,   734,     0,     0,     0,    57,
      58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    65,     0,     0,     0,   153,     0,     0,
      68,     0,     0,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   853,    25,    53,    54,    55,     0,    56,     0,
       0,     0,     0,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    65,     0,     0,
       0,   153,     0,     0,    68,     0,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    25,     0,    53,    54,
      55,     0,    56,     0,     0,     0,     0,    57,    58,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    65,     0,     0,   153,     0,     0,     0,    68,     0,
       0,     0,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,     0,
       0,    53,    54,    55,     0,    56,     0,     0,     0,     0,
      57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    68
};

static const yytype_int16 yycheck[] =
{
      13,     3,    15,    16,    17,    18,    19,    20,    21,    33,
      34,    22,    59,   309,   211,    28,    29,    30,    65,     3,
     263,   212,   410,   208,    32,   563,   269,   245,   263,   485,
     263,   485,   697,    30,   269,     4,   269,    12,   372,   373,
     374,   375,    66,    12,   402,    56,    32,    71,    12,    30,
      30,    30,    30,    28,    29,    14,    15,    16,   797,    28,
      29,    69,    70,    30,    28,    29,    32,    30,    24,    28,
      29,    31,    28,    29,    31,    28,    29,    32,    30,    31,
      30,    31,   665,   274,    30,    31,   277,    30,    31,   843,
      46,    47,    48,   686,    30,    30,   665,   553,    54,   665,
      30,    54,   293,    30,    30,   638,   879,   846,    32,   647,
      32,   108,    28,    29,   140,   665,   307,   143,   665,     0,
      30,   142,   665,   665,    87,   138,   947,   948,    84,   153,
     903,   587,   145,   587,    90,   156,   801,   142,    54,   140,
     383,   962,    27,    30,    29,   140,   151,   901,   383,   392,
     383,   198,    87,   150,   140,   152,   929,   392,   111,   392,
     155,   208,   140,   150,   145,   152,   462,   147,   147,   702,
     145,   120,   530,   912,   140,   631,   145,   140,   145,   762,
     140,   145,   131,   140,   777,   140,   145,    27,   140,   145,
     140,    69,   145,   762,   140,   111,   762,   140,   108,   210,
     443,   419,   420,   421,   140,   396,   397,   425,   443,   665,
     140,   665,   762,   140,   140,   762,   140,   882,   140,   762,
     762,   108,    72,   895,   142,   140,   140,   122,   126,   894,
     937,   938,     4,   128,    30,    31,    32,   151,   156,   904,
      12,   139,   485,   143,   291,   952,   918,    30,    12,    32,
     485,    57,    58,   143,   961,   155,    28,    29,   121,    30,
     967,    32,   309,   126,    28,    29,   156,   130,   292,   941,
     126,   127,   490,   125,    30,    58,    30,   949,   155,   131,
     132,   133,   155,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      96,    87,    85,    86,    87,    26,   762,   126,   762,   126,
     553,    94,    95,    96,    97,    24,   927,   928,   553,    28,
      29,   104,   105,   106,   107,   126,   569,    14,    15,    16,
     151,   942,   115,   126,   569,   105,   106,    46,    47,    48,
     951,    28,    29,   881,   587,    54,   141,   142,   959,    59,
      60,    61,   587,   544,   545,   546,   141,   142,   549,   693,
     126,   144,   144,   371,   141,   142,   147,   148,   149,   150,
     613,   152,    69,   154,   126,    84,   141,   142,   613,   387,
     126,    90,   429,    28,    29,    30,   141,   142,   631,   148,
     149,   150,   151,   152,   153,   144,   631,   141,   142,   446,
     402,   140,   141,   142,    30,    28,    29,    30,   144,    67,
      68,    69,   144,   437,    72,   462,   141,   142,   402,   141,
     142,   126,   665,   431,   141,   142,   434,   141,   142,   672,
     665,    54,   665,   140,    57,    58,   140,   672,   141,   142,
     141,   142,   126,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
     126,    84,    85,    86,    87,   126,    89,    57,   665,   141,
     142,    94,    95,   143,   482,    98,    99,   141,   142,   141,
     142,   141,   142,    30,    31,   108,   143,   110,   143,   674,
     113,   143,   115,   141,   142,   141,   142,   885,   511,   523,
     143,   548,   123,   124,   143,   126,   143,   803,   141,   142,
     806,   141,   142,   134,   135,   136,   137,   138,   143,   762,
     619,   620,   145,   141,   142,   141,   142,   762,   530,   762,
     728,   729,   957,   958,   824,   825,   143,    30,   143,   586,
     143,   143,   143,   143,   143,   143,   530,   143,   143,   143,
     143,   564,   565,   566,   143,    30,   144,    30,   150,   143,
     157,   143,   143,   143,   143,   762,   143,   143,   143,   582,
     583,   584,   585,   144,   141,   143,   159,   143,   143,   143,
     143,   143,   143,   140,   143,   143,   143,   778,   140,    89,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
      96,   158,   626,   160,   161,    32,    32,    32,    30,    30,
      30,    96,   143,   148,   144,    30,    88,   630,   141,   148,
      31,    31,    30,    32,   867,    32,   140,   674,   146,    30,
     146,   644,   867,    32,   867,    32,    32,   933,    32,   141,
     143,   142,   141,   129,   141,    12,   144,   838,   142,   892,
     841,   146,   143,   844,   950,   143,   146,   892,    89,   892,
     141,   144,   146,   140,    14,   141,    30,   141,   157,   693,
     114,    15,   143,   143,   143,   140,   144,    96,    30,   146,
     146,    32,   693,    32,    32,   146,   699,   146,   143,   880,
     141,   145,   109,   146,    16,   140,   148,    30,   140,   712,
     713,    64,   715,   716,   717,   718,   719,   720,   721,   722,
     723,   724,   725,   726,   905,   154,   157,   140,    30,   157,
     157,   157,   146,    30,   140,    58,   140,   145,   140,   920,
     143,    96,   923,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
     154,   144,    85,    86,    87,   142,   803,   141,   141,   806,
     141,    94,    95,   143,   143,   143,   158,   140,    30,   146,
      30,   146,   146,    45,   146,   146,   158,    44,   140,    33,
     827,   141,   115,    11,    12,   146,   799,   146,   146,   159,
      43,   159,    20,   140,     1,   141,    24,   336,   339,   342,
      28,    29,    30,   345,   348,   333,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   839,   631,   562,    46,    47,
      48,    49,   408,   433,   587,   827,    54,   412,   436,    57,
      58,    59,    60,    61,    62,    63,   405,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
     392,    89,    90,   418,   389,   439,    94,    95,   915,   378,
      98,    99,   383,   897,   802,   659,   368,    55,   365,   209,
     108,   672,   110,   663,   112,   113,   933,   115,   352,     3,
     118,   609,   612,   762,   888,   826,   272,   829,   548,   892,
     569,   185,    -1,   950,    -1,   483,    -1,   828,    -1,    -1,
      11,    12,   925,    -1,    -1,    -1,    -1,   145,    -1,    20,
     276,    -1,   935,    24,    -1,    -1,   154,    28,    29,    30,
      -1,    -1,   945,    34,    35,    36,    37,    38,    39,    40,
      41,    42,   955,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    59,    60,
      61,    62,    63,    -1,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    -1,    89,    90,
      -1,    -1,    -1,    94,    95,    -1,    -1,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,   110,
      -1,   112,   113,    -1,   115,    -1,     3,   118,     5,     6,
       7,     8,     9,    10,    11,    -1,    13,    -1,    -1,    -1,
      17,    18,    19,    -1,    21,    22,    23,    -1,    25,    26,
      -1,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    -1,    84,    85,    86,
      87,    -1,    89,    -1,    -1,    -1,    -1,    94,    95,    -1,
      -1,    98,    99,   100,   101,   102,   103,    28,    29,    30,
      -1,   108,    -1,   110,    -1,    -1,   113,    -1,   115,   116,
     117,    -1,   119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    30,    84,    85,    86,    87,    -1,    89,    -1,
      -1,    -1,    -1,    94,    95,    -1,    -1,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,   110,
      58,    -1,   113,    -1,   115,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    30,    -1,    85,    86,    87,
      -1,    89,    -1,    91,    92,    93,    94,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    -1,   115,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    30,
      85,    86,    87,    -1,    89,    90,    -1,    -1,    -1,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,    -1,    -1,    -1,    58,    -1,    -1,
     115,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    30,    85,    86,    87,    -1,    89,    -1,
      -1,    -1,    -1,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,
      -1,    58,    -1,    -1,   115,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    30,    -1,    85,    86,
      87,    -1,    89,    -1,    -1,    -1,    -1,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,    -1,    -1,    58,    -1,    -1,    -1,   115,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    -1,
      -1,    85,    86,    87,    -1,    89,    -1,    -1,    -1,    -1,
      94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   115
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   163,   164,   165,     0,   164,     3,     5,     6,     7,
       8,     9,    10,    11,    13,    17,    18,    19,    21,    22,
      23,    25,    26,    28,    29,    30,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    84,    85,    86,    87,    89,    94,    95,    98,
      99,   100,   101,   102,   103,   108,   110,   113,   115,   116,
     117,   119,   166,   167,   168,   172,   176,   180,   184,   188,
     191,   197,   199,   205,   209,   213,   217,   221,   225,   226,
     230,   234,   238,   242,   249,   259,   263,   270,   271,   272,
     273,   285,   287,   288,   289,   290,   291,   292,   293,   304,
     310,   325,   326,   327,   331,   332,   335,   337,   338,   367,
     391,   395,   399,   140,    32,   140,   169,    32,   140,   173,
      32,   140,   177,    30,   140,   181,    30,   140,   185,    27,
     308,   309,   308,    30,   140,   305,   308,   308,   308,   308,
     308,   308,   308,    58,   399,   140,    31,   140,   260,    31,
     140,   264,   308,   308,   308,    30,    31,   140,   250,   257,
     140,   243,   257,   325,   325,    69,    57,    58,   312,    72,
      67,    68,    69,    72,   326,   399,   395,    30,   218,   210,
      30,   140,   214,    32,   140,   227,   395,   325,   155,   140,
     231,   257,   140,   235,   257,   140,   239,   325,   155,    87,
      26,   336,    30,   108,   398,   126,   139,   189,   190,   126,
     170,   171,   126,   174,   175,   126,   178,   179,   126,   182,
     183,   126,   186,   187,   308,    27,    29,   122,   128,   306,
     307,   308,   325,   151,   372,   222,   126,   127,   261,   262,
     126,   265,   266,   123,   124,   126,   134,   135,   136,   137,
     138,   252,   253,   144,   254,   251,   126,   244,   245,   144,
     246,   339,   333,    30,   313,    69,   398,    30,   140,   206,
     144,   144,   126,   215,   216,   125,   131,   132,   133,   228,
     229,   140,   140,   192,   395,   400,   126,   232,   233,   126,
     236,   237,   126,   240,   241,   400,   327,   399,   338,   140,
     372,   140,   143,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   158,   160,   161,   368,   150,   152,   397,
     143,   143,   141,   142,   143,   141,   142,   143,   141,   142,
     143,   141,   142,   143,   141,   142,   143,   141,   142,   143,
     143,   141,   142,    30,   373,   374,   198,    30,   147,   223,
     224,   313,   143,   143,   141,   142,   143,   141,   142,   143,
     143,   143,   143,   143,   143,   143,   143,   141,   142,     4,
      12,   221,   225,   255,   256,   295,   299,   143,   141,   142,
     221,   225,   247,   248,   299,   157,   341,   341,   372,   397,
     372,   150,   144,   207,    30,   219,   220,    30,   211,   212,
     143,   141,   142,   143,   143,   143,   143,   141,   142,    91,
      92,    93,   380,   381,   382,   395,   396,   325,   372,   142,
     156,   143,   141,   142,   143,   141,   142,   143,   141,   142,
     156,   372,   380,   144,   392,   141,   140,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   155,   143,   156,   159,
     143,   143,   140,    89,    30,    32,   355,    96,   190,    32,
     171,    32,   175,    32,   179,    30,   183,    30,   187,    30,
      96,   307,   143,   142,   151,   144,   201,    30,    88,   141,
     148,    31,   105,   106,   328,   262,    31,   266,    32,    32,
     257,   328,   328,   328,   328,    30,   253,   140,   296,    32,
     140,   300,   145,   256,   257,   245,   145,   248,    59,    60,
      61,   342,   343,   344,   372,   372,   311,   146,    30,   167,
     208,   337,   146,   145,   220,   145,   212,   355,   216,    32,
      32,    32,    32,   229,   313,   313,   313,   141,   142,   313,
     143,   319,   141,   144,   193,   395,   257,   233,   257,   237,
     325,   241,   200,   141,    14,    15,    16,   221,   225,   393,
     394,   146,   395,   143,   143,   380,    30,    32,    96,   257,
     375,   374,    24,    46,    47,    48,    90,   202,   203,   204,
     221,   225,   273,   282,   286,   310,    30,   147,   313,   129,
     297,   298,   120,   131,   301,   302,   308,   146,   146,   142,
     325,   340,   334,   144,   299,   303,   141,   145,   167,   372,
     372,   372,    89,   369,   382,   372,   140,   320,   323,   324,
     111,   194,   195,   196,   221,   225,   273,   201,   369,   308,
     308,   308,   145,   394,    14,   267,   141,   141,   157,   308,
     308,   308,   308,   395,   145,   203,    30,   143,   141,   142,
     143,   143,   141,   142,   343,   144,   345,   345,    30,   221,
     225,   314,   315,   316,   140,   319,   319,   114,   370,   325,
     148,   149,   150,   151,   152,   153,   321,   147,   148,   149,
     150,   152,   154,   322,   308,   145,   195,   370,   308,    15,
     268,   146,   369,    96,   258,    30,   146,    32,   298,    32,
      32,   302,    11,    20,    30,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    47,    48,    49,    59,    60,    61,
      62,    63,    65,    83,    90,   112,   118,   154,   191,   205,
     221,   225,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   286,   294,   299,   310,   331,   332,
     335,   338,   346,   347,   348,   357,   359,   360,   362,   367,
     383,   386,   388,   389,   391,   146,   146,   143,   318,   145,
     316,   400,   109,   401,   141,   323,    32,    96,    97,   104,
     107,   144,   325,   328,   329,   399,   146,   401,   308,    16,
     269,   370,   148,   140,   308,   308,   140,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,    64,
     356,   356,   356,   157,   384,   385,   358,   390,   387,   361,
     140,   349,    30,   145,   347,   323,   372,   146,   141,   140,
     143,   371,   145,   140,   371,   308,   401,   258,   380,   380,
     157,   157,   157,    83,   386,   386,   359,   367,   395,   388,
      30,   362,   121,   126,   130,   350,   351,   144,   352,   140,
     142,   317,   372,   325,   402,    96,   372,   320,   330,   372,
     371,   141,   141,    30,   143,   143,   143,   141,   142,   221,
     225,   299,   353,   354,   141,   146,   141,   142,   158,   365,
     141,   142,   365,   372,   369,   401,    30,   355,    30,   351,
     145,   354,   401,   303,   325,   366,   146,   320,   146,   365,
     401,   372,   146,   371,   395,    45,   378,   303,   146,   372,
     158,   363,   372,   140,   308,    44,   377,   378,   378,   365,
     364,   146,   146,   380,   308,    33,   379,   377,   377,   146,
     140,   303,   378,   141,   308,    43,   376,   379,   379,   303,
     380,   378,   377,   159,   308,   376,   376,   378,   141,   377,
     379,   377,   159
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 557 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * We don't do these in parserEOF() because the parser is reading
             * ahead and that would be too early.
             */

            if (previousFile != NULL)
            {
                handleEOF();

                if (currentContext.prevmod != NULL)
                    handleEOM();

                free(previousFile);
                previousFile = NULL;
            }
    }
    break;

  case 49:
#line 621 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope == NULL)
                    yyerror("%TypeHeaderCode can only be used in a namespace, class or mapped type");

                appendCodeBlock(&scope->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 50:
#line 634 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                currentModule->defdocstringfmt = convertFormat((yyvsp[(2) - (2)].defdocstringfmt).name);
        }
    break;

  case 51:
#line 640 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defdocstringfmt).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 52:
#line 645 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt) = (yyvsp[(2) - (3)].defdocstringfmt);
        }
    break;

  case 54:
#line 651 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt) = (yyvsp[(1) - (3)].defdocstringfmt);

            switch ((yyvsp[(3) - (3)].defdocstringfmt).token)
            {
            case TK_NAME: (yyval.defdocstringfmt).name = (yyvsp[(3) - (3)].defdocstringfmt).name; break;
            }
        }
    break;

  case 55:
#line 661 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt).token = TK_NAME;

            (yyval.defdocstringfmt).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 56:
#line 668 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                currentModule->defdocstringsig = convertSignature((yyvsp[(2) - (2)].defdocstringsig).name);
        }
    break;

  case 57:
#line 674 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defdocstringsig).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 58:
#line 679 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig) = (yyvsp[(2) - (3)].defdocstringsig);
        }
    break;

  case 60:
#line 685 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig) = (yyvsp[(1) - (3)].defdocstringsig);

            switch ((yyvsp[(3) - (3)].defdocstringsig).token)
            {
            case TK_NAME: (yyval.defdocstringsig).name = (yyvsp[(3) - (3)].defdocstringsig).name; break;
            }
        }
    break;

  case 61:
#line 695 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig).token = TK_NAME;

            (yyval.defdocstringsig).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 62:
#line 702 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if ((currentModule->encoding = convertEncoding((yyvsp[(2) - (2)].defencoding).name)) == no_type)
                    yyerror("The %DefaultEncoding name must be one of \"ASCII\", \"Latin-1\", \"UTF-8\" or \"None\"");
            }
        }
    break;

  case 63:
#line 711 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defencoding).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 64:
#line 716 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding) = (yyvsp[(2) - (3)].defencoding);
        }
    break;

  case 66:
#line 722 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding) = (yyvsp[(1) - (3)].defencoding);

            switch ((yyvsp[(3) - (3)].defencoding).token)
            {
            case TK_NAME: (yyval.defencoding).name = (yyvsp[(3) - (3)].defencoding).name; break;
            }
        }
    break;

  case 67:
#line 732 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding).token = TK_NAME;

            (yyval.defencoding).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 68:
#line 739 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * Note that %Plugin is internal in SIP v4.  The current thinking
             * is that it won't be needed for SIP v5.
             */

            if (notSkipping())
                appendString(&currentSpec->plugins, (yyvsp[(2) - (2)].plugin).name);
        }
    break;

  case 69:
#line 750 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.plugin).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 70:
#line 755 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin) = (yyvsp[(2) - (3)].plugin);
        }
    break;

  case 72:
#line 761 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin) = (yyvsp[(1) - (3)].plugin);

            switch ((yyvsp[(3) - (3)].plugin).token)
            {
            case TK_NAME: (yyval.plugin).name = (yyvsp[(3) - (3)].plugin).name; break;
            }
        }
    break;

  case 73:
#line 771 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin).token = TK_NAME;

            (yyval.plugin).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 74:
#line 778 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].veh).name == NULL)
                yyerror("%VirtualErrorHandler must have a 'name' argument");

            if (notSkipping())
            {
                virtErrorHandler *veh, **tailp;

                /* Check there isn't already a handler with the same name. */
                for (tailp = &currentSpec->errorhandlers; (veh = *tailp) != NULL; tailp = &veh->next)
                    if (strcmp(veh->name, (yyvsp[(2) - (3)].veh).name) == 0)
                        break;

                if (veh != NULL)
                    yyerror("A virtual error handler with that name has already been defined");

                veh = sipMalloc(sizeof (virtErrorHandler));

                veh->name = (yyvsp[(2) - (3)].veh).name;
                appendCodeBlock(&veh->code, (yyvsp[(3) - (3)].codeb));
                veh->mod = currentModule;
                veh->index = -1;
                veh->next = NULL;

                *tailp = veh;
            }
        }
    break;

  case 75:
#line 807 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.veh).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 76:
#line 812 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh) = (yyvsp[(2) - (3)].veh);
        }
    break;

  case 78:
#line 818 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh) = (yyvsp[(1) - (3)].veh);

            switch ((yyvsp[(3) - (3)].veh).token)
            {
            case TK_NAME: (yyval.veh).name = (yyvsp[(3) - (3)].veh).name; break;
            }
        }
    break;

  case 79:
#line 828 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh).token = TK_NAME;

            (yyval.veh).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 80:
#line 835 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                apiVersionRangeDef *avd;

                if (findAPI(currentSpec, (yyvsp[(3) - (4)].api).name) != NULL)
                    yyerror("The API name in the %API directive has already been defined");

                if ((yyvsp[(3) - (4)].api).version < 1)
                    yyerror("The version number in the %API directive must be greater than or equal to 1");

                avd = sipMalloc(sizeof (apiVersionRangeDef));

                avd->api_name = cacheName(currentSpec, (yyvsp[(3) - (4)].api).name);
                avd->from = (yyvsp[(3) - (4)].api).version;
                avd->to = -1;

                avd->next = currentModule->api_versions;
                currentModule->api_versions = avd;

                if (inMainModule())
                    setIsUsedName(avd->api_name);
            }
        }
    break;

  case 82:
#line 862 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.api) = (yyvsp[(1) - (3)].api);

            switch ((yyvsp[(3) - (3)].api).token)
            {
            case TK_NAME: (yyval.api).name = (yyvsp[(3) - (3)].api).name; break;
            case TK_VERSION: (yyval.api).version = (yyvsp[(3) - (3)].api).version; break;
            }
        }
    break;

  case 83:
#line 873 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.api).token = TK_NAME;

            (yyval.api).name = (yyvsp[(3) - (3)].text);
            (yyval.api).version = 0;
        }
    break;

  case 84:
#line 879 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.api).token = TK_VERSION;

            (yyval.api).name = NULL;
            (yyval.api).version = (yyvsp[(3) - (3)].number);
        }
    break;

  case 85:
#line 887 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                static const char *annos[] = {
                    "Default",
                    "PyName",
                    NULL
                };

                exceptionDef *xd;
                const char *pyname;

                checkAnnos(&(yyvsp[(4) - (5)].optflags), annos);

                if (currentSpec->genc)
                    yyerror("%Exception not allowed in a C module");

                if ((yyvsp[(5) - (5)].exception).raise_code == NULL)
                    yyerror("%Exception must have a %RaiseCode sub-directive");

                pyname = getPythonName(currentModule, &(yyvsp[(4) - (5)].optflags), scopedNameTail((yyvsp[(2) - (5)].scpvalp)));

                checkAttributes(currentSpec, currentModule, NULL, NULL,
                        pyname, FALSE);

                xd = findException(currentSpec, (yyvsp[(2) - (5)].scpvalp), TRUE);

                if (xd->cd != NULL)
                    yyerror("%Exception name has already been seen as a class name - it must be defined before being used");

                if (xd->iff->module != NULL)
                    yyerror("The %Exception has already been defined");

                /* Complete the definition. */
                xd->iff->module = currentModule;
                appendCodeBlock(&xd->iff->hdrcode, (yyvsp[(5) - (5)].exception).type_header_code);
                xd->pyname = pyname;
                xd->bibase = (yyvsp[(3) - (5)].exceptionbase).bibase;
                xd->base = (yyvsp[(3) - (5)].exceptionbase).base;
                appendCodeBlock(&xd->raisecode, (yyvsp[(5) - (5)].exception).raise_code);

                if (getOptFlag(&(yyvsp[(4) - (5)].optflags), "Default", bool_flag) != NULL)
                    currentModule->defexception = xd;
            }
        }
    break;

  case 86:
#line 934 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exceptionbase).bibase = NULL;
            (yyval.exceptionbase).base = NULL;
        }
    break;

  case 87:
#line 938 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            exceptionDef *xd;

            (yyval.exceptionbase).bibase = NULL;
            (yyval.exceptionbase).base = NULL;

            /* See if it is a defined exception. */
            for (xd = currentSpec->exceptions; xd != NULL; xd = xd->next)
                if (compareScopedNames(xd->iff->fqcname, (yyvsp[(2) - (3)].scpvalp)) == 0)
                {
                    (yyval.exceptionbase).base = xd;
                    break;
                }

            if (xd == NULL && (yyvsp[(2) - (3)].scpvalp)->next == NULL && strncmp((yyvsp[(2) - (3)].scpvalp)->name, "SIP_", 4) == 0)
            {
                /* See if it is a builtin exception. */

                static char *builtins[] = {
                    "BaseException",
                    "Exception",
                    "StopIteration",
                    "GeneratorExit",
                    "ArithmeticError",
                    "LookupError",
                    "StandardError",                /* Python v2. */

                    "AssertionError",
                    "AttributeError",
                    "BufferError",
                    "EOFError",
                    "FloatingPointError",
                    "OSError",
                    "ImportError",
                    "IndexError",
                    "KeyError",
                    "KeyboardInterrupt",
                    "MemoryError",
                    "NameError",
                    "OverflowError",
                    "RuntimeError",
                    "NotImplementedError",
                    "SyntaxError",
                    "IndentationError",
                    "TabError",
                    "ReferenceError",
                    "SystemError",
                    "SystemExit",
                    "TypeError",
                    "UnboundLocalError",
                    "UnicodeError",
                    "UnicodeEncodeError",
                    "UnicodeDecodeError",
                    "UnicodeTranslateError",
                    "ValueError",
                    "ZeroDivisionError",
                    "EnvironmentError",             /* Python v2. */
                    "IOError",                      /* Python v2. */
                    "WindowsError",                 /* Python v2. */
                    "VMSError",                     /* Python v2. */

                    "BlockingIOError",
                    "BrokenPipeError",
                    "ChildProcessError",
                    "ConnectionError",
                    "ConnectionAbortedError",
                    "ConnectionRefusedError",
                    "ConnectionResetError",
                    "FileExistsError",
                    "FileNotFoundError",
                    "InterruptedError",
                    "IsADirectoryError",
                    "NotADirectoryError",
                    "PermissionError",
                    "ProcessLookupError",
                    "TimeoutError",

                    "Warning",
                    "UserWarning",
                    "DeprecationWarning",
                    "PendingDeprecationWarning",
                    "SyntaxWarning",
                    "RuntimeWarning",
                    "FutureWarning",
                    "ImportWarning",
                    "UnicodeWarning",
                    "BytesWarning",
                    "ResourceWarning",

                    NULL
                };

                char **cp;

                for (cp = builtins; *cp != NULL; ++cp)
                    if (strcmp((yyvsp[(2) - (3)].scpvalp)->name + 4, *cp) == 0)
                    {
                        (yyval.exceptionbase).bibase = *cp;
                        break;
                    }
            }

            if ((yyval.exceptionbase).bibase == NULL && (yyval.exceptionbase).base == NULL)
                yyerror("Unknown exception base type");
        }
    break;

  case 88:
#line 1045 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception) = (yyvsp[(2) - (4)].exception);
        }
    break;

  case 90:
#line 1051 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception) = (yyvsp[(1) - (2)].exception);

            switch ((yyvsp[(2) - (2)].exception).token)
            {
            case TK_RAISECODE: (yyval.exception).raise_code = (yyvsp[(2) - (2)].exception).raise_code; break;
            case TK_TYPEHEADERCODE: (yyval.exception).type_header_code = (yyvsp[(2) - (2)].exception).type_header_code; break;
            }
        }
    break;

  case 91:
#line 1062 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception).token = TK_IF;
        }
    break;

  case 92:
#line 1065 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception).token = TK_END;
        }
    break;

  case 93:
#line 1068 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.exception).token = TK_RAISECODE;
                (yyval.exception).raise_code = (yyvsp[(1) - (1)].codeb);
            }
            else
            {
                (yyval.exception).token = 0;
                (yyval.exception).raise_code = NULL;
            }

            (yyval.exception).type_header_code = NULL;
        }
    break;

  case 94:
#line 1082 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.exception).token = TK_TYPEHEADERCODE;
                (yyval.exception).type_header_code = (yyvsp[(1) - (1)].codeb);
            }
            else
            {
                (yyval.exception).token = 0;
                (yyval.exception).type_header_code = NULL;
            }

            (yyval.exception).raise_code = NULL;
        }
    break;

  case 95:
#line 1098 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 96:
#line 1103 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                static const char *annos[] = {
                    "AllowNone",
                    "API",
                    "DocType",
                    "NoRelease",
                    "PyName",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    "TypeHintValue",
                    NULL
                };

                checkAnnos(&(yyvsp[(3) - (3)].optflags), annos);

                currentMappedType = newMappedType(currentSpec, &(yyvsp[(2) - (3)].memArg), &(yyvsp[(3) - (3)].optflags));
            }
        }
    break;

  case 98:
#line 1126 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                static const char *annos[] = {
                    "AllowNone",
                    "DocType",
                    "NoRelease",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    "TypeHintValue",
                    NULL
                };

                int a;
                mappedTypeTmplDef *mtt;
                ifaceFileDef *iff;

                checkAnnos(&(yyvsp[(4) - (4)].optflags), annos);

                if (currentSpec->genc)
                    yyerror("%MappedType templates not allowed in a C module");

                /*
                 * Check the template arguments are basic types or simple
                 * names.
                 */
                for (a = 0; a < (yyvsp[(1) - (4)].signature).nrArgs; ++a)
                {
                    argDef *ad = &(yyvsp[(1) - (4)].signature).args[a];

                    if (ad->atype == defined_type && ad->u.snd->next != NULL)
                        yyerror("%MappedType template arguments must be simple names");
                }

                if ((yyvsp[(3) - (4)].memArg).atype != template_type)
                    yyerror("%MappedType template must map a template type");

                (yyvsp[(3) - (4)].memArg).u.td->fqname  = fullyQualifiedName((yyvsp[(3) - (4)].memArg).u.td->fqname);

                /* Check a template hasn't already been provided. */
                for (mtt = currentSpec->mappedtypetemplates; mtt != NULL; mtt = mtt->next)
                    if (compareScopedNames(mtt->mt->type.u.td->fqname, (yyvsp[(3) - (4)].memArg).u.td->fqname ) == 0 && sameTemplateSignature(&mtt->mt->type.u.td->types, &(yyvsp[(3) - (4)].memArg).u.td->types, TRUE))
                        yyerror("%MappedType template for this type has already been defined");

                (yyvsp[(3) - (4)].memArg).nrderefs = 0;
                (yyvsp[(3) - (4)].memArg).argflags = 0;

                mtt = sipMalloc(sizeof (mappedTypeTmplDef));

                mtt->sig = (yyvsp[(1) - (4)].signature);
                mtt->mt = allocMappedType(currentSpec, &(yyvsp[(3) - (4)].memArg));
                mappedTypeAnnos(mtt->mt, &(yyvsp[(4) - (4)].optflags));
                mtt->next = currentSpec->mappedtypetemplates;

                currentSpec->mappedtypetemplates = mtt;

                currentMappedType = mtt->mt;

                /* Create a dummy interface file. */
                iff = sipMalloc(sizeof (ifaceFileDef));
                iff->hdrcode = NULL;
                mtt->mt->iff = iff;
            }
        }
    break;

  case 100:
#line 1193 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType->convfromcode == NULL)
                    yyerror("%MappedType must have a %ConvertFromTypeCode directive");

                if (currentMappedType->convtocode == NULL)
                    yyerror("%MappedType must have a %ConvertToTypeCode directive");

                currentMappedType = NULL;
            }
        }
    break;

  case 105:
#line 1213 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 106:
#line 1217 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->typecode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 107:
#line 1221 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType->convfromcode != NULL)
                    yyerror("%MappedType has more than one %ConvertFromTypeCode directive");

                appendCodeBlock(&currentMappedType->convfromcode, (yyvsp[(2) - (2)].codeb));
            }
        }
    break;

  case 108:
#line 1230 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType->convtocode != NULL)
                    yyerror("%MappedType has more than one %ConvertToTypeCode directive");

                appendCodeBlock(&currentMappedType->convtocode, (yyvsp[(2) - (2)].codeb));
            }
        }
    break;

  case 109:
#line 1239 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType->instancecode != NULL)
                    yyerror("%MappedType has more than one %InstanceCode directive");

                appendCodeBlock(&currentMappedType->instancecode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 112:
#line 1252 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                applyTypeFlags(currentModule, &(yyvsp[(2) - (14)].memArg), &(yyvsp[(9) - (14)].optflags));

                (yyvsp[(5) - (14)].signature).result = (yyvsp[(2) - (14)].memArg);

                newFunction(currentSpec, currentModule, NULL, NULL,
                        currentMappedType, 0, TRUE, FALSE, FALSE, FALSE, (yyvsp[(3) - (14)].text),
                        &(yyvsp[(5) - (14)].signature), (yyvsp[(7) - (14)].number), FALSE, &(yyvsp[(9) - (14)].optflags), (yyvsp[(14) - (14)].codeb), NULL, NULL, (yyvsp[(8) - (14)].throwlist), (yyvsp[(10) - (14)].optsignature), (yyvsp[(12) - (14)].docstr),
                        FALSE, (yyvsp[(13) - (14)].codeb));
            }
        }
    break;

  case 113:
#line 1267 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("namespace definition not allowed in a C module");

            if (notSkipping())
            {
                classDef *ns, *c_scope;
                ifaceFileDef *scope;

                if ((c_scope = currentScope()) != NULL)
                    scope = c_scope->iff;
                else
                    scope = NULL;

                ns = newClass(currentSpec, namespace_iface, NULL,
                        text2scopedName(scope, (yyvsp[(2) - (2)].text)), NULL, NULL, NULL, NULL);

                pushScope(ns);

                sectionFlags = 0;
            }
        }
    break;

  case 114:
#line 1288 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (inMainModule())
                {
                    classDef *ns = currentScope();

                    setIsUsedName(ns->iff->name);
                    setIsUsedName(ns->pyname);
                }

                popScope();
            }
        }
    break;

  case 119:
#line 1312 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                qualDef *qd;

                for (qd = currentModule->qualifiers; qd != NULL; qd = qd->next)
                    if (qd->qtype == platform_qualifier)
                        yyerror("%Platforms has already been defined for this module");
            }
        }
    break;

  case 120:
#line 1322 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                qualDef *qd;
                int nrneeded;

                /* Check that exactly one platform in the set was requested. */
                nrneeded = 0;

                for (qd = currentModule->qualifiers; qd != NULL; qd = qd->next)
                    if (qd->qtype == platform_qualifier && selectedQualifier(neededQualifiers, qd))
                        ++nrneeded;

                if (nrneeded > 1)
                    yyerror("No more than one of these %Platforms must be specified with the -t flag");
            }
        }
    break;

  case 123:
#line 1345 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, -1, 0, notSkipping(), (yyvsp[(1) - (1)].text),
                    platform_qualifier);
        }
    break;

  case 124:
#line 1351 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, -1, 0, notSkipping(), (yyvsp[(2) - (2)].feature).name,
                    feature_qualifier);
        }
    break;

  case 125:
#line 1357 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.feature).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 126:
#line 1362 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature) = (yyvsp[(2) - (3)].feature);
        }
    break;

  case 128:
#line 1368 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature) = (yyvsp[(1) - (3)].feature);

            switch ((yyvsp[(3) - (3)].feature).token)
            {
            case TK_NAME: (yyval.feature).name = (yyvsp[(3) - (3)].feature).name; break;
            }
        }
    break;

  case 129:
#line 1378 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature).token = TK_NAME;

            (yyval.feature).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 130:
#line 1385 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            currentTimelineOrder = 0;
        }
    break;

  case 131:
#line 1388 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                qualDef *qd;
                int nrneeded;

                /*
                 * Check that exactly one time slot in the set was requested.
                 */
                nrneeded = 0;

                for (qd = currentModule->qualifiers; qd != NULL; qd = qd->next)
                    if (qd->qtype == time_qualifier && selectedQualifier(neededQualifiers, qd))
                        ++nrneeded;

                if (nrneeded > 1)
                    yyerror("At most one of this %Timeline must be specified with the -t flag");

                currentModule->nrtimelines++;
            }
        }
    break;

  case 134:
#line 1415 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, currentModule->nrtimelines,
                    currentTimelineOrder++, TRUE, (yyvsp[(1) - (1)].text), time_qualifier);
        }
    break;

  case 135:
#line 1421 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            currentPlatforms = NULL;
        }
    break;

  case 136:
#line 1423 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (stackPtr >= MAX_NESTED_IF)
                yyerror("Internal error: increase the value of MAX_NESTED_IF");

            /* Nested %Ifs are implicit logical ands. */

            if (stackPtr > 0)
                (yyvsp[(4) - (5)].boolean) = ((yyvsp[(4) - (5)].boolean) && skipStack[stackPtr - 1]);

            skipStack[stackPtr] = (yyvsp[(4) - (5)].boolean);

            platformStack[stackPtr] = currentPlatforms;

            ++stackPtr;
        }
    break;

  case 137:
#line 1440 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(1) - (1)].text), FALSE);
        }
    break;

  case 138:
#line 1443 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(2) - (2)].text), TRUE);
        }
    break;

  case 139:
#line 1446 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(3) - (3)].text), FALSE) || (yyvsp[(1) - (3)].boolean));
        }
    break;

  case 140:
#line 1449 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(4) - (4)].text), TRUE) || (yyvsp[(1) - (4)].boolean));
        }
    break;

  case 142:
#line 1455 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = timePeriod((yyvsp[(1) - (3)].text), (yyvsp[(3) - (3)].text));
        }
    break;

  case 143:
#line 1460 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (stackPtr-- <= 0)
                yyerror("Too many %End directives");

            currentPlatforms = (stackPtr == 0 ? NULL : platformStack[stackPtr - 1]);
        }
    break;

  case 144:
#line 1468 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (2)].license).type == NULL)
                yyerror("%License must have a 'type' argument");

            if (notSkipping())
            {
                currentModule->license = sipMalloc(sizeof (licenseDef));

                currentModule->license->type = (yyvsp[(2) - (2)].license).type;
                currentModule->license->licensee = (yyvsp[(2) - (2)].license).licensee;
                currentModule->license->sig = (yyvsp[(2) - (2)].license).signature;
                currentModule->license->timestamp = (yyvsp[(2) - (2)].license).timestamp;
            }
        }
    break;

  case 145:
#line 1484 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 146:
#line 1492 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).type = (yyvsp[(1) - (1)].text);
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 147:
#line 1498 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license) = (yyvsp[(2) - (3)].license);
        }
    break;

  case 149:
#line 1504 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license) = (yyvsp[(1) - (3)].license);

            switch ((yyvsp[(3) - (3)].license).token)
            {
            case TK_TYPE: (yyval.license).type = (yyvsp[(3) - (3)].license).type; break;
            case TK_LICENSEE: (yyval.license).licensee = (yyvsp[(3) - (3)].license).licensee; break;
            case TK_SIGNATURE: (yyval.license).signature = (yyvsp[(3) - (3)].license).signature; break;
            case TK_TIMESTAMP: (yyval.license).timestamp = (yyvsp[(3) - (3)].license).timestamp; break;
            }
        }
    break;

  case 150:
#line 1517 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_NAME;

            (yyval.license).type = (yyvsp[(3) - (3)].text);
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 151:
#line 1525 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_LICENSEE;

            (yyval.license).type = NULL;
            (yyval.license).licensee = (yyvsp[(3) - (3)].text);
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 152:
#line 1533 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_SIGNATURE;

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = (yyvsp[(3) - (3)].text);
            (yyval.license).timestamp = NULL;
        }
    break;

  case 153:
#line 1541 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_TIMESTAMP;

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = (yyvsp[(3) - (3)].text);
        }
    break;

  case 154:
#line 1551 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentModule->defmetatype != NULL)
                    yyerror("%DefaultMetatype has already been defined for this module");

                currentModule->defmetatype = cacheName(currentSpec, (yyvsp[(2) - (2)].defmetatype).name);
            }
        }
    break;

  case 155:
#line 1562 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defmetatype).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 156:
#line 1567 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype) = (yyvsp[(2) - (3)].defmetatype);
        }
    break;

  case 158:
#line 1573 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype) = (yyvsp[(1) - (3)].defmetatype);

            switch ((yyvsp[(3) - (3)].defmetatype).token)
            {
            case TK_NAME: (yyval.defmetatype).name = (yyvsp[(3) - (3)].defmetatype).name; break;
            }
        }
    break;

  case 159:
#line 1583 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype).token = TK_NAME;

            (yyval.defmetatype).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 160:
#line 1590 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if (currentModule->defsupertype != NULL)
                    yyerror("%DefaultSupertype has already been defined for this module");

                currentModule->defsupertype = cacheName(currentSpec, (yyvsp[(2) - (2)].defsupertype).name);
            }
        }
    break;

  case 161:
#line 1601 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defsupertype).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 162:
#line 1606 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype) = (yyvsp[(2) - (3)].defsupertype);
        }
    break;

  case 164:
#line 1612 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype) = (yyvsp[(1) - (3)].defsupertype);

            switch ((yyvsp[(3) - (3)].defsupertype).token)
            {
            case TK_NAME: (yyval.defsupertype).name = (yyvsp[(3) - (3)].defsupertype).name; break;
            }
        }
    break;

  case 165:
#line 1622 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype).token = TK_NAME;

            (yyval.defsupertype).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 166:
#line 1629 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *ns;

                ns = newClass(currentSpec, namespace_iface, NULL,
                        fullyQualifiedName((yyvsp[(2) - (2)].hiddenns).name), NULL, NULL, NULL, NULL);
                setHiddenNamespace(ns);
            }
        }
    break;

  case 167:
#line 1641 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.hiddenns).name = (yyvsp[(1) - (1)].scpvalp);
        }
    break;

  case 168:
#line 1646 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns) = (yyvsp[(2) - (3)].hiddenns);
        }
    break;

  case 170:
#line 1652 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns) = (yyvsp[(1) - (3)].hiddenns);

            switch ((yyvsp[(3) - (3)].hiddenns).token)
            {
            case TK_NAME: (yyval.hiddenns).name = (yyvsp[(3) - (3)].hiddenns).name; break;
            }
        }
    break;

  case 171:
#line 1662 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns).token = TK_NAME;

            (yyval.hiddenns).name = (yyvsp[(3) - (3)].scpvalp);
        }
    break;

  case 172:
#line 1669 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                /* Make sure this is the first mention of a module. */
                if (currentSpec->module != currentModule)
                    yyerror("A %CompositeModule cannot be %Imported");

                if (currentModule->fullname != NULL)
                    yyerror("%CompositeModule must appear before any %Module directive");

                setModuleName(currentSpec, currentModule, (yyvsp[(2) - (3)].compmodule).name);
                currentModule->docstring = (yyvsp[(3) - (3)].compmodule).docstring;

                setIsComposite(currentModule);
            }
        }
    break;

  case 173:
#line 1687 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.compmodule).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 174:
#line 1692 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(2) - (3)].compmodule);
        }
    break;

  case 176:
#line 1698 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(1) - (3)].compmodule);

            switch ((yyvsp[(3) - (3)].compmodule).token)
            {
            case TK_NAME: (yyval.compmodule).name = (yyvsp[(3) - (3)].compmodule).name; break;
            }
        }
    break;

  case 177:
#line 1708 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_NAME;

            (yyval.compmodule).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 178:
#line 1715 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = 0;
            (yyval.compmodule).docstring = NULL;
        }
    break;

  case 179:
#line 1719 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(2) - (4)].compmodule);
        }
    break;

  case 181:
#line 1725 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(1) - (2)].compmodule);

            switch ((yyvsp[(2) - (2)].compmodule).token)
            {
            case TK_DOCSTRING: (yyval.compmodule).docstring = (yyvsp[(2) - (2)].compmodule).docstring; break;
            }
        }
    break;

  case 182:
#line 1735 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_IF;
        }
    break;

  case 183:
#line 1738 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_END;
        }
    break;

  case 184:
#line 1741 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.compmodule).token = TK_DOCSTRING;
                (yyval.compmodule).docstring = (yyvsp[(1) - (1)].docstr);
            }
            else
            {
                (yyval.compmodule).token = 0;
                (yyval.compmodule).docstring = NULL;
            }
        }
    break;

  case 185:
#line 1755 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].module).name == NULL)
                yyerror("%Module must have a 'name' argument");

            if (notSkipping())
                currentModule = configureModule(currentSpec, currentModule,
                        currentContext.filename, (yyvsp[(2) - (3)].module).name, (yyvsp[(2) - (3)].module).c_module,
                        (yyvsp[(2) - (3)].module).kwargs, (yyvsp[(2) - (3)].module).use_arg_names, (yyvsp[(2) - (3)].module).use_limited_api,
                        (yyvsp[(2) - (3)].module).call_super_init, (yyvsp[(2) - (3)].module).all_raise_py_exc,
                        (yyvsp[(2) - (3)].module).def_error_handler, (yyvsp[(3) - (3)].module).docstring);
        }
    break;

  case 186:
#line 1768 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {resetLexerState();}
    break;

  case 187:
#line 1768 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = (yyvsp[(1) - (2)].text);
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 188:
#line 1778 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(2) - (3)].module);
        }
    break;

  case 190:
#line 1784 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(1) - (3)].module);

            switch ((yyvsp[(3) - (3)].module).token)
            {
            case TK_KWARGS: (yyval.module).kwargs = (yyvsp[(3) - (3)].module).kwargs; break;
            case TK_LANGUAGE: (yyval.module).c_module = (yyvsp[(3) - (3)].module).c_module; break;
            case TK_NAME: (yyval.module).name = (yyvsp[(3) - (3)].module).name; break;
            case TK_USEARGNAMES: (yyval.module).use_arg_names = (yyvsp[(3) - (3)].module).use_arg_names; break;
            case TK_USELIMITEDAPI: (yyval.module).use_limited_api = (yyvsp[(3) - (3)].module).use_limited_api; break;
            case TK_ALLRAISEPYEXC: (yyval.module).all_raise_py_exc = (yyvsp[(3) - (3)].module).all_raise_py_exc; break;
            case TK_CALLSUPERINIT: (yyval.module).call_super_init = (yyvsp[(3) - (3)].module).call_super_init; break;
            case TK_DEFERRORHANDLER: (yyval.module).def_error_handler = (yyvsp[(3) - (3)].module).def_error_handler; break;
            }
        }
    break;

  case 191:
#line 1801 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_KWARGS;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = convertKwArgs((yyvsp[(3) - (3)].text));
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 192:
#line 1813 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_LANGUAGE;

            if (strcmp((yyvsp[(3) - (3)].text), "C++") == 0)
                (yyval.module).c_module = FALSE;
            else if (strcmp((yyvsp[(3) - (3)].text), "C") == 0)
                (yyval.module).c_module = TRUE;
            else
                yyerror("%Module 'language' argument must be either \"C++\" or \"C\"");

            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 193:
#line 1831 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_NAME;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = (yyvsp[(3) - (3)].text);
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 194:
#line 1843 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_USEARGNAMES;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = (yyvsp[(3) - (3)].boolean);
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 195:
#line 1855 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_USELIMITEDAPI;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = (yyvsp[(3) - (3)].boolean);
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 196:
#line 1867 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_ALLRAISEPYEXC;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = (yyvsp[(3) - (3)].boolean);
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 197:
#line 1879 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_CALLSUPERINIT;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = (yyvsp[(3) - (3)].boolean);
            (yyval.module).def_error_handler = NULL;
        }
    break;

  case 198:
#line 1891 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_DEFERRORHANDLER;

            (yyval.module).c_module = FALSE;
            (yyval.module).kwargs = defaultKwArgs;
            (yyval.module).name = NULL;
            (yyval.module).use_arg_names = FALSE;
            (yyval.module).use_limited_api = FALSE;
            (yyval.module).all_raise_py_exc = FALSE;
            (yyval.module).call_super_init = -1;
            (yyval.module).def_error_handler = (yyvsp[(3) - (3)].text);
        }
    break;

  case 199:
#line 1905 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = 0;
            (yyval.module).docstring = NULL;
        }
    break;

  case 200:
#line 1909 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(2) - (4)].module);
        }
    break;

  case 202:
#line 1915 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(1) - (2)].module);

            switch ((yyvsp[(2) - (2)].module).token)
            {
            case TK_DOCSTRING: (yyval.module).docstring = (yyvsp[(2) - (2)].module).docstring; break;
            }
        }
    break;

  case 203:
#line 1925 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_IF;
        }
    break;

  case 204:
#line 1928 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_END;
        }
    break;

  case 205:
#line 1931 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_AUTOPYNAME;
        }
    break;

  case 206:
#line 1934 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.module).token = TK_DOCSTRING;
                (yyval.module).docstring = (yyvsp[(1) - (1)].docstr);
            }
            else
            {
                (yyval.module).token = 0;
                (yyval.module).docstring = NULL;
            }
        }
    break;

  case 208:
#line 1949 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * The grammar design is a bit broken and this is the easiest way
             * to allow periods in names.
             */

            char *cp;

            for (cp = (yyvsp[(1) - (1)].text); *cp != '\0'; ++cp)
                if (*cp != '.' && *cp != '_' && !isalnum(*cp))
                    yyerror("Invalid character in name");

            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 209:
#line 1965 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = -1;
        }
    break;

  case 211:
#line 1971 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (2)].include).name == NULL)
                yyerror("%Include must have a 'name' argument");

            if (notSkipping())
                parseFile(NULL, (yyvsp[(2) - (2)].include).name, NULL, (yyvsp[(2) - (2)].include).optional);
        }
    break;

  case 212:
#line 1980 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.include).name = (yyvsp[(1) - (1)].text);
            (yyval.include).optional = FALSE;
        }
    break;

  case 213:
#line 1986 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include) = (yyvsp[(2) - (3)].include);
        }
    break;

  case 215:
#line 1992 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include) = (yyvsp[(1) - (3)].include);

            switch ((yyvsp[(3) - (3)].include).token)
            {
            case TK_NAME: (yyval.include).name = (yyvsp[(3) - (3)].include).name; break;
            case TK_OPTIONAL: (yyval.include).optional = (yyvsp[(3) - (3)].include).optional; break;
            }
        }
    break;

  case 216:
#line 2003 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include).token = TK_NAME;

            (yyval.include).name = (yyvsp[(3) - (3)].text);
            (yyval.include).optional = FALSE;
        }
    break;

  case 217:
#line 2009 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include).token = TK_OPTIONAL;

            (yyval.include).name = NULL;
            (yyval.include).optional = (yyvsp[(3) - (3)].boolean);
        }
    break;

  case 218:
#line 2017 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                newImport((yyvsp[(2) - (2)].import).name);
        }
    break;

  case 219:
#line 2023 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.import).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 220:
#line 2028 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import) = (yyvsp[(2) - (3)].import);
        }
    break;

  case 222:
#line 2034 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import) = (yyvsp[(1) - (3)].import);

            switch ((yyvsp[(3) - (3)].import).token)
            {
            case TK_NAME: (yyval.import).name = (yyvsp[(3) - (3)].import).name; break;
            }
        }
    break;

  case 223:
#line 2044 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import).token = TK_NAME;

            (yyval.import).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 224:
#line 2051 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 225:
#line 2054 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 226:
#line 2059 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 227:
#line 2062 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 228:
#line 2067 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 229:
#line 2070 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 230:
#line 2075 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->copying, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 231:
#line 2081 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentSpec->exphdrcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 232:
#line 2087 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->hdrcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 233:
#line 2093 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 234:
#line 2098 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 235:
#line 2103 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 236:
#line 2108 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 237:
#line 2113 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 238:
#line 2118 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 239:
#line 2123 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 240:
#line 2128 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 241:
#line 2133 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 242:
#line 2138 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 243:
#line 2143 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 244:
#line 2148 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 245:
#line 2153 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->cppcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 246:
#line 2159 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 247:
#line 2164 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->preinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 248:
#line 2170 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->initcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 249:
#line 2176 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->postinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 250:
#line 2182 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 251:
#line 2188 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitpostinccode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 252:
#line 2194 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping() && !inMainModule())
                appendCodeBlock(&currentSpec->exptypehintcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 253:
#line 2200 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->typehintcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 254:
#line 2206 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 255:
#line 2211 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                addAutoPyName(currentModule, (yyvsp[(2) - (2)].autopyname).remove_leading);
        }
    break;

  case 256:
#line 2217 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname) = (yyvsp[(2) - (3)].autopyname);
        }
    break;

  case 258:
#line 2223 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname) = (yyvsp[(1) - (3)].autopyname);

            switch ((yyvsp[(3) - (3)].autopyname).token)
            {
            case TK_REMOVELEADING: (yyval.autopyname).remove_leading = (yyvsp[(3) - (3)].autopyname).remove_leading; break;
            }
        }
    break;

  case 259:
#line 2233 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname).token = TK_REMOVELEADING;

            (yyval.autopyname).remove_leading = (yyvsp[(3) - (3)].text);
        }
    break;

  case 260:
#line 2240 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstr) = sipMalloc(sizeof(docstringDef));

            (yyval.docstr)->signature = (yyvsp[(2) - (3)].docstring).signature;
            (yyval.docstr)->text = (yyvsp[(3) - (3)].codeb)->frag;
            free((yyvsp[(3) - (3)].codeb));

            /* Format the docstring. */
            if ((yyvsp[(2) - (3)].docstring).format == deindented)
            {
                const char *cp;
                char *dp;
                int min_indent, indent, skipping;

                /* Find the common indent. */
                min_indent = -1;
                indent = 0;
                skipping = FALSE;

                for (cp = (yyval.docstr)->text; *cp != '\0'; ++cp)
                {
                    if (skipping)
                    {
                        /*
                         * We have handled the indent and are just looking for
                         * the end of the line.
                         */
                        if (*cp == '\n')
                            skipping = FALSE;
                    }
                    else
                    {
                        if (*cp == ' ')
                        {
                            ++indent;
                        }
                        else if (*cp != '\n')
                        {
                            if (min_indent < 0 || min_indent > indent)
                                min_indent = indent;

                            /* Ignore the remaining characters of the line. */
                            skipping = TRUE;
                        }
                    }
                }

                /* In case the last line doesn't have a trailing newline. */
                if (min_indent < 0 || min_indent > indent)
                    min_indent = indent;

                /*
                 * Go through the text again removing the common indentation.
                 */
                cp = dp = (yyval.docstr)->text;

                while (*cp != '\0')
                {
                    const char *start = cp;
                    int non_blank = FALSE;

                    /* Find the end of the line. */
                    while (*cp != '\n' && *cp != '\0')
                        if (*cp++ != ' ')
                            non_blank = TRUE;

                    /* Find where we are copying from. */
                    if (non_blank)
                    {
                        start += min_indent;

                        while (*start != '\n' && *start != '\0')
                            *dp++ = *start++;
                    }

                    if (*cp == '\n')
                        *dp++ = *cp++;
                }

                *dp = '\0';
            }
        }
    break;

  case 261:
#line 2324 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).format = currentModule->defdocstringfmt;
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 262:
#line 2328 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.docstring).format = convertFormat((yyvsp[(1) - (1)].text));
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 263:
#line 2334 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring) = (yyvsp[(2) - (3)].docstring);
        }
    break;

  case 265:
#line 2340 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring) = (yyvsp[(1) - (3)].docstring);

            switch ((yyvsp[(3) - (3)].docstring).token)
            {
            case TK_FORMAT: (yyval.docstring).format = (yyvsp[(3) - (3)].docstring).format; break;
            case TK_SIGNATURE: (yyval.docstring).signature = (yyvsp[(3) - (3)].docstring).signature; break;
            }
        }
    break;

  case 266:
#line 2351 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).token = TK_FORMAT;

            (yyval.docstring).format = convertFormat((yyvsp[(3) - (3)].text));
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 267:
#line 2357 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).token = TK_SIGNATURE;

            (yyval.docstring).format = currentModule->defdocstringfmt;
            (yyval.docstring).signature = convertSignature((yyvsp[(3) - (3)].text));
        }
    break;

  case 268:
#line 2365 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstr) = NULL;
        }
    break;

  case 270:
#line 2371 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].extract).id == NULL)
                yyerror("%Extract must have an 'id' argument");

            if (notSkipping())
                addExtractPart(currentSpec, (yyvsp[(2) - (3)].extract).id, (yyvsp[(2) - (3)].extract).order, (yyvsp[(3) - (3)].codeb));
        }
    break;

  case 271:
#line 2380 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.extract).id = (yyvsp[(1) - (1)].text);
            (yyval.extract).order = -1;
        }
    break;

  case 272:
#line 2386 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract) = (yyvsp[(2) - (3)].extract);
        }
    break;

  case 274:
#line 2392 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract) = (yyvsp[(1) - (3)].extract);

            switch ((yyvsp[(3) - (3)].extract).token)
            {
            case TK_ID: (yyval.extract).id = (yyvsp[(3) - (3)].extract).id; break;
            case TK_ORDER: (yyval.extract).order = (yyvsp[(3) - (3)].extract).order; break;
            }
        }
    break;

  case 275:
#line 2403 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract).token = TK_ID;

            (yyval.extract).id = (yyvsp[(3) - (3)].text);
            (yyval.extract).order = -1;
        }
    break;

  case 276:
#line 2409 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract).token = TK_ORDER;

            if ((yyvsp[(3) - (3)].number) < 0)
                yyerror("The 'order' of an %Extract directive must not be negative");

            (yyval.extract).id = NULL;
            (yyval.extract).order = (yyvsp[(3) - (3)].number);
        }
    break;

  case 279:
#line 2424 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(1) - (2)].codeb);

            append(&(yyval.codeb)->frag, (yyvsp[(2) - (2)].codeb)->frag);

            free((yyvsp[(2) - (2)].codeb)->frag);
            free((yyvsp[(2) - (2)].codeb));
        }
    break;

  case 280:
#line 2434 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "NoScope",
                    "NoTypeHint",
                    "PyName",
                    NULL
                };

                checkAnnos(&(yyvsp[(4) - (4)].optflags), annos);

                if (sectionFlags != 0 && (sectionFlags & ~(SECT_IS_PUBLIC | SECT_IS_PROT)) != 0)
                    yyerror("Class enums must be in the public or protected sections");

                if (currentSpec->genc && (yyvsp[(2) - (4)].boolean))
                    yyerror("Scoped enums not allowed in a C module");

                currentEnum = newEnum(currentSpec, currentModule,
                        currentMappedType, (yyvsp[(3) - (4)].text), &(yyvsp[(4) - (4)].optflags), sectionFlags, (yyvsp[(2) - (4)].boolean));
            }
        }
    break;

  case 282:
#line 2458 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 283:
#line 2461 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 284:
#line 2464 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 285:
#line 2469 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.text) = NULL;
        }
    break;

  case 286:
#line 2472 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 293:
#line 2487 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "NoTypeHint",
                    "PyName",
                    NULL
                };

                enumMemberDef *emd, **tail;

                checkAnnos(&(yyvsp[(3) - (4)].optflags), annos);

                /* Note that we don't use the assigned value. */
                emd = sipMalloc(sizeof (enumMemberDef));

                emd->pyname = cacheName(currentSpec,
                        getPythonName(currentModule, &(yyvsp[(3) - (4)].optflags), (yyvsp[(1) - (4)].text)));
                emd->cname = (yyvsp[(1) - (4)].text);
                emd->no_typehint = getNoTypeHint(&(yyvsp[(3) - (4)].optflags));
                emd->ed = currentEnum;
                emd->platforms = currentPlatforms;
                emd->next = NULL;

                checkAttributes(currentSpec, currentModule, emd->ed->ecd,
                        emd->ed->emtd, emd->pyname->text, FALSE);

                /* Append to preserve the order. */
                for (tail = &currentEnum->members; *tail != NULL; tail = &(*tail)->next)
                    ;

                *tail = emd;

                if (inMainModule())
                    setIsUsedName(emd->pyname);
            }
        }
    break;

  case 298:
#line 2534 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.valp) = NULL;
        }
    break;

  case 299:
#line 2537 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.valp) = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 301:
#line 2543 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            valueDef *vd;
 
            if ((yyvsp[(1) - (3)].valp) -> vtype == string_value || (yyvsp[(3) - (3)].valp) -> vtype == string_value)
                yyerror("Invalid binary operator for string");
 
            /* Find the last value in the existing expression. */
 
            for (vd = (yyvsp[(1) - (3)].valp); vd -> next != NULL; vd = vd -> next)
                ;
 
            vd -> vbinop = (yyvsp[(2) - (3)].qchar);
            vd -> next = (yyvsp[(3) - (3)].valp);

            (yyval.valp) = (yyvsp[(1) - (3)].valp);
        }
    break;

  case 302:
#line 2561 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 303:
#line 2564 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 304:
#line 2567 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '*';
        }
    break;

  case 305:
#line 2570 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '/';
        }
    break;

  case 306:
#line 2573 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '&';
        }
    break;

  case 307:
#line 2576 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '|';
        }
    break;

  case 308:
#line 2581 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '\0';
        }
    break;

  case 309:
#line 2584 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '!';
        }
    break;

  case 310:
#line 2587 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '~';
        }
    break;

  case 311:
#line 2590 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 312:
#line 2593 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 313:
#line 2596 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '*';
        }
    break;

  case 314:
#line 2599 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '&';
        }
    break;

  case 315:
#line 2604 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].qchar) != '\0' && (yyvsp[(3) - (3)].value).vtype == string_value)
                yyerror("Invalid unary operator for string");
 
            /* Convert the value to a simple expression on the heap. */
            (yyval.valp) = sipMalloc(sizeof (valueDef));
 
            *(yyval.valp) = (yyvsp[(3) - (3)].value);
            (yyval.valp)->vunop = (yyvsp[(2) - (3)].qchar);
            (yyval.valp)->vbinop = '\0';
            (yyval.valp)->cast = (yyvsp[(1) - (3)].scpvalp);
            (yyval.valp)->next = NULL;
        }
    break;

  case 316:
#line 2619 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = NULL;
        }
    break;

  case 317:
#line 2622 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = (yyvsp[(2) - (3)].scpvalp);
        }
    break;

  case 318:
#line 2627 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Scoped names are not allowed in a C module");

            (yyval.scpvalp) = scopeScopedName(NULL, (yyvsp[(2) - (2)].scpvalp));
        }
    break;

  case 321:
#line 2637 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Scoped names are not allowed in a C module");

            appendScopedName(&(yyvsp[(1) - (3)].scpvalp), (yyvsp[(3) - (3)].scpvalp));
        }
    break;

  case 322:
#line 2645 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = text2scopePart((yyvsp[(1) - (1)].text));
        }
    break;

  case 323:
#line 2650 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 324:
#line 2653 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 325:
#line 2658 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * We let the C++ compiler decide if the value is a valid one - no
             * point in building a full C++ parser here.
             */

            (yyval.value).vtype = scoped_value;
            (yyval.value).u.vscp = (yyvsp[(1) - (1)].scpvalp);
        }
    break;

  case 326:
#line 2667 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            fcallDef *fcd;

            fcd = sipMalloc(sizeof (fcallDef));
            *fcd = (yyvsp[(3) - (4)].fcall);
            fcd -> type = (yyvsp[(1) - (4)].memArg);

            (yyval.value).vtype = fcall_value;
            (yyval.value).u.fcd = fcd;
        }
    break;

  case 327:
#line 2677 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = empty_value;
        }
    break;

  case 328:
#line 2680 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = real_value;
            (yyval.value).u.vreal = (yyvsp[(1) - (1)].real);
        }
    break;

  case 329:
#line 2684 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = (yyvsp[(1) - (1)].number);
        }
    break;

  case 330:
#line 2688 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = (yyvsp[(1) - (1)].boolean);
        }
    break;

  case 331:
#line 2692 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = 0;
        }
    break;

  case 332:
#line 2696 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = string_value;
            (yyval.value).u.vstr = (yyvsp[(1) - (1)].text);
        }
    break;

  case 333:
#line 2700 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = qchar_value;
            (yyval.value).u.vqchar = (yyvsp[(1) - (1)].qchar);
        }
    break;

  case 334:
#line 2706 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* No values. */

            (yyval.fcall).nrArgs = 0;
        }
    break;

  case 335:
#line 2711 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first expression. */

            (yyval.fcall).args[0] = (yyvsp[(1) - (1)].valp);
            (yyval.fcall).nrArgs = 1;
        }
    break;

  case 336:
#line 2717 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check that it wasn't ...(,expression...). */

            if ((yyval.fcall).nrArgs == 0)
                yyerror("First argument to function call is missing");

            /* Check there is room. */

            if ((yyvsp[(1) - (3)].fcall).nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.fcall) = (yyvsp[(1) - (3)].fcall);

            (yyval.fcall).args[(yyval.fcall).nrArgs] = (yyvsp[(3) - (3)].valp);
            (yyval.fcall).nrArgs++;
        }
    break;

  case 337:
#line 2735 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "Capsule",
                    "DocType",
                    "Encoding",
                    "NoTypeName",
                    "PyInt",
                    "PyName",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    NULL
                };

                checkAnnos(&(yyvsp[(4) - (6)].optflags), annos);

                applyTypeFlags(currentModule, &(yyvsp[(2) - (6)].memArg), &(yyvsp[(4) - (6)].optflags));
                newTypedef(currentSpec, currentModule, (yyvsp[(3) - (6)].text), &(yyvsp[(2) - (6)].memArg), &(yyvsp[(4) - (6)].optflags), (yyvsp[(6) - (6)].docstr));
            }
        }
    break;

  case 338:
#line 2757 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "DocType",
                    "Encoding",
                    "NoTypeName",
                    "PyInt",
                    "PyName",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    NULL
                };

                signatureDef *sig;
                argDef ftype;

                checkAnnos(&(yyvsp[(10) - (12)].optflags), annos);

                applyTypeFlags(currentModule, &(yyvsp[(2) - (12)].memArg), &(yyvsp[(10) - (12)].optflags));

                memset(&ftype, 0, sizeof (argDef));

                /* Create the full signature on the heap. */
                sig = sipMalloc(sizeof (signatureDef));
                *sig = (yyvsp[(8) - (12)].signature);
                sig->result = (yyvsp[(2) - (12)].memArg);

                /* Create the full type. */
                ftype.atype = function_type;
                ftype.nrderefs = 1;
                ftype.u.sa = sig;

                newTypedef(currentSpec, currentModule, (yyvsp[(5) - (12)].text), &ftype, &(yyvsp[(10) - (12)].optflags), (yyvsp[(12) - (12)].docstr));
            }
        }
    break;

  case 339:
#line 2796 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc && (yyvsp[(2) - (2)].scpvalp)->next != NULL)
                yyerror("Namespaces not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 340:
#line 2802 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "Abstract",
                    "AllowNone",
                    "API",
                    "DelayDtor",
                    "Deprecated",
                    "ExportDerived",
                    "External",
                    "FileExtension",
                    "Metatype",
                    "Mixin",
                    "NoDefaultCtors",
                    "NoTypeHint",
                    "PyName",
                    "PyQtFlags",
                    "PyQtFlagsEnums",
                    "PyQtInterface",
                    "PyQtNoQMetaObject",
                    "Supertype",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    "TypeHintValue",
                    "VirtualErrorHandler",
                    NULL
                };

                checkAnnos(&(yyvsp[(5) - (5)].optflags), annos);

                if (currentSpec->genc && currentSupers != NULL)
                    yyerror("Super-classes not allowed in a C module struct");

                defineClass((yyvsp[(2) - (5)].scpvalp), currentSupers, &(yyvsp[(5) - (5)].optflags));
                sectionFlags = SECT_IS_PUBLIC;
            }
        }
    break;

  case 341:
#line 2840 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 342:
#line 2846 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsTemplate = TRUE;}
    break;

  case 343:
#line 2846 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Class templates not allowed in a C module");

            if (notSkipping())
            {
                classTmplDef *tcd;

                /*
                 * Make sure there is room for the extra class name argument.
                 */
                if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                    yyerror("Internal error - increase the value of MAX_NR_ARGS");

                tcd = sipMalloc(sizeof (classTmplDef));
                tcd->sig = (yyvsp[(1) - (3)].signature);
                tcd->cd = (yyvsp[(3) - (3)].klass);
                tcd->next = currentSpec->classtemplates;

                currentSpec->classtemplates = tcd;
            }

            currentIsTemplate = FALSE;
        }
    break;

  case 344:
#line 2872 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.signature) = (yyvsp[(3) - (4)].signature);
        }
    break;

  case 345:
#line 2877 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Class definition not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 346:
#line 2883 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "Abstract",
                    "AllowNone",
                    "API",
                    "DelayDtor",
                    "Deprecated",
                    "ExportDerived",
                    "External",
                    "FileExtension",
                    "Metatype",
                    "Mixin",
                    "NoDefaultCtors",
                    "PyName",
                    "PyQtFlags",
                    "PyQtFlagsEnums",
                    "PyQtInterface",
                    "PyQtNoQMetaObject",
                    "Supertype",
                    "TypeHint",
                    "TypeHintIn",
                    "TypeHintOut",
                    "TypeHintValue",
                    "VirtualErrorHandler",
                    NULL
                };

                checkAnnos(&(yyvsp[(5) - (5)].optflags), annos);

                defineClass((yyvsp[(2) - (5)].scpvalp), currentSupers, &(yyvsp[(5) - (5)].optflags));
                sectionFlags = SECT_IS_PRIVATE;
            }
        }
    break;

  case 347:
#line 2917 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                (yyval.klass) = completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 352:
#line 2931 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping() && (yyvsp[(1) - (2)].token) == TK_PUBLIC)
            {
                argDef ad;
                classDef *super;
                scopedNameDef *snd = (yyvsp[(2) - (2)].scpvalp);

                /*
                 * This is a hack to allow typedef'ed classes to be used before
                 * we have resolved the typedef definitions.  Unlike elsewhere,
                 * we require that the typedef is defined before being used.
                 */
                for (;;)
                {
                    ad.atype = no_type;
                    ad.argflags = 0;
                    ad.nrderefs = 0;
                    ad.original_type = NULL;

                    searchTypedefs(currentSpec, snd, &ad);

                    if (ad.atype != defined_type)
                        break;

                    if (ad.nrderefs != 0 || isConstArg(&ad) || isReference(&ad))
                        break;

                    snd = ad.u.snd;
                }

                if (ad.atype != no_type)
                    yyerror("Super-class list contains an invalid type");

                /*
                 * This is a bug because we should look in the local scope
                 * rather than assume it is in the global scope.
                 */
                if (snd->name[0] != '\0')
                    snd = scopeScopedName(NULL, snd);

                /*
                 * Note that passing NULL as the API is a bug.  Instead we
                 * should pass the API of the sub-class being defined,
                 * otherwise we cannot create sub-classes of versioned classes.
                 */
                super = findClass(currentSpec, class_iface, NULL, snd,
                        currentIsTemplate);
                appendToClassList(&currentSupers, super);
            }
        }
    break;

  case 353:
#line 2983 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PUBLIC;
        }
    break;

  case 354:
#line 2986 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PUBLIC;
        }
    break;

  case 355:
#line 2989 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PROTECTED;
        }
    break;

  case 356:
#line 2992 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PRIVATE;
        }
    break;

  case 357:
#line 2997 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 358:
#line 3000 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 372:
#line 3020 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->docstring != NULL)
                    yyerror("%Docstring already given for class");

                scope->docstring = (yyvsp[(1) - (1)].docstr);
            }
        }
    break;

  case 373:
#line 3031 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->cppcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 374:
#line 3035 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 375:
#line 3039 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->travcode != NULL)
                    yyerror("%GCTraverseCode already given for class");

                appendCodeBlock(&scope->travcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 376:
#line 3050 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->clearcode != NULL)
                    yyerror("%GCClearCode already given for class");

                appendCodeBlock(&scope->clearcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 377:
#line 3061 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->getbufcode != NULL)
                    yyerror("%BIGetBufferCode already given for class");

                appendCodeBlock(&scope->getbufcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 378:
#line 3072 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->releasebufcode != NULL)
                    yyerror("%BIReleaseBufferCode already given for class");

                appendCodeBlock(&scope->releasebufcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 379:
#line 3083 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->readbufcode != NULL)
                    yyerror("%BIGetReadBufferCode already given for class");

                appendCodeBlock(&scope->readbufcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 380:
#line 3094 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->writebufcode != NULL)
                    yyerror("%BIGetWriteBufferCode already given for class");

                appendCodeBlock(&scope->writebufcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 381:
#line 3105 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->segcountcode != NULL)
                    yyerror("%BIGetSegCountCode already given for class");

                appendCodeBlock(&scope->segcountcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 382:
#line 3116 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->charbufcode != NULL)
                    yyerror("%BIGetCharBufferCode already given for class");

                appendCodeBlock(&scope->charbufcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 383:
#line 3127 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->instancecode != NULL)
                    yyerror("%InstanceCode already given for class");

                appendCodeBlock(&scope->instancecode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 384:
#line 3138 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->picklecode != NULL)
                    yyerror("%PickleCode already given for class");

                appendCodeBlock(&scope->picklecode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 385:
#line 3149 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->finalcode != NULL)
                    yyerror("%FinalisationCode already given for class");

                appendCodeBlock(&scope->finalcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 386:
#line 3160 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->typehintcode != NULL)
                    yyerror("%TypeHintCode already given for class");

                appendCodeBlock(&scope->typehintcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 390:
#line 3174 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtosubcode != NULL)
                    yyerror("Class has more than one %ConvertToSubClassCode directive");

                appendCodeBlock(&scope->convtosubcode, (yyvsp[(2) - (2)].codeb));
            }
        }
    break;

  case 391:
#line 3185 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtocode != NULL)
                    yyerror("Class has more than one %ConvertToTypeCode directive");

                appendCodeBlock(&scope->convtocode, (yyvsp[(2) - (2)].codeb));
            }
        }
    break;

  case 392:
#line 3196 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convfromcode != NULL)
                    yyerror("Class has more than one %ConvertFromTypeCode directive");

                appendCodeBlock(&scope->convfromcode, (yyvsp[(2) - (2)].codeb));
            }
        }
    break;

  case 393:
#line 3207 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("public section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PUBLIC | (yyvsp[(2) - (3)].number);
        }
    break;

  case 394:
#line 3214 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("protected section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PROT | (yyvsp[(2) - (3)].number);
        }
    break;

  case 395:
#line 3221 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("private section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PRIVATE | (yyvsp[(2) - (3)].number);
        }
    break;

  case 396:
#line 3228 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("signals section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_SIGNAL;
        }
    break;

  case 397:
#line 3237 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].property).name == NULL)
                yyerror("A %Property directive must have a 'name' argument");

            if ((yyvsp[(2) - (3)].property).get == NULL)
                yyerror("A %Property directive must have a 'get' argument");

            if (notSkipping())
                addProperty(currentSpec, currentModule, currentScope(),
                        (yyvsp[(2) - (3)].property).name, (yyvsp[(2) - (3)].property).get, (yyvsp[(2) - (3)].property).set, (yyvsp[(3) - (3)].property).docstring);
        }
    break;

  case 398:
#line 3250 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(2) - (3)].property);
        }
    break;

  case 400:
#line 3256 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(1) - (3)].property);

            switch ((yyvsp[(3) - (3)].property).token)
            {
            case TK_GET: (yyval.property).get = (yyvsp[(3) - (3)].property).get; break;
            case TK_NAME: (yyval.property).name = (yyvsp[(3) - (3)].property).name; break;
            case TK_SET: (yyval.property).set = (yyvsp[(3) - (3)].property).set; break;
            }
        }
    break;

  case 401:
#line 3268 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_GET;

            (yyval.property).get = (yyvsp[(3) - (3)].text);
            (yyval.property).name = NULL;
            (yyval.property).set = NULL;
        }
    break;

  case 402:
#line 3275 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_NAME;

            (yyval.property).get = NULL;
            (yyval.property).name = (yyvsp[(3) - (3)].text);
            (yyval.property).set = NULL;
        }
    break;

  case 403:
#line 3282 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_SET;

            (yyval.property).get = NULL;
            (yyval.property).name = NULL;
            (yyval.property).set = (yyvsp[(3) - (3)].text);
        }
    break;

  case 404:
#line 3291 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = 0;
            (yyval.property).docstring = NULL;
        }
    break;

  case 405:
#line 3295 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(2) - (4)].property);
        }
    break;

  case 407:
#line 3301 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(1) - (2)].property);

            switch ((yyvsp[(2) - (2)].property).token)
            {
            case TK_DOCSTRING: (yyval.property).docstring = (yyvsp[(2) - (2)].property).docstring; break;
            }
        }
    break;

  case 408:
#line 3311 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_IF;
        }
    break;

  case 409:
#line 3314 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_END;
        }
    break;

  case 410:
#line 3317 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.property).token = TK_DOCSTRING;
                (yyval.property).docstring = (yyvsp[(1) - (1)].docstr);
            }
            else
            {
                (yyval.property).token = 0;
                (yyval.property).docstring = NULL;
            }
        }
    break;

  case 413:
#line 3335 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 414:
#line 3338 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = SECT_IS_SLOT;
        }
    break;

  case 415:
#line 3344 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsVirt = TRUE;}
    break;

  case 418:
#line 3348 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Note that we allow non-virtual dtors in C modules. */

            if (notSkipping())
            {
                const char *annos[] = {
                    "HoldGIL",
                    "ReleaseGIL",
                    NULL
                };

                classDef *cd = currentScope();

                checkAnnos(&(yyvsp[(7) - (11)].optflags), annos);

                if (strcmp(classBaseName(cd),(yyvsp[(2) - (11)].text)) != 0)
                    yyerror("Destructor doesn't have the same name as its class");

                if (isDtor(cd))
                    yyerror("Destructor has already been defined");

                if (currentSpec -> genc && (yyvsp[(9) - (11)].codeb) == NULL)
                    yyerror("Destructor in C modules must include %MethodCode");


                appendCodeBlock(&cd->dealloccode, (yyvsp[(9) - (11)].codeb));  /* premethodcode */
                appendCodeBlock(&cd->dealloccode, (yyvsp[(10) - (11)].codeb)); /* methodcode */
                appendCodeBlock(&cd->dtorcode, (yyvsp[(11) - (11)].codeb));
                cd -> dtorexceptions = (yyvsp[(5) - (11)].throwlist);

                /*
                 * Note that we don't apply the protected/public hack to dtors
                 * as it (I think) may change the behaviour of the wrapped API.
                 */
                cd->classflags |= sectionFlags;

                if ((yyvsp[(6) - (11)].number))
                {
                    if (!currentIsVirt)
                        yyerror("Abstract destructor must be virtual");

                    setIsAbstractClass(cd);
                }

                /*
                 * The class has a shadow if we have a virtual dtor or some
                 * dtor code.
                 */
                if (currentIsVirt || (yyvsp[(10) - (11)].codeb) != NULL)
                {
                    if (currentSpec -> genc)
                        yyerror("Virtual destructor or %VirtualCatcherCode not allowed in a C module");

                    setNeedsShadow(cd);
                }

                if (getReleaseGIL(&(yyvsp[(7) - (11)].optflags)))
                    setIsReleaseGILDtor(cd);
                else if (getHoldGIL(&(yyvsp[(7) - (11)].optflags)))
                    setIsHoldGILDtor(cd);
            }

            currentIsVirt = FALSE;
        }
    break;

  case 419:
#line 3414 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentCtorIsExplicit = TRUE;}
    break;

  case 422:
#line 3418 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Note that we allow ctors in C modules. */

            if (notSkipping())
            {
                const char *annos[] = {
                    "API",
                    "Default",
                    "Deprecated",
                    "HoldGIL",
                    "KeywordArgs",
                    "NoDerived",
                    "NoRaisesPyException",
                    "NoTypeHint",
                    "PostHook",
                    "PreHook",
                    "RaisesPyException",
                    "ReleaseGIL",
                    "Transfer",
                    NULL
                };

                checkAnnos(&(yyvsp[(6) - (11)].optflags), annos);

                if (currentSpec -> genc)
                {
                    if ((yyvsp[(10) - (11)].codeb) == NULL && (yyvsp[(3) - (11)].signature).nrArgs != 0)
                        yyerror("Constructors with arguments in C modules must include %MethodCode");

                    if (currentCtorIsExplicit)
                        yyerror("Explicit constructors not allowed in a C module");
                }

                if ((sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE)) == 0)
                    yyerror("Constructor must be in the public, private or protected sections");

                newCtor(currentModule, (yyvsp[(1) - (11)].text), sectionFlags, &(yyvsp[(3) - (11)].signature), &(yyvsp[(6) - (11)].optflags), (yyvsp[(11) - (11)].codeb), (yyvsp[(5) - (11)].throwlist), (yyvsp[(7) - (11)].optsignature),
                        currentCtorIsExplicit, (yyvsp[(9) - (11)].docstr), (yyvsp[(10) - (11)].codeb));
            }

            free((yyvsp[(1) - (11)].text));

            currentCtorIsExplicit = FALSE;
        }
    break;

  case 423:
#line 3464 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 424:
#line 3467 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            parsingCSignature = TRUE;
        }
    break;

  case 425:
#line 3469 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(4) - (6)].signature);

            parsingCSignature = FALSE;
        }
    break;

  case 426:
#line 3478 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 427:
#line 3481 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            parsingCSignature = TRUE;
        }
    break;

  case 428:
#line 3483 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(5) - (7)].signature);
            (yyval.optsignature)->result = (yyvsp[(3) - (7)].memArg);

            parsingCSignature = FALSE;
        }
    break;

  case 429:
#line 3493 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                applyTypeFlags(currentModule, &(yyvsp[(1) - (17)].memArg), &(yyvsp[(10) - (17)].optflags));

                (yyvsp[(4) - (17)].signature).result = (yyvsp[(1) - (17)].memArg);

                newFunction(currentSpec, currentModule, currentScope(), NULL,
                        NULL, sectionFlags, currentIsStatic, currentIsSignal,
                        currentIsSlot, currentIsVirt, (yyvsp[(2) - (17)].text), &(yyvsp[(4) - (17)].signature), (yyvsp[(6) - (17)].number), (yyvsp[(9) - (17)].number), &(yyvsp[(10) - (17)].optflags),
                        (yyvsp[(15) - (17)].codeb), (yyvsp[(16) - (17)].codeb), (yyvsp[(17) - (17)].codeb), (yyvsp[(8) - (17)].throwlist), (yyvsp[(11) - (17)].optsignature), (yyvsp[(13) - (17)].docstr), (yyvsp[(7) - (17)].number), (yyvsp[(14) - (17)].codeb));
            }

            currentIsStatic = FALSE;
            currentIsSignal = FALSE;
            currentIsSlot = FALSE;
            currentIsVirt = FALSE;
        }
    break;

  case 430:
#line 3511 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * It looks like an assignment operator (though we don't bother to
             * check the types) so make sure it is private.
             */
            if (notSkipping())
            {
                classDef *cd = currentScope();

                if (cd == NULL || !(sectionFlags & SECT_IS_PRIVATE))
                    yyerror("Assignment operators may only be defined as private");

                setCannotAssign(cd);
            }

            currentIsStatic = FALSE;
            currentIsSignal = FALSE;
            currentIsSlot = FALSE;
            currentIsVirt = FALSE;
        }
    break;

  case 431:
#line 3531 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                classDef *cd = currentScope();
                ifaceFileDef *ns_scope;

                /*
                 * If the scope is a namespace then make sure the operator is
                 * handled as a global, but remember it's C++ scope..
                 */
                if (cd != NULL && cd->iff->type == namespace_iface)
                {
                    ns_scope = cd->iff;
                    cd = NULL;
                }
                else
                {
                    ns_scope = NULL;
                }

                applyTypeFlags(currentModule, &(yyvsp[(1) - (17)].memArg), &(yyvsp[(11) - (17)].optflags));

                /* Handle the unary '+' and '-' operators. */
                if ((cd != NULL && (yyvsp[(5) - (17)].signature).nrArgs == 0) || (cd == NULL && (yyvsp[(5) - (17)].signature).nrArgs == 1))
                {
                    if (strcmp((yyvsp[(3) - (17)].text), "__add__") == 0)
                        (yyvsp[(3) - (17)].text) = "__pos__";
                    else if (strcmp((yyvsp[(3) - (17)].text), "__sub__") == 0)
                        (yyvsp[(3) - (17)].text) = "__neg__";
                }

                (yyvsp[(5) - (17)].signature).result = (yyvsp[(1) - (17)].memArg);

                newFunction(currentSpec, currentModule, cd, ns_scope, NULL,
                        sectionFlags, currentIsStatic, currentIsSignal,
                        currentIsSlot, currentIsVirt, (yyvsp[(3) - (17)].text), &(yyvsp[(5) - (17)].signature), (yyvsp[(7) - (17)].number), (yyvsp[(10) - (17)].number), &(yyvsp[(11) - (17)].optflags),
                        (yyvsp[(15) - (17)].codeb), (yyvsp[(16) - (17)].codeb), (yyvsp[(17) - (17)].codeb), (yyvsp[(9) - (17)].throwlist), (yyvsp[(12) - (17)].optsignature), NULL, (yyvsp[(8) - (17)].number), (yyvsp[(14) - (17)].codeb));
            }

            currentIsStatic = FALSE;
            currentIsSignal = FALSE;
            currentIsSlot = FALSE;
            currentIsVirt = FALSE;
        }
    break;

  case 432:
#line 3575 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                char *sname;
                classDef *scope = currentScope();

                if (scope == NULL || (yyvsp[(4) - (16)].signature).nrArgs != 0)
                    yyerror("Operator casts must be specified in a class and have no arguments");

                applyTypeFlags(currentModule, &(yyvsp[(2) - (16)].memArg), &(yyvsp[(10) - (16)].optflags));

                switch ((yyvsp[(2) - (16)].memArg).atype)
                {
                case defined_type:
                    sname = NULL;
                    break;

                case bool_type:
                case cbool_type:
                case byte_type:
                case sbyte_type:
                case ubyte_type:
                case short_type:
                case ushort_type:
                case int_type:
                case cint_type:
                case uint_type:
                    sname = "__int__";
                    break;

                case long_type:
                case ulong_type:
                case longlong_type:
                case ulonglong_type:
                    sname = "__long__";
                    break;

                case float_type:
                case cfloat_type:
                case double_type:
                case cdouble_type:
                    sname = "__float__";
                    break;

                default:
                    yyerror("Unsupported operator cast");
                }

                if (sname != NULL)
                {
                    (yyvsp[(4) - (16)].signature).result = (yyvsp[(2) - (16)].memArg);

                    newFunction(currentSpec, currentModule, scope, NULL, NULL,
                            sectionFlags, currentIsStatic, currentIsSignal,
                            currentIsSlot, currentIsVirt, sname, &(yyvsp[(4) - (16)].signature), (yyvsp[(6) - (16)].number), (yyvsp[(9) - (16)].number),
                            &(yyvsp[(10) - (16)].optflags), (yyvsp[(14) - (16)].codeb), (yyvsp[(15) - (16)].codeb), (yyvsp[(16) - (16)].codeb), (yyvsp[(8) - (16)].throwlist), (yyvsp[(11) - (16)].optsignature), NULL, (yyvsp[(7) - (16)].number), (yyvsp[(13) - (16)].codeb));
                }
                else
                {
                    argList *al;

                    /* Check it doesn't already exist. */
                    for (al = scope->casts; al != NULL; al = al->next)
                        if (compareScopedNames((yyvsp[(2) - (16)].memArg).u.snd, al->arg.u.snd) == 0)
                            yyerror("This operator cast has already been specified in this class");

                    al = sipMalloc(sizeof (argList));
                    al->arg = (yyvsp[(2) - (16)].memArg);
                    al->next = scope->casts;

                    scope->casts = al;
                }
            }

            currentIsStatic = FALSE;
            currentIsSignal = FALSE;
            currentIsSlot = FALSE;
            currentIsVirt = FALSE;
        }
    break;

  case 433:
#line 3656 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__add__";}
    break;

  case 434:
#line 3657 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__sub__";}
    break;

  case 435:
#line 3658 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__mul__";}
    break;

  case 436:
#line 3659 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__div__";}
    break;

  case 437:
#line 3660 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__mod__";}
    break;

  case 438:
#line 3661 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__and__";}
    break;

  case 439:
#line 3662 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__or__";}
    break;

  case 440:
#line 3663 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__xor__";}
    break;

  case 441:
#line 3664 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__lshift__";}
    break;

  case 442:
#line 3665 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__rshift__";}
    break;

  case 443:
#line 3666 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__iadd__";}
    break;

  case 444:
#line 3667 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__isub__";}
    break;

  case 445:
#line 3668 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__imul__";}
    break;

  case 446:
#line 3669 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__idiv__";}
    break;

  case 447:
#line 3670 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__imod__";}
    break;

  case 448:
#line 3671 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__iand__";}
    break;

  case 449:
#line 3672 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ior__";}
    break;

  case 450:
#line 3673 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ixor__";}
    break;

  case 451:
#line 3674 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ilshift__";}
    break;

  case 452:
#line 3675 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__irshift__";}
    break;

  case 453:
#line 3676 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__invert__";}
    break;

  case 454:
#line 3677 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__call__";}
    break;

  case 455:
#line 3678 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__getitem__";}
    break;

  case 456:
#line 3679 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__lt__";}
    break;

  case 457:
#line 3680 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__le__";}
    break;

  case 458:
#line 3681 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__eq__";}
    break;

  case 459:
#line 3682 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ne__";}
    break;

  case 460:
#line 3683 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__gt__";}
    break;

  case 461:
#line 3684 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ge__";}
    break;

  case 462:
#line 3687 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 463:
#line 3690 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 464:
#line 3695 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 465:
#line 3698 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 466:
#line 3703 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 467:
#line 3706 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (2)].number) != 0)
                yyerror("Abstract virtual function '= 0' expected");

            (yyval.number) = TRUE;
        }
    break;

  case 468:
#line 3714 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags).nrFlags = 0;
        }
    break;

  case 469:
#line 3717 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags) = (yyvsp[(2) - (3)].optflags);
        }
    break;

  case 470:
#line 3723 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags).flags[0] = (yyvsp[(1) - (1)].flag);
            (yyval.optflags).nrFlags = 1;
        }
    break;

  case 471:
#line 3727 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check there is room. */

            if ((yyvsp[(1) - (3)].optflags).nrFlags == MAX_NR_FLAGS)
                yyerror("Too many optional flags");

            (yyval.optflags) = (yyvsp[(1) - (3)].optflags);

            (yyval.optflags).flags[(yyval.optflags).nrFlags++] = (yyvsp[(3) - (3)].flag);
        }
    break;

  case 472:
#line 3739 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = bool_flag;
            (yyval.flag).fname = (yyvsp[(1) - (1)].text);
        }
    break;

  case 473:
#line 3743 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag) = (yyvsp[(3) - (3)].flag);
            (yyval.flag).fname = (yyvsp[(1) - (3)].text);
        }
    break;

  case 474:
#line 3749 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = (strchr((yyvsp[(1) - (1)].text), '.') != NULL) ? dotted_name_flag : name_flag;
            (yyval.flag).fvalue.sval = (yyvsp[(1) - (1)].text);
        }
    break;

  case 475:
#line 3753 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            apiVersionRangeDef *avd;
            int from, to;

            (yyval.flag).ftype = api_range_flag;

            /* Check that the API is known. */
            if ((avd = findAPI(currentSpec, (yyvsp[(1) - (5)].text))) == NULL)
                yyerror("unknown API name in API annotation");

            if (inMainModule())
                setIsUsedName(avd->api_name);

            /* Unbounded values are represented by 0. */
            if ((from = (yyvsp[(3) - (5)].number)) < 0)
                from = 0;

            if ((to = (yyvsp[(5) - (5)].number)) < 0)
                to = 0;

            (yyval.flag).fvalue.aval = convertAPIRange(currentModule, avd->api_name,
                    from, to);
        }
    break;

  case 476:
#line 3776 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = string_flag;
            (yyval.flag).fvalue.sval = convertFeaturedString((yyvsp[(1) - (1)].text));
        }
    break;

  case 477:
#line 3780 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = integer_flag;
            (yyval.flag).fvalue.ival = (yyvsp[(1) - (1)].number);
        }
    break;

  case 478:
#line 3786 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 479:
#line 3789 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 480:
#line 3794 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 481:
#line 3797 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 482:
#line 3802 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 483:
#line 3805 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 484:
#line 3810 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 485:
#line 3813 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 486:
#line 3818 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            int a, nrrxcon, nrarray, nrarraysize;

            nrrxcon = nrarray = nrarraysize = 0;

            for (a = 0; a < (yyvsp[(1) - (1)].signature).nrArgs; ++a)
            {
                argDef *ad = &(yyvsp[(1) - (1)].signature).args[a];

                switch (ad -> atype)
                {
                case rxcon_type:
                    ++nrrxcon;
                    break;

                /* Suppress a compiler warning. */
                default:
                    ;
                }

                if (isArray(ad))
                    ++nrarray;

                if (isArraySize(ad))
                    ++nrarraysize;
            }

            if (nrrxcon != 0)
                yyerror("SIP_RXOBJ_CON and SIP_SLOT_CON must both be given and at most once");

            if (nrarray != nrarraysize || nrarray > 1)
                yyerror("/Array/ and /ArraySize/ must both be given and at most once");

            (yyval.signature) = (yyvsp[(1) - (1)].signature);
        }
    break;

  case 487:
#line 3855 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* No arguments. */

            (yyval.signature).nrArgs = 0;
        }
    break;

  case 488:
#line 3860 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first argument. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 489:
#line 3866 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check that it wasn't ...(,arg...). */
            if ((yyvsp[(1) - (3)].signature).nrArgs == 0)
                yyerror("First argument of the list is missing");

            /*
             * If this argument has no default value, then the
             * previous one mustn't either.
             */
            if ((yyvsp[(3) - (3)].memArg).defval == NULL && (yyvsp[(1) - (3)].signature).args[(yyvsp[(1) - (3)].signature).nrArgs - 1].defval != NULL)
                yyerror("Compulsory argument given after optional argument");

            /* Check there is room. */
            if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.signature) = (yyvsp[(1) - (3)].signature);

            (yyval.signature).args[(yyval.signature).nrArgs] = (yyvsp[(3) - (3)].memArg);
            (yyval.signature).nrArgs++;
        }
    break;

  case 490:
#line 3889 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            deprecated("SIP_SIGNAL is deprecated\n");
            checkNoAnnos(&(yyvsp[(3) - (4)].optflags), "SIP_SIGNAL has no annotations");

            (yyval.memArg).atype = signal_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (4)].text));
            (yyval.memArg).defval = (yyvsp[(4) - (4)].valp);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 491:
#line 3901 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            deprecated("SIP_SLOT is deprecated\n");
            checkNoAnnos(&(yyvsp[(3) - (4)].optflags), "SIP_SLOT has no annotations");

            (yyval.memArg).atype = slot_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (4)].text));
            (yyval.memArg).defval = (yyvsp[(4) - (4)].valp);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 492:
#line 3913 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            const char *annos[] = {
                "SingleShot",
                NULL
            };

            deprecated("SIP_RXOBJ_CON is deprecated\n");
            checkAnnos(&(yyvsp[(3) - (3)].optflags), annos);

            (yyval.memArg).atype = rxcon_type;
            (yyval.memArg).argflags = 0;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags), "SingleShot", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_SINGLE_SHOT;

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 493:
#line 3932 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (2)].memArg);
            (yyval.memArg).defval = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 494:
#line 3939 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsSignal = TRUE;}
    break;

  case 496:
#line 3940 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsSlot = TRUE;}
    break;

  case 499:
#line 3945 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsStatic = TRUE;}
    break;

  case 504:
#line 3955 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsVirt = TRUE;}
    break;

  case 507:
#line 3959 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "DocType",
                    "Encoding",
                    "NoSetter",
                    "NoTypeHint",
                    "PyInt",
                    "PyName",
                    "TypeHint",
                    NULL
                };

                checkAnnos(&(yyvsp[(3) - (8)].optflags), annos);

                if ((yyvsp[(6) - (8)].codeb) != NULL)
                {
                    if ((yyvsp[(4) - (8)].variable).access_code != NULL)
                        yyerror("%AccessCode already defined");

                    (yyvsp[(4) - (8)].variable).access_code = (yyvsp[(6) - (8)].codeb);

                    deprecated("%AccessCode should be used as a sub-directive");
                }

                if ((yyvsp[(7) - (8)].codeb) != NULL)
                {
                    if ((yyvsp[(4) - (8)].variable).get_code != NULL)
                        yyerror("%GetCode already defined");

                    (yyvsp[(4) - (8)].variable).get_code = (yyvsp[(7) - (8)].codeb);

                    deprecated("%GetCode should be used as a sub-directive");
                }

                if ((yyvsp[(8) - (8)].codeb) != NULL)
                {
                    if ((yyvsp[(4) - (8)].variable).set_code != NULL)
                        yyerror("%SetCode already defined");

                    (yyvsp[(4) - (8)].variable).set_code = (yyvsp[(8) - (8)].codeb);

                    deprecated("%SetCode should be used as a sub-directive");
                }

                newVar(currentSpec, currentModule, (yyvsp[(2) - (8)].text), currentIsStatic, &(yyvsp[(1) - (8)].memArg),
                        &(yyvsp[(3) - (8)].optflags), (yyvsp[(4) - (8)].variable).access_code, (yyvsp[(4) - (8)].variable).get_code, (yyvsp[(4) - (8)].variable).set_code,
                        sectionFlags);
            }

            currentIsStatic = FALSE;
        }
    break;

  case 508:
#line 4014 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = 0;
            (yyval.variable).access_code = NULL;
            (yyval.variable).get_code = NULL;
            (yyval.variable).set_code = NULL;
        }
    break;

  case 509:
#line 4020 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable) = (yyvsp[(2) - (3)].variable);
        }
    break;

  case 511:
#line 4026 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable) = (yyvsp[(1) - (2)].variable);

            switch ((yyvsp[(2) - (2)].variable).token)
            {
            case TK_ACCESSCODE: (yyval.variable).access_code = (yyvsp[(2) - (2)].variable).access_code; break;
            case TK_GETCODE: (yyval.variable).get_code = (yyvsp[(2) - (2)].variable).get_code; break;
            case TK_SETCODE: (yyval.variable).set_code = (yyvsp[(2) - (2)].variable).set_code; break;
            }
        }
    break;

  case 512:
#line 4038 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = TK_IF;
        }
    break;

  case 513:
#line 4041 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = TK_END;
        }
    break;

  case 514:
#line 4044 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.variable).token = TK_ACCESSCODE;
                (yyval.variable).access_code = (yyvsp[(2) - (2)].codeb);
            }
            else
            {
                (yyval.variable).token = 0;
                (yyval.variable).access_code = NULL;
            }

            (yyval.variable).get_code = NULL;
            (yyval.variable).set_code = NULL;
        }
    break;

  case 515:
#line 4059 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.variable).token = TK_GETCODE;
                (yyval.variable).get_code = (yyvsp[(2) - (2)].codeb);
            }
            else
            {
                (yyval.variable).token = 0;
                (yyval.variable).get_code = NULL;
            }

            (yyval.variable).access_code = NULL;
            (yyval.variable).set_code = NULL;
        }
    break;

  case 516:
#line 4074 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                (yyval.variable).token = TK_SETCODE;
                (yyval.variable).set_code = (yyvsp[(2) - (2)].codeb);
            }
            else
            {
                (yyval.variable).token = 0;
                (yyval.variable).set_code = NULL;
            }

            (yyval.variable).access_code = NULL;
            (yyval.variable).get_code = NULL;
        }
    break;

  case 517:
#line 4091 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg) = (yyvsp[(2) - (4)].memArg);
            add_derefs(&(yyval.memArg), &(yyvsp[(3) - (4)].memArg));
            (yyval.memArg).argflags |= ARG_IS_CONST | (yyvsp[(4) - (4)].number);
        }
    break;

  case 518:
#line 4096 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (3)].memArg);
            add_derefs(&(yyval.memArg), &(yyvsp[(2) - (3)].memArg));
            (yyval.memArg).argflags |= (yyvsp[(3) - (3)].number);

            /* PyObject * is a synonym for SIP_PYOBJECT. */
            if ((yyvsp[(1) - (3)].memArg).atype == defined_type && strcmp((yyvsp[(1) - (3)].memArg).u.snd->name, "PyObject") == 0 && (yyvsp[(1) - (3)].memArg).u.snd->next == NULL && (yyvsp[(2) - (3)].memArg).nrderefs == 1 && (yyvsp[(3) - (3)].number) == 0)
            {
                (yyval.memArg).atype = pyobject_type;
                (yyval.memArg).nrderefs = 0;
            }
        }
    break;

  case 519:
#line 4110 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            const char *annos[] = {
                "AllowNone",
                "Array",
                "ArraySize",
                "Constrained",
                "DisallowNone",
                "DocType",
                "DocValue",
                "Encoding",
                "GetWrapper",
                "In",
                "KeepReference",
                "NoCopy",
                "Out",
                "PyInt",
                "ResultSize",
                "ScopesStripped",
                "Transfer",
                "TransferBack",
                "TransferThis",
                "TypeHint",
                "TypeHintIn",
                "TypeHintOut",
                "TypeHintValue",
                NULL
            };

            optFlag *of;

            checkAnnos(&(yyvsp[(3) - (3)].optflags), annos);

            (yyval.memArg) = (yyvsp[(1) - (3)].memArg);
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));

            handleKeepReference(&(yyvsp[(3) - (3)].optflags), &(yyval.memArg), currentModule);

            if ((of = getOptFlag(&(yyvsp[(3) - (3)].optflags), "ScopesStripped", opt_integer_flag)) != NULL)
                if (((yyval.memArg).scopes_stripped = of->fvalue.ival) <= 0)
                    yyerror("/ScopesStripped/ must be greater than 0");

            if (getAllowNone(&(yyvsp[(3) - (3)].optflags)))
                (yyval.memArg).argflags |= ARG_ALLOW_NONE;

            if (getDisallowNone(&(yyvsp[(3) - (3)].optflags)))
                (yyval.memArg).argflags |= ARG_DISALLOW_NONE;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"GetWrapper",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_GET_WRAPPER;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"Array",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_ARRAY;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"ArraySize",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_ARRAY_SIZE;

            if (getTransfer(&(yyvsp[(3) - (3)].optflags)))
                (yyval.memArg).argflags |= ARG_XFERRED;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"TransferThis",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_THIS_XFERRED;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"TransferBack",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_XFERRED_BACK;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"In",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_IN;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"Out",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_OUT;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags), "ResultSize", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_RESULT_SIZE;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags), "NoCopy", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_NO_COPY;

            if (getOptFlag(&(yyvsp[(3) - (3)].optflags),"Constrained",bool_flag) != NULL)
            {
                (yyval.memArg).argflags |= ARG_CONSTRAINED;

                switch ((yyval.memArg).atype)
                {
                case bool_type:
                    (yyval.memArg).atype = cbool_type;
                    break;

                case int_type:
                    (yyval.memArg).atype = cint_type;
                    break;

                case float_type:
                    (yyval.memArg).atype = cfloat_type;
                    break;

                case double_type:
                    (yyval.memArg).atype = cdouble_type;
                    break;

                /* Suppress a compiler warning. */
                default:
                    ;
                }
            }

            applyTypeFlags(currentModule, &(yyval.memArg), &(yyvsp[(3) - (3)].optflags));
            (yyval.memArg).typehint_value = getTypeHintValue(&(yyvsp[(3) - (3)].optflags));
        }
    break;

  case 520:
#line 4220 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 521:
#line 4223 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("References not allowed in a C module");

            (yyval.number) = ARG_IS_REF;
        }
    break;

  case 522:
#line 4231 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg).nrderefs = 0;
        }
    break;

  case 523:
#line 4234 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            add_new_deref(&(yyval.memArg), &(yyvsp[(1) - (3)].memArg), TRUE);
        }
    break;

  case 524:
#line 4237 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            add_new_deref(&(yyval.memArg), &(yyvsp[(1) - (2)].memArg), FALSE);
        }
    break;

  case 525:
#line 4242 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = defined_type;
            (yyval.memArg).u.snd = (yyvsp[(1) - (1)].scpvalp);

            /* Try and resolve typedefs as early as possible. */
            resolveAnyTypedef(currentSpec, &(yyval.memArg));
        }
    break;

  case 526:
#line 4250 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            templateDef *td;

            td = sipMalloc(sizeof(templateDef));
            td->fqname = (yyvsp[(1) - (4)].scpvalp);
            td->types = (yyvsp[(3) - (4)].signature);

            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = template_type;
            (yyval.memArg).u.td = td;
        }
    break;

  case 527:
#line 4261 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));

            /* In a C module all structures must be defined. */
            if (currentSpec -> genc)
            {
                (yyval.memArg).atype = defined_type;
                (yyval.memArg).u.snd = (yyvsp[(2) - (2)].scpvalp);
            }
            else
            {
                (yyval.memArg).atype = struct_type;
                (yyval.memArg).u.sname = (yyvsp[(2) - (2)].scpvalp);
            }
        }
    break;

  case 528:
#line 4276 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ushort_type;
        }
    break;

  case 529:
#line 4280 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = short_type;
        }
    break;

  case 530:
#line 4284 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 531:
#line 4288 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 532:
#line 4292 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = int_type;
        }
    break;

  case 533:
#line 4296 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = long_type;
        }
    break;

  case 534:
#line 4300 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulong_type;
        }
    break;

  case 535:
#line 4304 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = longlong_type;
        }
    break;

  case 536:
#line 4308 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulonglong_type;
        }
    break;

  case 537:
#line 4312 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = float_type;
        }
    break;

  case 538:
#line 4316 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = double_type;
        }
    break;

  case 539:
#line 4320 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = bool_type;
        }
    break;

  case 540:
#line 4324 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = sstring_type;
        }
    break;

  case 541:
#line 4328 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ustring_type;
        }
    break;

  case 542:
#line 4332 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = string_type;
        }
    break;

  case 543:
#line 4336 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = wstring_type;
        }
    break;

  case 544:
#line 4340 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = void_type;
        }
    break;

  case 545:
#line 4344 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyobject_type;
        }
    break;

  case 546:
#line 4348 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytuple_type;
        }
    break;

  case 547:
#line 4352 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pylist_type;
        }
    break;

  case 548:
#line 4356 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pydict_type;
        }
    break;

  case 549:
#line 4360 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pycallable_type;
        }
    break;

  case 550:
#line 4364 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyslice_type;
        }
    break;

  case 551:
#line 4368 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytype_type;
        }
    break;

  case 552:
#line 4372 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pybuffer_type;
        }
    break;

  case 553:
#line 4376 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ssize_type;
        }
    break;

  case 554:
#line 4380 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = size_type;
        }
    break;

  case 555:
#line 4384 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ellipsis_type;
        }
    break;

  case 556:
#line 4390 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first type. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 557:
#line 4396 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check there is nothing after an ellipsis. */
            if ((yyvsp[(1) - (3)].signature).args[(yyvsp[(1) - (3)].signature).nrArgs - 1].atype == ellipsis_type)
                yyerror("An ellipsis must be at the end of the argument list");

            /* Check there is room. */
            if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.signature) = (yyvsp[(1) - (3)].signature);

            (yyval.signature).args[(yyval.signature).nrArgs] = (yyvsp[(3) - (3)].memArg);
            (yyval.signature).nrArgs++;
        }
    break;

  case 558:
#line 4412 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.throwlist) = NULL;
        }
    break;

  case 559:
#line 4415 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Exceptions not allowed in a C module");

            (yyval.throwlist) = (yyvsp[(3) - (4)].throwlist);
        }
    break;

  case 560:
#line 4423 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Empty list so use a blank. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 0;
        }
    break;

  case 561:
#line 4429 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The only or first exception. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 1;
            (yyval.throwlist) -> args[0] = findException(currentSpec, (yyvsp[(1) - (1)].scpvalp), FALSE);
        }
    break;

  case 562:
#line 4436 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check that it wasn't ...(,arg...). */

            if ((yyvsp[(1) - (3)].throwlist) -> nrArgs == 0)
                yyerror("First exception of throw specifier is missing");

            /* Check there is room. */

            if ((yyvsp[(1) - (3)].throwlist) -> nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.throwlist) = (yyvsp[(1) - (3)].throwlist);
            (yyval.throwlist) -> args[(yyval.throwlist) -> nrArgs++] = findException(currentSpec, (yyvsp[(3) - (3)].scpvalp), FALSE);
        }
    break;


/* Line 1267 of yacc.c.  */
#line 7944 "/Users/phil/hg/sip/code_generator/parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 4452 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"



/*
 * Parse the specification.
 */
void parse(sipSpec *spec, FILE *fp, char *filename, int strict,
        stringList *tsl, stringList *bsl, stringList *xfl, int protHack)
{
    classTmplDef *tcd;

    /* Initialise the spec. */
 
    memset(spec, 0, sizeof (sipSpec));
    spec->genc = -1;

    currentSpec = spec;
    strictParse = strict;
    backstops = bsl;
    neededQualifiers = tsl;
    excludedQualifiers = xfl;
    currentModule = NULL;
    currentMappedType = NULL;
    currentIsVirt = FALSE;
    currentCtorIsExplicit = FALSE;
    currentIsStatic = FALSE;
    currentIsSignal = FALSE;
    currentIsSlot = FALSE;
    currentIsTemplate = FALSE;
    previousFile = NULL;
    stackPtr = 0;
    currentPlatforms = NULL;
    currentScopeIdx = 0;
    sectionFlags = 0;
    defaultKwArgs = NoKwArgs;
    makeProtPublic = protHack;

    newModule(fp, filename);
    spec->module = currentModule;

    yyparse();

    handleEOF();
    handleEOM();

    /*
     * Go through each template class and remove it from the list of classes.
     */
    for (tcd = spec->classtemplates; tcd != NULL; tcd = tcd->next)
    {
        classDef **cdp;

        for (cdp = &spec->classes; *cdp != NULL; cdp = &(*cdp)->next)
            if (*cdp == tcd->cd)
            {
                ifaceFileDef **ifdp;

                /* Remove the interface file as well. */
                for (ifdp = &spec->ifacefiles; *ifdp != NULL; ifdp = &(*ifdp)->next)
                    if (*ifdp == tcd->cd->iff)
                    {
                        *ifdp = (*ifdp)->next;
                        break;
                    }

                *cdp = (*cdp)->next;
                break;
            }
    }
}


/*
 * Tell the parser that a complete file has now been read.
 */
void parserEOF(const char *name, parserContext *pc)
{
    previousFile = sipStrdup(name);
    currentContext = *pc;
}


/*
 * Append a class definition to a class list if it doesn't already appear.
 * Append is needed specifically for the list of super-classes because the
 * order is important to Python.
 */
void appendToClassList(classList **clp,classDef *cd)
{
    classList *new;

    /* Find the end of the list. */

    while (*clp != NULL)
    {
        if ((*clp) -> cd == cd)
            return;

        clp = &(*clp) -> next;
    }

    new = sipMalloc(sizeof (classList));

    new -> cd = cd;
    new -> next = NULL;

    *clp = new;
}


/*
 * Create a new module for the current specification and make it current.
 */
static void newModule(FILE *fp, const char *filename)
{
    moduleDef *mod;

    parseFile(fp, filename, currentModule, FALSE);

    mod = allocModule();
    mod->file = filename;

    if (currentModule != NULL)
        mod->defexception = currentModule->defexception;

    currentModule = mod;
}


/*
 * Allocate and initialise the memory for a new module.
 */
static moduleDef *allocModule()
{
    moduleDef *newmod, **tailp;

    newmod = sipMalloc(sizeof (moduleDef));

    newmod->defdocstringfmt = raw;
    newmod->encoding = no_type;
    newmod->next_key = -1;

    /*
     * The consolidated module support needs these to be in order that they
     * appeared.
     */
    for (tailp = &currentSpec->modules; *tailp != NULL; tailp = &(*tailp)->next)
        ;

    *tailp = newmod;

    return newmod;
}


/*
 * Switch to parsing a new file.
 */
static void parseFile(FILE *fp, const char *name, moduleDef *prevmod,
        int optional)
{
    parserContext pc;

    pc.filename = name;
    pc.ifdepth = stackPtr;
    pc.prevmod = prevmod;

    if (setInputFile(fp, &pc, optional))
        currentContext = pc;
}


/*
 * Find an interface file, or create a new one.
 */
ifaceFileDef *findIfaceFile(sipSpec *pt, moduleDef *mod, scopedNameDef *fqname,
        ifaceFileType iftype, apiVersionRangeDef *api_range, argDef *ad)
{
    ifaceFileDef *iff, *first_alt = NULL;

    /* See if the name is already used. */

    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
    {
        if (compareScopedNames(iff->fqcname, fqname) != 0)
            continue;

        /*
         * If they are both versioned then assume the user knows what they are
         * doing.
         */
        if (iff->api_range != NULL && api_range != NULL && iff->module == mod)
        {
            /* Remember the first of the alternate APIs. */
            if ((first_alt = iff->first_alt) == NULL)
                first_alt = iff;

            break;
        }

        /*
         * They must be the same type except that we allow a class if we want
         * an exception.  This is because we allow classes to be used before
         * they are defined.
         */
        if (iff->type != iftype)
            if (iftype != exception_iface || iff->type != class_iface)
                yyerror("A class, exception, namespace or mapped type has already been defined with the same name");

        /* Ignore an external class declared in another module. */
        if (iftype == class_iface && iff->module != mod)
        {
            classDef *cd;

            for (cd = pt->classes; cd != NULL; cd = cd->next)
                if (cd->iff == iff)
                    break;

            if (cd != NULL && iff->module != NULL && isExternal(cd))
                continue;
        }

        /*
         * If this is a mapped type with the same name defined in a different
         * module, then check that this type isn't the same as any of the
         * mapped types defined in that module.
         */
        if (iftype == mappedtype_iface && iff->module != mod)
        {
            mappedTypeDef *mtd;

            /*
             * This is a bit of a cheat.  With consolidated modules it's
             * possible to have two implementations of a mapped type in
             * different branches of the module hierarchy.  We assume that, if
             * there really are multiple implementations in the same branch,
             * then it will be picked up in a non-consolidated build.
             */
            if (isConsolidated(pt->module))
                continue;

            for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
            {
                if (mtd->iff != iff)
                    continue;

                if (ad->atype != template_type ||
                    mtd->type.atype != template_type ||
                    sameBaseType(ad, &mtd->type))
                    yyerror("Mapped type has already been defined in another module");
            }

            /*
             * If we got here then we have a mapped type based on an existing
             * template, but with unique parameters.  We don't want to use
             * interface files from other modules, so skip this one.
             */

            continue;
        }

        /* Ignore a namespace defined in another module. */
        if (iftype == namespace_iface && iff->module != mod)
            continue;

        return iff;
    }

    iff = sipMalloc(sizeof (ifaceFileDef));

    iff->name = cacheName(pt, scopedNameToString(fqname));
    iff->api_range = api_range;

    if (first_alt != NULL)
    {
        iff->first_alt = first_alt;
        iff->next_alt = first_alt->next_alt;

        first_alt->next_alt = iff;
    }
    else
    {
        /* This is the first alternate so point to itself. */
        iff->first_alt = iff;
    }

    /*
     * Note that we assume that the type (ie. class vs. mapped type vs.
     * exception) will be the same across all platforms.
     */
    iff->platforms = currentPlatforms;

    iff->type = iftype;
    iff->ifacenr = -1;
    iff->fqcname = fqname;
    iff->module = NULL;
    iff->hdrcode = NULL;
    iff->used = NULL;
    iff->file_extension = NULL;
    iff->next = pt->ifacefiles;

    pt->ifacefiles = iff;

    return iff;
}


/*
 * Find a class definition in a parse tree.
 */
static classDef *findClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname, int tmpl_arg)
{
    return findClassWithInterface(pt,
            findIfaceFile(pt, currentModule, fqname, iftype, api_range, NULL),
            tmpl_arg);
}


/*
 * Find a class definition given an existing interface file.
 */
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff,
        int tmpl_arg)
{
    classDef *cd;

    for (cd = pt->classes; cd != NULL; cd = cd->next)
        if (cd->iff == iff)
        {
            if (isTemplateArg(cd) && !tmpl_arg)
                resetTemplateArg(cd);

            return cd;
        }

    /* Create a new one. */
    cd = sipMalloc(sizeof (classDef));

    if (tmpl_arg)
        setTemplateArg(cd);

    cd->iff = iff;
    cd->pyname = cacheName(pt, classBaseName(cd));
    cd->next = pt->classes;

    pt->classes = cd;

    return cd;
}


/*
 * Add an interface file to an interface file list if it isn't already there.
 */
void appendToIfaceFileList(ifaceFileList **ifflp, ifaceFileDef *iff)
{
    /* Make sure we don't try to add an interface file to its own list. */
    if (&iff->used != ifflp)
    {
        ifaceFileList *iffl;

        while ((iffl = *ifflp) != NULL)
        {
            /* Don't bother if it is already there. */
            if (iffl->iff == iff)
                return;

            ifflp = &iffl -> next;
        }

        iffl = sipMalloc(sizeof (ifaceFileList));

        iffl->iff = iff;
        iffl->next = NULL;

        *ifflp = iffl;
    }
}


/*
 * Find an undefined (or create a new) exception definition in a parse tree.
 */
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new)
{
    exceptionDef *xd, **tail;
    ifaceFileDef *iff;
    classDef *cd;

    iff = findIfaceFile(pt, currentModule, fqname, exception_iface, NULL, NULL);

    /* See if it is an existing one. */
    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
        if (xd->iff == iff)
            return xd;

    /*
     * If it is an exception interface file then we have never seen this name
     * before.  We require that exceptions are defined before being used, but
     * don't make the same requirement of classes (for reasons of backwards
     * compatibility).  Therefore the name must be reinterpreted as a (as yet
     * undefined) class.
     */
    if (new)
    {
        if (iff->type == exception_iface)
            cd = NULL;
        else
            yyerror("There is already a class with the same name or the exception has been used before being defined");
    }
    else
    {
        if (iff->type == exception_iface)
            iff->type = class_iface;

        cd = findClassWithInterface(pt, iff, FALSE);
    }

    /* Create a new one. */
    xd = sipMalloc(sizeof (exceptionDef));

    xd->exceptionnr = -1;
    xd->needed = FALSE;
    xd->iff = iff;
    xd->pyname = NULL;
    xd->cd = cd;
    xd->bibase = NULL;
    xd->base = NULL;
    xd->raisecode = NULL;
    xd->next = NULL;

    /* Append it to the list. */
    for (tail = &pt->exceptions; *tail != NULL; tail = &(*tail)->next)
        ;

    *tail = xd;

    return xd;
}


/*
 * Find an undefined (or create a new) class definition in a parse tree.
 */
static classDef *newClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname,
        const char *virt_error_handler, typeHintDef *typehint_in,
        typeHintDef *typehint_out, const char *typehint_value)
{
    int flags;
    classDef *cd, *scope;
    codeBlockList *hdrcode;

    if (sectionFlags & SECT_IS_PRIVATE)
        yyerror("Classes, structs and namespaces must be in the public or protected sections");

    flags = 0;

    if ((scope = currentScope()) != NULL)
    {
        if (sectionFlags & SECT_IS_PROT && !makeProtPublic)
        {
            flags = CLASS_IS_PROTECTED;

            if (scope->iff->type == class_iface)
                setNeedsShadow(scope);
        }

        /* Header code from outer scopes is also included. */
        hdrcode = scope->iff->hdrcode;
    }
    else
        hdrcode = NULL;

    if (pt -> genc)
    {
        /* C structs are always global types. */
        while (fqname -> next != NULL)
            fqname = fqname -> next;

        scope = NULL;
    }

    cd = findClass(pt, iftype, api_range, fqname, FALSE);

    /* Check it hasn't already been defined. */
    if (iftype != namespace_iface && cd->iff->module != NULL)
        yyerror("The struct/class has already been defined");

    /* Complete the initialisation. */
    cd->classflags |= flags;
    cd->ecd = scope;
    cd->iff->module = currentModule;
    cd->virt_error_handler = virt_error_handler;
    cd->typehint_in = typehint_in;
    cd->typehint_out = typehint_out;
    cd->typehint_value = typehint_value;

    if (currentIsTemplate)
        setIsTemplateClass(cd);

    appendCodeBlockList(&cd->iff->hdrcode, hdrcode);

    /* See if it is a namespace extender. */
    if (iftype == namespace_iface)
    {
        classDef *ns;

        for (ns = pt->classes; ns != NULL; ns = ns->next)
        {
            if (ns == cd)
                continue;

            if (ns->iff->type != namespace_iface)
                continue;

            if (compareScopedNames(ns->iff->fqcname, fqname) != 0)
                continue;

            cd->real = ns;

            if (inMainModule())
                ns->iff->first_alt->needed = TRUE;

            break;
        }
    }

    return cd;
}


/*
 * Tidy up after finishing a class definition.
 */
static void finishClass(sipSpec *pt, moduleDef *mod, classDef *cd,
        optFlags *of)
{
    const char *pyname;
    optFlag *flg;

    /* Get the Python name and see if it is different to the C++ name. */
    pyname = getPythonName(mod, of, classBaseName(cd));

    cd->pyname = NULL;
    checkAttributes(pt, mod, cd->ecd, NULL, pyname, FALSE);
    cd->pyname = cacheName(pt, pyname);
    cd->no_typehint = getNoTypeHint(of);

    if ((flg = getOptFlag(of, "Metatype", dotted_name_flag)) != NULL)
        cd->metatype = cacheName(pt, flg->fvalue.sval);

    if ((flg = getOptFlag(of, "Supertype", dotted_name_flag)) != NULL)
        cd->supertype = cacheName(pt, flg->fvalue.sval);

    if (getOptFlag(of, "ExportDerived", bool_flag) != NULL)
        setExportDerived(cd);

    if (getOptFlag(of, "Mixin", bool_flag) != NULL)
        setMixin(cd);

    if ((flg = getOptFlag(of, "FileExtension", string_flag)) != NULL)
        cd->iff->file_extension = flg->fvalue.sval;

    if ((flg = getOptFlag(of, "PyQtFlagsEnums", string_list_flag)) != NULL)
    {
        cd->pyqt_flags_enums = flg->fvalue.slval;
        cd->pyqt_flags = 1;
    }

    /* This is deprecated and only used by versions before v5.12. */
    if ((flg = getOptFlag(of, "PyQtFlags", integer_flag)) != NULL)
        cd->pyqt_flags = flg->fvalue.ival;

    if (getOptFlag(of, "PyQtNoQMetaObject", bool_flag) != NULL)
        setPyQtNoQMetaObject(cd);

    if ((flg = getOptFlag(of, "PyQtInterface", string_flag)) != NULL)
        cd->pyqt_interface = flg->fvalue.sval;

    if (isOpaque(cd))
    {
        if (getOptFlag(of, "External", bool_flag) != NULL)
            setIsExternal(cd);
    }
    else
    {
        int seq_might, seq_not, default_to_sequence;
        memberDef *md;

        if (getOptFlag(of, "NoDefaultCtors", bool_flag) != NULL)
            setNoDefaultCtors(cd);

        if (cd -> ctors == NULL)
        {
            if (!noDefaultCtors(cd))
            {
                /* Provide a default ctor. */

                cd->ctors = sipMalloc(sizeof (ctorDef));
 
                cd->ctors->ctorflags = SECT_IS_PUBLIC;
                cd->ctors->pysig.result.atype = void_type;
                cd->ctors->cppsig = &cd->ctors->pysig;

                cd->defctor = cd->ctors;

                setCanCreate(cd);
            }
        }
        else if (cd -> defctor == NULL)
        {
            ctorDef *ct, *last = NULL;

            for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
            {
                if (!isPublicCtor(ct))
                    continue;

                if (ct -> pysig.nrArgs == 0 || ct -> pysig.args[0].defval != NULL)
                {
                    cd -> defctor = ct;
                    break;
                }

                if (last == NULL)
                    last = ct;
            }

            /* The last resort is the first public ctor. */
            if (cd->defctor == NULL)
                cd->defctor = last;
        }

        if (getDeprecated(of))
            setIsDeprecatedClass(cd);

        if (cd->convtocode != NULL && getAllowNone(of))
            setClassHandlesNone(cd);

        if (getOptFlag(of,"Abstract",bool_flag) != NULL)
        {
            setIsAbstractClass(cd);
            setIsIncomplete(cd);
            resetCanCreate(cd);
        }

        /* We assume a public dtor if nothing specific was provided. */
        if (!isDtor(cd))
            setIsPublicDtor(cd);

        if (getOptFlag(of, "DelayDtor", bool_flag) != NULL)
        {
            setIsDelayedDtor(cd);
            setHasDelayedDtors(mod);
        }

        /*
         * There are subtle differences between the add and concat methods and
         * the multiply and repeat methods.  The number versions can have their
         * operands swapped and may return NotImplemented.  If the user has
         * used the /Numeric/ annotation or there are other numeric operators
         * then we use add/multiply.  Otherwise, if the user has used the
         * /Sequence/ annotation or there are indexing operators then we use
         * concat/repeat.
         */
        seq_might = seq_not = FALSE;

        for (md = cd->members; md != NULL; md = md->next)
            switch (md->slot)
            {
            case getitem_slot:
            case setitem_slot:
            case delitem_slot:
                /* This might be a sequence. */
                seq_might = TRUE;
                break;

            case sub_slot:
            case isub_slot:
            case div_slot:
            case idiv_slot:
            case mod_slot:
            case imod_slot:
            case floordiv_slot:
            case ifloordiv_slot:
            case truediv_slot:
            case itruediv_slot:
            case pos_slot:
            case neg_slot:
                /* This is definately not a sequence. */
                seq_not = TRUE;
                break;

            /* Suppress a compiler warning. */
            default:
                ;
            }

        default_to_sequence = (!seq_not && seq_might);

        for (md = cd->members; md != NULL; md = md->next)
        {
            /* Ignore if it is explicitly numeric. */
            if (isNumeric(md))
                continue;

            if (isSequence(md) || default_to_sequence)
                switch (md->slot)
                {
                case add_slot:
                    md->slot = concat_slot;
                    break;

                case iadd_slot:
                    md->slot = iconcat_slot;
                    break;

                case mul_slot:
                    md->slot = repeat_slot;
                    break;

                case imul_slot:
                    md->slot = irepeat_slot;
                    break;

                /* Suppress a compiler warning. */
                default:
                    ;
                }
        }
    }

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsUsedName(cd->pyname);
    }
}


/*
 * Return the encoded name of a template (ie. including its argument types) as
 * a scoped name.
 */
scopedNameDef *encodedTemplateName(templateDef *td)
{
    int a;
    scopedNameDef *snd;

    snd = copyScopedName(td->fqname);

    for (a = 0; a < td->types.nrArgs; ++a)
    {
        char buf[50];
        int flgs;
        scopedNameDef *arg_snd;
        argDef *ad = &td->types.args[a];

        flgs = 0;

        if (isConstArg(ad))
            flgs += 1;

        if (isReference(ad))
            flgs += 2;

        /* We use numbers so they don't conflict with names. */
        sprintf(buf, "%02d%d%d", ad->atype, flgs, ad->nrderefs);

        switch (ad->atype)
        {
        case defined_type:
            arg_snd = copyScopedName(ad->u.snd);
            break;

        case template_type:
            arg_snd = encodedTemplateName(ad->u.td);
            break;

        case struct_type:
            arg_snd = copyScopedName(ad->u.sname);
            break;

        default:
            arg_snd = NULL;
        }

        /*
         * Replace the first element of the argument name with a copy with the
         * encoding prepended.
         */
        if (arg_snd != NULL)
            arg_snd->name = concat(buf, arg_snd->name, NULL);
        else
            arg_snd = text2scopePart(sipStrdup(buf));

        appendScopedName(&snd, arg_snd);
    }

    return snd;
}


/*
 * Create a new mapped type.
 */
static mappedTypeDef *newMappedType(sipSpec *pt, argDef *ad, optFlags *of)
{
    mappedTypeDef *mtd;
    scopedNameDef *snd;
    ifaceFileDef *iff;
    const char *cname;

    /* Check that the type is one we want to map. */
    switch (ad->atype)
    {
    case defined_type:
        snd = ad->u.snd = fullyQualifiedName(ad->u.snd);
        cname = scopedNameTail(snd);
        break;

    case template_type:
        ad->u.td->fqname = fullyQualifiedName(ad->u.td->fqname);
        snd = encodedTemplateName(ad->u.td);
        cname = NULL;
        break;

    case struct_type:
        snd = ad->u.sname = fullyQualifiedName(ad->u.sname);
        cname = scopedNameTail(snd);
        break;

    default:
        yyerror("Invalid type for %MappedType");
    }

    iff = findIfaceFile(pt, currentModule, snd, mappedtype_iface,
            getAPIRange(of), ad);

    /* Check it hasn't already been defined. */
    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff == iff)
        {
            /*
             * We allow types based on the same template but with different
             * arguments.
             */
            if (ad->atype != template_type || sameBaseType(ad, &mtd->type))
                yyerror("Mapped type has already been defined in this module");
        }

    /* The module may not have been set yet. */
    iff->module = currentModule;

    /* Create a new mapped type. */
    mtd = allocMappedType(pt, ad);

    if (cname != NULL)
        mtd->pyname = cacheName(pt, getPythonName(currentModule, of, cname));

    mappedTypeAnnos(mtd, of);

    mtd->iff = iff;
    mtd->next = pt->mappedtypes;

    pt->mappedtypes = mtd;

    if (inMainModule())
    {
        setIsUsedName(mtd->cname);

        if (mtd->pyname)
            setIsUsedName(mtd->pyname);
    }

    return mtd;
}


/*
 * Allocate, initialise and return a mapped type structure.
 */
mappedTypeDef *allocMappedType(sipSpec *pt, argDef *type)
{
    mappedTypeDef *mtd;

    mtd = sipMalloc(sizeof (mappedTypeDef));

    mtd->type = *type;
    mtd->type.argflags = 0;
    mtd->type.nrderefs = 0;

    mtd->cname = cacheName(pt, type2string(&mtd->type));

    /* Keep track of the original definition as it gets copied. */
    mtd->real = mtd;

    return mtd;
}


/*
 * Create a new enum.
 */
static enumDef *newEnum(sipSpec *pt, moduleDef *mod, mappedTypeDef *mt_scope,
        char *name, optFlags *of, int flags, int isscoped)
{
    enumDef *ed, *first_alt, *next_alt;
    classDef *c_scope;
    ifaceFileDef *scope;

    if (mt_scope != NULL)
    {
        scope = mt_scope->iff;
        c_scope = NULL;
    }
    else
    {
        if ((c_scope = currentScope()) != NULL)
            scope = c_scope->iff;
        else
            scope = NULL;
    }

    ed = sipMalloc(sizeof (enumDef));

    /* Assume the enum isn't versioned. */
    first_alt = ed;
    next_alt = NULL;

    if (name != NULL)
    {
        ed->pyname = cacheName(pt, getPythonName(mod, of, name));
        checkAttributes(pt, mod, c_scope, mt_scope, ed->pyname->text, FALSE);

        ed->fqcname = text2scopedName(scope, name);
        ed->cname = cacheName(pt, scopedNameToString(ed->fqcname));

        if (inMainModule())
        {
            setIsUsedName(ed->pyname);
            setIsUsedName(ed->cname);
        }

        /* If the scope is versioned then look for any alternate. */
        if (scope != NULL && scope->api_range != NULL)
        {
            enumDef *alt;

            for (alt = pt->enums; alt != NULL; alt = alt->next)
            {
                if (alt->module != mod || alt->fqcname == NULL)
                    continue;

                if (compareScopedNames(alt->fqcname, ed->fqcname) == 0)
                {
                    first_alt = alt->first_alt;
                    next_alt = first_alt->next_alt;
                    first_alt->next_alt = ed;

                    break;
                }
            }
        }
    }
    else
    {
        ed->pyname = NULL;
        ed->fqcname = NULL;
        ed->cname = NULL;
    }

    if (flags & SECT_IS_PROT)
    {
        if (makeProtPublic)
        {
            flags &= ~SECT_IS_PROT;
            flags |= SECT_IS_PUBLIC;
        }
        else if (c_scope != NULL)
        {
            setNeedsShadow(c_scope);
        }
    }

    ed->enumflags = flags;
    ed->no_typehint = getNoTypeHint(of);
    ed->enumnr = -1;
    ed->ecd = c_scope;
    ed->emtd = mt_scope;
    ed->first_alt = first_alt;
    ed->next_alt = next_alt;
    ed->module = mod;
    ed->members = NULL;
    ed->slots = NULL;
    ed->overs = NULL;
    ed->platforms = currentPlatforms;
    ed->next = pt -> enums;

    pt->enums = ed;

    if (getOptFlag(of, "NoScope", bool_flag) != NULL)
        setIsNoScope(ed);

    if (isscoped)
        setIsScopedEnum(ed);

    return ed;
}


/*
 * Get the type values and (optionally) the type names for substitution in
 * handwritten code.
 */
void appendTypeStrings(scopedNameDef *ename, signatureDef *patt, signatureDef *src, signatureDef *known, scopedNameDef **names, scopedNameDef **values)
{
    int a;

    for (a = 0; a < patt->nrArgs; ++a)
    {
        argDef *pad = &patt->args[a];

        if (pad->atype == defined_type)
        {
            char *nam = NULL, *val;
            argDef *sad;

            /*
             * If the type names are already known then check that this is one
             * of them.
             */
            if (known == NULL)
                nam = scopedNameTail(pad->u.snd);
            else if (pad->u.snd->next == NULL)
            {
                int k;

                for (k = 0; k < known->nrArgs; ++k)
                {
                    /* Skip base types. */
                    if (known->args[k].atype != defined_type)
                        continue;

                    if (strcmp(pad->u.snd->name, known->args[k].u.snd->name) == 0)
                    {
                        nam = pad->u.snd->name;
                        break;
                    }
                }
            }

            if (nam == NULL)
                continue;

            /* Add the name. */
            appendScopedName(names, text2scopePart(nam));

            /*
             * Add the corresponding value.  For defined types we don't want 
             * any indirection or references.
             */
            sad = &src->args[a];

            if (sad->atype == defined_type)
                val = scopedNameToString(sad->u.snd);
            else
                val = type2string(sad);

            /* We do want const. */
            if (isConstArg(sad))
            {
                char *const_val = sipStrdup("const ");

                append(&const_val, val);
                free(val);

                val = const_val;
            }

            appendScopedName(values, text2scopePart(val));
        }
        else if (pad->atype == template_type)
        {
            argDef *sad = &src->args[a];

            /* These checks shouldn't be necessary, but... */
            if (sad->atype == template_type && pad->u.td->types.nrArgs == sad->u.td->types.nrArgs)
                appendTypeStrings(ename, &pad->u.td->types, &sad->u.td->types, known, names, values);
        }
    }
}


/*
 * Convert a type to a string on the heap.  The string will use the minimum
 * whitespace while still remaining valid C++.
 */
static char *type2string(argDef *ad)
{
    int i, on_heap = FALSE;
    int nr_derefs = ad->nrderefs;
    int is_reference = isReference(ad);
    char *s;

    /* Use the original type if possible. */
    if (ad->original_type != NULL && !noTypeName(ad->original_type))
    {
        s = scopedNameToString(ad->original_type->fqname);
        on_heap = TRUE;

        nr_derefs -= ad->original_type->type.nrderefs;

        if (isReference(&ad->original_type->type))
            is_reference = FALSE;
    }
    else
        switch (ad->atype)
        {
        case template_type:
            {
                templateDef *td = ad->u.td;

                s = scopedNameToString(td->fqname);
                append(&s, "<");

                for (i = 0; i < td->types.nrArgs; ++i)
                {
                    char *sub_type = type2string(&td->types.args[i]);

                    if (i > 0)
                        append(&s, ",");

                    append(&s, sub_type);
                    free(sub_type);
                }

                if (s[strlen(s) - 1] == '>')
                    append(&s, " >");
                else
                    append(&s, ">");

                on_heap = TRUE;
                break;
            }

        case struct_type:
            s = scopedNameToString(ad->u.sname);
            on_heap = TRUE;
            break;

        case defined_type:
            s = scopedNameToString(ad->u.snd);
            on_heap = TRUE;
            break;

        case ubyte_type:
        case ustring_type:
            s = "unsigned char";
            break;

        case byte_type:
        case ascii_string_type:
        case latin1_string_type:
        case utf8_string_type:
        case string_type:
            s = "char";
            break;

        case sbyte_type:
        case sstring_type:
            s = "signed char";
            break;

        case wstring_type:
            s = "wchar_t";
            break;

        case ushort_type:
            s = "unsigned short";
            break;

        case short_type:
            s = "short";
            break;

        case uint_type:
            s = "uint";
            break;

        case int_type:
        case cint_type:
            s = "int";
            break;

        case ulong_type:
            s = "unsigned long";
            break;

        case long_type:
            s = "long";
            break;

        case ulonglong_type:
            s = "unsigned long long";
            break;

        case longlong_type:
            s = "long long";
            break;

        case float_type:
        case cfloat_type:
            s = "float";
            break;

        case double_type:
        case cdouble_type:
            s = "double";
            break;

        case bool_type:
        case cbool_type:
            s = "bool";
            break;

        case void_type:
            s = "void";
            break;

        case capsule_type:
            s = "void *";
            break;

        case size_type:
            s = "size_t";
            break;

        default:
            fatal("Unsupported type argument to type2string(): %d\n", ad->atype);
        }

    /* Make sure the string is on the heap. */
    if (!on_heap)
        s = sipStrdup(s);

    while (nr_derefs-- > 0)
        append(&s, "*");

    if (is_reference)
        append(&s, "&");

    return s;
}


/*
 * Remove any explicit global scope.
 */
scopedNameDef *removeGlobalScope(scopedNameDef *snd)
{
    return ((snd != NULL && snd->name[0] == '\0') ? snd->next : snd);
}


/*
 * Convert a scoped name to a string on the heap.
 */
static char *scopedNameToString(scopedNameDef *name)
{
    static const char scope_string[] = "::";
    size_t len;
    scopedNameDef *snd;
    char *s, *dp;

    /*
     * We don't want the global scope (which probably should always be there,
     * but we check anyway).
     */
    name = removeGlobalScope(name);

    /* Work out the length of buffer needed. */
    len = 0;

    for (snd = name; snd != NULL; snd = snd->next)
    {
        len += strlen(snd->name);

        if (snd->next != NULL)
        {
            /* Ignore the encoded part of template names. */
            if (isdigit(snd->next->name[0]))
                break;

            len += strlen(scope_string);
        }
    }

    /* Allocate and populate the buffer. */
    dp = s = sipMalloc(len + 1);

    for (snd = name; snd != NULL; snd = snd->next)
    {
        strcpy(dp, snd->name);
        dp += strlen(snd->name);

        if (snd->next != NULL)
        {
            /* Ignore the encoded part of template names. */
            if (isdigit(snd->next->name[0]))
                break;

            strcpy(dp, scope_string);
            dp += strlen(scope_string);
        }
    }

    return s;
}


/*
 * Instantiate a class template.
 */
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod,
        classDef *scope, scopedNameDef *fqname, classTmplDef *tcd,
        templateDef *td, const char *pyname, int use_template_name,
        docstringDef *docstring)
{
    scopedNameDef *type_names, *type_values;
    classDef *cd;
    ctorDef *oct, **cttail;
    argDef *ad;
    ifaceFileList *iffl, **used;
    classList *cl;
    stringList *sl;

    type_names = type_values = NULL;
    appendTypeStrings(classFQCName(tcd->cd), &tcd->sig, &td->types, NULL, &type_names, &type_values);

    /*
     * Add a mapping from the template name to the instantiated name.  If we
     * have got this far we know there is room for it.
     */
    ad = &tcd->sig.args[tcd->sig.nrArgs++];
    memset(ad, 0, sizeof (argDef));
    ad->atype = defined_type;
    ad->u.snd = classFQCName(tcd->cd);

    appendScopedName(&type_names, text2scopePart(scopedNameTail(classFQCName(tcd->cd))));
    appendScopedName(&type_values, text2scopePart(scopedNameToString(fqname)));

    /* Create the new class. */
    cd = sipMalloc(sizeof (classDef));

    /* Start with a shallow copy. */
    *cd = *tcd->cd;

    if (docstring != NULL)
        cd->docstring = docstring;

    resetIsTemplateClass(cd);
    cd->pyname = cacheName(pt, pyname);
    cd->td = td;

    if (use_template_name)
        setUseTemplateName(cd);

    /* Handle the interface file. */
    cd->iff = findIfaceFile(pt, mod, fqname, class_iface,
            (scope != NULL ? scope->iff->api_range : NULL), NULL);
    cd->iff->module = mod;

    appendCodeBlockList(&cd->iff->hdrcode, tcd->cd->iff->hdrcode);

    /* Make a copy of the used list and add the enclosing scope. */
    used = &cd->iff->used;

    for (iffl = tcd->cd->iff->used; iffl != NULL; iffl = iffl->next)
        appendToIfaceFileList(used, iffl->iff);

    /* Include any scope header code. */
    if (scope != NULL)
        appendCodeBlockList(&cd->iff->hdrcode, scope->iff->hdrcode);

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsUsedName(cd->pyname);
    }

    cd->ecd = currentScope();

    /* Handle any type hints. */
    if (cd->typehint_in != NULL)
        cd->typehint_in = newTypeHint(
                templateString(cd->typehint_in->raw_hint, type_names,
                        type_values));

    if (cd->typehint_out != NULL)
        cd->typehint_out = newTypeHint(
                templateString(cd->typehint_out->raw_hint, type_names,
                        type_values));

    /* Handle any flagged enums. */
    if ((sl = cd->pyqt_flags_enums) != NULL)
    {
        cd->pyqt_flags_enums = NULL;

        do
        {
            appendString(&cd->pyqt_flags_enums,
                    templateString(sl->s, type_names, type_values));
            sl = sl->next;
        }
        while (sl != NULL);
    }

    /* Handle the super-classes. */
    for (cl = cd->supers; cl != NULL; cl = cl->next)
    {
        int a;
        scopedNameDef *unscoped;

        unscoped = removeGlobalScope(cl->cd->iff->fqcname);

        /* Ignore defined or scoped classes. */
        if (cl->cd->iff->module != NULL || unscoped->next != NULL)
            continue;

        for (a = 0; a < tcd->sig.nrArgs - 1; ++a)
            if (strcmp(unscoped->name, scopedNameTail(tcd->sig.args[a].u.snd)) == 0)
            {
                argDef *tad = &td->types.args[a];
                classDef *icd;

                if (tad->atype == defined_type)
                    icd = findClass(pt, class_iface, NULL, tad->u.snd, FALSE);
                else if (tad->atype == class_type)
                    icd = tad->u.cd;
                else
                    fatal("Template argument %s must expand to a class\n",
                            unscoped->name);

                cl->cd = icd;
            }
    }

    /* Handle the enums. */
    instantiateTemplateEnums(pt, tcd, td, cd, used, type_names, type_values);

    /* Handle the variables. */
    instantiateTemplateVars(pt, tcd, td, cd, used, type_names, type_values);

    /* Handle the typedefs. */
    instantiateTemplateTypedefs(pt, tcd, td, cd, type_names, type_values);

    /* Handle the ctors. */
    cd->ctors = NULL;
    cttail = &cd->ctors;

    for (oct = tcd->cd->ctors; oct != NULL; oct = oct->next)
    {
        ctorDef *nct = sipMalloc(sizeof (ctorDef));

        /* Start with a shallow copy. */
        *nct = *oct;

        templateSignature(&nct->pysig, FALSE, tcd, td, cd, type_names,
                type_values);

        if (oct->cppsig == NULL)
            nct->cppsig = NULL;
        else if (oct->cppsig == &oct->pysig)
            nct->cppsig = &nct->pysig;
        else
        {
            nct->cppsig = sipMalloc(sizeof (signatureDef));

            *nct->cppsig = *oct->cppsig;

            templateSignature(nct->cppsig, FALSE, tcd, td, cd, type_names,
                    type_values);
        }

        nct->methodcode = templateCode(pt, used, nct->methodcode, type_names, type_values);
        nct->premethodcode = templateCode(pt, used, nct->premethodcode, type_names, type_values);

        nct->next = NULL;
        *cttail = nct;
        cttail = &nct->next;

        /* Handle the default ctor. */
        if (tcd->cd->defctor == oct)
            cd->defctor = nct;
    }

    cd->dealloccode = templateCode(pt, used, cd->dealloccode, type_names, type_values);
    cd->dtorcode = templateCode(pt, used, cd->dtorcode, type_names, type_values);

    /* Handle the methods. */
    cd->members = instantiateTemplateMethods(tcd->cd->members, mod);
    cd->overs = instantiateTemplateOverloads(pt, tcd->cd->overs,
            tcd->cd->members, cd->members, tcd, td, cd, used, type_names,
            type_values);

    cd->cppcode = templateCode(pt, used, cd->cppcode, type_names, type_values);
    cd->iff->hdrcode = templateCode(pt, used, cd->iff->hdrcode, type_names, type_values);
    cd->convtosubcode = templateCode(pt, used, cd->convtosubcode, type_names, type_values);
    cd->convtocode = templateCode(pt, used, cd->convtocode, type_names, type_values);
    cd->travcode = templateCode(pt, used, cd->travcode, type_names, type_values);
    cd->clearcode = templateCode(pt, used, cd->clearcode, type_names, type_values);
    cd->getbufcode = templateCode(pt, used, cd->getbufcode, type_names, type_values);
    cd->releasebufcode = templateCode(pt, used, cd->releasebufcode, type_names, type_values);
    cd->readbufcode = templateCode(pt, used, cd->readbufcode, type_names, type_values);
    cd->writebufcode = templateCode(pt, used, cd->writebufcode, type_names, type_values);
    cd->segcountcode = templateCode(pt, used, cd->segcountcode, type_names, type_values);
    cd->charbufcode = templateCode(pt, used, cd->charbufcode, type_names, type_values);
    cd->instancecode = templateCode(pt, used, cd->instancecode, type_names, type_values);
    cd->picklecode = templateCode(pt, used, cd->picklecode, type_names, type_values);
    cd->finalcode = templateCode(pt, used, cd->finalcode, type_names, type_values);
    cd->typehintcode = templateCode(pt, used, cd->typehintcode, type_names, type_values);
    cd->next = pt->classes;

    pt->classes = cd;

    tcd->sig.nrArgs--;

    freeScopedName(type_names);
    freeScopedName(type_values);
}


/*
 * Instantiate the methods of a template class.
 */
static memberDef *instantiateTemplateMethods(memberDef *tmd, moduleDef *mod)
{
    memberDef *md, *methods, **mdtail;

    methods = NULL;
    mdtail = &methods;

    for (md = tmd; md != NULL; md = md->next)
    {
        memberDef *nmd = sipMalloc(sizeof (memberDef));

        /* Start with a shallow copy. */
        *nmd = *md;

        nmd->module = mod;

        if (inMainModule())
            setIsUsedName(nmd->pyname);

        nmd->next = NULL;
        *mdtail = nmd;
        mdtail = &nmd->next;
    }

    return methods;
}


/*
 * Instantiate the overloads of a template class.
 */
static overDef *instantiateTemplateOverloads(sipSpec *pt, overDef *tod,
        memberDef *tmethods, memberDef *methods, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    overDef *od, *overloads, **odtail;

    overloads = NULL;
    odtail = &overloads;

    for (od = tod; od != NULL; od = od->next)
    {
        overDef *nod = sipMalloc(sizeof (overDef));
        memberDef *nmd, *omd;

        /* Start with a shallow copy. */
        *nod = *od;

        for (nmd = methods, omd = tmethods; omd != NULL; omd = omd->next, nmd = nmd->next)
            if (omd == od->common)
            {
                nod->common = nmd;
                break;
            }

        templateSignature(&nod->pysig, TRUE, tcd, td, cd, type_names,
                type_values);

        if (od->cppsig == &od->pysig)
            nod->cppsig = &nod->pysig;
        else
        {
            nod->cppsig = sipMalloc(sizeof (signatureDef));

            *nod->cppsig = *od->cppsig;

            templateSignature(nod->cppsig, TRUE, tcd, td, cd, type_names,
                    type_values);
        }

        nod->methodcode = templateCode(pt, used, nod->methodcode, type_names, type_values);
        nod->premethodcode = templateCode(pt, used, nod->premethodcode, type_names, type_values);
        nod->virtcallcode = templateCode(pt, used, nod->virtcallcode, type_names, type_values);
        nod->virtcode = templateCode(pt, used, nod->virtcode, type_names, type_values);

        nod->next = NULL;
        *odtail = nod;
        odtail = &nod->next;
    }

    return overloads;
}


/*
 * Instantiate the enums of a template class.
 */
static void instantiateTemplateEnums(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    enumDef *ted;
    moduleDef *mod = cd->iff->module;

    for (ted = pt->enums; ted != NULL; ted = ted->next)
        if (ted->ecd == tcd->cd)
        {
            enumDef *ed;
            enumMemberDef *temd;

            ed = sipMalloc(sizeof (enumDef));

            /* Start with a shallow copy. */
            *ed = *ted;

            if (ed->fqcname != NULL)
            {
                ed->fqcname = text2scopedName(cd->iff,
                        scopedNameTail(ed->fqcname));
                ed->cname = cacheName(pt, scopedNameToString(ed->fqcname));
            }

            if (inMainModule())
            {
                if (ed->pyname != NULL)
                    setIsUsedName(ed->pyname);

                if (ed->cname != NULL)
                    setIsUsedName(ed->cname);
            }

            ed->ecd = cd;
            ed->first_alt = ed;
            ed->module = mod;
            ed->members = NULL;

            for (temd = ted->members; temd != NULL; temd = temd->next)
            {
                enumMemberDef *emd;

                emd = sipMalloc(sizeof (enumMemberDef));

                /* Start with a shallow copy. */
                *emd = *temd;
                emd->ed = ed;

                emd->next = ed->members;
                ed->members = emd;
            }

            ed->slots = instantiateTemplateMethods(ted->slots, mod);
            ed->overs = instantiateTemplateOverloads(pt, ted->overs,
                    ted->slots, ed->slots, tcd, td, cd, used, type_names,
                    type_values);

            ed->next = pt->enums;
            pt->enums = ed;
        }
}


/*
 * Instantiate the variables of a template class.
 */
static void instantiateTemplateVars(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    varDef *tvd;

    for (tvd = pt->vars; tvd != NULL; tvd = tvd->next)
        if (tvd->ecd == tcd->cd)
        {
            varDef *vd;

            vd = sipMalloc(sizeof (varDef));

            /* Start with a shallow copy. */
            *vd = *tvd;

            if (inMainModule())
                setIsUsedName(vd->pyname);

            vd->fqcname = text2scopedName(cd->iff,
                    scopedNameTail(vd->fqcname));
            vd->ecd = cd;
            vd->module = cd->iff->module;

            templateType(&vd->type, tcd, td, cd, type_names, type_values);

            vd->accessfunc = templateCode(pt, used, vd->accessfunc, type_names, type_values);
            vd->getcode = templateCode(pt, used, vd->getcode, type_names, type_values);
            vd->setcode = templateCode(pt, used, vd->setcode, type_names, type_values);

            addVariable(pt, vd);
        }
}


/*
 * Instantiate the typedefs of a template class.
 */
static void instantiateTemplateTypedefs(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, scopedNameDef *type_names,
        scopedNameDef *type_values)
{
    typedefDef *tdd;

    for (tdd = pt->typedefs; tdd != NULL; tdd = tdd->next)
    {
        typedefDef *new_tdd;

        if (tdd->ecd != tcd->cd)
            continue;

        new_tdd = sipMalloc(sizeof (typedefDef));

        /* Start with a shallow copy. */
        *new_tdd = *tdd;

        new_tdd->fqname = text2scopedName(cd->iff,
                scopedNameTail(new_tdd->fqname));
        new_tdd->ecd = cd;
        new_tdd->module = cd->iff->module;

        templateType(&new_tdd->type, tcd, td, cd, type_names, type_values);

        addTypedef(pt, new_tdd);
    }
}


/*
 * Replace any template arguments in a signature.
 */
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd,
        templateDef *td, classDef *ncd, scopedNameDef *type_names,
        scopedNameDef *type_values)
{
    int a;

    if (result)
        templateType(&sd->result, tcd, td, ncd, type_names, type_values);

    for (a = 0; a < sd->nrArgs; ++a)
        templateType(&sd->args[a], tcd, td, ncd, type_names, type_values);
}


/*
 * Replace any template arguments in a type.
 */
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td,
        classDef *ncd, scopedNameDef *type_names, scopedNameDef *type_values)
{
    int a;
    char *name;

    /* Descend into any sub-templates. */
    if (ad->atype == template_type)
    {
        templateDef *new_td = sipMalloc(sizeof (templateDef));

        /* Make a deep copy of the template definition. */
        *new_td = *ad->u.td;
        ad->u.td = new_td;

        templateSignature(&ad->u.td->types, FALSE, tcd, td, ncd, type_names,
                type_values);

        return;
    }

    /* Handle any default value. */
    if (ad->defval != NULL && ad->defval->vtype == fcall_value)
    {
        /*
         * We only handle the subset where the value is an function call, ie. a
         * template ctor.
         */
        valueDef *vd = ad->defval;

        if (vd->vtype == fcall_value && vd->u.fcd->type.atype == defined_type)
        {
            valueDef *new_vd;
            fcallDef *fcd;
            scopedNameDef *snd, **tailp;

            fcd = sipMalloc(sizeof (fcallDef));
            *fcd = *vd->u.fcd;

            tailp = &fcd->type.u.snd;
            for (snd = vd->u.fcd->type.u.snd; snd != NULL; snd = snd->next)
            {
                *tailp = text2scopePart(
                        templateString(snd->name, type_names, type_values));
                tailp = &(*tailp)->next;
            }

            new_vd = sipMalloc(sizeof (valueDef));
            new_vd->vtype = fcall_value;
            new_vd->u.fcd = fcd;

            ad->defval = new_vd;
        }
    }

    /* Handle any type hints. */
    if (ad->typehint_in != NULL)
        ad->typehint_in = newTypeHint(
                templateString(ad->typehint_in->raw_hint, type_names,
                        type_values));

    if (ad->typehint_out != NULL)
        ad->typehint_out = newTypeHint(
                templateString(ad->typehint_out->raw_hint, type_names,
                        type_values));

    /* Ignore if it isn't an unscoped name. */
    if (ad->atype != defined_type || ad->u.snd->next != NULL)
        return;

    name = ad->u.snd->name;

    for (a = 0; a < tcd->sig.nrArgs - 1; ++a)
        if (strcmp(name, scopedNameTail(tcd->sig.args[a].u.snd)) == 0)
        {
            argDef *tad = &td->types.args[a];

            ad->atype = tad->atype;

            /* We take the constrained flag from the real type. */
            resetIsConstrained(ad);

            if (isConstrained(tad))
                setIsConstrained(ad);

            ad->u = tad->u;

            return;
        }

    /* Handle the class name itself. */
    if (strcmp(name, scopedNameTail(classFQCName(tcd->cd))) == 0)
    {
        ad->atype = class_type;
        ad->u.cd = ncd;
        ad->original_type = NULL;
    }
}


/*
 * Replace any template arguments in a literal code block.
 */
codeBlockList *templateCode(sipSpec *pt, ifaceFileList **used,
        codeBlockList *ocbl, scopedNameDef *names, scopedNameDef *values)
{
    codeBlockList *ncbl = NULL;

    while (ocbl != NULL)
    {
        char *at = ocbl->block->frag;
        int start_of_line = TRUE;

        do
        {
            char *from = at, *first = NULL;
            codeBlock *cb;
            scopedNameDef *nam, *val, *nam_first, *val_first;

            /* Suppress a compiler warning. */
            val_first = NULL;

            /*
             * Don't do any substitution in lines that appear to be
             * preprocessor directives.  This prevents #include'd file names
             * being broken.
             */
            if (start_of_line)
            {
                /* Strip leading whitespace. */
                while (isspace(*from))
                    ++from;

                if (*from == '#')
                {
                    /* Skip to the end of the line. */
                    do
                        ++from;
                    while (*from != '\n' && *from != '\0');
                }
                else
                {
                    start_of_line = FALSE;
                }
            }

            /*
             * Go through the rest of this fragment looking for each of the
             * types and the name of the class itself.
             */
            nam = names;
            val = values;

            while (nam != NULL && val != NULL)
            {
                char *cp;

                if ((cp = strstr(from, nam->name)) != NULL)
                    if (first == NULL || first > cp)
                    {
                        nam_first = nam;
                        val_first = val;
                        first = cp;
                    }

                nam = nam->next;
                val = val->next;
            }

            /* Create the new fragment. */
            cb = sipMalloc(sizeof (codeBlock));

            if (at == ocbl->block->frag)
            {
                cb->filename = ocbl->block->filename;
                cb->linenr = ocbl->block->linenr;
            }
            else
                cb->filename = NULL;

            appendCodeBlock(&ncbl, cb);

            /* See if anything was found. */
            if (first == NULL)
            {
                /* We can just point to this. */
                cb->frag = at;

                /* All done with this one. */
                at = NULL;
            }
            else
            {
                static char *gen_names[] = {
                    "sipType_",
                    "sipException_",
                    NULL
                };

                char *dp, *sp, **gn;
                int genname = FALSE;

                /*
                 * If the context in which the text is used is in the name of a
                 * SIP generated object then translate any "::" scoping to "_"
                 * and remove any const.
                 */
                for (gn = gen_names; *gn != NULL; ++gn)
                    if (search_back(first, at, *gn))
                    {
                        addUsedFromCode(pt, used, val_first->name);
                        genname = TRUE;
                        break;
                    }

                /* Fragment the fragment. */
                cb->frag = sipMalloc(first - at + strlen(val_first->name) + 1);

                strncpy(cb->frag, at, first - at);

                dp = &cb->frag[first - at];
                sp = val_first->name;

                if (genname)
                {
                    char gch;

                    if (strlen(sp) > 6 && strncmp(sp, "const ", 6) == 0)
                        sp += 6;

                    while ((gch = *sp++) != '\0')
                        if (gch == ':' && *sp == ':')
                        {
                            *dp++ = '_';
                            ++sp;
                        }
                        else
                            *dp++ = gch;

                    *dp = '\0';
                }
                else
                    strcpy(dp, sp);

                /* Move past the replaced text. */
                at = first + strlen(nam_first->name);

                if (*at == '\n')
                    start_of_line = TRUE;
            }
        }
        while (at != NULL && *at != '\0');

        ocbl = ocbl->next;
    }

    return ncbl;
}


/*
 * Return TRUE if the text at the end of a string matches the target string.
 */
static int search_back(const char *end, const char *start, const char *target)
{
    size_t tlen = strlen(target);

    if (start + tlen >= end)
        return FALSE;

    return (strncmp(end - tlen, target, tlen) == 0);
}


/*
 * Add any needed interface files based on handwritten code.
 */
static void addUsedFromCode(sipSpec *pt, ifaceFileList **used, const char *sname)
{
    ifaceFileDef *iff;
    enumDef *ed;

    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
    {
        if (iff->type != class_iface && iff->type != exception_iface)
            continue;

        if (sameName(iff->fqcname, sname))
        {
            appendToIfaceFileList(used, iff);
            return;
        }
    }

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->ecd == NULL)
            continue;

        if (sameName(ed->fqcname, sname))
        {
            appendToIfaceFileList(used, ed->ecd->iff);
            return;
        }
    }
}


/*
 * Compare a scoped name with its string equivalent.
 */
static int sameName(scopedNameDef *snd, const char *sname)
{
    /* Handle any explicit scopes. */
    if (sname[0] == ':' && sname[1] == ':')
    {
        if (snd->name[0] != '\0')
            return FALSE;

        sname += 2;
    }

    snd = removeGlobalScope(snd);

    while (snd != NULL && *sname != '\0')
    {
        const char *sp = snd->name;

        while (*sp != '\0' && *sname != ':' && *sname != '\0')
            if (*sp++ != *sname++)
                return FALSE;

        if (*sp != '\0' || (*sname != ':' && *sname != '\0'))
            return FALSE;

        snd = snd->next;

        if (*sname == ':')
            sname += 2;
    }

    return (snd == NULL && *sname == '\0');
}


/*
 * Compare a (possibly) relative scoped name with a fully qualified scoped name
 * while taking the current scope into account.
 */
static int foundInScope(scopedNameDef *fq_name, scopedNameDef *rel_name)
{
    classDef *scope;

    for (scope = currentScope(); scope != NULL; scope = scope->ecd)
    {
        scopedNameDef *snd;
        int found;

        snd = copyScopedName(classFQCName(scope));
        appendScopedName(&snd, copyScopedName(rel_name));

        found = (compareScopedNames(fq_name, snd) == 0);

        freeScopedName(snd);

        if (found)
            return TRUE;
    }

    return compareScopedNames(fq_name, rel_name) == 0;
}


/*
 * Create a new typedef.
 */
static void newTypedef(sipSpec *pt, moduleDef *mod, char *name, argDef *type,
        optFlags *optflgs, docstringDef *docstring)
{
    int no_type_name;
    typedefDef *td;
    scopedNameDef *fqname;
    classDef *scope;

    scope = currentScope();
    fqname = text2scopedName((scope != NULL ? scope->iff : NULL), name);
    no_type_name = (getOptFlag(optflgs, "NoTypeName", bool_flag) != NULL);

    /* See if we are instantiating a template class. */
    if (type->atype == template_type)
    {
        classTmplDef *tcd;
        templateDef *td = type->u.td;

        for (tcd = pt->classtemplates; tcd != NULL; tcd = tcd->next)
            if (foundInScope(tcd->cd->iff->fqcname, td->fqname) &&
                sameTemplateSignature(&tcd->sig, &td->types, FALSE))
            {
                instantiateClassTemplate(pt, mod, scope, fqname, tcd, td,
                        getPythonName(mod, optflgs, name), no_type_name,
                        docstring);

                /* All done. */
                return;
            }
    }

    td = sipMalloc(sizeof (typedefDef));

    td->tdflags = 0;
    td->fqname = fqname;
    td->ecd = scope;
    td->module = mod;
    td->platforms = currentPlatforms;
    td->type = *type;

    if (getOptFlag(optflgs, "Capsule", bool_flag) != NULL)
    {
        /* Make sure the type is void *. */
        if (type->atype != void_type || type->nrderefs != 1 || isConstArg(type) || isReference(type))
        {
            fatalScopedName(fqname);
            fatal(" must be a void* if /Capsule/ is specified\n");
        }

        td->type.atype = capsule_type;
        td->type.nrderefs = 0;
        td->type.u.cap = fqname;
    }

    if (no_type_name)
        setNoTypeName(td);

    addTypedef(pt, td);
}


/*
 * Add a typedef to the list so that the list remains sorted.
 */
static void addTypedef(sipSpec *pt, typedefDef *tdd)
{
    typedefDef **tdp;

    /*
     * Check it doesn't already exist (with a strict parse) and find the
     * position in the sorted list where it should be put.
     */
    for (tdp = &pt->typedefs; *tdp != NULL; tdp = &(*tdp)->next)
    {
        int res = compareScopedNames((*tdp)->fqname, tdd->fqname);

        if (res == 0 && strictParse)
        {
            fatalScopedName(tdd->fqname);
            fatal(" already defined\n");
        }

        if (res >= 0)
            break;
    }

    tdd->next = *tdp;
    *tdp = tdd;

    tdd->module->nrtypedefs++;
}


/*
 * Speculatively try and resolve any typedefs.  In some cases (eg. when
 * comparing template signatures) it helps to use the real type if it is known.
 * Note that this wouldn't be necessary if we required that all types be known
 * before they are used.
 */
static void resolveAnyTypedef(sipSpec *pt, argDef *ad)
{
    argDef orig = *ad;

    while (ad->atype == defined_type)
    {
        ad->atype = no_type;
        searchTypedefs(pt, ad->u.snd, ad);

        /*
         * Don't resolve to a template type as it may be superceded later on
         * by a more specific mapped type.
         */
        if (ad->atype == no_type || ad->atype == template_type)
        {
            *ad = orig;
            break;
        }
    }
}


/*
 * Return TRUE if the template signatures are the same.  A deep comparison is
 * used for mapped type templates where we want to recurse into any nested
 * templates.
 */
int sameTemplateSignature(signatureDef *tmpl_sd, signatureDef *args_sd,
        int deep)
{
    int a;

    if (tmpl_sd->nrArgs != args_sd->nrArgs)
        return FALSE;

    for (a = 0; a < tmpl_sd->nrArgs; ++a)
    {
        argDef *tmpl_ad = &tmpl_sd->args[a];
        argDef *args_ad = &args_sd->args[a];

        /*
         * If we are doing a shallow comparision (ie. for class templates) then
         * a type name in the template signature matches anything in the
         * argument signature.
         */
        if (tmpl_ad->atype == defined_type && !deep)
            continue;

        /*
         * For type names only compare the references and pointers, and do the
         * same for any nested templates.
         */
        if (tmpl_ad->atype == defined_type && args_ad->atype == defined_type)
        {
            if (isReference(tmpl_ad) != isReference(args_ad) || tmpl_ad->nrderefs != args_ad->nrderefs)
                return FALSE;
        }
        else if (tmpl_ad->atype == template_type && args_ad->atype == template_type)
        {
            if (!sameTemplateSignature(&tmpl_ad->u.td->types, &args_ad->u.td->types, deep))
                return FALSE;
        }
        else if (!sameBaseType(tmpl_ad, args_ad))
            return FALSE;
    }

    return TRUE;
}


/*
 * Create a new variable.
 */
static void newVar(sipSpec *pt, moduleDef *mod, char *name, int isstatic,
        argDef *type, optFlags *of, codeBlock *acode, codeBlock *gcode,
        codeBlock *scode, int section)
{
    varDef *var;
    classDef *escope = currentScope();
    nameDef *nd;

    /*
     * For the moment we don't support capsule variables because it needs the
     * API major version increasing.
     */
    if (type->atype == capsule_type)
        yyerror("Capsule variables not yet supported");

    /* Check the section. */
    if (section != 0)
    {
        if ((section & SECT_IS_PUBLIC) == 0)
            yyerror("Class variables must be in the public section");

        if (!isstatic && acode != NULL)
            yyerror("%AccessCode cannot be specified for non-static class variables");
    }

    if (isstatic && pt->genc)
        yyerror("Cannot have static members in a C structure");

    if (gcode != NULL || scode != NULL)
    {
        if (acode != NULL)
            yyerror("Cannot mix %AccessCode and %GetCode or %SetCode");

        if (escope == NULL)
            yyerror("Cannot specify %GetCode or %SetCode for global variables");
    }

    applyTypeFlags(mod, type, of);

    nd = cacheName(pt, getPythonName(mod, of, name));

    if (inMainModule())
        setIsUsedName(nd);

    checkAttributes(pt, mod, escope, NULL, nd->text, FALSE);

    var = sipMalloc(sizeof (varDef));

    var->pyname = nd;
    var->fqcname = text2scopedName((escope != NULL ? escope->iff : NULL),
            name);
    var->ecd = escope;
    var->module = mod;
    var->varflags = 0;
    var->no_typehint = getNoTypeHint(of);
    var->platforms = currentPlatforms;
    var->type = *type;
    appendCodeBlock(&var->accessfunc, acode);
    appendCodeBlock(&var->getcode, gcode);
    appendCodeBlock(&var->setcode, scode);

    if (isstatic || (escope != NULL && escope->iff->type == namespace_iface))
        setIsStaticVar(var);

    if (getOptFlag(of, "NoSetter", bool_flag) != NULL)
        setNoSetter(var);

    addVariable(pt, var);
}


/*
 * Create a new ctor.
 */
static void newCtor(moduleDef *mod, char *name, int sectFlags,
        signatureDef *args, optFlags *optflgs, codeBlock *methodcode,
        throwArgs *exceptions, signatureDef *cppsig, int explicit,
        docstringDef *docstring, codeBlock *premethodcode)
{
    int a;
    ctorDef *ct, **ctp;
    classDef *cd = currentScope();

    /* Check the name of the constructor. */
    if (strcmp(classBaseName(cd), name) != 0)
        yyerror("Constructor doesn't have the same name as its class");

    /* Add to the list of constructors. */
    ct = sipMalloc(sizeof (ctorDef));

    if (sectFlags & SECT_IS_PROT && makeProtPublic)
    {
        sectFlags &= ~SECT_IS_PROT;
        sectFlags |= SECT_IS_PUBLIC;
    }

    /* Allow the signature to be used like an function signature. */
    memset(&args->result, 0, sizeof (argDef));
    args->result.atype = void_type;

    ct->docstring = docstring;
    ct->ctorflags = sectFlags;
    ct->no_typehint = getNoTypeHint(optflgs);
    ct->api_range = getAPIRange(optflgs);
    ct->pysig = *args;
    ct->cppsig = (cppsig != NULL ? cppsig : &ct->pysig);
    ct->exceptions = exceptions;
    ct->platforms = currentPlatforms;
    appendCodeBlock(&ct->methodcode, methodcode);
    appendCodeBlock(&ct->premethodcode, premethodcode);

    if (!isPrivateCtor(ct))
        setCanCreate(cd);

    if (isProtectedCtor(ct))
        setNeedsShadow(cd);

    if (explicit)
        setIsExplicitCtor(ct);

    getHooks(optflgs, &ct->prehook, &ct->posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGILCtor(ct);
    else if (getHoldGIL(optflgs))
        setIsHoldGILCtor(ct);

    if (getTransfer(optflgs))
        setIsResultTransferredCtor(ct);

    if (getDeprecated(optflgs))
        setIsDeprecatedCtor(ct);

    if (!isPrivateCtor(ct))
        ct->kwargs = keywordArgs(mod, optflgs, &ct->pysig, FALSE);

    if (methodcode == NULL && getOptFlag(optflgs, "NoRaisesPyException", bool_flag) == NULL)
    {
        if (allRaisePyException(mod) || getOptFlag(optflgs, "RaisesPyException", bool_flag) != NULL)
            setRaisesPyExceptionCtor(ct);
    }

    if (getOptFlag(optflgs, "NoDerived", bool_flag) != NULL)
    {
        if (cppsig != NULL)
            yyerror("The /NoDerived/ annotation cannot be used with a C++ signature");

        if (methodcode == NULL)
            yyerror("The /NoDerived/ annotation must be used with %MethodCode");

        ct->cppsig = NULL;
    }

    if (getOptFlag(optflgs, "Default", bool_flag) != NULL)
    {
        if (cd->defctor != NULL)
            yyerror("A constructor with the /Default/ annotation has already been defined");

        cd->defctor = ct;
    }

    /* /Transfer/ arguments need the wrapper. */
    for (a = 0; a < ct->pysig.nrArgs; ++a)
    {
        argDef *ad = &ct->pysig.args[a];

        if (isTransferred(ad))
            setGetWrapper(ad);
    }

    /* Append to the list. */
    for (ctp = &cd->ctors; *ctp != NULL; ctp = &(*ctp)->next)
        ;

    *ctp = ct;
}


/*
 * Create a new function.
 */
static void newFunction(sipSpec *pt, moduleDef *mod, classDef *c_scope,
        ifaceFileDef *ns_scope, mappedTypeDef *mt_scope, int sflags,
        int isstatic, int issignal, int isslot, int isvirt, char *name,
        signatureDef *sig, int isconst, int isabstract, optFlags *optflgs,
        codeBlock *methodcode, codeBlock *vcode, codeBlock *virtcallcode,
        throwArgs *exceptions, signatureDef *cppsig, docstringDef *docstring,
        int isfinal, codeBlock *premethodcode)
{
    static const char *annos[] = {
        "__len__",
        "__imatmul__",
        "__matmul__",
        "AbortOnException",
        "AllowNone",
        "API",
        "AutoGen",
        "Deprecated",
        "DisallowNone",
        "DocType",
        "Encoding",
        "Factory",
        "HoldGIL",
        "KeywordArgs",
        "KeepReference",
        "NewThread",
        "NoArgParser",
        "NoCopy",
        "NoRaisesPyException",
        "NoTypeHint",
        "NoVirtualErrorHandler",
        "Numeric",
        "PostHook",
        "PreHook",
        "PyInt",
        "PyName",
        "PyQtSignalHack",
        "RaisesPyException",
        "ReleaseGIL",
        "Sequence",
        "VirtualErrorHandler",
        "Transfer",
        "TransferBack",
        "TransferThis",
        "TypeHint",
        NULL
    };

    const char *pyname, *virt_error_handler;
    int factory, xferback, no_arg_parser, no_virt_error_handler;
    overDef *od, **odp, **headp;
    optFlag *of;

    checkAnnos(optflgs, annos);

    /* Extra checks for a C module. */
    if (pt->genc)
    {
        if (c_scope != NULL)
            yyerror("Function declaration not allowed in a struct in a C module");

        if (isstatic)
            yyerror("Static functions not allowed in a C module");

        if (exceptions != NULL)
            yyerror("Exceptions not allowed in a C module");

        /* Handle C void prototypes. */
        if (sig->nrArgs == 1)
        {
            argDef *vad = &sig->args[0];

            if (vad->atype == void_type && vad->nrderefs == 0)
                sig->nrArgs = 0;
        }
    }

    if (mt_scope != NULL)
        headp = &mt_scope->overs;
    else if (c_scope != NULL)
        headp = &c_scope->overs;
    else
        headp = &mod->overs;

    /*
     * See if the function has a non-lazy method.  These are methods that
     * Python expects to see defined in the type before any instance of the
     * type is created.
     */
    if (c_scope != NULL)
    {
        static const char *lazy[] = {
            "__getattribute__",
            "__getattr__",
            "__enter__",
            "__exit__",
            "__aenter__",
            "__aexit__",
            NULL
        };

        const char **l;

        for (l = lazy; *l != NULL; ++l)
            if (strcmp(name, *l) == 0)
            {
                setHasNonlazyMethod(c_scope);
                break;
            }
    }

    /* See if it is a factory method. */
    if (getOptFlag(optflgs, "Factory", bool_flag) != NULL)
        factory = TRUE;
    else
    {
        int a;

        factory = FALSE;

        /* Check /TransferThis/ wasn't specified. */
        if (c_scope == NULL || isstatic)
            for (a = 0; a < sig->nrArgs; ++a)
                if (isThisTransferred(&sig->args[a]))
                    yyerror("/TransferThis/ may only be specified in constructors and class methods");
    }

    /* See if the result is to be returned to Python ownership. */
    xferback = (getOptFlag(optflgs, "TransferBack", bool_flag) != NULL);

    if (factory && xferback)
        yyerror("/TransferBack/ and /Factory/ cannot both be specified");

    /* Create a new overload definition. */

    od = sipMalloc(sizeof (overDef));

    getSourceLocation(&od->sloc);

    /* Set the overload flags. */

    if ((sflags & SECT_IS_PROT) && makeProtPublic)
    {
        sflags &= ~SECT_IS_PROT;
        sflags |= SECT_IS_PUBLIC | OVER_REALLY_PROT;
    }

    od->overflags = sflags;

    if (issignal)
    {
        resetIsSlot(od);
        setIsSignal(od);
    }
    else if (isslot)
    {
        resetIsSignal(od);
        setIsSlot(od);
    }

    od->no_typehint = getNoTypeHint(optflgs);

    if (isSignal(od))
        if ((of = getOptFlag(optflgs, "PyQtSignalHack", integer_flag)) != NULL)
            od->pyqt_signal_hack = of->fvalue.ival;

    if (factory)
        setIsFactory(od);

    if (xferback)
        setIsResultTransferredBack(od);

    if (getTransfer(optflgs))
        setIsResultTransferred(od);

    if (getOptFlag(optflgs, "TransferThis", bool_flag) != NULL)
        setIsThisTransferredMeth(od);

    if (methodcode == NULL && getOptFlag(optflgs, "NoRaisesPyException", bool_flag) == NULL)
    {
        if (allRaisePyException(mod) || getOptFlag(optflgs, "RaisesPyException", bool_flag) != NULL)
            setRaisesPyException(od);
    }

    if (isProtected(od))
        setNeedsShadow(c_scope);

    if ((isSlot(od) || isSignal(od)) && !isPrivate(od))
    {
        if (isSignal(od))
            setNeedsShadow(c_scope);

        pt->sigslots = TRUE;
    }

    if (isSignal(od) && (methodcode != NULL || vcode != NULL || virtcallcode != NULL))
        yyerror("Cannot provide code for signals");

    if (isstatic)
    {
        if (isSignal(od))
            yyerror("Static functions cannot be signals");

        if (isvirt)
            yyerror("Static functions cannot be virtual");

        setIsStatic(od);
    }

    if (isconst)
        setIsConst(od);

    if (isfinal)
        setIsFinal(od);

    if (isabstract)
    {
        if (sflags == 0)
            yyerror("Non-class function specified as abstract");

        setIsAbstract(od);
    }

    if ((of = getOptFlag(optflgs, "AutoGen", opt_name_flag)) != NULL)
    {
        if (of->fvalue.sval == NULL || isEnabledFeature(of->fvalue.sval))
            setIsAutoGen(od);
    }

    virt_error_handler = getVirtErrorHandler(optflgs);
    no_virt_error_handler = (getOptFlag(optflgs, "NoVirtualErrorHandler", bool_flag) != NULL);

    if (isvirt)
    {
        if (!isfinal)
        {
            setIsVirtual(od);
            setNeedsShadow(c_scope);
        }

        if (getOptFlag(optflgs, "AbortOnException", bool_flag) != NULL)
            setAbortOnException(od);

        if (no_virt_error_handler)
        {
            if (virt_error_handler != NULL)
                yyerror("/VirtualErrorHandler/ and /NoVirtualErrorHandler/ provided");

            setNoErrorHandler(od);
        }
        else
        {
            od->virt_error_handler = virt_error_handler;
        }
    }
    else
    {
        if (vcode != NULL)
            yyerror("%VirtualCatcherCode provided for non-virtual function");

        if (virt_error_handler != NULL)
            yyerror("/VirtualErrorHandler/ provided for non-virtual function");

        if (no_virt_error_handler)
            yyerror("/NoVirtualErrorHandler/ provided for non-virtual function");
    }

    od->cppname = name;
    od->pysig = *sig;
    od->cppsig = (cppsig != NULL ? cppsig : &od->pysig);
    od->exceptions = exceptions;
    od->platforms = currentPlatforms;
    appendCodeBlock(&od->methodcode, methodcode);
    appendCodeBlock(&od->premethodcode, premethodcode);
    appendCodeBlock(&od->virtcallcode, virtcallcode);
    appendCodeBlock(&od->virtcode, vcode);

    no_arg_parser = (getOptFlag(optflgs, "NoArgParser", bool_flag) != NULL);

    if (no_arg_parser)
    {
        if (methodcode == NULL)
            yyerror("%MethodCode must be supplied if /NoArgParser/ is specified");
    }
    else
    {
        /*
         * The argument parser requires that there is nothing after an
         * ellipsis.
         */
        checkEllipsis(sig);
    }

    if (cppsig != NULL)
        checkEllipsis(cppsig);

    if (getOptFlag(optflgs, "NoCopy", bool_flag) != NULL)
        setNoCopy(&od->pysig.result);

    if (getAllowNone(optflgs))
        setAllowNone(&od->pysig.result);

    if (getDisallowNone(optflgs))
        setDisallowNone(&od->pysig.result);

    handleKeepReference(optflgs, &od->pysig.result, mod);

    pyname = getPythonName(mod, optflgs, name);

    od->common = findFunction(pt, mod, c_scope, ns_scope, mt_scope, pyname,
            (methodcode != NULL), sig->nrArgs, no_arg_parser);

    if (isProtected(od))
        setHasProtected(od->common);

    if (strcmp(pyname, "__delattr__") == 0)
        setIsDelattr(od);

    od->docstring = docstring;

    od->api_range = getAPIRange(optflgs);

    if (od->api_range == NULL)
        setNotVersioned(od->common);

    if (getOptFlag(optflgs, "Numeric", bool_flag) != NULL)
    {
        if (isSequence(od->common))
            yyerror("/Sequence/ has already been specified");

        setIsNumeric(od->common);
    }

    if (getOptFlag(optflgs, "Sequence", bool_flag) != NULL)
    {
        if (isNumeric(od->common))
            yyerror("/Numeric/ has already been specified");

        setIsSequence(od->common);
    }

    /* Methods that run in new threads must be virtual. */
    if (getOptFlag(optflgs, "NewThread", bool_flag) != NULL)
    {
        argDef *res;

        if (!isvirt)
            yyerror("/NewThread/ may only be specified for virtual functions");

        /*
         * This is an arbitary limitation to make the code generator slightly
         * easier - laziness on my part.
         */
        res = &od->cppsig->result;

        if (res->atype != void_type || res->nrderefs != 0)
            yyerror("/NewThread/ may only be specified for void functions");

        setIsNewThread(od);
    }

    getHooks(optflgs, &od->prehook, &od->posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGIL(od);
    else if (getHoldGIL(optflgs))
        setIsHoldGIL(od);

    if (getDeprecated(optflgs))
        setIsDeprecated(od);

    if (!isPrivate(od) && (od->common->slot == no_slot || od->common->slot == call_slot))
    {
        od->kwargs = keywordArgs(mod, optflgs, &od->pysig, hasProtected(od->common));

        if (od->kwargs != NoKwArgs)
            setUseKeywordArgs(od->common);

        /*
         * If the overload is protected and defined in an imported module then
         * we need to make sure that any other overloads' keyword argument
         * names are marked as used.
         */
        if (!isSignal(od) && isProtected(od) && !inMainModule())
        {
            overDef *kwod;

            for (kwod = c_scope->overs; kwod != NULL; kwod = kwod->next)
                if (kwod->common == od->common && kwod->kwargs != NoKwArgs)
                {
                    int a;

                    for (a = 0; a < kwod->pysig.nrArgs; ++a)
                    {
                        argDef *ad = &kwod->pysig.args[a];

                        if (kwod->kwargs == OptionalKwArgs && ad->defval == NULL)
                            continue;

                        if (ad->name != NULL)
                            setIsUsedName(ad->name);
                    }
                }
        }
    }

    od->next = NULL;

    /* See if we want to auto-generate some methods. */
    if (getOptFlag(optflgs, "__len__", bool_flag) != NULL)
    {
        overDef *len;

        len = sipMalloc(sizeof (overDef));

        len->cppname = "__len__";
        len->overflags = SECT_IS_PUBLIC;
        len->pysig.result.atype = ssize_type;
        len->pysig.nrArgs = 0;
        len->cppsig = &len->pysig;

        if ((len->methodcode = od->methodcode) == NULL)
        {
            char *buf = sipStrdup("            sipRes = (Py_ssize_t)sipCpp->");
            codeBlock *code;

            append(&buf, od->cppname);
            append(&buf, "();\n");

            code = sipMalloc(sizeof (codeBlock));

            code->frag = buf;
            code->filename = "Auto-generated";
            code->linenr = 1;

            appendCodeBlock(&len->methodcode, code);
        }

        len->common = findFunction(pt, mod, c_scope, ns_scope, mt_scope,
                len->cppname, TRUE, 0, FALSE);

        len->platforms = od->platforms;
        len->next = od->next;
        od->next = len;
    }

    if (getOptFlag(optflgs, "__matmul__", bool_flag) != NULL)
    {
        overDef *matmul;

        matmul = sipMalloc(sizeof (overDef));

        matmul->cppname = "__matmul__";
        matmul->overflags = SECT_IS_PUBLIC;
        matmul->pysig = od->pysig;
        matmul->cppsig = (cppsig != NULL ? cppsig : &matmul->pysig);

        matmul->methodcode = od->methodcode;

        matmul->common = findFunction(pt, mod, c_scope, ns_scope, mt_scope,
                matmul->cppname, (matmul->methodcode != NULL),
                matmul->pysig.nrArgs, FALSE);

        matmul->platforms = od->platforms;
        matmul->next = od->next;
        od->next = matmul;
    }

    if (getOptFlag(optflgs, "__imatmul__", bool_flag) != NULL)
    {
        overDef *imatmul;

        imatmul = sipMalloc(sizeof (overDef));

        imatmul->cppname = "__imatmul__";
        imatmul->overflags = SECT_IS_PUBLIC;
        imatmul->pysig = od->pysig;
        imatmul->cppsig = (cppsig != NULL ? cppsig : &imatmul->pysig);

        imatmul->methodcode = od->methodcode;

        imatmul->common = findFunction(pt, mod, c_scope, ns_scope, mt_scope,
                imatmul->cppname, (imatmul->methodcode != NULL),
                imatmul->pysig.nrArgs, FALSE);

        imatmul->platforms = od->platforms;
        imatmul->next = od->next;
        od->next = imatmul;
    }

    /* Append to the list. */
    for (odp = headp; *odp != NULL; odp = &(*odp)->next)
        ;

    *odp = od;
}


/*
 * Return the Python name based on the C/C++ name and any /PyName/ annotation.
 */
static const char *getPythonName(moduleDef *mod, optFlags *optflgs,
        const char *cname)
{
    const char *pname;
    optFlag *of;
    autoPyNameDef *apnd;

    /* Use the explicit name if given. */
    if ((of = getOptFlag(optflgs, "PyName", name_flag)) != NULL)
        return of->fvalue.sval;

    /* Apply any automatic naming rules. */
    pname = cname;

    for (apnd = mod->autopyname; apnd != NULL; apnd = apnd->next)
    {
        size_t len = strlen(apnd->remove_leading);

        if (strncmp(pname, apnd->remove_leading, len) == 0)
            pname += len;
    }

    return pname;
}


/*
 * Cache a name in a module.  Entries in the cache are stored in order of
 * decreasing length.
 */
nameDef *cacheName(sipSpec *pt, const char *name)
{
    nameDef *nd, **ndp;
    size_t len;

    /* Allow callers to be lazy about checking if there is really a name. */
    if (name == NULL)
        return NULL;

    /* Skip entries that are too large. */
    ndp = &pt->namecache;
    len = strlen(name);

    while (*ndp != NULL && (*ndp)->len > len)
        ndp = &(*ndp)->next;

    /* Check entries that are the right length. */
    for (nd = *ndp; nd != NULL && nd->len == len; nd = nd->next)
        if (memcmp(nd->text, name, len) == 0)
            return nd;

    /* Create a new one. */
    nd = sipMalloc(sizeof (nameDef));

    nd->nameflags = 0;
    nd->text = name;
    nd->len = len;
    nd->next = *ndp;

    *ndp = nd;

    return nd;
}


/*
 * Find (or create) an overloaded function name.
 */
static memberDef *findFunction(sipSpec *pt, moduleDef *mod, classDef *c_scope,
        ifaceFileDef *ns_scope, mappedTypeDef *mt_scope, const char *pname,
        int hwcode, int nrargs, int no_arg_parser)
{
    static struct slot_map {
        const char *name;   /* The slot name. */
        slotType type;      /* The corresponding type. */
        int needs_hwcode;   /* Set if handwritten code is required. */
        int nrargs;         /* Nr. of arguments. */
    } slot_table[] = {
        {"__str__", str_slot, TRUE, 0},
        {"__int__", int_slot, FALSE, 0},
        {"__long__", long_slot, FALSE, 0},
        {"__float__", float_slot, FALSE, 0},
        {"__len__", len_slot, TRUE, 0},
        {"__contains__", contains_slot, TRUE, 1},
        {"__add__", add_slot, FALSE, 1},
        {"__sub__", sub_slot, FALSE, 1},
        {"__mul__", mul_slot, FALSE, 1},
        {"__div__", div_slot, FALSE, 1},
        {"__mod__", mod_slot, FALSE, 1},
        {"__floordiv__", floordiv_slot, TRUE, 1},
        {"__truediv__", truediv_slot, FALSE, 1},
        {"__and__", and_slot, FALSE, 1},
        {"__or__", or_slot, FALSE, 1},
        {"__xor__", xor_slot, FALSE, 1},
        {"__lshift__", lshift_slot, FALSE, 1},
        {"__rshift__", rshift_slot, FALSE, 1},
        {"__iadd__", iadd_slot, FALSE, 1},
        {"__isub__", isub_slot, FALSE, 1},
        {"__imul__", imul_slot, FALSE, 1},
        {"__idiv__", idiv_slot, FALSE, 1},
        {"__imod__", imod_slot, FALSE, 1},
        {"__ifloordiv__", ifloordiv_slot, TRUE, 1},
        {"__itruediv__", itruediv_slot, FALSE, 1},
        {"__iand__", iand_slot, FALSE, 1},
        {"__ior__", ior_slot, FALSE, 1},
        {"__ixor__", ixor_slot, FALSE, 1},
        {"__ilshift__", ilshift_slot, FALSE, 1},
        {"__irshift__", irshift_slot, FALSE, 1},
        {"__invert__", invert_slot, FALSE, 0},
        {"__call__", call_slot, FALSE, -1},
        {"__getitem__", getitem_slot, FALSE, 1},
        {"__setitem__", setitem_slot, TRUE, 2},
        {"__delitem__", delitem_slot, TRUE, 1},
        {"__lt__", lt_slot, FALSE, 1},
        {"__le__", le_slot, FALSE, 1},
        {"__eq__", eq_slot, FALSE, 1},
        {"__ne__", ne_slot, FALSE, 1},
        {"__gt__", gt_slot, FALSE, 1},
        {"__ge__", ge_slot, FALSE, 1},
        {"__cmp__", cmp_slot, FALSE, 1},
        {"__bool__", bool_slot, TRUE, 0},
        {"__nonzero__", bool_slot, TRUE, 0},
        {"__neg__", neg_slot, FALSE, 0},
        {"__pos__", pos_slot, FALSE, 0},
        {"__abs__", abs_slot, TRUE, 0},
        {"__repr__", repr_slot, TRUE, 0},
        {"__hash__", hash_slot, TRUE, 0},
        {"__index__", index_slot, TRUE, 0},
        {"__iter__", iter_slot, TRUE, 0},
        {"__next__", next_slot, TRUE, 0},
        {"__setattr__", setattr_slot, TRUE, 2},
        {"__delattr__", delattr_slot, TRUE, 1},
        {"__matmul__", matmul_slot, FALSE, 1},
        {"__imatmul__", imatmul_slot, FALSE, 1},
        {"__await__", await_slot, TRUE, 0},
        {"__aiter__", aiter_slot, TRUE, 0},
        {"__anext__", anext_slot, TRUE, 0},
        {NULL, no_slot, FALSE, 0}
    };

    memberDef *md, **flist;
    struct slot_map *sm;
    slotType st;

    /* Get the slot type. */
    st = no_slot;

    for (sm = slot_table; sm->name != NULL; ++sm)
        if (strcmp(sm->name, pname) == 0)
        {
            if (sm->needs_hwcode && !hwcode)
                yyerror("This Python slot requires %MethodCode");

            if (sm->nrargs >= 0)
            {
                if (mt_scope == NULL && c_scope == NULL)
                {
                    /* Global operators need one extra argument. */
                    if (sm -> nrargs + 1 != nrargs)
                        yyerror("Incorrect number of arguments to global operator");
                }
                else if (sm->nrargs != nrargs)
                    yyerror("Incorrect number of arguments to Python slot");
            }

            st = sm->type;

            break;
        }

    /* Check there is no name clash. */
    checkAttributes(pt, mod, c_scope, mt_scope, pname, TRUE);

    /* See if it already exists. */
    if (mt_scope != NULL)
        flist = &mt_scope->members;
    else if (c_scope != NULL)
        flist = &c_scope->members;
    else
        flist = &mod->othfuncs;

    /* __delattr__ is implemented as __setattr__. */
    if (st == delattr_slot)
    {
        if (inMainModule())
            setIsUsedName(cacheName(pt, pname));

        st = setattr_slot;
        pname = "__setattr__";
    }

    for (md = *flist; md != NULL; md = md->next)
        if (strcmp(md->pyname->text, pname) == 0 && md->module == mod)
            break;

    if (md == NULL)
    {
        /* Create a new one. */
        md = sipMalloc(sizeof (memberDef));

        md->pyname = cacheName(pt, pname);
        md->memberflags = 0;
        md->slot = st;
        md->module = mod;
        md->ns_scope = ns_scope;
        md->next = *flist;

        *flist = md;

        if (inMainModule())
            setIsUsedName(md->pyname);

        if (no_arg_parser)
            setNoArgParser(md);
    }
    else if (noArgParser(md))
        yyerror("Another overload has already been defined that is annotated as /NoArgParser/");

    /* Global operators are a subset. */
    if (mt_scope == NULL && c_scope == NULL && st != no_slot && st != neg_slot && st != pos_slot && !isNumberSlot(md) && !isInplaceNumberSlot(md) && !isRichCompareSlot(md))
        yyerror("Global operators must be either numeric or comparison operators");

    return md;
}


/*
 * Search a set of flags for a particular one.
 */
static optFlag *findOptFlag(optFlags *flgs, const char *name)
{
    int f;

    for (f = 0; f < flgs->nrFlags; ++f)
    {
        optFlag *of = &flgs->flags[f];

        if (strcmp(of->fname, name) == 0)
            return of;
    }

    return NULL;
}


/*
 * Search a set of flags for a particular one and check its type.
 */
static optFlag *getOptFlag(optFlags *flgs, const char *name, flagType ft)
{
    optFlag *of = findOptFlag(flgs, name);

    if (of != NULL)
    {
        /* An optional name can look like a boolean or a name. */
        if (ft == opt_name_flag)
        {
            if (of->ftype == bool_flag)
            {
                of->ftype = opt_name_flag;
                of->fvalue.sval = NULL;
            }
            else if (of->ftype == name_flag)
            {
                of->ftype = opt_name_flag;
            }
        }

        /* An optional integer can look like a boolean or an integer. */
        if (ft == opt_integer_flag)
        {
            if (of->ftype == bool_flag)
            {
                of->ftype = opt_integer_flag;
                of->fvalue.ival = -1;
            }
            else if (of->ftype == integer_flag)
            {
                of->ftype = opt_integer_flag;
            }
        }

        /* A string list will look like a string. */
        if (ft == string_list_flag && of->ftype == string_flag)
        {
            char *s;

            s = of->fvalue.sval;
            of->fvalue.slval = NULL;

            while (*s != '\0')
            {
                char *start;

                while (*s == ' ')
                    ++s;

                start = s;

                if (*start != '\0')
                {
                    char saved, *end;

                    for (end = start + 1; *end != ' ' && *end != '\0'; ++end)
                        ;

                    saved = *end;
                    *end = '\0';

                    appendString(&of->fvalue.slval, start);

                    *end = saved;
                    s = end;
                }
            }

            of->ftype = string_list_flag;
        }

        if (ft != of->ftype)
            yyerror("Annotation has a value of the wrong type");
    }

    return of;
}


/*
 * Append a string to a list of them.
 */
void appendString(stringList **headp, const char *s)
{
    stringList *sl;

    /* Create the new entry. */

    sl = sipMalloc(sizeof (stringList));

    sl -> s = s;
    sl -> next = NULL;

    /* Append it to the list. */

    while (*headp != NULL)
        headp = &(*headp) -> next;

    *headp = sl;
}


/*
 * A name is going to be used as a Python attribute name within a Python scope
 * (ie. a Python dictionary), so check against what we already know is going in
 * the same scope in case there is a clash.
 */
static void checkAttributes(sipSpec *pt, moduleDef *mod, classDef *py_c_scope,
        mappedTypeDef *py_mt_scope, const char *attr, int isfunc)
{
    enumDef *ed;
    varDef *vd;
    classDef *cd;

    /* We don't do any check for a non-strict parse. */
    if (!strictParse)
        return;

    /* Check the enums. */
    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->pyname == NULL)
            continue;

        if (py_c_scope != NULL)
        {
            if (ed->ecd != py_c_scope)
                continue;
        }
        else if (py_mt_scope != NULL)
        {
            if (ed->emtd != py_mt_scope)
                continue;
        }
        else if (ed->ecd != NULL || ed->emtd != NULL)
        {
            continue;
        }

        if (strcmp(ed->pyname->text, attr) == 0)
            yyerror("There is already an enum in scope with the same Python name");

        if (!isScopedEnum(ed))
        {
            enumMemberDef *emd;

            for (emd = ed->members; emd != NULL; emd = emd->next)
                if (strcmp(emd->pyname->text, attr) == 0)
                    yyerror("There is already an enum member in scope with the same Python name");
        }
    }

    /*
     * Only check the members if this attribute isn't a member because we can
     * handle members with the same name in the same scope.
     */
    if (!isfunc)
    {
        memberDef *md, *membs;
        overDef *overs;

        if (py_mt_scope != NULL)
        {
            membs = py_mt_scope->members;
            overs = py_mt_scope->overs;
        }
        else if (py_c_scope != NULL)
        {
            membs = py_c_scope->members;
            overs = py_c_scope->overs;
        }
        else
        {
            membs = mod->othfuncs;
            overs = mod->overs;
        }

        for (md = membs; md != NULL; md = md->next)
        {
            overDef *od;

            if (strcmp(md->pyname->text, attr) != 0)
                continue;

            /* Check for a conflict with all overloads. */
            for (od = overs; od != NULL; od = od->next)
            {
                if (od->common != md)
                    continue;

                yyerror("There is already a function in scope with the same Python name");
            }
        }
    }

    /* If the scope was a mapped type then that's all we have to check. */
    if (py_mt_scope != NULL)
        return;

    /* Check the variables. */
    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->ecd != py_c_scope)
            continue;

        if (strcmp(vd->pyname->text,attr) == 0)
            yyerror("There is already a variable in scope with the same Python name");
    }

    /* Check the classes. */
    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        if (cd->ecd != py_c_scope || cd->pyname == NULL)
            continue;

        if (strcmp(cd->pyname->text, attr) == 0 && !isExternal(cd))
            yyerror("There is already a class or namespace in scope with the same Python name");
    }

    /* Check the exceptions. */
    if (py_c_scope == NULL)
    {
        exceptionDef *xd;

        for (xd = pt->exceptions; xd != NULL; xd = xd->next)
            if (xd->pyname != NULL && strcmp(xd->pyname, attr) == 0)
                yyerror("There is already an exception with the same Python name");
    }

    /* Check the properties. */
    if (py_c_scope != NULL)
    {
        propertyDef *pd;

        for (pd = py_c_scope->properties; pd != NULL; pd = pd->next)
            if (strcmp(pd->name->text, attr) == 0)
                yyerror("There is already a property with the same name");
    }
}


/*
 * Append a code block to a list of them.
 */
static void appendCodeBlock(codeBlockList **headp, codeBlock *cb)
{
    codeBlockList *cbl;

    /* Handle the trivial case. */
    if (cb == NULL)
        return;

    /* Find the end of the list. */
    while (*headp != NULL)
    {
        /* Ignore if the block is already in the list. */
        if ((*headp)->block == cb)
            return;

        headp = &(*headp)->next;
    }

    cbl = sipMalloc(sizeof (codeBlockList));
    cbl->block = cb;

    *headp = cbl;
}


/*
 * Append a code block list to an existing list.
 */
void appendCodeBlockList(codeBlockList **headp, codeBlockList *cbl)
{
    while (cbl != NULL)
    {
        appendCodeBlock(headp, cbl->block);
        cbl = cbl->next;
    }
}


/*
 * Handle the end of a fully parsed a file.
 */
static void handleEOF()
{
    /*
     * Check that the number of nested if's is the same as when we started
     * the file.
     */

    if (stackPtr > currentContext.ifdepth)
        fatal("Too many %%If statements in %s\n", previousFile);

    if (stackPtr < currentContext.ifdepth)
        fatal("Too many %%End statements in %s\n", previousFile);
}


/*
 * Handle the end of a fully parsed a module.
 */
static void handleEOM()
{
    moduleDef *from;

    /* Check it has been named. */
    if (currentModule->name == NULL)
        fatal("No %%Module has been specified for module defined in %s\n",
                previousFile);

    from = currentContext.prevmod;

    if (from != NULL)
    {
        if (from->encoding == no_type)
            from->encoding = currentModule->encoding;

        if (isCallSuperInitUndefined(from))
        {
            if (isCallSuperInitYes(currentModule))
                setCallSuperInitYes(from);
            else
                setCallSuperInitNo(from);
        }
    }

    /* The previous module is now current. */
    currentModule = from;
}


/*
 * Find an existing qualifier.
 */
static qualDef *findQualifier(const char *name)
{
    moduleDef *mod;

    for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
    {
        qualDef *qd;

        for (qd = mod->qualifiers; qd != NULL; qd = qd->next)
            if (strcmp(qd->name, name) == 0)
                return qd;
    }

    /* Qualifiers corresponding to the SIP version are created on the fly. */
    if (name[0] == 'S' && name[1] == 'I' && name[2] == 'P' && name[3] == '_')
    {
        const char *cp = &name[3];
        int major, minor, patch;

        cp = getInt(cp, &major);
        cp = getInt(cp, &minor);
        cp = getInt(cp, &patch);

        if (*cp != '\0')
            yyerror("Unexpected character after SIP version number");

        return allocQualifier(currentModule, -1,
                (major << 16) | (minor << 8) | patch, TRUE, name,
                time_qualifier);
    }

    return NULL;
}


/*
 * Get an integer from string.
 */
static const char *getInt(const char *cp, int *ip)
{
    /* Handle the default value. */
    *ip = 0;

    if (*cp == '\0')
        return cp;

    /* There must be a leading underscore. */
    if (*cp++ != '_')
        yyerror("An underscore must separate the parts of a SIP version number");

    while (isdigit(*cp))
    {
        *ip *= 10;
        *ip += *cp - '0';
        ++cp;
    }

    return cp;
}


/*
 * Find an existing API.
 */
apiVersionRangeDef *findAPI(sipSpec *pt, const char *name)
{
    moduleDef *mod;

    for (mod = pt->modules; mod != NULL; mod = mod->next)
    {
        apiVersionRangeDef *avd;

        for (avd = mod->api_versions; avd != NULL; avd = avd->next)
            if (strcmp(avd->api_name->text, name) == 0)
                return avd;
    }

    return NULL;
}


/*
 * Return a copy of a scoped name.
 */
scopedNameDef *copyScopedName(scopedNameDef *snd)
{
    scopedNameDef *head;

    head = NULL;

    while (snd != NULL)
    {
        appendScopedName(&head,text2scopePart(snd -> name));
        snd = snd -> next;
    }

    return head;
}


/*
 * Append a name to a list of scopes.
 */
void appendScopedName(scopedNameDef **headp,scopedNameDef *newsnd)
{
    while (*headp != NULL)
        headp = &(*headp) -> next;

    *headp = newsnd;
}


/*
 * Free a scoped name - but not the text itself.
 */
void freeScopedName(scopedNameDef *snd)
{
    while (snd != NULL)
    {
        scopedNameDef *next = snd -> next;

        free(snd);

        snd = next;
    }
}


/*
 * Convert a text string to a scope part structure.
 */
scopedNameDef *text2scopePart(char *text)
{
    scopedNameDef *snd;

    snd = sipMalloc(sizeof (scopedNameDef));

    snd->name = text;
    snd->next = NULL;

    return snd;
}


/*
 * Convert a text string to a fully scoped name.
 */
static scopedNameDef *text2scopedName(ifaceFileDef *scope, char *text)
{
    return scopeScopedName(scope, text2scopePart(text));
}


/*
 * Prepend any current scope to a scoped name.
 */
static scopedNameDef *scopeScopedName(ifaceFileDef *scope, scopedNameDef *name)
{
    scopedNameDef *snd;

    snd = (scope != NULL ? copyScopedName(scope->fqcname) : text2scopePart(""));

    appendScopedName(&snd, name);

    return snd;
}


/*
 * Return a pointer to the tail part of a scoped name.
 */
char *scopedNameTail(scopedNameDef *snd)
{
    if (snd == NULL)
        return NULL;

    while (snd->next != NULL)
        snd = snd->next;

    return snd->name;
}


/*
 * Push the given scope onto the scope stack.
 */
static void pushScope(classDef *scope)
{
    if (currentScopeIdx >= MAX_NESTED_SCOPE)
        fatal("Internal error: increase the value of MAX_NESTED_SCOPE\n");

    scopeStack[currentScopeIdx] = scope;
    sectFlagsStack[currentScopeIdx] = sectionFlags;

    ++currentScopeIdx;
}


/*
 * Pop the scope stack.
 */
static void popScope(void)
{
    if (currentScopeIdx > 0)
        sectionFlags = sectFlagsStack[--currentScopeIdx];
}


/*
 * Return non-zero if the current input should be parsed rather than be
 * skipped.
 */
static int notSkipping()
{
    return (stackPtr == 0 ? TRUE : skipStack[stackPtr - 1]);
}


/*
 * Return the value of an expression involving a time period.
 */
static int timePeriod(const char *lname, const char *uname)
{
    int line;
    qualDef *qd, *lower, *upper;
    moduleDef *mod;

    if (lname == NULL)
        lower = NULL;
    else if ((lower = findQualifier(lname)) == NULL || lower->qtype != time_qualifier)
        yyerror("Lower bound is not a time version");

    if (uname == NULL)
        upper = NULL;
    else if ((upper = findQualifier(uname)) == NULL || upper->qtype != time_qualifier)
        yyerror("Upper bound is not a time version");

    /* Sanity checks on the bounds. */
    if (lower == NULL && upper == NULL)
        yyerror("Lower and upper bounds cannot both be omitted");

    if (lower != NULL && upper != NULL)
    {
        if (lower->module != upper->module || lower->line != upper->line)
            yyerror("Lower and upper bounds are from different timelines");

        if (lower == upper)
            yyerror("Lower and upper bounds must be different");

        if (lower->order > upper->order)
            yyerror("Later version specified as lower bound");
    }

    /* Go through each slot in the relevant timeline. */
    if (lower != NULL)
    {
        mod = lower->module;
        line = lower->line;
    }
    else
    {
        mod = upper->module;
        line = upper->line;
    }

    /* Handle the SIP version number pseudo-timeline. */
    if (line < 0)
    {
        if (lower != NULL && sipVersion < lower->order)
            return FALSE;

        if (upper != NULL && sipVersion >= upper->order)
            return FALSE;

        return TRUE;
    }

    for (qd = mod->qualifiers; qd != NULL; qd = qd->next)
    {
        if (qd->qtype != time_qualifier || qd->line != line)
            continue;

        if (selectedQualifier(neededQualifiers, qd))
        {
            if (lower != NULL && qd->order < lower->order)
                return FALSE;

            if (upper != NULL && qd->order >= upper->order)
                return FALSE;

            return TRUE;
        }
    }

    /*
     * If there is no upper bound then assume the expression is true unless
     * the lower bound is a backstop.
     */
    if (upper == NULL)
        return !isBackstop(lower);

    /*
     * If the upper limit corresponds to a backstop then assume the expression
     * is true.
     */
    return isBackstop(upper);
}


/*
 * See if a qualifier is a backstop.
 */
static int isBackstop(qualDef *qd)
{
    stringList *sl;

    for (sl = backstops; sl != NULL; sl = sl->next)
        if (strcmp(qd->name, sl->s) == 0)
            return TRUE;

    return FALSE;
}


/*
 * Return the value of an expression involving a single platform or feature.
 */
static int platOrFeature(char *name, int optnot)
{
    int this;
    qualDef *qd;

    if ((qd = findQualifier(name)) == NULL || qd->qtype == time_qualifier)
        yyerror("No such platform or feature");

    /* Assume this sub-expression is false. */

    this = FALSE;

    if (qd->qtype == feature_qualifier)
    {
        if (!excludedFeature(excludedQualifiers, qd))
            this = TRUE;
    }
    else
    {
        if (!strictParse)
        {
            if (optnot)
            {
                moduleDef *mod;

                /* Add every platform except the one we have. */
                for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
                {
                    qualDef *not_qd;

                    for (not_qd = mod->qualifiers; not_qd != NULL; not_qd = not_qd->next)
                        if (not_qd->qtype == platform_qualifier && strcmp(not_qd->name, qd->name) != 0)
                            addPlatform(not_qd);
                }
            }
            else
            {
                addPlatform(qd);
            }

            /*
             * If it is a non-strict parse then this is always TRUE, ie. we
             * never skip because of the platform.
             */
            return TRUE;
        }

        if (selectedQualifier(neededQualifiers, qd))
            this = TRUE;
    }

    if (optnot)
        this = !this;

    return this;
}


/*
 * Add a platform to the current list of platforms if it is not already there.
 */
static void addPlatform(qualDef *qd)
{
    platformDef *pd;

    for (pd = currentPlatforms; pd != NULL; pd = pd->next)
        if (pd->qualifier == qd)
            return;

    pd = sipMalloc(sizeof (platformDef));
    pd->qualifier = qd;
    pd->next = currentPlatforms;
    currentPlatforms = pd;
}


/*
 * Return TRUE if the given qualifier is excluded.
 */
int excludedFeature(stringList *xsl, qualDef *qd)
{
    while (xsl != NULL)
    {
        if (strcmp(qd->name, xsl->s) == 0)
            return TRUE;

        xsl = xsl->next;
    }

    return !qd->default_enabled;
}


/*
 * Return TRUE if the given qualifier is needed.
 */
int selectedQualifier(stringList *needed_qualifiers, qualDef *qd)
{
    stringList *sl;

    for (sl = needed_qualifiers; sl != NULL; sl = sl->next)
        if (strcmp(qd->name, sl->s) == 0)
            return qd->default_enabled;

    return FALSE;
}


/*
 * Return the current scope.  currentScope() is only valid if notSkipping()
 * returns non-zero.
 */
static classDef *currentScope(void)
{
    return (currentScopeIdx > 0 ? scopeStack[currentScopeIdx - 1] : NULL);
}


/*
 * Create a new qualifier.
 */
static void newQualifier(moduleDef *mod, int line, unsigned order,
        int default_enabled, const char *name, qualType qt)
{
    qualDef *qd;

    /* See if it already exists. */
    if ((qd = findQualifier(name)) != NULL)
    {
        /*
         * We allow versions to be defined more than once so long as they are
         * in different timelines.  It is sometimes necessary to define the
         * same timeline in multiple modules if a module that others depend on
         * is added during the timeline (eg. QtWebEngineCore).
         */
        if (qd->qtype != time_qualifier || qt != time_qualifier || (qd->module == mod && qd->line == line))
            yyerror("Version is already defined");
    }

    allocQualifier(mod, line, order, default_enabled, name, qt);
}


/*
 * Allocate a new qualifier.
 */
static qualDef *allocQualifier(moduleDef *mod, int line, unsigned order,
        int default_enabled, const char *name, qualType qt)
{
    qualDef *qd;

    qd = sipMalloc(sizeof (qualDef));

    qd->name = name;
    qd->qtype = qt;
    qd->module = mod;
    qd->line = line;
    qd->order = order;
    qd->default_enabled = default_enabled;
    qd->next = mod->qualifiers;

    mod->qualifiers = qd;

    return qd;
}


/*
 * Create a new imported module.
 */
static void newImport(const char *filename)
{
    moduleDef *from, *mod;
    moduleListDef *mld;

    /* Create a new module if it has not already been defined. */
    for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
        if (strcmp(mod->file, filename) == 0)
            break;

    from = currentModule;

    if (mod == NULL)
    {
        newModule(NULL, filename);
        mod = currentModule;
    }
    else if (from->encoding == no_type)
    {
        /* Import any defaults from the already parsed module. */
        from->encoding = mod->encoding;
    }

    /* Add the new import unless it has already been imported. */
    for (mld = from->imports; mld != NULL; mld = mld->next)
        if (mld->module == mod)
            return;

    mld = sipMalloc(sizeof (moduleListDef));
    mld->module = mod;
    mld->next = from->imports;

    from->imports = mld;
}


/*
 * Set up pointers to hook names.
 */
static void getHooks(optFlags *optflgs,char **pre,char **post)
{
    optFlag *of;

    if ((of = getOptFlag(optflgs,"PreHook",name_flag)) != NULL)
        *pre = of -> fvalue.sval;
    else
        *pre = NULL;

    if ((of = getOptFlag(optflgs,"PostHook",name_flag)) != NULL)
        *post = of -> fvalue.sval;
    else
        *post = NULL;
}


/*
 * Get the /Transfer/ option flag.
 */
static int getTransfer(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "Transfer", bool_flag) != NULL);
}


/*
 * Get the /ReleaseGIL/ option flag.
 */
static int getReleaseGIL(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "ReleaseGIL", bool_flag) != NULL);
}


/*
 * Get the /HoldGIL/ option flag.
 */
static int getHoldGIL(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "HoldGIL", bool_flag) != NULL);
}


/*
 * Get the /Deprecated/ option flag.
 */
static int getDeprecated(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "Deprecated", bool_flag) != NULL);
}


/*
 * Get the /AllowNone/ option flag.
 */
static int getAllowNone(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "AllowNone", bool_flag) != NULL);
}


/*
 * Get the /DisallowNone/ option flag.
 */
static int getDisallowNone(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "DisallowNone", bool_flag) != NULL);
}


/*
 * Get the /VirtualErrorHandler/ option flag.
 */
static const char *getVirtErrorHandler(optFlags *optflgs)
{
    optFlag *of = getOptFlag(optflgs, "VirtualErrorHandler", name_flag);

    if (of == NULL)
        return NULL;

    return of->fvalue.sval;
}


/*
 * Get the /DocType/ option flag.
 */
static const char *getDocType(optFlags *optflgs)
{
    optFlag *of = getOptFlag(optflgs, "DocType", string_flag);

    if (of == NULL)
        return NULL;

    deprecated("/DocType/ is deprecated\n");

    return of->fvalue.sval;
}


/*
 * Get the /TypeHintValue/ option flag.
 */
static const char *getTypeHintValue(optFlags *optflgs)
{
    optFlag *of = getOptFlag(optflgs, "TypeHintValue", string_flag);

    if (of != NULL)
        return of->fvalue.sval;

    if ((of = getOptFlag(optflgs, "DocValue", string_flag)) != NULL)
    {
        deprecated("/DocValue/ is deprecated\n");

        return of->fvalue.sval;
    }

    return NULL;
}


/*
 * Get the /TypeHint/, /TypeHintIn/ and /TypeHintOut/ option flags.
 */
static void getTypeHints(optFlags *optflgs, typeHintDef **in,
        typeHintDef **out)
{
    optFlag *of;
    typeHintDef *thd;

    if ((of = getOptFlag(optflgs, "TypeHint", string_flag)) != NULL)
        thd = newTypeHint(of->fvalue.sval);
    else
        thd = NULL;

    if ((of = getOptFlag(optflgs, "TypeHintIn", string_flag)) != NULL)
    {
        if (thd != NULL)
            yywarning("/TypeHintIn/ overrides /TypeHint/");

        *in = newTypeHint(of->fvalue.sval);
    }
    else
    {
        *in = thd;
    }

    if ((of = getOptFlag(optflgs, "TypeHintOut", string_flag)) != NULL)
    {
        if (thd != NULL)
            yywarning("/TypeHintOut/ overrides /TypeHint/");

        *out = newTypeHint(of->fvalue.sval);
    }
    else
    {
        *out = thd;
    }
}


/*
 * Get the /NoTypeHint/ option flag.
 */
static int getNoTypeHint(optFlags *optflgs)
{
    return (getOptFlag(optflgs, "NoTypeHint", bool_flag) != NULL);
}


/*
 * Return TRUE if the PyQt4 plugin was specified.
 */
int pluginPyQt4(sipSpec *pt)
{
    return stringFind(pt->plugins, "PyQt4");
}


/*
 * Return TRUE if the PyQt5 plugin was specified.
 */
int pluginPyQt5(sipSpec *pt)
{
    return stringFind(pt->plugins, "PyQt5");
}


/*
 * Return TRUE if a list of strings contains a given entry.
 */
static int stringFind(stringList *sl, const char *s)
{
    while (sl != NULL)
    {
        if (strcmp(sl->s, s) == 0)
            return TRUE;

        sl = sl->next;
    }

    return FALSE;
}


/*
 * Set the name of a module.
 */
static void setModuleName(sipSpec *pt, moduleDef *mod, const char *fullname)
{
    mod->fullname = cacheName(pt, fullname);

    if (inMainModule())
        setIsUsedName(mod->fullname);

    if ((mod->name = strrchr(fullname, '.')) != NULL)
        mod->name++;
    else
        mod->name = fullname;
}


/*
 * Define a new class and set its name.
 */
static void defineClass(scopedNameDef *snd, classList *supers, optFlags *of)
{
    classDef *cd;
    typeHintDef *in, *out;

    getTypeHints(of, &in, &out);

    cd = newClass(currentSpec, class_iface, getAPIRange(of),
            fullyQualifiedName(snd), getVirtErrorHandler(of), in, out,
            getTypeHintValue(of));
    cd->supers = supers;

    pushScope(cd);
}


/*
 * Return a fully qualified scoped name.
 */
static scopedNameDef *fullyQualifiedName(scopedNameDef *snd)
{
    classDef *scope = currentScope();

    return scopeScopedName((scope != NULL ? scope->iff : NULL), snd);
}


/*
 * Complete the definition of a class.
 */
static classDef *completeClass(scopedNameDef *snd, optFlags *of, int has_def)
{
    classDef *cd = currentScope();

    /* See if the class was defined or just declared. */
    if (has_def)
    {
        if (snd->next != NULL)
            yyerror("A scoped name cannot be given in a class/struct definition");

    }
    else if (cd->supers != NULL)
        yyerror("Class/struct has super-classes but no definition");
    else
        setIsOpaque(cd);

    finishClass(currentSpec, currentModule, cd, of);
    popScope();

    /*
     * Check that external classes have only been declared at the global scope.
     */
    if (isExternal(cd) && currentScope() != NULL)
        yyerror("External classes/structs can only be declared in the global scope");

    return cd;
}


/*
 * Add a variable to the list so that the list remains sorted.
 */
static void addVariable(sipSpec *pt, varDef *vd)
{
    varDef **at = &pt->vars;

    while (*at != NULL)
    {
        if (strcmp(vd->pyname->text, (*at)->pyname->text) < 0)
            break;

        at = &(*at)->next;
    }

    vd->next = *at;
    *at = vd;
}


/*
 * Update a type according to optional flags.
 */
static void applyTypeFlags(moduleDef *mod, argDef *ad, optFlags *flags)
{
    ad->doctype = getDocType(flags);
    getTypeHints(flags, &ad->typehint_in, &ad->typehint_out);

    if (getOptFlag(flags, "PyInt", bool_flag) != NULL)
    {
        if (ad->atype == string_type)
            ad->atype = byte_type;
        else if (ad->atype == sstring_type)
            ad->atype = sbyte_type;
        else if (ad->atype == ustring_type)
            ad->atype = ubyte_type;
    }

    if (ad->atype == string_type && !isArray(ad) && !isReference(ad))
    {
        optFlag *of;

        if ((of = getOptFlag(flags, "Encoding", string_flag)) == NULL)
        {
            if (mod->encoding != no_type)
                ad->atype = mod->encoding;
            else
                ad->atype = string_type;
        }
        else if ((ad->atype = convertEncoding(of->fvalue.sval)) == no_type)
            yyerror("The value of the /Encoding/ annotation must be one of \"ASCII\", \"Latin-1\", \"UTF-8\" or \"None\"");
    }
}


/*
 * Return the keyword argument support converted from a string.
 */
static KwArgs convertKwArgs(const char *kwargs)
{
    if (strcmp(kwargs, "None") == 0)
        return NoKwArgs;

    if (strcmp(kwargs, "All") == 0)
        return AllKwArgs;

    if (strcmp(kwargs, "Optional") == 0)
        return OptionalKwArgs;

    yyerror("The style of keyword argument support must be one of \"All\", \"Optional\" or \"None\"");
}


/*
 * Return the Format for a string.
 */
static Format convertFormat(const char *format)
{
    if (strcmp(format, "raw") == 0)
        return raw;

    if (strcmp(format, "deindented") == 0)
        return deindented;

    yyerror("The docstring format must be either \"raw\" or \"deindented\"");
}


/*
 * Return the Signature for a string.
 */
static Signature convertSignature(const char *signature)
{
    if (strcmp(signature, "discarded") == 0)
        return discarded;

    if (strcmp(signature, "prepended") == 0)
        return prepended;

    if (strcmp(signature, "appended") == 0)
        return appended;

    yyerror("The docstring signature must be either \"discarded\", \"prepended\" or \"appended\"");
}


/*
 * Return the argument type for a string with the given encoding or no_type if
 * the encoding was invalid.
 */
static argType convertEncoding(const char *encoding)
{
    if (strcmp(encoding, "ASCII") == 0)
        return ascii_string_type;

    if (strcmp(encoding, "Latin-1") == 0)
        return latin1_string_type;

    if (strcmp(encoding, "UTF-8") == 0)
        return utf8_string_type;

    if (strcmp(encoding, "None") == 0)
        return string_type;

    return no_type;
}


/*
 * Get the /API/ option flag.
 */
static apiVersionRangeDef *getAPIRange(optFlags *optflgs)
{
    optFlag *of;

    if ((of = getOptFlag(optflgs, "API", api_range_flag)) == NULL)
        return NULL;

    return of->fvalue.aval;
}


/*
 * Return the API range structure and version number corresponding to the
 * given API range.
 */
static apiVersionRangeDef *convertAPIRange(moduleDef *mod, nameDef *name,
        int from, int to)
{
    int index;
    apiVersionRangeDef *avd, **avdp;

    /* Handle the trivial case. */
    if (from == 0 && to == 0)
        return NULL;

    for (index = 0, avdp = &mod->api_ranges; (*avdp) != NULL; avdp = &(*avdp)->next, ++index)
    {
        avd = *avdp;

        if (avd->api_name == name && avd->from == from && avd->to == to)
            return avd;
    }

    /* The new one must be appended so that version numbers remain valid. */
    avd = sipMalloc(sizeof (apiVersionRangeDef));

    avd->api_name = name;
    avd->from = from;
    avd->to = to;
    avd->index = index;

    avd->next = NULL;
    *avdp = avd;

    return avd;
}


/*
 * Return the style of keyword argument support for a signature.
 */
static KwArgs keywordArgs(moduleDef *mod, optFlags *optflgs, signatureDef *sd,
        int need_name)
{
    KwArgs kwargs;
    optFlag *ka_anno, *no_ka_anno;

    /* Get the default. */
    kwargs = mod->kwargs;

    /*
     * Get the possible annotations allowing /KeywordArgs/ to have different
     * types of values.
     */
    ka_anno = findOptFlag(optflgs, "KeywordArgs");
    no_ka_anno = getOptFlag(optflgs, "NoKeywordArgs", bool_flag);

    if (no_ka_anno != NULL)
    {
        if (ka_anno != NULL)
            yyerror("/KeywordArgs/ and /NoKeywordArgs/ cannot both be specified");

        deprecated("/NoKeywordArgs/ is deprecated, use /KeywordArgs=\"None\" instead");

        kwargs = NoKwArgs;
    }
    else if (ka_anno != NULL)
    {
        /* A string value is the non-deprecated type. */
        if (ka_anno->ftype == string_flag)
        {
            kwargs = convertKwArgs(ka_anno->fvalue.sval);
        }
        else
        {
            deprecated("/KeywordArgs/ is deprecated, use /KeywordArgs=\"All\" instead");

            /* Get it again to check the type. */
            ka_anno = getOptFlag(optflgs, "KeywordArgs", bool_flag);
        }
    }

    /* An ellipsis cannot be used with keyword arguments. */
    if (sd->nrArgs > 0 && sd->args[sd->nrArgs - 1].atype == ellipsis_type)
        kwargs = NoKwArgs;

    if (kwargs != NoKwArgs)
    {
        int a, is_name = FALSE;

        /*
         * Mark argument names as being used and check there is at least one.
         */
        for (a = 0; a < sd->nrArgs; ++a)
        {
            argDef *ad = &sd->args[a];

            if (kwargs == OptionalKwArgs && ad->defval == NULL)
                continue;

            if (ad->name != NULL)
            {
                if (need_name || inMainModule())
                    setIsUsedName(ad->name);

                is_name = TRUE;
            }
        }

        if (!is_name)
            kwargs = NoKwArgs;
    }

    return kwargs;
}


/*
 * Extract the version of a string value optionally associated with a
 * particular feature.
 */
static char *convertFeaturedString(char *fs)
{
    while (fs != NULL)
    {
        char *next, *value;

        /* Individual values are ';' separated. */
        if ((next = strchr(fs, ';')) != NULL)
            *next++ = '\0';

        /* Features and values are ':' separated. */
        if ((value = strchr(fs, ':')) == NULL)
        {
            /* This is an unconditional value so just return it. */
            return strip(fs);
        }

        *value++ = '\0';

        if (isEnabledFeature(strip(fs)))
            return strip(value);

        fs = next;
    }

    /* No value was enabled. */
    return NULL;
}


/*
 * Return the stripped version of a string.
 */
static char *strip(char *s)
{
    while (*s == ' ')
        ++s;

    if (*s != '\0')
    {
        char *cp = &s[strlen(s) - 1];

        while (*cp == ' ')
            *cp-- = '\0';
    }

    return s;
}


/*
 * Return TRUE if the given feature is enabled.
 */
static int isEnabledFeature(const char *name)
{
    qualDef *qd;

    if ((qd = findQualifier(name)) == NULL || qd->qtype != feature_qualifier)
        yyerror("No such feature");

    return !excludedFeature(excludedQualifiers, qd);
}


/*
 * Add a property definition to a class.
 */
static void addProperty(sipSpec *pt, moduleDef *mod, classDef *cd,
        const char *name, const char *get, const char *set,
        docstringDef *docstring)
{
    propertyDef *pd;

    checkAttributes(pt, mod, cd, NULL, name, FALSE);

    pd = sipMalloc(sizeof (propertyDef));

    pd->name = cacheName(pt, name);
    pd->get = get;
    pd->set = set;
    pd->docstring = docstring;
    pd->platforms = currentPlatforms;
    pd->next = cd->properties;

    cd->properties = pd;

    if (inMainModule())
        setIsUsedName(pd->name);
}


/*
 * Configure a module and return the (possibly new) current module.
 */
static moduleDef *configureModule(sipSpec *pt, moduleDef *module,
        const char *filename, const char *name, int c_module, KwArgs kwargs,
        int use_arg_names, int use_limited_api, int call_super_init,
        int all_raise_py_exc, const char *def_error_handler,
        docstringDef *docstring)
{
    moduleDef *mod;

    /* Check the module hasn't already been defined. */
    for (mod = pt->modules; mod != NULL; mod = mod->next)
        if (mod->fullname != NULL && strcmp(mod->fullname->text, name) == 0)
            yyerror("Module is already defined");

    /*
     * If we are in a container module then create a component module and make
     * it current.
     */
    if (isContainer(module) || module->container != NULL)
    {
        mod = allocModule();

        mod->file = filename;
        mod->container = (isContainer(module) ? module : module->container);

        module = mod;
    }

    setModuleName(pt, module, name);
    module->kwargs = kwargs;
    module->virt_error_handler = def_error_handler;
    module->docstring = docstring;

    if (all_raise_py_exc)
        setAllRaisePyException(module);

    if (use_arg_names)
        setUseArgNames(module);

    if (use_limited_api)
        setUseLimitedAPI(module);

    if (call_super_init == 0)
        setCallSuperInitNo(module);
    else if (call_super_init > 0)
        setCallSuperInitYes(module);

    if (pt->genc < 0)
        pt->genc = c_module;
    else if (pt->genc != c_module)
        yyerror("Cannot mix C and C++ modules");

    return module;
}


/*
 * Add a Python naming rule to a module.
 */
static void addAutoPyName(moduleDef *mod, const char *remove_leading)
{
    autoPyNameDef *apnd, **apndp;

    for (apndp = &mod->autopyname; *apndp != NULL; apndp = &(*apndp)->next)
        ;

    apnd = sipMalloc(sizeof (autoPyNameDef));
    apnd->remove_leading = remove_leading;
    apnd->next = *apndp;

    *apndp = apnd;
}


/*
 * Check that no invalid or unknown annotations are given.
 */
static void checkAnnos(optFlags *annos, const char *valid[])
{
    if (parsingCSignature && annos->nrFlags != 0)
    {
        deprecated("Annotations should not be used in explicit C/C++ signatures");
    }
    else
    {
        int i;

        for (i = 0; i < annos->nrFlags; i++)
        {
            const char **name;

            for (name = valid; *name != NULL; ++name)
                if (strcmp(*name, annos->flags[i].fname) == 0)
                    break;

            if (*name == NULL)
                yywarning("Annotation is unknown");
        }
    }
}


/*
 * Check that no annotations were given.
 */
static void checkNoAnnos(optFlags *annos, const char *msg)
{
    if (annos->nrFlags != 0)
        deprecated(msg);
}


/*
 * Handle any /KeepReference/ annotation for a type.
 */
static void handleKeepReference(optFlags *optflgs, argDef *ad, moduleDef *mod)
{
    optFlag *of;

    if ((of = getOptFlag(optflgs, "KeepReference", opt_integer_flag)) != NULL)
    {
        setKeepReference(ad);

        if ((ad->key = of->fvalue.ival) < -1)
            yyerror("/KeepReference/ key cannot be negative");

        /* If there was no explicit key then auto-allocate one. */
        if (ad->key == -1)
            ad->key = mod->next_key--;
    }
}


/*
 * Configure the mapped type annotations that are also valid with mapped type
 * templates.
 */
static void mappedTypeAnnos(mappedTypeDef *mtd, optFlags *optflgs)
{
    if (getOptFlag(optflgs, "NoRelease", bool_flag) != NULL)
        setNoRelease(mtd);

    if (getAllowNone(optflgs))
        setHandlesNone(mtd);

    mtd->doctype = getDocType(optflgs);
    getTypeHints(optflgs, &mtd->typehint_in, &mtd->typehint_out);
    mtd->typehint_value = getTypeHintValue(optflgs);
}


/*
 * Initialise an argument with the derefences of another, plus a new one.
 */
static void add_new_deref(argDef *new, argDef *orig, int isconst)
{
    if ((new->nrderefs = orig->nrderefs + 1) >= MAX_NR_DEREFS)
        yyerror("Internal error - increase the value of MAX_NR_DEREFS");

    memcpy(&new->derefs[0], &orig->derefs[0], sizeof (new->derefs));
    new->derefs[orig->nrderefs] = isconst;
}


/*
 * Add the dereferences from one type to another.
 */
static void add_derefs(argDef *dst, argDef *src)
{
    int i;

    for (i = 0; i < src->nrderefs; ++i)
    {
        if (dst->nrderefs >= MAX_NR_DEREFS - 1)
            fatal("Internal error - increase the value of MAX_NR_DEREFS\n");

        dst->derefs[dst->nrderefs++] = src->derefs[i];
    }
}


/*
 * Check if a word is a Python keyword (or has been at any time).
 */
int isPyKeyword(const char *word)
{
    static const char *kwds[] = {
        "False", "None", "True", "and", "as", "assert", "break", "class",
        "continue", "def", "del", "elif", "else", "except", "finally", "for",
        "from", "global", "if", "import", "in", "is", "lambda", "nonlocal",
        "not", "or", "pass", "raise", "return", "try", "while", "with'"
        "yield",
        /* Historical keywords. */
        "exec", "print",
        NULL
    };

    const char **kwd;

    for (kwd = kwds; *kwd != NULL; ++kwd)
        if (strcmp(*kwd, word) == 0)
            return TRUE;

    return FALSE;
}


/*
 * Check there is nothing after an ellipsis.
 */
static void checkEllipsis(signatureDef *sd)
{
    int a;

    for (a = 0; a < sd->nrArgs; ++a)
        if (sd->args[a].atype == ellipsis_type && a < sd->nrArgs - 1)
            yyerror("An ellipsis must be at the end of the argument list if /NoArgParser/ is not specified");
}

