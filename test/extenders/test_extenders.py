# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'extenders'


def test_core_callable(package, abi_package_version):
    # Older versions of the ABI do not support extending existing callables
    # with new overloads.
    if abi_package_version >= 14:
        twice_i = package.core_module.NameSpace.twice(5)

        assert isinstance(twice_i, int)
        assert twice_i == 10

def test_core_callable_docstrings(package, abi_package_version):
    # Older versions of the ABI do not support dynamic docstrings.
    if abi_package_version >= 14:
        assert len(package.core_module.NameSpace.twice.__doc__.split('\n')) == 2

def test_extended_overload(package):
    twice_f = package.core_module.NameSpace.twice(5.0)

    assert isinstance(twice_f, float)
    assert twice_f == 10.0

def test_extended_callable_or_overload(package, abi_package_version):
    # Older versions of the ABI do not support extending existing callables
    # with new overloads.
    if abi_package_version >= 14:
        thrice_i = package.core_module.NameSpace.thrice(5)

        assert isinstance(thrice_i, int)
        assert thrice_i == 15

        thrice_f = package.core_module.NameSpace.thrice(5.0)

        assert isinstance(thrice_f, float)
        assert thrice_f == 15.0

def test_extended_callable_or_overload_docstrings(package, abi_package_version):
    # Older versions of the ABI do not support dynamic docstrings.
    if abi_package_version >= 14:
        assert len(package.core_module.NameSpace.thrice.__doc__.split('\n')) == 2

def test_extended_attribute_get(package):
    assert package.core_module.NameSpace.in_extras1 == 33

def test_extended_attribute_set(package, abi_package_version):
    package.core_module.NameSpace.in_extras1 = 44
    assert package.core_module.NameSpace.in_extras1 == 44

    # For ABI v14 check the C++ value has changed and not the type dict.
    if abi_package_version >= 14:
        assert package.extras1_module.get_in_extras1() == 44

def test_extended_operator_overload(package):
    klass = package.core_module.Klass()
    klass.set_int_attr(10)

    assert klass + 2.0 == 12.0

def test_extended_operator(package, abi_package_version):
    # Older versions of the ABI do not support new slots in extensions.
    if abi_package_version >= 14:
        klass = package.core_module.Klass()
        klass.set_int_attr(10)

        assert klass * 2 == 20

def test_init_extender(package):
    cast = package.extras1_module.Cast(20)
    klass = package.core_module.Klass(cast)

    assert klass.get_int_attr() == 20

def test_init_extender_docstrings(package, abi_package_version):
    # Older versions of the ABI do not support dynamic docstrings.
    if abi_package_version >= 14:
        assert len(package.core_module.Klass.__doc__.split('\n')) == 3

def test_dir(package):
    assert 'thrice' in dir(package.core_module.NameSpace)
