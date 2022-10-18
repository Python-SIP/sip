# Copyright (c) 2022, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


from ..formatters import (ArgumentFormatter, EnumFormatter, ClassFormatter,
        VariableFormatter)
from ..scoped_name import STRIP_GLOBAL
from ..specification import KwArgs


# The schema version number.
_XML_VERSION_NR = 0


def output_xml(spec, module, xml_filename):
    """ Output an XML export file. """

    # Note that we don't yet handle mapped types, templates or exceptions.

    with open(xml_filename, 'w') as xf:
        xf.write('<?xml version="1.0"?>\n')
        xf.write(f'<Module version="{_XML_VERSION_NR}" name="{module.py_name}">\n')

        for klass in spec.classes:
            if klass.iface_file.module is module and not klass.external:
                _xml_class(xf, spec, module, klass)

        for klass in module.proxies:
            _xml_class(xf, spec, module, klass)

        _xml_enums(xf, spec, module)
        _xml_variables(xf, spec, module)

        for member in module.global_functions:
            _xml_function(xf, spec, module, member, module.overloads)

        xf.write('</Module>\n')


/*
 * Generate a 'realname' attribute containing a fully qualified C/C++ name of
 * an object.
 */
static void xmlRealScopedName(classDef *scope, const char *cppname, FILE *fp)
{
    const char *sep = "";

    fprintf(fp, " realname=\"");

    if (scope != NULL)
    {
        scopedNameDef *snd;

        for (snd = removeGlobalScope(classFQCName(scope)); snd != NULL; snd = snd->next)
        {
            fprintf(fp, "%s%s", sep, snd->name);
            sep = "::";
        }
    }

    fprintf(fp, "%s%s\"", sep, cppname);
}


def _xml_realname(fq_cpp_name, member=None):
    """ Return a 'realname' attribute containing a fully qualified C/C++ name.
    """

    s = ' realname="' + fq_cpp_name.cpp_stripped(STRIP_GLOBAL)

    if member is not None:
        s += '::' + member

    s += '"'

    return s


ZZZ
def _xml_class(xf, spec, module, klass):
    """ Generate the XML for a class. """
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

    if (!isHiddenNamespace(cd))
    {
        xmlIndent(indent++, fp);
        fprintf(fp, "<Class name=\"");
        prScopedPythonName(fp, cd->ecd, cd->pyname->text);
        fprintf(fp, "\"");

        s += _xml_realname(classFQCName(cd))

        if (cd->picklecode != NULL)
            fprintf(fp, " pickle=\"1\"");

        if (cd->convtocode != NULL)
            fprintf(fp, " convert=\"1\"");

        if (cd->convfromcode != NULL)
            fprintf(fp, " convertfrom=\"1\"");

        if (cd->real != NULL)
            fprintf(fp, " extends=\"%s\"", cd->real->iff->module->name);

        if (cd->pyqt_flags_enums != NULL)
        {
            const char *sep;
            stringList *sl;

            fprintf(fp, " flagsenums=\"");
            sep = "";

            for (sl = cd->pyqt_flags_enums; sl != NULL; sl = sl->next)
            {
                fprintf(fp, "%s%s", sep, sl->s);
                sep = " ";
            }

            fprintf(fp, "\"");
        }

        if (cd->supers != NULL)
        {
            classList *cl;

            fprintf(fp, " inherits=\"");

            for (cl = cd->supers; cl != NULL; cl = cl->next)
            {
                if (cl != cd->supers)
                    fprintf(fp, " ");

                restPyClass(cl->cd, fp);
            }

            fprintf(fp, "\"");
        }

        fprintf(fp, ">\n");
    }

    for (ct = cd->ctors; ct != NULL; ct = ct->next)
    {
        if (isPrivateCtor(ct))
            continue;

        xmlCtor(pt, mod, cd, ct, indent, fp);
    }

    _xml_enums(xf, spec, module, cd, indent)
    _xml_variables(xf, spec, module, cd, indent)

    for (md = cd->members; md != NULL; md = md->next)
        _xml_function(xf, spec, module,  md, cd->overs, cd, indent)

    if (!isHiddenNamespace(cd))
    {
        xmlIndent(--indent, fp);
        fprintf(fp, "</Class>\n");
    }
}


