# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from ....python_slots import (is_hash_return_slot, is_inplace_number_slot,
        is_int_arg_slot, is_int_return_slot, is_number_slot, is_multi_arg_slot,
        is_rich_compare_slot, is_ssize_return_slot, is_void_return_slot,
        is_zero_arg_slot)
from ....scoped_name import STRIP_GLOBAL
from ....specification import (AccessSpecifier, ArgumentType, ArrayArgument,
        DocstringSignature, IfaceFileType, KwArgs, MappedType,
        PyQtMethodSpecifier, PySlot, Transfer, WrappedClass, WrappedEnum)
from ....utils import find_method

from ...formatters import fmt_argument_as_cpp_type, fmt_argument_as_name

from ..snippets import (g_argument_variable, g_call_args, g_ctor_type_hint,
        g_delete_temporaries, g_function_body, g_overload_type_hint,
        g_type_init_body, g_static_function)
from ..utils import (callable_overloads, get_class_from_void, get_const_cast,
        get_docstring_text, get_encoded_type, get_enum_member,
        get_function_table, get_mapped_type_flags, get_method_table,
        get_named_value_decl, get_normalised_cached_name, get_optional_ptr,
        get_use_in_code, get_user_state_suffix, get_void_ptr_cast,
        has_method_docstring, is_string, is_used_in_code, keep_py_reference,
        module_classes, need_dealloc, py_scope, pyqt5_supported,
        pyqt6_supported, scoped_class_name, scoped_variable_name,
        type_needs_user_state, variables_in_scope)

from .abstract_backend import AbstractBackend


