# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import os
import subprocess


from .sip_base_test_case import SIPBaseTestCase


class SIPGeneratorTestCase(SIPBaseTestCase):
    """ Encapsulate a test case that tests the generation (but not the build or
    execution) of a number of test modules.
    """

    @classmethod
    def run_generator_test(cls, sip_file):
        """ Run a test that generates a test module and return True if it was
        successfully generated.
        """

        root_dir = cls.get_test_root_directory()

        try:
            cls.build_test_module(os.path.join(root_dir, sip_file), root_dir,
                    no_compile=True)
        except subprocess.CalledProcessError:
            return False

        return True
