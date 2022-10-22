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


from ..formatters import (ArgumentFormatter, ClassFormatter, EnumFormatter,
        format_scoped_py_name, SignatureFormatter, ValueListFormatter,
        VariableFormatter)
from ..python_slots import is_number_slot
from ..scoped_name import ScopedName, STRIP_GLOBAL
from ..specification import (AccessSpecifier, ArgumentType, ArrayArgument,
        IfaceFileType, KwArgs, PyQtMethodSpecifier, PySlot, Transfer)
from ..type_hints import TypeHintManager


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


def _xml_realname(scope, member=None):
    """ Return a 'realname' attribute containing a fully qualified C/C++ name.
    """

    if scope is None:
        # The member should have a name in this context.
        fq_cpp_name = member
    else:
        if isinstance(scope, ScopedName):
            fq_cpp_name = scope
        else:
            fq_cpp_name = scope.iface_file.fq_cpp_name

        fq_cpp_name = fq_cpp_name.cpp_stripped(STRIP_GLOBAL)

        if member is not None:
            fq_cpp_name += '::' + member

    return f' realname="{fq_cpp_name}"'


def _xml_class(xf, spec, module, klass):
    """ Output the XML for a class. """

    indent = 1

    if klass.is_opaque:
        s = ' ' * indent

        s += '<OpaqueClass name="'
        s += ClassFormatter(spec, klass).fq_py_name
        s += '"/>\n'

        xf.write(s)

        return

    if not klass.is_hidden_namespace:
        s = ' ' * indent

        s += '<Class name="'
        s += ClassFormatter(spec, klass).fq_py_name
        s += '"'

        s += _xml_realname(klass.iface_file.fq_cpp_name)

        if klass.pickle_code is not None:
            s += ' pickle="1"'

        if klass.convert_to_type_code is not None:
            s += ' convert="1"'

        if klass.convert_from_type_code is not None:
            s += ' convertfrom="1"'

        if klass.real_class is not None:
            s += f' extends="{klass.real_class.iface_file.module.py_name}"'

        if klass.pyqt_flags_enums is not None:
            s += ' flagsenums="' + ' '.join(klass.pyqt_flags_enums) + '"'

        if len(klass.superclasses) != 0:
            super_refs = ' '.join(
                    [ClassFormatter(spec, s).rest_ref
                            for s in klass.superclasses])

            s += f' inherits="{super_refs}"'

        s += '>\n'

        xf.write(s)

    for ctor in klass.ctors:
        if ctor.access_specifier is not AccessSpecifier.PRIVATE:
            _xml_ctor(xf, spec, module, klass, ctor, indent + 1)

    _xml_enums(xf, spec, module, klass, indent + 1)
    _xml_variables(xf, spec, module, klass, indent + 1)

    for member in klass.members:
        _xml_function(xf, spec, module,  member, klass.overloads, klass,
                indent + 1)

    if not klass.is_hidden_namespace:
        s = ' ' * indent
        s += '</Class>\n'
        xf.write(s)


def _xml_enums(xf, spec, module, scope=None, indent=1):
    """ Output the XML for all the enums in a scope. """

    for enum in spec.enums:
        if enum.module is module and enum.scope is scope:
            if enum.py_name is None:
                for member in enum.members:
                    s = ' ' * indent

                    s += '<Member name="'
                    s += format_scoped_py_name(enum.scope, member.py_name.name)
                    s += '"'

                    s += _xml_realname(enum.scope, member.cpp_name)

                    s += ' const="1" typename="int"/>\n'

                    xf.write(s)
            else:
                s = ' ' * indent

                s += '<Enum name="'
                s += format_scoped_py_name(enum.scope, enum.py_name.name)
                s += '"'

                s += _xml_realname(enum.fq_cpp_name)

                s += '>\n'

                xf.write(s)

                for member in enum.members:
                    s = ' ' * (indent + 1)

                    s += '<EnumMember name="'
                    s += format_scoped_py_name(enum.scope, enum.py_name.name)
                    s += '.' + member.py_name.name + '"'

                    s += _xml_realname(enum.fq_cpp_name, member.cpp_name)

                    s += '/>\n'

                    xf.write(s)

                s = ' ' * indent

                s += '</Enum>\n'

                xf.write(s)


