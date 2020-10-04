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
     TK_PYSSIZET = 346,
     TK_SIZET = 347,
     TK_NUMBER_VALUE = 348,
     TK_REAL_VALUE = 349,
     TK_TYPEDEF = 350,
     TK_NAMESPACE = 351,
     TK_TIMELINE = 352,
     TK_PLATFORMS = 353,
     TK_FEATURE = 354,
     TK_LICENSE = 355,
     TK_QCHAR_VALUE = 356,
     TK_TRUE_VALUE = 357,
     TK_FALSE_VALUE = 358,
     TK_NULL_VALUE = 359,
     TK_OPERATOR = 360,
     TK_THROW = 361,
     TK_EXCEPTION = 362,
     TK_RAISECODE = 363,
     TK_EXPLICIT = 364,
     TK_TEMPLATE = 365,
     TK_FINAL = 366,
     TK_ELLIPSIS = 367,
     TK_DEFMETATYPE = 368,
     TK_DEFSUPERTYPE = 369,
     TK_PROPERTY = 370,
     TK_HIDE_NS = 371,
     TK_FORMAT = 372,
     TK_GET = 373,
     TK_ID = 374,
     TK_KWARGS = 375,
     TK_LANGUAGE = 376,
     TK_LICENSEE = 377,
     TK_NAME = 378,
     TK_OPTIONAL = 379,
     TK_ORDER = 380,
     TK_REMOVELEADING = 381,
     TK_SET = 382,
     TK_SIGNATURE = 383,
     TK_TIMESTAMP = 384,
     TK_TYPE = 385,
     TK_USEARGNAMES = 386,
     TK_USELIMITEDAPI = 387,
     TK_ALLRAISEPYEXC = 388,
     TK_CALLSUPERINIT = 389,
     TK_DEFERRORHANDLER = 390,
     TK_VERSION = 391
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
#define TK_PYSSIZET 346
#define TK_SIZET 347
#define TK_NUMBER_VALUE 348
#define TK_REAL_VALUE 349
#define TK_TYPEDEF 350
#define TK_NAMESPACE 351
#define TK_TIMELINE 352
#define TK_PLATFORMS 353
#define TK_FEATURE 354
#define TK_LICENSE 355
#define TK_QCHAR_VALUE 356
#define TK_TRUE_VALUE 357
#define TK_FALSE_VALUE 358
#define TK_NULL_VALUE 359
#define TK_OPERATOR 360
#define TK_THROW 361
#define TK_EXCEPTION 362
#define TK_RAISECODE 363
#define TK_EXPLICIT 364
#define TK_TEMPLATE 365
#define TK_FINAL 366
#define TK_ELLIPSIS 367
#define TK_DEFMETATYPE 368
#define TK_DEFSUPERTYPE 369
#define TK_PROPERTY 370
#define TK_HIDE_NS 371
#define TK_FORMAT 372
#define TK_GET 373
#define TK_ID 374
#define TK_KWARGS 375
#define TK_LANGUAGE 376
#define TK_LICENSEE 377
#define TK_NAME 378
#define TK_OPTIONAL 379
#define TK_ORDER 380
#define TK_REMOVELEADING 381
#define TK_SET 382
#define TK_SIGNATURE 383
#define TK_TIMESTAMP 384
#define TK_TYPE 385
#define TK_USEARGNAMES 386
#define TK_USELIMITEDAPI 387
#define TK_ALLRAISEPYEXC 388
#define TK_CALLSUPERINIT 389
#define TK_DEFERRORHANDLER 390
#define TK_VERSION 391




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
static stringList **mainModuleSipFiles; /* The list of .sip files for the main module. */
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
static moduleDef *allocModule(void);
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
static const char *getTypeHintValue(optFlags *optflgs);
static void getTypeHints(optFlags *optflgs, typeHintDef **in,
        typeHintDef **out);
