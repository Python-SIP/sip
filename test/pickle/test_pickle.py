# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest

import pickle


def test_pickle(module):
    orig = module.PickleClass(10, 20)
    ds = pickle.dumps(orig)
    clone = pickle.loads(ds)

    assert clone is not orig
    assert isinstance(clone, module.PickleClass)
    assert (clone.x() == orig.x() and clone.y() == orig.y())
