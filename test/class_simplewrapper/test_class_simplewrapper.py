# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


# The module specifies the fully qualified name of sipwrapper used by v13 and
# later.
cfg_disabled_for = [12]


def test_simple_plain_class_mro(module):
    assert len(module.SimpleKlass.__mro__) == 3

def test_simple_plain_class_isinstance(module):
    assert isinstance(module.SimpleKlass(), module.SimpleKlass)

def test_simple_py_subclass(module):
    class SimpleSubK(module.SimpleKlass):
        pass

    assert isinstance(SimpleSubK(), module.SimpleKlass)
