/*
 * The code generator module for SIP.
 *
 * @BS_LICENSE@
 */


#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "sip.h"


/*
 * These must match the values of SIP_TYPE_FLAGS_SHIFT and SIP_TYPE_FLAGS_MASK
 * in siplib/sip.h.
 */
#define TYPE_FLAGS_SHIFT        8
#define TYPE_FLAGS_MASK         0x0f00

/* Return the base (ie. C/C++) name of a super-type or meta-type. */
#define smtypeName(sm)          (strrchr((sm)->name->text, '.') + 1)


/* Control what generateCalledArgs() actually generates. */
typedef enum {
    Declaration,
    Definition
} funcArgType;


/* An entry in the sorted array of methods. */
typedef struct {
    memberDef *md;                      /* The method. */
    int is_static;                      /* Set if all overloads are static. */
} sortedMethTab;


static int currentLineNr;               /* Current output line number. */
static const char *currentFileName;     /* Current output file name. */
static int previousLineNr;              /* Previous output line number. */
static const char *previousFileName;    /* Previous output file name. */
static int exceptions;                  /* Set if exceptions are enabled. */
static int tracing;                     /* Set if tracing is enabled. */
static int generating_c;                /* Set if generating C. */
static int release_gil;                 /* Set if always releasing the GIL. */
static const char *prcode_last = NULL;  /* The last prcode format string. */
static int prcode_xml = FALSE;          /* Set if prcode is XML aware. */


static void generateDocumentation(sipSpec *pt, const char *docFile);
static void generateBuildFile(sipSpec *pt, const char *buildFile,
        const char *srcSuffix, const char *consModule);
static void generateBuildFileSources(sipSpec *pt, moduleDef *mod,
        const char *srcSuffix, FILE *fp);
static void generateInternalAPIHeader(sipSpec *pt, moduleDef *mod,
        const char *codeDir, stringList *xsl);
static void generateCpp(sipSpec *pt, moduleDef *mod, const char *codeDir,
        const char *srcSuffix, int parts, stringList *xsl);
static void generateCompositeCpp(sipSpec *pt, const char *codeDir);
static void generateConsolidatedCpp(sipSpec *pt, const char *codeDir,
        const char *srcSuffix);
static void generateComponentCpp(sipSpec *pt, const char *codeDir,
        const char *consModule);
static void generateSipImport(moduleDef *mod, FILE *fp);
static void generateSipImportVariables(FILE *fp);
static void generateModInitStart(moduleDef *mod, int gen_c, FILE *fp);
static void generateIfaceCpp(sipSpec *, ifaceFileDef *, const char *,
        const char *, FILE *);
static void generateMappedTypeCpp(mappedTypeDef *mtd, sipSpec *pt, FILE *fp);
static void generateImportedMappedTypeAPI(mappedTypeDef *mtd, moduleDef *mod,
        FILE *fp);
static void generateMappedTypeAPI(sipSpec *pt, mappedTypeDef *mtd, FILE *fp);
static void generateClassCpp(classDef *cd, sipSpec *pt, FILE *fp);
static void generateImportedClassAPI(classDef *cd, sipSpec *pt, moduleDef *mod,
        FILE *fp);
static void generateClassTableEntries(sipSpec *pt, moduleDef *mod, nodeDef *nd,
        FILE *fp);
