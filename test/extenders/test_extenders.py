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
