# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


def test_wrapped_module_name(module):
    assert module.__name__ == 'embedded_sip_module_module'

def test_toplevel_wrapped_types(module):
    assert module.Klass.__module__ == 'embedded_sip_module_module'
    assert module.Klass.__name__ == 'Klass'
    assert module.Klass.__qualname__ == 'Klass'

def test_nested_wrapped_types(module):
    assert module.Klass.Nested.__module__ == 'embedded_sip_module_module'
    assert module.Klass.Nested.__name__ == 'Nested'
    assert module.Klass.Nested.__qualname__ == 'Klass.Nested'

def test_simplewrapper(module, abi_version):
    module_name = 'sip' if abi_version == 12 else 'embedded_sip_module_module'
    assert module.simplewrapper.__module__ == module_name
    assert module.simplewrapper.__name__ == 'simplewrapper'
    assert module.simplewrapper.__qualname__ == 'simplewrapper'

def test_wrapper(module, abi_version):
    module_name = 'sip' if abi_version == 12 else 'embedded_sip_module_module'
    assert module.wrapper.__module__ == module_name
    assert module.wrapper.__name__ == 'wrapper'
    assert module.wrapper.__qualname__ == 'wrapper'

def test_wrappertype(module, abi_version):
    module_name = 'sip' if abi_version == 12 else 'embedded_sip_module_module'
    assert module.wrappertype.__module__ == module_name
    assert module.wrappertype.__name__ == 'wrappertype'
    assert module.wrappertype.__qualname__ == 'wrappertype'
