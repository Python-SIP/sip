/*
 * The XML and API file generator module for SIP.
 *
 * @BS_LICENSE@
 */


#include <stdio.h>

#include "sip.h"


#define XML_VERSION_NR  0       /* The schema version number. */


static void apiEnums(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp);
static void apiVars(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp);
static int apiCtor(moduleDef *mod, classDef *scope, ctorDef *ct, int sec,
        FILE *fp);
static int apiOverload(moduleDef *mod, classDef *scope, overDef *od, int sec,
        FILE *fp);
static int apiArgument(argDef *ad, int out, int need_comma, int sec, FILE *fp);
static void xmlClass(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static void xmlEnums(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp);
static void xmlVars(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp);
static void xmlFunction(classDef *scope, memberDef *md, overDef *oloads,
        int indent, FILE *fp);
static int xmlCtor(classDef *scope, ctorDef *ct, int sec, int indent, FILE *fp);
static int xmlOverload(classDef *scope, memberDef *md, overDef *od,
        classDef *xtnds, int stat, int sec, int indent, FILE *fp);
static void xmlCppSignature(FILE *fp, overDef *od);
static void xmlArgument(argDef *ad, const char *dir, int res_xfer, int sec,
        int indent, FILE *fp);
static void xmlType(argDef *ad, int sec, FILE *fp);
static void xmlIndent(int indent, FILE *fp);
static const char *dirAttribute(argDef *ad);
static void exportDefaultValue(argDef *ad, FILE *fp);
static const char *pyType(argDef *ad, int sec, classDef **scope);


/*
 * Generate the API file.
 */
void generateAPI(sipSpec *pt, moduleDef *mod, const char *apiFile)
{
    overDef *od;
    classDef *cd;
    FILE *fp;

    /* Generate the file. */
    if ((fp = fopen(apiFile, "w")) == NULL)
        fatal("Unable to create file \"%s\"\n", apiFile);

    apiEnums(pt, mod, NULL, fp);
    apiVars(pt, mod, NULL, fp);

    for (od = mod->overs; od != NULL; od = od->next)
    {
        if (od->common->module != mod)
            continue;

        if (od->common->slot != no_slot)
            continue;

        if (apiOverload(mod, NULL, od, FALSE, fp))
            apiOverload(mod, NULL, od, TRUE, fp);
    }

    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        ctorDef *ct;

        if (cd->iff->module != mod)
            continue;

        if (isExternal(cd))
            continue;

        apiEnums(pt, mod, cd, fp);
        apiVars(pt, mod, cd, fp);

        for (ct = cd->ctors; ct != NULL; ct = ct->next)
        {
            if (isPrivateCtor(ct))
                continue;

            if (apiCtor(mod, cd, ct, FALSE, fp))
                apiCtor(mod, cd, ct, TRUE, fp);
        }

        for (od = cd->overs; od != NULL; od = od->next)
        {
            if (isPrivate(od))
                continue;

            if (od->common->slot != no_slot)
                continue;

            if (apiOverload(mod, cd, od, FALSE, fp))
                apiOverload(mod, cd, od, TRUE, fp);
        }
    }

    fclose(fp);
}


/*
 * Generate an API ctor.
 */
static int apiCtor(moduleDef *mod, classDef *scope, ctorDef *ct, int sec,
        FILE *fp)
{
    int need_sec = FALSE, need_comma, a;

    /* Do the callable type form. */
    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope->ecd, scope->pyname);
    fprintf(fp, "(");

    need_comma = FALSE;

    for (a = 0; a < ct->pysig.nrArgs; ++a)
    {
        argDef *ad = &ct->pysig.args[a];

        need_comma = apiArgument(ad, FALSE, need_comma, sec, fp);

        if (ad->atype == rxcon_type || ad->atype == rxdis_type)
            need_sec = TRUE;
    }

    fprintf(fp, ")\n");

    /* Do the call __init__ form. */
    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope->ecd, scope->pyname);
    fprintf(fp, ".__init__(self");

    for (a = 0; a < ct->pysig.nrArgs; ++a)
        apiArgument(&ct->pysig.args[a], FALSE, TRUE, sec, fp);

    fprintf(fp, ")\n");

    return need_sec;
}


