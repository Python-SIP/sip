# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


# TODO Expand the tests to include attribrutes that are instances of Python
# sub-classes of wrapped types.  Specifically that the objects don't get
# garbage collected prematurely.
@pytest.fixture
def klass(module):
    """ This is a fixture that returns an instance of Klass. """

    return module.Klass()


def test_missing_class_attribute(module):
    with pytest.raises(AttributeError):
        module.Klass.foo

def test_new_class_attribute(module):
    module.Klass.foo = 'bar'
    assert module.Klass.foo == 'bar'

    del module.Klass.foo

    with pytest.raises(AttributeError):
        module.Klass.foo

def test_initial_class_attribute(module):
    assert module.Klass.s_attr == 0

def test_set_class_attribute(module, abi_version):
    module.Klass.s_attr = 10
    assert module.Klass.s_attr == 10

    # For ABI v14 check the C++ value has changed and not the type dict.
    if abi_version >= 14:
        assert module.Klass.get_s_attr() == 10

def test_del_class_attribute(module, abi_version):
    # For ABI v14 check a wrapped class attribute cannot be deleted.
    if abi_version >= 14:
        with pytest.raises(AttributeError):
            del module.Klass.s_attr;

def test_missing_instance_attribute(klass):
    with pytest.raises(AttributeError):
        klass.foo

def test_new_instance_attribute(klass):
    klass.foo = 'bar'
    assert klass.foo == 'bar'

    del klass.foo

    with pytest.raises(AttributeError):
        klass.foo

def test_initial_instance_attribute(klass):
    assert klass.attr == 0

def test_set_instance_attribute(klass):
    klass.attr = 10
    assert klass.attr == 10

    # Check the C++ value has changed and not the type dict.
    assert klass.get_attr() == 10

def test_del_instance_attribute(klass):
    with pytest.raises(AttributeError):
        del klass.attr;

def test_attribute_is_instance_attribute(module):
    with pytest.raises(AttributeError):
        module.Klass.attr