static void generateClassAPI(classDef *cd, sipSpec *pt, FILE *fp);
static void generateClassFunctions(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static void generateShadowCode(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static void generateFunction(memberDef *, overDef *, classDef *, classDef *,
        FILE *);
static void generateFunctionBody(overDef *, classDef *, classDef *, int deref,
        FILE *);
static void generateTypeDefinition(sipSpec *pt, classDef *cd, FILE *fp);
static void generateTypeInit(classDef *, FILE *);
static void generateCppCodeBlock(codeBlock *, FILE *);
static void generateUsedIncludes(ifaceFileList *iffl, FILE *fp);
static void generateModuleAPI(sipSpec *pt, moduleDef *mod, FILE *fp);
static void generateImportedModuleAPI(sipSpec *pt, moduleDef *mod,
        moduleDef *immod, FILE *fp);
static void generateShadowClassDeclaration(sipSpec *, classDef *, FILE *);
static int hasConvertToCode(argDef *ad);
static void deleteTemps(signatureDef *sd, FILE *fp);
static void gc_ellipsis(signatureDef *sd, FILE *fp);
static void generateCallArgs(classDef *, signatureDef *, signatureDef *,
        FILE *);
static void generateCalledArgs(classDef *, signatureDef *, funcArgType, int,
        FILE *);
static void generateVariable(classDef *, argDef *, int, FILE *);
static void generateNamedValueType(classDef *, argDef *, char *, FILE *);
static void generateBaseType(classDef *, argDef *, int, FILE *);
static void generateNamedBaseType(classDef *, argDef *, char *, int, FILE *);
static void generateTupleBuilder(signatureDef *, FILE *);
static void generateEmitters(classDef *cd, FILE *fp);
static void generateEmitter(classDef *, visibleList *, FILE *);
static void generateVirtualHandler(virtHandlerDef *vhd, FILE *fp);
static void generateVirtHandlerErrorReturn(argDef *res, FILE *fp);
static void generateVirtualCatcher(moduleDef *mod, classDef *cd, int virtNr,
        virtOverDef *vod, FILE *fp);
static void generateUnambiguousClass(classDef *cd, classDef *scope, FILE *fp);
static void generateProtectedEnums(sipSpec *, classDef *, FILE *);
static void generateProtectedDeclarations(classDef *, FILE *);
static void generateProtectedDefinitions(classDef *, FILE *);
static void generateProtectedCallArgs(overDef *od, FILE *fp);
static void generateConstructorCall(classDef *, ctorDef *, int, FILE *);
static void generateHandleResult(overDef *, int, int, char *, FILE *);
static void generateOrdinaryFunction(moduleDef *mod, classDef *cd,
        memberDef *md, FILE *fp);
static void generateSimpleFunctionCall(fcallDef *, FILE *);
static void generateFunctionCall(classDef *cd, classDef *ocd, overDef *od,
        int deref, FILE *fp);
static void generateCppFunctionCall(classDef *cd, classDef *ocd, overDef *od,
        FILE *fp);
static void generateSlotArg(signatureDef *sd, int argnr, FILE *fp);
static void generateComparisonSlotCall(classDef *cd, overDef *od,
        const char *op, const char *cop, int deref, FILE *fp);
static void generateBinarySlotCall(classDef *cd, overDef *od, const char *op,
        int deref, FILE *fp);
static void generateNumberSlotCall(overDef *od, char *op, FILE *fp);
static void generateVariableHandler(classDef *, varDef *, FILE *);
static int generateObjToCppConversion(argDef *, FILE *);
static void generateVarClassConversion(varDef *, int, FILE *);
static void generateVarMember(varDef *vd, FILE *fp);
static int generateVoidPointers(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateChars(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static int generateStrings(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static sortedMethTab *createFunctionTable(classDef *, int *);
static sortedMethTab *createMethodTable(classDef *, int *);
static int generateMethodTable(classDef *, FILE *);
static void generateEnumMacros(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateEnumMemberTable(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateInts(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static int generateLongs(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static int generateUnsignedLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateLongLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateUnsignedLongLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static int generateVariableType(sipSpec *pt, moduleDef *mod, classDef *cd,
        argType atype, const char *eng, const char *s1, const char *s2,
        FILE *fp);
static int generateDoubles(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static int generateClasses(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static void generateTypesInline(sipSpec *pt, moduleDef *mod, FILE *fp);
static void generateAccessFunctions(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp);
static void generateConvertToDefinitions(mappedTypeDef *, classDef *, FILE *);
static void generateEncodedClass(moduleDef *mod, classDef *cd, int last,
        FILE *fp);
static int generateArgParser(signatureDef *, classDef *, ctorDef *, overDef *,
        int, FILE *);
static void generateTry(throwArgs *, FILE *);
static void generateCatch(throwArgs *ta, signatureDef *sd, FILE *fp);
static void generateThrowSpecifier(throwArgs *, FILE *);
static void generateSlot(moduleDef *mod, classDef *cd, enumDef *ed,
        memberDef *md, FILE *fp);
static void generateCastZero(argDef *ad, FILE *fp);
static void generateCallDefaultCtor(ctorDef *ct, FILE *fp);
static int countVirtuals(classDef *);
static int skipOverload(overDef *, memberDef *, classDef *, classDef *, int);
static int compareMethTab(const void *, const void *);
static int compareEnumMembers(const void *, const void *);
static char *getSubFormatChar(char, argDef *);
static char *createIfaceFileName(const char *, ifaceFileDef *, const char *);
static FILE *createCompilationUnit(moduleDef *mod, const char *fname,
        const char *description);
static FILE *createFile(moduleDef *mod, const char *fname,
        const char *description);
static void closeFile(FILE *);
static void prScopedName(FILE *fp, scopedNameDef *snd, char *sep);
static void prTypeName(FILE *, argDef *, int);
static void prScopedClassName(FILE *fp, classDef *context, classDef *cd);
static int isZeroArgSlot(memberDef *md);
static int isMultiArgSlot(memberDef *md);
static int isIntArgSlot(memberDef *md);
static int isInplaceNumberSlot(memberDef *md);
static int isInplaceSequenceSlot(memberDef *md);
static int needErrorFlag(codeBlock *cb);
static int needNewInstance(argDef *ad);
static int needDealloc(classDef *cd);
static char getBuildResultFormat(argDef *ad);
static const char *getParseResultFormat(argDef *ad, int isres, int xfervh);
static void generateParseResultExtraArgs(argDef *ad, int isres, FILE *fp);
static char *makePartName(const char *codeDir, const char *mname, int part,
        const char *srcSuffix);
static void fakeProtectedArgs(signatureDef *sd);
static void normaliseArgs(signatureDef *);
static void restoreArgs(signatureDef *);
static const char *slotName(slotType st);
static void ints_intro(classDef *cd, FILE *fp);
static const char *argName(const char *name, codeBlock *cb);
static int usedInCode(codeBlock *code, const char *str);
static void generateDefaultValue(argDef *ad, int argnr, FILE *fp);
static void generateClassFromVoid(classDef *cd, const char *cname,
        const char *vname, FILE *fp);
static void generateMappedTypeFromVoid(mappedTypeDef *mtd, const char *cname,
        const char *vname, FILE *fp);
static int generateSubClassConvertors(sipSpec *pt, moduleDef *mod, FILE *fp);
static void generateNameCache(sipSpec *pt, FILE *fp);
static const char *resultOwner(overDef *od);
static void prCachedName(FILE *fp, nameDef *nd, const char *prefix);


/*
 * Generate the code from a specification.
 */
void generateCode(sipSpec *pt, char *codeDir, char *buildfile, char *docFile,
        const char *srcSuffix, int except, int trace, int releaseGIL,
        int parts, stringList *xsl, const char *consModule)
{
    exceptions = except;
    tracing = trace;
    release_gil = releaseGIL;
    generating_c = pt->genc;

    if (srcSuffix == NULL)
        srcSuffix = (generating_c ? ".c" : ".cpp");

    /* Generate the documentation. */
    if (docFile != NULL)
        generateDocumentation(pt,docFile);

    /* Generate the code. */
    if (codeDir != NULL)
    {
        if (isComposite(pt->module))
            generateCompositeCpp(pt, codeDir);
        else if (isConsolidated(pt->module))
        {
            moduleDef *mod;

            for (mod = pt->modules; mod != NULL; mod = mod->next)
                if (mod->container == pt->module)
                    generateCpp(pt, mod, codeDir, srcSuffix, parts, xsl);

            generateConsolidatedCpp(pt, codeDir, srcSuffix);
        }
        else if (consModule != NULL)
            generateComponentCpp(pt, codeDir, consModule);
        else
            generateCpp(pt, pt->module, codeDir, srcSuffix, parts, xsl);
    }

    /* Generate the build file. */
    if (buildfile != NULL)
        generateBuildFile(pt, buildfile, srcSuffix, consModule);
}


/*
 * Generate the documentation.
 */
static void generateDocumentation(sipSpec *pt, const char *docFile)
{
    FILE *fp;
    codeBlock *cb;

    fp = createFile(pt->module, docFile, NULL);

    for (cb = pt->docs; cb != NULL; cb = cb->next)
        fputs(cb->frag, fp);

    closeFile(fp);
}


/*
 * Generate the build file.
 */
static void generateBuildFile(sipSpec *pt, const char *buildFile,
        const char *srcSuffix, const char *consModule)
{
    const char *mname = pt->module->name;
    FILE *fp;

    fp = createFile(pt->module, buildFile, NULL);

    prcode(fp, "target = %s\nsources =", mname);

    if (isComposite(pt->module))
        prcode(fp, " sip%scmodule.c", mname);
    else if (isConsolidated(pt->module))
    {
        moduleDef *mod;

        for (mod = pt->modules; mod != NULL; mod = mod->next)
            if (mod->container == pt->module)
                generateBuildFileSources(pt, mod, srcSuffix, fp);

        prcode(fp, " sip%scmodule%s", mname, srcSuffix);
    }
    else if (consModule == NULL)
        generateBuildFileSources(pt, pt->module, srcSuffix, fp);
    else
        prcode(fp, " sip%scmodule.c", mname);

    if (isConsolidated(pt->module))
    {
        moduleDef *mod;

        prcode(fp, "\nheaders =");

        for (mod = pt->modules; mod != NULL; mod = mod->next)
            if (mod->container == pt->module)
                prcode(fp, " sipAPI%s.h", mod->name);
    }
    else if (!isComposite(pt->module) && consModule == NULL)
        prcode(fp, "\nheaders = sipAPI%s.h", mname);

    prcode(fp, "\n");

    closeFile(fp);
}


/*
 * Generate the list of source files for a module.
 */
static void generateBuildFileSources(sipSpec *pt, moduleDef *mod,
        const char *srcSuffix, FILE *fp)
{
    const char *mname = mod->name;

    if (mod->parts)
    {
        int p;

        for (p = 0; p < mod->parts; ++p)
            prcode(fp, " sip%spart%d%s", mname, p, srcSuffix);
    }
    else
    {
        ifaceFileDef *iff;

        prcode(fp, " sip%scmodule%s", mname, srcSuffix);

        for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
        {
            if (iff->module != mod)
                continue;

            if (iff->type == exception_iface)
                continue;

            prcode(fp, " sip%s%F%s", mname, iff->fqcname, srcSuffix);
        }
    }
}


/*
 * Generate an expression in C++.
 */
void generateExpression(valueDef *vd, FILE *fp)
{
    while (vd != NULL)
    {
        if (vd->vunop != '\0')
            prcode(fp,"%c",vd->vunop);

        switch (vd->vtype)
        {
        case qchar_value:
            prcode(fp,"'%c'",vd->u.vqchar);
            break;

        case string_value:
            prcode(fp,"\"%s\"",vd->u.vstr);
            break;

        case numeric_value:
            prcode(fp,"%l",vd->u.vnum);
            break;

        case real_value:
            prcode(fp,"%g",vd->u.vreal);
            break;

        case scoped_value:
            if (prcode_xml)
                prScopedName(fp, vd->u.vscp, ".");
            else
                prcode(fp, "%S", vd->u.vscp);

            break;

        case fcall_value:
            generateSimpleFunctionCall(vd->u.fcd,fp);
            break;
        }
 
        if (vd->vbinop != '\0')
            prcode(fp," %c ",vd->vbinop);
 
        vd = vd->next;
    }
}


/*
 * Generate the C++ internal module API header file.
 */
static void generateInternalAPIHeader(sipSpec *pt, moduleDef *mod,
        const char *codeDir, stringList *xsl)
{
    char *hfile;
    const char *mname = mod->name;
    int noIntro;
    FILE *fp;
    nameDef *nd;
    moduleDef *imp;
    moduleListDef *mld;

    hfile = concat(codeDir, "/sipAPI", mname, ".h",NULL);
    fp = createFile(mod, hfile, "Internal module API header file.");

    /* Include files. */

    prcode(fp,
"\n"
"#ifndef _%sAPI_H\n"
"#define _%sAPI_H\n"
"\n"
"\n"
"#include <sip.h>\n"
        , mname
        , mname);

    if (pluginPyQt4(pt))
        prcode(fp,
"\n"
"#include <QMetaType>\n"
            );

    /* Define the enabled features. */
    noIntro = TRUE;

    for (imp = pt->modules; imp != NULL; imp = imp->next)
    {
        qualDef *qd;

        for (qd = imp->qualifiers; qd != NULL; qd = qd->next)
            if (qd->qtype == feature_qualifier && !excludedFeature(xsl, qd))
            {
                if (noIntro)
                {
                    prcode(fp,
"\n"
"/* These are the features that are enabled. */\n"
                        );

                    noIntro = FALSE;
                }

                prcode(fp,
"#define SIP_FEATURE_%s\n"
                    , qd->name);
            }
    }

    if (!noIntro)
        prcode(fp,
"\n"
            );

    generateCppCodeBlock(pt->exphdrcode, fp);
    generateCppCodeBlock(mod->hdrcode, fp);

    /* Shortcuts that hide the messy detail of the APIs. */
    noIntro = TRUE;

    for (nd = pt->namecache; nd != NULL; nd = nd->next)
    {
        if (!isUsedName(nd))
            continue;

        if (noIntro)
        {
            prcode(fp,
"\n"
"/*\n"
" * Convenient names to refer to various strings defined in this module.\n"
" * Only the class names are part of the public API.\n"
" */\n"
                );

            noIntro = FALSE;
        }

        prcode(fp,
"#define %n %d\n"
"#define %N &sipStrings_%s[%d]\n"
            , nd, (int)nd->offset
            , nd, mname, (int)nd->offset);
    }

    prcode(fp,
"\n"
"#define sipMalloc                   sipAPI_%s->api_malloc\n"
"#define sipFree                     sipAPI_%s->api_free\n"
"#define sipBuildResult              sipAPI_%s->api_build_result\n"
"#define sipCallMethod               sipAPI_%s->api_call_method\n"
"#define sipParseResult              sipAPI_%s->api_parse_result\n"
"#define sipParseArgs                sipAPI_%s->api_parse_args\n"
"#define sipParsePair                sipAPI_%s->api_parse_pair\n"
"#define sipCommonCtor               sipAPI_%s->api_common_ctor\n"
"#define sipCommonDtor               sipAPI_%s->api_common_dtor\n"
"#define sipConvertFromSequenceIndex sipAPI_%s->api_convert_from_sequence_index\n"
"#define sipConvertFromVoidPtr       sipAPI_%s->api_convert_from_void_ptr\n"
"#define sipConvertToVoidPtr         sipAPI_%s->api_convert_to_void_ptr\n"
"#define sipNoFunction               sipAPI_%s->api_no_function\n"
"#define sipNoMethod                 sipAPI_%s->api_no_method\n"
"#define sipAbstractMethod           sipAPI_%s->api_abstract_method\n"
"#define sipBadClass                 sipAPI_%s->api_bad_class\n"
"#define sipBadSetType               sipAPI_%s->api_bad_set_type\n"
"#define sipBadCatcherResult         sipAPI_%s->api_bad_catcher_result\n"
"#define sipBadOperatorArg           sipAPI_%s->api_bad_operator_arg\n"
"#define sipTrace                    sipAPI_%s->api_trace\n"
"#define sipTransferBack             sipAPI_%s->api_transfer_back\n"
"#define sipTransferTo               sipAPI_%s->api_transfer_to\n"
"#define sipTransferBreak            sipAPI_%s->api_transfer_break\n"
"#define sipSimpleWrapper_Type       sipAPI_%s->api_simplewrapper_type\n"
"#define sipWrapper_Type             sipAPI_%s->api_wrapper_type\n"
"#define sipWrapperType_Type         sipAPI_%s->api_wrappertype_type\n"
"#define sipVoidPtr_Type             sipAPI_%s->api_voidptr_type\n"
"#define sipGetPyObject              sipAPI_%s->api_get_pyobject\n"
"#define sipGetCppPtr                sipAPI_%s->api_get_cpp_ptr\n"
"#define sipGetComplexCppPtr         sipAPI_%s->api_get_complex_cpp_ptr\n"
"#define sipIsPyMethod               sipAPI_%s->api_is_py_method\n"
"#define sipCallHook                 sipAPI_%s->api_call_hook\n"
"#define sipStartThread              sipAPI_%s->api_start_thread\n"
"#define sipEndThread                sipAPI_%s->api_end_thread\n"
"#define sipEmitSignal               sipAPI_%s->api_emit_signal\n"
"#define sipConnectRx                sipAPI_%s->api_connect_rx\n"
"#define sipDisconnectRx             sipAPI_%s->api_disconnect_rx\n"
"#define sipGetSender                sipAPI_%s->api_get_sender\n"
"#define sipRaiseUnknownException    sipAPI_%s->api_raise_unknown_exception\n"
"#define sipRaiseTypeException       sipAPI_%s->api_raise_type_exception\n"
"#define sipBadLengthForSlice        sipAPI_%s->api_bad_length_for_slice\n"
"#define sipAddTypeInstance          sipAPI_%s->api_add_type_instance\n"
"#define sipGetAddress               sipAPI_%s->api_get_address\n"
"#define sipFreeConnection           sipAPI_%s->api_free_connection\n"
"#define sipEmitToSlot               sipAPI_%s->api_emit_to_slot\n"
"#define sipSameConnection           sipAPI_%s->api_same_connection\n"
"#define sipPySlotExtend             sipAPI_%s->api_pyslot_extend\n"
"#define sipConvertRx                sipAPI_%s->api_convert_rx\n"
"#define sipAddDelayedDtor           sipAPI_%s->api_add_delayed_dtor\n"
"#define sipCanConvertToType         sipAPI_%s->api_can_convert_to_type\n"
"#define sipConvertToType            sipAPI_%s->api_convert_to_type\n"
"#define sipForceConvertToType       sipAPI_%s->api_force_convert_to_type\n"
"#define sipReleaseType              sipAPI_%s->api_release_type\n"
"#define sipConvertFromType          sipAPI_%s->api_convert_from_type\n"
"#define sipConvertFromNewType       sipAPI_%s->api_convert_from_new_type\n"
"#define sipConvertFromEnum          sipAPI_%s->api_convert_from_enum\n"
"#define sipGetState                 sipAPI_%s->api_get_state\n"
"#define sipLong_AsUnsignedLong      sipAPI_%s->api_long_as_unsigned_long\n"
"#define sipExportSymbol             sipAPI_%s->api_export_symbol\n"
"#define sipImportSymbol             sipAPI_%s->api_import_symbol\n"
"#define sipRegisterIntTypes         sipAPI_%s->api_register_int_types\n"
"#define sipParseSignature           sipAPI_%s->api_parse_signature\n"
"#define sipFindType                 sipAPI_%s->api_find_type\n"
"#define sipFindNamedEnum            sipAPI_%s->api_find_named_enum\n"
"#define sipString_AsChar            sipAPI_%s->api_string_as_char\n"
"#define sipUnicode_AsWChar          sipAPI_%s->api_unicode_as_wchar\n"
"#define sipUnicode_AsWString        sipAPI_%s->api_unicode_as_wstring\n"
"#define sipConvertFromConstVoidPtr  sipAPI_%s->api_convert_from_const_void_ptr\n"
"#define sipConvertFromVoidPtrAndSize    sipAPI_%s->api_convert_from_void_ptr_and_size\n"
"#define sipConvertFromConstVoidPtrAndSize   sipAPI_%s->api_convert_from_const_void_ptr_and_size\n"
"#define sipInvokeSlot               sipAPI_%s->api_invoke_slot\n"
"#define sipParseType                sipAPI_%s->api_parse_type\n"
"#define sipAssignType               sipAPI_%s->api_assign_type\n"
"#define sipWrappedTypeName(wt)      ((wt)->type->td_cname)\n"
"#define sipDeprecated               sipAPI_%s->api_deprecated\n"
"#define sipRegisterPyType           sipAPI_%s->api_register_py_type\n"
"#define sipTypeFromPyTypeObject     sipAPI_%s->api_type_from_py_type_object\n"
"#define sipTypeScope                sipAPI_%s->api_type_scope\n"
"\n"
"/* These are deprecated. */\n"
"#define sipMapStringToClass         sipAPI_%s->api_map_string_to_class\n"
"#define sipMapIntToClass            sipAPI_%s->api_map_int_to_class\n"
"#define sipFindClass                sipAPI_%s->api_find_class\n"
"#define sipFindMappedType           sipAPI_%s->api_find_mapped_type\n"
"#define sipWrapper_Check(w)         PyObject_TypeCheck((w), sipAPI_%s->api_wrapper_type)\n"
"#define sipGetWrapper(p, wt)        sipGetPyObject((p), (wt)->type)\n"
"#define sipReleaseInstance(p, wt, s)    sipReleaseType((p), (wt)->type, (s))\n"
"#define sipReleaseMappedType        sipReleaseType\n"
"#define sipCanConvertToInstance(o, wt, f)   sipCanConvertToType((o), (wt)->type, (f))\n"
"#define sipCanConvertToMappedType   sipCanConvertToType\n"
"#define sipConvertToInstance(o, wt, t, f, s, e)     sipConvertToType((o), (wt)->type, (t), (f), (s), (e))\n"
"#define sipConvertToMappedType      sipConvertToType\n"
"#define sipForceConvertToInstance(o, wt, t, f, s, e)    sipForceConvertToType((o), (wt)->type, (t), (f), (s), (e))\n"
"#define sipForceConvertToMappedType sipForceConvertToType\n"
"#define sipConvertFromInstance(p, wt, t)    sipConvertFromType((p), (wt)->type, (t))\n"
"#define sipConvertFromMappedType    sipConvertFromType\n"
"#define sipConvertFromNamedEnum(v, pt)  sipConvertFromEnum((v), ((sipEnumTypeObject *)(pt))->type)\n"
"#define sipConvertFromNewInstance(p, wt, t) sipConvertFromNewType((p), (wt)->type, (t))\n"
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname
        ,mname);

    /* The name strings. */
    prcode(fp,
"\n"
"/* The strings used by this module. */\n"
"extern const char *sipStrings_%s;\n"
        , mname);

    /* The unscoped enum macros. */
    generateEnumMacros(pt, mod, NULL, fp);

    generateModuleAPI(pt, mod, fp);

    prcode(fp,
"\n"
"/* The SIP API, this module's API and the APIs of any imported modules. */\n"
"extern const sipAPIDef *sipAPI_%s;\n"
"extern sipExportedModuleDef sipModuleAPI_%s;\n"
        , mname
        , mname, mname);

    for (mld = mod->allimports; mld != NULL; mld = mld->next)
    {
        generateImportedModuleAPI(pt, mod, mld->module, fp);

        prcode(fp,
"extern const sipExportedModuleDef *sipModuleAPI_%s_%s;\n"
            , mname, mld->module->name);
    }

    if (pluginPyQt4(pt))
        prcode(fp,
"\n"
"typedef const QMetaObject *(*sip_qt_metaobject_func)(sipSimpleWrapper *,sipTypeDef *);\n"
"extern sip_qt_metaobject_func sip_%s_qt_metaobject;\n"
"\n"
"typedef int (*sip_qt_metacall_func)(sipSimpleWrapper *,sipTypeDef *,QMetaObject::Call,int,void **);\n"
"extern sip_qt_metacall_func sip_%s_qt_metacall;\n"
"\n"
"typedef int (*sip_qt_metacast_func)(sipSimpleWrapper *,sipTypeDef *,const char *);\n"
"extern sip_qt_metacast_func sip_%s_qt_metacast;\n"
            , mname
            , mname
            , mname);

    /*
     * Note that we don't forward declare the virtual handlers.  This is
     * because we would need to #include everything needed for their argument
     * types.
     */
    prcode(fp,
"\n"
"#endif\n"
        );

    closeFile(fp);
    free(hfile);
}


/*
 * Return the filename of a source code part on the heap.
 */
static char *makePartName(const char *codeDir, const char *mname, int part,
        const char *srcSuffix)
{
    char buf[50];

    sprintf(buf, "part%d", part);

    return concat(codeDir, "/sip", mname, buf, srcSuffix, NULL);
}


/*
 * Generate the C/C++ code for a composite module.
 */
static void generateCompositeCpp(sipSpec *pt, const char *codeDir)
{
    char *cppfile;
    moduleDef *mod;
    FILE *fp;

    cppfile = concat(codeDir, "/sip", pt->module->name, "cmodule.c", NULL);
    fp = createCompilationUnit(pt->module, cppfile, "Composite module code.");

    prcode(fp,
"\n"
"#include <Python.h>\n"
"\n"
"\n"
"static void sip_import_component_module(PyObject *d, const char *name)\n"
"{\n"
"#if PY_VERSION_HEX >= 0x02050000\n"
"    PyObject *mod = PyImport_ImportModule(name);\n"
"#else\n"
"    PyObject *mod = PyImport_ImportModule((char *)name);\n"
"#endif\n"
"\n"
"    /*\n"
"     * Note that we don't complain if the module can't be imported.  This\n"
"     * is a favour to Linux distro packagers who like to split PyQt into\n"
"     * different sub-packages.\n"
"     */\n"
"    if (mod)\n"
"    {\n"
"        PyDict_Merge(d, PyModule_GetDict(mod), 0);\n"
"        Py_DECREF(mod);\n"
"    }\n"
"}\n"
        );

    generateModInitStart(pt->module, TRUE, fp);

    prcode(fp,
"    PyObject *sipModule, *sipModuleDict;\n"
"\n"
"    sipModule = Py_InitModule(\"%s\", 0);\n"
"    sipModuleDict = PyModule_GetDict(sipModule);\n"
"\n"
        , pt->module->fullname->text);

    for (mod = pt->modules; mod != NULL; mod = mod->next)
        if (mod->container == pt->module)
            prcode(fp,
"    sip_import_component_module(sipModuleDict, \"%s\");\n"
                , mod->fullname->text);

    prcode(fp,
"\n"
"    PyErr_Clear();\n"
"}\n"
        );

    closeFile(fp);
    free(cppfile);
}


/*
 * Generate the C/C++ code for a consolidated module.
 */
static void generateConsolidatedCpp(sipSpec *pt, const char *codeDir,
        const char *srcSuffix)
{
    char *cppfile;
    const char *mname = pt->module->name;
    moduleDef *mod;
    FILE *fp;

    cppfile = concat(codeDir, "/sip", mname, "cmodule", srcSuffix, NULL);
    fp = createCompilationUnit(pt->module, cppfile, "Consolidated module code.");

    prcode(fp,
"\n"
"#include <Python.h>\n"
"#include <string.h>\n"
        );

    generateNameCache(pt, fp);

    prcode(fp,
"\n"
"\n"
"/* The component module initialisers. */\n"
        );

    /* Declare the component module initialisers. */
    for (mod = pt->modules; mod != NULL; mod = mod->next)
        if (mod->container == pt->module)
            prcode(fp,
"extern void sip_init_%s(void);\n"
                , mod->name);

    /* Generate the init function. */
    prcode(fp,
"\n"
"\n"
        );

    if (!generating_c)
        prcode(fp,
"extern \"C\" {static PyObject *sip_init(PyObject *, PyObject *);}\n"
            );

    prcode(fp,
"static PyObject *sip_init(PyObject *%s, PyObject *arg)\n"
"{\n"
"    struct component {\n"
"        const char *name;\n"
"        void (*init)(void);\n"
"    };\n"
"\n"
"    static struct component components[] = {\n"
        , (generating_c ? "self" : ""));

    for (mod = pt->modules; mod != NULL; mod = mod->next)
        if (mod->container == pt->module)
            prcode(fp,
"        {%N, sip_init_%s},\n"
                , mod->fullname, mod->name);

    prcode(fp,
"        {NULL, NULL}\n"
"    };\n"
"\n"
"    char *name;\n"
"    struct component *scd;\n"
"\n"
"    if ((name = PyString_AsString(arg)) == NULL)\n"
"        return NULL;\n"
"\n"
"    for (scd = components; scd->name != NULL; ++scd)\n"
"        if (strcmp(scd->name, name) == 0)\n"
"        {\n"
"            (*scd->init)();\n"
"\n"
"            Py_INCREF(Py_True);\n"
"            return Py_True;\n"
"        }\n"
"\n"
"    PyErr_Format(PyExc_ImportError, \"unknown component module %%s\", name);\n"
"\n"
"    return NULL;\n"
"}\n"
        );

    generateModInitStart(pt->module, generating_c, fp);

    prcode(fp,
"    static PyMethodDef sip_methods[] = {\n"
"        {\"init\", sip_init, METH_O, NULL},\n"
"        {NULL, NULL, 0, NULL}\n"
"    };\n"
"\n"
"    Py_InitModule(\"%s\", sip_methods);\n"
"}\n"
        , mname);

    closeFile(fp);
    free(cppfile);
}


/*
 * Generate the C/C++ code for a component module.
 */
static void generateComponentCpp(sipSpec *pt, const char *codeDir,
        const char *consModule)
{
    char *cppfile;
    FILE *fp;

    cppfile = concat(codeDir, "/sip", pt->module->name, "cmodule.c", NULL);
    fp = createCompilationUnit(pt->module, cppfile, "Component module code.");

    prcode(fp,
"\n"
"#include <Python.h>\n"
        );

    generateModInitStart(pt->module, TRUE, fp);

    prcode(fp,
"    PyObject *sip_mod, *sip_result;\n"
"\n"
"    /* Import the consolidated module. */\n"
"    if ((sip_mod = PyImport_ImportModule(\"%s\")) == NULL)\n"
"        return;\n"
"\n"
        , consModule);

    prcode(fp,
"    /* Ask the consolidated module to do the initialistion. */\n"
"    sip_result = PyObject_CallMethod(sip_mod, \"init\", \"s\", \"%s\");\n"
"\n"
"    Py_XDECREF(sip_result);\n"
"    Py_DECREF(sip_mod);\n"
"}\n"
        , pt->module->fullname->text);

    closeFile(fp);
    free(cppfile);
}


/*
 * Generate the name cache definition.
 */
static void generateNameCache(sipSpec *pt, FILE *fp)
{
    nameDef *nd;

    prcode(fp,
"\n"
"/* Define the strings used by this module. */\n"
"const char *sipStrings_%s =", pt->module->name);

    for (nd = pt->namecache; nd != NULL; nd = nd->next)
    {
        if (!isUsedName(nd) || isSubstring(nd))
            continue;

        prcode(fp, "\n"
"    \"%s\\0\"", nd->text);
    }

    prcode(fp, ";\n");
}


/*
 * Generate the C/C++ code.
 */
static void generateCpp(sipSpec *pt, moduleDef *mod, const char *codeDir,
        const char *srcSuffix, int parts, stringList *xsl)
{
    char *cppfile;
    const char *mname = mod->name;
    int nrSccs = 0, files_in_part, max_per_part, this_part, mod_nr;
    int is_inst_class, is_inst_voidp, is_inst_char, is_inst_string;
    int is_inst_int, is_inst_long, is_inst_ulong, is_inst_longlong;
    int is_inst_ulonglong, is_inst_double, nr_enummembers;
    int hasexternal = FALSE, slot_extenders = FALSE, ctor_extenders = FALSE;
    FILE *fp;
    moduleListDef *mld;
    classDef *cd;
    memberDef *md;
    ifaceFileDef *iff;
    virtHandlerDef *vhd;
    exceptionDef *xd;

    /* Calculate the number of files in each part. */
    if (parts)
    {
        int nr_files = 1;

        for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
            if (iff->module == mod)
                ++nr_files;

        max_per_part = (nr_files + parts - 1) / parts;
        files_in_part = 1;
        this_part = 0;

        cppfile = makePartName(codeDir, mname, 0, srcSuffix);
    }
    else
        cppfile = concat(codeDir, "/sip", mname, "cmodule", srcSuffix, NULL);

    fp = createCompilationUnit(mod, cppfile, "Module code.");

    prcode(fp,
"\n"
"#include \"sipAPI%s.h\"\n"
        , mname);

    /*
     * Include the library headers for types used by virtual handlers, module
     * level functions, module level variables and Qt meta types.
     */
    generateUsedIncludes(mod->used, fp);

    /*
     * If there should be a Qt support API then generate stubs values for the
     * optional parts.  These should be undefined in %ModuleCode if a C++
     * implementation is provided.
     */
    if (mod->qobjclass >= 0)
        prcode(fp,
"\n"
"#define sipQtIsQtSignal                     0\n"
"#define sipQtCreateUniversalSignalShortcut  0\n"
"#define sipQtCreateUniversalSignal          0\n"
"#define sipQtFindUniversalSignalShortcut    0\n"
"#define sipQtFindUniversalSignal            0\n"
"#define sipQtEmitSignalShortcut             0\n"
"#define sipQtEmitSignal                     0\n"
"#define sipQtCreateUniversalSlotEx          0\n"
            );

    /* Define the names. */
    if (mod->container == NULL)
        generateNameCache(pt, fp);

    /* Generate the C++ code blocks. */
    generateCppCodeBlock(mod->cppcode, fp);

    /* Generate any virtual handler declarations. */
    for (vhd = mod->virthandlers; vhd != NULL; vhd = vhd->next)
        if (!isDuplicateVH(vhd))
            generateVirtualHandler(vhd, fp);

    /* Generate the global functions. */
    for (md = mod->othfuncs; md != NULL; md = md->next)
        if (md->slot == no_slot)
            generateOrdinaryFunction(mod, NULL, md, fp);
        else
        {
            overDef *od;

            /*
             * Make sure that there is still an overload and we haven't moved
             * them all to classes.
             */
            for (od = mod->overs; od != NULL; od = od->next)
                if (od->common == md)
                {
                    generateSlot(mod, NULL, NULL, md, fp);
                    slot_extenders = TRUE;
                    break;
                }
        }

    /* Generate any class specific ctor or slot extenders. */
    for (cd = mod->proxies; cd != NULL; cd = cd->next)
    {
        if (cd->ctors != NULL)
        {
            generateTypeInit(cd, fp);
            ctor_extenders = TRUE;
        }

        for (md = cd->members; md != NULL; md = md->next)
        {
            generateSlot(mod, cd, NULL, md, fp);
            slot_extenders = TRUE;
        }
    }

    /* Generate any ctor extender table. */
    if (ctor_extenders)
    {
        prcode(fp,
"\n"
"static sipInitExtenderDef initExtenders[] = {\n"
            );

        for (cd = mod->proxies; cd != NULL; cd = cd->next)
            if (cd->ctors != NULL)
            {
                prcode(fp,
"    {init_%C, ", classFQCName(cd));

                generateEncodedClass(mod, cd, 0, fp);

                prcode(fp, ", NULL},\n"
                    );
            }

        prcode(fp,
"    {NULL, {0, 0, 0}, NULL}\n"
"};\n"
            );
    }

    /* Generate any slot extender table. */
    if (slot_extenders)
    {
        prcode(fp,
"\n"
"static sipPySlotExtenderDef slotExtenders[] = {\n"
            );

        for (md = mod->othfuncs; md != NULL; md = md->next)
        {
            overDef *od;

            if (md->slot == no_slot)
                continue;

            for (od = mod->overs; od != NULL; od = od->next)
                if (od->common == md)
                {
                    prcode(fp,
"    {(void *)slot_%s, %s, {0, 0, 0}},\n"
                        , md->pyname->text, slotName(md->slot));

                    break;
                }
        }

        for (cd = mod->proxies; cd != NULL; cd = cd->next)
            for (md = cd->members; md != NULL; md = md->next)
            {
                prcode(fp,
"    {(void *)slot_%C_%s, %s, ", classFQCName(cd), md->pyname->text, slotName(md->slot));

                generateEncodedClass(mod, cd, 0, fp);

                prcode(fp, "},\n"
                      );
            }

        prcode(fp,
"    {NULL, (sipPySlotType)0, {0, 0, 0}}\n"
"};\n"
            );
    }

    /* Generate the global access functions. */
    generateAccessFunctions(pt, mod, NULL, fp);

    /* Generate the module data structures. */
    if (mod->nrclasses > 0)
    {
        nrSccs = generateSubClassConvertors(pt, mod, fp);

        prcode(fp,
"\n"
"\n"
"/*\n"
" * This defines each class in this module.\n"
" */\n"
"static sipTypeDef *typesTable[] = {\n"
            );

        generateClassTableEntries(pt, mod, &mod->root, fp);

        prcode(fp,
"};\n"
            );

        /* Generate the external classes table if needed. */
        for (cd = pt->classes; cd != NULL; cd = cd->next)
        {
            if (!isExternal(cd))
                continue;

            if (cd->iff->module != mod)
                continue;

            if (!hasexternal)
            {
                prcode(fp,
"\n"
"\n"
"/* This defines each external type declared in this module, */\n"
"static sipExternalTypeDef externalTypesTable[] = {\n"
                    );

                hasexternal = TRUE;
            }

            prcode(fp,
"    {%d, \"", cd->classnr);
            prScopedName(fp, classFQCName(cd), ".");
            prcode(fp,"\"},\n"
                );
        }

        if (hasexternal)
            prcode(fp,
"    {-1, NULL}\n"
"};\n"
                );
    }

    if (mod->nrmappedtypes > 0)
    {
        mappedTypeDef *mtd;
        argDef type;

        memset(&type, 0, sizeof (argDef));

        prcode(fp,
"\n"
"\n"
"/* This defines each mapped type in this module. */\n"
"static sipTypeDef *mappedTypesTable[] = {\n"
            );

        for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        {
            if (mtd->iff->module != mod)
                continue;

            type.atype = mapped_type;
            type.u.mtd = mtd;

            prcode(fp,
"    &sipMappedTypeDef_%T.mtd_base,\n"
                , &type);
        }

        prcode(fp,
"    0\n"
"};\n"
            );
    }

    if (mod->nrenums > 0)
    {
        int i;
        enumDef *ed;

        prcode(fp,
"\n"
"\n"
"/* Define each named enum in this module. */\n"
            );

        /* Generate any slot tables. */
        for (ed = pt->enums; ed != NULL; ed = ed->next)
        {
            memberDef *slot;

            if (ed->module != mod || ed->fqcname == NULL)
                continue;

            if (ed->slots == NULL)
                continue;

            for (slot = ed->slots; slot != NULL; slot = slot->next)
                generateSlot(mod, NULL, ed, slot, fp);

            prcode(fp,
"\n"
"static sipPySlotDef slots_%C[] = {\n"
                , ed->fqcname);

            for (slot = ed->slots; slot != NULL; slot = slot->next)
            {
                const char *stype;

                if ((stype = slotName(slot->slot)) != NULL)
                    prcode(fp,
"    {(void *)slot_%C_%s, %s},\n"
                        , ed->fqcname, slot->pyname->text, stype);
            }

            prcode(fp,
"    {0, (sipPySlotType)0}\n"
"};\n"
"\n"
                );
        }

        prcode(fp,
"static sipEnumTypeDef enumTypes[] = {\n"
            );

        for (ed = pt->enums; ed != NULL; ed = ed->next)
        {
            if (ed->module != mod || ed->fqcname == NULL)
                continue;

            if (ed->ecd != NULL && isTemplateClass(ed->ecd))
                continue;

            prcode(fp,
"    {{-1, 0, 0, SIP_TYPE_ENUM, %n, {0}}, %n, ", ed->cname, ed->pyname);

            if (ed->ecd == NULL)
                prcode(fp, "-1");
            else
                prcode(fp, "%d", ed->ecd->classnr);

            if (ed->slots != NULL)
                prcode(fp, ", slots_%C", ed->fqcname);
            else
                prcode(fp, ", NULL");

            prcode(fp, "},\n"
                );
        }

        prcode(fp,
"};\n"
            );

        prcode(fp,
"static sipTypeDef *enumTypesTable[] = {\n"
            );

        for (i = 0, ed = pt->enums; ed != NULL; ed = ed->next, ++i)
        {
            if (ed->module != mod || ed->fqcname == NULL)
                continue;

            if (ed->ecd != NULL && isTemplateClass(ed->ecd))
                continue;

            prcode(fp,
"    &enumTypes[%d].etd_base,\n"
                , i);
        }

        prcode(fp,
"};\n"
            );

        nr_enummembers = generateEnumMemberTable(pt, mod, NULL, fp);
    }
    else
        nr_enummembers = 0;

    if (mod->nrtypedefs > 0)
    {
        typedefDef *td;

        prcode(fp,
"\n"
"\n"
"/*\n"
" * These define each typedef in this module.  They are only needed in case\n"
" * they are used as arguments to Qt signals.\n"
" */\n"
"\n"
"static sipTypedefDef typedefsTable[] = {\n"
            );

        for (td = pt->typedefs; td != NULL; td = td->next)
        {
            const char *tdmname, *sat;
            scopedNameDef *fqname;
            argDef *argtype;

            if (td->module != mod)
                continue;

            fqname = NULL;
            tdmname = NULL;
            argtype = NULL;
            sat = "unknown";

            switch (td->type.atype)
            {
            case string_type:
                sat = (td->type.nrderefs == 0 ? "char" : "string");
                break;

            case sstring_type:
                sat = (td->type.nrderefs == 0 ? "schar" : "sstring");
                break;

            case ustring_type:
                sat = (td->type.nrderefs == 0 ? "uchar" : "ustring");
                break;

            case wstring_type:
                sat = (td->type.nrderefs == 0 ? "wchar" : "wstring");
                break;

            case short_type:
                sat = "short";
                break;

            case ushort_type:
                sat = "ushort";
                break;

            case cint_type:
            case int_type:
                sat = "int";
                break;

            case uint_type:
                sat = "uint";
                break;

            case long_type:
                sat = "long";
                break;

            case ulong_type:
                sat = "ulong";
                break;

            case longlong_type:
                sat = "longlong";
                break;

            case ulonglong_type:
                sat = "ulonglong";
                break;

            case cfloat_type:
            case float_type:
                sat = "float";
                break;

            case cdouble_type:
            case double_type:
                sat = "double";
                break;

            case bool_type:
            case cbool_type:
                sat = "bool";
                break;

            case fake_void_type:
            case void_type:
                if (td->type.nrderefs != 0)
                    sat = "void";
                break;

            case enum_type:
                if ((fqname = td->type.u.ed->fqcname) != NULL)
                {
                    sat = "enum";

                    if (td->type.u.ed->module != mod)
                        tdmname = td->type.u.ed->module->fullname->text;
                }
                break;

            case class_type:
                sat = "class";
                fqname = classFQCName(td->type.u.cd);

                if (td->type.u.cd->iff->module != mod)
                    tdmname = td->type.u.cd->iff->module->fullname->text;
                break;

            case mapped_type:
                sat = "mtype";
                argtype = &td->type.u.mtd->type;

                if (td->type.u.mtd->iff->module != mod)
                    tdmname = td->type.u.mtd->iff->module->fullname->text;
                break;
            }

            prcode(fp,
"    {\"%S\", %s_sat", td->fqname, sat);

            if (argtype != NULL)
                prcode(fp, ", \"%b\"", argtype);
            else if (fqname != NULL)
                prcode(fp, ", \"%S\"", fqname);
            else
                prcode(fp, ", NULL");

            if (tdmname != NULL)
                prcode(fp, ", \"%s\"", tdmname);
            else
                prcode(fp, ", NULL");

            prcode(fp, "},\n"
                );
        }

        prcode(fp,
"    {NULL, unknown_sat, NULL, NULL}\n"
"};\n"
            );
    }

    if (mod->nrvirthandlers > 0)
    {
        prcode(fp,
"\n"
"\n"
"/*\n"
" * This defines the virtual handlers that this module implements and can be\n"
" * used by other modules.\n"
" */\n"
"static sipVirtHandlerFunc virtHandlersTable[] = {\n"
            );

        for (vhd = mod->virthandlers; vhd != NULL; vhd = vhd->next)
            if (!isDuplicateVH(vhd))
                prcode(fp,
"    (sipVirtHandlerFunc)sipVH_%s_%d,\n"
                    , mname, vhd->virthandlernr);

        prcode(fp,
"};\n"
            );
    }

    if (mod->allimports != NULL)
    {
        prcode(fp,
"\n"
"\n"
"/* This defines the modules that this module needs to import. */\n"
"static sipImportedModuleDef importsTable[] = {\n"
            );

        for (mld = mod->allimports; mld != NULL; mld = mld->next)
            prcode(fp,
"    {\"%s\", %d, NULL},\n"
                , mld->module->fullname->text, mld->module->version);

        prcode(fp,
"    {NULL, -1, NULL}\n"
"};\n"
            );
    }

    if (nrSccs > 0)
    {
        prcode(fp,
"\n"
"\n"
"/* This defines the class sub-convertors that this module defines. */\n"
"static sipSubClassConvertorDef convertorsTable[] = {\n"
            );

        for (cd = pt->classes; cd != NULL; cd = cd->next)
        {
            if (cd->iff->module != mod)
                continue;

            if (cd->convtosubcode == NULL)
                continue;

            prcode(fp,
"    {sipSubClass_%C, ",classFQCName(cd));

            generateEncodedClass(mod, cd->subbase, 0, fp);

            prcode(fp,", NULL},\n");
        }

        prcode(fp,
"    {NULL, {0, 0, 0}, NULL}\n"
"};\n"
            );
    }

    /* Generate any license information. */
    if (mod->license != NULL)
    {
        licenseDef *ld = mod->license;

        prcode(fp,
"\n"
"\n"
"/* Define the module's license. */\n"
"static sipLicenseDef module_license = {\n"
            );

        prcode(fp,
"    \"%s\",\n"
            , ld->type);

        if (ld->licensee != NULL)
            prcode(fp,
"    \"%s\",\n"
                , ld->licensee);
        else
            prcode(fp,
"    NULL,\n"
                );

        if (ld->timestamp != NULL)
            prcode(fp,
"    \"%s\",\n"
                , ld->timestamp);
        else
            prcode(fp,
"    NULL,\n"
                );

        if (ld->sig != NULL)
            prcode(fp,
"    \"%s\"\n"
                , ld->sig);
        else
            prcode(fp,
"    NULL\n"
                );

        prcode(fp,
"};\n"
            );
    }

    /* Generate each instance table. */
    is_inst_class = generateClasses(pt, mod, NULL, fp);
    is_inst_voidp = generateVoidPointers(pt, mod, NULL, fp);
    is_inst_char = generateChars(pt, mod, NULL, fp);
    is_inst_string = generateStrings(pt, mod, NULL, fp);
    is_inst_int = generateInts(pt, mod, NULL, fp);
    is_inst_long = generateLongs(pt, mod, NULL, fp);
    is_inst_ulong = generateUnsignedLongs(pt, mod, NULL, fp);
    is_inst_longlong = generateLongLongs(pt, mod, NULL, fp);
    is_inst_ulonglong = generateUnsignedLongLongs(pt, mod, NULL, fp);
    is_inst_double = generateDoubles(pt, mod, NULL, fp);

    /* Generate any exceptions table. */
    if (mod->nrexceptions > 0)
           prcode(fp,
"\n"
"\n"
"static PyObject *exceptionsTable[%d];\n"
            , mod->nrexceptions);

    /* Generate any Qt support API. */
    if (mod->qobjclass >= 0)
        prcode(fp,
"\n"
"\n"
"/* This defines the Qt support API. */\n"
"\n"
"static sipQtAPI qtAPI = {\n"
"    &typesTable[%d],\n"
"    sipQtIsQtSignal,\n"
"    sipQtCreateUniversalSignalShortcut,\n"
"    sipQtCreateUniversalSignal,\n"
"    sipQtFindUniversalSignalShortcut,\n"
"    sipQtFindUniversalSignal,\n"
"    sipQtEmitSignalShortcut,\n"
"    sipQtEmitSignal,\n"
"    sipQtCreateUniversalSlot,\n"
"    sipQtDestroyUniversalSlot,\n"
"    sipQtFindSlot,\n"
"    sipQtConnect,\n"
"    sipQtDisconnect,\n"
"    sipQtSignalsBlocked,\n"
"    sipQtGetSender,\n"
"    sipQtForgetSender,\n"
"    sipQtSameSignalSlotName,\n"
"    sipQtFindConnection,\n"
"    sipQtCreateUniversalSlotEx\n"
"};\n"
            , mod->qobjclass);

    prcode(fp,
"\n"
"\n"
"/* This defines this module. */\n"
"sipExportedModuleDef sipModuleAPI_%s = {\n"
"    0,\n"
"    SIP_API_MINOR_NR,\n"
"    %n,\n"
"    0,\n"
"    %d,\n"
"    sipStrings_%s,\n"
"    %s,\n"
"    %s,\n"
"    %d,\n"
"    %s,\n"
"    %s,\n"
"    %d,\n"
"    %s,\n"
"    %d,\n"
"    %s,\n"
"    %d,\n"
"    %s,\n"
"    %s,\n"
"    %s,\n"
"    %s,\n"
"    {%s, %s, %s, %s, %s, %s, %s, %s, %s, %s},\n"
"    %s,\n"
"    %s,\n"
"    %s,\n"
"    %s,\n"
"    %s,\n"
"    0\n"
"};\n"
        , mname
        , mod->fullname
        , mod->version
        , mname
        , mod->allimports != NULL ? "importsTable" : "NULL"
        , mod->qobjclass >= 0 ? "&qtAPI" : "NULL"
        , mod->nrclasses
        , mod->nrclasses > 0 ? "typesTable" : "NULL"
        , hasexternal ? "externalTypesTable" : "NULL"
        , mod->nrmappedtypes
        , mod->nrmappedtypes > 0 ? "mappedTypesTable" : "NULL"
        , mod->nrenums
        , mod->nrenums > 0 ? "enumTypesTable" : "NULL"
        , nr_enummembers
        , nr_enummembers > 0 ? "enummembers" : "NULL"
        , mod->nrtypedefs > 0 ? "typedefsTable" : "NULL"
        , mod->nrvirthandlers > 0 ? "virtHandlersTable" : "NULL"
        , nrSccs > 0 ? "convertorsTable" : "NULL"
        , is_inst_class ? "typeInstances" : "NULL"
        , is_inst_voidp ? "voidPtrInstances" : "NULL"
        , is_inst_char ? "charInstances" : "NULL"
        , is_inst_string ? "stringInstances" : "NULL"
        , is_inst_int ? "intInstances" : "NULL"
        , is_inst_long ? "longInstances" : "NULL"
        , is_inst_ulong ? "unsignedLongInstances" : "NULL"
        , is_inst_longlong ? "longLongInstances" : "NULL"
        , is_inst_ulonglong ? "unsignedLongLongInstances" : "NULL"
        , is_inst_double ? "doubleInstances" : "NULL"
        , mod->license != NULL ? "&module_license" : "NULL"
        , mod->nrexceptions > 0 ? "exceptionsTable" : "NULL"
        , slot_extenders ? "slotExtenders" : "NULL"
        , ctor_extenders ? "initExtenders" : "NULL"
        , hasDelayedDtors(mod) ? "sipDelayedDtors" : "NULL");

    /* Generate the storage for the external API pointers. */
    prcode(fp,
"\n"
"\n"
"/* The SIP API and the APIs of any imported modules. */\n"
"const sipAPIDef *sipAPI_%s;\n"
        , mname);

    for (mld = mod->allimports; mld != NULL; mld = mld->next)
        prcode(fp,
"const sipExportedModuleDef *sipModuleAPI_%s_%s;\n"
            , mname, mld->module->name);

    if (pluginPyQt4(pt))
        prcode(fp,
"\n"
"sip_qt_metaobject_func sip_%s_qt_metaobject;\n"
"sip_qt_metacall_func sip_%s_qt_metacall;\n"
"sip_qt_metacast_func sip_%s_qt_metacast;\n"
            , mname
            , mname
            , mname);

    /* Generate the Python module initialisation function. */

    if (mod->container == pt->module)
        prcode(fp,
"void sip_init_%s()\n"
"{\n"
            , mname);
    else
        generateModInitStart(pt->module, generating_c, fp);

    /* Generate the global functions. */

    prcode(fp,
"    static PyMethodDef sip_methods[] = {\n"
        );

    for (md = mod->othfuncs; md != NULL; md = md->next)
        if (md->slot == no_slot)
            if (noArgParser(md))
                prcode(fp,
"        {%N, (PyCFunction)func_%s, METH_KEYWORDS, NULL},\n"
                    , md->pyname, md->pyname->text);
            else
                prcode(fp,
"        {%N, func_%s, METH_VARARGS, NULL},\n"
                    , md->pyname, md->pyname->text);

    prcode(fp,
"        {0, 0, 0, 0}\n"
"    };\n"
"\n"
"    PyObject *sipModule, *sipModuleDict;\n"
        );

    generateSipImportVariables(fp);

    /* Generate any pre-initialisation code. */
    generateCppCodeBlock(mod->preinitcode, fp);

    prcode(fp,
"    /* Initialise the module and get it's dictionary. */\n"
"    sipModule = Py_InitModule((char *)%N,sip_methods);\n"
"    sipModuleDict = PyModule_GetDict(sipModule);\n"
"\n"
        , mod->fullname);

    generateSipImport(mod, fp);

    /* Generate any initialisation code. */
    generateCppCodeBlock(mod->initcode, fp);

    prcode(fp,
"    /* Export the module and publish it's API. */\n"
"    if (sipAPI_%s->api_export_module(&sipModuleAPI_%s,SIP_API_MAJOR_NR,SIP_API_MINOR_NR,0) < 0)\n"
"       return;\n"
        , mname
        , mname);

    if (pluginPyQt4(pt))
    {
        int noIntro;

        /* Import the helpers. */
        prcode(fp,
"\n"
"    sip_%s_qt_metaobject = (sip_qt_metaobject_func)sipImportSymbol(\"qtcore_qt_metaobject\");\n"
"    sip_%s_qt_metacall = (sip_qt_metacall_func)sipImportSymbol(\"qtcore_qt_metacall\");\n"
"    sip_%s_qt_metacast = (sip_qt_metacast_func)sipImportSymbol(\"qtcore_qt_metacast\");\n"
"\n"
            , mname
            , mname
            , mname);

        /* Generate any Qt meta-type registration calls. */
        noIntro = TRUE;

        for (cd = pt->classes; cd != NULL; cd = cd->next)
            if (cd->iff->module == mod)
                if (registerQtMetaType(cd))
                {
                    if (noIntro)
                    {
                        prcode(fp,
"    typedef void (*sip_qt_register_func)(int,const sipTypeDef *);\n"
"    sip_qt_register_func sip_qt_register = (sip_qt_register_func)sipImportSymbol(\"qtcore_qt_register\");\n"
"\n"
                            );

                        noIntro = FALSE;
                    }

                    prcode(fp,
"    sip_qt_register(qRegisterMetaType<%S>(%N), sipType_%C);\n"
                        , classFQCName(cd), cd->iff->name, classFQCName(cd));
                }

        /*
         * FIXME: Generate registration calls for any mapped type that has
         * asked for it.
         */
    }

    prcode(fp,
"    /* Initialise the module now all its dependencies have been set up. */\n"
"    if (sipAPI_%s->api_init_module(&sipModuleAPI_%s,sipModuleDict) < 0)\n"
"       return;\n"
        , mname
        , mname);

    mod_nr = 0;

    for (mld = mod->allimports; mld != NULL; mld = mld->next)
    {
        if (mod_nr == 0)
            prcode(fp,
"\n"
"    /* Get the APIs of the modules that this one is dependent on. */\n"
                );

        prcode(fp,
"    sipModuleAPI_%s_%s = sipModuleAPI_%s.em_imports[%d].im_module;\n"
            , mname, mld->module->name, mname, mod_nr);

        ++mod_nr;
    }

    generateTypesInline(pt, mod, fp);

    /* Create any exceptions. */
    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
    {
        if (xd->iff->module != mod)
            continue;

        if (xd->iff->type != exception_iface)
            continue;

        if (xd->exceptionnr < 0)
            continue;

        prcode(fp,
"\n"
"    if ((exceptionsTable[%d] = PyErr_NewException((char *)\"%s.%s\",", xd->exceptionnr, xd->iff->module->name, xd->pyname);

        if (xd->bibase != NULL)
            prcode(fp, "PyExc_%s", xd->bibase);
        else if (xd->base->iff->module == mod)
            prcode(fp, "exceptionsTable[%d]", xd->base->exceptionnr);
        else
            prcode(fp, "sipException_%C", xd->base->iff->fqcname);

        prcode(fp, ",NULL)) == NULL || PyDict_SetItemString(sipModuleDict,\"%s\",exceptionsTable[%d]) < 0)\n"
"        return;\n"
            , xd->pyname, xd->exceptionnr);
    }

    /* Generate any post-initialisation code. */
    generateCppCodeBlock(mod->postinitcode, fp);

    prcode(fp,
"}\n"
        );

    /* Generate the interface source files. */
    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
        if (iff->module == mod && iff->type != exception_iface)
        {
            if (parts && files_in_part++ == max_per_part)
            {
                /* Close the old part. */
                closeFile(fp);
                free(cppfile);

                /* Create a new one. */
                files_in_part = 1;
                ++this_part;

                cppfile = makePartName(codeDir, mname, this_part, srcSuffix);
                fp = createCompilationUnit(mod, cppfile, "Module code.");

                prcode(fp,
"\n"
"#include \"sipAPI%s.h\"\n"
                    , mname);
            }

            generateIfaceCpp(pt, iff, codeDir, srcSuffix, (parts ? fp : NULL));
        }

    closeFile(fp);
    free(cppfile);

    /* How many parts we actually generated. */
    if (parts)
        parts = this_part + 1;

    mod->parts = parts;

    generateInternalAPIHeader(pt, mod, codeDir, xsl);
}


/*
 * Generate the code to import the sip module and get its API.
 */
static void generateSipImport(moduleDef *mod, FILE *fp)
{
    prcode(fp,
"    /* Import the SIP module and get it's API. */\n"
"#if PY_VERSION_HEX >= 0x02050000\n"
"    sip_sipmod = PyImport_ImportModule(\"sip\");\n"
"#else\n"
"    sip_sipmod = PyImport_ImportModule((char *)\"sip\");\n"
"#endif\n"
"\n"
"    if (sip_sipmod == NULL)\n"
"        return;\n"
"\n"
"    sip_capiobj = PyDict_GetItemString(PyModule_GetDict(sip_sipmod), \"_C_API\");\n"
"\n"
"    if (sip_capiobj == NULL || !PyCObject_Check(sip_capiobj))\n"
"        return;\n"
"\n"
        );

    if (generating_c)
        prcode(fp,
"    sipAPI_%s = (const sipAPIDef *)PyCObject_AsVoidPtr(sip_capiobj);\n"
        , mod->name);
    else
        prcode(fp,
"    sipAPI_%s = reinterpret_cast<const sipAPIDef *>(PyCObject_AsVoidPtr(sip_capiobj));\n"
        , mod->name);

    prcode(fp,
"\n"
        );
}


/*
 * Generate the variables needed by generateSipImport().
 */
static void generateSipImportVariables(FILE *fp)
{
    prcode(fp,
"    PyObject *sip_sipmod, *sip_capiobj;\n"
"\n"
        );
}


/*
 * Generate the start of the Python module initialisation function.
 */
static void generateModInitStart(moduleDef *mod, int gen_c, FILE *fp)
{
    prcode(fp,
"\n"
"\n"
"/* The Python module initialisation function. */\n"
"#if defined(SIP_STATIC_MODULE)\n"
"%svoid init%s()\n"
"#else\n"
"PyMODINIT_FUNC init%s()\n"
"#endif\n"
"{\n"
        , (gen_c ? "" : "extern \"C\" "), mod->name
        , mod->name);
}


/*
 * Generate all the sub-class convertors for a module.
 */
static int generateSubClassConvertors(sipSpec *pt, moduleDef *mod, FILE *fp)
{
    int nrSccs = 0;
    classDef *cd;

    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        int needs_sipClass;

        if (cd->iff->module != mod)
            continue;

        if (cd->convtosubcode == NULL)
            continue;

        prcode(fp,
"\n"
"\n"
"/* Convert to a sub-class if possible. */\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static const sipTypeDef *sipSubClass_%C(void **);}\n"
                , classFQCName(cd));

        /* Allow the deprecated use of sipClass rather than sipType. */
        needs_sipClass = usedInCode(cd->convtosubcode, "sipClass");

        prcode(fp,
"static const sipTypeDef *sipSubClass_%C(void **sipCppRet)\n"
"{\n"
"    %S *sipCpp = reinterpret_cast<%S *>(*sipCppRet);\n"
            , classFQCName(cd)
            , classFQCName(cd->subbase), classFQCName(cd->subbase));

        if (needs_sipClass)
            prcode(fp,
"    sipWrapperType *sipClass;\n"
"\n"
                );
        else
            prcode(fp,
"    const sipTypeDef *sipType;\n"
"\n"
                );

        generateCppCodeBlock(cd->convtosubcode, fp);

        if (needs_sipClass)
            prcode(fp,
"\n"
"    return (sipClass ? sipClass->type : 0);\n"
"}\n"
                );
        else
            prcode(fp,
"\n"
"    return sipType;\n"
"}\n"
                );

        ++nrSccs;
    }

    return nrSccs;
}


/*
 * Generate an entry for a class in the classes table and all its children.
 */
static void generateClassTableEntries(sipSpec *pt, moduleDef *mod, nodeDef *nd,
        FILE *fp)
{
    nodeDef *cnd;

    /* Generate the entry if it's not the root. */
    if (nd->cd != NULL)
    {
        if (isExternal(nd->cd))
            prcode(fp,
"    0,\n"
                );
        else
        {
            const char *type_prefix, *type_suffix;

            type_suffix = ".super";

            if (pluginPyQt4(pt))
                type_prefix = "pyqt4";
            else
            {
                type_prefix = "sip";
                type_suffix = "";
            }

            prcode(fp,
"    &%sType_%s_%C%s.ctd_base,\n"
                , type_prefix, mod->name, classFQCName(nd->cd), type_suffix);
        }
    }

    /* Generate all it's children. */
    for (cnd = nd->child; cnd != NULL; cnd = cnd->next)
        generateClassTableEntries(pt, mod, cnd, fp);
}


/*
 * Generate the structure representing an encoded class.
 */
static void generateEncodedClass(moduleDef *mod, classDef *cd, int last,
        FILE *fp)
{
    moduleDef *cmod = cd->iff->module;

    prcode(fp, "{%u, ", cd->classnr);

    if (cmod == mod)
        prcode(fp, "255");
    else
    {
        int mod_nr = 0;
        moduleListDef *mld;

        for (mld = mod->allimports; mld != NULL; mld = mld->next)
        {
            if (mld->module == cmod)
            {
                prcode(fp, "%u", mod_nr);
                break;
            }

            ++mod_nr;
        }
    }

    prcode(fp, ", %u}", last);
}


/*
 * Generate an ordinary function.
 */
static void generateOrdinaryFunction(moduleDef *mod, classDef *cd,
        memberDef *md, FILE *fp)
{
    overDef *od;
    int need_intro;

    prcode(fp,
"\n"
"\n"
        );

    if (cd != NULL)
    {
        if (!generating_c)
            prcode(fp,
"extern \"C\" {static PyObject *meth_%C_%s(PyObject *,PyObject *);}\n"
                , classFQCName(cd), md->pyname->text);

        prcode(fp,
"static PyObject *meth_%C_%s(PyObject *,PyObject *sipArgs)\n"
            ,classFQCName(cd),md->pyname->text);

        od = cd->overs;
    }
    else
    {
        const char *self = (generating_c ? "sipSelf" : "");

        if (!generating_c)
        {
            if (noArgParser(md))
                prcode(fp,
"extern \"C\" {static PyObject *func_%s(PyObject *,PyObject *,PyObject *);}\n"
                    , md->pyname->text);
            else
                prcode(fp,
"extern \"C\" {static PyObject *func_%s(PyObject *,PyObject *);}\n"
                    , md->pyname->text);
        }

        if (noArgParser(md))
            prcode(fp,
"static PyObject *func_%s(PyObject *%s,PyObject *sipArgs,PyObject *sipKwds)\n"
                , md->pyname->text, self);
        else
            prcode(fp,
"static PyObject *func_%s(PyObject *%s,PyObject *sipArgs)\n"
                , md->pyname->text, self);

        od = mod->overs;
    }

    prcode(fp,
"{\n"
        );

    need_intro = TRUE;

    while (od != NULL)
    {
        if (od->common == md)
        {
            if (noArgParser(md))
            {
                generateCppCodeBlock(od->methodcode, fp);
                break;
            }

            if (need_intro)
            {
                prcode(fp,
"    int sipArgsParsed = 0;\n"
                    );

                need_intro = FALSE;
            }

            generateFunctionBody(od, cd, cd, TRUE, fp);
        }

        od = od->next;
    }

    if (!need_intro)
        prcode(fp,
"\n"
"    /* Raise an exception if the arguments couldn't be parsed. */\n"
"    sipNoFunction(sipArgsParsed,%N);\n"
"\n"
"    return NULL;\n"
            ,md->pyname);

    prcode(fp,
"}\n"
        );
}


/*
 * Generate the table of enum members for a scope.  Return the number of them.
 */
static int generateEnumMemberTable(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    int i, nr_members;
    enumDef *ed;
    enumMemberDef **etab, **et;

    /* First we count how many. */

    nr_members = 0;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        enumMemberDef *emd;

        if (ed->ecd != cd || ed->module != mod)
            continue;

        if (cd == NULL && ed->fqcname == NULL)
            continue;

        for (emd = ed->members; emd != NULL; emd = emd->next)
            ++nr_members;
    }

    if (nr_members == 0)
        return 0;

    /* Create a table so they can be sorted. */

    etab = sipMalloc(sizeof (enumMemberDef *) * nr_members);

    et = etab;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        enumMemberDef *emd;

        if (ed->ecd != cd || ed->module != mod)
            continue;

        if (cd == NULL && ed->fqcname == NULL)
            continue;

        for (emd = ed->members; emd != NULL; emd = emd->next)
            *et++ = emd;
    }

    qsort(etab, nr_members, sizeof (enumMemberDef *), compareEnumMembers);

    /* Now generate the table. */

    if (cd != NULL)
        prcode(fp,
"\n"
"static sipEnumMemberDef enummembers_%C[] = {\n"
            , classFQCName(cd));
    else
        prcode(fp,
"\n"
"/* These are the enum members of all global enums. */\n"
"static sipEnumMemberDef enummembers[] = {\n"
        );

    for (i = 0; i < nr_members; ++i)
    {
        enumMemberDef *emd;

        emd = etab[i];

        prcode(fp,
"    {%N, ", emd->pyname);

        if (cd != NULL)
        {
            if (isProtectedEnum(emd->ed))
                prcode(fp, "sip%C::", classFQCName(cd));
            else if (isProtectedClass(cd))
                prcode(fp, "%U::", cd);
            else
                prcode(fp, "%S::", classFQCName(cd));
        }

        prcode(fp, "%s, %d},\n", emd->cname, emd->ed->enumnr);
    }

    prcode(fp,
"};\n"
        );

    return nr_members;
}


/*
 * The qsort helper to compare two enumMemberDef structures based on the name
 * of the enum member.
 */

static int compareEnumMembers(const void *m1,const void *m2)
{
    return strcmp((*(enumMemberDef **)m1)->pyname->text,
              (*(enumMemberDef **)m2)->pyname->text);
}


/*
 * Generate the access functions for the variables.
 */
static void generateAccessFunctions(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    varDef *vd;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->accessfunc == NULL)
            continue;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        prcode(fp,
"\n"
"\n"
"/* Access function. */\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void *access_%C();}\n"
            , vd->fqcname);

        prcode(fp,
"static void *access_%C()\n"
"{\n"
            , vd->fqcname);

        generateCppCodeBlock(vd->accessfunc, fp);

        prcode(fp,
"}\n"
            );
    }
}


/*
 * Generate the inline code to add a set of generated type instances to a
 * dictionary.
 */
static void generateTypesInline(sipSpec *pt, moduleDef *mod, FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->module != mod)
            continue;

        if (vd->type.atype != class_type && vd->type.atype != mapped_type && vd->type.atype != enum_type)
            continue;

        if (needsHandler(vd))
            continue;

        /* Skip classes that don't need inline code. */
        if (generating_c || vd->accessfunc != NULL || vd->type.nrderefs != 0)
            continue;

        if (noIntro)
        {
            prcode(fp,
"\n"
"    /*\n"
"     * Define the class, mapped type and enum instances that have to be\n"
"     * added inline.\n"
"     */\n"
                );

            noIntro = FALSE;
        }

        prcode(fp,
"    sipAddTypeInstance(");

        if (vd->ecd == NULL)
            prcode(fp, "sipModuleDict");
        else
            prcode(fp, "(PyObject *)sipTypeAsPyTypeObject(sipType_%C)", classFQCName(vd->ecd));

        prcode(fp, ",%N,", vd->pyname);

        if (isConstArg(&vd->type))
            prcode(fp, "const_cast<%b *>(&%S)", &vd->type, vd->fqcname);
        else
            prcode(fp, "&%S", vd->fqcname);

        if (vd->type.atype == class_type)
            prcode(fp, ",sipType_%C);\n"
                , classFQCName(vd->type.u.cd));
        else if (vd->type.atype == enum_type)
            prcode(fp, ",sipType_%C);\n"
                , vd->type.u.ed->fqcname);
        else
            prcode(fp, ",sipType_%T);\n"
                , &vd->type);
    }
}


/*
 * Generate the code to add a set of class instances to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateClasses(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (vd->type.atype != class_type && (vd->type.atype != enum_type || vd->type.u.ed->fqcname == NULL))
            continue;

        if (needsHandler(vd))
            continue;

        /*
         * Skip ordinary C++ class instances which need to be done with inline
         * code rather than through a static table.  This is because C++ does
         * not guarantee the order in which the table and the instance will be
         * created.  So far this has only been seen to be a problem when
         * statically linking SIP generated modules on Windows.
         */
        if (!generating_c && vd->accessfunc == NULL && vd->type.nrderefs == 0)
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the class and enum instances to be added to this type dictionary. */\n"
"static sipTypeInstanceDef typeInstances_%C[] = {\n"
                    , classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the class and enum instances to be added to this module dictionary. */\n"
"static sipTypeInstanceDef typeInstances[] = {\n"
                    );

            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, ", vd->pyname);

        if (vd->type.atype == class_type)
        {
            scopedNameDef *vcname = classFQCName(vd->type.u.cd);

            if (vd->accessfunc != NULL)
            {
                prcode(fp, "(void *)access_%C, &sipType_%C, SIP_ACCFUNC", vd->fqcname, vcname);
            }
            else if (vd->type.nrderefs != 0)
            {
                prcode(fp, "&%S, &sipType_%C, SIP_INDIRECT", vd->fqcname, vcname);
            }
            else if (isConstArg(&vd->type))
            {
                prcode(fp, "const_cast<%b *>(&%S), &sipType_%C, 0", &vd->type, vd->fqcname, vcname);
            }
            else
            {
                prcode(fp, "&%S, &sipType_%C, 0", vd->fqcname, vcname);
            }
        }
        else
        {
            prcode(fp, "&%S, &sipType_%C, 0", vd->fqcname, vd->type.u.ed->fqcname);
        }

        prcode(fp, "},\n"
            );
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0, 0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the code to add a set of void pointers to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateVoidPointers(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (vd->type.atype != void_type && vd->type.atype != struct_type)
            continue;

        if (needsHandler(vd))
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the void pointers to be added to this type dictionary. */\n"
"static sipVoidPtrInstanceDef voidPtrInstances_%C[] = {\n"
                    , classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the void pointers to be added to this module dictionary. */\n"
"static sipVoidPtrInstanceDef voidPtrInstances[] = {\n"
                    );

            noIntro = FALSE;
        }

        if (isConstArg(&vd->type))
            prcode(fp,
"    {%N, const_cast<%b *>(%S)},\n"
                , vd->pyname, &vd->type, vd->fqcname);
        else
            prcode(fp,
"    {%N, %S},\n"
                , vd->pyname, vd->fqcname);
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the code to add a set of characters to a dictionary.  Return TRUE
 * if there was at least one.
 */
static int generateChars(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        argType vtype = vd->type.atype;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (!((vtype == sstring_type || vtype == ustring_type || vtype == string_type || vtype == wstring_type) && vd->type.nrderefs == 0))
            continue;

        if (needsHandler(vd))
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the chars to be added to this type dictionary. */\n"
"static sipCharInstanceDef charInstances_%C[] = {\n"
                    , classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the chars to be added to this module dictionary. */\n"
"static sipCharInstanceDef charInstances[] = {\n"
                    );

            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, %S},\n"
            , vd->pyname, vd->fqcname);
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the code to add a set of strings to a dictionary.  Return TRUE if
 * there is at least one.
 */
