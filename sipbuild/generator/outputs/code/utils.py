# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from ...specification import (AccessSpecifier, ArgumentType, CodeBlock,
        GILAction, IfaceFileType, MappedType, PyQtMethodSpecifier,
        WrappedClass)

from ..formatters import fmt_argument_as_cpp_type, fmt_class_as_scoped_name


def arg_is_small_enum(arg):
    """ Return True if an argument refers to a small C++11 enum. """

    return arg.type is ArgumentType.ENUM and arg.definition.enum_base_type is not None


def callable_overloads(member, overloads):
    """ An iterator over the non-private and non-signal overloads. """

    for overload in overloads:
        if overload.common is member and overload.access_specifier is not AccessSpecifier.PRIVATE and overload.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
            yield overload


def get_class_flags(spec, klass, py_debug):
    """ Return the flags for a class. """

    module = spec.module
    flags = []

    if klass.is_abstract:
        flags.append('SIP_TYPE_ABSTRACT')

    if klass.subclass_base is not None:
        flags.append('SIP_TYPE_SCC')

    if klass.handles_none:
        flags.append('SIP_TYPE_ALLOW_NONE')

    if klass.has_nonlazy_method:
        flags.append('SIP_TYPE_NONLAZY')

    if module.call_super_init:
        flags.append('SIP_TYPE_SUPER_INIT')

    if not py_debug and module.use_limited_api:
        flags.append('SIP_TYPE_LIMITED_API')

    flags.append('SIP_TYPE_NAMESPACE' if klass.iface_file.type is IfaceFileType.NAMESPACE else 'SIP_TYPE_CLASS')

    return '|'.join(flags)


def get_const_cast(spec, type, value):
    """ Return a value with an appropriate const_cast to a type. """

    if type.is_const:
        cpp_type = fmt_argument_as_cpp_type(spec, type, plain=True,
                no_derefs=True)

        return f'const_cast<{cpp_type} *>({value})'

    return value


def get_convert_to_type_code(type):
    """ Return a type's %ConvertToTypeCode. """

    if type.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and not type.is_constrained:
        return type.definition.convert_to_type_code

    return None


def get_docstring_text(docstring):
    """ Return the text of a docstring. """

    text = docstring.text

    # Remove any single trailing newline.
    if text.endswith('\n'):
        text = text[:-1]

    s = ''

    for ch in text:
        if ch == '\n':
            # Let the compiler concatanate lines.
            s += '\\n"\n"'
        elif ch in r'\"':
            s += '\\'
            s += ch
        elif ch.isprintable():
            s += ch
        else:
            s += f'\\{ord(ch):03o}'

    return s


def get_encoded_type(module, klass, last=False):
    """ Return the structure representing an encoded type. """

    klass_module = klass.iface_file.module

    fields = [str(klass.iface_file.type_nr)]

    if klass_module is module:
        fields.append('255')
    else:
        for module_nr, imported_module in enumerate(module.all_imports):
            if imported_module is klass_module:
                fields.append(str(module_nr))
                break

    fields.append(str(int(last)))

    return '{' + ', '.join(fields) + '}'


def get_enum_class_scope(spec, enum):
    """ Return the scope of an unscoped enum as a string. """

    if enum.is_protected:
        scope_s = 'sip' + enum.scope.iface_file.fq_cpp_name.as_word
    else:
        scope_s = scoped_class_name(spec, enum.scope)

    return scope_s


def get_enum_member(spec, enum_member):
    """ Return an enum member as a string. """

    if spec.c_bindings:
        return enum_member.cpp_name

    enum = enum_member.scope

    if enum.no_scope:
        scope_s = ''
    else:
        if enum.is_scoped:
            scope_s = '::' + enum.cached_fq_cpp_name.name
        elif isinstance(enum.scope, WrappedClass):
            scope_s = get_enum_class_scope(spec, enum)
        elif isinstance(enum.scope, MappedType):
            scope_s = enum.scope.iface_file.fq_cpp_name.as_cpp
        else:
            # This can't happen.
            scope_s = ''

        scope_s += '::'

    return f'static_cast<int>({scope_s}{enum_member.cpp_name})'


def get_named_value_decl(spec, scope, type, name):
    """ Return the declaration of a named variable to hold a C++ value. """

    saved_derefs = type.derefs
    saved_is_const = type.is_const
    saved_is_reference = type.is_reference

    if len(type.derefs) == 0:
        if type.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            type.derefs = [False]
        else:
            type.is_const = False

    type.is_reference = False

    named_value_decl = fmt_argument_as_cpp_type(spec, type, name=name,
            scope=scope.iface_file if isinstance(scope, (WrappedClass, MappedType)) else None)

    type.derefs = saved_derefs
    type.is_const = saved_is_const
    type.is_reference = saved_is_reference

    return named_value_decl


def get_normalised_cached_name(cached_name):
    """ Return the normalised form of a cached name. """

    # If the name seems to be a template then just use the offset to ensure
    # that it is unique.
    if '<' in cached_name.name:
        return str(cached_name.offset)

    # Handle C++ and Python scopes.
    return cached_name.name.replace(':', '_').replace('.', '_')


def get_optional_ptr(is_ptr, name):
    """ Return an appropriate reference to an optional pointer. """

    return name if is_ptr else 'SIP_NULLPTR'


