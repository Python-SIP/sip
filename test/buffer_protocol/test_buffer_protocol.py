# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


def test_nonbuffer(module):
    nonbuffer = module.NonbufferClass()

    # Buffer was added in Python v3.12.
    try:
        from collections.abc import Buffer
        assert not isinstance(nonbuffer, Buffer)
    except ImportError:
        assert not hasattr(nonbuffer, '__buffer__')

def test_buffer(module):
    buffer = module.BufferClass(b'abc')
    mv = memoryview(buffer)
    assert mv.tobytes() == b'abc'
