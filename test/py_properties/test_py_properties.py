# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


def test_property_getter(module):
    ao = module.AnObject()
    assert ao.int_val == -1

def test_property_setter(module):
    ao = module.AnObject()
    ao.int_val = 100
    assert ao.int_val == 100
