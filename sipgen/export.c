/*
 * The XML and API file generator module for SIP.
 *
 * Copyright (c) 2015 Riverbank Computing Limited <info@riverbankcomputing.com>
 *
 * This file is part of SIP.
 *
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 *
 * This copy of SIP may also used under the terms of the GNU General Public
 * License v2 or v3 as published by the Free Software Foundation which can be
 * found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
 *
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <stdio.h>
#include <string.h>

#include "sip.h"


#define XML_VERSION_NR  0       /* The schema version number. */

/* Icon numbers.  The values are those used by the eric IDE. */
#define CLASS_ID        1
#define METHOD_ID       4
#define VARIABLE_ID     7
#define ENUM_ID         10


static void apiEnums(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp);
static void apiVars(sipSpec *pt, moduleDef *mod, classDef *scope, FILE *fp);
static int apiCtor(sipSpec *pt, moduleDef *mod, classDef *scope, ctorDef *ct,
        int sec, FILE *fp);
static int apiOverload(sipSpec *pt, moduleDef *mod, classDef *scope,
        overDef *od, int sec, FILE *fp);
static void xmlClass(sipSpec *pt, moduleDef *mod, classDef *cd, FILE *fp);
static void xmlEnums(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp);
static void xmlVars(sipSpec *pt, moduleDef *mod, classDef *scope, int indent,
        FILE *fp);
static void xmlFunction(sipSpec *pt, classDef *scope, memberDef *md,
        overDef *oloads, int indent, FILE *fp);
static int xmlCtor(sipSpec *pt, classDef *scope, ctorDef *ct, int sec,
        int indent, FILE *fp);
static int xmlOverload(sipSpec *pt, classDef *scope, memberDef *md,
        overDef *od, classDef *xtnds, int stat, int sec, int indent, FILE *fp);
static void xmlCppSignature(FILE *fp, overDef *od);
static void xmlArgument(sipSpec *pt, argDef *ad, const char *dir, int res_xfer,
        int sec, int indent, FILE *fp);
static void xmlType(sipSpec *pt, argDef *ad, int sec, FILE *fp);
static void xmlIndent(int indent, FILE *fp);
static const char *dirAttribute(argDef *ad);


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

        if (apiOverload(pt, mod, NULL, od, FALSE, fp))
            apiOverload(pt, mod, NULL, od, TRUE, fp);
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

            if (apiCtor(pt, mod, cd, ct, FALSE, fp))
                apiCtor(pt, mod, cd, ct, TRUE, fp);
        }

        for (od = cd->overs; od != NULL; od = od->next)
        {
            if (isPrivate(od))
                continue;

            if (od->common->slot != no_slot)
                continue;

            if (apiOverload(pt, mod, cd, od, FALSE, fp))
                apiOverload(pt, mod, cd, od, TRUE, fp);
        }
    }

    fclose(fp);
}


/*
 * Generate an API ctor.
 */
static int apiCtor(sipSpec *pt, moduleDef *mod, classDef *scope, ctorDef *ct,
        int sec, FILE *fp)
{
    int need_sec = FALSE, need_comma, a;

    /* Do the callable type form. */
    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope->ecd, scope->pyname->text);
    fprintf(fp, "?%d(", CLASS_ID);

    need_comma = FALSE;

    for (a = 0; a < ct->pysig.nrArgs; ++a)
    {
        argDef *ad = &ct->pysig.args[a];

        need_comma = prArgument(pt, ad, FALSE, need_comma, sec, TRUE, TRUE,
                FALSE, fp);

        if (ad->atype == rxcon_type || ad->atype == rxdis_type)
            need_sec = TRUE;
    }

    fprintf(fp, ")\n");

    /* Do the call __init__ form. */
    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope->ecd, scope->pyname->text);
    fprintf(fp, ".__init__?%d(self", CLASS_ID);

    for (a = 0; a < ct->pysig.nrArgs; ++a)
        prArgument(pt, &ct->pysig.args[a], FALSE, TRUE, sec, TRUE, TRUE,
                FALSE, fp);

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

        if (scope != NULL)
        {
            if (ed->ecd != scope)
                continue;
        }
        else if (ed->ecd != NULL || ed->emtd != NULL)
        {
            continue;
        }

        if (ed->pyname != NULL)
        {
            fprintf(fp, "%s.", mod->name);
            prScopedPythonName(fp, ed->ecd, ed->pyname->text);
            fprintf(fp, "?%d\n", ENUM_ID);
        }

        for (emd = ed->members; emd != NULL; emd = emd->next)
        {
            fprintf(fp, "%s.", mod->name);
            prScopedPythonName(fp, ed->ecd, emd->pyname->text);
            fprintf(fp, "?%d\n", ENUM_ID);
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
        fprintf(fp, "?%d\n", VARIABLE_ID);
    }
}


/*
 * Generate a single API overload.
 */