static int generateStrings(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        argType vtype = vd->type.atype;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (!((vtype == sstring_type || vtype == ustring_type || vtype == string_type || vtype == wstring_type) && vd->type.nrderefs != 0))
            continue;

        if (needsHandler(vd))
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the strings to be added to this type dictionary. */\n"
"static sipStringInstanceDef stringInstances_%C[] = {\n"
                    , classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the strings to be added to this module dictionary. */\n"
"static sipStringInstanceDef stringInstances[] = {\n"
                    );

            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, %S},\n"
            , vd->pyname, vd->fqcname);
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the code to add a set of ints to a dictionary.  Return TRUE if
 * there was at least one.
 */
static int generateInts(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int noIntro;
    varDef *vd;
    enumDef *ed;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        argType vtype = vd->type.atype;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (!(vtype == enum_type || vtype == ushort_type ||
              vtype == short_type || vtype == uint_type ||
              vtype == cint_type || vtype == int_type ||
              vtype == bool_type || vtype == cbool_type))
            continue;

        if (needsHandler(vd))
            continue;

        /* Named enums are handled elsewhere. */
        if (vtype == enum_type && vd->type.u.ed->fqcname != NULL)
            continue;

        if (noIntro)
        {
            ints_intro(cd, fp);
            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, %S},\n"
            , vd->pyname, vd->fqcname);
    }

    /* Now do global anonymous enums. */
    if (cd == NULL)
        for (ed = pt->enums; ed != NULL; ed = ed->next)
        {
            enumMemberDef *em;

            if (ed->ecd != cd || ed->module != mod)
                continue;

            if (ed->fqcname != NULL)
                continue;

            for (em = ed->members; em != NULL; em = em->next)
            {
                if (noIntro)
                {
                    ints_intro(cd, fp);
                    noIntro = FALSE;
                }

                prcode(fp,
"    {%N, %s},\n"
                    , em->pyname, em->cname);
            }
        }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the intro for a table of int instances.
 */
static void ints_intro(classDef *cd, FILE *fp)
{
    if (cd != NULL)
        prcode(fp,
"\n"
"\n"
"/* Define the ints to be added to this type dictionary. */\n"
"static sipIntInstanceDef intInstances_%C[] = {\n"
            ,classFQCName(cd));
    else
        prcode(fp,
"\n"
"\n"
"/* Define the ints to be added to this module dictionary. */\n"
"static sipIntInstanceDef intInstances[] = {\n"
            );
}


/*
 * Generate the code to add a set of longs to a dictionary.  Return TRUE if
 * there was at least one.
 */
static int generateLongs(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    return generateVariableType(pt, mod, cd, long_type, "long", "Long", "long", fp);
}


/*
 * Generate the code to add a set of unsigned longs to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateUnsignedLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    return generateVariableType(pt, mod, cd, ulong_type, "unsigned long", "UnsignedLong", "unsignedLong", fp);
}


/*
 * Generate the code to add a set of long longs to a dictionary.  Return TRUE
 * if there was at least one.
 */
static int generateLongLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    return generateVariableType(pt, mod, cd, longlong_type, "long long", "LongLong", "longLong", fp);
}


/*
 * Generate the code to add a set of unsigned long longs to a dictionary.
 * Return TRUE if there was at least one.
 */
static int generateUnsignedLongLongs(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    return generateVariableType(pt, mod, cd, ulonglong_type, "unsigned long long", "UnsignedLongLong", "unsignedLongLong", fp);
}


/*
 * Generate the code to add a set of a particular type to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateVariableType(sipSpec *pt, moduleDef *mod, classDef *cd,
        argType atype, const char *eng, const char *s1, const char *s2,
        FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        argType vtype = vd->type.atype;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (vtype != atype)
            continue;

        if (needsHandler(vd))
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the %ss to be added to this type dictionary. */\n"
"static sip%sInstanceDef %sInstances_%C[] = {\n"
                    , eng
                    , s1, s2, classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the %ss to be added to this module dictionary. */\n"
"static sip%sInstanceDef %sInstances[] = {\n"
                    , eng
                    , s1, s2);

            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, %S},\n"
            , vd->pyname, vd->fqcname);
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the code to add a set of doubles to a dictionary.  Return TRUE if
 * there was at least one.
 */
static int generateDoubles(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int noIntro;
    varDef *vd;

    noIntro = TRUE;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        argType vtype = vd->type.atype;

        if (vd->ecd != cd || vd->module != mod)
            continue;

        if (!(vtype == float_type || vtype == cfloat_type || vtype == double_type || vtype == cdouble_type))
            continue;

        if (needsHandler(vd))
            continue;

        if (noIntro)
        {
            if (cd != NULL)
                prcode(fp,
"\n"
"\n"
"/* Define the doubles to be added to this type dictionary. */\n"
"static sipDoubleInstanceDef doubleInstances_%C[] = {\n"
                    , classFQCName(cd));
            else
                prcode(fp,
"\n"
"\n"
"/* Define the doubles to be added to this module dictionary. */\n"
"static sipDoubleInstanceDef doubleInstances[] = {\n"
                    );

            noIntro = FALSE;
        }

        prcode(fp,
"    {%N, %S},\n"
            , vd->pyname, vd->fqcname);
    }

    if (!noIntro)
        prcode(fp,
"    {0, 0}\n"
"};\n"
            );

    return !noIntro;
}


/*
 * Generate the C/C++ code for an interface.
 */
static void generateIfaceCpp(sipSpec *pt, ifaceFileDef *iff,
        const char *codeDir, const char *srcSuffix, FILE *master)
{
    char *cppfile;
    const char *cmname = iff->module->name;
    classDef *cd;
    mappedTypeDef *mtd;
    FILE *fp;

    if (master == NULL)
    {
        cppfile = createIfaceFileName(codeDir,iff,srcSuffix);
        fp = createCompilationUnit(iff->module, cppfile, "Interface wrapper code.");

        prcode(fp,
"\n"
"#include \"sipAPI%s.h\"\n"
            , cmname);
    }
    else
        fp = master;

    prcode(fp,
"\n"
            );

    generateCppCodeBlock(iff->hdrcode, fp);
    generateUsedIncludes(iff->used, fp);

    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        /*
         * Protected classes must be generated in the interface file of the
         * enclosing scope.
         */
        if (isProtectedClass(cd))
            continue;

        if (cd->iff == iff && !isExternal(cd))
        {
            classDef *pcd;

            generateClassCpp(cd, pt, fp);

            /* Generate any enclosed protected classes. */
            for (pcd = pt->classes; pcd != NULL; pcd = pcd->next)
                if (isProtectedClass(pcd) && pcd->ecd == cd)
                    generateClassCpp(pcd, pt, fp);
        }
    }

    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff == iff)
            generateMappedTypeCpp(mtd, pt, fp);

    if (master == NULL)
    {
        closeFile(fp);
        free(cppfile);
    }
}


/*
 * Return a filename for an interface C++ or header file on the heap.
 */
static char *createIfaceFileName(const char *codeDir, ifaceFileDef *iff,
        const char *suffix)
{
    char *fn;
    scopedNameDef *snd;

    fn = concat(codeDir,"/sip",iff->module->name,NULL);

    for (snd = iff->fqcname; snd != NULL; snd = snd->next)
        append(&fn,snd->name);

    append(&fn,suffix);

    return fn;
}


/*
 * Generate the C++ code for a mapped type version.
 */
static void generateMappedTypeCpp(mappedTypeDef *mtd, sipSpec *pt, FILE *fp)
{
    int need_xfer;

    if (optAssignmentHelpers(pt) && !noRelease(mtd))
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void assign_%T(void *, const void*);}\n"
                , &mtd->type);

        prcode(fp,
"static void assign_%T(void *sipDst, const void *sipSrc)\n"
"{\n"
            , &mtd->type);

        if (generating_c)
            prcode(fp,
"    *(%b *)sipDst = *(const %b *)sipSrc;\n"
                , &mtd->type, &mtd->type);
        else
            prcode(fp,
"    *reinterpret_cast<%b *>(sipDst) = *reinterpret_cast<const %b *>(sipSrc);\n"
                , &mtd->type, &mtd->type);

        prcode(fp,
"}\n"
            );
    }

    if (!noRelease(mtd))
    {
        prcode(fp,
"\n"
"\n"
"/* Call the mapped type's destructor. */\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void release_%T(void *, int);}\n"
                , &mtd->type);

        prcode(fp,
"static void release_%T(void *ptr, int%s)\n"
"{\n"
            , &mtd->type, (generating_c ? " status" : ""));

        if (release_gil)
            prcode(fp,
"    Py_BEGIN_ALLOW_THREADS\n"
                );

        if (generating_c)
            prcode(fp,
"    sipFree(ptr);\n"
                );
        else
            prcode(fp,
"    delete reinterpret_cast<%b *>(ptr);\n"
                , &mtd->type);

        if (release_gil)
            prcode(fp,
"    Py_END_ALLOW_THREADS\n"
                );

        prcode(fp,
"}\n"
"\n"
            );
    }

    generateConvertToDefinitions(mtd,NULL,fp);

    /* Generate the from type convertor. */

    need_xfer = (generating_c || usedInCode(mtd->convfromcode, "sipTransferObj"));

    prcode(fp,
"\n"
"\n"
        );

    if (!generating_c)
        prcode(fp,
"extern \"C\" {static PyObject *convertFrom_%T(void *, PyObject *);}\n"
            , &mtd->type);

    prcode(fp,
"static PyObject *convertFrom_%T(void *sipCppV,PyObject *%s)\n"
"{\n"
"   ", &mtd->type, (need_xfer ? "sipTransferObj" : ""));

    generateMappedTypeFromVoid(mtd, "sipCpp", "sipCppV", fp);

    prcode(fp, ";\n"
"\n"
        );

    generateCppCodeBlock(mtd->convfromcode,fp);

    prcode(fp,
"}\n"
"\n"
"\n"
"sipMappedTypeDef sipMappedTypeDef_%T = {\n"
"    {\n"
"        -1,\n"
"        0,\n"
"        0,\n"
"        SIP_TYPE_MAPPED,\n"
"        %n,\n"
"        {0}\n"
"    },\n"
        , &mtd->type
        , mtd->cname);

    if (optAssignmentHelpers(pt) && !noRelease(mtd))
        prcode(fp,
"    assign_%T,\n"
            , &mtd->type);
    else
        prcode(fp,
"    0,\n"
            );

    if (noRelease(mtd))
        prcode(fp,
"    0,\n"
            );
    else
        prcode(fp,
"    release_%T,\n"
            , &mtd->type);

    prcode(fp,
"    convertTo_%T,\n"
"    convertFrom_%T\n"
"};\n"
        , &mtd->type
        , &mtd->type);
}


/*
 * Generate the C++ code for a class.
 */
static void generateClassCpp(classDef *cd, sipSpec *pt, FILE *fp)
{
    varDef *vd;
    moduleDef *mod = cd->iff->module;

    /* Generate any local class code. */

    generateCppCodeBlock(cd->cppcode, fp);

    generateClassFunctions(pt, mod, cd, fp);

    generateAccessFunctions(pt, mod, cd, fp);

    /* Generate the variable handlers. */
    if (hasVarHandlers(cd))
    {
        for (vd = pt->vars; vd != NULL; vd = vd->next)
            if (vd->ecd == cd && needsHandler(vd))
                generateVariableHandler(cd, vd, fp);

        /* Generate the variable table. */
        prcode(fp,
"\n"
"PyMethodDef variables_%C[] = {\n"
            ,classFQCName(cd));

        for (vd = pt->vars; vd != NULL; vd = vd->next)
            if (vd->ecd == cd && needsHandler(vd))
                prcode(fp,
"    {%N, var_%C, %s, NULL},\n"
                    ,vd->pyname,vd->fqcname,(isStaticVar(vd) ? "METH_STATIC" : "0"));

        prcode(fp,
"    {0, 0, 0, 0}\n"
"};\n"
            );
    }

    if (cd->iff->type != namespace_iface)
        generateConvertToDefinitions(NULL,cd,fp);

    /* The type definition structure. */
    generateTypeDefinition(pt, cd, fp);
}


/*
 * Return a sorted array of relevant functions for a namespace.
 */

static sortedMethTab *createFunctionTable(classDef *cd,int *nrp)
{
    int nr;
    sortedMethTab *mtab, *mt;
    memberDef *md;

    /* First we need to count the number of applicable functions. */

    nr = 0;

    for (md = cd->members; md != NULL; md = md->next)
        ++nr;

    if ((*nrp = nr) == 0)
        return NULL;

    /* Create the table of methods. */

    mtab = sipMalloc(sizeof (sortedMethTab) * nr);

    /* Initialise the table. */

    mt = mtab;

    for (md = cd->members; md != NULL; md = md->next)
    {
        mt->md = md;
        mt->is_static = TRUE;

        ++mt;
    }

    /* Finally sort the table. */

    qsort(mtab,nr,sizeof (sortedMethTab),compareMethTab);

    return mtab;
}


/*
 * Return a sorted array of relevant methods (either lazy or non-lazy) for a
 * class.
 */
static sortedMethTab *createMethodTable(classDef *cd, int *nrp)
{
    int nr;
    visibleList *vl;
    sortedMethTab *mtab, *mt;

    /*
     * First we need to count the number of applicable methods.  Only provide
     * an entry point if there is at least one overload that is defined in this
     * class and is a non-abstract function or slot.  We allow private (even
     * though we don't actually generate code) because we need to intercept the
     * name before it reaches a more public version further up the class
     * hierarchy.  We add the ctor and any variable handlers as special
     * entries.
     */
    nr = 0;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        overDef *od;

        if (vl->m->slot != no_slot)
            continue;

        for (od = vl->cd->overs; od != NULL; od = od->next)
        {
            /*
             * Skip protected methods if we don't have the means to handle
             * them.
             */
            if (isProtected(od) && !hasShadow(cd))
                continue;

            if (skipOverload(od,vl->m,cd,vl->cd,TRUE))
                continue;

            ++nr;

            break;
        }
    }

    if ((*nrp = nr) == 0)
        return NULL;

    /* Create the table of methods. */

    mtab = sipMalloc(sizeof (sortedMethTab) * nr);

    /* Initialise the table. */

    mt = mtab;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        int need_method, is_static;
        overDef *od;

        if (vl->m->slot != no_slot)
            continue;

        need_method = FALSE;
        is_static = TRUE;

        for (od = vl->cd->overs; od != NULL; od = od->next)
        {
            /*
             * Skip protected methods if we don't have the means to handle
             * them.
             */
            if (isProtected(od) && !hasShadow(cd))
                continue;

            if (!skipOverload(od,vl->m,cd,vl->cd,TRUE))
            {
                need_method = TRUE;

                if (!isPrivate(od) && !isStatic(od))
                    is_static = FALSE;
            }
        }

        if (need_method)
        {
            mt->md = vl->m;
            mt->is_static = is_static;

            ++mt;
        }
    }

    /* Finally sort the table. */

    qsort(mtab,nr,sizeof (sortedMethTab),compareMethTab);

    return mtab;
}


/*
 * The qsort helper to compare two sortedMethTab structures based on the Python
 * name of the method.
 */

static int compareMethTab(const void *m1,const void *m2)
{
    return strcmp(((sortedMethTab *)m1)->md->pyname->text,
              ((sortedMethTab *)m2)->md->pyname->text);
}


/*
 * Generate the sorted table of methods and return the number of entries.
 */
static int generateMethodTable(classDef *cd,FILE *fp)
{
    int nr;
    sortedMethTab *mtab;

    mtab = (cd->iff->type == namespace_iface) ?
        createFunctionTable(cd,&nr) :
        createMethodTable(cd,&nr);

    if (mtab != NULL)
    {
        int i;

        prcode(fp,
"\n"
"\n"
"static PyMethodDef methods_%C[] = {\n"
            ,classFQCName(cd));

        for (i = 0; i < nr; ++i)
        {
            memberDef *md = mtab[i].md;

            /*
             * For the moment we are suppressing the generation of
             * METH_STATIC until we understand descriptors better.
             * It could be that they will simplify the handling of
             * lazy attributes and allow things to be cached in the
             * type dictionary.
             */
            mtab[i].is_static = FALSE;

            prcode(fp,
"    {%N, meth_%C_%s, METH_VARARGS%s, NULL}%s\n"
                ,md->pyname,classFQCName(cd),md->pyname->text,(mtab[i].is_static ? "|METH_STATIC" : ""),((i + 1) < nr) ? "," : "");
        }

        free(mtab);

        prcode(fp,
"};\n"
            );
    }

    return nr;
}


/*
 * Generate the "to type" convertor definitions.
 */

static void generateConvertToDefinitions(mappedTypeDef *mtd,classDef *cd,
                     FILE *fp)
{
    codeBlock *convtocode;
    ifaceFileDef *iff;
    argDef type;

    memset(&type, 0, sizeof (argDef));

    if (cd != NULL)
    {
        convtocode = cd->convtocode;

        iff = cd->iff;

        type.atype = class_type;
        type.u.cd = cd;
    }
    else
    {
        convtocode = mtd->convtocode;

        iff = mtd->iff;

        type.atype = mapped_type;
        type.u.mtd = mtd;
    }

    /* Generate the type convertors. */

    if (convtocode != NULL)
    {
        int need_ptr, need_xfer;

        /*
         * Sometimes type convertors are just stubs that set the error
         * flag, so check if we actually need everything so that we
         * can avoid compiler warnings.
         */
        need_ptr = (generating_c || usedInCode(convtocode, "sipCppPtr"));
        need_xfer = (generating_c || usedInCode(convtocode, "sipTransferObj"));

        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static int convertTo_%T(PyObject *, void **, int *, PyObject *);}\n"
                , &type);

        prcode(fp,
"static int convertTo_%T(PyObject *sipPy,void **%s,int *sipIsErr,PyObject *%s)\n"
"{\n"
            , &type, (need_ptr ? "sipCppPtrV" : ""), (need_xfer ? "sipTransferObj" : ""));

        if (need_ptr)
        {
            if (generating_c)
                prcode(fp,
"    %b **sipCppPtr = (%b **)sipCppPtrV;\n"
"\n"
                    , &type, &type);
            else
                prcode(fp,
"    %b **sipCppPtr = reinterpret_cast<%b **>(sipCppPtrV);\n"
"\n"
                    , &type, &type);
        }

        generateCppCodeBlock(convtocode,fp);

        prcode(fp,
"}\n"
            );
    }
}


/*
 * Generate a variable handler.
 */
static void generateVariableHandler(classDef *context, varDef *vd, FILE *fp)
{
    argType atype = vd->type.atype;
    const char *first_arg;
    int no_setter;

    no_setter = (vd->type.nrderefs == 0 && isConstArg(&vd->type));

    if (generating_c || !isStaticVar(vd))
        first_arg = "sipSelf";
    else if (usedInCode(vd->getcode, "sipPyType") || usedInCode(vd->setcode, "sipPyType"))
        first_arg = "sipPyType";
    else
        first_arg = "";

    prcode(fp,
"\n"
"\n"
        );

    if (!generating_c)
        prcode(fp,
"extern \"C\" {static PyObject *var_%C(PyObject *, PyObject *);}\n"
            , vd->fqcname);

    prcode(fp,
"static PyObject *var_%C(PyObject *%s,PyObject *sipPy)\n"
"{\n"
        , vd->fqcname, first_arg);

    if (vd->getcode == NULL || (vd->setcode == NULL && !no_setter))
    {
        prcode(fp,
"    ");

        generateNamedValueType(context, &vd->type, "sipVal", fp);

        prcode(fp, ";\n"
            );
    }

    if (!isStaticVar(vd))
    {
        if (generating_c)
            prcode(fp,
"    %S *sipCpp = (%S *)sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_%C);\n"
                ,classFQCName(vd->ecd),classFQCName(vd->ecd),classFQCName(vd->ecd));
        else
            prcode(fp,
"    %S *sipCpp = reinterpret_cast<%S *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_%C));\n"
                ,classFQCName(vd->ecd),classFQCName(vd->ecd),classFQCName(vd->ecd));

        prcode(fp,
"\n"
"    if (!sipCpp)\n"
"        return NULL;\n"
            );
    }

    prcode(fp,
"\n"
"    if (sipPy == NULL)\n"
"    {\n"
        );

    /* Generate the get handler part. */

    if (vd->getcode != NULL)
    {
        generateCppCodeBlock(vd->getcode, fp);

        prcode(fp,
"\n"
"        return sipPy;\n"
            );
    }
    else
    {
        int pyobj = FALSE, needsNew;

        needsNew = ((atype == class_type || atype == mapped_type) && vd->type.nrderefs == 0);

        if (needsNew)
        {
            if (generating_c)
                prcode(fp,
"        *sipVal = ");
            else
                prcode(fp,
"        sipVal = new %b(", &vd->type);
        }
        else
            prcode(fp,
"        sipVal = ");

        generateVarMember(vd, fp);

        prcode(fp, "%s;\n"
"\n"
            , ((needsNew && !generating_c) ? ")" : ""));

        switch (atype)
        {
        case mapped_type:
            prcode(fp,
"        sipPy = sipConvertFromType(sipVal,sipType_%T,NULL);\n"
                ,&vd->type);

            break;

        case class_type:
            generateVarClassConversion(vd, needsNew, fp);
            break;

        case bool_type:
        case cbool_type:
            prcode(fp,
"        sipPy = PyBool_FromLong(sipVal);\n"
                );

            break;

        case sstring_type:
        case ustring_type:
        case string_type:
            if (vd->type.nrderefs == 0)
                prcode(fp,
"        sipPy = PyString_FromStringAndSize(%s&sipVal,1);\n"
                    ,(atype != string_type) ? "(char *)" : "");
            else
                prcode(fp,
"        sipPy = PyString_FromString(%ssipVal);\n"
                    ,(atype != string_type) ? "(char *)" : "");

            break;

        case wstring_type:
            if (vd->type.nrderefs == 0)
                prcode(fp,
"        sipPy = PyUnicode_FromWideChar(&sipVal,1);\n"
                    );
            else
                prcode(fp,
"        sipPy = PyUnicode_FromWideChar(sipVal,(SIP_SSIZE_T)wcslen(sipVal));\n"
                    );

            break;

        case float_type:
        case cfloat_type:
            prcode(fp,
"        sipPy = PyFloat_FromDouble((double)sipVal);\n"
                );
            break;

        case double_type:
        case cdouble_type:
            prcode(fp,
"        sipPy = PyFloat_FromDouble(sipVal);\n"
                );
            break;

        case enum_type:
            if (vd->type.u.ed->fqcname != NULL)
            {
                prcode(fp,
"        sipPy = sipConvertFromEnum(sipVal,sipType_%C);\n"
                    , vd->type.u.ed->fqcname);

                break;
            }

            /* Drop through. */

        case short_type:
        case cint_type:
        case int_type:
            prcode(fp,
"        sipPy = PyInt_FromLong(sipVal);\n"
                );
            break;

        case long_type:
            prcode(fp,
"        sipPy = PyLong_FromLong(sipVal);\n"
                );
            break;

        case ushort_type:
        case uint_type:
        case ulong_type:
            prcode(fp,
"        sipPy = PyLong_FromUnsignedLong(sipVal);\n"
                );
            break;

        case longlong_type:
            prcode(fp,
"        sipPy = PyLong_FromLongLong(sipVal);\n"
                );
            break;

        case ulonglong_type:
            prcode(fp,
"        sipPy = PyLong_FromUnsignedLongLong(sipVal);\n"
                );
            break;

        case struct_type:
        case void_type:
            prcode(fp,
"        sipPy = sipConvertFrom%sVoidPtr(sipVal);\n"
                , (isConstArg(&vd->type) ? "Const" : ""));
            break;

        case pyobject_type:
        case pytuple_type:
        case pylist_type:
        case pydict_type:
        case pycallable_type:
        case pyslice_type:
        case pytype_type:
            prcode(fp,
"        Py_XINCREF(sipVal);\n"
                );
            pyobj = TRUE;
            break;
        }

        prcode(fp,
"\n"
"        return %s;\n"
            ,(pyobj ? "sipVal" : "sipPy"));
    }

    prcode(fp,
"    }\n"
"\n"
        );

    /* Generate the set handler part. */

    if (vd->setcode != NULL)
    {
        prcode(fp,
"    {\n"
"       int sipErr = 0;\n"
"\n"
            );

        generateCppCodeBlock(vd->setcode, fp);

        prcode(fp,
"\n"
"        if (sipErr)\n"
"            return NULL;\n"
"    }\n"
            );
    }
    else if (no_setter)
    {
        prcode(fp,
"    sipBadSetType(%N,%N);\n"
"    return NULL;\n"
"}\n"
            , vd->ecd->pyname, vd->pyname);

        return;
    }
    else
    {
        char *deref;
        int might_be_temp;

        if (vd->type.nrderefs == 0 && (atype == mapped_type || (atype == class_type && vd->type.u.cd->convtocode != NULL)))
            prcode(fp,
"    int sipValState;\n"
                );

        if (atype == class_type || atype == mapped_type)
            prcode(fp,
"    int sipIsErr = 0;\n"
"\n"
                );

        might_be_temp = generateObjToCppConversion(&vd->type,fp);

        deref = "";

        if (atype == class_type || atype == mapped_type)
        {
            if (vd->type.nrderefs == 0)
                deref = "*";

            prcode(fp,
"\n"
"    if (sipIsErr)\n"
"        return NULL;\n"
"\n"
                );
        }
        else
        {
            if ((atype == sstring_type || atype == ustring_type || atype == string_type || atype == wstring_type) && vd->type.nrderefs != 0)
            {
                prcode(fp,
"\n"
"    if (sipVal == NULL)\n"
                    );
            }
            else
                prcode(fp,
"\n"
"    if (PyErr_Occurred() != NULL)\n"
                    );

            prcode(fp,
"    {\n"
"        sipBadSetType(%N,%N);\n"
"        return NULL;\n"
"    }\n"
"\n"
                , vd->ecd->pyname, vd->pyname);
        }

        if (atype == pyobject_type || atype == pytuple_type ||
            atype == pylist_type || atype == pydict_type ||
            atype == pycallable_type || atype == pyslice_type ||
            atype == pytype_type)
        {
            prcode(fp,
"    Py_XDECREF(");

            generateVarMember(vd, fp);

            prcode(fp, ");\n"
"    Py_INCREF(sipVal);\n"
"\n"
                );
        }

        prcode(fp,
"    ");

        generateVarMember(vd, fp);

        prcode(fp, " = %ssipVal;\n"
            , deref);

        /* Note that wchar_t * leaks here. */

        if (might_be_temp)
            prcode(fp,
"\n"
"    sipReleaseType(sipVal,sipType_%C,sipValState);\n"
                , classFQCName(vd->type.u.cd));
        else if (vd->type.atype == mapped_type && vd->type.nrderefs == 0 && !noRelease(vd->type.u.mtd))
            prcode(fp,
"\n"
"    sipReleaseType(sipVal,sipType_%T,sipValState);\n"
                , &vd->type);
    }

    prcode(fp,
"\n"
"    Py_INCREF(Py_None);\n"
"    return Py_None;\n"
"}\n"
        );
}


