# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'imports'


def test_namespace_extenders(package):
    assert package.core_module.NameSpace.get_core_value() == 10
    assert package.core_module.NameSpace.get_extras_value() == 20

    with pytest.raises(AttributeError):
        package.extras_module.NameSpace

def test_superclasses(package):
    assert issubclass(package.extras_module.Subclass,
            package.core_module.BaseClass)