static int getNoTypeHint(optFlags *optflgs);
static void templateSignature(signatureDef *sd, KwArgs kwargs, int result,
        classTmplDef *tcd, templateDef *td, classDef *ncd,
        scopedNameDef *type_names, scopedNameDef *type_values);
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
#line 202 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 595 "/Users/phil/hg/sip/code_generator/parser.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 608 "/Users/phil/hg/sip/code_generator/parser.c"

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
#define YYLAST   1375

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  159
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  238
/* YYNRULES -- Number of rules.  */
#define YYNRULES  553
/* YYNRULES -- Number of states.  */
#define YYNSTATES  953

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   391

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   144,     2,     2,     2,   157,   149,     2,
     137,   138,   147,   146,   139,   145,     2,   148,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   154,   143,
     152,   140,   153,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   155,     2,   156,   158,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   141,   150,   142,   151,     2,     2,     2,
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
     135,   136
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
     630,   634,   636,   640,   644,   647,   650,   653,   656,   659,
     662,   665,   668,   671,   674,   677,   680,   683,   686,   689,
     692,   695,   698,   701,   704,   707,   710,   713,   716,   719,
     722,   726,   728,   732,   736,   740,   741,   743,   747,   749,
     753,   757,   761,   762,   764,   768,   770,   774,   776,   780,
     784,   788,   791,   793,   796,   797,   807,   808,   810,   812,
     813,   815,   816,   818,   820,   823,   825,   827,   832,   833,
     835,   836,   839,   840,   843,   845,   849,   851,   853,   855,
     857,   859,   861,   862,   864,   866,   868,   870,   872,   874,
     878,   879,   883,   886,   888,   890,   894,   896,   898,   900,
     902,   907,   910,   912,   914,   916,   918,   920,   922,   923,
     925,   929,   936,   949,   950,   951,   960,   961,   965,   970,
     971,   972,   981,   982,   985,   987,   991,   994,   995,   997,
     999,  1001,  1002,  1006,  1007,  1009,  1012,  1014,  1016,  1018,
    1020,  1022,  1024,  1026,  1028,  1030,  1032,  1034,  1036,  1038,
    1040,  1042,  1044,  1046,  1048,  1050,  1052,  1054,  1056,  1058,
    1060,  1062,  1064,  1066,  1068,  1071,  1074,  1077,  1081,  1085,
    1089,  1092,  1096,  1100,  1102,  1106,  1110,  1114,  1118,  1119,
    1124,  1126,  1129,  1131,  1133,  1135,  1137,  1139,  1140,  1142,
    1143,  1147,  1149,  1161,  1162,  1166,  1168,  1180,  1181,  1182,
    1189,  1190,  1191,  1199,  1217,  1225,  1243,  1260,  1262,  1264,
    1266,  1268,  1270,  1272,  1274,  1276,  1279,  1282,  1285,  1288,
    1291,  1294,  1297,  1300,  1303,  1306,  1310,  1314,  1316,  1319,
    1322,  1324,  1327,  1330,  1333,  1335,  1338,  1339,  1341,  1342,
    1344,  1345,  1348,  1349,  1353,  1355,  1359,  1361,  1365,  1367,
    1373,  1375,  1377,  1378,  1381,  1382,  1385,  1386,  1389,  1390,
    1393,  1395,  1396,  1398,  1402,  1405,  1406,  1410,  1411,  1415,
    1417,  1418,  1422,  1424,  1426,  1428,  1429,  1433,  1435,  1441,
    1442,  1446,  1448,  1451,  1453,  1455,  1458,  1461,  1464,  1469,
    1473,  1477,  1478,  1480,  1481,  1485,  1488,  1490,  1495,  1498,
    1501,  1503,  1505,  1508,  1510,  1512,  1515,  1518,  1522,  1524,
    1526,  1528,  1531,  1534,  1536,  1538,  1540,  1542,  1544,  1546,
    1548,  1550,  1552,  1554,  1556,  1558,  1560,  1562,  1564,  1568,
    1569,  1574,  1575,  1577
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     160,     0,    -1,   161,    -1,   160,   161,    -1,    -1,   162,
     163,    -1,   177,    -1,   185,    -1,   239,    -1,   264,    -1,
     165,    -1,   169,    -1,   173,    -1,   227,    -1,   231,    -1,
     265,    -1,   286,    -1,   298,    -1,   210,    -1,   235,    -1,
     260,    -1,   256,    -1,   282,    -1,   223,    -1,   194,    -1,
     246,    -1,   279,    -1,   266,    -1,   206,    -1,   281,    -1,
     283,    -1,   214,    -1,   287,    -1,   284,    -1,   285,    -1,
     181,    -1,   196,    -1,   164,    -1,   218,    -1,   222,    -1,
     202,    -1,   326,    -1,   332,    -1,   329,    -1,   188,    -1,
     325,    -1,   304,    -1,   361,    -1,   385,    -1,   267,    -1,
       5,   166,    -1,    32,    -1,   137,   167,   138,    -1,   168,
      -1,   167,   139,   168,    -1,   123,   140,    32,    -1,     6,
     170,    -1,    32,    -1,   137,   171,   138,    -1,   172,    -1,
     171,   139,   172,    -1,   123,   140,    32,    -1,     7,   174,
      -1,    32,    -1,   137,   175,   138,    -1,   176,    -1,   175,
     139,   176,    -1,   123,   140,    32,    -1,     8,   178,    -1,
      30,    -1,   137,   179,   138,    -1,   180,    -1,   179,   139,
     180,    -1,   123,   140,    30,    -1,     9,   182,   302,    -1,
      30,    -1,   137,   183,   138,    -1,   184,    -1,   183,   139,
     184,    -1,   123,   140,    30,    -1,     3,   137,   186,   138,
      -1,   187,    -1,   186,   139,   187,    -1,   123,   140,   349,
      -1,   136,   140,    93,    -1,   107,   319,   189,   366,   190,
      -1,    -1,   137,   319,   138,    -1,   141,   191,   142,   143,
      -1,   192,    -1,   191,   192,    -1,   218,    -1,   222,    -1,
     193,    -1,   267,    -1,   108,   302,    -1,    -1,    26,   393,
     366,   195,   198,    -1,    -1,   331,    26,   393,   366,   197,
     198,    -1,   141,   199,   142,   143,    -1,   200,    -1,   199,
     200,    -1,   218,    -1,   222,    -1,   267,    -1,   280,    -1,
      47,   302,    -1,    48,   302,    -1,   276,    -1,   304,    -1,
     201,    -1,    90,   389,    30,   137,   374,   138,   363,   395,
     366,   359,   143,   297,   372,   371,    -1,    -1,    96,    30,
     203,   204,   143,    -1,    -1,   141,   205,   142,    -1,   164,
      -1,   205,   164,    -1,    -1,    98,   207,   141,   208,   142,
      -1,   209,    -1,   208,   209,    -1,    30,    -1,    99,   211,
      -1,    30,    -1,   137,   212,   138,    -1,   213,    -1,   212,
     139,   213,    -1,   123,   140,   349,    -1,    -1,    97,   215,
     141,   216,   142,    -1,   217,    -1,   216,   217,    -1,    30,
      -1,    -1,    28,   137,   219,   221,   138,    -1,    30,    -1,
     144,    30,    -1,   220,    88,    30,    -1,   220,    88,   144,
      30,    -1,   220,    -1,   307,   145,   307,    -1,    29,    -1,
     100,   224,    -1,    -1,    32,    -1,   137,   225,   138,    -1,
     226,    -1,   225,   139,   226,    -1,   130,   140,    32,    -1,
     122,   140,    32,    -1,   128,   140,    32,    -1,   129,   140,
      32,    -1,   113,   228,    -1,   254,    -1,   137,   229,   138,
      -1,   230,    -1,   229,   139,   230,    -1,   123,   140,   254,
      -1,   114,   232,    -1,   254,    -1,   137,   233,   138,    -1,
     234,    -1,   233,   139,   234,    -1,   123,   140,   254,    -1,
     116,   236,    -1,   319,    -1,   137,   237,   138,    -1,   238,
      -1,   237,   139,   238,    -1,   123,   140,   319,    -1,    56,
     240,   243,    -1,   254,    -1,   137,   241,   138,    -1,   242,
      -1,   241,   139,   242,    -1,   123,   140,   254,    -1,    -1,
     141,   244,   142,   143,    -1,   245,    -1,   244,   245,    -1,
     218,    -1,   222,    -1,   293,    -1,    55,   247,   251,    -1,
      -1,   254,   248,    -1,   137,   249,   138,    -1,   250,    -1,
     249,   139,   250,    -1,   120,   140,    32,    -1,   121,   140,
      32,    -1,   123,   140,   254,    -1,   131,   140,   322,    -1,
     132,   140,   322,    -1,   133,   140,   322,    -1,   134,   140,
     322,    -1,   135,   140,    30,    -1,    -1,   141,   252,   142,
     143,    -1,   253,    -1,   252,   253,    -1,   218,    -1,   222,
      -1,   289,    -1,   293,    -1,    30,    -1,    31,    -1,    -1,
      93,    -1,    50,   257,    -1,    31,    -1,   137,   258,   138,
      -1,   259,    -1,   258,   139,   259,    -1,   123,   140,    31,
      -1,   124,   140,   322,    -1,    51,   261,    -1,    31,    -1,
     137,   262,   138,    -1,   263,    -1,   262,   139,   263,    -1,
     123,   140,    31,    -1,    25,   302,    -1,    52,   302,    -1,
      53,   302,    -1,    54,   302,    -1,    34,   302,    -1,    35,
     302,    -1,    36,   302,    -1,    37,   302,    -1,    38,   302,
      -1,    39,   302,    -1,    40,   302,    -1,    41,   302,    -1,
      46,   302,    -1,    42,   302,    -1,    20,   302,    -1,    23,
     302,    -1,    24,   302,    -1,    17,   302,    -1,    18,   302,
      -1,    19,   302,    -1,    21,   302,    -1,    22,   302,    -1,
      10,   302,    -1,    11,   302,    -1,    11,   302,    -1,     4,
     290,    -1,   137,   291,   138,    -1,   292,    -1,   291,   139,
     292,    -1,   126,   140,    32,    -1,    12,   294,   302,    -1,
      -1,    32,    -1,   137,   295,   138,    -1,   296,    -1,   295,
     139,   296,    -1,   117,   140,    32,    -1,   128,   140,    32,
      -1,    -1,   293,    -1,    13,   299,   302,    -1,    30,    -1,
     137,   300,   138,    -1,   301,    -1,   300,   139,   301,    -1,
     119,   140,    30,    -1,   125,   140,    93,    -1,   303,    29,
      -1,    27,    -1,   303,    27,    -1,    -1,    84,   306,   307,
     366,   305,   141,   308,   142,   143,    -1,    -1,    57,    -1,
      58,    -1,    -1,    30,    -1,    -1,   309,    -1,   310,    -1,
     309,   310,    -1,   218,    -1,   222,    -1,    30,   312,   366,
     311,    -1,    -1,   139,    -1,    -1,   140,   317,    -1,    -1,
     140,   314,    -1,   317,    -1,   314,   315,   317,    -1,   145,
      -1,   146,    -1,   147,    -1,   148,    -1,   149,    -1,   150,
      -1,    -1,   144,    -1,   151,    -1,   145,    -1,   146,    -1,
     147,    -1,   149,    -1,   318,   316,   323,    -1,    -1,   137,
     319,   138,    -1,    87,   320,    -1,   320,    -1,   321,    -1,
     320,    87,   321,    -1,    30,    -1,   102,    -1,   103,    -1,
     319,    -1,   393,   137,   324,   138,    -1,   141,   142,    -1,
      94,    -1,    93,    -1,   322,    -1,   104,    -1,    32,    -1,
     101,    -1,    -1,   314,    -1,   324,   139,   314,    -1,    95,
     389,    30,   366,   143,   297,    -1,    95,   389,   137,   147,
      30,   138,   137,   394,   138,   366,   143,   297,    -1,    -1,
      -1,    58,   319,   327,   335,   366,   328,   339,   143,    -1,
      -1,   331,   330,   332,    -1,   110,   152,   394,   153,    -1,
      -1,    -1,    57,   319,   333,   335,   366,   334,   339,   143,
      -1,    -1,   154,   336,    -1,   337,    -1,   336,   139,   337,
      -1,   338,   319,    -1,    -1,    59,    -1,    60,    -1,    61,
      -1,    -1,   141,   340,   142,    -1,    -1,   341,    -1,   340,
     341,    -1,   218,    -1,   222,    -1,   202,    -1,   326,    -1,
     332,    -1,   329,    -1,   188,    -1,   325,    -1,   304,    -1,
     342,    -1,   293,    -1,   280,    -1,   267,    -1,   268,    -1,
     269,    -1,   270,    -1,   271,    -1,   272,    -1,   273,    -1,
     274,    -1,   275,    -1,   276,    -1,   277,    -1,   278,    -1,
     288,    -1,   354,    -1,   351,    -1,   377,    -1,    49,   302,
      -1,    48,   302,    -1,    47,   302,    -1,    59,   350,   154,
      -1,    60,   350,   154,    -1,    61,   350,   154,    -1,    62,
     154,    -1,   115,   343,   346,    -1,   137,   344,   138,    -1,
     345,    -1,   344,   139,   345,    -1,   118,   140,    30,    -1,
     123,   140,   349,    -1,   127,   140,    30,    -1,    -1,   141,
     347,   142,   143,    -1,   348,    -1,   347,   348,    -1,   218,
      -1,   222,    -1,   293,    -1,    30,    -1,    32,    -1,    -1,
      64,    -1,    -1,    83,   352,   353,    -1,   353,    -1,   151,
      30,   137,   138,   395,   365,   366,   143,   372,   371,   373,
      -1,    -1,   109,   355,   356,    -1,   356,    -1,    30,   137,
     374,   138,   395,   366,   357,   143,   297,   372,   371,    -1,
      -1,    -1,   155,   358,   137,   374,   138,   156,    -1,    -1,
      -1,   155,   360,   389,   137,   374,   138,   156,    -1,   389,
      30,   137,   374,   138,   363,   364,   395,   365,   366,   359,
     143,   297,   372,   371,   373,   370,    -1,   389,   105,   140,
     137,   389,   138,   143,    -1,   389,   105,   362,   137,   374,
     138,   363,   364,   395,   365,   366,   359,   143,   372,   371,
     373,   370,    -1,   105,   389,   137,   374,   138,   363,   364,
     395,   365,   366,   359,   143,   372,   371,   373,   370,    -1,
     146,    -1,   145,    -1,   147,    -1,   148,    -1,   157,    -1,
     149,    -1,   150,    -1,   158,    -1,   152,   152,    -1,   153,
     153,    -1,   146,   140,    -1,   145,   140,    -1,   147,   140,
      -1,   148,   140,    -1,   157,   140,    -1,   149,   140,    -1,
     150,   140,    -1,   158,   140,    -1,   152,   152,   140,    -1,
     153,   153,   140,    -1,   151,    -1,   137,   138,    -1,   155,
     156,    -1,   152,    -1,   152,   140,    -1,   140,   140,    -1,
     144,   140,    -1,   153,    -1,   153,   140,    -1,    -1,    89,
      -1,    -1,   111,    -1,    -1,   140,    93,    -1,    -1,   148,
     367,   148,    -1,   368,    -1,   367,   139,   368,    -1,    30,
      -1,    30,   140,   369,    -1,   254,    -1,    30,   154,   255,
     145,   255,    -1,    32,    -1,    93,    -1,    -1,    43,   302,
      -1,    -1,    44,   302,    -1,    -1,    45,   302,    -1,    -1,
      33,   302,    -1,   375,    -1,    -1,   376,    -1,   375,   139,
     376,    -1,   390,   313,    -1,    -1,    63,   378,   380,    -1,
      -1,    65,   379,   380,    -1,   380,    -1,    -1,    90,   381,
     382,    -1,   382,    -1,   383,    -1,   385,    -1,    -1,    83,
     384,   361,    -1,   361,    -1,   389,    30,   366,   386,   143,
      -1,    -1,   141,   387,   142,    -1,   388,    -1,   387,   388,
      -1,   218,    -1,   222,    -1,    14,   302,    -1,    15,   302,
      -1,    16,   302,    -1,    89,   393,   392,   391,    -1,   393,
     392,   391,    -1,   389,   307,   366,    -1,    -1,   149,    -1,
      -1,   392,   147,    89,    -1,   392,   147,    -1,   319,    -1,
     319,   152,   394,   153,    -1,    58,   319,    -1,    86,    67,
      -1,    67,    -1,    86,    -1,    86,    68,    -1,    68,    -1,
      69,    -1,    86,    69,    -1,    69,    69,    -1,    86,    69,
      69,    -1,    70,    -1,    71,    -1,    66,    -1,    85,    72,
      -1,    86,    72,    -1,    72,    -1,    73,    -1,    74,    -1,
      75,    -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,
      80,    -1,    81,    -1,    82,    -1,    91,    -1,    92,    -1,
     112,    -1,   389,    -1,   394,   139,   389,    -1,    -1,   106,
     137,   396,   138,    -1,    -1,   319,    -1,   396,   139,   319,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   546,   546,   547,   550,   550,   569,   570,   571,   572,
     573,   574,   575,   576,   577,   578,   579,   580,   581,   582,
     583,   584,   585,   586,   587,   588,   589,   590,   591,   592,
     593,   594,   595,   596,   597,   598,   599,   600,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     627,   633,   638,   643,   644,   654,   661,   667,   672,   677,
     678,   688,   695,   704,   709,   714,   715,   725,   732,   743,
     748,   753,   754,   764,   771,   800,   805,   810,   811,   821,
     828,   856,   857,   868,   874,   882,   929,   933,  1040,  1045,
    1046,  1057,  1060,  1063,  1077,  1093,  1098,  1098,  1120,  1120,
    1186,  1200,  1201,  1204,  1205,  1206,  1210,  1214,  1223,  1232,
    1241,  1242,  1245,  1260,  1260,  1297,  1298,  1301,  1302,  1305,
    1305,  1334,  1335,  1338,  1344,  1350,  1355,  1360,  1361,  1371,
    1378,  1378,  1404,  1405,  1408,  1414,  1414,  1433,  1436,  1439,
    1442,  1447,  1448,  1453,  1461,  1477,  1485,  1491,  1496,  1497,
    1510,  1518,  1526,  1534,  1544,  1555,  1560,  1565,  1566,  1576,
    1583,  1594,  1599,  1604,  1605,  1615,  1622,  1634,  1639,  1644,
    1645,  1655,  1662,  1680,  1685,  1690,  1691,  1701,  1708,  1712,
    1717,  1718,  1728,  1731,  1734,  1748,  1761,  1761,  1771,  1776,
    1777,  1794,  1806,  1824,  1836,  1848,  1860,  1872,  1884,  1898,
    1902,  1907,  1908,  1918,  1921,  1924,  1927,  1941,  1942,  1958,
    1961,  1964,  1973,  1979,  1984,  1985,  1996,  2002,  2010,  2016,
    2021,  2026,  2027,  2037,  2044,  2050,  2056,  2062,  2067,  2072,
    2077,  2082,  2087,  2092,  2097,  2102,  2107,  2112,  2117,  2122,
    2128,  2133,  2139,  2145,  2151,  2157,  2163,  2169,  2175,  2180,
    2186,  2191,  2192,  2202,  2209,  2293,  2297,  2303,  2308,  2309,
    2320,  2326,  2334,  2337,  2340,  2349,  2355,  2360,  2361,  2372,
    2378,  2389,  2392,  2393,  2403,  2403,  2427,  2430,  2433,  2438,
    2441,  2446,  2447,  2450,  2451,  2454,  2455,  2456,  2500,  2501,
    2504,  2505,  2508,  2511,  2516,  2517,  2535,  2538,  2541,  2544,
    2547,  2550,  2555,  2558,  2561,  2564,  2567,  2570,  2573,  2578,
    2593,  2596,  2601,  2607,  2610,  2611,  2619,  2624,  2627,  2632,
    2641,  2651,  2654,  2658,  2662,  2666,  2670,  2674,  2680,  2685,
    2691,  2709,  2730,  2768,  2774,  2768,  2818,  2818,  2844,  2849,
    2855,  2849,  2895,  2896,  2899,  2900,  2903,  2955,  2958,  2961,
    2964,  2969,  2972,  2977,  2978,  2979,  2982,  2983,  2984,  2985,
    2986,  2987,  2988,  2989,  2990,  2991,  2992,  3003,  3007,  3011,
    3022,  3033,  3044,  3055,  3058,  3061,  3064,  3067,  3078,  3089,
    3100,  3111,  3112,  3113,  3114,  3125,  3136,  3147,  3154,  3161,
    3168,  3177,  3190,  3195,  3196,  3208,  3215,  3222,  3231,  3235,
    3240,  3241,  3251,  3254,  3257,  3271,  3272,  3275,  3278,  3284,
    3284,  3285,  3288,  3354,  3354,  3355,  3358,  3404,  3407,  3407,
    3418,  3421,  3421,  3433,  3451,  3471,  3515,  3593,  3594,  3595,
    3596,  3597,  3598,  3599,  3600,  3601,  3602,  3603,  3604,  3605,
    3606,  3607,  3608,  3609,  3610,  3611,  3612,  3613,  3614,  3615,
    3616,  3617,  3618,  3619,  3620,  3621,  3624,  3627,  3632,  3635,
    3640,  3643,  3651,  3654,  3660,  3664,  3676,  3680,  3686,  3690,
    3713,  3717,  3723,  3726,  3731,  3734,  3739,  3742,  3747,  3750,
    3755,  3778,  3783,  3789,  3812,  3819,  3819,  3820,  3820,  3821,
    3825,  3825,  3826,  3830,  3831,  3835,  3835,  3836,  3839,  3863,
    3869,  3874,  3875,  3887,  3890,  3893,  3908,  3923,  3940,  3945,
    3959,  4067,  4070,  4078,  4081,  4084,  4089,  4097,  4108,  4123,
    4127,  4131,  4135,  4139,  4143,  4147,  4151,  4155,  4159,  4163,
    4167,  4171,  4175,  4179,  4183,  4187,  4191,  4195,  4199,  4203,
    4207,  4211,  4215,  4219,  4223,  4227,  4231,  4237,  4243,  4259,
    4262,  4270,  4276,  4283
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
  "TK_SCOPE", "TK_LOGICAL_OR", "TK_CONST", "TK_STATIC", "TK_PYSSIZET",
  "TK_SIZET", "TK_NUMBER_VALUE", "TK_REAL_VALUE", "TK_TYPEDEF",
  "TK_NAMESPACE", "TK_TIMELINE", "TK_PLATFORMS", "TK_FEATURE",
  "TK_LICENSE", "TK_QCHAR_VALUE", "TK_TRUE_VALUE", "TK_FALSE_VALUE",
  "TK_NULL_VALUE", "TK_OPERATOR", "TK_THROW", "TK_EXCEPTION",
  "TK_RAISECODE", "TK_EXPLICIT", "TK_TEMPLATE", "TK_FINAL", "TK_ELLIPSIS",
  "TK_DEFMETATYPE", "TK_DEFSUPERTYPE", "TK_PROPERTY", "TK_HIDE_NS",
  "TK_FORMAT", "TK_GET", "TK_ID", "TK_KWARGS", "TK_LANGUAGE",
  "TK_LICENSEE", "TK_NAME", "TK_OPTIONAL", "TK_ORDER", "TK_REMOVELEADING",
  "TK_SET", "TK_SIGNATURE", "TK_TIMESTAMP", "TK_TYPE", "TK_USEARGNAMES",
  "TK_USELIMITEDAPI", "TK_ALLRAISEPYEXC", "TK_CALLSUPERINIT",
  "TK_DEFERRORHANDLER", "TK_VERSION", "'('", "')'", "','", "'='", "'{'",
  "'}'", "';'", "'!'", "'-'", "'+'", "'*'", "'/'", "'&'", "'|'", "'~'",
  "'<'", "'>'", "':'", "'['", "']'", "'%'", "'^'", "$accept",
  "specification", "statement", "@1", "modstatement", "nsstatement",
  "defdocstringfmt", "defdocstringfmt_args", "defdocstringfmt_arg_list",
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
  "copying", "exphdrcode", "modhdrcode", "typehdrcode", "travcode",
  "clearcode", "getbufcode", "releasebufcode", "readbufcode",
  "writebufcode", "segcountcode", "charbufcode", "instancecode",
  "picklecode", "finalcode", "modcode", "typecode", "preinitcode",
  "initcode", "postinitcode", "unitcode", "unitpostinccode",
  "exptypehintcode", "modtypehintcode", "classtypehintcode", "autopyname",
  "autopyname_args", "autopyname_arg_list", "autopyname_arg", "docstring",
  "docstring_args", "docstring_arg_list", "docstring_arg", "optdocstring",
  "extract", "extract_args", "extract_arg_list", "extract_arg",
  "codeblock", "codelines", "enum", "@9", "optenumkey", "optname",
  "optenumbody", "enumbody", "enumline", "optcomma", "optenumassign",
  "optassign", "expr", "binop", "optunop", "value", "optcast",
  "scopedname", "scopednamehead", "scopepart", "bool_value", "simplevalue",
  "exprlist", "typedef", "struct", "@10", "@11", "classtmpl", "@12",
  "template", "class", "@13", "@14", "superclasses", "superlist",
  "superclass", "class_access", "optclassbody", "classbody", "classline",
  "property", "property_args", "property_arg_list", "property_arg",
  "property_body", "property_body_directives", "property_body_directive",
  "name_or_string", "optslot", "dtor", "@15", "dtor_decl", "ctor", "@16",
  "simplector", "optctorsig", "@17", "optsig", "@18", "function",
  "operatorname", "optconst", "optfinal", "optabstract", "optflags",
  "flaglist", "flag", "flagvalue", "virtualcallcode", "methodcode",
  "premethodcode", "virtualcatchercode", "arglist", "rawarglist",
  "argvalue", "varmember", "@19", "@20", "simple_varmem", "@21", "varmem",
  "member", "@22", "variable", "variable_body", "variable_body_directives",
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
     385,   386,   387,   388,   389,   390,   391,    40,    41,    44,
      61,   123,   125,    59,    33,    45,    43,    42,    47,    38,
     124,   126,    60,    62,    58,    91,    93,    37,    94
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   159,   160,   160,   162,   161,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     165,   166,   166,   167,   167,   168,   169,   170,   170,   171,
     171,   172,   173,   174,   174,   175,   175,   176,   177,   178,
     178,   179,   179,   180,   181,   182,   182,   183,   183,   184,
     185,   186,   186,   187,   187,   188,   189,   189,   190,   191,
     191,   192,   192,   192,   192,   193,   195,   194,   197,   196,
     198,   199,   199,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   201,   203,   202,   204,   204,   205,   205,   207,
     206,   208,   208,   209,   210,   211,   211,   212,   212,   213,
     215,   214,   216,   216,   217,   219,   218,   220,   220,   220,
     220,   221,   221,   222,   223,   224,   224,   224,   225,   225,
     226,   226,   226,   226,   227,   228,   228,   229,   229,   230,
     231,   232,   232,   233,   233,   234,   235,   236,   236,   237,
     237,   238,   239,   240,   240,   241,   241,   242,   243,   243,
     244,   244,   245,   245,   245,   246,   248,   247,   247,   249,
     249,   250,   250,   250,   250,   250,   250,   250,   250,   251,
     251,   252,   252,   253,   253,   253,   253,   254,   254,   255,
     255,   256,   257,   257,   258,   258,   259,   259,   260,   261,
     261,   262,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   291,   292,   293,   294,   294,   294,   295,   295,
     296,   296,   297,   297,   298,   299,   299,   300,   300,   301,
     301,   302,   303,   303,   305,   304,   306,   306,   306,   307,
     307,   308,   308,   309,   309,   310,   310,   310,   311,   311,
     312,   312,   313,   313,   314,   314,   315,   315,   315,   315,
     315,   315,   316,   316,   316,   316,   316,   316,   316,   317,
     318,   318,   319,   319,   320,   320,   321,   322,   322,   323,
     323,   323,   323,   323,   323,   323,   323,   323,   324,   324,
     324,   325,   325,   327,   328,   326,   330,   329,   331,   333,
     334,   332,   335,   335,   336,   336,   337,   338,   338,   338,
     338,   339,   339,   340,   340,   340,   341,   341,   341,   341,
     341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
     341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
     341,   341,   341,   341,   341,   341,   341,   341,   341,   341,
     341,   342,   343,   344,   344,   345,   345,   345,   346,   346,
     347,   347,   348,   348,   348,   349,   349,   350,   350,   352,
     351,   351,   353,   355,   354,   354,   356,   357,   358,   357,
     359,   360,   359,   361,   361,   361,   361,   362,   362,   362,
     362,   362,   362,   362,   362,   362,   362,   362,   362,   362,
     362,   362,   362,   362,   362,   362,   362,   362,   362,   362,
     362,   362,   362,   362,   362,   362,   363,   363,   364,   364,
     365,   365,   366,   366,   367,   367,   368,   368,   369,   369,
     369,   369,   370,   370,   371,   371,   372,   372,   373,   373,
     374,   375,   375,   375,   376,   378,   377,   379,   377,   377,
     381,   380,   380,   382,   382,   384,   383,   383,   385,   386,
     386,   387,   387,   388,   388,   388,   388,   388,   389,   389,
     390,   391,   391,   392,   392,   392,   393,   393,   393,   393,
     393,   393,   393,   393,   393,   393,   393,   393,   393,   393,
     393,   393,   393,   393,   393,   393,   393,   393,   393,   393,
     393,   393,   393,   393,   393,   393,   393,   394,   394,   395,
     395,   396,   396,   396
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
       3,     1,     3,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     1,     3,     3,     3,     0,     1,     3,     1,     3,
       3,     3,     0,     1,     3,     1,     3,     1,     3,     3,
       3,     2,     1,     2,     0,     9,     0,     1,     1,     0,
       1,     0,     1,     1,     2,     1,     1,     4,     0,     1,
       0,     2,     0,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     3,
       0,     3,     2,     1,     1,     3,     1,     1,     1,     1,
       4,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       3,     6,    12,     0,     0,     8,     0,     3,     4,     0,
       0,     8,     0,     2,     1,     3,     2,     0,     1,     1,
       1,     0,     3,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     3,     3,     3,
       2,     3,     3,     1,     3,     3,     3,     3,     0,     4,
       1,     2,     1,     1,     1,     1,     1,     0,     1,     0,
       3,     1,    11,     0,     3,     1,    11,     0,     0,     6,
       0,     0,     7,    17,     7,    17,    16,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     1,     2,     2,
       1,     2,     2,     2,     1,     2,     0,     1,     0,     1,
       0,     2,     0,     3,     1,     3,     1,     3,     1,     5,
       1,     1,     0,     2,     0,     2,     0,     2,     0,     2,
       1,     0,     1,     3,     2,     0,     3,     0,     3,     1,
       0,     3,     1,     1,     1,     0,     3,     1,     5,     0,
       3,     1,     2,     1,     1,     2,     2,     2,     4,     3,
       3,     0,     1,     0,     3,     2,     1,     4,     2,     2,
       1,     1,     2,     1,     1,     2,     2,     3,     1,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     0,
       4,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     4,     2,     0,     1,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   143,   316,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   530,   520,   523,   524,   528,
     529,   533,   534,   535,   536,   537,   538,   539,   540,   541,
     542,   543,   276,     0,   521,     0,     0,   544,   545,     0,
       0,   130,   119,     0,   145,     0,     0,     0,   546,     0,
       0,     0,     5,    37,    10,    11,    12,     6,    35,     7,
      44,    24,    36,    40,    28,    18,    31,    38,    39,    23,
      13,    14,    19,     8,    25,    21,    20,     9,    15,    27,
      49,    26,    29,    22,    30,    33,    34,    16,    32,    17,
      46,   516,   313,   314,    45,    41,    43,   336,    42,    47,
      48,     0,   513,     0,    51,     0,    50,    57,     0,    56,
      63,     0,    62,    69,     0,    68,    75,     0,     0,   272,
     246,     0,   247,   265,     0,     0,   241,   242,   243,   244,
     245,   239,   224,     0,   462,   135,   212,     0,   211,   219,
       0,   218,   225,   226,   227,   207,   208,     0,   199,   186,
       0,   178,   173,   339,   333,   526,   277,   278,   279,   531,
     519,   522,   525,   532,   312,   513,     0,   113,     0,     0,
     125,     0,   124,   146,     0,   144,     0,    86,     0,     0,
     154,   155,     0,   160,   161,     0,   166,   167,     0,     0,
       0,     0,   462,     0,   511,     0,     0,     0,    81,     0,
       0,    53,     0,     0,    59,     0,     0,    65,     0,     0,
      71,     0,     0,    77,    74,   273,   271,     0,     0,     0,
     267,   264,   518,     0,    96,   279,     0,     0,     0,   214,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   189,     0,   185,   187,     0,     0,   175,     0,
     172,   342,   342,   280,   462,   527,   511,   462,     0,   115,
       0,     0,     0,     0,   127,     0,     0,     0,     0,     0,
     148,   481,     0,   462,   547,     0,     0,     0,   157,     0,
       0,   163,     0,     0,   169,     0,   315,   462,   337,   481,
     499,     0,     0,     0,   428,   427,   429,   430,   432,   433,
     447,   450,   454,     0,   431,   434,     0,   515,   512,   509,
       0,     0,    80,     0,     0,    52,     0,     0,    58,     0,
       0,    64,     0,     0,    70,     0,     0,    76,     0,     0,
       0,   266,     0,   466,     0,   464,     0,   137,     0,   141,
       0,     0,     0,     0,   213,     0,     0,   220,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   188,     0,     0,
     255,   203,   204,     0,   201,   205,   206,     0,   174,     0,
     182,   183,     0,   180,   184,   347,   462,   462,   274,   508,
       0,     0,     0,     0,   134,     0,   132,   123,     0,   121,
       0,   126,     0,     0,     0,     0,     0,   147,     0,     0,
     480,   482,   279,   292,     0,     0,     0,   338,     0,   156,
       0,     0,   162,     0,     0,   168,     0,   517,    98,     0,
       0,     0,   448,     0,   452,   453,   438,   437,   439,   440,
     442,   443,   451,   435,   455,   436,   449,   441,   444,   481,
     514,   405,   406,    83,    84,    82,    55,    54,    61,    60,
      67,    66,    73,    72,    79,    78,   269,   270,   268,     0,
       0,   463,     0,    97,   138,     0,   136,   279,   216,   317,
     318,   217,   215,   223,   222,   191,   192,   193,   194,   195,
     196,   197,   198,   190,     0,   249,   256,     0,     0,     0,
     202,   177,   176,     0,   181,   348,   349,   350,   343,   344,
       0,   340,   334,     0,   262,     0,   117,     0,   336,   114,
     131,   133,   120,   122,   129,   128,   151,   152,   153,   150,
     149,   456,     0,   462,   310,   484,    87,     0,    85,   548,
     159,   158,   165,   164,   171,   170,     0,   456,     0,     0,
       0,   503,   504,     0,   501,   498,     0,   445,   446,     0,
     207,   470,   471,   468,   467,   465,     0,     0,     0,     0,
       0,     0,   101,   111,   103,   104,   105,   109,   106,   110,
     139,     0,   142,     0,     0,   251,     0,     0,     0,   258,
     254,   200,   179,   347,   346,   351,   351,   281,   263,   331,
       0,   116,   118,   457,   458,   483,   510,     0,   293,   294,
     302,     0,     0,    89,    93,    91,    92,    94,    99,   458,
     505,   506,   507,   500,   502,     0,   456,   209,   240,   236,
     107,   108,     0,     0,   102,   140,     0,   250,     0,     0,
       0,   257,     0,   345,   353,     0,     0,   290,   285,   286,
       0,   282,   283,     0,   459,   549,     0,   296,   297,   298,
     299,   300,   301,   310,   303,   305,   306,   307,   308,   304,
       0,    95,     0,    90,   549,   424,   458,   210,     0,     0,
     100,   253,   252,   260,   261,   259,     0,     0,   316,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   407,   407,   407,     0,   485,   487,   495,   490,   413,
       0,     0,   362,   358,   356,   357,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   367,   380,
     366,   364,   363,   359,   361,   360,     0,   354,   365,   382,
     411,   381,   415,   497,   383,   489,   492,   493,   494,   341,
     335,   310,   462,     0,   284,     0,     0,   460,   311,   295,
     326,   323,   322,   327,   325,     0,   319,   324,   309,     0,
      88,   460,   549,   209,   481,   248,   238,   481,   228,   229,
     230,   231,   232,   233,   234,   235,   237,   386,   385,   384,
     408,     0,     0,     0,   390,     0,     0,     0,     0,     0,
       0,     0,   398,     0,   352,   355,   291,   288,   275,   462,
     551,     0,   462,   321,   310,   462,   460,   469,     0,     0,
     387,   388,   389,   495,   486,   488,   410,   496,     0,   491,
       0,   414,     0,     0,     0,     0,   393,     0,   391,     0,
     289,   287,     0,   552,     0,   461,   420,   329,     0,   420,
     462,   456,   549,     0,     0,     0,     0,   392,     0,   402,
     403,   404,     0,   400,   549,   262,   550,     0,   421,     0,
     320,   310,     0,   420,   549,   462,   395,   396,   397,   394,
       0,   401,   460,   332,   553,     0,   476,   330,   262,     0,
     462,   417,   399,   462,     0,     0,   474,   476,   476,   420,
     418,     0,     0,   481,   477,     0,   478,   474,   474,     0,
       0,   262,   476,     0,   475,     0,   472,   478,   478,   262,
     481,   476,   474,     0,   479,     0,   426,   472,   472,   476,
       0,   474,   478,   422,   473,   423,   425,   474,     0,   416,
     412,   112,   419
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,    72,    73,    74,   126,   220,   221,
      75,   129,   223,   224,    76,   132,   226,   227,    77,   135,
     229,   230,    78,   138,   232,   233,    79,   217,   218,    80,
     293,   548,   622,   623,   624,    81,   356,    82,   556,   483,
     581,   582,   583,    83,   279,   403,   527,    84,   189,   408,
     409,    85,   192,   283,   284,    86,   188,   405,   406,    87,
     245,   359,   360,    88,    89,   195,   289,   290,    90,   200,
     297,   298,    91,   203,   300,   301,    92,   206,   303,   304,
      93,   171,   267,   268,   270,   392,   393,    94,   168,   265,
     261,   262,   264,   383,   384,   169,   688,    95,   158,   248,
     249,    96,   161,   251,   252,    97,    98,    99,   100,   727,
     728,   729,   730,   731,   732,   733,   734,   587,   736,   737,
     101,   588,   102,   103,   104,   105,   106,   107,   108,   739,
     385,   505,   594,   595,   608,   508,   598,   599,   609,   109,
     145,   239,   240,   140,   141,   110,   523,   178,   274,   660,
     661,   662,   851,   762,   545,   618,   673,   680,   619,   620,
     111,   112,   113,   491,   778,   858,   114,   115,   272,   606,
     116,   211,   528,   118,   271,   605,   396,   518,   519,   520,
     655,   746,   747,   748,   812,   845,   846,   848,   872,   873,
     463,   801,   749,   807,   750,   751,   810,   752,   911,   920,
     879,   895,   753,   326,   614,   665,   822,   244,   354,   355,
     574,   936,   916,   906,   926,   419,   420,   421,   754,   805,
     806,   755,   809,   756,   757,   808,   758,   441,   563,   564,
     121,   423,   329,   214,   122,   295,   767,   854
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -771
static const yytype_int16 yypact[] =
{
    -771,   171,  -771,  1099,  -771,  -771,    43,    62,    71,    72,
      41,    46,   161,   161,    48,   161,   161,   161,   161,   161,
     161,   161,  1263,    63,  -771,  -771,    14,    50,   161,   161,
     161,    52,    54,   125,   125,  -771,  -771,  -771,   160,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,   310,   165,   288,   225,  1263,  -771,  -771,  1210,
     232,  -771,  -771,    57,    74,  1210,   125,   124,  -771,    61,
      65,    33,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,   181,   251,  -771,  -771,  -771,  -771,   316,  -771,  -771,
    -771,    60,  -771,   108,  -771,   230,  -771,  -771,   242,  -771,
    -771,   247,  -771,  -771,   252,  -771,  -771,   260,   161,  -771,
    -771,    92,  -771,  -771,    67,   161,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,   125,   231,  -771,  -771,   258,  -771,  -771,
     262,  -771,  -771,  -771,  -771,  -771,  -771,   157,   274,  -771,
     275,   276,  -771,  -771,    26,  -771,  -771,  -771,   343,  -771,
    -771,  -771,   355,  -771,   251,  -771,    70,  -771,   289,   312,
    -771,   345,  -771,  -771,   153,  -771,   336,   346,  1210,   371,
    -771,  -771,   383,  -771,  -771,   384,  -771,  -771,  1210,   225,
    1263,   452,    88,   495,   118,   272,   372,   250,  -771,   373,
     253,  -771,   374,   255,  -771,   375,   263,  -771,   376,   265,
    -771,   377,   267,  -771,  -771,  -771,  -771,   378,   379,   269,
    -771,  -771,  -771,   490,  -771,     2,   381,   382,   271,  -771,
     386,   283,  -771,   387,   388,   389,   390,   391,   392,   393,
     395,   290,  -771,   210,  -771,  -771,   396,   294,  -771,   105,
    -771,   370,   370,  -771,   231,  -771,   118,   231,   397,   398,
     493,   507,   400,   324,  -771,   402,   403,   410,   411,   326,
    -771,  1210,   125,   231,  -771,   -27,   412,   328,  -771,   413,
     331,  -771,   414,   333,  -771,     5,  -771,   231,  -771,  1210,
     415,   417,   116,   418,   419,   420,   421,   427,   428,   430,
    -771,    -3,   101,   401,   431,   432,   436,   449,  -771,  -771,
     334,   481,  -771,   108,   543,  -771,   230,   544,  -771,   242,
     563,  -771,   247,   569,  -771,   252,   571,  -771,   260,   574,
     512,  -771,    67,   466,    87,  -771,   469,   462,   584,   527,
     480,   475,   590,   293,  -771,   258,   593,  -771,   262,   594,
     595,   447,   293,   293,   293,   293,   598,  -771,   157,   492,
      76,  -771,  -771,    37,  -771,  -771,  -771,   447,  -771,   275,
    -771,  -771,    18,  -771,  -771,   257,   231,   231,  -771,  -771,
     488,   603,   741,   506,  -771,    34,  -771,  -771,    40,  -771,
     334,  -771,   345,   606,   619,   622,   623,  -771,   153,   518,
     519,  -771,   343,   521,   524,   522,  1210,  -771,   447,  -771,
     371,   447,  -771,   383,   125,  -771,   384,  -771,  -771,   526,
     256,   516,  -771,  1210,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,   525,  -771,   528,  -771,  -771,  -771,  1210,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,    84,
     490,  -771,   330,  -771,  -771,    20,  -771,   343,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,   540,  -771,  -771,    25,   161,   532,
    -771,  -771,  -771,   534,  -771,  -771,  -771,  -771,   530,  -771,
     125,  -771,  -771,   529,   655,   533,  -771,   644,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,   589,  1210,   231,   542,  -771,  -771,    82,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,   469,   589,   161,   161,
     161,  -771,  -771,    39,  -771,  -771,   561,  -771,  -771,   566,
     551,  -771,  -771,  -771,  -771,  -771,   161,   161,   161,   161,
    1210,    51,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,   650,  -771,   541,   341,  -771,   567,   568,   347,  -771,
    -771,  -771,  -771,   257,  -771,   565,   565,   295,  -771,  -771,
     545,  -771,  -771,  -771,   616,  -771,  -771,   125,   202,  -771,
     190,   161,    32,  -771,  -771,  -771,  -771,  -771,  -771,   616,
    -771,  -771,  -771,  -771,  -771,   591,   589,   639,  -771,  -771,
    -771,  -771,   679,   599,  -771,  -771,   705,  -771,   540,   711,
     712,  -771,    25,  -771,   988,   602,   604,   608,  -771,  -771,
     610,   295,  -771,  1210,  -771,   640,   612,  -771,  -771,  -771,
    -771,  -771,  -771,   542,  -771,  -771,  -771,  -771,  -771,  -771,
     228,  -771,   614,  -771,   640,  -771,   616,  -771,   613,   618,
    -771,  -771,  -771,  -771,  -771,  -771,   161,   161,   624,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   689,   689,   689,   605,  -771,  -771,   609,  -771,  -771,
     626,   734,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,   869,  -771,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,   542,   231,   625,  -771,   350,   628,   627,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,   630,    80,  -771,  -771,   629,
    -771,   627,   640,   639,  1210,  -771,  -771,  1210,  -771,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,   620,   621,   631,  -771,   369,   369,   632,   511,  1152,
     743,   134,   635,   641,  -771,  -771,  -771,   638,  -771,   231,
     125,   686,   231,  -771,   224,   231,   627,  -771,   642,   643,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,    79,  -771,
     624,  -771,   647,   648,   649,   352,  -771,   105,  -771,   646,
    -771,  -771,   651,  -771,   354,  -771,   636,   202,   360,   636,
     231,   589,   640,   645,   760,   334,   762,  -771,   134,  -771,
    -771,  -771,    30,  -771,   640,   655,  -771,   125,  -771,   653,
    -771,   542,   657,   636,   640,   231,  -771,  -771,  -771,  -771,
     658,  -771,   627,  -771,  -771,  1210,   748,   202,   655,   659,
     231,   669,  -771,   231,   666,   161,   761,   748,   748,   636,
    -771,   661,   688,  1210,  -771,   161,   796,   761,   761,   691,
     698,   655,   748,   700,  -771,   161,   797,   796,   796,   655,
    1210,   748,   761,   683,  -771,   161,  -771,   797,   797,   748,
     703,   761,   796,  -771,  -771,  -771,  -771,   761,   687,  -771,
    -771,  -771,  -771
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -771,  -771,   841,  -771,  -771,  -358,  -771,  -771,  -771,   508,
    -771,  -771,  -771,   513,  -771,  -771,  -771,   503,  -771,  -771,
    -771,   502,  -771,  -771,  -771,   509,  -771,  -771,   517,  -603,
    -771,  -771,  -771,   233,  -771,  -771,  -771,  -771,  -771,   300,
    -771,   277,  -771,  -585,  -771,  -771,  -771,  -771,  -771,  -771,
     451,  -771,  -771,  -771,   448,  -771,  -771,  -771,   456,  -236,
    -771,  -771,  -771,  -235,  -771,  -771,  -771,   444,  -771,  -771,
    -771,   433,  -771,  -771,  -771,   435,  -771,  -771,  -771,   429,
    -771,  -771,  -771,   477,  -771,  -771,   472,  -771,  -771,  -771,
    -771,   491,  -771,  -771,   487,     3,    89,  -771,  -771,  -771,
     510,  -771,  -771,  -771,   505,  -771,  -771,  -771,  -459,  -771,
    -771,  -771,  -771,  -771,  -771,  -771,  -771,  -536,  -771,  -771,
    -771,  -531,  -771,  -771,  -771,  -771,  -771,  -771,  -771,  -771,
    -771,  -771,  -771,   223,  -238,  -771,  -771,   222,  -655,  -771,
    -771,  -771,   531,   -13,  -771,  -453,  -771,  -771,  -219,  -771,
    -771,   215,  -771,  -771,  -771,  -658,  -771,  -771,  -611,  -771,
     -23,   822,   670,  -336,  -771,  -771,  -530,  -527,  -771,  -771,
    -525,  -771,   875,  -197,  -771,  -771,   615,  -771,   279,  -771,
     280,  -771,   139,  -771,  -771,  -771,    22,  -771,  -771,    16,
    -390,  -212,  -771,  -771,    93,  -771,  -771,    81,  -771,  -771,
    -770,  -771,    -2,  -771,  -535,  -462,  -724,  -200,  -771,   416,
    -771,  -435,  -668,  -680,  -694,  -300,  -771,   353,  -771,  -771,
    -771,  -301,  -771,    85,  -771,  -771,    21,  -771,  -771,   338,
     -46,  -771,   637,   729,    -4,  -187,  -644,  -771
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -519
static const yytype_int16 yytable[] =
{
     142,   119,   146,   147,   148,   149,   150,   151,   152,   439,
     173,   174,   310,   186,   308,   162,   163,   164,   154,   196,
     534,   305,   629,   586,   120,   386,   361,   381,   382,   589,
     380,   394,   357,   390,   391,   172,   498,   499,   500,   501,
     781,   379,   380,   197,   526,   156,    23,    24,   207,   380,
     590,   722,   185,   558,   559,   560,  -518,   825,    23,    24,
      23,    24,   769,    25,   404,    23,    24,    23,    24,   723,
     407,   133,   201,   204,   398,   576,   136,   400,   143,    23,
      24,   159,   165,   166,   165,   166,    30,   190,   627,   882,
     212,   165,   166,   425,   124,   165,   166,   577,   578,   579,
     277,   686,   860,   127,   130,    30,   193,   438,   506,   863,
      23,    24,   426,   899,   570,   166,   571,   380,   735,   235,
      55,   236,   586,   738,   742,   234,   427,   743,   589,   744,
     242,  -518,   241,    23,    24,    52,    30,   452,   826,   919,
     621,   580,   596,   722,   426,   386,   358,   381,   382,   453,
     816,   157,   294,   597,   394,    25,   390,   391,   437,   569,
     513,   723,   294,   627,   591,   213,   857,   684,   903,   612,
     205,     4,   890,  -518,   682,  -518,   530,   572,   134,   509,
     123,   633,   532,   137,   213,   144,   237,   160,   139,   167,
     621,   170,   238,   643,   191,   726,   521,   522,   199,   125,
     155,   741,   202,   543,   561,   562,   307,   278,   128,   131,
     735,   194,    55,   507,   379,   738,   742,  -516,   885,   743,
     893,   744,   380,   897,   782,   309,   480,   917,   918,   175,
     892,   215,   208,   937,   938,   481,   243,   179,    23,    24,
     900,   454,   932,   907,   216,   422,   584,   585,   950,   927,
     928,   941,   842,   443,   455,    25,   444,   843,    25,   947,
     770,   844,   187,   422,   942,   327,   931,   328,   592,   424,
     558,   559,   560,   949,   939,   285,   198,   253,   254,   951,
     255,   286,   287,   288,    23,    24,   153,   726,   256,   257,
     258,   259,   260,   741,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,   625,   626,    53,    54,    55,   515,   516,   517,    57,
      58,   771,   772,    23,    24,   657,   884,   561,   562,   773,
     489,   490,   774,   208,   674,   675,   676,   677,   209,   678,
      68,   679,   210,   616,   777,   584,   585,   667,   668,   669,
     670,   671,   672,   219,   576,   180,   181,   182,    23,    24,
     183,   617,  -328,  -328,   461,   222,   462,   176,   177,   775,
     225,   658,   659,   273,   497,   228,   577,   578,   579,   243,
     549,   246,   247,   231,    30,   250,   625,   626,   332,   333,
     511,   335,   336,   338,   339,   489,   490,   566,   266,    25,
     119,   341,   342,   344,   345,   347,   348,   351,   352,   364,
     365,   554,   330,   422,    52,   263,   740,   269,   724,   725,
     580,   367,   368,   120,   275,   658,   659,   153,   377,   378,
     280,   550,   388,   389,   552,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   833,   281,    53,    54,    55,   745,    56,   718,
      57,    58,   411,   412,   417,   418,   429,   430,   282,   432,
     433,   435,   436,   291,    65,   887,   765,   165,   166,   647,
     648,    68,   573,   292,   828,   651,   652,   829,   819,   426,
     867,   868,   876,   877,   296,   600,   422,   604,   880,   881,
     802,   803,   945,   946,   834,   835,   299,   302,   740,    33,
     724,   725,   331,   334,   337,   340,   343,   346,   349,   350,
     353,   362,   363,   404,   395,   119,   366,   369,   370,   371,
     372,   373,   374,   375,   642,   376,   387,   407,   460,   402,
     410,    25,   413,   414,   401,   630,   631,   632,   120,   745,
     415,   416,   428,   431,   434,   442,   440,   456,   445,   446,
     447,   448,   817,   638,   639,   640,   641,   449,   450,   153,
     451,   457,   458,   459,   464,   466,   468,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,   666,   470,    53,    54,    55,   472,
      56,   474,    57,    58,   476,   477,   479,  -280,   681,   871,
     482,   869,   870,   923,   484,   485,    65,   294,   486,   852,
     487,   488,   856,    68,   493,   859,   495,   496,   502,   504,
     940,   524,   311,   525,   871,   312,   869,   870,   536,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   529,
     323,   537,   324,   325,   538,   539,   541,   776,   542,   565,
     883,   544,   546,   547,   557,   567,   593,   380,   568,   603,
     607,   610,    23,    24,    25,   601,   779,   602,   613,   617,
     645,   646,   663,   785,   786,   901,   788,   789,   790,   791,
     792,   793,   794,   795,   796,   797,   798,   799,    30,   635,
     909,    33,    34,   912,   636,   637,   654,   649,   650,   689,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,   664,    52,    53,
      54,    55,   687,    56,   685,    57,    58,   691,   422,    59,
      60,   422,   690,   693,   694,   759,   766,   760,   761,    65,
     768,    66,   763,   800,    67,   784,    68,   780,   783,   804,
    -409,   787,   838,   811,   813,   820,   824,   821,   818,    23,
      24,    25,   823,   840,   830,   831,   847,   850,   849,   855,
     861,   862,   309,   721,   874,   832,   611,   864,   865,   866,
     886,   878,   888,   905,   875,    30,   896,   853,    33,    34,
     898,   902,   908,   913,   921,   915,   837,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,   910,    52,    53,    54,    55,   925,
      56,   922,    57,    58,   929,   930,    59,    60,   933,   943,
     935,   948,     5,   952,   467,   471,    65,   473,    66,   904,
     465,    67,   469,    68,   894,   683,   628,   475,   644,   533,
     535,   531,   540,   551,   514,   555,   512,   422,   553,   503,
     510,   692,   827,   494,   695,   492,   764,   184,   117,   306,
     696,   380,   653,   478,   422,   815,   656,   397,   891,   697,
     889,   841,   914,   576,   839,   615,   575,    23,    24,   698,
     836,   634,   924,   699,   700,   701,   702,   703,   704,   705,
     706,   707,   934,   399,   276,   577,   708,   709,   710,     0,
       0,     0,   944,    30,     0,     0,    33,    34,   711,   712,
     713,   714,   715,     0,   716,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   717,    52,    53,    54,    55,     0,    56,   718,
      57,    58,     0,     0,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,    65,     0,    66,     0,   719,    67,
       0,    68,     0,     0,   720,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   696,
     380,     0,     0,     0,     0,     0,     0,     0,   697,     0,
       0,   814,   576,     0,     0,     0,    23,    24,   698,     0,
     721,     0,   699,   700,   701,   702,   703,   704,   705,   706,
     707,     0,     0,     0,   577,   708,   709,   710,     0,     0,
       0,     0,    30,     0,     0,    33,    34,   711,   712,   713,
     714,   715,     0,   716,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,   717,    52,    53,    54,    55,     0,    56,   718,    57,
      58,     0,     0,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,    65,     0,    66,     0,   719,    67,     0,
      68,     0,     6,   720,     7,     8,     9,    10,    11,    12,
      13,     0,    14,     0,     0,     0,    15,    16,    17,     0,
      18,    19,    20,     0,    21,    22,     0,    23,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   721,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,     0,
       0,     0,     0,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    25,    52,    53,    54,    55,     0,    56,     0,
      57,    58,     0,     0,    59,    60,    61,    62,    63,    64,
       0,     0,     0,     0,    65,     0,    66,     0,     0,    67,
     153,    68,    69,    70,     0,    71,     0,     0,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,   833,     0,    53,    54,    55,
      25,    56,     0,    57,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    65,     0,     0,
       0,     0,     0,     0,    68,     0,     0,     0,   153,     0,
       0,     0,     0,     0,     0,     0,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    25,     0,    53,    54,    55,     0,    56,
       0,    57,    58,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   153,    68,     0,     0,     0,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,     0,     0,    53,    54,
      55,     0,     0,     0,    57,    58,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    68
};

static const yytype_int16 yycheck[] =
{
      13,     3,    15,    16,    17,    18,    19,    20,    21,   309,
      33,    34,   212,    59,   211,    28,    29,    30,    22,    65,
     410,   208,   557,   482,     3,   263,   245,   263,   263,   482,
      12,   269,    30,   269,   269,    32,   372,   373,   374,   375,
     684,     4,    12,    66,   402,    31,    28,    29,    71,    12,
      30,   654,    56,    14,    15,    16,    30,   781,    28,    29,
      28,    29,   673,    30,    30,    28,    29,    28,    29,   654,
      30,    30,    69,    70,   274,    24,    30,   277,    30,    28,
      29,    31,    30,    31,    30,    31,    54,    30,   547,   859,
      30,    30,    31,   293,    32,    30,    31,    46,    47,    48,
      30,   636,   826,    32,    32,    54,    32,   307,    32,    30,
      28,    29,   139,   883,    30,    31,    32,    12,   654,    27,
      87,    29,   581,   654,   654,   138,   153,   654,   581,   654,
     153,   105,   145,    28,    29,    84,    54,   140,   782,   909,
     108,    90,   117,   746,   139,   383,   144,   383,   383,   152,
     761,   137,   198,   128,   392,    30,   392,   392,   153,   459,
     142,   746,   208,   622,   144,   105,   824,   629,   892,   527,
     137,     0,   142,   147,   142,   149,   142,    93,   137,   142,
     137,   142,   142,   137,   105,   137,   119,   137,    27,   137,
     108,   137,   125,   142,   137,   654,   396,   397,   137,   137,
     137,   654,   137,   422,   440,   440,   210,   137,   137,   137,
     746,   137,    87,   137,     4,   746,   746,   137,   862,   746,
     875,   746,    12,   881,   686,   137,   139,   907,   908,    69,
     874,   123,   152,   927,   928,   148,   148,    72,    28,    29,
     884,   140,   922,   898,   136,   291,   482,   482,   942,   917,
     918,   931,   118,   137,   153,    30,   140,   123,    30,   939,
      32,   127,    30,   309,   932,   147,   921,   149,   487,   292,
      14,    15,    16,   941,   929,   122,   152,   120,   121,   947,
     123,   128,   129,   130,    28,    29,    58,   746,   131,   132,
     133,   134,   135,   746,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,   547,   547,    85,    86,    87,    59,    60,    61,    91,
      92,    93,    94,    28,    29,    30,   861,   563,   563,   101,
     102,   103,   104,   152,   144,   145,   146,   147,    87,   149,
     112,   151,    26,   543,   680,   581,   581,   145,   146,   147,
     148,   149,   150,   123,    24,    67,    68,    69,    28,    29,
      72,   137,   138,   139,    30,   123,    32,    57,    58,   141,
     123,   607,   607,    30,   371,   123,    46,    47,    48,   148,
     426,   123,   124,   123,    54,   123,   622,   622,   138,   139,
     387,   138,   139,   138,   139,   102,   103,   443,   123,    30,
     402,   138,   139,   138,   139,   138,   139,   138,   139,   138,
     139,   434,   140,   459,    84,   141,   654,   141,   654,   654,
      90,   138,   139,   402,    69,   661,   661,    58,   138,   139,
     141,   428,   138,   139,   431,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,   141,    85,    86,    87,   654,    89,    90,
      91,    92,   138,   139,   138,   139,   138,   139,   123,   138,
     139,   138,   139,   137,   105,   865,   663,    30,    31,   138,
     139,   112,   479,   137,   784,   138,   139,   787,   138,   139,
     138,   139,   138,   139,   123,   508,   542,   520,   138,   139,
     712,   713,   937,   938,   805,   806,   123,   123,   746,    57,
     746,   746,   140,   140,   140,   140,   140,   140,   140,   140,
      30,   140,   140,    30,   154,   527,   140,   140,   140,   140,
     140,   140,   140,   140,   580,   140,   140,    30,    89,   141,
     140,    30,   140,   140,   147,   558,   559,   560,   527,   746,
     140,   140,   140,   140,   140,   138,   141,   156,   140,   140,
     140,   140,   762,   576,   577,   578,   579,   140,   140,    58,
     140,   140,   140,   137,    93,    32,    32,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,   617,    32,    85,    86,    87,    30,
      89,    30,    91,    92,    30,    93,   140,   145,   621,   847,
     141,   847,   847,   913,    30,    88,   105,   663,   138,   819,
     145,    31,   822,   112,    31,   825,    32,    32,    30,   137,
     930,   143,   137,    30,   872,   140,   872,   872,    32,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   143,
     155,    32,   157,   158,    32,    32,   138,   680,   139,   143,
     860,   140,   138,   141,   138,   140,   126,    12,   140,   139,
     141,   138,    28,    29,    30,   143,   680,   143,    89,   137,
      30,   140,   137,   696,   697,   885,   699,   700,   701,   702,
     703,   704,   705,   706,   707,   708,   709,   710,    54,   138,
     900,    57,    58,   903,   138,   154,   141,   140,   140,    30,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   111,    84,    85,
      86,    87,    93,    89,   143,    91,    92,    32,   784,    95,
      96,   787,   143,    32,    32,   143,   106,   143,   140,   105,
     138,   107,   142,    64,   110,   137,   112,   143,   145,   154,
     151,   137,   808,   137,    30,   137,   137,   140,   143,    28,
      29,    30,   142,    30,   154,   154,   141,   139,   137,    93,
     138,   138,   137,   151,   138,   154,   142,   140,   140,   140,
      30,   155,    30,    45,   143,    54,   143,   820,    57,    58,
     143,   143,   143,   137,   143,    44,   808,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,   155,    84,    85,    86,    87,    33,
      89,   143,    91,    92,   143,   137,    95,    96,   138,   156,
      43,   138,     1,   156,   336,   342,   105,   345,   107,   895,
     333,   110,   339,   112,   877,   622,   556,   348,   581,   408,
     412,   405,   418,   430,   392,   436,   389,   913,   433,   378,
     383,   648,   783,   368,   652,   365,   661,    55,     3,   209,
      11,    12,   603,   352,   930,   746,   606,   272,   872,    20,
     868,   810,   905,    24,   809,   542,   480,    28,    29,    30,
     807,   563,   915,    34,    35,    36,    37,    38,    39,    40,
      41,    42,   925,   276,   185,    46,    47,    48,    49,    -1,
      -1,    -1,   935,    54,    -1,    -1,    57,    58,    59,    60,
      61,    62,    63,    -1,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    -1,    89,    90,
      91,    92,    -1,    -1,    95,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   105,    -1,   107,    -1,   109,   110,
      -1,   112,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,
      -1,   142,    24,    -1,    -1,    -1,    28,    29,    30,    -1,
     151,    -1,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    59,    60,    61,
      62,    63,    -1,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    -1,    -1,    95,    96,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   105,    -1,   107,    -1,   109,   110,    -1,
     112,    -1,     3,   115,     5,     6,     7,     8,     9,    10,
      11,    -1,    13,    -1,    -1,    -1,    17,    18,    19,    -1,
      21,    22,    23,    -1,    25,    26,    -1,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    30,    84,    85,    86,    87,    -1,    89,    -1,
      91,    92,    -1,    -1,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,   105,    -1,   107,    -1,    -1,   110,
      58,   112,   113,   114,    -1,   116,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    -1,    85,    86,    87,
      30,    89,    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,    -1,
      -1,    -1,    -1,    -1,   112,    -1,    -1,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    30,    -1,    85,    86,    87,    -1,    89,
      -1,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,   112,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    -1,    -1,    85,    86,
      87,    -1,    -1,    -1,    91,    92,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   160,   161,   162,     0,   161,     3,     5,     6,     7,
       8,     9,    10,    11,    13,    17,    18,    19,    21,    22,
      23,    25,    26,    28,    29,    30,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    84,    85,    86,    87,    89,    91,    92,    95,
      96,    97,    98,    99,   100,   105,   107,   110,   112,   113,
     114,   116,   163,   164,   165,   169,   173,   177,   181,   185,
     188,   194,   196,   202,   206,   210,   214,   218,   222,   223,
     227,   231,   235,   239,   246,   256,   260,   264,   265,   266,
     267,   279,   281,   282,   283,   284,   285,   286,   287,   298,
     304,   319,   320,   321,   325,   326,   329,   331,   332,   361,
     385,   389,   393,   137,    32,   137,   166,    32,   137,   170,
      32,   137,   174,    30,   137,   178,    30,   137,   182,    27,
     302,   303,   302,    30,   137,   299,   302,   302,   302,   302,
     302,   302,   302,    58,   393,   137,    31,   137,   257,    31,
     137,   261,   302,   302,   302,    30,    31,   137,   247,   254,
     137,   240,   254,   319,   319,    69,    57,    58,   306,    72,
      67,    68,    69,    72,   320,   393,   389,    30,   215,   207,
      30,   137,   211,    32,   137,   224,   389,   319,   152,   137,
     228,   254,   137,   232,   254,   137,   236,   319,   152,    87,
      26,   330,    30,   105,   392,   123,   136,   186,   187,   123,
     167,   168,   123,   171,   172,   123,   175,   176,   123,   179,
     180,   123,   183,   184,   302,    27,    29,   119,   125,   300,
     301,   302,   319,   148,   366,   219,   123,   124,   258,   259,
     123,   262,   263,   120,   121,   123,   131,   132,   133,   134,
     135,   249,   250,   141,   251,   248,   123,   241,   242,   141,
     243,   333,   327,    30,   307,    69,   392,    30,   137,   203,
     141,   141,   123,   212,   213,   122,   128,   129,   130,   225,
     226,   137,   137,   189,   389,   394,   123,   229,   230,   123,
     233,   234,   123,   237,   238,   394,   321,   393,   332,   137,
     366,   137,   140,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   155,   157,   158,   362,   147,   149,   391,
     140,   140,   138,   139,   140,   138,   139,   140,   138,   139,
     140,   138,   139,   140,   138,   139,   140,   138,   139,   140,
     140,   138,   139,    30,   367,   368,   195,    30,   144,   220,
     221,   307,   140,   140,   138,   139,   140,   138,   139,   140,
     140,   140,   140,   140,   140,   140,   140,   138,   139,     4,
      12,   218,   222,   252,   253,   289,   293,   140,   138,   139,
     218,   222,   244,   245,   293,   154,   335,   335,   366,   391,
     366,   147,   141,   204,    30,   216,   217,    30,   208,   209,
     140,   138,   139,   140,   140,   140,   140,   138,   139,   374,
     375,   376,   389,   390,   319,   366,   139,   153,   140,   138,
     139,   140,   138,   139,   140,   138,   139,   153,   366,   374,
     141,   386,   138,   137,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   152,   140,   153,   156,   140,   140,   137,
      89,    30,    32,   349,    93,   187,    32,   168,    32,   172,
      32,   176,    30,   180,    30,   184,    30,    93,   301,   140,
     139,   148,   141,   198,    30,    88,   138,   145,    31,   102,
     103,   322,   259,    31,   263,    32,    32,   254,   322,   322,
     322,   322,    30,   250,   137,   290,    32,   137,   294,   142,
     253,   254,   242,   142,   245,    59,    60,    61,   336,   337,
     338,   366,   366,   305,   143,    30,   164,   205,   331,   143,
     142,   217,   142,   209,   349,   213,    32,    32,    32,    32,
     226,   138,   139,   307,   140,   313,   138,   141,   190,   389,
     254,   230,   254,   234,   319,   238,   197,   138,    14,    15,
      16,   218,   222,   387,   388,   143,   389,   140,   140,   374,
      30,    32,    93,   254,   369,   368,    24,    46,    47,    48,
      90,   199,   200,   201,   218,   222,   267,   276,   280,   304,
      30,   144,   307,   126,   291,   292,   117,   128,   295,   296,
     302,   143,   143,   139,   319,   334,   328,   141,   293,   297,
     138,   142,   164,    89,   363,   376,   366,   137,   314,   317,
     318,   108,   191,   192,   193,   218,   222,   267,   198,   363,
     302,   302,   302,   142,   388,   138,   138,   154,   302,   302,
     302,   302,   389,   142,   200,    30,   140,   138,   139,   140,
     140,   138,   139,   337,   141,   339,   339,    30,   218,   222,
     308,   309,   310,   137,   111,   364,   319,   145,   146,   147,
     148,   149,   150,   315,   144,   145,   146,   147,   149,   151,
     316,   302,   142,   192,   364,   143,   363,    93,   255,    30,
     143,    32,   292,    32,    32,   296,    11,    20,    30,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    47,    48,
      49,    59,    60,    61,    62,    63,    65,    83,    90,   109,
     115,   151,   188,   202,   218,   222,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   280,   288,
     293,   304,   325,   326,   329,   332,   340,   341,   342,   351,
     353,   354,   356,   361,   377,   380,   382,   383,   385,   143,
     143,   140,   312,   142,   310,   394,   106,   395,   138,   317,
      32,    93,    94,   101,   104,   141,   319,   322,   323,   393,
     143,   395,   364,   145,   137,   302,   302,   137,   302,   302,
     302,   302,   302,   302,   302,   302,   302,   302,   302,   302,
      64,   350,   350,   350,   154,   378,   379,   352,   384,   381,
     355,   137,   343,    30,   142,   341,   317,   366,   143,   138,
     137,   140,   365,   142,   137,   365,   395,   255,   374,   374,
     154,   154,   154,    83,   380,   380,   353,   361,   389,   382,
      30,   356,   118,   123,   127,   344,   345,   141,   346,   137,
     139,   311,   366,   319,   396,    93,   366,   314,   324,   366,
     365,   138,   138,    30,   140,   140,   140,   138,   139,   218,
     222,   293,   347,   348,   138,   143,   138,   139,   155,   359,
     138,   139,   359,   366,   363,   395,    30,   349,    30,   345,
     142,   348,   395,   297,   319,   360,   143,   314,   143,   359,
     395,   366,   143,   365,   389,    45,   372,   297,   143,   366,
     155,   357,   366,   137,   302,    44,   371,   372,   372,   359,
     358,   143,   143,   374,   302,    33,   373,   371,   371,   143,
     137,   297,   372,   138,   302,    43,   370,   373,   373,   297,
     374,   372,   371,   156,   302,   370,   370,   372,   138,   371,
     373,   371,   156
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
#line 550 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 614 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 627 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                currentModule->defdocstringfmt = convertFormat((yyvsp[(2) - (2)].defdocstringfmt).name);
        }
    break;

  case 51:
#line 633 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defdocstringfmt).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 52:
#line 638 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt) = (yyvsp[(2) - (3)].defdocstringfmt);
        }
    break;

  case 54:
