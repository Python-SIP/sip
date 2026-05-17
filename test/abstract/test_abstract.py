# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_abstract_instantiation(module):
    with pytest.raises(TypeError):
        module.AbstractBaseClass()

def test_abstract_py_instantiation(module, virtual_hook):
    class Klass(module.AbstractBaseClass):
        pass

    Klass().get_value()
    with pytest.raises(NotImplementedError):
        virtual_hook.reraise()

def test_concrete_instantiation(module):
    assert module.ConcreteClass().get_value() == 10

def test_concrete_py_instantiation(module):
    class Klass(module.AbstractBaseClass):
        def get_value(self):
            return 20

    assert Klass().get_value() == 20
