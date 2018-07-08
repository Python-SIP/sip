import sys
import unittest

from sip import enableoverflowchecking

from test import Test


# The exception raised by a virtual re-implementation.
_exc = None

# The saved exception hook.
_old_hook = None


def _hook(xtype, xvalue, xtb):
    """ The replacement exceptionhook. """

    global _exc

    # Save the exception for later.
    _exc = xvalue


def install_hook():
    """ Install an exception hook that will remember exceptions raised by
    virtual re-implementations.
    """

    global _exc, _old_hook

    # Clear the saved exception.
    _exc = None

    # Save the old hook and install the new one.
    _old_hook = sys.excepthook
    sys.excepthook = _hook


def uninstall_hook():
    """ Restore the original exception hook and re-raise any exception raised
    by a virtual re-implementation.
    """

    sys.excepthook = _old_hook

    if _exc is not None:
        raise _exc


class InvalidFixture(Test):
    """ A fixture for testing invalid values. """

    def scoped_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return 10

    def named_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def bool_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def unsigned_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def unsigned_short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def unsigned_int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def unsigned_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'

    def unsigned_long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return '0'


class ScopedEnumFixture(Test):
    """ A fixture for testing scoped enum values. """

    def scoped_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return Test.Scoped.scoped


class NamedEnumFixture(Test):
    """ A fixture for testing named enum values. """

    def __init__(self, value):
        """ Initialise the object. """

        super().__init__()

        self._value = value

    def named_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self._value


class BoolFixture(Test):
    """ A fixture for testing valid boolean values. """

    def __init__(self, value):
        """ Initialise the object. """

        super().__init__()

        self._value = value

    def bool_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self._value


class LimitsFixture(Test):
    """ The base test fixture for those implementing a range of values. """

    def __init__(self, limits):
        """ Initialise the object. """

        super().__init__()

        self.limits = limits


class ValidLowerFixture(LimitsFixture):
    """ A fixture for testing the lower bound of non-overflowing signed values.
    """

    def char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.CHAR_LOWER

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_LOWER

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_LOWER

    def int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.INT_LOWER

    def long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LOWER

    def long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LONG_LOWER


class ValidUpperFixture(LimitsFixture):
    """ A fixture for testing the upper bound of non-overflowing values.
    """

    def char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.CHAR_UPPER

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_UPPER

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_UPPER

    def int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.INT_UPPER

    def long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_UPPER

    def long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LONG_UPPER

    def unsigned_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_CHAR_UPPER

    def unsigned_short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_SHORT_UPPER

    def unsigned_int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_INT_UPPER

    def unsigned_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_LONG_UPPER

    def unsigned_long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_LONG_LONG_UPPER


class OverflowLowerFixture(LimitsFixture):
    """ A fixture for testing the lower bound of overflowing signed values. """

    def char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.CHAR_LOWER - 1

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_LOWER - 1

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_LOWER - 1

    def int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.INT_LOWER - 1

    def long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LOWER - 1

    def long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LONG_LOWER - 1


class OverflowUpperFixture(LimitsFixture):
    """ A fixture for testing the upper bound of overflowing values. """

    def char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.CHAR_UPPER + 1

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_UPPER + 1

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_UPPER + 1

    def int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.INT_UPPER + 1

    def long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_UPPER + 1

    def long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.LONG_LONG_UPPER + 1

    def unsigned_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_CHAR_UPPER + 1

    def unsigned_short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_SHORT_UPPER + 1

    def unsigned_int_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_INT_UPPER + 1

    def unsigned_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_LONG_UPPER + 1

    def unsigned_long_long_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.UNSIGNED_LONG_LONG_UPPER + 1


class TestScopedEnumConvertors(unittest.TestCase):
    """ This tests the scoped enum convertors with valid values. """

    def setUp(self):
        """ Set up a test. """

        self.fixture = ScopedEnumFixture()

    def tearDown(self):
        """ Tidy up after a test. """

        del self.fixture

    def test_scoped_get_member(self):
        """ scoped enum virtual result with a member value. """

        self.assertIs(self.fixture.scoped_get(), Test.Scoped.scoped)

    def test_scoped_set_member(self):
        """ scoped enum function argument with a member value. """

        self.fixture.scoped_set(Test.Scoped.scoped)

    def test_scoped_var_member(self):
        """ scoped enum instance variable with a member value. """

        self.fixture.scoped_var = Test.Scoped.scoped