#line 644 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt) = (yyvsp[(1) - (3)].defdocstringfmt);

            switch ((yyvsp[(3) - (3)].defdocstringfmt).token)
            {
            case TK_NAME: (yyval.defdocstringfmt).name = (yyvsp[(3) - (3)].defdocstringfmt).name; break;
            }
        }
    break;

  case 55:
#line 654 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringfmt).token = TK_NAME;

            (yyval.defdocstringfmt).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 56:
#line 661 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                currentModule->defdocstringsig = convertSignature((yyvsp[(2) - (2)].defdocstringsig).name);
        }
    break;

  case 57:
#line 667 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defdocstringsig).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 58:
#line 672 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig) = (yyvsp[(2) - (3)].defdocstringsig);
        }
    break;

  case 60:
#line 678 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig) = (yyvsp[(1) - (3)].defdocstringsig);

            switch ((yyvsp[(3) - (3)].defdocstringsig).token)
            {
            case TK_NAME: (yyval.defdocstringsig).name = (yyvsp[(3) - (3)].defdocstringsig).name; break;
            }
        }
    break;

  case 61:
#line 688 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defdocstringsig).token = TK_NAME;

            (yyval.defdocstringsig).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 62:
#line 695 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                if ((currentModule->encoding = convertEncoding((yyvsp[(2) - (2)].defencoding).name)) == no_type)
                    yyerror("The %DefaultEncoding name must be one of \"ASCII\", \"Latin-1\", \"UTF-8\" or \"None\"");
            }
        }
    break;

  case 63:
