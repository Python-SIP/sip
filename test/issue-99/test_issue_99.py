# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


def test_class_pyname(module):
    assert module.KlassC.get_cpp_name() == b'KlassA'

def test_class_pyname_using_existing_cpp_name(module):
    assert module.KlassA.get_cpp_name() == b'KlassB'
