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
     TK_AUTOPYNAME = 258,
     TK_DEFDOCSTRFMT = 259,
     TK_DEFDOCSTRSIG = 260,
     TK_DEFENCODING = 261,
     TK_PLUGIN = 262,
     TK_VIRTERRORHANDLER = 263,
     TK_EXPTYPEHINTCODE = 264,
     TK_TYPEHINTCODE = 265,
     TK_DOCSTRING = 266,
     TK_EXTRACT = 267,
     TK_ACCESSCODE = 268,
     TK_GETCODE = 269,
     TK_SETCODE = 270,
     TK_PREINITCODE = 271,
     TK_INITCODE = 272,
     TK_POSTINITCODE = 273,
     TK_FINALCODE = 274,
     TK_UNITCODE = 275,
     TK_UNITPOSTINCLUDECODE = 276,
     TK_MODCODE = 277,
     TK_TYPECODE = 278,
     TK_COPYING = 279,
     TK_MAPPEDTYPE = 280,
     TK_CODELINE = 281,
     TK_IF = 282,
     TK_END = 283,
     TK_NAME_VALUE = 284,
     TK_PATH_VALUE = 285,
     TK_STRING_VALUE = 286,
     TK_VIRTUALCATCHERCODE = 287,
     TK_TRAVERSECODE = 288,
     TK_CLEARCODE = 289,
     TK_GETBUFFERCODE = 290,
     TK_RELEASEBUFFERCODE = 291,
     TK_READBUFFERCODE = 292,
     TK_WRITEBUFFERCODE = 293,
     TK_SEGCOUNTCODE = 294,
     TK_CHARBUFFERCODE = 295,
     TK_PICKLECODE = 296,
     TK_VIRTUALCALLCODE = 297,
     TK_METHODCODE = 298,
     TK_PREMETHODCODE = 299,
     TK_INSTANCECODE = 300,
     TK_FROMTYPE = 301,
     TK_TOTYPE = 302,
     TK_TOSUBCLASS = 303,
     TK_INCLUDE = 304,
     TK_IMPORT = 305,
     TK_EXPHEADERCODE = 306,
     TK_MODHEADERCODE = 307,
     TK_TYPEHEADERCODE = 308,
     TK_MODULE = 309,
     TK_COMPOMODULE = 310,
     TK_CLASS = 311,
     TK_STRUCT = 312,
     TK_PUBLIC = 313,
     TK_PROTECTED = 314,
     TK_PRIVATE = 315,
     TK_SIGNALS = 316,
     TK_SIGNAL_METHOD = 317,
     TK_SLOTS = 318,
     TK_SLOT_METHOD = 319,
     TK_BOOL = 320,
     TK_SHORT = 321,
     TK_INT = 322,
     TK_LONG = 323,
     TK_FLOAT = 324,
     TK_DOUBLE = 325,
     TK_CHAR = 326,
     TK_WCHAR_T = 327,
     TK_VOID = 328,
     TK_PYOBJECT = 329,
     TK_PYTUPLE = 330,
     TK_PYLIST = 331,
     TK_PYDICT = 332,
     TK_PYCALLABLE = 333,
     TK_PYSLICE = 334,
     TK_PYTYPE = 335,
     TK_PYBUFFER = 336,
     TK_VIRTUAL = 337,
     TK_ENUM = 338,
     TK_SIGNED = 339,
     TK_UNSIGNED = 340,
     TK_SCOPE = 341,
     TK_LOGICAL_OR = 342,
     TK_CONST = 343,
     TK_STATIC = 344,
     TK_PYSSIZET = 345,
     TK_SIZET = 346,
     TK_NUMBER_VALUE = 347,
     TK_REAL_VALUE = 348,
     TK_TYPEDEF = 349,
     TK_NAMESPACE = 350,
     TK_TIMELINE = 351,
     TK_PLATFORMS = 352,
     TK_FEATURE = 353,
     TK_LICENSE = 354,
     TK_QCHAR_VALUE = 355,
     TK_TRUE_VALUE = 356,
     TK_FALSE_VALUE = 357,
     TK_NULL_VALUE = 358,
     TK_OPERATOR = 359,
     TK_THROW = 360,
     TK_EXCEPTION = 361,
     TK_RAISECODE = 362,
     TK_EXPLICIT = 363,
     TK_TEMPLATE = 364,
     TK_FINAL = 365,
     TK_ELLIPSIS = 366,
     TK_DEFMETATYPE = 367,
     TK_DEFSUPERTYPE = 368,
     TK_PROPERTY = 369,
     TK_HIDE_NS = 370,
     TK_FORMAT = 371,
     TK_GET = 372,
     TK_ID = 373,
     TK_KWARGS = 374,
     TK_LANGUAGE = 375,
     TK_LICENSEE = 376,
     TK_NAME = 377,
     TK_OPTIONAL = 378,
     TK_ORDER = 379,
     TK_REMOVELEADING = 380,
     TK_SET = 381,
     TK_SIGNATURE = 382,
     TK_TIMESTAMP = 383,
     TK_TYPE = 384,
     TK_USEARGNAMES = 385,
     TK_USELIMITEDAPI = 386,
     TK_ALLRAISEPYEXC = 387,
     TK_CALLSUPERINIT = 388,
     TK_DEFERRORHANDLER = 389,
     TK_VERSION = 390
   };
