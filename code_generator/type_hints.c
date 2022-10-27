/*
 * The PEP 484 type hints generator for SIP.
 *
 * Copyright (c) 2022 Riverbank Computing Limited <info@riverbankcomputing.com>
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


static void pyiPythonSignature(sipSpec *pt, moduleDef *mod, signatureDef *sd,
        int need_self, KwArgs kwargs, FILE *fp);
static int pyiArgument(sipSpec *pt, moduleDef *mod, argDef *ad, int arg_nr,
        int out, int need_comma, int names, int defaults, KwArgs kwargs,
        FILE *fp);
static void pyiType(sipSpec *pt, moduleDef *mod, argDef *ad, int out,
        FILE *fp);
static void pyiTypeHintNode(typeHintNodeDef *node, moduleDef *mod, FILE *fp);
static void prClassRef(classDef *cd, FILE *fp);
static void prEnumRef(enumDef *ed, FILE *fp);
static void prScopedEnumName(FILE *fp, enumDef *ed);
static void parseTypeHint(sipSpec *pt, typeHintDef *thd, int out);
static int parseTypeHintNode(sipSpec *pt, int out, int top_level, char *start,
        char *end, typeHintNodeDef **thnp);
static const char *typingModule(const char *name);
static typeHintNodeDef *lookupType(sipSpec *pt, char *name, int out);
static enumDef *lookupEnum(sipSpec *pt, const char *name, classDef *scope_cd,
        mappedTypeDef *scope_mtd);
static mappedTypeDef *lookupMappedType(sipSpec *pt, const char *name);
static classDef *lookupClass(sipSpec *pt, const char *name,
        classDef *scope_cd);
static void maybeAnyObject(const char *hint, FILE *fp);
static void strip_leading(char **startp, char *end);
static void strip_trailing(char *start, char **endp);
static typeHintNodeDef *flatten_unions(typeHintNodeDef *nodes);
static typeHintNodeDef *copyTypeHintNode(sipSpec *pt, typeHintDef *thd,
        int out);
static int isPyKeyword(const char *word);


/*
 * Generate an ctor type hint.
 */
void pyiCtor(sipSpec *pt, moduleDef *mod, classDef *cd, ctorDef *ct, FILE *fp)
{
    int a, need_comma = FALSE;

    prScopedPythonName(fp, cd->ecd, cd->pyname->text);
    fprintf(fp, "(");

    for (a = 0; a < ct->pysig.nrArgs; ++a)
        need_comma = pyiArgument(pt, mod, &ct->pysig.args[a], a, FALSE,
                need_comma, TRUE, TRUE, ct->kwargs, fp);

    fprintf(fp, ")");
}


/*
 * Generate the type hints for a single API overload.
 */
void pyiOverload(sipSpec *pt, moduleDef *mod, overDef *od, int is_method,
        FILE *fp)
{
    int need_self = (is_method && !isStatic(od));

    fprintf(fp, "%s", od->common->pyname->text);
    pyiPythonSignature(pt, mod, &od->pysig, need_self, od->kwargs, fp);
}


/*
 * Generate a Python argument.
 */