#line 704 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defencoding).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 64:
#line 709 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding) = (yyvsp[(2) - (3)].defencoding);
        }
    break;

  case 66:
#line 715 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding) = (yyvsp[(1) - (3)].defencoding);

            switch ((yyvsp[(3) - (3)].defencoding).token)
            {
            case TK_NAME: (yyval.defencoding).name = (yyvsp[(3) - (3)].defencoding).name; break;
            }
        }
    break;

  case 67:
#line 725 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defencoding).token = TK_NAME;

            (yyval.defencoding).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 68:
#line 732 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 743 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.plugin).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 70:
#line 748 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin) = (yyvsp[(2) - (3)].plugin);
        }
    break;

  case 72:
#line 754 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin) = (yyvsp[(1) - (3)].plugin);

            switch ((yyvsp[(3) - (3)].plugin).token)
            {
            case TK_NAME: (yyval.plugin).name = (yyvsp[(3) - (3)].plugin).name; break;
            }
        }
    break;

  case 73:
#line 764 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.plugin).token = TK_NAME;

            (yyval.plugin).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 74:
#line 771 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 800 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.veh).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 76:
#line 805 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh) = (yyvsp[(2) - (3)].veh);
        }
    break;

  case 78:
#line 811 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh) = (yyvsp[(1) - (3)].veh);

            switch ((yyvsp[(3) - (3)].veh).token)
            {
            case TK_NAME: (yyval.veh).name = (yyvsp[(3) - (3)].veh).name; break;
            }
        }
    break;

  case 79:
#line 821 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.veh).token = TK_NAME;

            (yyval.veh).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 80:
#line 828 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            deprecated("The %API directive is deprecated and will be removed in v6");

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
#line 857 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 868 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.api).token = TK_NAME;

            (yyval.api).name = (yyvsp[(3) - (3)].text);
            (yyval.api).version = 0;
        }
    break;

  case 84:
#line 874 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.api).token = TK_VERSION;

            (yyval.api).name = NULL;
            (yyval.api).version = (yyvsp[(3) - (3)].number);
        }
    break;

  case 85:
#line 882 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 929 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exceptionbase).bibase = NULL;
            (yyval.exceptionbase).base = NULL;
        }
    break;

  case 87:
#line 933 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1040 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception) = (yyvsp[(2) - (4)].exception);
        }
    break;

  case 90:
#line 1046 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1057 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception).token = TK_IF;
        }
    break;

  case 92:
#line 1060 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.exception).token = TK_END;
        }
    break;

  case 93:
#line 1063 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1077 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1093 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 96:
#line 1098 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                static const char *annos[] = {
                    "AllowNone",
                    "API",
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
#line 1120 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                static const char *annos[] = {
                    "AllowNone",
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
#line 1186 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1206 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 106:
#line 1210 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->typecode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 107:
#line 1214 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1223 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1232 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1245 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1260 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1281 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1305 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1315 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1338 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, -1, 0, notSkipping(), (yyvsp[(1) - (1)].text),
                    platform_qualifier);
        }
    break;

  case 124:
#line 1344 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, -1, 0, notSkipping(), (yyvsp[(2) - (2)].feature).name,
                    feature_qualifier);
        }
    break;

  case 125:
#line 1350 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.feature).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 126:
#line 1355 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature) = (yyvsp[(2) - (3)].feature);
        }
    break;

  case 128:
#line 1361 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature) = (yyvsp[(1) - (3)].feature);

            switch ((yyvsp[(3) - (3)].feature).token)
            {
            case TK_NAME: (yyval.feature).name = (yyvsp[(3) - (3)].feature).name; break;
            }
        }
    break;

  case 129:
#line 1371 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.feature).token = TK_NAME;

            (yyval.feature).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 130:
#line 1378 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            currentTimelineOrder = 0;
        }
    break;

  case 131:
#line 1381 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1408 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            newQualifier(currentModule, currentModule->nrtimelines,
                    currentTimelineOrder++, TRUE, (yyvsp[(1) - (1)].text), time_qualifier);
        }
    break;

  case 135:
#line 1414 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            currentPlatforms = NULL;
        }
    break;

  case 136:
#line 1416 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1433 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(1) - (1)].text), FALSE);
        }
    break;

  case 138:
#line 1436 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(2) - (2)].text), TRUE);
        }
    break;

  case 139:
#line 1439 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(3) - (3)].text), FALSE) || (yyvsp[(1) - (3)].boolean));
        }
    break;

  case 140:
#line 1442 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(4) - (4)].text), TRUE) || (yyvsp[(1) - (4)].boolean));
        }
    break;

  case 142:
#line 1448 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = timePeriod((yyvsp[(1) - (3)].text), (yyvsp[(3) - (3)].text));
        }
    break;

  case 143:
#line 1453 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (stackPtr-- <= 0)
                yyerror("Too many %End directives");

            currentPlatforms = (stackPtr == 0 ? NULL : platformStack[stackPtr - 1]);
        }
    break;

  case 144:
#line 1461 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1477 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 146:
#line 1485 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).type = (yyvsp[(1) - (1)].text);
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 147:
#line 1491 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license) = (yyvsp[(2) - (3)].license);
        }
    break;

  case 149:
#line 1497 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1510 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_NAME;

            (yyval.license).type = (yyvsp[(3) - (3)].text);
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 151:
#line 1518 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_LICENSEE;

            (yyval.license).type = NULL;
            (yyval.license).licensee = (yyvsp[(3) - (3)].text);
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = NULL;
        }
    break;

  case 152:
#line 1526 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_SIGNATURE;

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = (yyvsp[(3) - (3)].text);
            (yyval.license).timestamp = NULL;
        }
    break;

  case 153:
#line 1534 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.license).token = TK_TIMESTAMP;

            (yyval.license).type = NULL;
            (yyval.license).licensee = NULL;
            (yyval.license).signature = NULL;
            (yyval.license).timestamp = (yyvsp[(3) - (3)].text);
        }
    break;

  case 154:
#line 1544 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1555 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defmetatype).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 156:
#line 1560 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype) = (yyvsp[(2) - (3)].defmetatype);
        }
    break;

  case 158:
#line 1566 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype) = (yyvsp[(1) - (3)].defmetatype);

            switch ((yyvsp[(3) - (3)].defmetatype).token)
            {
            case TK_NAME: (yyval.defmetatype).name = (yyvsp[(3) - (3)].defmetatype).name; break;
            }
        }
    break;

  case 159:
#line 1576 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defmetatype).token = TK_NAME;

            (yyval.defmetatype).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 160:
#line 1583 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1594 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.defsupertype).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 162:
#line 1599 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype) = (yyvsp[(2) - (3)].defsupertype);
        }
    break;

  case 164:
#line 1605 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype) = (yyvsp[(1) - (3)].defsupertype);

            switch ((yyvsp[(3) - (3)].defsupertype).token)
            {
            case TK_NAME: (yyval.defsupertype).name = (yyvsp[(3) - (3)].defsupertype).name; break;
            }
        }
    break;

  case 165:
#line 1615 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.defsupertype).token = TK_NAME;

            (yyval.defsupertype).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 166:
#line 1622 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1634 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.hiddenns).name = (yyvsp[(1) - (1)].scpvalp);
        }
    break;

  case 168:
#line 1639 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns) = (yyvsp[(2) - (3)].hiddenns);
        }
    break;

  case 170:
#line 1645 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns) = (yyvsp[(1) - (3)].hiddenns);

            switch ((yyvsp[(3) - (3)].hiddenns).token)
            {
            case TK_NAME: (yyval.hiddenns).name = (yyvsp[(3) - (3)].hiddenns).name; break;
            }
        }
    break;

  case 171:
#line 1655 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.hiddenns).token = TK_NAME;

            (yyval.hiddenns).name = (yyvsp[(3) - (3)].scpvalp);
        }
    break;

  case 172:
#line 1662 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1680 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.compmodule).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 174:
#line 1685 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(2) - (3)].compmodule);
        }
    break;

  case 176:
#line 1691 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(1) - (3)].compmodule);

            switch ((yyvsp[(3) - (3)].compmodule).token)
            {
            case TK_NAME: (yyval.compmodule).name = (yyvsp[(3) - (3)].compmodule).name; break;
            }
        }
    break;

  case 177:
#line 1701 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_NAME;

            (yyval.compmodule).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 178:
#line 1708 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = 0;
            (yyval.compmodule).docstring = NULL;
        }
    break;

  case 179:
#line 1712 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(2) - (4)].compmodule);
        }
    break;

  case 181:
#line 1718 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule) = (yyvsp[(1) - (2)].compmodule);

            switch ((yyvsp[(2) - (2)].compmodule).token)
            {
            case TK_DOCSTRING: (yyval.compmodule).docstring = (yyvsp[(2) - (2)].compmodule).docstring; break;
            }
        }
    break;

  case 182:
#line 1728 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_IF;
        }
    break;

  case 183:
#line 1731 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.compmodule).token = TK_END;
        }
    break;

  case 184:
#line 1734 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1748 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1761 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {resetLexerState();}
    break;

  case 187:
#line 1761 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1771 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(2) - (3)].module);
        }
    break;

  case 190:
#line 1777 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1794 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1806 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1824 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1836 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1848 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1860 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1872 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1884 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1898 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = 0;
            (yyval.module).docstring = NULL;
        }
    break;

  case 200:
#line 1902 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(2) - (4)].module);
        }
    break;

  case 202:
#line 1908 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module) = (yyvsp[(1) - (2)].module);

            switch ((yyvsp[(2) - (2)].module).token)
            {
            case TK_DOCSTRING: (yyval.module).docstring = (yyvsp[(2) - (2)].module).docstring; break;
            }
        }
    break;

  case 203:
#line 1918 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_IF;
        }
    break;

  case 204:
#line 1921 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_END;
        }
    break;

  case 205:
#line 1924 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.module).token = TK_AUTOPYNAME;
        }
    break;

  case 206:
#line 1927 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1942 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1958 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = -1;
        }
    break;

  case 211:
#line 1964 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (2)].include).name == NULL)
                yyerror("%Include must have a 'name' argument");

            if (notSkipping())
                parseFile(NULL, (yyvsp[(2) - (2)].include).name, NULL, (yyvsp[(2) - (2)].include).optional);
        }
    break;

  case 212:
#line 1973 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.include).name = (yyvsp[(1) - (1)].text);
            (yyval.include).optional = FALSE;
        }
    break;

  case 213:
