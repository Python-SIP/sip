# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from sys import getrefcount

import pytest


def test_attrs_bool(module, abi_version):
    if abi_version >= 14:
        # Values are mapped to True and False.
        assert module.bool_attr is True
        module.bool_attr = False
        assert module.bool_attr is False
        module.bool_attr = 10
        assert module.bool_attr is True
        module.bool_attr = 0
        assert module.bool_attr is False
    else:
        # Values are left as integers.
        assert module.bool_attr
        module.bool_attr = False
        assert not module.bool_attr
        module.bool_attr = 10
        assert module.bool_attr
        module.bool_attr = 0
        assert not module.bool_attr

def test_attrs_byte(module):
    assert module.byte_attr == 10
    module.byte_attr = 20
    assert module.byte_attr == 20

def test_attrs_signed_byte(module):
    assert module.sbyte_attr == -10
    module.sbyte_attr = 20
    assert module.sbyte_attr == 20

def test_attrs_unsigned_byte(module):
    assert module.ubyte_attr == 10
    module.ubyte_attr = 20
    assert module.ubyte_attr == 20

def test_attrs_short(module):
    assert module.short_attr == -10
    module.short_attr = 20
    assert module.short_attr == 20

def test_attrs_unsigned_short(module):
    assert module.ushort_attr == 10
    module.ushort_attr = 20
    assert module.ushort_attr == 20

def test_attrs_int(module, abi_version):
    assert module.int_attr == -10
    module.int_attr = 20
    assert module.int_attr == 20

    # For ABI v14 check the C++ value has changed and not the module dict.
    if abi_version >= 14:
        assert module.get_int_attr() == 20

def test_attrs_unsigned_int(module):
    assert module.uint_attr == 10
    module.uint_attr = 20
    assert module.uint_attr == 20

def test_attrs_long(module):
    assert module.long_attr == -10
    module.long_attr = 20
    assert module.long_attr == 20

def test_attrs_unsigned_long(module):
    assert module.ulong_attr == 10
    module.ulong_attr = 20
    assert module.ulong_attr == 20

def test_attrs_long_long(module):
    assert module.longlong_attr == -10
    module.longlong_attr = 20
    assert module.longlong_attr == 20

def test_attrs_unsigned_long_long(module):
    assert module.ulonglong_attr == 10
    module.ulonglong_attr = 20
    assert module.ulonglong_attr == 20

def test_attrs_pyhasht(module):
    assert module.pyhasht_attr == -10
    module.pyhasht_attr = 20
    assert module.pyhasht_attr == 20

def test_attrs_pyssizet(module):
    assert module.pyssizet_attr == -10
    module.pyssizet_attr = 20
    assert module.pyssizet_attr == 20

def test_attrs_sizet(module):
    assert module.sizet_attr == 10
    module.sizet_attr = 20
    assert module.sizet_attr == 20

def test_attrs_float(module):
    assert module.float_attr == 10.
    module.float_attr = 20.
    assert module.float_attr == 20.

def test_attrs_double(module):
    assert module.double_attr == 10.
    module.double_attr = 20.
    assert module.double_attr == 20.

def test_attrs_char(module):
    assert module.char_attr == b'\x0a'
    module.char_attr = b'\x14'
    assert module.char_attr == b'\x14'

def test_attrs_char_ascii(module):
    assert module.char_ascii_attr == 'A'
    module.char_ascii_attr = 'Z'
    assert module.char_ascii_attr == 'Z'

def test_attrs_char_latin1(module):
    assert module.char_latin1_attr == '£'
    module.char_latin1_attr = '§'
    assert module.char_latin1_attr == '§'

def test_attrs_char_utf8(module):
    assert module.char_utf8_attr == 'A'
    module.char_utf8_attr = 'Z'
    assert module.char_utf8_attr == 'Z'

def test_attrs_signed_char(module):
    assert module.schar_attr == b'\x0a'
    module.schar_attr = b'\x14'
    assert module.schar_attr == b'\x14'

def test_attrs_unsigned_char(module):
    assert module.uchar_attr == b'\x0a'
    module.uchar_attr = b'\x14'
    assert module.uchar_attr == b'\x14'

def test_attrs_wchar(module):
    assert module.wchar_attr == 'β'
    module.wchar_attr = 'α'
    assert module.wchar_attr == 'α'

def test_attrs_string(module, abi_version):
    assert module.string_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.string_attr = b'bad'

def test_attrs_const_string(module):
    assert module.string_attr_const == b'str'
    module.string_attr_const = b'new_str'
    assert module.string_attr_const == b'new_str'
    module.string_attr_const = None
    assert module.string_attr_const is None

def test_attrs_string_ascii(module, abi_version):
    assert module.string_ascii_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.string_ascii_attr = 'bad'

def test_attrs_const_string_ascii(module, abi_version):
    assert module.string_ascii_attr_const == 'str'
    module.string_ascii_attr_const = 'new_str'
    assert module.string_ascii_attr_const == 'new_str'
    module.string_ascii_attr_const = None
    assert module.string_ascii_attr_const is None

    # ABI v14 will update the wrapped module attribute and convert the type in
    # the process.
    if abi_version >= 14:
        module.string_ascii_attr_const = b'bytes'
        assert module.string_ascii_attr_const == 'bytes'

