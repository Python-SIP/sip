# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class ImportedExceptionsTestCase(SIPTestCase):
    """ Test the support for imported exceptions. """

    # Enable exception support.
    exceptions = True

    def test_Exceptions(self):
        """ Test the throwing of an imported exception. """

        from handler_module import StdException
        from thrower_module import throwException

        self.assertRaises(StdException, throwException)