#line 1979 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include) = (yyvsp[(2) - (3)].include);
        }
    break;

  case 215:
#line 1985 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 1996 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include).token = TK_NAME;

            (yyval.include).name = (yyvsp[(3) - (3)].text);
            (yyval.include).optional = FALSE;
        }
    break;

  case 217:
#line 2002 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.include).token = TK_OPTIONAL;

            (yyval.include).name = NULL;
            (yyval.include).optional = (yyvsp[(3) - (3)].boolean);
        }
    break;

  case 218:
#line 2010 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                newImport((yyvsp[(2) - (2)].import).name);
        }
    break;

  case 219:
#line 2016 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.import).name = (yyvsp[(1) - (1)].text);
        }
    break;

  case 220:
#line 2021 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import) = (yyvsp[(2) - (3)].import);
        }
    break;

  case 222:
#line 2027 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import) = (yyvsp[(1) - (3)].import);

            switch ((yyvsp[(3) - (3)].import).token)
            {
            case TK_NAME: (yyval.import).name = (yyvsp[(3) - (3)].import).name; break;
            }
        }
    break;

  case 223:
#line 2037 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.import).token = TK_NAME;

            (yyval.import).name = (yyvsp[(3) - (3)].text);
        }
    break;

  case 224:
#line 2044 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->copying, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 225:
#line 2050 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentSpec->exphdrcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 226:
#line 2056 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->hdrcode, (yyvsp[(2) - (2)].codeb));
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
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 229:
#line 2072 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 230:
#line 2077 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 231:
#line 2082 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 232:
#line 2087 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 233:
#line 2092 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 234:
#line 2097 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 235:
#line 2102 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 236:
#line 2107 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 237:
#line 2112 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 238:
#line 2117 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 239:
#line 2122 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->cppcode, (yyvsp[(2) - (2)].codeb));
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
            if (notSkipping())
                appendCodeBlock(&currentModule->preinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 242:
#line 2139 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->initcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 243:
#line 2145 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->postinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 244:
#line 2151 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 245:
#line 2157 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitpostinccode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 246:
#line 2163 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping() && !inMainModule())
                appendCodeBlock(&currentSpec->exptypehintcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 247:
#line 2169 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->typehintcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 248:
#line 2175 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 249:
#line 2180 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                addAutoPyName(currentModule, (yyvsp[(2) - (2)].autopyname).remove_leading);
        }
    break;

  case 250:
#line 2186 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname) = (yyvsp[(2) - (3)].autopyname);
        }
    break;

  case 252:
#line 2192 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname) = (yyvsp[(1) - (3)].autopyname);

            switch ((yyvsp[(3) - (3)].autopyname).token)
            {
            case TK_REMOVELEADING: (yyval.autopyname).remove_leading = (yyvsp[(3) - (3)].autopyname).remove_leading; break;
            }
        }
    break;

  case 253:
#line 2202 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.autopyname).token = TK_REMOVELEADING;

            (yyval.autopyname).remove_leading = (yyvsp[(3) - (3)].text);
        }
    break;

  case 254:
#line 2209 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 255:
#line 2293 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).format = currentModule->defdocstringfmt;
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 256:
#line 2297 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.docstring).format = convertFormat((yyvsp[(1) - (1)].text));
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 257:
#line 2303 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring) = (yyvsp[(2) - (3)].docstring);
        }
    break;

  case 259:
#line 2309 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring) = (yyvsp[(1) - (3)].docstring);

            switch ((yyvsp[(3) - (3)].docstring).token)
            {
            case TK_FORMAT: (yyval.docstring).format = (yyvsp[(3) - (3)].docstring).format; break;
            case TK_SIGNATURE: (yyval.docstring).signature = (yyvsp[(3) - (3)].docstring).signature; break;
            }
        }
    break;

  case 260:
#line 2320 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).token = TK_FORMAT;

            (yyval.docstring).format = convertFormat((yyvsp[(3) - (3)].text));
            (yyval.docstring).signature = currentModule->defdocstringsig;
        }
    break;

  case 261:
#line 2326 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstring).token = TK_SIGNATURE;

            (yyval.docstring).format = currentModule->defdocstringfmt;
            (yyval.docstring).signature = convertSignature((yyvsp[(3) - (3)].text));
        }
    break;

  case 262:
#line 2334 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.docstr) = NULL;
        }
    break;

  case 264:
#line 2340 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (3)].extract).id == NULL)
                yyerror("%Extract must have an 'id' argument");

            if (notSkipping())
                addExtractPart(currentSpec, (yyvsp[(2) - (3)].extract).id, (yyvsp[(2) - (3)].extract).order, (yyvsp[(3) - (3)].codeb));
        }
    break;

  case 265:
#line 2349 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            resetLexerState();

            (yyval.extract).id = (yyvsp[(1) - (1)].text);
            (yyval.extract).order = -1;
        }
    break;

  case 266:
#line 2355 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract) = (yyvsp[(2) - (3)].extract);
        }
    break;

  case 268:
#line 2361 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract) = (yyvsp[(1) - (3)].extract);

            switch ((yyvsp[(3) - (3)].extract).token)
            {
            case TK_ID: (yyval.extract).id = (yyvsp[(3) - (3)].extract).id; break;
            case TK_ORDER: (yyval.extract).order = (yyvsp[(3) - (3)].extract).order; break;
            }
        }
    break;

  case 269:
#line 2372 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract).token = TK_ID;

            (yyval.extract).id = (yyvsp[(3) - (3)].text);
            (yyval.extract).order = -1;
        }
    break;

  case 270:
#line 2378 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.extract).token = TK_ORDER;

            if ((yyvsp[(3) - (3)].number) < 0)
                yyerror("The 'order' of an %Extract directive must not be negative");

            (yyval.extract).id = NULL;
            (yyval.extract).order = (yyvsp[(3) - (3)].number);
        }
    break;

  case 273:
#line 2393 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(1) - (2)].codeb);

            append(&(yyval.codeb)->frag, (yyvsp[(2) - (2)].codeb)->frag);

            free((yyvsp[(2) - (2)].codeb)->frag);
            free((yyvsp[(2) - (2)].codeb));
        }
    break;

  case 274:
#line 2403 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 276:
#line 2427 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 277:
#line 2430 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 278:
#line 2433 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 279:
#line 2438 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.text) = NULL;
        }
    break;

  case 280:
#line 2441 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 287:
#line 2456 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

                /*
                 * Note that we don't check that members of scoped enums are
                 * unique.
                 */
                if (!isScopedEnum(currentEnum))
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

  case 292:
#line 2508 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.valp) = NULL;
        }
    break;

  case 293:
#line 2511 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.valp) = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 295:
#line 2517 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 296:
#line 2535 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 297:
#line 2538 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 298:
#line 2541 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '*';
        }
    break;

  case 299:
#line 2544 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '/';
        }
    break;

  case 300:
#line 2547 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '&';
        }
    break;

  case 301:
#line 2550 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '|';
        }
    break;

  case 302:
#line 2555 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '\0';
        }
    break;

  case 303:
#line 2558 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '!';
        }
    break;

  case 304:
#line 2561 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '~';
        }
    break;

  case 305:
#line 2564 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 306:
#line 2567 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 307:
#line 2570 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '*';
        }
    break;

  case 308:
#line 2573 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.qchar) = '&';
        }
    break;

  case 309:
#line 2578 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 310:
#line 2593 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = NULL;
        }
    break;

  case 311:
#line 2596 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = (yyvsp[(2) - (3)].scpvalp);
        }
    break;

  case 312:
#line 2601 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Scoped names are not allowed in a C module");

            (yyval.scpvalp) = scopeScopedName(NULL, (yyvsp[(2) - (2)].scpvalp));
        }
    break;

  case 315:
#line 2611 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Scoped names are not allowed in a C module");

            appendScopedName(&(yyvsp[(1) - (3)].scpvalp), (yyvsp[(3) - (3)].scpvalp));
        }
    break;

  case 316:
#line 2619 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.scpvalp) = text2scopePart((yyvsp[(1) - (1)].text));
        }
    break;

  case 317:
#line 2624 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 318:
#line 2627 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 319:
#line 2632 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /*
             * We let the C++ compiler decide if the value is a valid one - no
             * point in building a full C++ parser here.
             */

            (yyval.value).vtype = scoped_value;
            (yyval.value).u.vscp = (yyvsp[(1) - (1)].scpvalp);
        }
    break;

  case 320:
#line 2641 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            fcallDef *fcd;

            fcd = sipMalloc(sizeof (fcallDef));
            *fcd = (yyvsp[(3) - (4)].fcall);
            fcd -> type = (yyvsp[(1) - (4)].memArg);

            (yyval.value).vtype = fcall_value;
            (yyval.value).u.fcd = fcd;
        }
    break;

  case 321:
#line 2651 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = empty_value;
        }
    break;

  case 322:
#line 2654 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = real_value;
            (yyval.value).u.vreal = (yyvsp[(1) - (1)].real);
        }
    break;

  case 323:
#line 2658 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = (yyvsp[(1) - (1)].number);
        }
    break;

  case 324:
#line 2662 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = (yyvsp[(1) - (1)].boolean);
        }
    break;

  case 325:
#line 2666 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = 0;
        }
    break;

  case 326:
#line 2670 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = string_value;
            (yyval.value).u.vstr = (yyvsp[(1) - (1)].text);
        }
    break;

  case 327:
#line 2674 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.value).vtype = qchar_value;
            (yyval.value).u.vqchar = (yyvsp[(1) - (1)].qchar);
        }
    break;

  case 328:
#line 2680 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* No values. */

            (yyval.fcall).nrArgs = 0;
        }
    break;

  case 329:
#line 2685 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first expression. */

            (yyval.fcall).args[0] = (yyvsp[(1) - (1)].valp);
            (yyval.fcall).nrArgs = 1;
        }
    break;

  case 330:
#line 2691 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 331:
#line 2709 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "Capsule",
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

  case 332:
#line 2730 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
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

  case 333:
#line 2768 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc && (yyvsp[(2) - (2)].scpvalp)->next != NULL)
                yyerror("Namespaces not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 334:
#line 2774 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 335:
#line 2812 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 336:
#line 2818 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsTemplate = TRUE;}
    break;

  case 337:
#line 2818 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 338:
#line 2844 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.signature) = (yyvsp[(3) - (4)].signature);
        }
    break;

  case 339:
#line 2849 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Class definition not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 340:
#line 2855 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 341:
#line 2889 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                (yyval.klass) = completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 346:
#line 2903 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 347:
#line 2955 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PUBLIC;
        }
    break;

  case 348:
#line 2958 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PUBLIC;
        }
    break;

  case 349:
#line 2961 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PROTECTED;
        }
    break;

  case 350:
#line 2964 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
        (yyval.token) = TK_PRIVATE;
        }
    break;

  case 351:
#line 2969 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 352:
#line 2972 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 366:
#line 2992 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 367:
#line 3003 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->cppcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 368:
#line 3007 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 369:
#line 3011 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 370:
#line 3022 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 371:
#line 3033 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 372:
#line 3044 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 373:
#line 3055 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Remove in v6. */
        }
    break;

  case 374:
#line 3058 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Remove in v6. */
        }
    break;

  case 375:
#line 3061 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Remove in v6. */
        }
    break;

  case 376:
#line 3064 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Remove in v6. */
        }
    break;

  case 377:
#line 3067 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 378:
#line 3078 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 379:
#line 3089 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 380:
#line 3100 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 384:
#line 3114 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 385:
#line 3125 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 386:
#line 3136 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 387:
#line 3147 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("public section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PUBLIC | (yyvsp[(2) - (3)].number);
        }
    break;

  case 388:
#line 3154 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("protected section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PROT | (yyvsp[(2) - (3)].number);
        }
    break;

  case 389:
#line 3161 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("private section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PRIVATE | (yyvsp[(2) - (3)].number);
        }
    break;

  case 390:
#line 3168 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("signals section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_SIGNAL;
        }
    break;

  case 391:
#line 3177 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 392:
#line 3190 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(2) - (3)].property);
        }
    break;

  case 394:
#line 3196 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 395:
#line 3208 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_GET;

            (yyval.property).get = (yyvsp[(3) - (3)].text);
            (yyval.property).name = NULL;
            (yyval.property).set = NULL;
        }
    break;

  case 396:
#line 3215 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_NAME;

            (yyval.property).get = NULL;
            (yyval.property).name = (yyvsp[(3) - (3)].text);
            (yyval.property).set = NULL;
        }
    break;

  case 397:
#line 3222 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_SET;

            (yyval.property).get = NULL;
            (yyval.property).name = NULL;
            (yyval.property).set = (yyvsp[(3) - (3)].text);
        }
    break;

  case 398:
#line 3231 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = 0;
            (yyval.property).docstring = NULL;
        }
    break;

  case 399:
#line 3235 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(2) - (4)].property);
        }
    break;

  case 401:
#line 3241 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property) = (yyvsp[(1) - (2)].property);

            switch ((yyvsp[(2) - (2)].property).token)
            {
            case TK_DOCSTRING: (yyval.property).docstring = (yyvsp[(2) - (2)].property).docstring; break;
            }
        }
    break;

  case 402:
#line 3251 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_IF;
        }
    break;

  case 403:
#line 3254 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.property).token = TK_END;
        }
    break;

  case 404:
#line 3257 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 407:
#line 3275 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 408:
#line 3278 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = SECT_IS_SLOT;
        }
    break;

  case 409:
#line 3284 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsVirt = TRUE;}
    break;

  case 412:
#line 3288 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 413:
#line 3354 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentCtorIsExplicit = TRUE;}
    break;

  case 416:
#line 3358 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 417:
#line 3404 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 418:
#line 3407 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            parsingCSignature = TRUE;
        }
    break;

  case 419:
#line 3409 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(4) - (6)].signature);

            parsingCSignature = FALSE;
        }
    break;

  case 420:
#line 3418 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 421:
#line 3421 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            parsingCSignature = TRUE;
        }
    break;

  case 422:
#line 3423 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(5) - (7)].signature);
            (yyval.optsignature)->result = (yyvsp[(3) - (7)].memArg);

            parsingCSignature = FALSE;
        }
    break;

  case 423:
#line 3433 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 424:
#line 3451 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 425:
#line 3471 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 426:
#line 3515 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
                case long_type:
                case ulong_type:
                case longlong_type:
                case ulonglong_type:
                    sname = "__int__";
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

  case 427:
#line 3593 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__add__";}
    break;

  case 428:
#line 3594 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__sub__";}
    break;

  case 429:
