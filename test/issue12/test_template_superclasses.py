# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class TemplateSuperclassesTestCase(SIPTestCase):
    """ Test the support for template arguments as super-classes in a template
    class.  (See issue/12.)
    """

    ###########################################################################
    # The following test anonymous enums.
    ###########################################################################

    def test_TemplateSuperclasses(self):
        """ Test the use of template arguments as super-classes. """

        from .template_superclasses import (AValue, AValueWrapper, BaseClass,
                BValue, BValueWrapper)

        self.assertTrue(issubclass(AValueWrapper, BaseClass))
        a = AValueWrapper()
        self.assertIsInstance(a.getValue(), AValue)

        self.assertTrue(issubclass(BValueWrapper, BaseClass))
        b = BValueWrapper()
        self.assertIsInstance(b.getValue(), BValue)