class TestNamedEnumConvertors(unittest.TestCase):
    """ This tests the named enum convertors with valid values. """

    def setUp(self):
        """ Set up a test. """

        self.member_fixture = NamedEnumFixture(Test.named)
        self.int_fixture = NamedEnumFixture(0)

    def tearDown(self):
        """ Tidy up after a test. """

        del self.member_fixture
        del self.int_fixture

    def test_named_get_member(self):
        """ named enum virtual result with a member value. """

        self.assertEqual(self.member_fixture.named_get(), Test.named)

    def test_named_set_member(self):
        """ named enum function argument with a member value. """

        self.member_fixture.named_set(Test.named)

    def test_named_var_member(self):
        """ named enum instance variable with a member value. """

        self.member_fixture.named_var = Test.named

    def test_named_overload_set(self):
        """ overloaded named enum function argument. """

        self.member_fixture.named_overload_set(Test.named)
        self.assertIs(self.member_fixture.named_overload, True)

    def test_named_get_int(self):
        """ named enum virtual result with an integer value. """

        self.assertEqual(self.int_fixture.named_get(), 0)

    def test_named_set_int(self):
        """ named enum function argument with an integer value. """

        self.int_fixture.named_set(0)

    def test_named_var_int(self):
        """ named enum instance variable with an integer value. """

        self.int_fixture.named_var = 0


class TestBoolConvertors(unittest.TestCase):
    """ This tests the bool convertors with valid values. """

    def setUp(self):
        """ Set up a test. """

        self.true_fixture = BoolFixture(True)
        self.false_fixture = BoolFixture(False)
        self.nonzero_fixture = BoolFixture(-1)
        self.zero_fixture = BoolFixture(0)

    def tearDown(self):
        """ Tidy up after a test. """

        del self.true_fixture
        del self.false_fixture
        del self.nonzero_fixture
        del self.zero_fixture

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


class TestIntConvertors(unittest.TestCase):
    """ This tests the integer convertors with valid values. """

    @classmethod
    def setUpClass(cls):
        """ Set up a test case. """

        # Compute the various test values based on the native sizes.
        cls.CHAR_LOWER = Test.char_lower()
        cls.CHAR_UPPER = Test.char_upper()
        cls.SIGNED_CHAR_LOWER, cls.SIGNED_CHAR_UPPER = cls._signed_bounds(
                Test.signed_char_sizeof())
        cls.SHORT_LOWER, cls.SHORT_UPPER = cls._signed_bounds(
                Test.short_sizeof())
        cls.INT_LOWER, cls.INT_UPPER = cls._signed_bounds(Test.int_sizeof())
        cls.LONG_LOWER, cls.LONG_UPPER = cls._signed_bounds(Test.long_sizeof())
        cls.LONG_LONG_LOWER, cls.LONG_LONG_UPPER = cls._signed_bounds(
                Test.long_long_sizeof())
        cls.UNSIGNED_CHAR_UPPER = cls._unsigned_upper_bound(
                Test.unsigned_char_sizeof())
        cls.UNSIGNED_SHORT_UPPER = cls._unsigned_upper_bound(
                Test.unsigned_short_sizeof())
        cls.UNSIGNED_INT_UPPER = cls._unsigned_upper_bound(
                Test.unsigned_int_sizeof())
        cls.UNSIGNED_LONG_UPPER = cls._unsigned_upper_bound(
                Test.unsigned_long_sizeof())
        cls.UNSIGNED_LONG_LONG_UPPER = cls._unsigned_upper_bound(
                Test.unsigned_long_long_sizeof())

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


