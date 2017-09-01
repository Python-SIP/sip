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


class BaseFixture(Test):
    """ The base test fixture. """

    def __init__(self, limits):
        """ Initialise the object. """

        super().__init__()

        self.limits = limits


class ValidLowerFixture(BaseFixture):
    """ A fixture for testing the lower bound of non-overflowing signed values.
    """

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_LOWER

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_LOWER


class ValidUpperFixture(BaseFixture):
    """ A fixture for testing the upper bound of non-overflowing signed values.
    """

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_UPPER

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_UPPER


class OverflowLowerFixture(BaseFixture):
    """ A fixture for testing the lower bound of overflowing signed values. """

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_LOWER - 1

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_LOWER - 1


class OverflowUpperFixture(BaseFixture):
    """ A fixture for testing the upper bound of overflowing signed values. """

    def signed_char_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SIGNED_CHAR_UPPER + 1

    def short_virt(self):
        """ Re-implemented to return the fixture-specific value. """

        return self.limits.SHORT_UPPER + 1


class TestIntConvertors(unittest.TestCase):
    """ This tests the integer and enum convertors with valid values. """

    @classmethod
    def setUpClass(cls):
        """ Set up a test case. """

        # Compute the various test values based on the native sizes.
        cls.SIGNED_CHAR_LOWER, cls.SIGNED_CHAR_UPPER = cls._signed_bounds(
                Test.signed_char_sizeof())
        cls.SHORT_LOWER, cls.SHORT_UPPER = cls._signed_bounds(
                Test.short_sizeof())

    @staticmethod
    def _signed_bounds(nrbytes):
        """ Return the values range of values for a number of bytes
        representing a signed value.
        """

        v = 1 << ((nrbytes * 8) - 1)

        return -v, v - 1


class TestInvalidValues(TestIntConvertors):
    """ This tests the integer and enum convertors with invalid values. """

    def setUp(self):
        """ Set up a test. """

        self.fixture = Test()

    def tearDown(self):
        """ Tidy up after a test. """

        del self.fixture

    def test_signed_char_set(self):
        """ signed char function argument. """

        with self.assertRaises(TypeError):
            self.fixture.signed_char_set('0')

    def test_signed_char_var(self):
        """ signed char instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.signed_char_var = '0'

    def test_short_set(self):
        """ short function argument. """

        with self.assertRaises(TypeError):
            self.fixture.short_set('0')

    def test_short_var(self):
        """ short instance variable. """

        with self.assertRaises(TypeError):
            self.fixture.short_var = '0'


class TestValidValues(TestIntConvertors):
    """ This tests the integer and enum convertors with valid values. """

    def setUp(self):
        """ Set up a test. """

        self.lower_fixture = ValidLowerFixture(self)
        self.upper_fixture = ValidUpperFixture(self)

    def tearDown(self):
        """ Tidy up after a test. """

        del self.lower_fixture
        del self.upper_fixture

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


class TestNoOverflowChecking(TestIntConvertors):
    """ This tests the integer and enum convertors with overflowing values with
    overflow checking disabled.
    """

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


class TestOverflowChecking(TestNoOverflowChecking):
    """ This tests the integer and enum convertors with overflowing values with
    overflow checking enabled.
    """

    def setUp(self):
        """ Set up a test. """

        super().setUp()

        enableoverflowchecking(True)

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


if __name__ == '__main__':
    unittest.main()
