# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'imports'


def test_superclasses(package):
    assert issubclass(package.extras_module.Subclass,
            package.core_module.BaseClass)