/*
 * Generate the member variable of a class.
 */
static void generateVarMember(varDef *vd, FILE *fp)
{
    if (isStaticVar(vd))
        prcode(fp,"%S::",classFQCName(vd->ecd));
    else
        prcode(fp,"sipCpp->");

    prcode(fp, "%s", scopedNameTail(vd->fqcname));
}


/*
 * Generate an variable class conversion fragment.
 */
static void generateVarClassConversion(varDef *vd, int is_new, FILE *fp)
{
    classDef *cd = vd->type.u.cd;

    prcode(fp,
"        sipPy = sipConvertFrom%sType(", (is_new ? "New" : ""));

    if (isConstArg(&vd->type))
        prcode(fp,"const_cast<%b *>(sipVal)",&vd->type);
    else
        prcode(fp,"sipVal");

    prcode(fp,",sipType_%C,NULL);\n"
        ,classFQCName(cd));
}


/*
 * Generate the declaration of a variable that is initialised from a Python
 * object.  Return TRUE if the value might be a temporary on the heap.
 */
static int generateObjToCppConversion(argDef *ad,FILE *fp)
{
    int might_be_temp = FALSE;
    char *rhs = NULL;

    prcode(fp,
"    sipVal = ");

    switch (ad->atype)
    {
    case mapped_type:
        {
            const char *tail;

            if (generating_c)
            {
                prcode(fp, "(%b *)", ad);
                tail = "";
            }
            else
            {
                prcode(fp, "reinterpret_cast<%b *>(", ad);
                tail = ")";
            }

            /* Note that we don't support /Transfer/ but could do. */

            prcode(fp, "sipForceConvertToType(sipPy,sipType_%T,NULL,%s,%s,&sipIsErr)", ad, (ad->nrderefs ? "0" : "SIP_NOT_NONE"), (ad->nrderefs ? "NULL" : "&sipValState"));

            prcode(fp, "%s;\n"
                , tail);
        }
        break;

    case class_type:
        {
            const char *tail;

            if (ad->nrderefs == 0 && ad->u.cd->convtocode != NULL)
                might_be_temp = TRUE;

            if (generating_c)
            {
                prcode(fp, "(%b *)", ad);
                tail = "";
            }
            else
            {
                prcode(fp, "reinterpret_cast<%b *>(", ad);
                tail = ")";
            }

            /*
             * Note that we don't support /Transfer/ but could do.  We could
             * also support /Constrained/ (so long as we also supported it for
             * all types).
             */

            prcode(fp, "sipForceConvertToType(sipPy,sipType_%C,NULL,%s,%s,&sipIsErr)", classFQCName(ad->u.cd), (ad->nrderefs ? "0" : "SIP_NOT_NONE"), (might_be_temp ? "&sipValState" : "NULL"));

            prcode(fp, "%s;\n"
                , tail);
        }
        break;

    case enum_type:
        prcode(fp, "(%E)PyInt_AsLong(sipPy);\n"
            , ad->u.ed);
        break;

    case sstring_type:
        if (ad->nrderefs == 0)
            rhs = "(signed char)sipString_AsChar(sipPy)";
        else
            rhs = "(signed char *)PyString_AsString(sipPy)";
        break;

    case ustring_type:
        if (ad->nrderefs == 0)
            rhs = "(unsigned char)sipString_AsChar(sipPy)";
        else
            rhs = "(unsigned char *)PyString_AsString(sipPy)";
        break;

    case string_type:
        if (ad->nrderefs == 0)
            rhs = "sipString_AsChar(sipPy)";
        else
            rhs = "PyString_AsString(sipPy)";
        break;

    case wstring_type:
        if (ad->nrderefs == 0)
            rhs = "sipUnicode_AsWChar(sipPy)";
        else
            rhs = "sipUnicode_AsWString(sipPy)";
        break;

    case float_type:
    case cfloat_type:
        rhs = "(float)PyFloat_AsDouble(sipPy)";
        break;

    case double_type:
    case cdouble_type:
        rhs = "PyFloat_AsDouble(sipPy)";
        break;

    case bool_type:
    case cbool_type:
        rhs = "(bool)PyInt_AsLong(sipPy)";
        break;

    case ushort_type:
        rhs = "(unsigned short)sipLong_AsUnsignedLong(sipPy)";
        break;

    case short_type:
        rhs = "(short)PyInt_AsLong(sipPy)";
        break;

    case uint_type:
        rhs = "(unsigned)sipLong_AsUnsignedLong(sipPy)";
        break;

    case int_type:
    case cint_type:
        rhs = "(int)PyInt_AsLong(sipPy)";
        break;

    case ulong_type:
        rhs = "sipLong_AsUnsignedLong(sipPy)";
        break;

    case long_type:
        rhs = "PyLong_AsLong(sipPy)";
        break;

    case ulonglong_type:
        rhs = "PyLong_AsUnsignedLongLong(sipPy)";
        break;

    case longlong_type:
        rhs = "PyLong_AsLongLong(sipPy)";
        break;

    case struct_type:
        prcode(fp, "(struct %S *)sipConvertToVoidPtr(sipPy);\n"
            , ad->u.sname);
        break;

    case void_type:
        rhs = "sipConvertToVoidPtr(sipPy)";
        break;

    case pyobject_type:
    case pytuple_type:
    case pylist_type:
    case pydict_type:
    case pycallable_type:
    case pyslice_type:
    case pytype_type:
        rhs = "sipPy";
        break;
    }

    if (rhs != NULL)
        prcode(fp, "%s;\n"
            , rhs);

    return might_be_temp;
}


/*
 * Returns TRUE if the given method is a slot that takes zero arguments.
 */
static int isZeroArgSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == str_slot || st == int_slot || st == long_slot ||
        st == float_slot || st == invert_slot || st == neg_slot ||
        st == len_slot || st == nonzero_slot || st == pos_slot ||
        st == abs_slot || st == repr_slot || st == hash_slot);
}


/*
 * Returns TRUE if the given method is a slot that takes more than one
 * argument.
 */
static int isMultiArgSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == setitem_slot || st == call_slot);
}


/*
 * Returns TRUE if the given method is a slot that returns void (ie. nothing
 * other than an error indicator).
 */
int isVoidReturnSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == setitem_slot || st == delitem_slot);
}


/*
 * Returns TRUE if the given method is a slot that returns int.
 */
int isIntReturnSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == len_slot || st == nonzero_slot || st == contains_slot ||
        st == cmp_slot);
}


/*
 * Returns TRUE if the given method is a slot that returns long.
 */
int isLongReturnSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == hash_slot);
}


/*
 * Returns TRUE if the given method is a slot that takes an int argument.
 */
static int isIntArgSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == repeat_slot || st == irepeat_slot);
}


/*
 * Returns TRUE if the given method is an inplace number slot.
 */
static int isInplaceNumberSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == iadd_slot || st == isub_slot || st == imul_slot ||
        st == idiv_slot || st == imod_slot ||
        st == ior_slot || st == ixor_slot || st == iand_slot ||
        st == ilshift_slot || st == irshift_slot);
}


/*
 * Returns TRUE if the given method is an inplace sequence slot.
 */
static int isInplaceSequenceSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == iconcat_slot || st == irepeat_slot);
}


/*
 * Returns TRUE if the given method is a number slot.
 */
int isNumberSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == add_slot || st == sub_slot || st == mul_slot ||
        st == div_slot || st == mod_slot ||
        st == and_slot || st == or_slot || st == xor_slot ||
        st == lshift_slot || st == rshift_slot);
}


/*
 * Returns TRUE if the given method is a rich compare slot.
 */
int isRichCompareSlot(memberDef *md)
{
    slotType st = md->slot;

    return (st == lt_slot || st == le_slot || st == eq_slot ||
        st == ne_slot || st == gt_slot || st == ge_slot);
}


/*
 * Generate a Python slot handler for either a class, an enum or an extender.
 */
static void generateSlot(moduleDef *mod, classDef *cd, enumDef *ed,
        memberDef *md, FILE *fp)
{
    char *arg_str, *prefix, *ret_type;
    int ret_int, nr_args;
    overDef *od, *overs;
    scopedNameDef *fqcname;
    nameDef *pyname;

    if (ed != NULL)
    {
        prefix = "Enum";
        pyname = ed->pyname;
        fqcname = ed->fqcname;
        overs = ed->overs;
    }
    else if (cd != NULL)
    {
        prefix = "Class";
        pyname = cd->pyname;
        fqcname = classFQCName(cd);
        overs = cd->overs;
    }
    else
    {
        prefix = NULL;
        pyname = NULL;
        fqcname = NULL;
        overs = mod->overs;
    }

    if (isVoidReturnSlot(md) || isIntReturnSlot(md))
    {
        ret_int = TRUE;
        ret_type = "int ";
    }
    else
    {
        ret_int = FALSE;

        if (isLongReturnSlot(md))
            ret_type = "long ";
        else
            ret_type = "PyObject *";
    }

    if (isIntArgSlot(md))
    {
        nr_args = 0;
        arg_str = "PyObject *sipSelf,int a0";
    }
    else if (isMultiArgSlot(md))
    {
        nr_args = 2;
        arg_str = "PyObject *sipSelf,PyObject *sipArgs";
    }
    else if (isZeroArgSlot(md))
    {
        nr_args = 0;
        arg_str = "PyObject *sipSelf";
    }
    else if (isNumberSlot(md))
    {
        nr_args = 2;
        arg_str = "PyObject *sipArg0,PyObject *sipArg1";
    }
    else
    {
        nr_args = 1;
        arg_str = "PyObject *sipSelf,PyObject *sipArg";
    }

    prcode(fp,
"\n"
"\n"
        );

    if (!generating_c)
    {
        prcode(fp,
"extern \"C\" {static %sslot_", ret_type);

        if (fqcname != NULL)
            prcode(fp, "%C_", fqcname);

        prcode(fp, "%s(%s);}\n"
            , md->pyname->text, arg_str);
    }

    prcode(fp,
"static %sslot_", ret_type);

    if (fqcname != NULL)
        prcode(fp, "%C_", fqcname);

    prcode(fp, "%s(%s)\n"
"{\n"
        , md->pyname->text, arg_str);

    if (isInplaceNumberSlot(md))
        prcode(fp,
"    if (!PyObject_TypeCheck(sipSelf,(PyTypeObject *)sip%s_%C))\n"
"    {\n"
"        Py_INCREF(Py_NotImplemented);\n"
"        return Py_NotImplemented;\n"
"    }\n"
"\n"
            , prefix, fqcname);

    if (!isNumberSlot(md))
    {
        if (cd != NULL)
            prcode(fp,
"    %S *sipCpp = reinterpret_cast<%S *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_%C));\n"
"\n"
"    if (!sipCpp)\n"
"        return %s;\n"
"\n"
                , fqcname, fqcname, fqcname
                , (md->slot == cmp_slot ? "-2" : (ret_int ? "-1" : "0")));
        else
            prcode(fp,
"    %S sipCpp = static_cast<%S>(PyInt_AsLong(sipSelf));\n"
"\n"
                , fqcname, fqcname);
    }

    if (nr_args > 0)
        prcode(fp,
"    int sipArgsParsed = 0;\n"
            );

    for (od = overs; od != NULL; od = od->next)
        if (od->common == md && isAbstract(od))
        {
            prcode(fp,
"    bool sipSelfWasArg = !sipSelf;\n"
                );

            break;
        }

    for (od = overs; od != NULL; od = od->next)
        if (od->common == md)
            generateFunctionBody(od, cd, cd, (ed == NULL && !dontDerefSelf(od)), fp);

    if (nr_args > 0)
        switch (md->slot)
        {
        case cmp_slot:
            prcode(fp,
"\n"
"    return 2;\n"
                );
            break;

        case concat_slot:
        case iconcat_slot:
        case repeat_slot:
        case irepeat_slot:
            prcode(fp,
"\n"
"    /* Raise an exception if the argument couldn't be parsed. */\n"
"    sipBadOperatorArg(sipSelf,sipArg,%s);\n"
"\n"
"    return NULL;\n"
                ,slotName(md->slot));
            break;

        default:
            if (isNumberSlot(md) || isRichCompareSlot(md))
            {
                /* We can't extend enum slots. */
                if (cd == NULL)
                    prcode(fp,
"\n"
"    Py_INCREF(Py_NotImplemented);\n"
"    return Py_NotImplemented;\n"
                        );
                else if (isNumberSlot(md))
                    prcode(fp,
"\n"
"    return sipPySlotExtend(&sipModuleAPI_%s,%s,NULL,sipArg0,sipArg1);\n"
                        , mod->name, slotName(md->slot));
                else
                    prcode(fp,
"\n"
"    return sipPySlotExtend(&sipModuleAPI_%s,%s,sipType_%C,sipSelf,sipArg);\n"
                        , mod->name, slotName(md->slot), fqcname);
            }
            else if (isInplaceNumberSlot(md))
                prcode(fp,
"\n"
"    PyErr_Clear();\n"
"\n"
"    Py_INCREF(Py_NotImplemented);\n"
"    return Py_NotImplemented;\n"
                    );
            else
                prcode(fp,
"\n"
"    /* Raise an exception if the arguments couldn't be parsed. */\n"
"    sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"    return %s;\n"
                    , pyname, md->pyname
                    ,ret_int ? "-1" : "0");
        }

    prcode(fp,
"}\n"
        );
}


/*
 * Generate the member functions for a class.
 */
static void generateClassFunctions(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    visibleList *vl;
    memberDef *md;

    /* Any shadow code. */
    if (hasShadow(cd))
    {
        generateShadowClassDeclaration(pt, cd, fp);
        generateShadowCode(pt, mod, cd, fp);
    }

    /* The member functions. */
    for (vl = cd->visible; vl != NULL; vl = vl->next)
        if (vl->m->slot == no_slot)
            generateFunction(vl->m, vl->cd->overs, cd, vl->cd, fp);

    /* The slot functions. */
    for (md = cd->members; md != NULL; md = md->next)
        if (cd->iff->type == namespace_iface)
            generateOrdinaryFunction(mod, cd, md, fp);
        else if (md->slot != no_slot && md->slot != unicode_slot)
            generateSlot(mod, cd, NULL, md, fp);

    if (cd->iff->type != namespace_iface && !generating_c)
    {
        classList *cl;
        int need_ptr, need_state;

        /* The cast function. */
        prcode(fp,
"\n"
"\n"
"/* Cast a pointer to a type somewhere in its superclass hierarchy. */\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void *cast_%C(void *, const sipTypeDef *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static void *cast_%C(void *ptr,const sipTypeDef *targetType)\n"
"{\n"
            ,classFQCName(cd));

        if (cd->supers != NULL)
            prcode(fp,
"    void *res;\n"
"\n"
                );

        prcode(fp,
"    if (targetType == sipType_%C)\n"
"        return ptr;\n"
            ,classFQCName(cd));

        for (cl = cd->supers; cl != NULL; cl = cl->next)
        {
            scopedNameDef *sname = cl->cd->iff->fqcname;

            prcode(fp,
"\n"
"    if ((res = ((const sipClassTypeDef *)sipType_%C)->ctd_cast((%S *)(%S *)ptr,targetType)) != NULL)\n"
"        return res;\n"
                ,sname,sname,classFQCName(cd));
        }

        prcode(fp,
"\n"
"    return NULL;\n"
"}\n"
            );

        /* Generate the release function without compiler warnings. */
        need_ptr = need_state = FALSE;

        if (cd->dealloccode != NULL)
            need_ptr = usedInCode(cd->dealloccode, "sipCpp");
        else if (canCreate(cd) || isPublicDtor(cd))
        {
            if (hasShadow(cd))
                need_ptr = need_state = TRUE;
            else if (isPublicDtor(cd))
                need_ptr = TRUE;
        }

        prcode(fp,
"\n"
"\n"
"/* Call the instance's destructor. */\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void release_%C(void *, int);}\n"
                , classFQCName(cd));

        prcode(fp,
"static void release_%C(void *%s,int%s)\n"
"{\n"
            , classFQCName(cd), (need_ptr ? "sipCppV" : ""), (need_state ? " state" : ""));

        if (cd->dealloccode != NULL)
        {
            if (need_ptr)
            {
                prcode(fp,
"    ");

                generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

                prcode(fp, ";\n"
                    );
            }

            generateCppCodeBlock(cd->dealloccode, fp);
        }
        else if (canCreate(cd) || isPublicDtor(cd))
        {
            int rgil = ((release_gil || isReleaseGILDtor(cd)) && !isHoldGILDtor(cd));

            /*
             * If there is an explicit public dtor then assume there is some
             * way to call it which we haven't worked out (because we don't
             * fully understand C++).
             */

            if (rgil)
                prcode(fp,
"    Py_BEGIN_ALLOW_THREADS\n"
"\n"
                    );

            if (hasShadow(cd))
            {
                prcode(fp,
"    if (state & SIP_DERIVED_CLASS)\n"
"        delete reinterpret_cast<sip%C *>(sipCppV);\n"
                    , classFQCName(cd));

                if (isPublicDtor(cd))
                    prcode(fp,
"    else\n"
"        delete reinterpret_cast<%U *>(sipCppV);\n"
                        , cd);
            }
            else if (isPublicDtor(cd))
                prcode(fp,
"    delete reinterpret_cast<%U *>(sipCppV);\n"
                    , cd);

            if (rgil)
                prcode(fp,
"\n"
"    Py_END_ALLOW_THREADS\n"
                    );
        }

        prcode(fp,
"}\n"
            );
    }

    /* The traverse function. */
    if (cd->travcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static int traverse_%C(void *, visitproc, void *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static int traverse_%C(void *sipCppV,visitproc sipVisit,void *sipArg)\n"
"{\n"
"    ", classFQCName(cd));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    int sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->travcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    /* The clear function. */
    if (cd->clearcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static int clear_%C(void *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static int clear_%C(void *sipCppV)\n"
"{\n"
"    ", classFQCName(cd));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    int sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->clearcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    /* The buffer interface functions. */
    if (cd->readbufcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static SIP_SSIZE_T getreadbuffer_%C(PyObject *, void *, SIP_SSIZE_T, void **);}\n"
                , classFQCName(cd));

        prcode(fp,
"static SIP_SSIZE_T getreadbuffer_%C(PyObject *%s, void *sipCppV, SIP_SSIZE_T %s, void **%s)\n"
"{\n"
"    ", classFQCName(cd)
     , argName("sipSelf", cd->readbufcode)
     , argName("sipSegment", cd->readbufcode)
     , argName("sipPtrPtr", cd->readbufcode));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    SIP_SSIZE_T sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->readbufcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    if (cd->writebufcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static SIP_SSIZE_T getwritebuffer_%C(PyObject *, void *, SIP_SSIZE_T, void **);}\n"
                , classFQCName(cd));

        prcode(fp,
"static SIP_SSIZE_T getwritebuffer_%C(PyObject *%s, void *sipCppV, SIP_SSIZE_T %s, void **%s)\n"
"{\n"
"    ", classFQCName(cd)
     , argName("sipSelf", cd->writebufcode)
     , argName("sipSegment", cd->writebufcode)
     , argName("sipPtrPtr", cd->writebufcode));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    SIP_SSIZE_T sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->writebufcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    if (cd->segcountcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static SIP_SSIZE_T getsegcount_%C(PyObject *, void *, SIP_SSIZE_T *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static SIP_SSIZE_T getsegcount_%C(PyObject *%s, void *sipCppV, SIP_SSIZE_T *%s)\n"
"{\n"
"    ", classFQCName(cd)
     , argName("sipSelf", cd->segcountcode)
     , argName("sipLenPtr", cd->segcountcode));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    SIP_SSIZE_T sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->segcountcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    if (cd->charbufcode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static SIP_SSIZE_T getcharbuffer_%C(PyObject *, void *, SIP_SSIZE_T, void **);}\n"
                , classFQCName(cd));

        prcode(fp,
"static SIP_SSIZE_T getcharbuffer_%C(PyObject *%s, void *sipCppV, SIP_SSIZE_T %s, void **%s)\n"
"{\n"
"    ", classFQCName(cd)
     , argName("sipSelf", cd->charbufcode)
     , argName("sipSegment", cd->charbufcode)
     , argName("sipPtrPtr", cd->charbufcode));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    SIP_SSIZE_T sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->charbufcode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    /* The pickle function. */
    if (cd->picklecode != NULL)
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static PyObject *pickle_%C(void *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static PyObject *pickle_%C(void *sipCppV)\n"
"{\n"
"    ", classFQCName(cd));

        generateClassFromVoid(cd, "sipCpp", "sipCppV", fp);

        prcode(fp, ";\n"
"    PyObject *sipRes;\n"
"\n"
            );

        generateCppCodeBlock(cd->picklecode, fp);

        prcode(fp,
"\n"
"    return sipRes;\n"
"}\n"
            );
    }

    /* The assignment helper. */
    if (optAssignmentHelpers(pt) && (generating_c || canAssign(cd)))
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void assign_%C(void *, const void*);}\n"
                , classFQCName(cd));

        prcode(fp,
"static void assign_%C(void *sipDst, const void *sipSrc)\n"
"{\n"
            ,classFQCName(cd));

        if (generating_c)
            prcode(fp,
"    *(%S *)sipDst = *(const %S *)sipSrc;\n"
                , classFQCName(cd), classFQCName(cd));
        else
            prcode(fp,
"    *reinterpret_cast<%S *>(sipDst) = *reinterpret_cast<const %S *>(sipSrc);\n"
                , classFQCName(cd), classFQCName(cd));

        prcode(fp,
"}\n"
            );
    }

    /* The dealloc function. */
    if (needDealloc(cd))
    {
        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static void dealloc_%C(sipSimpleWrapper *);}\n"
                , classFQCName(cd));

        prcode(fp,
"static void dealloc_%C(sipSimpleWrapper *sipSelf)\n"
"{\n"
            ,classFQCName(cd));

        if (tracing)
            prcode(fp,
"    sipTrace(SIP_TRACE_DEALLOCS,\"dealloc_%C()\\n\");\n"
"\n"
                ,classFQCName(cd));

        /* Disable the virtual handlers. */
        if (hasShadow(cd))
            prcode(fp,
"    if (sipIsDerived(sipSelf))\n"
"        reinterpret_cast<sip%C *>(sipSelf->u.cppPtr)->sipPySelf = NULL;\n"
"\n"
                ,classFQCName(cd));

        if (generating_c || isPublicDtor(cd) || (hasShadow(cd) && isProtectedDtor(cd)))
        {
            prcode(fp,
"    if (sipIsPyOwned(sipSelf))\n"
"    {\n"
                );

            if (isDelayedDtor(cd))
                prcode(fp,
"        sipAddDelayedDtor(sipSelf);\n"
                    );
            else if (generating_c)
                prcode(fp,
"        sipFree(sipSelf->u.cppPtr);\n"
                    );
            else
                prcode(fp,
"        release_%C(sipSelf->u.cppPtr,%s);\n"
                    , classFQCName(cd), (hasShadow(cd) ? "sipSelf->flags" : "0"));

            prcode(fp,
"    }\n"
                );
        }

        prcode(fp,
"}\n"
            );
    }

    /* The type initialisation function. */
    if (canCreate(cd))
        generateTypeInit(cd, fp);
}


/*
 * Generate the shadow (derived) class code.
 */
static void generateShadowCode(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    int nrVirts, virtNr;
    virtOverDef *vod;
    ctorDef *ct;

    nrVirts = countVirtuals(cd);

    /* Generate the wrapper class constructors. */

    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        char *prefix;
        int a;
        ctorDef *dct;

        if (isPrivateCtor(ct))
            continue;

        if (ct->cppsig == NULL)
            continue;

        /* Check we haven't already handled this C++ signature. */
        for (dct = cd->ctors; dct != ct; dct = dct->next)
            if (dct->cppsig != NULL && sameSignature(dct->cppsig, ct->cppsig, TRUE))
                break;

        if (dct != ct)
            continue;

        prcode(fp,
"\n"
"sip%C::sip%C(",classFQCName(cd),classFQCName(cd));

        generateCalledArgs(cd, ct->cppsig, Definition, TRUE, fp);

        prcode(fp,")%X: %S(",ct->exceptions,classFQCName(cd));

        prefix = "";

        for (a = 0; a < ct->cppsig->nrArgs; ++a)
        {
            prcode(fp,"%sa%d",prefix,a);
            prefix = ",";
        }

        prcode(fp,"), sipPySelf(0)\n"
"{\n"
            );

        if (tracing)
        {
            prcode(fp,
"    sipTrace(SIP_TRACE_CTORS,\"sip%C::sip%C(",classFQCName(cd),classFQCName(cd));
            generateCalledArgs(cd, ct->cppsig, Declaration, TRUE, fp);
            prcode(fp,")%X (this=0x%%08x)\\n\",this);\n"
"\n"
                ,ct->exceptions);
        }

        prcode(fp,
"    sipCommonCtor(%s,%d);\n"
"}\n"
            ,(nrVirts > 0 ? "sipPyMethods" : "NULL"),nrVirts);
    }

    /* The destructor. */

    if (!isPrivateDtor(cd))
    {
        prcode(fp,
"\n"
"sip%C::~sip%C()%X\n"
"{\n"
            ,classFQCName(cd),classFQCName(cd),cd->dtorexceptions);

        if (tracing)
            prcode(fp,
"    sipTrace(SIP_TRACE_DTORS,\"sip%C::~sip%C()%X (this=0x%%08x)\\n\",this);\n"
"\n"
                ,classFQCName(cd),classFQCName(cd),cd->dtorexceptions);

        if (cd->dtorcode != NULL)
            generateCppCodeBlock(cd->dtorcode,fp);

        prcode(fp,
"    sipCommonDtor(sipPySelf);\n"
"}\n"
            );
    }

    /* The meta methods if required. */
    if (pluginPyQt4(pt) && isQObjectSubClass(cd))
    {
        if (!noQMetaObject(cd))
            prcode(fp,
"\n"
"const QMetaObject *sip%C::metaObject() const\n"
"{\n"
"    return sip_%s_qt_metaobject(sipPySelf,sipType_%C);\n"
"}\n"
                , classFQCName(cd)
                , mod->name, classFQCName(cd));

        prcode(fp,
"\n"
"int sip%C::qt_metacall(QMetaObject::Call _c,int _id,void **_a)\n"
"{\n"
"    _id = %S::qt_metacall(_c,_id,_a);\n"
"\n"
"    if (_id >= 0)\n"
"        _id = sip_%s_qt_metacall(sipPySelf,sipType_%C,_c,_id,_a);\n"
"\n"
"    return _id;\n"
"}\n"
"\n"
"void *sip%C::qt_metacast(const char *_clname)\n"
"{\n"
"    return (sip_%s_qt_metacast && sip_%s_qt_metacast(sipPySelf,sipType_%C,_clname)) ? this : %S::qt_metacast(_clname);\n"
"}\n"
            , classFQCName(cd)
            , classFQCName(cd)
            , mod->name, classFQCName(cd)
            , classFQCName(cd)
            , mod->name, mod->name, classFQCName(cd), classFQCName(cd));
    }

    /* Generate the virtual catchers. */
 
    virtNr = 0;
 
    for (vod = cd->vmembers; vod != NULL; vod = vod->next)
    {
        overDef *od = &vod->o;
        virtOverDef *dvod;

        if (isPrivate(od))
            continue;

        /* Check we haven't already handled this C++ signature. */
        for (dvod = cd->vmembers; dvod != vod; dvod = dvod->next)
            if (strcmp(dvod->o.cppname,od->cppname) == 0 && sameSignature(dvod->o.cppsig,od->cppsig,TRUE))
                break;

        if (dvod != vod)
            continue;

        generateVirtualCatcher(mod, cd, virtNr++, vod, fp);
    }

    /* Generate the wrapper around each protected member function. */

    generateProtectedDefinitions(cd,fp);

    /* Generate the emitters if needed. */
    if (!optNoEmitters(pt))
        generateEmitters(cd, fp);
}


/*
 * Generate the emitter functions.
 */
static void generateEmitters(classDef *cd, FILE *fp)
{
    int noIntro;
    visibleList *vl;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        overDef *od;

        for (od = vl->cd->overs; od != NULL; od = od->next)
            if (od->common == vl->m && isSignal(od))
            {
                generateEmitter(cd,vl,fp);
                break;
            }
    }

    /* Generate the table of signals to support fan-outs. */

    noIntro = TRUE;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        overDef *od;

        for (od = vl->cd->overs; od != NULL; od = od->next)
            if (od->common == vl->m && isSignal(od))
            {
                if (noIntro)
                {
                    setHasSigSlots(cd);

                    prcode(fp,
"\n"
"static sipQtSignal signals_%C[] = {\n"
                        ,classFQCName(cd));

                    noIntro = FALSE;
                }

                prcode(fp,
"    {%N, %C_emit_%s},\n"
                    ,vl->m->pyname,classFQCName(cd),vl->m->pyname->text);

                break;
            }
    }

    if (!noIntro)
        prcode(fp,
"    {NULL, NULL}\n"
"};\n"
            );
}


/*
 * Generate the protected enums for a class.
 */
static void generateProtectedEnums(sipSpec *pt,classDef *cd,FILE *fp)
{
    enumDef *ed;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        char *eol;
        mroDef *mro;
        enumMemberDef *emd;

        if (!isProtectedEnum(ed))
            continue;

        /* See if the class defining the enum is in our class hierachy. */
        for (mro = cd->mro; mro != NULL; mro = mro->next)
            if (mro->cd == ed->ecd)
                break;

        if (mro == NULL)
            continue;

        prcode(fp,
"\n"
"    /* Expose this protected enum. */\n"
"    enum");

        if (ed->fqcname != NULL)
            prcode(fp," sip%s",scopedNameTail(ed->fqcname));

        prcode(fp," {");

        eol = "\n";

        for (emd = ed->members; emd != NULL; emd = emd->next)
        {
            prcode(fp,"%s"
"        %s = %S::%s",eol,emd->cname,classFQCName(ed->ecd),emd->cname);

            eol = ",\n";
        }

        prcode(fp,"\n"
"    };\n"
            );
    }
}


/*
 * Generate the catcher for a virtual function.
 */