class TestInvalidValues(TestIntConvertors):
    """ This tests the integer, boolean and enum convertors with invalid
    values.
    """

    def setUp(self):
        """ Set up a test. """

        self.fixture = InvalidFixture()

    def tearDown(self):
        """ Tidy up after a test. """

        del self.fixture

    def test_scoped_get(self):
        """ scoped enum virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.scoped_get()
            uninstall_hook()

    def test_scoped_set(self):
        """ scoped enum function argument. """

        with self.assertRaises(TypeError):
            self.fixture.scoped_set(10)

    def test_scoped_var(self):
        """ scoped enum instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.scoped_var = 10

    def test_named_get(self):
        """ named enum virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.named_get()
            uninstall_hook()

    def test_named_set(self):
        """ named enum function argument. """

        with self.assertRaises(TypeError):
            self.fixture.named_set('0')

    def test_named_var(self):
        """ named enum instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.named_var = '0'

    def test_bool_get(self):
        """ bool virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.bool_get()
            uninstall_hook()

    def test_bool_set(self):
        """ bool function argument. """

        with self.assertRaises(TypeError):
            self.fixture.bool_set('0')

    def test_bool_var(self):
        """ bool instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.bool_var = '0'

    def test_char_get(self):
        """ char virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.char_get()
            uninstall_hook()

    def test_char_set(self):
        """ char function argument. """

        with self.assertRaises(TypeError):
            self.fixture.char_set('0')

    def test_char_var(self):
        """ char instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.char_var = '0'

    def test_signed_char_get(self):
        """ signed char virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.signed_char_get()
            uninstall_hook()

    def test_signed_char_set(self):
        """ signed char function argument. """

        with self.assertRaises(TypeError):
            self.fixture.signed_char_set('0')

    def test_signed_char_var(self):
        """ signed char instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.signed_char_var = '0'

    def test_short_get(self):
        """ short virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.short_get()
            uninstall_hook()

    def test_short_set(self):
        """ short function argument. """

        with self.assertRaises(TypeError):
            self.fixture.short_set('0')

    def test_short_var(self):
        """ short instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.short_var = '0'

    def test_int_get(self):
        """ int virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.int_get()
            uninstall_hook()

    def test_int_set(self):
        """ int function argument. """

        with self.assertRaises(TypeError):
            self.fixture.int_set('0')

    def test_int_var(self):
        """ int instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.int_var = '0'

    def test_long_get(self):
        """ long virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.long_get()
            uninstall_hook()

    def test_long_set(self):
        """ long function argument. """

        with self.assertRaises(TypeError):
            self.fixture.long_set('0')

    def test_long_var(self):
        """ long instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.long_var = '0'

    def test_long_long_get(self):
        """ long long virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.long_long_get()
            uninstall_hook()

    def test_long_long_set(self):
        """ long long function argument. """

        with self.assertRaises(TypeError):
            self.fixture.long_long_set('0')

    def test_long_long_var(self):
        """ long long instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.long_long_var = '0'

    def test_unsigned_char_get(self):
        """ unsigned char virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.unsigned_char_get()
            uninstall_hook()

    def test_unsigned_char_set(self):
        """ unsigned char function argument. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_char_set('0')

    def test_unsigned_char_var(self):
        """ unsigned char instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_char_var = '0'

    def test_unsigned_short_get(self):
        """ unsigned short virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.unsigned_short_get()
            uninstall_hook()

    def test_unsigned_short_set(self):
        """ unsigned short function argument. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_short_set('0')

    def test_unsigned_short_var(self):
        """ unsigned short instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_short_var = '0'

    def test_unsigned_int_get(self):
        """ unsigned int virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.unsigned_int_get()
            uninstall_hook()

    def test_unsigned_int_set(self):
        """ unsigned int function argument. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_int_set('0')

    def test_unsigned_int_var(self):
        """ unsigned int instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_int_var = '0'

    def test_unsigned_long_get(self):
        """ unsigned long virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.unsigned_long_get()
            uninstall_hook()

    def test_unsigned_long_set(self):
        """ unsigned long function argument. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_long_set('0')

    def test_unsigned_long_var(self):
        """ unsigned long instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_long_var = '0'

    def test_unsigned_long_long_get(self):
        """ unsigned long long virtual result. """

        with self.assertRaises(TypeError):
            install_hook()
            self.fixture.unsigned_long_long_get()
            uninstall_hook()

    def test_unsigned_long_long_set(self):
        """ unsigned long long function argument. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_long_long_set('0')

    def test_unsigned_long_long_var(self):
        """ unsigned long long instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.unsigned_long_long_var = '0'