#endif
/* Tokens.  */
#define TK_AUTOPYNAME 258
#define TK_DEFDOCSTRFMT 259
#define TK_DEFDOCSTRSIG 260
#define TK_DEFENCODING 261
#define TK_PLUGIN 262
#define TK_VIRTERRORHANDLER 263
#define TK_EXPTYPEHINTCODE 264
#define TK_TYPEHINTCODE 265
#define TK_DOCSTRING 266
#define TK_EXTRACT 267
#define TK_ACCESSCODE 268
#define TK_GETCODE 269
#define TK_SETCODE 270
#define TK_PREINITCODE 271
#define TK_INITCODE 272
#define TK_POSTINITCODE 273
#define TK_FINALCODE 274
#define TK_UNITCODE 275
#define TK_UNITPOSTINCLUDECODE 276
#define TK_MODCODE 277
#define TK_TYPECODE 278
#define TK_COPYING 279
#define TK_MAPPEDTYPE 280
#define TK_CODELINE 281
#define TK_IF 282
#define TK_END 283
#define TK_NAME_VALUE 284
#define TK_PATH_VALUE 285
#define TK_STRING_VALUE 286
#define TK_VIRTUALCATCHERCODE 287
#define TK_TRAVERSECODE 288
#define TK_CLEARCODE 289
#define TK_GETBUFFERCODE 290
#define TK_RELEASEBUFFERCODE 291
#define TK_READBUFFERCODE 292
#define TK_WRITEBUFFERCODE 293
#define TK_SEGCOUNTCODE 294
#define TK_CHARBUFFERCODE 295
#define TK_PICKLECODE 296
#define TK_VIRTUALCALLCODE 297
#define TK_METHODCODE 298
#define TK_PREMETHODCODE 299
#define TK_INSTANCECODE 300
#define TK_FROMTYPE 301
#define TK_TOTYPE 302
#define TK_TOSUBCLASS 303
#define TK_INCLUDE 304
#define TK_IMPORT 305
#define TK_EXPHEADERCODE 306
#define TK_MODHEADERCODE 307
#define TK_TYPEHEADERCODE 308
#define TK_MODULE 309
#define TK_COMPOMODULE 310
#define TK_CLASS 311
#define TK_STRUCT 312
#define TK_PUBLIC 313
#define TK_PROTECTED 314
#define TK_PRIVATE 315
#define TK_SIGNALS 316
#define TK_SIGNAL_METHOD 317
#define TK_SLOTS 318
#define TK_SLOT_METHOD 319
#define TK_BOOL 320
#define TK_SHORT 321
#define TK_INT 322
#define TK_LONG 323
#define TK_FLOAT 324
#define TK_DOUBLE 325
#define TK_CHAR 326
#define TK_WCHAR_T 327
#define TK_VOID 328
#define TK_PYOBJECT 329
#define TK_PYTUPLE 330
#define TK_PYLIST 331
#define TK_PYDICT 332
#define TK_PYCALLABLE 333
#define TK_PYSLICE 334
#define TK_PYTYPE 335
#define TK_PYBUFFER 336
#define TK_VIRTUAL 337
#define TK_ENUM 338
#define TK_SIGNED 339
#define TK_UNSIGNED 340
#define TK_SCOPE 341
#define TK_LOGICAL_OR 342
#define TK_CONST 343
#define TK_STATIC 344
#define TK_PYSSIZET 345
#define TK_SIZET 346
#define TK_NUMBER_VALUE 347
#define TK_REAL_VALUE 348
#define TK_TYPEDEF 349
#define TK_NAMESPACE 350
#define TK_TIMELINE 351
#define TK_PLATFORMS 352
#define TK_FEATURE 353
#define TK_LICENSE 354
#define TK_QCHAR_VALUE 355
#define TK_TRUE_VALUE 356
#define TK_FALSE_VALUE 357
#define TK_NULL_VALUE 358
#define TK_OPERATOR 359
#define TK_THROW 360
#define TK_EXCEPTION 361
#define TK_RAISECODE 362
#define TK_EXPLICIT 363
#define TK_TEMPLATE 364
#define TK_FINAL 365
#define TK_ELLIPSIS 366
#define TK_DEFMETATYPE 367
#define TK_DEFSUPERTYPE 368
#define TK_PROPERTY 369
#define TK_HIDE_NS 370
#define TK_FORMAT 371
#define TK_GET 372
#define TK_ID 373
#define TK_KWARGS 374
#define TK_LANGUAGE 375
#define TK_LICENSEE 376
#define TK_NAME 377
#define TK_OPTIONAL 378
#define TK_ORDER 379
#define TK_REMOVELEADING 380
#define TK_SET 381
#define TK_SIGNATURE 382
#define TK_TIMESTAMP 383
#define TK_TYPE 384
#define TK_USEARGNAMES 385
#define TK_USELIMITEDAPI 386
#define TK_ALLRAISEPYEXC 387
#define TK_CALLSUPERINIT 388
#define TK_DEFERRORHANDLER 389
#define TK_VERSION 390




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 199 "/Users/phil/hg/sip/code_generator/metasrc/parser.y"
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
#line 362 "/Users/phil/hg/sip/code_generator/parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

