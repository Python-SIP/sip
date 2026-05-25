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

V14_PICKLE = b'\x80\x05\x95)\x00\x00\x00\x00\x00\x00\x00\x8c\rpickle_module\x94\x8c\x0bPickleClass\x94\x93\x94K\nK\x14\x86\x94R\x94.'
LEGACY_PICKLE = b'\x80\x05\x95L\x00\x00\x00\x00\x00\x00\x00\x8c\rpickle_module\x94\x8c\x0e_unpickle_type\x94\x93\x94\x8c\rpickle_module\x94\x8c\x0bPickleClass\x94K\nK\x14\x86\x94\x87\x94R\x94.'

def test_pickle_compatibility(module, abi_version):
    if abi_version >= 14:
        orig = module.PickleClass(10, 20)
        ds = pickle.dumps(orig)

        assert ds == V14_PICKLE
    else:
        l = pickle.loads(V14_PICKLE)
        assert isinstance(l, module.PickleClass)

        l = pickle.loads(LEGACY_PICKLE)
        assert isinstance(l, module.PickleClass)
