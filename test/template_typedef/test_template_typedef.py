# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


def test_mapped_type_typedef(module):
    assert module.incrementVectorInt([1, 2, 3]) == [2, 3, 4]

def test_mapped_type_template_typedef(module):
    assert module.incrementVectorVectorInt([[1, 2, 3], [4, 5, 6]]) == [[2, 3, 4], [5, 6, 7]]