def _xml_variables(xf, spec, module, scope=None, indent=1):
    """ Output the XML for all the variables in a scope. """

    for variable in spec.variables:
        if variable.module is module and variable.scope is scope:
            formatter = VariableFormatter(spec, variable)

            s = ' ' * indent

            s += f'<Member name="{formatter.fq_py_name}"'

            s += _xml_realname(variable.fq_cpp_name)

            if variable.type.is_const or scope is None:
                s += ' const="1"'

            if variable.is_static:
                s += ' static="1"'

            s += _xml_type(spec, module, variable.type) + '/>\n'

            xf.write(s)


def _xml_ctor(xf, spec, module, scope, ctor, indent):
    """ Output the XML for a ctor. """

    s = ' ' * indent

    s += '<Function name="'
    s += format_scoped_py_name(scope, '__init__')
    s += '"'

    s += _xml_realname(scope, '__init__')

    if _has_cpp_signature(ctor.cpp_signature):
        s += ' cppsig="'
        s += _xml_cpp_signature(spec, ctor.cpp_signature)
        s += '"'

    # Handle the trivial case.
    if len(ctor.py_signature.args) == 0:
        s += '/>\n'
        xf.write(s)
    else:
        s += '>\n'
        xf.write(s)

        for arg in ctor.py_signature.args:
            if arg.is_in:
                _xml_argument(xf, spec, module, arg, ctor.kw_args, indent + 1)

            if arg.is_out:
                _xml_argument(xf, spec, moduke, arg, ctor.kw_args, indent + 1,
                        out=True)

        s = ' ' * indent
        s += '</Function>\n'
        xf.write(s)


def _xml_function(xf, spec, module, member, overloads, scope=None, indent=1):
    """ Output the XML for a function. """

    for overload in overloads:
        if overload.common is not member:
            continue

        if overload.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if overload.pyqt_method_specifier is PyQtMethodSpecifier.SIGNAL:
            s = ' ' * indent

            s += '<Signal name="'
            s += format_scoped_py_name(scope, member.py_name.name)
            s += '"'

            s += _xml_realname(scope, overload.cpp_name)

            if _has_cpp_signature(overload.cpp_signature):
                s += ' cppsig="'
                s += _xml_cpp_signature(spec, overload.cpp_signature)
                s += '"'

            # Handle the trivial case.
            if len(overload.py_signature.args) == 0:
                s += '/>\n'
                xf.write(s)
            else:
                s += '>\n'
                xf.write(s)

                for arg in overload.py_signature.args:
                    _xml_argument(xf, spec, module, arg, overload.kw_args,
                            indent + 1)

                s = ' ' * indent
                s += '</Signal>\n'
                xf.write(s)
        else:
            extends = None
            is_static = (scope is None or scope.iface_file.type is IfaceFileType.NAMESPACE or overload.is_static)

            if scope is None and member.py_slot is not None and overload.py_signature.args[0].type is ArgumentType.CLASS:
                extends = overload.py_signature.args[0].definition
                is_static = False

            _xml_overload(xf, spec, module, scope, overload, extends,
                    is_static, indent)


def _xml_overload(xf, spec, module, scope, overload, extends, is_static,
        indent):
    """ Output the XML for an overload. """

    s = ' ' * indent

    s += '<Function name="'

    if overload.is_reflected:
        name = _reflected_slot(overload.common.py_slot)
    else:
        name = None

    if name is None:
        name = overload.common.py_name.name
        cpp_name = overload.cpp_name
    else:
        cpp_name = name
        
    s += format_scoped_py_name(scope, name)

    s += '"'

    s += _xml_realname(scope, cpp_name)

    if _has_cpp_signature(overload.cpp_signature):
        s += ' cppsig="'
        s += _xml_cpp_signature(spec, overload.cpp_signature,
                is_const=overload.is_const)
        s += '"'

    if overload.is_abstract:
        s += ' abstract="1"'

    if is_static:
        s += ' static="1"'

    if overload.pyqt_method_specifier is PyQtMethodSpecifier.SLOT:
        s += ' slot="1"'

    if overload.is_virtual:
        s += ' virtual="1"'

    if extends is not None:
        s += ' extends="'
        s += ClassFormatter(spec, extends).fq_py_name
        s += '"'

    # An empty type hint specifies a void return.
    result = overload.py_signature.result

    if result.type_hints is not None and result.type_hints.hint_out is not None and result.type_hints.hint_out.text == '':
        no_result = True
    else:
        no_result = (result.type is ArgumentType.VOID and len(result.derefs) == 0)

    # Handle the trivial case.
    if no_result and len(overload.py_signature.args) == 0:
        s += '/>\n'
        xf.write(s)
        return

    s += '>\n'
    xf.write(s)

    if not no_result:
        _xml_argument(xf, spec, module, overload.py_signature.result,
                KwArgs.NONE, indent + 1, out=True,
                transfer_result=overload.transfer is Transfer.TRANSFER_BACK)

    # Ignore the first argument of non-reflected number slots and the second
    # argument of reflected number slots.
    might_ignore = (is_number_slot(overload.common.py_slot) and len(overload.py_signature.args) == 2)

    for a, arg in enumerate(overload.py_signature.args):
        if might_ignore:
            if a == 0 and not overload.is_reflected:
                continue

            if a == 1 and overload.is_reflected:
                continue

        if arg.is_in:
            _xml_argument(xf, spec, module, arg, overload.kw_args, indent + 1)

        if arg.is_out:
            _xml_argument(xf, spec, module, arg, overload.kw_args, indent + 1,
                    out=True)

    s = ' ' * indent

    s += '</Function>\n'

    xf.write(s)


