# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from enum import Enum, Flag, IntEnum, IntFlag

import pytest


# TODO
#cfg_enabled_for = [13, 14]
cfg_enabled_for = [13]


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
def members_invalid(named_enum_class):
    """ A fixture that creates the instance of a test class with invalid enum
    members.
    """

    return named_enum_class(0)


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


# The following test the /BaseType/ annotation.

def test_Enum_BaseType(module):
    assert issubclass(module.EnumBase, Enum)
    assert not issubclass(module.EnumBase, Flag)
    assert not issubclass(module.EnumBase, IntEnum)
    assert not issubclass(module.EnumBase, IntFlag)

def test_Flag_BaseType(module):
    assert issubclass(module.FlagBase, Flag)
    assert not issubclass(module.FlagBase, IntEnum)
    assert not issubclass(module.FlagBase, IntFlag)

def test_IntEnum_BaseType(module):
    assert not issubclass(module.IntEnumBase, Flag)
    assert issubclass(module.IntEnumBase, IntEnum)
    assert not issubclass(module.IntEnumBase, IntFlag)

def test_IntFlag_BaseType(module):
    assert not issubclass(module.IntFlagBase, IntEnum)
    assert issubclass(module.IntFlagBase, IntFlag)


# The following test named enums.

def test_module_named_type(module):
    assert issubclass(module.NamedEnum, Enum)

def test_module_named_value(module):
    assert module.NamedEnum.NamedMember.value == 20

def test_class_named_type(module):
    assert issubclass(module.EnumClass.ClassNamedEnum, Enum)

def test_class_named_value(module):
    assert module.EnumClass.ClassNamedEnum.ClassNamedMember.value == 50

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

def test_named_get_invalid(members_invalid, virtual_hook):
    members_invalid.named_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_named_set_invalid(members_invalid):
    with pytest.raises(TypeError):
        members_invalid.named_set(50)

def test_named_var_invalid(members_invalid):
    with pytest.raises(TypeError):
        members_invalid.named_var = 50


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
