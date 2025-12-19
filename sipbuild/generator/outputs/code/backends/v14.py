# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from .....sip_module_configuration import SipModuleConfiguration

from ....python_slots import is_rich_compare_slot
from ....scoped_name import STRIP_GLOBAL
from ....specification import (ArgumentType, GILUse, IfaceFileType,
        MultiInterpreterSupport, PySlot, WrappedEnum)

from ...formatters import fmt_class_as_scoped_py_name

from ..snippets import (g_class_docstring, g_class_method_table,
        g_module_docstring, g_type_init_body, g_pyqt_class_plugin,
        g_pyqt_helper_defns, g_pyqt_helper_init)
from ..utils import (get_class_flags, get_optional_ptr, get_type_from_void,
        has_method_docstring, need_dealloc, py_scope, pyqt5_supported,
        pyqt6_supported, scoped_class_name, variables_in_scope)

from .abstract_backend import AbstractBackend


class v14Backend(AbstractBackend):
    """ The backend code generator for v14 of the ABI. """

    def g_create_wrapped_module(self, sf, bindings,
        # TODO These will probably be generated here at some point.
        has_sip_strings,
        has_external,
        nr_enum_members,
        has_virtual_error_handlers,
        nr_subclass_convertors,
        static_variables_state,
        slot_extenders,
        init_extenders
    ):
        """ Generate the code to generate a wrapped module. """

        spec = self.spec
        target_abi = spec.target_abi
        module = spec.module
        module_name = module.py_name

        nr_static_variables, nr_types = static_variables_state

        sf.write(
f'''/* The wrapped module's immutable definition. */
static const sipWrappedModuleDef sipWrappedModule_{module_name} = {{
    .abi_major = {target_abi[0]},
    .abi_minor = {target_abi[1]},
    .sip_configuration = 0x{spec.sip_module_configuration:04x},
''')

        if len(module.all_imports) != 0:
            sf.write('    .imports = importsTable,\n')

        # TODO Exclude non-local types.  They are needed (ie. I think we need
        # the iface file) but we don't generated definition structures.
        if len(module.needed_types) != 0:
            sf.write(f'    .nr_type_defs = {len(module.needed_types)},\n')
            sf.write(f'    .type_defs = sipTypeDefs_{module_name},\n')

        if has_external:
            sf.write('    .imports = externalTypesTable,\n')

        if self.custom_enums_supported() and nr_enum_members != 0:
            sf.write(f'    .nr_enum_members = {nr_enum_members},\n')
            sf.write('    .enum_members = enum_members,\n')

        if module.nr_typedefs != 0:
            sf.write(f'    .nr_typedefs = {module.nr_typedefs},\n')
            sf.write('    .typedefs = typedefsTable,\n')

        if has_virtual_error_handlers:
            sf.write('    .virterrorhandlers = virtErrorHandlersTable,\n')

        if nr_subclass_convertors != 0:
            sf.write('    .convertors = convertorsTable,\n')

        if nr_static_variables != 0:
            sf.write(f'    .attributes.nr_static_variables = {nr_static_variables},\n')
            sf.write(f'    .attributes.static_variables = sipWrappedModuleVariables_{module_name},\n')

        if nr_types != 0:
            sf.write(f'    .attributes.nr_types = {nr_types},\n')
            sf.write(f'    .attributes.type_nrs = sipTypeNrs_{module_name},\n')

        if module.license is not None:
            sf.write('    .license = &module_license,\n')

        if slot_extenders:
            sf.write('    .slotextend = slotExtenders,\n')

        if init_extenders:
            sf.write('    .initextend = initExtenders,\n')

        if module.has_delayed_dtors:
            sf.write('    .delayeddtors = sipDelayedDtors,\n')

        if bindings.exceptions and module.nr_exceptions != 0:
            sf.write(f'    .exception_handler = sipExceptionHandler_{module_name},\n')

        sf.write('};\n')

        g_module_docstring(sf, module)
        g_pyqt_helper_defns(sf, spec)
        self._g_module_clear(sf)
        self._g_module_exec(sf)
        self._g_module_free(sf)
        self._g_module_traverse(sf)
        has_module_functions = self.g_module_functions_table(sf, bindings,
                module)
        self.g_module_definition(sf, has_module_functions=has_module_functions)
        self.g_module_init_start(sf)

        sf.write(
f'''    return sipWrappedModuleSlots_{module_name};
}}
''')

    def g_enum_macros(self, sf, scope=None, imported_module=None):
        """ Generate the type macros for enums. """

        # TODO
        pass

    def g_function_support_vars(self, sf):
        """ Generate the variables needed by a function. """

        sf.write('    const sipAPIDef *sipAPI = sipGetAPI(sipModule);\n')

    def g_init_mixin_impl_body(self, sf, klass):
        """ Generate the body of the implementation of a mixin initialisation
        function.
        """

        self.g_slot_support_vars(sf)

        type_ref = self.get_type_ref(klass)

        sf.write(f'    return sipInitMixin(sipModule, sipSelf, sipArgs, sipKwds, {type_ref});\n')

    def g_mapped_type_api(self, sf, mapped_type):
        """ Generate the API details for a mapped type. """

        # TODO
        pass

    def g_method_support_vars(self, sf):
        """ Generate the variables needed by a method. """

        sf.write('    PyObject *sipModule = PyType_GetModule(sipDefType);\n')
        self.g_function_support_vars(sf)

    # Map GILUse values.
    _MAP_GIL_USED = {
        GILUse.USED:        'Py_MOD_GIL_USED',
        GILUse.NOT_USED:    'Py_MOD_GIL_NOT_USED',
    }

    # Map MultiInterpreterSupport values.
    _MAP_MULTI_INTERPRETER_SUPPORT = {
        MultiInterpreterSupport.NOT_SUPPORTED:
                'Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED',
        MultiInterpreterSupport.PER_INTERPRETER_GIL_SUPPORTED:
                'Py_MOD_PER_INTERPRETER_GIL_SUPPORTED',
        MultiInterpreterSupport.SUPPORTED:
                'Py_MOD_MULTIPLE_INTERPRETERS_SUPPORTED',
    }

    def g_module_definition(self, sf, has_module_functions=False):
        """ Generate the module definition structure. """

        module = self.spec.module

        gil_support = self._MAP_GIL_USED[module.gil_use]
        interp_support = self._MAP_MULTI_INTERPRETER_SUPPORT[module.multi_interpreter_support]

        # Note that the sip module implementation expects Py_mod_name to be the
        # first slot.
        sf.write(
f'''

/* The wrapped module's immutable slot definitions. */
PyABIInfo_VAR(sip_abi_info);

PyModuleDef_Slot sipWrappedModuleSlots_{module.py_name}[] = {{
    {{Py_mod_name, (void *)"{module.fq_py_name}"}},
    {{Py_mod_abi, &sip_abi_info}},
    {{Py_mod_exec, (void *)wrapped_module_exec}},
    {{Py_mod_gil, {gil_support}}},
    {{Py_mod_multiple_interpreters, {interp_support}}},
    {{Py_mod_state_clear, (void *)wrapped_module_clear}},
    {{Py_mod_state_free, (void *)wrapped_module_free}},
    {{Py_mod_state_size, (void *)sizeof (sipWrappedModuleState)}},
    {{Py_mod_state_traverse, (void *)wrapped_module_traverse}},
''')

        if module.docstring is not None:
            # TODO The name should have a sip_ prefix.
            sf.write(f'    {{Py_mod_doc, (void *)doc_mod_{module.py_name}}},\n')

        if has_module_functions:
            sf.write(f'    {{Py_mod_methods, sip_methods_{module.py_name}}},\n')

        sf.write('    {0}\n};\n')

    def g_module_functions_table(self, sf, bindings, module):
        """ Generate the table of module functions and return True if anything
        was actually generated.
        """

        has_module_functions = self._g_module_function_table_entries(sf,
                bindings, module, module.global_functions)

        # Generate the module functions for any hidden namespaces.
        for klass in self.spec.classes:
            if klass.iface_file.module is module and klass.is_hidden_namespace:
                has_module_functions = self._g_module_function_table_entries(
                        sf, bindings, module, klass.members,
                        has_module_functions=has_module_functions)

        if has_module_functions:
            sf.write(
'''    {}
};

''')

        return has_module_functions

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
{extern_c}PyObject *PyModExport_{module_name}({arg_type})
#else
PyMODEXPORT_FUNC PyModExport_{module_name}({arg_type})
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

            docstring = get_optional_ptr(
                    has_method_docstring(bindings, member, scope.overloads),
                    f'doc_{scope_name}_{py_name.name}')

            if no_intro:
                sf.write(
f'''

static PyMethodDef sipMethods_{scope_name}[] = {{
''')

                no_intro = False

            sf.write(f'    {{{cached_py_name}, SIP_MLMETH_CAST(meth_{scope_name}_{py_name.name}), METH_METHOD|METH_FASTCALL|METH_KEYWORDS, {docstring}}},\n')

        if not no_intro:
            sf.write('    {}\n};\n')

        return len(members)

    def g_sip_api(self, sf, module_name):
        """ Generate the SIP API as seen by generated code. """

        # TODO These have been reviewed as part of the public v14 API.
        sf.write(
f'''

extern PyModuleDef_Slot sipWrappedModuleSlots_{module_name}[];

#define sipBuildResult              sipAPI->api_build_result
#define sipFindTypeID               sipAPI->api_find_type_id
#define sipGetAddress               sipAPI->api_get_address
#define sipIsOwnedByPython          sipAPI->api_is_owned_by_python
#define sipGetTypeUserData          sipAPI->api_get_type_user_data
#define sipSetTypeUserData          sipAPI->api_set_type_user_data
''')

        # TODO These have been reviewed as part of the private v14 API.
        sf.write(
f'''#define sipInitMixin                sipAPI->api_init_mixin
#define sipNoFunction               sipAPI->api_no_function
#define sipNoMethod                 sipAPI->api_no_method
#define sipParseArgs                sipAPI->api_parse_args
#define sipParseKwdArgs             sipAPI->api_parse_kwd_args
#define sipParseVectorcallArgs      sipAPI->api_parse_vectorcall_args
#define sipParseVectorcallKwdArgs   sipAPI->api_parse_vectorcall_kwd_args
#define sipParsePair                sipAPI->api_parse_pair
''')

        # TODO These have yet to be reviewed.
        sf.write(
f'''#define sipMalloc                   sipAPI->api_malloc
#define sipFree                     sipAPI->api_free
#define sipCallMethod               sipAPI->api_call_method
#define sipCallProcedureMethod      sipAPI->api_call_procedure_method
#define sipCallErrorHandler         sipAPI->api_call_error_handler
#define sipParseResultEx            sipAPI->api_parse_result_ex
#define sipParseResult              sipAPI->api_parse_result
#define sipInstanceDestroyed        sipAPI->api_instance_destroyed
#define sipInstanceDestroyedEx      sipAPI->api_instance_destroyed_ex
#define sipConvertFromSequenceIndex sipAPI->api_convert_from_sequence_index
#define sipConvertFromSliceObject   sipAPI->api_convert_from_slice_object
#define sipConvertFromVoidPtr       sipAPI->api_convert_from_void_ptr
#define sipConvertToVoidPtr         sipAPI->api_convert_to_void_ptr
#define sipAddException             sipAPI->api_add_exception
#define sipAbstractMethod           sipAPI->api_abstract_method
#define sipBadClass                 sipAPI->api_bad_class
#define sipBadCatcherResult         sipAPI->api_bad_catcher_result
#define sipBadCallableArg           sipAPI->api_bad_callable_arg
#define sipBadOperatorArg           sipAPI->api_bad_operator_arg
#define sipTrace                    sipAPI->api_trace
#define sipTransferBack             sipAPI->api_transfer_back
#define sipTransferTo               sipAPI->api_transfer_to
#define sipSimpleWrapper_Type       sipAPI->api_simplewrapper_type
#define sipWrapper_Type             sipAPI->api_wrapper_type
#define sipWrapperType_Type         sipAPI->api_wrappertype_type
#define sipVoidPtr_Type             sipAPI->api_voidptr_type
#define sipGetPyObject              sipAPI->api_get_pyobject
#define sipGetMixinAddress          sipAPI->api_get_mixin_address
#define sipGetCppPtr                sipAPI->api_get_cpp_ptr
#define sipCallHook                 sipAPI->api_call_hook
#define sipEndThread                sipAPI->api_end_thread
#define sipRaiseUnknownException    sipAPI->api_raise_unknown_exception
#define sipRaiseTypeException       sipAPI->api_raise_type_exception
#define sipBadLengthForSlice        sipAPI->api_bad_length_for_slice
#define sipAddTypeInstance          sipAPI->api_add_type_instance
#define sipPySlotExtend             sipAPI->api_py_slot_extend
#define sipAddDelayedDtor           sipAPI->api_add_delayed_dtor
#define sipCanConvertToType         sipAPI->api_can_convert_to_type
#define sipConvertToType            sipAPI->api_convert_to_type
#define sipForceConvertToType       sipAPI->api_force_convert_to_type
#define sipConvertToEnum            sipAPI->api_convert_to_enum
#define sipConvertToBool            sipAPI->api_convert_to_bool
#define sipReleaseType              sipAPI->api_release_type
#define sipConvertFromType          sipAPI->api_convert_from_type
#define sipConvertFromNewType       sipAPI->api_convert_from_new_type
#define sipConvertFromNewPyType     sipAPI->api_convert_from_new_pytype
#define sipConvertFromEnum          sipAPI->api_convert_from_enum
#define sipGetState                 sipAPI->api_get_state
#define sipExportSymbol             sipAPI->api_export_symbol
#define sipImportSymbol             sipAPI->api_import_symbol
#define sipBytes_AsChar             sipAPI->api_bytes_as_char
#define sipBytes_AsString           sipAPI->api_bytes_as_string
#define sipString_AsASCIIChar       sipAPI->api_string_as_ascii_char
#define sipString_AsASCIIString     sipAPI->api_string_as_ascii_string
#define sipString_AsLatin1Char      sipAPI->api_string_as_latin1_char
#define sipString_AsLatin1String    sipAPI->api_string_as_latin1_string
#define sipString_AsUTF8Char        sipAPI->api_string_as_utf8_char
#define sipString_AsUTF8String      sipAPI->api_string_as_utf8_string
#define sipUnicode_AsWChar          sipAPI->api_unicode_as_wchar
#define sipUnicode_AsWString        sipAPI->api_unicode_as_wstring
#define sipConvertFromConstVoidPtr  sipAPI->api_convert_from_const_void_ptr
#define sipConvertFromVoidPtrAndSize    sipAPI->api_convert_from_void_ptr_and_size
#define sipConvertFromConstVoidPtrAndSize   sipAPI->api_convert_from_const_void_ptr_and_size
#define sipWrappedTypeName(wt)      ((wt)->wt_td->td_cname)
#define sipGetReference             sipAPI->api_get_reference
#define sipKeepReference            sipAPI->api_keep_reference
#define sipRegisterPyType           sipAPI->api_register_py_type
#define sipTypeFromPyTypeObject     sipAPI->api_type_from_py_type_object
#define sipTypeScope                sipAPI->api_type_scope
#define sipResolveTypedef           sipAPI->api_resolve_typedef
#define sipEnableAutoconversion     sipAPI->api_enable_autoconversion
#define sipExportModule             sipAPI->api_export_module
#define sipInitModule               sipAPI->api_init_module
#define sipGetInterpreter           sipAPI->api_get_interpreter
#define sipPyTypeName               sipAPI->api_py_type_name
#define sipGetCFunction             sipAPI->api_get_c_function
#define sipGetMethod                sipAPI->api_get_method
#define sipFromMethod               sipAPI->api_from_method
#define sipGetDate                  sipAPI->api_get_date
#define sipFromDate                 sipAPI->api_from_date
#define sipGetDateTime              sipAPI->api_get_datetime
#define sipFromDateTime             sipAPI->api_from_datetime
#define sipGetTime                  sipAPI->api_get_time
#define sipFromTime                 sipAPI->api_from_time
#define sipIsUserType               sipAPI->api_is_user_type
#define sipCheckPluginForType       sipAPI->api_check_plugin_for_type
#define sipUnicodeNew               sipAPI->api_unicode_new
#define sipUnicodeWrite             sipAPI->api_unicode_write
#define sipUnicodeData              sipAPI->api_unicode_data
#define sipGetBufferInfo            sipAPI->api_get_buffer_info
#define sipReleaseBufferInfo        sipAPI->api_release_buffer_info
#define sipIsDerivedClass           sipAPI->api_is_derived_class
#define sipGetUserObject            sipAPI->api_get_user_object
#define sipSetUserObject            sipAPI->api_set_user_object
#define sipRegisterEventHandler     sipAPI->api_register_event_handler
#define sipConvertToArray           sipAPI->api_convert_to_array
#define sipConvertToTypedArray      sipAPI->api_convert_to_typed_array
#define sipEnableGC                 sipAPI->api_enable_gc
#define sipPrintObject              sipAPI->api_print_object
#define sipLong_AsChar              sipAPI->api_long_as_char
#define sipLong_AsSignedChar        sipAPI->api_long_as_signed_char
#define sipLong_AsUnsignedChar      sipAPI->api_long_as_unsigned_char
#define sipLong_AsShort             sipAPI->api_long_as_short
#define sipLong_AsUnsignedShort     sipAPI->api_long_as_unsigned_short
#define sipLong_AsInt               sipAPI->api_long_as_int
#define sipLong_AsUnsignedInt       sipAPI->api_long_as_unsigned_int
#define sipLong_AsLong              sipAPI->api_long_as_long
#define sipLong_AsUnsignedLong      sipAPI->api_long_as_unsigned_long
#define sipLong_AsLongLong          sipAPI->api_long_as_long_long
#define sipLong_AsUnsignedLongLong  sipAPI->api_long_as_unsigned_long_long
#define sipLong_AsSizeT             sipAPI->api_long_as_size_t
#define sipVisitWrappers            sipAPI->api_visit_wrappers
#define sipRegisterExitNotifier     sipAPI->api_register_exit_notifier
#define sipGetFrame                 sipAPI->api_get_frame
#define sipDeprecated               sipAPI->api_deprecated
#define sipPyTypeDictRef            sipAPI->api_py_type_dict_ref
#define sipNextExceptionHandler     sipAPI->api_next_exception_handler
#define sipConvertToTypeUS          sipAPI->api_convert_to_type_us
#define sipForceConvertToTypeUS     sipAPI->api_force_convert_to_type_us
#define sipReleaseTypeUS            sipAPI->api_release_type_us
#define sipIsPyMethod               sipAPI->api_is_py_method
''')

        if self.py_enums_supported():
            sf.write(
f'''#define sipIsEnumFlag               sipAPI->api_is_enum_flag
''')

    def g_slot_support_vars(self, sf):
        """ Generate the variables needed by a slot function. """

        sf.write(
f'''    PyObject *sipModule = sipGetModule(sipSelf, sipWrappedModuleSlots_{self.spec.module.py_name});
    const sipAPIDef *sipAPI = sipGetAPI(sipModule);

''')

    def g_static_variables_table(self, sf, scope=None):
        """ Generate the tables of static variables and types for a scope and
        return a 2-tuple of the length of each table.
        """

        # Do the variables.
        nr_variables = self._g_variables_table(sf, scope, for_unbound=True)

        # Do the types.
        # TODO Check this excludes non-local types.
        module = self.spec.module
        suffix = module.py_name if scope is None else scope.iface_file.fq_cpp_name.as_word

        nr_types = 0

        for type_nr, needed_type in enumerate(module.needed_types):
            if needed_type.type is ArgumentType.CLASS:
                klass = needed_type.definition

                if klass.scope is not scope or klass.external or klass.is_hidden_namespace:
                    continue

                if nr_types == 0:
                    sf.write(f'\nstatic const sipTypeNr sipTypeNrs_{suffix}[] = {{')
                else:
                    sf.write(', ')

                sf.write(str(type_nr))
                nr_types += 1

        if nr_types != 0:
            sf.write('};\n')

        return nr_variables, nr_types

    def g_type_definition(self, sf, bindings, klass, py_debug):
        """ Generate the type structure that contains all the information
        needed by the meta-type.  A sub-set of this is used to extend
        namespaces.
        """

        spec = self.spec
        module = spec.module
        module_name = module.py_name
        klass_name = klass.iface_file.fq_cpp_name.as_word

        # Generate the slots table.
        slots = []
        has_setdelitem_slots = False
        has_rich_compare_slots = False

        if klass.mixin:
            slots.append(('Py_tp_init', 'mixin_' + klass_name))

        for member in klass.members:
            if member.py_slot is None:
                continue

            if member.py_slot in (PySlot.SETITEM, PySlot.DELITEM):
                has_setdelitem_slots = True
                continue

            if is_rich_compare_slot(member.py_slot):
                has_rich_compare_slots = True
                continue

            if member.py_slot is PySlot.GETITEM:
                slots.append(('Py_sq_item', f'slot_{klass_name}___sq_item__'))

            self._append_slot_table_entry(slots, klass_name, member)

        if has_rich_compare_slots:
            slots.append(
                    ('Py_tp_richcompare',
                            f'slot_{klass_name}___richcompare__'))

        if has_setdelitem_slots:
            slots.append(
                    ('Py_mp_ass_subscript',
                            f'slot_{klass_name}___mp_ass_subscript__'))
            slots.append(
                    ('Py_sq_ass_item', f'slot_{klass_name}___sq_ass_item__'))

        if slots:
            sf.write(
f'''

/* Define this type's Python slots. */
static PyType_Slot sip_py_slots_{klass_name}[] = {{
''')

            for slot_id, slot_impl in slots:
                sf.write(f'    {{{slot_id}, (void *){slot_impl}}},\n')

            sf.write('    {}\n};\n')

        # Generate the methods table.
        nr_methods = g_class_method_table(self, sf, bindings, klass)

        # Generate the static variables table.
        nr_static_variables, nr_types = self.g_static_variables_table(sf,
                scope=klass)

        # Generate the instance variables table.
        nr_instance_variables = self._g_variables_table(sf, scope=klass,
                for_unbound=False)

        # Generate the docstring.
        docstring_ref = g_class_docstring(sf, spec, bindings, klass)

        # Generate the type definition itself.
        fields = []

        fields.append(
                '.ctd_base.td_flags = ' + get_class_flags(spec, klass, py_debug))
        fields.append(
                '.ctd_base.td_cname = ' + self.cached_name_ref(klass.iface_file.cpp_name))

        if pyqt5_supported(spec) or pyqt6_supported(spec):
            if g_pyqt_class_plugin(self, sf, bindings, klass):
                fields.append(
                        '.ctd_base.td_plugin_data = &plugin_' + klass_name)

        if klass.real_class is None:
            fields.append(
                    f'.ctd_container.cod_name = "{fmt_class_as_scoped_py_name(klass)}"')

        if klass.real_class is not None:
            cod_scope = self.get_type_ref(klass.real_class)
        elif py_scope(klass.scope) is not None:
            cod_scope = self.get_type_ref(klass.scope)
        else:
            cod_scope = None

        if cod_scope is not None:
            fields.append('.ctd_container.cod_scope = ' + cod_scope)

        if nr_methods != 0:
            fields.append(
                    '.ctd_container.cod_methods = sipMethods_' + klass_name)

        if nr_instance_variables != 0:
            fields.append(
                    '.ctd_container.cod_instance_variables = sipWrappedInstanceVariables_' + klass_name)

        if nr_static_variables != 0:
            fields.append(
                    '.ctd_container.cod_attributes.nr_static_variables = ' + str(nr_static_variables))
            fields.append(
                    '.ctd_container.cod_attributes.static_variables = sipWrappedStaticVariables_' + klass_name)

        if nr_types != 0:
            fields.append(
                    '.ctd_container.cod_attributes.nr_types = ' + str(nr_types))
            fields.append(
                    '.ctd_container.cod_attributes.type_nrs = sipTypeNrs_' + klass_name)


        if slots:
            fields.append(
                    '.ctd_container.cod_py_slots = sip_py_slots_' + klass_name)

        # TODO
        #if self.custom_enums_supported() and nrenummembers > 0:
        #    cod_nrenummembers
        #    cod_enummembers

        fields.append('.ctd_docstring = ' + docstring_ref)

        if klass.metatype is not None:
            fields.append(
                    '.ctd_metatype = ' + self.cached_name_ref(klass.metatype))

        if klass.supertype is not None:
            fields.append(
                    '.ctd_supertype = ' + self.cached_name_ref(klass.supertype))


        # TODO
        #if len(klass.superclasses) != 0:
        #    ctd_supers

        if klass.can_create:
            fields.append('.ctd_init = init_type_' + klass_name)

        if need_dealloc(spec, bindings, klass):
            fields.append('.ctd_dealloc = dealloc_' + klass_name)

        if klass.can_create:
            fields.append(
                    f'.ctd_sizeof = sizeof ({scoped_class_name(self.spec, klass)})')

        if klass.gc_traverse_code is not None:
            fields.append('.ctd_traverse = traverse_' + klass_name)

        if klass.gc_clear_code is not None:
            fields.append('.ctd_clear = clear_' + klass_name)

        if klass.bi_get_buffer_code is not None:
            fields.append('.ctd_getbuffer = getbuffer_' + klass_name)

        if klass.bi_release_buffer_code is not None:
            fields.append('.ctd_releasebuffer = releasebuffer_' + klass_name)

        if spec.c_bindings or klass.needs_copy_helper:
            fields.append('.ctd_assign = assign_' + klass_name)

        if spec.c_bindings or klass.needs_array_helper:
            fields.append('.ctd_array = array_' + klass_name)

        if spec.c_bindings or klass.needs_copy_helper:
            fields.append('.ctd_copy = copy_' + klass_name)

        if not spec.c_bindings and klass.iface_file.type is not IfaceFileType.NAMESPACE:
            fields.append('.ctd_release = release_' + klass_name)

        if len(klass.superclasses) != 0:
            fields.append('.ctd_cast = cast_' + klass_name)

        if klass.convert_to_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE:
            fields.append('.ctd_cto = convertTo_' + klass_name)

        if klass.convert_from_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE:
            fields.append('.ctd_cfrom = convertFrom_' + klass_name)

        if klass.pickle_code is not None:
            fields.append('.ctd_pickle = pickle_' + klass_name)

        if klass.finalisation_code is not None:
            fields.append('.ctd_final = final_' + klass_name)

        if spec.c_bindings or klass.needs_array_helper:
            fields.append('.ctd_array_delete = array_delete_' + klass_name)

        fields = ',\n    '.join(fields)

        sf.write(
f'''

sipClassTypeDef sipTypeDef_{module_name}_{klass_name} = {{
    {fields}
}};
''')

    def g_type_init(self, sf, bindings, klass, need_self, need_owner):
        """ Generate the code that initialises a type. """

        spec = self.spec
        klass_name = klass.iface_file.fq_cpp_name.as_word

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *init_type_{klass_name}(PyObject *, PyObject *const *, Py_ssize_t, PyObject *, PyObject **, PyObject **, PyObject **);}}\n')

        sip_owner = 'sipOwner' if need_owner else ''

        sf.write(
f'''static void *init_type_{klass_name}(PyObject *sipSelf, PyObject *const *sipArgs, Py_ssize_t sipNrArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **{sip_owner}, PyObject **sipParseErr)
{{
''')

        self.g_slot_support_vars(sf)

        g_type_init_body(self, sf, bindings, klass)

        sf.write('}\n')

    @staticmethod
    def cached_name_ref(cached_name, as_nr=False):
        """ Return a reference to a cached name. """

        # In v14 we always use the literal text.
        assert(not as_nr)

        return '"' + cached_name.name + '"'

    def custom_enums_supported(self):
        """ Return True if custom enums are supported. """

        return SipModuleConfiguration.CustomEnums in self.spec.sip_module_configuration

    def get_class_ref_value(self, klass):
        """ Return the value of a class's reference. """

        return f'SIP_TYPE_ID_TYPE_CLASS|SIP_TYPE_ID_CURRENT_MODULE|{klass.iface_file.type_nr}'

    def get_py_method_args(self, *, is_impl, is_module_fn, need_self=False,
            need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        if is_impl:
            if is_module_fn:
                args = 'PyObject *sipModule'
            else:
                args = 'PyObject *sipSelf, PyTypeObject *sipDefType'
        else:
            if is_module_fn:
                args = 'PyObject *'
            else:
                args = 'PyObject *, PyTypeObject *'

        args += ', PyObject *const *'

        if is_impl and need_args:
            args += 'sipArgs'

        args += ', Py_ssize_t'

        if is_impl and need_args:
            args += ' sipNrArgs'

        return args

    @staticmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        if is_rich_compare_slot(slot_type):
            return 'Py_tp_richcompare'

        return _SLOT_ID_MAP[slot_type]

    @staticmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        fq_cpp_name = wrapped_object.fq_cpp_name if isinstance(wrapped_object, WrappedEnum) else wrapped_object.iface_file.fq_cpp_name

        return 'sipTypeID_' + fq_cpp_name.as_word

    @staticmethod
    def get_types_table_prefix():
        """ Return the prefix in the name of the wrapped types table. """

        return 'static const sipTypeDef *const sipTypeDefs'

    @staticmethod
    def get_wrapped_type_type():
        """ Return the type of the C representation of a wrapped object. """

        return 'PyObject *'

    def py_enums_supported(self):
        """ Return True if Python enums are supported. """

        return SipModuleConfiguration.PyEnums in self.spec.sip_module_configuration

    @staticmethod
    def _g_module_clear(sf):
        """ Generate the module clear slot. """

        sf.write(
'''

/* The wrapped module's clear slot. */
static int wrapped_module_clear(PyObject *wmod)
{
    sipWrappedModuleState *wms = (sipWrappedModuleState *)PyModule_GetState(
            wmod);

    return wms->sip_api->api_wrapped_module_clear(wms);
}
''')

    def _g_module_exec(self, sf):
        """ Generate the module exec slot. """

        spec = self.spec
        sip_module_name = spec.sip_module
        module = spec.module
        module_name = module.py_name

        sf.write(
'''

/* The wrapped module's exec function. */
static int wrapped_module_exec(PyObject *sipModule)
{
''')

        sf.write_code(module.preinitialisation_code)

        if sip_module_name:
            sip_init_func_ref = 'sip_init_func'
            sip_module_ref = 'sip_sip_module'
            sf.write(
f'''    PyObject *{sip_module_ref} = PyImport_ImportModule("{sip_module_name}");
    if ({sip_module_ref} == SIP_NULLPTR)
        return -1;

    PyObject *sip_capsule = PyDict_GetItemString(PyModule_GetDict(sip_sip_module), "_C_BOOTSTRAP");
    if (!PyCapsule_IsValid(sip_capsule, "_C_BOOTSTRAP"))
    {{
        Py_XDECREF(sip_capsule);
        Py_DECREF(sip_sip_module);
        return -1;
    }}

    sipBootstrapFunc sip_bootstrap = (sipBootstrapFunc)PyCapsule_GetPointer(sip_capsule, "_C_BOOTSTRAP");
    Py_DECREF(sip_capsule);

    sipWrappedModuleInitFunc {sip_init_func_ref} = sip_bootstrap({spec.target_abi[0]});
    if ({sip_init_func_ref} == SIP_NULLPTR)
    {{
        Py_DECREF({sip_module_ref});
        return -1;
    }}

''')
        else:
            sip_init_func_ref = 'sip_api_wrapped_module_init';
            sip_module_ref = 'SIP_NULLPTR';

        sf.write_code(module.initialisation_code)

        g_pyqt_helper_init(sf, spec)

        sf.write(
f'''    if ({sip_init_func_ref}(sipModule, &sipWrappedModule_{module_name}, {sip_module_ref}) < 0)
        return -1;
''')

        # TODO Handle post-initialisation code.  Get the module dict if the
        # code uses it (sipModuleDict).

        sf.write(
'''
    return 0;
}
''')

    def _g_module_free(self, sf):
        """ Generate the module free slot. """

        # Note that this will be called if wrapped_module_exec() fails in any
        # way, so we can't assume the sip API is available.
        sf.write(
'''

/* The wrapped module's free slot. */
static void wrapped_module_free(void *wmod_ptr)
{
    sipWrappedModuleState *wms = (sipWrappedModuleState *)PyModule_GetState(
            (PyObject *)wmod_ptr);

    if (wms->sip_api != NULL)
        wms->sip_api->api_wrapped_module_free(wms);
}
''')

    @staticmethod
    def _g_module_function_table_entries(sf, bindings, module, members,
            has_module_functions=False):
        """ Generate the entries in a table of PyMethodDef for module
        functions.
        """

        for member in members:
            if member.py_slot is None:
                if not has_module_functions:
                    sf.write(f'\n\nstatic PyMethodDef sip_methods_{module.py_name}[] = {{\n')
                    has_module_functions = True

                py_name = member.py_name.name

                sf.write(f'    {{"{py_name}", SIP_MLMETH_CAST(func_{py_name}), METH_FASTCALL')

                if member.no_arg_parser or member.allow_keyword_args:
                    sf.write('|METH_KEYWORDS')

                docstring_ref = get_optional_ptr(
                        has_method_docstring(bindings, member,
                                module.overloads),
                        'doc_' + member.py_name.name)
                sf.write(f', {docstring_ref}}},\n')

        return has_module_functions

    @staticmethod
    def _g_module_traverse(sf):
        """ Generate the module traverse slot. """

        sf.write(
'''

/* The wrapped module's traverse slot. */
static int wrapped_module_traverse(PyObject *wmod, visitproc visit, void *arg)
{
    sipWrappedModuleState *wms = (sipWrappedModuleState *)PyModule_GetState(
            wmod);

    return wms->sip_api->api_wrapped_module_traverse(wms, visit, arg);
}
''')

    def _g_variables_table(self, sf, scope, *, for_unbound):
        """ Generate the table of either bound or unbound variables for a scope
        and return the length of the table.
        """

        spec = self.spec
        c_bindings = spec.c_bindings
        module = spec.module

        # Get the sorted list of variables.
        variables = list(variables_in_scope(spec, scope, check_handler=False))
        variables.sort(key=lambda k: k.py_name.name)

        table = []

        for variable in variables:
            # Check we are handling this sort of variable.
            if scope is None or variable.is_static:
                if not for_unbound:
                    continue
            elif for_unbound:
                continue

            v_type = variable.type
            v_ref = variable.fq_cpp_name.as_word

            # Generally const variables cannot be set.  However for string
            # pointers the reverse is true as a const pointer can be replaced
            # by another, but we can't allow a the contents of a non-const
            # string/array to be modified by C/C++ because they are immutable
            # in Python.
            not_settable = False
            might_need_key = False

            # TODO Unnamed enums, custom enums, Python enums and classes/mapped
            # types.
            if v_type.type is ArgumentType.CLASS or (v_type.type is ArgumentType.ENUM and v_type.definition.fq_cpp_name is not None):
                pass

            elif v_type.type is ArgumentType.BYTE:
                type_id = 'sipTypeID_byte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SBYTE:
                type_id = 'sipTypeID_sbyte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.UBYTE:
                type_id = 'sipTypeID_ubyte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SHORT:
                type_id = 'sipTypeID_short'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.USHORT:
                type_id = 'sipTypeID_ushort'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.INT, ArgumentType.CINT):
                type_id = 'sipTypeID_int'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.UINT:
                type_id = 'sipTypeID_uint'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.LONG:
                type_id = 'sipTypeID_long'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.ULONG:
                type_id = 'sipTypeID_ulong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.LONGLONG:
                type_id = 'sipTypeID_longlong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.ULONGLONG:
                type_id = 'sipTypeID_ulonglong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.HASH:
                type_id = 'sipTypeID_Py_hash_t'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SSIZE:
                type_id = 'sipTypeID_Py_ssize_t'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SIZE:
                type_id = 'sipTypeID_size_t'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
                type_id = 'sipTypeID_float'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
                type_id = 'sipTypeID_double'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_char'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_str'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.ASCII_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_char_ascii'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_str_ascii'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.LATIN1_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_char_latin1'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_str_latin1'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.UTF8_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_char_utf8'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_str_utf8'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.SSTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_schar'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_sstr'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.USTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_uchar'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipTypeID_ustr'
                    not_settable = not v_type.is_const
                    might_need_key = True

            elif v_type.type is ArgumentType.WSTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipTypeID_wchar'
                    not_settable = v_type.is_const
                else:
                    # Note that wchar_t strings/arrays are mutable.
                    type_id = 'sipTypeID_wstr'
                    might_need_key = True

            elif v_type.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
                type_id = 'sipTypeID_bool'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.VOID:
                # This is the only type that we need to make a distinction
                # between const and non-const (because if affects the behaviour
                # of a corresponding voidptr instance).  Using a flag
                # (potentially applicable to all types) would smell better but
                # we don't have anywhere to store it.  (SIP_WV_RO is a special
                # value rather than a flag).
                type_id = 'sipTypeID_voidptr_const' if v_type.is_const else 'sipTypeID_voidptr'

            elif v_type.type is ArgumentType.PYOBJECT:
                type_id = 'sipTypeID_pyobject'

            elif v_type.type is ArgumentType.PYTUPLE:
                type_id = 'sipTypeID_pytuple'

            elif v_type.type is ArgumentType.PYLIST:
                type_id = 'sipTypeID_pylist'

            elif v_type.type is ArgumentType.PYDICT:
                type_id = 'sipTypeID_pydict'

            elif v_type.type is ArgumentType.PYCALLABLE:
                type_id = 'sipTypeID_pycallable'

            elif v_type.type is ArgumentType.PYSLICE:
                type_id = 'sipTypeID_pyslice'

            elif v_type.type is ArgumentType.PYTYPE:
                type_id = 'sipTypeID_pytype'

            elif v_type.type is ArgumentType.PYBUFFER:
                type_id = 'sipTypeID_pybuffer'

            elif v_type.type is ArgumentType.CAPSULE:
                type_id = 'sipTypeID_pycapsule'

            else:
                continue

            read_only = not_settable or variable.no_setter

            fields = []

            fields.append(f'.name = "{variable.py_name.name}"')
            fields.append('.type_id = ' + type_id)

            if read_only:
                fields.append('.key = SIP_WV_RO')
            elif might_need_key:
                fields.append('.key = ' + str(module.next_key))
                module.next_key -= 1

            if scope is None or variable.is_static:
                # TODO Why STRIP_GLOBAL here in particular?
                cpp_name = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
                address = '&' + cpp_name
            else:
                address = 'sipWrappedVariableAddrGetter_' + v_ref

            fields.append('.address = (void *)' + address)

            if variable.get_code is not None:
                fields.append('.get_code = sipWrappedVariableGetCode_' + v_ref)

            if variable.set_code is not None:
                fields.append('.set_code = sipWrappedVariableSetCode_' + v_ref)

            table.append(fields)

            # Generate any %GetCode wrapper.
            if variable.get_code is not None:
                # TODO Support sipPyType when scope is not None.
                # TODO Review the need to cache class instances (see legacy
                # variable handlers).  Or is that now in the sip module
                # wrapper?
                sf.write('\n')

                if not c_bindings:
                    sf.write(f'extern "C" {{static PyObject *sipWrappedVariableGetCode_{v_ref}();}}\n')

                sf.write(
f'''static PyObject *sipWrappedVariableGetCode_{v_ref}()
{{
    PyObject *sipPy;

''')

                sf.write_code(variable.get_code)

                sf.write(
'''
    return sipPy;
}

''')

            # Generate any %SetCode wrapper.
            if variable.set_code is not None:
                # TODO Support sipPyType when scope is not None.
                sf.write('\n')

                if not c_bindings:
                    sf.write(f'extern "C" {{static int sipWrappedVariableSetCode_{v_ref}(PyObject *);}}\n')

                sf.write(
f'''static int sipWrappedVariableSetCode_{v_ref}(PyObject *sipPy)
{{
    int sipErr = 0;

''')

                sf.write_code(variable.set_code)

                sf.write(
'''
    return sipErr ? -1 : 0;
}

''')

            # See if we need a descriptor address getter.
            if scope is None or variable.is_static:
                continue

            sf.write('\n\n')

            # TODO Why STRIP_GLOBAL here in particular?
            scope_name = scope.iface_file.fq_cpp_name.cpp_stripped(
                    STRIP_GLOBAL)
            cast = get_type_from_void(spec, scope_name, 'sipCppV')

            if not c_bindings:
                sf.write(f'extern "C" {{static void *sipWrappedVariableAddrGetter_{v_ref}(void *);}}\n')

            sf.write(
f'''static void *sipWrappedVariableAddrGetter_{v_ref}(void *sipCppV)
{{
    return &{cast}->{variable.py_name.name};
}}
''')

        nr_variables = len(table)

        if nr_variables != 0:
            if scope is None:
                scope_type = 'module'
                table_type = 'Module'
                suffix = module.py_name
            else:
                scope_type = 'type'
                table_type = 'Static' if for_unbound else 'Instance'
                suffix = scope.iface_file.fq_cpp_name.as_word

            sf.write(
f'''
/* Define the {table_type.lower()} variables for the {scope_type}. */
static const sipWrappedVariableDef sipWrapped{table_type}Variables_{suffix}[] = {{
''')

            for fields in table:
                line = ', '.join(fields)
                sf.write(f'    {{{line}}},\n')

            if not for_unbound:
                sf.write('    {}\n')

            sf.write('};\n')

        return nr_variables

    @staticmethod
    def _append_slot_table_entry(slots, scope_name, member):
        """ Append an entry in the slot table for a scope. """

        # setitem, delitem and the rich comparison slots are handled elsewhere.

        py_slot = member.py_slot
        py_name = member.py_name

        slot_id = _SLOT_ID_MAP[py_slot]
        slots.append((slot_id, f'slot_{scope_name}_{py_name}'))

        # __len__ is placed in two slots.
        if py_slot is PySlot.LEN:
            slots.append(('Py_sq_length', f'slot_{scope_name}_{py_name}'))


