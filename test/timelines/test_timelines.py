# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from utils import SIPTestCase


class TimelinesTestCase(SIPTestCase):
    """ Test the support for timelines. """

    # The list of tags.
    tags = ['v2']

    def test_Timelines(self):
        """ Test the support for timelines. """

        import timelines_module

        mod_dict = timelines_module.__dict__

        self.assertIn('between_v1_and_v3_enabled', mod_dict)
        self.assertIn('up_to_unknown_enabled', mod_dict)

        self.assertNotIn('up_to_v2_disabled', mod_dict)
        self.assertNotIn('v3_and_after_disabled', mod_dict)
        self.assertNotIn('unknown_and_after_disabled', mod_dict)
        self.assertNotIn('between_unknown1_and_unknown2_disabled', mod_dict)
