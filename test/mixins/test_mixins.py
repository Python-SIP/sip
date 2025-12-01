# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


@pytest.fixture
def TestClass(module):
    """ This fixture returns a Python class that sub-classes from a C++ class
    and a mixin class.
    """

    class MixedIn(module.Klass, module.Mixin):
        pass

    return MixedIn


@pytest.fixture
def instance(TestClass):
    """ This fixture returns an instance of a Python class that sub-classes
    from a C++ class and a mixin class.
    """

    return TestClass(10, mixin_value=20)


def test_argument_handling(TestClass):
    test = TestClass(10, mixin_value=20, extra_kwarg=30)
    assert test.extra_kwarg == 30

def test_unknown_argument_handling(TestClass):
    with pytest.raises(TypeError):
        TestClass(10, unknown_kwarg_1=30, mixin_value=20, unknown_kwarg_2=40)

def test_attr_initial(instance):
    assert instance.get_attr() == 10
    assert instance.attr == 10

def test_setter(instance):
    instance.set_attr(11)
    assert instance.get_attr() == 11
    assert instance.attr == 11

def test_attr_set(instance):
    instance.attr = (12)
    assert instance.get_attr() == 12
    assert instance.attr == 12

def test_mixin_attr_initial(instance):
    assert instance.get_mixin_attr() == 20
    assert instance.mixin_attr == 20

def test_mixin_setter(instance):
    instance.set_mixin_attr(21)
    assert instance.get_mixin_attr() == 21
    assert instance.mixin_attr == 21

def test_mixin_attr_set(instance):
    instance.mixin_attr = (22)
    assert instance.get_mixin_attr() == 22
    assert instance.mixin_attr == 22
