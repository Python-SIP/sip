# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


# TODO: Needs in-development v14 support.
cfg_disabled_for = [14]


@pytest.fixture
def klass(module):
    """ This is a fixture that returns an instance of Klass. """

    return module.Klass()


def test_class_callables(module):
    assert module.Klass.get_s_attr_int() == 0
    module.Klass.set_s_attr_int(10)
    assert module.Klass.get_s_attr_int() == 10

def test_instance_callables(klass):
    assert klass.get_attr_int() == 0
    klass.set_attr_int(10)
    assert klass.get_attr_int() == 10

def test_slot_call(klass):
    klass.set_attr_int(33)
    assert klass(2) == 66

def test_slot_delitem(klass):
    original_count = klass.count()
    assert klass[2] == 2

    del klass[2]

    assert klass.count() == original_count - 1
    assert klass[2] == 3

def test_slot_eq(klass, module):
    other = module.Klass()

    assert klass == other

    klass.set_attr_int(10)
    assert not (klass == other)

    assert not (klass == 100)

def test_slot_getitem(klass):
    assert klass[2] == 2

    with pytest.raises(IndexError):
        klass[-1]

    with pytest.raises(IndexError):
        klass[klass.count()]

def test_slot_len(klass):
    assert klass.count() == len(klass)

def test_slot_neg(klass):
    klass.set_attr_int(10)
    assert -klass == -10

def test_slot_setitem(klass):
    assert klass[2] == 2
    klass[2] = 20
    assert klass[2] == 20

    with pytest.raises(IndexError):
        klass[-1] = 0

    with pytest.raises(IndexError):
        klass[klass.count()] = 0
