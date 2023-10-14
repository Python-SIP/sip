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


from ...scoped_name import STRIP_NONE
from ...specification import IfaceFileType

from .misc import fmt_scoped_py_name, iface_is_defined


def fmt_enum_member_as_rest_ref(enum_member):
    """ Return the fully qualified Python name of a member as a reST reference.
    """

    enum = enum_member.scope
    module_name = enum.module.fq_py_name.name

    if enum.py_name is None:
        member_name = fmt_scoped_py_name(enum.scope, enum_member.py_name.name)

        return f':sip:ref:`~{module_name}.{member_name}`'

    enum_name = fmt_scoped_py_name(enum.scope, enum.py_name.name)
    member_name = enum_member.py_name.name

    return f':sip:ref:`~{module_name}.{enum_name}.{member_name}`'


def fmt_enum_as_cpp_type(enum, make_public=False, strip=STRIP_NONE):
    """ Return an enum's fully qualified C++ type name. """

    if enum.fq_cpp_name is None or (enum.is_protected and not make_public):
        return 'int'

    return enum.fq_cpp_name.cpp_stripped(strip)


def fmt_enum_as_rest_ref(enum):
    """ Return a fully qualified Python name as a reST reference. """

    module_name = enum.module.fq_py_name.name
    enum_name = fmt_scoped_py_name(enum.scope, enum.py_name.name)

    return f':sip:ref:`~{module_name}.{enum_name}`'


def fmt_enum_as_type_hint(spec, enum, defined):
    """ Return the type hint. """

    module = spec.module

    if enum.scope is None:
        # Global enums are defined early on.
        is_defined = True
    else:
        scope_iface = enum.scope.iface_file
        outer_scope = enum.scope.scope if scope_iface.type is IfaceFileType.CLASS else None

        is_defined = iface_is_defined(scope_iface, outer_scope, module,
                defined)

    quote = '' if is_defined else "'"

    # Include the module name if it is not the current one.
    module_name = enum.module.py_name + '.' if enum.module is not module and defined is not None else ''
    py_name = fmt_scoped_py_name(enum.scope, enum.py_name.name)

    return f'{quote}{module_name}{py_name}{quote}'
