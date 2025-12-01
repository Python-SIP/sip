# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


# TODO
cfg_disabled_for = [14]


def test_simple_template_superclasses(module):
    assert issubclass(module.CWrapper, module.CValue)

def test_scoped_template_superclasses(module):
    assert issubclass(module.NestedWrapper, module.Scope.Nested)

def test_template_superclasses(module):
    assert issubclass(module.AValueWrapper, module.BaseClass)
    a = module.AValueWrapper()
    assert isinstance(a.getValue(), module.AValue)

    assert issubclass(module.BValueWrapper, module.BaseClass)
    b = module.BValueWrapper()
    assert isinstance(b.getValue(), module.BValue)
