# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPGeneratorTestCase


class GenerateClassesTestCase(SIPGeneratorTestCase):
    """ Test the generator support for classes. """

    def test_Nonpublic_Superclasses(self):
        """ Test the support non-public super-classes. """

        # Check that the use of non-public super-classes fails.
        self.assertFalse(
                self.run_generator_test(
                        'gen_nonpublic_superclasses_module.sip'))