/*
 * Generate the APIs for all the enums in a scope.
 */
static void apiEnums(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp)
{
    enumDef *ed;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        enumMemberDef *emd;

        if (ed->module != mod)
            continue;

        if (ed->ecd != scope)
            continue;

        if (ed->pyname != NULL)
        {
            fprintf(fp, "%s.", mod->name);
            prScopedPythonName(fp, ed->ecd, ed->pyname->text);
            fprintf(fp, "\n");
        }

        for (emd = ed->members; emd != NULL; emd = emd->next)
        {
            fprintf(fp, "%s.", mod->name);
            prScopedPythonName(fp, ed->ecd, emd->pyname->text);
            fprintf(fp, "\n");
        }
    }
}


/*
 * Generate the APIs for all the variables in a scope.
 */
static void apiVars(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp)
{
    varDef *vd;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->module != mod)
            continue;

        if (vd->ecd != scope)
            continue;

        fprintf(fp, "%s.", mod->name);
        prScopedPythonName(fp, vd->ecd, vd->pyname->text);
        fprintf(fp, "\n");
    }
}


/*
 * Generate a single API overload.
 */
static int apiOverload(moduleDef *mod, classDef *scope, overDef *od, int sec,
        FILE *fp)
{
    int need_sec = FALSE, need_comma = FALSE, is_res, nr_out, a;

    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope, od->common->pyname->text);
    fprintf(fp, "(");

    if (scope != NULL && scope->iff->type != namespace_iface && !isStatic(od))
    {
        fprintf(fp, "self");
        need_comma = TRUE;
    }

    nr_out = 0;

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        argDef *ad = &od->pysig.args[a];

        if (isOutArg(ad))
            ++nr_out;

        if (!isInArg(ad))
            continue;

        need_comma = apiArgument(ad, FALSE, need_comma, sec, fp);

        if (ad->atype == rxcon_type || ad->atype == rxdis_type)
            need_sec = TRUE;
    }

    fprintf(fp, ")");

    is_res = (od->pysig.result.atype != void_type || od->pysig.result.nrderefs != 0);

    if (is_res || nr_out > 0)
    {
        fprintf(fp, " -> ");

        if ((is_res && nr_out > 0) || nr_out > 1)
            fprintf(fp, "(");

        if (is_res)
            need_comma = apiArgument(&od->pysig.result, TRUE, FALSE, sec, fp);
        else
            need_comma = FALSE;

        for (a = 0; a < od->pysig.nrArgs; ++a)
        {
            argDef *ad = &od->pysig.args[a];

            if (!isOutArg(ad))
                continue;

            need_comma = apiArgument(ad, TRUE, need_comma, sec, fp);
        }

        if ((is_res && nr_out > 0) || nr_out > 1)
            fprintf(fp, ")");
    }

    fprintf(fp, "\n");

    return need_sec;
}


/*
 * Generate the API for an argument.
 */
static int apiArgument(argDef *ad, int out, int need_comma, int sec, FILE *fp)
{
    const char *tname;
    classDef *tscope;

    if (isArraySize(ad))
        return need_comma;

    if (sec && (ad->atype == slotcon_type || ad->atype == slotdis_type))
        return need_comma;

    if ((tname = pyType(ad, sec, &tscope)) == NULL)
        return need_comma;

    if (need_comma)
        fprintf(fp, ", ");

    prScopedPythonName(fp, tscope, tname);

    if (ad->name != NULL)
        fprintf(fp, " %s", ad->name);

    /*
     * Handle the default value, but ignore it if it is an output only
     * argument.
     */
    if (ad->defval && !out)
    {
        fprintf(fp, "=");
        prcode(fp, "%M");
        exportDefaultValue(ad, fp);
        prcode(fp, "%M");
    }

    return TRUE;
}


/*
 * Generate the XML export file.
 */
