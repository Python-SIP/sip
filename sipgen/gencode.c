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


/* Control what generateSingleArg() actually generates. */
typedef enum {
	Call,
	Declaration,
	Definition
} funcArgType;


/* An entry in the sorted array of methods. */
typedef struct {
	memberDef	*md;		/* The method. */
	int		is_static;	/* Set if all overloads are static. */
} sortedMethTab;


/* An API definition. */
typedef struct _apiDef {
	char		*name;		/* The function name. */
	signatureDef	*sd;		/* The function signature. */
	struct _apiDef	*next;		/* The next in the list. */
} apiDef;


static int currentLineNr;		/* Current output line number. */
static char *currentFileName;		/* Current output file name. */
static int previousLineNr;		/* Previous output line number. */
static char *previousFileName;		/* Previous output file name. */
static int exceptions;			/* Set if exceptions are enabled. */
static int tracing;			/* Set if tracing is enabled. */
static int generating_c;		/* Set if generating C. */
static int release_gil;			/* Set if always releasing the GIL. */
static char *prcode_last = NULL;	/* The last prcode format string. */


static void generateDocumentation(sipSpec *,char *);
static void generateAPI(sipSpec *,char *);
static void generateBuildFile(sipSpec *,char *,char *,int);
static void generateInternalAPIHeader(sipSpec *,char *,stringList *);
static void generateCpp(sipSpec *,char *,char *,int *);
static void generateIfaceCpp(sipSpec *,ifaceFileDef *,char *,char *,FILE *);
static void generateMappedTypeCpp(mappedTypeDef *,FILE *);
static void generateImportedMappedTypeHeader(mappedTypeDef *mtd,sipSpec *pt,
					     FILE *fp);
static void generateMappedTypeHeader(mappedTypeDef *,int,FILE *);
static void generateClassCpp(classDef *,sipSpec *,FILE *);
static void generateImportedClassHeader(classDef *cd,sipSpec *pt,FILE *fp);
static void generateClassTableEntries(nodeDef *,FILE *);
static void generateClassHeader(classDef *,int,sipSpec *,FILE *);
static void generateClassFunctions(sipSpec *,classDef *,FILE *);
static void generateShadowCode(sipSpec *,classDef *,FILE *);
static void generateFunction(sipSpec *,memberDef *,overDef *,classDef *,
			     classDef *,FILE *);
static void generateFunctionBody(sipSpec *,overDef *,classDef *,classDef *,
				 FILE *);
static void generateTypeDefinition(sipSpec *,classDef *,FILE *);
static void generateTypeInit(sipSpec *,classDef *,FILE *);
static void generateCppCodeBlock(codeBlock *,FILE *);
static void generateUsedIncludes(ifaceFileList *, int, FILE *);
static void generateIfaceHeader(sipSpec *,ifaceFileDef *,char *);
static void generateShadowClassDeclaration(sipSpec *,classDef *,FILE *);
static void generateResultType(argDef *,FILE *);
static int hasConvertToCode(argDef *ad);
static void deleteTemps(signatureDef *sd, FILE *fp);
static void gc_ellipsis(signatureDef *sd, FILE *fp);
static void generateArgs(signatureDef *,funcArgType,FILE *);
static void generateVariable(argDef *,int,FILE *);
static void generateNamedValueType(argDef *,char *,FILE *);
static void generateSingleArg(argDef *,int,funcArgType,FILE *);
static void generateBaseType(argDef *,FILE *);
static void generateNamedBaseType(argDef *,char *,FILE *);
static void generateExpression(valueDef *,FILE *);
static void generateTupleBuilder(signatureDef *,FILE *);
static void generateEmitter(sipSpec *,classDef *,visibleList *,FILE *);
static void generateVirtualHandler(sipSpec *,virtHandlerDef *,FILE *);
static void generateVirtHandlerErrorReturn(argDef *res,FILE *fp);
static void generateVirtualCatcher(sipSpec *,classDef *,int,virtOverDef *,
				   FILE *);
static void generateUnambiguousClass(classDef *cd,classDef *scope,FILE *fp);
static void generateProtectedEnums(sipSpec *,classDef *,FILE *);
static void generateProtectedDeclarations(classDef *,FILE *);
static void generateProtectedDefinitions(classDef *,FILE *);
static void generateConstructorCall(classDef *,ctorDef *,int,FILE *);
static void generateHandleResult(overDef *,int,char *,FILE *);
static void generateOrdinaryFunction(sipSpec *,classDef *,memberDef *,FILE *);
static void generateSimpleFunctionCall(fcallDef *,FILE *);
static void generateFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				 FILE *fp);
static void generateCppFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				    FILE *fp);
static void generateSlotArg(signatureDef *sd, int argnr, FILE *fp);
static void generateBinarySlotCall(overDef *od, char *op, FILE *fp);
static void generateNumberSlotCall(overDef *od, char *op, FILE *fp);
static void generateVariableHandler(varDef *,FILE *);
static void generateObjToCppConversion(argDef *,FILE *);
static void generateVarClassConversion(varDef *,FILE *);
static void generateVarMember(varDef *vd, FILE *fp);
static int generateVoidPointers(sipSpec *,classDef *,FILE *);
static int generateChars(sipSpec *,classDef *,FILE *);
static int generateStrings(sipSpec *,classDef *,FILE *);
static sortedMethTab *createFunctionTable(classDef *,int *);
static sortedMethTab *createMethodTable(classDef *,int *);
static int generateMethodTable(classDef *,FILE *);
static void generateEnumMacros(sipSpec *pt, classDef *cd, FILE *fp);
static int generateEnumMemberTable(sipSpec *,classDef *,FILE *);
static int generateInts(sipSpec *,classDef *,FILE *);
static int generateLongs(sipSpec *,classDef *,FILE *);
static int generateUnsignedLongs(sipSpec *,classDef *,FILE *);
static int generateLongLongs(sipSpec *,classDef *,FILE *);
static int generateUnsignedLongLongs(sipSpec *,classDef *,FILE *);
static int generateVariableType(sipSpec *pt, classDef *cd, argType atype, const char *eng, const char *s1, const char *s2, FILE *fp);
static int generateDoubles(sipSpec *,classDef *,FILE *);
static int generateEnums(sipSpec *,classDef *,FILE *);
static int generateClasses(sipSpec *,classDef *,FILE *);
static void generateClassesInline(sipSpec *,FILE *);
static void generateAccessFunctions(sipSpec *,classDef *,FILE *);
static void generateConvertToDefinitions(mappedTypeDef *,classDef *,FILE *);
static void generateEncodedClass(sipSpec *,classDef *,int,FILE *);
static int generateArgParser(sipSpec *, signatureDef *, classDef *, ctorDef *,
			     overDef *, int, FILE *);
static void generateTry(throwArgs *,FILE *);
static void generateCatch(throwArgs *,FILE *);
static void generateThrowSpecifier(throwArgs *,FILE *);
static void generateSlot(sipSpec *pt, classDef *cd, enumDef *ed, memberDef *md, FILE *fp);
static void generateCastZero(argDef *ad,FILE *fp);
static void generateCallDefaultCtor(ctorDef *ct,FILE *fp);
static void addUniqueAPI(apiDef **,char *,signatureDef *);
static int countVirtuals(classDef *);
static int skipOverload(overDef *,memberDef *,classDef *,classDef *,int);
static int compareMethTab(const void *,const void *);
static int compareEnumMembers(const void *,const void *);
static char *getSubFormatChar(char,argDef *);
static char *createIfaceFileName(char *,ifaceFileDef *,char *);
static FILE *createFile(sipSpec *,char *,char *);
static void closeFile(FILE *);
static void prcode(FILE *,char *,...);
static void prScopedName(FILE *,scopedNameDef *,char *);
static void prScopedPythonName(FILE *fp, classDef *scope, char *pyname);
static void prTypeName(FILE *,argDef *,int);
static void prScopedClassName(FILE *,classDef *,char *);
static int isZeroArgSlot(memberDef *md);
static int isMultiArgSlot(memberDef *md);
static int isIntArgSlot(memberDef *md);
static int isInplaceNumberSlot(memberDef *md);
static int isInplaceSequenceSlot(memberDef *md);
static int needErrorFlag(codeBlock *cb);
static int needNewInstance(argDef *ad);
static int needDealloc(classDef *cd);
static char getBuildResultFormat(argDef *ad);
static char getParseResultFormat(argDef *ad);
static void generateParseResultExtraArgs(argDef *ad,FILE *fp);
static char *makePartName(char *codeDir,char *mname,int part,char *srcSuffix);
static void normaliseArgs(signatureDef *);
static void restoreArgs(signatureDef *);
static const char *slotName(slotType st);
static void ints_intro(classDef *cd, FILE *fp);
static const char *argName(const char *name, codeBlock *cb);
static int usedInCode(codeBlock *code, const char *str);
static void generateDefaultValue(argDef *ad, int argnr, FILE *fp);


/*
 * Generate the code from a specification.
 */
void generateCode(sipSpec *pt,char *codeDir,char *buildfile,char *docFile,
		  char *apiFile,char *srcSuffix,int except,int trace,
		  int releaseGIL,int parts,stringList *xsl)
{
	exceptions = except;
	tracing = trace;
	release_gil = releaseGIL;
	generating_c = pt -> genc;

	if (srcSuffix == NULL)
		srcSuffix = (generating_c ? ".c" : ".cpp");

	/* Generate the documentation. */
	if (docFile != NULL)
		generateDocumentation(pt,docFile);

	/* Generate the code. */
	if (codeDir != NULL)
	{
		generateCpp(pt,codeDir,srcSuffix,&parts);
		generateInternalAPIHeader(pt,codeDir,xsl);
	}

	/* Generate the build file. */
	if (buildfile != NULL)
		generateBuildFile(pt,buildfile,srcSuffix,parts);

	/* Generate the API file. */
	if (apiFile != NULL)
		generateAPI(pt,apiFile);
}


/*
 * Generate the Scintilla API file.
 */
static void generateAPI(sipSpec *pt,char *apiFile)
{
	apiDef *head, *ad;
	overDef *od;
	classDef *cd;
	FILE *fp;

	/* Create the list of unique names/signatures. */
	head = NULL;

	for (od = pt -> overs; od != NULL; od = od -> next)
		addUniqueAPI(&head,od -> cppname,&od -> pysig);

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
	{
		ctorDef *ct;

		if (cd -> iff -> module != pt -> module)
			continue;

		for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
		{
			if (isPrivateCtor(ct))
				continue;

			addUniqueAPI(&head,classBaseName(cd),&ct -> pysig);
		}

		for (od = cd -> overs; od != NULL; od = od -> next)
		{
			if (isPrivate(od))
				continue;

			addUniqueAPI(&head,od -> cppname,&od -> pysig);
		}
	}

	/* Generate the file. */
	fp = createFile(pt,apiFile,NULL);

	for (ad = head; ad != NULL; ad = ad -> next)
	{
		fprintf(fp,"%s(",ad -> name);
		generateArgs(ad -> sd,Declaration,fp);
		fprintf(fp,")\n");
	}

	closeFile(fp);
}


/*
 * Add an API function to a list if it isn't already there.
 */
static void addUniqueAPI(apiDef **headp,char *name,signatureDef *sd)
{
	apiDef *ad;

	for (ad = *headp; ad != NULL; ad = ad -> next)
	{
		if (strcmp(ad -> name,name) != 0)
			continue;

		if (sameSignature(ad -> sd,sd,TRUE))
			break;
	}

	if (ad == NULL)
	{
		ad = sipMalloc(sizeof (apiDef));

		ad -> name = name;
		ad -> sd = sd;
		ad -> next = *headp;
		*headp = ad;
	}
}


/*
 * Generate the documentation.
 */
static void generateDocumentation(sipSpec *pt,char *docFile)
{
	FILE *fp;
	codeBlock *cb;

	fp = createFile(pt,docFile,NULL);

	for (cb = pt -> docs; cb != NULL; cb = cb -> next)
		fputs(cb -> frag,fp);

	closeFile(fp);
}


/*
 * Generate the build file.
 */
static void generateBuildFile(sipSpec *pt,char *buildFile,char *srcSuffix,
			      int parts)
{
	char *mname = pt -> module -> name;
	ifaceFileDef *iff;
	FILE *fp;

	fp = createFile(pt,buildFile,NULL);

	prcode(fp,"target = %s\nsources = ",mname);

	if (parts)
	{
		int p;

		for (p = 0; p < parts; ++p)
		{
			if (p > 0)
				prcode(fp," ");

			prcode(fp,"sip%spart%d%s",mname,p,srcSuffix);
		}
	}
	else
	{
		prcode(fp,"sip%scmodule%s",mname,srcSuffix);

		for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
		{
			if (iff -> module != pt -> module)
				continue;

			if (iff -> type == exception_iface)
				continue;

			prcode(fp," sip%s%F%s",mname,iff -> fqcname,srcSuffix);
		}
	}

	prcode(fp,"\nheaders = sipAPI%s.h",mname);

	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
	{
		char *imname;

		imname = (iff -> module == pt -> module ? mname : iff -> module -> name);

		prcode(fp," sip%s%F.h",imname,iff -> fqcname);
	}

	prcode(fp,"\n");

	closeFile(fp);
}


/*
 * Generate an expression in C++.
 */
static void generateExpression(valueDef *vd,FILE *fp)
{
	while (vd != NULL)
	{
		if (vd -> vunop != '\0')
			prcode(fp,"%c",vd -> vunop);

		switch (vd -> vtype)
		{
		case qchar_value:
			prcode(fp,"'%c'",vd -> u.vqchar);
			break;

		case string_value:
			prcode(fp,"\"%s\"",vd -> u.vstr);
			break;

		case numeric_value:
			prcode(fp,"%l",vd -> u.vnum);
			break;

		case real_value:
			prcode(fp,"%g",vd -> u.vreal);
			break;

		case scoped_value:
			prcode(fp,"%S",vd -> u.vscp);
			break;

		case fcall_value:
			generateSimpleFunctionCall(vd -> u.fcd,fp);
			break;
		}
 
		if (vd -> vbinop != '\0')
			prcode(fp," %c ",vd -> vbinop);
 
		vd = vd -> next;
	}
}


/*
 * Generate the C++ internal module API header file.
 */
static void generateInternalAPIHeader(sipSpec *pt,char *codeDir,stringList *xsl)
{
	char *hfile, *mname = pt -> module -> name;
	int noIntro;
	FILE *fp;
	nameDef *nd;
	moduleDef *mod;
	moduleListDef *mld;

	hfile = concat(codeDir,"/sipAPI",mname,".h",NULL);
	fp = createFile(pt,hfile,"Internal module API header file.");

	/* Include files. */

	prcode(fp,
"\n"
"#ifndef _%sAPI_H\n"
"#define	_%sAPI_H\n"
"\n"
"\n"
"#include <sip.h>\n"
		,mname
		,mname);

	/* Define the enabled features. */
	noIntro = TRUE;

	for (mod = pt -> modules; mod != NULL; mod = mod -> next)
	{
		qualDef *qd;

		for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
			if (qd -> qtype == feature_qualifier && !excludedFeature(xsl,qd))
			{
				if (noIntro)
				{
					prcode(fp,
"\n"
"\n"
"/* These are the features that are enabled. */\n"
						);

					noIntro = FALSE;
				}

				prcode(fp,
"#define	SIP_FEATURE_%s\n"
					,qd -> name);
			}
	}

	generateCppCodeBlock(pt -> hdrcode,fp);

	/* Shortcuts that hide the messy detail of the APIs. */
	noIntro = TRUE;

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
	{
		if (!isClassName(nd))
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"\n"
"/*\n"
" * Convenient names to refer to the names of classes defined in this module.\n"
" * These are part of the public API.\n"
" */\n"
"\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"#define	sipName_%s	%N\n"
			,nd -> text,nd);
	}

	prcode(fp,
"\n"
"\n"
"/* Convenient names to call the SIP API. */\n"
"#define	sipConvertFromSliceObject(o,len,start,stop,step,slen)	PySlice_GetIndicesEx((PySliceObject *)(o),(len),(start),(stop),(step),(slen))\n"
"#define	sipIsSubClassInstance(o,wt)	PyObject_TypeCheck((o),(PyTypeObject *)(wt))\n"
"\n"
"#define	sipMapStringToClass		sipAPI_%s -> api_map_string_to_class\n"
"#define	sipMapIntToClass		sipAPI_%s -> api_map_int_to_class\n"
"#define	sipMalloc			sipAPI_%s -> api_malloc\n"
"#define	sipFree				sipAPI_%s -> api_free\n"
"#define	sipBuildResult			sipAPI_%s -> api_build_result\n"
"#define	sipCallMethod			sipAPI_%s -> api_call_method\n"
"#define	sipParseResult			sipAPI_%s -> api_parse_result\n"
"#define	sipParseArgs			sipAPI_%s -> api_parse_args\n"
"#define	sipParsePair			sipAPI_%s -> api_parse_pair\n"
"#define	sipCommonCtor			sipAPI_%s -> api_common_ctor\n"
"#define	sipCommonDtor			sipAPI_%s -> api_common_dtor\n"
"#define	sipConvertFromSequenceIndex	sipAPI_%s -> api_convert_from_sequence_index\n"
"#define	sipConvertFromVoidPtr		sipAPI_%s -> api_convert_from_void_ptr\n"
"#define	sipConvertToCpp			sipAPI_%s -> api_convert_to_cpp\n"
"#define	sipConvertToVoidPtr		sipAPI_%s -> api_convert_to_void_ptr\n"
"#define	sipNoFunction			sipAPI_%s -> api_no_function\n"
"#define	sipNoMethod			sipAPI_%s -> api_no_method\n"
"#define	sipBadClass			sipAPI_%s -> api_bad_class\n"
"#define	sipBadSetType			sipAPI_%s -> api_bad_set_type\n"
"#define	sipBadCatcherResult		sipAPI_%s -> api_bad_catcher_result\n"
"#define	sipBadOperatorArg		sipAPI_%s -> api_bad_operator_arg\n"
"#define	sipTrace			sipAPI_%s -> api_trace\n"
"#define	sipTransfer			sipAPI_%s -> api_transfer\n"
"#define	sipTransferBack			sipAPI_%s -> api_transfer_back\n"
"#define	sipTransferTo			sipAPI_%s -> api_transfer_to\n"
"#define	sipWrapper_Check		sipAPI_%s -> api_wrapper_check\n"
"#define	sipGetWrapper			sipAPI_%s -> api_get_wrapper\n"
"#define	sipGetCppPtr			sipAPI_%s -> api_get_cpp_ptr\n"
"#define	sipGetComplexCppPtr		sipAPI_%s -> api_get_complex_cpp_ptr\n"
"#define	sipIsPyMethod			sipAPI_%s -> api_is_py_method\n"
"#define	sipMapCppToSelf			sipAPI_%s -> api_map_cpp_to_self\n"
"#define	sipMapCppToSelfSubClass		sipAPI_%s -> api_map_cpp_to_self_sub_class\n"
"#define	sipNewCppToSelf			sipAPI_%s -> api_new_cpp_to_self\n"
"#define	sipNewCppToSelfSubClass		sipAPI_%s -> api_new_cpp_to_self_sub_class\n"
"#define	sipCallHook			sipAPI_%s -> api_call_hook\n"
"#define	sipStartThread			sipAPI_%s -> api_start_thread\n"
"#define	sipEndThread			sipAPI_%s -> api_end_thread\n"
"#define	sipEmitSignal			sipAPI_%s -> api_emit_signal\n"
"#define	sipConnectRx			sipAPI_%s -> api_connect_rx\n"
"#define	sipDisconnectRx			sipAPI_%s -> api_disconnect_rx\n"
"#define	sipGetSender			sipAPI_%s -> api_get_sender\n"
"#define	sipRaiseUnknownException	sipAPI_%s -> api_raise_unknown_exception\n"
"#define	sipRaiseClassException		sipAPI_%s -> api_raise_class_exception\n"
"#define	sipRaiseSubClassException	sipAPI_%s -> api_raise_sub_class_exception\n"
"#define	sipBadLengthForSlice		sipAPI_%s -> api_bad_length_for_slice\n"
"#define	sipClassName			sipAPI_%s -> api_class_name\n"
"#define	sipAddClassInstance		sipAPI_%s -> api_add_class_instance\n"
"#define	sipConvertFromNamedEnum		sipAPI_%s -> api_convert_from_named_enum\n"
"#define	sipGetAddress			sipAPI_%s -> api_get_address\n"
"#define	sipFreeConnection		sipAPI_%s -> api_free_connection\n"
"#define	sipEmitToSlot			sipAPI_%s -> api_emit_to_slot\n"
"#define	sipSameConnection		sipAPI_%s -> api_same_connection\n"
"#define	sipPySlotExtend			sipAPI_%s -> api_pyslot_extend\n"
"#define	sipConvertRx			sipAPI_%s -> api_convert_rx\n"
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
	noIntro = TRUE;

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
	{
		if (!isUsedName(nd))
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"\n"
"/* The strings used by this module. */\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"extern char %N[];\n"
			,nd);
	}

	/* The unscoped enum macros. */
	generateEnumMacros(pt, NULL, fp);

	prcode(fp,
"\n"
"\n"
"/* The SIP API, this module's API and the APIs of any imported modules. */\n"
"extern const sipAPIDef *sipAPI_%s;\n"
"extern sipExportedModuleDef sipModuleAPI_%s;\n"
		,mname
		,mname,mname);

	for (mld = pt -> module -> imports; mld != NULL; mld = mld -> next)
		prcode(fp,
"extern const sipExportedModuleDef *sipModuleAPI_%s_%s;\n"
			,mname,mld -> module -> name);

	/*
	 * Note that we don't forward declare the virtual handlers.  This is
	 * because we would need to #include everything needed for their
	 * argument types.
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
static char *makePartName(char *codeDir,char *mname,int part,char *srcSuffix)
{
	char buf[20];

	sprintf(buf,"part%d",part);

	return concat(codeDir,"/sip",mname,buf,srcSuffix,NULL);
}


/*
 * Generate the C/C++ code.
 */