# The mapping of slots to Python slot IDs.
_SLOT_ID_MAP = {
    PySlot.STR: 'Py_tp_str',
    PySlot.INT: 'Py_nb_int',
    PySlot.FLOAT: 'Py_nb_float',
    PySlot.LEN: 'Py_mp_length',
    PySlot.CONTAINS: 'Py_sq_contains',
    PySlot.ADD: 'Py_nb_add',
    PySlot.CONCAT: 'Py_sq_concat',
    PySlot.SUB: 'Py_nb_subtract',
    PySlot.MUL: 'Py_nb_multiply',
    PySlot.REPEAT: 'Py_sq_repeat',
    PySlot.MOD: 'Py_nb_remainder',
    PySlot.FLOORDIV: 'Py_nb_floor_divide',
    PySlot.TRUEDIV: 'Py_nb_true_divide',
    PySlot.AND: 'Py_nb_and',
    PySlot.OR: 'Py_nb_or',
    PySlot.XOR: 'Py_nb_xor',
    PySlot.LSHIFT: 'Py_nb_lshift',
    PySlot.RSHIFT: 'Py_nb_rshift',
    PySlot.IADD: 'Py_nb_inplace_add',
    PySlot.ICONCAT: 'Py_sq_inplace_concat',
    PySlot.ISUB: 'Py_nb_inplace_subtract',
    PySlot.IMUL: 'Py_nb_inplace_multiply',
    PySlot.IREPEAT: 'Py_sq_inplace_repeat',
    PySlot.IMOD: 'Py_nb_inplace_remainder',
    PySlot.IFLOORDIV: 'Py_nb_inplace_floor_divide',
    PySlot.ITRUEDIV: 'Py_nb_inplace_true_divide',
    PySlot.IAND: 'Py_nb_inplace_and',
    PySlot.IOR: 'Py_nb_inplace_or',
    PySlot.IXOR: 'Py_nb_inplace_xor',
    PySlot.ILSHIFT: 'Py_nb_inplace_lshift',
    PySlot.IRSHIFT: 'Py_nb_inplace_rshift',
    PySlot.INVERT: 'Py_nb_invert',
    PySlot.CALL: 'Py_tp_call',
    PySlot.GETITEM: 'Py_mp_subscript',
    PySlot.BOOL: 'Py_nb_bool',
    PySlot.NEG: 'Py_nb_negative',
    PySlot.REPR: 'Py_tp_repr',
    PySlot.HASH: 'Py_tp_hash',
    PySlot.POS: 'Py_nb_positive',
    PySlot.ABS: 'Py_nb_absolute',
    PySlot.INDEX: 'Py_nb_index',
    PySlot.ITER: 'Py_tp_iter',
    PySlot.NEXT: 'Py_tp_iternext',
    PySlot.SETATTR: 'Py_tp_setattro',
    PySlot.MATMUL: 'Py_nb_matrix_multiply',
    PySlot.IMATMUL: 'Py_nb_inplace_matrix_multiply',
    PySlot.AWAIT: 'Py_am_await',
    PySlot.AITER: 'Py_am_aiter',
    PySlot.ANEXT: 'Py_am_anext',
}
