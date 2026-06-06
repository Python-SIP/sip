# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


cfg_enabled_for = [13, 14]
cfg_package = 'v13_ns'


def test_wrapped_module_name(module):
    assert module.__name__ == 'v13_ns.v13_standalone_sip_module_module'

def test_toplevel_wrapped_types(module):
    assert module.Klass.__module__ == 'v13_ns.v13_standalone_sip_module_module'
    assert module.Klass.__name__ == 'Klass'
    assert module.Klass.__qualname__ == 'Klass'

def test_nested_wrapped_types(module):
    assert module.Klass.Nested.__module__ == 'v13_ns.v13_standalone_sip_module_module'
    assert module.Klass.Nested.__name__ == 'Nested'
    assert module.Klass.Nested.__qualname__ == 'Klass.Nested'

def test_sip_simplewrapper(module):
    from v13_ns.sip import simplewrapper, wrapper

    assert simplewrapper.__module__ == 'v13_ns.sip'
    assert simplewrapper.__name__ == 'simplewrapper'
    assert simplewrapper.__qualname__ == 'simplewrapper'

    assert issubclass(module.Klass.Nested, simplewrapper)
    assert not issubclass(module.Klass.Nested, wrapper)

def test_sip_wrapper(module):
    from v13_ns.sip import wrapper

    assert wrapper.__module__ == 'v13_ns.sip'
    assert wrapper.__name__ == 'wrapper'
    assert wrapper.__qualname__ == 'wrapper'

def test_sip_wrappertype(module):
    from v13_ns.sip import wrappertype

    assert wrappertype.__module__ == 'v13_ns.sip'
    assert wrappertype.__name__ == 'wrappertype'
    assert wrappertype.__qualname__ == 'wrappertype'