void generateXML(sipSpec *pt, moduleDef *mod, const char *xmlFile)
{
    FILE *fp;
    classDef *cd;
    memberDef *md;

    if ((fp = fopen(xmlFile, "w")) == NULL)
        fatal("Unable to create file \"%s\"\n", xmlFile);

    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<Module version=\"%u\" name=\"%s\">\n",
            XML_VERSION_NR, mod->name);

    /*
     * Note that we don't yet handle mapped types, templates or exceptions.
     */

    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        if (cd->iff->module != mod)
            continue;

        if (isExternal(cd))
            continue;

        xmlClass(pt, mod, cd, fp);
    }

    for (cd = mod->proxies; cd != NULL; cd = cd->next)
        xmlClass(pt, mod, cd, fp);

    xmlEnums(pt, mod, NULL, 1, fp);
    xmlVars(pt, mod, NULL, 1, fp);

    for (md = mod->othfuncs; md != NULL; md = md->next)
        xmlFunction(NULL, md, mod->overs, 1, fp);

    fprintf(fp, "</Module>\n");

    fclose(fp);
}


/*
 * Generate the XML for a class.
 */
static void xmlClass(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp)
{
    int indent = 1;
    ctorDef *ct;
    memberDef *md;

    if (isOpaque(cd))
    {
        xmlIndent(indent, fp);
        fprintf(fp, "<OpaqueClass name=\"", cd->pyname);
        prScopedPythonName(fp, cd->ecd, cd->pyname);
        fprintf(fp, "\"/>\n");

        return;
    }

    xmlIndent(indent++, fp);
    fprintf(fp, "<Class name=\"", cd->pyname);
    prScopedPythonName(fp, cd->ecd, cd->pyname);
    fprintf(fp, "\"");

    if (cd->convtocode != NULL)
        fprintf(fp, " convert=\"1\"");

    if (cd->real != NULL)
        fprintf(fp, " extends=\"%s\"", cd->real->iff->module->name);

    if (cd->supers != NULL)
    {
        classList *cl;

        fprintf(fp, " inherits=\"");

        for (cl = cd->supers; cl != NULL; cl = cl->next)
        {
            if (cl != cd->supers)
                fprintf(fp, " ");

            prScopedPythonName(fp, cl->cd->ecd, cl->cd->pyname);
        }

        fprintf(fp, "\"");
    }

    fprintf(fp, ">\n");

    xmlEnums(pt, mod, cd, indent, fp);
    xmlVars(pt, mod, cd, indent, fp);

    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        if (isPrivateCtor(ct))
            continue;

        if (xmlCtor(cd, ct, FALSE, indent, fp))
            xmlCtor(cd, ct, TRUE, indent, fp);
    }

    for (md = cd->members; md != NULL; md = md->next)
        xmlFunction(cd, md, cd->overs, indent, fp);

    xmlIndent(--indent, fp);
    fprintf(fp, "</Class>\n");
}


/*
 * Generate the XML for all the enums in a scope.
 */
static void xmlEnums(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp)
{
    enumDef *ed;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->module != mod)
            continue;

        if (ed->ecd != scope)
            continue;

        if (ed->pyname != NULL)
        {
            enumMemberDef *emd;

            xmlIndent(indent++, fp);
            fprintf(fp, "<Enum name=\"");
            prScopedPythonName(fp, ed->ecd, ed->pyname->text);
            fprintf(fp, "\">\n");

            for (emd = ed->members; emd != NULL; emd = emd->next)
            {
                xmlIndent(indent, fp);
                fprintf(fp, "<EnumMember name=\"");
                prScopedPythonName(fp, ed->ecd, emd->pyname->text);
                fprintf(fp, "\"/>\n");
            }

            xmlIndent(--indent, fp);
            fprintf(fp, "</Enum>\n");
        }
        else
        {
            enumMemberDef *emd;

            for (emd = ed->members; emd != NULL; emd = emd->next)
            {
                xmlIndent(indent, fp);
                fprintf(fp, "<Member name=\"");
                prScopedPythonName(fp, ed->ecd, emd->pyname->text);
                fprintf(fp, "\" const=\"1\" typename=\"int\"/>\n");
            }
        }
    }
}


