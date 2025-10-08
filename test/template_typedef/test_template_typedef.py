# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class TypedefsTestCase(SIPTestCase):
    """ Test the support for template typedef. """

    def test_ModuleTemplateTypedef(self):
        """ Test the support for template typedef. """

        import template_typedef_module as ttdm

        self.assertEqual(ttdm.incrementVectorInt([1, 2, 3]), [2, 3, 4])
        self.assertEqual(ttdm.incrementVectorVectorInt([[1, 2, 3], [4, 5, 6]]), [[2, 3, 4], [5, 6, 7]])
