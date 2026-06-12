# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'subclass_convertors'


def test_local_module(package):
    obj = package.core_module.Subclass1()
    obj.init_instance()

    assert isinstance(obj.get_instance(), package.core_module.Subclass1)

def test_imported_module(package):
    obj = package.extras_module.Subclass2()
    obj.init_instance()

    assert isinstance(obj.get_instance(), package.extras_module.Subclass2)

def test_needs_cast(package):
    obj = package.extras_module.Subclass4()
    obj.init_instance()

    assert isinstance(obj.get_instance(), package.extras_module.Subclass4)