def get_type_from_void(spec, type_name, variable_name, derefs='*',
        tight=False):
    """ Return a cast from a void pointer variable to a pointer type. """

    if spec.c_bindings:
        if tight:
            return f'(({type_name} {derefs}){variable_name})'

        return f'({type_name} {derefs}){variable_name}'

    return f'reinterpret_cast<{type_name} {derefs}>({variable_name})'


def get_use_in_code(code, s, spec=None):
    """ Return the string to use depending on whether it is used in some code
    and optionally if the bindings are for C.
    """

    # Always use the string for C bindings.
    if spec is not None and spec.c_bindings:
        return s

    return s if is_used_in_code(code, s) else ''


def get_user_state_suffix(spec, type):
    """ Return the suffix for functions that have a variant that supports a
    user state.
    """

    return 'US' if spec.target_abi >= (13, 0) and type_needs_user_state(type) else ''


def get_void_ptr_cast(type):
    """ Return a void* cast for an argument if needed. """

    # Generate a cast if the argument's type was a typedef.  This allows us to
    # use typedef's to void* to hide something more complex that we don't
    # handle.
    if type.original_typedef is None:
        return ''

    return '(const void *)' if type.is_const else '(void *)'


def has_method_docstring(bindings, member, overloads):
    """ Return True if a function/method has a docstring. """

    auto_docstring = False

    # Check for any explicit docstrings and remember if there were any that
    # could be automatically generated.
    for overload in callable_overloads(member, overloads):
        if overload.docstring is not None:
            return True

        if bindings.docstrings:
            auto_docstring = True

    if member.no_arg_parser:
        return False

    return auto_docstring


def is_used_in_code(code, s):
    """ Return True if a string is used in code. """

    # The code may be a list of code blocks or an optional code block.
    if code is None:
        return False

    if isinstance(code, CodeBlock):
        code = [code]

    for cb in code:
        if s in cb.text:
            return True

    return False


# The types that need a Python reference.
_PY_REF_TYPES = (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING,
    ArgumentType.UTF8_STRING, ArgumentType.USTRING, ArgumentType.SSTRING,
    ArgumentType.STRING)

def keep_py_reference(spec, arg):
    """ Return True if the argument has a type that requires an extra reference
    to the originating object to be kept.
    """

    if arg.is_reference or len(arg.derefs) == 0:
        return False

    if arg.type in _PY_REF_TYPES:
        return True

    # wchar_t strings/arrays don't leak in ABI v14 and later.  Note that
    # this solution could be adopted for earlier ABIs.
    return spec.target_abi >= (14, 0) and arg.type is ArgumentType.WSTRING


def need_dealloc(spec, bindings, klass):
    """ Return True if a dealloc function is needed for a class. """

    if klass.iface_file.type is IfaceFileType.NAMESPACE:
        return False

    # Each of these conditions cause some code to be generated.

    if bindings.tracing:
        return True

    if spec.c_bindings:
        return True

    if len(klass.dealloc_code) != 0:
        return True

    if klass.dtor is AccessSpecifier.PUBLIC:
        return True

    if klass.has_shadow:
        return True

    return False


def need_error_flag(code):
    """ Return True if handwritten code uses the error flag. """

    return is_used_in_code(code, 'sipError')


def py_scope(scope):
    """ Return the Python scope by accounting for hidden C++ namespaces. """

    return None if isinstance(scope, WrappedClass) and scope.is_hidden_namespace else scope


def pyqt5_supported(spec):
    """ Return True if the PyQt5 plugin was specified. """

    return 'PyQt5' in spec.plugins


def pyqt6_supported(spec):
    """ Return True if the PyQt6 plugin was specified. """

    return 'PyQt6' in spec.plugins


def release_gil(gil_action, bindings):
    """ Return True if the GIL is to be released. """

    return bindings.release_gil if gil_action is GILAction.DEFAULT else gil_action is GILAction.RELEASE


def scoped_class_name(spec, klass):
    """ Return a scoped class name as a string.  Protected classes have to be
    explicitly scoped.
    """

    return fmt_class_as_scoped_name(spec, klass, scope=klass.iface_file)


def scoped_variable_name(spec, variable):
    """ Return a scoped variable name as a string.  This should be used
    whenever the scope may be the instantiation of a template which specified
    /NoTypeName/.
    """

    scope = variable.scope
    fq_cpp_name = variable.fq_cpp_name

    if scope is None:
        return fq_cpp_name.as_cpp

    return scoped_class_name(spec, scope) + '::' + fq_cpp_name.base_name


def skip_overload(overload, member, klass, scope, want_local=True):
    """ See if a member overload should be skipped. """

    # Skip if it's not the right name.
    if overload.common is not member:
        return True

    # Skip if it's a signal.
    if overload.pyqt_method_specifier is PyQtMethodSpecifier.SIGNAL:
        return True

    # Skip if it's a private abstract.
    if overload.is_abstract and overload.access_specifier is AccessSpecifier.PRIVATE:
        return True

    # If we are disallowing them, skip if it's not in the current class unless
    # it is protected.
    if want_local and overload.access_specifier is not AccessSpecifier.PROTECTED and klass is not scope:
        return True

    return False


def type_needs_user_state(type):
    """ Return True if a type needs user state to be provided. """

    return type.type is ArgumentType.MAPPED and type.definition.needs_user_state


def variables_in_scope(spec, scope, check_handler=True):
    """ An iterator over the variables in a scope. """

    for variable in spec.variables:
        if py_scope(variable.scope) is scope and variable.module is spec.module:
            if check_handler and variable.needs_handler:
                continue

            yield variable