static int pyiArgument(sipSpec *pt, moduleDef *mod, argDef *ad, int arg_nr,
        int out, int need_comma, int names, int defaults, KwArgs kwargs,
        FILE *fp)
{
    int optional, use_optional;

    if (isArraySize(ad))
        return need_comma;

    if (need_comma)
        fprintf(fp, ", ");

    optional = (defaults && ad->defval && !out);

    if (names && ad->atype != ellipsis_type)
    {
        if (ad->name != NULL)
            fprintf(fp, "%s%s: ", ad->name->text,
                    (isPyKeyword(ad->name->text) ? "_" : ""));
        else
            fprintf(fp, "a%d: ", arg_nr);
    }

    use_optional = FALSE;

    if (optional)
    {
        /* Assume pointers can be None unless specified otherwise. */
        if (isAllowNone(ad) || (!isDisallowNone(ad) && ad->nrderefs > 0))
        {
            fprintf(fp, "typing.Optional[");
            use_optional = TRUE;
        }
    }

    if (isArray(ad))
        fprintf(fp, "%s.array[", (sipName != NULL) ? sipName : "sip");

    pyiType(pt, mod, ad, out, fp);

    if (names && ad->atype == ellipsis_type)
    {
        if (ad->name != NULL)
            fprintf(fp, "%s%s", ad->name->text,
                    (isPyKeyword(ad->name->text) ? "_" : ""));
        else
            fprintf(fp, "a%d", arg_nr);
    }

    if (isArray(ad))
        fprintf(fp, "]");

    if (optional)
    {
        if (use_optional)
            fprintf(fp, "]");

        fprintf(fp, " = ");

        prDefaultValue(ad, fp);
    }

    return TRUE;
}


/*
 * Generate the default value of an argument.
 */
void prDefaultValue(argDef *ad, FILE *fp)
{
    /* Use any explicitly provided documentation. */
    if (ad->typehint_value != NULL)
    {
        fprintf(fp, "%s", ad->typehint_value);
        return;
    }

    /* Translate some special cases. */
    if (ad->defval->next == NULL && ad->defval->vtype == numeric_value)
    {
        if (ad->nrderefs > 0 && ad->defval->u.vnum == 0)
        {
            fprintf(fp, "None");
            return;
        }

        if (ad->atype == bool_type || ad->atype == cbool_type)
        {
            fprintf(fp, ad->defval->u.vnum ? "True" : "False");
            return;
        }
    }

    /* SIP v5 won't need this. */
    prcode(fp, "%M");
    generateExpression(ad->defval, TRUE, fp);
    prcode(fp, "%M");
}


/*
 * Generate the Python representation of a type.
 */
static void pyiType(sipSpec *pt, moduleDef *mod, argDef *ad, int out, FILE *fp)
{
    const char *type_name, *sip_name;
    typeHintDef *thd;

    /* Use any explicit type hint unless the argument is constrained. */
    thd = (out ? ad->typehint_out : (isConstrained(ad) ? NULL : ad->typehint_in));

    if (thd != NULL)
    {
        pyiTypeHint(pt, thd, mod, out, fp);
        return;
    }

    type_name = NULL;

    sip_name = (sipName != NULL ? sipName : "sip");

    switch (ad->atype)
    {
    case class_type:
        prClassRef(ad->u.cd, fp);
        break;

    case mapped_type:
        /*
         * This should never happen as it should have been picked up when
         * generating code - but maybe we haven't been asked to generate code.
         */
        fprintf(fp, "object");
        break;

    case enum_type:
        if (ad->u.ed->pyname != NULL)
            prEnumRef(ad->u.ed, fp);
        else
            type_name = "int";

        break;

    case capsule_type:
        type_name = scopedNameTail(ad->u.cap);
        break;

    case struct_type:
    case void_type:
        fprintf(fp, "%s.voidptr", sip_name);
        break;

    case string_type:
    case sstring_type:
    case ustring_type:
        type_name = "bytes";
        break;

    case wstring_type:
    case ascii_string_type:
    case latin1_string_type:
    case utf8_string_type:
        type_name = isArray(ad) ? "bytes" : "str";
        break;

    case byte_type:
    case sbyte_type:
    case ubyte_type:
    case ushort_type:
    case uint_type:
    case long_type:
    case longlong_type:
    case ulong_type:
    case ulonglong_type:
    case short_type:
    case int_type:
    case cint_type:
    case ssize_type:
    case size_type:
    case hash_type:
        type_name = "int";
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
        type_name = "Tuple";
        break;

    case pylist_type:
        type_name = "List";
        break;

    case pydict_type:
        type_name = "Dict";
        break;

    case pycallable_type:
        type_name = "Callable[..., None]";
        break;

    case pyslice_type:
        type_name = "slice";
        break;

    case pytype_type:
        type_name = "type";
        break;

    case pybuffer_type:
        /* This replicates sip.pyi. */
        fprintf(fp, "Union[bytes, bytearray, memoryview, %s.array, %s.voidptr]", sip_name, sip_name);
        break;

    case pyenum_type:
        type_name = "enum.Enum";
        break;

    case ellipsis_type:
        type_name = "*";
        break;

    default:
        type_name = "object";
    }

    if (type_name != NULL)
        fprintf(fp, "%s", type_name);
}


