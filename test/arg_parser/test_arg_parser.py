# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_ctor_with_transfer_arg(module):
    owned = module.OwnedClass()
    assert module.ispyowned(owned)

    klass = module.Klass(owned)
    assert not module.ispyowned(owned)

def test_static_callable_from_type(module):
    module.Klass.set_static(10)
    assert module.Klass.get_static() == 10

def test_static_callable_from_instance(module):
    klass = module.Klass()

    klass.set_static(20)
    assert klass.get_static() == 20

def test_bound_callable(module):
    klass = module.Klass()

    klass.set_inst(10)
    assert klass.get_inst() == 10

def test_unbound_callable(module):
    klass = module.Klass()

    module.Klass.set_inst(klass, 20)
    assert module.Klass.get_inst(klass) == 20

def test_protected_bound_callable(module):
    klass = module.Klass()

    klass.set_prot_inst(10)
    assert klass.get_prot_inst() == 10

def test_protected_unbound_callable(module):
    klass = module.Klass()

    module.Klass.set_prot_inst(klass, 20)
    assert module.Klass.get_prot_inst(klass) == 20