static int apiOverload(sipSpec *pt, moduleDef *mod, classDef *scope,
        overDef *od, int sec, FILE *fp)
{
    int need_sec;

    fprintf(fp, "%s.", mod->name);
    prScopedPythonName(fp, scope, od->common->pyname->text);
    fprintf(fp, "?%d", METHOD_ID);

    need_sec = prPythonSignature(pt, fp, &od->pysig, sec, TRUE, TRUE, FALSE,
            FALSE, FALSE);

    fprintf(fp, "\n");

    return need_sec;
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
        xmlFunction(pt, NULL, md, mod->overs, 1, fp);

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
        fprintf(fp, "<OpaqueClass name=\"");
        prScopedPythonName(fp, cd->ecd, cd->pyname->text);
        fprintf(fp, "\"/>\n");

        return;
    }

    xmlIndent(indent++, fp);
    fprintf(fp, "<Class name=\"");
    prScopedPythonName(fp, cd->ecd, cd->pyname->text);
    fprintf(fp, "\"");

    if (cd->picklecode != NULL)
        fprintf(fp, " pickle=\"1\"");

    if (cd->convtocode != NULL)
        fprintf(fp, " convert=\"1\"");

    if (cd->convfromcode != NULL)
        fprintf(fp, " convertfrom=\"1\"");

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

            prScopedPythonName(fp, cl->cd->ecd, cl->cd->pyname->text);
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

        if (xmlCtor(pt, cd, ct, FALSE, indent, fp))
            xmlCtor(pt, cd, ct, TRUE, indent, fp);
    }

    for (md = cd->members; md != NULL; md = md->next)
        xmlFunction(pt, cd, md, cd->overs, indent, fp);

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

        if (scope != NULL)
        {
            if (ed->ecd != scope)
                continue;
        }
        else if (ed->ecd != NULL || ed->emtd != NULL)
        {
            continue;
        }

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

        xmlType(pt, &vd->type, FALSE, fp);
        fprintf(fp, "/>\n");
    }
}


/*
 * Generate the XML for a ctor.
 */
static int xmlCtor(sipSpec *pt, classDef *scope, ctorDef *ct, int sec,
        int indent, FILE *fp)
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

        xmlArgument(pt, ad, dirAttribute(ad), FALSE, sec, indent, fp);

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
static void xmlFunction(sipSpec *pt, classDef *scope, memberDef *md,
        overDef *oloads, int indent, FILE *fp)
{
    overDef *od;
    const char *default_str = "default=\"1\" ";

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
            fprintf(fp, "<Signal %sname=\"", default_str);
            prScopedPythonName(fp, scope, md->pyname->text);
            fprintf(fp, "\" sig=\"");
            xmlCppSignature(fp, od);
            fprintf(fp, "\"/>\n");

            default_str = "";

            continue;
        }

        xtnds = NULL;
        isstat = (scope == NULL || scope->iff->type == namespace_iface || isStatic(od));

        if (scope == NULL && md->slot != no_slot && od->pysig.args[0].atype == class_type)
        {
            xtnds = od->pysig.args[0].u.cd;
            isstat = FALSE;
        }

        if (xmlOverload(pt, scope, md, od, xtnds, isstat, FALSE, indent, fp))
            xmlOverload(pt, scope, md, od, xtnds, isstat, TRUE, indent, fp);
    }
}


/*
 * Generate the XML for an overload.
 */
static int xmlOverload(sipSpec *pt, classDef *scope, memberDef *md,
        overDef *od, classDef *xtnds, int stat, int sec, int indent, FILE *fp)
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

    if (isVirtual(od))
    {
        fprintf(fp, " virtual=\"1\"");
    }

    if (xtnds != NULL)
    {
        fprintf(fp, " extends=\"");
        prScopedPythonName(fp, xtnds->ecd, xtnds->pyname->text);
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
        xmlArgument(pt, &od->pysig.result, "out", isResultTransferredBack(od),
                FALSE, indent, fp);

    need_sec = FALSE;

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        argDef *ad = &od->pysig.args[a];

        /* Ignore the first argument of number slots. */
        if (isNumberSlot(md) && a == 0 && od->pysig.nrArgs == 2)
            continue;

        xmlArgument(pt, ad, dirAttribute(ad), FALSE, sec, indent, fp);

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
static void xmlArgument(sipSpec *pt, argDef *ad, const char *dir, int res_xfer,
        int sec, int indent, FILE *fp)
{
    if (isArraySize(ad))
        return;

    if (sec && (ad->atype == slotcon_type || ad->atype == slotdis_type))
        return;

    xmlIndent(indent, fp);
    fprintf(fp, "<Argument");
    xmlType(pt, ad, sec, fp);

    if (dir != NULL)
        fprintf(fp, " dir=\"%s\"", dir);

    if (isAllowNone(ad))
        fprintf(fp, " allownone=\"1\"");

    if (isDisallowNone(ad))
        fprintf(fp, " disallownone=\"1\"");

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
        prDefaultValue(ad, FALSE, fp);
        prcode(fp, "%M\"");
    }

    fprintf(fp, "/>\n");
}


/*
 * Generate the XML for a type.
 */
static void xmlType(sipSpec *pt, argDef *ad, int sec, FILE *fp)
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
        type_type = "mappedtype";
        break;

    /* Suppress a compiler warning. */
    default:
        ;
    }

    if ((type_name = pyType(pt, ad, sec, &type_scope)) != NULL)
        prScopedPythonName(fp, type_scope, type_name);

    fprintf(fp, "\"");

    if (type_type != NULL)
        fprintf(fp, " typetype=\"%s\"", type_type);

    if (ad->name != NULL)
        fprintf(fp, " name=\"%s\"", ad->name->text);
}


/*
 * Generate the indentation for a line.
 */
static void xmlIndent(int indent, FILE *fp)
{
    while (indent-- > 0)
        fprintf(fp, "  ");
}
