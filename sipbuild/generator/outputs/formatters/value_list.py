# Copyright (c) 2023, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ('AS IS'
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


from ...specification import ValueType

from .argument import fmt_argument_as_cpp_type, fmt_argument_as_py_type
from .enum import fmt_enum_member_as_rest_ref


def fmt_value_list_as_cpp_expression(spec, value_list):
    """ Return the C++ representation of a value list as an expression. """

    return _expression(spec, value_list)


def fmt_value_list_as_py_expression(spec, value_list, embedded=False,
        as_xml=False):
    """ The Python representation of a value list as an expression. """

    return _expression(spec, value_list, as_python=True, embedded=embedded,
            as_xml=as_xml)


def fmt_value_list_as_rest_ref(spec, value_list, as_xml=False):
    """ Return the Python representation of a value list as a reST reference.
    """

    # The value must be a scoped name and we don't handle expressions.
    if len(value_list) != 1 or value_list[0].value_type is not ValueType.SCOPED:
        return None

    target = value_list[0].value

    # See if it is an attribute.
    for variable in spec.variables:
        if variable.fq_cpp_name == target:
            return fmt_variable_as_rest_ref(variable)

    # See if it is an enum member.
    target_scope = target.scope
    if target_scope is not None:
        target_scope.make_absolute()

    target_base_name = target.base_name

    for enum in spec.enums:
        # Look for the member name first before working out if it is the
        # correct enum.
        for member in enum.members:
            if member.cpp_name == target_base_name:
                if enum.is_scoped:
                    # It's a scoped enum so the fully qualified name of the
                    # enum must match the scope of the name.
                    if target_scope is not None and enum.fq_cpp_name == target_scope:
                        return fmt_enum_member_as_rest_ref(member)
                else:
                    # It's a traditional enum so the scope of the enum must
                    # match the scope of the name.
                    if (enum.scope is None and target_scope is None) or (enum.scope is not None and target_scope is not None and enum.scope.iface_file.fq_cpp_name == target_scope):
                        return fmt_enum_member_as_rest_ref(member)

                break

    return None


def _expression(spec, value_list, as_python=False, embedded=False,
        as_xml=False):
    """ The representation of a value list as an expression. """

    s = ''

    for value in value_list:
        if value.cast is not None and not as_python:
            s += '(' + value.cast.as_cpp + ')'

        if value.unary_operator is not None:
            s += value.unary_operator

        if value.value_type is ValueType.QCHAR:
            if value.value == '"' and embedded:
                s += "'\\\"'"
            elif value.value.isprintable():
                s += "'" + value.value + "'"
            else:
                s += f"'\\{ord(value.value):03o}'"

        elif value.value_type is ValueType.STRING:
            quote = "\\\"" if embedded else "\""

            s += quote

            for ch in value.value:
                escape = True

                if ch in "\\\"":
                    pass
                elif ch == '\n':
                    ch = 'n';
                elif ch == '\r':
                    ch = 'r'
                elif ch == '\t':
                    ch = 't'
                else:
                    escape = False

                if escape:
                    s += "\\"

                s += ch

            s += quote

        elif value.value_type is ValueType.NUMERIC:
            s += str(value.value) if as_python else str(int(value.value))

        elif value.value_type is ValueType.REAL:
            s += format(value.value, 'g')

        elif value.value_type is ValueType.SCOPED:
            s += value.value.as_py if as_python else value.value.as_cpp

        elif value.value_type is ValueType.FCALL:
            if as_python:
                s += fmt_argument_as_py_type(spec, value.value.result)
            else:
                s += fmt_argument_as_cpp_type(spec, value.value.result)

            args = [_expression(spec, arg, as_python=as_python,
                            embedded=embedded, as_xml=as_xml)
                    for arg in value.value.args]

            separator = ',' if as_xml else ', '
            s += '(' + separator.join(args) + ')'

        elif value.value_type is ValueType.EMPTY:
            s += '{}'

        if value.binary_operator is not None:
            s += value.binary_operator

    return s