/*
 * Generate a scoped Python name.
 */
void prScopedPythonName(FILE *fp, classDef *scope, const char *pyname)
{
    if (scope != NULL && !isHiddenNamespace(scope))
    {
        prScopedPythonName(fp, scope->ecd, NULL);
        fprintf(fp, "%s.", scope->pyname->text);
    }

    if (pyname != NULL)
        fprintf(fp, "%s", pyname);
}


/*
 * Generate a Python signature.
 */
static void pyiPythonSignature(sipSpec *pt, moduleDef *mod, signatureDef *sd,
        int need_self, KwArgs kwargs, FILE *fp)
{
    int void_return, need_comma, is_res, nr_out, a;

    if (need_self)
    {
        fprintf(fp, "(self");
        need_comma = TRUE;
    }
    else
    {
        fprintf(fp, "(");
        need_comma = FALSE;
    }

    nr_out = 0;

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        if (isOutArg(ad))
            ++nr_out;

        if (!isInArg(ad))
            continue;

        need_comma = pyiArgument(pt, mod, ad, a, FALSE, need_comma, TRUE, TRUE,
                kwargs, fp);
    }

    fprintf(fp, ")");

    /* An empty type hint specifies a void return. */
    if (sd->result.typehint_out != NULL)
        void_return = (sd->result.typehint_out->raw_hint[0] == '\0');
    else
        void_return = FALSE;

    is_res = !((sd->result.atype == void_type && sd->result.nrderefs == 0) ||
            void_return);

    if (is_res || nr_out > 0)
    {
        fprintf(fp, " -> ");

        if ((is_res && nr_out > 0) || nr_out > 1)
            fprintf(fp, "Tuple[");

        if (is_res)
            need_comma = pyiArgument(pt, mod, &sd->result, -1, TRUE, FALSE,
                    FALSE, FALSE, kwargs, fp);
        else
            need_comma = FALSE;

        for (a = 0; a < sd->nrArgs; ++a)
        {
            argDef *ad = &sd->args[a];

            if (isOutArg(ad))
                /* We don't want the name in the result tuple. */
                need_comma = pyiArgument(pt, mod, ad, -1, TRUE, need_comma,
                        FALSE, FALSE, kwargs, fp);
        }

        if ((is_res && nr_out > 0) || nr_out > 1)
            fprintf(fp, "]");
    }
}


/*
 * Generate a class reference, including its owning module if necessary and
 * handling forward references if necessary.
 */
static void prClassRef(classDef *cd, FILE *fp)
{
    prScopedPythonName(fp, cd->ecd, cd->pyname->text);
}


/*
 * Generate an enum reference, including its owning module if necessary and
 * handling forward references if necessary.
 */
static void prEnumRef(enumDef *ed, FILE *fp)
{
    prScopedEnumName(fp, ed);
}


/*
 * Generate a scoped enum name.
 */
static void prScopedEnumName(FILE *fp, enumDef *ed)
{
    if (ed->emtd != NULL)
        fprintf(fp, "%s.%s", ed->emtd->pyname->text, ed->pyname->text);
    else
        prScopedPythonName(fp, ed->ecd, ed->pyname->text);
}


/*
 * Generate a type hint from a /TypeHint/ annotation.
 */
void pyiTypeHint(sipSpec *pt, typeHintDef *thd, moduleDef *mod, int out,
        FILE *fp)
{
    parseTypeHint(pt, thd, out);

    if (thd->root != NULL)
        pyiTypeHintNode(thd->root, mod, fp);
    else
        maybeAnyObject(thd->raw_hint, fp);
}


