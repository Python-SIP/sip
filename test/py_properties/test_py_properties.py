# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class PyPropertiesTestCase(SIPTestCase):
    """ Test the support for the %Property directive.  (See issue/74.)
    """

    def test_PyProperties(self):
        """ Test the support for %Property. """

        from py_properties_module import AnObject

        ao = AnObject(3)
        self.assertEqual(ao.int_val, 3)

        ao.int_val = 100
        self.assertEqual(ao.int_val, 100)