# Argument types that imply handwritten code.
_HANDWRITTEN_CODE_TYPES = (
    ArgumentType.PYOBJECT,
    ArgumentType.PYTUPLE,
    ArgumentType.PYLIST,
    ArgumentType.PYDICT,
    ArgumentType.PYCALLABLE,
    ArgumentType.PYSLICE,
    ArgumentType.PYTYPE,
    ArgumentType.PYBUFFER,
    ArgumentType.PYENUM,
    ArgumentType.CAPSULE,
)

def _has_cpp_signature(signature):
    """ Return True if there is a C/C++ signature. """

    if signature is None:
        return False

    # See if there are any arguments that could only have come from handwritten
    # code.
    for arg in signature.args:
        if arg in _HANDWRITTEN_CODE_TYPES:
            return False

    return True


def _xml_cpp_signature(spec, signature, is_const=False):
    """ Return the XML for a C++ signature. """

    formatter = SignatureFormatter(spec, signature)

    args = formatter.cpp_arguments(strip=STRIP_GLOBAL, make_public=True,
        as_xml=True)
    const = ' const' if is_const else ''

    return f'({args}){const}'


def _xml_argument(xf, spec, module, arg, kw_args, indent, out=False,
        transfer_result=False):
    """ Ouput the XML for an argument. """

    if arg.array is ArrayArgument.ARRAY_SIZE:
        return

    s = ' ' * indent

    s += '<' + ('Return' if out else 'Argument')
    s += _xml_type(spec, module, arg, kw_args=kw_args, out=out)

    if not out:
        if arg.allow_none:
            s += ' allownone="1"'

        if arg.disallow_none:
            s += ' disallownone="1"'

        if arg.transfer is Transfer.TRANSFER:
            s += ' transfer="to"'
        elif arg.transfer is Transfer.TRANSFER_THIS:
            s += ' transfer="this"'

    if transfer_result or arg.transfer is Transfer.TRANSFER_BACK:
        s += ' transfer="back"'

    s += '/>\n'

    xf.write(s)


def _xml_type(spec, module, arg, kw_args=KwArgs.NONE, out=False):
    """ Return the XML for a type. """

    formatter = ArgumentFormatter(spec, arg)

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
            s += ClassFormatter(spec, arg.definition).rest_ref
        elif arg.type is ArgumentType.ENUM:
            if arg.definition.py_name is not None:
                s += EnumFormatter(spec, arg.definition).rest_ref
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
        rest_ref = ValueListFormatter(spec, arg.default_value).rest_ref
        if rest_ref is None:
            rest_ref = formatter.py_default_value()

        s += rest_ref

    s += '"'

    return s


# A map of slots and the names of their reflections.
_SLOT_REFLECTIONS = {
    PySlot.ADD: '__radd__',
    PySlot.SUB: '__rsub__',
    PySlot.MUL: '__rmul__',
    PySlot.MATMUL: '__rmatmul__',
    PySlot.TRUEDIV: '__rtruediv__',
    PySlot.FLOORDIV: '__rfloordiv__',
    PySlot.MOD: '__rmod__',
    PySlot.LSHIFT: '__rlshift__',
    PySlot.RSHIFT: '__rrshift__',
    PySlot.AND: '__rand__',
    PySlot.OR: '__ror__',
    PySlot.XOR: '__rxor__',
}

def _reflected_slot(py_slot):
    """ Return the name of the reflected version of a slot or None if it
    doesn't have one.
    """

    return _SLOT_REFLECTIONS.get(py_slot)