ZZZ
def _xml_enums(xf, spec, module, scope=None, indent=1):
    """ Generate the XML for all the enums in a scope. """
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
            fprintf(fp, "\"");

            s += _xml_realname(ed->fqcname)

            fprintf(fp, ">\n");

            for (emd = ed->members; emd != NULL; emd = emd->next)
            {
                xmlIndent(indent, fp);
                fprintf(fp, "<EnumMember name=\"");
                prScopedPythonName(fp, ed->ecd, ed->pyname->text);
                fprintf(fp, ".%s\"", emd->pyname->text);

                s += _xml_realname(ed->fqcname, emd->cname)

                fprintf(fp, "/>\n");
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
                fprintf(fp, "\"");

                xmlRealScopedName(scope, emd->cname, fp);

                fprintf(fp, " const=\"1\" typename=\"int\"/>\n");
            }
        }
    }
}


def _xml_variables(xf, spec, module, scope=None, indent=1):
    """ Generate the XML for all the variables in a scope. """

    for variable in spec.variables:
        if variable.module is module and variable.scope is scope:
            formatter = VariableFormatter(variable)

            s = ' ' * indent

            s += f'<Member name="{formatter.fq_py_name}"'

            s += _xml_realname(variable.fq_cpp_name)

            if variable.type.is_const or scope is None:
                s += ' const="1"'

            if variable.is_static:
                s += ' static="1"'

            s += _xml_type(spec, module, variable.type) + '/>\n'

            xf.write(s)


/*
 * Generate the XML for a ctor.
 */
static void xmlCtor(sipSpec *pt, moduleDef *mod, classDef *scope, ctorDef *ct,
        int indent, FILE *fp)
{
    int a;

    xmlIndent(indent++, fp);
    fprintf(fp, "<Function name=\"");
    prScopedPythonName(fp, scope, "__init__");
    fprintf(fp, "\"");

    xmlRealScopedName(scope, "__init__", fp);

    if (hasCppSignature(ct->cppsig))
    {
        fprintf(fp, " cppsig=\"");
        xmlCppSignature(fp, ct->cppsig, FALSE);
        fprintf(fp, "\"");
    }

    /* Handle the trivial case. */
    if (ct->pysig.nrArgs == 0)
    {
        fprintf(fp, "/>\n");
        return;
    }

    fprintf(fp, ">\n");

    for (a = 0; a < ct->pysig.nrArgs; ++a)
    {
        argDef *ad = &ct->pysig.args[a];

        if (isInArg(ad))
            xmlArgument(pt, mod, ad, FALSE, ct->kwargs, FALSE, indent, fp);

        if (isOutArg(ad))
            xmlArgument(pt, mod, ad, TRUE, ct->kwargs, FALSE, indent, fp);
    }

    xmlIndent(--indent, fp);
    fprintf(fp, "</Function>\n");
}


def _xml_function(xf, spec, module, member, overloads, scope=None, indent=1):
    """ Generate the XML for a function. """
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
            int a;

            xmlIndent(indent++, fp);
            fprintf(fp, "<Signal name=\"");
            prScopedPythonName(fp, scope, md->pyname->text);
            fprintf(fp, "\"");

            xmlRealScopedName(scope, od->cppname, fp);

            if (hasCppSignature(od->cppsig))
            {
                fprintf(fp, " cppsig=\"");
                xmlCppSignature(fp, od->cppsig, FALSE);
                fprintf(fp, "\"");
            }

            /* Handle the trivial case. */
            if (od->pysig.nrArgs == 0)
            {
                fprintf(fp, "/>\n");
                continue;
            }

            fprintf(fp, ">\n");

            for (a = 0; a < od->pysig.nrArgs; ++a)
            {
                argDef *ad = &od->pysig.args[a];

                xmlArgument(pt, mod, ad, FALSE, od->kwargs, FALSE, indent, fp);
            }

            xmlIndent(--indent, fp);
            fprintf(fp, "</Signal>\n");

            continue;
        }

        xtnds = NULL;
        isstat = (scope == NULL || scope->iff->type == namespace_iface || isStatic(od));

        if (scope == NULL && md->slot != no_slot && od->pysig.args[0].atype == class_type)
        {
            xtnds = od->pysig.args[0].u.cd;
            isstat = FALSE;
        }

        xmlOverload(pt, mod, scope, md, od, xtnds, isstat, indent, fp);
    }
}