/*
 * Generate a single node of a type hint.
 */
static void pyiTypeHintNode(typeHintNodeDef *node, moduleDef *mod, FILE *fp)
{
    switch (node->type)
    {
    case typing_node:
        if (node->u.name != NULL)
            fprintf(fp, "%s", node->u.name);

        if (node->children != NULL)
        {
            int need_comma = FALSE;
            typeHintNodeDef *thnd;

            fprintf(fp, "[");

            for (thnd = node->children; thnd != NULL; thnd = thnd->next)
            {
                if (need_comma)
                    fprintf(fp, ", ");

                need_comma = TRUE;

                pyiTypeHintNode(thnd, mod, fp);
            }

            fprintf(fp, "]");
        }

        break;

    case class_node:
        prClassRef(node->u.cd, fp);
        break;

    case enum_node:
        prEnumRef(node->u.ed, fp);
        break;

    case other_node:
        maybeAnyObject(node->u.name, fp);
        break;
    }
}


/*
 * Parse a type hint and update its status accordingly.
 */
static void parseTypeHint(sipSpec *pt, typeHintDef *thd, int out)
{
    if (thd->status == needs_parsing)
    {
        thd->status = being_parsed;
        parseTypeHintNode(pt, out, TRUE, thd->raw_hint,
                thd->raw_hint + strlen(thd->raw_hint), &thd->root);
        thd->status = parsed;
    }
}


/*
 * Recursively parse a type hint.  Return FALSE if the parse failed.
 */
static int parseTypeHintNode(sipSpec *pt, int out, int top_level, char *start,
        char *end, typeHintNodeDef **thnp)
{
    char *cp, *name_start, *name_end;
    int have_brackets = FALSE;
    typeHintNodeDef *node, *children = NULL;

    /* Assume there won't be a node. */
    *thnp = NULL;

    /* Find the name and any opening and closing bracket. */
    strip_leading(&start, end);
    name_start = start;

    strip_trailing(start, &end);
    name_end = end;

    for (cp = start; cp < end; ++cp)
        if (*cp == '[')
        {
            typeHintNodeDef **tail = &children;

            /* The last character must be a closing bracket. */
            if (end[-1] != ']')
                return FALSE;

            /* Find the end of any name. */
            name_end = cp;
            strip_trailing(name_start, &name_end);

            for (;;)
            {
                char *pp;
                int depth;

                /* Skip the opening bracket or comma. */
                ++cp;

                /* Find the next comma, if any. */
                depth = 0;

                for (pp = cp; pp < end; ++pp)
                    if (*pp == '[')
                    {
                        ++depth;
                    }
                    else if (*pp == ']' && depth != 0)
                    {
                        --depth;
                    }
                    else if ((*pp == ',' || *pp == ']') && depth == 0)
                    {
                        typeHintNodeDef *child;

                        /* Recursively parse this part. */
                        if (!parseTypeHintNode(pt, out, FALSE, cp, pp, &child))
                            return FALSE;

                        if (child != NULL)
                        {
                            /*
                             * Append the child to the list of children.  There
                             * might not be a child if we have detected a
                             * recursive definition.
                             */
                            *tail = child;
                            tail = &child->next;
                        }

                        cp = pp;
                        break;
                    }

                if (pp == end)
                    break;
            }

            have_brackets = TRUE;

            break;
        }

    /* See if we have a name. */
    if (name_start == name_end)
    {
        /*
         * At the top level we must have brackets and they must not be empty.
         */
        if (top_level && (!have_brackets || children == NULL))
            return FALSE;

        /* Return the representation of brackets. */
        node = sipMalloc(sizeof (typeHintNodeDef));
        node->type = typing_node;
        node->u.name = NULL;
        node->children = children;
    }
    else
    {
        char saved;
        const char *typing;

        /* Isolate the name. */
        saved = *name_end;
        *name_end = '\0';

        /* See if it is an object in the typing module. */
        if ((typing = typingModule(name_start)) != NULL)
        {
            if (strcmp(typing, "Union") == 0)
            {
                /*
                 * If there are no children assume it is because they have been
                 * omitted.
                 */
                if (children == NULL)
                    return TRUE;

                children = flatten_unions(children);
            }

            node = sipMalloc(sizeof (typeHintNodeDef));
            node->type = typing_node;
            node->u.name = typing;
            node->children = children;
        }
        else
        {
            /* Search for the type. */
            node = lookupType(pt, name_start, out);
        }

        *name_end = saved;

        /* Only objects from the typing module can have brackets. */
        if (typing == NULL && have_brackets)
            return FALSE;
    }

    *thnp = node;

    return TRUE;
}


