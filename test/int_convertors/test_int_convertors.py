# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import pytest


@pytest.fixture(scope='module')
def bool_value(module):
    """ A fixture for creating a test sub-class of IntConvertors that
    reimplements bool_virt().
    """

    class BoolValue(module.IntConvertors):
        def __init__(self, value):
            super().__init__()
            self._value = value

        def bool_virt(self):
            return self._value

    return BoolValue


@pytest.fixture(scope='module')
def bool_true_value(bool_value):
    """ A fixture for testing True. """

    return bool_value(True)


@pytest.fixture(scope='module')
def bool_false_value(bool_value):
    """ A fixture for testing False. """

    return bool_value(False)


@pytest.fixture(scope='module')
def bool_nonzero_value(bool_value):
    """ A fixture for testing a non-zero value as a bool. """

    return bool_value(-1)


@pytest.fixture(scope='module')
def bool_zero_value(bool_value):
    """ A fixture for testing a zero value as a bool. """

    return bool_value(0)


@pytest.fixture(scope='module')
def invalid_values(module):
    """ A fixture for testing invalid values. """

    class InvalidValues(module.IntConvertors):
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

    return InvalidValues()


@pytest.fixture(scope='module')
def limits(module):
    """ A fixture defining upper and lower limits on the values of various
    integer types.
    """

    class Limits: pass

    l = Limits()

    # Compute the various values based on the native sizes.
    l.CHAR_LOWER = module.IntConvertors.char_lower()
    l.CHAR_UPPER = module.IntConvertors.char_upper()
    l.SIGNED_CHAR_LOWER, l.SIGNED_CHAR_UPPER = _signed_bounds(
            module.IntConvertors.signed_char_sizeof())
    l.SHORT_LOWER, l.SHORT_UPPER = _signed_bounds(
            module.IntConvertors.short_sizeof())
    l.INT_LOWER, l.INT_UPPER = _signed_bounds(
            module.IntConvertors.int_sizeof())
    l.LONG_LOWER, l.LONG_UPPER = _signed_bounds(
            module.IntConvertors.long_sizeof())
    l.LONG_LONG_LOWER, l.LONG_LONG_UPPER = _signed_bounds(
            module.IntConvertors.long_long_sizeof())
    l.UNSIGNED_CHAR_UPPER = _unsigned_upper_bound(
            module.IntConvertors.unsigned_char_sizeof())
    l.UNSIGNED_SHORT_UPPER = _unsigned_upper_bound(
            module.IntConvertors.unsigned_short_sizeof())
    l.UNSIGNED_INT_UPPER = _unsigned_upper_bound(
            module.IntConvertors.unsigned_int_sizeof())
    l.UNSIGNED_LONG_UPPER = _unsigned_upper_bound(
            module.IntConvertors.unsigned_long_sizeof())
    l.UNSIGNED_LONG_LONG_UPPER = _unsigned_upper_bound(
            module.IntConvertors.unsigned_long_long_sizeof())

    return l


@pytest.fixture(scope='module')
def overflow_lower_bounds(module, limits):
    """ A fixture for testing the lower bound of overflowing signed values. """

    class OverflowLowerBounds(module.IntConvertors):
        def char_virt(self):
            return limits.CHAR_LOWER - 1

        def signed_char_virt(self):
            return limits.SIGNED_CHAR_LOWER - 1

        def short_virt(self):
            return limits.SHORT_LOWER - 1

        def int_virt(self):
            return limits.INT_LOWER - 1

        def long_virt(self):
            return limits.LONG_LOWER - 1

        def long_long_virt(self):
            return limits.LONG_LONG_LOWER - 1

    return OverflowLowerBounds()


@pytest.fixture(scope='module')
def overflow_upper_bounds(module, limits):
    """ A fixture for testing the upper bound of overflowing signed values. """

    class OverflowUpperBounds(module.IntConvertors):
        def char_virt(self):
            return limits.CHAR_UPPER + 1

        def signed_char_virt(self):
            return limits.SIGNED_CHAR_UPPER + 1

        def short_virt(self):
            return limits.SHORT_UPPER + 1

        def int_virt(self):
            return limits.INT_UPPER + 1

        def long_virt(self):
            return limits.LONG_UPPER + 1

        def long_long_virt(self):
            return limits.LONG_LONG_UPPER + 1

        def unsigned_char_virt(self):
            return limits.UNSIGNED_CHAR_UPPER + 1

        def unsigned_short_virt(self):
            return limits.UNSIGNED_SHORT_UPPER + 1

        def unsigned_int_virt(self):
            return limits.UNSIGNED_INT_UPPER + 1

        def unsigned_long_virt(self):
            return limits.UNSIGNED_LONG_UPPER + 1

        def unsigned_long_long_virt(self):
            return limits.UNSIGNED_LONG_LONG_UPPER + 1

    return OverflowUpperBounds()


