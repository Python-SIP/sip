# Copyright (c) 2020, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


from utils import SIPTestCase


class IntConvertorsTestCase(SIPTestCase):
    """ Test the integer convertors. """

    @classmethod
    def setUpClass(cls):
        """ Set up the test case. """

        super().setUpClass()

        from .int_convertors import IntConvertors

        # Compute the various test values based on the native sizes.
        cls.CHAR_LOWER = IntConvertors.char_lower()
        cls.CHAR_UPPER = IntConvertors.char_upper()
        cls.SIGNED_CHAR_LOWER, cls.SIGNED_CHAR_UPPER = cls._signed_bounds(
                IntConvertors.signed_char_sizeof())
        cls.SHORT_LOWER, cls.SHORT_UPPER = cls._signed_bounds(
                IntConvertors.short_sizeof())
        cls.INT_LOWER, cls.INT_UPPER = cls._signed_bounds(
                IntConvertors.int_sizeof())
        cls.LONG_LOWER, cls.LONG_UPPER = cls._signed_bounds(
                IntConvertors.long_sizeof())
        cls.LONG_LONG_LOWER, cls.LONG_LONG_UPPER = cls._signed_bounds(
                IntConvertors.long_long_sizeof())
        cls.UNSIGNED_CHAR_UPPER = cls._unsigned_upper_bound(
                IntConvertors.unsigned_char_sizeof())
        cls.UNSIGNED_SHORT_UPPER = cls._unsigned_upper_bound(
                IntConvertors.unsigned_short_sizeof())
        cls.UNSIGNED_INT_UPPER = cls._unsigned_upper_bound(
                IntConvertors.unsigned_int_sizeof())
        cls.UNSIGNED_LONG_UPPER = cls._unsigned_upper_bound(
                IntConvertors.unsigned_long_sizeof())
        cls.UNSIGNED_LONG_LONG_UPPER = cls._unsigned_upper_bound(
                IntConvertors.unsigned_long_long_sizeof())

        class LimitsFixture(IntConvertors):
            """ The base test fixture for those implementing a range of values.
            """

            def __init__(self, limits):
                """ Initialise the object. """

                super().__init__()

                self.limits = limits

        class ValidLowerFixture(LimitsFixture):
            """ A fixture for testing the lower bound of non-overflowing signed
            values.
            """

            def char_virt(self):
                return self.limits.CHAR_LOWER

            def signed_char_virt(self):
                return self.limits.SIGNED_CHAR_LOWER

            def short_virt(self):
                return self.limits.SHORT_LOWER

            def int_virt(self):
                return self.limits.INT_LOWER

            def long_virt(self):
                return self.limits.LONG_LOWER

            def long_long_virt(self):
                return self.limits.LONG_LONG_LOWER

        cls.valid_lower_fixture = ValidLowerFixture(cls)

        class ValidUpperFixture(LimitsFixture):
            """ A fixture for testing the upper bound of non-overflowing
            values.
            """

            def char_virt(self):
                return self.limits.CHAR_UPPER

            def signed_char_virt(self):
                return self.limits.SIGNED_CHAR_UPPER

            def short_virt(self):
                return self.limits.SHORT_UPPER

            def int_virt(self):
                return self.limits.INT_UPPER

            def long_virt(self):
                return self.limits.LONG_UPPER

            def long_long_virt(self):
                return self.limits.LONG_LONG_UPPER

            def unsigned_char_virt(self):
                return self.limits.UNSIGNED_CHAR_UPPER

            def unsigned_short_virt(self):
                return self.limits.UNSIGNED_SHORT_UPPER

            def unsigned_int_virt(self):
                return self.limits.UNSIGNED_INT_UPPER

            def unsigned_long_virt(self):
                return self.limits.UNSIGNED_LONG_UPPER

            def unsigned_long_long_virt(self):
                return self.limits.UNSIGNED_LONG_LONG_UPPER

        cls.valid_upper_fixture = ValidUpperFixture(cls)

        class InvalidFixture(IntConvertors):
            """ A fixture for testing invalid values. """

            def bool_virt(self):
                return '0'

            def char_virt(self):
                return '0'

            def signed_char_virt(self):
                return '0'

            def short_virt(self):
                return '0'

            def int_virt(self):
                return '0'

            def long_virt(self):
                return '0'

            def long_long_virt(self):
                return '0'

            def unsigned_char_virt(self):
                return '0'

            def unsigned_short_virt(self):
                return '0'

            def unsigned_int_virt(self):
                return '0'

            def unsigned_long_virt(self):
                return '0'

            def unsigned_long_long_virt(self):
                return '0'

        cls.invalid_fixture = InvalidFixture()

        class OverflowLowerFixture(LimitsFixture):
            """ A fixture for testing the lower bound of overflowing signed
            values.
            """

            def char_virt(self):
                return self.limits.CHAR_LOWER - 1

            def signed_char_virt(self):
                return self.limits.SIGNED_CHAR_LOWER - 1

            def short_virt(self):
                return self.limits.SHORT_LOWER - 1

            def int_virt(self):
                return self.limits.INT_LOWER - 1

            def long_virt(self):
                return self.limits.LONG_LOWER - 1

            def long_long_virt(self):
                return self.limits.LONG_LONG_LOWER - 1

        cls.overflow_lower_fixture = OverflowLowerFixture(cls)

        class OverflowUpperFixture(LimitsFixture):
            """ A fixture for testing the upper bound of overflowing values. """

            def char_virt(self):
                return self.limits.CHAR_UPPER + 1

            def signed_char_virt(self):
                return self.limits.SIGNED_CHAR_UPPER + 1

            def short_virt(self):
                return self.limits.SHORT_UPPER + 1

            def int_virt(self):
                return self.limits.INT_UPPER + 1

            def long_virt(self):
                return self.limits.LONG_UPPER + 1

            def long_long_virt(self):
                return self.limits.LONG_LONG_UPPER + 1

            def unsigned_char_virt(self):
                return self.limits.UNSIGNED_CHAR_UPPER + 1

            def unsigned_short_virt(self):
                return self.limits.UNSIGNED_SHORT_UPPER + 1

            def unsigned_int_virt(self):
                return self.limits.UNSIGNED_INT_UPPER + 1

            def unsigned_long_virt(self):
                return self.limits.UNSIGNED_LONG_UPPER + 1

            def unsigned_long_long_virt(self):
                return self.limits.UNSIGNED_LONG_LONG_UPPER + 1

        cls.overflow_upper_fixture = OverflowUpperFixture(cls)

        class BoolFixture(IntConvertors):
            """ A fixture for testing valid boolean values. """

            def __init__(self, value):
                """ Initialise the object. """

                super().__init__()

                self._value = value

            def bool_virt(self):
                return self._value

        cls.true_fixture = BoolFixture(True)
        cls.false_fixture = BoolFixture(False)
        cls.nonzero_fixture = BoolFixture(-1)
        cls.zero_fixture = BoolFixture(0)

    @staticmethod
    def _signed_bounds(nrbytes):
        """ Return the range of values for a number of bytes representing a
        signed value.
        """

        v = 1 << ((nrbytes * 8) - 1)

        return -v, v - 1

    @staticmethod
    def _unsigned_upper_bound(nrbytes):
        """ Return the upper bound for a number of bytes representing an
        unsigned value.
        """

        return (1 << (nrbytes * 8)) - 1

    @classmethod
    def tearDownClass(cls):
        """ Tear down the test case. """

        # Remove all references to the extension module so that the superclass
        # can unload it.
        del cls.valid_lower_fixture
        del cls.valid_upper_fixture

        del cls.invalid_fixture

        del cls.overflow_lower_fixture
        del cls.overflow_upper_fixture

        del cls.true_fixture
        del cls.false_fixture
        del cls.nonzero_fixture
        del cls.zero_fixture

        super().tearDownClass()

    ###########################################################################
    # The following test for valid values.
    ###########################################################################

    def test_char_get_lower_valid(self):
        """ char virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.char_get(), self.CHAR_LOWER)

    def test_char_get_upper_valid(self):
        """ char virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.char_get(), self.CHAR_UPPER)

    def test_char_set_lower_valid(self):
        """ char function argument lower bound. """

        self.valid_lower_fixture.char_set(self.CHAR_LOWER)

    def test_char_set_upper_valid(self):
        """ char function argument upper bound. """

        self.valid_upper_fixture.char_set(self.CHAR_UPPER)

    def test_char_var_lower_valid(self):
        """ char instance variable lower bound. """

        self.valid_lower_fixture.char_var = self.CHAR_LOWER

    def test_char_var_upper_valid(self):
        """ char instance variable upper bound. """

        self.valid_upper_fixture.char_var = self.CHAR_UPPER

    def test_signed_char_get_lower_valid(self):
        """ signed char virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.signed_char_get(),
                self.SIGNED_CHAR_LOWER)

    def test_signed_char_get_upper_valid(self):
        """ signed char virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.signed_char_get(),
                self.SIGNED_CHAR_UPPER)

    def test_signed_char_set_lower_valid(self):
        """ signed char function argument lower bound. """

        self.valid_lower_fixture.signed_char_set(self.SIGNED_CHAR_LOWER)

    def test_signed_char_set_upper_valid(self):
        """ signed char function argument upper bound. """

        self.valid_upper_fixture.signed_char_set(self.SIGNED_CHAR_UPPER)

    def test_signed_char_var_lower_valid(self):
        """ signed char instance variable lower bound. """

        self.valid_lower_fixture.signed_char_var = self.SIGNED_CHAR_LOWER

    def test_signed_char_var_upper_valid(self):
        """ signed char instance variable upper bound. """

        self.valid_upper_fixture.signed_char_var = self.SIGNED_CHAR_UPPER

    def test_short_get_lower_valid(self):
        """ short virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.short_get(),
                self.SHORT_LOWER)

    def test_short_get_upper_valid(self):
        """ short virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.short_get(),
                self.SHORT_UPPER)

    def test_short_set_lower_valid(self):
        """ short function argument lower bound. """

        self.valid_lower_fixture.short_set(self.SHORT_LOWER)

    def test_short_set_upper_valid(self):
        """ short function argument upper bound. """

        self.valid_upper_fixture.short_set(self.SHORT_UPPER)

    def test_short_var_lower_valid(self):
        """ short instance variable lower bound. """

        self.valid_lower_fixture.short_var = self.SHORT_LOWER

    def test_short_var_upper_valid(self):
        """ short instance variable upper bound. """

        self.valid_upper_fixture.short_var = self.SHORT_UPPER

    def test_int_get_lower_valid(self):
        """ int virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.int_get(), self.INT_LOWER)

    def test_int_get_upper_valid(self):
        """ int virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.int_get(), self.INT_UPPER)

    def test_int_set_lower_valid(self):
        """ int function argument lower bound. """

        self.valid_lower_fixture.int_set(self.INT_LOWER)

    def test_int_set_upper_valid(self):
        """ int function argument upper bound. """

        self.valid_upper_fixture.int_set(self.INT_UPPER)

    def test_int_var_lower_valid(self):
        """ int instance variable lower bound. """

        self.valid_lower_fixture.int_var = self.INT_LOWER

    def test_int_var_upper_valid(self):
        """ int instance variable upper bound. """

        self.valid_upper_fixture.int_var = self.INT_UPPER

    def test_long_get_lower_valid(self):
        """ long virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.long_get(), self.LONG_LOWER)

    def test_long_get_upper_valid(self):
        """ long virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.long_get(), self.LONG_UPPER)

    def test_long_set_lower_valid(self):
        """ long function argument lower bound. """

        self.valid_lower_fixture.long_set(self.LONG_LOWER)

    def test_long_set_upper_valid(self):
        """ long function argument upper bound. """

        self.valid_upper_fixture.long_set(self.LONG_UPPER)

    def test_long_var_lower_valid(self):
        """ long instance variable lower bound. """

        self.valid_lower_fixture.long_var = self.LONG_LOWER

    def test_long_var_upper_valid(self):
        """ long instance variable upper bound. """

        self.valid_upper_fixture.long_var = self.LONG_UPPER

    def test_long_long_get_lower_valid(self):
        """ long long virtual result lower bound. """

        self.assertEqual(self.valid_lower_fixture.long_long_get(),
                self.LONG_LONG_LOWER)

    def test_long_long_get_upper_valid(self):
        """ long long virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.long_long_get(),
                self.LONG_LONG_UPPER)

    def test_long_long_set_lower_valid(self):
        """ long long function argument lower bound. """

        self.valid_lower_fixture.long_long_set(self.LONG_LONG_LOWER)

    def test_long_long_set_upper_valid(self):
        """ long long function argument upper bound. """

        self.valid_upper_fixture.long_long_set(self.LONG_LONG_UPPER)

    def test_long_long_var_lower_valid(self):
        """ long long instance variable lower bound. """

        self.valid_lower_fixture.long_long_var = self.LONG_LONG_LOWER

    def test_long_long_var_upper_valid(self):
        """ long long instance variable upper bound. """

        self.valid_upper_fixture.long_long_var = self.LONG_LONG_UPPER

    def test_unsigned_char_get_upper_valid(self):
        """ unsigned char virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.unsigned_char_get(),
                self.UNSIGNED_CHAR_UPPER)

    def test_unsigned_char_set_upper_valid(self):
        """ unsigned char function argument upper bound. """

        self.valid_upper_fixture.unsigned_char_set(self.UNSIGNED_CHAR_UPPER)

    def test_unsigned_char_var_upper_valid(self):
        """ unsigned char instance variable upper bound. """

        self.valid_upper_fixture.unsigned_char_var = self.UNSIGNED_CHAR_UPPER

    def test_unsigned_short_get_upper_valid(self):
        """ unsigned short virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.unsigned_short_get(),
                self.UNSIGNED_SHORT_UPPER)

    def test_unsigned_short_set_upper_valid(self):
        """ unsigned short function argument upper bound. """

        self.valid_upper_fixture.unsigned_short_set(self.UNSIGNED_SHORT_UPPER)

    def test_unsigned_short_var_upper_valid(self):
        """ unsigned short instance variable upper bound. """

        self.valid_upper_fixture.unsigned_short_var = self.UNSIGNED_SHORT_UPPER

    def test_unsigned_int_get_upper_valid(self):
        """ unsigned int virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.unsigned_int_get(),
                self.UNSIGNED_INT_UPPER)

    def test_unsigned_int_set_upper_valid(self):
        """ unsigned int function argument upper bound. """

        self.valid_upper_fixture.unsigned_int_set(self.UNSIGNED_INT_UPPER)

    def test_unsigned_int_var_upper_valid(self):
        """ unsigned int instance variable upper bound. """

        self.valid_upper_fixture.unsigned_int_var = self.UNSIGNED_INT_UPPER

    def test_unsigned_long_get_upper_valid(self):
        """ unsigned long virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.unsigned_long_get(),
                self.UNSIGNED_LONG_UPPER)

    def test_unsigned_long_set_upper_valid(self):
        """ unsigned long function argument upper bound. """

        self.valid_upper_fixture.unsigned_long_set(self.UNSIGNED_LONG_UPPER)

    def test_unsigned_long_var_upper_valid(self):
        """ unsigned long instance variable upper bound. """

        self.valid_upper_fixture.unsigned_long_var = self.UNSIGNED_LONG_UPPER

    def test_unsigned_long_long_get_upper_valid(self):
        """ unsigned long long virtual result upper bound. """

        self.assertEqual(self.valid_upper_fixture.unsigned_long_long_get(),
                self.UNSIGNED_LONG_LONG_UPPER)

    def test_unsigned_long_long_set_upper_valid(self):
        """ unsigned long long function argument upper bound. """

        self.valid_upper_fixture.unsigned_long_long_set(
                self.UNSIGNED_LONG_LONG_UPPER)

    def test_unsigned_long_long_var_upper_valid(self):
        """ unsigned long long instance variable upper bound. """

        self.valid_upper_fixture.unsigned_long_long_var = self.UNSIGNED_LONG_LONG_UPPER

    ###########################################################################
    # The following test for invalid values.
    ###########################################################################

    def test_bool_get_invalid(self):
        """ bool virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.bool_get()
            self.uninstall_hook()

    def test_bool_set_invalid(self):
        """ bool function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.bool_set('0')

    def test_bool_var_invalid(self):
        """ bool instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.bool_var = '0'

    def test_char_get_invalid(self):
        """ char virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.char_get()
            self.uninstall_hook()

    def test_char_set_invalid(self):
        """ char function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.char_set('0')

    def test_char_var_invalid(self):
        """ char instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.char_var = '0'

    def test_signed_char_get_invalid(self):
        """ signed char virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.signed_char_get()
            self.uninstall_hook()

    def test_signed_char_set_invalid(self):
        """ signed char function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.signed_char_set('0')

    def test_signed_char_var_invalid(self):
        """ signed char instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.signed_char_var = '0'

    def test_short_get_invalid(self):
        """ short virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.short_get()
            self.uninstall_hook()

    def test_short_set_invalid(self):
        """ short function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.short_set('0')

    def test_short_var_invalid(self):
        """ short instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.short_var = '0'

    def test_int_get_invalid(self):
        """ int virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.int_get()
            self.uninstall_hook()

    def test_int_set_invalid(self):
        """ int function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.int_set('0')

    def test_int_var_invalid(self):
        """ int instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.int_var = '0'

    def test_long_get_invalid(self):
        """ long virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.long_get()
            self.uninstall_hook()

    def test_long_set_invalid(self):
        """ long function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.long_set('0')

    def test_long_var_invalid(self):
        """ long instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.long_var = '0'

    def test_long_long_get_invalid(self):
        """ long long virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.long_long_get()
            self.uninstall_hook()

    def test_long_long_set_invalid(self):
        """ long long function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.long_long_set('0')

    def test_long_long_var_invalid(self):
        """ long long instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.long_long_var = '0'

    def test_unsigned_char_get_invalid(self):
        """ unsigned char virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.unsigned_char_get()
            self.uninstall_hook()

    def test_unsigned_char_set_invalid(self):
        """ unsigned char function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_char_set('0')

    def test_unsigned_char_var_invalid(self):
        """ unsigned char instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_char_var = '0'

    def test_unsigned_short_get_invalid(self):
        """ unsigned short virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.unsigned_short_get()
            self.uninstall_hook()

    def test_unsigned_short_set_invalid(self):
        """ unsigned short function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_short_set('0')

    def test_unsigned_short_var_invalid(self):
        """ unsigned short instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_short_var = '0'

    def test_unsigned_int_get_invalid(self):
        """ unsigned int virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.unsigned_int_get()
            self.uninstall_hook()

    def test_unsigned_int_set_invalid(self):
        """ unsigned int function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_int_set('0')

    def test_unsigned_int_var_invalid(self):
        """ unsigned int instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_int_var = '0'

    def test_unsigned_long_get_invalid(self):
        """ unsigned long virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.unsigned_long_get()
            self.uninstall_hook()

    def test_unsigned_long_set_invalid(self):
        """ unsigned long function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_long_set('0')

    def test_unsigned_long_var_invalid(self):
        """ unsigned long instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_long_var = '0'

    def test_unsigned_long_long_get_invalid(self):
        """ unsigned long long virtual result. """

        with self.assertRaises(TypeError):
            self.install_hook()
            self.invalid_fixture.unsigned_long_long_get()
            self.uninstall_hook()

    def test_unsigned_long_long_set_invalid(self):
        """ unsigned long long function argument. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_long_long_set('0')

    def test_unsigned_long_long_var_invalid(self):
        """ unsigned long long instance variable. """

        with self.assertRaises(TypeError):
            self.invalid_fixture.unsigned_long_long_var = '0'

    ###########################################################################
    # The following test for under/overlowing values.
    ###########################################################################

    def test_char_get_lower_overflow(self):
        """ char virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.char_get()
            self.uninstall_hook()

    def test_char_get_upper_overflow(self):
        """ char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.char_get()
            self.uninstall_hook()

    def test_char_set_lower_overflow(self):
        """ char function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.char_set(self.CHAR_LOWER - 1)

    def test_char_set_upper_overflow(self):
        """ char function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.char_set(self.CHAR_UPPER + 1)

    def test_char_var_lower_overflow(self):
        """ char instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.char_var = self.CHAR_LOWER - 1

    def test_char_var_upper_overflow(self):
        """ char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.char_var = self.CHAR_UPPER + 1

    def test_signed_char_get_lower_overflow(self):
        """ signed char virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.signed_char_get()
            self.uninstall_hook()

    def test_signed_char_get_upper_overflow(self):
        """ signed char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.signed_char_get()
            self.uninstall_hook()

    def test_signed_char_set_lower_overflow(self):
        """ signed char function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.signed_char_set(
                    self.SIGNED_CHAR_LOWER - 1)

    def test_signed_char_set_upper_overflow(self):
        """ signed char function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.signed_char_set(
                    self.SIGNED_CHAR_UPPER + 1)

    def test_signed_char_var_lower_overflow(self):
        """ signed char instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.signed_char_var = self.SIGNED_CHAR_LOWER - 1

    def test_signed_char_var_upper_overflow(self):
        """ signed char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.signed_char_var = self.SIGNED_CHAR_UPPER + 1

    def test_short_get_lower_overflow(self):
        """ short virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.short_get()
            self.uninstall_hook()

    def test_short_get_upper_overflow(self):
        """ short virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.short_get()
            self.uninstall_hook()

    def test_short_set_lower_overflow(self):
        """ short function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.short_set(self.SHORT_LOWER - 1)

    def test_short_set_upper_overflow(self):
        """ short function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.short_set(self.SHORT_UPPER + 1)

    def test_short_var_lower_overflow(self):
        """ short instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.short_var = self.SHORT_LOWER - 1

    def test_short_var_upper_overflow(self):
        """ short instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.short_var = self.SHORT_UPPER + 1

    def test_int_get_lower_overflow(self):
        """ int virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.int_get()
            self.uninstall_hook()

    def test_int_get_upper_overflow(self):
        """ int virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.int_get()
            self.uninstall_hook()

    def test_int_set_lower_overflow(self):
        """ int function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.int_set(self.INT_LOWER - 1)

    def test_int_set_upper_overflow(self):
        """ int function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.int_set(self.INT_UPPER + 1)

    def test_int_var_lower_overflow(self):
        """ int instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.int_var = self.INT_LOWER - 1

    def test_int_var_upper_overflow(self):
        """ int instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.int_var = self.INT_UPPER + 1

    def test_long_get_lower_overflow(self):
        """ long virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.long_get()
            self.uninstall_hook()

    def test_long_get_upper_overflow(self):
        """ long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.long_get()
            self.uninstall_hook()

    def test_long_set_lower_overflow(self):
        """ long function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.long_set(self.LONG_LOWER - 1)

    def test_long_set_upper_overflow(self):
        """ long function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.long_set(self.LONG_UPPER + 1)

    def test_long_var_lower_overflow(self):
        """ long instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.long_var = self.LONG_LOWER - 1

    def test_long_var_upper_overflow(self):
        """ long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.long_var = self.LONG_UPPER + 1

    def test_long_long_get_lower_overflow(self):
        """ long long virtual result lower bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_lower_fixture.long_long_get()
            self.uninstall_hook()

    def test_long_long_get_upper_overflow(self):
        """ long long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.long_long_get()
            self.uninstall_hook()

    def test_long_long_set_lower_overflow(self):
        """ long long function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.long_long_set(self.LONG_LONG_LOWER - 1)

    def test_long_long_set_upper_overflow(self):
        """ long long function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.long_long_set(self.LONG_LONG_UPPER + 1)

    def test_long_long_var_lower_overflow(self):
        """ long long instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.overflow_lower_fixture.long_long_var = self.LONG_LONG_LOWER - 1

    def test_long_long_var_upper_overflow(self):
        """ long long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.long_long_var = self.LONG_LONG_UPPER + 1

    def test_unsigned_char_get_upper_overflow(self):
        """ unsigned char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.unsigned_char_get()
            self.uninstall_hook()

    def test_unsigned_char_set_upper_overflow(self):
        """ unsigned char function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_char_set(
                    self.UNSIGNED_CHAR_UPPER + 1)

    def test_unsigned_char_var_upper_overflow(self):
        """ unsigned char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_char_var = self.UNSIGNED_CHAR_UPPER + 1

    def test_unsigned_short_get_upper_overflow(self):
        """ unsigned short virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.unsigned_short_get()
            self.uninstall_hook()

    def test_unsigned_short_set_upper_overflow(self):
        """ unsigned short function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_short_set(
                    self.UNSIGNED_SHORT_UPPER + 1)

    def test_unsigned_short_var_upper_overflow(self):
        """ unsigned short instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_short_var = self.UNSIGNED_SHORT_UPPER + 1

    def test_unsigned_int_get_upper_overflow(self):
        """ unsigned int virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.unsigned_int_get()
            self.uninstall_hook()

    def test_unsigned_int_set_upper_overflow(self):
        """ unsigned int function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_int_set(
                    self.UNSIGNED_INT_UPPER + 1)

    def test_unsigned_int_var_upper_overflow(self):
        """ unsigned int instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_int_var = self.UNSIGNED_INT_UPPER + 1

    def test_unsigned_long_get_upper_overflow(self):
        """ unsigned long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.unsigned_long_get()
            self.uninstall_hook()

    def test_unsigned_long_set_upper_overflow(self):
        """ unsigned long function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_long_set(
                    self.UNSIGNED_LONG_UPPER + 1)

    def test_unsigned_long_var_upper_overflow(self):
        """ unsigned long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_long_var = self.UNSIGNED_LONG_UPPER + 1

    def test_unsigned_long_long_get_upper_overflow(self):
        """ unsigned long long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            self.install_hook()
            self.overflow_upper_fixture.unsigned_long_long_get()
            self.uninstall_hook()

    def test_unsigned_long_long_set_upper_overflow(self):
        """ unsigned long long function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_long_long_set(
                    self.UNSIGNED_LONG_LONG_UPPER + 1)

    def test_unsigned_long_long_var_upper_overflow(self):
        """ unsigned long long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.overflow_upper_fixture.unsigned_long_long_var = self.UNSIGNED_LONG_LONG_UPPER + 1

    ###########################################################################
    # The following test bool convertors for valid values.
    ###########################################################################

    def test_bool_get_true(self):
        """ bool virtual result with a True value. """

        self.assertIs(self.true_fixture.bool_get(), True)

    def test_bool_set_true(self):
        """ bool function argument with a True value. """

        self.true_fixture.bool_set(True)

    def test_bool_var_true(self):
        """ bool instance variable with a True value. """

        self.true_fixture.bool_var = True

    def test_bool_get_false(self):
        """ bool virtual result with a True value. """

        self.assertIs(self.false_fixture.bool_get(), False)

    def test_bool_set_false(self):
        """ bool function argument with a False value. """

        self.false_fixture.bool_set(False)

    def test_bool_var_false(self):
        """ bool instance variable with a False value. """

        self.false_fixture.bool_var = False

    def test_bool_get_nonzero(self):
        """ bool virtual result with a non-zero value. """

        self.assertIs(self.nonzero_fixture.bool_get(), True)

    def test_bool_set_nonzero(self):
        """ bool function argument with a non-zero value. """

        self.nonzero_fixture.bool_set(-1)

    def test_bool_var_nonzero(self):
        """ bool instance variable with a non-zero value. """

        self.nonzero_fixture.bool_var = -1

    def test_bool_get_zero(self):
        """ bool virtual result with a zero value. """

        self.assertIs(self.zero_fixture.bool_get(), False)

    def test_bool_set_zero(self):
        """ bool function argument with a zero value. """

        self.zero_fixture.bool_set(0)

    def test_bool_var_zero(self):
        """ bool instance variable with a zero value. """

        self.zero_fixture.bool_var = 0