static void generateCpp(sipSpec *pt,char *codeDir,char *srcSuffix,int *parts)
{
	char *cppfile, *mname = pt -> module -> name;
	int noIntro, nrSccs = 0, files_in_part, max_per_part, this_part;
	int is_inst_class, is_inst_voidp, is_inst_char, is_inst_string;
	int is_inst_int, is_inst_long, is_inst_ulong, is_inst_longlong;
	int is_inst_ulonglong, is_inst_double, is_inst_enum, nr_enummembers;
	int hasexternal = FALSE, slot_extenders = FALSE, ctor_extenders = FALSE;
	FILE *fp;
	moduleListDef *mld;
	classDef *cd;
	memberDef *md;
	ifaceFileDef *iff;
	virtHandlerDef *vhd;
	nameDef *nd;
	exceptionDef *xd;

	/* Calculate the number of files in each part. */
	if (*parts)
	{
		int nr_files = 1;

		for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
			if (iff -> module == pt -> module)
				++nr_files;

		max_per_part = (nr_files + *parts - 1) / *parts;
		files_in_part = 1;
		this_part = 0;

		cppfile = makePartName(codeDir,mname,0,srcSuffix);
	}
	else
		cppfile = concat(codeDir,"/sip",mname,"cmodule",srcSuffix,NULL);

	fp = createFile(pt,cppfile,"Module code.");

	prcode(fp,
"\n"
"#include \"sipAPI%s.h\"\n"
"\n"
		,mname);

	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
		if (iff -> module == pt -> module && iff -> type != exception_iface)
			prcode(fp,
"#include \"sip%s%F.h\"\n"
				,iff -> module -> name,iff -> fqcname);

	generateUsedIncludes(pt->used, FALSE, fp);

	/* Define the names. */
	noIntro = TRUE;

	for (nd = pt -> namecache; nd != NULL; nd = nd -> next)
	{
		if (!isUsedName(nd))
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"\n"
"/* Define the strings used by this module. */\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"char %N[] = \"%s\";\n"
			,nd,nd -> text);
	}

	/* Generate the C++ code blocks. */
	generateCppCodeBlock(pt -> cppcode,fp);

	/* Generate any virtual handler declarations. */
	for (vhd = pt -> module -> virthandlers; vhd != NULL; vhd = vhd -> next)
		if (!isDuplicateVH(vhd))
			generateVirtualHandler(pt,vhd,fp);

	/* Generate the global functions. */
	for (md = pt -> othfuncs; md != NULL; md = md -> next)
	{
		if (md->module != pt->module)
			continue;

		if (md->slot == no_slot)
			generateOrdinaryFunction(pt,NULL,md,fp);
		else
		{
			overDef *od;

			/*
			 * Make sure that there is still an overload and we
			 * haven't moved them all to classes.
			 */
			for (od = pt->overs; od != NULL; od = od->next)
				if (od->common == md)
				{
					generateSlot(pt, NULL, NULL, md, fp);
					slot_extenders = TRUE;
					break;
				}
		}
	}

	/* Generate any class specific ctor or slot extenders. */
	for (cd = pt->proxies; cd != NULL; cd = cd->next)
	{
		/* ZZZ - need to add support for namespace extenders. */
		if (cd->iff->type == namespace_iface)
			continue;

		if (cd->ctors != NULL)
		{
			generateTypeInit(pt, cd, fp);
			ctor_extenders = TRUE;
		}

		for (md = cd->members; md != NULL; md = md->next)
		{
			generateSlot(pt, cd, NULL, md, fp);
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

		for (cd = pt->proxies; cd != NULL; cd = cd->next)
		{
			if (cd->iff->type == namespace_iface)
				continue;

			if (cd->ctors != NULL)
			{
				prcode(fp,
"	{init_%C, ", classFQCName(cd));

				generateEncodedClass(pt, cd, 0, fp);

				prcode(fp, ", NULL},\n"
					);
			}
		}

		prcode(fp,
"	{NULL, {0, 0, 0}, NULL}\n"
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

		for (md = pt->othfuncs; md != NULL; md = md->next)
		{
			overDef *od;

			if (md->module != pt->module)
				continue;

			if (md->slot == no_slot)
				continue;

			for (od = pt->overs; od != NULL; od = od->next)
				if (od->common == md)
				{
					prcode(fp,
"	{(void *)slot_%s, %s, {0, 0, 0}},\n"
						, md->pyname->text, slotName(md->slot));

					break;
				}
		}

		for (cd = pt->proxies; cd != NULL; cd = cd->next)
		{
			if (cd->iff->type == namespace_iface)
				continue;

			for (md = cd->members; md != NULL; md = md->next)
			{
				prcode(fp,
"	{(void *)slot_%C_%s, %s, ", classFQCName(cd), md->pyname->text, slotName(md->slot));

				generateEncodedClass(pt, cd, 0, fp);

				prcode(fp, "},\n"
				      );
			}
		}

		prcode(fp,
"	{NULL, (sipPySlotType)0, {0, 0, 0}}\n"
"};\n"
			);
	}

	/* Generate the global access functions. */
	generateAccessFunctions(pt,NULL,fp);

	/* Generate the module data structures. */
	if (pt -> module -> nrclasses > 0)
	{
		/* Generate the sub-class convertors. */
		for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		{
			if (cd -> iff -> module != pt -> module)
				continue;

			if (cd -> convtosubcode == NULL)
				continue;

			prcode(fp,
"\n"
"\n"
"/* Convert to a sub-class if possible. */\n"
"static sipWrapperType *sipSubClass_%C(void **sipCppRet)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(*sipCppRet);\n"
"	sipWrapperType *sipClass;\n"
"\n"
				,classFQCName(cd)
				,classFQCName(cd -> subbase),classFQCName(cd -> subbase));

			generateCppCodeBlock(cd -> convtosubcode,fp);

			prcode(fp,
"\n"
"	return sipClass;\n"
"}\n"
				);

			++nrSccs;
		}

		prcode(fp,
"\n"
"\n"
"/*\n"
" * This defines each class in this module.  The values are replaced by the\n"
" * proper Python types during the export process.\n"
" */\n"
"static sipWrapperType *typesTable[] = {\n"
			);

		generateClassTableEntries(&pt -> module -> root,fp);

		prcode(fp,
"};\n"
			);

		/* Generate the external classes table if needed. */
		for (cd = pt->classes; cd != NULL; cd = cd->next)
		{
			if (!isExternal(cd))
				continue;

			if (cd->iff->module != pt->module)
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
"	{%d, \"", cd->classnr);
			prScopedName(fp, classFQCName(cd), ".");
			prcode(fp,"\"},\n"
				);
		}

		if (hasexternal)
			prcode(fp,
"	{-1, NULL}\n"
"};\n"
				);
	}

	if (pt -> module -> nrmappedtypes > 0)
	{
		mappedTypeDef *mtd;
		argDef type;

		type.argflags = 0;
		type.name = NULL;
		type.nrderefs = 0;
		type.defval = NULL;

		prcode(fp,
"\n"
"\n"
"/* This defines each mapped type in this module. */\n"
"static sipMappedTypeDef *mappedTypesTable[] = {\n"
			);

		for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		{
			if (mtd -> iff -> module != pt -> module)
				continue;

			type.atype = mapped_type;
			type.u.mtd = mtd;

			prcode(fp,
"	&sipMappedType_%T,\n"
				,&type);
		}

		prcode(fp,
"	0\n"
"};\n"
			);
	}

	if (pt -> module -> nrenums > 0)
	{
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

			if (ed->module != pt->module || ed->fqcname == NULL)
				continue;

			if (ed->slots == NULL)
				continue;

			for (slot = ed->slots; slot != NULL; slot = slot->next)
				generateSlot(pt, NULL, ed, slot, fp);

			prcode(fp,
"\n"
"static sipPySlotDef slots_%C[] = {\n"
				, ed->fqcname);

			for (slot = ed->slots; slot != NULL; slot = slot->next)
			{
				const char *stype;

				if ((stype = slotName(slot->slot)) != NULL)
					prcode(fp,
"	{(void *)slot_%C_%s, %s},\n"
						, ed->fqcname, slot->pyname->text, stype);
			}

			prcode(fp,
"	{0, (sipPySlotType)0}\n"
"};\n"
"\n"
				);
		}

		prcode(fp,
"static sipEnumDef enumTypesTable[] = {\n"
			);

		for (ed = pt -> enums; ed != NULL; ed = ed -> next)
		{
			char *emname;

			if (ed -> module != pt -> module || ed -> fqcname == NULL)
				continue;

			/*
			 * Take the module name from the enclosing scope in
			 * case it is a proxy namespace.
			 */
			if (ed->ecd != NULL)
				emname = ed->ecd->iff->module->name;
			else
				emname = mname;

			prcode(fp,
"	{\"%s.%P\", ", emname, ed->ecd, ed->pyname->text);

			if (isRenamedEnum(ed) || (ed -> ecd != NULL && isRenamedClass(ed -> ecd)))
				prcode(fp, "\"%S\", ", ed -> fqcname);
			else
				prcode(fp, "NULL, ");

			if (ed->ecd != NULL)
				generateEncodedClass(pt, ed->ecd, 0, fp);
			else
				prcode(fp, "{0, 0, 1}");

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

		nr_enummembers = generateEnumMemberTable(pt,NULL,fp);
	}
	else
		nr_enummembers = 0;

	if (pt -> module -> nrtypedefs > 0)
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

		for (td = pt -> typedefs; td != NULL; td = td -> next)
		{
			char *mname, *sat;
			scopedNameDef *fqname;

			if (td -> module != pt -> module)
				continue;

			fqname = NULL;
			mname = NULL;

			switch (td -> type.atype)
			{
			case string_type:
				sat = (td -> type.nrderefs == 0 ? "char" : "string");
				break;

			case ustring_type:
				sat = (td -> type.nrderefs == 0 ? "uchar" : "ustring");
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
				sat = "bool";
				break;

			case void_type:
				if (td -> type.nrderefs != 0)
					sat = "void";
				break;

			case enum_type:
				if ((fqname = td -> type.u.ed -> fqcname) != NULL)
				{
					sat = "enum";

					if (td -> type.u.ed -> module != pt -> module)
						mname = td -> type.u.ed -> module -> name;
				}
				break;

			case class_type:
				sat = "class";
				fqname = classFQCName(td -> type.u.cd);

				if (td -> type.u.cd -> iff -> module != pt -> module)
					mname = td -> type.u.cd -> iff -> module -> name;
				break;

			case mapped_type:
				sat = "mtype";
				fqname = td -> type.u.mtd -> iff -> fqcname;

				if (td -> type.u.mtd -> iff -> module != pt -> module)
					mname = td -> type.u.mtd -> iff -> module -> name;
				break;

			default:
				sat = "unknown";
			}

			prcode(fp,
"	{\"%S\", %s_sat", td -> fqname, sat);

			if (fqname != NULL)
				prcode(fp, ", \"%S\"", fqname);
			else
				prcode(fp, ", NULL");

			if (mname != NULL)
				prcode(fp, ", \"%s\"", mname);
			else
				prcode(fp, ", NULL");

			prcode(fp, "},\n"
				);
		}

		prcode(fp,
"	{NULL, unknown_sat, NULL, NULL}\n"
"};\n"
			);
	}

	if (pt -> module -> nrvirthandlers > 0)
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

		for (vhd = pt -> module -> virthandlers; vhd != NULL; vhd = vhd -> next)
			if (!isDuplicateVH(vhd))
				prcode(fp,
"	(sipVirtHandlerFunc)sipVH_%s_%d,\n"
					,mname,vhd -> virthandlernr);

		prcode(fp,
"};\n"
			);
	}

	if (pt -> module -> imports != NULL)
	{
		prcode(fp,
"\n"
"\n"
"/* This defines the modules that this module needs to import. */\n"
"static sipImportedModuleDef importsTable[] = {\n"
			);

		for (mld = pt -> module -> imports; mld != NULL; mld = mld -> next)
			prcode(fp,
"	{\"%s\", %d, NULL},\n"
				, mld->module->fullname, mld->module->version);

		prcode(fp,
"	{NULL, -1, NULL}\n"
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

		for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		{
			if (cd -> iff -> module != pt -> module)
				continue;

			if (cd -> convtosubcode == NULL)
				continue;

			prcode(fp,
"	{sipSubClass_%C, ",classFQCName(cd));

			generateEncodedClass(pt,cd -> subbase,0,fp);

			prcode(fp,", NULL},\n");
		}

		prcode(fp,
"	{NULL, {0, 0, 0}, NULL}\n"
"};\n"
			);
	}

	/* Generate any license information. */
	if (pt -> module -> license != NULL)
	{
		licenseDef *ld = pt -> module -> license;

		prcode(fp,
"\n"
"\n"
"/* Define the module's license. */\n"
"static sipLicenseDef module_license = {\n"
			);

		prcode(fp,
"	\"%s\",\n"
			,ld -> type);

		if (ld -> licensee != NULL)
			prcode(fp,
"	\"%s\",\n"
				,ld -> licensee);
		else
			prcode(fp,
"	NULL,\n"
				);

		if (ld -> timestamp != NULL)
			prcode(fp,
"	\"%s\",\n"
				,ld -> timestamp);
		else
			prcode(fp,
"	NULL,\n"
				);

		if (ld -> sig != NULL)
			prcode(fp,
"	\"%s\"\n"
				,ld -> sig);
		else
			prcode(fp,
"	NULL\n"
				);

		prcode(fp,
"};\n"
			);
	}

	/* Generate each instance table. */
	is_inst_class = generateClasses(pt,NULL,fp);
	is_inst_voidp = generateVoidPointers(pt,NULL,fp);
	is_inst_char = generateChars(pt,NULL,fp);
	is_inst_string = generateStrings(pt,NULL,fp);
	is_inst_int = generateInts(pt,NULL,fp);
	is_inst_long = generateLongs(pt,NULL,fp);
	is_inst_ulong = generateUnsignedLongs(pt,NULL,fp);
	is_inst_longlong = generateLongLongs(pt,NULL,fp);
	is_inst_ulonglong = generateUnsignedLongLongs(pt,NULL,fp);
	is_inst_double = generateDoubles(pt,NULL,fp);
	is_inst_enum = generateEnums(pt,NULL,fp);

	/* Generate any exceptions table. */
	if (pt->module->nrexceptions > 0)
	       prcode(fp,
"\n"
"\n"
"static PyObject *exceptionsTable[%d];\n"
			,pt->module->nrexceptions);

	/* Generate any Qt support API. */
	if (pt->qobjclass >= 0)
		prcode(fp,
"\n"
"\n"
"/* This defines the Qt support API. */\n"
"static sipQtAPI qtAPI = {\n"
"	&typesTable[%d],\n"
"	sipQtCreateUniversalSlot,\n"
"	sipQtDestroyUniversalSlot,\n"
"	sipQtFindSlot,\n"
"	sipQtConnect,\n"
"	sipQtDisconnect,\n"
"	sipQtSignalsBlocked,\n"
"	sipQtGetSender,\n"
"	sipQtForgetSender,\n"
"	sipQtIsSignal\n"
"};\n"
			,pt->qobjclass);

	prcode(fp,
"\n"
"\n"
"/* This defines this module. */\n"
"sipExportedModuleDef sipModuleAPI_%s = {\n"
"	NULL,\n"
"	SIP_API_MINOR_NR,\n"
"	\"%s\",\n"
"	NULL,\n"
"	%d,\n"
"	%s,\n"
"	%s,\n"
"	%d,\n"
"	%s,\n"
"	%s,\n"
"	%s,\n"
"	%d,\n"
"	NULL,\n"
"	%s,\n"
"	%d,\n"
"	%s,\n"
"	%s,\n"
"	%s,\n"
"	%s,\n"
"	{%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s},\n"
"	%s,\n"
"	%s,\n"
"	%s,\n"
"	%s\n"
"};\n"
		,mname
		, pt->module->fullname
		,pt -> module -> version
		,(pt -> module -> imports != NULL ? "importsTable" : "NULL")
		,(pt -> qobjclass >= 0 ? "&qtAPI" : "NULL")
		,pt -> module -> nrclasses
		,(pt -> module -> nrclasses > 0 ? "typesTable" : "NULL")
		, (hasexternal ? "externalTypesTable" : "NULL")
		,(pt -> module -> nrmappedtypes > 0 ? "mappedTypesTable" : "NULL")
		,pt -> module -> nrenums
		,(pt -> module -> nrenums > 0 ? "enumTypesTable" : "NULL")
		,nr_enummembers
		,(nr_enummembers > 0 ? "enummembers" : "NULL")
		,(pt -> module -> nrtypedefs > 0 ? "typedefsTable" : "NULL")
		,(pt -> module -> nrvirthandlers > 0 ? "virtHandlersTable" : "NULL")
		,(nrSccs > 0 ? "convertorsTable" : "NULL")
		,(is_inst_class ? "classInstances" : "NULL")
		,(is_inst_voidp ? "voidPtrInstances" : "NULL")
		,(is_inst_char ? "charInstances" : "NULL")
		,(is_inst_string ? "stringInstances" : "NULL")
		,(is_inst_int ? "intInstances" : "NULL")
		,(is_inst_long ? "longInstances" : "NULL")
		,(is_inst_ulong ? "unsignedLongInstances" : "NULL")
		,(is_inst_longlong ? "longLongInstances" : "NULL")
		,(is_inst_ulonglong ? "unsignedLongLongInstances" : "NULL")
		,(is_inst_double ? "doubleInstances" : "NULL")
		,(is_inst_enum ? "enumInstances" : "NULL")
		,(pt -> module -> license != NULL ? "&module_license" : "NULL")
		,(pt -> module -> nrexceptions > 0 ? "exceptionsTable" : "NULL")
		, (slot_extenders ? "slotExtenders" : "NULL")
		, (ctor_extenders ? "initExtenders" : "NULL"));

	/* Generate the storage for the external API pointers. */
	prcode(fp,
"\n"
"\n"
"/* The SIP API and the APIs of any imported modules. */\n"
"const sipAPIDef *sipAPI_%s;\n"
		,mname);

	for (mld = pt -> module -> imports; mld != NULL; mld = mld -> next)
		prcode(fp,
"const sipExportedModuleDef *sipModuleAPI_%s_%s;\n"
			,mname,mld -> module -> name);

	/* Generate the Python module initialisation function. */
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
"	static PyMethodDef sip_methods[] = {\n"
		,(generating_c ? "" : "extern \"C\" "), mname
		,mname);

	/* Generate the global functions. */

	for (md = pt -> othfuncs; md != NULL; md = md -> next)
		if (md -> module == pt -> module && md -> slot == no_slot)
			prcode(fp,
"		{%N, func_%s, METH_VARARGS, NULL},\n"
				,md -> pyname,md -> pyname -> text);

	prcode(fp,
"		{0, 0, 0, 0}\n"
"	};\n"
"\n"
"	PyObject *sip_mod, *sip_mdict, *sip_sipmod, *sip_capiobj;\n"
"\n"
		);

	/* Generate the pre-initialisation code. */
	generateCppCodeBlock(pt -> preinitcode,fp);

	prcode(fp,
"	/* Initialise the module and get it's dictionary. */\n"
"	sip_mod = Py_InitModule(sipModuleAPI_%s.em_name,sip_methods);\n"
"	sip_mdict = PyModule_GetDict(sip_mod);\n"
"\n"
"	/* Import the SIP module and get it's API. */\n"
"	sip_sipmod = PyImport_ImportModule(\"sip\");\n"
"\n"
"	if (sip_sipmod == NULL)\n"
"		return;\n"
"\n"
"	sip_capiobj = PyDict_GetItemString(PyModule_GetDict(sip_sipmod),\"_C_API\");\n"
"\n"
"	if (sip_capiobj == NULL || !PyCObject_Check(sip_capiobj))\n"
"		return;\n"
"\n"
		,mname);

	if (generating_c)
		prcode(fp,
"	sipAPI_%s = (const sipAPIDef *)PyCObject_AsVoidPtr(sip_capiobj);\n"
		,mname);
	else
		prcode(fp,
"	sipAPI_%s = reinterpret_cast<const sipAPIDef *>(PyCObject_AsVoidPtr(sip_capiobj));\n"
		,mname);

	prcode(fp,
"\n"
"	/* Export the module and publish it's API. */\n"
"	if (sipAPI_%s -> api_export_module(&sipModuleAPI_%s,SIP_API_MAJOR_NR,SIP_API_MINOR_NR,sip_mdict) < 0)\n"
"		return;\n"
		,mname
		,mname);

	noIntro = TRUE;

	for (mld = pt -> module -> imports; mld != NULL; mld = mld -> next)
	{
		if (noIntro)
		{
			prcode(fp,
"\n"
"	/* Get the APIs of the modules that this one is dependent on. */\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"	sipModuleAPI_%s_%s = sipModuleAPI_%s.em_imports[%d].im_module;\n"
			,mname,mld -> module -> name,mname,mld -> module -> modulenr);
	}

	generateClassesInline(pt,fp);

	/* Create any exceptions. */
	for (xd = pt->exceptions; xd != NULL; xd = xd->next)
	{
		if (xd->iff->module != pt->module)
			continue;

		if (xd->iff->type != exception_iface)
			continue;

		if (xd->exceptionnr < 0)
			continue;

		prcode(fp,
"\n"
"	if ((exceptionsTable[%d] = PyErr_NewException(\"%s.%s\",", xd->exceptionnr, xd->iff->module->name, xd->pyname);

		if (xd->bibase != NULL)
			prcode(fp, "PyExc_%s", xd->bibase);
		else if (xd->base->iff->module == pt->module)
			prcode(fp, "exceptionsTable[%d]", xd->base->exceptionnr);
		else
			prcode(fp, "sipException_%C", xd->base->iff->fqcname);

		prcode(fp, ",NULL)) == NULL || PyDict_SetItemString(sip_mdict,\"%s\",exceptionsTable[%d]) < 0)\n"
"		return;\n"
			, xd->pyname, xd->exceptionnr);
	}

	/* Generate the post-initialisation code. */
	generateCppCodeBlock(pt -> postinitcode,fp);

	prcode(fp,
"}\n"
		);

	/* Generate the interface source and header files. */
	for (iff = pt -> ifacefiles; iff != NULL; iff = iff -> next)
	{
		if (iff -> module == pt -> module && iff -> type != exception_iface)
		{
			if (*parts && files_in_part++ == max_per_part)
			{
				/* Close the old part. */
				closeFile(fp);
				free(cppfile);

				/* Create a new one. */
				files_in_part = 1;
				++this_part;

				cppfile = makePartName(codeDir,mname,this_part,srcSuffix);
				fp = createFile(pt,cppfile,"Module code.");
			}

			generateIfaceCpp(pt,iff,codeDir,srcSuffix,(*parts ? fp : NULL));
		}

		generateIfaceHeader(pt,iff,codeDir);
	}

	closeFile(fp);
	free(cppfile);

	/* How many parts we actually generated. */
	if (*parts)
		*parts = this_part + 1;
}


/*
 * Generate an entry for a class in the classes table and all its children.
 */
static void generateClassTableEntries(nodeDef *nd,FILE *fp)
{
	nodeDef *cnd;

	/* Generate the entry if it's not the root. */
	if (nd -> cd != NULL)
		if (isExternal(nd->cd))
			prcode(fp,
"	0,\n"
				);
		else
			prcode(fp,
"	(sipWrapperType *)&sipType_%C,\n"
				,classFQCName(nd -> cd));

	/* Generate all it's children. */
	for (cnd = nd -> child; cnd != NULL; cnd = cnd -> next)
		generateClassTableEntries(cnd,fp);
}


/*
 * Generate the structure representing an encoded class.
 */
static void generateEncodedClass(sipSpec *pt,classDef *cd,int last,FILE *fp)
{
	moduleDef *mod = cd -> iff -> module;

	prcode(fp,"{%u, ",cd -> classnr);

	if (mod == pt -> module)
		prcode(fp,"255");
	else if (mod -> modulenr >= 0)
		prcode(fp,"%u",mod -> modulenr);
	else
		fatal("The %s module must explicitly %%Import the %s module\n",pt -> module -> name,mod -> name);

	prcode(fp,", %u}",last);
}


/*
 * Generate an ordinary function (ie. not a class method).
 */