@pytest.fixture(scope='module')
def valid_lower_bounds(module, limits):
    """ A fixture for testing the lower bound of non-overflowing signed values.
    """

    class ValidLowerBounds(module.IntConvertors):
        def char_virt(self):
            return limits.CHAR_LOWER

        def signed_char_virt(self):
            return limits.SIGNED_CHAR_LOWER

        def short_virt(self):
            return limits.SHORT_LOWER

        def int_virt(self):
            return limits.INT_LOWER

        def long_virt(self):
            return limits.LONG_LOWER

        def long_long_virt(self):
            return limits.LONG_LONG_LOWER

    return ValidLowerBounds()


@pytest.fixture(scope='module')
def valid_upper_bounds(module, limits):
    """ A fixture for testing the upper bound of non-overflowing signed values.
    """

    class ValidUpperBounds(module.IntConvertors):
        def char_virt(self):
            return limits.CHAR_UPPER

        def signed_char_virt(self):
            return limits.SIGNED_CHAR_UPPER

        def short_virt(self):
            return limits.SHORT_UPPER

        def int_virt(self):
            return limits.INT_UPPER

        def long_virt(self):
            return limits.LONG_UPPER

        def long_long_virt(self):
            return limits.LONG_LONG_UPPER

        def unsigned_char_virt(self):
            return limits.UNSIGNED_CHAR_UPPER

        def unsigned_short_virt(self):
            return limits.UNSIGNED_SHORT_UPPER

        def unsigned_int_virt(self):
            return limits.UNSIGNED_INT_UPPER

        def unsigned_long_virt(self):
            return limits.UNSIGNED_LONG_UPPER

        def unsigned_long_long_virt(self):
            return limits.UNSIGNED_LONG_LONG_UPPER

    return ValidUpperBounds()


# The following test for valid values.

def test_char_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.char_get() == limits.CHAR_LOWER

def test_char_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.char_get() == limits.CHAR_UPPER

def test_char_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.char_set(limits.CHAR_LOWER)

def test_char_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.char_set(limits.CHAR_UPPER)

def test_char_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.char_var = limits.CHAR_LOWER

def test_char_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.char_var = limits.CHAR_UPPER

def test_signed_char_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.signed_char_get() == limits.SIGNED_CHAR_LOWER

def test_signed_char_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.signed_char_get() == limits.SIGNED_CHAR_UPPER

def test_signed_char_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.signed_char_set(limits.SIGNED_CHAR_LOWER)

def test_signed_char_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.signed_char_set(limits.SIGNED_CHAR_UPPER)

def test_signed_char_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.signed_char_var = limits.SIGNED_CHAR_LOWER

def test_signed_char_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.signed_char_var = limits.SIGNED_CHAR_UPPER

def test_short_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.short_get() == limits.SHORT_LOWER

def test_short_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.short_get() == limits.SHORT_UPPER

def test_short_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.short_set(limits.SHORT_LOWER)

def test_short_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.short_set(limits.SHORT_UPPER)

def test_short_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.short_var = limits.SHORT_LOWER

def test_short_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.short_var = limits.SHORT_UPPER

def test_int_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.int_get() == limits.INT_LOWER

def test_int_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.int_get() == limits.INT_UPPER

def test_int_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.int_set(limits.INT_LOWER)

def test_int_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.int_set(limits.INT_UPPER)

def test_int_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.int_var = limits.INT_LOWER

def test_int_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.int_var = limits.INT_UPPER

def test_long_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.long_get() == limits.LONG_LOWER

def test_long_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.long_get() == limits.LONG_UPPER

def test_long_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.long_set(limits.LONG_LOWER)

def test_long_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.long_set(limits.LONG_UPPER)

def test_long_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.long_var = limits.LONG_LOWER

def test_long_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.long_var = limits.LONG_UPPER

def test_long_long_get_lower_valid(valid_lower_bounds, limits):
    assert valid_lower_bounds.long_long_get() == limits.LONG_LONG_LOWER

def test_long_long_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.long_long_get() == limits.LONG_LONG_UPPER

def test_long_long_set_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.long_long_set(limits.LONG_LONG_LOWER)

def test_long_long_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.long_long_set(limits.LONG_LONG_UPPER)

def test_long_long_var_lower_valid(valid_lower_bounds, limits):
    valid_lower_bounds.long_long_var = limits.LONG_LONG_LOWER

def test_long_long_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.long_long_var = limits.LONG_LONG_UPPER

def test_unsigned_char_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.unsigned_char_get() == limits.UNSIGNED_CHAR_UPPER

