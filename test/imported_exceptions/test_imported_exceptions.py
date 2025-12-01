# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


# TODO
cfg_disabled_for = [14]

cfg_exceptions = True
cfg_package = 'pkg'


def test_imported_exceptions(package):
    with pytest.raises(package.handler_module.StdException):
        package.thrower_module.throwException()
