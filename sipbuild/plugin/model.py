# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from functools import cached_property

from ..exceptions import UserException
from ..generator.specification import (AccessSpecifier, IfaceFileType,
        WrappedClass)


class _Proxy:
    """ The base class for all proxies. """

    def __init__(self, subject):
        """ Initialise the proxy. """

        self._subject = subject

    def get_plugin_attribute(self, plugin, name, default=None):
        """ Return the value of a plugin-specific attribute. """

        return getattr(self._subject, f'_{plugin.sip_key}_{name}', default)

    def set_plugin_attribute(self, plugin, name, value):
        """ Set the value of a plugin-specific attribute. """

        setattr(self._subject, f'_{plugin.sip_key}_{name}', value)


class Argument(_Proxy):
    """ Encapsulates an argument of a signature. """

    def __init__(self, subject, scope, spec):
        """ Initialise the argument. """

        super().__init__(subject)

        self._scope = scope
        self._spec = spec

    def get_declaration(self, *, nr_of_scopes_stripped=None):
        """ The C/C++ declaration of the argument. """

        from ..generator.outputs.formatters import fmt_argument_as_cpp_type
        from ..generator.scoped_name import STRIP_GLOBAL

        scope = None if self._scope is None else self._scope.iface_file

        strip = nr_of_scopes_stripped
        if strip is None:
            strip = STRIP_GLOBAL

        return fmt_argument_as_cpp_type(self._spec, self._subject, scope=scope,
                strip=strip)

    @property
    def is_const(self):
        """ True if the argument is const. """

        return self._subject.is_const

    @is_const.setter
    def is_const(self, value):
        """ The is_const setter. """

        self._subject.is_const = value

    @property
    def is_optional(self):
        """ True if the argument is optional. """

        return self._subject.default_value is not None

    @property
    def is_reference(self):
        """ True if the argument is a reference. """

        return self._subject.is_reference

    @is_reference.setter
    def is_reference(self, value):
        """ The is_reference setter. """

        self._subject.is_reference = value

    @property
    def nr_of_dereferences(self):
        """ The number of dereferences. """

        return len(self._subject.derefs)


class Class(_Proxy):
    """ Encapsulates a wrapped class. """

    def __init__(self, subject, spec):
        """ Initialise the wrapped class. """

        super().__init__(subject)

        self._spec = spec

    @cached_property
    def cpp_name(self):
        """ The C++ name of the class. """

        return self._subject.iface_file.fq_cpp_name.as_cpp

    @cached_property
    def cpp_name_as_word(self):
        """ The C++ name of the class transformed so that it can be used in a
        C++ identifier.
        """

        return self._subject.iface_file.fq_cpp_name.as_word

    @property
    def dtor_is_public(self):
        """ True if the dtor of the class is public. """

        return self._subject.dtor is AccessSpecifier.PUBLIC

    @cached_property
    def functions(self):
        """ The sequence of the class's functions.  Note that this is based on
        the Python names, not the C++ names.
        """

        return [Function(m, self._subject, self._spec)
                for m in self._subject.members]

    @property
    def needs_derived_class(self):
        """ True if the class needs a derived class to be generated. """

        return self._subject.needs_shadow

    @needs_derived_class.setter
    def needs_derived_class(self, value):
        """ The needs_derived_class setter. """

        self._subject.needs_shadow = value

    @cached_property
    def py_name_as_str(self):
        """ The Python name of the class as a string literal. """

        return self._spec.bindings.project.backend.cached_name_ref(
                self._subject.py_name)

    @property
    def unscoped_cpp_name(self):
        """ The C++ name of the class. """

        return self._subject.iface_file.fq_cpp_name.base_name


class Constructor(_Proxy):
    """ Encapsulates a constructor. """

    pass


class Enum(_Proxy):
    """ Encapsulates a wrapped enum. """

    def __init__(self, subject, spec):
        """ Initialise the function. """

        super().__init__(subject)

        self._spec = spec

    @cached_property
    def cpp_name(self):
        """ The C++ name of the enum.  This will be None for anonymous enums.
        """

        fq_cpp_name = self._subject.fq_cpp_name

        return None if fq_cpp_name is None else fq_cpp_name.as_cpp

    @property
    def is_public(self):
        """ True if the enum is public. """

        return not self._subject.is_protected

    @property
    def scope(self):
        """ The optional enclosing scope of the enum. """

        scope = self._subject.scope
        if scope is None:
            return None

        return Class(scope, self._spec)