static void generateVirtualCatcher(moduleDef *mod, classDef *cd, int virtNr,
        virtOverDef *vod, FILE *fp)
{
    overDef *od = &vod->o;
    virtHandlerDef *vhd = od->virthandler;
    argDef *res, *ad;
    signatureDef saved;
    int a;

    normaliseArgs(od->cppsig);

    res = &od->cppsig->result;

    if (res->atype == void_type && res->nrderefs == 0)
        res = NULL;

    prcode(fp,
"\n");

    generateBaseType(cd, &od->cppsig->result, TRUE, fp);

    prcode(fp," sip%C::%O(",classFQCName(cd),od);
    generateCalledArgs(cd, od->cppsig, Definition, TRUE, fp);
    prcode(fp,")%s%X\n"
"{\n"
        ,(isConst(od) ? " const" : ""),od->exceptions);

    if (tracing)
    {
        prcode(fp,
"    sipTrace(SIP_TRACE_CATCHERS,\"");

        generateBaseType(cd, &od->cppsig->result, TRUE, fp);
        prcode(fp," sip%C::%O(",classFQCName(cd),od);
        generateCalledArgs(cd, od->cppsig, Declaration, TRUE, fp);
        prcode(fp,")%s%X (this=0x%%08x)\\n\",this);\n"
"\n"
            ,(isConst(od) ? " const" : ""),od->exceptions);
    }

    restoreArgs(od->cppsig);

    saved = *vhd->cppsig;
    fakeProtectedArgs(vhd->cppsig);

    if (vhd->module == mod)
    {
        prcode(fp,
"    extern ");

        generateBaseType(cd, &od->cppsig->result, FALSE, fp);

        prcode(fp," sipVH_%s_%d(sip_gilstate_t,PyObject *",vhd->module->name,vhd->virthandlernr);
    }
    else
    {
        prcode(fp,
"    typedef ");

        generateBaseType(cd, &od->cppsig->result, FALSE, fp);

        prcode(fp," (*sipVH_%s_%d)(sip_gilstate_t,PyObject *",vhd->module->name,vhd->virthandlernr);
    }

    if (vhd->cppsig->nrArgs > 0)
    {
        prcode(fp,",");
        generateCalledArgs(cd, vhd->cppsig, Declaration, FALSE, fp);
    }

    prcode(fp,");\n"
        );

    *vhd->cppsig = saved;

    if (isNewThread(od))
        prcode(fp,
"\n"
"    SIP_BLOCK_THREADS\n"
            );

    prcode(fp,
"\n"
"    sip_gilstate_t sipGILState;\n"
"    PyObject *meth;\n"
"\n"
"    meth = sipIsPyMethod(&sipGILState,");

    if (isConst(od))
        prcode(fp,"const_cast<sipMethodCache *>(");

    prcode(fp,"&sipPyMethods[%d]",virtNr);

    if (isConst(od))
        prcode(fp,")");

    prcode(fp,",sipPySelf,");

    if (isAbstract(od))
        prcode(fp, "%N", cd->pyname);
    else
        prcode(fp,"NULL");

    prcode(fp,",%N);\n"
"\n"
        ,od->common->pyname);

    if (isNewThread(od))
        prcode(fp,
"    if (meth)\n"
"    {\n"
"        sipStartThread();\n"
"        ");
    else
    {
        prcode(fp,
"    if (!meth)\n"
            );

        if (isAbstract(od))
            generateVirtHandlerErrorReturn(res,fp);
        else
        {
            if (res == NULL)
                prcode(fp,
"    {\n"
"        ");
            else
                prcode(fp,
"        return ");

            generateUnambiguousClass(cd,vod->scope,fp);

            prcode(fp,"::%O(",od);
 
            for (a = 0; a < od->cppsig->nrArgs; ++a)
                prcode(fp,"%sa%d",(a == 0 ? "" : ","),a);
 
            prcode(fp,");\n"
                );

            if (res == NULL)
                prcode(fp,
"        return;\n"
"    }\n"
                    );
        }

        prcode(fp,
"\n"
"    ");

        if (res != NULL)
            prcode(fp,"return ");
    }

    if (vhd->module == mod)
        prcode(fp,"sipVH_%s_%d",vhd->module->name,vhd->virthandlernr);
    else
        prcode(fp,"((sipVH_%s_%d)(sipModuleAPI_%s_%s->em_virthandlers[%d]))",vhd->module->name,vhd->virthandlernr,mod->name,vhd->module->name,vhd->virthandlernr);

    prcode(fp,"(sipGILState,meth");

    ad = od->cppsig->args;

    for (a = 0; a < od->cppsig->nrArgs; ++a)
    {
        if (ad->atype == class_type && isProtectedClass(ad->u.cd))
            prcode(fp, ",%sa%d", ((isReference(ad) || ad->nrderefs == 0) ? "&" : ""), a);
        else if (ad->atype == enum_type && isProtectedEnum(ad->u.ed))
            prcode(fp, ",(%E)a%d", ad->u.ed, a);
        else
            prcode(fp,",a%d",a);

        ++ad;
    }
 
    prcode(fp,");\n"
        );

    if (isNewThread(od))
        prcode(fp,
"        sipEndThread();\n"
"    }\n"
"\n"
"    SIP_UNBLOCK_THREADS\n"
            );

    prcode(fp,
"}\n"
        );
}


/*
 * Generate the scope of the near class of a virtual taking duplicate
 * super-classes into account.
 */
static void generateUnambiguousClass(classDef *cd,classDef *scope,FILE *fp)
{
    mroDef *mro;

    /* See if the near class has a duplicate. */
    for (mro = cd->mro; mro != NULL; mro = mro->next)
        if (mro->cd == scope)
        {
            if (hasDuplicateSuper(mro))
            {
                mroDef *guardc;

                /*
                 * Backtrack to find the class that directly
                 * sub-classes the duplicated one.  This will
                 * be the one that disambiguates the duplicated
                 * one.
                 */
                guardc = mro;

                while (guardc != cd->mro)
                {
                    mroDef *sub;
                    classList *cl;

                    for (sub = cd->mro; sub->next != guardc; sub = sub->next)
                        ;

                    for (cl = sub->cd->supers; cl != NULL; cl = cl->next)
                        if (cl->cd == mro->cd)
                        {
                            prcode(fp,"%S",classFQCName(sub->cd));

                            return;
                        }

                    /* Try the previous one. */
                    guardc = sub;
                }
            }

            break;
        }

    /* If we got here there is nothing to worry about. */
    prcode(fp,"%S",classFQCName(scope));
}


/*
 * Generate a cast to zero.
 */
static void generateCastZero(argDef *ad,FILE *fp)
{
    if (ad->atype == enum_type)
        prcode(fp,"(%E)",ad->u.ed);

    prcode(fp,"0");
}


/*
 * Generate the return statement for a virtual handler when there has been an
 * error (ie. there is nothing sensible to return).
 */
static void generateVirtHandlerErrorReturn(argDef *res,FILE *fp)
{
    prcode(fp,
"        return");

    if (res == NULL)
    {
        prcode(fp,";\n"
            );

        return;
    }

    prcode(fp," ");

    if (res->atype == mapped_type && res->nrderefs == 0)
    {
        argDef res_noconstref;

        /*
         * We don't know anything about the mapped type so we just hope
         * is has a default ctor.
         */

        if (isReference(res))
            prcode(fp,"*new ");

        res_noconstref = *res;
        resetIsConstArg(&res_noconstref);
        resetIsReference(&res_noconstref);
        prcode(fp,"%B()",&res_noconstref);
    }
    else if (res->atype == class_type && res->nrderefs == 0)
    {
        ctorDef *ct = res->u.cd->defctor;

        /*
         * If we don't have a suitable ctor then the generated code
         * will issue an error message.
         */
        if (ct != NULL && isPublicCtor(ct) && ct->cppsig != NULL)
        {
            argDef res_noconstref;

            /*
             * If this is a badly designed class.  We can only
             * generate correct code by leaking memory.
             */
            if (isReference(res))
                prcode(fp,"*new ");

            res_noconstref = *res;
            resetIsConstArg(&res_noconstref);
            resetIsReference(&res_noconstref);
            prcode(fp,"%B",&res_noconstref);

            generateCallDefaultCtor(ct,fp);
        }
        else
        {
            fatalScopedName(classFQCName(res->u.cd));
            fatal(" must have a default constructor\n");
        }
    }
    else
        generateCastZero(res,fp);

    prcode(fp,";\n"
        );
}


/*
 * Generate the call to a default ctor.
 */
static void generateCallDefaultCtor(ctorDef *ct, FILE *fp)
{
    int a;

    prcode(fp, "(");

    for (a = 0; a < ct->cppsig->nrArgs; ++a)
    {
        argDef *ad = &ct->cppsig->args[a];

        if (ad->defval != NULL)
            break;

        if (a > 0)
            prcode(fp, ",");

        /*
         * Do what we can to provide type information to the compiler.
         */
        if (ad->atype == class_type && ad->nrderefs > 0 && !isReference(ad))
            prcode(fp, "static_cast<%B>(0)", ad);
        else if (ad->atype == enum_type)
            prcode(fp, "static_cast<%E>(0)", ad->u.ed);
        else if (ad->atype == float_type || ad->atype == cfloat_type)
            prcode(fp, "0.0F");
        else if (ad->atype == double_type || ad->atype == cdouble_type)
            prcode(fp, "0.0");
        else if (ad->atype == uint_type)
            prcode(fp, "0U");
        else if (ad->atype == long_type || ad->atype == longlong_type)
            prcode(fp, "0L");
        else if (ad->atype == ulong_type || ad->atype == ulonglong_type)
            prcode(fp, "0UL");
        else if ((ad->atype == ustring_type || ad->atype == sstring_type || ad->atype == string_type) && ad->nrderefs == 0)
            prcode(fp, "'\\0'");
        else if (ad->atype == wstring_type && ad->nrderefs == 0)
            prcode(fp, "L'\\0'");
        else
            prcode(fp, "0");
    }

    prcode(fp, ")");
}


/*
 * Generate the emitter function for a signal.
 */
static void generateEmitter(classDef *cd, visibleList *vl, FILE *fp)
{
    const char *pname = vl->m->pyname->text;
    overDef *od;

    prcode(fp,
"\n"
"int sip%C::sipEmit_%s(PyObject *sipArgs)\n"
"{\n"
"    int sipArgsParsed = 0;\n"
        ,classFQCName(cd),pname);

    for (od = vl->cd->overs; od != NULL; od = od->next)
    {
        int rgil = ((release_gil || isReleaseGIL(od)) && !isHoldGIL(od));

        if (od->common != vl->m || !isSignal(od))
            continue;

        /*
         * Generate the code that parses the args and emits the
         * appropriate overloaded signal.
         */
        prcode(fp,
"\n"
"    {\n"
            );

        generateArgParser(&od->pysig, cd, NULL, NULL, FALSE, fp);

        prcode(fp,
"        {\n"
            );

        if (rgil)
            prcode(fp,
"            Py_BEGIN_ALLOW_THREADS\n"
                );

        prcode(fp,
"            emit %s("
            ,od->cppname);

        generateCallArgs(cd, od->cppsig, &od->pysig, fp);

        prcode(fp,");\n"
            );

        if (rgil)
            prcode(fp,
"            Py_END_ALLOW_THREADS\n"
                );

        deleteTemps(&od->pysig, fp);

        prcode(fp,
"\n"
"            return 0;\n"
"        }\n"
"    }\n"
            );
    }

    prcode(fp,
"\n"
"    sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"    return -1;\n"
"}\n"
"\n"
        , cd->pyname, vl->m->pyname);

    if (!generating_c)
        prcode(fp,
"extern \"C\" {static int %C_emit_%s(sipSimpleWrapper *, PyObject *);}\n"
            , classFQCName(cd), pname);

    prcode(fp,
"static int %C_emit_%s(sipSimpleWrapper *sw,PyObject *sipArgs)\n"
"{\n"
"    sip%C *ptr = reinterpret_cast<sip%C *>(sipGetComplexCppPtr(sw));\n"
"\n"
"    return (ptr ? ptr->sipEmit_%s(sipArgs) : -1);\n"
"}\n"
        ,classFQCName(cd),pname
        ,classFQCName(cd),classFQCName(cd)
        ,pname);
}


/*
 * Generate the declarations of the protected wrapper functions for a class.
 */

static void generateProtectedDeclarations(classDef *cd,FILE *fp)
{
    int noIntro;
    visibleList *vl;

    noIntro = TRUE;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        overDef *od;

        if (vl->m->slot != no_slot)
            continue;

        for (od = vl->cd->overs; od != NULL; od = od->next)
        {
            if (od->common != vl->m || !isProtected(od))
                continue;

            if (noIntro)
            {
                prcode(fp,
"\n"
"    /*\n"
"     * There is a public method for every protected method visible from\n"
"     * this class.\n"
"     */\n"
                    );

                noIntro = FALSE;
            }

            prcode(fp,
"    ");

            if (isStatic(od))
                prcode(fp,"static ");

            generateBaseType(cd, &od->cppsig->result, TRUE, fp);

            if (!isStatic(od) && !isAbstract(od) && (isVirtual(od) || isVirtualReimp(od)))
            {
                prcode(fp, " sipProtectVirt_%s(bool", od->cppname);

                if (od->cppsig->nrArgs > 0)
                    prcode(fp, ",");
            }
            else
                prcode(fp, " sipProtect_%s(", od->cppname);

            generateCalledArgs(cd, od->cppsig, Declaration, TRUE, fp);
            prcode(fp,")%s;\n"
                ,(isConst(od) ? " const" : ""));
        }
    }
}


/*
 * Generate the definitions of the protected wrapper functions for a class.
 */
static void generateProtectedDefinitions(classDef *cd,FILE *fp)
{
    visibleList *vl;

    for (vl = cd->visible; vl != NULL; vl = vl->next)
    {
        overDef *od;

        if (vl->m->slot != no_slot)
            continue;

        for (od = vl->cd->overs; od != NULL; od = od->next)
        {
            char *mname = od->cppname;
            int parens;
            argDef *res;

            if (od->common != vl->m || !isProtected(od))
                continue;

            prcode(fp,
"\n"
                );

            generateBaseType(cd, &od->cppsig->result, TRUE, fp);

            if (!isStatic(od) && !isAbstract(od) && (isVirtual(od) || isVirtualReimp(od)))
            {
                prcode(fp, " sip%C::sipProtectVirt_%s(bool sipSelfWasArg", classFQCName(cd), mname);

                if (od->cppsig->nrArgs > 0)
                    prcode(fp, ",");
            }
            else
                prcode(fp, " sip%C::sipProtect_%s(", classFQCName(cd), mname);

            generateCalledArgs(cd, od->cppsig, Definition, TRUE, fp);
            prcode(fp,")%s\n"
"{\n"
                ,(isConst(od) ? " const" : ""));

            parens = 1;

            res = &od->cppsig->result;

            if (res->atype == void_type && res->nrderefs == 0)
                prcode(fp,
"    ");
            else
            {
                prcode(fp,
"    return ");

                if (res->atype == class_type && isProtectedClass(res->u.cd))
                {
                    prcode(fp,"static_cast<%U *>(",res->u.cd);
                    ++parens;
                }
                else if (res->atype == enum_type && isProtectedEnum(res->u.ed))
                    /*
                     * One or two older compilers can't handle a static_cast
                     * here so we revert to a C-style cast.
                     */
                    prcode(fp,"(%E)",res->u.ed);
            }

            if (!isAbstract(od))
            {
                if (isVirtual(od) || isVirtualReimp(od))
                {
                    prcode(fp, "(sipSelfWasArg ? %S::%s(", classFQCName(vl->cd), mname);

                    generateProtectedCallArgs(od, fp);

                    prcode(fp, ") : ");
                    ++parens;
                }
                else
                    prcode(fp, "%S::", classFQCName(vl->cd));
            }

            prcode(fp,"%s(",mname);

            generateProtectedCallArgs(od, fp);

            while (parens--)
                prcode(fp,")");

            prcode(fp,";\n"
"}\n"
                );
        }
    }
}


/*
 * Generate the arguments for a call to a protected method.
 */
static void generateProtectedCallArgs(overDef *od, FILE *fp)
{
    int a;

    for (a = 0; a < od->cppsig->nrArgs; ++a)
    {
        argDef *ad = &od->cppsig->args[a];

        if (a > 0)
            prcode(fp, ",");

        if (ad->atype == enum_type && isProtectedEnum(ad->u.ed))
            prcode(fp, "(%S)", ad->u.ed->fqcname);

        prcode(fp, "a%d", a);
    }
}


/*
 * Generate the function that does most of the work to handle a particular
 * virtual function.
 */
static void generateVirtualHandler(virtHandlerDef *vhd, FILE *fp)
{
    int a, nrvals, copy, isref;
    argDef *res, res_noconstref;
    signatureDef saved;

    res = &vhd->cppsig->result;

    copy = isref = FALSE;

    if (res->atype == void_type && res->nrderefs == 0)
        res = NULL;
    else
    {
        /*
         * If we are returning a reference to an instance then we take care to
         * handle Python errors but still return a valid C++ instance.  If we
         * are returning an instance then we take care to make a local copy of
         * the instance returned from Python before the Python object is
         * garbage collected and the C++ instance (possibly) destroyed.
         */
        if ((res->atype == class_type || res->atype == mapped_type) && res->nrderefs == 0)
        {
            if (isReference(res))
                isref = TRUE;
            else
                copy = TRUE;
        }

        res_noconstref = *res;
        resetIsConstArg(&res_noconstref);
        resetIsReference(&res_noconstref);
    }

    prcode(fp,
"\n"
        );

    saved = *vhd->cppsig;
    fakeProtectedArgs(vhd->cppsig);

    generateBaseType(NULL, &vhd->cppsig->result, FALSE, fp);

    prcode(fp," sipVH_%s_%d(sip_gilstate_t sipGILState,PyObject *sipMethod"
        , vhd->module->name, vhd->virthandlernr);

    if (vhd->cppsig->nrArgs > 0)
    {
        prcode(fp,",");

        generateCalledArgs(NULL, vhd->cppsig, Definition, FALSE, fp);
    }

    *vhd->cppsig = saved;

    prcode(fp,")\n"
"{\n"
        );

    if (res != NULL)
    {
        prcode(fp, "    ");

        /*
         * wchar_t * return values are always on the heap.  To reduce memory
         * leaks we keep the last result around until we have a new one.  This
         * means that ownership of the return value stays with the function
         * returning it - which is consistent with how other types work, even
         * thought it may not be what's required in all cases.
         */
        if (res->atype == wstring_type && res->nrderefs == 1)
            prcode(fp, "static ");

        generateBaseType(NULL, &res_noconstref, FALSE, fp);

        prcode(fp," %ssipRes",(isref ? "*" : ""));

        if (copy && res->atype == class_type && res->nrderefs == 0)
        {
            ctorDef *ct = res->u.cd->defctor;

            if (ct != NULL && isPublicCtor(ct) && ct->cppsig != NULL && ct->cppsig->nrArgs > 0 && ct->cppsig->args[0].defval == NULL)
                generateCallDefaultCtor(ct,fp);
        }
        else if (!copy)
        {
            /*
             * We initialise the result to try and suppress a
             * compiler warning.
             */
            prcode(fp," = ");
            generateCastZero(res,fp);
        }

        prcode(fp,";\n"
            );

        if (res->atype == wstring_type && res->nrderefs == 1)
            prcode(fp,
"\n"
"    if (sipRes)\n"
"    {\n"
"        // Return any previous result to the heap.\n"
"        sipFree(%s);\n"
"        sipRes = 0;\n"
"    }\n"
"\n"
                , (isConstArg(res) ? "const_cast<wchar_t *>(sipRes)" : "sipRes"));
    }

    if (vhd->virtcode != NULL)
    {
        int error_flag = needErrorFlag(vhd->virtcode);

        if (error_flag)
            prcode(fp,
"    int sipIsErr = 0;\n"
                );

        prcode(fp,
"\n"
            );

        generateCppCodeBlock(vhd->virtcode,fp);

        if (error_flag)
            prcode(fp,
"\n"
"    if (sipIsErr)\n"
"        PyErr_Print();\n"
                );

        prcode(fp,
"\n"
"    Py_DECREF(sipMethod);\n"
"\n"
"    SIP_RELEASE_GIL(sipGILState)\n"
            );

        if (res != NULL)
            prcode(fp,
"\n"
"    return sipRes;\n"
                );

        prcode(fp,
"}\n"
            );

        return;
    }

    /* See how many values we expect. */
    nrvals = (res != NULL ? 1 : 0);

    for (a = 0; a < vhd->pysig->nrArgs; ++a)
        if (isOutArg(&vhd->pysig->args[a]))
            ++nrvals;

    if (copy)
    {
        prcode(fp,
"    ");

        generateBaseType(NULL, &res_noconstref, FALSE, fp);

        prcode(fp," *sipResOrig;\n");

        if (res->atype == class_type && res->u.cd->convtocode != NULL)
            prcode(fp,
"    int sipResState;\n"
                );
    }

    /* Call the method. */
    prcode(fp,
"    PyObject *sipResObj = sipCallMethod(0,sipMethod,");

    saved = *vhd->pysig;
    fakeProtectedArgs(vhd->pysig);
    generateTupleBuilder(vhd->pysig, fp);
    *vhd->pysig = saved;

    prcode(fp,");\n"
"\n"
"    %s (!sipResObj || sipParseResult(0,sipMethod,sipResObj,\"",(isref ? "int sipIsErr =" : "if"));

    /* Build the format string. */
    if (nrvals == 0)
        prcode(fp,"Z");
    else
    {
        if (nrvals > 1)
            prcode(fp,"(");

        if (res != NULL)
            prcode(fp, "%s", getParseResultFormat(res, TRUE, isTransferVH(vhd)));

        for (a = 0; a < vhd->pysig->nrArgs; ++a)
        {
            argDef *ad = &vhd->pysig->args[a];

            if (isOutArg(ad))
                prcode(fp, "%s", getParseResultFormat(ad, FALSE, FALSE));
        }

        if (nrvals > 1)
            prcode(fp,")");
    }

    prcode(fp,"\"");

    /* Pass the destination pointers. */
    if (res != NULL)
    {
        generateParseResultExtraArgs(res, TRUE, fp);
        prcode(fp,",&sipRes%s",(copy ? "Orig" : ""));
    }

    for (a = 0; a < vhd->pysig->nrArgs; ++a)
    {
        argDef *ad = &vhd->pysig->args[a];

        if (isOutArg(ad))
        {
            generateParseResultExtraArgs(ad, FALSE, fp);
            prcode(fp,",%sa%d",(isReference(ad) ? "&" : ""),a);
        }
    }

    if (isref)
        prcode(fp,") < 0);\n"
"\n"
"    if (sipIsErr)\n"
            );
    else
        prcode(fp,") < 0)\n"
            );

    prcode(fp,
"        PyErr_Print();\n"
        );

    /* Make a copy if needed. */
    if (copy)
    {
        prcode(fp,
"    else\n"
"    {\n"
"        sipRes = *sipResOrig;\n"
            );

        if (res->atype == mapped_type)
            prcode(fp,
"        delete sipResOrig;\n"
                );
        else if (res->atype == class_type && res->u.cd->convtocode != NULL)
            prcode(fp,
"        sipReleaseType(sipResOrig,sipType_%C,sipResState);\n"
                , classFQCName(res->u.cd));

        prcode(fp,
"    }\n"
            );
    }

    prcode(fp,
"\n"
"    Py_XDECREF(sipResObj);\n"
"    Py_DECREF(sipMethod);\n"
"\n"
"    SIP_RELEASE_GIL(sipGILState)\n"
        );

    if (res != NULL)
    {
        if (isref)
        {
            prcode(fp,
"\n"
"    if (sipIsErr)\n"
                );

            generateVirtHandlerErrorReturn(res,fp);
        }

        prcode(fp,
"\n"
"    return %ssipRes;\n"
            ,(isref ? "*" : ""));
    }

    prcode(fp,
"}\n"
        );
}


/*
 * Generate the extra arguments needed by sipParseResult() for a particular
 * type.
 */
static void generateParseResultExtraArgs(argDef *ad, int isres, FILE *fp)
{
    switch (ad->atype)
    {
    case mapped_type:
        prcode(fp, ",sipType_%T", ad);
        break;

    case class_type:
        prcode(fp, ",sipType_%C", classFQCName(ad->u.cd));

        if (isres && ad->nrderefs == 0 && ad->u.cd->convtocode != NULL && !isReference(ad))
            prcode(fp, ",&sipResState");

        break;

    case pytuple_type:
        prcode(fp,",&PyTuple_Type");
        break;

    case pylist_type:
        prcode(fp,",&PyList_Type");
        break;

    case pydict_type:
        prcode(fp,",&PyDict_Type");
        break;

    case pyslice_type:
        prcode(fp,",&PySlice_Type");
        break;

    case pytype_type:
        prcode(fp,",&PyType_Type");
        break;

    case enum_type:
        if (ad->u.ed->fqcname != NULL)
            prcode(fp, ",sipType_%C", ad->u.ed->fqcname);
        break;
    }
}


/*
 * Return the format characters used by sipParseResult() for a particular type.
 */
static const char *getParseResultFormat(argDef *ad, int isres, int xfervh)
{
    switch (ad->atype)
    {
    case mapped_type:
        {
            static const char *s[] = {
                "D0", "D1", "D2", "D3",
                "D4", "D5", "D6", "D7"
            };

            int f = 0x04;

            if (isres && ad->nrderefs == 0)
                f |= 0x01;

            if (isres && xfervh)
                f |= 0x02;

            return s[f];
        }

    case fake_void_type:
    case class_type:
        {
            static char s[] = "D?";

            int f = 0x04;

            if (isres && ad->nrderefs == 0)
            {
                f |= 0x01;

                if (ad->u.cd->convtocode != NULL)
                {
                    f &= ~0x04;

                    /*
                     * If it is a reference then we are going to return the
                     * dereference.  To make sure it remains valid we can
                     * either leak the temporary from the %ConvertToCode or we
                     * can suppress the %ConvertToCode.  We choose the latter.
                     */
                    if (isReference(ad))
                        f |= 0x10;
                }
            }

            if (isres && xfervh)
                f |= 0x02;

            s[1] = '0' + f;

            return s;
        }

    case bool_type:
    case cbool_type:
        return "b";

    case sstring_type:
    case ustring_type:
    case string_type:
        return ((ad->nrderefs == 0) ? "c" : "s");

    case wstring_type:
        return ((ad->nrderefs == 0) ? "w" : "x");

    case enum_type:
        return ((ad->u.ed->fqcname != NULL) ? "F" : "e");

    case ushort_type:
        return "t";

    case short_type:
        return "h";

    case int_type:
    case cint_type:
        return "i";

    case uint_type:
        return "u";

    case long_type:
        return "l";

    case ulong_type:
        return "m";

    case longlong_type:
        return "n";

    case ulonglong_type:
        return "o";

    case void_type:
    case struct_type:
        return "V";

    case float_type:
    case cfloat_type:
        return "f";

    case double_type:
    case cdouble_type:
        return "d";

    case pyobject_type:
        return "O";

    case pytuple_type:
    case pylist_type:
    case pydict_type:
    case pyslice_type:
    case pytype_type:
        return (isAllowNone(ad) ? "N" : "T");
    }

    /* We should never get here. */
    return " ";
}


/*
 * Generate the code to build a tuple of Python arguments.
 */
static void generateTupleBuilder(signatureDef *sd,FILE *fp)
{
    int a, arraylenarg;

    prcode(fp,"\"");

    for (a = 0; a < sd->nrArgs; ++a)
    {
        char *fmt = "";
        argDef *ad = &sd->args[a];

        if (!isInArg(ad))
            continue;

        switch (ad->atype)
        {
        case sstring_type:
        case ustring_type:
        case string_type:
            if (ad->nrderefs == 0 || (ad->nrderefs == 1 && isOutArg(ad)))
                fmt = "c";
            else if (isArray(ad))
                fmt = "g";
            else
                fmt = "s";

            break;

        case wstring_type:
            if (ad->nrderefs == 0 || (ad->nrderefs == 1 && isOutArg(ad)))
                fmt = "w";
            else if (isArray(ad))
                fmt = "G";
            else
                fmt = "x";

            break;

        case bool_type:
        case cbool_type:
            fmt = "b";
            break;

        case enum_type:
            fmt = (ad->u.ed->fqcname != NULL) ? "F" : "e";
            break;

        case cint_type:
            fmt = "i";
            break;

        case uint_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "u";

            break;

        case int_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "i";

            break;

        case ushort_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "t";

            break;

        case short_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "h";

            break;

        case long_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "l";

            break;

        case ulong_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "m";

            break;

        case longlong_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "n";

            break;

        case ulonglong_type:
            if (isArraySize(ad))
                arraylenarg = a;
            else
                fmt = "o";

            break;

        case struct_type:
        case void_type:
            fmt = "V";
            break;

        case float_type:
        case cfloat_type:
            fmt = "f";
            break;

        case double_type:
        case cdouble_type:
            fmt = "d";
            break;

        case signal_type:
        case slot_type:
        case slotcon_type:
        case slotdis_type:
            fmt = "s";
            break;

        case mapped_type:
        case fake_void_type:
        case class_type:
        case rxcon_type:
        case rxdis_type:
        case qobject_type:
            fmt = "D";
            break;

        case pyobject_type:
        case pytuple_type:
        case pylist_type:
        case pydict_type:
        case pycallable_type:
        case pyslice_type:
        case pytype_type:
            fmt = "S";
            break;
        }

        prcode(fp,fmt);
    }

    prcode(fp,"\"");

    for (a = 0; a < sd->nrArgs; ++a)
    {
        int derefs;
        argDef *ad = &sd->args[a];

        if (!isInArg(ad))
            continue;

        derefs = ad->nrderefs;

        switch (ad->atype)
        {
        case sstring_type:
        case ustring_type:
        case string_type:
        case wstring_type:
            if (!(ad->nrderefs == 0 || (ad->nrderefs == 1 && isOutArg(ad))))
                --derefs;

            break;

        case mapped_type:
        case fake_void_type:
        case class_type:
            if (ad->nrderefs > 0)
                --derefs;

            break;

        case struct_type:
        case void_type:
            --derefs;
            break;
        }

        if (ad->atype == mapped_type || ad->atype == class_type ||
            ad->atype == rxcon_type || ad->atype == rxdis_type ||
            ad->atype == qobject_type || ad->atype == fake_void_type)
        {
            prcode(fp,",");

            if (isConstArg(ad))
                prcode(fp,"const_cast<%b *>(",ad);

            if (ad->nrderefs == 0)
                prcode(fp,"&");
            else
                while (derefs-- != 0)
                    prcode(fp,"*");

            prcode(fp,"a%d",a);

            if (isConstArg(ad))
                prcode(fp,")");

            if (ad->atype == mapped_type)
                prcode(fp, ",sipType_%T,NULL", ad);
            else if (ad->atype == fake_void_type || ad->atype == class_type)
                prcode(fp, ",sipType_%C,NULL", classFQCName(ad->u.cd));
            else
                prcode(fp,",sipType_QObject,NULL");
        }
        else
        {
            if (!isArraySize(ad))
            {
                prcode(fp, ",");

                while (derefs-- != 0)
                    prcode(fp, "*");

                prcode(fp, "a%d", a);
            }

            if (isArray(ad))
                prcode(fp, ",(SIP_SSIZE_T)a%d", arraylenarg);
            else if (ad->atype == enum_type && ad->u.ed->fqcname != NULL)
                prcode(fp, ",sipType_%C", ad->u.ed->fqcname);
        }
    }
}


/*
 * Generate the library header #include directives required by either a class
 * or a module.
 */
static void generateUsedIncludes(ifaceFileList *iffl, FILE *fp)
{
    prcode(fp,
"\n"
        );

    while (iffl != NULL)
    {
        generateCppCodeBlock(iffl->iff->hdrcode, fp);
        iffl = iffl->next;
    }
}


/*
 * Generate the API details for a module.
 */
static void generateModuleAPI(sipSpec *pt, moduleDef *mod, FILE *fp)
{
    classDef *cd;
    mappedTypeDef *mtd;
    exceptionDef *xd;

    for (cd = pt->classes; cd != NULL; cd = cd->next)
        if (cd->iff->module == mod)
            generateClassAPI(cd, pt, fp);

    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff->module == mod)
            generateMappedTypeAPI(pt, mtd, fp);

    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
        if (xd->iff->module == mod && xd->exceptionnr >= 0)
            prcode(fp,
"\n"
"#define sipException_%C sipModuleAPI_%s.em_exceptions[%d]\n"
                , xd->iff->fqcname, mod->name, xd->exceptionnr);
}


/*
 * Generate the API details for an imported module.
 */
static void generateImportedModuleAPI(sipSpec *pt, moduleDef *mod,
        moduleDef *immod, FILE *fp)
{
    classDef *cd;
    mappedTypeDef *mtd;
    exceptionDef *xd;

    for (cd = pt->classes; cd != NULL; cd = cd->next)
        if (cd->iff->module == immod && !isExternal(cd))
            generateImportedClassAPI(cd, pt, mod, fp);

    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff->module == immod)
            generateImportedMappedTypeAPI(mtd, mod, fp);

    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
        if (xd->iff->module == immod && xd->exceptionnr >= 0)
                prcode(fp,
"\n"
"#define sipException_%C sipModuleAPI_%s_%s->em_exceptions[%d]\n"
                    , xd->iff->fqcname, mod->name, xd->iff->module->name, xd->exceptionnr);
}


/*
 * Generate the API details for an imported mapped type.
 */
static void generateImportedMappedTypeAPI(mappedTypeDef *mtd, moduleDef *mod,
        FILE *fp)
{
    const char *mname = mod->name;
    const char *imname = mtd->iff->module->name;
    argDef type;

    memset(&type, 0, sizeof (argDef));

    type.atype = mapped_type;
    type.u.mtd = mtd;

    prcode(fp,
"\n"
"#define sipType_%T      sipModuleAPI_%s_%s->em_mappedtypes[%d]\n"
        , &type, mname, imname, mtd->mappednr);
}


