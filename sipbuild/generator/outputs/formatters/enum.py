# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...scoped_name import STRIP_NONE
from ...specification import IfaceFileType

from .misc import fmt_scoped_py_name


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


def fmt_enum_as_type_hint(spec, enum, is_optional=False, pep484=False):
    """ Return the type hint. """

    module = spec.module

    # Include the module name if it is not the current one.
    module_name = enum.module.py_name + '.' if enum.module is not module and pep484 else ''
    py_name = fmt_scoped_py_name(enum.scope, enum.py_name.name)
    optional = ' | None' if is_optional else ''

    return f'{module_name}{py_name}{optional}'
