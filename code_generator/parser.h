/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_HOME_PHIL_HG_SIP_CODE_GENERATOR_PARSER_H_INCLUDED
# define YY_YY_HOME_PHIL_HG_SIP_CODE_GENERATOR_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 202 "/home/phil/hg/sip/code_generator/metasrc/parser.y" /* yacc.c:1909  */

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

#line 370 "/home/phil/hg/sip/code_generator/parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_PHIL_HG_SIP_CODE_GENERATOR_PARSER_H_INCLUDED  */
