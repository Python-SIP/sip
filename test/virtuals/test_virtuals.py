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
            inst = Derived(b'Derived')
            inst.py_attr = True
            return inst

    value = Derived().get_value_class()

    assert module.ispyowned(value)

    # Note that we don't test the type as it will be Base (not Derived) as we
    # haven't implemented %ConvertToSubClassCode.
    assert value.get_class_name() == b'Derived'

    assert value.py_attr


def test_py_reimplementation_int(module):
    class Derived(module.Base):
        def default_value_int(self):
            return 2 * super().default_value_int()

    value = Derived().get_value_int()

    assert value == 20


def test_transferto_factory(module):
    parent = module.Base()
    child = module.Child.New(parent)
    assert not module.ispyowned(child)

    orphan = module.Child.New()
    assert module.ispyowned(orphan)