/*
 * Generate the XML for all the variables in a scope.
 */
static void xmlVars(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp)
{
    varDef *vd;

    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->module != mod)
            continue;

        if (vd->ecd != scope)
            continue;

        xmlIndent(indent, fp);
        fprintf(fp, "<Member name=\"");
        prScopedPythonName(fp, vd->ecd, vd->pyname->text);
        fprintf(fp, "\"");

        if (isConstArg(&vd->type) || scope == NULL)
            fprintf(fp, " const=\"1\"");

        if (isStaticVar(vd))
            fprintf(fp, " static=\"1\"");

        xmlType(&vd->type, FALSE, fp);
        fprintf(fp, "/>\n");
    }
}


/*
 * Generate the XML for a ctor.
 */
static int xmlCtor(classDef *scope, ctorDef *ct, int sec, int indent, FILE *fp)
{
    int a, need_sec;

    xmlIndent(indent++, fp);
    fprintf(fp, "<Function name=\"");
    prScopedPythonName(fp, scope, "__init__");
    fprintf(fp, "\"");

    /* Handle the trivial case. */
    if (ct->pysig.nrArgs == 0)
    {
        fprintf(fp, "/>\n");
        return FALSE;
    }

    fprintf(fp, ">\n");

    need_sec = FALSE;

    for (a = 0; a < ct->pysig.nrArgs; ++a)
    {
        argDef *ad = &ct->pysig.args[a];

        xmlArgument(ad, dirAttribute(ad), FALSE, sec, indent, fp);

        if (ad->atype == rxcon_type || ad->atype == rxdis_type)
            need_sec = TRUE;
    }

    xmlIndent(--indent, fp);
    fprintf(fp, "</Function>\n");

    return need_sec;
}


/*
 * Generate the XML for a function.
 */
static void xmlFunction(classDef *scope, memberDef *md, overDef *oloads,
        int indent, FILE *fp)
{
    overDef *od;

    for (od = oloads; od != NULL; od = od->next)
    {
        int isstat;
        classDef *xtnds;

        if (od->common != md)
            continue;

        if (isPrivate(od))
            continue;

        if (isSignal(od))
        {
            xmlIndent(indent, fp);
            fprintf(fp, "<Signal name=\"");
            prScopedPythonName(fp, scope, md->pyname->text);
            fprintf(fp, "\" sig=\"");
            xmlCppSignature(fp, od);
            fprintf(fp, "\"/>\n", md->pyname->text);

            continue;
        }

        xtnds = NULL;
        isstat = (scope == NULL || scope->iff->type == namespace_iface || isStatic(od));

        if (scope == NULL && md->slot != no_slot && od->pysig.args[0].atype == class_type)
        {
            xtnds = od->pysig.args[0].u.cd;
            isstat = FALSE;
        }

        if (xmlOverload(scope, md, od, xtnds, isstat, FALSE, indent, fp))
            xmlOverload(scope, md, od, xtnds, isstat, TRUE, indent, fp);
    }
}


/*
 * Generate the XML for an overload.
 */
static int xmlOverload(classDef *scope, memberDef *md, overDef *od,
        classDef *xtnds, int stat, int sec, int indent, FILE *fp)
{
    int a, need_sec, no_res;

    xmlIndent(indent++, fp);
    fprintf(fp, "<Function name=\"");
    prScopedPythonName(fp, scope, md->pyname->text);
    fprintf(fp, "\"");

    if (isAbstract(od))
        fprintf(fp, " abstract=\"1\"");

    if (stat)
        fprintf(fp, " static=\"1\"");

    if (isSlot(od))
    {
        fprintf(fp, " slot=\"");
        xmlCppSignature(fp, od);
        fprintf(fp, "\"");
    }

    if (xtnds != NULL)
    {
        fprintf(fp, " extends=\"");
        prScopedPythonName(fp, xtnds->ecd, xtnds->pyname);
        fprintf(fp, "\"");
    }

    no_res = (od->pysig.result.atype == void_type && od->pysig.result.nrderefs == 0);

    /* Handle the trivial case. */
    if (no_res && od->pysig.nrArgs == 0)
    {
        fprintf(fp, "/>\n");
        return FALSE;
    }

    fprintf(fp, ">\n");

    if (!no_res)
        xmlArgument(&od->pysig.result, "out", isResultTransferredBack(od), FALSE, indent, fp);

    need_sec = FALSE;

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        argDef *ad = &od->pysig.args[a];

        /* Ignore the first argument of number slots. */
        if (isNumberSlot(md) && a == 0 && od->pysig.nrArgs == 2)
            continue;

        xmlArgument(ad, dirAttribute(ad), FALSE, sec, indent, fp);

        if (ad->atype == rxcon_type || ad->atype == rxdis_type)
            need_sec = TRUE;
    }

    xmlIndent(--indent, fp);
    fprintf(fp, "</Function>\n");

    return need_sec;
}


