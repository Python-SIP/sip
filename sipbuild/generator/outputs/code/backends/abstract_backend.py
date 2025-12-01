# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from abc import ABC, abstractmethod


class AbstractBackend(ABC):
    """ The abstract base class for backend code generators. """

    def __init__(self, spec):
        """ Initialise the backend. """

        self.spec = spec

    @staticmethod
    def factory(spec):
        """ Return an appropriate backend for the target ABI. """

        if spec.target_abi >= (14, 0):
            from .v14 import v14Backend as backend
        else:
            from .v12v13 import v12v13Backend as backend

        return backend(spec)

    @abstractmethod
    def g_create_wrapped_module(self, sf, bindings,
        # TODO These will probably be generated here at some point.
        has_sip_strings,
        has_external,
        nr_enum_members,
        has_virtual_error_handlers,
        nr_subclass_convertors,
        inst_state,
        slot_extenders,
        init_extenders
    ):
        """ Generate the code to create a wrapped module. """

        ...

    @abstractmethod
    def g_enum_macros(self, sf, scope=None, imported_module=None):
        """ Generate the type macros for enums. """

        ...

    def g_function_support_vars(self, sf):
        """ Generate the variables needed by a function. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_init_mixin_impl_body(self, sf, klass):
        """ Generate the body of the implementation of a mixin initialisation
        function.
        """

        ...

    @abstractmethod
    def g_mapped_type_api(self, sf, mapped_type):
        """ Generate the API details for a mapped type. """

        ...

    def g_mapped_type_int_instances(self, sf, mapped_type):
        """ Generate the code to add a set of ints to a mapped type.  Return
        True if there was at least one.
        """

        # This default implementation does nothing.
        return False

    def g_method_support_vars(self, sf):
        """ Generate the variables needed by a method. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_module_definition(self, sf, has_module_functions=False):
        """ Generate the module definition structure. """

        ...

    @abstractmethod
    def g_module_functions_table(self, sf, bindings, module):
        """ Generate the table of module functions and return True if anything
        was actually generated.
        """

        ...

    @abstractmethod
    def g_py_method_table(self, sf, bindings, members, scope):
        """ Generate a Python method table for a class or mapped type and
        return the number of entries.
        """

        ...

    @abstractmethod
    def g_sip_api(self, sf, module_name):
        """ Generate the SIP API as seen by generated code. """

        ...

    def g_slot_support_vars(self, sf):
        """ Generate the variables needed by a slot function. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_static_variables_table(self, sf, scope=None):
        """ Generate the tables of static variables for a scope and return a
        set of strings corresponding to the tables actually generated.
        """

    @abstractmethod
    def g_type_definition(self, sf, bindings, klass, py_debug):
        """ Generate the type structure that contains all the information
        needed by the meta-type.  A sub-set of this is used to extend
        namespaces.
        """

        ...

    @abstractmethod
    def g_type_init(self, sf, bindings, klass, need_self, need_owner):
        """ Generate the code that initialises a type. """

        ...

    def abi_has_deprecated_message(self):
        """ Return True if the ABI implements sipDeprecated() with a message.
        """

        return True

    def abi_has_next_exception_handler(self):
        """ Return True if the ABI implements sipNextExceptionHandler(). """

        return True

    def abi_has_working_char_conversion(self):
        """ Return True if the ABI has working char to/from a Python integer
        converters (ie. char is not assumed to be signed).
        """

        return True

    def abi_supports_array(self):
        """ Return True if the ABI supports sip.array. """

        return True

    @staticmethod
    @abstractmethod
    def cached_name_ref(cached_name, as_nr=False):
        """ Return a reference to a cached name. """

        ...

    @abstractmethod
    def custom_enums_supported(self):
        """ Return True if custom enums are supported. """

        ...

    @abstractmethod
    def get_class_ref_value(self, klass):
        """ Return the value of a class's reference. """

        ...

    @abstractmethod
    def get_py_method_args(self, *, is_impl, self_is_type, need_self=False,
            need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        ...

    @staticmethod
    @abstractmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        ...

    @staticmethod
    @abstractmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        ...

    @staticmethod
    @abstractmethod
    def get_types_table_prefix():
        """ Return the prefix in the name of the wrapped types table. """

        ...

    @staticmethod
    @abstractmethod
    def get_wrapped_type_type():
        """ Return the type of the C representation of a wrapped object. """

        ...

    def legacy_qt_support(self):
        """ Return True if the module implements legacy Qt support. """

        return False

    @staticmethod
    def need_deprecated_error_flag(code):
        """ Return True if the deprecated error flag is needed by some
        handwritten code.
        """

        return False

    @abstractmethod
    def py_enums_supported(self):
        """ Return True if Python enums are supported. """

        ...
