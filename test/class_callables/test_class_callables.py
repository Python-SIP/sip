# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


@pytest.fixture
def klass(module):
    """ This is a fixture that returns an instance of Klass. """

    return module.Klass()


@pytest.fixture
def reverse(module):
    """ This is a fixture that returns an instance of Reverse. """

    return module.Reverse()


def test_class_callables(module):
    assert module.Klass.get_s_attr_int() == 0
    module.Klass.set_s_attr_int(10)
    assert module.Klass.get_s_attr_int() == 10

def test_ctor_error(module):
    with pytest.raises(ValueError):
        module.Klass(10)

def test_method_error(klass):
    with pytest.raises(ValueError):
        klass.error_method()

def test_instance_callables(klass):
    assert klass.get_attr_int() == 0
    klass.set_attr_int(10)
    assert klass.get_attr_int() == 10

def test_slot_neg(klass):
    klass.set_attr_int(10)
    assert -klass == -10

def test_slot_sub(klass):
    klass.set_attr_int(10)
    assert klass - 5 == 5

def test_slot_add(klass):
    klass.set_attr_int(10)
    assert klass + 5 == 15

def test_slot_mul(klass):
    klass.set_attr_int(10)
    assert klass * 5 == 50

def test_slot_truediv(klass):
    klass.set_attr_int(10)
    assert klass / 3 == 3

def test_slot_mod(klass):
    klass.set_attr_int(10)
    assert klass % 3 == 1

def test_slot_and(klass):
    klass.set_attr_int(10)
    assert klass & 2 == 2

def test_slot_or(klass):
    klass.set_attr_int(10)
    assert klass | 5 == 15

def test_slot_xor(klass):
    klass.set_attr_int(10)
    assert klass ^ 1 == 11

def test_slot_invert(klass):
    klass.set_attr_int(10)
    assert ~klass == -11

def test_slot_lshift(klass):
    klass.set_attr_int(10)
    assert klass << 2 == 40

def test_slot_rshift(klass):
    klass.set_attr_int(10)
    assert klass >> 2 == 2

def test_slot_isub(klass):
    klass.set_attr_int(10)
    klass -= 5
    assert klass.get_attr_int() == 5

def test_slot_iadd(klass):
    klass.set_attr_int(10)
    klass += 5
    assert klass.get_attr_int() == 15

def test_slot_imul(klass):
    klass.set_attr_int(10)
    klass *= 5
    assert klass.get_attr_int() == 50

def test_slot_itruediv(klass):
    klass.set_attr_int(10)
    klass /= 3
    assert klass.get_attr_int() == 3

def test_slot_imod(klass):
    klass.set_attr_int(10)
    klass %= 3
    assert klass.get_attr_int() == 1

def test_slot_iand(klass):
    klass.set_attr_int(10)
    klass &= 2
    assert klass.get_attr_int() == 2

def test_slot_ior(klass):
    klass.set_attr_int(10)
    klass |= 5
    assert klass.get_attr_int() == 15

def test_slot_ixor(klass):
    klass.set_attr_int(10)
    klass ^= 1
    assert klass.get_attr_int() == 11

def test_slot_ilshift(klass):
    klass.set_attr_int(10)
    klass <<= 2
    assert klass.get_attr_int() == 40

def test_slot_irshift(klass):
    klass.set_attr_int(10)
    klass >>= 2
    assert klass.get_attr_int() == 2

def test_slot_eq(klass, module):
    klass.set_attr_int(10)
    assert klass == 10

def test_slot_ne(klass, module):
    klass.set_attr_int(10)
    assert klass != 1

def test_slot_lt(klass, module):
    klass.set_attr_int(10)
    assert klass < 100

def test_slot_le(klass, module):
    klass.set_attr_int(10)
    assert klass <= 10

def test_slot_gt(klass, module):
    klass.set_attr_int(10)
    assert klass > 1

def test_slot_ge(klass, module):
    klass.set_attr_int(10)
    assert klass >= 10

def test_slot_call(klass):
    klass.set_attr_int(33)
    assert klass(2) == 66

def test_slot_delitem(klass):
    original_count = klass.count()
    assert klass[2] == 2

    del klass[2]

    assert klass.count() == original_count - 1
    assert klass[2] == 3

def test_slot_getitem(klass):
    assert klass[2] == 2

    with pytest.raises(IndexError):
        klass[-1]

    with pytest.raises(IndexError):
        klass[klass.count()]

def test_slot_len(klass):
    assert klass.count() == len(klass)

def test_slot_setitem(klass):
    assert klass[2] == 2
    klass[2] = 20
    assert klass[2] == 20

    with pytest.raises(IndexError):
        klass[-1] = 0

    with pytest.raises(IndexError):
        klass[klass.count()] = 0

def test_slot_rsub(reverse):
    reverse.set_attr_int(10)
    assert 5 - reverse == -5

def test_slot_radd(reverse):
    reverse.set_attr_int(10)
    assert 5 + reverse == 15

def test_slot_rmul(reverse):
    reverse.set_attr_int(10)
    assert 5 * reverse == 50

def test_slot_rtruediv(reverse):
    reverse.set_attr_int(10)
    assert 16 / reverse == 1

def test_slot_rmod(reverse):
    reverse.set_attr_int(10)
    assert 16 % reverse == 6

def test_slot_rand(reverse):
    reverse.set_attr_int(10)
    assert 2 & reverse == 2

def test_slot_ror(reverse):
    reverse.set_attr_int(10)
    assert 5 | reverse == 15

def test_slot_rxor(reverse):
    reverse.set_attr_int(10)
    assert 1 ^ reverse == 11

def test_slot_rlshift(reverse):
    reverse.set_attr_int(2)
    assert 10 << reverse == 40

def test_slot_rrshift(reverse):
    reverse.set_attr_int(2)
    assert 10 >> reverse == 2