/*
 * Generate the XML for an overload.
 */
static void xmlOverload(sipSpec *pt, moduleDef *mod, classDef *scope,
        memberDef *md, overDef *od, classDef *xtnds, int stat, int indent,
        FILE *fp)
{
    const char *name, *cppname = od->cppname;
    int a, no_res;

    xmlIndent(indent++, fp);
    fprintf(fp, "<Function name=\"");

    if (isReflected(od))
    {
        if ((name = reflectedSlot(md->slot)) != NULL)
            cppname = name;
        else
            name = md->pyname->text;
    }
    else
    {
        name = md->pyname->text;
    }

    prScopedPythonName(fp, scope, name);

    fprintf(fp, "\"");

    xmlRealScopedName(scope, cppname, fp);

    if (hasCppSignature(od->cppsig))
    {
        fprintf(fp, " cppsig=\"");
        xmlCppSignature(fp, od->cppsig, isConst(od));
        fprintf(fp, "\"");
    }

    if (isAbstract(od))
        fprintf(fp, " abstract=\"1\"");

    if (stat)
        fprintf(fp, " static=\"1\"");

    if (isSlot(od))
        fprintf(fp, " slot=\"1\"");

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

    /* An empty type hint specifies a void return. */
    if (od->pysig.result.typehint_out != NULL && od->pysig.result.typehint_out->raw_hint[0] == '\0')
        no_res = TRUE;
    else
        no_res = (od->pysig.result.atype == void_type && od->pysig.result.nrderefs == 0);

    /* Handle the trivial case. */
    if (no_res && od->pysig.nrArgs == 0)
    {
        fprintf(fp, "/>\n");
        return;
    }

    fprintf(fp, ">\n");

    if (!no_res)
        xmlArgument(pt, mod, &od->pysig.result, TRUE, NoKwArgs,
                isResultTransferredBack(od), indent, fp);

    for (a = 0; a < od->pysig.nrArgs; ++a)
    {
        argDef *ad = &od->pysig.args[a];

        /*
         * Ignore the first argument of non-reflected number slots and the
         * second argument of reflected number slots.
         */
        if (isNumberSlot(md) && od->pysig.nrArgs == 2)
            if ((a == 0 && !isReflected(od)) || (a == 1 && isReflected(od)))
                continue;

        if (isInArg(ad))
            xmlArgument(pt, mod, ad, FALSE, od->kwargs, FALSE, indent, fp);

        if (isOutArg(ad))
            xmlArgument(pt, mod, ad, TRUE, od->kwargs, FALSE, indent, fp);
    }

    xmlIndent(--indent, fp);
    fprintf(fp, "</Function>\n");
}


/*
 * Return TRUE if there is a C/C++ signature.
 */
static int hasCppSignature(signatureDef *sd)
{
    int a;

    if (sd == NULL)
        return FALSE;

    /*
     * See if there are any arguments that could only have come from
     * handwritten code.
     */
    for (a = 0; a < sd->nrArgs; ++a)
    {
        switch (sd->args[a].atype)
        {
        case pyobject_type:
        case pytuple_type:
        case pylist_type:
        case pydict_type:
        case pycallable_type:
        case pyslice_type:
        case pytype_type:
        case pybuffer_type:
        case pyenum_type:
        case capsule_type:
            return FALSE;

        default:
            break;
        }
    }

    return TRUE;
}


/*
 * Generate the XML for a C++ signature.
 */
