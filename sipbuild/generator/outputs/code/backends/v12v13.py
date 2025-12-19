# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from ....scoped_name import STRIP_GLOBAL
from ....specification import (ArgumentType, IfaceFileType, WrappedClass,
        WrappedEnum)
from ....utils import find_method

from ...formatters import fmt_argument_as_cpp_type

from ..snippets import (g_class_docstring, g_class_method_table,
        g_enum_member_table, g_module_docstring, g_type_init_body,
        g_pyqt_class_plugin, g_pyqt_helper_defns, g_pyqt_helper_init)
from ..utils import (get_class_flags, get_const_cast, get_docstring_text,
        get_encoded_type, get_enum_member, get_named_value_decl,
        get_normalised_cached_name, get_optional_ptr, get_use_in_code,
        get_user_state_suffix, get_void_ptr_cast, has_method_docstring,
        is_used_in_code, keep_py_reference, need_dealloc, py_scope,
        pyqt5_supported, pyqt6_supported, scoped_class_name,
        scoped_variable_name, type_needs_user_state, variables_in_scope)

from .abstract_backend import AbstractBackend


class v12v13Backend(AbstractBackend):
    """ The backend code generator for v12 and v13 of the ABI. """

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

        spec = self.spec
        target_abi = spec.target_abi
        module = spec.module
        module_name = module.py_name
        fq_py_name_ref = self.cached_name_ref(module.fq_py_name, as_nr=True)

        imports_table = get_optional_ptr(len(module.all_imports) != 0,
                'importsTable')
        exported_types = get_optional_ptr(len(module.needed_types) != 0,
                'sipExportedTypes_' + module_name)
        external_types = get_optional_ptr(has_external, 'externalTypesTable')
        typedefs_table = get_optional_ptr(module.nr_typedefs != 0,
                'typedefsTable')

        sf.write(
f'''/* This defines this module. */
sipExportedModuleDef sipModuleAPI_{module_name} = {{
    SIP_NULLPTR,
    {target_abi[1]},
    {fq_py_name_ref},
    0,
    sipStrings_{module_name},
    {imports_table},
''')

        if target_abi < (13, 0):
            qt_api = get_optional_ptr(self.legacy_qt_support(), '&qtAPI')
            sf.write(f'    {qt_api},\n')

        sf.write(
f'''    {len(module.needed_types)},
    {exported_types},
    {external_types},
''')

        if self.custom_enums_supported():
            enum_members = get_optional_ptr(nr_enum_members > 0,
                    'enummembers')
            sf.write(
f'''    {nr_enum_members},
    {enum_members},
''')

        veh_table = get_optional_ptr(has_virtual_error_handlers,
                'virtErrorHandlersTable')
        convertors = get_optional_ptr(nr_subclass_convertors > 0,
                'convertorsTable')
        type_instances = get_optional_ptr('class' in inst_state,
                'typeInstances')
        void_ptr_instances = get_optional_ptr('voidp' in inst_state,
                'voidPtrInstances')
        char_instances = get_optional_ptr('char' in inst_state,
                'charInstances')
        string_instances = get_optional_ptr('string' in inst_state,
                'stringInstances')
        int_instances = get_optional_ptr('int' in inst_state, 'intInstances')
        long_instances = get_optional_ptr('long' in inst_state,
                'longInstances')
        unsigned_long_instances = get_optional_ptr('ulong' in inst_state,
                'unsignedLongInstances')
        long_long_instances = get_optional_ptr('longlong' in inst_state,
                'longLongInstances')
        unsigned_long_long_instances = get_optional_ptr(
                'ulonglong' in inst_state, 'unsignedLongLongInstances')
        double_instances = get_optional_ptr('double' in inst_state,
                'doubleInstances')
        module_license = get_optional_ptr(module.license is not None,
                '&module_license')
        exported_exceptions = get_optional_ptr(module.nr_exceptions > 0,
                'sipExportedExceptions_' + module_name)
        slot_extender_table = get_optional_ptr(slot_extenders,
                'slotExtenders')
        init_extender_table = get_optional_ptr(init_extenders,
                'initExtenders')
        delayed_dtors = get_optional_ptr(module.has_delayed_dtors,
                'sipDelayedDtors')

        sf.write(
f'''    {module.nr_typedefs},
    {typedefs_table},
    {veh_table},
    {convertors},
    {{{type_instances}, {void_ptr_instances}, {char_instances}, {string_instances}, {int_instances}, {long_instances}, {unsigned_long_instances}, {long_long_instances}, {unsigned_long_long_instances}, {double_instances}}},
    {module_license},
    {exported_exceptions},
    {slot_extender_table},
    {init_extender_table},
    {delayed_dtors},
    SIP_NULLPTR,
''')

        if target_abi < (13, 0):
            # The unused version support.
            sf.write(
'''    SIP_NULLPTR,
    SIP_NULLPTR,
''')

        exception_handler = get_optional_ptr(
                (self.abi_has_next_exception_handler() and bindings.exceptions and module.nr_exceptions > 0),
                'sipExceptionHandler_' + module_name)

        sf.write(
f'''    {exception_handler},
}};
''')

        g_module_docstring(sf, module)

        # Generate the storage for the external API pointers.
        sf.write(
f'''

/* The SIP API and the APIs of any imported modules. */
const sipAPIDef *sipAPI_{module_name};
''')

        g_pyqt_helper_defns(sf, spec)
        self.g_module_init_start(sf)
        has_module_functions = self.g_module_functions_table(sf, bindings,
                module)
        self.g_module_definition(sf, has_module_functions=has_module_functions)
        self._g_module_init_body(sf)

    def g_enum_macros(self, sf, scope=None, imported_module=None):
        """ Generate the type macros for enums. """

        spec = self.spec

        for enum in spec.enums:
            if enum.fq_cpp_name is None:
                continue

            # Continue unless the scopes match.
            if scope is not None:
                if enum.scope is not scope:
                    continue
            elif enum.scope is not None:
                continue

            value = None

            if imported_module is None:
                if enum.module is spec.module:
                    value = f'sipExportedTypes_{spec.module.py_name}[{enum.type_nr}]'
            elif enum.module is imported_module and enum.needed:
                value = f'sipImportedTypes_{spec.module.py_name}_{enum.module.py_name}[{enum.type_nr}].it_td'

            if value is not None:
                sf.write(f'\n#define {self.get_type_ref(enum)} {value}\n')

    def g_init_mixin_impl_body(self, sf, klass):
        """ Generate the body of the implementation of a mixin initialisation
        function.
        """

        module_name = self.spec.module.py_name
        klass_name = klass.iface_file.fq_cpp_name.as_word

        sf.write(f'    return sipInitMixin(sipSelf, sipArgs, sipKwds, (sipClassTypeDef *)&sipTypeDef_{module_name}_{klass_name});\n')

    def g_mapped_type_api(self, sf, mapped_type):
        """ Generate the API details for a mapped type. """

        spec = self.spec
        iface_file = mapped_type.iface_file

        module_name = spec.module.py_name
        mapped_type_name = iface_file.fq_cpp_name.as_word

        sf.write(
f'''
#define {self.get_type_ref(mapped_type)} sipExportedTypes_{module_name}[{iface_file.type_nr}]

extern sipMappedTypeDef sipTypeDef_{module_name}_{mapped_type_name};
''')

        self.g_enum_macros(sf, scope=mapped_type)

    def g_mapped_type_int_instances(self, sf, mapped_type):
        """ Generate the code to add a set of ints to a mapped type.  Return
        True if there was at least one.
        """

        return self._g_instances_int(sf, mapped_type)

    def g_module_definition(self, sf, has_module_functions=False):
        """ Generate the module definition structure. """

        module = self.spec.module

        docstring_ref = get_optional_ptr(module.docstring is not None,
                'doc_mod_' + module.py_name)
        method_table = get_optional_ptr(has_module_functions, 'sip_methods')

        sf.write(
f'''    static PyModuleDef sip_module_def = {{
        PyModuleDef_HEAD_INIT,
        "{module.fq_py_name}",
        {docstring_ref},
        -1,
        {method_table},
        SIP_NULLPTR,
        SIP_NULLPTR,
        SIP_NULLPTR,
        SIP_NULLPTR
    }};
''')

    def g_module_functions_table(self, sf, bindings, module):
        """ Generate the table of module functions and return True if anything
        was actually generated.
        """

        # We always generate a table.
        sf.write('    static PyMethodDef sip_methods[] = {\n')

        self._g_module_function_table_entries(sf, bindings, module,
                module.global_functions)

        # Generate the global functions for any hidden namespaces.
        for klass in self.spec.classes:
            if klass.iface_file.module is module and klass.is_hidden_namespace:
                self._g_module_function_table_entries(sf, bindings, module,
                        klass.members)

        sf.write(
'''        {SIP_NULLPTR, SIP_NULLPTR, 0, SIP_NULLPTR}
    };
''')

        return True

    def g_module_init_start(self, sf):
        """ Generate the start of the Python module initialisation function.
        """

        spec = self.spec

        if spec.is_composite or spec.c_bindings:
            extern_c = ''
            arg_type = 'void'
        else:
            extern_c = 'extern "C" '
            arg_type = ''

        module_name = spec.module.py_name

        sf.write(
f'''

/* The Python module initialisation function. */
#if defined(SIP_STATIC_MODULE)
{extern_c}PyObject *PyInit_{module_name}({arg_type})
#else
PyMODINIT_FUNC PyInit_{module_name}({arg_type})
#endif
{{
''')

    def g_py_method_table(self, sf, bindings, members, scope):
        """ Generate a Python method table for a class or mapped type and
        return the number of entries.
        """

        scope_name = scope.iface_file.fq_cpp_name.as_word

        no_intro = True

        for member_nr, member in enumerate(members):
            # Save the index in the table.
            member.member_nr = member_nr

            py_name = member.py_name
            cached_py_name = self.cached_name_ref(py_name)
            comma = '' if member is members[-1] else ','

            if member.no_arg_parser or member.allow_keyword_args:
                cast = 'SIP_MLMETH_CAST('
                cast_suffix = ')'
                flags = '|METH_KEYWORDS'
            else:
                cast = ''
                cast_suffix = ''
                flags = ''

            if has_method_docstring(bindings, member, scope.overloads):
                docstring = f'doc_{scope_name}_{py_name.name}'
            else:
                docstring = 'SIP_NULLPTR'

            if no_intro:
                sf.write(
f'''

static PyMethodDef methods_{scope_name}[] = {{
''')

                no_intro = False

            sf.write(f'    {{{cached_py_name}, {cast}meth_{scope_name}_{py_name.name}{cast_suffix}, METH_VARARGS{flags}, {docstring}}}{comma}\n')

        if not no_intro:
            sf.write('};\n')

        return len(members)

    def g_sip_api(self, sf, module_name):
        """ Generate the SIP API as seen by generated code. """

        spec = self.spec

        sf.write(
f'''
#define sipMalloc                   sipAPI_{module_name}->api_malloc
#define sipFree                     sipAPI_{module_name}->api_free
#define sipBuildResult              sipAPI_{module_name}->api_build_result
#define sipCallMethod               sipAPI_{module_name}->api_call_method
#define sipCallProcedureMethod      sipAPI_{module_name}->api_call_procedure_method
#define sipCallErrorHandler         sipAPI_{module_name}->api_call_error_handler
#define sipParseResultEx            sipAPI_{module_name}->api_parse_result_ex
#define sipParseResult              sipAPI_{module_name}->api_parse_result
#define sipParseArgs                sipAPI_{module_name}->api_parse_args
#define sipParseKwdArgs             sipAPI_{module_name}->api_parse_kwd_args
#define sipParsePair                sipAPI_{module_name}->api_parse_pair
#define sipInstanceDestroyed        sipAPI_{module_name}->api_instance_destroyed
#define sipInstanceDestroyedEx      sipAPI_{module_name}->api_instance_destroyed_ex
#define sipConvertFromSequenceIndex sipAPI_{module_name}->api_convert_from_sequence_index
#define sipConvertFromSliceObject   sipAPI_{module_name}->api_convert_from_slice_object
#define sipConvertFromVoidPtr       sipAPI_{module_name}->api_convert_from_void_ptr
#define sipConvertToVoidPtr         sipAPI_{module_name}->api_convert_to_void_ptr
#define sipAddException             sipAPI_{module_name}->api_add_exception
#define sipNoFunction               sipAPI_{module_name}->api_no_function
#define sipNoMethod                 sipAPI_{module_name}->api_no_method
#define sipAbstractMethod           sipAPI_{module_name}->api_abstract_method
#define sipBadClass                 sipAPI_{module_name}->api_bad_class
#define sipBadCatcherResult         sipAPI_{module_name}->api_bad_catcher_result
#define sipBadCallableArg           sipAPI_{module_name}->api_bad_callable_arg
#define sipBadOperatorArg           sipAPI_{module_name}->api_bad_operator_arg
#define sipTrace                    sipAPI_{module_name}->api_trace
#define sipTransferBack             sipAPI_{module_name}->api_transfer_back
#define sipTransferTo               sipAPI_{module_name}->api_transfer_to
#define sipSimpleWrapper_Type       sipAPI_{module_name}->api_simplewrapper_type
#define sipWrapper_Type             sipAPI_{module_name}->api_wrapper_type
#define sipWrapperType_Type         sipAPI_{module_name}->api_wrappertype_type
#define sipVoidPtr_Type             sipAPI_{module_name}->api_voidptr_type
#define sipGetPyObject              sipAPI_{module_name}->api_get_pyobject
#define sipGetAddress               sipAPI_{module_name}->api_get_address
#define sipGetMixinAddress          sipAPI_{module_name}->api_get_mixin_address
#define sipGetCppPtr                sipAPI_{module_name}->api_get_cpp_ptr
#define sipGetComplexCppPtr         sipAPI_{module_name}->api_get_complex_cpp_ptr
#define sipCallHook                 sipAPI_{module_name}->api_call_hook
#define sipEndThread                sipAPI_{module_name}->api_end_thread
#define sipRaiseUnknownException    sipAPI_{module_name}->api_raise_unknown_exception
#define sipRaiseTypeException       sipAPI_{module_name}->api_raise_type_exception
#define sipBadLengthForSlice        sipAPI_{module_name}->api_bad_length_for_slice
#define sipAddTypeInstance          sipAPI_{module_name}->api_add_type_instance
#define sipPySlotExtend             sipAPI_{module_name}->api_pyslot_extend
#define sipAddDelayedDtor           sipAPI_{module_name}->api_add_delayed_dtor
#define sipCanConvertToType         sipAPI_{module_name}->api_can_convert_to_type
#define sipConvertToType            sipAPI_{module_name}->api_convert_to_type
#define sipForceConvertToType       sipAPI_{module_name}->api_force_convert_to_type
#define sipConvertToEnum            sipAPI_{module_name}->api_convert_to_enum
#define sipConvertToBool            sipAPI_{module_name}->api_convert_to_bool
#define sipReleaseType              sipAPI_{module_name}->api_release_type
#define sipConvertFromType          sipAPI_{module_name}->api_convert_from_type
#define sipConvertFromNewType       sipAPI_{module_name}->api_convert_from_new_type
#define sipConvertFromNewPyType     sipAPI_{module_name}->api_convert_from_new_pytype
#define sipConvertFromEnum          sipAPI_{module_name}->api_convert_from_enum
#define sipGetState                 sipAPI_{module_name}->api_get_state
#define sipExportSymbol             sipAPI_{module_name}->api_export_symbol
#define sipImportSymbol             sipAPI_{module_name}->api_import_symbol
#define sipFindType                 sipAPI_{module_name}->api_find_type
#define sipBytes_AsChar             sipAPI_{module_name}->api_bytes_as_char
#define sipBytes_AsString           sipAPI_{module_name}->api_bytes_as_string
#define sipString_AsASCIIChar       sipAPI_{module_name}->api_string_as_ascii_char
#define sipString_AsASCIIString     sipAPI_{module_name}->api_string_as_ascii_string
#define sipString_AsLatin1Char      sipAPI_{module_name}->api_string_as_latin1_char
#define sipString_AsLatin1String    sipAPI_{module_name}->api_string_as_latin1_string
#define sipString_AsUTF8Char        sipAPI_{module_name}->api_string_as_utf8_char
#define sipString_AsUTF8String      sipAPI_{module_name}->api_string_as_utf8_string
#define sipUnicode_AsWChar          sipAPI_{module_name}->api_unicode_as_wchar
#define sipUnicode_AsWString        sipAPI_{module_name}->api_unicode_as_wstring
#define sipConvertFromConstVoidPtr  sipAPI_{module_name}->api_convert_from_const_void_ptr
#define sipConvertFromVoidPtrAndSize    sipAPI_{module_name}->api_convert_from_void_ptr_and_size
#define sipConvertFromConstVoidPtrAndSize   sipAPI_{module_name}->api_convert_from_const_void_ptr_and_size
#define sipWrappedTypeName(wt)      ((wt)->wt_td->td_cname)
#define sipGetReference             sipAPI_{module_name}->api_get_reference
#define sipKeepReference            sipAPI_{module_name}->api_keep_reference
#define sipRegisterProxyResolver    sipAPI_{module_name}->api_register_proxy_resolver
#define sipRegisterPyType           sipAPI_{module_name}->api_register_py_type
#define sipTypeFromPyTypeObject     sipAPI_{module_name}->api_type_from_py_type_object
#define sipTypeScope                sipAPI_{module_name}->api_type_scope
#define sipResolveTypedef           sipAPI_{module_name}->api_resolve_typedef
#define sipRegisterAttributeGetter  sipAPI_{module_name}->api_register_attribute_getter
#define sipEnableAutoconversion     sipAPI_{module_name}->api_enable_autoconversion
#define sipInitMixin                sipAPI_{module_name}->api_init_mixin
#define sipExportModule             sipAPI_{module_name}->api_export_module
#define sipInitModule               sipAPI_{module_name}->api_init_module
#define sipGetInterpreter           sipAPI_{module_name}->api_get_interpreter
#define sipSetTypeUserData          sipAPI_{module_name}->api_set_type_user_data
#define sipGetTypeUserData          sipAPI_{module_name}->api_get_type_user_data
#define sipPyTypeDict               sipAPI_{module_name}->api_py_type_dict
#define sipPyTypeName               sipAPI_{module_name}->api_py_type_name
#define sipGetCFunction             sipAPI_{module_name}->api_get_c_function
#define sipGetMethod                sipAPI_{module_name}->api_get_method
#define sipFromMethod               sipAPI_{module_name}->api_from_method
#define sipGetDate                  sipAPI_{module_name}->api_get_date
#define sipFromDate                 sipAPI_{module_name}->api_from_date
#define sipGetDateTime              sipAPI_{module_name}->api_get_datetime
#define sipFromDateTime             sipAPI_{module_name}->api_from_datetime
#define sipGetTime                  sipAPI_{module_name}->api_get_time
#define sipFromTime                 sipAPI_{module_name}->api_from_time
#define sipIsUserType               sipAPI_{module_name}->api_is_user_type
#define sipCheckPluginForType       sipAPI_{module_name}->api_check_plugin_for_type
#define sipUnicodeNew               sipAPI_{module_name}->api_unicode_new
#define sipUnicodeWrite             sipAPI_{module_name}->api_unicode_write
#define sipUnicodeData              sipAPI_{module_name}->api_unicode_data
#define sipGetBufferInfo            sipAPI_{module_name}->api_get_buffer_info
#define sipReleaseBufferInfo        sipAPI_{module_name}->api_release_buffer_info
#define sipIsOwnedByPython          sipAPI_{module_name}->api_is_owned_by_python
#define sipIsDerivedClass           sipAPI_{module_name}->api_is_derived_class
#define sipGetUserObject            sipAPI_{module_name}->api_get_user_object
#define sipSetUserObject            sipAPI_{module_name}->api_set_user_object
#define sipRegisterEventHandler     sipAPI_{module_name}->api_register_event_handler
#define sipConvertToArray           sipAPI_{module_name}->api_convert_to_array
#define sipConvertToTypedArray      sipAPI_{module_name}->api_convert_to_typed_array
#define sipEnableGC                 sipAPI_{module_name}->api_enable_gc
#define sipPrintObject              sipAPI_{module_name}->api_print_object
#define sipLong_AsChar              sipAPI_{module_name}->api_long_as_char
#define sipLong_AsSignedChar        sipAPI_{module_name}->api_long_as_signed_char
#define sipLong_AsUnsignedChar      sipAPI_{module_name}->api_long_as_unsigned_char
#define sipLong_AsShort             sipAPI_{module_name}->api_long_as_short
#define sipLong_AsUnsignedShort     sipAPI_{module_name}->api_long_as_unsigned_short
#define sipLong_AsInt               sipAPI_{module_name}->api_long_as_int
#define sipLong_AsUnsignedInt       sipAPI_{module_name}->api_long_as_unsigned_int
#define sipLong_AsLong              sipAPI_{module_name}->api_long_as_long
#define sipLong_AsUnsignedLong      sipAPI_{module_name}->api_long_as_unsigned_long
#define sipLong_AsLongLong          sipAPI_{module_name}->api_long_as_long_long
#define sipLong_AsUnsignedLongLong  sipAPI_{module_name}->api_long_as_unsigned_long_long
#define sipLong_AsSizeT             sipAPI_{module_name}->api_long_as_size_t
#define sipVisitWrappers            sipAPI_{module_name}->api_visit_wrappers
#define sipRegisterExitNotifier     sipAPI_{module_name}->api_register_exit_notifier
''')

        # These are dependent on the specific ABI version.
        if spec.target_abi >= (13, 0):
            if spec.target_abi >= (13, 9):
                # ABI v13.9 and later.
                sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated_13_9