static void generateOrdinaryFunction(sipSpec *pt,classDef *cd,memberDef *md,
				     FILE *fp)
{
	overDef *od;

	prcode(fp,
"\n"
"\n"
		);

	if (cd != NULL)
	{
		prcode(fp,
"static PyObject *meth_%C_%s(PyObject *,PyObject *sipArgs)\n"
			,classFQCName(cd),md -> pyname -> text);

		od = cd -> overs;
	}
	else
	{
		prcode(fp,
"static PyObject *func_%s(PyObject *%s,PyObject *sipArgs)\n"
			,md -> pyname -> text,(generating_c ? "sipSelf" : ""));

		od = pt -> overs;
	}

	prcode(fp,
"{\n"
"	int sipArgsParsed = 0;\n"
		);

	while (od != NULL)
	{
		if (od -> common == md)
			generateFunctionBody(pt,od,cd,cd,fp);

		od = od -> next;
	}

	prcode(fp,
"\n"
"	/* Raise an exception if the arguments couldn't be parsed. */\n"
"	sipNoFunction(sipArgsParsed,%N);\n"
"\n"
"	return NULL;\n"
"}\n"
		,md -> pyname);
}


/*
 * Generate the table of enum members for a scope.  Return the number of them.
 */
static int generateEnumMemberTable(sipSpec *pt,classDef *cd,FILE *fp)
{
	int i, nr_members;
	enumDef *ed;
	enumMemberDef **etab, **et;

	/* First we count how many. */

	nr_members = 0;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		enumMemberDef *emd;

		if (ed -> ecd != cd || ed -> module != pt -> module)
			continue;

		if (cd == NULL && ed -> fqcname == NULL)
			continue;

		for (emd = ed -> members; emd != NULL; emd = emd -> next)
			++nr_members;
	}

	if (nr_members == 0)
		return 0;

	/* Create a table so they can be sorted. */

	etab = sipMalloc(sizeof (enumMemberDef *) * nr_members);

	et = etab;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		enumMemberDef *emd;

		if (ed -> ecd != cd || ed -> module != pt -> module)
			continue;

		if (cd == NULL && ed -> fqcname == NULL)
			continue;

		for (emd = ed -> members; emd != NULL; emd = emd -> next)
			*et++ = emd;
	}

	qsort(etab,nr_members,sizeof (enumMemberDef *),compareEnumMembers);

	/* Now generate the table. */

	if (cd != NULL)
		prcode(fp,
"\n"
"static sipEnumMemberDef enummembers_%C[] = {\n"
			,classFQCName(cd));
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
"	{%N, ",emd -> pyname);

		if (cd != NULL)
		{
			if (isProtectedEnum(emd -> ed))
				prcode(fp,"sip");

			prcode(fp,"%S::%s",classFQCName(cd),emd -> cname);
		}
		else
			prcode(fp,"%s"
				,emd -> cname);

		prcode(fp, ", %d},\n", emd -> ed -> enumnr);
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
	return strcmp((*(enumMemberDef **)m1) -> pyname -> text,
		      (*(enumMemberDef **)m2) -> pyname -> text);
}


/*
 * Generate the access functions for the variables.
 */
static void generateAccessFunctions(sipSpec *pt,classDef *cd,FILE *fp)
{
	varDef *vd;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> accessfunc == NULL)
			continue;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		prcode(fp,
"\n"
"\n"
"/* Access function. */\n"
"static void *access_%C()\n"
"{\n"
			,vd -> fqcname);

		generateCppCodeBlock(vd -> accessfunc,fp);

		prcode(fp,
"}\n"
			);
	}
}


/*
 * Generate the inline code to add a set of class instances to a dictionary.
 */
static void generateClassesInline(sipSpec *pt,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != class_type)
			continue;

		if (needsHandler(vd))
			continue;

		/* Skip classes that don't need inline code. */
		if (generating_c || vd -> accessfunc != NULL || vd -> type.nrderefs != 0)
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"	/* Define the class instances that have to be added inline. */\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"	sipAddClassInstance(");

		if (vd -> ecd == NULL)
			prcode(fp,"sip_mdict");
		else
			prcode(fp,"(PyObject *)sipClass_%C",classFQCName(vd -> ecd));

		prcode(fp,",%N,",vd -> pyname);

		if (isConstArg(&vd -> type))
			prcode(fp,"const_cast<%b *>(&%S)",&vd -> type,vd -> fqcname);
		else
			prcode(fp,"&%S",vd -> fqcname);

		prcode(fp,",sipClass_%C);\n"
			,classFQCName(vd -> type.u.cd));
	}
}


/*
 * Generate the code to add a set of class instances to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateClasses(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		scopedNameDef *vcname;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != class_type)
			continue;

		if (needsHandler(vd))
			continue;

		/*
		 * Skip ordinary C++ class instances which need to be done with
		 * inline code rather than through a static table.  This is
		 * because C++ does not guarantee the order in which the table
		 * and the instance will be created.  So far this has only been
		 * seen to be a problem when statically linking SIP generated
		 * modules on Windows.
		 */
		if (!generating_c && vd -> accessfunc == NULL && vd -> type.nrderefs == 0)
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"\n"
"/* Define the class instances to be added to this type dictionary. */\n"
"static sipClassInstanceDef classInstances_%C[] = {\n"
					,classFQCName(cd));
			else
				prcode(fp,
"\n"
"\n"
"/* Define the class instances to be added to this module dictionary. */\n"
"static sipClassInstanceDef classInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		vcname = classFQCName(vd -> type.u.cd);

		if (vd -> accessfunc != NULL)
		{
			prcode(fp,
"	{%N, (void *)access_%C, &sipClass_%C, SIP_ACCFUNC},\n"
				,vd -> pyname,vd -> fqcname,vcname);
		}
		else if (vd -> type.nrderefs != 0)
		{
			prcode(fp,
"	{%N, &%S, &sipClass_%C, SIP_INDIRECT},\n"
				,vd -> pyname,vd -> fqcname,vcname);
		}
		else if (isConstArg(&vd -> type))
		{
			prcode(fp,
"	{%N, const_cast<%b *>(&%S), &sipClass_%C, 0},\n"
				,vd -> pyname,&vd -> type,vd -> fqcname,vcname);
		}
		else
		{
			prcode(fp,
"	{%N, &%S, &sipClass_%C, 0},\n"
				,vd -> pyname,vd -> fqcname,vcname);
		}
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0, 0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of void pointers to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateVoidPointers(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != void_type && vd -> type.atype != struct_type)
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
					,classFQCName(cd));
			else
				prcode(fp,
"\n"
"\n"
"/* Define the void pointers to be added to this module dictionary. */\n"
"static sipVoidPtrInstanceDef voidPtrInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of characters to a dictionary.  Return TRUE
 * if there was at least one.
 */
static int generateChars(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!((vtype == ustring_type || vtype == string_type) && vd -> type.nrderefs == 0))
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
					,classFQCName(cd));
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
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of strings to a dictionary.  Return TRUE if
 * there is at least one.
 */
static int generateStrings(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!((vtype == ustring_type || vtype == string_type) && vd -> type.nrderefs != 0))
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
					,classFQCName(cd));
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
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of enum instances to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateEnums(sipSpec *pt, classDef *cd, FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (vd -> type.atype != enum_type || vd -> type.u.ed -> fqcname == NULL)
			continue;

		if (needsHandler(vd))
			continue;

		if (noIntro)
		{
			if (cd != NULL)
				prcode(fp,
"\n"
"\n"
"/* Define the enum instances to be added to this type dictionary. */\n"
"static sipEnumInstanceDef enumInstances_%C[] = {\n"
					,classFQCName(cd));
			else
				prcode(fp,
"\n"
"\n"
"/* Define the enum instances to be added to this module dictionary. */\n"
"static sipEnumInstanceDef enumInstances[] = {\n"
					);

			noIntro = FALSE;
		}

		prcode(fp,
"	{%N, (int)%S, &sipEnum_%C},\n"
			,vd -> pyname,vd -> fqcname,vd -> type.u.ed -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of ints to a dictionary.  Return TRUE if
 * there was at least one.
 */
static int generateInts(sipSpec *pt, classDef *cd, FILE *fp)
{
	int noIntro;
	varDef *vd;
	enumDef *ed;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
			continue;

		if (!(vtype == enum_type || vtype == ushort_type || vtype == short_type || vtype == uint_type || vtype == cint_type || vtype == int_type || vtype == bool_type))
			continue;

		if (needsHandler(vd))
			continue;

		/* Named enums are handled elsewhere. */
		if (vtype == enum_type && vd -> type.u.ed -> fqcname != NULL)
			continue;

		if (noIntro)
		{
			ints_intro(cd, fp);
			noIntro = FALSE;
		}

		prcode(fp,
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	/* Now do global anonymous enums. */
	if (cd == NULL)
		for (ed = pt->enums; ed != NULL; ed = ed->next)
		{
			enumMemberDef *em;

			if (ed->ecd != cd || ed->module != pt->module)
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
"	{%N, %s},\n"
					, em->pyname, em->cname);
			}
		}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
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
static int generateLongs(sipSpec *pt, classDef *cd, FILE *fp)
{
	return generateVariableType(pt, cd, long_type, "long", "Long", "long", fp);
}


/*
 * Generate the code to add a set of unsigned longs to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateUnsignedLongs(sipSpec *pt, classDef *cd, FILE *fp)
{
	return generateVariableType(pt, cd, ulong_type, "unsigned long", "UnsignedLong", "unsignedLong", fp);
}


/*
 * Generate the code to add a set of long longs to a dictionary.  Return TRUE
 * if there was at least one.
 */
static int generateLongLongs(sipSpec *pt, classDef *cd, FILE *fp)
{
	return generateVariableType(pt, cd, longlong_type, "long long", "LongLong", "longLong", fp);
}


/*
 * Generate the code to add a set of unsigned long longs to a dictionary.
 * Return TRUE if there was at least one.
 */
static int generateUnsignedLongLongs(sipSpec *pt, classDef *cd, FILE *fp)
{
	return generateVariableType(pt, cd, ulonglong_type, "unsigned long long", "UnsignedLongLong", "unsignedLongLong", fp);
}


/*
 * Generate the code to add a set of a particular type to a dictionary.  Return
 * TRUE if there was at least one.
 */
static int generateVariableType(sipSpec *pt, classDef *cd, argType atype, const char *eng, const char *s1, const char *s2, FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
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
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the code to add a set of doubles to a dictionary.  Return TRUE if
 * there was at least one.
 */
static int generateDoubles(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro;
	varDef *vd;

	noIntro = TRUE;

	for (vd = pt -> vars; vd != NULL; vd = vd -> next)
	{
		argType vtype = vd -> type.atype;

		if (vd -> ecd != cd || vd -> module != pt -> module)
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
					,classFQCName(cd));
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
"	{%N, %S},\n"
			,vd -> pyname,vd -> fqcname);
	}

	if (!noIntro)
		prcode(fp,
"	{0, 0}\n"
"};\n"
			);

	return !noIntro;
}


/*
 * Generate the C/C++ code for an interface.
 */
static void generateIfaceCpp(sipSpec *pt,ifaceFileDef *iff,char *codeDir,
			     char *srcSuffix,FILE *master)
{
	char *cppfile, *cmname = iff -> module -> name;
	classDef *cd;
	mappedTypeDef *mtd;
	FILE *fp;

	if (master == NULL)
	{
		cppfile = createIfaceFileName(codeDir,iff,srcSuffix);
		fp = createFile(pt,cppfile,"Interface wrapper code.");
	}
	else
		fp = master;

	prcode(fp,
"\n"
"#include \"sipAPI%s.h\"\n"
"#include \"sip%s%F.h\"\n"
		,cmname
		,cmname,iff -> fqcname);

	generateUsedIncludes(iff->used, FALSE, fp);

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff == iff)
		{
			if (isProtectedClass(cd))
				prcode(fp,
"\n"
"#include \"sip%s%F.h\"\n"
					,cmname,cd -> ecd -> iff -> fqcname);

			if (!isExternal(cd))
				generateClassCpp(cd,pt,fp);
		}

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (mtd -> iff == iff)
			generateMappedTypeCpp(mtd,fp);

	if (master == NULL)
	{
		closeFile(fp);
		free(cppfile);
	}
}


/*
 * Return a filename for an interface C++ or header file on the heap.
 */
static char *createIfaceFileName(char *codeDir,ifaceFileDef *iff,char *suffix)
{
	char *fn;
	scopedNameDef *snd;

	fn = concat(codeDir,"/sip",iff -> module -> name,NULL);

	for (snd = iff -> fqcname; snd != NULL; snd = snd -> next)
		append(&fn,snd -> name);

	append(&fn,suffix);

	return fn;
}


/*
 * Generate the C++ code for a mapped type version.
 */
static void generateMappedTypeCpp(mappedTypeDef *mtd,FILE *fp)
{
	prcode(fp,
"\n"
		);

	generateConvertToDefinitions(mtd,NULL,fp);

	/* Generate the from type convertor. */

	prcode(fp,
"\n"
"\n"
"static PyObject *convertFrom_%T(void *sipCppV)\n"
"{\n"
"	%b *sipCpp = reinterpret_cast<%b *>(sipCppV);\n"
"\n"
		,&mtd -> type
		,&mtd -> type,&mtd -> type);

	generateCppCodeBlock(mtd -> convfromcode,fp);

	prcode(fp,
"}\n"
"\n"
"\n"
"sipMappedTypeDef sipMappedType_%T = {\n"
"	\"%B\",\n"
"	forceConvertTo_%T,\n"
"	convertTo_%T,\n"
"	convertFrom_%T\n"
"};\n"
		,&mtd -> type
		,&mtd -> type
		,&mtd -> type
		,&mtd -> type
		,&mtd -> type);
}


/*
 * Generate the C++ code for a class.
 */
