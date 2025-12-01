# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from sys import getrefcount

import pytest


# TODO
cfg_disabled_for = [14]


def test_movable(module):
    """ Test the support for the /Movable/ mapped type annotation.  (See
    issue/60.)  It also verifies the test implementation of the
    std::unique_ptr mapped type.
    """

    ao = module.AnObject(3)
    ow = module.ObjectWrapper()

    # As of Python v3.14 we can't make assumptions about initial reference
    # counts so we test for increases and decreases rather than absolute
    # values.
    ao_base_refcount = getrefcount(ao)

    # Test the value of the object.
    assert ao.getValue() == 3
    assert getrefcount(ao) == ao_base_refcount

    # Test an empty wrapper.
    assert ow.getObjectValue() == -1000

    # Test an non-empty wrapper.
    ow.setObject(ao)
    assert getrefcount(ao) == ao_base_refcount + 1
    assert ow.getObjectValue() == 4

    # Unwrap the object and test the wrapper.
    ao2 = ow.takeObject()
    assert ow.getObjectValue() == -1000

    # Re-test the value of the object.
    assert ao2.getValue() == 4

    # Check that the original Python object no longer wraps the C++ object.
    assert getrefcount(ao) == ao_base_refcount

    with pytest.raises(RuntimeError):
        ao.getValue()
