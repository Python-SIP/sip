# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_module_void_ret_no_args(module):
    module.reset_module_procedure_called()
    module.module_void_ret_no_args()
    assert module.was_module_procedure_called()

def test_module_int_ret_int_arg(module):
    assert module.module_doubler(3) == 6

def test_module_int_int_ret_int_int_args(module):
    # Testing a module int(int, int, int *) function.
    res = module.module_sum_diff(3, 1)

    assert isinstance(res, tuple)
    assert len(res) == 2
    assert res[0] == 4
    assert res[1] == 2

def test_module_var_args(module):
    res = module.module_var_args(0, 1, 2)

    assert isinstance(res, tuple)
    assert len(res) == 2
    assert res[0] == 1
    assert res[1] == 2

def test_insufficient_args(module):
    with pytest.raises(TypeError):
        module.module_doubler()

def test_excessive_args(module):
    with pytest.raises(TypeError):
        module.module_doubler(1, 1)

def test_wrong_type_args(module):
    with pytest.raises(TypeError):
        module.module_doubler('1')

def test_no_kwd_args_support(module):
    with pytest.raises(TypeError):
        module.module_doubler(value=3)

def test_bad_kwd_args(module):
    with pytest.raises(TypeError):
        module.incr_optional_kwd_args(1, bad=3)

    with pytest.raises(TypeError):
        module.incr_optional_kwd_args(1, 2, incr=3)

def test_optional_kwd_args_support(module):
    assert module.incr_optional_kwd_args(1) == 2
    assert module.incr_optional_kwd_args(1, 2) == 3
    assert module.incr_optional_kwd_args(1, incr=3) == 4

def test_all_kwd_args_support(module):
    assert module.incr_all_kwd_args(1) == 2
    assert module.incr_all_kwd_args(1, 2) == 3
    assert module.incr_all_kwd_args(1, incr=3) == 4

    assert module.incr_all_kwd_args(value=1) == 2
    assert module.incr_all_kwd_args(value=1, incr=2) == 3
    assert module.incr_all_kwd_args(incr=3, value=1) == 4

    with pytest.raises(TypeError):
        module.incr_all_kwd_args(incr=3)
