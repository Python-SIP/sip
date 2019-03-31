/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
     TK_OPTINCLUDE = 306,
     TK_IMPORT = 307,
     TK_EXPHEADERCODE = 308,
     TK_MODHEADERCODE = 309,
     TK_TYPEHEADERCODE = 310,
     TK_MODULE = 311,
     TK_CMODULE = 312,
     TK_COMPOMODULE = 313,
     TK_CLASS = 314,
     TK_STRUCT = 315,
     TK_PUBLIC = 316,
     TK_PROTECTED = 317,
     TK_PRIVATE = 318,
     TK_SIGNALS = 319,
     TK_SIGNAL_METHOD = 320,
     TK_SLOTS = 321,
     TK_SLOT_METHOD = 322,
     TK_BOOL = 323,
     TK_SHORT = 324,
     TK_INT = 325,
     TK_LONG = 326,
     TK_FLOAT = 327,
     TK_DOUBLE = 328,
     TK_CHAR = 329,
     TK_WCHAR_T = 330,
     TK_VOID = 331,
     TK_PYOBJECT = 332,
     TK_PYTUPLE = 333,
     TK_PYLIST = 334,
     TK_PYDICT = 335,
     TK_PYCALLABLE = 336,
     TK_PYSLICE = 337,
     TK_PYTYPE = 338,
     TK_PYBUFFER = 339,
     TK_VIRTUAL = 340,
     TK_ENUM = 341,
     TK_SIGNED = 342,
     TK_UNSIGNED = 343,
     TK_SCOPE = 344,
     TK_LOGICAL_OR = 345,
     TK_CONST = 346,
     TK_STATIC = 347,
     TK_SIPSIGNAL = 348,
     TK_SIPSLOT = 349,
     TK_SIPANYSLOT = 350,
     TK_SIPRXCON = 351,
     TK_SIPRXDIS = 352,
     TK_SIPSLOTCON = 353,
     TK_SIPSLOTDIS = 354,
     TK_SIPSSIZET = 355,
     TK_SIZET = 356,
     TK_NUMBER_VALUE = 357,
     TK_REAL_VALUE = 358,
     TK_TYPEDEF = 359,
     TK_NAMESPACE = 360,
     TK_TIMELINE = 361,
     TK_PLATFORMS = 362,
     TK_FEATURE = 363,
     TK_LICENSE = 364,
     TK_QCHAR_VALUE = 365,
     TK_TRUE_VALUE = 366,
     TK_FALSE_VALUE = 367,
     TK_NULL_VALUE = 368,
     TK_OPERATOR = 369,
     TK_THROW = 370,
     TK_QOBJECT = 371,
     TK_EXCEPTION = 372,
     TK_RAISECODE = 373,
     TK_EXPLICIT = 374,
     TK_TEMPLATE = 375,
     TK_FINAL = 376,
     TK_ELLIPSIS = 377,
     TK_DEFMETATYPE = 378,
     TK_DEFSUPERTYPE = 379,
     TK_PROPERTY = 380,
     TK_HIDE_NS = 381,
     TK_FORMAT = 382,
     TK_GET = 383,
     TK_ID = 384,
     TK_KWARGS = 385,
     TK_LANGUAGE = 386,
     TK_LICENSEE = 387,
     TK_NAME = 388,
     TK_OPTIONAL = 389,
     TK_ORDER = 390,
     TK_REMOVELEADING = 391,
     TK_SET = 392,
     TK_SIGNATURE = 393,
     TK_TIMESTAMP = 394,
     TK_TYPE = 395,
     TK_USEARGNAMES = 396,
     TK_USELIMITEDAPI = 397,
     TK_ALLRAISEPYEXC = 398,
     TK_CALLSUPERINIT = 399,
     TK_DEFERRORHANDLER = 400,
     TK_VERSION = 401
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
#define TK_OPTINCLUDE 306
#define TK_IMPORT 307
#define TK_EXPHEADERCODE 308
#define TK_MODHEADERCODE 309
#define TK_TYPEHEADERCODE 310
#define TK_MODULE 311
#define TK_CMODULE 312
#define TK_COMPOMODULE 313
#define TK_CLASS 314
#define TK_STRUCT 315
#define TK_PUBLIC 316
#define TK_PROTECTED 317
#define TK_PRIVATE 318
#define TK_SIGNALS 319
#define TK_SIGNAL_METHOD 320
#define TK_SLOTS 321
#define TK_SLOT_METHOD 322
#define TK_BOOL 323
#define TK_SHORT 324
#define TK_INT 325
#define TK_LONG 326
#define TK_FLOAT 327
#define TK_DOUBLE 328
#define TK_CHAR 329
#define TK_WCHAR_T 330
#define TK_VOID 331
#define TK_PYOBJECT 332
#define TK_PYTUPLE 333
#define TK_PYLIST 334
#define TK_PYDICT 335
#define TK_PYCALLABLE 336
#define TK_PYSLICE 337
#define TK_PYTYPE 338
#define TK_PYBUFFER 339
#define TK_VIRTUAL 340
#define TK_ENUM 341
#define TK_SIGNED 342
#define TK_UNSIGNED 343
#define TK_SCOPE 344
#define TK_LOGICAL_OR 345
#define TK_CONST 346
#define TK_STATIC 347
#define TK_SIPSIGNAL 348
#define TK_SIPSLOT 349
#define TK_SIPANYSLOT 350
#define TK_SIPRXCON 351
#define TK_SIPRXDIS 352
#define TK_SIPSLOTCON 353
#define TK_SIPSLOTDIS 354
#define TK_SIPSSIZET 355
#define TK_SIZET 356
#define TK_NUMBER_VALUE 357
#define TK_REAL_VALUE 358
#define TK_TYPEDEF 359
#define TK_NAMESPACE 360
#define TK_TIMELINE 361
#define TK_PLATFORMS 362
#define TK_FEATURE 363
#define TK_LICENSE 364
#define TK_QCHAR_VALUE 365
#define TK_TRUE_VALUE 366
#define TK_FALSE_VALUE 367
#define TK_NULL_VALUE 368
#define TK_OPERATOR 369
#define TK_THROW 370
#define TK_QOBJECT 371
#define TK_EXCEPTION 372
#define TK_RAISECODE 373
#define TK_EXPLICIT 374
#define TK_TEMPLATE 375
#define TK_FINAL 376
#define TK_ELLIPSIS 377
#define TK_DEFMETATYPE 378
#define TK_DEFSUPERTYPE 379
#define TK_PROPERTY 380
#define TK_HIDE_NS 381
#define TK_FORMAT 382
#define TK_GET 383
#define TK_ID 384
#define TK_KWARGS 385
#define TK_LANGUAGE 386
#define TK_LICENSEE 387
#define TK_NAME 388
#define TK_OPTIONAL 389
#define TK_ORDER 390
#define TK_REMOVELEADING 391
#define TK_SET 392
#define TK_SIGNATURE 393
#define TK_TIMESTAMP 394
#define TK_TYPE 395
#define TK_USEARGNAMES 396
#define TK_USELIMITEDAPI 397
#define TK_ALLRAISEPYEXC 398
#define TK_CALLSUPERINIT 399
#define TK_DEFERRORHANDLER 400
#define TK_VERSION 401




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 203 "/Users/phil/hg/sip/sipgen/metasrc/parser.y"
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
/* Line 1529 of yacc.c.  */
#line 385 "/Users/phil/hg/sip/sipgen/parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

