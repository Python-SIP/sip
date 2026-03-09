# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


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
    def g_cast_function(self, sf, klass):
        """ Generate the function that casts a C++ pointer to a target type.
        """

        ...

    @abstractmethod
    def g_catch_body(self, sf):
        """ Generate the body of a catch clause. """

        ...

    @abstractmethod
    def g_class_api(self, sf, klass):
        """ Generate the API details for a class. """

        ...

    @abstractmethod
    def g_conversion_to_enum(self, sf, enum):
        """ Generate the code to convert a Python enum (sipSelf) to a C/C++
        enum (sipCpp).
        """

        ...

    @abstractmethod
    def g_cpp_dtor(self, sf):
        """ Generate the body of the dtor of a generated shadow class. """

        ...

    @abstractmethod
    def g_create_wrapped_module(self, sf, bindings,
        # TODO These will probably be generated here at some point.
        name_cache_closure,
        has_external,
        enums_closure,
        has_virtual_error_handlers,
        nr_subclass_convertors,
        inst_state,
        slot_extenders,
        init_extenders
    ):
        """ Generate the code to create a wrapped module and return an
        ABI-specific object that will be passed back to the backend at some
        point. """

        ...

    @abstractmethod
    def g_enums_specifications(self, sf, scope=None):
        """ Generate the specifications for the wrapped enums in a scope and
        return an ABI-specific object which will be passed back to the backend
        at some point.
        """

        ...

    def g_exceptions_specifications(self, sf):
        """ Generate the specifications for any exceptions. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_exceptions_decls(self, sf):
        """ Generate the declarations of all exceptions. """

        ...

    def g_exceptions_defn(self, sf):
        """ Generate the definition of the exceptions data structure. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_get_py_reimpl(self, sf, klass, overload, virt_nr):
        """ Generate the code to get the Python reimplementation of a C++
        virtual.
        """

        ...

    @abstractmethod
    def g_import_tables(self, sf):
        """ Generated the tables related to imported modules. """

        ...

    @abstractmethod
    def g_imported_module_decls(self, sf, imported_module):
        """ Generate any declarations related to an imported module. """

    ...

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

    @abstractmethod
    def g_mapped_type_definition(self, sf, bindings, mapped_type):
        """ Generate the type structure that contains all the information
        needed by a mapped type.
        """

        ...

    def g_mapped_type_int_instances(self, sf, mapped_type):
        """ Generate the code to add a set of ints to a mapped type.  Return
        True if there was at least one.
        """

        # This default implementation does nothing.
        return False

    @abstractmethod
    def g_module_definition(self, sf, has_module_functions=False):
        """ Generate the module definition structure. """

        ...

    def g_name_cache(self, sf):
        """ Generate the name cache definition and return an ABI-specific
        object which will be passed back to the backend at some point.
        """

        # This default implementation returns None.
        return None

    @staticmethod
    @abstractmethod
    def g_not_implemented(sf):
        """ Generate the code to clear any exception and return
        Py_NotImplemented.
        """

        ...

    @abstractmethod
    def g_py_method_end(self, sf, state, nr_signatures):
        """ Generate the end of a method implementation. """

        ...

    @abstractmethod
    def g_py_method_start(self, sf, bindings, klass, member, original_klass,
            need_args, need_self):
        """ Generate the start of a method implementation and return an
        ABI-specific object which will be passed to g_py_method_end().
        """

        ...

    def g_py_method_support_vars(self, sf, scope):
        """ Generate the variables needed by a method implementation. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_py_method_table(self, sf, bindings, members, scope):
        """ Generate a Python method table for a class or mapped type and
        return the number of entries.
        """

        ...

    @abstractmethod
    def g_sip_api(self, sf, module_name, module_closure):
        """ Generate the SIP API as seen by generated code. """

        ...

    @abstractmethod
    def g_slot_implementations(self, sf, bindings, scope, members):
        """ Generate the slot implementations for a scope. """

        ...

    def g_slot_support_vars(self, sf, scope, member):
        """ Generate the variables needed by a slot function. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def g_static_function_end(self, sf, state, nr_signatures):
        """ Generate the end of a static function implementation. """

        ...

    @abstractmethod
    def g_static_function_start(self, sf, bindings, scope_py, member,
            overloads):
        """ Generate the start of a static function implementation and return
        an ABI-specific object which will be passed to g_static_function_end().
        """

        ...

    def g_static_function_support_vars(self, sf, scope):
        """ Generate the variables needed by a static function implementation.
        """

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
    def get_enum_to_py_conversion(self, enum, value_name):
        """ Return the code to convert a C/C++ enum to a Python object. """

        ...

    @abstractmethod
    def get_enum_ref_value(self, enum):
        """ Return the value of an enum's reference. """

        ...

    @staticmethod
    def get_module_context():
        """ Return the value of a module context passed as the first argument
        to many ABI calls.
        """

        # This default implementation returns nothing.
        return ''

    @staticmethod
    def get_module_context_decl():
        """ Return the declaration of the value of a module context passed as
        the first argument to many ABI calls.
        """

        # This default implementation returns nothing.
        return ''

    @abstractmethod
    def get_py_method_args(self, *, is_impl, self_is_type, need_self=False,
            need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        ...

    @staticmethod
    @abstractmethod
    def get_raise_unknown_exception():
        """ Return the call to raise an exception about an unknown exception.
        """

        ...

    @staticmethod
    @abstractmethod
    def get_result_parser():
        """ Return the name of the Python reimplementation result parser. """

        ...

    @abstractmethod
    def get_sipself_test(self, klass):
        """ Return the code that checks if 'sipSelf' was bound or passed as an
        argument.
        """

        ...

    @staticmethod
    @abstractmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        ...

    @abstractmethod
    def get_spec_for_class(self, klass):
        """ Return the name of the data structure specifying a class. """

        ...

    @abstractmethod
    def get_spec_for_enum(self, enum, enums_closure):
        """ Return the name of the data structure specifying an enum. """

        ...

    def get_spec_for_exception(self, exception):
        """ Return the name of the data structure specifying an exception. """

        # This default implementation does nothing.
        pass

    @abstractmethod
    def get_spec_for_mapped_type(self, mapped_type):
        """ Return the name of the data structure specifying a mapped type. """

        ...

    @staticmethod
    @abstractmethod
    def get_spec_suffix():
        """ Return the suffix used for immutable specifications. """

        ...

    @staticmethod
    @abstractmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        ...

    @staticmethod
    @abstractmethod
    def get_types_table_decl(module):
        """ Return the declaration of a module's wrapped types table. """

        ...

    @staticmethod
    @abstractmethod
    def get_wrapper_type():
        """ Return the type of the C representation of a wrapped object. """

        ...

    @staticmethod
    @abstractmethod
    def get_wrapper_type_cast():
        """ Return the cast from a PyObject* of the C representation of a
        wrapped object.
        """

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