def test_unsigned_char_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_char_set(limits.UNSIGNED_CHAR_UPPER)

def test_unsigned_char_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_char_var = limits.UNSIGNED_CHAR_UPPER

def test_unsigned_short_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.unsigned_short_get() == limits.UNSIGNED_SHORT_UPPER

def test_unsigned_short_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_short_set(limits.UNSIGNED_SHORT_UPPER)

def test_unsigned_short_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_short_var = limits.UNSIGNED_SHORT_UPPER

def test_unsigned_int_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.unsigned_int_get() == limits.UNSIGNED_INT_UPPER

def test_unsigned_int_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_int_set(limits.UNSIGNED_INT_UPPER)

def test_unsigned_int_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_int_var = limits.UNSIGNED_INT_UPPER

def test_unsigned_long_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.unsigned_long_get() == limits.UNSIGNED_LONG_UPPER

def test_unsigned_long_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_long_set(limits.UNSIGNED_LONG_UPPER)

def test_unsigned_long_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_long_var = limits.UNSIGNED_LONG_UPPER

def test_unsigned_long_long_get_upper_valid(valid_upper_bounds, limits):
    assert valid_upper_bounds.unsigned_long_long_get() == limits.UNSIGNED_LONG_LONG_UPPER

def test_unsigned_long_long_set_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_long_long_set(limits.UNSIGNED_LONG_LONG_UPPER)

def test_unsigned_long_long_var_upper_valid(valid_upper_bounds, limits):
    valid_upper_bounds.unsigned_long_long_var = limits.UNSIGNED_LONG_LONG_UPPER


# The following test for invalid values.

def test_bool_get_invalid(invalid_values, virtual_hook):
    invalid_values.bool_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_bool_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.bool_set('0')

def test_bool_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.bool_var = '0'

def test_char_get_invalid(invalid_values, virtual_hook):
    invalid_values.char_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_char_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.char_set('0')

def test_char_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.char_var = '0'

def test_signed_char_get_invalid(invalid_values, virtual_hook):
    invalid_values.signed_char_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_signed_char_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.signed_char_set('0')

def test_signed_char_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.signed_char_var = '0'

def test_short_get_invalid(invalid_values, virtual_hook):
    invalid_values.short_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_short_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.short_set('0')

def test_short_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.short_var = '0'

def test_int_get_invalid(invalid_values, virtual_hook):
    invalid_values.int_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_int_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.int_set('0')

def test_int_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.int_var = '0'

def test_long_get_invalid(invalid_values, virtual_hook):
    invalid_values.long_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_long_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.long_set('0')

def test_long_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.long_var = '0'

def test_long_long_get_invalid(invalid_values, virtual_hook):
    invalid_values.long_long_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_long_long_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.long_long_set('0')

def test_long_long_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.long_long_var = '0'

def test_unsigned_char_get_invalid(invalid_values, virtual_hook):
    invalid_values.unsigned_char_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_unsigned_char_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_char_set('0')

def test_unsigned_char_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_char_var = '0'

def test_unsigned_short_get_invalid(invalid_values, virtual_hook):
    invalid_values.unsigned_short_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_unsigned_short_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_short_set('0')

def test_unsigned_short_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_short_var = '0'

def test_unsigned_int_get_invalid(invalid_values, virtual_hook):
    invalid_values.unsigned_int_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_unsigned_int_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_int_set('0')

def test_unsigned_int_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_int_var = '0'

def test_unsigned_long_get_invalid(invalid_values, virtual_hook):
    invalid_values.unsigned_long_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_unsigned_long_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_long_set('0')

def test_unsigned_long_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_long_var = '0'

def test_unsigned_long_long_get_invalid(invalid_values, virtual_hook):
    invalid_values.unsigned_long_long_get()

    with pytest.raises(TypeError):
        virtual_hook.reraise()

def test_unsigned_long_long_set_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_long_long_set('0')

def test_unsigned_long_long_var_invalid(invalid_values):
    with pytest.raises(TypeError):
        invalid_values.unsigned_long_long_var = '0'


# The following test for under/overlowing values.

def test_char_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.char_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_char_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.char_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_char_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.char_set(limits.CHAR_LOWER - 1)

def test_char_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.char_set(limits.CHAR_UPPER + 1)

def test_char_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.char_var = limits.CHAR_LOWER - 1

def test_char_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.char_var = limits.CHAR_UPPER + 1

def test_signed_char_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.signed_char_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_signed_char_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.signed_char_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_signed_char_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.signed_char_set(limits.SIGNED_CHAR_LOWER - 1)

def test_signed_char_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.signed_char_set(limits.SIGNED_CHAR_UPPER + 1)

def test_signed_char_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.signed_char_var = limits.SIGNED_CHAR_LOWER - 1

