/*
 * The XML export file generator module for SIP.
 *
 * @BS_LICENSE@
 */


#include <stdio.h>

#include "sip.h"


#define	XML_VERSION_NR	0		/* The schema version number. */


static void xmlClasses(sipSpec *pt, classDef *scope, int indent, FILE *fp);
static void xmlEnums(sipSpec *pt, classDef *scope, int indent, FILE *fp);
static void xmlVars(sipSpec *pt, classDef *scope, int indent, FILE *fp);
static void xmlInit(ctorDef *ctors, int indent, FILE *fp);
static void xmlFunction(memberDef *md, overDef *oloads, int indent, FILE *fp);
static int xmlOverload(const char *name, signatureDef *sd, int seqnr, int ctor,
		int res_xfer, int stat, int abstract, int sec, int indent,
		FILE *fp);
static void xmlArgument(argDef *ad, const char *dir, int res_xfer, int sec,
		int indent, FILE *fp);
static void xmlType(argDef *ad, int sec, FILE *fp);
static void xmlIndent(int indent, FILE *fp);
static const char *dirAttribute(argDef *ad);


/*
 * Generate the XML export file.
 */
void generateXML(sipSpec *pt, const char *xmlFile)
{
	FILE *fp;
	memberDef *md;

	if ((fp = fopen(xmlFile, "w")) == NULL)
		fatal("Unable to create file \"%s\"\n", xmlFile);

	fprintf(fp, "<?xml version=\"1.0\"?>\n");
	fprintf(fp, "<Module version=\"%u\" name=\"%s\">\n",
			XML_VERSION_NR, pt->module->name);

	/*
	 * Note that we don't yet handle mapped types, templates or exceptions.
	 */

	xmlClasses(pt, NULL, 1, fp);
	xmlEnums(pt, NULL, 1, fp);
	xmlVars(pt, NULL, 1, fp);

	for (md = pt->othfuncs; md != NULL; md = md->next)
	{
		if (md->module != pt->module)
			continue;

		xmlFunction(md, pt->overs, 1, fp);
	}

	fprintf(fp, "</Module>\n");

	fclose(fp);
}


/*
 * Generate the XML for all the classes in a scope.
 */
static void xmlClasses(sipSpec *pt, classDef *scope, int indent, FILE *fp)
{
	classDef *cd;

	for (cd = pt->classes; cd != NULL; cd = cd->next)
	{
		memberDef *md;

		if (cd->iff->module != pt->module)
			continue;

		if (cd->ecd != scope)
			continue;

		if (isExternal(cd))
			continue;

		xmlIndent(indent++, fp);
		fprintf(fp, "<Class name=\"%s\"", cd->pyname);

		if (cd->real != NULL)
			fprintf(fp, " extends=\"%s\"", cd->real->iff->module->name);

		if (cd->supers != NULL)
		{
			classList *cl;

			for (cl = cd->supers; cl != NULL; cl = cl->next)
				fprintf(fp, " %s%s", (cl == cd->supers ? "inherits=\"" : ""), cl->cd->pyname);

			fprintf(fp, "\"");
		}

		fprintf(fp, ">\n");

		xmlClasses(pt, cd, indent, fp);
		xmlEnums(pt, cd, indent, fp);
		xmlVars(pt, cd, indent, fp);

		if (cd->ctors != NULL)
			xmlInit(cd->ctors, indent, fp);

		for (md = cd->members; md != NULL; md = md->next)
			xmlFunction(md, cd->overs, indent, fp);

		xmlIndent(--indent, fp);
		fprintf(fp, "</Class>\n");
	}
}


/*
 * Generate the XML for all the enums in a scope.
 */
static void xmlEnums(sipSpec *pt, classDef *scope, int indent, FILE *fp)
{
	enumDef *ed;

	for (ed = pt->enums; ed != NULL; ed = ed->next)
	{
		if (ed->module != pt->module)
			continue;

		if (ed->ecd != scope)
			continue;

		if (ed->pyname != NULL)
		{
			xmlIndent(indent, fp);
			fprintf(fp, "<Enum name=\"%s\"/>\n", ed->pyname->text);
		}
		else
		{
			enumMemberDef *emd;

			for (emd = ed->members; emd != NULL; emd = emd->next)
			{
				xmlIndent(indent, fp);
				fprintf(fp, "<Member name=\"%s\" const=\"1\" typename=\"int\"/>\n", emd->pyname->text);
			}
		}
	}
}


/*
 * Generate the XML for all the variables in a scope.
 */
