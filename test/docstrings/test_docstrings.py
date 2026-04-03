# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


# TODO Reimplement docstrings support for ABI v14.
cfg_disabled_for = [14]


def test_module_docstrings(module):
    assert module.__doc__ == 'Module'

def test_pod_ptr_arg_docstring(module):
    assert module.pod_ptr_arg.__doc__ == 'pod_ptr_arg(arg: bytes|None)'

def test_pod_ptr_opt_arg_docstring(module):
    assert module.pod_ptr_opt_arg.__doc__ == 'pod_ptr_opt_arg(arg: bytes|None = None)'

def test_bool_opt_arg_docstring(module):
    assert module.bool_opt_arg.__doc__ == 'bool_opt_arg(arg: bool = True)'
