# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_simplewrapper_instantiation(module):
    with pytest.raises(TypeError):
        module.simplewrapper()

def test_wrapper_instantiation(module):
    with pytest.raises(TypeError):
        module.wrapper()

def test_plain_class_mro(module):
    assert len(module.Klass.__mro__) == 4

def test_plain_class_isinstance(module):
    assert isinstance(module.Klass(), module.Klass)

def test_nested_class_isinstance(module):
    assert isinstance(module.Klass.Nested(), module.Klass.Nested)

def test_py_subclass(module):
    class SubK(module.Klass):
        pass

    assert isinstance(SubK(), module.Klass)