static void xmlCppSignature(FILE *fp, signatureDef *sd, int is_const)
{
    int a;

    prcode(fp, "%M");
    normaliseArgs(sd);

    prcode(fp, "(");

    for (a = 0; a < sd->nrArgs; ++a)
    {
        argDef *ad = &sd->args[a];

        if (a > 0)
            prcode(fp, ",");

        generateBaseType(NULL, ad, TRUE, STRIP_GLOBAL, fp);
    }

    prcode(fp, ")%s", (is_const ? " const" : ""));

    restoreArgs(sd);
    prcode(fp, "%M");
}


/*
 * Generate the XML for an argument.
 */
static void xmlArgument(sipSpec *pt, moduleDef *mod, argDef *ad, int out,
        KwArgs kwargs, int res_xfer, int indent, FILE *fp)
{
    if (isArraySize(ad))
        return;

    xmlIndent(indent, fp);
    fprintf(fp, "<%s", (out ? "Return" : "Argument"));
    s += _xml_type(spec, module, ad, out, kwargs)

    if (!out)
    {
        if (isAllowNone(ad))
            fprintf(fp, " allownone=\"1\"");

        if (isDisallowNone(ad))
            fprintf(fp, " disallownone=\"1\"");

        if (isTransferred(ad))
            fprintf(fp, " transfer=\"to\"");
        else if (isThisTransferred(ad))
            fprintf(fp, " transfer=\"this\"");
    }

    if (res_xfer || isTransferredBack(ad))
        fprintf(fp, " transfer=\"back\"");

    fprintf(fp, "/>\n");
}


def _xml_type(spec, module, arg, out=False, kw_args=KwArgs.NONE):
    """ Return the XML for a type. """

    formatter = ArgumentFormatter(arg)

    s = ' typename="'

    # Handle the argument name.
    if not out and arg.name is not None:
        if kw_args is KwArgs.ALL or (kw_args is KwArgs.OPTIONAL and arg.default_value is not None):
            s += arg.name.name + ': '

    # Use any explicit type hint unless the argument is constrained.
    if arg.type_hints is None:
        hint = None
    elif out:
        hint = arg.type_hints.hint_out
    elif arg.is_constrained:
        hint = None
    else:
        hint = arg.type_hints.hint_in

    if hint is None:
        if arg.type is ArgumentType.CLASS:
            s += ClassFormatter(arg.definition).rest_ref
        elif arg.type is ArgumentType.ENUM:
            if arg.definition.py_name is not None:
                s += EnumFormatter(arg.definition).rest_ref
            else:
                s += 'int'
        elif arg.type is ArgumentType.MAPPED:
            # There would normally be a type hint.
            s += "unknown-type"
        else:
            s += formatter.py_type()
    else:
        s += TypeHintManager(spec).rest_ref(hint, out)

    if not out and arg.name is not None and arg.default_value is not None:
        s += ' = '

        # Try and convert the value to a reST reference.  We don't try very
        # hard but will get most cases.
        rest_ref = ValueListFormatter(arg.default_value).rest_ref(spec)
        if rest_ref is None:
            rest_ref = formatter.py_default_value()

        s += rest_ref

    s += '"'

    return s


/*
 * Return the name of the reflected version of a slot or NULL if it doesn't
 * have one.
 */
static const char *reflectedSlot(slotType st)
{
    switch (st)
    {
    case add_slot:
        return "__radd__";

    case sub_slot:
        return "__rsub__";

    case mul_slot:
        return "__rmul__";

    case matmul_slot:
        return "__rmatmul__";

    case truediv_slot:
        return "__rtruediv__";

    case floordiv_slot:
        return "__rfloordiv__";

    case mod_slot:
        return "__rmod__";

    case lshift_slot:
        return "__rlshift__";

    case rshift_slot:
        return "__rrshift__";

    case and_slot:
        return "__rand__";

    case or_slot:
        return "__ror__";

    case xor_slot:
        return "__rxor__";

    default:
        break;
    }

    return NULL;
}