class EnumMember(_Proxy):
    """ Encapsulates a member of an enum. """

    pass


class Function(_Proxy):
    """ Encapsulates a function (ie. a set of wrapped overloads). """

    def __init__(self, subject, scope, spec):
        """ Initialise the function. """

        super().__init__(subject)

        self._scope = scope
        self._spec = spec

    @cached_property
    def impl_reference(self):
        """ Return a backend-specific reference to the implementation of the
        function.
        """

        # Note that the use case only calls this for a class.  Also note that
        # this currently does not work for ABI v14.
        return f'&methods_{self._scope.iface_file.fq_cpp_name.as_word}[{self._subject.member_nr}]'

    @cached_property
    def overloads(self):
        """ The sequence of the overloads of the function. """

        overloads = self._spec.module.overloads if self._scope is None else self._scope.overloads

        return [Overload(o, self._scope, self._spec)
                for o in overloads if o.common is self._subject]

    @cached_property
    def py_name_as_str(self):
        """ The Python name of the class as a string literal. """

        return self._spec.bindings.project.backend.cached_name_ref(
                self._subject.py_name)


class MappedType(_Proxy):
    """ Encapsulates a mapped type. """

    def __init__(self, subject, spec):
        """ Initialise the mapped type. """

        super().__init__(subject)

        self._spec = spec

    @cached_property
    def cpp_name(self):
        """ The C++ name of the mapped type. """

        from ..generator.outputs.formatters import fmt_argument_as_cpp_type

        return fmt_argument_as_cpp_type(self._spec, self._subject.type,
                plain=True, no_derefs=True)

    @cached_property
    def cpp_name_as_word(self):
        """ The C++ name of the mapped type transformed so that it can be used
        in a C++ identifier.
        """

        return self._subject.iface_file.fq_cpp_name.as_word


class Overload(_Proxy):
    """ Encapsulates a wrapped overload of a function. """

    def __init__(self, subject, scope, spec):
        """ Initialise the overload. """

        super().__init__(subject)

        self._scope = scope
        self._spec = spec

    @cached_property
    def cpp_signature(self):
        """ The overload's C++ signature. """

        return Signature(self._subject.cpp_signature, self._scope, self._spec)

    @cached_property
    def docstring(self):
        """ The overload's docstring, None if the %Docstring directive wasn't
        specified.
        """

        overload = self._subject

        if overload.docstring is None:
            return None

        return self._spec.bindings.project.backend.get_overload_docstring(
                self._spec, self._scope, overload)

    def generate_signature_parser(self, sf, *, overload_nr, simple_call=False):
        """ Generate a signature parser. """

        # TODO: The overload number is only used by ABI v14 to access the type
        # hint.  As the PyQt signal handling for ABI v14 hasn't been designed
        # yet it may not be needed.
        self._spec.bindings.project.backend.g_arg_parser(sf, self._spec,
                self._scope, self._subject.py_signature, overload_nr,
                overload=None if simple_call else self._subject)

    def generate_call_arguments(self, sf):
        """ Generate the arguments to a call that were produced from a parsed
        signature.
        """

        from ..generator.outputs.code.snippets import g_call_args

        overload = self._subject

        g_call_args(sf, self._spec, overload.cpp_signature,
                overload.py_signature)

    def generate_signature_parser_tidy_up(self, sf):
        """ Generate the code to tidy up after a signature parser. """

        from ..generator.outputs.code.snippets import g_delete_temporaries

        g_delete_temporaries(sf, self._spec, self._subject.py_signature)

    @property
    def is_callable(self):
        """ True if the overload is callable.  Overloads are callable by
        default.
        """

        return self._subject.is_callable

    @is_callable.setter
    def is_callable(self, value):
        """ The is_callable setter. """

        self._subject.is_callable = value

    @property
    def is_public(self):
        """ True if the overload is public. """

        return self._subject.access_specifier is AccessSpecifier.PUBLIC

    @property
    def is_static(self):
        """ True if the overload is static. """

        return self._subject.is_static

    @property
    def py_name(self):
        """ The Python name of the overload. """

        return self._subject.common.py_name.name

    @cached_property
    def type_hint(self):
        """ The overload's type hint. """

        from ..generator.outputs.code.utils import get_overload_type_hint

        return get_overload_type_hint(self._spec, self._scope, self._subject)

    @property
    def unscoped_cpp_name(self):
        """ The C++ name of the overload. """

        return self._subject.cpp_name


