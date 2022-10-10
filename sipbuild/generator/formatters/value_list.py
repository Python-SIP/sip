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


from ..specification import ValueType

from .argument import ArgumentFormatter
from .base_formatter import BaseFormatter


class ValueListFormatter(BaseFormatter):
    """ This creates various string representations of a list of values. """

    @property
    def cpp_expression(self):
        """ The C++ representation of the value list as an expression. """

        return self._expression()

    @property
    def embedded_py_expression(self):
        """ The Python representation of the value list as an expression
        embedded in a string.
        """

        return self._expression(as_python=True, embedded=True)

    @property
    def py_expression(self):
        """ The Python representation of the value list as an expression. """

        return self._expression(as_python=True)

    def _expression(self, as_python=False, embedded=False):
        """ The representation of the value list as an expression. """

        s = ''

        for value in self.object:
            if value.cast is not None and not as_python:
                s += '(' + value.cast.as_cpp + ')'

            if value.unary_operator is not None:
                s += value.unary_operator

            if value.value_type is ValueType.QCHAR:
                if value.value == '"' and embedded:
                    s += "'\\\"'"
                else:
                    s += "'" + value.value + "'"

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

            elif value.value_type in (ValueType.NUMERIC, ValueType.REAL):
                s += str(value.value)

            elif value.value_type is ValueType.SCOPED:
                if as_python:
                    s += value.value.as_py
                else:
                    s += value.value.as_cpp

            elif value.value_type is ValueType.FCALL:
                arg_formatter = ArgumentFormatter(self.spec,
                        value.value.result)

                if as_python:
                    s += arg_formatter.py_type()
                else:
                    s += arg_formatter.cpp_type()

                args = [ValueListFormatter(self.spec, a)._expression
                        for a in value.value.args]

                s += '(' + ', '.join(args) + ')'

            elif value.value_type is ValueType.EMPTY:
                s += '{}'

            if value.binary_operator is not None:
                s += value.binary_operator

        return s
