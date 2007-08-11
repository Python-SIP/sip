/*
 * The SIP parser.
 *
 * @BS_LICENSE@
 */

%{
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sip.h"


#define MAX_NESTED_IF       10
#define MAX_NESTED_SCOPE    10

#define inMainModule()      (currentSpec->module == currentModule || currentModule->container != NULL)


static sipSpec *currentSpec;            /* The current spec being parsed. */
static stringList *neededQualifiers;    /* The list of required qualifiers. */
static stringList *excludedQualifiers;  /* The list of excluded qualifiers. */
static moduleDef *currentModule;        /* The current module being parsed. */
static mappedTypeDef *currentMappedType;    /* The current mapped type. */
static enumDef *currentEnum;            /* The current enum being parsed. */
static int sectionFlags;                /* The current section flags. */
static int currentOverIsVirt;           /* Set if the overload is virtual. */
static int currentCtorIsExplicit;       /* Set if the ctor is explicit. */
static int currentIsStatic;             /* Set if the current is static. */
static char *previousFile;              /* The file just parsed. */
static parserContext currentContext;    /* The current context. */
static int skipStackPtr;                /* The skip stack pointer. */
static int skipStack[MAX_NESTED_IF];    /* Stack of skip flags. */
static classDef *scopeStack[MAX_NESTED_SCOPE];  /* The scope stack. */
static int sectFlagsStack[MAX_NESTED_SCOPE];    /* The section flags stack. */
static int currentScopeIdx;             /* The scope stack index. */
static int currentTimelineOrder;        /* The current timeline order. */


static char *getPythonName(optFlags *optflgs, char *cname);
static nameDef *cacheName(sipSpec *,char *);
static classDef *findClass(sipSpec *,ifaceFileType,scopedNameDef *);
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff);
static classDef *newClass(sipSpec *,ifaceFileType,scopedNameDef *);
static void finishClass(sipSpec *,moduleDef *,classDef *,optFlags *);
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new);
static mappedTypeDef *newMappedType(sipSpec *,argDef *);
static enumDef *newEnum(sipSpec *,moduleDef *,char *,optFlags *,int);
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod, classDef *scope, scopedNameDef *fqname, classTmplDef *tcd, templateDef *td);
static void newTypedef(sipSpec *,moduleDef *,char *,argDef *);
static void newVar(sipSpec *,moduleDef *,char *,int,argDef *,optFlags *,
           codeBlock *,codeBlock *,codeBlock *);
static void newCtor(char *,int,signatureDef *,optFlags *,codeBlock *,
            throwArgs *,signatureDef *,int);
static void newFunction(sipSpec *,moduleDef *,int,int,int,char *,
            signatureDef *,int,int,optFlags *,codeBlock *,
            codeBlock *,throwArgs *,signatureDef *);
static optFlag *findOptFlag(optFlags *,char *,flagType);
static memberDef *findFunction(sipSpec *,moduleDef *,classDef *,nameDef *,int,
                   int);
static void checkAttributes(sipSpec *, moduleDef *, classDef *, char *, int);
static void newModule(FILE *fp, char *filename);
static moduleDef *allocModule();
static void parseFile(FILE *fp, char *name, moduleDef *prevmod, int optional);
static void handleEOF(void);
static void handleEOM(void);
static qualDef *findQualifier(char *);
static scopedNameDef *text2scopedName(char *);
static scopedNameDef *scopeScopedName(scopedNameDef *name);
static void pushScope(classDef *);
static void popScope(void);
static classDef *currentScope(void);
static void newQualifier(moduleDef *,int,int,char *,qualType);
static void newImport(char *filename);
static void usedInMainModule(sipSpec *,ifaceFileDef *);
static int timePeriod(char *,char *);
static int platOrFeature(char *,int);
static int isNeeded(qualDef *);
static int notSkipping(void);
static void getHooks(optFlags *,char **,char **);
static int getReleaseGIL(optFlags *);
static int getHoldGIL(optFlags *);
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd, templateDef *td, classDef *ncd);
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td, classDef *ncd);
static int search_back(const char *end, const char *start, const char *target);
static char *getType(scopedNameDef *ename, argDef *ad);
static char *scopedNameToString(scopedNameDef *name);
static void addUsedFromCode(sipSpec *pt, ifaceFileList **used, const char *sname);
static int sameName(scopedNameDef *snd, const char *sname);
static int optFind(sipSpec *pt, const char *opt);
static void setModuleName(moduleDef *mod, const char *fullname);
static int foundInScope(scopedNameDef *fq_name, scopedNameDef *rel_name);
%}

%union {
    char            qchar;
    char            *text;
    long            number;
    double          real;
    argDef          memArg;
    signatureDef    signature;
    signatureDef    *optsignature;
    throwArgs       *throwlist;
    codeBlock       *codeb;
    valueDef        value;
    valueDef        *valp;
    optFlags        optflags;
    optFlag         flag;
    scopedNameDef   *scpvalp;
    fcallDef        fcall;
    int             boolean;
    exceptionDef    exceptionbase;
    classDef        *klass;
}

%token          TK_OPTIONS
%token          TK_NOEMITTERS
%token          TK_DOC
%token          TK_EXPORTEDDOC
%token          TK_MAKEFILE
%token          TK_ACCESSCODE
%token          TK_GETCODE
%token          TK_SETCODE
%token          TK_PREINITCODE
%token          TK_POSTINITCODE
%token          TK_UNITCODE
%token          TK_MODCODE
%token          TK_TYPECODE
%token          TK_PREPYCODE
%token          TK_COPYING
%token          TK_MAPPEDTYPE
%token <codeb>  TK_CODELINE
%token          TK_IF
%token          TK_END
%token <text>   TK_NAME
%token <text>   TK_PATHNAME
%token <text>   TK_STRING
%token          TK_VIRTUALCATCHERCODE
%token          TK_TRAVERSECODE
%token          TK_CLEARCODE
%token          TK_READBUFFERCODE
%token          TK_WRITEBUFFERCODE
%token          TK_SEGCOUNTCODE
%token          TK_CHARBUFFERCODE
%token          TK_PICKLECODE
%token          TK_METHODCODE
%token          TK_FROMTYPE
%token          TK_TOTYPE
%token          TK_TOSUBCLASS
%token          TK_INCLUDE
%token          TK_OPTINCLUDE
%token          TK_IMPORT
%token          TK_EXPHEADERCODE
%token          TK_MODHEADERCODE
%token          TK_TYPEHEADERCODE
%token          TK_MODULE
%token          TK_CMODULE
%token          TK_CONSMODULE
%token          TK_COMPOMODULE
%token          TK_CLASS
%token          TK_STRUCT
%token          TK_PUBLIC
%token          TK_PROTECTED
%token          TK_PRIVATE
%token          TK_SIGNALS
%token          TK_SLOTS
%token          TK_BOOL
%token          TK_SHORT
%token          TK_INT
%token          TK_LONG
%token          TK_FLOAT
%token          TK_DOUBLE
%token          TK_CHAR
%token          TK_WCHAR_T
%token          TK_VOID
%token          TK_PYOBJECT
%token          TK_PYTUPLE
%token          TK_PYLIST
%token          TK_PYDICT
%token          TK_PYCALLABLE
%token          TK_PYSLICE
%token          TK_PYTYPE
%token          TK_VIRTUAL
%token          TK_ENUM
%token          TK_SIGNED
%token          TK_UNSIGNED
%token          TK_SCOPE
%token          TK_LOGICAL_OR
%token          TK_CONST
%token          TK_STATIC
%token          TK_SIPSIGNAL
%token          TK_SIPSLOT
%token          TK_SIPANYSLOT
%token          TK_SIPRXCON
%token          TK_SIPRXDIS
%token          TK_SIPSLOTCON
%token          TK_SIPSLOTDIS
%token <number> TK_NUMBER
%token <real>   TK_REAL
%token          TK_TYPEDEF
%token          TK_NAMESPACE
%token          TK_TIMELINE
%token          TK_PLATFORMS
%token          TK_FEATURE
%token          TK_LICENSE
%token <qchar>  TK_QCHAR
%token          TK_TRUE
%token          TK_FALSE
%token          TK_NULL
%token          TK_OPERATOR
%token          TK_THROW
%token          TK_QOBJECT
%token          TK_EXCEPTION
%token          TK_RAISECODE
%token          TK_EXPLICIT
%token          TK_TEMPLATE
%token          TK_ELLIPSIS

%type <memArg>          argvalue
%type <memArg>          argtype
%type <memArg>          cpptype
%type <memArg>          basetype
%type <signature>       template
%type <signature>       arglist
%type <signature>       rawarglist
%type <signature>       cpptypelist
%type <optsignature>    optsig
%type <optsignature>    optctorsig
%type <throwlist>       optexceptions
%type <throwlist>       exceptionlist
%type <number>          optslot
%type <number>          optref
%type <number>          optconst
%type <number>          optvirtual
%type <number>          optabstract
%type <number>          deref
%type <number>          optnumber
%type <value>           simplevalue
%type <valp>            value
%type <valp>            expr
%type <valp>            optassign
%type <codeb>           optaccesscode
%type <codeb>           optgetcode
%type <codeb>           optsetcode
%type <codeb>           exphdrcode
%type <codeb>           modhdrcode
%type <codeb>           typehdrcode
%type <codeb>           opttypehdrcode
%type <codeb>           travcode
%type <codeb>           clearcode
%type <codeb>           readbufcode
%type <codeb>           writebufcode
%type <codeb>           segcountcode
%type <codeb>           charbufcode
%type <codeb>           picklecode
%type <codeb>           modcode
%type <codeb>           typecode
%type <codeb>           codeblock
%type <codeb>           codelines
%type <codeb>           virtualcatchercode
%type <codeb>           methodcode
%type <codeb>           raisecode
%type <text>            operatorname
%type <text>            optfilename
%type <text>            optname
%type <text>            modname
%type <optflags>        optflags
%type <optflags>        flaglist
%type <flag>            flag
%type <flag>            flagvalue
%type <qchar>           optunop
%type <qchar>           binop
%type <scpvalp>         scopepart
%type <scpvalp>         scopedname
%type <fcall>           exprlist
%type <boolean>         qualifiers
%type <boolean>         oredqualifiers
%type <boolean>         modlang
%type <boolean>         optclassbody
%type <exceptionbase>   baseexception
%type <klass>           class

%%

specification:  statement
    |   specification statement
    ;

statement:  {
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
    } modstatement
    ;

modstatement:   module
    |   consmodule
    |   compmodule
    |   options
    |   noemitters
    |   copying
    |   include
    |   optinclude
    |   import
    |   timeline
    |   platforms
    |   feature
    |   license
    |   exphdrcode {
            if (notSkipping())
                appendCodeBlock(&currentSpec->exphdrcode, $1);
        }
    |   modhdrcode {
            if (notSkipping())
                appendCodeBlock(&currentModule->hdrcode, $1);
        }
    |   modcode {
            if (notSkipping())
                appendCodeBlock(&currentModule->cppcode, $1);
        }
    |   preinitcode
    |   postinitcode
    |   unitcode
    |   prepycode
    |   doc
    |   exporteddoc
    |   makefile
    |   mappedtype
    |   mappedtypetmpl
    |   nsstatement
    ;

nsstatement:    ifstart
    |   ifend
    |   namespace
    |   struct
    |   class
    |   classtmpl
    |   exception
    |   typedef
    |   enum
    |   function
    |   variable
    |   typehdrcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope == NULL)
                    yyerror("%TypeHeaderCode can only be used in a namespace, class or mapped type");

                appendCodeBlock(&scope->iff->hdrcode, $1);
            }
        }
    ;

options:    TK_OPTIONS '(' optionlist ')'
    ;

optionlist: TK_NAME {
            appendString(&currentSpec->options, $1);
        }
    |   optionlist ',' TK_NAME {
            appendString(&currentSpec->options, $3);
        }
    ;

noemitters: TK_NOEMITTERS {
            if (notSkipping())
            {
                yywarning("%SIPNoEmitters is deprecated, please use %SIPOptions instead");
                appendString(&currentSpec->options, "QtNoEmitters");
            }
        }
    ;

exception:  TK_EXCEPTION scopedname baseexception optflags '{' opttypehdrcode raisecode '}' ';' {
            if (notSkipping())
            {
                exceptionDef *xd;
                char *pyname;

                if (currentSpec->genc)
                    yyerror("%Exception not allowed in a C module");

                pyname = getPythonName(&$4, scopedNameTail($2));

                checkAttributes(currentSpec, currentModule, NULL, pyname, FALSE);

                xd = findException(currentSpec, $2, TRUE);

                if (xd->cd != NULL)
                    yyerror("%Exception name has already been seen as a class name - it must be defined before being used");

                if (xd->iff->module != NULL)
                    yyerror("The %Exception has already been defined");

                /* Complete the definition. */
                xd->iff->module = currentModule;
                xd->iff->hdrcode = $6;
                xd->pyname = pyname;
                xd->bibase = $3.bibase;
                xd->base = $3.base;
                xd->raisecode = $7;

                if (xd->bibase != NULL || xd->base != NULL)
                    xd->exceptionnr = currentModule->nrexceptions++;
            }
        }
    ;

baseexception:  {
            $$.bibase = NULL;
            $$.base = NULL;
        }
    |   '(' scopedname ')' {
            exceptionDef *xd;

            $$.bibase = NULL;
            $$.base = NULL;

            /* See if it is a defined exception. */
            for (xd = currentSpec->exceptions; xd != NULL; xd = xd->next)
                if (sameScopedName(xd->iff->fqcname, $2))
                {
                    $$.base = xd;
                    break;
                }

            if (xd == NULL && $2->next == NULL && strncmp($2->name, "SIP_", 4) == 0)
            {
                /* See if it is a builtin exception. */

                static char *builtins[] = {
                    "Exception",
                    "StopIteration",
                    "StandardError",
                    "ArithmeticError",
                    "LookupError",
                    "AssertionError",
                    "AttributeError",
                    "EOFError",
                    "FloatingPointError",
                    "EnvironmentError",
                    "IOError",
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
                    "WindowsError",
                    "VMSError",
                    NULL
                };

                char **cp;

                for (cp = builtins; *cp != NULL; ++cp)
                    if (strcmp($2->name + 4, *cp) == 0)
                    {
                        $$.bibase = *cp;
                        break;
                    }
            }

            if ($$.bibase == NULL && $$.base == NULL)
                yyerror("Unknown exception base type");
        }
    ;