/*
 * Strip leading spaces from a string.
 */
static void strip_leading(char **startp, char *end)
{
    char *start;

    start = *startp;

    while (start < end && start[0] == ' ')
        ++start;

    *startp = start;
}


/*
 * Strip trailing spaces from a string.
 */
static void strip_trailing(char *start, char **endp)
{
    char *end;

    end = *endp;

    while (end > start && end[-1] == ' ')
        --end;

    *endp = end;
}


/*
 * Look up an object in the typing module.
 */
static const char *typingModule(const char *name)
{
    static const char *typing[] = {
        "Any",
        "Callable",
        "Dict",
        "Iterable",
        "Iterator",
        "List",
        "Mapping",
        "NamedTuple",
        "Optional",
        "Sequence",
        "Set",
        "Tuple",
        "Union",
        NULL
    };

    const char **np;

    for (np = typing; *np != NULL; ++np)
        if (strcmp(*np, name) == 0)
            return *np;

    return NULL;
}


/*
 * Flatten any unions in a list of nodes.
 */
static typeHintNodeDef *flatten_unions(typeHintNodeDef *nodes)
{
    typeHintNodeDef *head, **tailp, *thnd;

    head = NULL;
    tailp = &head;

    for (thnd = nodes; thnd != NULL; thnd = thnd->next)
    {
        if (thnd->type == typing_node && strcmp(thnd->u.name, "Union") == 0)
        {
            typeHintNodeDef *child;

            for (child = thnd->children; child != NULL; child = child->next)
            {
                *tailp = child;
                tailp = &child->next;
            }
        }
        else
        {
            /* Move this one to the new list. */
            *tailp = thnd;
            tailp = &thnd->next;
        }
    }

    *tailp = NULL;

    return head;
}


/*
 * Look up a qualified Python type and return the corresponding node (or NULL
 * if the type should be omitted because of a recursive definition).
 */