static void generateClassCpp(classDef *cd,sipSpec *pt,FILE *fp)
{
	varDef *vd;

	/* Generate any local class code. */

	generateCppCodeBlock(cd -> cppcode,fp);

	generateClassFunctions(pt,cd,fp);

	generateAccessFunctions(pt,cd,fp);

	/* Generate the variable handlers. */
	if (hasVarHandlers(cd))
	{
		for (vd = pt -> vars; vd != NULL; vd = vd -> next)
			if (vd -> ecd == cd && needsHandler(vd))
				generateVariableHandler(vd,fp);

		/* Generate the variable table. */
		prcode(fp,
"\n"
"PyMethodDef variables_%C[] = {\n"
			,classFQCName(cd));

		for (vd = pt -> vars; vd != NULL; vd = vd -> next)
			if (vd -> ecd == cd && needsHandler(vd))
				prcode(fp,
"	{%N, var_%C, %s, NULL},\n"
					,vd -> pyname,vd -> fqcname,(isStaticVar(vd) ? "METH_STATIC" : "0"));

		prcode(fp,
"	{0, 0, 0, 0}\n"
"};\n"
			);
	}

	if (cd -> iff -> type != namespace_iface)
		generateConvertToDefinitions(NULL,cd,fp);

	/* The type definition structure. */
	generateTypeDefinition(pt,cd,fp);
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

	for (md = cd -> members; md != NULL; md = md -> next)
		++nr;

	if ((*nrp = nr) == 0)
		return NULL;

	/* Create the table of methods. */

	mtab = sipMalloc(sizeof (sortedMethTab) * nr);

	/* Initialise the table. */

	mt = mtab;

	for (md = cd -> members; md != NULL; md = md -> next)
	{
		mt -> md = md;
		mt -> is_static = TRUE;

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
static sortedMethTab *createMethodTable(classDef *cd,int *nrp)
{
	int nr;
	visibleList *vl;
	sortedMethTab *mtab, *mt;

	/*
	 * First we need to count the number of applicable methods.  Only
	 * provide an entry point if there is at least one overload that is
	 * defined in this class and is a non-abstract function or slot.  We
	 * allow private (even though we don't actually generate code) because
	 * we need to intercept the name before it reaches a more public
	 * version further up the class hierarchy.  We add the ctor and any
	 * variable handlers as special entries.
	 */
	nr = 0;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			/*
			 * Skip protected methods if we don't have the means to
			 * handle them.
			 */
			if (isProtected(od) && !hasShadow(cd))
				continue;

			if (skipOverload(od,vl -> m,cd,vl -> cd,TRUE))
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

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		int need_method, is_static;
		overDef *od;

		need_method = FALSE;
		is_static = TRUE;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			/*
			 * Skip protected methods if we don't have the means to
			 * handle them.
			 */
			if (isProtected(od) && !hasShadow(cd))
				continue;

			if (!skipOverload(od,vl -> m,cd,vl -> cd,TRUE))
			{
				need_method = TRUE;

				if (!isPrivate(od) && !isStatic(od))
					is_static = FALSE;
			}
		}

		if (need_method)
		{
			mt -> md = vl -> m;
			mt -> is_static = is_static;

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
	return strcmp(((sortedMethTab *)m1) -> md -> pyname -> text,
		      ((sortedMethTab *)m2) -> md -> pyname -> text);
}


/*
 * Generate the sorted table of methods and return the number of entries.
 */
static int generateMethodTable(classDef *cd,FILE *fp)
{
	int nr;
	sortedMethTab *mtab;

	mtab = (cd -> iff -> type == namespace_iface) ?
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
"	{%N, meth_%C_%s, METH_VARARGS%s, NULL}%s\n"
				,md -> pyname,classFQCName(cd),md -> pyname -> text,(mtab[i].is_static ? "|METH_STATIC" : ""),((i + 1) < nr) ? "," : "");
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

	if (cd != NULL)
	{
		convtocode = cd -> convtocode;

		iff = cd -> iff;

		type.atype = class_type;
		type.u.cd = cd;
	}
	else
	{
		convtocode = mtd -> convtocode;

		iff = mtd -> iff;

		type.atype = mapped_type;
		type.u.mtd = mtd;
	}

	type.argflags = 0;
	type.name = NULL;
	type.nrderefs = 0;
	type.defval = NULL;

	/* Generate the type convertors. */

	if (convtocode != NULL)
	{
		int need_ptr;

		/*
		 * Sometimes type convertors are just stubs that set the error
		 * flag, so check if we actually need everything so that we
		 * can avoid compiler warnings.
		 */
		need_ptr = usedInCode(convtocode, "sipCppPtr");

		prcode(fp,
"\n"
"\n"
"static int convertTo_%T(PyObject *sipPy,void **%s,int *sipIsErr)\n"
"{\n"
			,&type,(need_ptr ? "sipCppPtrV" : ""));

		if (need_ptr)
			prcode(fp,
"	%b **sipCppPtr = reinterpret_cast<%b **>(sipCppPtrV);\n"
"\n"
				,&type,&type);

		generateCppCodeBlock(convtocode,fp);

		prcode(fp,
"}\n"
			);
	}

	prcode(fp,
"\n"
"\n"
"static void *forceConvertTo_%T(PyObject *valobj,int *iserrp)\n"
"{\n"
"	if (*iserrp || valobj == NULL)\n"
"		return NULL;\n"
"\n"
		,&type);

	if (convtocode != NULL)
		prcode(fp,
"	if (convertTo_%T(valobj,NULL,NULL))\n"
"	{\n"
"		void *val;\n"
"\n"
"		/*\n"
"		 * Note that we throw away the flag that says if the value\n"
"		 * has just been created on the heap or not.\n"
"		 */\n"
"		convertTo_%T(valobj,&val,iserrp);\n"
"\n"
"		return val;\n"
"	}\n"
			,&type
			,&type);
	else
		prcode(fp,
"	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_%T))\n"
"		return sipConvertToCpp(valobj,sipClass_%T,iserrp);\n"
			,&type
			,&type);

	if (cd != NULL)
		prcode(fp,
"\n"
"	sipBadClass(%N);\n"
			, iff->name);
	else
		prcode(fp,
"\n"
"	sipBadClass(\"%B\");\n"
			, &mtd->type);

	prcode(fp,
"\n"
"	*iserrp = 1;\n"
"\n"
"	return NULL;\n"
"}\n"
		);
}


/*
 * Generate a variable handler.
 */
static void generateVariableHandler(varDef *vd,FILE *fp)
{
	char *ptr;
	argType atype = vd -> type.atype;

	prcode(fp,
"\n"
"\n"
"static PyObject *var_%C(PyObject *%s,PyObject *sipPy)\n"
"{\n"
		,vd -> fqcname,(isStaticVar(vd) ? "" : "sipSelf"));

	if (atype == class_type || atype == mapped_type)
		prcode(fp,
"	int sipIsErr = 0;\n"
			);

	if (vd->getcode == NULL || vd->setcode == NULL)
	{
		char name[50];
		argDef mod;

		prcode(fp,
"	");

		mod = vd -> type;

		if ((atype == string_type || atype == ustring_type) && vd -> type.nrderefs == 0)
		{
			ptr = "Ptr";
			mod.nrderefs = 1;
		}
		else
			ptr = "";

		sprintf(name,"sipVal%s",ptr);

		generateNamedValueType(&mod,name,fp);

		prcode(fp,";\n"
			);
	}

	if (!isStaticVar(vd))
	{
		if (generating_c)
			prcode(fp,
"	%S *sipCpp = (%S *)sipGetCppPtr((sipWrapper *)sipSelf,sipClass_%C);\n"
				,classFQCName(vd -> ecd),classFQCName(vd -> ecd),classFQCName(vd -> ecd));
		else
			prcode(fp,
"	%S *sipCpp = reinterpret_cast<%S *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_%C));\n"
				,classFQCName(vd -> ecd),classFQCName(vd -> ecd),classFQCName(vd -> ecd));

		prcode(fp,
"\n"
"	if (!sipCpp)\n"
"		return NULL;\n"
			);
	}

	prcode(fp,
"\n"
"	if (sipPy == NULL)\n"
"	{\n"
		);

	/* Generate the get handler part. */

	if (vd->getcode != NULL)
	{
		generateCppCodeBlock(vd->getcode, fp);

		prcode(fp,
"\n"
"		return sipPy;\n"
			);
	}
	else
	{
		int pyobj = FALSE;

		prcode(fp,
"		");

		if (ptr[0] == '\0')
			prcode(fp,"sipVal");
		else
			generateNamedValueType(&vd -> type,"sipVal",fp);

		prcode(fp," = %s",(((atype == class_type || atype == mapped_type) && vd -> type.nrderefs == 0) ? "&" : ""));

		generateVarMember(vd, fp);

		prcode(fp,";\n"
"\n"
			);

		switch (atype)
		{
		case mapped_type:
			prcode(fp,
"		sipPy = sipConvertFrom_%T(sipVal);\n"
				,&vd -> type);

			break;

		case class_type:
			generateVarClassConversion(vd,fp);
			break;

		case bool_type:
			prcode(fp,
"		sipPy = PyBool_FromLong(sipVal);\n"
				);

			break;

		case ustring_type:
		case string_type:
			if (vd -> type.nrderefs == 0)
				prcode(fp,
"		sipPy = PyString_FromStringAndSize(%s&sipVal,1);\n"
					,(atype == ustring_type) ? "(char *)" : "");
			else
				prcode(fp,
"		sipPy = PyString_FromString(%ssipVal);\n"
					,(atype == ustring_type) ? "(char *)" : "");

			break;

		case float_type:
		case cfloat_type:
			prcode(fp,
"		sipPy = PyFloat_FromDouble((double)sipVal);\n"
				);
			break;

		case double_type:
		case cdouble_type:
			prcode(fp,
"		sipPy = PyFloat_FromDouble(sipVal);\n"
				);
			break;

		case enum_type:
			if (vd -> type.u.ed -> fqcname != NULL)
			{
				prcode(fp,
"		sipPy = sipConvertFromNamedEnum(sipVal,sipEnum_%C);\n"
					,vd -> type.u.ed -> fqcname);

				break;
			}

			/* Drop through. */

		case ushort_type:
		case short_type:
		case uint_type:
		case cint_type:
		case int_type:
			prcode(fp,
"		sipPy = PyInt_FromLong(sipVal);\n"
				);
			break;

		case long_type:
			prcode(fp,
"		sipPy = PyLong_FromLong(sipVal);\n"
				);
			break;

		case ulong_type:
			prcode(fp,
"		sipPy = PyLong_FromUnsignedLong(sipVal);\n"
				);
			break;

		case longlong_type:
			prcode(fp,
"		sipPy = PyLong_FromLongLong(sipVal);\n"
				);
			break;

		case ulonglong_type:
			prcode(fp,
"		sipPy = PyLong_FromUnsignedLongLong(sipVal);\n"
				);
			break;

		case struct_type:
		case void_type:
			prcode(fp,
"		sipPy = sipConvertFromVoidPtr(sipVal);\n"
				);
			break;

		case pyobject_type:
		case pytuple_type:
		case pylist_type:
		case pydict_type:
		case pycallable_type:
		case pyslice_type:
		case pytype_type:
			prcode(fp,
"		Py_XINCREF(sipVal);\n"
				);
			pyobj = TRUE;
			break;
		}

		prcode(fp,
"\n"
"		return %s;\n"
			,(pyobj ? "sipVal" : "sipPy"));
	}

	prcode(fp,
"	}\n"
"\n"
		);

	/* Generate the set handler part. */

	if (vd->setcode != NULL)
	{
		prcode(fp,
"	{\n"
"		int sipErr = 0;\n"
"\n"
			);

		generateCppCodeBlock(vd->setcode, fp);

		prcode(fp,
"\n"
"		if (sipErr)\n"
"		{\n"
"			sipBadSetType(%N,%N);\n"
"			return NULL;\n"
"		}\n"
"	}\n"
			, vd->ecd->iff->name, vd->pyname);
	}
	else
	{
		char *deref;

		generateObjToCppConversion(&vd -> type,fp);

		deref = "";

		if (atype == class_type || atype == mapped_type)
		{
			char *none_test = "";

			if (vd -> type.nrderefs == 0)
			{
				deref = "*";
				none_test = " || sipVal == NULL";
			}

			prcode(fp,
"\n"
"	if (sipIsErr%s)\n"
				,none_test);
		}
		else if (atype == ustring_type || atype == string_type)
		{
			if (vd -> type.nrderefs == 0)
				deref = "*";

			prcode(fp,
"\n"
"	if (sipVal%s == NULL)\n"
				,ptr);
		}
		else
			prcode(fp,
"\n"
"	if (PyErr_Occurred() != NULL)\n"
				);

		prcode(fp,
"	{\n"
"		sipBadSetType(%N,%N);\n"
"		return NULL;\n"
"	}\n"
"\n"
			,vd -> ecd -> iff -> name,vd -> pyname);

		if (atype == pyobject_type || atype == pytuple_type ||
		    atype == pylist_type || atype == pydict_type ||
		    atype == pycallable_type || atype == pyslice_type ||
		    atype == pytype_type)
		{
			prcode(fp,
"	Py_XDECREF(");

			generateVarMember(vd, fp);

			prcode(fp, ");\n"
"	Py_INCREF(sipVal);\n"
"\n"
				);
		}

		prcode(fp,
"	");

		generateVarMember(vd, fp);

		prcode(fp," = %ssipVal%s;\n"
			,deref,ptr);
	}

	prcode(fp,
"\n"
"	Py_INCREF(Py_None);\n"
"	return Py_None;\n"
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
		prcode(fp,"sipCpp -> ");

	prcode(fp, "%s", scopedNameTail(vd->fqcname));
}


/*
 * Generate an variable class conversion fragment.
 */
static void generateVarClassConversion(varDef *vd,FILE *fp)
{
	classDef *cd = vd -> type.u.cd;

	prcode(fp,
"		sipPy = sipMapCppToSelf%s(",(cd -> subbase != NULL ? "SubClass" : ""));

	if (isConstArg(&vd -> type))
		prcode(fp,"const_cast<%b *>(sipVal)",&vd -> type);
	else
		prcode(fp,"sipVal");

	prcode(fp,",sipClass_%C);\n"
		,classFQCName(cd));
}


/*
 * Generate the declaration of a variable that is initialised from a Python
 * object.
 */
static void generateObjToCppConversion(argDef *ad,FILE *fp)
{
	char *fmt = NULL;

	switch (ad -> atype)
	{
	case mapped_type:
	case class_type:
		if (generating_c)
			prcode(fp,
"	sipVal = (%b *)sipForceConvertTo_%T(sipPy,&sipIsErr);\n"
				,ad,ad);
		else
			prcode(fp,
"	sipVal = reinterpret_cast<%b *>(sipForceConvertTo_%T(sipPy,&sipIsErr));\n"
				,ad,ad);
		break;

	case enum_type:
		prcode(fp,
"	sipVal = (%E)PyInt_AsLong(sipPy);\n"
			,ad -> u.ed);
		break;

	case ustring_type:
		if (ad -> nrderefs == 0)
			fmt = "\tsipValPtr = (unsigned char *)PyString_AsString(sipPy);\n";
		else
			fmt = "\tsipVal = (unsigned char *)PyString_AsString(sipPy);\n";
		break;

	case string_type:
		if (ad -> nrderefs == 0)
			fmt = "\tsipValPtr = PyString_AsString(sipPy);\n";
		else
			fmt = "\tsipVal = PyString_AsString(sipPy);\n";
		break;

	case float_type:
	case cfloat_type:
		fmt = "\tsipVal = (float)PyFloat_AsDouble(sipPy);\n";
		break;

	case double_type:
	case cdouble_type:
		fmt = "\tsipVal = PyFloat_AsDouble(sipPy);\n";
		break;

	case bool_type:
		fmt = "\tsipVal = (bool)PyInt_AsLong(sipPy);\n";
		break;

	case ushort_type:
		fmt = "\tsipVal = (unsigned short)PyInt_AsLong(sipPy);\n";
		break;

	case short_type:
		fmt = "\tsipVal = (short)PyInt_AsLong(sipPy);\n";
		break;

	case uint_type:
		fmt = "\tsipVal = (unsigned)PyInt_AsLong(sipPy);\n";
		break;

	case int_type:
	case cint_type:
		fmt = "\tsipVal = (int)PyInt_AsLong(sipPy);\n";
		break;

	case ulong_type:
		fmt = "\tsipVal = PyLong_AsUnsignedLong(sipPy);\n";
		break;

	case long_type:
		fmt = "\tsipVal = PyLong_AsLong(sipPy);\n";
		break;

	case ulonglong_type:
		fmt = "\tsipVal = PyLong_AsUnsignedLongLong(sipPy);\n";
		break;

	case longlong_type:
		fmt = "\tsipVal = PyLong_AsLongLong(sipPy);\n";
		break;

	case struct_type:
		prcode(fp,
"	sipVal = (struct %S *)sipConvertToVoidPtr(sipPy);\n"
			,ad -> u.sname);
		break;

	case void_type:
		fmt = "\tsipVal = sipConvertToVoidPtr(sipPy);\n";
		break;

	case pyobject_type:
	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pycallable_type:
	case pyslice_type:
	case pytype_type:
		fmt = "\tsipVal = sipPy;\n";
		break;
	}

	if (fmt != NULL)
		prcode(fp,"%s",fmt);
}


/*
 * Returns TRUE if the given method is a slot that takes zero arguments.
 */
static int isZeroArgSlot(memberDef *md)
{
	slotType st = md -> slot;

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
	slotType st = md -> slot;

	return (st == setitem_slot || st == call_slot);
}


/*
 * Returns TRUE if the given method is a slot that returns void (ie. nothing
 * other than an error indicator).
 */
int isVoidReturnSlot(memberDef *md)
{
	slotType st = md -> slot;

	return (st == setitem_slot || st == delitem_slot);
}


/*
 * Returns TRUE if the given method is a slot that returns int.
 */
int isIntReturnSlot(memberDef *md)
{
	slotType st = md -> slot;

	return (st == len_slot || st == nonzero_slot || st == contains_slot ||
		st == cmp_slot);
}


/*
 * Returns TRUE if the given method is a slot that takes an int argument.
 */
static int isIntArgSlot(memberDef *md)
{
	slotType st = md -> slot;

	return (st == repeat_slot || st == irepeat_slot);
}


/*
 * Returns TRUE if the given method is an inplace number slot.
 */
static int isInplaceNumberSlot(memberDef *md)
{
	slotType st = md -> slot;

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
	slotType st = md -> slot;

	return (st == iconcat_slot || st == irepeat_slot);
}


/*
 * Returns TRUE if the given method is a number slot slot.
 */
int isNumberSlot(memberDef *md)
{
	slotType st = md -> slot;

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
	slotType st = md -> slot;

	return (st == lt_slot || st == le_slot || st == eq_slot ||
		st == ne_slot || st == gt_slot || st == ge_slot);
}


/*
 * Generate a Python slot handler for either a class, an enum or an extender.
 */
static void generateSlot(sipSpec *pt, classDef *cd, enumDef *ed, memberDef *md, FILE *fp)
{
	char *arg_str, *prefix;
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
		pyname = cd->iff->name;
		fqcname = classFQCName(cd);
		overs = cd->overs;
	}
	else
	{
		prefix = NULL;
		pyname = NULL;
		fqcname = NULL;
		overs = pt->overs;
	}

	ret_int = (isVoidReturnSlot(md) || isIntReturnSlot(md));

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
"static %sslot_", (ret_int ? "int " : "PyObject *"));

	if (fqcname != NULL)
		prcode(fp, "%C_", fqcname);

	prcode(fp, "%s(%s)\n"
"{\n"
		, md->pyname->text, arg_str);

	if (isInplaceNumberSlot(md))
		prcode(fp,
"	if (!PyObject_TypeCheck(sipSelf,(PyTypeObject *)sip%s_%C))\n"
"	{\n"
"		Py_INCREF(Py_NotImplemented);\n"
"		return Py_NotImplemented;\n"
"	}\n"
"\n"
			, prefix, fqcname);

	if (!isNumberSlot(md))
		if (cd != NULL)
			prcode(fp,
"	%S *sipCpp = reinterpret_cast<%S *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_%C));\n"
"\n"
"	if (!sipCpp)\n"
"		return %s;\n"
"\n"
				, fqcname, fqcname, fqcname
				, (md->slot == cmp_slot ? "-2" : (ret_int ? "-1" : "0")));
		else
			prcode(fp,
"	%S sipCpp = static_cast<%S>(PyInt_AsLong(sipSelf));\n"
"\n"
				, fqcname, fqcname);

	if (nr_args > 0)
		prcode(fp,
"	int sipArgsParsed = 0;\n"
			);

	for (od = overs; od != NULL; od = od -> next)
		if (od -> common == md)
			generateFunctionBody(pt,od,cd,cd,fp);

	if (nr_args > 0)
		switch (md -> slot)
		{
		case cmp_slot:
			prcode(fp,
"\n"
"	return 2;\n"
				);
			break;

		case concat_slot:
		case iconcat_slot:
		case repeat_slot:
		case irepeat_slot:
			prcode(fp,
"\n"
"	/* Raise an exception if the argument couldn't be parsed. */\n"
"	sipBadOperatorArg(sipSelf,sipArg,%s);\n"
"\n"
"	return NULL;\n"
				,slotName(md -> slot));
			break;

		default:
			if (isNumberSlot(md) || isRichCompareSlot(md))
			{
				if (cd == NULL && ed == NULL)
					prcode(fp,
"\n"
"	Py_INCREF(Py_NotImplemented);\n"
"	return Py_NotImplemented;\n"
						);
				else if (isNumberSlot(md))
					prcode(fp,
"\n"
"	return sipPySlotExtend(&sipModuleAPI_%s,%s,NULL,sipArg0,sipArg1);\n"
						, pt->module->name, slotName(md->slot));
				else
					prcode(fp,
"\n"
"	return sipPySlotExtend(&sipModuleAPI_%s,%s,sip%s_%C,sipSelf,sipArg);\n"
						, pt->module->name, slotName(md->slot), prefix, fqcname);
			}
			else if (isInplaceNumberSlot(md))
				prcode(fp,
"\n"
"	PyErr_Clear();\n"
"\n"
"	Py_INCREF(Py_NotImplemented);\n"
"	return Py_NotImplemented;\n"
					);
			else
				prcode(fp,
"\n"
"	/* Raise an exception if the arguments couldn't be parsed. */\n"
"	sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"	return %s;\n"
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
static void generateClassFunctions(sipSpec *pt,classDef *cd,FILE *fp)
{
	visibleList *vl;
	memberDef *md;

	/* Any shadow code. */
	if (hasShadow(cd))
		generateShadowCode(pt,cd,fp);

	/* The member functions. */
	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
		generateFunction(pt,vl -> m,vl -> cd -> overs,cd,vl -> cd,fp);

	/* The slot functions. */
	for (md = cd -> members; md != NULL; md = md -> next)
		if (cd -> iff -> type == namespace_iface)
			generateOrdinaryFunction(pt,cd,md,fp);
		else if (md -> slot != no_slot && md -> slot != unicode_slot)
			generateSlot(pt, cd, NULL, md, fp);

	if (cd -> iff -> type != namespace_iface && !generating_c)
	{
		classList *cl;

		/* The cast function. */
		prcode(fp,
"\n"
"\n"
"/* Cast a pointer to a type somewhere in its superclass hierarchy. */\n"
"static void *cast_%C(void *ptr,sipWrapperType *targetClass)\n"
"{\n"
			,classFQCName(cd));

		if (cd -> supers != NULL)
			prcode(fp,
"	void *res;\n"
"\n"
				);

		prcode(fp,
"	if (targetClass == sipClass_%C)\n"
"		return ptr;\n"
			,classFQCName(cd));

		for (cl = cd -> supers; cl != NULL; cl = cl -> next)
		{
			scopedNameDef *sname = cl -> cd -> iff -> fqcname;

			prcode(fp,
"\n"
"	if ((res = sipCast_%C((%S *)(%S *)ptr,targetClass)) != NULL)\n"
"		return res;\n"
				,sname,sname,classFQCName(cd));
		}

		prcode(fp,
"\n"
"	return NULL;\n"
"}\n"
			);
	}

	/* The traverse function. */
	if (cd->travcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int traverse_%C(void *sipCppV,visitproc sipVisit,void *sipArg)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"	int sipRes;\n"
"\n"
			,classFQCName(cd)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->travcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	/* The clear function. */
	if (cd->clearcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int clear_%C(void *sipCppV)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"	int sipRes;\n"
"\n"
			,classFQCName(cd)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->clearcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	/* The buffer interface functions. */
	if (cd->readbufcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int getreadbuffer_%C(PyObject *%s, void *sipCppV, int %s, void **%s)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"	int sipRes;\n"
"\n"
			,classFQCName(cd)
			, argName("sipSelf", cd->readbufcode)
			, argName("sipSegment", cd->readbufcode)
			, argName("sipPtrPtr", cd->readbufcode)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->readbufcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	if (cd->writebufcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int getwritebuffer_%C(PyObject *%s, void *sipCppV, int %s, void **%s)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"	int sipRes;\n"
"\n"
			,classFQCName(cd)
			, argName("sipSelf", cd->writebufcode)
			, argName("sipSegment", cd->writebufcode)
			, argName("sipPtrPtr", cd->writebufcode)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->writebufcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	if (cd->segcountcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int getsegcount_%C(PyObject *%s, void *sipCppV, int *%s)\n"
"{\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"	int sipRes;\n"
"\n"
			,classFQCName(cd)
			, argName("sipSelf", cd->segcountcode)
			, argName("sipLenPtr", cd->segcountcode)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->segcountcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	if (cd->charbufcode != NULL)
	{
		prcode(fp,
"\n"
"\n"
"static int getcharbuffer_%C(PyObject *%s, void *sipCppV, int %s, void **%s)\n"
"{\n"
"	int sipRes;\n"
"	%S *sipCpp = reinterpret_cast<%S *>(sipCppV);\n"
"\n"
			,classFQCName(cd)
			, argName("sipSelf", cd->charbufcode)
			, argName("sipSegment", cd->charbufcode)
			, argName("sipPtrPtr", cd->charbufcode)
			,classFQCName(cd),classFQCName(cd));

		generateCppCodeBlock(cd->charbufcode, fp);

		prcode(fp,
"\n"
"	return sipRes;\n"
"}\n"
			);
	}

	/* The dealloc function. */
	if (needDealloc(cd))
	{
		prcode(fp,
"\n"
"\n"
"static void dealloc_%C(sipWrapper *sipSelf)\n"
"{\n"
			,classFQCName(cd));

		if (tracing)
			prcode(fp,
"	sipTrace(SIP_TRACE_DEALLOCS,\"dealloc_%C()\\n\");\n"
"\n"
				,classFQCName(cd));

		/* Disable the virtual handlers. */
		if (hasShadow(cd))
			prcode(fp,
"	if (!sipIsSimple(sipSelf))\n"
"		reinterpret_cast<sip%C *>(sipSelf -> u.cppPtr) -> sipPySelf = NULL;\n"
"\n"
				,classFQCName(cd));

		if (generating_c || isPublicDtor(cd) || (hasShadow(cd) && isProtectedDtor(cd)))
		{
			prcode(fp,
"	if (sipIsPyOwned(sipSelf))\n"
"	{\n"
				);

			if (cd -> dealloccode != NULL)
			{
				if (usedInCode(cd->dealloccode, "sipCpp"))
					if (generating_c)
						prcode(fp,
"		%S *sipCpp = (%S *)sipSelf -> u.cppPtr;\n"
"\n"
							,classFQCName(cd),classFQCName(cd));
					else
						prcode(fp,
"		%S *sipCpp = reinterpret_cast<%S *>(sipSelf -> u.cppPtr);\n"
"\n"
							,classFQCName(cd),classFQCName(cd));

				generateCppCodeBlock(cd -> dealloccode,fp);

				prcode(fp,
"\n"
					);
			}

			if (generating_c)
				prcode(fp,
"		sipFree(sipSelf -> u.cppPtr);\n"
					);
			else
			{
				if (release_gil || isReleaseGILDtor(cd))
					prcode(fp,
"\n"
"		Py_BEGIN_ALLOW_THREADS\n"
"\n"
						);

				if (hasShadow(cd))
				{
					prcode(fp,
"		if (!sipIsSimple(sipSelf))\n"
"			delete reinterpret_cast<sip%C *>(sipSelf -> u.cppPtr);\n"
						,classFQCName(cd));

					if (isPublicDtor(cd))
						prcode(fp,
"		else\n"
"			delete reinterpret_cast<%U *>(sipSelf -> u.cppPtr);\n"
						,cd);
				}
				else
					prcode(fp,
"		delete reinterpret_cast<%U *>(sipSelf -> u.cppPtr);\n"
						,cd);

				if (release_gil || isReleaseGILDtor(cd))
					prcode(fp,
"\n"
"		Py_END_ALLOW_THREADS\n"
						);
			}

			prcode(fp,
"	}\n"
				);
		}

		prcode(fp,
"}\n"
			);
	}

	/* The type initialisation function. */
	if (canCreate(cd))
		generateTypeInit(pt, cd, fp);
}


/*
 * Generate the shadow class code.
 */
static void generateShadowCode(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro, nrVirts, virtNr;
	virtOverDef *vod;
	visibleList *vl;
	ctorDef *ct;

	nrVirts = countVirtuals(cd);

	/* Generate the wrapper class constructors. */

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		char *prefix;
		int a;
		ctorDef *dct;

		if (isPrivateCtor(ct))
			continue;

		/* Check we haven't already handled this C++ signature. */
		for (dct = cd -> ctors; dct != ct; dct = dct -> next)
			if (sameSignature(dct -> cppsig,ct -> cppsig,TRUE))
				break;

		if (dct != ct)
			continue;

		prcode(fp,
"\n"
"sip%C::sip%C(",classFQCName(cd),classFQCName(cd));

		generateArgs(ct -> cppsig,Definition,fp);

		prcode(fp,")%X: %S(",ct -> exceptions,classFQCName(cd));

		prefix = "";

		for (a = 0; a < ct -> cppsig -> nrArgs; ++a)
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
"	sipTrace(SIP_TRACE_CTORS,\"sip%C::sip%C(",classFQCName(cd),classFQCName(cd));
			generateArgs(ct -> cppsig,Declaration,fp);
			prcode(fp,")%X (this=0x%%08x)\\n\",this);\n"
"\n"
				,ct -> exceptions);
		}

		prcode(fp,
"	sipCommonCtor(%s,%d);\n"
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
			,classFQCName(cd),classFQCName(cd),cd -> dtorexceptions);

		if (tracing)
			prcode(fp,
"	sipTrace(SIP_TRACE_DTORS,\"sip%C::~sip%C()%X (this=0x%%08x)\\n\",this);\n"
"\n"
				,classFQCName(cd),classFQCName(cd),cd -> dtorexceptions);

		if (cd -> dtorcode != NULL)
			generateCppCodeBlock(cd -> dtorcode,fp);

		prcode(fp,
"	sipCommonDtor(sipPySelf);\n"
"}\n"
			);
	}

	/* Generate the virtual catchers. */
 
	virtNr = 0;
 
	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
	{
		overDef *od = &vod -> o;
		virtOverDef *dvod;

		if (isPrivate(od))
			continue;

		/* Check we haven't already handled this C++ signature. */
		for (dvod = cd -> vmembers; dvod != vod; dvod = dvod -> next)
			if (strcmp(dvod -> o.cppname,od -> cppname) == 0 && sameSignature(dvod -> o.cppsig,od -> cppsig,TRUE))
				break;

		if (dvod != vod)
			continue;

		generateVirtualCatcher(pt,cd,virtNr++,vod,fp);
	}

	/* Generate the wrapper around each protected member function. */

	generateProtectedDefinitions(cd,fp);

	/* Generate the emitter functions. */

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
			if (od -> common == vl -> m && isSignal(od))
			{
				generateEmitter(pt,cd,vl,fp);
				break;
			}
	}

	/* Generate the table of signals to support fan-outs. */

	noIntro = TRUE;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
			if (od -> common == vl -> m && isSignal(od))
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
"	{%N, %C_emit_%s},\n"
					,vl -> m -> pyname,classFQCName(cd),vl -> m -> pyname -> text);

				break;
			}
	}

	if (!noIntro)
		prcode(fp,
"	{NULL, NULL}\n"
"};\n"
			);
}


/*
 * Generate the protected enums for a class.
 */
static void generateProtectedEnums(sipSpec *pt,classDef *cd,FILE *fp)
{
	enumDef *ed;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		char *eol;
		enumMemberDef *emd;

		/* Ignore unless this class is the publisher. */
		if (cd != ed -> pcd)
			continue;

		prcode(fp,
"\n"
"	/* Expose this protected enum. */\n"
"	enum");

		if (ed -> fqcname != NULL)
			prcode(fp," sip%s",scopedNameTail(ed -> fqcname));

		prcode(fp," {");

		eol = "\n";

		for (emd = ed -> members; emd != NULL; emd = emd -> next)
		{
			prcode(fp,"%s"
"		%s = %S::%s",eol,emd -> cname,classFQCName(ed -> ecd),emd -> cname);

			eol = ",\n";
		}

		prcode(fp,"\n"
"	};\n"
			);
	}
}


