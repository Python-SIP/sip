# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from sys import getrefcount

import pytest


# Module %GetCode and %SetCode is only supported by ABI v14.
cfg_enabled_for = [14]


def test_getters_and_setters(module):
    assert module.int_attr_getter == 20

    module.int_attr_setter = 20
    assert module.int_attr_setter == 40

    with pytest.raises(NameError):
        module.int_attr_bad_setter = 0

    assert module.int_attr_no_setter == 10
    with pytest.raises(ValueError):
        module.int_attr_no_setter = 0