static typeHintNodeDef *lookupType(sipSpec *pt, char *name, int out)
{
    char *sp, *ep;
    classDef *scope_cd;
    mappedTypeDef *scope_mtd;
    typeHintDef *thd;
    typeHintNodeDef *node;

    /* Start searching at the global level. */
    scope_cd = NULL;
    scope_mtd = NULL;

    sp = name;
    ep = NULL;

    while (*sp != '\0')
    {
        enumDef *ed;

        /* Isolate the next part of the name. */
        if ((ep = strchr(sp, '.')) != NULL)
            *ep = '\0';

        /* See if it's an enum. */
        if ((ed = lookupEnum(pt, sp, scope_cd, scope_mtd)) != NULL)
        {
            /* Make sure we have used the whole name. */
            if (ep == NULL)
            {
                node = sipMalloc(sizeof (typeHintNodeDef));
                node->type = enum_node;
                node->u.ed = ed;

                return node;
            }

            /* There is some left so the whole lookup has failed. */
            break;
        }

        /*
         * If we have a mapped type scope then we must be looking for an enum,
         * which we have failed to find.
         */
        if (scope_mtd != NULL)
            break;

        if (scope_cd == NULL)
        {
            mappedTypeDef *mtd;

            /*
             * We are looking at the global level, so see if it is a mapped
             * type.
             */
            if ((mtd = lookupMappedType(pt, sp)) != NULL)
            {
                /*
                 * If we have used the whole name then the lookup has
                 * succeeded.
                 */
                if (ep == NULL)
                {
                    thd = (out ? mtd->typehint_out : mtd->typehint_in);

                    if (thd != NULL && thd->status != being_parsed)
                        return copyTypeHintNode(pt, thd, out);

                    /*
                     * If we get here we have a recursively defined mapped type
                     * so we simply omit it.
                     */
                    return NULL;
                }

                /* Otherwise this is the scope for the next part. */
                scope_mtd = mtd;
            }
        }

        if (scope_mtd == NULL)
        {
            classDef *cd;

            /* If we get here then it must be a class. */
            if ((cd = lookupClass(pt, sp, scope_cd)) == NULL)
                break;

            /* If we have used the whole name then the lookup has succeeded. */
            if (ep == NULL)
            {
                thd = (out ? cd->typehint_out : cd->typehint_in);

                if (thd != NULL && thd->status != being_parsed)
                    return copyTypeHintNode(pt, thd, out);

                node = sipMalloc(sizeof (typeHintNodeDef));
                node->type = class_node;
                node->u.cd = cd;

                return node;
            }

            /* Otherwise this is the scope for the next part. */
            scope_cd = cd;
        }

        /* If we have run out of name then the lookup has failed. */
        if (ep == NULL)
            break;

        /* Repair the name and go on to the next part. */
        *ep++ = '.';
        sp = ep;
    }

    /* Repair the name. */
    if (ep != NULL)
        *ep = '.';

    /* Nothing was found. */
    node = sipMalloc(sizeof (typeHintNodeDef));
    node->type = other_node;
    node->u.name = sipStrdup(name);

    return node;
}


/*
 * Copy the root node of a type hint.
 */
static typeHintNodeDef *copyTypeHintNode(sipSpec *pt, typeHintDef *thd,
        int out)
{
    typeHintNodeDef *node;

    parseTypeHint(pt, thd, out);

    if (thd->root == NULL)
        return NULL;

    node = sipMalloc(sizeof (typeHintNodeDef));
    *node = *thd->root;
    node->next = NULL;

    return node;
}


/*
 * Lookup an enum using its C/C++ name.
 */
static enumDef *lookupEnum(sipSpec *pt, const char *name, classDef *scope_cd,
        mappedTypeDef *scope_mtd)
{
    enumDef *ed;

    for (ed = pt->enums; ed != NULL; ed = ed->next)
        if (ed->fqcname != NULL && strcmp(scopedNameTail(ed->fqcname), name) == 0 && ed->ecd == scope_cd && ed->emtd == scope_mtd)
            return ed;

    return NULL;
}


/*
 * Lookup a mapped type using its C/C++ name.
 */
static mappedTypeDef *lookupMappedType(sipSpec *pt, const char *name)
{
    mappedTypeDef *mtd;

    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->cname != NULL && strcmp(mtd->cname->text, name) == 0)
            return mtd;

    return NULL;
}


/*
 * Lookup a class/struct/union using its C/C++ name.
 */
static classDef *lookupClass(sipSpec *pt, const char *name, classDef *scope_cd)
{
    classDef *cd;

    for (cd = pt->classes; cd != NULL; cd = cd->next)
        if (strcmp(classBaseName(cd), name) == 0 && cd->ecd == scope_cd && !isExternal(cd))
            return cd;

    return NULL;
}


/*
 * Generate a hint taking into account that it may be any sort of object.
 */
static void maybeAnyObject(const char *hint, FILE *fp)
{
    fprintf(fp, "%s", (strcmp(hint, "Any") != 0 ? hint : "object"));
}


/*
 * Check if a word is a Python keyword (or has been at any time).
 */
static int isPyKeyword(const char *word)
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
