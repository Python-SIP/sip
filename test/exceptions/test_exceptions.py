# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class ExceptionsTestCase(SIPTestCase):
    """ Test the support for exceptions across multiple modules. """

    # Enable exception support.
    exceptions = True

    def test_Exceptions(self):
        """ Test the throwing of an exception. """

        from handler import StdException
        from thrower import throwException

        self.assertRaises(StdException, throwException)
