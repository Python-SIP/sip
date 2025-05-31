# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class TemplatesTestCase(SIPTestCase):
    """ Test the support for templates. """

    def test_SimpleTemplateSuperclasses(self):
        """ Test the use of simple template arguments as super-classes. """

        from templates_module import CValue, CWrapper

        self.assertTrue(issubclass(CWrapper, CValue))

    def test_ScopedTemplateSuperclasses(self):
        """ Test the use of simple template arguments as super-classes within
        a scope.
        """

        from templates_module import NestedWrapper, Scope

        self.assertTrue(issubclass(NestedWrapper, Scope.Nested))

    def test_TemplateSuperclasses(self):
        """ Test the use of template arguments as super-classes. """

        from templates_module import (AValue, AValueWrapper, BaseClass, BValue,
                BValueWrapper)

        self.assertTrue(issubclass(AValueWrapper, BaseClass))
        a = AValueWrapper()
        self.assertIsInstance(a.getValue(), AValue)

        self.assertTrue(issubclass(BValueWrapper, BaseClass))
        b = BValueWrapper()
        self.assertIsInstance(b.getValue(), BValue)
