# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from enum import Enum

import pytest


cfg_enabled_for = [12, 14]
cfg_sip_module_configuration = ['CustomEnums']


@pytest.fixture(scope='module')
def named_enum_class(module):
    """ A fixture that creates a test class that is a sub-class of the
    EnumClass in the test module and reimplements the named_virt() method.
    """

    class NamedEnumClass(module.EnumClass):
        def __init__(self, value):
            super().__init__()

            self._value = value

        def named_virt(self):
            return self._value

    return NamedEnumClass


@pytest.fixture
def members_valid(module, named_enum_class):
    """ A fixture that creates the instance of a test class with valid enum
    members.
    """

    return named_enum_class(module.EnumClass.ClassNamedEnum.ClassNamedMember)


@pytest.fixture
def scoped_members(module):
    """ A fixture that creates the instance of a test class with valid scoped
    enum members.
    """

    class ScopedEnumClass(module.EnumClass):
        def scoped_virt(self):
            return module.EnumClass.ClassScopedEnum.ClassScopedMember

    return ScopedEnumClass()


# The following test anonymous enums.

def test_module_anon_type(module):
    assert isinstance(module.AnonMember, int)

def test_module_anon_value(module):
    assert module.AnonMember == 10

def test_class_anon_type(module):
    assert isinstance(module.EnumClass.ClassAnonMember, int)

def test_class_anon_value(module):
    assert module.EnumClass.ClassAnonMember == 40


# The following test named enums.

def test_module_named_enum_attrs(module):
    assert module.NamedEnum.__module__ == 'custom_enums_module'
    assert module.NamedEnum.__name__ == 'NamedEnum'
    assert module.NamedEnum.__qualname__ == 'NamedEnum'

def test_class_named_enum_attrs(module):
    assert module.EnumClass.ClassNamedEnum.__module__ == 'custom_enums_module'
    assert module.EnumClass.ClassNamedEnum.__name__ == 'ClassNamedEnum'
    assert module.EnumClass.ClassNamedEnum.__qualname__ == 'EnumClass.ClassNamedEnum'

def test_module_named_value(module):
    assert module.NamedEnum.NamedMember == 20

def test_module_named_value_legacy(module):
    assert module.NamedMember == 20

def test_enum_operator_add(module):
    assert module.NamedEnum.NamedMember + 10 == 30

def test_enum_operator_eq(module):
    assert module.NamedEnum.NamedMember == 20

def test_class_named_value(module):
    assert module.EnumClass.ClassNamedEnum.ClassNamedMember == 50

def test_class_named_value_legacy(module):
    assert module.EnumClass.ClassNamedMember == 50

def test_named_get_member(module, members_valid, virtual_hook):
    assert members_valid.named_get() == module.EnumClass.ClassNamedEnum.ClassNamedMember
    virtual_hook.reraise()

def test_named_set_member(module, members_valid):
    members_valid.named_set(module.EnumClass.ClassNamedEnum.ClassNamedMember)

def test_named_var_member(module, members_valid):
    members_valid.named_var = module.EnumClass.ClassNamedEnum.ClassNamedMember

def test_named_overload_set(module, members_valid):
    members_valid.named_overload_set(
            module.EnumClass.ClassNamedEnum.ClassNamedMember)
    assert members_valid.named_overload


# The following test scoped enums.

def test_module_scoped_type(module):
    assert issubclass(module.ScopedEnum, Enum)

def test_module_scoped_value(module):
    assert module.ScopedEnum.ScopedMember.value == 30

def test_class_scoped_type(module):
    assert issubclass(module.EnumClass.ClassScopedEnum, Enum)

def test_class_scoped_value(module):
    assert module.EnumClass.ClassScopedEnum.ClassScopedMember.value == 70

def test_scoped_get_member(module, scoped_members, virtual_hook):
    assert scoped_members.scoped_get() is module.EnumClass.ClassScopedEnum.ClassScopedMember
    virtual_hook.reraise()

def test_scoped_set_member(module, scoped_members):
    scoped_members.scoped_set(
            module.EnumClass.ClassScopedEnum.ClassScopedMember)

def test_scoped_var_member(module, scoped_members):
    scoped_members.scoped_var = module.EnumClass.ClassScopedEnum.ClassScopedMember