#line 3595 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__mul__";}
    break;

  case 430:
#line 3596 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__truediv__";}
    break;

  case 431:
#line 3597 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__mod__";}
    break;

  case 432:
#line 3598 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__and__";}
    break;

  case 433:
#line 3599 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__or__";}
    break;

  case 434:
#line 3600 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__xor__";}
    break;

  case 435:
#line 3601 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__lshift__";}
    break;

  case 436:
#line 3602 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__rshift__";}
    break;

  case 437:
#line 3603 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__iadd__";}
    break;

  case 438:
#line 3604 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__isub__";}
    break;

  case 439:
#line 3605 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__imul__";}
    break;

  case 440:
#line 3606 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__itruediv__";}
    break;

  case 441:
#line 3607 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__imod__";}
    break;

  case 442:
#line 3608 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__iand__";}
    break;

  case 443:
#line 3609 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ior__";}
    break;

  case 444:
#line 3610 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ixor__";}
    break;

  case 445:
#line 3611 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ilshift__";}
    break;

  case 446:
#line 3612 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__irshift__";}
    break;

  case 447:
#line 3613 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__invert__";}
    break;

  case 448:
#line 3614 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__call__";}
    break;

  case 449:
#line 3615 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__getitem__";}
    break;

  case 450:
#line 3616 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__lt__";}
    break;

  case 451:
#line 3617 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__le__";}
    break;

  case 452:
#line 3618 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__eq__";}
    break;

  case 453:
#line 3619 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ne__";}
    break;

  case 454:
#line 3620 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__gt__";}
    break;

  case 455:
#line 3621 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {(yyval.text) = "__ge__";}
    break;

  case 456:
#line 3624 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 457:
#line 3627 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 458:
#line 3632 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 459:
#line 3635 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 460:
#line 3640 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 461:
#line 3643 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if ((yyvsp[(2) - (2)].number) != 0)
                yyerror("Abstract virtual function '= 0' expected");

            (yyval.number) = TRUE;
        }
    break;

  case 462:
#line 3651 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags).nrFlags = 0;
        }
    break;

  case 463:
#line 3654 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags) = (yyvsp[(2) - (3)].optflags);
        }
    break;

  case 464:
#line 3660 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.optflags).flags[0] = (yyvsp[(1) - (1)].flag);
            (yyval.optflags).nrFlags = 1;
        }
    break;

  case 465:
#line 3664 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Check there is room. */

            if ((yyvsp[(1) - (3)].optflags).nrFlags == MAX_NR_FLAGS)
                yyerror("Too many optional flags");

            (yyval.optflags) = (yyvsp[(1) - (3)].optflags);

            (yyval.optflags).flags[(yyval.optflags).nrFlags++] = (yyvsp[(3) - (3)].flag);
        }
    break;

  case 466:
#line 3676 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = bool_flag;
            (yyval.flag).fname = (yyvsp[(1) - (1)].text);
        }
    break;

  case 467:
#line 3680 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag) = (yyvsp[(3) - (3)].flag);
            (yyval.flag).fname = (yyvsp[(1) - (3)].text);
        }
    break;

  case 468:
#line 3686 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = (strchr((yyvsp[(1) - (1)].text), '.') != NULL) ? dotted_name_flag : name_flag;
            (yyval.flag).fvalue.sval = (yyvsp[(1) - (1)].text);
        }
    break;

  case 469:
#line 3690 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 470:
#line 3713 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = string_flag;
            (yyval.flag).fvalue.sval = convertFeaturedString((yyvsp[(1) - (1)].text));
        }
    break;

  case 471:
#line 3717 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.flag).ftype = integer_flag;
            (yyval.flag).fvalue.ival = (yyvsp[(1) - (1)].number);
        }
    break;

  case 472:
#line 3723 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 473:
#line 3726 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 474:
#line 3731 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 475:
#line 3734 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 476:
#line 3739 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 477:
#line 3742 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 478:
#line 3747 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 479:
#line 3750 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 480:
#line 3755 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            int a, nrarray, nrarraysize;

            nrarray = nrarraysize = 0;

            for (a = 0; a < (yyvsp[(1) - (1)].signature).nrArgs; ++a)
            {
                argDef *ad = &(yyvsp[(1) - (1)].signature).args[a];

                if (isArray(ad))
                    ++nrarray;

                if (isArraySize(ad))
                    ++nrarraysize;
            }

            if (nrarray != nrarraysize || nrarray > 1)
                yyerror("/Array/ and /ArraySize/ must both be given and at most once");

            (yyval.signature) = (yyvsp[(1) - (1)].signature);
        }
    break;

  case 481:
#line 3778 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* No arguments. */

            (yyval.signature).nrArgs = 0;
        }
    break;

  case 482:
#line 3783 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first argument. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 483:
#line 3789 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 484:
#line 3812 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (2)].memArg);
            (yyval.memArg).defval = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 485:
#line 3819 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsSignal = TRUE;}
    break;

  case 487:
#line 3820 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsSlot = TRUE;}
    break;

  case 490:
#line 3825 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsStatic = TRUE;}
    break;

  case 495:
#line 3835 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {currentIsVirt = TRUE;}
    break;

  case 498:
#line 3839 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (notSkipping())
            {
                const char *annos[] = {
                    "Encoding",
                    "NoSetter",
                    "NoTypeHint",
                    "PyInt",
                    "PyName",
                    "TypeHint",
                    NULL
                };

                checkAnnos(&(yyvsp[(3) - (5)].optflags), annos);

                newVar(currentSpec, currentModule, (yyvsp[(2) - (5)].text), currentIsStatic, &(yyvsp[(1) - (5)].memArg),
                        &(yyvsp[(3) - (5)].optflags), (yyvsp[(4) - (5)].variable).access_code, (yyvsp[(4) - (5)].variable).get_code, (yyvsp[(4) - (5)].variable).set_code,
                        sectionFlags);
            }

            currentIsStatic = FALSE;
        }
    break;

  case 499:
#line 3863 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = 0;
            (yyval.variable).access_code = NULL;
            (yyval.variable).get_code = NULL;
            (yyval.variable).set_code = NULL;
        }
    break;

  case 500:
#line 3869 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable) = (yyvsp[(2) - (3)].variable);
        }
    break;

  case 502:
#line 3875 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 503:
#line 3887 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = TK_IF;
        }
    break;

  case 504:
#line 3890 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.variable).token = TK_END;
        }
    break;

  case 505:
#line 3893 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 506:
#line 3908 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 507:
#line 3923 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 508:
#line 3940 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg) = (yyvsp[(2) - (4)].memArg);
            add_derefs(&(yyval.memArg), &(yyvsp[(3) - (4)].memArg));
            (yyval.memArg).argflags |= ARG_IS_CONST | (yyvsp[(4) - (4)].number);
        }
    break;

  case 509:
#line 3945 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 510:
#line 3959 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            const char *annos[] = {
                "AllowNone",
                "Array",
                "ArraySize",
                "Constrained",
                "DisallowNone",
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

  case 511:
#line 4067 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 512:
#line 4070 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("References not allowed in a C module");

            (yyval.number) = ARG_IS_REF;
        }
    break;

  case 513:
#line 4078 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.memArg).nrderefs = 0;
        }
    break;

  case 514:
#line 4081 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            add_new_deref(&(yyval.memArg), &(yyvsp[(1) - (3)].memArg), TRUE);
        }
    break;

  case 515:
#line 4084 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            add_new_deref(&(yyval.memArg), &(yyvsp[(1) - (2)].memArg), FALSE);
        }
    break;

  case 516:
#line 4089 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = defined_type;
            (yyval.memArg).u.snd = (yyvsp[(1) - (1)].scpvalp);

            /* Try and resolve typedefs as early as possible. */
            resolveAnyTypedef(currentSpec, &(yyval.memArg));
        }
    break;

  case 517:
#line 4097 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 518:
#line 4108 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 519:
#line 4123 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ushort_type;
        }
    break;

  case 520:
#line 4127 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = short_type;
        }
    break;

  case 521:
#line 4131 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 522:
#line 4135 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 523:
#line 4139 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = int_type;
        }
    break;

  case 524:
#line 4143 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = long_type;
        }
    break;

  case 525:
#line 4147 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulong_type;
        }
    break;

  case 526:
#line 4151 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = longlong_type;
        }
    break;

  case 527:
#line 4155 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulonglong_type;
        }
    break;

  case 528:
#line 4159 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = float_type;
        }
    break;

  case 529:
#line 4163 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = double_type;
        }
    break;

  case 530:
#line 4167 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = bool_type;
        }
    break;

  case 531:
#line 4171 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = sstring_type;
        }
    break;

  case 532:
#line 4175 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ustring_type;
        }
    break;

  case 533:
#line 4179 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = string_type;
        }
    break;

  case 534:
#line 4183 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = wstring_type;
        }
    break;

  case 535:
#line 4187 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = void_type;
        }
    break;

  case 536:
#line 4191 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyobject_type;
        }
    break;

  case 537:
#line 4195 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytuple_type;
        }
    break;

  case 538:
#line 4199 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pylist_type;
        }
    break;

  case 539:
#line 4203 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pydict_type;
        }
    break;

  case 540:
#line 4207 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pycallable_type;
        }
    break;

  case 541:
#line 4211 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyslice_type;
        }
    break;

  case 542:
#line 4215 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytype_type;
        }
    break;

  case 543:
#line 4219 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pybuffer_type;
        }
    break;

  case 544:
#line 4223 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ssize_type;
        }
    break;

  case 545:
#line 4227 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = size_type;
        }
    break;

  case 546:
#line 4231 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ellipsis_type;
        }
    break;

  case 547:
#line 4237 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The single or first type. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 548:
#line 4243 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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

  case 549:
#line 4259 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            (yyval.throwlist) = NULL;
        }
    break;

  case 550:
#line 4262 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Exceptions not allowed in a C module");

            (yyval.throwlist) = (yyvsp[(3) - (4)].throwlist);
        }
    break;

  case 551:
#line 4270 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* Empty list so use a blank. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 0;
        }
    break;

  case 552:
#line 4276 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
    {
            /* The only or first exception. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 1;
            (yyval.throwlist) -> args[0] = findException(currentSpec, (yyvsp[(1) - (1)].scpvalp), FALSE);
        }
    break;

  case 553:
#line 4283 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 7724 "/Users/phil/hg/sip/code_generator/parser.c"
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


#line 4299 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"



/*
 * Parse the specification.
 */
void parse(sipSpec *spec, FILE *fp, char *filename, int strict,
        stringList **tsl, stringList *bsl, stringList **xfl, int protHack,
        stringList **sip_files)
{
    classTmplDef *tcd;

    initialiseLexer();

    /* Initialise the spec. */
 
    memset(spec, 0, sizeof (sipSpec));
    spec->genc = -1;

    currentSpec = spec;
    strictParse = strict;
    backstops = bsl;
    neededQualifiers = *tsl;
    excludedQualifiers = *xfl;
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
    mainModuleSipFiles = sip_files;

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

    /* These may have been updated from imported bindings. */
    *tsl = neededQualifiers;
    *xfl = excludedQualifiers;
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
static moduleDef *allocModule(void)
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
    {
        currentContext = pc;

        /* Save the name of the file if this is the main module. */
        if (prevmod == NULL)
            appendString(mainModuleSipFiles, sipStrdup(name));
    }
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

        templateSignature(&nct->pysig, oct->kwargs, FALSE, tcd, td, cd,
                type_names, type_values);

        if (oct->cppsig == NULL)
            nct->cppsig = NULL;
        else if (oct->cppsig == &oct->pysig)
            nct->cppsig = &nct->pysig;
        else
        {
            nct->cppsig = sipMalloc(sizeof (signatureDef));

            *nct->cppsig = *oct->cppsig;

            templateSignature(nct->cppsig, NoKwArgs, FALSE, tcd, td, cd,
                    type_names, type_values);
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

        templateSignature(&nod->pysig, od->kwargs, TRUE, tcd, td, cd,
                type_names, type_values);

        if (od->cppsig == &od->pysig)
            nod->cppsig = &nod->pysig;
        else
        {
            nod->cppsig = sipMalloc(sizeof (signatureDef));

            *nod->cppsig = *od->cppsig;

            templateSignature(nod->cppsig, NoKwArgs, TRUE, tcd, td, cd,
                    type_names, type_values);
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
static void templateSignature(signatureDef *sd, KwArgs kwargs, int result,
        classTmplDef *tcd, templateDef *td, classDef *ncd,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    int a;

    if (result)
        templateType(&sd->result, tcd, td, ncd, type_names, type_values);

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        templateType(ad, tcd, td, ncd, type_names, type_values);

        /* Make sure we have the name of any keyword argument. */
        if (inMainModule() && ad->name != NULL)
        {
            if (kwargs == AllKwArgs || (kwargs == OptionalKwArgs && ad->defval != NULL))
                setIsUsedName(ad->name);
        }
    }
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

        templateSignature(&ad->u.td->types, NoKwArgs, FALSE, tcd, td, ncd,
                type_names, type_values);

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
        {"__float__", float_slot, FALSE, 0},
        {"__len__", len_slot, TRUE, 0},
        {"__contains__", contains_slot, TRUE, 1},
        {"__add__", add_slot, FALSE, 1},
        {"__sub__", sub_slot, FALSE, 1},
        {"__mul__", mul_slot, FALSE, 1},
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
        /* Read any configuration .toml file. */
        get_bindings_configuration(filename, &neededQualifiers,
                &excludedQualifiers);

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
 * Get the /TypeHintValue/ option flag.
 */
static const char *getTypeHintValue(optFlags *optflgs)
{
    optFlag *of = getOptFlag(optflgs, "TypeHintValue", string_flag);

    if (of == NULL)
        return NULL;

    return of->fvalue.sval;
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

    deprecated("The /API/ annotation is deprecated and will be removed in v6");

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
    optFlag *ka_anno;

    /* See if there was an explicit annotation. */
    ka_anno = getOptFlag(optflgs, "KeywordArgs", string_flag);

    if (ka_anno != NULL)
        kwargs = convertKwArgs(ka_anno->fvalue.sval);
    else
        kwargs = mod->kwargs;

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
    int i;

    if (parsingCSignature && annos->nrFlags != 0)
        yyerror("Annotations must not be used in explicit C/C++ signatures");

    for (i = 0; i < annos->nrFlags; i++)
    {
        const char **name;

        for (name = valid; *name != NULL; ++name)
            if (strcmp(*name, annos->flags[i].fname) == 0)
                break;

        if (*name == NULL)
            yyerror("Annotation is unknown");
    }
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

