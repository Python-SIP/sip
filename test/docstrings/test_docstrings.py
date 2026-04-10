# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


CLASS_DOCSTRING = '''Klass overview

Klass()
Default constructor

Klass(other: Klass)
Copy constructor'''


OVERLOADED_DOCSTRINGS = '''overloaded()

overloaded(arg: bytes|None)
prepended

appended
overloaded(arg: int)

discarded'''


def test_module_docstring(module):
    assert module.__doc__ == 'Module'

def test_unbound_overloaded_docstrings(module):
    assert module.overloaded.__doc__ == OVERLOADED_DOCSTRINGS

def test_pod_ptr_arg_docstring(module):
    assert module.pod_ptr_arg.__doc__ == 'pod_ptr_arg(arg: bytes|None)'

def test_pod_ptr_opt_arg_docstring(module):
    assert module.pod_ptr_opt_arg.__doc__ == 'pod_ptr_opt_arg(arg: bytes|None = None)'

def test_bool_opt_arg_docstring(module):
    assert module.bool_opt_arg.__doc__ == 'bool_opt_arg(arg: bool = True)'

def test_class_docstring(module):
    assert module.Klass.__doc__ == CLASS_DOCSTRING

def test_class_instance_docstring(module):
    klass = module.Klass()

    assert klass.__doc__ == CLASS_DOCSTRING

def test_bound_class_callable(module):
    klass = module.Klass()

    assert klass.callable.__doc__ == 'callable'

def test_unbound_class_callable(module):
    assert module.Klass.callable.__doc__ == 'callable'