def test_attrs_string_latin1(module, abi_version):
    assert module.string_latin1_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.string_latin1_attr = 'bad'

def test_attrs_const_string_latin1(module, abi_version):
    assert module.string_latin1_attr_const == '££'
    module.string_latin1_attr_const = '§§'
    assert module.string_latin1_attr_const == '§§'
    module.string_latin1_attr_const = None
    assert module.string_latin1_attr_const is None

    # ABI v14 will update the wrapped module attribute and convert the type in
    # the process.
    if abi_version >= 14:
        module.string_latin1_attr_const = '££'.encode('latin-1')
        assert module.string_latin1_attr_const == '££'

def test_attrs_string_utf8(module, abi_version):
    assert module.string_utf8_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.string_utf8_attr = 'bad'

def test_attrs_const_string_utf8(module, abi_version):
    assert module.string_utf8_attr_const == '2H₂ + O₂ ⇌ 2H₂O'
    module.string_utf8_attr_const = 'ሲተረጉሙ ይደረግሙ።'
    assert module.string_utf8_attr_const == 'ሲተረጉሙ ይደረግሙ።'
    module.string_utf8_attr_const = None
    assert module.string_utf8_attr_const is None

    # ABI v14 will update the wrapped module attribute and convert the type in
    # the process.
    if abi_version >= 14:
        module.string_utf8_attr_const = 'Καλημέρα'.encode('utf-8')
        assert module.string_utf8_attr_const == 'Καλημέρα'

def test_attrs_signed_string(module, abi_version):
    assert module.sstring_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.sstring_attr = b'bad'

def test_attrs_const_signed_string(module):
    assert module.sstring_attr_const == b'str'
    module.sstring_attr_const = b'new_str'
    assert module.sstring_attr_const == b'new_str'
    module.sstring_attr_const = None
    assert module.sstring_attr_const is None

def test_attrs_unsigned_string(module, abi_version):
    assert module.ustring_attr is None

    # For ABI v14 check a NULL wrapped module attribute cannot be updated to
    # point to potentially ephemeral data.  (Earlier versions just update the
    # module dictionary.)
    if abi_version >= 14:
        with pytest.raises(ValueError):
            module.ustring_attr = b'bad'

def test_attrs_const_unsigned_string(module):
    assert module.ustring_attr_const == b'str'
    module.ustring_attr_const = b'new_str'
    assert module.ustring_attr_const == b'new_str'
    module.ustring_attr_const = None
    assert module.ustring_attr_const is None

def test_attrs_voidptr(module):
    vp = module.voidptr_attr
    assert vp.asstring(size=5) == b'bytes'
    assert vp.getwriteable() is True

    module.voidptr_attr = None
    assert module.voidptr_attr is None

def test_attrs_const_voidptr(module, abi_version):
    const_vp = module.voidptr_const_attr
    assert const_vp.asstring(size=11) == b'bytes const'

    # ABIs prior to v14 don't honour the const of a wrapped module attribute.
    if abi_version >= 14:
        assert const_vp.getwriteable() is False

    module.voidptr_const_attr = None
    assert module.voidptr_const_attr is None

def test_attrs_pyobject(module):
    # Note that SIP does not check the Python types of these attributes (which
    # is really a bug) so we don't test the different types.
    assert module.pyobject_attr is None

    obj = object()
    obj_refcount = getrefcount(obj)

    module.pyobject_attr = obj
    assert module.pyobject_attr is obj
    assert getrefcount(obj) == obj_refcount + 1

def test_del_attrs(module, abi_version):
    # For ABI v14 check a wrapped module attribute cannot be deleted.
    if abi_version >= 14:
        with pytest.raises(AttributeError):
            del module.int_attr

def test_nonwrapped_attrs(module):
    with pytest.raises(AttributeError):
        module.foo

    module.foo = 'bar'
    assert module.foo == 'bar'

    del module.foo
    with pytest.raises(AttributeError):
        module.foo

def test_const_types(module, abi_version):
    # For ABI v14 check a wrapped const module attribute cannot be modified.
    if abi_version >= 14:
        assert module.int_attr_const == 10

        with pytest.raises(ValueError):
            module.int_attr_const = 0

def test_nonwrapped_attrs(module):
    module.nonwrapped_int = 10
    assert module.nonwrapped_int == 10

def test_py_name_annotation_attrs(module):
    assert module.py_int_attr == 10

def test_getters_and_setters(module, abi_version):
    if abi_version >= 14:
        assert module.int_attr_getter == 20

        module.int_attr_setter = 20
        assert module.int_attr_setter == 40

        with pytest.raises(NameError):
            module.int_attr_bad_setter = 0

        assert module.int_attr_no_setter == 10
        with pytest.raises(ValueError):
            module.int_attr_no_setter = 0
