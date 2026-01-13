# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


def test_base_implementation_int(module):
    class Derived(module.Base):
        def default_value_int(self):
            return 2 * super().default_value_int()

    klass = Derived()
    value = module.Base.default_value_int(klass)

    assert value == 10


def test_py_reimplementation_class(module):
    class Derived(module.Base):
        def default_value_class(self):
            return Derived(b'Derived')

    value = Derived().get_value_class()

    assert module.ispyowned(value)

    # Note that we don't test the type as it will be Base (not Derived) as we
    # haven't implemented %ConvertToSubClassCode.
    assert value.get_class_name() == b'Derived'


def test_py_reimplementation_int(module):
    class Derived(module.Base):
        def default_value_int(self):
            return 2 * super().default_value_int()

    value = Derived().get_value_int()

    assert value == 20