raisecode:  TK_RAISECODE codeblock {
            $$ = $2;
        }
    ;

mappedtype: TK_MAPPEDTYPE basetype {
            if (notSkipping())
                currentMappedType = newMappedType(currentSpec, &$2);
        } mtdefinition
    ;

mappedtypetmpl: template TK_MAPPEDTYPE basetype {
            int a;

            if (currentSpec->genc)
                yyerror("%MappedType templates not allowed in a C module");

            /* Check the template arguments are basic types or simple names. */
            for (a = 0; a < $1.nrArgs; ++a)
                if ($1.args[a].atype == defined_type && $1.args[a].u.snd->next != NULL)
                    yyerror("%MappedType template arguments must be simple names");

            if ($3.atype != template_type)
                yyerror("%MappedType template must map a template type");

            if (notSkipping())
            {
                mappedTypeTmplDef *mtt;
                ifaceFileDef *iff;

                /* Check a template hasn't already been provided. */
                for (mtt = currentSpec->mappedtypetemplates; mtt != NULL; mtt = mtt->next)
                    if (sameScopedName(mtt->mt->type.u.td->fqname, $3.u.td->fqname) && sameTemplateSignature(&mtt->mt->type.u.td->types, &$3.u.td->types, TRUE))
                        yyerror("%MappedType template for this type has already been defined");

                $3.nrderefs = 0;
                $3.argflags = 0;

                mtt = sipMalloc(sizeof (mappedTypeTmplDef));

                mtt->sig = $1;
                mtt->mt = allocMappedType(&$3);
                mtt->next = currentSpec->mappedtypetemplates;

                currentSpec->mappedtypetemplates = mtt;

                currentMappedType = mtt->mt;

                /* Create a dummy interface file. */
                iff = sipMalloc(sizeof (ifaceFileDef));
                iff->hdrcode = NULL;
                mtt->mt->iff = iff;
            }
        } mtdefinition
    ;

mtdefinition:   '{' mtbody '}' ';' {
            if (notSkipping())
            {
                if (currentMappedType->convfromcode == NULL)
                    yyerror("%MappedType must have a %ConvertFromTypeCode directive");

                if (currentMappedType->convtocode == NULL)
                    yyerror("%MappedType must have a %ConvertToTypeCode directive");

                currentMappedType = NULL;
            }
        }
    ;

mtbody:     mtline
    |   mtbody mtline
    ;

mtline:     typehdrcode {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->iff->hdrcode, $1);
        }
    |   TK_FROMTYPE codeblock {
            if (notSkipping())
            {
                if (currentMappedType -> convfromcode != NULL)
                    yyerror("%MappedType has more than one %ConvertFromTypeCode directive");

                currentMappedType -> convfromcode = $2;
            }
        }
    |   TK_TOTYPE codeblock {
            if (notSkipping())
            {
                if (currentMappedType -> convtocode != NULL)
                    yyerror("%MappedType has more than one %ConvertToTypeCode directive");

                currentMappedType -> convtocode = $2;
            }
        }
    ;

namespace:  TK_NAMESPACE TK_NAME {
            if (currentSpec -> genc)
                yyerror("namespace definition not allowed in a C module");

            if (notSkipping())
            {
                classDef *ns;

                ns = newClass(currentSpec,namespace_iface,text2scopedName($2));

                pushScope(ns);

                sectionFlags = 0;
            }
        } '{' nsbody '}' ';' {
            if (notSkipping())
            {
                if (inMainModule())
                {
                    classDef *ns = currentScope();

                    if (!isUsedName(ns->iff->name))
                    {
                        varDef *vd;

                        for (vd = currentSpec->vars; vd != NULL; vd = vd->next)
                            if (vd->ecd == ns)
                            {
                                setIsUsedName(ns->iff->name);
                                break;
                            }
                    }
                }

                popScope();
            }
        }
    ;

nsbody:     nsstatement
    |   nsbody nsstatement
    ;

platforms:  TK_PLATFORMS {
            qualDef *qd;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == platform_qualifier)
                    yyerror("%Platforms has already been defined for this module");
        }
        '{' platformlist '}' {
            qualDef *qd;
            int nrneeded;

            /*
             * Check that exactly one platform in the set was
             * requested.
             */

            nrneeded = 0;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == platform_qualifier && isNeeded(qd))
                    ++nrneeded;

            if (nrneeded > 1)
                yyerror("No more than one of these %Platforms must be specified with the -t flag");
        }
    ;

platformlist:   platform
    |   platformlist platform
    ;

platform:   TK_NAME {
            newQualifier(currentModule,-1,-1,$1,platform_qualifier);
        }
    ;

feature:    TK_FEATURE TK_NAME {
            newQualifier(currentModule,-1,-1,$2,feature_qualifier);
        }
    ;

timeline:   TK_TIMELINE {
            currentTimelineOrder = 0;
        }
        '{' qualifierlist '}' {
            qualDef *qd;
            int nrneeded;

            /*
             * Check that exactly one time slot in the set was
             * requested.
             */

            nrneeded = 0;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == time_qualifier && isNeeded(qd))
                    ++nrneeded;

            if (nrneeded > 1)
                yyerror("At most one of this %Timeline must be specified with the -t flag");

            currentModule -> nrtimelines++;
        }
    ;

qualifierlist:  qualifiername
    |   qualifierlist qualifiername
    ;

qualifiername:  TK_NAME {
            newQualifier(currentModule,currentModule -> nrtimelines,currentTimelineOrder++,$1,time_qualifier);
        }
    ;

ifstart:    TK_IF '(' qualifiers ')' {
            if (skipStackPtr >= MAX_NESTED_IF)
                yyerror("Internal error: increase the value of MAX_NESTED_IF");

            /* Nested %Ifs are implicit logical ands. */

            if (skipStackPtr > 0)
                $3 = ($3 && skipStack[skipStackPtr - 1]);

            skipStack[skipStackPtr++] = $3;
        }
    ;

oredqualifiers: TK_NAME {
            $$ = platOrFeature($1,FALSE);
        }
    |   '!' TK_NAME {
            $$ = platOrFeature($2,TRUE);
        }
    |   oredqualifiers TK_LOGICAL_OR TK_NAME {
            $$ = (platOrFeature($3,FALSE) || $1);
        }
    |   oredqualifiers TK_LOGICAL_OR '!' TK_NAME {
            $$ = (platOrFeature($4,TRUE) || $1);
        }
    ;

qualifiers: oredqualifiers
    |   optname '-' optname {
            $$ = timePeriod($1,$3);
        }
    ;

ifend:      TK_END {
            if (skipStackPtr-- <= 0)
                yyerror("Too many %End directives");
        }
    ;