static void xmlVars(sipSpec *pt, classDef *scope, int indent, FILE *fp)
{
	varDef *vd;

	for (vd = pt->vars; vd != NULL; vd = vd->next)
	{
		if (vd->module != pt->module)
			continue;

		if (vd->ecd != scope)
			continue;

		xmlIndent(indent, fp);
		fprintf(fp, "<Member name=\"%s\"%s", vd->pyname->text, (isConstArg(&vd->type) || scope == NULL ? " const=\"1\"" : ""));

		if (isStaticVar(vd))
			fprintf(fp, " static=\"1\"");

		xmlType(&vd->type, FALSE, fp);
		fprintf(fp, "/>\n");
	}
}


/*
 * Generate the XML for an __init__ method.
 */
static void xmlInit(ctorDef *ctors, int indent, FILE *fp)
{
	int seqnr = 1;
	ctorDef *ct;

	for (ct = ctors; ct != NULL; ct = ct->next)
	{
		if (isPrivateCtor(ct))
			continue;

		if (xmlOverload("__init__", &ct->pysig, seqnr++, TRUE, FALSE, FALSE, FALSE, FALSE, indent, fp))
			xmlOverload("__init__", &ct->pysig, seqnr++, TRUE, FALSE, FALSE, FALSE, TRUE, indent, fp);
	}
}


/*
 * Generate the XML for a function.
 */
static void xmlFunction(memberDef *md, overDef *oloads, int indent, FILE *fp)
{
	int seqnr = 1;
	overDef *od;

	for (od = oloads; od != NULL; od = od->next)
	{
		if (od->common != md)
			continue;

		if (isPrivate(od))
			continue;

		if (xmlOverload(md->pyname->text, &od->pysig, seqnr++, FALSE, isResultTransferredBack(od), isStatic(od), isAbstract(od), FALSE, indent, fp))
			xmlOverload(md->pyname->text, &od->pysig, seqnr++, FALSE, isResultTransferredBack(od), isStatic(od), isAbstract(od), TRUE, indent, fp);
	}
}


/*
 * Generate the XML for an overload.
 */
static int xmlOverload(const char *name, signatureDef *sd, int seqnr, int ctor,
		int res_xfer, int stat, int abstract, int sec, int indent,
		FILE *fp)
{
	int a, need_sec = FALSE, no_res;
	const char *abstr = (abstract ? " abstract=\"1\"" : "");
	const char *ststr = (stat ? " static=\"1\"" : "");

	no_res = (ctor || (sd->result.atype == void_type &&
				sd->result.nrderefs == 0));

	/* Handle the trivial case. */
	if (no_res && sd->nrArgs == 0)
	{
		xmlIndent(indent, fp);
		fprintf(fp, "<Function name=\"%s\" seqnr=\"%d\"%s%s/>\n", name, seqnr, ststr, abstr);

		return FALSE;
	}

	xmlIndent(indent++, fp);
	fprintf(fp, "<Function name=\"%s\" seqnr=\"%d\"%s%s>\n", name, seqnr, ststr, abstr);

	if (!no_res)
		xmlArgument(&sd->result, "out", res_xfer, FALSE, indent, fp);

	for (a = 0; a < sd->nrArgs; ++a)
	{
		argDef *ad = &sd->args[a];

		xmlArgument(ad, dirAttribute(ad), FALSE, sec, indent, fp);

		if (ad->atype == rxcon_type || ad->atype == rxdis_type)
			need_sec = TRUE;
	}

	xmlIndent(--indent, fp);
	fprintf(fp, "</Function>\n");

	return need_sec;
}


/*
 * Convert an arguments direction to an XML attribute value.
 */
static const char *dirAttribute(argDef *ad)
{
	if (isInArg(ad))
	{
		if (isOutArg(ad))
			return "inout";

		return NULL;
	}

	return "out";
}


/*
 * Generate the XML for an argument.
 */