class TestValidValues(TestIntConvertors):
    """ This tests the integer convertors with valid values. """

    def setUp(self):
        """ Set up a test. """

        self.lower_fixture = ValidLowerFixture(self)
        self.upper_fixture = ValidUpperFixture(self)

    def tearDown(self):
        """ Tidy up after a test. """

        del self.lower_fixture
        del self.upper_fixture

    def test_char_get_lower(self):
        """ char virtual result lower bound. """

        self.assertEqual(self.lower_fixture.char_get(), self.CHAR_LOWER)

    def test_char_get_upper(self):
        """ char virtual result upper bound. """

        self.assertEqual(self.upper_fixture.char_get(), self.CHAR_UPPER)

    def test_char_set_lower(self):
        """ char function argument lower bound. """

        self.lower_fixture.char_set(self.CHAR_LOWER)

    def test_char_set_upper(self):
        """ char function argument upper bound. """

        self.upper_fixture.char_set(self.CHAR_UPPER)

    def test_char_var_lower(self):
        """ char instance variable lower bound. """

        self.lower_fixture.char_var = self.CHAR_LOWER

    def test_char_var_upper(self):
        """ char instance variable upper bound. """

        self.upper_fixture.char_var = self.CHAR_UPPER

    def test_signed_char_get_lower(self):
        """ signed char virtual result lower bound. """

        self.assertEqual(self.lower_fixture.signed_char_get(),
                self.SIGNED_CHAR_LOWER)

    def test_signed_char_get_upper(self):
        """ signed char virtual result upper bound. """

        self.assertEqual(self.upper_fixture.signed_char_get(),
                self.SIGNED_CHAR_UPPER)

    def test_signed_char_set_lower(self):
        """ signed char function argument lower bound. """

        self.lower_fixture.signed_char_set(self.SIGNED_CHAR_LOWER)

    def test_signed_char_set_upper(self):
        """ signed char function argument upper bound. """

        self.upper_fixture.signed_char_set(self.SIGNED_CHAR_UPPER)

    def test_signed_char_var_lower(self):
        """ signed char instance variable lower bound. """

        self.lower_fixture.signed_char_var = self.SIGNED_CHAR_LOWER

    def test_signed_char_var_upper(self):
        """ signed char instance variable upper bound. """

        self.upper_fixture.signed_char_var = self.SIGNED_CHAR_UPPER

    def test_short_get_lower(self):
        """ short virtual result lower bound. """

        self.assertEqual(self.lower_fixture.short_get(), self.SHORT_LOWER)

    def test_short_get_upper(self):
        """ short virtual result upper bound. """

        self.assertEqual(self.upper_fixture.short_get(), self.SHORT_UPPER)

    def test_short_set_lower(self):
        """ short function argument lower bound. """

        self.lower_fixture.short_set(self.SHORT_LOWER)

    def test_short_set_upper(self):
        """ short function argument upper bound. """

        self.upper_fixture.short_set(self.SHORT_UPPER)

    def test_short_var_lower(self):
        """ short instance variable lower bound. """

        self.lower_fixture.short_var = self.SHORT_LOWER

    def test_short_var_upper(self):
        """ short instance variable upper bound. """

        self.upper_fixture.short_var = self.SHORT_UPPER

    def test_int_get_lower(self):
        """ int virtual result lower bound. """

        self.assertEqual(self.lower_fixture.int_get(), self.INT_LOWER)

    def test_int_get_upper(self):
        """ int virtual result upper bound. """

        self.assertEqual(self.upper_fixture.int_get(), self.INT_UPPER)

    def test_int_set_lower(self):
        """ int function argument lower bound. """

        self.lower_fixture.int_set(self.INT_LOWER)

    def test_int_set_upper(self):
        """ int function argument upper bound. """

        self.upper_fixture.int_set(self.INT_UPPER)

    def test_int_var_lower(self):
        """ int instance variable lower bound. """

        self.lower_fixture.int_var = self.INT_LOWER

    def test_int_var_upper(self):
        """ int instance variable upper bound. """

        self.upper_fixture.int_var = self.INT_UPPER

    def test_long_get_lower(self):
        """ long virtual result lower bound. """

        self.assertEqual(self.lower_fixture.long_get(), self.LONG_LOWER)

    def test_long_get_upper(self):
        """ long virtual result upper bound. """

        self.assertEqual(self.upper_fixture.long_get(), self.LONG_UPPER)

    def test_long_set_lower(self):
        """ long function argument lower bound. """

        self.lower_fixture.long_set(self.LONG_LOWER)

    def test_long_set_upper(self):
        """ long function argument upper bound. """

        self.upper_fixture.long_set(self.LONG_UPPER)

    def test_long_var_lower(self):
        """ long instance variable lower bound. """

        self.lower_fixture.long_var = self.LONG_LOWER

    def test_long_var_upper(self):
        """ long instance variable upper bound. """

        self.upper_fixture.long_var = self.LONG_UPPER

    def test_long_long_get_lower(self):
        """ long long virtual result lower bound. """

        self.assertEqual(self.lower_fixture.long_long_get(),
                self.LONG_LONG_LOWER)

    def test_long_long_get_upper(self):
        """ long long virtual result upper bound. """

        self.assertEqual(self.upper_fixture.long_long_get(),
                self.LONG_LONG_UPPER)

    def test_long_long_set_lower(self):
        """ long long function argument lower bound. """

        self.lower_fixture.long_long_set(self.LONG_LONG_LOWER)

    def test_long_long_set_upper(self):
        """ long long function argument upper bound. """

        self.upper_fixture.long_long_set(self.LONG_LONG_UPPER)

    def test_long_long_var_lower(self):
        """ long long instance variable lower bound. """

        self.lower_fixture.long_long_var = self.LONG_LONG_LOWER

    def test_long_long_var_upper(self):
        """ long long instance variable upper bound. """

        self.upper_fixture.long_long_var = self.LONG_LONG_UPPER

    def test_unsigned_char_get_upper(self):
        """ unsigned char virtual result upper bound. """

        self.assertEqual(self.upper_fixture.unsigned_char_get(),
                self.UNSIGNED_CHAR_UPPER)

    def test_unsigned_char_set_upper(self):
        """ unsigned char function argument upper bound. """

        self.upper_fixture.unsigned_char_set(self.UNSIGNED_CHAR_UPPER)

    def test_unsigned_char_var_upper(self):
        """ unsigned char instance variable upper bound. """

        self.upper_fixture.unsigned_char_var = self.UNSIGNED_CHAR_UPPER

    def test_unsigned_short_get_upper(self):
        """ unsigned short virtual result upper bound. """

        self.assertEqual(self.upper_fixture.unsigned_short_get(),
                self.UNSIGNED_SHORT_UPPER)

    def test_unsigned_short_set_upper(self):
        """ unsigned short function argument upper bound. """

        self.upper_fixture.unsigned_short_set(self.UNSIGNED_SHORT_UPPER)

    def test_unsigned_short_var_upper(self):
        """ unsigned short instance variable upper bound. """

        self.upper_fixture.unsigned_short_var = self.UNSIGNED_SHORT_UPPER

    def test_unsigned_int_get_upper(self):
        """ unsigned int virtual result upper bound. """

        self.assertEqual(self.upper_fixture.unsigned_int_get(),
                self.UNSIGNED_INT_UPPER)

    def test_unsigned_int_set_upper(self):
        """ unsigned int function argument upper bound. """

        self.upper_fixture.unsigned_int_set(self.UNSIGNED_INT_UPPER)

    def test_unsigned_int_var_upper(self):
        """ unsigned int instance variable upper bound. """

        self.upper_fixture.unsigned_int_var = self.UNSIGNED_INT_UPPER

    def test_unsigned_long_get_upper(self):
        """ unsigned long virtual result upper bound. """

        self.assertEqual(self.upper_fixture.unsigned_long_get(),
                self.UNSIGNED_LONG_UPPER)

    def test_unsigned_long_set_upper(self):
        """ unsigned long function argument upper bound. """

        self.upper_fixture.unsigned_long_set(self.UNSIGNED_LONG_UPPER)

    def test_unsigned_long_var_upper(self):
        """ unsigned long instance variable upper bound. """

        self.upper_fixture.unsigned_long_var = self.UNSIGNED_LONG_UPPER

    def test_unsigned_long_long_get_upper(self):
        """ unsigned long long virtual result upper bound. """

        self.assertEqual(self.upper_fixture.unsigned_long_long_get(),
                self.UNSIGNED_LONG_LONG_UPPER)

    def test_unsigned_long_long_set_upper(self):
        """ unsigned long long function argument upper bound. """

        self.upper_fixture.unsigned_long_long_set(
                self.UNSIGNED_LONG_LONG_UPPER)

    def test_unsigned_long_long_var_upper(self):
        """ unsigned long long instance variable upper bound. """

        self.upper_fixture.unsigned_long_long_var = self.UNSIGNED_LONG_LONG_UPPER


