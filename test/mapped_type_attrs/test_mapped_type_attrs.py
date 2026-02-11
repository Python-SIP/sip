# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from sys import getrefcount

import pytest


def test_enum_attrs(module, abi_version):
    """ Test a mapped typed containing an enum. """

    member = module.EnumAttrs.MTEnum.MTEnumMember
    value = member.value if abi_version >= 13 else member
    assert value == 10

def test_callables(module):
    """ Test a mapped typed containing a callable. """

    assert module.Callables.callable() == 42