static void xmlArgument(argDef *ad, const char *dir, int res_xfer, int sec,
		int indent, FILE *fp)
{
	if (isArraySize(ad))
		return;

	if (sec && (ad->atype == slotcon_type || ad->atype == slotdis_type))
		return;

	xmlIndent(indent, fp);
	fprintf(fp, "<Argument");
	xmlType(ad, sec, fp);

	if (dir != NULL)
		fprintf(fp, " dir=\"%s\"", dir);

	if (isAllowNone(ad))
		fprintf(fp, " allownone=\"1\"");

	if (isTransferred(ad))
		fprintf(fp, " transfer=\"to\"");
	else if (isThisTransferred(ad))
		fprintf(fp, " transfer=\"this\"");
	else if (res_xfer || isTransferredBack(ad))
		fprintf(fp, " transfer=\"back\"");

	/*
	 * Handle the default value, but ignore it if it is an output only
	 * argument.
	 */
	if (ad->defval && (dir == NULL || strcmp(dir, "out") != 0))
	{
		int handled = FALSE;

		prcode(fp, " default=\"%M");

		/* Translate some special cases. */
		if (ad->defval->next == NULL && ad->defval->vtype == numeric_value)
			if (ad->nrderefs > 0 && ad->defval->u.vnum == 0)
			{
				prcode(fp, "None");
				handled = TRUE;
			}
			else if (ad->atype == bool_type || ad->atype == cbool_type)
			{
				prcode(fp, ad->defval->u.vnum ? "True" : "False");
				handled = TRUE;
			}

		if (!handled)
			generateExpression(ad->defval, fp);

		prcode(fp, "%M\"");
	}

	fprintf(fp, "/>\n");
}


/*
 * Generate the XML for a type.
 */
static void xmlType(argDef *ad, int sec, FILE *fp)
{
	const char *type_type = NULL, *type_name = NULL;
	classDef *type_scope = NULL;

	fprintf(fp, " typename=\"");

	switch (ad->atype)
	{
	case class_type:
		type_type = "class";
		type_name = ad->u.cd->pyname;
		type_scope = ad->u.cd->ecd;
		break;

	case struct_type:
	case void_type:
		type_name = "sip.voidptr";
		break;

	case enum_type:
		if (ad->u.ed->pyname != NULL)
		{
			type_type = "enum";
			type_name = ad->u.ed->pyname->text;
			type_scope = ad->u.ed->ecd;
		}
		else
			type_name = "int";
		break;

	case signal_type:
		type_name = "SIGNAL()";
		break;

	case slot_type:
		type_name = "SLOT()";
		break;

	case rxcon_type:
	case rxdis_type:
		if (sec)
			type_name = "callable";
		else
		{
			type_type = "class";
			type_name = "QObject";
		}

		break;

	case qobject_type:
		type_type = "class";
		type_name = "QObject";
		break;

	case slotcon_type:
	case slotdis_type:
		{
			int a;

			prcode(fp, "SLOT(");

			for (a = 0; a < ad->u.sa->nrArgs; ++a)
			{
				if (a > 0)
					prcode(fp, ",");

				prcode(fp, "%M%B%M", &ad->u.sa->args[a]);
			}

			prcode(fp, ")");
		}

		break;

	case ustring_type:
	case string_type:
	case sstring_type:
		if (ad->nrderefs > 0)
			type_name = "str";
		else
			type_name = "char";

		break;

	case short_type:
	case int_type:
	case cint_type:
		type_name = "int";
		break;

	case ushort_type:
	case uint_type:
	case ulong_type:
	case ulonglong_type:
		type_name = "unsigned long";
		break;

	case long_type:
	case longlong_type:
		type_name = "long";
		break;

	case float_type:
	case cfloat_type:
	case double_type:
	case cdouble_type:
		type_name = "float";
		break;

	case bool_type:
	case cbool_type:
		type_name = "bool";
		break;

	case mapped_type:
		prcode(fp, "%M%B%M", &ad->u.mtd->type);
		type_type = "mappedtype";
		break;

	case pyobject_type:
		type_name = "object";
		break;

	case pytuple_type:
		type_name = "tuple";
		break;

	case pylist_type:
		type_name = "list";
		break;

	case pydict_type:
		type_name = "dict";
		break;

	case pycallable_type:
		type_name = "callable";
		break;

	case pyslice_type:
		type_name = "slice";
		break;

	case pytype_type:
		type_name = "type";
		break;

	case ellipsis_type:
		type_name = "...";
		break;

	case anyslot_type:
		/* Need to check if this is enough. */
		type_name = "SLOT()";
		break;

	default:
		type_name = "unknown";
	}

	if (type_name != NULL)
		prScopedPythonName(fp, type_scope, type_name);

	fprintf(fp, "\"");

	if (type_type != NULL)
		fprintf(fp, " typetype=\"%s\"", type_type);

	if (ad->name != NULL)
		fprintf(fp, " name=\"%s\"", ad->name);
}


/*
 * Generate the indentation for a line.
 */
static void xmlIndent(int indent, FILE *fp)
{
	while (indent-- > 0)
		fprintf(fp, "  ");
}