def test_signed_char_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.signed_char_var = limits.SIGNED_CHAR_UPPER + 1

def test_short_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.short_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_short_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.short_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_short_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.short_set(limits.SHORT_LOWER - 1)

def test_short_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.short_set(limits.SHORT_UPPER + 1)

def test_short_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.short_var = limits.SHORT_LOWER - 1

def test_short_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.short_var = limits.SHORT_UPPER + 1

def test_int_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.int_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_int_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.int_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_int_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.int_set(limits.INT_LOWER - 1)

def test_int_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.int_set(limits.INT_UPPER + 1)

def test_int_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.int_var = limits.INT_LOWER - 1

def test_int_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.int_var = limits.INT_UPPER + 1

def test_long_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_long_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_long_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.long_set(limits.LONG_LOWER - 1)

def test_long_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.long_set(limits.LONG_UPPER + 1)

def test_long_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.long_var = limits.LONG_LOWER - 1

def test_long_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.long_var = limits.LONG_UPPER + 1

def test_long_long_get_lower_overflow(overflow_lower_bounds, virtual_hook):
    overflow_lower_bounds.long_long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_long_long_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.long_long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_long_long_set_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.long_long_set(limits.LONG_LONG_LOWER - 1)

def test_long_long_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.long_long_set(limits.LONG_LONG_UPPER + 1)

def test_long_long_var_lower_overflow(overflow_lower_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_lower_bounds.long_long_var = limits.LONG_LONG_LOWER - 1

def test_long_long_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.long_long_var = limits.LONG_LONG_UPPER + 1

def test_unsigned_char_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.unsigned_char_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_unsigned_char_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_char_set(limits.UNSIGNED_CHAR_UPPER + 1)

def test_unsigned_char_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_char_var = limits.UNSIGNED_CHAR_UPPER + 1

def test_unsigned_short_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.unsigned_short_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_unsigned_short_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_short_set(
                limits.UNSIGNED_SHORT_UPPER + 1)

def test_unsigned_short_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_short_var = limits.UNSIGNED_SHORT_UPPER + 1

def test_unsigned_int_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.unsigned_int_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_unsigned_int_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_int_set(limits.UNSIGNED_INT_UPPER + 1)

def test_unsigned_int_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_int_var = limits.UNSIGNED_INT_UPPER + 1

def test_unsigned_long_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.unsigned_long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_unsigned_long_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_long_set(limits.UNSIGNED_LONG_UPPER + 1)

def test_unsigned_long_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_long_var = limits.UNSIGNED_LONG_UPPER + 1

def test_unsigned_long_long_get_upper_overflow(overflow_upper_bounds, virtual_hook):
    overflow_upper_bounds.unsigned_long_long_get()

    with pytest.raises(OverflowError):
        virtual_hook.reraise()

def test_unsigned_long_long_set_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_long_long_set(
                limits.UNSIGNED_LONG_LONG_UPPER + 1)

def test_unsigned_long_long_var_upper_overflow(overflow_upper_bounds, limits):
    with pytest.raises(OverflowError):
        overflow_upper_bounds.unsigned_long_long_var = limits.UNSIGNED_LONG_LONG_UPPER + 1


# The following test bool convertors for valid values.

def test_bool_get_true(bool_true_value):
    assert bool_true_value.bool_get() is True

def test_bool_set_true(bool_true_value):
    bool_true_value.bool_set(True)

def test_bool_var_true(bool_true_value):
    bool_true_value.bool_var = True

def test_bool_get_false(bool_false_value):
    assert bool_false_value.bool_get() is False

def test_bool_set_false(bool_false_value):
    bool_false_value.bool_set(False)

def test_bool_var_false(bool_false_value):
    bool_false_value.bool_var = False

def test_bool_get_nonzero(bool_nonzero_value):
    assert bool_nonzero_value.bool_get() is True

def test_bool_set_nonzero(bool_nonzero_value):
    bool_nonzero_value.bool_set(-1)

def test_bool_var_nonzero(bool_nonzero_value):
    bool_nonzero_value.bool_var = -1

def test_bool_get_zero(bool_zero_value):
    assert bool_zero_value.bool_get() is False

def test_bool_set_zero(bool_zero_value):
    bool_zero_value.bool_set(0)

def test_bool_var_zero(bool_zero_value):
    bool_zero_value.bool_var = 0


def _signed_bounds(nrbytes):
    """ Return the range of values for a number of bytes representing a signed
    value.
    """

    v = 1 << ((nrbytes * 8) - 1)

    return -v, v - 1


def _unsigned_upper_bound(nrbytes):
    """ Return the upper bound for a number of bytes representing an unsigned
    value.
    """

    return (1 << (nrbytes * 8)) - 1