class Signature(_Proxy):
    """ Encapsulates a signature. """

    def __init__(self, subject, scope, spec):
        """ Initialise the signature. """

        super().__init__(subject)

        self._scope = scope
        self._spec = spec

    @cached_property
    def arguments(self):
        """ The signatures arguments. """

        return [Argument(a, self._scope, self._spec)
                for a in self._subject.args]


class Specification(_Proxy):
    """ Encapsulate a complete specification. """

    def __init__(self, subject, *, error_logger=None, error_log=None,
            production=None, symbol=None):
        """ Initialise the specification. """

        super().__init__(subject)

        self._error_logger = error_logger
        self._error_log = error_log

        self._production = production
        self._symbol = symbol

    @cached_property
    def abi_major_version(self):
        """ The major version of the target ABI. """

        return self._subject.bindings.project.abi_version[0]

    @cached_property
    def abi_minor_version(self):
        """ The minor version of the target ABI. """

        return self._subject.bindings.project.abi_version[1]

    def add_declaration(self, klass):
        """ The header file that declares a class is added to those #included
        by the generated module code.
        """

        from ..generator.utils import append_iface_file

        append_iface_file(self._subject.module.used, klass._subject.iface_file)

    @cached_property
    def all_enums(self):
        """ The sequence of all the enums. """

        spec = self._subject

        return [Enum(e, spec) for e in spec.enums if e.module is spec.module]

    @cached_property
    def all_mapped_types(self):
        """ The sequence of all the mapped types. """

        spec = self._subject

        return [MappedType(mt, spec) for mt in spec.mapped_types
                if mt.iface_file.module is spec.module]

    @staticmethod
    def apply_template_arguments(proto_string, template_arguments):
        """ Applies the arguments of a template to a prototype string and
        return the expanded result.
        """

        from ..generator.templates import template_string

        return template_string(proto_string, template_arguments)

    @property
    def c_bindings(self):
        """ True if C (rather than C++) bindings are being generated. """

        return self._subject.c_bindings

    @property
    def docstrings(self):
        """ True if docstrings should be generated. """

        return self._subject.bindings.docstrings

    def error(self, message, *, annotation=None, overload=None):
        """ Report an error related to a particular context. """

        # This makes assumptions about the calling context based on the keyword
        # arguments.  It may prove to be a little fragile but it will do for
        # now.

        if overload is not None:
            self._error_logger(self._error_log, message, overload._subject)

        elif self._error_log is not None:
            self._error_log.log(message)

        elif self._production is not None:
            p = self._production
            p.parser.pm.parser_error(p, self._symbol, message)

        elif annotation is not None:
            anno = self._annotations[annotation]
            p = anno.production
            p.parser.pm.parser_error(p, anno.symbol, message)

        else:
            raise UserException(message)

    def get_type_specification(self, wrapped_object):
        """ Return the name of the type specification of a wrapped object. """

        return self._subject.bindings.project.backend.get_type_ref(
                wrapped_object._subject)

    @staticmethod
    def is_class(wrapped_object):
        """ Return True if an optional wrapped object is a class. """

        if wrapped_object is None:
            return False

        obj = wrapped_object._subject

        return isinstance(obj, WrappedClass) and obj.iface_file.type is IfaceFileType.CLASS

    @staticmethod
    def is_namespace(wrapped_object):
        """ Return True if an optional wrapped object is a namespace. """

        if wrapped_object is None:
            return False

        obj = wrapped_object._subject

        return isinstance(obj, WrappedClass) and obj.iface_file.type is IfaceFileType.NAMESPACE

    @cached_property
    def module_name(self):
        """ The name of the module. """

        return self._subject.module.py_name

    @staticmethod
    def parse_annotation(annotations, name, annotation_type):
        """ Parse an annotation of a particular type and return its value. """

        anno = dict.pop(annotations, name, None)

        return annotations.validate(name, anno, annotation_type)