class v12v13Backend(AbstractBackend):
    """ The backend code generator for v12 and v13 of the ABI. """

    def g_arg_parser(self, sf, scope, py_signature, signature_nr, ctor=None,
        is_method=False, overload=None):
        """ Generate an argument parser call. """

        return _g_arg_parser(self, sf, scope, py_signature, signature_nr, ctor,
                is_method, overload)

    def g_cast_function(self, sf, klass):
        """ Generate the function that casts a C++ pointer to a target type.
        """

        spec = self.spec
        as_word = klass.iface_file.fq_cpp_name.as_word

        sf.write(
f'''

/* Cast a pointer to a type somewhere in its inheritance hierarchy. */
extern "C" {{static void *cast_{as_word}(void *, const sipTypeDef *);}}
static void *cast_{as_word}(void *sipCppV, const sipTypeDef *targetType)
{{
    {get_class_from_void(spec, klass)};

    if (targetType == {_get_type_ref(klass)})
        return sipCppV;

''')

        for superclass in klass.superclasses:
            sc_fq_cpp_name = superclass.iface_file.fq_cpp_name
            sc_scope_s = scoped_class_name(spec, superclass)
            sc_type_ref = _get_type_ref(superclass)

            if len(superclass.superclasses) != 0:
                # Delegate to the super-class's cast function.  This will
                # handle virtual and non-virtual diamonds.
                sf.write(
f'''    sipCppV = ((const sipClassTypeDef *){sc_type_ref})->ctd_cast(static_cast<{sc_scope_s} *>(sipCpp), targetType);
    if (sipCppV)
        return sipCppV;

''')
            else:
                # The super-class is a base class and so doesn't have a cast
                # function.  It also means that a simple check will do instead.
                sf.write(
f'''    if (targetType == {sc_type_ref})
        return static_cast<{sc_scope_s} *>(sipCpp);

''')

        sf.write(
'''    return SIP_NULLPTR;
}
''')

    def g_catch_body(self, sf):
        """ Generate the body of a catch clause. """

        sf.write(
'''                void *sipExcState = SIP_NULLPTR;
                sipExceptionHandler sipExcHandler;
                std::exception_ptr sipExcPtr = std::current_exception();

                while ((sipExcHandler = sipNextExceptionHandler(&sipExcState)) != SIP_NULLPTR)
                    if (sipExcHandler(sipExcPtr))
                        return SIP_NULLPTR;

''')

    def g_class_api(self, sf, klass):
        """ Generate the API details for a class. """

        module = self.spec.module
        module_name = module.py_name
        iface_file = klass.iface_file

        if iface_file.module is module:
            sf.write(f'#define {_get_type_ref(klass)} sipExportedTypes_{module_name}[{iface_file.type_nr}]\n')
        else:
            type_ref = _get_type_ref(klass)

            if iface_file.type is IfaceFileType.NAMESPACE:
                sf.write(f'\n#if !defined({type_ref})')

            sf.write(f'\n#define {type_ref} sipImportedTypes_{module_name}_{iface_file.module.py_name}[{iface_file.type_nr}].it_td\n')

            if iface_file.type is IfaceFileType.NAMESPACE:
                sf.write('#endif\n')

    def g_class_spec_extern_decl(self, sf, klass):
        """ Generate the extern declaration of a class specification. """

        module_name = self.spec.module.py_name
        klass_name = klass.iface_file.fq_cpp_name.as_word

        sf.write(f'\nextern sipClassTypeDef sipTypeDef_{module_name}_{klass_name};\n')

    @staticmethod
    def g_conversion_to_enum(sf, enum):
        """ Generate the code to convert a Python enum (sipSelf) to a C/C++
        enum (sipCpp).
        """

        _g_conversion_to_enum(sf, enum)

    def g_cpp_dtor(self, sf):
        """ Generate the body of the dtor of a generated shadow class. """

        sf.write('    sipInstanceDestroyedEx(&sipPySelf);\n')

    def g_create_wrapped_module(self, sf, bindings,
        name_cache_state,
        has_external,
        enums_state,
        has_virtual_error_handlers,
        nr_subclass_convertors,
        inst_state,
        has_slot_extenders,
        has_init_extenders
    ):
        """ Generate the code to create a wrapped module and return the name
        cache state.
        """

        spec = self.spec
        target_abi = spec.target_abi
        module = spec.module
        module_name = module.py_name
        fq_py_name_ref = _get_cached_name_ref(module.fq_py_name, as_nr=True)

        sf.write('\n\n')

        # Generate any Qt support API.
        if spec.target_abi < (13, 0) and self._legacy_qt_support():
            sf.write(
f'''/* This defines the Qt support API. */

static sipQtAPI qtAPI = {{
    &sipExportedTypes_{module_name}[{spec.pyqt_qobject.iface_file.type_nr}],
    sipQtCreateUniversalSignal,
    sipQtFindUniversalSignal,
    sipQtCreateUniversalSlot,
    sipQtDestroyUniversalSlot,
    sipQtFindSlot,
    sipQtConnect,
    sipQtDisconnect,
    sipQtSameSignalSlotName,
    sipQtFindSipslot,
    sipQtEmitSignal,
    sipQtConnectPySignal,
    sipQtDisconnectPySignal
}};
''')

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
            qt_api = get_optional_ptr(self._legacy_qt_support(), '&qtAPI')
            sf.write(f'    {qt_api},\n')

        sf.write(
f'''    {len(module.needed_types)},
    {exported_types},
    {external_types},
''')

        if self.custom_enums_supported():
            nr_enum_members, _ = enums_state
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
        slot_extender_table = get_optional_ptr(has_slot_extenders,
                'slotExtenders')
        init_extender_table = get_optional_ptr(has_init_extenders,
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

        if module.docstring is not None:
            sf.write(
f'''
PyDoc_STRVAR(doc_mod_{module_name}, "{get_docstring_text(module.docstring)}");
''')

        # Generate the storage for the external API pointers.
        sf.write(
f'''

/* The SIP API and the APIs of any imported modules. */
const sipAPIDef *sipAPI_{module_name};
''')

        _g_pyqt_helper_defns(sf, spec)
        self.g_module_init_start(sf)
        has_module_functions = self._g_module_functions_table(sf, bindings,
                module)
        self.g_module_definition(sf, bindings,
                has_module_functions=has_module_functions)
        self._g_module_init_body(sf)

        return name_cache_state

    def g_enums_specifications(self, sf, bindings, scope=None):
        """ Generate the specifications for the wrapped enums in a scope and
        return the total number of enum members.
        """

        spec = self.spec
        needed_enums = []

        # We generate the enum definitions for all scopes in the same place.
        if scope is None:
            # Generate any enum slot tables.
            for enum in spec.enums:
                if enum.module is not spec.module or enum.fq_cpp_name is None:
                    continue

                if len(enum.slots) == 0:
                    continue

                for member in enum.slots:
                    _g_py_slot(self, sf, bindings, member, scope=enum)

                enum_name = enum.fq_cpp_name.as_word

                sf.write(
f'''
static sipPySlotDef slots_{enum_name}[] = {{
''')

                for member in enum.slots:
                    if member.py_slot is not None:
                        slot_ref = _get_slot_ref(member.py_slot)
                        sf.write(f'    {{(void *)slot_{enum_name}_{member.py_name}, {slot_ref}}},\n')

                sf.write(
'''    {SIP_NULLPTR, (sipPySlotType)0}
};

''')

            self._g_enums_defs(sf, needed_enums)

        if not self.custom_enums_supported():
            return -1, needed_enums

        enum_members = []

        for enum in spec.enums:
            if enum.module is not spec.module:
                continue

            enum_py_scope = py_scope(enum.scope)

            if isinstance(scope, WrappedClass):
                # The scope is a class.
                if enum_py_scope is not scope or (enum.is_protected and not scope.has_shadow):
                    continue

            elif scope is not None:
                # The scope is a mapped type.
                if enum.scope != scope:
                    continue

            elif enum_py_scope is not None or isinstance(enum.scope, MappedType) or enum.fq_cpp_name is None:
                continue

            enum_members.extend(enum.members)

        nr_members = len(enum_members)
        if nr_members == 0:
            return 0, needed_enums

        enum_members.sort(key=lambda v: v.scope.type_nr)
        enum_members.sort(key=lambda v: v.py_name.name)

        if py_scope(scope) is None:
            sf.write(
'''
/* These are the enum members of all global enums. */
static sipEnumMemberDef enummembers[] = {
''')
        else:
            sf.write(
f'''
static sipEnumMemberDef enummembers_{scope.iface_file.fq_cpp_name.as_word}[] = {{
''')

        for enum_member in enum_members:
            sf.write(f'    {{{_get_cached_name_ref(enum_member.py_name)}, ')
            sf.write(self._get_cast_enum_member(enum_member))
            sf.write(f', {enum_member.scope.type_nr}}},\n')

        sf.write('};\n')

        return len(enum_members), needed_enums

    def g_exceptions_decls(self, sf):
        """ Generate the declarations of all exceptions. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name
        no_exceptions = True

        for exception in spec.exceptions:
            if exception.iface_file.module is module and exception.exception_nr >= 0:
                if no_exceptions:
                    sf.write(
f'''
/* The exceptions defined in this module. */
extern PyObject *sipExportedExceptions_{module_name}[];

''')

                    no_exceptions = False

                sf.write(f'#define sipException_{exception.iface_file.fq_cpp_name.as_word} sipExportedExceptions_{module_name}[{exception.exception_nr}]\n')

    def g_exceptions_defn(self, sf):
        """ Generate the definition of the exceptions data structure. """

        module = self.spec.module

        sf.write(
f'''

PyObject *sipExportedExceptions_{module.py_name}[{module.nr_exceptions + 1}];
''')

    def g_externals(self, sf):
        """ Generate the external types.  Return True if something was
        generated.
        """

        spec = self.spec
        module = spec.module

        has_external = False

        for klass in spec.classes:
            if not klass.external:
                continue

            if klass.iface_file.module is not module:
                continue

            if not has_external:
                sf.write(
'''

/* This defines each external type declared in this module, */
static sipExternalTypeDef externalTypesTable[] = {
''')

                has_external = True

            type_nr = klass.iface_file.type_nr
            klass_py = klass.iface_file.fq_cpp_name.as_py

            sf.write(f'    {{{type_nr}, "{klass_py}"}},\n')

        if has_external:
            sf.write(
'''    {-1, SIP_NULLPTR}
};
''')

        return has_external

    def g_get_py_reimpl(self, sf, klass, overload, virt_nr):
        """ Generate the code to get the Python reimplementation of a C++
        virtual.
        """

        if overload.is_const:
            const_cast_char = 'const_cast<char *>('
            const_cast_sw = 'const_cast<sipSimpleWrapper **>('
            const_cast_tail = ')'
        else:
            const_cast_char = ''
            const_cast_sw = ''
            const_cast_tail = ''

        abi_12_8_arg = f'{const_cast_sw}&sipPySelf{const_cast_tail}, ' if self.spec.target_abi >= (12, 8) else ''

        klass_py_name_ref = _get_cached_name_ref(klass.py_name) if overload.is_abstract else 'SIP_NULLPTR'
        member_py_name_ref = _get_cached_name_ref(overload.common.py_name)

        sf.write(f'\n    sipMeth = sipIsPyMethod(&sipGILState, {const_cast_char}&sipPyMethods[{virt_nr}]{const_cast_tail}, {abi_12_8_arg}{klass_py_name_ref}, {member_py_name_ref});\n')

    def g_import_tables(self, sf):
        """ Generated the tables related to imported modules. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        for imported_module in module.all_imports:
            imported_module_name = imported_module.py_name

            if len(imported_module.needed_types) != 0:
                sf.write(
f'''

/* This defines the types that this module needs to import from {imported_module_name}. */
sipImportedTypeDef sipImportedTypes_{module_name}_{imported_module_name}[] = {{
''')

                for needed_type in imported_module.needed_types:
                    if needed_type.type is ArgumentType.MAPPED:
                        type_name = needed_type.definition.cpp_name
                    else:
                        if needed_type.type is ArgumentType.CLASS:
                            scoped_name = needed_type.definition.iface_file.fq_cpp_name
                        else:
                            scoped_name = needed_type.definition.fq_cpp_name

                        type_name = scoped_name.cpp_stripped(STRIP_GLOBAL)

                    sf.write(f'    {{"{type_name}"}},\n')

                sf.write(
'''    {SIP_NULLPTR}
};
''')

            if imported_module.nr_virtual_error_handlers > 0:
                sf.write(
f'''

/*
 * This defines the virtual error handlers that this module needs to import
 * from {imported_module_name}.
 */
sipImportedVirtErrorHandlerDef sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}[] = {{
''')

                # The handlers are unordered so search for each in turn.  There
                # will probably be only one so speed isn't an issue.
                for i in range(imported_module.nr_virtual_error_handlers):
                    for handler in spec.virtual_error_handlers:
                        if handler.module is imported_module and handler.handler_nr == i:
                            sf.write(f'    {{"{handler.name}"}},\n')

                sf.write(
'''    {SIP_NULLPTR}
};
''')

            if imported_module.nr_exceptions > 0:
                sf.write(
f'''

/*
 * This defines the exception objects that this module needs to import from
 * {imported_module_name}.
 */
sipImportedExceptionDef sipImportedExceptions_{module_name}_{imported_module_name}[] = {{
''')

                # The exceptions are unordered so search for each in turn.
                # There will probably be very few so speed isn't an issue.
                for i in range(imported_module.nr_exceptions):
                    for exception in spec.exceptions:
                        if exception.iface_file.module is imported_module and exception.exception_nr == i:
                            sf.write(f'    {{"{exception.py_name}"}},\n')

                sf.write(
'''    {SIP_NULLPTR}
};
''')

        sf.write(
'''

/* This defines the modules that this module needs to import. */
static sipImportedModuleDef importsTable[] = {
''')

        for imported_module in module.all_imports:
            imported_module_name = imported_module.py_name

            types = handlers = exceptions = 'SIP_NULLPTR'

            if len(imported_module.needed_types) != 0:
                types = f'sipImportedTypes_{module_name}_{imported_module_name}'

            if imported_module.nr_virtual_error_handlers != 0:
                handlers = f'sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}'

            if imported_module.nr_exceptions != 0:
                exceptions = f'sipImportedExceptions_{module_name}_{imported_module_name}'

            sf.write(f'    {{"{imported_module.fq_py_name}", {types}, {handlers}, {exceptions}}},\n')

        sf.write(
'''    {SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}
};
''')

    def g_imported_module_decls(self, sf, imported_module):
        """ Generate any declarations related to an imported module. """

        module_name = self.spec.module.py_name
        imported_module_name = imported_module.py_name

        if len(imported_module.needed_types) != 0:
            sf.write(f'extern sipImportedTypeDef sipImportedTypes_{module_name}_{imported_module_name}[];\n')

        if imported_module.nr_virtual_error_handlers != 0:
            sf.write(f'extern sipImportedVirtErrorHandlerDef sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}[];\n')

        if imported_module.nr_exceptions != 0:
            sf.write(f'extern sipImportedExceptionDef sipImportedExceptions_{module_name}_{imported_module_name}[];\n')

    def g_init_extenders_table(self, sf):
        """ Generate the init extenders table. """

        spec = self.spec
        module = spec.module

        sf.write(
'''
static sipInitExtenderDef initExtenders[] = {
''')

        first_field = '-1, ' if spec.target_abi < (13, 0) else ''

        for klass in module.extenders:
            if len(klass.ctors) != 0:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                encoded_type = get_encoded_type(module, klass)

                sf.write(f'    {{{first_field}init_type_{klass_name}, {encoded_type}, SIP_NULLPTR}},\n')

        sf.write(
f'''    {{{first_field}SIP_NULLPTR, {{0, 0, 0}}, SIP_NULLPTR}}
}};
''')

    def g_mapped_type_api(self, sf, mapped_type):
        """ Generate the API details for a mapped type. """

        spec = self.spec
        module = spec.module
        iface_file = mapped_type.iface_file

        module_name = module.py_name
        mapped_type_name = iface_file.fq_cpp_name.as_word

        if iface_file.module is spec.module:
            sf.write(
f'''
#define {_get_type_ref(mapped_type)} sipExportedTypes_{module_name}[{iface_file.type_nr}]

extern sipMappedTypeDef sipTypeDef_{module_name}_{mapped_type_name};
''')
        else:
            sf.write(
f'''
#define {_get_type_ref(mapped_type)} sipImportedTypes_{module_name}_{iface_file.module.py_name}[{iface_file.type_nr}].it_td
''')

    def g_mapped_type_definition(self, sf, bindings, mapped_type):
        """ Generate the type structure that contains all the information
        needed by a mapped type.
        """

        spec = self.spec
        mapped_type_name = mapped_type.iface_file.fq_cpp_name.as_word

        members = get_function_table(mapped_type.members)
        cod_nrmethods = self._g_py_method_table(sf, bindings, members,
                mapped_type)

        id_int = 'SIP_NULLPTR'

        if self.custom_enums_supported():
            cod_nrenummembers, _ = self.g_enums_specifications(sf, bindings,
                    scope=mapped_type)
            has_ints = False
            needs_namespace = (cod_nrenummembers > 0)
        else:
            if self.g_mapped_type_int_instances(sf, mapped_type):
                id_int = 'intInstances_' + mapped_type_name
                needs_namespace = True
            else:
                needs_namespace = False

        if cod_nrmethods > 0:
            needs_namespace = True

        if pyqt6_supported(spec) and mapped_type.pyqt_flags != 0:
            sf.write(f'\n\nstatic pyqt6MappedTypePluginDef plugin_{mapped_type_name} = {{{mapped_type.pyqt_flags}}};\n')

            td_plugin_data = '&plugin_' + mapped_type_name
        else:
            td_plugin_data = 'SIP_NULLPTR'

        sf.write(
f'''

sipMappedTypeDef sipTypeDef_{mapped_type.iface_file.module.py_name}_{mapped_type_name} = {{
    {{
''')

        if spec.target_abi < (13, 0):
            sf.write(
'''        -1,
        SIP_NULLPTR,
''')

        td_flags = get_mapped_type_flags(mapped_type)
        td_cname = _get_cached_name_ref(mapped_type.cpp_name, as_nr=True)
        cod_name = _get_cached_name_ref(mapped_type.py_name, as_nr=True) if needs_namespace else '-1'
        cod_methods = 'SIP_NULLPTR' if cod_nrmethods == 0 else 'methods_' + mapped_type_name

        sf.write(
f'''        SIP_NULLPTR,
        {td_flags},
        {td_cname},
        SIP_NULLPTR,
        {td_plugin_data},
    }},
    {{
        {cod_name},
        {{0, 0, 1}},
        {cod_nrmethods}, {cod_methods},
''')

        if self.custom_enums_supported():
            cod_enummembers = 'SIP_NULLPTR' if cod_nrenummembers == 0 else 'enummembers_' + mapped_type_name

            sf.write(
f'''        {cod_nrenummembers}, {cod_enummembers},
''')

        mtd_assign = 'SIP_NULLPTR' if mapped_type.no_assignment_operator and not mapped_type.movable else 'assign_' + mapped_type_name
        mtd_array = 'SIP_NULLPTR' if mapped_type.no_default_ctor else 'array_' + mapped_type_name
        mtd_copy = 'SIP_NULLPTR' if mapped_type.no_copy_ctor else 'copy_' + mapped_type_name
        mtd_release = 'SIP_NULLPTR' if mapped_type.no_release else 'release_' + mapped_type_name
        mtd_cto = 'SIP_NULLPTR' if mapped_type.convert_to_type_code is None else 'convertTo_' + mapped_type_name
        mtd_cfrom = 'SIP_NULLPTR' if mapped_type.convert_from_type_code is None else 'convertFrom_' + mapped_type_name

        sf.write(
f'''        0, SIP_NULLPTR,
        {{SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, {id_int}, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}}
    }},
    {mtd_assign},
    {mtd_array},
    {mtd_copy},
    {mtd_release},
    {mtd_cto},
    {mtd_cfrom}
}};
''')

    def g_mapped_type_int_instances(self, sf, mapped_type):
        """ Generate the code to add a set of ints to a mapped type.  Return
        True if there was at least one.
        """

        return self._g_instances_int(sf, mapped_type)

    @staticmethod
    def g_method_error_handler_end(sf, overload):
        """ Generate the end of the error handling of a method. """

        sf.write('            }\n')

        if not is_zero_arg_slot(overload.common.py_slot):
            sf.write('\n            sipAddException(sipError, &sipParseErr);\n')

    @staticmethod
    def g_method_error_handler_start(sf, overload, error_value):
        """ Generate the start of the error handling of a method. """

        if not is_zero_arg_slot(overload.common.py_slot):
            sf.write(
f'''            if (sipError == sipErrorFail)
                return {error_value};

''')

        sf.write(
'''            if (sipError == sipErrorNone)
            {
''')

    def g_mixin_support(self, sf, klass):
        """ Generate the support for mixins. """

        if klass.mixin:
            spec = self.spec
            module_name = spec.module.py_name
            klass_name = klass.iface_file.fq_cpp_name.as_word

            sf.write('\n\n')

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static int mixin_{klass_name}(PyObject *, PyObject *, PyObject *);}}\n')

            sf.write(
f'''static int mixin_{klass_name}(PyObject *sipSelf, PyObject *sipArgs, PyObject *sipKwds)
{{
    return sipInitMixin(sipSelf, sipArgs, sipKwds, (sipClassTypeDef *)&sipTypeDef_{module_name}_{klass_name});
}}
''')

    def g_module_definition(self, sf, bindings, has_module_functions=False):
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

    def g_name_cache(self, sf):
        """ Generate the name cache definition and return the transformed name
        cache.
        """

        spec = self.spec
        module = spec.module

        # If there should be a Qt support API then generate stubs values for
        # the optional parts.  These should be undefined in %ModuleCode if a
        # C++ implementation is provided.
        if spec.target_abi < (13, 0) and self._legacy_qt_support():
            sf.write(
'''
#define sipQtCreateUniversalSignal          0
#define sipQtFindUniversalSignal            0
#define sipQtEmitSignal                     0
#define sipQtConnectPySignal                0
#define sipQtDisconnectPySignal             0
''')

        # Make sure the module name is cached.
        module.fq_py_name.used = True

        # Transform the name cache.
        name_cache_list = _name_cache_as_list(spec.name_cache)

        # Generate the names.
        has_sip_strings = False

        for name in name_cache_list:
            if not name.used or name.is_substring:
                continue

            if not has_sip_strings:
                has_sip_strings = True

                sf.write(
f'''
/* Define the strings used by this module. */
const char sipStrings_{module.py_name}[] = {{
''')

            sf.write('    ')

            for ch in name.name:
                sf.write(f"'{ch}', ")

            sf.write('0,\n')

        if has_sip_strings:
            sf.write('};\n')

        return name_cache_list

    @staticmethod
    def g_not_implemented(sf):
        """ Generate the code to clear any exception and return
        Py_NotImplemented.
        """

        _g_not_implemented(sf)

    def g_other_members(self, sf, bindings, scope, members):
        """ Generate other (backend-specific) members for a scope. """

        is_ns = isinstance(scope, WrappedClass) and scope.iface_file.type is IfaceFileType.NAMESPACE

        for member in members:
            if is_ns:
                g_static_function(self, sf, bindings, member, scope=scope)
            elif member.py_slot is not None:
                _g_py_slot(self, sf, bindings, member, scope=scope)

    def g_py_method_end(self, sf, state, nr_signatures):
        """ Generate the end of a method implementation. """

        klass, member, has_auto_docstring, need_args = state

        if not member.no_arg_parser:
            klass_name = klass.iface_file.fq_cpp_name.as_word
            member_py_name = member.py_name.name

            sip_parse_err = 'sipParseErr' if need_args else 'SIP_NULLPTR'
            klass_py_name_ref = _get_cached_name_ref(klass.py_name)
            member_py_name_ref = _get_cached_name_ref(member.py_name)
            docstring_ref = f'doc_{klass_name}_{member_py_name}' if has_auto_docstring else 'SIP_NULLPTR'

            sf.write(
f'''
    sipNoMethod({sip_parse_err}, {klass_py_name_ref}, {member_py_name_ref}, {docstring_ref});

    return SIP_NULLPTR;
''')

        sf.write('}\n')

    def g_py_method_start(self, sf, bindings, scope, member, original_scope,
            need_args, need_self):
        """ Generate the start of a method implementation and return a 4-tuple
        the class, member, whether it has an automatically generated docstring
        and whether an argument parser will be needed.
        """

        spec = self.spec
        scope_name = scope.iface_file.fq_cpp_name.as_word
        member_py_name = member.py_name.name

        # Generate the docstrings.
        if has_method_docstring(bindings, member, original_scope.overloads):
            sf.write(f'PyDoc_STRVAR(doc_{scope_name}_{member_py_name}, "')

            has_auto_docstring = _g_method_docstring(sf, spec, bindings,
                    member, original_scope.overloads,
                    is_method=not scope.is_hidden_namespace)

            sf.write('");\n\n')
        else:
            has_auto_docstring = False

        if member.no_arg_parser or member.allow_keyword_args:
            kw_fw_decl = ', PyObject *'
            kw_decl = ', PyObject *sipKwds'
        else:
            kw_fw_decl = kw_decl = ''

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *meth_{scope_name}_{member_py_name}({self.get_py_method_args(is_impl=False)}{kw_fw_decl});}}\n')

        sf.write(f'static PyObject *meth_{scope_name}_{member_py_name}({self.get_py_method_args(is_impl=True, need_self=need_self, need_args=need_args)}{kw_decl})\n{{\n')

        return scope, member, has_auto_docstring, need_args

    def g_py_method_support_vars(self, sf, need_args):
        """ Generate the variables needed by a method implementation. """

        if need_args:
            sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n');

    def g_sip_api(self, sf, module_name, module_state):
        """ Generate the SIP API as seen by generated code. """

        spec = self.spec

        # Generate the references to (potentially) shared strings.
        sf.write(
'''
/*
 * Convenient names to refer to various strings defined in this module.
 * Only the class names are part of the public API.
 */
''')

        for cached_name in module_state:
            if cached_name.used:
                sf.write(
f'''#define {_get_cached_name_ref(cached_name, as_nr=True)} {cached_name.offset}
#define {_get_cached_name_ref(cached_name)} &sipStrings_{module_name}[{cached_name.offset}]
''')

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

        # Generate the name strings.
        sf.write(
f'''
/* The strings used by this module. */
extern const char sipStrings_{module_name}[];
''')

    def g_static_function_end(self, sf, state, nr_signatures):
        """ Generate the end of a static function implementation. """

        if nr_signatures != 0:
            member, member_ref, has_auto_docstring = state

            sf.write(
f'''
    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoFunction(sipParseErr, {_get_cached_name_ref(member.py_name)}, ''')

            if has_auto_docstring:
                sf.write(f'doc_{member_ref}')
            else:
                sf.write('SIP_NULLPTR')

            sf.write(');\n\n    return SIP_NULLPTR;\n')

        sf.write('}\n')

    def g_static_function_start(self, sf, bindings, scope_py, member,
            overloads):
        """ Generate the start of a static function implementation and return
        a 3-tuple of the member, the member reference and whether it has an
        automatically generated docstring.
        """

        spec = self.spec
        member_name = member.py_name.name

        if scope_py is not None:
            member_name = scope_py.iface_file.fq_cpp_name.as_word + '_' + member_name

        # Generate the docstrings.
        if has_method_docstring(bindings, member, overloads):
            sf.write(f'PyDoc_STRVAR(doc_{member_name}, "')
            has_auto_docstring = _g_method_docstring(sf, spec, bindings,
                    member, overloads)
            sf.write('");\n\n')
        else:
            has_auto_docstring = False

        if member.no_arg_parser or member.allow_keyword_args:
            kw_fw_decl = ', PyObject *'
            kw_decl = ', PyObject *sipKwds'
        else:
            kw_fw_decl = kw_decl = ''

        if scope_py is None:
            if not spec.c_bindings:
                sf.write(f'extern "C" {{static PyObject *func_{member_name}({self.get_py_method_args(is_impl=False)}{kw_fw_decl});}}\n')

            sf.write(f'static PyObject *func_{member_name}({self.get_py_method_args(is_impl=True)}{kw_decl})\n')
        else:
            # This can only happen with C++ bindings.
            sf.write(f'extern "C" {{static PyObject *meth_{member_name}({self.get_py_method_args(is_impl=False)}{kw_fw_decl});}}\n')

            sf.write(f'static PyObject *meth_{member_name}({self.get_py_method_args(is_impl=True)}{kw_decl})\n')

        sf.write('{\n')

        return member, member_name, has_auto_docstring

    def g_slot_extender_impl(self, sf, bindings, member, klass=None):
        """ Generate the implementation of a slot extender. """

        _g_py_slot(self, sf, bindings, member, scope=klass)

    def g_slot_extenders_table(self, sf):
        """ Generate the table of slot implementations. """

        module = self.spec.module

        sf.write(
'''
static sipPySlotExtenderDef slotExtenders[] = {\n''')

        for member in module.global_functions:
            if member.py_slot is None:
                continue

            # These are any remaining global slots not move to a class or enum
            # by the resolver.  It isn't clear what these would be or how they
            # are used.
            for overload in module.overloads:
                if overload.common is member:
                    slot_ref = _get_slot_ref(member.py_slot)
                    sf.write(
f'    {{(void *)slot_{member.py_name}, {slot_ref}, {{0, 0, 0}}}},\n')
                    break

        for klass in module.extenders:
            for member in klass.members:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                slot_ref = _get_slot_ref(member.py_slot)
                encoded_type = get_encoded_type(module, klass)
                sf.write(f'    {{(void *)slot_{klass_name}_{member.py_name}, {slot_ref}, {encoded_type}}},\n')

        sf.write(
'''    {SIP_NULLPTR, (sipPySlotType)0, {0, 0, 0}}
};
''')

    def g_static_function_support_vars(self, sf, scope):
        """ Generate the variables needed by a static function implementation.
        """

        sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n')

        if scope is None and self.spec.c_bindings:
            sf.write(
'''
    (void)sipSelf;
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

    def g_subclass_convertor(self, sf, klass):
        """ Generate a sub-class convertor. """

        klass_name = klass.iface_file.fq_cpp_name.as_word
        base_cpp = klass.subclass_base.iface_file.fq_cpp_name.as_cpp

        if not self.spec.c_bindings:
            sf.write(
f'extern "C" {{static const sipTypeDef *sipSubClass_{klass_name}(void **);}}\n')

        # Allow the deprecated use of sipClass rather than sipType.
        if is_used_in_code(klass.convert_to_subclass_code, 'sipClass'):
            decl = 'sipWrapperType *sipClass'
            result = '(sipClass ? sipClass->wt_td : 0)'
        else:
            decl = 'const sipTypeDef *sipType'
            result = 'sipType'

        sf.write(
f'''static const sipTypeDef *sipSubClass_{klass_name}(void **sipCppRet)
{{
    {base_cpp} *sipCpp = reinterpret_cast<{base_cpp} *>(*sipCppRet);
    {decl};

''')

        sf.write_code(klass.convert_to_subclass_code)

        sf.write(
f'''
    return {result};
}}
''')

    def g_subclass_convertors_table(self, sf):
        """ Generate the table of sub-class convertors. """

        spec = self.spec
        module = spec.module

        sf.write(
'''

/* This defines the class sub-convertors that this module defines. */
static sipSubClassConvertorDef convertorsTable[] = {
''')

        for klass in module_classes(spec):
            if klass.convert_to_subclass_code is None:
                continue

            klass_name = klass.iface_file.fq_cpp_name.as_word
            encoded_type = get_encoded_type(module, klass.subclass_base)

            sf.write(f'    {{sipSubClass_{klass_name}, {encoded_type}, SIP_NULLPTR}},\n')

        sf.write(
'''    {SIP_NULLPTR, {0, 0, 0}, SIP_NULLPTR}
};
''')

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

            slot_ref = _get_slot_ref(member.py_slot)
            member_name = member.py_name
            sf.write(f'    {{(void *)slot_{klass_name}_{member_name}, {slot_ref}}},\n')

        if is_slots:
            sf.write('    {0, (sipPySlotType)0}\n};\n')

        # The attributes tables.
        nr_methods = self._g_class_method_table(sf, bindings, klass)
        nr_enum_members, _ = self.g_enums_specifications(sf, bindings,
                scope=klass)

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
            fields = ['PropertyVariable', _get_cached_name_ref(prop.name)]

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
                    fields.append(_get_cached_name_ref(variable.py_name))
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
        docstring_ref = _g_class_docstring(sf, spec, bindings, klass)

        # Generate any plugin-specific data structures.
        plugin_ref = 'SIP_NULLPTR'

        if pyqt5_supported(spec) or pyqt6_supported(spec):
            if _g_pyqt_class_plugin(self, sf, bindings, klass):
                plugin_ref = '&plugin_' + klass_name

        # The type definition structure itself.
        base_fields = []
        container_fields = []
        class_fields = []

        if spec.target_abi < (13, 0):
            base_fields.append('-1')
            base_fields.append('SIP_NULLPTR')

        base_fields.append('SIP_NULLPTR')
        base_fields.append(_get_class_flags(module, klass, py_debug))
        base_fields.append(_get_cached_name_ref(klass.iface_file.cpp_name,
                as_nr=True))
        base_fields.append('SIP_NULLPTR')
        base_fields.append(plugin_ref)

        container_fields.append(
                _get_cached_name_ref(klass.py_name, as_nr=True) if klass.real_class is None else '-1')

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
                _get_cached_name_ref(klass.metatype, as_nr=True) if klass.metatype is not None else '-1')
        class_fields.append(
                _get_cached_name_ref(klass.supertype, as_nr=True) if klass.supertype is not None else '-1')
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

    def g_types_table(self, sf, enums_state):
        """ Generate the types table for a module. """

        module = self.spec.module
        module_name = module.py_name

        sf.write(
f'''

/*
 * This defines each type in this module.
 */
sipTypeDef *sipExportedTypes_{module_name}[] = {{
''')

        for needed_type in module.needed_types:
            if needed_type.type is ArgumentType.CLASS:
                klass = needed_type.definition

                if klass.external:
                    sf.write('    0,\n')
                elif not klass.is_hidden_namespace:
                    sf.write(f'    &sipTypeDef_{module_name}_{klass.iface_file.fq_cpp_name.as_word}.ctd_base,\n')

            elif needed_type.type is ArgumentType.MAPPED:
                mapped_type = needed_type.definition

                sf.write(f'    &sipTypeDef_{module_name}_{mapped_type.iface_file.fq_cpp_name.as_word}.mtd_base,\n')

            elif needed_type.type is ArgumentType.ENUM:
                enum = needed_type.definition

                _, needed_enums = enums_state
                enum_nr = needed_enums.index(enum)

                sf.write(f'    &enumTypes[{enum_nr}].etd_base,\n')

        sf.write('};\n')

    def g_virt_error_handler_decls(self, sf):
        """ Generate the declarations of any locally defined virtual error
        handlers.
        """

        spec = self.spec
        module = spec.module

        for virtual_error_handler in spec.virtual_error_handlers:
            if virtual_error_handler.module is module:
                sf.write(f'\nvoid sipVEH_{module.py_name}_{virtual_error_handler.name}(sipSimpleWrapper *, sip_gilstate_t);\n')

    def g_virt_error_handler_impl(self, sf, virtual_error_handler):
        """ Generate the implementations of any locally defined virtual error
        handlers.
        """

        code = virtual_error_handler.code

        self_name = get_use_in_code(code, 'sipPySelf')
        state_name = get_use_in_code(code, 'sipGILState')

        sf.write(
f'''

void sipVEH_{self.spec.module.py_name}_{virtual_error_handler.name}(sipSimpleWrapper *{self_name}, sip_gilstate_t {state_name})
{{
''')

        sf.write_code(code)

        sf.write('}\n')

    @staticmethod
    def g_wrapper_ref_decl(sf):
        """ Generate the code that declares a wrapper reference. """

        sf.write('    sipSimpleWrapper *sipPySelf;\n')

    @staticmethod
    def g_wrapper_ref_set(sf):
        """ Generate the code that sets the value of a wrapper reference. """

        sf.write('            sipCpp->sipPySelf = sipSelf;\n\n')

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

        return _get_cached_name_ref(cached_name, as_nr=as_nr)

    def custom_enums_supported(self):
        """ Return True if custom enums are supported. """

        return self.spec.target_abi[0] < 13

    @staticmethod
    def get_add_exception_call(error_state):
        """ Return a call to sipAddException(). """

        return f'sipAddException({error_state}, sipParseErr)'

    def get_enum_to_py_conversion(self, enum, value_name):
        """ Return the code to convert a C/C++ enum to a Python object. """

        if enum.fq_cpp_name is None:
            return  f'PyLong_FromLong({value_name})'

        if not self.spec.c_bindings:
            value_name = f'static_cast<int>({value_name})'

        return f'sipConvertFromEnum({value_name}, {_get_type_ref(enum)})'

    def get_enum_ref_value(self, enum):
        """ Return the value of an enum's reference. """

        spec = self.spec

        if enum.module is spec.module:
            return f'sipExportedTypes_{spec.module.py_name}[{enum.type_nr}]'

        return f'sipImportedTypes_{spec.module.py_name}_{enum.module.py_name}[{enum.type_nr}].it_td'

    def get_error_handler_ref(self, error_handler):
        """ Return a reference to an error handler. """

        if error_handler is None:
            return '0'

        module = self.spec.module
        module_name = module.py_name

        if error_handler.module is module:
            return f'sipVEH_{module_name}_{error_handler.name}'

        return f'sipImportedVirtErrorHandlers_{module_name}_{error_handler.module.py_name}[{error_handler.handler_nr}].iveh_handler'

    @staticmethod
    def get_error_handler_ref_type():
        """ Return the type of a reference to an error handler. """

        return 'sipVirtErrorHandlerFunc'

    def get_py_method_args(self, *, is_impl, need_self=False, need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        self_name = 'sipSelf' if is_impl and (self.spec.c_bindings or need_self) else ''
        args_name = 'sipArgs' if is_impl and need_args else ''

        return f'PyObject *{self_name}, PyObject *{args_name}'

    @staticmethod
    def get_raise_unknown_exception():
        """ Return the call to raise an exception about an unknown exception.
        """

        return 'sipRaiseUnknownException()'

    @staticmethod
    def get_result_parser():
        """ Return the name of the Python reimplementation result parser. """

        return 'sipParseResultEx'

    def get_sipself_test(self, klass):
        """ Return the code that checks if 'sipSelf' was bound or passed as an
        argument.
        """

        if self.spec.target_abi >= (13, 0):
            sipself_test = f'!PyObject_TypeCheck(sipSelf, sipTypeAsPyTypeObject({_get_type_ref(klass)}))'
        else:
            sipself_test = '!sipSelf'

        return f'({sipself_test} || sipIsDerivedClass((sipSimpleWrapper *)sipSelf))'

    @staticmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        return _get_slot_ref(slot_type)

    @staticmethod
    def get_spec_suffix():
        """ Return the suffix used for immutable specifications. """

        return 'Def'

    @staticmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        return _get_type_ref(wrapped_object)

    @staticmethod
    def get_wrapper_ref_init():
        """ Return the initialisation of a wrapper reference. """

        return 'sipPySelf(SIP_NULLPTR)'

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

    def _g_class_method_table(self, sf, bindings, klass):
        """ Generate the sorted table of methods for a class and return the
        number of entries.
        """

        if klass.iface_file.type is IfaceFileType.NAMESPACE:
            members = get_function_table(klass.members)
        else:
            members = get_method_table(klass, ignore_slots=True)

        return self._g_py_method_table(sf, bindings, members, klass)

    def _g_enums_defs(self, sf, needed_enums):
        """ Generate the definitions for all wrapped enums. """

        spec = self.spec
        module = spec.module

        # Note that we go through the sorted table of needed types rather than
        # the unsorted list of all enums.
        for needed_type in module.needed_types:
            if needed_type.type is not ArgumentType.ENUM:
                continue

            enum = needed_type.definition

            scope_type_nr = -1 if enum.scope is None else enum.scope.iface_file.type_nr

            if len(needed_enums) == 0:
                sf.write('static sipEnumTypeDef enumTypes[] = {\n')

            cpp_name = get_normalised_cached_name(enum.cached_fq_cpp_name)
            py_name = get_normalised_cached_name(enum.py_name)

            if self.py_enums_supported():
                base_type = 'SIP_ENUM_' + enum.base_type.name
                nr_members = len(enum.members)

                sf.write(
f'    {{{{SIP_NULLPTR, SIP_TYPE_ENUM, sipNameNr_{cpp_name}, SIP_NULLPTR, 0}}, {base_type}, sipNameNr_{py_name}, {scope_type_nr}, {nr_members}')
            else:
                sip_type = 'SIP_TYPE_SCOPED_ENUM' if enum.is_scoped else 'SIP_TYPE_ENUM'

                v12_fields = '-1, SIP_NULLPTR, ' if spec.target_abi < (13, 0)  else ''

                sf.write(
f'    {{{{{v12_fields}SIP_NULLPTR, {sip_type}, sipNameNr_{cpp_name}, SIP_NULLPTR, 0}}, sipNameNr_{py_name}, {scope_type_nr}')

            if len(enum.slots) == 0:
                sf.write(', SIP_NULLPTR')
            else:
                sf.write(', slots_' + enum.fq_cpp_name.as_word)

            sf.write('},\n')

            needed_enums.append(enum)

        if len(needed_enums) != 0:
            sf.write('};\n')

    def _g_instances_char(self, sf, scope):
        """ Generate the code to add a set of characters to a dictionary.
        Return True if there was at least one.
        """

        instances = []

        for variable in variables_in_scope(self.spec, scope):
            if variable.type.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING) or len(variable.type.derefs) != 0:
                continue

            ci_name = _get_cached_name_ref(variable.py_name)
            ci_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            ci_encoding = _get_encoding(variable.type)

            if variable.type.type is ArgumentType.USTRING:
                ci_val = '(char)' + ci_val;

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

            ti_name = _get_cached_name_ref(variable.py_name)
            ti_ptr = '&' + scoped_variable_name(spec, variable)
            ti_type = '&' + _get_type_ref(variable.type.definition)
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

            di_name = _get_cached_name_ref(variable.py_name)
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
                    ii_name = _get_cached_name_ref(enum_member.py_name)
                    ii_val = self._get_cast_enum_member(enum_member)
                    instances.append((ii_name, ii_val))

        # Handle int variables.
        for variable in variables_in_scope(spec, scope):
            if variable.type.type not in (ArgumentType.ENUM, ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.UBYTE, ArgumentType.USHORT, ArgumentType.SHORT, ArgumentType.CINT, ArgumentType.INT, ArgumentType.BOOL, ArgumentType.CBOOL):
                continue

            # Named enums are handled elsewhere.
            if variable.type.type is ArgumentType.ENUM and variable.type.definition.fq_cpp_name is not None:
                continue

            ii_name = _get_cached_name_ref(variable.py_name)
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
                    ii_name = _get_cached_name_ref(enum_member.py_name)
                    ii_val = self._get_cast_enum_member(enum_member)
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

            si_name = _get_cached_name_ref(variable.py_name)
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

            vi_name = _get_cached_name_ref(variable.py_name)
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

        _g_pyqt_helper_init(sf, spec)

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

    def _g_module_functions_table(self, sf, bindings, module):
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

    def _g_py_method_table(self, sf, bindings, members, scope):
        """ Generate a Python method table for a class or mapped type and
        return the number of entries.
        """

        scope_name = scope.iface_file.fq_cpp_name.as_word

        no_intro = True

        for member_nr, member in enumerate(members):
            # Save the index in the table.
            member.member_nr = member_nr

            py_name = member.py_name
            cached_py_name = _get_cached_name_ref(py_name)
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

            py_name = _get_cached_name_ref(variable.py_name)
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
                dict_name = f'(PyObject *)sipTypeAsPyTypeObject({_get_type_ref(variable.scope)})'

            py_name = _get_cached_name_ref(variable.py_name)
            ptr = '&' + scoped_variable_name(spec, variable)

            if variable.type.is_const:
                type_name = fmt_argument_as_cpp_type(spec, variable.type,
                        plain=True, no_derefs=True)
                ptr = f'const_cast<{type_name} *>({ptr})'

            sf.write(f'    sipAddTypeInstance({dict_name}, {py_name}, {ptr}, {_get_type_ref(variable.type.definition)});\n')

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
        # only if the latter is non-static).  However this can result in some
        # surprising behaviour which is best illustrated by an example (see the
        # 'tests/class_attrs/test_class_attrs.py::test_value_attribute' test).
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

            sf.write(f'    {prefix_s} sipConvertFrom{new_s}Type({sip_val_s}, {_get_type_ref(variable.type.definition)}, SIP_NULLPTR);\n')

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
                sf.write(f'    return sipConvertFromEnum({sip_val_s}, {_get_type_ref(variable.type.definition)});\n')

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

        last_arg = 'sipPySelf' if spec.c_bindings or variable.is_static or keep else ''

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
    sipReleaseType{suffix}(sipVal, {_get_type_ref(variable.type.definition)}, sipValState''')

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

    def _get_cast_enum_member(self, enum_member):
        """ Return the appropriately cast enum member. """

        return f'static_cast<int>({get_enum_member(self.spec, enum_member)})'

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

            statement += f'sipForceConvertToType{suffix}(sipPy, {_get_type_ref(variable.type.definition)}, SIP_NULLPTR, {flags}, {state_ptr}'

            if type_needs_user_state(variable.type):
                statement += ', &sipValUserState'

            statement += ', &sipIsErr)' + cast_tail

        elif variable_type is ArgumentType.ENUM:
            statement = f'({type_s})sipConvertToEnum(sipPy, {_get_type_ref(variable.type.definition)})'

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

    def _legacy_qt_support(self):
        """ Return True if the module implements legacy Qt support. """

        spec = self.spec

        return spec.pyqt_qobject is not None and spec.pyqt_qobject.iface_file.module is spec.module

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

            ii_name = _get_cached_name_ref(variable.py_name)
            ii_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
            instances.append((ii_name, ii_val))

        table_type_name = type_name.title().replace(' ', '')
        table_name = table_type_name[0].lower() + table_type_name[1:]

        declaration_template = f'''/* Define the {type_name}s to be added to this {{dict_type}} dictionary. */
static sip{table_type_name}InstanceDef {table_name}Instances{{suffix}}[]'''

        return _write_instances_table(sf, scope, instances,
                declaration_template)


def _g_arg_parser(backend, sf, scope, py_signature, signature_nr, ctor,
        is_method, overload):
    """ Generate an argument parser call. """

    spec = backend.spec

    # If the scope is a mapped type or a namespace, then ignore it.
    if isinstance(scope, MappedType) or (isinstance(scope, WrappedClass) and scope.iface_file.type is IfaceFileType.NAMESPACE):
        scope = None

    # See if the parser handles self.
    if scope is None or overload is None or overload.common.py_slot is not None:
        handle_self = False
    else:
        # In ABI v13 static methods use self for the type object.
        handle_self = True if spec.target_abi >= (13, 0) else not overload.is_static

    # Generate the local variables that will hold the parsed arguments and
    # values returned via arguments.
    array_len_arg_nr = -1
    need_owner = False
    ctor_needs_self = False

    for arg_nr, arg in enumerate(py_signature.args):
        if arg.array is ArrayArgument.ARRAY_SIZE:
            array_len_arg_nr = arg_nr

        g_argument_variable(backend, sf, scope, arg, arg_nr)

        if arg.transfer is Transfer.TRANSFER_THIS:
            need_owner = True

        if ctor is not None and arg.transfer is Transfer.TRANSFER:
            ctor_needs_self = True

    if overload is not None and need_owner:
        sf.write('        sipWrapper *sipOwner = SIP_NULLPTR;\n')

    if handle_self and not overload.is_static:
        cpp_type = 'const ' if overload.is_const else ''

        if isinstance(scope, WrappedEnum):
            ptr = ''
            cpp_type = fmt_enum_as_cpp_type(scope)
        else:
            ptr = '*'
            if overload.access_specifier is AccessSpecifier.PROTECTED and scope.has_shadow:
                cpp_type += 'sip' + scope.iface_file.fq_cpp_name.as_word
            else:
                cpp_type += scoped_class_name(spec, scope)

        sf.write(f'        {cpp_type} {ptr}sipCpp;\n\n')
    elif len(py_signature.args) != 0:
        sf.write('\n')

    # Generate the call to the parser function.
    parser_function, args, single_arg = _arg_parser_arguments(backend, sf,
            scope, ctor, overload, py_signature, signature_nr)

    # Generate the format string.
    format_s = '"'
    optional_args = False

    if single_arg:
        format_s += '1'

    if ctor_needs_self:
        format_s += '#'
    elif handle_self:
        if overload.is_static:
            format_s += 'C'
        elif overload.access_is_really_protected:
            format_s += 'p'
        else:
            format_s += 'B'

    for arg in py_signature.args:
        if not arg.is_in:
            continue

        if arg.default_value is not None and not optional_args:
            format_s += '|'
            optional_args = True

        # Get the wrapper if explicitly asked for or we are going to keep a
        # reference to.  However if it is an encoded string then we will get
        # the actual wrapper from the format character.
        if arg.get_wrapper:
            format_s += '@'
        elif arg.key is not None:
            if not (arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) and len(arg.derefs) == 1):
                format_s += '@'

        if arg.type is ArgumentType.ASCII_STRING:
            format_s += 'AA' if is_string(arg) else 'aA'

        elif arg.type is ArgumentType.LATIN1_STRING:
            format_s += 'AL' if is_string(arg) else 'aL'

        elif arg.type is ArgumentType.UTF8_STRING:
            format_s += 'A8' if is_string(arg) else 'a8'

        elif arg.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
            if arg.array is ArrayArgument.ARRAY:
                format_s += 'k'
            elif is_string(arg):
                format_s += 's'
            else:
                format_s += 'c'

        elif arg.type is ArgumentType.WSTRING:
            if arg.array is ArrayArgument.ARRAY:
                format_s += 'K'
            elif is_string(arg):
                format_s += 'x'
            else:
                format_s += 'w'

        elif arg.type is ArgumentType.ENUM:
            if arg.definition.fq_cpp_name is None:
                format_s += 'e'
            elif arg.is_constrained:
                format_s += 'XE'
            else:
                format_s += 'E'

        elif arg.type is ArgumentType.BOOL:
            format_s += 'b'

        elif arg.type is ArgumentType.CBOOL:
            format_s += 'Xb'

        elif arg.type is ArgumentType.INT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'i'

        elif arg.type is ArgumentType.UINT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'u'

        elif arg.type is ArgumentType.SIZE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += '='

        elif arg.type is ArgumentType.CINT:
            format_s += 'Xi'

        elif arg.type is ArgumentType.BYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'I' if backend.abi_has_working_char_conversion() else 'L'

        elif arg.type is ArgumentType.SBYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'L'

        elif arg.type is ArgumentType.UBYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'M'

        elif arg.type is ArgumentType.SHORT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'h'

        elif arg.type is ArgumentType.USHORT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 't'

        elif arg.type is ArgumentType.LONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'l'

        elif arg.type is ArgumentType.ULONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'm'

        elif arg.type is ArgumentType.LONGLONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'n'

        elif arg.type is ArgumentType.ULONGLONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'o'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            format_s += 'v'

        elif arg.type is ArgumentType.CAPSULE:
            format_s += 'z'

        elif arg.type is ArgumentType.FLOAT:
            format_s += 'f'

        elif arg.type is ArgumentType.CFLOAT:
            format_s += 'Xf'

        elif arg.type is ArgumentType.DOUBLE:
            format_s += 'd'

        elif arg.type is ArgumentType.CDOUBLE:
            format_s += 'Xd'

        elif arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if arg.array is ArrayArgument.ARRAY:
                format_s += '>' if arg.type is ArgumentType.CLASS and backend.abi_supports_array() else 'r'
            else:
                format_s += 'J' + _get_subformat_char(arg)

        elif arg.type is ArgumentType.PYOBJECT:
            format_s += 'P' + _get_subformat_char(arg)

        elif arg.type in (ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYSLICE, ArgumentType.PYTYPE):
            format_s += 'N' if arg.allow_none else 'T'

        elif arg.type is ArgumentType.PYCALLABLE:
            format_s += 'H' if arg.allow_none else 'F'

        elif arg.type is ArgumentType.PYBUFFER:
            format_s += '$' if arg.allow_none else '!'

        elif arg.type is ArgumentType.PYENUM:
            format_s += '^' if arg.allow_none else '&'

        elif arg.type is ArgumentType.ELLIPSIS:
            format_s += 'W'

    format_s += '"'
    args.append(format_s)

    # Generate the parameters corresponding to the format string.
    if ctor_needs_self:
        args.append('sipSelf')
    elif handle_self:
        args.append('&sipSelf')

        if not overload.is_static:
            args.append(_get_type_ref(scope))
            args.append('&sipCpp')

    for arg_nr, arg in enumerate(py_signature.args):
        if not arg.is_in:
            continue

        arg_name = fmt_argument_as_name(spec, arg, arg_nr)
        arg_name_ref = '&' + arg_name

        # Use the wrapper name if it was explicitly asked for.
        if arg.get_wrapper:
            args.append(f'&{arg_name}Wrapper')
        elif arg.key is not None:
            args.append(f'&{arg_name}Keep')

        if arg.type is ArgumentType.MAPPED:
            mapped_type = arg.definition

            args.append(_get_type_ref(mapped_type))
            args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)
            elif mapped_type.convert_to_type_code is not None and not arg.is_constrained:
                args.append('SIP_NULLPTR' if mapped_type.no_release else f'&{arg_name}State')

                if mapped_type.needs_user_state:
                    args.append(f'&{arg_name}UserState')

        elif arg.type is ArgumentType.CLASS:
            klass = arg.definition

            args.append(_get_type_ref(klass))
            args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)

                if backend.abi_supports_array():
                    args.append(f'&{arg_name}IsTemp')
            else:
                if arg.transfer is Transfer.TRANSFER_THIS:
                    args.append('sipOwner' if ctor is not None else '&sipOwner')

                if klass.convert_to_type_code is not None and not arg.is_constrained:
                    args.append(f'&{arg_name}State')

        elif arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
            if arg.key is None and len(arg.derefs) == 1:
                args.append(f'&{arg_name}Keep')

            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYTUPLE:
            args.append('&PyTuple_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYLIST:
            args.append('&PyList_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYDICT:
            args.append('&PyDict_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYSLICE:
            args.append('&PySlice_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYTYPE:
            args.append('&PyType_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.ENUM:
            if arg.definition.fq_cpp_name is not None:
                args.append(_get_type_ref(arg.definition))

            args.append(arg_name_ref)

        elif arg.type is ArgumentType.CAPSULE:
            args.append('"' + arg.definition + '"')
            args.append(arg_name_ref)

        else:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)

    args = ', '.join(args)

    sf.write(f'        if ({parser_function}({args}))\n')


def _arg_parser_arguments(backend, sf, scope, ctor, overload, py_signature,
        signature_nr):
    """ Generate any code required before an argument parser is invoked and
    return a 3-tuple of the name of the parser function, the parser arguments
    (prior to the format string) and a flag which is set if the signature is
    known to require a single argument.
    """

    args = []
    single_arg = False

    if overload is not None and is_number_slot(overload.common.py_slot):
        parser_function = 'sipParsePair'
        args.append('&sipParseErr')
        args.append('sipArg0')
        args.append('sipArg1')

    elif overload is not None and overload.common.py_slot is PySlot.SETATTR:
        # We don't even try to invoke the parser if there is a value and there
        # shouldn't be (or vice versa) so that the list of errors doesn't get
        # polluted with signatures that can never apply.
        if overload.is_delattr:
            operator = '=='
            sip_value = 'SIP_NULLPTR'
        else:
            operator = '!='
            sip_value = 'sipValue'

        parser_function = f'sipValue {operator} SIP_NULLPTR && sipParsePair'
        args.append('&sipParseErr')
        args.append('sipName')
        args.append(sip_value)

    elif (overload is not None and overload.common.allow_keyword_args) or ctor is not None:
        kwd_list = _g_keyword_list(sf, ctor, overload, py_signature)

        parser_function = 'sipParseKwdArgs'
        args.append('sipParseErr' if ctor is not None else '&sipParseErr')
        args.append('sipArgs')
        args.append('sipKwds')
        args.append(kwd_list)
        args.append('sipUnused' if ctor is not None else 'SIP_NULLPTR')

    else:
        single_arg = not (overload is None or overload.common.py_slot is None or is_multi_arg_slot(overload.common.py_slot))

        parser_function = 'sipParseArgs'
        args.append('&sipParseErr')
        args.append('sipArg' + ('' if single_arg else 's'))

    return parser_function, args, single_arg


def _g_class_docstring(sf, spec, bindings, klass):
    """ Generate any docstring for a class and return an appropriate reference
    to it.
    """

    if _has_class_docstring(bindings, klass):
        docstring_ref = 'doc_' + klass.iface_file.fq_cpp_name.as_word

        sf.write(f'\nPyDoc_STRVAR({docstring_ref}, "')
        _g_class_docstring_text(sf, spec, bindings, klass)
        sf.write('");\n')
    else:
        docstring_ref = 'SIP_NULLPTR'

    return docstring_ref


def _g_class_docstring_text(sf, spec, bindings, klass):
    """ Generate the docstring text for a class. """

    NEWLINE = '\\n"\n"'

    # See if all the docstrings are automatically generated.
    all_auto = (klass.docstring is None)
    any_implied = False

    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if ctor.docstring is not None:
            all_auto = False

            if ctor.docstring.signature is not DocstringSignature.DISCARDED:
                any_implied = True

    # Generate the docstring.
    if all_auto:
        sf.write('\\1')

    if klass.docstring is not None and klass.docstring.signature is not DocstringSignature.PREPENDED:
        sf.write(get_docstring_text(klass.docstring))
        is_first = False
    else:
        is_first = True

    if klass.docstring is None or klass.docstring.signature is not DocstringSignature.DISCARDED:
        for ctor in klass.ctors:
            if ctor.access_specifier is AccessSpecifier.PRIVATE:
                continue

            if not is_first:
                sf.write(NEWLINE)

                # Insert a blank line if any explicit docstring wants to
                # include a signature.  This maintains compatibility with
                # previous versions.
                if any_implied:
                    sf.write(NEWLINE)

            if ctor.docstring is not None:
                if ctor.docstring.signature is DocstringSignature.PREPENDED:
                    _g_ctor_auto_docstring(sf, spec, bindings, klass, ctor)
                    sf.write(NEWLINE)

                sf.write(get_docstring_text(ctor.docstring))

                if ctor.docstring.signature is DocstringSignature.APPENDED:
                    sf.write(NEWLINE)
                    _g_ctor_auto_docstring(sf, spec, bindings, klass, ctor)
            elif all_auto or any_implied:
                _g_ctor_auto_docstring(sf, spec, bindings, klass, ctor)

            is_first = False

    if klass.docstring is not None and klass.docstring.signature is DocstringSignature.PREPENDED:
        if not is_first:
            sf.write(NEWLINE)
            sf.write(NEWLINE)

        sf.write(get_docstring_text(klass.docstring))


def _g_conversion_to_enum(sf, enum):
    """ Generate the code to convert a Python enum (sipSelf) to a C/C++ enum
    (sipCpp).
    """

    type_ref = _get_type_ref(enum)
    cpp_name = enum.fq_cpp_name.as_cpp

    sf.write(
f'''    {cpp_name} sipCpp = static_cast<{cpp_name}>(sipConvertToEnum(sipSelf, {type_ref}));

    if (PyErr_Occurred())
''')


def _g_ctor_auto_docstring(sf, spec, bindings, klass, ctor):
    """ Generate the automatic docstring for a ctor. """

    if bindings.docstrings:
        g_ctor_type_hint(sf, spec, bindings, klass, ctor)


def _g_keyword_list(sf, ctor, overload, py_signature):
    """ Generate the list of keywords for a signature.  Return an appropriate
    reference to the list.
    """

    # We handle keywords if we might have been passed some (because one of the
    # overloads uses them or we are a ctor).  However this particular signature
    # might not have any.
    if overload is not None:
        kw_args = overload.kw_args
    elif ctor is not None:
        kw_args = ctor.kw_args
    else:
        kw_args = KwArgs.NONE

    # The above test isn't good enough because when the flags were set in the
    # parser we couldn't know for sure if an argument was an output pointer.
    # Therefore we check here.  The drawback is that we may generate the name
    # string for the argument but never use it, or we might have an empty
    # keyword name array or one that contains only NULLs.
    is_ka_list = False

    if kw_args is not KwArgs.NONE:
        for arg in py_signature.args:
            if not arg.is_in:
                continue

            if not is_ka_list:
                sf.write('        static const char *sipKwdList[] = {\n')
                is_ka_list = True

            if arg.name is not None and (kw_args is KwArgs.ALL or arg.default_value is not None):
                arg_name_ref = _get_cached_name_ref(arg.name)
            else:
                arg_name_ref = 'SIP_NULLPTR'

            sf.write(f'            {arg_name_ref},\n')

        if is_ka_list:
            sf.write('        };\n\n')

    return 'sipKwdList' if is_ka_list else 'SIP_NULLPTR'


def _g_method_auto_docstring(sf, spec, bindings, overload, is_method):
    """ Generate the automatic docstring for a function/method. """

    if bindings.docstrings:
        g_overload_type_hint(sf, spec, overload, is_method=is_method)


def _g_method_docstring(sf, spec, bindings, member, overloads, is_method=False):
    """ Generate the docstring for all overloads of a function/method.  Return
    True if the docstring was entirely automatically generated.
    """

    NEWLINE = '\\n"\n"'

    auto_docstring = True

    # See if all the docstrings are automatically generated.
    all_auto = True
    any_implied = False

    for overload in callable_overloads(member, overloads):
        if overload.docstring is not None:
            all_auto = False

            if overload.docstring.signature is not DocstringSignature.DISCARDED:
                any_implied = True

    # Generate the docstring.
    is_first = True

    for overload in callable_overloads(member, overloads):
        if not is_first:
            sf.write(NEWLINE)

            # Insert a blank line if any explicit docstring wants to include a
            # signature.  This maintains compatibility with previous versions.
            if any_implied:
                sf.write(NEWLINE)

        if overload.docstring is not None:
            if overload.docstring.signature is DocstringSignature.PREPENDED:
                _g_method_auto_docstring(sf, spec, bindings, overload,
                        is_method)
                sf.write(NEWLINE)

            sf.write(get_docstring_text(overload.docstring))

            if overload.docstring.signature is DocstringSignature.APPENDED:
                sf.write(NEWLINE)
                _g_method_auto_docstring(sf, spec, bindings, overload,
                        is_method)

            auto_docstring = False
        elif all_auto or any_implied:
            _g_method_auto_docstring(sf, spec, bindings, overload, is_method)

        is_first = False

    return auto_docstring


def _g_not_implemented(sf):
    """ Generate the code to clear any exception and return Py_NotImplemented.
    """

    sf.write(
'''
    PyErr_Clear();

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
''')


def _g_py_slot(backend, sf, bindings, member, scope=None):
    """ Generate a Python slot handler for either a class, an enum or an
    extender.
    """

    spec = backend.spec

    if scope is None:
        prefix = ''
        py_name = None
        fq_cpp_name = None
        overloads = spec.module.overloads
    elif isinstance(scope, WrappedEnum):
        prefix = 'Type'
        py_name = scope.py_name
        fq_cpp_name = scope.fq_cpp_name
        overloads = scope.overloads
    else:
        prefix = 'Type'
        py_name = scope.py_name
        fq_cpp_name = scope.iface_file.fq_cpp_name
        overloads = scope.overloads

    if is_void_return_slot(member.py_slot) or is_int_return_slot(member.py_slot):
        ret_type = 'int '
        ret_value = '-1'
    elif is_ssize_return_slot(member.py_slot):
        ret_type = 'Py_ssize_t '
        ret_value = '0'
    elif is_hash_return_slot(member.py_slot):
        if spec.target_abi >= (13, 0):
            ret_type = 'Py_hash_t '
            ret_value = '0'
        else:
            ret_type = 'long '
            ret_value = '0L'
    else:
        ret_type = 'PyObject *'
        ret_value = 'SIP_NULLPTR'

    has_args = True

    if member.py_slot is PySlot.CALL:
        if spec.c_bindings or member.allow_keyword_args or member.no_arg_parser:
            arg_str = 'PyObject *sipSelf, PyObject *sipArgs, PyObject *sipKwds'
        else:
            arg_str = 'PyObject *sipSelf, PyObject *sipArgs, PyObject *'

        decl_arg_str = 'PyObject *, PyObject *, PyObject *'
    elif member.py_slot is PySlot.SETATTR:
        arg_str = 'PyObject *sipSelf, PyObject *sipName, PyObject *sipValue'
        decl_arg_str = 'PyObject *, PyObject *, PyObject *'
    elif is_int_arg_slot(member.py_slot):
        has_args = False
        arg_str = 'PyObject *sipSelf, int a0'
        decl_arg_str = 'PyObject *, int'
    elif is_multi_arg_slot(member.py_slot):
        arg_str = 'PyObject *sipSelf, PyObject *sipArgs'
        decl_arg_str = 'PyObject *, PyObject *'
    elif is_zero_arg_slot(member.py_slot):
        has_args = False
        arg_str = 'PyObject *sipSelf'
        decl_arg_str = 'PyObject *'
    elif is_number_slot(member.py_slot):
        arg_str = 'PyObject *sipArg0, PyObject *sipArg1'
        decl_arg_str = 'PyObject *, PyObject *'
    else:
        arg_str = 'PyObject *sipSelf, PyObject *sipArg'
        decl_arg_str = 'PyObject *, PyObject *'

    sf.write('\n\n')

    slot_decl = f'static {ret_type}slot_'

    if fq_cpp_name is not None:
        slot_decl += fq_cpp_name.as_word + '_'

    if not spec.c_bindings:
        sf.write(f'extern "C" {{{slot_decl}{member.py_name.name}({decl_arg_str});}}\n')

    sf.write(f'{slot_decl}{member.py_name.name}({arg_str})\n{{\n')

    if member.py_slot is PySlot.CALL and member.no_arg_parser:
        for overload in overloads:
            if overload.common is member:
                sf.write_code(overload.method_code)
    else:
        if is_inplace_number_slot(member.py_slot):
            sf.write(
f'''    if (!PyObject_TypeCheck(sipSelf, sipTypeAsPyTypeObject(sip{prefix}_{fq_cpp_name.as_word})))
    {{
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }}

''')

        if not is_number_slot(member.py_slot):
            if isinstance(scope, WrappedClass):
                cpp_name = scoped_class_name(spec, scope)
                type_ref = _get_type_ref(scope)

                sf.write(
f'''    {cpp_name} *sipCpp = reinterpret_cast<{cpp_name} *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf, {type_ref}));

    if (!sipCpp)
''')
            else:
                _g_conversion_to_enum(sf, scope)

            sf.write(f'        return {ret_value};\n\n')

        if has_args:
            sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n')

        for overload in overloads:
            if overload.common is member and overload.is_abstract:
                sf.write('    PyObject *sipOrigSelf = sipSelf;\n')
                break

        scope_not_enum = not isinstance(scope, WrappedEnum)
        signature_nr = 0

        for overload in overloads:
            if overload.common is member:
                dereferenced = scope_not_enum and not overload.dont_deref_self

                g_function_body(backend, sf, bindings, scope, overload,
                        signature_nr, dereferenced=dereferenced)
                signature_nr += 1

        if has_args:
            if member.py_slot in (PySlot.CONCAT, PySlot.ICONCAT, PySlot.REPEAT, PySlot.IREPEAT):
                slot_ref = _get_slot_ref(member.py_slot)
                sf.write(
f'''
    /* Raise an exception if the argument couldn't be parsed. */
    sipBadOperatorArg(sipSelf, sipArg, {slot_ref});

    return SIP_NULLPTR;
''')

            else:
                if is_rich_compare_slot(member.py_slot):
                    sf.write(
'''
    Py_XDECREF(sipParseErr);
''')
                elif is_number_slot(member.py_slot) or is_inplace_number_slot(member.py_slot):
                    sf.write(
'''
    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return SIP_NULLPTR;
''')

                if is_number_slot(member.py_slot) or is_rich_compare_slot(member.py_slot):
                    # We can only extend class slots. */
                    if isinstance(scope, WrappedClass):
                        extend_context = f'&sipModuleAPI_{spec.module.py_name}'
                        slot_ref = _get_slot_ref(member.py_slot)

                        if is_number_slot(member.py_slot):
                            sf.write(
f'''
    return sipPySlotExtend({extend_context}, {slot_ref}, SIP_NULLPTR, sipArg0, sipArg1);
''')
                        else:
                            sf.write(
f'''
    return sipPySlotExtend({extend_context}, {slot_ref}, {_get_type_ref(scope)}, sipSelf, sipArg);
''')
                    else:
                        _g_not_implemented(sf)
                elif is_inplace_number_slot(member.py_slot):
                    _g_not_implemented(sf)
                else:
                    member_name = '(sipValue != SIP_NULLPTR ? sipName___setattr__ : sipName___delattr__)' if member.py_slot is PySlot.SETATTR else _get_cached_name_ref(member.py_name)

                    sf.write(
f'''
    sipNoMethod(sipParseErr, {_get_cached_name_ref(py_name)}, {member_name}, SIP_NULLPTR);

    return {ret_value};
''')
        else:
            sf.write(
'''
    return 0;
''')

    sf.write('}\n')


def _g_pyqt_class_plugin(backend, sf, bindings, klass):
    """ Generate any extended class definition data for PyQt.  Return True if
    anything was generated.
    """

    spec = backend.spec

    is_signals = _g_pyqt_signals_table(backend, sf, bindings, klass)

    # The PyQt6 support code doesn't assume the structure is generated.
    if pyqt6_supported(spec):
        generated = is_signals

        if klass.is_qobject and not klass.pyqt_no_qmetaobject:
            generated = True

        if klass.pyqt_interface is not None:
            generated = True

        if not generated:
            return False

    klass_name = klass.iface_file.fq_cpp_name.as_word

    pyqt_version = '5' if pyqt5_supported(spec) else '6'
    sf.write(f'\n\nstatic pyqt{pyqt_version}ClassPluginDef plugin_{klass_name} = {{\n')

    mo_ref = f'&{scoped_class_name(spec, klass)}::staticMetaObject' if klass.is_qobject and not klass.pyqt_no_qmetaobject else 'SIP_NULLPTR'
    sf.write(f'    {mo_ref},\n')

    if pyqt5_supported(spec):
        sf.write(f'    {klass.pyqt_flags},\n')

    signals_ref = f'signals_{klass_name}' if is_signals else 'SIP_NULLPTR'
    sf.write(f'    {signals_ref},\n')

    interface_ref = f'"{klass.pyqt_interface}"' if klass.pyqt_interface is not None else 'SIP_NULLPTR'
    sf.write(f'    {interface_ref}\n')

    sf.write('};\n')

    return True


def _g_pyqt_emitters(backend, sf, klass):
    """ Generate the PyQt emitters for a class. """

    spec = backend.spec
    klass_name = klass.iface_file.fq_cpp_name.as_word
    scope_s = scoped_class_name(spec, klass)
    klass_name_ref = backend.cached_name_ref(klass.py_name)

    for member in klass.members:
        in_emitter = False
        signature_nr = 0

        for overload in klass.overloads:
            if not (overload.common is member and overload.pyqt_method_specifier is PyQtMethodSpecifier.SIGNAL and _has_optional_args(overload)):
                continue

            if not in_emitter:
                in_emitter = True

                sf.write('\n\n')

                if not spec.c_bindings:
                    sf.write(f'extern "C" {{static int emit_{klass_name}_{overload.cpp_name}(void *, PyObject *);}}\n\n')

                sf.write(
f'''static int emit_{klass_name}_{overload.cpp_name}(void *sipCppV, PyObject *sipArgs)
{{
    PyObject *sipParseErr = SIP_NULLPTR;
    {scope_s} *sipCpp = reinterpret_cast<{scope_s} *>(sipCppV);
''')

            # Generate the code that parses the args and emits the appropriate
            # overloaded signal.
            sf.write('\n    {\n')

            backend.g_arg_parser(sf, klass, overload.py_signature,
                    signature_nr)
            signature_nr += 1

            sf.write(
f'''        {{
            Py_BEGIN_ALLOW_THREADS
            sipCpp->{overload.cpp_name}(''')

            g_call_args(sf, spec, overload.cpp_signature,
                    overload.py_signature)

            sf.write(''');
            Py_END_ALLOW_THREADS

''')

            g_delete_temporaries(backend, sf, overload.py_signature)

            sf.write(
'''
            return 0;
        }
    }
''')

        if in_emitter:
            member_name_ref = backend.cached_name_ref(member.py_name)

            sf.write(
f'''
    sipNoMethod(sipParseErr, {klass_name_ref}, {member_name_ref}, SIP_NULLPTR);

    return -1;
}}
''')


def _g_pyqt_helper_defns(sf, spec):
    """ Generate the PyQt helper definitions. """

    if pyqt5_supported(spec) or pyqt6_supported(spec):
        module_name = spec.module.py_name

        sf.write(
f'''
sip_qt_metaobject_func sip_{module_name}_qt_metaobject;
sip_qt_metacall_func sip_{module_name}_qt_metacall;
sip_qt_metacast_func sip_{module_name}_qt_metacast;
''')


def _g_pyqt_helper_init(sf, spec):
    """ Initialise the PyQt helpers. """

    if pyqt5_supported(spec) or pyqt6_supported(spec):
        module_name = spec.module.py_name

        sf.write(
f'''

    sip_{module_name}_qt_metaobject = (sip_qt_metaobject_func)sipImportSymbol("qtcore_qt_metaobject");
    sip_{module_name}_qt_metacall = (sip_qt_metacall_func)sipImportSymbol("qtcore_qt_metacall");
    sip_{module_name}_qt_metacast = (sip_qt_metacast_func)sipImportSymbol("qtcore_qt_metacast");

    if (!sip_{module_name}_qt_metacast)
        Py_FatalError("Unable to import qtcore_qt_metacast");
''')


def _g_pyqt_signal_table_entry(sf, spec, bindings, klass, signal, member_nr):
    """ Generate an entry in the PyQt signal table. """

    klass_name = klass.iface_file.fq_cpp_name.as_word

    stripped = False
    signature_state = {}

    args = []

    for arg in signal.cpp_signature.args:
        # Do some signal argument normalisation so that Qt doesn't have to.
        if arg.is_const and (arg.is_reference or len(arg.derefs) == 0):
            signature_state[arg] = arg.is_reference

            arg.is_const = False
            arg.is_reference = False

        if arg.scopes_stripped != 0:
            strip = arg.scopes_stripped
            stripped = True
        else:
            strip = STRIP_GLOBAL

        args.append(
                fmt_argument_as_cpp_type(spec, arg, scope=klass.iface_file,
                        strip=strip))

    # Note the lack of a separating space.
    args = ','.join(args)

    sf.write(f'    {{"{signal.cpp_name}({args})')

    # If a scope was stripped then append an unstripped version which can
        # be parsed by PyQt.
    if stripped:
        args = []

        for arg in signal.cpp_signature.args:
            args.append(
                    fmt_argument_as_cpp_type(spec, arg,
                            scope=klass.iface_file, strip=STRIP_GLOBAL))

        # Note the lack of a separating space.
        args = ','.join(args)

        sf.write(f'|({args})')

    sf.write('", ')

    # Restore the signature state.
    for arg, is_reference in signature_state.items():
        arg.is_const = True
        arg.is_reference = is_reference

    if bindings.docstrings:
        sf.write('"')

        if signal.docstring is not None:
            if signal.docstring.signature is DocstringSignature.PREPENDED:
                g_overload_type_hint(sf, spec, signal)
                sf.write('\\n')

            sf.write(get_docstring_text(signal.docstring))

            if signal.docstring.signature is DocstringSignature.APPENDED:
                sf.write('\\n')
                g_overload_type_hint(sf, spec, signal)
        else:
            sf.write('\\1')
            g_overload_type_hint(sf, spec, signal)

        sf.write('", ')
    else:
        sf.write('SIP_NULLPTR, ')

    sf.write(f'&methods_{klass_name}[{member_nr}], ' if member_nr >= 0 else 'SIP_NULLPTR, ')

    sf.write(f'emit_{klass_name}_{signal.cpp_name}' if _has_optional_args(signal) else 'SIP_NULLPTR')

    sf.write('},\n')


def _g_pyqt_signals_table(backend, sf, bindings, klass):
    """ Generate the PyQt signals table and return True if anything was
    generated.
    """

    # Handle the trivial case.
    if not klass.is_qobject:
        return False

    spec = backend.spec
    is_signals = False

    # The signals must be grouped by name.
    for member in klass.members:
        member_nr = member.member_nr

        for overload in klass.overloads:
            if overload.common is not member or overload.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
                continue

            if member_nr >= 0:
                # See if there is a non-signal overload.
                for non_sig in klass.overloads:
                    if non_sig is not overload and non_sig.common is member and non_sig.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
                        break
                else:
                    member_nr = -1

            if not is_signals:
                is_signals = True

                _g_pyqt_emitters(backend, sf, klass)

                pyqt_version = '5' if pyqt5_supported(spec) else '6'
                sf.write(
f'''

/* Define this type's signals. */
static const pyqt{pyqt_version}QtSignal signals_{klass.iface_file.fq_cpp_name.as_word}[] = {{
''')

            # We enable a hack that supplies any missing optional arguments.
            # We only include the version with all arguments and provide an
            # emitter function which handles the optional arguments.
            _g_pyqt_signal_table_entry(sf, spec, bindings, klass, overload,
                    member_nr)

            member_nr = -1

    if is_signals:
        sf.write('    {SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}\n};\n')

    return is_signals


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


def _get_cached_name_ref(cached_name, as_nr=False):
    """ Return a reference to a cached name. """

    prefix = 'sipNameNr_' if as_nr else 'sipName_'

    return prefix + get_normalised_cached_name(cached_name)


def _get_class_flags(module, klass, py_debug):
    """ Return the flags for a class. """

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


def _get_slot_ref(slot_type):
    """ Return a reference to a slot. """

    return slot_type.name.lower() + '_slot'


def _get_subformat_char(arg):
    """ Return the sub-format character for an argument. """

    flags = 0

    if arg.transfer is Transfer.TRANSFER:
        flags |= 0x02

    if arg.transfer is Transfer.TRANSFER_BACK:
        flags |= 0x04

    if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        if len(arg.derefs) == 0 or arg.disallow_none:
            flags |= 0x01

        if arg.transfer is Transfer.TRANSFER_THIS:
            flags |= 0x10

        if arg.is_constrained or (arg.type is ArgumentType.CLASS and arg.definition.convert_to_type_code is None):
            flags |= 0x08

    return chr(ord('0') + flags)


def _get_type_ref(wrapped_object):
    """ Return the reference to the type of a wrapped object. """

    fq_cpp_name = wrapped_object.fq_cpp_name if isinstance(wrapped_object, WrappedEnum) else wrapped_object.iface_file.fq_cpp_name

    return 'sipType_' + fq_cpp_name.as_word


def _has_class_docstring(bindings, klass):
    """ Return True if a class has a docstring. """

    auto_docstring = False

    # Check for any explicit docstrings and remember if there were any that
    # could be automatically generated.
    if klass.docstring is not None:
        return True

    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if ctor.docstring is not None:
            return True

        if bindings.docstrings:
            auto_docstring = True

    if not klass.can_create:
        return False

    return auto_docstring


def _has_optional_args(overload):
    """ Return True if an overload has optional arguments. """

    args = overload.cpp_signature.args

    return len(args) != 0 and args[-1].default_value is not None


def _name_cache_as_list(name_cache):
    """ Return a name cache as a correctly ordered list of CachedName objects.
    """

    name_cache_list = []

    # Create the list sorted first by descending name length and then
    # alphabetical order.
    for k in sorted(name_cache.keys(), reverse=True):
        name_cache_list.extend(sorted(name_cache[k], key=lambda k: k.name))

    # Set the offset into the string pool for every used name.
    offset = 0

    # Map of suffix to previously processed name
    suffixes = {}

    for cached_name in name_cache_list:
        if not cached_name.used:
            continue

        name_len = len(cached_name.name)

        # See if the tail of a previous used name could be used instead.
        prev_name = suffixes.get(cached_name.name)
        if prev_name:
            cached_name.is_substring = True
            cached_name.offset = prev_name.offset + len(prev_name.name) - name_len

        if not cached_name.is_substring:
            cached_name.offset = offset
            offset += name_len + 1

            for i in range(len(cached_name.name)):
                suffixes.setdefault(cached_name.name[i:], cached_name)

    return name_cache_list


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