/*
 * Generate the API details for a mapped type.
 */
static void generateMappedTypeAPI(sipSpec *pt, mappedTypeDef *mtd, FILE *fp)
{
    prcode(fp,
"\n"
"#define sipType_%T      sipModuleAPI_%s.em_mappedtypes[%d]\n"
"\n"
"extern sipMappedTypeDef sipMappedTypeDef_%T;\n"
        , &mtd->type, mtd->iff->module->name, mtd->mappednr
        , &mtd->type);
}


/*
 * Generate the API details for an imported class.
 */
static void generateImportedClassAPI(classDef *cd, sipSpec *pt, moduleDef *mod,
        FILE *fp)
{
    const char *mname = mod->name;
    const char *imname = cd->iff->module->name;

    prcode(fp,
"\n"
        );

    if (cd->iff->type == namespace_iface)
        prcode(fp,
"#if !defined(sipType_%C)\n"
            , classFQCName(cd));

    prcode(fp,
"#define sipType_%C              sipModuleAPI_%s_%s->em_classtypes[%d]\n"
"#define sipClass_%C             sipModuleAPI_%s_%s->em_classtypes[%d]->u.td_wrapper_type\n"
        , classFQCName(cd), mname, imname, cd->classnr
        , classFQCName(cd), mname, imname, cd->classnr);

    if (cd->iff->type == namespace_iface)
        prcode(fp,
"#endif\n"
            );

    generateEnumMacros(pt, mod, cd, fp);
}


/*
 * Generate the C++ API for a class.
 */
static void generateClassAPI(classDef *cd, sipSpec *pt, FILE *fp)
{
    const char *mname = cd->iff->module->name;

    prcode(fp,
"\n"
            );

    if (cd->real == NULL)
        prcode(fp,
"#define sipType_%C              sipModuleAPI_%s.em_classtypes[%d]\n"
"#define sipClass_%C             sipModuleAPI_%s.em_classtypes[%d]->u.td_wrapper_type\n"
            , classFQCName(cd), mname, cd->classnr
            , classFQCName(cd), mname, cd->classnr);

    generateEnumMacros(pt, cd->iff->module, cd, fp);

    if (!isExternal(cd))
    {
        const char *type_prefix;

        if (pluginPyQt4(pt))
            type_prefix = "pyqt4";
        else
            type_prefix = "sip";

        prcode(fp,
"\n"
"extern %sClassTypeDef %sType_%s_%C;\n"
            , type_prefix, type_prefix, mname, classFQCName(cd));
    }
}


/*
 * Generate the sipEnum_* macros.
 */
static void generateEnumMacros(sipSpec *pt, moduleDef *mod, classDef *cd,
        FILE *fp)
{
    enumDef *ed;
    int noIntro = TRUE;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->fqcname == NULL || ed->ecd != cd)
            continue;

        if (noIntro)
        {
            prcode(fp,
"\n"
                );

            noIntro = FALSE;
        }

        if (mod == ed->module)
            prcode(fp,
"#define sipType_%C              sipModuleAPI_%s.em_enumtypes[%d]\n"
"#define sipEnum_%C              sipModuleAPI_%s.em_enumtypes[%d]->u.td_py_type\n"
                , ed->fqcname, mod->name, ed->enumnr
                , ed->fqcname, mod->name, ed->enumnr);
        else
            prcode(fp,
"#define sipType_%C              sipModuleAPI_%s_%s->em_enumtypes[%d]\n"
"#define sipEnum_%C              sipModuleAPI_%s_%s->em_enumtypes[%d]->u.td_py_type\n"
                , ed->fqcname, mod->name, ed->module->name, ed->enumnr
                , ed->fqcname, mod->name, ed->module->name, ed->enumnr);
    }
}


/*
 * Generate the shadow class declaration.
 */
static void generateShadowClassDeclaration(sipSpec *pt,classDef *cd,FILE *fp)
{
    int noIntro, nrVirts;
    ctorDef *ct;
    virtOverDef *vod;
    classDef *pcd;

    prcode(fp,
"\n"
"\n"
"class sip%C : public %S\n"
"{\n"
"public:\n"
        ,classFQCName(cd),classFQCName(cd));

    /* Define a shadow class for any protected classes we have. */

    for (pcd = pt->classes; pcd != NULL; pcd = pcd->next)
    {
        mroDef *mro;

        if (!isProtectedClass(pcd))
            continue;

        /* See if the class defining the class is in our class hierachy. */
        for (mro = cd->mro; mro != NULL; mro = mro->next)
            if (mro->cd == pcd->ecd)
                break;

        if (mro == NULL)
            continue;

        prcode(fp,
"    class sip%s : public %s {\n"
"    public:\n"
                , classBaseName(pcd), classBaseName(pcd));

        generateProtectedEnums(pt, pcd, fp);

        prcode(fp,
"    };\n"
"\n"
                );
    }

    /* The constructor declarations. */

    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        ctorDef *dct;

        if (isPrivateCtor(ct))
            continue;

        if (ct->cppsig == NULL)
            continue;

        /* Check we haven't already handled this C++ signature. */
        for (dct = cd->ctors; dct != ct; dct = dct->next)
            if (dct->cppsig != NULL && sameSignature(dct->cppsig, ct->cppsig, TRUE))
                break;

        if (dct != ct)
            continue;

        prcode(fp,
"    sip%C(",classFQCName(cd));

        generateCalledArgs(cd, ct->cppsig, Declaration, TRUE, fp);

        prcode(fp,")%X;\n"
            ,ct->exceptions);
    }

    /* The destructor. */

    if (!isPrivateDtor(cd))
        prcode(fp,
"    %s~sip%C()%X;\n"
            ,(cd->vmembers != NULL ? "virtual " : ""),classFQCName(cd),cd->dtorexceptions);

    /* The metacall methods if required. */
    if (pluginPyQt4(pt) && isQObjectSubClass(cd))
    {
        prcode(fp,
"\n"
"    int qt_metacall(QMetaObject::Call,int,void **);\n"
"    void *qt_metacast(const char *);\n"
            );

        if (!noQMetaObject(cd))
            prcode(fp,
"    const QMetaObject *metaObject() const;\n"
                );
    }

    /* The exposure of protected enums. */

    generateProtectedEnums(pt,cd,fp);

    /* The wrapper around each protected member function. */

    generateProtectedDeclarations(cd,fp);

    /* The public wrapper around each signal emitter. */
    if (!optNoEmitters(pt))
    {
        visibleList *vl;

        noIntro = TRUE;

        for (vl = cd->visible; vl != NULL; vl = vl->next)
        {
            overDef *od;

            if (vl->m->slot != no_slot)
                continue;

            for (od = vl->cd->overs; od != NULL; od = od->next)
            {
                if (od->common != vl->m || !isSignal(od))
                    continue;

                if (noIntro)
                {
                    prcode(fp,
"\n"
"    /*\n"
"     * There is a public method for every Qt signal that can be emitted\n"
"     * by this object.  This function is called by Python to emit the\n"
"     * signal.\n"
"     */\n"
                        );

                    noIntro = FALSE;
                }

                prcode(fp,
"    int sipEmit_%s(PyObject *);\n"
                    ,vl->m->pyname->text);

                break;
            }
        }
    }

    /* The catcher around each virtual function in the hierarchy. */
    noIntro = TRUE;

    for (vod = cd->vmembers; vod != NULL; vod = vod->next)
    {
        overDef *od = &vod->o;
        virtOverDef *dvod;

        if (isPrivate(od))
            continue;

        /* Check we haven't already handled this C++ signature. */
        for (dvod = cd->vmembers; dvod != vod; dvod = dvod->next)
            if (strcmp(dvod->o.cppname,od->cppname) == 0 && sameSignature(dvod->o.cppsig,od->cppsig,TRUE))
                break;

        if (dvod != vod)
            continue;

        if (noIntro)
        {
            prcode(fp,
"\n"
"    /*\n"
"     * There is a protected method for every virtual method visible from\n"
"     * this class.\n"
"     */\n"
"protected:\n"
                );

            noIntro = FALSE;
        }

        prcode(fp,
"    ");
 
        prOverloadDecl(fp, cd, od, FALSE);
        prcode(fp, ";\n");
    }

    prcode(fp,
"\n"
"public:\n"
"    sipSimpleWrapper *sipPySelf;\n"
        );

    /* The private declarations. */

    prcode(fp,
"\n"
"private:\n"
"    sip%C(const sip%C &);\n"
"    sip%C &operator = (const sip%C &);\n"
        ,classFQCName(cd),classFQCName(cd)
        ,classFQCName(cd),classFQCName(cd));

    if ((nrVirts = countVirtuals(cd)) > 0)
        prcode(fp,
"\n"
"    sipMethodCache sipPyMethods[%d];\n"
            ,nrVirts);

    prcode(fp,
"};\n"
        );
}


/*
 * Generate the C++ declaration for an overload.
 */
void prOverloadDecl(FILE *fp, classDef *context, overDef *od, int defval)
{
    int a;

    normaliseArgs(od->cppsig);

    generateBaseType(context, &od->cppsig->result, TRUE, fp);
 
    prcode(fp, " %O(", od);

    for (a = 0; a < od->cppsig->nrArgs; ++a)
    {
        argDef *ad = &od->cppsig->args[a];

        if (a > 0)
            prcode(fp, ",");

        generateBaseType(context, ad, TRUE, fp);

        if (defval && ad->defval != NULL)
        {
            prcode(fp, " = ");
            generateExpression(ad->defval, fp);
        }
    }
 
    prcode(fp, ")%s%X", (isConst(od) ? " const" : ""), od->exceptions);

    restoreArgs(od->cppsig);
}


/*
 * Generate typed arguments for a declaration or a definition.
 */
static void generateCalledArgs(classDef *context, signatureDef *sd,
        funcArgType ftype, int use_typename, FILE *fp)
{
    char name[50];
    int a;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        if (a > 0)
            prcode(fp,",");

        if (ftype == Definition)
            sprintf(name, "a%d", a);
        else
            name[0] = '\0';

        generateNamedBaseType(context, ad, name, use_typename, fp);
    }
}


/*
 * Generate typed arguments for a call.
 */
static void generateCallArgs(classDef *context, signatureDef *sd,
        signatureDef *py_sd, FILE *fp)
{
    int a;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        char *ind = NULL;
        argDef *ad, *py_ad;

        if (a > 0)
            prcode(fp,",");

        ad = &sd->args[a];

        /* See if the argument needs dereferencing or it's address taking. */
        switch (ad->atype)
        {
        case sstring_type:
        case ustring_type:
        case string_type:
        case wstring_type:
            if (ad->nrderefs > (isOutArg(ad) ? 0 : 1))
                ind = "&";

            break;

        case mapped_type:
        case class_type:
            if (ad->nrderefs == 2)
                ind = "&";
            else if (ad->nrderefs == 0)
                ind = "*";

            break;

        case struct_type:
        case void_type:
            if (ad->nrderefs == 2)
                ind = "&";

            break;

        default:
            if (ad->nrderefs == 1)
                ind = "&";
        }

        if (ind != NULL)
            prcode(fp, ind);

        /*
         * See if we need to cast a Python void * to the correct C/C++ pointer
         * type.
         */
        if (py_sd != sd)
        {
            py_ad = &py_sd->args[a];

            if (py_ad->atype != void_type || ad->atype == void_type || py_ad->nrderefs != ad->nrderefs)
                py_ad = NULL;
        }
        else
            py_ad = NULL;

        if (py_ad == NULL)
        {
            if (isArraySize(ad))
                prcode(fp, "(%b)", ad);

            prcode(fp, "a%d", a);
        }
        else if (generating_c)
            prcode(fp, "(%b *)a%d", ad, a);
        else
            prcode(fp, "reinterpret_cast<%b *>(a%d)", ad, a);
    }
}


/*
 * Generate the declaration of a named variable to hold a result from a C++
 * function call.
 */
static void generateNamedValueType(classDef *context, argDef *ad, char *name,
        FILE *fp)
{
    argDef mod = *ad;

    if (ad->nrderefs == 0)
    {
        if (ad->atype == class_type || ad->atype == mapped_type)
            mod.nrderefs = 1;
        else
            resetIsConstArg(&mod);
    }

    resetIsReference(&mod);
    generateNamedBaseType(context, &mod, name, TRUE, fp);
}


/*
 * Generate a C++ type.
 */
static void generateBaseType(classDef *context, argDef *ad, int use_typename,
        FILE *fp)
{
    generateNamedBaseType(context, ad, "", use_typename, fp);
}


/*
 * Generate a C++ type and name.
 */
static void generateNamedBaseType(classDef *context, argDef *ad, char *name,
        int use_typename, FILE *fp)
{
    typedefDef *td = ad->original_type;
    int nr_derefs = ad->nrderefs;
    int is_reference = isReference(ad);

    if (use_typename && td != NULL && !noTypeName(td))
    {
        if (isConstArg(ad) && !isConstArg(&td->type))
            prcode(fp, "const ");

        nr_derefs -= td->type.nrderefs;

        if (isReference(&td->type))
            is_reference = FALSE;

        prcode(fp, "%S", td->fqname);
    }
    else
    {
        /*
         * A function type is handled differently because of the position of
         * the name.
         */
        if (ad->atype == function_type)
        {
            int i;
            signatureDef *sig = ad->u.sa;

            generateBaseType(context, &sig->result, TRUE, fp);

            prcode(fp," (");

            for (i = 0; i < nr_derefs; ++i)
                prcode(fp, "*");

            prcode(fp, "%s)(",name);
            generateCalledArgs(context, sig, Declaration, use_typename, fp);
            prcode(fp, ")");

            return;
        }

        if (isConstArg(ad))
            prcode(fp, "const ");

        switch (ad->atype)
        {
        case sstring_type:
            prcode(fp, "signed char");
            break;

        case ustring_type:
            prcode(fp, "unsigned char");
            break;

        case wstring_type:
            prcode(fp, "wchar_t");
            break;

        case signal_type:
        case slot_type:
        case anyslot_type:
        case slotcon_type:
        case slotdis_type:
            nr_derefs = 1;

            /* Drop through. */

        case string_type:
            prcode(fp, "char");
            break;

        case ushort_type:
            prcode(fp, "unsigned short");
            break;

        case short_type:
            prcode(fp, "short");
            break;

        case uint_type:
            prcode(fp, "unsigned");
            break;

        case int_type:
        case cint_type:
            prcode(fp, "int");
            break;

        case ssize_type:
            prcode(fp, "SIP_SSIZE_T");
            break;

        case ulong_type:
            prcode(fp, "unsigned long");
            break;

        case long_type:
            prcode(fp, "long");
            break;

        case ulonglong_type:
            prcode(fp, "unsigned PY_LONG_LONG");
            break;

        case longlong_type:
            prcode(fp, "PY_LONG_LONG");
            break;

        case struct_type:
            prcode(fp, "struct %S", ad->u.sname);
            break;

        case fake_void_type:
        case void_type:
            prcode(fp, "void");
            break;

        case bool_type:
        case cbool_type:
            prcode(fp, "bool");
            break;

        case float_type:
        case cfloat_type:
            prcode(fp, "float");
            break;

        case double_type:
        case cdouble_type:
            prcode(fp, "double");
            break;

        case defined_type:
            /*
             * The only defined types still remaining are arguments to
             * templates.
             */
            prcode(fp, "%S", ad->u.snd);
            break;

        case rxcon_type:
        case rxdis_type:
            nr_derefs = 1;
            prcode(fp, "QObject");
            break;

        case mapped_type:
            generateBaseType(context, &ad->u.mtd->type, TRUE, fp);
            break;

        case class_type:
            prcode(fp, "%V", context, ad->u.cd);
            break;

        case template_type:
            {
                static const char tail[] = ">";
                int a;
                templateDef *td = ad->u.td;

                prcode(fp, "%S%s", td->fqname, (prcode_xml ? "&lt;" : "<"));

                for (a = 0; a < td->types.nrArgs; ++a)
                {
                    if (a > 0)
                        prcode(fp, ",");

                    generateBaseType(context, &td->types.args[a], TRUE, fp);
                }

                if (prcode_last == tail)
                    prcode(fp, " ");

                prcode(fp, (prcode_xml ? "&gt;" : tail));
                break;
            }

        case enum_type:
            prcode(fp, "%E", ad->u.ed);
            break;

        case pyobject_type:
        case pytuple_type:
        case pylist_type:
        case pydict_type:
        case pycallable_type:
        case pyslice_type:
        case pytype_type:
        case qobject_type:
        case ellipsis_type:
            prcode(fp, "PyObject *");
            break;
        }
    }

    if (nr_derefs > 0)
    {
        int i;

        prcode(fp, " ");

        for (i = 0; i < nr_derefs; ++i)
            prcode(fp, "*");
    }

    if (is_reference)
        prcode(fp, (prcode_xml ? "&amp;" : "&"));

    if (*name != '\0')
    {
        if (nr_derefs == 0)
            prcode(fp, " ");

        prcode(fp, name);
    }
}


/*
 * Generate the definition of an argument variable and any supporting
 * variables.
 */
static void generateVariable(classDef *context, argDef *ad, int argnr,
        FILE *fp)
{
    argType atype = ad->atype;
    argDef orig;

    if (isInArg(ad) && ad->defval != NULL &&
        (atype == class_type || atype == mapped_type) &&
        (ad->nrderefs == 0 || isReference(ad)))
    {
        /*
         * Generate something to hold the default value as it cannot be
         * assigned straight away.
         */
        prcode(fp,
"        %A a%ddef = ", context, ad, argnr);

        generateExpression(ad->defval,fp);

        prcode(fp,";\n"
            );
    }

    /* Adjust the type so we have the type that will really handle it. */

    orig = *ad;

    switch (atype)
    {
    case sstring_type:
    case ustring_type:
    case string_type:
    case wstring_type:
        if (!isReference(ad))
        {
            if (ad->nrderefs == 2)
                ad->nrderefs = 1;
            else if (ad->nrderefs == 1 && isOutArg(ad))
                ad->nrderefs = 0;
        }

        break;

    case mapped_type:
    case class_type:
    case void_type:
    case struct_type:
        ad->nrderefs = 1;
        break;

    default:
        ad->nrderefs = 0;
    }

    /* Array sizes are always SIP_SSIZE_T. */
    if (isArraySize(ad))
        ad->atype = ssize_type;

    resetIsReference(ad);

    if (ad->nrderefs == 0)
        resetIsConstArg(ad);

    prcode(fp,
"        %A a%d", context, ad, argnr);

    if (atype == anyslot_type)
        prcode(fp, "Name");

    *ad = orig;

    generateDefaultValue(ad, argnr, fp);

    prcode(fp,";\n"
        );

    /* Some types have supporting variables. */
    if (isInArg(ad))
        switch (atype)
        {
        case class_type:
            if (ad->u.cd->convtocode != NULL && !isConstrained(ad))
                prcode(fp,
"        int a%dState = 0;\n"
                    ,argnr);

            if (isGetWrapper(ad))
                prcode(fp,
"        PyObject *a%dWrapper;\n"
                    ,argnr);

            break;

        case mapped_type:
            if (!noRelease(ad->u.mtd))
                prcode(fp,
"        int a%dState = 0;\n"
                    ,argnr);

            break;

        case anyslot_type:
            prcode(fp,
"        PyObject *a%dCallable", argnr);
            generateDefaultValue(ad, argnr, fp);
            prcode(fp, ";\n"
                );
            break;
        }
}


/*
 * Generate a default value.
 */
static void generateDefaultValue(argDef *ad, int argnr, FILE *fp)
{
    if (isInArg(ad) && ad->defval != NULL)
    {
        prcode(fp," = ");

        if ((ad->atype == class_type || ad->atype == mapped_type) &&
                (ad->nrderefs == 0 || isReference(ad)))
            prcode(fp, "&a%ddef", argnr);
        else
            generateExpression(ad->defval,fp);
    }
}


/*
 * Generate a simple function call.
 */
static void generateSimpleFunctionCall(fcallDef *fcd,FILE *fp)
{
    int i;

    prcode(fp, "%B(", &fcd->type);

    for (i = 0; i < fcd->nrArgs; ++i)
    {
        if (i > 0)
            prcode(fp,",");

        generateExpression(fcd->args[i],fp);
    }

    prcode(fp,")");
}


/*
 * Generate the type structure that contains all the information needed by the
 * meta-type.  A sub-set of this is used to extend namespaces.
 */
