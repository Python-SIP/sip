# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest

from collections.abc import Buffer


def test_nonbuffer(module):
    nonbuffer = module.NonbufferClass()
    assert not isinstance(nonbuffer, Buffer)

def test_buffer(module):
    buffer = module.BufferClass(b'abc')
    mv = memoryview(buffer)
    assert mv.tobytes() == b'abc'
