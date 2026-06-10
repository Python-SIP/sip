# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'imports'


def test_external_class(package):
    # Note that this only verifies that the generated code compiles but not
    # that it actually works.
    assert package.core_module.get_external() is None

def test_undefined_external_class(package):
    with pytest.raises(TypeError):
        package.core_module.check_undefined(None)

def test_superclasses(package):
    assert issubclass(package.extras_module.Subclass,
            package.core_module.BaseClass)
