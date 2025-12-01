# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_instantiation(module):
    with pytest.raises(TypeError):
        module.NS()

def test_py_attributes(module):
    with pytest.raises(AttributeError):
        module.NS.foo

    module.NS.foo = 'bar'
    assert module.NS.foo == 'bar'

    del module.NS.foo
    with pytest.raises(AttributeError):
        module.NS.foo

def test_wrapped_attributes(module, abi_version):
    assert module.NS.attr == 0
    module.NS.attr = 10
    assert module.NS.attr == 10

    # For ABI v14 check the C++ value has changed and not the type dict.
    if abi_version >= 14:
        assert module.NS.get_attr() == 10

    # For ABI v14 check a wrapped namespace attribute cannot be deleted.
    if abi_version >= 14:
        with pytest.raises(AttributeError):
            del module.NS.attr