''')
            else:
                sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated
''')

            # ABI v13.6 and later.
            if spec.target_abi >= (13, 6):
                sf.write(
f'''#define sipPyTypeDictRef            sipAPI_{module_name}->api_py_type_dict_ref
''')

            # ABI v13.1 and later.
            if spec.target_abi >= (13, 1):
                sf.write(
f'''#define sipNextExceptionHandler     sipAPI_{module_name}->api_next_exception_handler
''')

            sf.write(
f'''#define sipIsEnumFlag               sipAPI_{module_name}->api_is_enum_flag
#define sipConvertToTypeUS          sipAPI_{module_name}->api_convert_to_type_us
#define sipForceConvertToTypeUS     sipAPI_{module_name}->api_force_convert_to_type_us
#define sipReleaseTypeUS            sipAPI_{module_name}->api_release_type_us
''')
        else:
            # ABI v12.16 and later
            if spec.target_abi >= (12, 16):
                sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated_12_16
''')
            else:
                sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated
''')

            # ABI v12.13 and later.
            if spec.target_abi >= (12, 13):
                sf.write(
f'''#define sipPyTypeDictRef            sipAPI_{module_name}->api_py_type_dict_ref
''')

            # ABI v12.9 and later.
            if spec.target_abi >= (12, 9):
                sf.write(
f'''#define sipNextExceptionHandler     sipAPI_{module_name}->api_next_exception_handler
''')

            # ABI v12.8 and earlier.
            sf.write(
f'''#define sipSetNewUserTypeHandler    sipAPI_{module_name}->api_set_new_user_type_handler
#define sipGetFrame                 sipAPI_{module_name}->api_get_frame
#define sipSetDestroyOnExit         sipAPI_{module_name}->api_set_destroy_on_exit
#define sipEnableOverflowChecking   sipAPI_{module_name}->api_enable_overflow_checking
#define sipIsAPIEnabled             sipAPI_{module_name}->api_is_api_enabled
#define sipClearAnySlotReference    sipAPI_{module_name}->api_clear_any_slot_reference
#define sipConnectRx                sipAPI_{module_name}->api_connect_rx
#define sipConvertRx                sipAPI_{module_name}->api_convert_rx
#define sipDisconnectRx             sipAPI_{module_name}->api_disconnect_rx
#define sipFreeSipslot              sipAPI_{module_name}->api_free_sipslot
#define sipInvokeSlot               sipAPI_{module_name}->api_invoke_slot
#define sipInvokeSlotEx             sipAPI_{module_name}->api_invoke_slot_ex
#define sipSameSlot                 sipAPI_{module_name}->api_same_slot
#define sipSaveSlot                 sipAPI_{module_name}->api_save_slot
#define sipVisitSlot                sipAPI_{module_name}->api_visit_slot
''')

        if spec.target_abi >= (12, 8):
            # ABI v12.8 and later.
            sf.write(
f'''#define sipIsPyMethod               sipAPI_{module_name}->api_is_py_method_12_8
''')
        else:
            # ABI v12.7 and earlier.
            sf.write(
f'''#define sipIsPyMethod               sipAPI_{module_name}->api_is_py_method
''')

    def g_static_variables_table(self, sf, scope=None):
        """ Generate the tables of static variables for a scope and return a
        set of strings corresponding to the tables actually generated.
        """

        inst_state = set()

        if self._g_instances_class(sf, scope):
            inst_state.add('class')

        if self._g_instances_voidp(sf, scope):
            inst_state.add('voidp')

        if self._g_instances_char(sf, scope):
            inst_state.add('char')

        if self._g_instances_string(sf, scope):
            inst_state.add('string')

        if self._g_instances_int(sf, scope):
            inst_state.add('int')

        if self._g_instances_long(sf, scope):
            inst_state.add('long')

        if self._g_instances_ulong(sf, scope):
            inst_state.add('ulong')

        if self._g_instances_longlong(sf, scope):
            inst_state.add('longlong')

        if self._g_instances_ulonglong(sf, scope):
            inst_state.add('ulonglong')

        if self._g_instances_double(sf, scope):
            inst_state.add('double')

        return inst_state

    def g_type_definition(self, sf, bindings, klass, py_debug):
        """ Generate the type structure that contains all the information
        needed by the meta-type.  A sub-set of this is used to extend
        namespaces.
        """

        spec = self.spec
        module = spec.module
        klass_name = klass.iface_file.fq_cpp_name.as_word

        # The super-types table.
        if len(klass.superclasses) != 0:
            encoded_types = []

            for superclass in klass.superclasses:
                last = superclass is klass.superclasses[-1]
                encoded_types.append(
                        get_encoded_type(module, superclass, last=last))

            encoded_types = ', '.join(encoded_types)

            sf.write(
f'''

/* Define this type's super-types. */
static sipEncodedTypeDef supers_{klass_name}[] = {{{encoded_types}}};
''')

        # The slots table.
        is_slots = False

        for member in klass.members:
            if member.py_slot is None:
                continue

            if not is_slots:
                sf.write(
f'''

/* Define this type's Python slots. */
static sipPySlotDef slots_{klass_name}[] = {{
''')

                is_slots = True

            slot_ref = self.get_slot_ref(member.py_slot)
            member_name = member.py_name
            sf.write(f'    {{(void *)slot_{klass_name}_{member_name}, {slot_ref}}},\n')

        if is_slots:
            sf.write('    {0, (sipPySlotType)0}\n};\n')

        # The attributes tables.
        nr_methods = g_class_method_table(self, sf, bindings, klass)

        if self.custom_enums_supported():
            nr_enum_members = g_enum_member_table(self, sf, scope=klass)
        else:
            nr_enum_members = -1

        # The property and variable handlers.
        nr_variables = 0

        if klass.has_variable_handlers:
            for variable in spec.variables:
                if variable.scope is klass and variable.needs_handler:
                    nr_variables += 1

                    self._g_variable_getter(sf, variable)

                    if _can_set_variable(variable):
                        self._g_variable_setter(sf, variable)

        # Generate any property docstrings.
        for prop in klass.properties:
            nr_variables += 1

            if prop.docstring is not None:
                docstring = get_docstring_text(prop.docstring)
                sf.write(f'\nPyDoc_STRVAR(doc_{klass_name}_{prop.name}, "{docstring}");\n')

        # The variables table.
        if nr_variables != 0:
            sf.write(f'\nsipVariableDef variables_{klass_name}[] = {{\n')

        for prop in klass.properties:
            fields = ['PropertyVariable', self.cached_name_ref(prop.name)]

            getter_nr = find_method(klass, prop.getter).member_nr
            fields.append(f'&methods_{klass_name}[{getter_nr}]')

            if prop.setter is None:
                fields.append('SIP_NULLPTR')
            else:
                setter_nr = find_method(klass, prop.setter).member_nr
                fields.append(f'&methods_{klass_name}[{setter_nr}]')

            # We don't support a deleter yet.
            fields.append('SIP_NULLPTR')

            if prop.docstring is None:
                fields.append('SIP_NULLPTR')
            else:
                fields.append(f'doc_{klass_name}_{prop.name}')

            fields = ', '.join(fields)
            sf.write(f'    {{{fields}}},\n')

        if klass.has_variable_handlers:
            for variable in spec.variables:
                if variable.scope is klass and variable.needs_handler:
                    variable_name = variable.fq_cpp_name.as_word

                    fields = []

                    fields.append('ClassVariable' if variable.is_static else 'InstanceVariable')
                    fields.append(self.cached_name_ref(variable.py_name))
                    fields.append('(PyMethodDef *)varget_' + variable_name)

                    if _can_set_variable(variable):
                        fields.append('(PyMethodDef *)varset_' + variable_name)
                    else:
                        fields.append('SIP_NULLPTR')

                    fields.append('SIP_NULLPTR')
                    fields.append('SIP_NULLPTR')

                    fields = ', '.join(fields)
                    sf.write(f'    {{{fields}}},\n')

        if nr_variables != 0:
            sf.write('};\n')

        # Generate the static variables table.
        sv_state = self.g_static_variables_table(sf, scope=klass)

        # Generate the docstring.
        docstring_ref = g_class_docstring(sf, spec, bindings, klass)

        # Generate any plugin-specific data structures.
        plugin_ref = 'SIP_NULLPTR'

        if pyqt5_supported(spec) or pyqt6_supported(spec):
            if g_pyqt_class_plugin(self, sf, bindings, klass):
                plugin_ref = '&plugin_' + klass_name

        # The type definition structure itself.
        base_fields = []
        container_fields = []
        class_fields = []

        if spec.target_abi < (13, 0):
            base_fields.append('-1')
            base_fields.append('SIP_NULLPTR')

        base_fields.append('SIP_NULLPTR')
        base_fields.append(get_class_flags(spec, klass, py_debug))
        base_fields.append(self.cached_name_ref(klass.iface_file.cpp_name,
                as_nr=True))
        base_fields.append('SIP_NULLPTR')
        base_fields.append(plugin_ref)

        container_fields.append(
                self.cached_name_ref(klass.py_name, as_nr=True) if klass.real_class is None else '-1')

        if klass.real_class is not None:
            encoded_type = get_encoded_type(module, klass.real_class)
        elif py_scope(klass.scope) is not None:
            encoded_type = get_encoded_type(module, klass.scope)
        else:
            encoded_type = '{0, 0, 1}'

        container_fields.append(encoded_type)

        if nr_methods == 0:
            container_fields.append('0, SIP_NULLPTR')
        else:
            container_fields.append(
                    str(nr_methods) + ', methods_' + klass_name)

        if nr_enum_members == 0:
            container_fields.append('0, SIP_NULLPTR')
        elif nr_enum_members > 0:
            container_fields.append(
                    str(nr_enum_members) + ', enummembers_' + klass_name)

        if nr_variables == 0:
            container_fields.append('0, SIP_NULLPTR')
        else:
            container_fields.append(
                    str(nr_variables) + ', variables_' + klass_name)

        instances = []

        instances.append(
                _class_object_ref('class' in sv_state, 'typeInstances',
                        klass_name))
        instances.append(
                _class_object_ref('voidp' in sv_state, 'voidPtrInstances',
                        klass_name))
        instances.append(
                _class_object_ref('char' in sv_state, 'charInstances',
                        klass_name))
        instances.append(
                _class_object_ref('string' in sv_state, 'stringInstances',
                        klass_name))
        instances.append(
                _class_object_ref('int' in sv_state, 'intInstances',
                        klass_name))
        instances.append(
                _class_object_ref('long' in sv_state, 'longInstances',
                        klass_name))
        instances.append(
                _class_object_ref('ulong' in sv_state, 'unsignedLongInstances',
                        klass_name))
        instances.append(
                _class_object_ref('longlong' in sv_state, 'longLongInstances',
                        klass_name))
        instances.append(
                _class_object_ref('ulonglong' in sv_state,
                        'unsignedLongLongInstances', klass_name))
        instances.append(
                _class_object_ref('double' in sv_state, 'doubleInstances',
                        klass_name))

        container_fields.append('{' + ', '.join(instances) + '}')

        class_fields.append(docstring_ref)
        class_fields.append(
                self.cached_name_ref(klass.metatype, as_nr=True) if klass.metatype is not None else '-1')
        class_fields.append(
                self.cached_name_ref(klass.supertype, as_nr=True) if klass.supertype is not None else '-1')
        class_fields.append(
                _class_object_ref((len(klass.superclasses) != 0), 'supers',
                        klass_name))
        class_fields.append(_class_object_ref(is_slots, 'slots', klass_name))
        class_fields.append(
                _class_object_ref(klass.can_create, 'init_type', klass_name))
        class_fields.append(
                _class_object_ref((klass.gc_traverse_code is not None),
                        'traverse', klass_name))
        class_fields.append(
                _class_object_ref((klass.gc_clear_code is not None), 'clear',
                        klass_name))
        class_fields.append(
                _class_object_ref((klass.bi_get_buffer_code is not None),
                        'getbuffer', klass_name))
        class_fields.append(
                _class_object_ref((klass.bi_release_buffer_code is not None),
                        'releasebuffer', klass_name))
        class_fields.append(
                _class_object_ref(need_dealloc(spec, bindings, klass),
                        'dealloc', klass_name))
        class_fields.append(
                _class_object_ref((spec.c_bindings or klass.needs_copy_helper),
                        'assign', klass_name))
        class_fields.append(
                _class_object_ref(
                        (spec.c_bindings or klass.needs_array_helper), 'array',
                        klass_name))
        class_fields.append(
                _class_object_ref((spec.c_bindings or klass.needs_copy_helper),
                        'copy', klass_name))
        class_fields.append(
                _class_object_ref(
                        (not spec.c_bindings and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'release', klass_name))
        class_fields.append(
                _class_object_ref((len(klass.superclasses) != 0), 'cast',
                        klass_name))
        class_fields.append(
                _class_object_ref(
                        (klass.convert_to_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'convertTo', klass_name))
        class_fields.append(
                _class_object_ref(
                        (klass.convert_from_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'convertFrom', klass_name))
        class_fields.append('SIP_NULLPTR')
        class_fields.append(
                _class_object_ref((klass.pickle_code is not None), 'pickle',
                        klass_name))
        class_fields.append(
                _class_object_ref((klass.finalisation_code is not None),
                        'final', klass_name))
        class_fields.append(
                _class_object_ref(klass.mixin, 'mixin', klass_name))

        if self.abi_supports_array():
            class_fields.append(
                    _class_object_ref(
                            (spec.c_bindings or klass.needs_array_helper),
                            'array_delete', klass_name))

            if klass.can_create:
                class_fields.append(
                        f'sizeof ({scoped_class_name(spec, klass)})')
            else:
                class_fields.append('0')

        base_fields = ',\n        '.join(base_fields)
        container_fields = ',\n        '.join(container_fields)
        class_fields = ',\n    '.join(class_fields)

        sf.write(
f'''

sipClassTypeDef sipTypeDef_{module.py_name}_{klass_name} = {{
    {{
        {base_fields},
    }},
    {{
        {container_fields},
    }},
    {class_fields},
}};
''')

    def g_type_init(self, sf, bindings, klass, need_self, need_owner):
        """ Generate the code that initialises a type. """

        spec = self.spec
        klass_name = klass.iface_file.fq_cpp_name.as_word

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *init_type_{klass_name}(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}}\n')

        sip_self = 'sipSelf' if need_self else ''
        sip_owner = 'sipOwner' if need_owner else ''

        sf.write(
f'''static void *init_type_{klass_name}(sipSimpleWrapper *{sip_self}, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **{sip_owner}, PyObject **sipParseErr)
{{
''')

        g_type_init_body(self, sf, bindings, klass)

        sf.write('}\n')

    def abi_has_deprecated_message(self):
        """ Return True if the ABI implements sipDeprecated() with a message.
        """

        return self._abi_version_check((12, 16), (13, 9))

    def abi_has_next_exception_handler(self):
        """ Return True if the ABI implements sipNextExceptionHandler(). """

        return self._abi_version_check((12, 9), (13, 1))

    def abi_has_working_char_conversion(self):
        """ Return True if the ABI has working char to/from a Python integer
        converters (ie. char is not assumed to be signed).
        """

        return self._abi_version_check((12, 15), (13, 8))

    def abi_supports_array(self):
        """ Return True if the ABI supports sip.array. """

        return self._abi_version_check((12, 11), (13, 4))

    @staticmethod
    def cached_name_ref(cached_name, as_nr=False):
        """ Return a reference to a cached name. """

        prefix = 'sipNameNr_' if as_nr else 'sipName_'

        return prefix + get_normalised_cached_name(cached_name)

    def custom_enums_supported(self):
        """ Return True if custom enums are supported. """

        return self.spec.target_abi[0] < 13

    def get_class_ref_value(self, klass):
        """ Return the value of a class's reference. """

        module_name = self.spec.module.py_name
        iface_file = klass.iface_file

        return f'sipExportedTypes_{module_name}[{iface_file.type_nr}]'

    def get_py_method_args(self, *, is_impl, is_module_fn, need_self=False,
            need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        self_name = 'sipSelf' if is_impl and (self.spec.c_bindings or need_self) else ''
        args_name = 'sipArgs' if is_impl and need_args else ''

        return f'PyObject *{self_name}, PyObject *{args_name}'

    @staticmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        return slot_type.name.lower() + '_slot'

    @staticmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        fq_cpp_name = wrapped_object.fq_cpp_name if isinstance(wrapped_object, WrappedEnum) else wrapped_object.iface_file.fq_cpp_name

        return 'sipType_' + fq_cpp_name.as_word

    @staticmethod
    def get_types_table_prefix():
        """ Return the prefix in the name of the wrapped types table. """

        return 'sipTypeDef *sipExportedTypes'

    @staticmethod
    def get_wrapped_type_type():
        """ Return the type of the C representation of a wrapped object. """

        return 'sipSimpleWrapper *'

    def legacy_qt_support(self):
        """ Return True if the module implements legacy Qt support. """

        spec = self.spec

        return spec.pyqt_qobject is not None and spec.pyqt_qobject.iface_file.module is spec.module

    @staticmethod
    def need_deprecated_error_flag(code):
        """ Return True if the deprecated error flag is need by some
        handwritten code.
        """

        return is_used_in_code(code, 'sipIsErr')

    def py_enums_supported(self):
        """ Return True if Python enums are supported. """

        return self.spec.target_abi[0] == 13

    def _abi_version_check(self, min_12, min_13):
        """ Return True if the ABI version meets minimum version requirements.
        """

        target_abi = self.spec.target_abi

        return target_abi >= min_13 or (min_12 <= target_abi < (13, 0))

    def _g_instances_char(self, sf, scope):
        """ Generate the code to add a set of characters to a dictionary.
        Return True if there was at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            if variable.type.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING) or len(variable.type.derefs) != 0:
                continue

            ci_name = self.cached_name_ref(variable.py_name)
            ci_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            ci_encoding = _get_encoding(variable.type)

            instances.append((ci_name, ci_val, ci_encoding))

        return _write_instances_table(sf, scope, instances,
'''/* Define the chars to be added to this {dict_type} dictionary. */
static sipCharInstanceDef charInstances{suffix}[]''')

    def _g_instances_class(self, sf, scope):
        """ Generate the code to add a set of class instances to a dictionary.
        Return True if there was at least one.
        """

        spec = self.spec
        instances = []

        for variable in variables_in_scope(spec, scope):
            if variable.type.type is not ArgumentType.CLASS and (variable.type.type is not ArgumentType.ENUM or variable.type.definition.fq_cpp_name is None):
                continue

            # Skip ordinary C++ class instances which need to be done with
            # inline code rather than through a static table.  This is because
            # C++ does not guarantee the order in which the table and the
            # instance will be created.  So far this has only been seen to be a
            # problem when statically linking SIP generated modules on Windows.
            if not spec.c_bindings and variable.access_code is None and len(variable.type.derefs) == 0:
                continue

            ti_name = self.cached_name_ref(variable.py_name)
            ti_ptr = '&' + scoped_variable_name(spec, variable)
            ti_type = '&' + self.get_type_ref(variable.type.definition)
            ti_flags = '0'

            if variable.type.type is ArgumentType.CLASS:
                if variable.access_code is not None:
                    ti_ptr = '(void *)access_' + variable.fq_cpp_name.as_word
                    ti_flags = 'SIP_ACCFUNC|SIP_NOT_IN_MAP'
                elif len(variable.type.derefs) != 0:
                    # This may be a bit heavy handed.
                    if variable.type.is_const:
                        ti_ptr = '(void *)' + ti_ptr

                    ti_flags = 'SIP_INDIRECT'
                else:
                    ti_ptr = get_const_cast(spec, variable.type, ti_ptr)

            instances.append((ti_name, ti_ptr, ti_type, ti_flags))

        return _write_instances_table(sf, scope, instances,
'''/* Define the class and enum instances to be added to this {dict_type} dictionary. */
static sipTypeInstanceDef typeInstances{suffix}[]''')

    def _g_instances_double(self, sf, scope):
        """ Generate the code to add a set of doubles to a dictionary.  Return
        True if there was at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            if variable.type.type not in (ArgumentType.FLOAT, ArgumentType.CFLOAT, ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
                continue

            di_name = self.cached_name_ref(variable.py_name)
            di_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            instances.append((di_name, di_val))

        return _write_instances_table(sf, scope, instances,
'''/* Define the doubles to be added to this {dict_type} dictionary. */
static sipDoubleInstanceDef doubleInstances{suffix}[]''')

    def _g_instances_int(self, sf, scope):
        """ Generate the code to add a set of ints.  Return True if there was
        at least one.
        """

        spec = self.spec
        instances = []

        if self.py_enums_supported():
            # Named enum members are handled as int variables but must be
            # placed at the start of the table.  Note we use the sorted table
            # of needed types rather than the unsorted table of all enums.
            for type in spec.module.needed_types:
                if type.type is not ArgumentType.ENUM:
                    continue

                enum = type.definition

                if py_scope(enum.scope) is not scope or enum.module is not spec.module:
                    continue

                for enum_member in enum.members:
                    ii_name = self.cached_name_ref(enum_member.py_name)
                    ii_val = get_enum_member(spec, enum_member)
                    instances.append((ii_name, ii_val))

        # Handle int variables.
        for variable in variables_in_scope(spec, scope):
            if variable.type.type not in (ArgumentType.ENUM, ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.UBYTE, ArgumentType.USHORT, ArgumentType.SHORT, ArgumentType.CINT, ArgumentType.INT, ArgumentType.BOOL, ArgumentType.CBOOL):
                continue

            # Named enums are handled elsewhere.
            if variable.type.type is ArgumentType.ENUM and variable.type.definition.fq_cpp_name is not None:
                continue

            ii_name = self.cached_name_ref(variable.py_name)
            ii_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            instances.append((ii_name, ii_val))

        # Anonymous enum members are handled as int variables.
        if self.py_enums_supported() or scope is None:
            for enum in spec.enums:
                if py_scope(enum.scope) is not scope or enum.module is not spec.module:
                    continue

                if enum.fq_cpp_name is not None:
                    continue

                for enum_member in enum.members:
                    ii_name = self.cached_name_ref(enum_member.py_name)
                    ii_val = get_enum_member(spec, enum_member)
                    instances.append((ii_name, ii_val))

        return _write_instances_table(sf, scope, instances,
'''/* Define the enum members and ints to be added to this {dict_type}. */
static sipIntInstanceDef intInstances{suffix}[]''')

    def _g_instances_long(self, sf, scope):
        """ Generate the code to add a set of longs to a dictionary.  Return
        True if there was at least one.
        """

        return self._write_int_instances(sf, scope, ArgumentType.LONG, 'long')

    def _g_instances_longlong(self, sf, scope):
        """ Generate the code to add a set of long longs to a dictionary.
        Return True if there was at least one.
        """

        return self._write_int_instances(sf, scope, ArgumentType.LONGLONG,
                'long long')

    def _g_instances_string(self, sf, scope):
        """ Generate the code to add a set of strings to a dictionary.  Return
        True if there is at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            if (variable.type.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING) or len(variable.type.derefs) == 0) and variable.type.type is not ArgumentType.WSTRING:
                continue

            if variable.type.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.WSTRING):
                cast = '(const char *)'

                if variable.type.type is ArgumentType.WSTRING and len(variable.type.derefs) == 0:
                    cast += '&'
            else:
                cast = ''

            si_name = self.cached_name_ref(variable.py_name)
            si_val = cast + variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            si_encoding = _get_encoding(variable.type)

            instances.append((si_name, si_val, si_encoding))

        return _write_instances_table(sf, scope, instances,
'''/* Define the strings to be added to this {dict_type} dictionary. */
static sipStringInstanceDef stringInstances{suffix}[]''')

    def _g_instances_ulong(self, sf, scope):
        """ Generate the code to add a set of unsigned longs to a dictionary.
        Return True if there was at least one.
        """

        return self._write_int_instances(sf, scope, ArgumentType.ULONG,
                'unsigned long')

    def _g_instances_ulonglong(self, sf, scope):
        """ Generate the code to add a set of unsigned long longs to a
        dictionary.  Return True if there was at least one.
        """

        return self._write_int_instances(sf, scope, ArgumentType.ULONGLONG,
                'unsigned long long')

    def _g_instances_voidp(self, sf, scope):
        """ Generate the code to add a set of void pointers to a dictionary.
        Return True if there was at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            if variable.type.type not in (ArgumentType.VOID, ArgumentType.STRUCT, ArgumentType.UNION):
                continue

            vi_name = self.cached_name_ref(variable.py_name)
            vi_val = get_const_cast(self.spec, variable.type,
                    variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL))
            instances.append((vi_name, vi_val))

        return _write_instances_table(sf, scope, instances,
'''/* Define the void pointers to be added to this {dict_type} dictionary. */
static sipVoidPtrInstanceDef voidPtrInstances{suffix}[]''')

    def _g_module_init_body(self, sf):
        """ Generate the body of the module initialisation function. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        sf.write('\n    PyObject *sipModule, *sipModuleDict;\n')

        if spec.sip_module:
            sf.write('    PyObject *sip_sipmod, *sip_capiobj;\n\n')

        # Generate any pre-initialisation code.
        sf.write_code(module.preinitialisation_code)

        sf.write(
'''    /* Initialise the module and get it's dictionary. */
    if ((sipModule = PyModule_Create(&sip_module_def)) == SIP_NULLPTR)
        return SIP_NULLPTR;

    sipModuleDict = PyModule_GetDict(sipModule);

''')

        self._g_sip_api(sf)

        # Generate any initialisation code.
        sf.write_code(module.initialisation_code)

        abi_major, abi_minor = spec.target_abi

        sf.write(
f'''    /* Export the module and publish it's API. */
    if (sipExportModule(&sipModuleAPI_{module_name}, {abi_major}, {abi_minor}, 0) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

        g_pyqt_helper_init(sf, spec)

        sf.write(
f'''
    /* Initialise the module now all its dependencies have been set up. */
    if (sipInitModule(&sipModuleAPI_{module_name}, sipModuleDict) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

        self._g_types_inline(sf)
        self._g_py_objects(sf)

        # Create any exception objects.
        for exception in spec.exceptions:
            if exception.iface_file.module is not module:
                continue

            if exception.exception_nr < 0:
                continue

            if exception.builtin_base_exception is not None:
                exception_type = 'PyExc_' + exception.builtin_base_exception
            else:
                exception_type = 'sipException_' + exception.defined_base_exception.iface_file.fq_cpp_name.as_word

            sf.write(
f'''
    if ((sipExportedExceptions_{module_name}[{exception.exception_nr}] = PyErr_NewException(
            "{module_name}.{exception.py_name}",
            {exception_type}, SIP_NULLPTR)) == SIP_NULLPTR || PyDict_SetItemString(sipModuleDict, "{exception.py_name}", sipExportedExceptions_{module_name}[{exception.exception_nr}]) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

        if module.nr_exceptions > 0:
            sf.write(
f'''
    sipExportedExceptions_{module_name}[{module.nr_exceptions}] = SIP_NULLPTR;
''')

        # Generate the enum and QFlag meta-type registrations for PyQt6.
        if pyqt6_supported(spec):
            for enum in spec.enums:
                if enum.module is not module or enum.fq_cpp_name is None:
                    continue

                if enum.is_protected:
                    continue

                if isinstance(enum.scope, WrappedClass) and enum.scope.pyqt_no_qmetaobject:
                    continue

                sf.write(f'    qMetaTypeId<{enum.fq_cpp_name.as_cpp}>();\n')

            for mapped_type in spec.mapped_types:
                if mapped_type.iface_file.module is not module:
                    continue

                if mapped_type.pyqt_flags == 0:
                    continue

                mapped_type_type = fmt_argument_as_cpp_type(spec,
                        mapped_type.type, plain=True, no_derefs=True)
                sf.write(f'    qMetaTypeId<{mapped_type_type}>();\n')

        # Generate any post-initialisation code. */
        sf.write_code(module.postinitialisation_code)

        sf.write(
'''
    return sipModule;
}
''')

    @staticmethod
    def _g_module_function_table_entries(sf, bindings, module, members):
        """ Generate the entries in a table of PyMethodDef for module functions.
        """

        for member in members:
            if member.py_slot is None:
                py_name = get_normalised_cached_name(member.py_name)
                sf.write(f'        {{sipName_{py_name}, ')

                if member.no_arg_parser or member.allow_keyword_args:
                    sf.write(f'SIP_MLMETH_CAST(func_{member.py_name.name}), METH_VARARGS|METH_KEYWORDS')
                else:
                    sf.write(f'func_{member.py_name.name}, METH_VARARGS')

                docstring_ref = get_optional_ptr(
                        has_method_docstring(bindings, member,
                                module.overloads),
                        'doc_' + member.py_name.name)
                sf.write(f', {docstring_ref}}},\n')

    # The types that are implemented as PyObject*.
    _PY_OBJECT_TYPES = (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE,
        ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE,
        ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER,
        ArgumentType.PYENUM)

    def _g_py_objects(self, sf):
        """ Generate the inline code to add a set of Python objects to a module
        dictionary.
        """

        spec = self.spec

        no_intro = True

        for variable in spec.variables:
            if variable.module is not spec.module:
                continue

            if variable.type.type not in self._PY_OBJECT_TYPES:
                continue

            if variable.needs_handler:
                continue

            if no_intro:
                sf.write('\n    /* Define the Python objects wrapped as such. */\n')
                no_intro = False

            py_name = self.cached_name_ref(variable.py_name)
            cpp_name = scoped_variable_name(spec, variable)

            sf.write(f'    PyDict_SetItemString(sipModuleDict, {py_name}, ({cpp_name} != NULL ? {cpp_name} : Py_None));\n')

    def _g_sip_api(self, sf):
        """ Generate the code to get the sip API. """

        spec = self.spec
        sip_module_name = spec.sip_module
        module_name = spec.module.py_name

        if sip_module_name:
            # Note that we don't use PyCapsule_Import() because we thought
            # (incorrectly) that it doesn't handle package.module.attribute.

            sf.write(
f'''    /* Get the SIP module's API. */
    if ((sip_sipmod = PyImport_ImportModule("{sip_module_name}")) == SIP_NULLPTR)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

    sip_capiobj = PyDict_GetItemString(PyModule_GetDict(sip_sipmod), "_C_API");
    Py_DECREF(sip_sipmod);

    if (sip_capiobj == SIP_NULLPTR || !PyCapsule_CheckExact(sip_capiobj))
    {{
        PyErr_SetString(PyExc_AttributeError, "{sip_module_name}._C_API is missing or has the wrong type");
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

''')

            if spec.c_bindings:
                c_api = f'(const sipAPIDef *)PyCapsule_GetPointer(sip_capiobj, "{sip_module_name}._C_API")'
            else:
                c_api = f'reinterpret_cast<const sipAPIDef *>(PyCapsule_GetPointer(sip_capiobj, "{sip_module_name}._C_API"))'

            sf.write(
f'''    sipAPI_{module_name} = {c_api};

    if (sipAPI_{module_name} == SIP_NULLPTR)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

''')
        else:
            # If there is no sip module name then we are getting the API from a
            # non-shared sip module.
            sf.write(
f'''    if ((sipAPI_{module_name} = sip_init_library(sipModuleDict)) == SIP_NULLPTR)
        return SIP_NULLPTR;

''')

    def _g_types_inline(self, sf):
        """ Generate the inline code to add a set of generated type instances
        to a dictionary.
        """

        spec = self.spec
        no_intro = True

        for variable in spec.variables:
            if variable.module is not spec.module:
                continue

            if variable.type.type not in (ArgumentType.CLASS, ArgumentType.MAPPED, ArgumentType.ENUM):
                continue

            if variable.needs_handler:
                continue

            # Skip classes that don't need inline code.
            if spec.c_bindings or variable.access_code is not None or len(variable.type.derefs) != 0:
                continue

            if no_intro:
                sf.write(
'''
    /*
     * Define the class, mapped type and enum instances that have to be
     * added inline.
     */
''')

                no_intro = False

            if py_scope(variable.scope) is None:
                dict_name = 'sipModuleDict'
            else:
                dict_name = f'(PyObject *)sipTypeAsPyTypeObject({self.get_type_ref(variable.scope)})'

            py_name = self.cached_name_ref(variable.py_name)
            ptr = '&' + scoped_variable_name(spec, variable)

            if variable.type.is_const:
                type_name = fmt_argument_as_cpp_type(spec, variable.type,
                        plain=True, no_derefs=True)
                ptr = f'const_cast<{type_name} *>({ptr})'

            sf.write(f'    sipAddTypeInstance({dict_name}, {py_name}, {ptr}, {self.get_type_ref(variable.type.definition)});\n')

    def _g_variable_getter(self, sf, variable):
        """ Generate a variable getter. """

        spec = self.spec
        variable_type = variable.type.type
        first_arg = 'sipSelf' if spec.c_bindings or not variable.is_static else ''
        last_arg = get_use_in_code(variable.get_code, 'sipPyType', spec=spec)

        needs_new = (variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(variable.type.derefs) == 0 and variable.type.is_const)

        # If the variable is itself a non-const instance of a wrapped class
        # then two things must happen.  Firstly, the getter must return the
        # same Python object each time - it must not re-wrap the instance.
        # This is because the Python object can contain important state
        # information that must not be lost (specifically references to other
        # Python objects that it owns).  Therefore the Python object wrapping
        # the containing class must cache a reference to the Python object
        # wrapping the variable.  Secondly, the Python object wrapping the
        # containing class must not be garbage collected before the Python
        # object wrapping the variable is (because the latter references
        # memory, ie. the variable itself, that is managed by the former).
        # Therefore the Python object wrapping the variable must keep a
        # reference to the Python object wrapping the containing class (but
        # only if the latter is non-static).
        var_key = self_key = 0

        if variable_type is ArgumentType.CLASS and len(variable.type.derefs) == 0 and not variable.type.is_const:
            var_key = variable.type.definition.iface_file.module.next_key
            variable.type.definition.iface_file.module.next_key -= 1

            if not variable.is_static:
                self_key = variable.module.next_key
                variable.module.next_key -= 1

        second_arg = 'sipPySelf' if spec.c_bindings or var_key < 0 else ''
        variable_as_word = variable.fq_cpp_name.as_word

        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *varget_{variable_as_word}(void *, PyObject *, PyObject *);}}\n')

        sf.write(
f'''static PyObject *varget_{variable_as_word}(void *{first_arg}, PyObject *{second_arg}, PyObject *{last_arg})
{{
''')

        if variable.get_code is not None:
            sip_py_decl = 'PyObject *sipPy'
        elif var_key < 0:
            if variable.is_static:
                sip_py_decl = 'static PyObject *sipPy = SIP_NULLPTR'
            else:
                sip_py_decl = 'PyObject *sipPy'
        else:
            sip_py_decl = None

        if sip_py_decl is not None:
            sf.write('    ' + sip_py_decl + ';\n')

        if variable.get_code is None:
            value_decl = get_named_value_decl(spec, variable.scope,
                    variable.type, 'sipVal')
            sf.write(f'    {value_decl};\n')

        if not variable.is_static:
            scope_s = scoped_class_name(spec, variable.scope)

            if spec.c_bindings:
                sip_self = f'({scope_s} *)sipSelf'
            else:
                sip_self = f'reinterpret_cast<{scope_s} *>(sipSelf)'

            sf.write(f'    {scope_s} *sipCpp = {sip_self};\n')

        sf.write('\n')

        # Handle any handwritten getter.
        if variable.get_code is not None:
            sf.write_code(variable.get_code)

            sf.write(
'''
    return sipPy;
}
''')

            return

        # Get any previously wrapped cached object.
        if var_key < 0:
            if variable.is_static:
                sf.write(
'''    if (sipPy)
    {
        Py_INCREF(sipPy);
        return sipPy;
    }

''')
            else:
                sf.write(
f'''    sipPy = sipGetReference(sipPySelf, {self_key});

    if (sipPy)
        return sipPy;

''')

        variable_type_s = fmt_argument_as_cpp_type(spec, variable.type,
                plain=True, no_derefs=True)

        if needs_new:
            if spec.c_bindings:
                sf.write('    *sipVal = ')
            else:
                sf.write(f'    sipVal = new {variable_type_s}(')
        else:
            sf.write('    sipVal = ')

            if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(variable.type.derefs) == 0:
                sf.write('&')

        sf.write(self._get_variable_member(variable))

        if needs_new and not spec.c_bindings:
            sf.write(')')

        sf.write(';\n\n')

        if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            prefix_s = 'sipPy =' if var_key < 0 else 'return'
            new_s = 'New' if needs_new else ''
            sip_val_s = get_const_cast(spec, variable.type, 'sipVal')

            sf.write(f'    {prefix_s} sipConvertFrom{new_s}Type({sip_val_s}, {self.get_type_ref(variable.type.definition)}, SIP_NULLPTR);\n')

            if var_key < 0:
                if variable.is_static:
                    ref_code = 'Py_INCREF(sipPy)'
                else:
                    ref_code = f'sipKeepReference(sipPySelf, {self_key}, sipPy)'

                sf.write(
f'''
    if (sipPy)
    {{
        sipKeepReference(sipPy, {var_key}, sipPySelf);
        {ref_code};
    }}

    return sipPy;
''')

        elif variable_type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            sf.write('    return PyBool_FromLong(sipVal);\n')

        elif variable_type is ArgumentType.ASCII_STRING:
            if len(variable.type.derefs) == 0:
                sf.write('    return PyUnicode_DecodeASCII(&sipVal, 1, SIP_NULLPTR);\n')
            else:
                sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_DecodeASCII(sipVal, strlen(sipVal), SIP_NULLPTR);
''')

        elif variable_type is ArgumentType.LATIN1_STRING:
            if len(variable.type.derefs) == 0:
                sf.write('    return PyUnicode_DecodeLatin1(&sipVal, 1, SIP_NULLPTR);\n')
            else:
                sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_DecodeLatin1(sipVal, strlen(sipVal), SIP_NULLPTR);
''')

        elif variable_type is ArgumentType.UTF8_STRING:
            if len(variable.type.derefs) == 0:
                sf.write('    return PyUnicode_FromStringAndSize(&sipVal, 1);\n')
            else:
                sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_FromString(sipVal);
''')

        elif variable_type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
            cast_s = '' if variable_type is ArgumentType.STRING else '(char *)'

            if len(variable.type.derefs) == 0:
                sf.write(f'    return PyBytes_FromStringAndSize({cast_s}&sipVal, 1);\n')
            else:
                sf.write(
f'''    if (sipVal == SIP_NULLPTR)
    {{
        Py_INCREF(Py_None);
        return Py_None;
    }}

    return PyBytes_FromString({cast_s}sipVal);
''')

        elif variable_type is ArgumentType.WSTRING:
            if len(variable.type.derefs) == 0:
                sf.write('    return PyUnicode_FromWideChar(&sipVal, 1);\n')
            else:
                sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_FromWideChar(sipVal, (Py_ssize_t)wcslen(sipVal));
''')

        elif variable_type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            sf.write('    return PyFloat_FromDouble((double)sipVal);\n')

        elif variable_type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            sf.write('    return PyFloat_FromDouble(sipVal);\n')

        elif variable_type is ArgumentType.ENUM:
            if variable.type.definition.fq_cpp_name is None:
                sf.write('    return PyLong_FromLong(sipVal);\n')
            else:
                sip_val_s = 'sipVal' if spec.c_bindings else 'static_cast<int>(sipVal)'
                sf.write(f'    return sipConvertFromEnum({sip_val_s}, {self.get_type_ref(variable.type.definition)});\n')

        elif variable_type in (ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.SHORT, ArgumentType.INT, ArgumentType.CINT):
            sf.write('    return PyLong_FromLong(sipVal);\n')

        elif variable_type is ArgumentType.LONG:
            sf.write('    return PyLong_FromLong(sipVal);\n')

        elif variable_type in (ArgumentType.UBYTE, ArgumentType.USHORT):
            sf.write('    return PyLong_FromUnsignedLong(sipVal);\n')

        elif variable_type in (ArgumentType.UINT, ArgumentType.ULONG, ArgumentType.SIZE):
            sf.write('    return PyLong_FromUnsignedLong(sipVal);\n')

        elif variable_type is ArgumentType.LONGLONG:
            sf.write('    return PyLong_FromLongLong(sipVal);\n')

        elif variable_type is ArgumentType.ULONGLONG:
            sf.write('    return PyLong_FromUnsignedLongLong(sipVal);\n')

        elif variable_type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            const_s = 'Const' if variable.type.is_const else ''
            cast_s = get_void_ptr_cast(variable.type)

            sf.write(f'    return sipConvertFrom{const_s}VoidPtr({cast_s}sipVal);\n')

        elif variable_type is ArgumentType.CAPSULE:
            cast_s = get_void_ptr_cast(variable.type)

            sf.write(f'    return PyCapsule_New({cast_s}sipVal, "{variable.type.definition.as_cpp}", SIP_NULLPTR);\n')

        elif variable_type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
            sf.write(
'''    Py_XINCREF(sipVal);
    return sipVal;
''')

        sf.write('}\n')

    def _g_variable_setter(self, sf, variable):
        """ Generate a variable setter. """

        spec = self.spec
        variable_type = variable.type.type

        # We need to keep a reference to the original Python object if it is
        # providing the memory that the C/C++ variable is pointing to.
        keep = keep_py_reference(spec, variable.type)

        need_sip_cpp = (spec.c_bindings or variable.set_code is None or is_used_in_code(variable.set_code, 'sipCpp'))

        first_arg = 'sipSelf' if spec.c_bindings or not variable.is_static else ''
        if not need_sip_cpp:
            first_arg = ''

        last_arg = 'sipPySelf' if spec.c_bindings or (not variable.is_static and keep) else ''

        sip_py = 'sipPy' if spec.c_bindings or variable.set_code is None or is_used_in_code(variable.set_code, 'sipPy') else ''
        variable_as_word = variable.fq_cpp_name.as_word

        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int varset_{variable_as_word}(void *, PyObject *, PyObject *);}}\n')

        sf.write(
f'''static int varset_{variable_as_word}(void *{first_arg}, PyObject *{sip_py}, PyObject *{last_arg})
{{
''')

        if variable.set_code is None:
            if variable_type is ArgumentType.BOOL:
                value_decl = 'int sipVal'
            else:
                value_decl = get_named_value_decl(spec, variable.scope,
                        variable.type, 'sipVal')

            sf.write(f'    {value_decl};\n')

        if not variable.is_static and need_sip_cpp:
            scope_s = scoped_class_name(spec, variable.scope)

            if spec.c_bindings:
                statement = f'({scope_s} *)sipSelf'
            else:
                statement = f'reinterpret_cast<{scope_s} *>(sipSelf)'

            sf.write(f'    {scope_s} *sipCpp = {statement};\n\n')

        # Handle any handwritten setter.
        if variable.set_code is not None:
            sf.write('   int sipErr = 0;\n\n')
            sf.write_code(variable.set_code)
            sf.write(
'''
    return (sipErr ? -1 : 0);
}
''')

            return

        has_state = False

        if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            sf.write('    int sipIsErr = 0;\n')

            if len(variable.type.derefs) == 0:
                convert_to_type_code = variable.type.definition.convert_to_type_code

                if variable_type is ArgumentType.MAPPED and variable.type.definition.no_release:
                    convert_to_type_code = None

                if convert_to_type_code is not None:
                    has_state = True

                    sf.write('    int sipValState;\n')

                    if type_needs_user_state(variable.type):
                        sf.write('    void *sipValUserState;\n')

        sf.write(f'    sipVal = {self._get_variable_to_cpp(variable, has_state)};\n')

        deref = ''

        if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if len(variable.type.derefs) == 0:
                deref = '*'

            error_test = 'sipIsErr'
        elif variable_type is ArgumentType.BOOL:
            error_test = 'sipVal < 0'
        else:
            error_test = 'PyErr_Occurred() != SIP_NULLPTR'

        sf.write(
f'''
    if ({error_test})
        return -1;

''')

        member = self._get_variable_member(variable)

        if variable_type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
            sf.write(
f'''    Py_XDECREF({member});
    Py_INCREF(sipVal);

''')

        value = deref + 'sipVal'

        if variable_type is ArgumentType.BOOL:
            if spec.c_bindings:
                value = '(bool)' + value
            else:
                value = f'static_cast<bool>({value})'

        sf.write(f'    {member} = {value};\n')

        # Note that wchar_t * leaks here.

        if has_state:
            suffix = get_user_state_suffix(spec, variable.type)

            sf.write(
f'''
    sipReleaseType{suffix}(sipVal, {self.get_type_ref(variable.type.definition)}, sipValState''')

            if type_needs_user_state(variable.type):
                sf.write(', sipValUserState')

            sf.write(');\n')

        # Generate the code to keep the object alive while we use its data.
        if keep:
            if variable.is_static:
                sf.write(
'''
    static PyObject *sipKeep = SIP_NULLPTR;

    Py_XDECREF(sipKeep);
    sipKeep = sipPy;
    Py_INCREF(sipKeep);
''')
            else:
                key = variable.module.next_key
                variable.module.next_key -= 1

                sf.write(
f'''
    sipKeepReference(sipPySelf, {key}, sipPy);
''')

        sf.write(
'''
    return 0;
}
''')

    def _get_variable_member(self, variable):
        """ Return the member variable of a class. """

        if variable.is_static:
            scope = scoped_variable_name(self.spec, variable)
        else:
            scope = 'sipCpp->' + variable.fq_cpp_name.base_name

        return scope


    def _get_variable_to_cpp(self, variable, has_state):
        """ Return the statement to convert a Python variable to C/C++. """

        spec = self.spec
        type_s = fmt_argument_as_cpp_type(spec, variable.type, plain=True,
                no_derefs=True)

        variable_type = variable.type.type

        if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if spec.c_bindings:
                statement = f'({type_s} *)'
                cast_tail = ''
            else:
                statement = f'reinterpret_cast<{type_s} *>('
                cast_tail = ')'

            # Note that we don't support /Transfer/ but could do.  We could
            # also support /Constrained/ (so long as we also supported it for
            # all types).

            suffix = get_user_state_suffix(spec, variable.type)
            flags = '0' if len(variable.type.derefs) != 0 else 'SIP_NOT_NONE'
            state_ptr = '&sipValState' if has_state else 'SIP_NULLPTR'

            statement += f'sipForceConvertToType{suffix}(sipPy, {self.get_type_ref(variable.type.definition)}, SIP_NULLPTR, {flags}, {state_ptr}'

            if type_needs_user_state(variable.type):
                statement += ', &sipValUserState'

            statement += ', &sipIsErr)' + cast_tail

        elif variable_type is ArgumentType.ENUM:
            statement = f'({type_s})sipConvertToEnum(sipPy, {self.get_type_ref(variable.type.definition)})'

        elif variable_type is ArgumentType.SSTRING:
            if len(variable.type.derefs) == 0:
                statement = '(signed char)sipBytes_AsChar(sipPy)'
            elif variable.type.is_const:
                statement = '(const signed char *)sipBytes_AsString(sipPy)'
            else:
                statement = '(signed char *)sipBytes_AsString(sipPy)'

        elif variable_type is ArgumentType.USTRING:
            if len(variable.type.derefs) == 0:
                statement = '(unsigned char)sipBytes_AsChar(sipPy)'
            elif variable.type.is_const:
                statement = '(const unsigned char *)sipBytes_AsString(sipPy)'
            else:
                statement = '(unsigned char *)sipBytes_AsString(sipPy)'

        elif variable_type is ArgumentType.ASCII_STRING:
            if len(variable.type.derefs) == 0:
                statement = 'sipString_AsASCIIChar(sipPy)'
            elif variable.type.is_const:
                statement = 'sipString_AsASCIIString(&sipPy)'
            else:
                statement = '(char *)sipString_AsASCIIString(&sipPy)'

        elif variable_type is ArgumentType.LATIN1_STRING:
            if len(variable.type.derefs) == 0:
                statement = 'sipString_AsLatin1Char(sipPy)'
            elif variable.type.is_const:
                statement = 'sipString_AsLatin1String(&sipPy)'
            else:
                statement = '(char *)sipString_AsLatin1String(&sipPy)'

        elif variable_type is ArgumentType.UTF8_STRING:
            if len(variable.type.derefs) == 0:
                statement = 'sipString_AsUTF8Char(sipPy)'
            elif variable.type.is_const:
                statement = 'sipString_AsUTF8String(&sipPy)'
            else:
                statement = '(char *)sipString_AsUTF8String(&sipPy)'

        elif variable_type is ArgumentType.STRING:
            if len(variable.type.derefs) == 0:
                statement = 'sipBytes_AsChar(sipPy)'
            elif variable.type.is_const:
                statement = 'sipBytes_AsString(sipPy)'
            else:
                statement = '(char *)sipBytes_AsString(sipPy)'

        elif variable_type is ArgumentType.WSTRING:
            if len(variable.type.derefs) == 0:
                statement = 'sipUnicode_AsWChar(sipPy)'
            else:
                statement = 'sipUnicode_AsWString(sipPy)'

        elif variable_type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            statement = '(float)PyFloat_AsDouble(sipPy)'

        elif variable_type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            statement = 'PyFloat_AsDouble(sipPy)'

        elif variable_type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            statement = 'sipConvertToBool(sipPy)'

        elif variable_type is ArgumentType.BYTE:
            statement = 'sipLong_AsChar(sipPy)'

        elif variable_type is ArgumentType.SBYTE:
            statement = 'sipLong_AsSignedChar(sipPy)'

        elif variable_type is ArgumentType.UBYTE:
            statement = 'sipLong_AsUnsignedChar(sipPy)'

        elif variable_type is ArgumentType.USHORT:
            statement = 'sipLong_AsUnsignedShort(sipPy)'

        elif variable_type is ArgumentType.SHORT:
            statement = 'sipLong_AsShort(sipPy)'

        elif variable_type is ArgumentType.UINT:
            statement = 'sipLong_AsUnsignedInt(sipPy)'

        elif variable_type is ArgumentType.SIZE:
            statement = 'sipLong_AsSizeT(sipPy)'

        elif variable_type in (ArgumentType.INT, ArgumentType.CINT):
            statement = 'sipLong_AsInt(sipPy)'

        elif variable_type is ArgumentType.ULONG:
            statement = 'sipLong_AsUnsignedLong(sipPy)'

        elif variable_type is ArgumentType.LONG:
            statement = 'sipLong_AsLong(sipPy)'

        elif variable_type is ArgumentType.ULONGLONG:
            statement = 'sipLong_AsUnsignedLongLong(sipPy)'

        elif variable_type is ArgumentType.LONGLONG:
            statement = 'sipLong_AsLongLong(sipPy)'

        elif variable_type in (ArgumentType.STRUCT, ArgumentType.UNION):
            statement = f'({type_s} *)sipConvertToVoidPtr(sipPy)'

        elif variable_type is ArgumentType.VOID:
            statement = 'sipConvertToVoidPtr(sipPy)'

        elif variable_type is ArgumentType.CAPSULE:
            statement = f'PyCapsule_GetPointer(sipPy, "{variable.type.definition.as_cpp}")'

        else:
            # These are just the PyObject types.
            statement = 'sipPy'

        return statement

    def _write_int_instances(self, sf, scope, target_type, type_name):
        """ Generate the code to add a set of a particular type to a
        dictionary.  Return True if there was at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            variable_type = variable.type.type

            # We treat unsigned and size_t as unsigned long as we don't
            # generate a separate table for them.
            if variable_type in (ArgumentType.UINT, ArgumentType.SIZE) and target_type is ArgumentType.ULONG:
                variable_type = ArgumentType.ULONG

            # Likewise we treat Py_hash_t and Py_ssize_t as long.
            if variable_type in (ArgumentType.HASH, ArgumentType.SSIZE) and target_type is ArgumentType.LONG:
                variable_type = ArgumentType.LONG

            if variable_type is not target_type:
                continue

            ii_name = self.cached_name_ref(variable.py_name)
            ii_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            instances.append((ii_name, ii_val))

        table_type_name = type_name.title().replace(' ', '')
        table_name = table_type_name[0].lower() + table_type_name[1:]

        declaration_template = f'''/* Define the {type_name}s to be added to this {{dict_type}} dictionary. */
static sip{table_type_name}InstanceDef {table_name}Instances{{suffix}}[]'''

        return _write_instances_table(sf, scope, instances,
                declaration_template)


def _can_set_variable(variable):
    """ Return True if a variable can be set. """

    if variable.no_setter:
        return False

    if len(variable.type.derefs) == 0 and variable.type.is_const:
        return False

    return True


def _class_object_ref(test, object_name, klass_name):
    """ Return an appropriate reference to a class-specific object. """

    return object_name + '_' + klass_name if test else 'SIP_NULLPTR'


def _get_encoding(type):
    """ Return the encoding character for the given type. """

    if type.type is ArgumentType.ASCII_STRING:
        encoding = "'A'"
    elif type.type is ArgumentType.LATIN1_STRING:
        encoding = "'L'"
    elif type.type is ArgumentType.UTF8_STRING:
        encoding = "'8'"
    elif type.type is ArgumentType.WSTRING:
        encoding = "'w'" if len(type.derefs) == 0 else "'W'"
    else:
        encoding = "'N'"

    return encoding


def _write_instances_table(sf, scope, instances, declaration_template):
    """ Write a table of instances.  Return True if there was a table written.
    """

    if len(instances) == 0:
        return False

    if scope is None:
        dict_type = 'module'
        suffix = ''
    else:
        dict_type = 'type'
        suffix = '_' + scope.iface_file.fq_cpp_name.as_word

    declaration = declaration_template.format(dict_type=dict_type,
            suffix=suffix)
    sf.write(f'\n\n{declaration} = {{\n')

    for instance in instances:
        entry = ', '.join(instance)
        sf.write(f'    {{{entry}}},\n')

    sentinals = ', '.join('0' * len(instances[0]))
    sf.write(f'    {{{sentinals}}}\n}};\n')

    return True