class TestNoOverflowChecking(TestIntConvertors):
    """ This tests the integer convertors with overflowing values with overflow
    checking disabled.
    """

    @staticmethod
    def _long_long_is_long():
        """ Return True if (unsigned) long long is the same size as (unsigned)
        long.
        """

        return Test.long_long_sizeof() == Test.long_sizeof()

    def setUp(self):
        """ Set up a test. """

        self.lower_fixture = OverflowLowerFixture(self)
        self.upper_fixture = OverflowUpperFixture(self)

        self.was_enabled = enableoverflowchecking(False)

    def tearDown(self):
        """ Tidy up after a test. """

        enableoverflowchecking(self.was_enabled)

        del self.lower_fixture
        del self.upper_fixture

    def test_char_get_lower(self):
        """ char virtual result lower bound. """

        install_hook()
        self.lower_fixture.char_get()
        uninstall_hook()

    def test_char_get_upper(self):
        """ char virtual result upper bound. """

        install_hook()
        self.upper_fixture.char_get()
        uninstall_hook()

    def test_char_set_lower(self):
        """ char function argument lower bound. """

        self.lower_fixture.char_set(self.CHAR_LOWER - 1)

    def test_char_set_upper(self):
        """ char function argument upper bound. """

        self.upper_fixture.char_set(self.CHAR_UPPER + 1)

    def test_char_var_lower(self):
        """ char instance variable lower bound. """

        self.lower_fixture.char_var = self.CHAR_LOWER - 1

    def test_char_var_upper(self):
        """ char instance variable upper bound. """

        self.upper_fixture.char_var = self.CHAR_UPPER + 1

    def test_signed_char_get_lower(self):
        """ signed char virtual result lower bound. """

        install_hook()
        self.lower_fixture.signed_char_get()
        uninstall_hook()

    def test_signed_char_get_upper(self):
        """ signed char virtual result upper bound. """

        install_hook()
        self.upper_fixture.signed_char_get()
        uninstall_hook()

    def test_signed_char_set_lower(self):
        """ signed char function argument lower bound. """

        self.lower_fixture.signed_char_set(self.SIGNED_CHAR_LOWER - 1)

    def test_signed_char_set_upper(self):
        """ signed char function argument upper bound. """

        self.upper_fixture.signed_char_set(self.SIGNED_CHAR_UPPER + 1)

    def test_signed_char_var_lower(self):
        """ signed char instance variable lower bound. """

        self.lower_fixture.signed_char_var = self.SIGNED_CHAR_LOWER - 1

    def test_signed_char_var_upper(self):
        """ signed char instance variable upper bound. """

        self.upper_fixture.signed_char_var = self.SIGNED_CHAR_UPPER + 1

    def test_short_get_lower(self):
        """ short virtual result lower bound. """

        install_hook()
        self.lower_fixture.short_get()
        uninstall_hook()

    def test_short_get_upper(self):
        """ short virtual result upper bound. """

        install_hook()
        self.upper_fixture.short_get()
        uninstall_hook()

    def test_short_set_lower(self):
        """ short function argument lower bound. """

        self.lower_fixture.short_set(self.SHORT_LOWER - 1)

    def test_short_set_upper(self):
        """ short function argument upper bound. """

        self.upper_fixture.short_set(self.SHORT_UPPER + 1)

    def test_short_var_lower(self):
        """ short instance variable lower bound. """

        self.lower_fixture.short_var = self.SHORT_LOWER - 1

    def test_short_var_upper(self):
        """ short instance variable upper bound. """

        self.upper_fixture.short_var = self.SHORT_UPPER + 1

    def test_int_get_lower(self):
        """ int virtual result lower bound. """

        install_hook()
        self.lower_fixture.int_get()
        uninstall_hook()

    def test_int_get_upper(self):
        """ int virtual result upper bound. """

        install_hook()
        self.upper_fixture.int_get()
        uninstall_hook()

    def test_int_set_lower(self):
        """ int function argument lower bound. """

        self.lower_fixture.int_set(self.INT_LOWER - 1)

    def test_int_set_upper(self):
        """ int function argument upper bound. """

        self.upper_fixture.int_set(self.INT_UPPER + 1)

    def test_int_var_lower(self):
        """ int instance variable lower bound. """

        self.lower_fixture.int_var = self.INT_LOWER - 1

    def test_int_var_upper(self):
        """ int instance variable upper bound. """

        self.upper_fixture.int_var = self.INT_UPPER + 1

    def test_long_get_lower(self):
        """ long virtual result lower bound. """

        install_hook()
        self.lower_fixture.long_get()

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                uninstall_hook()
        else:
            uninstall_hook()

    def test_long_get_upper(self):
        """ long virtual result upper bound. """

        install_hook()
        self.upper_fixture.long_get()

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                uninstall_hook()
        else:
            uninstall_hook()

    def test_long_set_lower(self):
        """ long function argument lower bound. """

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                self.lower_fixture.long_set(self.LONG_LOWER - 1)
        else:
            self.lower_fixture.long_set(self.LONG_LOWER - 1)

    def test_long_set_upper(self):
        """ long function argument upper bound. """

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                self.upper_fixture.long_set(self.LONG_UPPER + 1)
        else:
            self.upper_fixture.long_set(self.LONG_UPPER + 1)

    def test_long_var_lower(self):
        """ long instance variable lower bound. """

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                self.lower_fixture.long_var = self.LONG_LOWER - 1
        else:
            self.lower_fixture.long_var = self.LONG_LOWER - 1

    def test_long_var_upper(self):
        """ long instance variable upper bound. """

        if self._long_long_is_long():
            # To maintain compatibility with older versions of SIP this
            # overflows even with overflow checking disabled.
            with self.assertRaises(OverflowError):
                self.upper_fixture.long_var = self.LONG_UPPER + 1
        else:
            self.upper_fixture.long_var = self.LONG_UPPER + 1

    def test_long_long_get_lower(self):
        """ long long virtual result lower bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            install_hook()
            self.lower_fixture.long_long_get()
            uninstall_hook()

    def test_long_long_get_upper(self):
        """ long long virtual result upper bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            install_hook()
            self.upper_fixture.long_long_get()
            uninstall_hook()

    def test_long_long_set_lower(self):
        """ long long function argument lower bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            self.lower_fixture.long_long_set(self.LONG_LONG_LOWER - 1)

    def test_long_long_set_upper(self):
        """ long long function argument upper bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            self.upper_fixture.long_long_set(self.LONG_LONG_UPPER + 1)

    def test_long_long_var_lower(self):
        """ long long instance variable lower bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            self.lower_fixture.long_long_var = self.LONG_LONG_LOWER - 1

    def test_long_long_var_upper(self):
        """ long long instance variable upper bound. """

        # To maintain compatibility with older versions of SIP this overflows
        # even with overflow checking disabled.
        with self.assertRaises(OverflowError):
            self.upper_fixture.long_long_var = self.LONG_LONG_UPPER + 1

    def test_unsigned_char_get_upper(self):
        """ unsigned char virtual result upper bound. """

        install_hook()
        self.upper_fixture.unsigned_char_get()
        uninstall_hook()

    def test_unsigned_char_set_upper(self):
        """ unsigned char function argument upper bound. """

        self.upper_fixture.unsigned_char_set(self.UNSIGNED_CHAR_UPPER + 1)

    def test_unsigned_char_var_upper(self):
        """ unsigned char instance variable upper bound. """

        self.upper_fixture.unsigned_char_var = self.UNSIGNED_CHAR_UPPER + 1

    def test_unsigned_short_get_upper(self):
        """ unsigned short virtual result upper bound. """

        install_hook()
        self.upper_fixture.unsigned_short_get()
        uninstall_hook()

    def test_unsigned_short_set_upper(self):
        """ unsigned short function argument upper bound. """

        self.upper_fixture.unsigned_short_set(self.UNSIGNED_SHORT_UPPER + 1)

    def test_unsigned_short_var_upper(self):
        """ unsigned short instance variable upper bound. """

        self.upper_fixture.unsigned_short_var = self.UNSIGNED_SHORT_UPPER + 1

    def test_unsigned_int_get_upper(self):
        """ unsigned int virtual result upper bound. """

        install_hook()
        self.upper_fixture.unsigned_int_get()
        uninstall_hook()

    def test_unsigned_int_set_upper(self):
        """ unsigned int function argument upper bound. """

        self.upper_fixture.unsigned_int_set(self.UNSIGNED_INT_UPPER + 1)

    def test_unsigned_int_var_upper(self):
        """ unsigned int instance variable upper bound. """

        self.upper_fixture.unsigned_int_var = self.UNSIGNED_INT_UPPER + 1

    def test_unsigned_long_get_upper(self):
        """ unsigned long virtual result upper bound. """

        install_hook()
        self.upper_fixture.unsigned_long_get()
        uninstall_hook()

    def test_unsigned_long_set_upper(self):
        """ unsigned long function argument upper bound. """

        self.upper_fixture.unsigned_long_set(self.UNSIGNED_LONG_UPPER + 1)

    def test_unsigned_long_var_upper(self):
        """ unsigned long instance variable upper bound. """

        self.upper_fixture.unsigned_long_var = self.UNSIGNED_LONG_UPPER + 1

    def test_unsigned_long_long_get_upper(self):
        """ unsigned long long virtual result upper bound. """

        install_hook()
        self.upper_fixture.unsigned_long_long_get()
        uninstall_hook()

    def test_unsigned_long_long_set_upper(self):
        """ unsigned long long function argument upper bound. """

        self.upper_fixture.unsigned_long_long_set(
                self.UNSIGNED_LONG_LONG_UPPER + 1)

    def test_unsigned_long_long_var_upper(self):
        """ unsigned long long instance variable upper bound. """

        self.upper_fixture.unsigned_long_long_var = self.UNSIGNED_LONG_LONG_UPPER + 1


class TestOverflowChecking(TestNoOverflowChecking):
    """ This tests the integer convertors with overflowing values with overflow
    checking enabled.
    """

    def setUp(self):
        """ Set up a test. """

        super().setUp()

        enableoverflowchecking(True)

    def test_char_get_lower(self):
        """ char virtual result lower bound. """

        with self.assertRaises(OverflowError):
            super().test_char_get_lower()

    def test_char_get_upper(self):
        """ char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_char_get_upper()

    def test_char_set_lower(self):
        """ char function argument lower bound. """

        with self.assertRaises(OverflowError):
            super().test_char_set_lower()

    def test_char_set_upper(self):
        """ char function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_char_set_upper()

    def test_char_var_lower(self):
        """ char instance variable lower bound. """

        with self.assertRaises(OverflowError):
            super().test_char_var_lower()

    def test_char_var_upper(self):
        """ char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_char_var_upper()

    def test_signed_char_get_lower(self):
        """ signed char virtual result lower bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_get_lower()

    def test_signed_char_get_upper(self):
        """ signed char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_get_upper()

    def test_signed_char_set_lower(self):
        """ signed char function argument lower bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_set_lower()

    def test_signed_char_set_upper(self):
        """ signed char function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_set_upper()

    def test_signed_char_var_lower(self):
        """ signed char instance variable lower bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_var_lower()

    def test_signed_char_var_upper(self):
        """ signed char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_signed_char_var_upper()

    def test_short_get_lower(self):
        """ short virtual result lower bound. """

        with self.assertRaises(OverflowError):
            super().test_short_get_lower()

    def test_short_get_upper(self):
        """ short virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_short_get_upper()

    def test_short_set_lower(self):
        """ short function argument lower bound. """

        with self.assertRaises(OverflowError):
            super().test_short_set_lower()

    def test_short_set_upper(self):
        """ short function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_short_set_upper()

    def test_short_var_lower(self):
        """ short instance variable lower bound. """

        with self.assertRaises(OverflowError):
            super().test_short_var_lower()

    def test_short_var_upper(self):
        """ short instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_short_var_upper()

    def test_int_get_lower(self):
        """ int virtual result lower bound. """

        with self.assertRaises(OverflowError):
            super().test_int_get_lower()

    def test_int_get_upper(self):
        """ int virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_int_get_upper()

    def test_int_set_lower(self):
        """ int function argument lower bound. """

        with self.assertRaises(OverflowError):
            super().test_int_set_lower()

    def test_int_set_upper(self):
        """ int function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_int_set_upper()

    def test_int_var_lower(self):
        """ int instance variable lower bound. """

        with self.assertRaises(OverflowError):
            super().test_int_var_lower()

    def test_int_var_upper(self):
        """ int instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_int_var_upper()

    def test_long_get_lower(self):
        """ long virtual result lower bound. """

        with self.assertRaises(OverflowError):
            install_hook()
            self.lower_fixture.long_get()
            uninstall_hook()

    def test_long_get_upper(self):
        """ long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            install_hook()
            self.upper_fixture.long_get()
            uninstall_hook()

    def test_long_set_lower(self):
        """ long function argument lower bound. """

        with self.assertRaises(OverflowError):
            self.lower_fixture.long_set(self.LONG_LOWER - 1)

    def test_long_set_upper(self):
        """ long function argument upper bound. """

        with self.assertRaises(OverflowError):
            self.upper_fixture.long_set(self.LONG_UPPER + 1)

    def test_long_var_lower(self):
        """ long instance variable lower bound. """

        with self.assertRaises(OverflowError):
            self.lower_fixture.long_var = self.LONG_LOWER - 1

    def test_long_var_upper(self):
        """ long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            self.upper_fixture.long_var = self.LONG_UPPER + 1

    def test_long_long_get_lower(self):
        """ long long virtual result lower bound. """

        super().test_long_long_get_lower()

    def test_long_long_get_upper(self):
        """ long long virtual result upper bound. """

        super().test_long_long_get_upper()

    def test_long_long_set_lower(self):
        """ long long function argument lower bound. """

        super().test_long_long_set_lower()

    def test_long_long_set_upper(self):
        """ long long function argument upper bound. """

        super().test_long_long_set_upper()

    def test_long_long_var_lower(self):
        """ long long instance variable lower bound. """

        super().test_long_long_var_lower()

    def test_long_long_var_upper(self):
        """ long long instance variable upper bound. """

        super().test_long_long_var_upper()

    def test_unsigned_char_get_upper(self):
        """ unsigned char virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_char_get_upper()

    def test_unsigned_char_set_upper(self):
        """ unsigned char function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_char_set_upper()

    def test_unsigned_char_var_upper(self):
        """ unsigned char instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_char_var_upper()

    def test_unsigned_short_get_upper(self):
        """ unsigned short virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_short_get_upper()

    def test_unsigned_short_set_upper(self):
        """ unsigned short function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_short_set_upper()

    def test_unsigned_short_var_upper(self):
        """ unsigned short instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_short_var_upper()

    def test_unsigned_int_get_upper(self):
        """ unsigned int virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_int_get_upper()

    def test_unsigned_int_set_upper(self):
        """ unsigned int function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_int_set_upper()

    def test_unsigned_int_var_upper(self):
        """ unsigned int instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_int_var_upper()

    def test_unsigned_long_get_upper(self):
        """ unsigned long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_get_upper()

    def test_unsigned_long_set_upper(self):
        """ unsigned long function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_set_upper()

    def test_unsigned_long_var_upper(self):
        """ unsigned long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_var_upper()

    def test_unsigned_long_long_get_upper(self):
        """ unsigned long long virtual result upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_long_get_upper()

    def test_unsigned_long_long_set_upper(self):
        """ unsigned long long function argument upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_long_set_upper()

    def test_unsigned_long_long_var_upper(self):
        """ unsigned long long instance variable upper bound. """

        with self.assertRaises(OverflowError):
            super().test_unsigned_long_long_var_upper()


if __name__ == '__main__':
    unittest.main()
