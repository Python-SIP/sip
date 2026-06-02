# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


import pytest


cfg_package = 'virtual_error_handlers'


def test_default_handler(package):
    class MyUseDefault(package.extras_module.UseDefault):
        def use_default(self):
            raise NotImplementedError

    inst = MyUseDefault()

    assert not hasattr(inst, 'default_used')
    inst.trigger_default()
    assert inst.default_used

def test_class_handler(package):
    class MyKlass(package.extras_module.Klass):
        def use_klass(self):
            raise NotImplementedError

    inst = MyKlass()

    assert not hasattr(inst, 'klass_used')
    inst.trigger_klass()
    assert inst.klass_used

def test_inst_handler(package):
    class MyKlass(package.extras_module.Klass):
        def use_inst(self):
            raise NotImplementedError

    inst = MyKlass()

    assert not hasattr(inst, 'inst_used')
    inst.trigger_inst()
    assert inst.inst_used

def test_no_handler(package, virtual_hook):
    class MyKlass(package.extras_module.Klass):
        def use_none(self):
            raise NotImplementedError

    inst = MyKlass()

    inst.trigger_none()
    with pytest.raises(NotImplementedError):
        virtual_hook.reraise()
