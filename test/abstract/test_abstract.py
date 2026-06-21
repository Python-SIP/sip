# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


# This is the first test in the entire suite.
def test_free_threading(module, abi_version):
    import sys

    if hasattr(sys.flags, 'gil') and not sys.flags.gil:
        # The interpreter supports free threading.
        if abi_version >= 14:
            # Check that the GIL hasn't been enabled.
            assert not sys._is_gil_enabled()
        else:
            # Check that the GIL has been enabled.
            assert sys._is_gil_enabled()

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