/*
 * Generate the XML for a C++ signature.
 */
static void xmlCppSignature(FILE *fp, overDef *od)
{
    prcode(fp, "%M");
    prOverloadDecl(fp, NULL, od, TRUE);
    prcode(fp, "%M");
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
        prcode(fp, " default=\"%M");
        exportDefaultValue(ad, fp);
        prcode(fp, "%M\"");
    }

    fprintf(fp, "/>\n");
}


/*
 * Generate the XML for a type.
 */
static void xmlType(argDef *ad, int sec, FILE *fp)
{
    const char *type_type = NULL, *type_name;
    classDef *type_scope;

    fprintf(fp, " typename=\"");

    switch (ad->atype)
    {
    case class_type:
        type_type = (isOpaque(ad->u.cd) ? "opaque" : "class");
        break;

    case enum_type:
        if (ad->u.ed->pyname != NULL)
            type_type = "enum";
        break;

    case rxcon_type:
    case rxdis_type:
        if (!sec)
            type_type = "class";
        break;

    case qobject_type:
        type_type = "class";
        break;

    case slotcon_type:
    case slotdis_type:
        {
            int a;

            prcode(fp, "SLOT(");

            for (a = 0; a < ad->u.sa->nrArgs; ++a)
            {
                if (a > 0)
                    prcode(fp, ", ");

                prcode(fp, "%M%B%M", &ad->u.sa->args[a]);
            }

            prcode(fp, ")");
        }

        break;

    case mapped_type:
        prcode(fp, "%M%B%M", &ad->u.mtd->type);
        type_type = "mappedtype";
        break;
    }

    if ((type_name = pyType(ad, sec, &type_scope)) != NULL)
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


/*
 * Export the default value of an argument.
 */
static void exportDefaultValue(argDef *ad, FILE *fp)
{
    /* Translate some special cases. */
    if (ad->defval->next == NULL && ad->defval->vtype == numeric_value)
    {
        if (ad->nrderefs > 0 && ad->defval->u.vnum == 0)
        {
            prcode(fp, "None");
            return;
        }

        if (ad->atype == bool_type || ad->atype == cbool_type)
        {
            prcode(fp, ad->defval->u.vnum ? "True" : "False");
            return;
        }
    }

    generateExpression(ad->defval, fp);
}


/*
 * Get the Python representation of a type.
 */
static const char *pyType(argDef *ad, int sec, classDef **scope)
{
    const char *type_name;

    *scope = NULL;

    switch (ad->atype)
    {
    case class_type:
        type_name = ad->u.cd->pyname;
        *scope = ad->u.cd->ecd;
        break;

    case struct_type:
    case void_type:
        type_name = "sip.voidptr";
        break;

    case enum_type:
        if (ad->u.ed->pyname != NULL)
        {
            type_name = ad->u.ed->pyname->text;
            *scope = ad->u.ed->ecd;
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
            type_name = "QObject";

        break;

    case qobject_type:
        type_name = "QObject";
        break;

    case ustring_type:
    case string_type:
    case sstring_type:
    case wstring_type:
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
        type_name = NULL;
    }

    return type_name;
}