static void generateTypeDefinition(sipSpec *pt, classDef *cd, FILE *fp)
{
    const char *mname, *sep, *type_prefix;
    int is_slots, nr_methods, nr_enums, embedded;
    int is_inst_class, is_inst_voidp, is_inst_char, is_inst_string;
    int is_inst_int, is_inst_long, is_inst_ulong, is_inst_longlong;
    int is_inst_ulonglong, is_inst_double;
    memberDef *md;
    moduleDef *mod;

    mod = cd->iff->module;
    mname = mod->name;

    if (cd->supers != NULL)
    {
        classList *cl;

        prcode(fp,
"\n"
"\n"
"/* Define this type's super-types. */\n"
"static sipEncodedClassDef supers_%C[] = {", classFQCName(cd));

        for (cl = cd->supers; cl != NULL; cl = cl->next)
        {
            if (cl != cd->supers)
                prcode(fp, ", ");

            generateEncodedClass(mod, cl->cd, (cl->next == NULL), fp);
        }

        prcode(fp,"};\n"
            );
    }

    /* Generate the slots table. */
    is_slots = FALSE;

    for (md = cd->members; md != NULL; md = md->next)
    {
        const char *stype;

        if (md->slot == no_slot)
            continue;

        if (!is_slots)
        {
            prcode(fp,
"\n"
"\n"
"/* Define this type's Python slots. */\n"
"static sipPySlotDef slots_%C[] = {\n"
                , classFQCName(cd));

            is_slots = TRUE;
        }

        if ((stype = slotName(md->slot)) != NULL)
            prcode(fp,
"    {(void *)slot_%C_%s, %s},\n"
                , classFQCName(cd), md->pyname->text, stype);
    }

    if (is_slots)
        prcode(fp,
"    {0, (sipPySlotType)0}\n"
"};\n"
            );

    /* Generate the attributes tables. */
    nr_methods = generateMethodTable(cd,fp);
    nr_enums = generateEnumMemberTable(pt, mod, cd, fp);

    /* Generate each instance table. */
    is_inst_class = generateClasses(pt, mod, cd, fp);
    is_inst_voidp = generateVoidPointers(pt, mod, cd, fp);
    is_inst_char = generateChars(pt, mod, cd, fp);
    is_inst_string = generateStrings(pt, mod, cd, fp);
    is_inst_int = generateInts(pt, mod, cd, fp);
    is_inst_long = generateLongs(pt, mod, cd, fp);
    is_inst_ulong = generateUnsignedLongs(pt, mod, cd, fp);
    is_inst_longlong = generateLongLongs(pt, mod, cd, fp);
    is_inst_ulonglong = generateUnsignedLongLongs(pt, mod, cd, fp);
    is_inst_double = generateDoubles(pt, mod, cd, fp);

    embedded = TRUE;

    if (pluginPyQt4(pt))
        type_prefix = "pyqt4";
    else
    {
        type_prefix = "sip";
        embedded = FALSE;
    }

    prcode(fp,
"\n"
"\n"
"%sClassTypeDef %sType_%s_%C = {\n"
"%s"
"    {\n"
"        -1,\n"
"        0,\n"
"        0,\n"
"        "
        , type_prefix, type_prefix, mname, classFQCName(cd)
        , (embedded ? "{\n" : ""));

    sep = "";

    if (cd->userflags)
    {
        prcode(fp, "%s%x", sep, ((cd->userflags << TYPE_FLAGS_SHIFT) & TYPE_FLAGS_MASK));
        sep = "|";
    }

    if (isAbstractClass(cd))
    {
        prcode(fp, "%sSIP_TYPE_ABSTRACT", sep);
        sep = "|";
    }

    if (cd->subbase != NULL)
    {
        prcode(fp, "%sSIP_TYPE_SCC", sep);
        sep = "|";
    }

    if (cd->iff->type == namespace_iface)
    {
        prcode(fp, "%sSIP_TYPE_NAMESPACE", sep);
        sep = "|";
    }
    else
    {
        prcode(fp, "%sSIP_TYPE_CLASS", sep);
        sep = "|";
    }

    if (*sep == '\0')
        prcode(fp, "0");

    prcode(fp, ",\n");

    prcode(fp,
"        %n,\n"
"        {0}\n"
"    },\n"
        , cd->iff->name);

    if (cd->real == NULL)
        prcode(fp,
"    %n,\n"
            , cd->pyname);
    else
        prcode(fp,
"    -1,\n"
            );

    if (cd->metatype != NULL)
        prcode(fp,
"    %n,\n"
            , cd->metatype);
    else
        prcode(fp,
"    -1,\n"
            );

    if (cd->supertype != NULL)
        prcode(fp,
"    %n,\n"
            , cd->supertype);
    else
        prcode(fp,
"    -1,\n"
            );

    prcode(fp,
"    ");

    if (cd->real != NULL)
        generateEncodedClass(mod, cd->real, 0, fp);
    else if (cd->ecd != NULL)
        generateEncodedClass(mod, cd->ecd, 0, fp);
    else
        prcode(fp, "{0, 0, 1}");

    prcode(fp, ",\n"
        );

    if (cd->supers != NULL)
        prcode(fp,
"    supers_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (is_slots)
        prcode(fp,
"    slots_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (nr_methods == 0)
        prcode(fp,
"    0, 0,\n"
            );
    else
        prcode(fp,
"    %d, methods_%C,\n"
            , nr_methods, classFQCName(cd));

    if (nr_enums == 0)
        prcode(fp,
"    0, 0,\n"
            );
    else
        prcode(fp,
"    %d, enummembers_%C,\n"
            , nr_enums, classFQCName(cd));

    if (hasVarHandlers(cd))
        prcode(fp,
"    variables_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (canCreate(cd))
        prcode(fp,
"    init_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->travcode != NULL)
        prcode(fp,
"    traverse_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->clearcode != NULL)
        prcode(fp,
"    clear_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->readbufcode != NULL)
        prcode(fp,
"    getreadbuffer_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->writebufcode != NULL)
        prcode(fp,
"    getwritebuffer_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->segcountcode != NULL)
        prcode(fp,
"    getsegcount_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->charbufcode != NULL)
        prcode(fp,
"    getcharbuffer_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (needDealloc(cd))
        prcode(fp,
"    dealloc_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (optAssignmentHelpers(pt) && (generating_c || canAssign(cd)))
        prcode(fp,
"    assign_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (cd->iff->type == namespace_iface || generating_c)
        prcode(fp,
"    0,\n"
"    0,\n"
            );
    else
        prcode(fp,
"    release_%C,\n"
"    cast_%C,\n"
            , classFQCName(cd)
            , classFQCName(cd));

    if (cd->iff->type == namespace_iface)
        prcode(fp,
"    0,\n"
            );
    else
    {
        if (cd->convtocode != NULL)
            prcode(fp,
"    convertTo_%C,\n"
                , classFQCName(cd));
        else
            prcode(fp,
"    0,\n"
                );
    }

    prcode(fp,
"    {");

    if (is_inst_class)
        prcode(fp, "typeInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_voidp)
        prcode(fp, "voidPtrInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_char)
        prcode(fp, "charInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_string)
        prcode(fp, "stringInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_int)
        prcode(fp, "intInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_long)
        prcode(fp, "longInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_ulong)
        prcode(fp, "unsignedLongInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_longlong)
        prcode(fp, "longLongInstances_%C, ", classFQCName(cd));
    else
        prcode(fp,"0, ");

    if (is_inst_ulonglong)
        prcode(fp, "unsignedLongLongInstances_%C, ", classFQCName(cd));
    else
        prcode(fp, "0, ");

    if (is_inst_double)
        prcode(fp, "doubleInstances_%C", classFQCName(cd));
    else
        prcode(fp, "0");

    prcode(fp,"},\n"
"    0,\n"
        );

    if (cd->picklecode != NULL)
        prcode(fp,
"    pickle_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0,\n"
            );

    if (!optNoEmitters(pt) && hasSigSlots(cd))
        prcode(fp,
"    signals_%C,\n"
            , classFQCName(cd));
    else
        prcode(fp,
"    0\n"
            );

    if (embedded)
        prcode(fp,
"},\n"
            );

    if (pluginPyQt4(pt))
    {
        if (isQObjectSubClass(cd) && !noQMetaObject(cd))
            prcode(fp,
"    &%U::staticMetaObject\n"
                , cd);
        else
            prcode(fp,
"    0\n"
                );
    }

    prcode(fp,
"};\n"
        );
}


/*
 * Return the sip module's string equivalent of a slot.
 */
static const char *slotName(slotType st)
{
    const char *sn;

    switch (st)
    {
    case str_slot:
        sn = "str_slot";
        break;

    case int_slot:
        sn = "int_slot";
        break;

    case long_slot:
        sn = "long_slot";
        break;

    case float_slot:
        sn = "float_slot";
        break;

    case len_slot:
        sn = "len_slot";
        break;

    case contains_slot:
        sn = "contains_slot";
        break;

    case add_slot:
        sn = "add_slot";
        break;

    case concat_slot:
        sn = "concat_slot";
        break;

    case sub_slot:
        sn = "sub_slot";
        break;

    case mul_slot:
        sn = "mul_slot";
        break;

    case repeat_slot:
        sn = "repeat_slot";
        break;

    case div_slot:
        sn = "div_slot";
        break;

    case mod_slot:
        sn = "mod_slot";
        break;

    case and_slot:
        sn = "and_slot";
        break;

    case or_slot:
        sn = "or_slot";
        break;

    case xor_slot:
        sn = "xor_slot";
        break;

    case lshift_slot:
        sn = "lshift_slot";
        break;

    case rshift_slot:
        sn = "rshift_slot";
        break;

    case iadd_slot:
        sn = "iadd_slot";
        break;

    case iconcat_slot:
        sn = "iconcat_slot";
        break;

    case isub_slot:
        sn = "isub_slot";
        break;

    case imul_slot:
        sn = "imul_slot";
        break;

    case irepeat_slot:
        sn = "irepeat_slot";
        break;

    case idiv_slot:
        sn = "idiv_slot";
        break;

    case imod_slot:
        sn = "imod_slot";
        break;

    case iand_slot:
        sn = "iand_slot";
        break;

    case ior_slot:
        sn = "ior_slot";
        break;

    case ixor_slot:
        sn = "ixor_slot";
        break;

    case ilshift_slot:
        sn = "ilshift_slot";
        break;

    case irshift_slot:
        sn = "irshift_slot";
        break;

    case invert_slot:
        sn = "invert_slot";
        break;

    case call_slot:
        sn = "call_slot";
        break;

    case getitem_slot:
        sn = "getitem_slot";
        break;

    case setitem_slot:
        sn = "setitem_slot";
        break;

    case delitem_slot:
        sn = "delitem_slot";
        break;

    case lt_slot:
        sn = "lt_slot";
        break;

    case le_slot:
        sn = "le_slot";
        break;

    case eq_slot:
        sn = "eq_slot";
        break;

    case ne_slot:
        sn = "ne_slot";
        break;

    case gt_slot:
        sn = "gt_slot";
        break;

    case ge_slot:
        sn = "ge_slot";
        break;

    case cmp_slot:
        sn = "cmp_slot";
        break;

    case nonzero_slot:
        sn = "nonzero_slot";
        break;

    case neg_slot:
        sn = "neg_slot";
        break;

    case pos_slot:
        sn = "pos_slot";
        break;

    case abs_slot:
        sn = "abs_slot";
        break;

    case repr_slot:
        sn = "repr_slot";
        break;

    case hash_slot:
        sn = "hash_slot";
        break;

    default:
        sn = NULL;
    }

    return sn;
}


/*
 * Generate the initialisation function or cast operators for the type.
 */
static void generateTypeInit(classDef *cd, FILE *fp)
{
    ctorDef *ct;
    int need_self, need_owner;

    /*
     * See if we need to name the self and owner arguments so that we can
     * avoid a compiler warning about an unused argument.
     */
    need_self = (generating_c || hasShadow(cd));
    need_owner = generating_c;

    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        if (usedInCode(ct->methodcode, "sipSelf"))
            need_self = TRUE;

        if (isResultTransferredCtor(ct))
            need_owner = TRUE;
        else
        {
            int a;

            for (a = 0; a < ct->pysig.nrArgs; ++a)
                if (isThisTransferred(&ct->pysig.args[a]))
                {
                    need_owner = TRUE;
                    break;
                }
        }
    }

    prcode(fp,
"\n"
"\n"
        );

    if (!generating_c)
        prcode(fp,
"extern \"C\" {static void *init_%C(sipSimpleWrapper *, PyObject *, sipWrapper **, int *);}\n"
            , classFQCName(cd));

    prcode(fp,
"static void *init_%C(sipSimpleWrapper *%s,PyObject *sipArgs,sipWrapper **%s,int *sipArgsParsed)\n"
"{\n"
        ,classFQCName(cd),(need_self ? "sipSelf" : ""),(need_owner ? "sipOwner" : ""));

    if (hasShadow(cd))
        prcode(fp,
"    sip%C *sipCpp = 0;\n"
            ,classFQCName(cd));
    else
        prcode(fp,
"    %U *sipCpp = 0;\n"
            ,cd);

    if (tracing)
        prcode(fp,
"\n"
"    sipTrace(SIP_TRACE_INITS,\"init_%C()\\n\");\n"
            ,classFQCName(cd));

    /*
     * Generate the code that parses the Python arguments and calls the
     * correct constructor.
     */
    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        int needSecCall, error_flag = FALSE;

        if (isPrivateCtor(ct))
            continue;

        prcode(fp,
"\n"
"    if (!sipCpp)\n"
"    {\n"
            );

        if (ct->methodcode != NULL && needErrorFlag(ct->methodcode))
        {
            prcode(fp,
"        int sipIsErr = 0;\n"
                );

            error_flag = TRUE;
        }

        needSecCall = generateArgParser(&ct->pysig, cd, ct, NULL, FALSE, fp);
        generateConstructorCall(cd,ct,error_flag,fp);

        if (needSecCall)
        {
                prcode(fp,
"    }\n"
"\n"
"    if (!sipCpp)\n"
"    {\n"
                );

            if (error_flag)
                prcode(fp,
"        int sipIsErr = 0;\n"
                    );

            generateArgParser(&ct->pysig, cd, ct, NULL, TRUE, fp);
            generateConstructorCall(cd,ct,error_flag,fp);
        }

        prcode(fp,
"    }\n"
            );
    }

    if (hasShadow(cd))
        prcode(fp,
"\n"
"    if (sipCpp)\n"
"        sipCpp->sipPySelf = sipSelf;\n"
            );

    prcode(fp,
"\n"
"    return sipCpp;\n"
"}\n"
        );
}


/*
 * Count the number of virtual members in a class.
 */
static int countVirtuals(classDef *cd)
{
    int nrvirts;
    virtOverDef *vod;
 
    nrvirts = 0;
 
    for (vod = cd->vmembers; vod != NULL; vod = vod->next)
        if (!isPrivate(&vod->o))
            ++nrvirts;
 
    return nrvirts;
}

 
/*
 * Generate the try block for a call.
 */
static void generateTry(throwArgs *ta,FILE *fp)
{
    /*
     * Generate the block if there was no throw specifier, or a non-empty
     * throw specifier.
     */
    if (exceptions && (ta == NULL || ta->nrArgs > 0))
        prcode(fp,
"            try\n"
"            {\n"
            );
}


/*
 * Generate the catch block for a call.
 */
static void generateCatch(throwArgs *ta, signatureDef *sd, FILE *fp)
{
    /*
     * Generate the block if there was no throw specifier, or a non-empty
     * throw specifier.
     */
    if (exceptions && (ta == NULL || ta->nrArgs > 0))
    {
        prcode(fp,
"            }\n"
            );

        if (ta == NULL)
        {
            prcode(fp,
"            catch (...)\n"
"            {\n"
                );

            if (release_gil)
                prcode(fp,
"                Py_BLOCK_THREADS\n"
"\n"
                    );

            deleteTemps(sd, fp);

            prcode(fp,
"                sipRaiseUnknownException();\n"
"                return NULL;\n"
"            }\n"
                );
        }
        else
        {
            int a;

            for (a = 0; a < ta->nrArgs; ++a)
            {
                exceptionDef *xd = ta->args[a];
                scopedNameDef *ename = xd->iff->fqcname;

                prcode(fp,
"            catch (%S &%s)\n"
"            {\n"
                    ,ename,(xd->cd != NULL || usedInCode(xd->raisecode, "sipExceptionRef")) ? "sipExceptionRef" : "");

                if (release_gil)
                    prcode(fp,
"\n"
"                Py_BLOCK_THREADS\n"
                        );

                deleteTemps(sd, fp);

                /* See if the exception is a wrapped class. */
                if (xd->cd != NULL)
                    prcode(fp,
"                /* Hope that there is a valid copy ctor. */\n"
"                %S *sipExceptionCopy = new %S(sipExceptionRef);\n"
"\n"
"                sipRaiseTypeException(sipType_%C,sipExceptionCopy);\n"
                        , ename, ename
                        , ename);
                else
                    generateCppCodeBlock(xd->raisecode,fp);

                prcode(fp,
"\n"
"                return NULL;\n"
"            }\n"
                    );
            }
        }
    }
}


/*
 * Generate a throw specifier.
 */
static void generateThrowSpecifier(throwArgs *ta,FILE *fp)
{
    if (exceptions && ta != NULL)
    {
        int a;

        prcode(fp," throw(");

        for (a = 0; a < ta->nrArgs; ++a)
        {
            if (a > 0)
                prcode(fp,",");

            prcode(fp,"%S",ta->args[a]->iff->fqcname);
        }

        prcode(fp,")");
    }
}


/*
 * Generate a single constructor call.
 */
static void generateConstructorCall(classDef *cd,ctorDef *ct,int error_flag,
                                    FILE *fp)
{
    prcode(fp,
"        {\n"
        );

    if (isDeprecatedCtor(ct))
        /* Note that any temporaries will leak if an exception is raised. */
        prcode(fp,
"            if (sipDeprecated(%N,NULL) < 0)\n"
"                return NULL;\n"
"\n"
            , cd->pyname);

    /* Call any pre-hook. */
    if (ct->prehook != NULL)
        prcode(fp,
"            sipCallHook(\"%s\");\n"
"\n"
            ,ct->prehook);

    if (ct->methodcode != NULL)
        generateCppCodeBlock(ct->methodcode,fp);
    else if (generating_c)
        prcode(fp,
"            sipCpp = sipMalloc(sizeof (%S));\n"
            ,classFQCName(cd));
    else
    {
        int rgil = ((release_gil || isReleaseGILCtor(ct)) && !isHoldGILCtor(ct));

        if (rgil)
            prcode(fp,
"            Py_BEGIN_ALLOW_THREADS\n"
                );

        generateTry(ct->exceptions,fp);

        if (hasShadow(cd))
            prcode(fp,
"            sipCpp = new sip%C(",classFQCName(cd));
        else
            prcode(fp,
"            sipCpp = new %U(",cd);

        if (isCastCtor(ct))
        {
            classDef *ocd;

            /* We have to fiddle the type to generate the correct code. */
            ocd = ct->pysig.args[0].u.cd;
            ct->pysig.args[0].u.cd = cd;
            prcode(fp, "a0->operator %B()", &ct->pysig.args[0]);
            ct->pysig.args[0].u.cd = ocd;
        }
        else
            generateCallArgs(cd, ct->cppsig, &ct->pysig, fp);

        prcode(fp,");\n"
            );

        generateCatch(ct->exceptions, &ct->pysig, fp);

        if (rgil)
            prcode(fp,
"            Py_END_ALLOW_THREADS\n"
                );

        /*
         * This is a bit of a hack to say we want the result transferred.  We
         * don't simply call sipTransferTo() because the wrapper object hasn't
         * been fully initialised yet.
         */
        if (isResultTransferredCtor(ct))
            prcode(fp,
"\n"
"            *sipOwner = (sipWrapper *)Py_None;\n"
                );
    }

    gc_ellipsis(&ct->pysig, fp);

    deleteTemps(&ct->pysig, fp);

    if (error_flag)
        prcode(fp,
"\n"
"            if (sipIsErr)\n"
"                return 0;\n"
            );

    /* Call any post-hook. */
    if (ct->posthook != NULL)
        prcode(fp,
"\n"
"            sipCallHook(\"%s\");\n"
            ,ct->posthook);

    prcode(fp,
"        }\n"
        );
}


/*
 * See if a member overload should be skipped.
 */
static int skipOverload(overDef *od,memberDef *md,classDef *cd,classDef *ccd,
            int want_local)
{
    /* Skip if it's not the right name. */
    if (od->common != md)
        return TRUE;

    /* Skip if it's a signal. */
    if (isSignal(od))
        return TRUE;

    /* Skip if it's a private abstract. */
    if (isAbstract(od) && isPrivate(od))
        return TRUE;

    /*
     * If we are disallowing them, skip if it's not in the current class
     * unless it is protected.
     */
    if (want_local && !isProtected(od) && ccd != cd)
        return TRUE;

    return FALSE;
}


/*
 * Generate a class member function.
 */
static void generateFunction(memberDef *md, overDef *overs, classDef *cd,
        classDef *ocd, FILE *fp)
{
    overDef *od;
    int need_method, need_self, need_args, need_selfarg;

    /*
     * Check that there is at least one overload that needs to be handled.
     * See if we can avoid naming the "self" argument (and suppress a
     * compiler warning).  Finally see if we need to remember if "self" was
     * explicitly passed as an argument.
     */
    need_method = need_self = need_args = need_selfarg = FALSE;

    for (od = overs; od != NULL; od = od->next)
    {
        /*
         * Skip protected methods if we don't have the means to handle
         * them.
         */
        if (isProtected(od) && !hasShadow(cd))
            continue;

        if (!skipOverload(od,md,cd,ocd,TRUE))
        {
            need_method = TRUE;

            if (!isPrivate(od))
            {
                need_args = TRUE;

                if (!isStatic(od))
                {
                    need_self = TRUE;

                    if (isAbstract(od) || isVirtual(od) || isVirtualReimp(od) || usedInCode(od->methodcode, "sipSelfWasArg"))
                        need_selfarg = TRUE;
                }
            }
        }
    }

    if (need_method)
    {
        const char *pname = md->pyname->text;

        prcode(fp,
"\n"
"\n"
            );

        if (!generating_c)
            prcode(fp,
"extern \"C\" {static PyObject *meth_%C_%s(PyObject *, PyObject *);}\n"
            , classFQCName(cd), pname);

        prcode(fp,
"static PyObject *meth_%C_%s(PyObject *%s,PyObject *%s)\n"
"{\n"
            ,classFQCName(cd),pname,(need_self ? "sipSelf" : ""),(need_args ? "sipArgs" : ""));

        if (tracing)
            prcode(fp,
"    sipTrace(SIP_TRACE_METHODS,\"meth_%C_%s()\\n\");\n"
"\n"
                ,classFQCName(cd),pname);

        if (need_args)
            prcode(fp,
"    int sipArgsParsed = 0;\n"
                );

        if (need_selfarg)
            prcode(fp,
"    bool sipSelfWasArg = !sipSelf;\n"
                );

        for (od = overs; od != NULL; od = od->next)
        {
            /*
             * If we are handling one variant then we must handle
             * them all.
             */
            if (skipOverload(od,md,cd,ocd,FALSE))
                continue;

            if (isPrivate(od))
                continue;

            generateFunctionBody(od, cd, ocd, TRUE, fp);
        }

        prcode(fp,
"\n"
"    /* Raise an exception if the arguments couldn't be parsed. */\n"
"    sipNoMethod(%s,%N,%N);\n"
"\n"
"    return NULL;\n"
"}\n"
            ,(need_args ? "sipArgsParsed" : "0"),cd->pyname,md->pyname);
    }
}


/*
 * Generate the function calls for a particular overload.
 */
static void generateFunctionBody(overDef *od, classDef *cd, classDef *ocd,
        int deref, FILE *fp)
{
    int needSecCall;
    signatureDef saved;

    prcode(fp,
"\n"
"    {\n"
        );

    /* In case we have to fiddle with it. */
    saved = od->pysig;

    if (isNumberSlot(od->common))
    {
        /*
         * Number slots must have two arguments because we parse them slightly
         * differently.
         */
        if (od->pysig.nrArgs == 1)
        {
            od->pysig.nrArgs = 2;
            od->pysig.args[1] = od->pysig.args[0];

            /* Insert self in the right place. */
            od->pysig.args[0].atype = class_type;
            od->pysig.args[0].name = NULL;
            od->pysig.args[0].argflags = ARG_IS_REF|ARG_IN;
            od->pysig.args[0].nrderefs = 0;
            od->pysig.args[0].defval = NULL;
            od->pysig.args[0].original_type = NULL;
            od->pysig.args[0].u.cd = ocd;
        }

        generateArgParser(&od->pysig, cd, NULL, od, FALSE, fp);
        needSecCall = FALSE;
    }
    else if (isIntArgSlot(od->common) || isZeroArgSlot(od->common))
        needSecCall = FALSE;
    else
        needSecCall = generateArgParser(&od->pysig, cd, NULL, od, FALSE, fp);

    generateFunctionCall(cd,ocd,od,deref,fp);

    if (needSecCall)
    {
        prcode(fp,
"    }\n"
"\n"
"    {\n"
            );

        generateArgParser(&od->pysig, cd, NULL, od, TRUE, fp);
        generateFunctionCall(cd,ocd,od,deref,fp);
    }

    prcode(fp,
"    }\n"
        );

    od->pysig = saved;
}


/*
 * Generate the code to handle the result of a call to a member function.
 */
static void generateHandleResult(overDef *od, int isNew, int result_size,
        char *prefix, FILE *fp)
{
    char *vname, vnamebuf[50];
    int a, nrvals, only, has_owner;
    argDef *res, *ad;

    res = &od->pysig.result;

    if (res->atype == void_type && res->nrderefs == 0)
        res = NULL;

    /* See if we are returning 0, 1 or more values. */
    nrvals = 0;

    if (res != NULL)
    {
        only = -1;
        ++nrvals;
    }

    has_owner = FALSE;

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        if (isOutArg(&od->pysig.args[a]))
        {
            only = a;
            ++nrvals;
        }

        if (isThisTransferred(&od->pysig.args[a]))
            has_owner = TRUE;
    }

    /* Handle the trivial case. */
    if (nrvals == 0)
    {
        prcode(fp,
"            Py_INCREF(Py_None);\n"
"            %s Py_None;\n"
            ,prefix);

        return;
    }

    /* Handle results that are classes or mapped types separately. */
    if (res != NULL)
    {
        if (res->atype == mapped_type)
        {
            prcode(fp,
"            PyObject *sipResObj = sipConvertFromType(");

            if (isConstArg(res))
                prcode(fp,"const_cast<%b *>(sipRes)",res);
            else
                prcode(fp,"sipRes");

            prcode(fp,",sipType_%T,%s);\n"
                , res, resultOwner(od));

            if (isNew)
                prcode(fp,
"            delete sipRes;\n"
                    );

            /* Shortcut if this is the only value returned. */
            if (nrvals == 1)
            {
                prcode(fp,
"\n"
"            %s sipResObj;\n"
                    ,prefix);

                return;
            }
        }
        else if (res->atype == class_type)
        {
            classDef *cd = res->u.cd;

            if (isNew || isFactory(od))
            {
                prcode(fp,
"            %s sipConvertFromNewType(",(nrvals == 1 ? prefix : "PyObject *sipResObj ="));

                if (isConstArg(res))
                    prcode(fp,"const_cast<%b *>(sipRes)",res);
                else
                    prcode(fp,"sipRes");

                prcode(fp,",sipType_%C,%s);\n"
                    ,classFQCName(cd),((has_owner && isFactory(od)) ? "(PyObject *)sipOwner" : "NULL"));

                /*
                 * Shortcut if this is the only value returned.
                 */
                if (nrvals == 1)
                    return;
            }
            else
            {
                prcode(fp,
"            %s sipConvertFromType(",(nrvals == 1 ? prefix : "PyObject *sipResObj ="));

                if (isConstArg(res))
                    prcode(fp,"const_cast<%b *>(sipRes)",res);
                else
                    prcode(fp,"sipRes");

                prcode(fp, ",sipType_%C,%s);\n"
                    , classFQCName(cd), resultOwner(od));

                /*
                 * Shortcut if this is the only value returned.
                 */
                if (nrvals == 1)
                    return;
            }
        }
    }

    /* If there are multiple values then build a tuple. */
    if (nrvals > 1)
    {
        prcode(fp,
"            %s sipBuildResult(0,\"(",prefix);

        /* Build the format string. */
        if (res != NULL)
            prcode(fp,"%c",((res->atype == mapped_type || res->atype == class_type) ? 'R' : getBuildResultFormat(res)));

        for (a = 0; a < od->pysig.nrArgs; ++a)
        {
            argDef *ad = &od->pysig.args[a];

            if (isOutArg(ad))
                prcode(fp,"%c",getBuildResultFormat(ad));
        }

        prcode(fp,")\"");

        /* Pass the values for conversion. */
        if (res != NULL)
        {
            prcode(fp, ",sipRes");

            if (res->atype == mapped_type || res->atype == class_type)
                prcode(fp, "Obj");
            else if (res->atype == enum_type && res->u.ed->fqcname != NULL)
                prcode(fp, ",sipType_%C", res->u.ed->fqcname);
        }

        for (a = 0; a < od->pysig.nrArgs; ++a)
        {
            argDef *ad = &od->pysig.args[a];

            if (isOutArg(ad))
            {
                prcode(fp, ",a%d", a);

                if (ad->atype == mapped_type)
                    prcode(fp, ",sipType_%T,%s", ad, (isTransferredBack(ad) ? "Py_None" : "NULL"));
                else if (ad->atype == class_type)
                    prcode(fp, ",sipType_%C,%s", classFQCName(ad->u.cd), (isTransferredBack(ad) ? "Py_None" : "NULL"));
                else if (ad->atype == enum_type && ad->u.ed->fqcname != NULL)
                    prcode(fp,",sipType_%C", ad->u.ed->fqcname);
            }
        }

        prcode(fp,");\n"
            );

        /* All done for multiple values. */
        return;
    }

    /* Deal with the only returned value. */
    if (only < 0)
    {
        ad = res;
        vname = "sipRes";
    }
    else
    {
        ad = &od->pysig.args[only];

        sprintf(vnamebuf,"a%d",only);
        vname = vnamebuf;
    }

    switch (ad->atype)
    {
    case mapped_type:
        prcode(fp,
"            %s sipConvertFromType(", prefix);

        if (isConstArg(ad))
            prcode(fp,"const_cast<%b *>(%s)",ad,vname);
        else
            prcode(fp,"%s",vname);

        prcode(fp,",sipType_%T,%s);\n"
            , ad, (isTransferredBack(ad) ? "Py_None" : "NULL"));

        break;

    case class_type:
        {
            classDef *cd = ad->u.cd;
            int needNew = needNewInstance(ad);

            if (needNew)
                prcode(fp,
"            %s sipConvertFromNewType(", prefix);
            else
                prcode(fp,
"            %s sipConvertFromType(", prefix);

            if (isConstArg(ad))
                prcode(fp,"const_cast<%b *>(%s)",ad,vname);
            else
                prcode(fp,"%s",vname);

            prcode(fp,",sipType_%C,",classFQCName(cd));

            if (needNew)
                prcode(fp,"NULL");
            else
                prcode(fp,"%s\n"
                    , (isTransferredBack(ad) ? "Py_None" : "NULL"));

            prcode(fp,");\n"
                );
        }

        break;

    case bool_type:
    case cbool_type:
        prcode(fp,
"            %s PyBool_FromLong(%s);\n"
            ,prefix,vname);

        break;

    case sstring_type:
    case ustring_type:
    case string_type:
        if (ad->nrderefs == 0)
            prcode(fp,
"            %s PyString_FromStringAndSize(%s&%s,1);\n"
                ,prefix,(ad->atype != string_type) ? "(char *)" : "",vname);
        else
            prcode(fp,
"            if (%s == NULL)\n"
"            {\n"
"                Py_INCREF(Py_None);\n"
"                return Py_None;\n"
"            }\n"
"\n"
"            %s PyString_FromString(%s%s);\n"
            ,vname
            ,prefix,(ad->atype != string_type) ? "(char *)" : "",vname);

        break;

    case wstring_type:
        if (ad->nrderefs == 0)
            prcode(fp,
"            %s PyUnicode_FromWideChar(&%s,1);\n"
                , prefix, vname);
        else
            prcode(fp,
"            if (%s == NULL)\n"
"            {\n"
"                Py_INCREF(Py_None);\n"
"                return Py_None;\n"
"            }\n"
"\n"
"            %s PyUnicode_FromWideChar(%s,(SIP_SSIZE_T)wcslen(%s));\n"
            , vname
            , prefix, vname, vname);

        break;

    case enum_type:
        if (ad->u.ed->fqcname != NULL)
        {
            prcode(fp,
"            %s sipConvertFromEnum(%s,sipType_%C);\n"
                , prefix, vname, ad->u.ed->fqcname);

            break;
        }

        /* Drop through. */

    case short_type:
    case int_type:
    case cint_type:
        prcode(fp,
"            %s PyInt_FromLong(%s);\n"
            ,prefix,vname);

        break;

    case long_type:
        prcode(fp,
"            %s PyLong_FromLong(%s);\n"
            ,prefix,vname);

        break;

    case ushort_type:
    case uint_type:
    case ulong_type:
        prcode(fp,
"            %s PyLong_FromUnsignedLong(%s);\n"
            ,prefix,vname);

        break;

    case longlong_type:
        prcode(fp,
"            %s PyLong_FromLongLong(%s);\n"
            ,prefix,vname);

        break;

    case ulonglong_type:
        prcode(fp,
"            %s PyLong_FromUnsignedLongLong(%s);\n"
            ,prefix,vname);

        break;

    case void_type:
        {
            const char *cnst = (isConstArg(ad) ? "Const" : "");

            if (result_size < 0)
                prcode(fp,
"            %s sipConvertFrom%sVoidPtr(%s);\n"
                    , prefix, cnst, vname);
            else
                prcode(fp,
"            %s sipConvertFrom%sVoidPtrAndSize(%s,a%d);\n"
                    , prefix, cnst, vname, result_size);
        }

        break;

    case struct_type:
        prcode(fp,
"            %s sipConvertFrom%sVoidPtr(%s);\n"
            , prefix, (isConstArg(ad) ? "Const" : ""), vname);
        break;

    case float_type:
    case cfloat_type:
        prcode(fp,
"            %s PyFloat_FromDouble((double)%s);\n"
            ,prefix,vname);

        break;

    case double_type:
    case cdouble_type:
        prcode(fp,
"            %s PyFloat_FromDouble(%s);\n"
            ,prefix,vname);

        break;

    case pyobject_type:
    case pytuple_type:
    case pylist_type:
    case pydict_type:
    case pycallable_type:
    case pyslice_type:
    case pytype_type:
        prcode(fp,
"            %s %s;\n"
            ,prefix,vname);

        break;
    }
}


/*
 * Return the owner of a method result.
 */
static const char *resultOwner(overDef *od)
{
    if (isResultTransferredBack(od))
        return "Py_None";

    if (isResultTransferred(od))
        return "sipSelf";

    return "NULL";
}


/*
 * Return the format character used by sipBuildResult() for a particular type.
 */
static char getBuildResultFormat(argDef *ad)
{
    switch (ad->atype)
    {
    case mapped_type:
        return 'D';

    case fake_void_type:
    case class_type:
        if (needNewInstance(ad))
            return 'N';

        return 'D';

    case bool_type:
    case cbool_type:
        return 'b';

    case sstring_type:
    case ustring_type:
    case string_type:
        return (ad->nrderefs > (isOutArg(ad) ? 1 : 0)) ? 's' : 'c';

    case wstring_type:
        return (ad->nrderefs > (isOutArg(ad) ? 1 : 0)) ? 'x' : 'w';

    case enum_type:
        return (ad->u.ed->fqcname != NULL) ? 'F' : 'e';

    case short_type:
        return 'h';

    case ushort_type:
        return 't';

    case int_type:
    case cint_type:
        return 'i';

    case uint_type:
        return 'u';

    case long_type:
        return 'l';

    case ulong_type:
        return 'm';

    case longlong_type:
        return 'n';

    case ulonglong_type:
        return 'o';

    case void_type:
    case struct_type:
        return 'V';

    case float_type:
    case cfloat_type:
        return 'f';

    case double_type:
    case cdouble_type:
        return 'd';

    case pyobject_type:
    case pytuple_type:
    case pylist_type:
    case pydict_type:
    case pycallable_type:
    case pyslice_type:
    case pytype_type:
        return 'R';
    }

    /* We should never get here. */
    return ' ';
}


/*
 * Generate a function call.
 */
static void generateFunctionCall(classDef *cd,classDef *ocd,overDef *od,
                 int deref, FILE *fp)
{
    int needsNew, error_flag = FALSE, newline, is_result, result_size, a,
            deltemps;
    argDef *res = &od->pysig.result, orig_res;

    prcode(fp,
"        {\n"
        );

    /*
     * If there is no shadow class then protected methods can never be
     * called.
     */
    if (isProtected(od) && !hasShadow(cd))
    {
        prcode(fp,
"            /* Never reached. */\n"
"        }\n"
            );

        return;
    }

    /* Save the full result type as we may want to fiddle with it. */
    orig_res = *res;

    /* See if we need to make a copy of the result on the heap. */
    if ((res->atype == class_type || res->atype == mapped_type) &&
        !isReference(res) &&
        res->nrderefs == 0)
    {
        needsNew = TRUE;
        resetIsConstArg(res);
    }
    else
        needsNew = FALSE;

    /* See if sipRes is needed. */
    is_result = (!isInplaceNumberSlot(od->common) &&
             !isInplaceSequenceSlot(od->common) &&
             (res->atype != void_type || res->nrderefs != 0));

    newline = FALSE;

    if (is_result)
    {
        prcode(fp,
"            ");

        generateNamedValueType(cd, res, "sipRes", fp);

        /*
         * The typical %MethodCode usually causes a compiler warning,
         * so we initialise the result in that case to try and suppress
         * it.
         */
        if (od->methodcode != NULL)
        {
            prcode(fp," = ");

            generateCastZero(res,fp);
        }

        prcode(fp,";\n"
            );

        newline = TRUE;
    }

    result_size = -1;
    deltemps = TRUE;

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        argDef *ad = &od->pysig.args[a];

        if (isResultSize(ad))
            result_size = a;

        /*
         * If we have an In,Out argument that has conversion code then we delay
         * the destruction of any temporary variables until after we have
         * converted the outputs.
         */
        if (isInArg(ad) && isOutArg(ad) && hasConvertToCode(ad) && deltemps)
        {
            deltemps = FALSE;

            prcode(fp,
"            PyObject *sipResult;\n"
                );

            newline = TRUE;
        }

        /*
         * If we are returning a class via an output only reference or pointer
         * then we need an instance on the heap.
         */
        if (needNewInstance(ad))
        {
            /*
             * If it's a mapped type then we will need to destroy the instance
             * after it has been converted.
             */
            if (ad->atype == mapped_type && deltemps)
            {
                deltemps = FALSE;

                prcode(fp,
"            PyObject *sipResult;\n"
                    );
            }

            prcode(fp,
"            a%d = new %b();\n"
                ,a,ad);

            newline = TRUE;
        }
    }

    if (od->methodcode != NULL)
    {
        /* See if the handwritten code seems to be using the error flag. */
        if (needErrorFlag(od->methodcode))
        {
            prcode(fp,
"            int sipIsErr = 0;\n"
                );

            newline = TRUE;
            error_flag = TRUE;
        }
    }

    if (newline)
        prcode(fp,
"\n"
            );

    /* If it is abstract make sure that self was bound. */
    if (isAbstract(od))
        prcode(fp,
"            if (sipSelfWasArg)\n"
"            {\n"
"                sipAbstractMethod(%N,%N);\n"
"                return NULL;\n"
"            }\n"
"\n"
            , cd->pyname, od->common->pyname);

    if (isDeprecated(od))
    {
        /* Note that any temporaries will leak if an exception is raised. */
        if (cd != NULL)
            prcode(fp,
"            if (sipDeprecated(%N,%N) < 0)\n"
                , cd->pyname, od->common->pyname);
        else
            prcode(fp,
"            if (sipDeprecated(NULL,%N) < 0)\n"
                , od->common->pyname);

        prcode(fp,
"                return %s;\n"
"\n"
            , ((isVoidReturnSlot(od->common) || isIntReturnSlot(od->common) || isLongReturnSlot(od->common)) ? "-1" : "NULL"));
    }

    /* Call any pre-hook. */
    if (od->prehook != NULL)
        prcode(fp,
"            sipCallHook(\"%s\");\n"
"\n"
            ,od->prehook);

    if (od->methodcode != NULL)
        generateCppCodeBlock(od->methodcode,fp);
    else
    {
        int rgil = ((release_gil || isReleaseGIL(od)) && !isHoldGIL(od));

        if (needsNew && generating_c)
        {
            prcode(fp,
"            if ((sipRes = (%b *)sipMalloc(sizeof (%b))) == NULL)\n"
"        {\n"
                ,res,res);

            gc_ellipsis(&od->pysig, fp);

            prcode(fp,
"                return NULL;\n"
"            }\n"
"\n"
                );
        }

        if (rgil)
            prcode(fp,
"            Py_BEGIN_ALLOW_THREADS\n"
                );

        generateTry(od->exceptions,fp);

        prcode(fp,
"            ");

        if (od->common->slot != cmp_slot && is_result)
        {
            /* Construct a copy on the heap if needed. */
            if (needsNew)
            {
                if (generating_c)
                    prcode(fp,"*sipRes = ");
                else
                    prcode(fp,"sipRes = new %b(",res);
            }
            else
                prcode(fp,"sipRes = ");

            /* See if we need the address of the result. */
            if ((res->atype == class_type || res->atype == mapped_type) && isReference(res))
                prcode(fp,"&");
        }

        switch (od->common->slot)
        {
        case no_slot:
            generateCppFunctionCall(cd,ocd,od,fp);
            break;

        case getitem_slot:
            prcode(fp, "(*sipCpp)[");
            generateSlotArg(&od->pysig, 0, fp);
            prcode(fp,"]");
            break;

        case call_slot:
            prcode(fp, "(*sipCpp)(");
            generateCallArgs(cd, od->cppsig, &od->pysig, fp);
            prcode(fp,")");
            break;

        case int_slot:
        case long_slot:
        case float_slot:
            prcode(fp, "*sipCpp");
            break;

        case add_slot:
            generateNumberSlotCall(od,"+",fp);
            break;

        case concat_slot:
            generateBinarySlotCall(cd, od, "+", deref, fp);
            break;

        case sub_slot:
            generateNumberSlotCall(od,"-",fp);
            break;

        case mul_slot:
            generateNumberSlotCall(od,"*",fp);
            break;

        case repeat_slot:
            generateBinarySlotCall(cd, od, "*", deref, fp);
            break;

        case div_slot:
            generateNumberSlotCall(od,"/",fp);
            break;

        case mod_slot:
            generateNumberSlotCall(od,"%",fp);
            break;

        case and_slot:
            generateNumberSlotCall(od,"&",fp);
            break;

        case or_slot:
            generateNumberSlotCall(od,"|",fp);
            break;

        case xor_slot:
            generateNumberSlotCall(od,"^",fp);
            break;

        case lshift_slot:
            generateNumberSlotCall(od,"<<",fp);
            break;

        case rshift_slot:
            generateNumberSlotCall(od,">>",fp);
            break;

        case iadd_slot:
        case iconcat_slot:
            generateBinarySlotCall(cd, od, "+=", deref, fp);
            break;

        case isub_slot:
            generateBinarySlotCall(cd, od, "-=", deref, fp);
            break;

        case imul_slot:
        case irepeat_slot:
            generateBinarySlotCall(cd, od, "*=", deref, fp);
            break;

        case idiv_slot:
            generateBinarySlotCall(cd, od, "/=", deref, fp);
            break;

        case imod_slot:
            generateBinarySlotCall(cd, od, "%=", deref, fp);
            break;

        case iand_slot:
            generateBinarySlotCall(cd, od, "&=", deref, fp);
            break;

        case ior_slot:
            generateBinarySlotCall(cd, od, "|=", deref, fp);
            break;

        case ixor_slot:
            generateBinarySlotCall(cd, od, "^=", deref, fp);
            break;

        case ilshift_slot:
            generateBinarySlotCall(cd, od, "<<=", deref, fp);
            break;

        case irshift_slot:
            generateBinarySlotCall(cd, od, ">>=", deref, fp);
            break;

        case invert_slot:
            prcode(fp, "~(*sipCpp)");
            break;

        case lt_slot:
            generateComparisonSlotCall(cd, od, "<", ">=", deref, fp);
            break;

        case le_slot:
            generateComparisonSlotCall(cd, od, "<=", ">", deref, fp);
            break;

        case eq_slot:
            generateComparisonSlotCall(cd, od, "==", "!=", deref, fp);
            break;

        case ne_slot:
            generateComparisonSlotCall(cd, od, "!=", "==", deref, fp);
            break;

        case gt_slot:
            generateComparisonSlotCall(cd, od, ">", "<=", deref, fp);
            break;

        case ge_slot:
            generateComparisonSlotCall(cd, od, ">=", "<", deref, fp);
            break;

        case neg_slot:
            prcode(fp, "-(*sipCpp)");
            break;

        case pos_slot:
            prcode(fp, "+(*sipCpp)");
            break;

        case cmp_slot:
            prcode(fp,"if ");
            generateBinarySlotCall(cd, od, "<", deref, fp);
            prcode(fp,"\n"
"                sipRes = -1;\n"
"            else if ");
            generateBinarySlotCall(cd, od, ">", deref, fp);
            prcode(fp,"\n"
"                sipRes = 1;\n"
"            else\n"
"                sipRes = 0");

            break;
        }

        if (needsNew && !generating_c)
            prcode(fp,")");

        prcode(fp,";\n"
            );

        generateCatch(od->exceptions, &od->pysig, fp);

        if (rgil)
            prcode(fp,
"            Py_END_ALLOW_THREADS\n"
                );
    }

    /* Handle /TransferThis/ for non-factory methods. */
    if (!isFactory(od))
        for (a = 0; a < od->pysig.nrArgs; ++a)
        {
            argDef *ad = &od->pysig.args[a];

            if (!isInArg(ad))
                continue;

            if (isThisTransferred(ad))
            {
                prcode(fp,
"\n"
"            if (sipOwner)\n"
"                sipTransferTo(sipSelf, (PyObject *)sipOwner);\n"
"            else\n"
"                sipTransferBack(sipSelf);\n"
                        );

                break;
            }
        }

    if (isThisTransferredMeth(od))
        prcode(fp,
"\n"
"            sipTransferTo(sipSelf, NULL);\n"
                );

    gc_ellipsis(&od->pysig, fp);

    if (deltemps && !isZeroArgSlot(od->common))
        deleteTemps(&od->pysig, fp);

    prcode(fp,
"\n"
        );

    /* Handle sipIsErr if it was used. */
    if (error_flag)
        prcode(fp,
"            if (sipIsErr)\n"
"                return %s;\n"
"\n"
            ,((isVoidReturnSlot(od->common) || isIntReturnSlot(od->common) || isLongReturnSlot(od->common)) ? "-1" : "0"));

    /* Call any post-hook. */
    if (od->posthook != NULL)
        prcode(fp,
"\n"
"            sipCallHook(\"%s\");\n"
            ,od->posthook);

    if (isVoidReturnSlot(od->common))
        prcode(fp,
"            return 0;\n"
            );
    else if (isInplaceNumberSlot(od->common) || isInplaceSequenceSlot(od->common))
        prcode(fp,
"            Py_INCREF(sipSelf);\n"
"            return sipSelf;\n"
            );
    else if (isIntReturnSlot(od->common) || isLongReturnSlot(od->common))
        prcode(fp,
"            return sipRes;\n"
            );
    else
    {
        generateHandleResult(od, needsNew, result_size,
                (deltemps ? "return" : "sipResult ="), fp);

        /* Delete the temporaries now if we haven't already done so. */
        if (!deltemps)
        {
            deleteTemps(&od->pysig, fp);

            prcode(fp,
"\n"
"            return sipResult;\n"
                );
        }
    }

    prcode(fp,
"        }\n"
        );

    /* Restore the full type of the result. */
    *res = orig_res;
}


/*
 * Generate a call to a C++ function.
 */
static void generateCppFunctionCall(classDef *cd, classDef *ocd, overDef *od,
        FILE *fp)
{
    char *mname = od->cppname;
    int parens = 1;

    /*
     * If the function is protected then call the public wrapper.  If it is
     * virtual then call the explicit scoped function if "self" was passed as
     * the first argument.
     */

    if (cd == NULL)
        prcode(fp,"%s(",mname);
    else if (cd->iff->type == namespace_iface)
        prcode(fp,"%S::%s(",classFQCName(cd),mname);
    else if (isStatic(od))
    {
        if (isProtected(od))
            prcode(fp,"sip%C::sipProtect_%s(",classFQCName(cd),mname);
        else
            prcode(fp,"%S::%s(",classFQCName(ocd),mname);
    }
    else if (isProtected(od))
    {
        if (!isAbstract(od) && (isVirtual(od) || isVirtualReimp(od)))
        {
            prcode(fp, "sipCpp->sipProtectVirt_%s(sipSelfWasArg", mname);

            if (od->cppsig->nrArgs > 0)
                prcode(fp, ",");
        }
        else
            prcode(fp, "sipCpp->sipProtect_%s(", mname);
    }
    else if (!isAbstract(od) && (isVirtual(od) || isVirtualReimp(od)))
    {
        prcode(fp, "(sipSelfWasArg ? sipCpp->%U::%s(", ocd, mname);
        generateCallArgs(cd, od->cppsig, &od->pysig, fp);
        prcode(fp, ") : sipCpp->%s(", mname);
        ++parens;
    }
    else
        prcode(fp, "sipCpp->%s(", mname);

    generateCallArgs(cd, od->cppsig, &od->pysig, fp);

    while (parens--)
        prcode(fp, ")");
}


/*
 * Generate argument to a slot.
 */
static void generateSlotArg(signatureDef *sd, int argnr, FILE *fp)
{
    argDef *ad;
    int deref;

    ad = &sd->args[argnr];
    deref = ((ad->atype == class_type || ad->atype == mapped_type) && ad->nrderefs == 0);

    prcode(fp, "%sa%d", (deref ? "*" : ""), argnr);
}


/*
 * Generate the call to a comparison slot method.
 */
static void generateComparisonSlotCall(classDef *cd, overDef *od,
        const char *op, const char *cop, int deref, FILE *fp)
{
    if (isComplementary(od))
    {
        op = cop;
        prcode(fp, "!");
    }

    if (!isGlobal(od))
        prcode(fp, "sipCpp%s%S::operator%s(", (deref ? "->" : "."),
                classFQCName(cd), op);
    else if (deref)
        prcode(fp, "operator%s((*sipCpp), ", op);
    else
        prcode(fp, "operator%s(sipCpp, ", op);

    generateSlotArg(&od->pysig, 0, fp);
    prcode(fp, ")");
}


/*
 * Generate the call to a binary (non-number) slot method.
 */
static void generateBinarySlotCall(classDef *cd, overDef *od, const char *op,
        int deref, FILE *fp)
{
    generateComparisonSlotCall(cd, od, op, "", deref, fp);
}


/*
 * Generate the call to a binary number slot method.
 */
static void generateNumberSlotCall(overDef *od, char *op, FILE *fp)
{
    prcode(fp, "(");
    generateSlotArg(&od->pysig, 0, fp);
    prcode(fp, " %s ", op);
    generateSlotArg(&od->pysig, 1, fp);
    prcode(fp, ")");
}


/*
 * Generate the argument variables for a member function/constructor/operator.
 */
static int generateArgParser(signatureDef *sd, classDef *cd, ctorDef *ct,
        overDef *od, int secCall, FILE *fp)
{
    int a, isQtSlot, optargs, arraylenarg, sigarg, handle_self, single_arg;
    int slotconarg, slotdisarg, need_owner;

    /* If the class is just a namespace, then ignore it. */
    if (cd != NULL && cd->iff->type == namespace_iface)
        cd = NULL;

    handle_self = (od != NULL && od->common->slot == no_slot && !isStatic(od) && cd != NULL);

    /* Assume there isn't a Qt slot. */
    isQtSlot = FALSE;

    /*
     * Generate the local variables that will hold the parsed arguments and
     * values returned via arguments.
     */
    sigarg = -1;
    need_owner = FALSE;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        switch (ad->atype)
        {
        case signal_type:
            sigarg = a;
            break;

        case rxcon_type:
        case rxdis_type:
            isQtSlot = TRUE;
            break;

        case slotcon_type:
            slotconarg = a;
            break;

        case slotdis_type:
            slotdisarg = a;
            break;
        }

        if (isArraySize(ad))
            arraylenarg = a;

        generateVariable(cd, ad, a, fp);

        if (isThisTransferred(ad))
            need_owner = TRUE;
    }

    if (od != NULL && need_owner)
        prcode(fp,
"        sipWrapper *sipOwner = 0;\n"
            );

    if (handle_self)
    {
        if (isProtected(od) && hasShadow(cd))
            prcode(fp,
"        sip%C *sipCpp;\n"
                ,classFQCName(cd));
        else
            prcode(fp,
"        %U *sipCpp;\n"
                ,cd);

        prcode(fp,
"\n"
            );
    }
    else if (sd->nrArgs != 0)
        prcode(fp,
"\n"
            );

    /* Generate the call to the parser function. */
    if (od != NULL && isNumberSlot(od->common))
    {
        single_arg = FALSE;

        prcode(fp,
"        if (sipParsePair(%ssipArgsParsed,sipArg0,sipArg1,\"", (ct != NULL ? "" : "&"));
    }
    else
    {
        single_arg = (od != NULL && od->common->slot != no_slot && !isMultiArgSlot(od->common));

        prcode(fp,
"        if (sipParseArgs(%ssipArgsParsed,sipArg%s,\"", (ct != NULL ? "" : "&"), (single_arg ? "" : "s"));
    }

    /* Generate the format string. */
    optargs = FALSE;

    if (single_arg)
        prcode(fp, "1");

    if (handle_self)
        prcode(fp,"%c",(isProtected(od) ? 'p' : 'B'));
    else if (isQtSlot && od == NULL)
        prcode(fp,"C");

    for (a = 0; a < sd->nrArgs; ++a)
    {
        char *fmt = "";
        argDef *ad = &sd->args[a];

        if (!isInArg(ad))
            continue;

        if (ad->defval != NULL && !optargs)
        {
            prcode(fp,"|");
            optargs = TRUE;
        }

        switch (ad->atype)
        {
        case sstring_type:
        case ustring_type:
        case string_type:
            if (ad->nrderefs == 0 || (isOutArg(ad) && ad->nrderefs == 1))
                fmt = "c";
            else if (isArray(ad))
                fmt = "k";
            else
                fmt = "s";

            break;

        case wstring_type:
            if (ad->nrderefs == 0 || (isOutArg(ad) && ad->nrderefs == 1))
                fmt = "w";
            else if (isArray(ad))
                fmt = "K";
            else
                fmt = "x";

            break;

        case enum_type:
            if (ad->u.ed->fqcname == NULL)
                fmt = "e";
            else if (isConstrained(ad))
                fmt = "XE";
            else
                fmt = "E";
            break;

        case bool_type:
            fmt = "b";
            break;

        case cbool_type:
            fmt = "Xb";
            break;

        case int_type:
            if (!isArraySize(ad))
                fmt = "i";

            break;

        case uint_type:
            if (!isArraySize(ad))
                fmt = "u";

            break;

        case cint_type:
            fmt = "Xi";
            break;

        case short_type:
            if (!isArraySize(ad))
                fmt = "h";

            break;

        case ushort_type:
            if (!isArraySize(ad))
                fmt = "t";

            break;

        case long_type:
            if (!isArraySize(ad))
                fmt = "l";

            break;

        case ulong_type:
            if (!isArraySize(ad))
                fmt = "m";

            break;

        case longlong_type:
            if (!isArraySize(ad))
                fmt = "n";

            break;

        case ulonglong_type:
            if (!isArraySize(ad))
                fmt = "o";

            break;

        case struct_type:
        case void_type:
            fmt = "v";
            break;

        case float_type:
            fmt = "f";
            break;

        case cfloat_type:
            fmt = "Xf";
            break;

        case double_type:
            fmt = "d";
            break;

        case cdouble_type:
            fmt = "Xd";
            break;

        case signal_type:
            fmt = "G";
            break;

        case slot_type:
            fmt = "S";
            break;

        case anyslot_type:
            fmt = "U";
            break;

        case slotcon_type:
        case slotdis_type:
            fmt = (secCall ? "" : "S");
            break;

        case rxcon_type:
            fmt = (secCall ? (isSingleShot(ad) ? "g" : "y") : "q");
            break;

        case rxdis_type:
            fmt = (secCall ? "Y" : "Q");
            break;

        case mapped_type:
        case class_type:
            fmt = getSubFormatChar('J', ad);
            break;

        case pyobject_type:
            fmt = getSubFormatChar('P',ad);
            break;

        case pytuple_type:
        case pylist_type:
        case pydict_type:
        case pyslice_type:
        case pytype_type:
            fmt = (isAllowNone(ad) ? "N" : "T");
            break;

        case pycallable_type:
            fmt = (isAllowNone(ad) ? "H" : "F");
            break;

        case qobject_type:
            fmt = "R";
            break;

        case ellipsis_type:
            fmt = "W";
            break;
        }

        prcode(fp,fmt);
    }

    prcode(fp,"\"");

    /* Generate the parameters corresponding to the format string. */

    if (handle_self)
        prcode(fp,",&sipSelf,sipType_%C,&sipCpp",classFQCName(cd));
    else if (isQtSlot && od == NULL)
        prcode(fp,",sipSelf");

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        if (!isInArg(ad))
            continue;

        switch (ad->atype)
        {
        case mapped_type:
            if (noRelease(ad->u.mtd))
                prcode(fp, ",sipType_%T,&a%d,NULL", ad, a);
            else
                prcode(fp, ",sipType_%T,&a%d,&a%dState", ad, a, a);

            break;

        case class_type:
            prcode(fp, ",sipType_%T,&a%d", ad, a);

            if (isThisTransferred(ad))
                prcode(fp, ",%ssipOwner", (ct != NULL ? "" : "&"));
            else if (isGetWrapper(ad))
                prcode(fp, ",&a%dWrapper", a);

            if (ad->u.cd->convtocode != NULL && !isConstrained(ad))
                prcode(fp, ",&a%dState", a);

            break;

        case rxcon_type:
            {
                if (sigarg > 0)
                    prcode(fp,",a%d",sigarg);
                else
                {
                    prcode(fp,",\"(");

                    generateCalledArgs(cd, sd->args[slotconarg].u.sa, Declaration, TRUE, fp);

                    prcode(fp,")\"");
                }

                prcode(fp,",&a%d,&a%d",a,slotconarg);

                break;
            }

        case rxdis_type:
            {
                prcode(fp,",\"(");

                generateCalledArgs(cd, sd->args[slotdisarg].u.sa, Declaration, TRUE, fp);

                prcode(fp,")\",&a%d,&a%d",a,slotdisarg);

                break;
            }

        case slotcon_type:
        case slotdis_type:
            if (!secCall)
                prcode(fp,",&a%d",a);

            break;

        case anyslot_type:
            prcode(fp, ",&a%dName,&a%dCallable", a, a);
            break;

        case pytuple_type:
            prcode(fp,",&PyTuple_Type,&a%d",a);
            break;

        case pylist_type:
            prcode(fp,",&PyList_Type,&a%d",a);
            break;

        case pydict_type:
            prcode(fp,",&PyDict_Type,&a%d",a);
            break;

        case pyslice_type:
            prcode(fp,",&PySlice_Type,&a%d",a);
            break;

        case pytype_type:
            prcode(fp,",&PyType_Type,&a%d",a);
            break;

        case enum_type:
            if (ad->u.ed->fqcname != NULL)
                prcode(fp, ",sipType_%C", ad->u.ed->fqcname);

            prcode(fp,",&a%d",a);
            break;

        default:
            if (!isArraySize(ad))
                prcode(fp,",&a%d",a);

            if (isArray(ad))
                prcode(fp,",&a%d",arraylenarg);
        }
    }

    prcode(fp,"))\n");

    return isQtSlot;
}


/*
 * Get the format character string for something that has sub-formats.
 */

static char *getSubFormatChar(char fc,argDef *ad)
{
    static char fmt[3];
    char flags;

    fmt[0] = fc;

    flags = 0;

    if (fc != 'P' && ad->nrderefs == 0)
        flags |= 0x01;

    if (isTransferred(ad))
        flags |= 0x02;

    if (isTransferredBack(ad))
        flags |= 0x04;

    if (fc == 'J')
    {
        if (isThisTransferred(ad))
            flags |= 0x20;

        if (isGetWrapper(ad))
            flags |= 0x08;

        if (ad->u.cd->convtocode == NULL || isConstrained(ad))
            flags |= 0x10;
    }

    fmt[1] = '0' + flags;

    fmt[2] = '\0';

    return fmt;
}


/*
 * Return TRUE if a type has %ConvertToTypeCode.
 */
static int hasConvertToCode(argDef *ad)
{
    codeBlock *convtocode;

    if (ad->atype == class_type && !isConstrained(ad))
        convtocode = ad->u.cd->convtocode;
    else if (ad->atype == mapped_type)
        convtocode = ad->u.mtd->convtocode;
    else
        convtocode = NULL;

    return (convtocode != NULL);
}


/*
 * Garbage collect any ellipsis argument.
 */
static void gc_ellipsis(signatureDef *sd, FILE *fp)
{
    if (sd->nrArgs > 0 && sd->args[sd->nrArgs - 1].atype == ellipsis_type)
        prcode(fp,
"\n"
"            Py_DECREF(a%d);\n"
            , sd->nrArgs - 1);
}


/*
 * Delete any temporary variables on the heap created by type convertors.
 */
static void deleteTemps(signatureDef *sd, FILE *fp)
{
    int a, first = TRUE;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        if (ad->atype == mapped_type && needNewInstance(ad))
        {
            prcode(fp,
"            delete a%d;\n"
                , a);

            continue;
        }

        if (!isInArg(ad))
            continue;

        if (ad->atype == wstring_type && ad->nrderefs == 1)
        {
            if (generating_c || !isConstArg(ad))
                prcode(fp,
"            sipFree(a%d);\n"
                    , a);
            else
                prcode(fp,
"            sipFree(const_cast<wchar_t *>(a%d));\n"
                    , a);
        }
        else if (hasConvertToCode(ad))
        {
            const char *fstr, *sstr;

            if (ad->atype == mapped_type)
            {
                if (noRelease(ad->u.mtd))
                    continue;

                fstr = "MappedType";
                sstr = "Type";
            }
            else
            {
                fstr = "Instance";
                sstr = "Class";
            }

            if (first)
            {
                prcode(fp,
"\n"
                    );

                first = FALSE;
            }

            if (generating_c || !isConstArg(ad))
                prcode(fp,
"            sipRelease%s(a%d,sip%s_%T,a%dState);\n"
                    , fstr, a, sstr, ad, a);
            else
                prcode(fp,
"            sipRelease%s(const_cast<%b *>(a%d),sip%s_%T,a%dState);\n"
                    , fstr, ad, a, sstr, ad, a);
        }
    }
}


/*
 * Generate a C++ code block.
 */
static void generateCppCodeBlock(codeBlock *code, FILE *fp)
{
    int reset_line = FALSE;
    codeBlock *cb;

    for (cb = code; cb != NULL; cb = cb->next)
    {
        const char *cp;

        /*
         * Fragmented fragments (possibly created when applying template types)
         * don't have a filename.
         */
        if ((cp = cb->filename) != NULL)
        {
            reset_line = TRUE;

            prcode(fp,
"#line %d \"", cb->linenr);

            while (*cp != '\0')
            {
                prcode(fp, "%c", *cp);

                if (*cp == '\\')
                    prcode(fp, "\\");

                ++cp;
            }

            prcode(fp, "\"\n"
                );
        }

        prcode(fp, "%s", cb->frag);
    }

    if (reset_line)
    {
        const char *bn;

        /* Just use the base name. */
        if ((bn = strrchr(currentFileName, '/')) != NULL)
            ++bn;
        else
            bn = currentFileName;

        prcode(fp,
"#line %d \"%s\"\n"
            , currentLineNr + 1, bn);
    }
}


/*
 * Create a source file.
 */
static FILE *createCompilationUnit(moduleDef *mod, const char *fname,
        const char *description)
{
    FILE *fp = createFile(mod, fname, description);

    if (fp != NULL)
        generateCppCodeBlock(mod->unitcode, fp);

    return fp;
}


/*
 * Create a file with an optional standard header.
 */
static FILE *createFile(moduleDef *mod, const char *fname,
        const char *description)
{
    FILE *fp;

    /* Create the file. */
    if ((fp = fopen(fname, "w")) == NULL)
        fatal("Unable to create file \"%s\"\n",fname);

    /* The "stack" doesn't have to be very deep. */
    previousLineNr = currentLineNr;
    currentLineNr = 1;
    previousFileName = currentFileName;
    currentFileName = fname;

    if (description != NULL)
    {
        int needComment;
        codeBlock *cb;
        time_t now;

        /* Write the header. */
        now = time(NULL);

        prcode(fp,
"/*\n"
" * %s\n"
" *\n"
" * Generated by SIP %s on %s"
            ,description
            ,sipVersion,ctime(&now));

        if (mod->copying != NULL)
            prcode(fp,
" *\n"
                );

        needComment = TRUE;

        for (cb = mod->copying; cb != NULL; cb = cb->next)
        {
            const char *cp;

            for (cp = cb->frag; *cp != '\0'; ++cp)
            {
                if (needComment)
                {
                    needComment = FALSE;
                    prcode(fp," * ");
                }

                prcode(fp,"%c",*cp);

                if (*cp == '\n')
                    needComment = TRUE;
            }
        }

        prcode(fp,
" */\n"
            );
    }

    return fp;
}


/*
 * Close a file and report any errors.
 */
static void closeFile(FILE *fp)
{
    if (ferror(fp))
        fatal("Error writing to \"%s\"\n",currentFileName);

    if (fclose(fp))
        fatal("Error closing \"%s\"\n",currentFileName);

    currentLineNr = previousLineNr;
    currentFileName = previousFileName;
}


/*
 * Print formatted code.
 */
void prcode(FILE *fp, const char *fmt, ...)
{
    char ch;
    va_list ap;

    prcode_last = fmt;

    va_start(ap,fmt);

    while ((ch = *fmt++) != '\0')
        if (ch == '%')
        {
            ch = *fmt++;

            switch (ch)
            {
            case 'c':
                {
                    char c = (char)va_arg(ap,int);

                    if (c == '\n')
                        ++currentLineNr;

                    fputc(c,fp);
                    break;
                }

            case 's':
                {
                    const char *cp = va_arg(ap,const char *);

                    while (*cp != '\0')
                    {
                        if (*cp == '\n')
                            ++currentLineNr;

                        fputc(*cp,fp);
                        ++cp;
                    }

                    break;
                }

            case 'l':
                fprintf(fp,"%ld",va_arg(ap,long));
                break;

            case 'u':
                fprintf(fp,"%u",va_arg(ap,unsigned));
                break;

            case 'd':
                fprintf(fp,"%d",va_arg(ap,int));
                break;

            case 'g':
                fprintf(fp,"%g",va_arg(ap,double));
                break;

            case 'x':
                fprintf(fp,"0x%08x",va_arg(ap,unsigned));
                break;

            case '\0':
                fputc('%',fp);
                --fmt;
                break;

            case '\n':
                fputc('\n',fp);
                ++currentLineNr;
                break;

            case 'b':
                {
                    argDef *ad, orig;

                    ad = va_arg(ap,argDef *);
                    orig = *ad;

                    resetIsConstArg(ad);
                    resetIsReference(ad);
                    ad->nrderefs = 0;

                    generateBaseType(NULL, ad, TRUE, fp);

                    *ad = orig;

                    break;
                }

            case 'M':
                prcode_xml = !prcode_xml;
                break;

            case 'A':
                {
                    classDef *context = va_arg(ap, classDef *);
                    argDef *ad = va_arg(ap, argDef *);

                    generateBaseType(context, ad, TRUE, fp);
                    break;
                }

            case 'B':
                generateBaseType(NULL, va_arg(ap,argDef *),TRUE, fp);
                break;

            case 'T':
                prTypeName(fp,va_arg(ap,argDef *),FALSE);
                break;

            case 'I':
                {
                    int indent = va_arg(ap,int);

                    while (indent-- > 0)
                        fputc('\t',fp);

                    break;
                }

            case 'N':
                {
                    nameDef *nd = va_arg(ap,nameDef *);

                    prCachedName(fp, nd, "sipName_");
                    break;
                }

            case 'n':
                {
                    nameDef *nd = va_arg(ap,nameDef *);

                    prCachedName(fp, nd, "sipNameNr_");
                    break;
                }

            case 'E':
                {
                    enumDef *ed = va_arg(ap,enumDef *);

                    if (ed->fqcname == NULL || isProtectedEnum(ed))
                        fprintf(fp,"int");
                    else
                        prScopedName(fp,ed->fqcname,"::");

                    break;
                }

            case 'F':
                prScopedName(fp,va_arg(ap,scopedNameDef *),"");
                break;

            case 'C':
                prScopedName(fp,va_arg(ap,scopedNameDef *),"_");
                break;

            case 'S':
                if (generating_c)
                    fprintf(fp,"struct ");

                prScopedName(fp,va_arg(ap,scopedNameDef *),"::");
                break;

            case 'U':
                {
                    classDef *cd = va_arg(ap, classDef *);

                    if (generating_c)
                        fprintf(fp,"struct ");

                    prScopedClassName(fp, cd, cd);
                    break;
                }

            case 'V':
                {
                    classDef *context = va_arg(ap, classDef *);
                    classDef *cd = va_arg(ap, classDef *);

                    if (generating_c)
                        fprintf(fp,"struct ");

                    prScopedClassName(fp, context, cd);
                    break;
                }

            case 'O':
                prOverloadName(fp, va_arg(ap, overDef *));
                break;

            case 'X':
                generateThrowSpecifier(va_arg(ap,throwArgs *),fp);
                break;

            default:
                fputc(ch,fp);
            }
        }
        else if (ch == '\n')
        {
            fputc('\n',fp);
            ++currentLineNr;
        }
        else
            fputc(ch,fp);

    va_end(ap);
}


/*
 * Generate the symbolic name of a cached name.
 */
static void prCachedName(FILE *fp, nameDef *nd, const char *prefix)
{
    prcode(fp, "%s", prefix);

    /*
     * If the name seems to be a template then just use the offset to ensure
     * that it is unique.
     */
    if (strchr(nd->text, '<') != NULL)
        prcode(fp, "%d", nd->offset);
    else
    {
        const char *cp;

        /* Handle C++ and Python scopes. */
        for (cp = nd->text; *cp != '\0'; ++cp)
        {
            char ch = *cp;

            if (ch == ':' || ch == '.')
                ch = '_';

            prcode(fp, "%c", ch);
        }
    }
}


/*
 * Generate the C++ name of an overloaded function.
 */
void prOverloadName(FILE *fp, overDef *od)
{
    char *pt1, *pt2;

    pt1 = "operator";

    switch (od->common->slot)
    {
    case add_slot:
        pt2 = "+";
        break;

    case sub_slot:
        pt2 = "-";
        break;

    case mul_slot:
        pt2 = "*";
        break;

    case div_slot:
        pt2 = "/";
        break;

    case mod_slot:
        pt2 = "%";
        break;

    case and_slot:
        pt2 = "&";
        break;

    case or_slot:
        pt2 = "|";
        break;

    case xor_slot:
        pt2 = "^";
        break;

    case lshift_slot:
        pt2 = "<<";
        break;

    case rshift_slot:
        pt2 = ">>";
        break;

    case iadd_slot:
        pt2 = "+=";
        break;

    case isub_slot:
        pt2 = "-=";
        break;

    case imul_slot:
        pt2 = "*=";
        break;

    case idiv_slot:
        pt2 = "/=";
        break;

    case imod_slot:
        pt2 = "%=";
        break;

    case iand_slot:
        pt2 = "&=";
        break;

    case ior_slot:
        pt2 = "|=";
        break;

    case ixor_slot:
        pt2 = "^=";
        break;

    case ilshift_slot:
        pt2 = "<<=";
        break;

    case irshift_slot:
        pt2 = ">>=";
        break;

    case invert_slot:
        pt2 = "~";
        break;

    case call_slot:
        pt2 = "()";
        break;

    case getitem_slot:
        pt2 = "[]";
        break;

    case lt_slot:
        pt2 = "<";
        break;

    case le_slot:
        pt2 = "<=";
        break;

    case eq_slot:
        pt2 = "==";
        break;

    case ne_slot:
        pt2 = "!=";
        break;

    case gt_slot:
        pt2 = ">";
        break;

    case ge_slot:
        pt2 = ">=";
        break;

    default:
        pt1 = "";
        pt2 = od->cppname;
    }

    fprintf(fp, "%s%s", pt1, pt2);
}


/*
 * Generate a scoped name with the given separator string.
 */
static void prScopedName(FILE *fp,scopedNameDef *snd,char *sep)
{
    while (snd != NULL)
    {
        fprintf(fp,"%s",snd->name);

        if ((snd = snd->next) != NULL)
            fprintf(fp,"%s",sep);
    }
}


/*
 * Generate a scoped class name in a particular class context.
 */
static void prScopedClassName(FILE *fp, classDef *context, classDef *cd)
{
    /* Protected classes have to be referred to by the context class. */
    if (isProtectedClass(cd))
    {
        if (context == NULL)
            context = cd;

        prcode(fp, "sip%C::sip%s", classFQCName(context), classBaseName(cd));
    }
    else
    {
        scopedNameDef *snd = classFQCName(cd);

        while (snd != NULL)
        {
            fprintf(fp,"%s",snd->name);

            if ((snd = snd->next) != NULL)
                fprintf(fp, "::");
        }
    }
}


/*
 * Generate a type name to be used as part of an identifier name.
 */
static void prTypeName(FILE *fp,argDef *ad,int intmpl)
{
    if (intmpl)
    {
        char buf[50];
        int flgs;

        /* We use numbers so they don't conflict with names. */

        sprintf(buf,"%02d",ad->atype);

        flgs = 0;

        if (isConstArg(ad))
            flgs += 1;

        if (isReference(ad))
            flgs += 2;

        prcode(fp,"%s%d%d",buf,flgs,ad->nrderefs);
    }

    /* Now add a name if there is one associated with the type. */

    switch (ad->atype)
    {
    case struct_type:
        prcode(fp,"%C",ad->u.sname);
        break;

    case defined_type:
        prcode(fp,"%C",ad->u.snd);
        break;

    case enum_type:
        prcode(fp,"%C",ad->u.ed->fqcname);
        break;

    case mapped_type:
        prTypeName(fp,&ad->u.mtd->type,intmpl);
        break;

    case class_type:
        prcode(fp,"%C",classFQCName(ad->u.cd));
        break;

    case template_type:
        {
            int a;
            templateDef *td = ad->u.td;

            prcode(fp,"%C",td->fqname);

            for (a = 0; a < td->types.nrArgs; ++a)
            {
                prcode(fp,"_");
                prTypeName(fp,&td->types.args[a],TRUE);
            }

            break;
        }
    }
}


/*
 * Return TRUE if handwritten code uses the error flag.
 */
static int needErrorFlag(codeBlock *cb)
{
    return usedInCode(cb, "sipIsErr");
}


/*
 * Return TRUE if the argument type means an instance needs to be created on
 * the heap to pass back to Python.
 */
static int needNewInstance(argDef *ad)
{
    return ((ad->atype == mapped_type || ad->atype == class_type) &&
        ((isReference(ad) && ad->nrderefs == 0) || (!isReference(ad) && ad->nrderefs == 1)) &&
        !isInArg(ad) && isOutArg(ad));
}


/*
 * Convert any protected arguments (ie. those whose type is unavailable outside
 * of a shadow class) to a fundamental type to be used instead (with suitable
 * casts).
 */
static void fakeProtectedArgs(signatureDef *sd)
{
    int a;
    argDef *ad = sd->args;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        if (ad->atype == class_type && isProtectedClass(ad->u.cd))
        {
            ad->atype = fake_void_type;
            ad->nrderefs = 1;
            resetIsReference(ad);
        }
        else if (ad->atype == enum_type && isProtectedEnum(ad->u.ed))
            ad->atype = int_type;

        ++ad;
    }
}


/*
 * Reset and save any argument flags so that the signature will be rendered
 * exactly as defined in C++.
 */
static void normaliseArgs(signatureDef *sd)
{
    int a;
    argDef *ad = sd->args;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        if (ad->atype == class_type && isProtectedClass(ad->u.cd))
        {
            resetIsProtectedClass(ad->u.cd);
            setWasProtectedClass(ad->u.cd);
        }
        else if (ad->atype == enum_type && isProtectedEnum(ad->u.ed))
        {
            resetIsProtectedEnum(ad->u.ed);
            setWasProtectedEnum(ad->u.ed);
        }

        ++ad;
    }
}


/*
 * Restore any argument flags modified by normaliseArgs().
 */
static void restoreArgs(signatureDef *sd)
{
    int a;
    argDef *ad = sd->args;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        if (ad->atype == class_type && wasProtectedClass(ad->u.cd))
        {
            resetWasProtectedClass(ad->u.cd);
            setIsProtectedClass(ad->u.cd);
        }
        else if (ad->atype == enum_type && wasProtectedEnum(ad->u.ed))
        {
            resetWasProtectedEnum(ad->u.ed);
            setIsProtectedEnum(ad->u.ed);
        }

        ++ad;
    }
}


/*
 * Return TRUE if a dealloc function is needed for a class.
 */
static int needDealloc(classDef *cd)
{
    if (cd->iff->type == namespace_iface)
        return FALSE;

    /* All of these conditions cause some code to be generated. */

    if (tracing)
        return TRUE;

    if (generating_c)
        return TRUE;

    if (cd->dealloccode != NULL)
        return TRUE;

    if (isPublicDtor(cd))
        return TRUE;

    if (hasShadow(cd))
        return TRUE;

    return FALSE;
}


/*
 * Return the argument name to use in a function definition for handwritten
 * code.
 */
static const char *argName(const char *name, codeBlock *cb)
{
    static const char noname[] = "";

    /* Always use the name in C code. */
    if (generating_c)
        return name;

    /* Use the name if it is used in the handwritten code. */
    if (usedInCode(cb, name))
        return name;

    /* Don't use the name and avoid a compiler warning. */
    return noname;
}


/*
 * Returns TRUE if a string is used in a code block.
 */
static int usedInCode(codeBlock *code, const char *str)
{
    while (code != NULL)
    {
        if (strstr(code->frag, str) != NULL)
            return TRUE;

        code = code->next;
    }

    return FALSE;
}


/*
 * Generate an assignment statement from a void * variable to a class instance
 * variable.
 */
static void generateClassFromVoid(classDef *cd, const char *cname,
        const char *vname, FILE *fp)
{
    if (generating_c)
        prcode(fp, "%S *%s = (%S *)%s", classFQCName(cd), cname, classFQCName(cd), vname);
    else
        prcode(fp, "%S *%s = reinterpret_cast<%S *>(%s)", classFQCName(cd), cname, classFQCName(cd), vname);
}


/*
 * Generate an assignment statement from a void * variable to a mapped type
 * variable.
 */
static void generateMappedTypeFromVoid(mappedTypeDef *mtd, const char *cname,
        const char *vname, FILE *fp)
{
    if (generating_c)
        prcode(fp, "%b *%s = (%b *)%s", &mtd->type, cname, &mtd->type, vname);
    else
        prcode(fp, "%b *%s = reinterpret_cast<%b *>(%s)", &mtd->type, cname, &mtd->type, vname);
}
