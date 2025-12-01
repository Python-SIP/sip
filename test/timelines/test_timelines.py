# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


# The list of tags.
cfg_tags = ['v2']


def test_upto(module):
    assert 'up_to_v2_disabled' not in module.__dict__

def test_after(module):
    assert 'v3_and_after_disabled' not in module.__dict__

def test_between(module):
    assert 'between_v1_and_v3_enabled' in module.__dict__

def test_upto_unknown(module):
    assert 'up_to_unknown_enabled' in module.__dict__

def test_after_unknown(module):
    assert 'unknown_and_after_disabled' not in module.__dict__

def test_between_unknowns(module):
    assert 'between_unknown1_and_unknown2_disabled' not in module.__dict__