/*
 * Generate the catcher for a virtual function.
 */

static void generateVirtualCatcher(sipSpec *pt,classDef *cd,int virtNr,
				   virtOverDef *vod,FILE *fp)
{
	overDef *od = &vod -> o;
	virtHandlerDef *vhd = od -> virthandler;
	argDef *res, *ad;
	int a;

	normaliseArgs(od -> cppsig);

	res = &od -> cppsig -> result;

	if (res -> atype == void_type && res -> nrderefs == 0)
		res = NULL;

	prcode(fp,
"\n");

	generateResultType(&od -> cppsig -> result,fp);

	prcode(fp," sip%C::%O(",classFQCName(cd),od);
	generateArgs(od -> cppsig,Definition,fp);
	prcode(fp,")%s%X\n"
"{\n"
		,(isConst(od) ? " const" : ""),od -> exceptions);

	if (tracing)
	{
		prcode(fp,
"	sipTrace(SIP_TRACE_CATCHERS,\"");

		generateResultType(&od -> cppsig -> result,fp);
		prcode(fp," sip%C::%O(",classFQCName(cd),od);
		generateArgs(od -> cppsig,Declaration,fp);
		prcode(fp,")%s%X (this=0x%%08x)\\n\",this);\n"
"\n"
			,(isConst(od) ? " const" : ""),od -> exceptions);
	}

	restoreArgs(od -> cppsig);

	if (vhd -> module == pt -> module)
	{
		prcode(fp,
"	extern ");

		generateResultType(&od -> cppsig -> result,fp);

		prcode(fp," sipVH_%s_%d(sip_gilstate_t,PyObject *",vhd -> module -> name,vhd -> virthandlernr);
	}
	else
	{
		prcode(fp,
"	typedef ");

		generateResultType(&od -> cppsig -> result,fp);

		prcode(fp," (*sipVH_%s_%d)(sip_gilstate_t,PyObject *",vhd -> module -> name,vhd -> virthandlernr);
	}

	if (vhd -> sd -> nrArgs > 0)
	{
		prcode(fp,",");
		generateArgs(vhd -> sd,Declaration,fp);
	}

	prcode(fp,");\n"
		);

	if (isNewThread(od))
		prcode(fp,
"\n"
"	SIP_BLOCK_THREADS\n"
			);

	prcode(fp,
"\n"
"	sip_gilstate_t sipGILState;\n"
"	PyObject *meth;\n"
"\n"
"	meth = sipIsPyMethod(&sipGILState,");

	if (isConst(od))
		prcode(fp,"const_cast<sipMethodCache *>(");

	prcode(fp,"&sipPyMethods[%d]",virtNr);

	if (isConst(od))
		prcode(fp,")");

	prcode(fp,",sipPySelf,");

	if (isAbstract(od))
		prcode(fp,"%N",cd -> iff -> name);
	else
		prcode(fp,"NULL");

	prcode(fp,",%N);\n"
"\n"
		,od -> common -> pyname);

	if (isNewThread(od))
		prcode(fp,
"	if (meth)\n"
"	{\n"
"		sipStartThread();\n"
"		");
	else
	{
		prcode(fp,
"	if (!meth)\n"
			);

		if (isAbstract(od))
			generateVirtHandlerErrorReturn(res,fp);
		else
		{
			if (res == NULL)
				prcode(fp,
"	{\n"
"		");
			else
				prcode(fp,
"		return ");

			generateUnambiguousClass(cd,vod -> scope,fp);

			prcode(fp,"::%O(",od);
 
			for (a = 0; a < od -> cppsig -> nrArgs; ++a)
				prcode(fp,"%sa%d",(a == 0 ? "" : ","),a);
 
			prcode(fp,");\n"
				);

			if (res == NULL)
				prcode(fp,
"		return;\n"
"	}\n"
					);
		}

		prcode(fp,
"\n"
"	");

		if (res != NULL)
			prcode(fp,"return ");
	}

	if (vhd -> module == pt -> module)
		prcode(fp,"sipVH_%s_%d",vhd -> module -> name,vhd -> virthandlernr);
	else
		prcode(fp,"((sipVH_%s_%d)(sipModuleAPI_%s_%s -> em_virthandlers[%d]))",vhd -> module -> name,vhd -> virthandlernr,pt -> module -> name,vhd -> module -> name,vhd -> virthandlernr);

	prcode(fp,"(sipGILState,meth");

	ad = od -> cppsig -> args;

	for (a = 0; a < od -> cppsig -> nrArgs; ++a)
	{
		if (ad -> atype == class_type && isProtectedClass(ad -> u.cd))
			prcode(fp,",static_cast<%U *>(a%d)",ad -> u.cd,a);
		else if (ad -> atype == enum_type && isProtectedEnum(ad -> u.ed))
			prcode(fp, ",(%E)a%d", ad->u.ed, a);
		else
			prcode(fp,",a%d",a);

		++ad;
	}
 
	prcode(fp,");\n"
		);

	if (isNewThread(od))
		prcode(fp,
"		sipEndThread();\n"
"	}\n"
"\n"
"	SIP_UNBLOCK_THREADS\n"
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
	for (mro = cd -> mro; mro != NULL; mro = mro -> next)
		if (mro -> cd == scope)
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

				while (guardc != cd -> mro)
				{
					mroDef *sub;
					classList *cl;

					for (sub = cd -> mro; sub -> next != guardc; sub = sub -> next)
						;

					for (cl = sub -> cd -> supers; cl != NULL; cl = cl -> next)
						if (cl -> cd == mro -> cd)
						{
							prcode(fp,"%S",classFQCName(sub -> cd));

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
	if (ad -> atype == enum_type)
		prcode(fp,"(%E)",ad -> u.ed);

	prcode(fp,"0");
}


/*
 * Generate the return statement for a virtual handler when there has been an
 * error (ie. there is nothing sensible to return).
 */
static void generateVirtHandlerErrorReturn(argDef *res,FILE *fp)
{
	prcode(fp,
"		return");

	if (res == NULL)
	{
		prcode(fp,";\n"
			);

		return;
	}

	prcode(fp," ");

	if (res -> atype == mapped_type && res -> nrderefs == 0)
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
	else if (res -> atype == class_type && res -> nrderefs == 0)
	{
		ctorDef *ct = res -> u.cd -> defctor;

		/*
		 * If we don't have a suitable ctor then the generated code
		 * will issue an error message.
		 */
		if (ct != NULL && isPublicCtor(ct))
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
static void generateCallDefaultCtor(ctorDef *ct,FILE *fp)
{
	int a;

	prcode(fp,"(");

	for (a = 0; a < ct -> cppsig -> nrArgs; ++a)
	{
		argDef *ad = &ct -> cppsig -> args[a];

		if (ad -> defval != NULL)
			break;

		if (a > 0)
			prcode(fp,",");

		/*
		 * Do what we can to provide type information to the compiler.
		 */
		if (ad -> atype == class_type && ad -> nrderefs > 0 && !isReference(ad))
			prcode(fp,"static_cast<%B>(0)",ad);
		else if (ad -> atype == enum_type)
			prcode(fp,"static_cast<%E>(0)",ad -> u.ed);
		else if (ad -> atype == float_type || ad -> atype == cfloat_type)
			prcode(fp,"0.0F");
		else if (ad -> atype == double_type || ad -> atype == cdouble_type)
			prcode(fp,"0.0");
		else if (ad -> atype == uint_type)
			prcode(fp,"0U");
		else if (ad->atype == long_type || ad->atype == longlong_type)
			prcode(fp,"0L");
		else if (ad->atype == ulong_type || ad->atype == ulonglong_type)
			prcode(fp,"0UL");
		else
			prcode(fp,"0");
	}

	prcode(fp,")");
}


/*
 * Generate the emitter function for a signal.
 */
static void generateEmitter(sipSpec *pt,classDef *cd,visibleList *vl,FILE *fp)
{
	char *pname = vl -> m -> pyname -> text;
	overDef *od;

	prcode(fp,
"\n"
"int sip%C::sipEmit_%s(PyObject *sipArgs)\n"
"{\n"
"	int sipArgsParsed = 0;\n"
		,classFQCName(cd),pname);

	for (od = vl -> cd -> overs; od != NULL; od = od -> next)
	{
		if (od -> common != vl -> m || !isSignal(od))
			continue;

		/*
		 * Generate the code that parses the args and emits the
		 * appropriate overloaded signal.
		 */
		prcode(fp,
"\n"
"	{\n"
			);

		generateArgParser(pt, &od -> pysig, cd, NULL, NULL, FALSE, fp);

		prcode(fp,
"		{\n"
			);

		if (release_gil || isReleaseGIL(od))
			prcode(fp,
"			Py_BEGIN_ALLOW_THREADS\n"
				);

		prcode(fp,
"			emit %s("
			,od -> cppname);

		generateArgs(od -> cppsig,Call,fp);

		prcode(fp,");\n"
			);

		if (release_gil || isReleaseGIL(od))
			prcode(fp,
"			Py_END_ALLOW_THREADS\n"
				);

		prcode(fp,
"\n"
			);

		deleteTemps(&od -> pysig, fp);

		prcode(fp,
"			return 0;\n"
"		}\n"
"	}\n"
			);
	}

	prcode(fp,
"\n"
"	sipNoMethod(sipArgsParsed,%N,%N);\n"
"\n"
"	return -1;\n"
"}\n"
"\n"
"static int %C_emit_%s(sipWrapper *w,PyObject *sipArgs)\n"
"{\n"
"	sip%C *ptr = reinterpret_cast<sip%C *>(sipGetComplexCppPtr(w));\n"
"\n"
"	return (ptr ? ptr -> sipEmit_%s(sipArgs) : -1);\n"
"}\n"
		,cd -> iff -> name,vl -> m -> pyname
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

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			if (od -> common != vl -> m || !isProtected(od))
				continue;

			if (noIntro)
			{
				prcode(fp,
"\n"
"	/*\n"
"	 * There is a public method for every protected method visible from\n"
"	 * this class.\n"
"	 */\n"
					);

				noIntro = FALSE;
			}

			prcode(fp,
"	");

			if (isStatic(od))
				prcode(fp,"static ");

			generateResultType(&od -> cppsig -> result,fp);

			prcode(fp," sipProtect_%s(",od -> cppname);
			generateArgs(od -> cppsig,Declaration,fp);
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

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			char *mname = od -> cppname;
			int a, incast;
			argDef *res;

			if (od -> common != vl -> m || !isProtected(od))
				continue;

			prcode(fp,
"\n"
				);

			generateResultType(&od -> cppsig -> result,fp);

			prcode(fp,
" sip%C::sipProtect_%s(",classFQCName(cd),mname);
			generateArgs(od -> cppsig,Definition,fp);
			prcode(fp,")%s\n"
"{\n"
				,(isConst(od) ? " const" : ""));

			incast = FALSE;

			res = &od -> cppsig -> result;

			if (res -> atype == void_type && res -> nrderefs == 0)
				prcode(fp,
"	");
			else
			{
				prcode(fp,
"	return ");

				if (res -> atype == class_type && isProtectedClass(res -> u.cd))
				{
					prcode(fp,"static_cast<%U *>(",res -> u.cd);
					incast = TRUE;
				}
				else if (res -> atype == enum_type && isProtectedEnum(res -> u.ed))
					/*
					 * One or two older compilers can't
					 * handle a static_cast here so we
					 * revert to a C-style cast.
					 */
					prcode(fp,"(%E)",res -> u.ed);
			}

			/* Don't scope it if it is abstract. */
			if (!isAbstract(od))
				prcode(fp,"%S::",classFQCName(vl -> cd));

			prcode(fp,"%s(",mname);

			for (a = 0; a < od -> cppsig -> nrArgs; ++a)
			{
				argDef *ad = &od -> cppsig -> args[a];

				if (a > 0)
					prcode(fp,",");

				if (ad -> atype == enum_type && isProtectedEnum(ad -> u.ed))
					prcode(fp,"(%S)",ad -> u.ed -> fqcname);

				prcode(fp,"a%d",a);
			}

			if (incast)
				prcode(fp,")");

			prcode(fp,");\n"
"}\n"
				);
		}
	}
}


/*
 * Generate the function that does most of the work to handle a particular
 * virtual function.
 */
static void generateVirtualHandler(sipSpec *pt,virtHandlerDef *vhd,FILE *fp)
{
	int a, nrvals, copy, isref;
	signatureDef *cppsig;
	argDef *res, res_noconstref;

	cppsig = vhd -> sd;
	res = &cppsig -> result;

	copy = isref = FALSE;

	if (res -> atype == void_type && res -> nrderefs == 0)
		res = NULL;
	else
	{
		/*
		 * If we are returning a reference to an instance then we take
		 * care to handle Python errors but still return a valid C++
		 * instance.  If we are returning an instance then we take care
		 * to make a local copy of the instance returned from Python
		 * before the Python object is garbage collected and the C++
		 * instance (possibly) destroyed.
		 */
		if ((res -> atype == class_type || res -> atype == mapped_type) && res -> nrderefs == 0)
			if (isReference(res))
				isref = TRUE;
			else
				copy = TRUE;

		res_noconstref = *res;
		resetIsConstArg(&res_noconstref);
		resetIsReference(&res_noconstref);
	}

	prcode(fp,
"\n"
		);

	generateResultType(&cppsig -> result,fp);

	prcode(fp," sipVH_%s_%d(sip_gilstate_t sipGILState,PyObject *sipMethod"
		,pt -> module -> name,vhd -> virthandlernr);

	if (cppsig -> nrArgs > 0)
	{
		prcode(fp,",");

		generateArgs(cppsig,Definition,fp);
	}

	prcode(fp,")\n"
"{\n"
		);

	if (res != NULL)
	{
		prcode(fp,"	");

		generateResultType(&res_noconstref,fp);

		prcode(fp," %ssipRes",(isref ? "*" : ""));

		if (copy && res -> atype == class_type && res -> nrderefs == 0)
		{
			ctorDef *ct = res -> u.cd -> defctor;

			if (ct != NULL && isPublicCtor(ct) && ct -> cppsig -> nrArgs > 0 && ct -> cppsig -> args[0].defval == NULL)
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
	}

	if (vhd -> virtcode != NULL)
	{
		int error_flag = needErrorFlag(vhd -> virtcode);

		if (error_flag)
			prcode(fp,
"	int sipIsErr = 0;\n"
				);

		prcode(fp,
"\n"
			);

		generateCppCodeBlock(vhd -> virtcode,fp);

		if (error_flag)
			prcode(fp,
"\n"
"	if (sipIsErr)\n"
"		PyErr_Print();\n"
				);

		prcode(fp,
"\n"
"	Py_DECREF(sipMethod);\n"
"\n"
"	SIP_RELEASE_GIL(sipGILState)\n"
			);

		if (res != NULL)
			prcode(fp,
"\n"
"	return sipRes;\n"
				);

		prcode(fp,
"}\n"
			);

		return;
	}

	/* See how many values we expect. */
	nrvals = (res != NULL ? 1 : 0);

	for (a = 0; a < cppsig -> nrArgs; ++a)
		if (isOutArg(&cppsig -> args[a]))
			++nrvals;

	if (copy)
	{
		prcode(fp,
"	");

		generateResultType(&res_noconstref,fp);

		prcode(fp," *sipResOrig;\n");
	}

	/* Call the method. */
	prcode(fp,
"	PyObject *sipResObj = sipCallMethod(0,sipMethod,");

	generateTupleBuilder(cppsig,fp);

	prcode(fp,");\n"
"\n"
"	%s (!sipResObj || sipParseResult(0,sipMethod,sipResObj,\"",(isref ? "int sipIsErr =" : "if"));

	/* Build the format string. */
	if (nrvals == 0)
		prcode(fp,"Z");
	else
	{
		if (nrvals > 1)
			prcode(fp,"(");

		if (res != NULL)
			prcode(fp,"%c",getParseResultFormat(res));

		for (a = 0; a < cppsig -> nrArgs; ++a)
		{
			argDef *ad = &cppsig -> args[a];

			if (isOutArg(ad))
				prcode(fp,"%c",getParseResultFormat(ad));
		}

		if (nrvals > 1)
			prcode(fp,")");
	}

	prcode(fp,"\"");

	/* Pass the destination pointers. */
	if (res != NULL)
	{
		generateParseResultExtraArgs(res,fp);
		prcode(fp,",&sipRes%s",(copy ? "Orig" : ""));
	}

	for (a = 0; a < cppsig -> nrArgs; ++a)
	{
		argDef *ad = &cppsig -> args[a];

		if (isOutArg(ad))
		{
			generateParseResultExtraArgs(ad,fp);
			prcode(fp,",%sa%d",(isReference(ad) ? "&" : ""),a);
		}
	}

	if (isref)
		prcode(fp,") < 0);\n"
"\n"
"	if (sipIsErr)\n"
			);
	else
		prcode(fp,") < 0)\n"
			);

	prcode(fp,
"		PyErr_Print();\n"
		);

	/* Make a copy if needed. */
	if (copy)
	{
		prcode(fp,
"	else\n"
"	{\n"
"		sipRes = *sipResOrig;\n"
			);

		/*
		 * If the result is a mapped type then it is always on the heap
		 * so delete it now that it has been copied.
		 */
		if (res -> atype == mapped_type)
			prcode(fp,
"		delete sipResOrig;\n"
				);
		else if (res -> atype == class_type && res -> u.cd -> convtocode != NULL)
		{
			/*
			 * If the result type had handwritten conversion code
			 * then the result might be newly created on the heap.
			 * (The conversion code tells us this, but it has been
			 * thrown away by the time we get to here.)  If the
			 * result object was not a wrapped type then it must be
			 * on the heap, otherwise we assume it's on the heap if
			 * the addresses are different.  We know by this point
			 * that the returned pointer cannot be NULL.
			 */
			prcode(fp,
"\n"
"		if (!sipWrapper_Check(sipResObj) || sipGetCppPtr((sipWrapper *)sipResObj,sipClass_%T) != sipResOrig)\n"
"			delete sipResOrig;\n"
				,res);
		}

		prcode(fp,
"	}\n"
			);
	}

	if (isTransferVH(vhd))
		prcode(fp,
"\n"
"	sipTransferTo(sipResObj,sipResObj);\n"
			);

	prcode(fp,
"\n"
"	Py_XDECREF(sipResObj);\n"
"	Py_DECREF(sipMethod);\n"
"\n"
"	SIP_RELEASE_GIL(sipGILState)\n"
		);

	if (res != NULL)
	{
		if (isref)
		{
			prcode(fp,
"\n"
"	if (sipIsErr)\n"
				);

			generateVirtHandlerErrorReturn(res,fp);
		}

		prcode(fp,
"\n"
"	return %ssipRes;\n"
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
static void generateParseResultExtraArgs(argDef *ad,FILE *fp)
{
	switch (ad -> atype)
	{
	case mapped_type:
	case class_type:
		prcode(fp,",sipForceConvertTo_%T",ad);
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
		if (ad -> u.ed -> fqcname != NULL)
			prcode(fp,",sipEnum_%C",ad -> u.ed -> fqcname);
		break;
	}
}


/*
 * Return the format character used by sipParseResult() for a particular type.
 */
static char getParseResultFormat(argDef *ad)
{
	switch (ad -> atype)
	{
	case mapped_type:
	case class_type:
		return ((ad -> nrderefs == 0) ? 'M' : 'L');

	case bool_type:
		return 'b';

	case ustring_type:
	case string_type:
		return ((ad -> nrderefs == 0) ? 'c' : 's');

	case enum_type:
		return ((ad -> u.ed -> fqcname != NULL) ? 'E' : 'e');

	case ushort_type:
	case short_type:
		return 'h';

	case uint_type:
	case int_type:
	case cint_type:
		return 'i';

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
		return 'O';

	case pytuple_type:
	case pylist_type:
	case pydict_type:
	case pyslice_type:
	case pytype_type:
		return (isAllowNone(ad) ? 'N' : 'T');
	}

	/* We should never get here. */
	return ' ';
}


/*
 * Generate the code to build a tuple of Python arguments.
 */
static void generateTupleBuilder(signatureDef *sd,FILE *fp)
{
	int a, arraylenarg;

	prcode(fp,"\"");

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		char *fmt = "";
		argDef *ad = &sd -> args[a];

		if (!isInArg(ad))
			continue;

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (ad -> nrderefs == 0 || (ad -> nrderefs == 1 && isOutArg(ad)))
				fmt = "c";
			else if (isArray(ad))
				fmt = "a";
			else
				fmt = "s";

			break;

		case bool_type:
			fmt = "b";
			break;

		case enum_type:
			fmt = (ad -> u.ed -> fqcname != NULL) ? "E" : "e";
			break;

		case cint_type:
			fmt = "i";
			break;

		case uint_type:
		case int_type:
			if (isArraySize(ad))
				arraylenarg = a;
			else
				fmt = "i";

			break;

		case ushort_type:
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
			fmt = "T";
			break;

		case class_type:
			fmt = (ad -> u.cd -> subbase != NULL ? "M" : "O");
			break;

		case rxcon_type:
		case rxdis_type:
		case qobject_type:
			fmt = "O";
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

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		int derefs;
		argDef *ad = &sd -> args[a];

		if (!isInArg(ad))
			continue;

		derefs = ad -> nrderefs;

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (!(ad -> nrderefs == 0 || (ad -> nrderefs == 1 && isOutArg(ad))))
				--derefs;

			break;

		case mapped_type:
		case class_type:
			if (ad -> nrderefs > 0)
				--derefs;

			break;

		case struct_type:
		case void_type:
			--derefs;
			break;
		}

		if (ad -> atype == mapped_type || ad -> atype == class_type ||
		    ad -> atype == rxcon_type || ad -> atype == rxdis_type ||
		    ad -> atype == qobject_type)
		{
			prcode(fp,",");

			if (isConstArg(ad))
				prcode(fp,"const_cast<%b *>(",ad);

			if (ad -> nrderefs == 0)
				prcode(fp,"&");
			else
				while (derefs-- != 0)
					prcode(fp,"*");

			prcode(fp,"a%d",a);

			if (isConstArg(ad))
				prcode(fp,")");

			if (ad -> atype == mapped_type)
				prcode(fp,",sipConvertFrom_%T",ad);
			else if (ad -> atype == class_type)
				prcode(fp,",sipClass_%C",classFQCName(ad -> u.cd));
			else
				prcode(fp,",sipClass_QObject");
		}
		else
		{
			if (!isArraySize(ad))
			{
				prcode(fp,",");

				while (derefs-- != 0)
					prcode(fp,"*");

				prcode(fp,"a%d",a);
			}

			if (isArray(ad))
			{
				argType astype = sd -> args[arraylenarg].atype;

				prcode(fp,",%sa%d",(astype == int_type || astype == uint_type ? "" : "(int)"),arraylenarg);
			}
			else if (ad -> atype == enum_type && ad -> u.ed -> fqcname != NULL)
				prcode(fp,",sipEnum_%C",ad -> u.ed -> fqcname);
		}
	}
}


/*
 * Generate the class interface #include directives required by either a class
 * or a module.
 */
static void generateUsedIncludes(ifaceFileList *iffl, int header, FILE *fp)
{
	int newl = TRUE;

	while (iffl != NULL)
	{
		if (header == iffl->header)
		{
			if (newl)
			{
				prcode(fp,
"\n"
					);

				newl = FALSE;
			}

			prcode(fp,
"#include \"sip%s%F.h\"\n"
				, iffl->iff->module->name, iffl->iff->fqcname);
		}

		iffl = iffl->next;
	}

	if (!newl)
		prcode(fp,
"\n"
			);
}


/*
 * Generate the header file for the C++ interface.
 */
static void generateIfaceHeader(sipSpec *pt,ifaceFileDef *iff,char *codeDir)
{
	char *wfile;
	char *cmname = iff -> module -> name;
	classDef *cd;
	mappedTypeDef *mtd;
	exceptionDef *xd;
	int genused;
	FILE *fp;

	/* Create the header file. */

	wfile = createIfaceFileName(codeDir,iff,".h");
	fp = createFile(pt,wfile,"Interface header file.");

	prcode(fp,
"\n"
"#ifndef _%s%F_h\n"
"#define	_%s%F_h\n"
"\n"
		,cmname,iff -> fqcname,cmname,iff -> fqcname);

	genused = TRUE;

	for (cd = pt -> classes; cd != NULL; cd = cd -> next)
		if (cd -> iff == iff)
		{
			if (iff -> module == pt -> module)
				generateClassHeader(cd,genused,pt,fp);
			else if (!isExternal(cd))
				generateImportedClassHeader(cd,pt,fp);

			genused = FALSE;
		}

	for (cd = pt->proxies; cd != NULL; cd = cd->next)
		if (cd->iff == iff && iff->type == namespace_iface)
			generateClassHeader(cd, FALSE, pt, fp);

	genused = TRUE;

	for (mtd = pt -> mappedtypes; mtd != NULL; mtd = mtd -> next)
		if (mtd -> iff == iff)
		{
			if (iff -> module == pt -> module)
				generateMappedTypeHeader(mtd,genused,fp);
			else
				generateImportedMappedTypeHeader(mtd,pt,fp);

			genused = FALSE;
		}

	for (xd = pt -> exceptions; xd != NULL; xd = xd -> next)
		if (xd -> iff == iff)
		{
			generateCppCodeBlock(xd -> hdrcode,fp);

			if (xd -> exceptionnr >= 0)
			{
				prcode(fp,
"\n"
"#define	sipException_%C	sipModuleAPI_%s"
					,iff -> fqcname,pt -> module -> name);

				if (iff -> module == pt -> module)
					prcode(fp,".");
				else
					prcode(fp,"_%s -> ",iff -> module -> name);

				prcode(fp,"em_exceptions[%d]\n"
					,xd -> exceptionnr);
			}
		}

	prcode(fp,
"\n"
"#endif\n"
		);

	closeFile(fp);
	free(wfile);
}


/*
 * Generate the C++ header code for an imported mapped type.
 */
static void generateImportedMappedTypeHeader(mappedTypeDef *mtd,sipSpec *pt,
					     FILE *fp)
{
	char *mname = pt -> module -> name;
	char *imname = mtd -> iff -> module -> name;
	argDef type;

	generateCppCodeBlock(mtd -> hdrcode,fp);

	type.atype = mapped_type;
	type.u.mtd = mtd;
	type.argflags = 0;
	type.name = NULL;
	type.nrderefs = 0;
	type.defval = NULL;

	prcode(fp,
"\n"
"#define	sipForceConvertTo_%T	sipModuleAPI_%s_%s -> em_mappedtypes[%d] -> mt_fcto\n"
"#define	sipConvertTo_%T	sipModuleAPI_%s_%s -> em_mappedtypes[%d] -> mt_cto\n"
"#define	sipConvertFrom_%T	sipModuleAPI_%s_%s -> em_mappedtypes[%d] -> mt_cfrom\n"
		,&type,mname,imname,mtd -> mappednr
		,&type,mname,imname,mtd -> mappednr
		,&type,mname,imname,mtd -> mappednr);
}


/*
 * Generate the C++ header code for a generated mapped type.
 */
static void generateMappedTypeHeader(mappedTypeDef *mtd,int genused,FILE *fp)
{
	prcode(fp,
"\n"
"\n"
		);

	generateCppCodeBlock(mtd -> hdrcode,fp);

	if (genused)
		generateUsedIncludes(mtd->iff->used, TRUE, fp);

	prcode(fp,
"\n"
"#define	sipForceConvertTo_%T	sipMappedType_%T.mt_fcto\n"
"#define	sipConvertTo_%T	sipMappedType_%T.mt_cto\n"
"#define	sipConvertFrom_%T	sipMappedType_%T.mt_cfrom\n"
"\n"
"extern sipMappedTypeDef sipMappedType_%T;\n"
		,&mtd -> type,&mtd -> type
		,&mtd -> type,&mtd -> type
		,&mtd -> type,&mtd -> type
		,&mtd -> type);
}


/*
 * Generate the C++ header code for an imported class.
 */
static void generateImportedClassHeader(classDef *cd,sipSpec *pt,FILE *fp)
{
	char *mname = pt -> module -> name;
	char *imname = cd -> iff -> module -> name;
	classDef *hcd;

	for (hcd = cd; hcd != NULL; hcd = hcd -> ecd)
		generateCppCodeBlock(hcd -> hdrcode,fp);

	prcode(fp,
"\n"
"#define	sipClass_%C	sipModuleAPI_%s_%s -> em_types[%d]\n"
"#define	sipCast_%C	sipModuleAPI_%s_%s -> em_types[%d] -> type -> td_cast\n"
"#define	sipForceConvertTo_%C	sipModuleAPI_%s_%s -> em_types[%d] -> type -> td_fcto\n"
		,classFQCName(cd),mname,imname,cd -> classnr
		,classFQCName(cd),mname,imname,cd -> classnr
		,classFQCName(cd),mname,imname,cd -> classnr);

	if (cd -> convtocode != NULL)
		prcode(fp,
"#define	sipConvertTo_%C	sipModuleAPI_%s_%s -> em_types[%d] -> type -> td_cto\n"
			,classFQCName(cd),mname,imname,cd -> classnr);

	generateEnumMacros(pt, cd, fp);
}


/*
 * Generate the C++ header code for a generated class.
 */
static void generateClassHeader(classDef *cd,int genused,sipSpec *pt,FILE *fp)
{
	char *mname = pt -> module -> name;
	classDef *hcd;

	for (hcd = cd; hcd != NULL; hcd = hcd -> ecd)
		generateCppCodeBlock(hcd -> hdrcode,fp);

	if (genused)
		generateUsedIncludes(cd->iff->used, TRUE, fp);

	if (isOpaque(cd))
		prcode(fp,
"\n"
"class %S;\n"
			,classFQCName(cd));

	if (cd -> iff -> type != namespace_iface)
	{
		prcode(fp,
"\n"
"#define	sipClass_%C	sipModuleAPI_%s.em_types[%d]\n"
			,classFQCName(cd),mname,cd -> classnr);

		if (!isExternal(cd))
		{
			prcode(fp,
"#define	sipCast_%C	sipType_%C.td_cast\n"
"#define	sipForceConvertTo_%C	sipType_%C.td_fcto\n"
				,classFQCName(cd),classFQCName(cd)
				,classFQCName(cd),classFQCName(cd));

			if (cd -> convtocode != NULL)
				prcode(fp,
"#define	sipConvertTo_%C	sipType_%C.td_cto\n"
					,classFQCName(cd),classFQCName(cd));
		}
	}

	generateEnumMacros(pt, cd, fp);

	if (!isExternal(cd))
		prcode(fp,
"\n"
"extern sipTypeDef sipType_%C;\n"
			,classFQCName(cd));

	if (hasShadow(cd))
		generateShadowClassDeclaration(pt,cd,fp);
}


/*
 * Generate the sipEnum_* macros.
 */
static void generateEnumMacros(sipSpec *pt, classDef *cd, FILE *fp)
{
	enumDef *ed;
	int noIntro = TRUE;

	for (ed = pt -> enums; ed != NULL; ed = ed -> next)
	{
		if (ed -> fqcname == NULL || ed -> ecd != cd)
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"#define	sipEnum_%C	sipModuleAPI_%s", ed -> fqcname, pt -> module -> name);

		if (pt -> module == ed -> module)
			prcode(fp, ".");
		else
			prcode(fp, "_%s -> ", ed -> module -> name);

		prcode(fp, "em_enums[%d]\n"
			, ed -> enumnr);
	}
}


/*
 * Generate the shadow class declaration.
 */
static void generateShadowClassDeclaration(sipSpec *pt,classDef *cd,FILE *fp)
{
	int noIntro, nrVirts;
	ctorDef *ct;
	visibleList *vl;
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

	for (pcd = pt -> classes; pcd != NULL; pcd = pcd -> next)
	{
		if (pcd -> ecd != cd || !isProtectedClass(pcd))
			continue;

		prcode(fp,
"	class sip%s : public %s {};\n"
			,classBaseName(pcd),classBaseName(pcd));
	}

	/* The constructor declarations. */

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		ctorDef *dct;

		if (isPrivateCtor(ct))
			continue;

		/* Check we haven't already handled this C++ signature. */
		for (dct = cd -> ctors; dct != ct; dct = dct -> next)
			if (sameSignature(dct -> cppsig,ct -> cppsig,TRUE))
				break;

		if (dct != ct)
			continue;

		prcode(fp,
"	sip%C(",classFQCName(cd));

		generateArgs(ct -> cppsig,Declaration,fp);

		prcode(fp,")%X;\n"
			,ct -> exceptions);
	}

	/* The destructor. */

	if (!isPrivateDtor(cd))
		prcode(fp,
"	%s~sip%C()%X;\n"
			,(cd -> vmembers != NULL ? "virtual " : ""),classFQCName(cd),cd -> dtorexceptions);

	/* The exposure of protected enums. */

	generateProtectedEnums(pt,cd,fp);

	/* The wrapper around each protected member function. */

	generateProtectedDeclarations(cd,fp);

	/* The public wrapper around each signal emitter. */

	noIntro = TRUE;

	for (vl = cd -> visible; vl != NULL; vl = vl -> next)
	{
		overDef *od;

		for (od = vl -> cd -> overs; od != NULL; od = od -> next)
		{
			if (od -> common != vl -> m || !isSignal(od))
				continue;

			if (noIntro)
			{
				prcode(fp,
"\n"
"	/*\n"
"	 * There is a public method for every Qt signal that can be emitted\n"
"	 * by this object.  This function is called by Python to emit the\n"
"	 * signal.\n"
"	 */\n"
					);

				noIntro = FALSE;
			}

			prcode(fp,
"	int sipEmit_%s(PyObject *);\n"
				,vl -> m -> pyname -> text);

			break;
		}
	}

	/* The catcher around each virtual function in the hierarchy. */
	noIntro = TRUE;

	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
	{
		overDef *od = &vod -> o;
		virtOverDef *dvod;

		if (isPrivate(od))
			continue;

		/* Check we haven't already handled this C++ signature. */
		for (dvod = cd -> vmembers; dvod != vod; dvod = dvod -> next)
			if (strcmp(dvod -> o.cppname,od -> cppname) == 0 && sameSignature(dvod -> o.cppsig,od -> cppsig,TRUE))
				break;

		if (dvod != vod)
			continue;

		if (noIntro)
		{
			prcode(fp,
"\n"
"	/*\n"
"	 * There is a protected method for every virtual method visible from\n"
"	 * this class.\n"
"	 */\n"
"protected:\n"
				);

			noIntro = FALSE;
		}

		prcode(fp,
"	");
 
		normaliseArgs(od -> cppsig);

		generateResultType(&od -> cppsig -> result,fp);
 
		prcode(fp," %O(",od);
		generateArgs(od -> cppsig,Declaration,fp);
 
		prcode(fp,")%s%X;\n"
			,(isConst(od) ? " const" : ""),od -> exceptions);

		restoreArgs(od -> cppsig);
	}

	prcode(fp,
"\n"
"public:\n"
"	sipWrapper *sipPySelf;\n"
		);

	/* The private declarations. */

	prcode(fp,
"\n"
"private:\n"
"	sip%C(const sip%C &);\n"
"	sip%C &operator = (const sip%C &);\n"
		,classFQCName(cd),classFQCName(cd)
		,classFQCName(cd),classFQCName(cd));

	if ((nrVirts = countVirtuals(cd)) > 0)
		prcode(fp,
"\n"
"	sipMethodCache sipPyMethods[%d];\n"
			,nrVirts);

	prcode(fp,
"};\n"
		);
}


/*
 * Generate the return type of a member function.
 */
static void generateResultType(argDef *res,FILE *fp)
{
	generateSingleArg(res,-1,Declaration,fp);
}


/*
 * Generate typed arguments.
 */
static void generateArgs(signatureDef *sd,funcArgType ftype,FILE *fp)
{
	int a;

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		if (a > 0)
			prcode(fp,",");

		generateSingleArg(&sd -> args[a],a,ftype,fp);
	}
}


/*
 * Generate the declaration of a named variable to hold a result from a C++
 * function call.
 */
static void generateNamedValueType(argDef *ad,char *name,FILE *fp)
{
	argDef mod = *ad;

	if (ad -> nrderefs == 0)
		if (ad -> atype == class_type || ad -> atype == mapped_type)
			mod.nrderefs = 1;
		else
			resetIsConstArg(&mod);

	resetIsReference(&mod);
	generateNamedBaseType(&mod,name,fp);
}


/*
 * Generate a single argument.
 */
static void generateSingleArg(argDef *ad,int argnr,funcArgType ftype,FILE *fp)
{
	char name[50];
	int genType, genName, derefPtr;

	/* Break the type down to individual modifications. */

	genType = FALSE;
	genName = FALSE;
	derefPtr = FALSE;

	switch (ftype)
	{
	case Call:
		genName = TRUE;
		derefPtr = TRUE;
		break;

	case Declaration:
		genType = TRUE;
		break;

	case Definition:
		genType = TRUE;
		genName = TRUE;
		break;
	}

	if (genName)
	{
		char *ind = "";

		if (derefPtr)
			switch (ad -> atype)
			{
			case ustring_type:
			case string_type:
				if (ad -> nrderefs > (isOutArg(ad) ? 0 : 1))
					ind = "&";

				break;

			case mapped_type:
			case class_type:
				if (ad -> nrderefs == 2)
					ind = "&";
				else if (ad -> nrderefs == 0)
					ind = "*";

				break;

			case struct_type:
			case void_type:
				if (ad -> nrderefs == 2)
					ind = "&";

				break;

			default:
				if (ad -> nrderefs == 1)
					ind = "&";
			}

		sprintf(name,"%sa%d",ind,argnr);
	}
	else
		name[0] = '\0';

	if (genType)
		generateNamedBaseType(ad,name,fp);
	else if (genName)
		prcode(fp,name);
}


/*
 * Generate a C++ type.
 */
static void generateBaseType(argDef *ad,FILE *fp)
{
	generateNamedBaseType(ad,"",fp);
}


/*
 * Generate a C++ type and name.
 */
static void generateNamedBaseType(argDef *ad,char *name,FILE *fp)
{
	int nr_derefs = ad -> nrderefs;

	/*
	 * A function type is handled differently because of the position of
	 * the name.
	 */
	if (ad -> atype == function_type)
	{
		int i;
		signatureDef *sig = ad -> u.sa;

		generateBaseType(&sig -> result,fp);

		prcode(fp," (");

		for (i = 0; i < nr_derefs; ++i)
			prcode(fp,"*");

		prcode(fp,"%s)(",name);
		generateArgs(sig,Declaration,fp);
		prcode(fp,")");

		return;
	}

	if (isConstArg(ad))
		prcode(fp,"const ");

	switch (ad -> atype)
	{
	case ustring_type:
		prcode(fp,"unsigned char");
		break;

	case signal_type:
	case slot_type:
	case anyslot_type:
	case slotcon_type:
	case slotdis_type:
		nr_derefs = 1;

		/* Drop through. */

	case string_type:
		prcode(fp,"char");
		break;

	case ushort_type:
		prcode(fp,"unsigned short");
		break;

	case short_type:
		prcode(fp,"short");
		break;

	case uint_type:
		prcode(fp,"unsigned");
		break;

	case int_type:
	case cint_type:
		prcode(fp,"int");
		break;

	case ulong_type:
		prcode(fp,"unsigned long");
		break;

	case long_type:
		prcode(fp,"long");
		break;

	case ulonglong_type:
		prcode(fp,"unsigned long long");
		break;

	case longlong_type:
		prcode(fp,"long long");
		break;

	case struct_type:
		prcode(fp,"struct %S",ad -> u.sname);
		break;

	case void_type:
		prcode(fp,"void");
		break;

	case bool_type:
		prcode(fp,"bool");
		break;

	case float_type:
	case cfloat_type:
		prcode(fp,"float");
		break;

	case double_type:
	case cdouble_type:
		prcode(fp,"double");
		break;

	case defined_type:
		/*
		 * The only defined types still remaining are arguments to
		 * templates.
		 */

		prcode(fp,"%S",ad -> u.snd);
		break;

	case rxcon_type:
	case rxdis_type:
		nr_derefs = 1;
		prcode(fp,"QObject");
		break;

	case mapped_type:
		generateBaseType(&ad -> u.mtd -> type,fp);
		break;

	case class_type:
		prcode(fp,"%U",ad -> u.cd);
		break;

	case template_type:
		{
			static const char tail[] = ">";
			int a;
			templateDef *td = ad -> u.td;

			prcode(fp,"%S<",td -> fqname);

			for (a = 0; a < td -> types.nrArgs; ++a)
			{
				if (a > 0)
					prcode(fp,",");

				generateBaseType(&td -> types.args[a],fp);
			}

			if (prcode_last == tail)
				prcode(fp, " ");

			prcode(fp, tail);
			break;
		}

	case enum_type:
		prcode(fp,"%E",ad -> u.ed);
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
		prcode(fp,"PyObject *",fp);
		break;
	}

	if (nr_derefs > 0)
	{
		int i;

		prcode(fp," ");

		for (i = 0; i < nr_derefs; ++i)
			prcode(fp,"*");
	}

	if (isReference(ad))
		prcode(fp,"&");

	if (*name != '\0')
	{
		if (nr_derefs == 0)
			prcode(fp," ");

		prcode(fp,name);
	}
}


/*
 * Generate the definition of an argument variable and any supporting
 * variables.
 */
static void generateVariable(argDef *ad,int argnr,FILE *fp)
{
	argType atype = ad -> atype;
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
"		%B a%ddef = ",ad,argnr);

		generateExpression(ad->defval,fp);

		prcode(fp,";\n"
			);
	}

	/* Adjust the type so we have the type that will really handle it. */

	orig = *ad;

	switch (atype)
	{
	case ustring_type:
	case string_type:
		if (!isReference(ad))
			if (ad -> nrderefs == 2)
				ad -> nrderefs = 1;
			else if (ad -> nrderefs == 1 && isOutArg(ad))
				ad -> nrderefs = 0;

		break;

	case mapped_type:
	case class_type:
	case void_type:
	case struct_type:
		ad -> nrderefs = 1;
		break;

	default:
		ad -> nrderefs = 0;
	}

	/* Array sizes are always integers. */
	if (isArraySize(ad))
		ad -> atype = int_type;

	resetIsReference(ad);

	if (ad -> nrderefs == 0)
		resetIsConstArg(ad);

	prcode(fp,
"		%B a%d",ad,argnr);

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
			if (ad -> u.cd -> convtocode != NULL && !isConstrained(ad))
				prcode(fp,
"		int a%dIsTemp = 0;\n"
					,argnr);
			else if (isGetWrapper(ad))
				prcode(fp,
"		PyObject *a%dWrapper;\n"
					,argnr);

			break;

		case mapped_type:
			prcode(fp,
"		int a%dIsTemp = 0;\n"
				,argnr);
			break;

		case anyslot_type:
			prcode(fp,
"		PyObject *a%dCallable", argnr);
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

	for (i = 0; i < fcd -> nrArgs; ++i)
	{
		if (i > 0)
			prcode(fp,",");

		generateExpression(fcd -> args[i],fp);
	}

	prcode(fp,")");
}


/*
 * Generate the type structure that contains all the information needed by the
 * metatype.
 */
static void generateTypeDefinition(sipSpec *pt,classDef *cd,FILE *fp)
{
	char *mname = pt -> module -> name;
	int is_slots, nr_methods, nr_enums;
	int is_inst_class, is_inst_voidp, is_inst_char, is_inst_string;
	int is_inst_int, is_inst_long, is_inst_ulong, is_inst_longlong;
	int is_inst_ulonglong, is_inst_double, is_inst_enum;
	memberDef *md;

	if (cd -> supers != NULL)
	{
		classList *cl;

		prcode(fp,
"\n"
"\n"
"/* Define this type's super-types. */\n"
"static sipEncodedClassDef supers_%C[] = {",classFQCName(cd));

		for (cl = cd -> supers; cl != NULL; cl = cl -> next)
		{
			if (cl != cd -> supers)
				prcode(fp,", ");

			generateEncodedClass(pt,cl -> cd,(cl -> next == NULL),fp);
		}

		prcode(fp,"};\n"
			);
	}

	/* Generate the slots table. */
	is_slots = FALSE;

	for (md = cd -> members; md != NULL; md = md -> next)
	{
		const char *stype;

		if (md -> slot == no_slot)
			continue;

		if (!is_slots)
		{
			prcode(fp,
"\n"
"\n"
"/* Define this type's Python slots. */\n"
"static sipPySlotDef slots_%C[] = {\n"
				,classFQCName(cd));

			is_slots = TRUE;
		}

		if ((stype = slotName(md -> slot)) != NULL)
			prcode(fp,
"	{(void *)slot_%C_%s, %s},\n"
				,classFQCName(cd),md -> pyname -> text,stype);
	}

	if (is_slots)
		prcode(fp,
"	{0, (sipPySlotType)0}\n"
"};\n"
			);

	/* Generate the attributes tables. */
	nr_methods = generateMethodTable(cd,fp);
	nr_enums = generateEnumMemberTable(pt,cd,fp);

	/* Generate each instance table. */
	is_inst_class = generateClasses(pt,cd,fp);
	is_inst_voidp = generateVoidPointers(pt,cd,fp);
	is_inst_char = generateChars(pt,cd,fp);
	is_inst_string = generateStrings(pt,cd,fp);
	is_inst_int = generateInts(pt,cd,fp);
	is_inst_long = generateLongs(pt,cd,fp);
	is_inst_ulong = generateUnsignedLongs(pt,cd,fp);
	is_inst_longlong = generateLongLongs(pt,cd,fp);
	is_inst_ulonglong = generateUnsignedLongLongs(pt,cd,fp);
	is_inst_double = generateDoubles(pt,cd,fp);
	is_inst_enum = generateEnums(pt,cd,fp);

	prcode(fp,
"\n"
"\n"
"/* The main type data structure. */\n"
"sipTypeDef sipType_%C = {\n"
"	0,\n"
"	%s,\n"
"	\"%s.%P\",\n"
		,classFQCName(cd)
		,(isAbstractClass(cd) ? "SIP_TYPE_ABSTRACT" : "0")
		,mname,cd -> ecd,cd -> pyname);

	if (isRenamedClass(cd))
		prcode(fp,
"	\"%S\",\n"
			, classFQCName(cd));
	else
		prcode(fp,
"	NULL,\n"
			);

	prcode(fp,
"	%d,\n"
		,(cd -> ecd != NULL) ? cd -> ecd -> classnr : -1);

	if (cd -> supers != NULL)
		prcode(fp,
"	supers_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (is_slots)
		prcode(fp,
"	slots_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (nr_methods == 0)
		prcode(fp,
"	0, 0,\n"
			);
	else
		prcode(fp,
"	%d, methods_%C,\n"
			,nr_methods,classFQCName(cd));

	if (nr_enums == 0)
		prcode(fp,
"	0, 0,\n"
			);
	else
		prcode(fp,
"	%d, enummembers_%C,\n"
			,nr_enums,classFQCName(cd));

	if (hasVarHandlers(cd))
		prcode(fp,
"	variables_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (canCreate(cd))
		prcode(fp,
"	init_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->travcode != NULL)
		prcode(fp,
"	traverse_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->clearcode != NULL)
		prcode(fp,
"	clear_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->readbufcode != NULL)
		prcode(fp,
"	getreadbuffer_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->writebufcode != NULL)
		prcode(fp,
"	getwritebuffer_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->segcountcode != NULL)
		prcode(fp,
"	getsegcount_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd->charbufcode != NULL)
		prcode(fp,
"	getcharbuffer_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (needDealloc(cd))
		prcode(fp,
"	dealloc_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	if (cd -> iff -> type == namespace_iface || generating_c)
		prcode(fp,
"	0,\n"
			);
	else
		prcode(fp,
"	cast_%C,\n"
			,classFQCName(cd));

	if (cd -> iff -> type == namespace_iface)
		prcode(fp,
"	0,\n"
"	0,\n"
			);
	else
	{
		prcode(fp,
"	forceConvertTo_%C,\n"
			,classFQCName(cd));

		if (cd -> convtocode != NULL)
			prcode(fp,
"	convertTo_%C,\n"
				,classFQCName(cd));
		else
			prcode(fp,
"	0,\n"
				);
	}

	if (hasSigSlots(cd))
		prcode(fp,
"	signals_%C,\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	0,\n"
			);

	prcode(fp,
"	{");

	if (is_inst_class)
		prcode(fp,"classInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_voidp)
		prcode(fp,"voidPtrInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_char)
		prcode(fp,"charInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_string)
		prcode(fp,"stringInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_int)
		prcode(fp,"intInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_long)
		prcode(fp,"longInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_ulong)
		prcode(fp,"unsignedLongInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_longlong)
		prcode(fp,"longLongInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_ulonglong)
		prcode(fp,"unsignedLongLongInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_double)
		prcode(fp,"doubleInstances_%C, ",classFQCName(cd));
	else
		prcode(fp,"0, ");

	if (is_inst_enum)
		prcode(fp,"enumInstances_%C",classFQCName(cd));
	else
		prcode(fp,"0");

	prcode(fp,"}\n"
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
static void generateTypeInit(sipSpec *pt, classDef *cd, FILE *fp)
{
	ctorDef *ct;
	int need_owner;

	/*
	 * See if we need to name the owner argument so that we can avoid a
	 * compiler warning about an unused argument.
	 */
	need_owner = generating_c;

	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		int a;

		for (a = 0; a < ct -> pysig.nrArgs; ++a)
			if (isThisTransferred(&ct -> pysig.args[a]))
			{
				need_owner = TRUE;
				break;
			}

		if (need_owner)
			break;
	}

	prcode(fp,
"\n"
"\n"
"static void *init_%C(sipWrapper *%s,PyObject *sipArgs,sipWrapper **%s,int *sipArgsParsed)\n"
"{\n"
		,classFQCName(cd),((generating_c || hasShadow(cd)) ? "sipSelf" : ""),(need_owner ? "sipOwner" : ""));

	if (hasShadow(cd))
		prcode(fp,
"	sip%C *sipCpp = 0;\n"
			,classFQCName(cd));
	else
		prcode(fp,
"	%U *sipCpp = 0;\n"
			,cd);

	if (tracing)
		prcode(fp,
"\n"
"	sipTrace(SIP_TRACE_INITS,\"init_%C()\\n\");\n"
			,classFQCName(cd));

	/*
	 * Generate the code that parses the Python arguments and calls the
	 * correct constructor.
	 */
	for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
	{
		int needSecCall, error_flag = FALSE;

		if (isPrivateCtor(ct))
			continue;

		prcode(fp,
"\n"
"	if (!sipCpp)\n"
"	{\n"
			);

		if (ct -> methodcode != NULL && needErrorFlag(ct -> methodcode))
		{
			prcode(fp,
"		int sipIsErr = 0;\n"
				);

			error_flag = TRUE;
		}

		needSecCall = generateArgParser(pt, &ct->pysig, cd, ct, NULL, FALSE, fp);
		generateConstructorCall(cd,ct,error_flag,fp);

		if (needSecCall)
		{
				prcode(fp,
"	}\n"
"\n"
"	if (!sipCpp)\n"
"	{\n"
				);

			if (error_flag)
				prcode(fp,
"		int sipIsErr = 0;\n"
					);

			generateArgParser(pt, &ct->pysig, cd, ct, NULL, TRUE, fp);
			generateConstructorCall(cd,ct,error_flag,fp);
		}

		prcode(fp,
"	}\n"
			);
	}

	if (hasShadow(cd))
		prcode(fp,
"\n"
"	if (sipCpp)\n"
"		sipCpp -> sipPySelf = sipSelf;\n"
			,classFQCName(cd));

	prcode(fp,
"\n"
"	return sipCpp;\n"
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
 
	for (vod = cd -> vmembers; vod != NULL; vod = vod -> next)
		if (!isPrivate(&vod -> o))
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
	if (exceptions && (ta == NULL || ta -> nrArgs > 0))
		prcode(fp,
"			try\n"
"			{\n"
			);
}


/*
 * Generate the catch block for a call.
 */
static void generateCatch(throwArgs *ta,FILE *fp)
{
	/*
	 * Generate the block if there was no throw specifier, or a non-empty
	 * throw specifier.
	 */
	if (exceptions && (ta == NULL || ta -> nrArgs > 0))
	{
		prcode(fp,
"			}\n"
			);

		if (ta == NULL)
		{
			prcode(fp,
"			catch (...)\n"
"			{\n"
				);

			if (release_gil)
				prcode(fp,
"				Py_BLOCK_THREADS\n"
"\n"
					);

			prcode(fp,
"				sipRaiseUnknownException();\n"
"				return NULL;\n"
"			}\n"
				);
		}
		else
		{
			int a;

			for (a = 0; a < ta -> nrArgs; ++a)
			{
				exceptionDef *xd = ta->args[a];
				scopedNameDef *ename = xd->iff->fqcname;

				prcode(fp,
"			catch (%S &%s)\n"
"			{\n"
					,ename,(xd->cd != NULL || usedInCode(xd->raisecode, "sipExceptionRef")) ? "sipExceptionRef" : "");

				if (xd->cd != NULL)
				{
					/* The exception is a wrapped class. */

					prcode(fp,
"				/* Hope that there is a valid copy ctor. */\n"
"				%S *sipExceptionCopy = new %S(sipExceptionRef);\n"
"\n"
						,ename,ename);

					if (release_gil)
						prcode(fp,
"				Py_BLOCK_THREADS\n"
"\n"
							);

					prcode(fp,
"				sipRaise%sClassException(sipClass_%C,sipExceptionCopy);\n"
						,(xd->cd->subbase != NULL ? "Sub" : ""),ename);
				}
				else
					generateCppCodeBlock(xd->raisecode,fp);

				prcode(fp,
"\n"
"				return NULL;\n"
"			}\n"
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

		for (a = 0; a < ta -> nrArgs; ++a)
		{
			if (a > 0)
				prcode(fp,",");

			prcode(fp,"%S",ta -> args[a] -> iff -> fqcname);
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
"		{\n"
		);

	/* Call any pre-hook. */
	if (ct -> prehook != NULL)
		prcode(fp,
"			sipCallHook(\"%s\");\n"
"\n"
			,ct -> prehook);

	if (ct -> methodcode != NULL)
		generateCppCodeBlock(ct -> methodcode,fp);
	else if (generating_c)
		prcode(fp,
"			sipCpp = sipMalloc(sizeof (%S));\n"
			,classFQCName(cd));
	else
	{
		if (release_gil || isReleaseGILCtor(ct))
			prcode(fp,
"			Py_BEGIN_ALLOW_THREADS\n"
				);

		generateTry(ct -> exceptions,fp);

		if (hasShadow(cd))
			prcode(fp,
"			sipCpp = new sip%C(",classFQCName(cd));
		else
			prcode(fp,
"			sipCpp = new %U(",cd);

		if (isCastCtor(ct))
			prcode(fp, "a0 -> operator %S()", classFQCName(cd));
		else
			generateArgs(ct->cppsig, Call, fp);

		prcode(fp,");\n"
			);

		generateCatch(ct -> exceptions,fp);

		if (release_gil || isReleaseGILCtor(ct))
			prcode(fp,
"			Py_END_ALLOW_THREADS\n"
				);
	}

	gc_ellipsis(&ct->pysig, fp);

	deleteTemps(&ct -> pysig, fp);

	if (error_flag)
		prcode(fp,
"\n"
"			if (sipIsErr)\n"
"				return 0;\n"
			);

	/* Call any post-hook. */
	if (ct -> posthook != NULL)
		prcode(fp,
"\n"
"			sipCallHook(\"%s\");\n"
			,ct -> posthook);

	prcode(fp,
"		}\n"
		);
}


/*
 * See if a member overload should be skipped.
 */
static int skipOverload(overDef *od,memberDef *md,classDef *cd,classDef *ccd,
			int want_local)
{
	/* Skip if it's not the right name. */
	if (od -> common != md)
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
static void generateFunction(sipSpec *pt,memberDef *md,overDef *overs,
			     classDef *cd,classDef *ocd,FILE *fp)
{
	overDef *od;
	int need_method, need_self, need_args;

	/*
	 * Check that there is at least one overload that needs to be handled.
	 * Also see if we can avoid naming the "self" argument (and suppress a
	 * compiler warning).
	 */
	need_method = need_self = need_args = FALSE;

	for (od = overs; od != NULL; od = od -> next)
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
					need_self = TRUE;
			}
		}
	}

	if (need_method)
	{
		char *pname = md -> pyname -> text;

		prcode(fp,
"\n"
"\n"
"static PyObject *meth_%C_%s(PyObject *%s,PyObject *%s)\n"
"{\n"
			,classFQCName(cd),pname,(need_self ? "sipSelf" : ""),(need_args ? "sipArgs" : ""));

		if (tracing)
			prcode(fp,
"	sipTrace(SIP_TRACE_METHODS,\"meth_%C_%s()\\n\");\n"
"\n"
				,classFQCName(cd),pname);

		if (need_args)
			prcode(fp,
"	int sipArgsParsed = 0;\n"
				);

		for (od = overs; od != NULL; od = od -> next)
		{
			/*
			 * If we are handling one variant then we must handle
			 * them all.
			 */
			if (skipOverload(od,md,cd,ocd,FALSE))
				continue;

			if (isPrivate(od))
				continue;

			generateFunctionBody(pt,od,cd,ocd,fp);
		}

		prcode(fp,
"\n"
"	/* Raise an exception if the arguments couldn't be parsed. */\n"
"	sipNoMethod(%s,%N,%N);\n"
"\n"
"	return NULL;\n"
"}\n"
			,(need_args ? "sipArgsParsed" : "0"),cd -> iff -> name,md -> pyname);
	}
}


/*
 * Generate the function calls for a particular overload.
 */
static void generateFunctionBody(sipSpec *pt,overDef *od,classDef *cd,
				 classDef *ocd,FILE *fp)
{
	int needSecCall;
	signatureDef saved;

	prcode(fp,
"\n"
"	{\n"
		);

	/* In case we have to fiddle with it. */
	saved = od->pysig;

	if (isNumberSlot(od -> common))
	{
		/*
		 * Number slots must have two arguments because we parse them
		 * slightly differently.
		 */
		if (od->pysig.nrArgs == 1)
		{
			od->pysig.nrArgs = 2;
			od->pysig.args[1] = od->pysig.args[0];

			/* Insert self as the first argument. */
			od->pysig.args[0].atype = class_type;
			od->pysig.args[0].name = NULL;
			od->pysig.args[0].argflags = ARG_IS_REF|ARG_IN;
			od->pysig.args[0].nrderefs = 0;
			od->pysig.args[0].defval = NULL;
			od->pysig.args[0].u.cd = ocd;
		}

		generateArgParser(pt, &od->pysig, cd, NULL, od, FALSE, fp);
		needSecCall = FALSE;
	}
	else if (isIntArgSlot(od -> common) || isZeroArgSlot(od -> common))
		needSecCall = FALSE;
	else
		needSecCall = generateArgParser(pt, &od->pysig, cd, NULL, od, FALSE, fp);

	generateFunctionCall(cd,ocd,od,fp);

	if (needSecCall)
	{
		prcode(fp,
"	}\n"
"\n"
"	{\n"
			);

		generateArgParser(pt, &od->pysig, cd, NULL, od, TRUE, fp);
		generateFunctionCall(cd,ocd,od,fp);
	}

	prcode(fp,
"	}\n"
		);

	od->pysig = saved;
}


/*
 * Generate the code to handle the result of a call to a member function.
 */
static void generateHandleResult(overDef *od,int isNew,char *prefix,FILE *fp)
{
	char *vname, vnamebuf[50];
	int a, nrvals, only, has_owner;
	argDef *res, *ad;

	res = &od -> pysig.result;

	if (res -> atype == void_type && res -> nrderefs == 0)
		res = NULL;

	/* See if we are returning 0, 1 or more values. */
	nrvals = 0;

	if (res != NULL)
	{
		only = -1;
		++nrvals;
	}

	has_owner = FALSE;

	for (a = 0; a < od -> pysig.nrArgs; ++a)
	{
		if (isOutArg(&od -> pysig.args[a]))
		{
			only = a;
			++nrvals;
		}

		if (isThisTransferred(&od -> pysig.args[a]))
			has_owner = TRUE;
	}

	/* Handle the trivial case. */
	if (nrvals == 0)
	{
		prcode(fp,
"			Py_INCREF(Py_None);\n"
"			%s Py_None;\n"
			,prefix);

		return;
	}

	/* Handle results that are classes or mapped types separately. */
	if (res != NULL)
		if (res -> atype == mapped_type)
		{
			prcode(fp,
"			PyObject *sipResObj = sipConvertFrom_%T(",res);

			if (isConstArg(res))
				prcode(fp,"const_cast<%b *>(sipRes)",res);
			else
				prcode(fp,"sipRes");

			prcode(fp,");\n"
				);

			if (isNew)
				prcode(fp,
"			delete sipRes;\n"
					);

			/* Shortcut if this is the only value returned. */
			if (nrvals == 1)
			{
				prcode(fp,
"\n"
"			%s sipResObj;\n"
					,prefix);

				return;
			}
		}
		else if (res -> atype == class_type)
		{
			classDef *cd = res -> u.cd;

			if (isNew || isFactory(od))
			{
				prcode(fp,
"			%s sipNewCppToSelf%s(",(nrvals == 1 ? prefix : "PyObject *sipResObj ="),(cd -> subbase != NULL ? "SubClass" : ""));

				if (isConstArg(res))
					prcode(fp,"const_cast<%b *>(sipRes)",res);
				else
					prcode(fp,"sipRes");

				prcode(fp,",sipClass_%C,%s);\n"
					,classFQCName(cd),((has_owner && isFactory(od)) ? "sipOwner" : "NULL"));

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
					return;
			}
			else if (isResultTransferredBack(od))
			{
				prcode(fp,
"			PyObject *sipResObj = sipMapCppToSelf%s(",(cd -> subbase != NULL ? "SubClass" : ""));

				if (isConstArg(res))
					prcode(fp,"const_cast<%b *>(sipRes)",res);
				else
					prcode(fp,"sipRes");

				prcode(fp,",sipClass_%C);\n"
					,classFQCName(cd));

				prcode(fp,
"			sipTransferBack(sipResObj);\n"
					);

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
				{
					prcode(fp,
"\n"
"			%s sipResObj;\n"
						,prefix);

					return;
				}
			}
			else
			{
				prcode(fp,
"			%s sipMapCppToSelf%s(",(nrvals == 1 ? prefix : "PyObject *sipResObj ="),(cd -> subbase != NULL ? "SubClass" : ""));

				if (isConstArg(res))
					prcode(fp,"const_cast<%b *>(sipRes)",res);
				else
					prcode(fp,"sipRes");

				prcode(fp,",sipClass_%C);\n"
					,classFQCName(cd));

				/*
				 * Shortcut if this is the only value returned.
				 */
				if (nrvals == 1)
					return;
			}
		}

	/* If there are multiple values then build a tuple. */
	if (nrvals > 1)
	{
		prcode(fp,
"			%s sipBuildResult(0,\"(",prefix);

		/* Build the format string. */
		if (res != NULL)
			prcode(fp,"%c",((res -> atype == mapped_type || res -> atype == class_type) ? 'R' : getBuildResultFormat(res)));

		for (a = 0; a < od -> pysig.nrArgs; ++a)
		{
			argDef *ad = &od -> pysig.args[a];

			if (isOutArg(ad))
				prcode(fp,"%c",getBuildResultFormat(ad));
		}

		prcode(fp,")\"");

		/* Pass the values for conversion. */
		if (res != NULL)
		{
			prcode(fp,",sipRes");

			if (res -> atype == mapped_type || res -> atype == class_type)
				prcode(fp,"Obj");
			else if (res -> atype == enum_type && res -> u.ed -> fqcname != NULL)
				prcode(fp,",sipEnum_%C",res -> u.ed -> fqcname);
		}

		for (a = 0; a < od -> pysig.nrArgs; ++a)
		{
			argDef *ad = &od -> pysig.args[a];

			if (isOutArg(ad))
			{
				prcode(fp,",a%d",a);

				if (ad -> atype == mapped_type)
					prcode(fp,",sipConvertFrom_%T",ad);
				else if (ad -> atype == class_type)
					prcode(fp,",sipClass_%C",classFQCName(ad -> u.cd));
				else if (ad -> atype == enum_type && ad -> u.ed -> fqcname != NULL)
					prcode(fp,",sipEnum_%C",ad -> u.ed -> fqcname);
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
		ad = &od -> pysig.args[only];

		sprintf(vnamebuf,"a%d",only);
		vname = vnamebuf;
	}

	switch (ad -> atype)
	{
	case mapped_type:
		prcode(fp,
"			%s sipConvertFrom_%T(",prefix,ad);

		if (isConstArg(ad))
			prcode(fp,"const_cast<%b *>(%s)",ad,vname);
		else
			prcode(fp,"%s",vname);

		prcode(fp,");\n"
			);
		break;

	case class_type:
		{
			classDef *cd = ad -> u.cd;
			int needNew = needNewInstance(ad);

			if (needNew)
				prcode(fp,
"			%s sipNewCppToSelf(",prefix);
			else
				prcode(fp,
"			%s sipMapCppToSelf%s(",prefix,(cd -> subbase != NULL ? "SubClass" : ""));

			if (isConstArg(ad))
				prcode(fp,"const_cast<%b *>(%s)",ad,vname);
			else
				prcode(fp,"%s",vname);

			prcode(fp,",sipClass_%C",classFQCName(cd));

			if (needNew)
				prcode(fp,",NULL");

			prcode(fp,");\n"
				);
		}

		break;

	case bool_type:
		prcode(fp,
"			%s PyBool_FromLong(%s);\n"
			,prefix,vname);

		break;

	case ustring_type:
	case string_type:
		if (ad -> nrderefs == 0)
			prcode(fp,
"			%s PyString_FromStringAndSize(%s&%s,1);\n"
				,prefix,(ad -> atype == ustring_type) ? "(char *)" : "",vname);
		else
			prcode(fp,
"			if (%s == NULL)\n"
"			{\n"
"				Py_INCREF(Py_None);\n"
"				return Py_None;\n"
"			}\n"
"\n"
"			%s PyString_FromString(%s%s);\n"
			,vname
			,prefix,(ad -> atype == ustring_type) ? "(char *)" : "",vname);

		break;

	case enum_type:
		if (ad -> u.ed -> fqcname != NULL)
		{
			prcode(fp,
"			%s sipConvertFromNamedEnum(%s,sipEnum_%C);\n"
				,prefix,vname,ad -> u.ed -> fqcname);

			break;
		}

		/* Drop through. */

	case ushort_type:
	case short_type:
	case uint_type:
	case int_type:
	case cint_type:
		prcode(fp,
"			%s PyInt_FromLong(%s);\n"
			,prefix,vname);

		break;

	case long_type:
		prcode(fp,
"			%s PyLong_FromLong(%s);\n"
			,prefix,vname);

		break;

	case ulong_type:
		prcode(fp,
"			%s PyLong_FromUnsignedLong(%s);\n"
			,prefix,vname);

		break;

	case longlong_type:
		prcode(fp,
"			%s PyLong_FromLongLong(%s);\n"
			,prefix,vname);

		break;

	case ulonglong_type:
		prcode(fp,
"			%s PyLong_FromUnsignedLongLong(%s);\n"
			,prefix,vname);

		break;

	case void_type:
		prcode(fp,
"			%s sipConvertFromVoidPtr(%s);\n"
			,prefix,vname);

		break;

	case struct_type:
		prcode(fp,
"			%s sipConvertFromVoidPtr(",prefix);

		if (isConstArg(ad))
			prcode(fp,"const_cast<%b *>(%s)",ad,vname);
		else
			prcode(fp,"%s",vname);

		prcode(fp,");\n"
			);

		break;

	case float_type:
	case cfloat_type:
		prcode(fp,
"			%s PyFloat_FromDouble((double)%s);\n"
			,prefix,vname);

		break;

	case double_type:
	case cdouble_type:
		prcode(fp,
"			%s PyFloat_FromDouble(%s);\n"
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
"			%s %s;\n"
			,prefix,vname);

		break;
	}
}


/*
 * Return the format character used by sipBuildResult() for a particular type.
 */
static char getBuildResultFormat(argDef *ad)
{
	switch (ad -> atype)
	{
	case mapped_type:
		return 'T';

	case class_type:
		if (needNewInstance(ad))
			return 'P';

		return (ad -> u.cd -> subbase != NULL) ? 'M' : 'O';

	case bool_type:
		return 'b';

	case ustring_type:
	case string_type:
		return (ad -> nrderefs > (isOutArg(ad) ? 1 : 0)) ? 's' : 'c';

	case enum_type:
		return (ad -> u.ed -> fqcname != NULL) ? 'E' : 'e';

	case ushort_type:
	case short_type:
		return 'h';

	case uint_type:
	case int_type:
	case cint_type:
		return 'i';

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
				 FILE *fp)
{
	int needsNew, error_flag = FALSE, newline, is_result, a, deltemps;
	argDef *res = &od -> pysig.result, orig_res;

	prcode(fp,
"		{\n"
		);

	/*
	 * If there is no shadow class then protected methods can never be
	 * called.
	 */
	if (isProtected(od) && !hasShadow(cd))
	{
		prcode(fp,
"			/* Never reached. */\n"
"		}\n"
			);

		return;
	}

	/* Save the full result type as we may want to fiddle with it. */
	orig_res = *res;

	/* See if we need to make a copy of the result on the heap. */
	if ((res -> atype == class_type || res -> atype == mapped_type) &&
	    !isReference(res) &&
	    res -> nrderefs == 0)
	{
		needsNew = TRUE;
		resetIsConstArg(res);
	}
	else
		needsNew = FALSE;

	/* See if sipRes is needed. */
	is_result = (!isInplaceNumberSlot(od -> common) &&
		     !isInplaceSequenceSlot(od -> common) &&
		     (res -> atype != void_type || res -> nrderefs != 0));

	newline = FALSE;

	if (is_result)
	{
		prcode(fp,
"			");

		generateNamedValueType(res,"sipRes",fp);

		/*
		 * The typical %MethodCode usually causes a compiler warning,
		 * so we initialise the result in that case to try and suppress
		 * it.
		 */
		if (od -> methodcode != NULL)
		{
			prcode(fp," = ");

			generateCastZero(res,fp);
		}

		prcode(fp,";\n"
			);

		newline = TRUE;
	}

	deltemps = TRUE;

	for (a = 0; a < od -> pysig.nrArgs; ++a)
	{
		argDef *ad = &od -> pysig.args[a];

		/*
		 * If we have an In,Out argument that has conversion code then
		 * we delay the destruction of any temporary variables until
		 * after we have converted the outputs.
		 */
		if (isInArg(ad) && isOutArg(ad) && hasConvertToCode(ad) && deltemps)
		{
			deltemps = FALSE;

			prcode(fp,
"			PyObject *sipResult;\n"
				);

			newline = TRUE;
		}

		/*
		 * If we are returning a class via an output only reference or
		 * pointer then we need an instance on the heap.
		 */
		if (needNewInstance(ad))
		{
			prcode(fp,
"			a%d = new %b();\n"
				,a,ad);

			newline = TRUE;
		}
	}

	if (od -> methodcode != NULL)
	{
		/*
		 * See if the handwritten code seems to be using the error
		 * flag.
		 */
		if (needErrorFlag(od -> methodcode))
		{
			prcode(fp,
"			int sipIsErr = 0;\n"
				);

			newline = TRUE;
			error_flag = TRUE;
		}
	}

	if (newline)
		prcode(fp,
"\n"
			);

	/* Call any pre-hook. */
	if (od -> prehook != NULL)
		prcode(fp,
"			sipCallHook(\"%s\");\n"
"\n"
			,od -> prehook);

	if (od -> methodcode != NULL)
		generateCppCodeBlock(od -> methodcode,fp);
	else
	{
		if (needsNew && generating_c)
		{
			prcode(fp,
"			if ((sipRes = (%b *)sipMalloc(sizeof (%b))) == NULL)\n"
"		{\n"
				,res,res);

			gc_ellipsis(&od->pysig, fp);

			prcode(fp,
"				return NULL;\n"
"			}\n"
"\n"
				);
		}

		if (release_gil || isReleaseGIL(od))
			prcode(fp,
"			Py_BEGIN_ALLOW_THREADS\n"
				);

		generateTry(od -> exceptions,fp);

		prcode(fp,
"			");

		if (od -> common -> slot != cmp_slot && is_result)
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
			if ((res -> atype == class_type || res -> atype == mapped_type) && isReference(res))
				prcode(fp,"&");
		}

		switch (od -> common -> slot)
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
			generateArgs(od -> cppsig,Call,fp);
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
			generateBinarySlotCall(od,"+",fp);
			break;

		case sub_slot:
			generateNumberSlotCall(od,"-",fp);
			break;

		case mul_slot:
			generateNumberSlotCall(od,"*",fp);
			break;

		case repeat_slot:
			generateBinarySlotCall(od,"*",fp);
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
			generateBinarySlotCall(od,"+=",fp);
			break;

		case isub_slot:
			generateBinarySlotCall(od,"-=",fp);
			break;

		case imul_slot:
		case irepeat_slot:
			generateBinarySlotCall(od,"*=",fp);
			break;

		case idiv_slot:
			generateBinarySlotCall(od,"/=",fp);
			break;

		case imod_slot:
			generateBinarySlotCall(od,"%=",fp);
			break;

		case iand_slot:
			generateBinarySlotCall(od,"&=",fp);
			break;

		case ior_slot:
			generateBinarySlotCall(od,"|=",fp);
			break;

		case ixor_slot:
			generateBinarySlotCall(od,"^=",fp);
			break;

		case ilshift_slot:
			generateBinarySlotCall(od,"<<=",fp);
			break;

		case irshift_slot:
			generateBinarySlotCall(od,">>=",fp);
			break;

		case invert_slot:
			prcode(fp, "~(*sipCpp)");
			break;

		case lt_slot:
			generateBinarySlotCall(od,"<",fp);
			break;

		case le_slot:
			generateBinarySlotCall(od,"<=",fp);
			break;

		case eq_slot:
			generateBinarySlotCall(od,"==",fp);
			break;

		case ne_slot:
			generateBinarySlotCall(od,"!=",fp);
			break;

		case gt_slot:
			generateBinarySlotCall(od,">",fp);
			break;

		case ge_slot:
			generateBinarySlotCall(od,">=",fp);
			break;

		case neg_slot:
			prcode(fp, "-(*sipCpp)");
			break;

		case pos_slot:
			prcode(fp, "+(*sipCpp)");
			break;

		case cmp_slot:
			prcode(fp,"if ");
			generateBinarySlotCall(od,"<",fp);
			prcode(fp,"\n"
"				sipRes = -1;\n"
"			else if ");
			generateBinarySlotCall(od,">",fp);
			prcode(fp,"\n"
"				sipRes = 1;\n"
"			else\n"
"				sipRes = 0");

			break;
		}

		if (needsNew && !generating_c)
			prcode(fp,")");

		prcode(fp,";\n"
			);

		generateCatch(od -> exceptions,fp);

		if (release_gil || isReleaseGIL(od))
			prcode(fp,
"			Py_END_ALLOW_THREADS\n"
				);
	}

	/* Handle /TransferThis/ for non-factory methods. */
	if (!isFactory(od))
		for (a = 0; a < od -> pysig.nrArgs; ++a)
		{
			argDef *ad = &od -> pysig.args[a];

			if (!isInArg(ad))
				continue;

			if (isThisTransferred(ad))
			{
				prcode(fp,
"\n"
"			if (sipOwner)\n"
"				sipTransferTo(sipSelf, (PyObject *)sipOwner);\n"
"			else\n"
"				sipTransferBack(sipSelf);\n"
				);

				break;
			}
		}

	gc_ellipsis(&od->pysig, fp);

	if (deltemps)
		deleteTemps(&od->pysig, fp);

	prcode(fp,
"\n"
		);

	/* Handle sipIsErr if it was used. */
	if (error_flag)
		prcode(fp,
"			if (sipIsErr)\n"
"				return %s;\n"
"\n"
			,((isVoidReturnSlot(od -> common) || isIntReturnSlot(od -> common)) ? "-1" : "0"));

	/* Call any post-hook. */
	if (od -> posthook != NULL)
		prcode(fp,
"\n"
"			sipCallHook(\"%s\");\n"
			,od -> posthook);

	if (isVoidReturnSlot(od -> common))
		prcode(fp,
"			return 0;\n"
			);
	else if (isInplaceNumberSlot(od -> common) || isInplaceSequenceSlot(od -> common))
		prcode(fp,
"			Py_INCREF(sipSelf);\n"
"			return sipSelf;\n"
			);
	else if (isIntReturnSlot(od -> common))
		prcode(fp,
"			return sipRes;\n"
			);
	else
	{
		generateHandleResult(od,needsNew,(deltemps ? "return" : "sipResult ="),fp);

		/* Delete the temporaries now if we haven't already done so. */
		if (!deltemps)
		{
			deleteTemps(&od->pysig, fp);

			prcode(fp,
"\n"
"			return sipResult;\n"
				);
		}
	}

	prcode(fp,
"		}\n"
		);

	/* Restore the full type of the result. */
	*res = orig_res;
}


/*
 * Generate a call to a C++ function.
 */
static void generateCppFunctionCall(classDef *cd,classDef *ocd,overDef *od,
				    FILE *fp)
{
	char *mname = od -> cppname;

	/*
	 * If the function is protected then call the public wrapper.
	 * Otherwise explicitly call the real function and not any version in
	 * the wrapper class in case it is virtual.  This will prevent virtual
	 * loops.  The exception is pure virtual functions as this is the way
	 * we hook into the catcher.  Don't need to worry about indirected
	 * objects for protected functions.
	 */

	if (cd == NULL)
		prcode(fp,"%s(",mname);
	else if (cd -> iff -> type == namespace_iface)
		prcode(fp,"%S::%s(",classFQCName(cd),mname);
	else if (isStatic(od))
	{
		if (isProtected(od))
			prcode(fp,"sip%C::sipProtect_%s(",classFQCName(cd),mname);
		else
			prcode(fp,"%S::%s(",classFQCName(ocd),mname);
	}
	else if (isProtected(od))
		prcode(fp,"sipCpp -> sipProtect_%s(",mname);
	else if (isAbstract(od))
		prcode(fp,"sipCpp -> %s(",mname);
	else
		prcode(fp,"sipCpp -> %U::%s(",ocd,mname);

	generateArgs(od -> cppsig,Call,fp);

	prcode(fp,")");
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
 * Generate the call to a binary (non-number) slot method.
 */
static void generateBinarySlotCall(overDef *od, char *op, FILE *fp)
{
	prcode(fp, "((*sipCpp) %s ", op);
	generateSlotArg(&od->pysig, 0, fp);
	prcode(fp, ")");
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
static int generateArgParser(sipSpec *pt, signatureDef *sd, classDef *cd,
			     ctorDef *ct, overDef *od, int secCall, FILE *fp)
{
	char *mname = pt -> module -> name;
	int a, isQtSlot, optargs, arraylenarg, sigarg, handle_self;
	int slotconarg, slotdisarg, need_owner;

	/* If the class is just a namespace, then ignore it. */

	if (cd != NULL && cd -> iff -> type == namespace_iface)
		cd = NULL;

	handle_self = (od != NULL && od -> common -> slot == no_slot && !isStatic(od) && cd != NULL);

	/* Assume there isn't a Qt slot. */

	isQtSlot = FALSE;

	/*
	 * Generate the local variables that will hold the parsed arguments
	 * and values returned via arguments.
	 */

	sigarg = -1;
	need_owner = FALSE;

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		argDef *ad = &sd -> args[a];

		switch (ad -> atype)
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

		generateVariable(ad,a,fp);

		if (isThisTransferred(ad))
			need_owner = TRUE;
	}

	if (od != NULL && need_owner)
		prcode(fp,
"		sipWrapper *sipOwner;\n"
			);

	if (handle_self)
	{
		if (isProtected(od) && hasShadow(cd))
			prcode(fp,
"		sip%C *sipCpp;\n"
				,classFQCName(cd));
		else
			prcode(fp,
"		%U *sipCpp;\n"
				,cd);

		prcode(fp,
"\n"
			);
	}
	else if (sd -> nrArgs != 0)
		prcode(fp,
"\n"
			);

	/* Generate the call to the parser function. */
	if (od != NULL && isNumberSlot(od->common))
		prcode(fp,
"		if (sipParsePair(%ssipArgsParsed,sipArg0,sipArg1,\"", (ct != NULL ? "" : "&"));
	else
		prcode(fp,
"		if (sipParseArgs(%ssipArgsParsed,sipArg%s,\"", (ct != NULL ? "" : "&"), (od == NULL || od -> common -> slot == no_slot || isMultiArgSlot(od -> common)) ? "s" : "");

	/* Generate the format string. */
	optargs = FALSE;

	if (handle_self)
		prcode(fp,"%c",(isProtected(od) ? 'p' : 'B'));
	else if (isQtSlot && od == NULL)
		prcode(fp,"C");

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		char *fmt = "";
		argDef *ad = &sd -> args[a];

		if (!isInArg(ad))
			continue;

		if (ad -> defval != NULL && !optargs)
		{
			prcode(fp,"|");
			optargs = TRUE;
		}

		switch (ad -> atype)
		{
		case ustring_type:
		case string_type:
			if (ad -> nrderefs == 0 || (isOutArg(ad) && ad -> nrderefs == 1))
				fmt = "c";
			else if (isArray(ad))
				fmt = "a";
			else
				fmt = "s";

			break;

		case enum_type:
			fmt = (ad -> u.ed -> fqcname != NULL) ? "E" : "e";
			break;

		case bool_type:
			fmt = "b";
			break;

		case uint_type:
		case int_type:
			if (!isArraySize(ad))
				fmt = "i";

			break;

		case cint_type:
			fmt = "Xi";
			break;

		case ushort_type:
		case short_type:
			if (!isArraySize(ad))
				fmt = "h";

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
			fmt = (secCall ? "y" : "q");
			break;

		case rxdis_type:
			fmt = (secCall ? "Y" : "Q");
			break;

		case mapped_type:
			fmt = getSubFormatChar('M',ad);
			break;

		case class_type:
			fmt = getSubFormatChar(((ad -> u.cd -> convtocode != NULL && !isConstrained(ad)) ? 'M' : 'J'),ad);
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
		prcode(fp,",&sipSelf,sipClass_%C,&sipCpp",classFQCName(cd));
	else if (isQtSlot && od == NULL)
		prcode(fp,",sipSelf");

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		argDef *ad = &sd -> args[a];

		if (!isInArg(ad))
			continue;

		switch (ad -> atype)
		{
		case mapped_type:
			prcode(fp,",sipConvertTo_%T,&a%d,&a%dIsTemp",ad,a,a);
			break;

		case class_type:
			if (ad -> u.cd -> convtocode != NULL && !isConstrained(ad))
				prcode(fp,",sipConvertTo_%T,&a%d,&a%dIsTemp",ad,a,a);
			else
			{
				prcode(fp,",sipClass_%T,&a%d",ad,a);

				if (isThisTransferred(ad))
					prcode(fp,",%ssipOwner",(ct != NULL ? "" : "&"));
				else if (isGetWrapper(ad))
					prcode(fp,",&a%dWrapper",a);
			}

			break;

		case rxcon_type:
			{
				if (sigarg > 0)
					prcode(fp,",a%d",sigarg);
				else
				{
					prcode(fp,",\"(");

					generateArgs(sd -> args[slotconarg].u.sa,Declaration,fp);

					prcode(fp,")\"");
				}

				prcode(fp,",&a%d,&a%d",a,slotconarg);

				break;
			}

		case rxdis_type:
			{
				prcode(fp,",\"(");

				generateArgs(sd -> args[slotdisarg].u.sa,Declaration,fp);

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
			if (ad -> u.ed -> fqcname != NULL)
				prcode(fp,",sipEnum_%C",ad -> u.ed -> fqcname);

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

	if (fc != 'P' && ad -> nrderefs == 0)
		flags |= 0x01;

	if (isTransferred(ad))
		flags |= 0x02;

	if (isTransferredBack(ad))
		flags |= 0x04;

	if (fc == 'J' && (isThisTransferred(ad) || isGetWrapper(ad)))
		flags |= 0x08;

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

	if (ad -> atype == class_type && !isConstrained(ad))
		convtocode = ad -> u.cd -> convtocode;
	else if (ad -> atype == mapped_type)
		convtocode = ad -> u.mtd -> convtocode;
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
"			Py_DECREF(a%d);\n"
			, sd->nrArgs - 1);
}


/*
 * Delete any temporary variables on the heap created by type convertors.
 */
static void deleteTemps(signatureDef *sd, FILE *fp)
{
	int a;

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		argDef *ad = &sd -> args[a];

		if (isInArg(ad) && hasConvertToCode(ad))
		{
			prcode(fp,
"\n"
"			if (a%dIsTemp)\n"
				,a);

			if (generating_c)
				prcode(fp,
"				free(a%d);\n"
					,a);
			else if (isConstArg(ad))
				prcode(fp,
"				delete const_cast<%b *>(a%d);\n"
					,ad,a);
			else
				prcode(fp,
"				delete a%d;\n"
					,a);
		}
	}
}


/*
 * Generate a C++ code block.
 */
static void generateCppCodeBlock(codeBlock *code,FILE *fp)
{
	int reset_line = FALSE;
	codeBlock *cb;

	for (cb = code; cb != NULL; cb = cb -> next)
	{
		char *cp;

		/*
		 * Fragmented fragments (possibly created when applying
		 * template types) don't have a filename.
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
		char *bn;

		/* Just use the base name. */

		if ((bn = strrchr(currentFileName,'/')) != NULL)
			++bn;
		else
			bn = currentFileName;

		prcode(fp,
"#line %d \"%s\"\n"
			,currentLineNr + 1,bn);
	}
}


/*
 * Create a file with an optional standard header.
 */
static FILE *createFile(sipSpec *pt,char *fname,char *description)
{
	FILE *fp;

	/* Create the file. */

	if ((fp = fopen(fname,"w")) == NULL)
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

		if (pt -> copying != NULL)
			prcode(fp,
" *\n"
				);

		needComment = TRUE;

		for (cb = pt -> copying; cb != NULL; cb = cb -> next)
		{
			char *cp;

			for (cp = cb -> frag; *cp != '\0'; ++cp)
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
static void prcode(FILE *fp,char *fmt,...)
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
					ad -> nrderefs = 0;

					generateBaseType(ad,fp);

					*ad = orig;

					break;
				}

			case 'B':
				generateBaseType(va_arg(ap,argDef *),fp);
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

					fprintf(fp,"sipNm_%s_%s",nd -> module -> name,nd -> text);
					break;
				}

			case 'E':
				{
					enumDef *ed = va_arg(ap,enumDef *);

					if (ed -> fqcname == NULL)
						fprintf(fp,"int");
					else if (isProtectedEnum(ed))
					{
						fprintf(fp,"sip");
						prScopedName(fp,classFQCName(ed -> pcd),"_");

						fprintf(fp,"::sip%s",scopedNameTail(ed -> fqcname));
					}
					else
						prScopedName(fp,ed -> fqcname,"::");

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
				if (generating_c)
					fprintf(fp,"struct ");

				prScopedClassName(fp,va_arg(ap,classDef *),"::");
				break;

			case 'O':
				prOverloadName(fp, va_arg(ap, overDef *));
				break;

			case 'P':
				{
					classDef *ecd = va_arg(ap, classDef *);
					char *pyname = va_arg(ap, char *);

					prScopedPythonName(fp, ecd, pyname);
					break;
				}

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
 * Generate the C++ name of an overloaded function.
 */
void prOverloadName(FILE *fp, overDef *od)
{
	char *pt1, *pt2;

	pt1 = "operator";

	switch (od -> common -> slot)
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
		pt2 = od -> cppname;
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
		fprintf(fp,"%s",snd -> name);

		if ((snd = snd -> next) != NULL)
			fprintf(fp,"%s",sep);
	}
}


/*
 * Generate a scoped class name with the given separator string.  At the moment
 * this provides (probably) broken support for protected classes.
 */
static void prScopedClassName(FILE *fp,classDef *cd,char *sep)
{
	scopedNameDef *snd = classFQCName(cd);

	while (snd != NULL)
	{
		if (isProtectedClass(cd))
			fprintf(fp,"sip");

		fprintf(fp,"%s",snd -> name);

		if ((snd = snd -> next) != NULL)
			fprintf(fp,"%s",sep);
	}
}


/*
 * Generate a scoped Python name.
 */
static void prScopedPythonName(FILE *fp, classDef *scope, char *pyname)
{
	if (scope != NULL)
	{
		prScopedPythonName(fp, scope->ecd, NULL);
		fprintf(fp, "%s.", scope->pyname);
	}

	if (pyname != NULL)
		fprintf(fp, "%s", pyname);
}


/*
 * Generate a type name to be used as part of an identifier name.
 */
static void prTypeName(FILE *fp,argDef *ad,int intmpl)
{
	if (intmpl)
	{
		char buf[10];
		int flgs;

		/* We use numbers so they don't conflict with names. */

		sprintf(buf,"%02d",ad -> atype);

		flgs = 0;

		if (isConstArg(ad))
			flgs += 1;

		if (isReference(ad))
			flgs += 2;

		prcode(fp,"%s%d%d",buf,flgs,ad -> nrderefs);
	}

	/* Now add a name if there is one associated with the type. */

	switch (ad -> atype)
	{
	case struct_type:
		prcode(fp,"%C",ad -> u.sname);
		break;

	case defined_type:
		prcode(fp,"%C",ad -> u.snd);
		break;

	case enum_type:
		prcode(fp,"%C",ad -> u.ed -> fqcname);
		break;

	case mapped_type:
		prTypeName(fp,&ad -> u.mtd -> type,intmpl);
		break;

	case class_type:
		prcode(fp,"%C",classFQCName(ad -> u.cd));
		break;

	case template_type:
		{
			int a;
			templateDef *td = ad -> u.td;

			prcode(fp,"%C",td -> fqname);

			for (a = 0; a < td -> types.nrArgs; ++a)
			{
				prcode(fp,"_");
				prTypeName(fp,&td -> types.args[a],TRUE);
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
	return ((ad -> atype == mapped_type || ad -> atype == class_type) &&
		((isReference(ad) && ad -> nrderefs == 0) || (!isReference(ad) && ad -> nrderefs == 1)) &&
		!isInArg(ad) && isOutArg(ad));
}


/*
 * Reset and save any argument flags so that the signature will be rendered
 * exactly as defined in C++.
 */
static void normaliseArgs(signatureDef *sd)
{
	int a;
	argDef *ad = sd -> args;

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		if (ad -> atype == class_type && isProtectedClass(ad -> u.cd))
		{
			resetIsProtectedClass(ad -> u.cd);
			setWasProtectedClass(ad -> u.cd);
		}
		else if (ad -> atype == enum_type && isProtectedEnum(ad -> u.ed))
		{
			resetIsProtectedEnum(ad -> u.ed);
			setWasProtectedEnum(ad -> u.ed);
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
	argDef *ad = sd -> args;

	for (a = 0; a < sd -> nrArgs; ++a)
	{
		if (ad -> atype == class_type && wasProtectedClass(ad -> u.cd))
		{
			resetWasProtectedClass(ad -> u.cd);
			setIsProtectedClass(ad -> u.cd);
		}
		else if (ad -> atype == enum_type && wasProtectedEnum(ad -> u.ed))
		{
			resetWasProtectedEnum(ad -> u.ed);
			setIsProtectedEnum(ad -> u.ed);
		}

		++ad;
	}
}


/*
 * Return TRUE if a dealloc function is needed for a class.
 */
static int needDealloc(classDef *cd)
{
	if (cd -> iff -> type == namespace_iface)
		return FALSE;

	/* All of these conditions cause some code to be generated. */

	if (tracing)
		return TRUE;

	if (generating_c)
		return TRUE;

	if (cd -> dealloccode != NULL)
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