license:    TK_LICENSE optflags {
            optFlag *of;

            if ($2.nrFlags == 0)
                yyerror("%License details not specified");

            if ((of = findOptFlag(&$2,"Type",string_flag)) == NULL)
                yyerror("%License type not specified");

            currentModule -> license = sipMalloc(sizeof (licenseDef));

            currentModule -> license -> type = of -> fvalue.sval;

            currentModule -> license -> licensee = 
                ((of = findOptFlag(&$2,"Licensee",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;

            currentModule -> license -> timestamp = 
                ((of = findOptFlag(&$2,"Timestamp",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;

            currentModule -> license -> sig = 
                ((of = findOptFlag(&$2,"Signature",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;
        }
    ;

consmodule: TK_CONSMODULE modname {
            /* Make sure this is the first mention of a module. */
            if (currentSpec->module != currentModule)
                yyerror("A %ConsolidatedModule cannot be %Imported");

            if (currentModule->fullname != NULL)
                yyerror("%ConsolidatedModule must appear before any %Module or %CModule directive");

            setModuleName(currentModule, $2);
            setIsConsolidated(currentModule);
        }
    ;

compmodule: TK_COMPOMODULE modname {
            /* Make sure this is the first mention of a module. */
            if (currentSpec->module != currentModule)
                yyerror("A %CompositeModule cannot be %Imported");

            if (currentModule->fullname != NULL)
                yyerror("%CompositeModule must appear before any %Module or %CModule directive");

            setModuleName(currentModule, $2);
            setIsComposite(currentModule);
        }
    ;

module:     modlang modname optnumber {
            /* Check the module hasn't already been defined. */

            moduleDef *mod;

            for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
                if (mod->fullname != NULL && strcmp(mod->fullname, $2) == 0)
                    yyerror("Module is already defined");

            /*
             * If we are in a container module then create a component module
             * and make it current.
             */
            if (isContainer(currentModule) || currentModule->container != NULL)
            {
                mod = allocModule();

                mod->file = currentContext.filename;
                mod->container = (isContainer(currentModule) ? currentModule : currentModule->container);

                currentModule = mod;
            }

            setModuleName(currentModule, $2);
            currentModule->version = $3;

            if (currentSpec->genc < 0)
                currentSpec->genc = $1;
            else if (currentSpec->genc != $1)
                yyerror("Cannot mix C and C++ modules");
        }
    ;

modlang:    TK_MODULE {
            $$ = FALSE;
        }
    |   TK_CMODULE {
            $$ = TRUE;
        }
    ;

modname:    TK_NAME
    |   TK_PATHNAME {
            /*
             * The grammar design is a bit broken and this is the
             * easiest way to allow periods in module names.
             */

            char *cp;

            for (cp = $1; *cp != '\0'; ++cp)
                if (*cp != '.' && *cp != '_' && !isalnum(*cp))
                    yyerror("Invalid character in module name");

            $$ = $1;
        }
    ;

optnumber:  {
            $$ = -1;
        }
    |   TK_NUMBER
    ;

include:    TK_INCLUDE TK_PATHNAME {
            parseFile(NULL, $2, NULL, FALSE);
        }
    ;

optinclude: TK_OPTINCLUDE TK_PATHNAME {
            parseFile(NULL, $2, NULL, TRUE);
        }
    ;

import:     TK_IMPORT TK_PATHNAME {
            newImport($2);
        }
    ;

optaccesscode:  {
            $$ = NULL;
        }
    |   TK_ACCESSCODE codeblock {
            $$ = $2;
        }
    ;

optgetcode: {
            $$ = NULL;
        }
    |   TK_GETCODE codeblock {
            $$ = $2;
        }
    ;

optsetcode: {
            $$ = NULL;
        }
    |   TK_SETCODE codeblock {
            $$ = $2;
        }
    ;

copying:    TK_COPYING codeblock {
            appendCodeBlock(&currentModule->copying, $2);
        }
    ;

exphdrcode: TK_EXPHEADERCODE codeblock {
            $$ = $2;
        }
    ;

modhdrcode: TK_MODHEADERCODE codeblock {
            $$ = $2;
        }
    ;

typehdrcode:    TK_TYPEHEADERCODE codeblock {
            $$ = $2;
        }
    ;

opttypehdrcode: {
            $$ = NULL;
        }
    |   typehdrcode
    ;

travcode:   TK_TRAVERSECODE codeblock {
            $$ = $2;
        }
    ;

clearcode:  TK_CLEARCODE codeblock {
            $$ = $2;
        }
    ;

readbufcode:    TK_READBUFFERCODE codeblock {
            $$ = $2;
        }
    ;

writebufcode:   TK_WRITEBUFFERCODE codeblock {
            $$ = $2;
        }
    ;

segcountcode:   TK_SEGCOUNTCODE codeblock {
            $$ = $2;
        }
    ;

charbufcode:    TK_CHARBUFFERCODE codeblock {
            $$ = $2;
        }
    ;

picklecode: TK_PICKLECODE codeblock {
            $$ = $2;
        }
    ;

modcode:    TK_MODCODE codeblock {
            $$ = $2;
        }
    ;

typecode:   TK_TYPECODE codeblock {
            $$ = $2;
        }
    ;

preinitcode:    TK_PREINITCODE codeblock {
            if (notSkipping())
                appendCodeBlock(&currentModule->preinitcode, $2);
        }
    ;

postinitcode:   TK_POSTINITCODE codeblock {
            if (notSkipping())
                appendCodeBlock(&currentModule->postinitcode, $2);
        }
    ;

unitcode:   TK_UNITCODE codeblock {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitcode, $2);
        }
    ;

prepycode:  TK_PREPYCODE codeblock {
            /*
             * This is a no-op and is retained for compatibility
             * until the last use of it (by SIP v3) can be removed
             * from PyQt.
             */
        }
    ;

doc:        TK_DOC codeblock {
            if (inMainModule())
                appendCodeBlock(&currentSpec -> docs,$2);
        }
    ;

exporteddoc:    TK_EXPORTEDDOC codeblock {
            appendCodeBlock(&currentSpec -> docs,$2);
        }
    ;

makefile:   TK_MAKEFILE TK_PATHNAME optfilename codeblock {
            if (inMainModule())
                yywarning("%Makefile is ignored, please use the -b flag instead");
        }
    ;

codeblock:  codelines TK_END
    ;

codelines:  TK_CODELINE
    |   codelines TK_CODELINE {
            $$ = $1;

            append(&$$->frag, $2->frag);

            free($2->frag);
            free($2->filename);
            free($2);
        }
    ;

enum:       TK_ENUM optname optflags {
            if (notSkipping())
            {
                if (sectionFlags != 0 && (sectionFlags & ~(SECT_IS_PUBLIC | SECT_IS_PROT)) != 0)
                    yyerror("Class enums must be in the public or protected sections");

                currentEnum = newEnum(currentSpec,currentModule,$2,&$3,sectionFlags);
            }
        } '{' optenumbody '}' ';'
    ;

optfilename:    {
            $$ = NULL;
        }
    |   TK_PATHNAME {
            $$ = $1;
        }
    ;

optname:    {
            $$ = NULL;
        }
    |   TK_NAME {
            $$ = $1;
        }
    ;

optenumbody:
    |   enumbody
    ;

enumbody:   enumline
    |   enumbody enumline
    ;

enumline:   ifstart
    |   ifend
    |   TK_NAME optenumassign optflags optcomma {
            if (notSkipping())
            {
                /*
                 * Note that we don't use the assigned value.
                 * This is a hangover from when enums where
                 * generated in Python.  We can remove it when
                 * we have got around to updating all the .sip
                 * files.
                 */
                enumMemberDef *emd, **tail;

                emd = sipMalloc(sizeof (enumMemberDef));

                emd -> pyname = cacheName(currentSpec, getPythonName(&$3, $1));
                emd -> cname = $1;
                emd -> ed = currentEnum;
                emd -> next = NULL;

                checkAttributes(currentSpec, currentModule, emd->ed->ecd, emd->pyname->text, FALSE);

                /* Append to preserve the order. */
                for (tail = &currentEnum->members; *tail != NULL; tail = &(*tail)->next)
                    ;

                *tail = emd;

                if (inMainModule())
                    setIsUsedName(emd -> pyname);
            }
        }
    ;

optcomma:
    |   ','
    ;

optenumassign:
    |   '=' value
    ;

optassign:  {
            $$ = NULL;
        }
    |   '=' expr {
            $$ = $2;
        }
    ;

expr:       value
    |   expr binop value {
            valueDef *vd;
 
            if ($1 -> vtype == string_value || $3 -> vtype == string_value)
                yyerror("Invalid binary operator for string");
 
            /* Find the last value in the existing expression. */
 
            for (vd = $1; vd -> next != NULL; vd = vd -> next)
                ;
 
            vd -> vbinop = $2;
            vd -> next = $3;

            $$ = $1;
        }
    ;

binop:      '-' {
            $$ = '-';
        }
    |   '+' {
            $$ = '+';
        }
    |   '*' {
            $$ = '*';
        }
    |   '/' {
            $$ = '/';
        }
    |   '&' {
            $$ = '&';
        }
    |   '|' {
            $$ = '|';
        }
    ;

optunop:    {
            $$ = '\0';
        }
    |   '!' {
            $$ = '!';
        }
    |   '~' {
            $$ = '~';
        }
    |   '-' {
            $$ = '-';
        }
    |   '+' {
            $$ = '+';
        }
    ;

value:      optunop simplevalue {
            if ($1 != '\0' && $2.vtype == string_value)
                yyerror("Invalid unary operator for string");
 
            /*
             * Convert the value to a simple expression on the
             * heap.
             */
 
            $$ = sipMalloc(sizeof (valueDef));
 
            *$$ = $2;
            $$ -> vunop = $1;
            $$ -> vbinop = '\0';
            $$ -> next = NULL;
        }
    ;

scopedname: scopepart
    |   scopedname TK_SCOPE scopepart {
            if (currentSpec -> genc)
                yyerror("Scoped names are not allowed in a C module");

            appendScopedName(&$1,$3);
        }
    ;

scopepart:  TK_NAME {
            $$ = text2scopePart($1);
        }
    ;

simplevalue:    scopedname {
            /*
             * We let the C++ compiler decide if the value is a
             * valid one - no point in building a full C++ parser
             * here.
             */

            $$.vtype = scoped_value;
            $$.u.vscp = $1;
        }
    |   basetype '(' exprlist ')' {
            fcallDef *fcd;

            fcd = sipMalloc(sizeof (fcallDef));
            *fcd = $3;
            fcd -> type = $1;

            $$.vtype = fcall_value;
            $$.u.fcd = fcd;
        }
    |   TK_REAL {
            $$.vtype = real_value;
            $$.u.vreal = $1;
        }
    |   TK_NUMBER {
            $$.vtype = numeric_value;
            $$.u.vnum = $1;
        }
    |   TK_TRUE {
            $$.vtype = numeric_value;
            $$.u.vnum = 1;
        }
    |   TK_FALSE {
            $$.vtype = numeric_value;
            $$.u.vnum = 0;
        }
    |   TK_NULL {
            $$.vtype = numeric_value;
            $$.u.vnum = 0;
        }
    |   TK_STRING {
            $$.vtype = string_value;
            $$.u.vstr = $1;
        }
    |   TK_QCHAR {
            $$.vtype = qchar_value;
            $$.u.vqchar = $1;
        }
    ;

exprlist:   {
            /* No values. */

            $$.nrArgs = 0;
        }
    |   expr {
            /* The single or first expression. */

            $$.args[0] = $1;
            $$.nrArgs = 1;
        }
    |   exprlist ',' expr {
            /* Check that it wasn't ...(,expression...). */

            if ($$.nrArgs == 0)
                yyerror("First argument to function call is missing");

            /* Check there is room. */

            if ($1.nrArgs == MAX_NR_ARGS)
                yyerror("Too many arguments to function call");

            $$ = $1;

            $$.args[$$.nrArgs] = $3;
            $$.nrArgs++;
        }
    ;

typedef:    TK_TYPEDEF cpptype TK_NAME ';' {
            if (notSkipping())
                newTypedef(currentSpec,currentModule,$3,&$2);
        }
    |   TK_TYPEDEF cpptype '(' deref TK_NAME ')' '(' cpptypelist ')' ';' {
            if (notSkipping())
            {
                argDef ftype;
                signatureDef *sig;

                /* Create the full signature on the heap. */
                sig = sipMalloc(sizeof (signatureDef));
                *sig = $8;
                sig -> result = $2;

                /* Create the full type. */
                ftype.atype = function_type;
                ftype.argflags = 0;
                ftype.nrderefs = $4;
                ftype.defval = NULL;
                ftype.u.sa = sig;

                newTypedef(currentSpec,currentModule,$5,&ftype);
            }
        }
    ;

struct:     TK_STRUCT TK_NAME {
            if (notSkipping())
            {
                classDef *cd;

                cd = newClass(currentSpec,class_iface,text2scopedName($2));

                pushScope(cd);

                sectionFlags = SECT_IS_PUBLIC;
            }
        } optflags '{' classbody '}' ';' {
            if (notSkipping())
            {
                finishClass(currentSpec, currentModule, currentScope(), &$4);
                popScope();
            }
        }
    ;

classtmpl:  template class {
            if (currentSpec->genc)
                yyerror("Class templates not allowed in a C module");

            if (notSkipping())
            {
                classTmplDef *tcd;

                /*
                 * Make sure there is room for the extra class
                 * name argument.
                 */
                if ($1.nrArgs == MAX_NR_ARGS)
                    yyerror("Internal error - increase the value of MAX_NR_ARGS");

                tcd = sipMalloc(sizeof (classTmplDef));
                tcd->sig = $1;
                tcd->cd = $2;
                tcd->next = currentSpec->classtemplates;

                currentSpec->classtemplates = tcd;
            }
        }
    ;

template:   TK_TEMPLATE '<' cpptypelist '>' {
            $$ = $3;
        }
    ;

class:      TK_CLASS scopedname {
            if (currentSpec -> genc)
                yyerror("Class definition not allowed in a C module");

            if (notSkipping())
            {
                classDef *cd;

                cd = newClass(currentSpec, class_iface, scopeScopedName($2));

                pushScope(cd);

                sectionFlags = SECT_IS_PRIVATE;
            }
        } superclasses optflags optclassbody ';' {
            if (notSkipping())
            {
                classDef *cd = currentScope();

                /*
                 * See if the class was defined or just
                 * declared.
                 */
                if ($6)
                {
                    if ($2->next != NULL)
                        yyerror("A scoped name cannot be given in a class definition");

                }
                else if (cd->supers != NULL)
                    yyerror("Class has super-classes but no definition");
                else
                    setIsOpaque(cd);

                finishClass(currentSpec, currentModule, cd, &$5);
                popScope();

                /*
                 * Check that external classes have only been
                 * declared at the global scope.
                 */
                if (isExternal(cd) && currentScope() != NULL)
                    yyerror("External classes can only be declared in the global scope");

                $$ = cd;
            }
        }
    ;

superclasses:
    |   ':' superlist
    ;

superlist:  superclass
    |   superlist ',' superclass 
    ;

superclass: scopedname {
            if (notSkipping())
            {
                classDef *cd, *super;

                cd = currentScope();

                super = findClass(currentSpec,class_iface,$1);

                appendToClassList(&cd -> supers,super);
            }
        }
    ;

optclassbody:   {
            $$ = FALSE;
        }
    |   '{' classbody '}' {
            $$ = TRUE;
        }
    ;

classbody:  classline
    |   classbody classline
    ;

classline:  ifstart
    |   ifend
    |   namespace
    |   struct
    |   class
    |   exception
    |   typedef
    |   enum
    |   typecode {
            if (notSkipping())
                appendCodeBlock(&currentScope()->cppcode, $1);
        }
    |   typehdrcode {
            if (notSkipping())
                appendCodeBlock(&currentScope()->iff->hdrcode, $1);
        }
    |   travcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->travcode != NULL)
                    yyerror("%GCTraverseCode already given for class");

                scope->travcode = $1;
            }
        }
    |   clearcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->clearcode != NULL)
                    yyerror("%GCClearCode already given for class");

                scope->clearcode = $1;
            }
        }
    |   readbufcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->readbufcode != NULL)
                    yyerror("%BIGetReadBufferCode already given for class");

                scope->readbufcode = $1;
            }
        }
    |   writebufcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->writebufcode != NULL)
                    yyerror("%BIGetWriteBufferCode already given for class");

                scope->writebufcode = $1;
            }
        }
    |   segcountcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->segcountcode != NULL)
                    yyerror("%BIGetSegCountCode already given for class");

                scope->segcountcode = $1;
            }
        }
    |   charbufcode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->charbufcode != NULL)
                    yyerror("%BIGetCharBufferCode already given for class");

                scope->charbufcode = $1;
            }
        }
    |   picklecode {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->picklecode != NULL)
                    yyerror("%PickleCode already given for class");

                scope->picklecode = $1;
            }
        }
    |   ctor
    |   dtor
    |   varmember
    |   TK_TOSUBCLASS codeblock {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtosubcode != NULL)
                    yyerror("Class has more than one %ConvertToSubClassCode directive");

                scope->convtosubcode = $2;
            }
        }
    |   TK_TOTYPE codeblock {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtocode != NULL)
                    yyerror("Class has more than one %ConvertToTypeCode directive");

                scope->convtocode = $2;
            }
        }
    |   TK_PUBLIC optslot ':' {
            if (currentSpec -> genc)
                yyerror("public section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PUBLIC | $2;
        }
    |   TK_PROTECTED optslot ':' {
            if (currentSpec -> genc)
                yyerror("protected section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PROT | $2;
        }
    |   TK_PRIVATE optslot ':' {
            if (currentSpec -> genc)
                yyerror("private section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PRIVATE | $2;
        }
    |   TK_SIGNALS ':' {
            if (currentSpec -> genc)
                yyerror("signals section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_SIGNAL;
        }
    ;

optslot:    {
            $$ = 0;
        }
    |   TK_SLOTS {
            $$ = SECT_IS_SLOT;
        }
    ;

dtor:       optvirtual '~' TK_NAME '(' ')' optexceptions optabstract optflags ';' methodcode virtualcatchercode {
            /* Note that we allow non-virtual dtors in C modules. */

            if (notSkipping())
            {
                classDef *cd = currentScope();

                if (strcmp(classBaseName(cd),$3) != 0)
                    yyerror("Destructor doesn't have the same name as its class");

                if (isDtor(cd))
                    yyerror("Destructor has already been defined");

                if (currentSpec -> genc && $10 == NULL)
                    yyerror("Destructor in C modules must include %MethodCode");

                cd -> dealloccode = $10;
                cd -> dtorcode = $11;
                cd -> dtorexceptions = $6;
                cd -> classflags |= sectionFlags;

                if ($7)
                {
                    if (!$1)
                        yyerror("Abstract destructor must be virtual");

                    setIsAbstractClass(cd);
                }

                /*
                 * The class has a shadow if we have a virtual dtor or some
                 * dtor code.
                 */
                if ($1 || $11 != NULL)
                {
                    if (currentSpec -> genc)
                        yyerror("Virtual destructor or %VirtualCatcherCode not allowed in a C module");

                    setHasShadow(cd);
                }
            }
        }
    ;

ctor:       TK_EXPLICIT {currentCtorIsExplicit = TRUE;} simplector
    |   simplector
    ;

simplector: TK_NAME '(' arglist ')' optexceptions optflags optctorsig ';' methodcode {
            /* Note that we allow ctors in C modules. */

            if (notSkipping())
            {
                if (currentSpec -> genc)
                {
                    if ($9 == NULL && $3.nrArgs != 0)
                        yyerror("Constructors with arguments in C modules must include %MethodCode");

                    if (currentCtorIsExplicit)
                        yyerror("Explicit constructors not allowed in a C module");
                }

                if ((sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE)) == 0)
                    yyerror("Constructor must be in the public, private or protected sections");

                newCtor($1,sectionFlags,&$3,&$6,$9,$5,$7,currentCtorIsExplicit);
            }

            free($1);

            currentCtorIsExplicit = FALSE;
        }
    ;

optctorsig: {
            $$ = NULL;
        }
    |   '[' '(' arglist ')' ']' {
            $$ = sipMalloc(sizeof (signatureDef));

            *$$ = $3;
        }
    ;

optsig: {
            $$ = NULL;
        }
    |   '[' cpptype '(' arglist ')' ']' {
            $$ = sipMalloc(sizeof (signatureDef));

            *$$ = $4;
            $$ -> result = $2;
        }
    ;

optvirtual: {
            $$ = FALSE;
        }
    |   TK_VIRTUAL {
            $$ = TRUE;
        }
    ;

function:   cpptype TK_NAME '(' arglist ')' optconst optexceptions optabstract optflags optsig ';' methodcode virtualcatchercode {
            if (notSkipping())
            {
                if (sectionFlags != 0 && (sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE | SECT_IS_SLOT | SECT_IS_SIGNAL)) == 0)
                    yyerror("Class function must be in the public, private, protected, slot or signal sections");

                $4.result = $1;

                newFunction(currentSpec,currentModule,
                        sectionFlags,currentIsStatic,
                        currentOverIsVirt,
                        $2,&$4,$6,$8,&$9,$12,$13,$7,$10);
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    |   cpptype TK_OPERATOR operatorname '(' arglist ')' optconst optexceptions optabstract optflags optsig ';' methodcode virtualcatchercode {
            if (notSkipping())
            {
                classDef *cd = currentScope();

                /* Handle the unary '+' and '-' operators. */
                if ((cd != NULL && $5.nrArgs == 0) || (cd == NULL && $5.nrArgs == 1))
                {
                    if (strcmp($3, "__add__") == 0)
                        $3 = "__pos__";
                    else if (strcmp($3, "__sub__") == 0)
                        $3 = "__neg__";
                }

                $5.result = $1;

                newFunction(currentSpec,currentModule,
                        sectionFlags,currentIsStatic,
                        currentOverIsVirt,
                        $3,&$5,$7,$9,&$10,$13,$14,$8,$11);
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    |   TK_OPERATOR cpptype '(' arglist ')' optconst optexceptions optabstract optflags optsig ';' methodcode virtualcatchercode {
            if (notSkipping())
            {
                char *sname;
                classDef *scope = currentScope();

                if (scope == NULL || $4.nrArgs != 0)
                    yyerror("Operator casts must be specified in a class and have no arguments");

                switch ($2.atype)
                {
                case defined_type:
                    sname = NULL;
                    break;

                case bool_type:
                case cbool_type:
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
                    $4.result = $2;

                    newFunction(currentSpec, currentModule,
                            sectionFlags,
                            currentIsStatic,
                            currentOverIsVirt, sname,
                            &$4, $6, $8, &$9, $12, $13,
                            $7, $10);
                }
                else
                {
                    argList *al;

                    /* Check it doesn't already exist. */
                    for (al = scope->casts; al != NULL; al = al->next)
                        if (sameScopedName($2.u.snd, al->arg.u.snd))
                            yyerror("This operator cast has already been specified in this class");

                    al = sipMalloc(sizeof (argList));
                    al->arg = $2;
                    al->next = scope->casts;

                    scope->casts = al;
                }
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    ;

operatorname:   '+'     {$$ = "__add__";}
    |   '-'     {$$ = "__sub__";}
    |   '*'     {$$ = "__mul__";}
    |   '/'     {$$ = "__div__";}
    |   '%'     {$$ = "__mod__";}
    |   '&'     {$$ = "__and__";}
    |   '|'     {$$ = "__or__";}
    |   '^'     {$$ = "__xor__";}
    |   '<' '<'     {$$ = "__lshift__";}
    |   '>' '>'     {$$ = "__rshift__";}
    |   '+' '='     {$$ = "__iadd__";}
    |   '-' '='     {$$ = "__isub__";}
    |   '*' '='     {$$ = "__imul__";}
    |   '/' '='     {$$ = "__idiv__";}
    |   '%' '='     {$$ = "__imod__";}
    |   '&' '='     {$$ = "__iand__";}
    |   '|' '='     {$$ = "__ior__";}
    |   '^' '='     {$$ = "__ixor__";}
    |   '<' '<' '=' {$$ = "__ilshift__";}
    |   '>' '>' '=' {$$ = "__irshift__";}
    |   '~'     {$$ = "__invert__";}
    |   '(' ')'     {$$ = "__call__";}
    |   '[' ']'     {$$ = "__getitem__";}
    |   '<'     {$$ = "__lt__";}
    |   '<' '='     {$$ = "__le__";}
    |   '=' '='     {$$ = "__eq__";}
    |   '!' '='     {$$ = "__ne__";}
    |   '>'     {$$ = "__gt__";}
    |   '>' '='     {$$ = "__ge__";}
    ;

optconst:   {
            $$ = FALSE;
        }
    |   TK_CONST {
            $$ = TRUE;
        }
    ;

optabstract:    {
            $$ = 0;
        }
    |   '=' TK_NUMBER {
            if ($2 != 0)
                yyerror("Abstract virtual function '= 0' expected");

            $$ = TRUE;
        }
    ;

optflags:   {
            $$.nrFlags = 0;
        }
    |   '/' flaglist '/' {
            $$ = $2;
        }
    ;


flaglist:   flag {
            $$.flags[0] = $1;
            $$.nrFlags = 1;
        }
    |   flaglist ',' flag {
            /* Check there is room. */

            if ($1.nrFlags == MAX_NR_FLAGS)
                yyerror("Too many optional flags");

            $$ = $1;

            $$.flags[$$.nrFlags++] = $3;
        }
    ;

flag:       TK_NAME {
            $$.ftype = bool_flag;
            $$.fname = $1;
        }
    |   TK_NAME '=' flagvalue {
            $$ = $3;
            $$.fname = $1;
        }
    ;

flagvalue:  TK_NAME {
            $$.ftype = name_flag;
            $$.fvalue.sval = $1;
        }
    |   TK_STRING {
            $$.ftype = string_flag;
            $$.fvalue.sval = $1;
        }
    |   TK_NUMBER {
            $$.ftype = integer_flag;
            $$.fvalue.ival = $1;
        }
    ;

methodcode: {
            $$ = NULL;
        }
    |   TK_METHODCODE codeblock {
            $$ = $2;
        }
    ;

virtualcatchercode: {
            $$ = NULL;
        }
    |   TK_VIRTUALCATCHERCODE codeblock {
            $$ = $2;
        }
    ;

arglist:    rawarglist {
            int a, nrrxcon, nrrxdis, nrslotcon, nrslotdis, nrarray, nrarraysize;

            nrrxcon = nrrxdis = nrslotcon = nrslotdis = nrarray = nrarraysize = 0;

            for (a = 0; a < $1.nrArgs; ++a)
            {
                argDef *ad = &$1.args[a];

                switch (ad -> atype)
                {
                case rxcon_type:
                    ++nrrxcon;
                    break;

                case rxdis_type:
                    ++nrrxdis;
                    break;

                case slotcon_type:
                    ++nrslotcon;
                    break;

                case slotdis_type:
                    ++nrslotdis;
                    break;
                }

                if (isArray(ad))
                    ++nrarray;

                if (isArraySize(ad))
                    ++nrarraysize;
            }

            if (nrrxcon != nrslotcon || nrrxcon > 1)
                yyerror("SIP_RXOBJ_CON and SIP_SLOT_CON must both be given and at most once");

            if (nrrxdis != nrslotdis || nrrxdis > 1)
                yyerror("SIP_RXOBJ_DIS and SIP_SLOT_DIS must both be given and at most once");

            if (nrarray != nrarraysize || nrarray > 1)
                yyerror("/Array/ and /ArraySize/ must both be given and at most once");

            $$ = $1;
        }
    ;

rawarglist: {
            /* No arguments. */

            $$.nrArgs = 0;
        }
    |   argvalue {
            /* The single or first argument. */

            $$.args[0] = $1;
            $$.nrArgs = 1;
        }
    |   rawarglist ',' argvalue {
            /* Check that it wasn't ...(,arg...). */
            if ($1.nrArgs == 0)
                yyerror("First argument of the list is missing");

            /* Check there is nothing after an ellipsis. */
            if ($1.args[$1.nrArgs - 1].atype == ellipsis_type)
                yyerror("An ellipsis must be at the end of the argument list");

            /*
             * If this argument has no default value, then the
             * previous one mustn't either.
             */
            if ($3.defval == NULL && $1.args[$1.nrArgs - 1].defval != NULL)
                yyerror("Compulsory argument given after optional argument");

            /* Check there is room. */
            if ($1.nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            $$ = $1;

            $$.args[$$.nrArgs] = $3;
            $$.nrArgs++;
        }
    ;

argvalue:   TK_SIPSIGNAL optname optassign {
            $$.atype = signal_type;
            $$.argflags = ARG_IS_CONST;
            $$.nrderefs = 0;
            $$.name = $2;
            $$.defval = $3;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPSLOT optname optassign {
            $$.atype = slot_type;
            $$.argflags = ARG_IS_CONST;
            $$.nrderefs = 0;
            $$.name = $2;
            $$.defval = $3;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPANYSLOT optname optassign {
            $$.atype = anyslot_type;
            $$.argflags = ARG_IS_CONST;
            $$.nrderefs = 0;
            $$.name = $2;
            $$.defval = $3;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPRXCON optname {
            $$.atype = rxcon_type;
            $$.argflags = 0;
            $$.nrderefs = 0;
            $$.name = $2;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPRXDIS optname {
            $$.atype = rxdis_type;
            $$.argflags = 0;
            $$.nrderefs = 0;
            $$.name = $2;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPSLOTCON '(' arglist ')' optname {
            $$.atype = slotcon_type;
            $$.argflags = ARG_IS_CONST;
            $$.nrderefs = 0;
            $$.name = $5;

            $3.result.atype = void_type;
            $3.result.argflags = 0;
            $3.result.nrderefs = 0;

            $$.u.sa = sipMalloc(sizeof (signatureDef));
            *$$.u.sa = $3;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_SIPSLOTDIS '(' arglist ')' optname {
            $$.atype = slotdis_type;
            $$.argflags = ARG_IS_CONST;
            $$.nrderefs = 0;
            $$.name = $5;

            $3.result.atype = void_type;
            $3.result.argflags = 0;
            $3.result.nrderefs = 0;

            $$.u.sa = sipMalloc(sizeof (signatureDef));
            *$$.u.sa = $3;

            currentSpec -> sigslots = TRUE;
        }
    |   TK_QOBJECT optname {
            $$.atype = qobject_type;
            $$.argflags = 0;
            $$.nrderefs = 0;
            $$.name = $2;
        }
    |   argtype optassign {
            $$ = $1;
            $$.defval = $2;
        }
    ;

varmember:  TK_STATIC {currentIsStatic = TRUE;} varmem
    |   varmem
    ;

varmem:     member
    |   variable
    ;

member:     TK_VIRTUAL {currentOverIsVirt = TRUE;} function
    |   function
    ;

variable:   cpptype TK_NAME optflags ';' optaccesscode optgetcode optsetcode {
            if (notSkipping())
            {
                /* Check the section. */

                if (sectionFlags != 0)
                {
                    if ((sectionFlags & SECT_IS_PUBLIC) == 0)
                        yyerror("Class variables must be in the public section");

                    if (!currentIsStatic && $5 != NULL)
                        yyerror("%AccessCode cannot be specified for non-static class variables");
                }

                if (currentIsStatic && currentSpec -> genc)
                    yyerror("Cannot have static members in a C structure");

                if ($6 != NULL || $7 != NULL)
                {
                    if ($5 != NULL)
                        yyerror("Cannot mix %AccessCode and %GetCode or %SetCode");

                    if (currentScope() == NULL)
                        yyerror("Cannot specify %GetCode or %SetCode for global variables");
                }

                newVar(currentSpec,currentModule,$2,currentIsStatic,&$1,&$3,$5,$6,$7);
            }

            currentIsStatic = FALSE;
        }
    ;

cpptype:    TK_CONST basetype deref optref {
            $$ = $2;
            $$.nrderefs = $3;
            $$.argflags = ARG_IS_CONST | $4;
            $$.name = NULL;
        }
    |   basetype deref optref {
            $$ = $1;
            $$.nrderefs = $2;
            $$.argflags = $3;
            $$.name = NULL;
        }
    ;

argtype:    cpptype optname optflags {
            $$ = $1;
            $$.name = $2;

            if (findOptFlag(&$3,"AllowNone",bool_flag) != NULL)
                $$.argflags |= ARG_ALLOW_NONE;

            if (findOptFlag(&$3,"GetWrapper",bool_flag) != NULL)
                $$.argflags |= ARG_GET_WRAPPER;

            if (findOptFlag(&$3,"Array",bool_flag) != NULL)
                $$.argflags |= ARG_ARRAY;

            if (findOptFlag(&$3,"ArraySize",bool_flag) != NULL)
                $$.argflags |= ARG_ARRAY_SIZE;

            if (findOptFlag(&$3,"Transfer",bool_flag) != NULL)
                $$.argflags |= ARG_XFERRED;

            if (findOptFlag(&$3,"TransferThis",bool_flag) != NULL)
                $$.argflags |= ARG_THIS_XFERRED;

            if (findOptFlag(&$3,"TransferBack",bool_flag) != NULL)
                $$.argflags |= ARG_XFERRED_BACK;

            if (findOptFlag(&$3,"In",bool_flag) != NULL)
                $$.argflags |= ARG_IN;

            if (findOptFlag(&$3,"Out",bool_flag) != NULL)
                $$.argflags |= ARG_OUT;

            if (findOptFlag(&$3,"Constrained",bool_flag) != NULL)
            {
                $$.argflags |= ARG_CONSTRAINED;

                switch ($$.atype)
                {
                case bool_type:
                    $$.atype = cbool_type;
                    break;

                case int_type:
                    $$.atype = cint_type;
                    break;

                case float_type:
                    $$.atype = cfloat_type;
                    break;

                case double_type:
                    $$.atype = cdouble_type;
                    break;
                }
            }
        }
    ;

optref:     {
            $$ = 0;
        }
    |   '&' {
            if (currentSpec -> genc)
                yyerror("References not allowed in a C module");

            $$ = ARG_IS_REF;
        }
    ;

deref:      {
            $$ = 0;
        }
    |   deref '*' {
            $$ = $1 + 1;
        }
    ;

basetype:   scopedname {
            $$.atype = defined_type;
            $$.u.snd = $1;
        }
    |   scopedname '<' cpptypelist '>' {
            templateDef *td;

            td = sipMalloc(sizeof(templateDef));
            td -> fqname = $1;
            td -> types = $3;

            $$.atype = template_type;
            $$.u.td = td;
        }
    |   TK_STRUCT scopedname {
            /* In a C module all structures must be defined. */
            if (currentSpec -> genc)
            {
                $$.atype = defined_type;
                $$.u.snd = $2;
            }
            else
            {
                $$.atype = struct_type;
                $$.u.sname = $2;
            }
        }
    |   TK_UNSIGNED TK_SHORT {
            $$.atype = ushort_type;
        }
    |   TK_SHORT {
            $$.atype = short_type;
        }
    |   TK_UNSIGNED {
            $$.atype = uint_type;
        }
    |   TK_UNSIGNED TK_INT {
            $$.atype = uint_type;
        }
    |   TK_INT {
            $$.atype = int_type;
        }
    |   TK_LONG {
            $$.atype = long_type;
        }
    |   TK_UNSIGNED TK_LONG {
            $$.atype = ulong_type;
        }
    |   TK_LONG TK_LONG {
            $$.atype = longlong_type;
        }
    |   TK_UNSIGNED TK_LONG TK_LONG {
            $$.atype = ulonglong_type;
        }
    |   TK_FLOAT {
            $$.atype = float_type;
        }
    |   TK_DOUBLE {
            $$.atype = double_type;
        }
    |   TK_BOOL {
            $$.atype = bool_type;
        }
    |   TK_SIGNED TK_CHAR {
            $$.atype = sstring_type;
        }
    |   TK_UNSIGNED TK_CHAR {
            $$.atype = ustring_type;
        }
    |   TK_CHAR {
            $$.atype = string_type;
        }
    |   TK_WCHAR_T {
            $$.atype = wstring_type;
        }
    |   TK_VOID {
            $$.atype = void_type;
        }
    |   TK_PYOBJECT {
            $$.atype = pyobject_type;
        }
    |   TK_PYTUPLE {
            $$.atype = pytuple_type;
        }
    |   TK_PYLIST {
            $$.atype = pylist_type;
        }
    |   TK_PYDICT {
            $$.atype = pydict_type;
        }
    |   TK_PYCALLABLE {
            $$.atype = pycallable_type;
        }
    |   TK_PYSLICE {
            $$.atype = pyslice_type;
        }
    |   TK_PYTYPE {
            $$.atype = pytype_type;
        }
    |   TK_ELLIPSIS {
            $$.atype = ellipsis_type;
        }
    ;

cpptypelist:    cpptype {
            /* The single or first type. */

            $$.args[0] = $1;
            $$.nrArgs = 1;
        }
    |   cpptypelist ',' cpptype {
            /* Check there is nothing after an ellipsis. */
            if ($1.args[$1.nrArgs - 1].atype == ellipsis_type)
                yyerror("An ellipsis must be at the end of the argument list");

            /* Check there is room. */
            if ($1.nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            $$ = $1;

            $$.args[$$.nrArgs] = $3;
            $$.nrArgs++;
        }
    ;

optexceptions:  {
            $$ = NULL;
        }
    |   TK_THROW '(' exceptionlist ')' {
            if (currentSpec->genc)
                yyerror("Exceptions not allowed in a C module");

            $$ = $3;
        }
    ;

exceptionlist:  {
            /* Empty list so use a blank. */

            $$ = sipMalloc(sizeof (throwArgs));
            $$ -> nrArgs = 0;
        }
    |   scopedname {
            /* The only or first exception. */

            $$ = sipMalloc(sizeof (throwArgs));
            $$ -> nrArgs = 1;
            $$ -> args[0] = findException(currentSpec, $1, FALSE);
        }
    |   exceptionlist ',' scopedname {
            /* Check that it wasn't ...(,arg...). */

            if ($1 -> nrArgs == 0)
                yyerror("First exception of throw specifier is missing");

            /* Check there is room. */

            if ($1 -> nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            $$ = $1;
            $$ -> args[$$ -> nrArgs++] = findException(currentSpec, $3, FALSE);
        }
    ;

%%


/*
 * Parse the specification.
 */
void parse(sipSpec *spec, FILE *fp, char *filename, stringList *tsl,
        stringList *xfl)
{
    classTmplDef *tcd;

        /* Initialise the spec. */
 
    spec -> modules = NULL;
    spec -> namecache = NULL;
    spec -> ifacefiles = NULL;
    spec -> classes = NULL;
    spec -> classtemplates = NULL;
    spec -> exceptions = NULL;
    spec -> mappedtypes = NULL;
    spec -> mappedtypetemplates = NULL;
    spec -> enums = NULL;
    spec -> vars = NULL;
    spec -> typedefs = NULL;
    spec -> exphdrcode = NULL;
    spec -> docs = NULL;
    spec -> sigslots = FALSE;
    spec -> genc = -1;
    spec -> options = NULL;

    currentSpec = spec;
    neededQualifiers = tsl;
    excludedQualifiers = xfl;
    currentModule = NULL;
    currentMappedType = NULL;
    currentOverIsVirt = FALSE;
    currentCtorIsExplicit = FALSE;
    currentIsStatic = FALSE;
    previousFile = NULL;
    skipStackPtr = 0;
    currentScopeIdx = 0;
    sectionFlags = 0;

    newModule(fp, filename);
    spec -> module = currentModule;

    yyparse();

    handleEOF();
    handleEOM();

    /*
     * Go through each template class and remove it from the list of
     * classes.
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
void parserEOF(char *name, parserContext *pc)
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
static void newModule(FILE *fp, char *filename)
{
    parseFile(fp, filename, currentModule, FALSE);
    currentModule = allocModule();
    currentModule->file = filename;
}


/*
 * Allocate and initialise the memory for a new module.
 */
static moduleDef *allocModule()
{
    moduleDef *newmod, **tailp;

    newmod = sipMalloc(sizeof (moduleDef));
    newmod->fullname = NULL;
    newmod->name = NULL;
    newmod->version = -1;
    newmod->modflags = 0;
    newmod->qobjclass = -1;
    newmod->othfuncs = NULL;
    newmod->overs = NULL;
    newmod->hdrcode = NULL;
    newmod->cppcode = NULL;
    newmod->copying = NULL;
    newmod->preinitcode = NULL;
    newmod->postinitcode = NULL;
    newmod->unitcode = NULL;
    newmod->modulenr = -1;
    newmod->file = NULL;
    newmod->qualifiers = NULL;
    newmod->root.cd = NULL;
    newmod->root.child = NULL;
    newmod->nrtimelines = 0;
    newmod->nrclasses = 0;
    newmod->nrexceptions = 0;
    newmod->nrmappedtypes = 0;
    newmod->nrenums = 0;
    newmod->nrtypedefs = 0;
    newmod->nrvirthandlers = -1;
    newmod->virthandlers = NULL;
    newmod->license = NULL;
    newmod->proxies = NULL;
    newmod->container = NULL;
    newmod->used = NULL;
    newmod->allimports = NULL;
    newmod->imports = NULL;
    newmod->next = NULL;

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
static void parseFile(FILE *fp, char *name, moduleDef *prevmod, int optional)
{
    parserContext pc;

    pc.filename = name;
    pc.ifdepth = skipStackPtr;
    pc.prevmod = prevmod;

    if (setInputFile(fp, &pc, optional))
        currentContext = pc;
}


/*
 * Find an interface file, or create a new one.
 */
ifaceFileDef *findIfaceFile(sipSpec *pt, moduleDef *mod, scopedNameDef *fqname,
        ifaceFileType iftype, argDef *ad)
{
    ifaceFileDef *iff;

    /* See if the name is already used. */

    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
    {
        if (!sameScopedName(iff->fqcname, fqname))
            continue;

        /*
         * They must be the same type except that we allow a class if if we
         * want an exception.  This is because we allow classes to be used
         * before they are defined.
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

    iff->name = cacheName(pt, scopedNameTail(fqname));
    iff->type = iftype;
    iff->fqcname = fqname;
    iff->module = NULL;
    iff->hdrcode = NULL;
    iff->used = NULL;
    iff->next = pt->ifacefiles;

    pt->ifacefiles = iff;

    return iff;
}


/*
 * Find a class definition in a parse tree.
 */
static classDef *findClass(sipSpec *pt,ifaceFileType iftype,
               scopedNameDef *fqname)
{
    return findClassWithInterface(pt, findIfaceFile(pt, currentModule, fqname, iftype, NULL));
}


/*
 * Find a class definition given an existing interface file.
 */
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff)
{
    classDef *cd;

    for (cd = pt -> classes; cd != NULL; cd = cd -> next)
        if (cd -> iff == iff)
            return cd;

    /* Create a new one. */
    cd = sipMalloc(sizeof (classDef));

    cd -> iff = iff;
    cd -> pyname = classBaseName(cd);
    cd -> classnr = -1;
    cd -> classflags = 0;
    cd -> userflags = 0;
    cd -> ecd = NULL;
    cd -> dtorexceptions = NULL;
    cd -> real = NULL;
    cd -> node = NULL;
    cd -> supers = NULL;
    cd -> mro = NULL;
    cd -> td = NULL;
    cd -> ctors = NULL;
    cd -> defctor = NULL;
    cd -> dealloccode = NULL;
    cd -> dtorcode = NULL;
    cd -> members = NULL;
    cd -> overs = NULL;
    cd -> casts = NULL;
    cd -> vmembers = NULL;
    cd -> visible = NULL;
    cd -> cppcode = NULL;
    cd -> convtosubcode = NULL;
    cd -> subbase = NULL;
    cd -> convtocode = NULL;
    cd -> travcode = NULL;
    cd -> clearcode = NULL;
    cd -> readbufcode = NULL;
    cd -> writebufcode = NULL;
    cd -> segcountcode = NULL;
    cd -> charbufcode = NULL;
    cd -> picklecode = NULL;
    cd -> next = pt -> classes;

    pt -> classes = cd;

    return cd;
}


/*
 * Add an interface file to an interface file list if it isn't already there.
 */
ifaceFileList *addToUsedList(ifaceFileList **ifflp, ifaceFileDef *iff)
{
    ifaceFileList *iffl;

    while ((iffl = *ifflp) != NULL)
    {
        /* Don't bother if it is already there. */
        if (iffl->iff == iff)
            return iffl;

        ifflp = &iffl -> next;
    }

    iffl = sipMalloc(sizeof (ifaceFileList));

    iffl->iff = iff;
    iffl->next = NULL;

    *ifflp = iffl;

    return iffl;
}


/*
 * Find an undefined (or create a new) exception definition in a parse tree.
 */
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new)
{
    exceptionDef *xd, **tail;
    ifaceFileDef *iff;
    classDef *cd;

    iff = findIfaceFile(pt, currentModule, fqname, exception_iface, NULL);

    /* See if it is an existing one. */
    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
        if (xd->iff == iff)
            return xd;

    /*
     * If it is an exception interface file then we have never seen this
     * name before.  We require that exceptions are defined before being
     * used, but don't make the same requirement of classes (for reasons of
     * backwards compatibility).  Therefore the name must be reinterpreted
     * as a (as yet undefined) class.
     */
    if (new)
        if (iff->type == exception_iface)
            cd = NULL;
        else
            yyerror("There is already a class with the same name or the exception has been used before being defined");
    else
    {
        if (iff->type == exception_iface)
            iff->type = class_iface;

        cd = findClassWithInterface(pt, iff);
    }

    /* Create a new one. */
    xd = sipMalloc(sizeof (exceptionDef));

    xd->exceptionnr = -1;
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
static classDef *newClass(sipSpec *pt,ifaceFileType iftype,
              scopedNameDef *fqname)
{
    int flags;
    classDef *cd, *scope;
    codeBlock *hdrcode;

    if (sectionFlags & SECT_IS_PRIVATE)
        yyerror("Classes, structs and namespaces must be in the public or or protected sections");

    flags = 0;

    if ((scope = currentScope()) != NULL)
    {
        if (sectionFlags & SECT_IS_PROT)
            flags = CLASS_IS_PROTECTED;

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

    cd = findClass(pt, iftype, fqname);

    /* Check it hasn't already been defined. */
    if (iftype != namespace_iface && cd->iff->module != NULL)
        yyerror("The struct/class has already been defined");

    /* Complete the initialisation. */
    cd->classflags |= flags;
    cd->ecd = scope;
    cd->iff->module = currentModule;

    appendCodeBlock(&cd->iff->hdrcode, hdrcode);

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

            if (!sameScopedName(ns->iff->fqcname, fqname))
                continue;

            cd->real = ns;
            break;
        }
    }

    return cd;
}


/*
 * Tidy up after finishing a class definition.
 */
static void finishClass(sipSpec *pt, moduleDef *mod, classDef *cd, optFlags *of)
{
    char *pyname;
    optFlag *flg;

    /* Get the Python name and see if it is different to the C++ name. */
    pyname = getPythonName(of, classBaseName(cd));

    cd -> pyname = NULL;
    checkAttributes(pt, mod, cd->ecd, pyname, FALSE);
    cd->pyname = pyname;

    if (cd->pyname != classBaseName(cd))
        setIsRenamedClass(cd);

    if ((flg = findOptFlag(of, "TypeFlags", integer_flag)) != NULL)
        cd->userflags = flg->fvalue.ival;

    if (isOpaque(cd))
    {
        if (findOptFlag(of, "External", bool_flag) != NULL)
            setIsExternal(cd);
    }
    else
    {
        int seq_might, seq_not;
        memberDef *md;

        if (findOptFlag(of, "NoDefaultCtors", bool_flag) != NULL)
            setNoDefaultCtors(cd);

        if (cd -> ctors == NULL)
        {
            if (!noDefaultCtors(cd))
            {
                /* Provide a default ctor. */

                cd->ctors = sipMalloc(sizeof (ctorDef));
 
                cd->ctors->ctorflags = SECT_IS_PUBLIC;
                cd->ctors->pysig.nrArgs = 0;
                cd->ctors->cppsig = &cd -> ctors -> pysig;
                cd->ctors->exceptions = NULL;
                cd->ctors->methodcode = NULL;
                cd->ctors->prehook = NULL;
                cd->ctors->posthook = NULL;
                cd->ctors->next = NULL;

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

        if (findOptFlag(of,"Abstract",bool_flag) != NULL)
        {
            setIsAbstractClass(cd);
            setIsIncomplete(cd);
            resetCanCreate(cd);
        }

        /* We assume a public dtor if nothing specific was provided. */
        if (!isDtor(cd))
            setIsPublicDtor(cd);

        if (findOptFlag(of, "DelayDtor", bool_flag) != NULL)
        {
            setIsDelayedDtor(cd);
            setHasDelayedDtors(mod);
        }

        /*
         * There are subtle differences between the add and concat methods and
         * the multiply and repeat methods.  The number versions can have their
         * operands swapped and may return NotImplemented.  If the user has
         * used the /Numeric/ annotation or there are other numeric operators
         * then we use add/multiply.  Otherwise, if there are indexing
         * operators then we use concat/repeat.
         */
        seq_might = seq_not = FALSE;

        for (md = cd -> members; md != NULL; md = md -> next)
            switch (md -> slot)
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
            case pos_slot:
            case neg_slot:
                /* This is definately not a sequence. */
                seq_not = TRUE;
                break;
            }

        if (!seq_not && seq_might)
            for (md = cd -> members; md != NULL; md = md -> next)
            {
                /* Ignore if the user has been explicit. */
                if (isNumeric(md))
                    continue;

                switch (md -> slot)
                {
                case add_slot:
                    md -> slot = concat_slot;
                    break;

                case iadd_slot:
                    md -> slot = iconcat_slot;
                    break;

                case mul_slot:
                    md -> slot = repeat_slot;
                    break;

                case imul_slot:
                    md -> slot = irepeat_slot;
                    break;
                }
            }
    }

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsClassName(cd->iff->name);
    }
}


/*
 * Create a new mapped type.
 */
static mappedTypeDef *newMappedType(sipSpec *pt, argDef *ad)
{
    mappedTypeDef *mtd;
    scopedNameDef *snd;
    ifaceFileDef *iff;

    /* Check that the type is one we want to map. */
    switch (ad->atype)
    {
    case defined_type:
        snd = ad->u.snd;
        break;

    case template_type:
        snd = ad->u.td->fqname;
        break;

    case struct_type:
        snd = ad->u.sname;
        break;

    default:
        yyerror("Invalid type for %MappedType");
    }

    iff = findIfaceFile(pt, currentModule, snd, mappedtype_iface, ad);

    if (inMainModule())
        setIsUsedName(iff->name);

    /* Check it hasn't already been defined. */
    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff == iff)
        {
            /*
             * We allow types based on the same template but with
             * different arguments.
             */
            if (ad->atype != template_type || sameBaseType(ad, &mtd->type))
                yyerror("Mapped type has already been defined in this module");
        }

    /* The module may not have been set yet. */
    iff->module = currentModule;

    /* Create a new mapped type. */
    mtd = allocMappedType(ad);

    mtd->iff = iff;
    mtd->next = pt->mappedtypes;

    pt->mappedtypes = mtd;

    return mtd;
}


/*
 * Allocate, intialise and return a mapped type structure.
 */
mappedTypeDef *allocMappedType(argDef *type)
{
    mappedTypeDef *mtd;

    mtd = sipMalloc(sizeof (mappedTypeDef));

    mtd->type = *type;
    mtd->type.argflags = 0;
    mtd->type.nrderefs = 0;

    mtd->mappednr = -1;
    mtd->iff = NULL;
    mtd->convfromcode = NULL;
    mtd->convtocode = NULL;
    mtd->next = NULL;

    return mtd;
}


/*
 * Create a new enum.
 */
static enumDef *newEnum(sipSpec *pt,moduleDef *mod,char *name,optFlags *of,
            int flags)
{
    enumDef *ed;
    classDef *escope = currentScope();

    ed = sipMalloc(sizeof (enumDef));

    if (name != NULL)
    {
        ed -> fqcname = text2scopedName(name);
        ed -> pyname = cacheName(pt, getPythonName(of, name));

        checkAttributes(pt, mod, escope, ed->pyname->text, FALSE);
    }
    else
    {
        ed -> fqcname = NULL;
        ed -> pyname = NULL;
    }

    ed -> enumflags = flags;
    ed -> enumnr = -1;
    ed -> ecd = escope;
    ed -> pcd = (flags & SECT_IS_PROT) ? escope : NULL;
    ed -> module = mod;
    ed -> members = NULL;
    ed -> slots = NULL;
    ed -> overs = NULL;
    ed -> next = pt -> enums;

    if (name != NULL && strcmp(ed->pyname->text, name) != 0)
        setIsRenamedEnum(ed);

    pt -> enums = ed;

    if (escope != NULL)
        setHasEnums(escope);

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
            char *nam = NULL;

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

            /* Add the corresponding value. */
            appendScopedName(values, text2scopePart(getType(ename, &src->args[a])));
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
 * Convert a type to a string.  We impose some limitations because I'm too lazy
 * to handle everything that might be needed one day.
 */
static char *getType(scopedNameDef *ename, argDef *ad)
{
    if (ad->atype == defined_type)
        return scopedNameToString(ad->u.snd);

    fatalScopedName(ename);
    fatal(": unsupported type argument to template class instantiation\n");

    return NULL;
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

    /* Work out the length of buffer needed. */
    len = 0;

    for (snd = name; snd != NULL; snd = snd->next)
    {
        len += strlen(snd->name);

        if (snd->next != NULL)
            len += strlen(scope_string);
    }

    /* Allocate and populate the buffer. */
    dp = s = sipMalloc(len + 1);

    for (snd = name; snd != NULL; snd = snd->next)
    {
        strcpy(dp, snd->name);
        dp += strlen(snd->name);

        if (snd->next != NULL)
        {
            strcpy(dp, scope_string);
            dp += strlen(scope_string);
        }
    }

    return s;
}


/*
 * Instantiate a class template.
 */
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod, classDef *scope, scopedNameDef *fqname, classTmplDef *tcd, templateDef *td)
{
    scopedNameDef *type_names, *type_values;
    classDef *cd;
    ctorDef *oct, **cttail;
    memberDef *omd, **mdtail;
    overDef *ood, **odtail;
    argDef *ad;
    ifaceFileList *iffl, **used;

    type_names = type_values = NULL;
    appendTypeStrings(classFQCName(tcd->cd), &tcd->sig, &td->types, NULL, &type_names, &type_values);

    /*
     * Add a mapping from the template name to the instantiated name.  If
     * we have got this far we know there is room for it.
     */
    ad = &tcd->sig.args[tcd->sig.nrArgs++];
    ad->atype = defined_type;
    ad->name = NULL;
    ad->argflags = 0;
    ad->nrderefs = 0;
    ad->defval = NULL;
    ad->u.snd = classFQCName(tcd->cd);

    appendScopedName(&type_names, text2scopePart(scopedNameTail(classFQCName(tcd->cd))));
    appendScopedName(&type_values, text2scopePart(scopedNameToString(fqname)));

    /* Create the new class. */
    cd = sipMalloc(sizeof (classDef));

    /* Start with a shallow copy. */
    *cd = *tcd->cd;

    cd->pyname = scopedNameTail(fqname);
    cd->td = td;

    /* Handle the interface file. */
    cd->iff = findIfaceFile(pt, mod, fqname, class_iface, NULL);
    cd->iff->module = mod;

    /* Make a copy of the used list and add the enclosing scope. */
    used = &cd->iff->used;

    for (iffl = tcd->cd->iff->used; iffl != NULL; iffl = iffl->next)
        addToUsedList(used, iffl->iff);

    /* Include any scope header code. */
    if (scope != NULL)
        appendCodeBlock(&cd->iff->hdrcode, scope->iff->hdrcode);

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsClassName(cd->iff->name);
    }

    cd->ecd = currentScope();

    /* Handle the ctors. */
    cd->ctors = NULL;
    cttail = &cd->ctors;

    for (oct = tcd->cd->ctors; oct != NULL; oct = oct->next)
    {
        ctorDef *nct = sipMalloc(sizeof (ctorDef));

        /* Start with a shallow copy. */
        *nct = *oct;

        templateSignature(&nct->pysig, FALSE, tcd, td, cd);

        if (oct->cppsig == NULL)
            nct->cppsig = NULL;
        else if (oct->cppsig == &oct->pysig)
            nct->cppsig = &nct->pysig;
        else
        {
            nct->cppsig = sipMalloc(sizeof (signatureDef));

            *nct->cppsig = *oct->cppsig;

            templateSignature(nct->cppsig, FALSE, tcd, td, cd);
        }

        nct->methodcode = templateCode(pt, used, nct->methodcode, type_names, type_values);

        nct->next = NULL;
        *cttail = nct;
        cttail = &nct->next;

        /* Handle the default ctor. */
        if (tcd->cd->defctor == oct)
            cd->defctor = nct;
    }

    cd->dealloccode = templateCode(pt, used, cd->dealloccode, type_names, type_values);
    cd->dtorcode = templateCode(pt, used, cd->dtorcode, type_names, type_values);

    /* Handle the members, ie. the common parts of overloads. */
    cd->members = NULL;
    mdtail = &cd->members;

    for (omd = tcd->cd->members; omd != NULL; omd = omd->next)
    {
        memberDef *nmd = sipMalloc(sizeof (memberDef));

        /* Start with a shallow copy. */
        *nmd = *omd;

        nmd->module = mod;

        nmd->next = NULL;
        *mdtail = nmd;
        mdtail = &nmd->next;
    }

    /* Handle the overloads. */
    cd->overs = NULL;
    odtail = &cd->overs;

    for (ood = tcd->cd->overs; ood != NULL; ood = ood->next)
    {
        overDef *nod = sipMalloc(sizeof (overDef));
        memberDef *nmd;

        /* Start with a shallow copy. */
        *nod = *ood;

        for (nmd = cd->members, omd = tcd->cd->members; omd != NULL; omd = omd->next, nmd = nmd->next)
            if (omd == ood->common)
            {
                nod->common = nmd;
                break;
            }

        templateSignature(&nod->pysig, TRUE, tcd, td, cd);

        if (ood->cppsig == &ood->pysig)
            nod->cppsig = &nod->pysig;
        else
        {
            nod->cppsig = sipMalloc(sizeof (signatureDef));

            *nod->cppsig = *ood->cppsig;

            templateSignature(nod->cppsig, TRUE, tcd, td, cd);
        }

        nod->methodcode = templateCode(pt, used, nod->methodcode, type_names, type_values);

        /* Handle any virtual handler. */
        if (ood->virthandler != NULL)
        {
            nod->virthandler = sipMalloc(sizeof (virtHandlerDef));

            /* Start with a shallow copy. */
            *nod->virthandler = *ood->virthandler;

            if (ood->virthandler->cppsig == &ood->pysig)
                nod->virthandler->cppsig = &nod->pysig;
            else
            {
                nod->virthandler->cppsig = sipMalloc(sizeof (signatureDef));

                *nod->virthandler->cppsig = *ood->virthandler->cppsig;

                templateSignature(nod->virthandler->cppsig, TRUE, tcd, td, cd);
            }

            nod->virthandler->module = mod;
            nod->virthandler->virtcode = templateCode(pt, used, nod->virthandler->virtcode, type_names, type_values);
            nod->virthandler->next = mod->virthandlers;

            mod->virthandlers = nod->virthandler;
        }

        nod->next = NULL;
        *odtail = nod;
        odtail = &nod->next;
    }

    cd->cppcode = templateCode(pt, used, cd->cppcode, type_names, type_values);
    cd->iff->hdrcode = templateCode(pt, used, cd->iff->hdrcode, type_names, type_values);
    cd->convtosubcode = templateCode(pt, used, cd->convtosubcode, type_names, type_values);
    cd->convtocode = templateCode(pt, used, cd->convtocode, type_names, type_values);
    cd->travcode = templateCode(pt, used, cd->travcode, type_names, type_values);
    cd->clearcode = templateCode(pt, used, cd->clearcode, type_names, type_values);
    cd->readbufcode = templateCode(pt, used, cd->readbufcode, type_names, type_values);
    cd->writebufcode = templateCode(pt, used, cd->writebufcode, type_names, type_values);
    cd->segcountcode = templateCode(pt, used, cd->segcountcode, type_names, type_values);
    cd->charbufcode = templateCode(pt, used, cd->charbufcode, type_names, type_values);
    cd->picklecode = templateCode(pt, used, cd->picklecode, type_names, type_values);
    cd->next = pt->classes;

    pt->classes = cd;

    tcd->sig.nrArgs--;

    freeScopedName(type_names);
    freeScopedName(type_values);
}


/*
 * Replace any template arguments in a signature.
 */
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd, templateDef *td, classDef *ncd)
{
    int a;

    if (result)
        templateType(&sd->result, tcd, td, ncd);

    for (a = 0; a < sd->nrArgs; ++a)
        templateType(&sd->args[a], tcd, td, ncd);
}


/*
 * Replace any template arguments in a type.
 */
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td, classDef *ncd)
{
    int a;
    char *name;

    /* Ignore if it isn't an unscoped name. */
    if (ad->atype != defined_type || ad->u.snd->next != NULL)
        return;

    name = ad->u.snd->name;

    for (a = 0; a < tcd->sig.nrArgs - 1; ++a)
        if (strcmp(name, scopedNameTail(tcd->sig.args[a].u.snd)) == 0)
        {
            ad->atype = td->types.args[a].atype;

            /* We take the constrained flag from the real type. */
            resetIsConstrained(ad);

            if (isConstrained(&td->types.args[a]))
                setIsConstrained(ad);

            ad->u = td->types.args[a].u;

            return;
        }

    /* Handle the class name itself. */
    if (strcmp(name, scopedNameTail(classFQCName(tcd->cd))) == 0)
    {
        ad->atype = class_type;
        ad->u.cd = ncd;
    }
}


/*
 * Replace any template arguments in a literal code block.
 */
codeBlock *templateCode(sipSpec *pt, ifaceFileList **used, codeBlock *ocb,
        scopedNameDef *names, scopedNameDef *values)
{
    codeBlock *ncb = NULL, **tail = &ncb;

    while (ocb != NULL)
    {
        char *at = ocb->frag;

        do
        {
            char *first = NULL;
            codeBlock *cb;
            scopedNameDef *nam, *val, *nam_first, *val_first;

            /*
             * Go through the rest of this fragment looking for each of the
             * types and the name of the class itself.
             */
            nam = names;
            val = values;

            while (nam != NULL && val != NULL)
            {
                char *cp;

                if ((cp = strstr(at, nam->name)) != NULL)
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

            if (at == ocb->frag)
            {
                cb->filename = ocb->filename;
                cb->linenr = ocb->linenr;
            }
            else
                cb->filename = NULL;

            cb->next = NULL;
            *tail = cb;
            tail = &cb->next;

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
                    "sipForceConvertToTransfer_",
                    "sipForceConvertTo_",
                    "sipConvertFromTransfer_",
                    "sipConvertFrom_",
                    "sipClass_",
                    "sipEnum_",
                    "sipException_",
                    NULL
                };

                char *dp, *sp, **gn;
                int genname = FALSE;

                /*
                 * If the context in which the text is used is in the name of a
                 * SIP generated object then translate any "::" scoping to "_".
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
            }
        }
        while (at != NULL && *at != '\0');

        ocb = ocb->next;
    }

    return ncb;
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
            addToUsedList(used, iff);

            return;
        }
    }

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->ecd == NULL)
            continue;

        if (sameName(ed->fqcname, sname))
        {
            addToUsedList(used, ed->ecd->iff);

            return;
        }
    }
}


/*
 * Compare a scoped name with its string equivalent.
 */
static int sameName(scopedNameDef *snd, const char *sname)
{
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

        found = sameScopedName(fq_name, snd);

        freeScopedName(snd);

        if (found)
            return TRUE;
    }

    return sameScopedName(fq_name, rel_name);
}


/*
 * Create a new typedef.
 */
static void newTypedef(sipSpec *pt,moduleDef *mod,char *name,argDef *type)
{
    typedefDef *td;
    scopedNameDef *fqname = text2scopedName(name);
    classDef *scope = currentScope();

    /* See if we are instantiating a template class. */
    if (type->atype == template_type)
    {
        classTmplDef *tcd;
        templateDef *td = type->u.td;

        for (tcd = pt->classtemplates; tcd != NULL; tcd = tcd->next)
            if (foundInScope(tcd->cd->iff->fqcname, td->fqname) &&
                sameTemplateSignature(&tcd->sig, &td->types, FALSE))
            {
                instantiateClassTemplate(pt, mod, scope, fqname, tcd, td);

                /* All done. */
                return;
            }
    }

    /* Check it doesn't already exist. */
    for (td = pt -> typedefs; td != NULL; td = td -> next)
        if (sameScopedName(td -> fqname,fqname))
        {
            fatalScopedName(fqname);
            fatal(" already defined\n");
        }

    td = sipMalloc(sizeof (typedefDef));

    td -> fqname = fqname;
    td -> ecd = scope;
    td -> module = mod;
    td -> type = *type;
    td -> next = pt -> typedefs;

    mod -> nrtypedefs++;

    pt -> typedefs = td;
}


/*
 * Return TRUE if the template signatures are the same.  A deep comparison is
 * used for mapped type templates where we want to recurse into any nested
 * templates.
 */
int sameTemplateSignature(signatureDef *sd1, signatureDef *sd2, int deep)
{
    int a;

    if (sd1->nrArgs != sd2->nrArgs)
        return FALSE;

    for (a = 0; a < sd1->nrArgs; ++a)
    {
        argDef *ad1 = &sd1->args[a];
        argDef *ad2 = &sd2->args[a];

        /*
         * If we are doing a shallow comparision (ie. for class
         * templates) then a type name on the left hand side matches
         * anything on the right hand side.
         */
        if (ad1->atype == defined_type && !deep)
            continue;

        /*
         * For type names only compare the references and pointers, and
         * do the same for any nested templates.
         */
        if (ad1->atype == defined_type && ad2->atype == defined_type)
        {
            if (isReference(ad1) != isReference(ad2) || ad1->nrderefs != ad2->nrderefs)
                return FALSE;
        }
        else if (ad1->atype == template_type && ad2->atype == template_type)
        {
            if (!sameTemplateSignature(&ad1->u.td->types, &ad2->u.td->types, deep))
                return FALSE;
        }
        else if (!sameBaseType(ad1, ad2))
            return FALSE;
    }

    return TRUE;
}


/*
 * Create a new variable.
 */
static void newVar(sipSpec *pt,moduleDef *mod,char *name,int isstatic,
           argDef *type,optFlags *of,codeBlock *acode,codeBlock *gcode,
           codeBlock *scode)
{
    varDef *var;
    classDef *escope = currentScope();
    nameDef *nd = cacheName(pt,getPythonName(of,name));

    if (inMainModule())
        setIsUsedName(nd);

    checkAttributes(pt, mod, escope, nd->text, FALSE);

    var = sipMalloc(sizeof (varDef));

    var -> pyname = nd;
    var -> fqcname = text2scopedName(name);
    var -> ecd = escope;
    var -> module = mod;
    var -> varflags = 0;
    var -> type = *type;
    var -> accessfunc = acode;
    var -> getcode = gcode;
    var -> setcode = scode;
    var -> next = pt -> vars;

    if (isstatic || (escope != NULL && escope->iff->type == namespace_iface))
        setIsStaticVar(var);

    pt -> vars = var;
}


/*
 * Create a new ctor.
 */
static void newCtor(char *name,int sectFlags,signatureDef *args,
            optFlags *optflgs,codeBlock *methodcode,
            throwArgs *exceptions,signatureDef *cppsig,int explicit)
{
    ctorDef *ct, **ctp;
    classDef *cd = currentScope();

    /* Check the name of the constructor. */
    if (strcmp(classBaseName(cd),name) != 0)
        yyerror("Constructor doesn't have the same name as its class");

    /* Add to the list of constructors. */
    ct = sipMalloc(sizeof (ctorDef));

    ct -> ctorflags = sectFlags;
    ct -> pysig = *args;
    ct -> cppsig = (cppsig != NULL ? cppsig : &ct -> pysig);
    ct -> exceptions = exceptions;
    ct -> methodcode = methodcode;
    ct -> next = NULL;

    if (!isPrivateCtor(ct))
        setCanCreate(cd);

    if (isProtectedCtor(ct))
        setHasShadow(cd);

    if (explicit)
        setIsExplicitCtor(ct);

    getHooks(optflgs,&ct -> prehook,&ct -> posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGILCtor(ct);
    else if (getHoldGIL(optflgs))
        setIsHoldGILCtor(ct);

    if (findOptFlag(optflgs,"NoDerived",bool_flag) != NULL)
    {
        if (cppsig != NULL)
            yyerror("The /NoDerived/ annotation cannot be used with a C++ signature");

        if (methodcode == NULL)
            yyerror("The /NoDerived/ annotation must be used with %MethodCode");

        ct->cppsig = NULL;
    }

    if (findOptFlag(optflgs,"Default",bool_flag) != NULL)
    {
        if (cd -> defctor != NULL)
            yyerror("A constructor with the /Default/ annotation has already been defined");

        cd -> defctor = ct;
    }

    /* Append to the list. */
    for (ctp = &cd->ctors; *ctp != NULL; ctp = &(*ctp)->next)
        ;

    *ctp = ct;
}


/*
 * Create a new function.
 */
static void newFunction(sipSpec *pt,moduleDef *mod,int sflags,int isstatic,
            int isvirt,char *name,signatureDef *sig,int isconst,
            int isabstract,optFlags *optflgs,codeBlock *methodcode,
            codeBlock *vcode,throwArgs *exceptions,
            signatureDef *cppsig)
{
    classDef *cd = currentScope();
    nameDef *pname;
    int factory, xferback;
    overDef *od, **odp, **headp;
    optFlag *of;
    virtHandlerDef *vhd;

    /* Extra checks for a C module. */
    if (pt -> genc)
    {
        if (cd != NULL)
            yyerror("Function declaration not allowed in a struct in a C module");

        if (isstatic)
            yyerror("Static functions not allowed in a C module");

        if (exceptions != NULL)
            yyerror("Exceptions not allowed in a C module");
    }

    headp = (cd != NULL ?  &cd->overs : &mod->overs);

    /* See if it is a factory method. */
    if (findOptFlag(optflgs,"Factory",bool_flag) != NULL)
        factory = TRUE;
    else
    {
        int a;

        factory = FALSE;

        /* Check /TransferThis/ wasn't specified. */
        if (cd == NULL || isstatic)
            for (a = 0; a < sig -> nrArgs; ++a)
                if (isThisTransferred(&sig -> args[a]))
                    yyerror("/TransferThis/ may only be specified in constructors and class methods");
    }

    /* See if the result is to be returned to Python ownership. */
    xferback = (findOptFlag(optflgs,"TransferBack",bool_flag) != NULL);

    if (factory && xferback)
        yyerror("/TransferBack/ and /Factory/ cannot both be specified");

    /* Use the C++ name if a Python name wasn't given. */
    pname = cacheName(pt, getPythonName(optflgs, name));

    /* Create a new overload definition. */

    od = sipMalloc(sizeof (overDef));

    /* Set the overload flags. */

    od -> overflags = sflags;

    if (factory)
        setIsFactory(od);

    if (xferback)
        setIsResultTransferredBack(od);

    if (findOptFlag(optflgs,"Transfer",bool_flag) != NULL)
        setIsResultTransferred(od);

    if (isProtected(od))
        setHasShadow(cd);

    if ((isSlot(od) || isSignal(od)) && !isPrivate(od))
    {
        if (isSignal(od))
            setHasShadow(cd);

        pt -> sigslots = TRUE;
    }

    if (isSignal(od) && (methodcode != NULL || vcode != NULL))
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

    if (isabstract)
    {
        if (sflags == 0)
            yyerror("Non-class function specified as abstract");

        setIsAbstract(od);
    }

    if ((of = findOptFlag(optflgs,"AutoGen",opt_name_flag)) != NULL)
    {
        setIsAutoGen(od);

        if (of -> fvalue.sval != NULL)
        {
            qualDef *qd;

            if ((qd = findQualifier(of -> fvalue.sval)) == NULL || qd -> qtype != feature_qualifier)
                yyerror("No such feature");

            if (excludedFeature(excludedQualifiers,qd))
                resetIsAutoGen(od);
        }
    }

    if (isvirt)
    {
        if (isSignal(od) && !optNoEmitters(pt))
            yyerror("Virtual signals aren't supported");

        setIsVirtual(od);
        setHasShadow(cd);

        vhd = sipMalloc(sizeof (virtHandlerDef));

        vhd -> virthandlernr = -1;
        vhd -> vhflags = 0;
        vhd -> pysig = &od -> pysig;
        vhd -> cppsig = (cppsig != NULL ? cppsig : &od -> pysig);
        vhd -> module = currentModule;
        vhd -> virtcode = vcode;
        vhd -> next = currentModule -> virthandlers;

        if (factory || xferback)
            setIsTransferVH(vhd);

        currentModule -> virthandlers = vhd;
    }
    else
    {
        if (vcode != NULL)
            yyerror("%VirtualCatcherCode provided for non-virtual function");

        vhd = NULL;
    }

    od -> cppname = name;
    od -> pysig = *sig;
    od -> cppsig = (cppsig != NULL ? cppsig : &od -> pysig);
    od -> exceptions = exceptions;
    od -> methodcode = methodcode;
    od -> virthandler = vhd;
    od -> common = findFunction(pt,mod,cd,pname,(methodcode != NULL),sig -> nrArgs);

    if (findOptFlag(optflgs,"Numeric",bool_flag) != NULL)
        setIsNumeric(od -> common);

    /* Methods that run in new threads must be virtual. */
    if (findOptFlag(optflgs,"NewThread",bool_flag) != NULL)
    {
        argDef *res;

        if (!isvirt)
            yyerror("/NewThread/ may only be specified for virtual functions");

        /*
         * This is an arbitary limitation to make the code generator
         * slightly easier - laziness on my part.
         */
        res = &od -> cppsig -> result;

        if (res -> atype != void_type || res -> nrderefs != 0)
            yyerror("/NewThread/ may only be specified for void functions");

        setIsNewThread(od);
    }

    getHooks(optflgs,&od -> prehook,&od -> posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGIL(od);
    else if (getHoldGIL(optflgs))
        setIsHoldGIL(od);

    od -> next = NULL;

    /* Append to the list. */
    for (odp = headp; *odp != NULL; odp = &(*odp)->next)
        ;

    *odp = od;
}


/*
 * Return the Python name based on the C/C++ name and any /PyName/ annotation.
 */
static char *getPythonName(optFlags *optflgs, char *cname)
{
    char *pname;
    optFlag *of;

    if ((of = findOptFlag(optflgs, "PyName", name_flag)) != NULL)
        pname = of -> fvalue.sval;
    else
        pname = cname;

    return pname;
}


/*
 * Cache a name in a module.
 */
static nameDef *cacheName(sipSpec *pt, char *name)
{
    nameDef *nd;

    /* See if it already exists. */
    for (nd = pt->namecache; nd != NULL; nd = nd->next)
        if (strcmp(nd->text, name) == 0)
            return nd;

    /* Create a new one. */
    nd = sipMalloc(sizeof (nameDef));

    nd->nameflags = 0;
    nd->module = currentSpec->module;
    nd->text = name;
    nd->next = pt->namecache;

    pt->namecache = nd;

    return nd;
}


/*
 * Find (or create) an overloaded function name.
 */
static memberDef *findFunction(sipSpec *pt,moduleDef *mod,classDef *cd,
                   nameDef *pname,int hwcode,int nrargs)
{
    static struct slot_map {
        char *name;     /* The slot name. */
        slotType type;      /* The corresponding type. */
        int needs_hwcode;   /* If handwritten code is required. */
        int nrargs;     /* Nr. of arguments. */
    } slot_table[] = {
        {"__str__", str_slot, TRUE, 0},
        {"__unicode__", unicode_slot, TRUE, 0},
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
        {"__iand__", iand_slot, FALSE, 1},
        {"__ior__", ior_slot, FALSE, 1},
        {"__ixor__", ixor_slot, FALSE, 1},
        {"__ilshift__", ilshift_slot, FALSE, 1},
        {"__irshift__", irshift_slot, FALSE, 1},
        {"__invert__", invert_slot, FALSE, 0},
        {"__call__", call_slot, FALSE, -1},
        {"__getitem__", getitem_slot, FALSE, -1},
        {"__setitem__", setitem_slot, TRUE, -1},
        {"__delitem__", delitem_slot, TRUE, -1},
        {"__lt__", lt_slot, FALSE, 1},
        {"__le__", le_slot, FALSE, 1},
        {"__eq__", eq_slot, FALSE, 1},
        {"__ne__", ne_slot, FALSE, 1},
        {"__gt__", gt_slot, FALSE, 1},
        {"__ge__", ge_slot, FALSE, 1},
        {"__cmp__", cmp_slot, FALSE, 1},
        {"__nonzero__", nonzero_slot, TRUE, 0},
        {"__neg__", neg_slot, FALSE, 0},
        {"__pos__", pos_slot, FALSE, 0},
        {"__abs__", abs_slot, TRUE, 0},
        {"__repr__", repr_slot, TRUE, 0},
        {"__hash__", hash_slot, TRUE, 0},
        {NULL}
    };

    memberDef *md, **flist;
    struct slot_map *sm;
    slotType st;

    /* Get the slot type. */
    st = no_slot;

    for (sm = slot_table; sm -> name != NULL; ++sm)
        if (strcmp(sm -> name,pname -> text) == 0)
        {
            if (sm -> needs_hwcode && !hwcode)
                yyerror("This Python slot requires %MethodCode");

            if (sm -> nrargs < 0)
            {
                int min_nr;

                /* These require a minimum number. */
                switch (sm -> type)
                {
                case getitem_slot:
                case delitem_slot:
                    min_nr = 1;
                    break;

                case setitem_slot:
                    min_nr = 2;
                    break;

                default:
                    min_nr = 0;
                }

                if (nrargs < min_nr)
                    yyerror("Insufficient number of arguments to Python slot");
            }
            else if (cd == NULL)
            {
                /* Global operators need one extra argument. */
                if (sm -> nrargs + 1 != nrargs)
                    yyerror("Incorrect number of arguments to global operator");
            }
            else if (sm -> nrargs != nrargs)
                yyerror("Incorrect number of arguments to Python slot");

            st = sm -> type;

            break;
        }

    if (inMainModule())
        setIsUsedName(pname);

    /* Check there is no name clash. */
    checkAttributes(pt, mod, cd, pname->text, TRUE);

    /* See if it already exists. */
    flist = (cd != NULL ? &cd->members : &mod->othfuncs);

    for (md = *flist; md != NULL; md = md -> next)
        if (md -> pyname == pname && md -> module == mod)
            return md;

    /* Create a new one. */
    md = sipMalloc(sizeof (memberDef));

    md -> pyname = pname;
    md -> memberflags = 0;
    md -> slot = st;
    md -> module = mod;
    md -> next = *flist;

    *flist = md;

    /* Global operators are a subset. */
    if (cd == NULL && st != no_slot && st != neg_slot && st != pos_slot && !isNumberSlot(md) && !isRichCompareSlot(md))
        yyerror("Global operators must be either numeric or comparison operators");

    return md;
}


/*
 * Search a set of flags for a particular one and check its type.
 */
static optFlag *findOptFlag(optFlags *flgs,char *name,flagType ft)
{
    int f;

    for (f = 0; f < flgs -> nrFlags; ++f)
    {
        optFlag *of = &flgs -> flags[f];

        if (strcmp(of -> fname,name) == 0)
        {
            /*
             * An optional name can look like a boolean or a name.
             */

            if (ft == opt_name_flag)
            {
                if (of -> ftype == bool_flag)
                {
                    of -> ftype = opt_name_flag;
                    of -> fvalue.sval = NULL;
                }
                else if (of -> ftype == name_flag)
                    of -> ftype = opt_name_flag;
            }

            if (ft != of -> ftype)
                yyerror("Optional flag has a value of the wrong type");

            return of;
        }
    }

    return NULL;
}


/*
 * A name is going to be used as a Python attribute name within a Python scope
 * (ie. a Python dictionary), so check against what we already know is going in
 * the same scope in case there is a clash.
 */
static void checkAttributes(sipSpec *pt, moduleDef *mod, classDef *pyscope,
        char *attr, int isfunc)
{
    enumDef *ed;
    varDef *vd;
    classDef *cd;

    /* Check the enums. */

    for (ed = pt -> enums; ed != NULL; ed = ed -> next)
    {
        enumMemberDef *emd;

        if (ed -> ecd != pyscope || ed -> pyname == NULL)
            continue;

        if (strcmp(ed->pyname->text, attr) == 0)
            yyerror("There is already an enum in scope with the same Python name");

        for (emd = ed -> members; emd != NULL; emd = emd -> next)
            if (strcmp(emd -> pyname -> text, attr) == 0)
                yyerror("There is already an enum member in scope with the same Python name");
    }

    /* Check the variables. */

    for (vd = pt -> vars; vd != NULL; vd = vd -> next)
    {
        if (vd -> ecd != pyscope)
            continue;

        if (strcmp(vd -> pyname -> text, attr) == 0)
            yyerror("There is already a variable in scope with the same Python name");
    }

    /*
     * Only check the members if this attribute isn't a member because we
     * can handle members with the same name in the same scope.
     */
    if (!isfunc)
    {
        memberDef *md, *membs;

        membs = (pyscope != NULL ? pyscope->members : mod->othfuncs);

        for (md = membs; md != NULL; md = md -> next)
        {
            overDef *od, *overs;

            if (strcmp(md -> pyname -> text, attr) != 0)
                continue;

            /* Check for a conflict with all overloads. */

            overs = (pyscope != NULL ? pyscope->overs : mod->overs);

            for (od = overs; od != NULL; od = od -> next)
            {
                if (od -> common != md)
                    continue;

                yyerror("There is already a function in scope with the same Python name");
            }
        }
    }

    /* Check the classes. */

    for (cd = pt -> classes; cd != NULL; cd = cd -> next)
    {
        if (cd -> ecd != pyscope || cd -> pyname == NULL)
            continue;

        if (strcmp(cd->pyname, attr) == 0 && !isExternal(cd))
            yyerror("There is already a class or namespace in scope with the same Python name");
    }

    /* Check the exceptions. */

    if (pyscope == NULL)
    {
        exceptionDef *xd;

        for (xd = pt->exceptions; xd != NULL; xd = xd->next)
            if (xd->pyname != NULL && strcmp(xd->pyname, attr) == 0)
                yyerror("There is already an exception with the same Python name");
    }
}


/*
 * Append a code block to a list of them.  Append is needed to give the
 * specifier easy control over the order of the documentation.
 */
void appendCodeBlock(codeBlock **headp, codeBlock *new)
{
    while (*headp != NULL)
        headp = &(*headp)->next;

    *headp = new;
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

    if (skipStackPtr > currentContext.ifdepth)
        fatal("Too many %%If statements in %s\n", previousFile);

    if (skipStackPtr < currentContext.ifdepth)
        fatal("Too many %%End statements in %s\n", previousFile);
}


/*
 * Handle the end of a fully parsed a module.
 */
static void handleEOM()
{
    /* Check it has been named. */

    if (currentModule -> name == NULL)
        fatal("No %%Module has been specified for module defined in %s\n",previousFile);

    /* The previous module is now current. */

    currentModule = currentContext.prevmod;
}


/*
 * Find an existing qualifier.
 */
static qualDef *findQualifier(char *name)
{
    moduleDef *mod;

    for (mod = currentSpec -> modules; mod != NULL; mod = mod -> next)
    {
        qualDef *qd;

        for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
            if (strcmp(qd -> name,name) == 0)
                return qd;
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

    snd -> name = text;
    snd -> next = NULL;

    return snd;
}


/*
 * Convert a text string to a fully scoped name.
 */
static scopedNameDef *text2scopedName(char *text)
{
    return scopeScopedName(text2scopePart(text));
}


/*
 * Prepend any current scope to a scoped name.
 */
static scopedNameDef *scopeScopedName(scopedNameDef *name)
{
    classDef *cd = currentScope();
    scopedNameDef *snd;

    snd = (cd != NULL ? copyScopedName(cd->iff->fqcname) : NULL);

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

    while (snd -> next != NULL)
        snd = snd -> next;

    return snd -> name;
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
    return (skipStackPtr == 0 ? TRUE : skipStack[skipStackPtr - 1]);
}


/*
 * Return the value of an expression involving a time period.
 */
static int timePeriod(char *lname,char *uname)
{
    int this, line;
    qualDef *qd, *lower, *upper;
    moduleDef *mod;

    if (lname == NULL)
        lower = NULL;
    else if ((lower = findQualifier(lname)) == NULL || lower -> qtype != time_qualifier)
        yyerror("Lower bound is not a time version");

    if (uname == NULL)
        upper = NULL;
    else if ((upper = findQualifier(uname)) == NULL || upper -> qtype != time_qualifier)
        yyerror("Upper bound is not a time version");

    /* Sanity checks on the bounds. */

    if (lower == NULL && upper == NULL)
        yyerror("Lower and upper bounds cannot both be omitted");

    if (lower != NULL && upper != NULL)
    {
        if (lower -> module != upper -> module || lower -> line != upper -> line)
            yyerror("Lower and upper bounds are from different timelines");

        if (lower == upper)
            yyerror("Lower and upper bounds must be different");

        if (lower -> order > upper -> order)
            yyerror("Later version specified as lower bound");
    }

    /* Go through each slot in the relevant timeline. */

    if (lower != NULL)
    {
        mod = lower -> module;
        line = lower -> line;
    }
    else
    {
        mod = upper -> module;
        line = upper -> line;
    }

    this = FALSE;

    for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
    {
        if (qd -> qtype != time_qualifier || qd -> line != line)
            continue;

        if (lower != NULL && qd -> order < lower -> order)
            continue;

        if (upper != NULL && qd -> order >= upper -> order)
            continue;

        /*
         * This is within the required range so if it is also needed
         * then the expression is true.
         */

        if (isNeeded(qd))
        {
            this = TRUE;
            break;
        }
    }

    return this;
}


/*
 * Return the value of an expression involving a single platform or feature.
 */
static int platOrFeature(char *name,int optnot)
{
    int this;
    qualDef *qd;

    if ((qd = findQualifier(name)) == NULL || qd -> qtype == time_qualifier)
        yyerror("No such platform or feature");

    /* Assume this sub-expression is false. */

    this = FALSE;

    if (qd -> qtype == feature_qualifier)
    {
        if (!excludedFeature(excludedQualifiers,qd))
            this = TRUE;
    }
    else if (isNeeded(qd))
        this = TRUE;

    if (optnot)
        this = !this;

    return this;
}


/*
 * Return TRUE if the given qualifier is excluded.
 */
int excludedFeature(stringList *xsl,qualDef *qd)
{
    while (xsl != NULL)
    {
        if (strcmp(qd -> name,xsl -> s) == 0)
            return TRUE;

        xsl = xsl -> next;
    }

    return FALSE;
}


/*
 * Return TRUE if the given qualifier is needed.
 */
static int isNeeded(qualDef *qd)
{
    stringList *sl;

    for (sl = neededQualifiers; sl != NULL; sl = sl -> next)
        if (strcmp(qd -> name,sl -> s) == 0)
            return TRUE;

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
static void newQualifier(moduleDef *mod, int line, int order, char *name,
        qualType qt)
{
    qualDef *qd;

    /* Check it doesn't already exist. */

    if (findQualifier(name) != NULL)
        yyerror("Version is already defined");

    qd = sipMalloc(sizeof (qualDef));
    qd->name = name;
    qd->qtype = qt;
    qd->module = mod;
    qd->line = line;
    qd->order = order;
    qd->next = mod -> qualifiers;
    mod->qualifiers = qd;
}


/*
 * Create a new imported module.
 */
static void newImport(char *filename)
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

    if ((of = findOptFlag(optflgs,"PreHook",name_flag)) != NULL)
        *pre = of -> fvalue.sval;
    else
        *pre = NULL;

    if ((of = findOptFlag(optflgs,"PostHook",name_flag)) != NULL)
        *post = of -> fvalue.sval;
    else
        *post = NULL;
}


/*
 * Get the /ReleaseGIL/ option flag.
 */
static int getReleaseGIL(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "ReleaseGIL", bool_flag) != NULL);
}


/*
 * Get the /HoldGIL/ option flag.
 */
static int getHoldGIL(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "HoldGIL", bool_flag) != NULL);
}


/*
 * Return TRUE if the QtNoEmitters option was specified.
 */
int optNoEmitters(sipSpec *pt)
{
    return optFind(pt, "QtNoEmitters");
}


/*
 * Return TRUE if the QtRegisterTypes option was specified.
 */
int optRegisterTypes(sipSpec *pt)
{
    return optFind(pt, "QtRegisterTypes");
}


/*
 * Return TRUE if the Qt4Q_OBJECT option was specified.
 */
int optQ_OBJECT4(sipSpec *pt)
{
    return optFind(pt, "Qt4Q_OBJECT");
}


/*
 * Return TRUE if a particular option was specified with %SIPOptions.
 */
static int optFind(sipSpec *pt, const char *opt)
{
    stringList *sl;

    for (sl = pt->options; sl != NULL; sl = sl->next)
        if (strcmp(sl->s, opt) == 0)
            return TRUE;

    return FALSE;
}


/*
 * Set the name of a module.
 */
static void setModuleName(moduleDef *mod, const char *fullname)
{
    mod->fullname = fullname;

    if ((mod->name = strrchr(fullname, '.')) != NULL)
        mod->name++;
    else
        mod->name = fullname;
}
