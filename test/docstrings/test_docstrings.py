# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


def test_module_docstrings(module):
    assert module.__doc__ == 'Module'
