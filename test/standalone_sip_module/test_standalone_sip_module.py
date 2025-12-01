# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


cfg_package = 'ns'


def test_wrapped_module_name(module):
    assert module.__name__ == 'ns.standalone_sip_module_module'

def test_toplevel_wrapped_types(module):
    assert module.Klass.__module__ == 'ns.standalone_sip_module_module'
    assert module.Klass.__name__ == 'Klass'
    assert module.Klass.__qualname__ == 'Klass'

def test_nested_wrapped_types(module):
    assert module.Klass.Nested.__module__ == 'ns.standalone_sip_module_module'
    assert module.Klass.Nested.__name__ == 'Nested'
    assert module.Klass.Nested.__qualname__ == 'Klass.Nested'

def test_sip_simplewrapper(module):
    from ns.sip import SIP_ABI_VERSION, simplewrapper

    abi_version = SIP_ABI_VERSION >> 16
    module_name = 'sip' if abi_version == 12 else 'ns.sip'

    assert simplewrapper.__module__ == module_name
    assert simplewrapper.__name__ == 'simplewrapper'
    assert simplewrapper.__qualname__ == 'simplewrapper'

def test_sip_wrapper(module):
    from ns.sip import SIP_ABI_VERSION, wrapper

    abi_version = SIP_ABI_VERSION >> 16
    module_name = 'sip' if abi_version == 12 else 'ns.sip'

    assert wrapper.__module__ == module_name
    assert wrapper.__name__ == 'wrapper'
    assert wrapper.__qualname__ == 'wrapper'

def test_sip_wrappertype(module):
    from ns.sip import SIP_ABI_VERSION, wrappertype

    abi_version = SIP_ABI_VERSION >> 16
    module_name = 'sip' if abi_version == 12 else 'ns.sip'

    assert wrappertype.__module__ == module_name
    assert wrappertype.__name__ == 'wrappertype'
    assert wrappertype.__qualname__ == 'wrappertype'
