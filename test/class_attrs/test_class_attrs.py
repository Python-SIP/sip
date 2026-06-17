# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


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

def test_instance_attribute(module):
    klass = module.Klass()
    value = module.Value()
    value.my_attr = 10
    klass.class_attr = value

    assert klass.get_class_attr().my_attr == 10

def test_value_attribute(module):
    # This tests (probably unexpected) behaviour as explained in the following
    # comments.
    klass = module.Klass()

    # The 'value' object is cached and will always be returned on future gets
    # of the attribute.  It wraps the address of the underlying value so it
    # will reflect changes made internally by the library being wrapped.  It
    # also means that new Python-specific attributes added to 'value' will be
    # retained.
    value = klass.value_attr

    assert value.get_value() == 0

    # Set a new Python-specific attribute.
    value.py_attr = 10

    # Remove our explicit reference to the value object.
    del value

    # Create a new value object with a different Python-specific attribute than
    # before.
    new_value = module.Value(50)
    new_value.py_attr = 500

    # Set the new value.
    klass.value_attr = new_value

    # Get the value.
    value = klass.value_attr

    # This shows that the new value object isn't simply being returned.
    assert value is not new_value

    # This shows that the new value was copied to the wrapped attribute.
    assert value.get_value() == 50

    # This shows that the value object is the original one with the original
    # Python-specific attribute rather than the newer attribute as might be
    # expected.  There are a number of potential solutions to this:
    # 1. Replace the original cached object with the new one when it is set.
    #    The problem is that it doesn't wrap the same C/C++ instance.
    # 2. Stop caching the value object meaning that Python-specific attributes
    #    will be lost.
    # 3. Update the cached object so that it has the same Python-specific
    #    attributes as the new one.
    # For now we stick with the existing behaviour as any change would not be
    # backwards compatible (and may be the best compromise anyway).
    assert value.py_attr == 10

def test_class_attribute_with_code(module):
    assert module.Klass.s_attr_with_code == 0
    module.Klass.s_attr_with_code = 5
    assert module.Klass.s_attr_with_code == 5

def test_class_attribute_invalid(module):
    klass = module.Klass()
    value = module.Klass()

    with pytest.raises(TypeError):
        klass.class_attr = value

def test_subclass_attribute(module):
    class MyValue(module.Value): pass

    klass = module.Klass()
    value = MyValue()
    klass.class_attr = value

    assert klass.get_class_attr() is value

def test_dir(klass):
    assert 'get_attr' in dir(klass)

def test_dir_dynamic(klass):
    assert 'attr' in dir(klass)
