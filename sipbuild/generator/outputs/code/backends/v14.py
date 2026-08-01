# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from collections.abc import Callable
from dataclasses import dataclass, field

from .....sip_module_configuration import SipModuleConfiguration

from ....python_slots import (is_extendable_slot, is_number_slot,
        is_rich_compare_slot)
from ....scoped_name import ScopedName, STRIP_GLOBAL
from ....specification import (AccessSpecifier, Argument, ArgumentType,
        ArrayArgument, DocstringSignature, GILUse, IfaceFileType, KwArgs,
        MappedType, Module, MultiInterpreterSupport, PySlot, Transfer,
        WrappedClass, WrappedEnum, WrappedVariable)
from ....utils import is_namespace_extender

from ...formatters import fmt_argument_as_name, fmt_enum_as_cpp_type

from ..snippets import (g_argument_variable, g_ctor_type_hint,
        g_member_function, g_overload_type_hint, g_static_function,
        g_type_init_body)
from ..utils import (callable_overloads, get_class_from_void,
        get_docstring_text, get_enum_member, get_function_table,
        get_mapped_type_flags, get_method_table, get_optional_ptr,
        get_type_from_void, get_use_in_code, has_method_docstring, is_string,
        is_used_in_code, module_classes, need_dealloc, py_scope,
        scoped_class_name, variables_in_scope)

from .abstract_backend import AbstractBackend


@dataclass
class StructField:
    """ Describe a field of a data structure to be generated. """

    # The name of the field.
    name: str

    # The value of the field if the selector is True.  If this is a callable
    # then it is called to get the actual value.
    value: str|int|Callable[[], str|int]

    # The selector.
    selector: bool = True

    # Set if the field is supported.
    supported: bool = True


@dataclass
class Struct:
    """ Describe a data structure to be generated. """

    # The declaration.  This will be omitted for nested structures.
    decl: str|None = None

    # The optional leading comment.
    comment: str|None = None

    # The structure's (possibly nested) fields.
    fields: tuple[StructField|Struct] = field(default_factory=tuple)

    # This is set of the fields are arranged in one line.
    in_one_line: bool = False

    # The name if this is a nested structure.
    name: str = None


def _generate_struct(sf, struct, indent=0):
    """ Generate a data structure. """

    indent_s = '    ' * indent

    if struct.decl:
        comment = indent_s + struct.comment + '\n' if struct.comment else ''
        sf.write('\n' + comment + indent_s + struct.decl + ' = {')
    else:
        if not struct.in_one_line:
            sf.write(indent_s)

        sf.write(f'.{struct.name} = {{')

    if struct.in_one_line:
        first_prefix = ''
        later_prefix = ', '
        suffix = ''
    else:
        sf.write('\n')
        first_prefix = later_prefix = '    ' + indent_s
        suffix = ',\n'

    prefix = first_prefix

    for field in struct.fields:
        if isinstance(field, Struct):
            _generate_struct(sf, field, indent=indent + 1)
        else:
            if not field.supported or not field.selector:
                continue

            value = field.value
            if callable(value):
                value = value()

            sf.write(f'{prefix}.{field.name} = {value}{suffix}')

        prefix = later_prefix

    if struct.decl:
        sf.write(indent_s + '};\n')
    elif struct.in_one_line:
        sf.write('}')
    else:
        sf.write(indent_s + '},\n')


class v14Backend(AbstractBackend):
    """ The backend code generator for v14 of the ABI. """

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
extern "C" {{static void *cast_{as_word}(sipModuleState *, void *, const sipClassTypeSpec *);}}
static void *cast_{as_word}(sipModuleState *sipMS, void *sipCppV, const sipClassTypeSpec *target_cts)
{{
    {get_class_from_void(spec, klass)};

    if (target_cts == &sipTypeSpec_{spec.module.py_name}_{as_word})
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
f'''    sipCppV = sipCastToTargetType(sipMS, {sc_type_ref}, static_cast<{sc_scope_s} *>(sipCpp), target_cts);
    if (sipCppV)
        return sipCppV;

''')
            else:
                # The super-class is a base class and so doesn't have a cast
                # function.  It also means that a simple check will do instead.
                sf.write(
f'''    if (sipIsTargetType(sipMS, {sc_type_ref}, target_cts))
        return static_cast<{sc_scope_s} *>(sipCpp);

''')

        sf.write(
'''    return SIP_NULLPTR;
}
''')

    def g_catch_body(self, sf):
        """ Generate the body of a catch clause. """

        sf.write(
'''                Py_ssize_t sipExcState = 0;
                PyObject *sipHandlerModule;
                sipModuleState *sipHandlerMS;
                sipExceptionHandler sipExcHandler;
                std::exception_ptr sipExcPtr = std::current_exception();

                while ((sipExcHandler = sipNextExceptionHandler(sipMS, &sipHandlerModule, &sipHandlerMS, &sipExcState)) != SIP_NULLPTR)
                {
                    bool sipCaught = sipExcHandler(sipHandlerMS, sipExcPtr);
                    Py_DECREF(sipHandlerModule);

                    if (sipCaught)
                    {
                        sipSetParserError(sipPStateP);
                        return SIP_NULLPTR;
                    }
                }

''')

    def g_class_api(self, sf, klass):
        """ Generate the API details for a class. """

        module = self.spec.module
        iface_file = klass.iface_file

        external = 'SIP_TYPE_ID_EXTERNAL|' if klass.external else ''

        if iface_file.module is module:
            module_ref = 'SIP_TYPE_ID_LOCAL_MODULE'
        else:
            module_ref = iface_file.module.module_nr

        if not is_namespace_extender(klass):
            sf.write(f'#define {_get_type_ref(klass)} SIP_TYPE_ID_TYPE_CLASS|{external}{module_ref}|{iface_file.type_nr}\n')

    def g_class_spec_extern_decl(self, sf, klass):
        """ Generate the extern declaration of a class specification. """

        klass_name = klass.iface_file.fq_cpp_name.as_word

        if is_namespace_extender(klass):
            sf.write(f'\nextern const sipAttrGroupSpec sipAttrsGroup_{klass_name};\n')
        else:
            module_name = self.spec.module.py_name
            sf.write(f'\nextern const sipClassTypeSpec sipTypeSpec_{module_name}_{klass_name};\n')

    def g_conversion_to_enum(self, sf, enum):
        """ Generate the code to convert a Python enum (sipSelf) to a C/C++
        enum (sipCpp).
        """

        type_ref = _get_type_ref(enum)
        cpp_name = enum.fq_cpp_name.as_cpp

        sf.write(
f'''
    {cpp_name} sipCpp;
    if (sipConvertToEnum(sipMS, sipSelf, &sipCpp, {type_ref}) < 0)
''')

    def g_cpp_dtor(self, sf):
        """ Generate the body of the dtor of a generated shadow class. """

        sf.write(
'''    if (sipPySelf)
        sipInstanceDestroyed(sipMS, &sipPySelf);
''')

    def g_create_wrapped_module(self, sf, bindings,
        name_cache_state,
        has_external,
        enums_state,
        has_virtual_error_handlers,
        nr_subclass_convertors,
        static_variables_state,
        has_slot_extenders,
        has_init_extenders
    ):
        """ Generate the code to generate a wrapped module and return the
        enums state.
        """

        spec = self.spec
        target_abi = spec.target_abi
        module = spec.module
        module_name = module.py_name

        nr_extenders = self._g_extenders_table(sf)

        # Generate the attributes tables.
        attrs = []
        attrs_static_variables = []
        attrs_types = []

        self._g_module_functions_table(sf, attrs)
        self._g_variables_table(sf, attrs, attrs_static_variables)
        self._add_type_attributes(attrs_types)

        if attrs:
            self._g_attributes_table(sf, attrs, "Attrs", module_name)

        if attrs_static_variables:
            self._g_attributes_table(sf, attrs_static_variables,
                    "AttrsStaticVariables", module_name)

        if attrs_types:
            self._g_attributes_table(sf, attrs_types, "AttrsTypes",
                    module_name)

        # Generate the pointer to the immutable SIP ABI structure that is
        # obtained from the sip module.  It is the only static variable used
        # and is set when the wrapped module is first imported into an
        # interpreter.  If the module is imported into another interpreter then
        # it will be overwritten but always by the same value.  It would be
        # possible to keep this pointer in the module state but it could only
        # be obtained by first acquiring the GIL and there are calls in the ABI
        # that don't otherwise need the GIL (and so would be less performant
        # than older ABIs).
        sf.write(
f'''

/* The immutable SIP ABI implementation. */
const sipABISpec *sipABI_{module_name};

''')

        # Generate the module specification.
        nr_imports = len(module.all_imports)
        nr_needed_types = len(module.needed_types)
        nr_typedefs = module.nr_typedefs

        struct = Struct(
            comment='/* The module\'s immutable specification. */',
            decl=f'static const sipModuleSpec sipModule_{module_name}',
            fields=(
                StructField('abi_major', target_abi[0]),
                StructField('abi_minor', target_abi[1]),
                StructField('sip_configuration',
                        f'0x{spec.sip_module_configuration:04x}'),
                StructField('nr_import_specs', nr_imports,
                        selector=(nr_imports != 0)),
                StructField('import_specs',
                        'sipImportedModules_' + module_name,
                        selector=(nr_imports != 0)),
                StructField('nr_type_specs', nr_needed_types,
                        selector=(nr_needed_types != 0)),
                StructField('type_specs', 'sipTypeSpecs_' + module_name,
                        selector=(nr_needed_types != 0)),
                StructField('nr_typedefs', nr_typedefs,
                        selector=(nr_typedefs != 0)),
                StructField('typedefs', 'typedefsTable',
                        selector=(nr_typedefs != 0)),
                StructField('virt_error_handlers', 'virtErrorHandlersTable',
                        selector=has_virtual_error_handlers),
                StructField('convertors',
                        'sipSubClassConvertors_' + module_name,
                        selector=(nr_subclass_convertors != 0)),
                StructField('attrs', '&sipAttrsGroup_' + module_name,
                        selector=attrs),
                StructField('attrs_static_variables',
                        '&sipAttrsStaticVariablesGroup_' + module_name,
                        selector=attrs_static_variables),
                StructField('attrs_types',
                        '&sipAttrsTypesGroup_' + module_name,
                        selector=attrs_types),
                StructField('license', '&module_license',
                        selector=(module.license is not None)),
                StructField('init_extenders',
                        'sipInitExtenders_' + module_name,
                        selector=has_init_extenders),
                StructField('extenders', 'sipExtenders_' + module_name,
                        selector=(nr_extenders != 0)),
                StructField('exception_handler',
                        'sipExceptionHandler_' + module_name,
                        selector=(bindings.exceptions and module.nr_exceptions != 0))
            )
        )

        _generate_struct(sf, struct)

        self._g_module_clear(sf)
        self._g_module_exec(sf)
        self._g_module_free(sf)
        self._g_module_traverse(sf)
        self.g_module_definition(sf, bindings)
        self.g_module_init_start(sf)

        if spec.sip_module:
            self._g_module_bootstrap(sf)
        else:
            sf.write(f'    sipABI_{module_name} = &sip_abi;\n\n')

        sf.write_code(module.preinitialisation_code)

        sf.write(f'    return sipModuleSlots_{module_name};\n}}\n')

        return enums_state

    def g_enums_specifications(self, sf, bindings, scope=None):
        """ Generate the specifications for the wrapped enums in a scope and
        return the optional dict of all enums defined in the module.
        """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        # If we are generating the specs at the module level then return a dict
        # of all enums (ie. including those defined with a scope) keyed by the
        # enum and with a value that is the index of the enum within the
        # scope's table of enums.
        if scope is None:
            enums_in_module = {}
            enum_nrs_by_scope = {}
        else:
            enums_in_module = None

        # Note that we go through the sorted table of needed types rather than
        # the unsorted list of all enums.
        enums_in_scope = []

        for needed_type in module.needed_types:
            if needed_type.type is not ArgumentType.ENUM:
                continue

            enum = needed_type.definition
            enum_py_scope = py_scope(enum.scope)

            # If required add the enum to the dict of all required enums even
            # we aren't generating a specification for it.
            if enums_in_module is not None:
                enum_nr = enum_nrs_by_scope.setdefault(enum_py_scope, 0)
                enums_in_module[enum] = enum_nr
                enum_nrs_by_scope[enum_py_scope] = enum_nr + 1

            if enum_py_scope is not scope:
                continue

            enums_in_scope.append(enum)

            # Generate the members table.
            sf.write(f'\nstatic const sipEnumMemberSpec sipEnumMembers_{module_name}_{enum.fq_cpp_name.as_word}[] = {{\n')

            for member in enum.members:
                name = str(member.py_name)
                value_field = self._get_enum_member_value_field(member)
                sf.write(f'    {{.name = "{name}", {value_field}}},\n')

            sf.write('    {0}\n};\n')

        for enum in enums_in_scope:
            enum_name = enum.fq_cpp_name.as_word

            # Generate any enum slot implementations.
            for member in enum.slots:
                g_member_function(self, sf, bindings, enum, member)

            # Generate any slot attributes table.
            attrs = []

            _g_py_method_table(sf, enum.slots, enum, attrs)

            if attrs:
                self._g_attributes_table(sf, attrs, "Attrs", enum_name)

            # Generate the enum type specification.
            if self.py_enums_supported():
                flags = 'SIP_TYPE_ENUM'
            else:
                flags = 'SIP_TYPE_SCOPED_ENUM' if enum.is_scoped else 'SIP_TYPE_ENUM'

            struct = Struct(
                decl=f'const sipEnumTypeSpec sipEnumTypeSpec_{module_name}_{enum_name}',
                fields=(
                    Struct(
                        name='base',
                        fields=(
                            StructField('cpp_name',
                                    self.cached_name_ref(
                                            enum.cached_fq_cpp_name)),
                            StructField('tp_name',
                                    f'"{module.fq_py_name}.{enum.py_name}"'),
                            StructField('scope_id',
                                    lambda: _get_type_ref(enum.scope),
                                    selector=(py_scope(enum.scope) is not None)),
                            StructField('flags', flags)
                        ),
                    ),
                    StructField('cpp_base_type',
                            'sipType_' + _get_enum_base_type_as_type_id_suffix(enum)),
                    StructField('members',
                            f'sipEnumMembers_{module_name}_{enum.fq_cpp_name.as_word}'),
                    StructField('py_base_type',
                            'SIP_ENUM_' + enum.base_type.name,
                            supported=self.py_enums_supported()),
                    StructField('attrs', '&sipAttrsGroup_' + enum_name,
                            selector=attrs)
                )
            )

            _generate_struct(sf, struct)

        return enums_in_module

    def g_exceptions_specifications(self, sf):
        """ Generate the specifications for any exceptions. """

        spec = self.spec
        module = spec.module

        if module.nr_exceptions > 0:
            module_name = module.py_name

            sf.write(
f'''
/* The module's immutable exceptions specifications. */
static const sipExceptionTypeSpec sipExceptionTypeSpecs_{module_name}[] = {{
''')

            for exception in spec.exceptions:
                if exception.exception_nr < 0:
                    continue

                if exception.builtin_base_exception is not None:
                    base_type_id = 'sipType_' + exception.builtin_base_exception
                else:
                    base_type_id = _get_type_ref(
                            exception.defined_base_exception)

                sf.write(
f'''    {{
        {{
            {self.cached_name_ref(exception.iface_file.cpp_name)},
            "{module.fq_py_name}.{exception.py_name}",
            sipType_Invalid,
            SIP_TYPE_EXCEPTION,
        }},
        {base_type_id}
    }},
''')

            sf.write('};\n')

    def g_exceptions_decls(self, sf):
        """ Generate the declarations of all exceptions. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name
        newline = '\n'

        for exception in spec.exceptions:
            if exception.iface_file.module is module and exception.exception_nr >= 0:
                sf.write(f'{newline}#define {_get_type_ref(exception)} SIP_TYPE_ID_TYPE_EXCEPTION|SIP_TYPE_ID_LOCAL_MODULE|{exception.iface_file.type_nr}\n')
                newline = ''

    def g_externals(self, sf):
        """ Generate the external types. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        for klass in spec.classes:
            if not klass.external:
                continue

            if klass.iface_file.module is not module:
                continue

            klass_name = klass.iface_file.fq_cpp_name.as_word
            cpp_name = self.cached_name_ref(klass.iface_file.cpp_name)

            sf.write(
f'''
static const sipTypeSpec sipTypeSpec_{module_name}_{klass_name} = {{
    {cpp_name}, NULL, sipType_Invalid, SIP_TYPE_EXTERNAL
}};
''')

        return None

    def g_get_py_reimpl(self, sf, klass, overload, virt_nr):
        """ Generate the code to get the Python reimplementation of a C++
        virtual.
        """

        if overload.is_const:
            const_cast_char = 'const_cast<char *>('
            const_cast_po = 'const_cast<sipSimpleWrapper **>('
            const_cast_tail = ')'
        else:
            const_cast_char = ''
            const_cast_po = ''
            const_cast_tail = ''

        klass_py_name_ref = self.cached_name_ref(klass.py_name) if overload.is_abstract else 'SIP_NULLPTR'
        member_py_name_ref = self.cached_name_ref(overload.common.py_name)

        sf.write(
f'''
    if (sipPySelf)
        sipMeth = sipIsPyMethod(sipMS, &sipGILState, {const_cast_char}&sipPyMethods[{virt_nr}]{const_cast_tail}, {const_cast_po}&sipPySelf{const_cast_tail}, {klass_py_name_ref}, {member_py_name_ref});
    else
        sipMeth = SIP_NULLPTR;
''')

    def g_import_tables(self, sf):
        """ Generated the tables related to imported modules. """

        module = self.spec.module
        module_name = module.py_name

        # Generate the subsiduary tables.
        for imported_module in module.all_imports:
            imported_module_name = imported_module.py_name

            if len(imported_module.needed_types) != 0:
                sf.write(
f'''

/* This defines the types that this module needs to import from {imported_module_name}. */
const char *sipImportedTypes_{module_name}_{imported_module_name}[] = {{
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

                    sf.write(f'    "{type_name}",\n')

                sf.write('};\n')

        # Generate the main table.
        sf.write(
f'''

/* This defines the modules that this module needs to import. */
static const sipImportedModuleSpec sipImportedModules_{module_name}[] = {{
''')

        for imported_module in module.all_imports:
            nr_types = len(imported_module.needed_types)
            type_names = f'sipImportedTypes_{module_name}_{imported_module.py_name}' if nr_types != 0 else 'NULL'

            sf.write(f'    {{"{imported_module.fq_py_name}", {nr_types}, {type_names}}},\n')

        sf.write('};\n')

    def g_imported_module_decls(self, sf, imported_module):
        """ Generate any declarations related to an imported module. """

        # Nothing to do.
        pass

    def g_init_extenders_table(self, sf):
        """ Generate the init extenders table. """

        module = self.spec.module

        sf.write(
f'''/* This defines the init extenders this module exports. */
static const sipInitExtenderSpec sipInitExtenders_{module.py_name}[] = {{
''')

        for klass in module.extenders:
            if klass.ctors:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                sf.write(f'    {{sipType_{klass_name}, sipDocs_{klass_name}, init_type_{klass_name}}},\n')

        sf.write('    {sipType_Invalid}\n};\n\n')

    def g_mapped_type_api(self, sf, mapped_type):
        """ Generate the API details for a mapped type. """

        module = self.spec.module
        module_name = module.py_name
        iface_file = mapped_type.iface_file
        mapped_type_name = iface_file.fq_cpp_name.as_word

        if iface_file.module is module:
            module_ref = 'SIP_TYPE_ID_LOCAL_MODULE'
        else:
            module_ref = iface_file.module.module_nr

        sf.write(
f'''
#define {_get_type_ref(mapped_type)} SIP_TYPE_ID_TYPE_MAPPED|{module_ref}|{iface_file.type_nr}
''')

        if iface_file.module is module:
            sf.write(
f'''
extern const sipMappedTypeSpec sipTypeSpec_{module_name}_{mapped_type_name};
''')

    def g_mapped_type_definition(self, sf, bindings, mapped_type):
        """ Generate the type structure that contains all the information
        needed by a mapped type.
        """

        spec = self.spec
        module = spec.module
        module_name = module.py_name
        mapped_type_name = mapped_type.iface_file.fq_cpp_name.as_word

        # Generate the enums table.
        self.g_enums_specifications(sf, bindings, scope=mapped_type)

        # Generate the attributes tables.
        attrs = []
        attrs_types = []

        _g_py_method_table(sf, get_function_table(mapped_type.members),
                mapped_type, attrs)

        self._add_type_attributes(attrs_types, scope=mapped_type)

        if attrs:
            self._g_attributes_table(sf, attrs, "Attrs", mapped_type_name)

        if attrs_types:
            self._g_attributes_table(sf, attrs_types, "AttrsTypes",
                    mapped_type_name)

        # Generate the type structure.
        struct = Struct(
            decl=f'const sipMappedTypeSpec sipTypeSpec_{module_name}_{mapped_type_name}',
            fields=(
                Struct(
                    name='base',
                    fields=(
                        StructField('cpp_name',
                                self.cached_name_ref(mapped_type.cpp_name)),
                        StructField('tp_name',
                                f'"{module.fq_py_name}.{mapped_type.py_name}"',
                                selector=(attrs or attrs_types)),
                        StructField('flags',
                                get_mapped_type_flags(mapped_type))
                    ),
                ),
                StructField('attrs', '&sipAttrsGroup_' + mapped_type_name,
                        selector=attrs),
                StructField('attrs_types',
                        '&sipAttrsTypesGroup_' + mapped_type_name,
                        selector=attrs_types),
                StructField('assign', 'assign_' + mapped_type_name,
                        selector=(not mapped_type.no_assignment_operator or mapped_type.movable)),
                StructField('array', 'array_' + mapped_type_name,
                        selector=(not mapped_type.no_default_ctor)),
                StructField('copy', 'copy_' + mapped_type_name,
                        selector=(not mapped_type.no_copy_ctor)),
                StructField('release', 'release_' + mapped_type_name,
                        selector=(not mapped_type.no_release)),
                StructField('cto', 'convertTo_' + mapped_type_name,
                        selector=(mapped_type.convert_to_type_code is not None)),
                StructField('cfrom', 'convertFrom_' + mapped_type_name,
                        selector=(mapped_type.convert_from_type_code is not None))
            )
        )

        _generate_struct(sf, struct)

    @staticmethod
    def g_method_error_handler_end(sf, overload):
        """ Generate the end of the error handling of a method. """

        sf.write(
'''            }

            sipAddException(sipError, sipPStateP, sipAPSpec.type_hint);

            if (sipError == sipErrorFail)
                return SIP_NULLPTR;
''')

    @staticmethod
    def g_method_error_handler_start(sf, overload, error_value):
        """ Generate the start of the error handling of a method. """

        sf.write(
'''            if (sipError == sipErrorNone)
            {
''')

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

    def g_module_definition(self, sf, bindings, has_module_functions=False):
        """ Generate the module definition structure. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        gil_support = self._MAP_GIL_USED[module.gil_use]
        interp_support = self._MAP_MULTI_INTERPRETER_SUPPORT[module.multi_interpreter_support]

        if spec.sip_module:
            state_size = '0'
        else:
            state_size = 'sizeof (sipModuleState)'
            sf.write(
'''

#include "sip_core.h"
#include "sip_wrapped_module.h"
''')

        # Note that the sip module implementation expects Py_mod_name and
        # Py_mod_state_size to be the first and second slots respectively.
        sf.write(
f'''

/* The module's immutable slot definitions. */
PyABIInfo_VAR(sip_abi_info);

static char sipModuleName_{module_name}[] = "{module.fq_py_name}";
''')

        if module.docstring is not None and bindings.docstrings:
            sf.write(f'static char sipModuleDocstring_{module_name}[] = "{get_docstring_text(module.docstring)}";\n');

        prefix = '' if spec.c_bindings else 'extern "C" '

        sf.write(
f'''
{prefix}PySlot sipModuleSlots_{module_name}[] = {{
    PySlot_STATIC_DATA(Py_mod_name, sipModuleName_{module_name}),
    PySlot_SIZE(Py_mod_state_size, {state_size}),
    PySlot_STATIC_DATA(Py_mod_abi, &sip_abi_info),
    PySlot_FUNC(Py_mod_exec, module_exec),
    PySlot_DATA(Py_mod_gil, {gil_support}),
    PySlot_DATA(Py_mod_multiple_interpreters, {interp_support}),
    PySlot_FUNC(Py_mod_state_clear, module_clear),
    PySlot_FUNC(Py_mod_state_free, module_free),
    PySlot_FUNC(Py_mod_state_traverse, module_traverse),
''')

        if module.docstring is not None and bindings.docstrings:
            sf.write(f'    PySlot_STATIC_DATA(Py_mod_doc, sipModuleDocstring_{module_name}),\n')

        sf.write('    PySlot_END\n};\n')

    def g_module_init_start(self, sf):
        """ Generate the start of the Python module initialisation function.
        """

        spec = self.spec

        arg_type = 'void' if spec.is_composite or spec.c_bindings else ''
        module_name = spec.module.py_name

        # This should only be needed until
        # https://github.com/pypa/distutils/issues/387 is resolved.
        sf.write(
f'''

#ifdef _WIN32
PyMODINIT_FUNC PyInit_{module_name}({arg_type})
{{
    PyErr_SetString(PyExc_NotImplementedError, "legacy init not supported");
    return NULL;
}}
#endif
''')

        sf.write(
f'''

/* The module's export function. */
PyMODEXPORT_FUNC PyModExport_{module_name}({arg_type})
{{
''')

    @staticmethod
    def g_not_implemented(sf):
        """ Generate the code to clear any exception and return
        Py_NotImplemented.
        """

        sf.write(
'''
    PyErr_Clear();

    Py_RETURN_NOTIMPLEMENTED;
''')

    def g_other_members(self, sf, bindings, scope, members):
        """ Generate other (backend-specific) members for a scope. """

        if scope.iface_file.type is IfaceFileType.NAMESPACE:
            for member in members:
                g_static_function(self, sf, bindings, member, scope=scope)

    def g_py_method_end(self, sf, state, nr_signatures):
        """ Generate the end of a method implementation. """

        self.g_static_function_end(sf, state, nr_signatures)

    def g_py_method_start(self, sf, bindings, scope, member, original_scope,
            need_args, need_self):
        """ Generate the start of a method implementation. """

        callable_name = _get_callable_name(scope, member)

        self._g_type_hints_docstring(sf, bindings, member,
                original_scope.overloads, callable_name, is_method=True)

        if not self.spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *callable_{callable_name}({self.get_py_method_args(is_impl=False)});}}\n')

        sf.write(f'static PyObject *callable_{callable_name}({self.get_py_method_args(is_impl=True, need_self=need_self, need_args=need_args)})\n{{\n')

        return None

    def g_sip_api(self, sf, module_name, module_state):
        """ Generate the SIP API as seen by generated code. """

        # These comprise the public ABI.
        sf.write(
f'''

/* The immutable SIP ABI. */
extern const sipABISpec *sipABI_{module_name};

#define sipBadCallableArg               sipABI_{module_name}->api_bad_callable_arg
#define sipBadCatcherResult             sipABI_{module_name}->api_bad_catcher_result
#define sipBuildResult(...)             sipABI_{module_name}->api_build_result(sipMS, __VA_ARGS__)
#define sipCallMethod(...)              sipABI_{module_name}->api_call_method(sipMS, __VA_ARGS__)
#define sipCanConvertToType(...)        sipABI_{module_name}->api_can_convert_to_type(sipMS, __VA_ARGS__)
#define sipConvertFromConstVoidPtr(...) sipABI_{module_name}->api_convert_from_const_void_ptr(sipMS, __VA_ARGS__)
#define sipConvertFromConstVoidPtrAndSize(...)  sipABI_{module_name}->api_convert_from_const_void_ptr_and_size(sipMS, __VA_ARGS__)
#define sipConvertFromEnum(...)         sipABI_{module_name}->api_convert_from_enum(sipMS, __VA_ARGS__)
#define sipConvertFromNewType(...)      sipABI_{module_name}->api_convert_from_new_type(sipMS, __VA_ARGS__)
#define sipConvertFromNewPyType(...)    sipABI_{module_name}->api_convert_from_new_py_type(sipMS, __VA_ARGS__)
#define sipConvertFromSliceObject       sipABI_{module_name}->api_convert_from_slice_object
#define sipConvertFromType(...)         sipABI_{module_name}->api_convert_from_type(sipMS, __VA_ARGS__)
#define sipConvertFromVoidPtr(...)      sipABI_{module_name}->api_convert_from_void_ptr(sipMS, __VA_ARGS__)
#define sipConvertFromVoidPtrAndSize(...)   sipABI_{module_name}->api_convert_from_void_ptr_and_size(sipMS, __VA_ARGS__)
#define sipConvertToArray(...)          sipABI_{module_name}->api_convert_to_array(sipMS, __VA_ARGS__)
#define sipConvertToBool                sipABI_{module_name}->api_convert_to_bool
#define sipConvertToEnum(...)           sipABI_{module_name}->api_convert_to_enum(sipMS, __VA_ARGS__)
#define sipConvertToType(...)           sipABI_{module_name}->api_convert_to_type(sipMS, __VA_ARGS__)
#define sipConvertToTypedArray(...)     sipABI_{module_name}->api_convert_to_typed_array(sipMS, __VA_ARGS__)
#define sipConvertToTypeUS(...)         sipABI_{module_name}->api_convert_to_type_us(sipMS, __VA_ARGS__)
#define sipConvertToVoidPtr             sipABI_{module_name}->api_convert_to_void_ptr
#define sipEnableAutoconversion(...)    sipABI_{module_name}->api_enable_autoconversion(sipMS, __VA_ARGS__)
#define sipExportSymbols(...)           sipABI_{module_name}->api_export_symbol(sipMS, __VA_ARGS__)
#define sipFindType(...)                sipABI_{module_name}->api_find_type(sipMS, __VA_ARGS__)
#define sipForceConvertToType(...)      sipABI_{module_name}->api_force_convert_to_type(sipMS, __VA_ARGS__)
#define sipForceConvertToTypeUS(...)    sipABI_{module_name}->api_force_convert_to_type_us(sipMS, __VA_ARGS__)
#define sipFree                         sipABI_{module_name}->api_free
#define sipGetAddress                   sipABI_{module_name}->api_get_address
#define sipGetInterpreterView()         sipABI_{module_name}->api_get_interpreter_view(sipMS)
#define sipGetModuleUserState()         sipABI_{module_name}->api_get_module_user_state(sipMS)
#define sipGetPyObjectRef(...)          sipABI_{module_name}->api_get_py_object_ref(sipMS, __VA_ARGS__)
#define sipGetPyTypeRef(...)            sipABI_{module_name}->api_get_py_type_ref(sipMS, __VA_ARGS__)
#define sipGetState                     sipABI_{module_name}->api_get_state
#define sipGetTypeUserObject            sipABI_{module_name}->api_get_type_user_object
#define sipGetUserObject                sipABI_{module_name}->api_get_user_object
#define sipIsUserType(...)              sipABI_{module_name}->api_is_user_type
#define sipGetSimpleWrapperType(...)    sipABI_{module_name}->api_get_simple_wrapper_type(sipMS, __VA_ARGS__)
#define sipGetVoidPtrType(...)          sipABI_{module_name}->api_get_void_ptr_type(sipMS, __VA_ARGS__)
#define sipGetWrapperType(...)          sipABI_{module_name}->api_get_wrapper_type(sipMS, __VA_ARGS__)
#define sipGetWrapperTypeType(...)      sipABI_{module_name}->api_get_wrapper_type_type(sipMS, __VA_ARGS__)
#define sipImportSymbol(...)            sipABI_{module_name}->api_import_symbol(sipMS, __VA_ARGS__)
#define sipIsOwnedByPython              sipABI_{module_name}->api_is_owned_by_python
#define sipLong_AsChar                  sipABI_{module_name}->api_long_as_char
#define sipLong_AsSignedChar            sipABI_{module_name}->api_long_as_signed_char
#define sipLong_AsUnsignedChar          sipABI_{module_name}->api_long_as_unsigned_char
#define sipLong_AsShort                 sipABI_{module_name}->api_long_as_short
#define sipLong_AsUnsignedShort         sipABI_{module_name}->api_long_as_unsigned_short
#define sipLong_AsInt                   sipABI_{module_name}->api_long_as_int
#define sipLong_AsUnsignedInt           sipABI_{module_name}->api_long_as_unsigned_int
#define sipLong_AsLong                  sipABI_{module_name}->api_long_as_long
#define sipLong_AsUnsignedLong          sipABI_{module_name}->api_long_as_unsigned_long
#define sipLong_AsLongLong              sipABI_{module_name}->api_long_as_long_long
#define sipLong_AsUnsignedLongLong      sipABI_{module_name}->api_long_as_unsigned_long_long
#define sipLong_AsSizeT                 sipABI_{module_name}->api_long_as_size_t
#define sipMalloc                       sipABI_{module_name}->api_malloc
#define sipParseResult(...)             sipABI_{module_name}->api_parse_result(sipMS, __VA_ARGS__)
#define sipRaiseTypeException(...)      sipABI_{module_name}->api_raise_type_exception(sipMS, __VA_ARGS__)
#define sipRegisterEventHandlers(...)   sipABI_{module_name}->api_register_event_handlers(sipMS, __VA_ARGS__)
#define sipRegisterPyType(...)          sipABI_{module_name}->api_register_py_type(sipMS, __VA_ARGS__)
#define sipReleaseType(...)             sipABI_{module_name}->api_release_type(sipMS, __VA_ARGS__)
#define sipReleaseTypeUS(...)           sipABI_{module_name}->api_release_type_us(sipMS, __VA_ARGS__)
#define sipResolveTypedef(...)          sipABI_{module_name}->api_resolve_typedef(sipMS, __VA_ARGS__)
#define sipSetModuleUserState(...)      sipABI_{module_name}->api_set_module_user_state(sipMS, __VA_ARGS__)
#define sipSetTypeUserObject            sipABI_{module_name}->api_set_type_user_object
#define sipSetUserObject                sipABI_{module_name}->api_set_user_object
#define sipTransferBack(...)            sipABI_{module_name}->api_transfer_back(sipMS, __VA_ARGS__)
#define sipTransferTo(...)              sipABI_{module_name}->api_transfer_to(sipMS, __VA_ARGS__)
#define sipTypeScope(...)               sipABI_{module_name}->api_type_scope(sipMS, __VA_ARGS__)
#define sipVisitWrappers(...)           sipABI_{module_name}->api_visit_wrappers(sipMS, __VA_ARGS__)
''')

        if self.py_enums_supported():
            sf.write(
f'''#define sipIsEnumFlag(...)              sipABI_{module_name}->api_is_enum_flag(sipMS, __VA_ARGS__)
''')

        # These comprise the public helper ABI.
        sf.write(
f'''#define sipBadLengthForSlice            sipABI_{module_name}->api_bad_length_for_slice
#define sipConvertFromSequenceIndex     sipABI_{module_name}->api_convert_from_sequence_index
#define sipEnableGC                     sipABI_{module_name}->api_enable_gc
#define sipFromDate                     sipABI_{module_name}->api_from_date
#define sipFromDateTime                 sipABI_{module_name}->api_from_date_time
#define sipFromMethod                   sipABI_{module_name}->api_from_method
#define sipFromTime                     sipABI_{module_name}->api_from_time
#define sipGetCFunction                 sipABI_{module_name}->api_get_c_function
#define sipGetDate                      sipABI_{module_name}->api_get_date
#define sipGetDateTime                  sipABI_{module_name}->api_get_date_time
#define sipGetFrameRef                  sipABI_{module_name}->api_get_frame_ref
#define sipGetMethod                    sipABI_{module_name}->api_get_method
#define sipGetTime                      sipABI_{module_name}->api_get_time
#define sipObjectDump                   sipABI_{module_name}->api_object_dump
#define sipPyTypeDictRef                sipABI_{module_name}->api_py_type_dict_ref
#define sipUnicodeData                  sipABI_{module_name}->api_unicode_data
#define sipUnicodeNew                   sipABI_{module_name}->api_unicode_new
#define sipUnicodeWrite                 sipABI_{module_name}->api_unicode_write
''')

        # These comprise the private ABI.
        sf.write(
f'''#define sipModuleClear                  sipABI_{module_name}->api_module_clear
#define sipModuleExec                   sipABI_{module_name}->api_module_exec
#define sipModuleFree                   sipABI_{module_name}->api_module_free
#define sipModuleTraverse               sipABI_{module_name}->api_module_traverse

#define sipAbstractMethod               sipABI_{module_name}->api_abstract_method
#define sipAddException                 sipABI_{module_name}->api_add_exception
#define sipCallErrorHandler             sipABI_{module_name}->api_call_error_handler
#define sipCallProcedureMethod(...)     sipABI_{module_name}->api_call_procedure_method(sipMS, __VA_ARGS__)
#define sipCallHook                     sipABI_{module_name}->api_call_hook
#define sipCastToTargetType             sipABI_{module_name}->api_cast_to_target_type
#define sipDeprecated                   sipABI_{module_name}->api_deprecated
#define sipGetCppPtr                    sipABI_{module_name}->api_get_cpp_ptr
#define sipInitSlotImpl                 sipABI_{module_name}->api_init_slot_impl
#define sipInstanceDestroyed            sipABI_{module_name}->api_instance_destroyed
#define sipIsDerivedClass               sipABI_{module_name}->api_is_derived_class
#define sipIsPyMethod                   sipABI_{module_name}->api_is_py_method
#define sipIsTargetType                 sipABI_{module_name}->api_is_target_type
#define sipKeepReference(...)           sipABI_{module_name}->api_keep_reference(sipMS, __VA_ARGS__)
#define sipNextExceptionHandler         sipABI_{module_name}->api_next_exception_handler
#define sipParseArgs                    sipABI_{module_name}->api_parse_args
#define sipParseArgsC                   sipABI_{module_name}->api_parse_args_c
#define sipParseCtorArgs                sipABI_{module_name}->api_parse_ctor_args
#define sipRaiseUnknownException        sipABI_{module_name}->api_raise_unknown_exception
#define sipSelfIsArg                    sipABI_{module_name}->api_self_is_arg
#define sipSetParserError               sipABI_{module_name}->api_set_parser_error
#define sipTrace(...)                   sipABI_{module_name}->api_trace(sipMS, __VA_ARGS__)
''')

        # Generate the declaration of the module token.
        lang = '' if self.spec.c_bindings else '"C" '

        sf.write(f'''

/* The module token. */
extern {lang}PySlot sipModuleSlots_{module_name}[];
''')

        # Generate the declarations of the individual scope enum specification
        # tables.
        need_intro = True
        all_module_enums = module_state

        for enum in all_module_enums:
            # Ignore global enums.
            if py_scope(enum.scope) is None:
                continue

            if need_intro:
                sf.write('\n\n/* Declare the enum specifications for each scope. */\n')
                need_intro = False

            sf.write(f'extern const sipEnumTypeSpec sipEnumTypeSpec_{module_name}_{enum.fq_cpp_name.as_word};\n')

    def g_slot_extender_impl(self, sf, bindings, member, klass=None):
        """ Generate the implementation of a slot extender. """

        assert klass is not None

        g_member_function(self, sf, bindings, klass, member)

    def g_slot_extenders_table(self, sf):
        """ Generate the table of slot implementations. """

        for klass in self.spec.module.extenders:
            attrs = []

            _g_py_method_table(sf, klass.members, klass, attrs)

            if attrs:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                self._g_attributes_table(sf, attrs, 'Attrs', klass_name)

    def g_static_function_end(self, sf, state, nr_signatures):
        """ Generate the end of a static function implementation. """

        if nr_signatures != 0:
            sf.write('\n    return SIP_NULLPTR;\n')

        sf.write('}\n')

    def g_static_function_start(self, sf, bindings, scope_py, member,
            overloads):
        """ Generate the start of a static function implementation. """

        spec = self.spec
        callable_name = _get_callable_name(scope_py, member)

        self._g_type_hints_docstring(sf, bindings, member, overloads,
                callable_name)

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *callable_{callable_name}({self.get_py_method_args(is_impl=False)});}}\n')

        sf.write(f'static PyObject *callable_{callable_name}({self.get_py_method_args(is_impl=True, need_self=True)})\n')

        sf.write('{')

        return None

    def g_static_variables_table(self, sf, scope=None):
        """ Generate the table of static variables. """

        # We actually do it later.
        return None

    def g_subclass_convertor(self, sf, klass):
        """ Generate a sub-class convertor. """

        klass_name = klass.iface_file.fq_cpp_name.as_word
        base_cpp = klass.subclass_base.iface_file.fq_cpp_name.as_cpp

        if not self.spec.c_bindings:
            sf.write(f'extern "C" {{static sipTypeID sipSubClass_{klass_name}(void **);}}\n')

        sf.write(
f'''static sipTypeID sipSubClass_{klass_name}(void **sipCppRet)
{{
    {base_cpp} *sipCpp = reinterpret_cast<{base_cpp} *>(*sipCppRet);
    sipTypeID sipType;

''')

        sf.write_code(klass.convert_to_subclass_code)

        sf.write('\n    return sipType;\n}\n')

    def g_subclass_convertors_table(self, sf):
        """ Generate the table of sub-class convertors. """

        spec = self.spec
        module = spec.module

        sf.write(
f'''

/* This defines the sub-class convertors that this module defines. */
static sipSubClassConvertorSpec sipSubClassConvertors_{module.py_name}[] = {{
''')

        for klass in module_classes(spec):
            if klass.convert_to_subclass_code is None:
                continue

            klass_name = klass.iface_file.fq_cpp_name.as_word

            sf.write(f'    {{{_get_type_ref(klass.subclass_base)}, sipSubClass_{klass_name}}},\n')

        sf.write('    {0}\n};\n')

    def g_type_definition(self, sf, bindings, klass, py_debug):
        """ Generate the type structure that contains all the information
        needed by the meta-type.  A sub-set of this is used to extend
        namespaces.
        """

        spec = self.spec
        module = spec.module
        module_name = module.py_name
        klass_name = klass.iface_file.fq_cpp_name.as_word

        # Generate the enums table.
        self.g_enums_specifications(sf, bindings, scope=klass)

        # Generate the attributes tables.
        attrs = []

        if is_namespace_extender(klass):
            attrs_static_variables = attrs
            attrs_types = attrs
        else:
            attrs_static_variables = []
            attrs_types = []

        _g_class_method_table(sf, attrs, klass)
        self._g_properties_table_decl(sf, attrs, klass)

        self._g_variables_table(sf, attrs, attrs_static_variables, scope=klass)

        self._add_type_attributes(attrs_types, scope=klass)

        if is_namespace_extender(klass):
            # Generate the extender attributes table.
            if attrs:
                self._g_attributes_table(sf, attrs, "Attrs", klass_name,
                        is_static=False)

            return

        if attrs:
            self._g_attributes_table(sf, attrs, "Attrs", klass_name)

        self._g_properties_table(sf, bindings, attrs, klass)

        if attrs_static_variables:
            self._g_attributes_table(sf, attrs_static_variables,
                    "AttrsStaticVariables", klass_name)

        if attrs_types:
            self._g_attributes_table(sf, attrs_types, "AttrsTypes", klass_name)

        # Generate the array of super-class type IDs.
        if len(klass.superclasses) != 0:
            supers = ', '.join(
                    [_get_type_ref(s) for s in klass.superclasses])

            sf.write(f'\nstatic const sipTypeID supers_{klass_name}[] = {{{supers}|SIP_TYPE_ID_SENTINEL}};\n')

        # Generate the class type specification.
        if klass.real_class is not None:
            scope_id = _get_type_ref(klass.real_class)
        elif py_scope(klass.scope) is not None:
            scope_id = _get_type_ref(klass.scope)
        else:
            scope_id = None

        if klass.supertype is not None:
            # This is a bit of a hack.
            supertype = str(klass.supertype)
            if supertype.endswith('.simplewrapper'):
                supertype = '""'
        else:
            supertype = 'NULL'

        struct = Struct(
            decl=f'const sipClassTypeSpec sipTypeSpec_{module_name}_{klass_name}',
            fields=(
                Struct(
                    name='base',
                    fields=(
                        StructField('cpp_name',
                                self.cached_name_ref(
                                        klass.iface_file.cpp_name),
                                selector=(klass.real_class is None)),
                        StructField('tp_name',
                                f'"{module.fq_py_name}.{klass.py_name}"'),
                        StructField('scope_id', scope_id,
                                selector=(scope_id is not None)),
                        StructField('flags',
                                _get_class_flags(module, klass, py_debug))
                    ),
                ),
                StructField('docstring',
                        lambda: f'"{_get_typed_docstring_text(klass.docstring)}"',
                        selector=(klass.docstring is not None and bindings.docstrings)),
                StructField('docs', 'sipDocs_' + klass_name,
                        selector=(klass.can_create and bindings.docstrings)),
                StructField('metatype',
                        lambda: self.cached_name_ref(klass.metatype),
                        selector=(klass.metatype is not None)),
                StructField('supertype', supertype,
                        selector=(supertype is not None)),
                StructField('supers', 'supers_' + klass_name,
                        selector=(len(klass.superclasses) != 0)),
                StructField('attrs',
                        '&sipAttrsGroup_' + klass_name, selector=attrs),
                StructField('attrs_static_variables',
                        '&sipAttrsStaticVariablesGroup_' + klass_name,
                        selector=attrs_static_variables),
                StructField('attrs_types', '&sipAttrsTypesGroup_' + klass_name,
                        selector=attrs_types),
                StructField('init', 'init_type_' + klass_name,
                        selector=klass.can_create),
                StructField('init_slot', 'init_slot_' + klass_name,
                        selector=klass.can_create),
                StructField('traverse', 'traverse_' + klass_name,
                        selector=(klass.gc_traverse_code is not None)),
                StructField('clear', 'clear_' + klass_name,
                        selector=(klass.gc_clear_code is not None)),
                StructField('getbuffer', 'getbuffer_' + klass_name,
                        selector=(klass.bi_get_buffer_code is not None)),
                StructField('releasebuffer', 'releasebuffer_' + klass_name,
                        selector=(klass.bi_release_buffer_code is not None)),
                StructField('dealloc', 'dealloc_' + klass_name,
                        selector=need_dealloc(spec, bindings, klass)),
                StructField('assign', 'assign_' + klass_name,
                        selector=(spec.c_bindings or klass.needs_copy_helper)),
                StructField('array', 'array_' + klass_name,
                        selector=(spec.c_bindings or klass.needs_array_helper)),
                StructField('copy', 'copy_' + klass_name,
                        selector=(spec.c_bindings or klass.needs_copy_helper)),
                StructField('release', 'release_' + klass_name,
                        selector=(not spec.c_bindings and klass.iface_file.type is not IfaceFileType.NAMESPACE)),
                StructField('cast', 'cast_' + klass_name,
                        selector=(len(klass.superclasses) != 0)),
                StructField('cto', 'convertTo_' + klass_name,
                        selector=(klass.convert_to_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE)),
                StructField('cfrom', 'convertFrom_' + klass_name,
                        selector=(klass.convert_from_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE)),
                StructField('pickle', 'pickle_' + klass_name,
                        selector=(klass.pickle_code is not None)),
                StructField('final', 'final_' + klass_name,
                        selector=(klass.finalisation_code is not None)),
                StructField('array_delete', 'array_delete_' + klass_name,
                        selector=(spec.c_bindings or klass.needs_array_helper)),
                StructField('sizeof_class',
                        f'sizeof ({scoped_class_name(self.spec, klass)})',
                        selector=klass.can_create)
            )
        )

        _generate_struct(sf, struct)

    def g_type_init(self, sf, bindings, klass, need_self, need_owner):
        """ Generate the code that initialises a type. """

        spec = self.spec
        klass_name = klass.iface_file.fq_cpp_name.as_word

        # Generate any constructor documentation.
        if bindings.docstrings:
            need_decl = True

            for ctor in klass.ctors:
                if ctor.access_specifier is AccessSpecifier.PRIVATE:
                    continue

                if need_decl:
                    sf.write(f'static const sipDocSpec sipDocs_{klass_name}[] = {{\n')
                    need_decl = False

                sf.write('    {"')
                g_ctor_type_hint(sf, spec, bindings, klass, ctor)

                if ctor.docstring is not None:
                    sf.write(f'", "{_get_typed_docstring_text(ctor.docstring)}"}},\n')
                else:
                    sf.write('", NULL},\n')

            if not need_decl:
                sf.write('    {0}\n};\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *init_type_{klass_name}(sipModuleState *, PyObject **, PyObject *, PyObject *const *, Py_ssize_t, PyObject *, PyObject **, PyObject **);}}\n')

        sip_owner = _get_optional_arg('sipOwner', need_owner)

        sf.write(
f'''static void *init_type_{klass_name}(sipModuleState *sipMS, PyObject **sipPStateP, PyObject *sipSelf, PyObject *const *sipArgs, Py_ssize_t sipNrArgs, PyObject *sipKwdNames, PyObject **sipUnused, PyObject **{sip_owner})
{{
''')

        g_type_init_body(self, sf, bindings, klass)

        sf.write('}\n\n')

        # Now generate the tp_init slot implementation.
        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int init_slot_{klass_name}(PyObject *, PyObject *, PyObject *);}}\n')

        sf.write(f'''static int init_slot_{klass_name}(PyObject *self, PyObject *args, PyObject *kwds)
{{
    return sipInitSlotImpl(self, args, kwds, (void *)sipModuleSlots_{spec.module.py_name}, sipType_{klass_name});
}}

''')

    def g_types_table(self, sf, enums_state):
        """ Generate the types table for a module. """

        module = self.spec.module
        module_name = module.py_name

        sf.write(
f'''

/*
 * This defines each type in this module.
 */
static const sipTypeSpec *const sipTypeSpecs_{module_name}[] = {{
''')

        for needed_type in module.needed_types:
            if needed_type.type is ArgumentType.CLASS:
                klass = needed_type.definition

                if klass.external:
                    sf.write(f'    &sipTypeSpec_{module_name}_{klass.iface_file.fq_cpp_name.as_word},\n')
                elif not klass.is_hidden_namespace:
                    sf.write(f'    &sipTypeSpec_{module_name}_{klass.iface_file.fq_cpp_name.as_word}.base,\n')

            elif needed_type.type is ArgumentType.MAPPED:
                mapped_type = needed_type.definition

                sf.write(f'    &sipTypeSpec_{module_name}_{mapped_type.iface_file.fq_cpp_name.as_word}.base,\n')

            elif needed_type.type is ArgumentType.ENUM:
                enum = needed_type.definition

                sf.write(f'    &sipEnumTypeSpec_{module_name}_{enum.fq_cpp_name.as_word}.base,\n')

            elif needed_type.type is ArgumentType.EXCEPTION:
                exception = needed_type.definition

                sf.write(f'    &sipExceptionTypeSpecs_{module_name}[{exception.exception_nr}].base,\n')

        sf.write('};\n')

    def g_virt_error_handler_decls(self, sf):
        """ Generate the declarations of any locally defined virtual error
        handlers.
        """

        # The handlers are private.
        pass

    def g_virt_error_handler_impl(self, sf, virtual_error_handler):
        """ Generate the implementations of any locally defined virtual error
        handlers.
        """

        code = virtual_error_handler.code

        self_name = get_use_in_code(code, 'sipPySelf')
        state_name = get_use_in_code(code, 'sipThreadStateToken')

        # Look for the legacy name if the modern name hasn't been used.
        if state_name == '':
            state_name = get_use_in_code(code, 'sipGILState')

        sf.write(
f'''

static void sipVEH_{self.spec.module.py_name}_{virtual_error_handler.name}(sipModuleState *sipMS, sipSimpleWrapper *{self_name}, PyThreadStateToken *{state_name})
{{
''')

        sf.write_code(code)

        sf.write('}\n')

    @staticmethod
    def g_wrapper_ref_decl(sf):
        """ Generate the code that declares a wrapper reference. """

        sf.write('    sipModuleState *sipMS;\n')
        sf.write('    sipSimpleWrapper *sipPySelf;\n')

    @staticmethod
    def cached_name_ref(cached_name, as_nr=False):
        """ Return a reference to a cached name. """

        # In v14 we always use the literal text.
        assert(not as_nr)

        return '"' + cached_name.name + '"'

    def custom_enums_supported(self):
        """ Return True if custom enums are supported. """

        return SipModuleConfiguration.CustomEnums in self.spec.sip_module_configuration

    @staticmethod
    def get_add_exception_call(error_state):
        """ Return a call to sipAddException(). """

        return f'sipAddException({error_state}, sipPStateP, sipAPSpec.type_hint)'

    def get_enum_to_py_conversion(self, enum, value_name):
        """ Return the code to convert a C/C++ enum to a Python object. """

        if enum.fq_cpp_name is None:
            return f'PyLong_FromLong({value_name})'

        return f'sipConvertFromEnum(&{value_name}, {_get_type_ref(enum)})'

    def get_enum_ref_value(self, enum):
        """ Return the value of an enum's reference. """

        module_nr = 'SIP_TYPE_ID_LOCAL_MODULE' if enum.module is self.spec.module else enum.module.module_nr

        return f'SIP_TYPE_ID_TYPE_ENUM|{module_nr}|{enum.type_nr}'

    def get_error_handler_ref(self, error_handler):
        """ Return a reference to an error handler. """

        return 'SIP_NULLPTR' if error_handler is None else f'"{error_handler.name}"'

    @staticmethod
    def get_error_handler_ref_type():
        """ Return the type of a reference to an error handler. """

        return 'const char *'

    @staticmethod
    def get_module_context():
        """ Return the value of a module context passed as the first argument
        to many ABI calls.
        """

        return 'sipMS, '

    @staticmethod
    def get_module_context_decl():
        """ Return the declaration of the value of a module context passed as
        the first argument to many ABI calls.
        """

        return 'sipModuleState *sipMS, '

    def get_py_method_args(self, *, is_impl, need_self=False, need_args=True):
        """ Return the part of a Python method signature that are ABI
        dependent.
        """

        if is_impl:
            args = 'sipModuleState *sipMS, PyObject **sipPStateP, PyObject *'

            if need_self:
                args += 'sipSelf'
        else:
            args = 'PyObject *, PyObject **, PyObject *'

        args += ', PyObject *const *'

        if is_impl and need_args:
            args += 'sipArgs'

        args += ', Py_ssize_t'

        if is_impl and need_args:
            args += ' sipNrArgs'

        args += ', PyObject *'

        if is_impl and need_args:
            args += 'sipKwdNames'

        return args

    @staticmethod
    def get_raise_unknown_exception():
        """ Return the call to raise an exception about an unknown exception.
        """

        return 'sipRaiseUnknownException(sipPStateP)'

    @staticmethod
    def get_result_parser():
        """ Return the name of the Python reimplementation result parser. """

        return 'sipParseResult'

    def get_sipself_test(self, klass):
        """ Return the code that checks if 'sipSelf' was bound or passed as an
        argument.
        """

        return f'sipSelfIsArg(sipMS, sipSelf, {_get_type_ref(klass)})'

    @staticmethod
    def get_slot_ref(slot_type):
        """ Return a reference to a slot. """

        if is_rich_compare_slot(slot_type):
            return 'Py_tp_richcompare'

        return _SLOT_ID_MAP[slot_type]

    @staticmethod
    def get_spec_suffix():
        """ Return the suffix used for immutable specifications. """

        return 'Spec'

    @staticmethod
    def get_type_ref(wrapped_object):
        """ Return the reference to the type of a wrapped object. """

        return _get_type_ref(wrapped_object)

    @staticmethod
    def get_wrapper_ref_init():
        """ Return the initialisation of a wrapper reference. """

        return 'sipMS(SIP_NULLPTR), sipPySelf(SIP_NULLPTR)'

    @staticmethod
    def g_wrapper_ref_set(sf):
        """ Generate the code that sets the value of a wrapper reference. """

        sf.write('            sipCpp->sipMS = sipMS;\n')
        sf.write('            sipCpp->sipPySelf = (sipSimpleWrapper *)sipSelf;\n\n')

    def py_enums_supported(self):
        """ Return True if Python enums are supported. """

        return SipModuleConfiguration.PyEnums in self.spec.sip_module_configuration

    def _g_module_bootstrap(self, sf):
        """ Generate the module bootstrap code. """

        spec = self.spec
        module_name = spec.module.py_name

        sf.write(
f'''    PyObject *sip_module = PyImport_ImportModule("{spec.sip_module}");
    if (sip_module == NULL)
        return NULL;

    PyObject *capsule = PyObject_GetAttrString(sip_module, "_C_BOOTSTRAP");
    if (capsule == NULL)
    {{
        Py_DECREF(sip_module);
        return NULL;
    }}

    if (!PyCapsule_IsValid(capsule, "_C_BOOTSTRAP"))
    {{
        Py_DECREF(capsule);
        Py_DECREF(sip_module);
        return NULL;
    }}

    /*
     * The first stage of the bootstrap is to get a function that will be
     * called with the ABI version as its only argument and will return the
     * corresponding SIP ABI implementation.
     */
    sipBootstrapFunc bootstrap_func = (sipBootstrapFunc)PyCapsule_GetPointer(
            capsule, "_C_BOOTSTRAP");

    Py_DECREF(capsule);
    Py_DECREF(sip_module);

    if (bootstrap_func == NULL)
        return NULL;

    /*
     * The second stage of the bootstrap is to call the function from the first
     * stage to get the SIP ABI implementation (or NULL if it is not
     * supported).
     */
    sipABI_{module_name} = bootstrap_func({spec.target_abi[0]});
    if (sipABI_{module_name} == NULL)
        return NULL;

    /* Set the wrapped module state size from the sip module. */
    sipModuleSlots_{module_name}[1].sl_size = sipABI_{module_name}->module_state_size;

''')

    @staticmethod
    def _g_module_clear(sf):
        """ Generate the module clear slot. """

        sf.write(
'''

/* The module's clear slot. */
static int module_clear(PyObject *mod)
{
    return sipModuleClear(mod);
}
''')

    def _g_module_exec(self, sf):
        """ Generate the module exec slot. """

        spec = self.spec
        module = spec.module
        module_name = module.py_name

        sf.write(
'''

/* The module's exec function. */
static int module_exec(PyObject *sipModule)
{
''')

        if spec.sip_module:
            sip_init_func_ref = 'sipModuleExec'
        else:
            sip_init_func_ref = 'sip_api_module_exec';

        if module.initialisation_code or module.postinitialisation_code:
            sf.write(
'''    sipModuleState *sipMS = (sipModuleState *)PyModule_GetState(sipModule);
    if (sipMS == NULL)
        return -1;

''')

        if module.initialisation_code:
            sf.write_code(module.initialisation_code)

            sf.write(
'''
    if (PyErr_Occurred())
        return -1;

''')

        sf.write(
f'''    if ({sip_init_func_ref}(sipModule, &sipModule_{module_name}) < 0)
        return -1;
''')

        if module.postinitialisation_code:
            if is_used_in_code(module.postinitialisation_code, 'sipModuleDict'):
                sf.write(
'''
    PyObject *sipModuleDict = PyModule_GetDict(sipModule);
    if (sipModuleDict == NULL)
        return -1;
''')

            sf.write('\n')
            sf.write_code(module.postinitialisation_code)

        sf.write(
'''
    return 0;
}
''')

    def _g_module_free(self, sf):
        """ Generate the module free slot. """

        sf.write(
'''

/* The module's free slot. */
static void module_free(void *mod_ptr)
{
    sipModuleFree(mod_ptr);
}
''')

    def _g_attributes_table(self, sf, attrs, attrs_name, table_name,
            is_static=True):
        """ Generate an attributes specification table and related group. """

        # Sort the table on attribute name.
        attrs.sort(key=lambda a: a[1])

        sf.write(
f'''
static const sipAttrSpec sip{attrs_name}_{table_name}[] = {{
''')

        for attr_type, name, spec_member, spec_name in attrs:
            sf.write(f'    {{"{attr_type}{name}", {{.{spec_member} = {spec_name}}}}},\n')

        static = 'static ' if is_static else ''

        sf.write(
f'''}};

{static}const sipAttrGroupSpec sip{attrs_name}Group_{table_name} = {{{len(attrs)}, sip{attrs_name}_{table_name}}};
''')

    def _g_extenders_table(self, sf):
        """ Generate the table of type extenders and return the number actually
        generated.
        """

        spec = self.spec
        module = self.spec.module

        # For historical reasons class slot extenders and namespace extenders
        # are in different data structures.
        extenders = []
        extenders = [k for k in spec.classes
                if k.iface_file.module is module and is_namespace_extender(k)]
        extenders.extend(module.extenders)

        if not extenders:
            return 0

        sf.write(
f'''/* This defines the type extenders this module exports. */
static const sipExtenderSpec sipExtenders_{module.py_name}[] = {{
''')

        for klass in extenders:
            klass_name = klass.iface_file.fq_cpp_name.as_word
            sf.write(f'    {{sipType_{klass_name}, &sipAttrsGroup_{klass_name}}},\n')

        sf.write('    {sipType_Invalid}\n};\n\n')

        return len(extenders);

    def _g_module_functions_table(self, sf, attrs):
        """ Generate the table of module functions and update the attributes
        generated.
        """

        module = self.spec.module

        nr_callables = _g_callables_table(sf, module.global_functions, module,
                attrs)

        # Generate the module functions for any hidden namespaces.
        for klass in self.spec.classes:
            if klass.iface_file.module is module and klass.is_hidden_namespace:
                nr_callables = _g_callables_table(sf, klass.members, klass,
                        attrs, nr_callables=nr_callables)

        if nr_callables != 0:
            sf.write('};\n')

    @staticmethod
    def _g_module_traverse(sf):
        """ Generate the module traverse slot. """

        sf.write(
'''

/* The module's traverse slot. */
static int module_traverse(PyObject *mod, visitproc visit, void *arg)
{
    return sipModuleTraverse(mod, visit, arg);
}
''')

    def _g_properties_table(self, sf, bindings, attrs, klass):
        """ Generate the properties table for a type. """

        if len(klass.properties) == 0:
            return

        klass_name = klass.iface_file.fq_cpp_name.as_word

        # Note that we can't use 'static' because of the need for the earlier
        # 'extern' forward declaration.
        sf.write(
f'''
const sipPropertySpec sipProperties_{klass_name}[] = {{
''')

        for prop in klass.properties:
            if prop.docstring is not None and bindings.docstrings:
                docstring = f'"{get_docstring_text(prop.docstring)}"'
            else:
                docstring = 'NULL'

            getter_nr = self._get_attribute_nr(attrs, prop.getter)
            getter = f'&sipAttrs_{klass_name}[{getter_nr}]'

            if prop.setter is not None:
                setter_nr = self._get_attribute_nr(attrs, prop.setter)
                setter = f'&sipAttrs_{klass_name}[{setter_nr}]'
            else:
                setter = 'NULL'

            sf.write(f'    {{{docstring}, {getter}, {setter}}},\n')

        sf.write('};\n')

    def _g_properties_table_decl(self, sf, attrs, klass):
        """ Generate the forward declaration of the properties table for a type
        and update the attributes that will be generated.
        """

        nr_props = len(klass.properties)

        if nr_props == 0:
            return

        klass_name = klass.iface_file.fq_cpp_name.as_word

        # The forward declaration is needed because the attributes table and
        # the properties table contain mutual references.
        sf.write(
f'''
extern const sipPropertySpec sipProperties_{klass_name}[];
''')

        for prop_nr, prop in enumerate(klass.properties):
            spec_name = f'&sipProperties_{klass_name}[{prop_nr}]'
            attrs.append(('p', prop.name.name, 'property', spec_name))

    def _g_type_hints_docstring(self, sf, bindings, member, overloads,
            callable_name, is_method=False):
        """ Generate any type hints and docstring. """

        if bindings.docstrings and _has_documentation(member, overloads):
            sf.write(f'static const sipDocSpec sipDocs_{callable_name}[] = {{\n')

            for overload in callable_overloads(member, overloads):
                if member.no_arg_parser:
                    sf.write('    {NULL, ')
                else:
                    sf.write('    {"')
                    g_overload_type_hint(sf, self.spec, overload,
                            is_method=is_method)
                    sf.write('", ')

                if overload.docstring is not None:
                    sf.write(f'"{_get_typed_docstring_text(overload.docstring)}"')
                else:
                    sf.write('NULL')

                sf.write('},\n')

            sf.write('    {0}\n};\n\n')

    def _g_variables_table(self, sf, attrs, static_variables, scope=None):
        """ Generate the table of variables for a scope and update the
        attributes generated.
        """

        spec = self.spec
        c_bindings = spec.c_bindings
        module = spec.module

        # Get the sorted list of variables.
        variables = list(variables_in_scope(spec, scope, check_handler=False))

        # Add the members of any anonymous enums.  Note that this would be
        # be better handled by the parser but that would require refactoring of
        # the legacy backend.
        for enum in spec.enums:
            if py_scope(enum.scope) is not scope:
                continue

            if enum.fq_cpp_name is not None:
                # Add the legacy support for members of custom enums to be
                # visible in the same scope as the enum.
                if not self.custom_enums_supported() or enum.is_scoped:
                    continue

            for member in enum.members:
                fq_cpp_name = ScopedName.parse(get_enum_member(spec, member))
                base_type = enum.enum_base_type or Argument(ArgumentType.INT)

                pseudo_var = WrappedVariable(fq_cpp_name, enum.module,
                        member.py_name, scope, base_type)

                # This is a bit of a hack.
                pseudo_var._enum_member = self._get_enum_member_value_field(
                        member, base_type=base_type)

                variables.append(pseudo_var)

        # We sort it for reproducable builds.
        variables.sort(key=lambda k: k.py_name.name)

        table = []

        for variable in variables:
            v_type = variable.type
            v_ref = variable.fq_cpp_name.as_word

            # Generally const variables cannot be set.  However for string
            # pointers the reverse is true as a const pointer can be replaced
            # by another, but we can't allow the contents of a non-const
            # string/array to be modified by C/C++ because they are immutable
            # in Python.
            not_settable = False
            need_key = False

            enum_member_value = getattr(variable, '_enum_member', None)

            if v_type.type is ArgumentType.CLASS:
                type_id = _get_type_ref(v_type.definition)

                if len(v_type.derefs) == 0:
                    need_key = True
                    not_settable = v_type.is_const

            elif v_type.type is ArgumentType.ENUM:
                type_id = _get_type_ref(v_type.definition)

                if v_type.definition.fq_cpp_name is not None:
                    not_settable = v_type.is_const

            elif v_type.type is ArgumentType.BYTE:
                type_id = 'sipType_byte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SBYTE:
                type_id = 'sipType_sbyte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.UBYTE:
                type_id = 'sipType_ubyte'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SHORT:
                type_id = 'sipType_short'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.USHORT:
                type_id = 'sipType_ushort'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.INT, ArgumentType.CINT):
                type_id = 'sipType_int'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.UINT:
                type_id = 'sipType_uint'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.LONG:
                type_id = 'sipType_long'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.ULONG:
                type_id = 'sipType_ulong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.LONGLONG:
                type_id = 'sipType_longlong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.ULONGLONG:
                type_id = 'sipType_ulonglong'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.HASH:
                type_id = 'sipType_Py_hash_t'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SSIZE:
                type_id = 'sipType_Py_ssize_t'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.SIZE:
                type_id = 'sipType_size_t'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
                type_id = 'sipType_float'
                not_settable = v_type.is_const

            elif v_type.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
                type_id = 'sipType_double'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_char'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_str'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.ASCII_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_char_ascii'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_str_ascii'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.LATIN1_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_char_latin1'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_str_latin1'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.UTF8_STRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_char_utf8'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_str_utf8'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.SSTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_schar'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_sstr'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.USTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_uchar'
                    not_settable = v_type.is_const
                else:
                    type_id = 'sipType_ustr'
                    not_settable = not v_type.is_const
                    need_key = True

            elif v_type.type is ArgumentType.WSTRING:
                if len(v_type.derefs) == 0:
                    type_id = 'sipType_wchar'
                    not_settable = v_type.is_const
                else:
                    # Note that wchar_t strings/arrays are mutable.
                    type_id = 'sipType_wstr'
                    need_key = True

            elif v_type.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
                type_id = 'sipType_bool'
                not_settable = v_type.is_const

            elif v_type.type is ArgumentType.VOID:
                # This is the only type that we need to make a distinction
                # between const and non-const (because if affects the behaviour
                # of a corresponding voidptr instance).  Using a flag
                # (potentially applicable to all types) would smell better but
                # we don't have anywhere to store it.  (SIP_WV_RO is a special
                # value rather than a flag).
                type_id = 'sipType_voidptr_const' if v_type.is_const else 'sipType_voidptr'

            elif v_type.type is ArgumentType.PYOBJECT:
                type_id = 'sipType_pyobject'

            elif v_type.type is ArgumentType.PYTUPLE:
                type_id = 'sipType_pytuple'

            elif v_type.type is ArgumentType.PYLIST:
                type_id = 'sipType_pylist'

            elif v_type.type is ArgumentType.PYDICT:
                type_id = 'sipType_pydict'

            elif v_type.type is ArgumentType.PYCALLABLE:
                type_id = 'sipType_pycallable'

            elif v_type.type is ArgumentType.PYSLICE:
                type_id = 'sipType_pyslice'

            elif v_type.type is ArgumentType.PYTYPE:
                type_id = 'sipType_pytype'

            elif v_type.type is ArgumentType.PYBUFFER:
                type_id = 'sipType_pybuffer'

            elif v_type.type is ArgumentType.CAPSULE:
                type_id = 'sipType_pycapsule'

            else:
                continue

            read_only = not_settable or variable.no_setter
            need_sip_cpp = False

            fields = ['.type_id = ' + type_id]

            flags = []
            key = 0

            if enum_member_value is not None:
                flags.append('SIP_WV_LITERAL')
                value = enum_member_value
            else:
                if read_only:
                    flags.append('SIP_WV_RO')

                if need_key:
                    key = module.next_key
                    module.next_key -= 1

                if scope is None or variable.is_static:
                    cpp_name = variable.fq_cpp_name.as_cpp
                    address = '&' + cpp_name
                else:
                    address = 'sipVariableAddrGetter_' + v_ref
                    need_sip_cpp = True

                value = f'.value = {{.ptr_t = (void *){address}}}'

            if flags:
                fields.append('.flags = ' + '|'.join(flags))

            if key:
                fields.append('.key = ' + str(key))

            fields.append(value)

            if variable.get_code is not None:
                fields.append('.get_code = sipVariableGetCode_' + v_ref)

            if variable.set_code is not None:
                fields.append('.set_code = sipVariableSetCode_' + v_ref)

            table.append((variable, ', '.join(fields)))

            # Generate any %GetCode wrapper.
            if variable.get_code is not None:
                sf.write('\n')

                addr_arg = _get_optional_arg('sipCppV', need_sip_cpp)
                type_arg = _get_optional_arg('sipPyType',
                        is_used_in_code(variable.get_code, 'sipPyType'))

                if not c_bindings:
                    sf.write(f'extern "C" {{static PyObject *sipVariableGetCode_{v_ref}(sipModuleState *, void *, PyObject *);}}\n')

                sf.write(
f'''static PyObject *sipVariableGetCode_{v_ref}(sipModuleState *sipMS, void *{addr_arg}, PyObject *{type_arg})
{{
''')

                if need_sip_cpp:
                    sf.write(f'    {get_class_from_void(spec, scope)};\n')

                sf.write(
'''    PyObject *sipPy;

''')

                sf.write_code(variable.get_code)

                sf.write(
'''
    return sipPy;
}

''')

            # Generate any %SetCode wrapper.
            if variable.set_code is not None:
                sf.write('\n')

                addr_arg = _get_optional_arg('sipCppV', need_sip_cpp)
                type_arg = _get_optional_arg('sipPyType',
                        is_used_in_code(variable.set_code, 'sipPyType'))

                if not c_bindings:
                    sf.write(f'extern "C" {{static int sipVariableSetCode_{v_ref}(sipModuleState *, void *, PyObject *, PyObject *);}}\n')

                sf.write(
f'''static int sipVariableSetCode_{v_ref}(sipModuleState *ms, void *{addr_arg}, PyObject *sipPy, PyObject *{type_arg})
{{
''')

                if need_sip_cpp:
                    sf.write(f'    {get_class_from_void(spec, scope)};\n')

                sf.write(
'''    int sipErr = 0;

''')

                sf.write_code(variable.set_code)

                sf.write(
'''
    return sipErr ? -1 : 0;
}

''')

            # See if we need a descriptor address getter.
            if scope is None or variable.is_static or enum_member_value is not None:
                continue

            cast = get_type_from_void(spec,
                    scope.iface_file.fq_cpp_name.as_cpp, 'sipCppV')

            sf.write('\n')

            if not c_bindings:
                sf.write(f'extern "C" {{static void *sipVariableAddrGetter_{v_ref}(void *);}}\n')

            sf.write(
f'''static void *sipVariableAddrGetter_{v_ref}(void *sipCppV)
{{
    return &{cast}->{variable.py_name.name};
}}
''')

        if table:
            if scope is None:
                suffix = module.py_name
            else:
                suffix = scope.iface_file.fq_cpp_name.as_word

            sf.write(
f'''
static const sipVariableSpec sipVariables_{suffix}[] = {{
''')

            for variable_nr, (variable, fields) in enumerate(table):
                sf.write(f'    {{{fields}}},\n')

                variable_name = variable.py_name.name
                spec_name = f'&sipVariables_{suffix}[{variable_nr}]'

                if scope is None or variable.is_static:
                    static_variables.append(
                            ('v', variable_name, 'variable', spec_name))
                else:
                    attrs.append(('i', variable_name, 'variable', spec_name))

            sf.write('};\n')

    def _add_type_attributes(self, attrs, scope=None):
        """ Add the type attributes of a scope to a list of attributes. """

        for needed_type in self.spec.module.needed_types:
            if needed_type.type is ArgumentType.CLASS:
                klass = needed_type.definition

                if py_scope(klass.scope) is not scope or klass.external or klass.real_class is not None:
                    continue

                py_name = str(klass.py_name)
                type_nr = klass.iface_file.type_nr

            elif needed_type.type is ArgumentType.MAPPED:
                mapped_type = needed_type.definition

                if scope is not None or mapped_type.py_name is None:
                    continue

                py_name = str(mapped_type.py_name)
                type_nr = mapped_type.iface_file.type_nr

            elif needed_type.type is ArgumentType.ENUM:
                enum = needed_type.definition

                if py_scope(enum.scope) is not scope:
                    continue

                py_name = str(enum.py_name)
                type_nr = enum.type_nr

            elif needed_type.type is ArgumentType.EXCEPTION:
                if scope is not None:
                    continue

                exception = needed_type.definition

                py_name = exception.py_name
                type_nr = exception.iface_file.type_nr

            attrs.append(('t', py_name, 'type_nr', str(type_nr)))

    @staticmethod
    def _get_attribute_nr(attrs, name):
        """ Return the index of an attribute with the given name. """

        for attr_nr, attr in enumerate(attrs):
            if attr[1] == name:
                return attr_nr

        # This should never happen.
        return -1

    def _get_enum_member_value_field(self, member, base_type=None):
        """ Return the initialisation of the value field of an enum member
        specification.
        """

        if base_type is not None:
            arg_type = base_type.type
        elif member.scope.enum_base_type is not None:
            arg_type = member.scope.enum_base_type.type
        else:
            arg_type = ArgumentType.INT

        field, cast = _ENUM_MEMBER_TYPE_MAP[arg_type]

        return f'.value = {{.{field} = static_cast<{cast}>({get_enum_member(self.spec, member)})}}'


def _g_arg_parser(backend, sf, scope, py_signature, signature_nr, ctor,
        is_method, overload):
    """ Generate the argument variables for a callable. """

    spec = backend.spec

    # If the scope is a mapped type or a namespace, then ignore it.
    if isinstance(scope, MappedType) or (isinstance(scope, WrappedClass) and scope.iface_file.type is IfaceFileType.NAMESPACE):
        scope = None

    # See if the parser handles self for methods.
    handle_self = scope is not None and overload is not None

    # See how keyword arguments are being handled.
    if overload is not None:
        kw_args = overload.kw_args
    elif ctor is not None:
        kw_args = ctor.kw_args
    else:
        kw_args = KwArgs.NONE

    # Generate the local variables that will hold the parsed arguments and
    # values returned via arguments.
    array_len_arg_nr = -1
    need_owner = False

    for arg_nr, arg in enumerate(py_signature.args):
        if arg.array is ArrayArgument.ARRAY_SIZE:
            array_len_arg_nr = arg_nr

        if arg.transfer is Transfer.TRANSFER_THIS:
            need_owner = True

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

    # Collect the argument formats and associated parameter pointers.
    formats = []
    params = []
    next_param_base = 0
    nr_required = 0

    for arg_nr, arg in enumerate(py_signature.args):
        if not arg.is_in:
            g_argument_variable(backend, sf, scope, arg, arg_nr)
            continue

        if arg.array is ArrayArgument.ARRAY_SIZE:
            continue

        if arg.default_value is None:
            nr_required += 1

        param_base = next_param_base
        arg_params = g_argument_variable(backend, sf, scope, arg, arg_nr)

        if arg.type == ArgumentType.CAPSULE:
            arg_params.insert(0, '"' + arg.definition + '"')

        next_param_base += len(arg_params)
        params.append(arg_params)

        flags = []

        if arg.array is ArrayArgument.ARRAY:
            flags.append('IS_ARRAY')

        # Get the wrapper if explicitly asked for or we are going to keep a
        # reference to.  However if it is an encoded string then we will get
        # the actual wrapper from the format character.
        if arg.get_wrapper:
            flags.append('GET_WRAPPER')
        elif arg.key is not None:
            if not (arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) and len(arg.derefs) == 1):
                flags.append('GET_WRAPPER')

        if arg.type is ArgumentType.ASCII_STRING:
            type_id = 'str_ascii' if _add_string_flags(flags, arg) else 'char_ascii'

        elif arg.type is ArgumentType.LATIN1_STRING:
            type_id = 'str_latin1' if _add_string_flags(flags, arg) else 'char_latin1'

        elif arg.type is ArgumentType.UTF8_STRING:
            type_id = 'str_utf8' if _add_string_flags(flags, arg) else 'char_utf8'

        elif arg.type is ArgumentType.STRING:
            type_id = 'str' if _add_string_flags(flags, arg) else 'char'

        elif arg.type is ArgumentType.SSTRING:
            type_id = 'sstr' if _add_string_flags(flags, arg) else 'schar'

        elif arg.type is ArgumentType.USTRING:
            type_id = 'ustr' if _add_string_flags(flags, arg) else 'uchar'

        elif arg.type is ArgumentType.WSTRING:
            type_id = 'wstr' if _add_string_flags(flags, arg) else 'wchar'

        elif arg.type is ArgumentType.ENUM:
            if arg.definition.fq_cpp_name is None:
                type_id = _get_enum_base_type_as_type_id_suffix(arg.definition)
            else:
                type_id = arg.definition.fq_cpp_name.as_word

                if arg.is_constrained:
                    flags.append('CONSTRAINED')

        elif arg.type is ArgumentType.BOOL:
            type_id = 'bool'

        elif arg.type is ArgumentType.CBOOL:
            type_id = 'bool'
            flags.append('CONSTRAINED')

        elif arg.type is ArgumentType.INT:
            type_id = 'int'

        elif arg.type is ArgumentType.CINT:
            type_id = 'int'
            flags.append('CONSTRAINED')

        elif arg.type is ArgumentType.UINT:
            type_id = 'uint'

        elif arg.type is ArgumentType.SIZE:
            type_id = 'size_t'

        elif arg.type is ArgumentType.BYTE:
            type_id = 'byte'

        elif arg.type is ArgumentType.SBYTE:
            type_id = 'sbyte'

        elif arg.type is ArgumentType.UBYTE:
            type_id = 'ubyte'

        elif arg.type is ArgumentType.SHORT:
            type_id = 'short'

        elif arg.type is ArgumentType.USHORT:
            type_id = 'ushort'

        elif arg.type is ArgumentType.LONG:
            type_id = 'long'

        elif arg.type is ArgumentType.ULONG:
            type_id = 'ulong'

        elif arg.type is ArgumentType.LONGLONG:
            type_id = 'longlong'

        elif arg.type is ArgumentType.ULONGLONG:
            type_id = 'ulonglong'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            type_id = 'voidptr'

            if not arg.disallow_none:
                flags.append('ALLOW_NONE')

        elif arg.type is ArgumentType.CAPSULE:
            # Note that the handling of capsules is different to all other
            # Python objects as they disallow None by default and need an
            # explicit /AllowNone/.
            type_id = 'pycapsule'

            if not arg.disallow_none:
                flags.append('ALLOW_NONE')

        elif arg.type is ArgumentType.FLOAT:
            type_id = 'float'

        elif arg.type is ArgumentType.CFLOAT:
            type_id = 'float'
            flags.append('CONSTRAINED')

        elif arg.type is ArgumentType.DOUBLE:
            type_id = 'double'

        elif arg.type is ArgumentType.CDOUBLE:
            type_id = 'double'
            flags.append('CONSTRAINED')

        elif arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            type_id = arg.definition.iface_file.fq_cpp_name.as_word
            _add_format_flags(flags, arg)

            if arg.transfer is Transfer.TRANSFER_THIS:
                flags.append('TRANSFER_THIS')

            if len(arg.derefs) != 0 and not arg.disallow_none:
                flags.append('ALLOW_NONE')

            if arg.is_constrained or (arg.type is ArgumentType.CLASS and arg.definition.convert_to_type_code is None):
                flags.append('CONSTRAINED')

        elif arg.type is ArgumentType.ELLIPSIS:
            type_id = 'ellipsis'

        else:
            # The remaining types should be different types of Python object.
            type_id = arg.type.name.lower()
            _add_format_flags(flags, arg)

            if arg.allow_none:
                flags.append('ALLOW_NONE')

        fields = []

        if kw_args is not KwArgs.NONE and arg.name is not None and (kw_args is KwArgs.ALL or arg.default_value is not None):
            fields.append(f'.kw_name = "{arg.name}"')

        fields.append(f'.param_base = {param_base}')
        fields.append(f'.type_id = sipType_{type_id}')

        if flags:
            fields.append(
                    '.flags = ' + '|'.join(['SIP_APFF_' + f for f in flags]))

        formats.append(', '.join(fields))

    # Generate the argument parser parameters.
    if params:
        sf.write('\n        void *sipAPParams[] = {\n')

        for arg_params in params:
            ps = ', '.join(['(void *)&' + p for p in arg_params])
            sf.write(f'            {ps},\n')

        sf.write('        };\n\n')

    # Generate the argument parser specification.
    if formats:
        formats_spec = 'sipAPFormatsSpec'

        sf.write(f'        static const sipArgParserFormatSpec {formats_spec}[] = {{\n')

        for fields in formats:
            sf.write(f'            {{{fields}}},\n')

        sf.write('        };\n\n')
    else:
        formats_spec = 'NULL'

    sf.write('        static const sipArgParserSpec sipAPSpec = {\n')

    # For C we pass the type hint as an argument because C can't (yet) handle
    # constant expressions.
    type_hint_arg = None
    type_hint_ref = 'NULL'

    if overload is None or not overload.common.no_arg_parser:
        callable_name = _get_callable_name(scope,
                None if overload is None else overload.common)

        type_hint = f'sipDocs_{callable_name}[{signature_nr}].type_hint'

        if spec.c_bindings:
            type_hint_arg = type_hint
        else:
            type_hint_ref = type_hint

    sf.write(f'            {type_hint_ref},\n')
    sf.write(f'            {formats_spec}, {len(formats)}, {nr_required},\n')

    if handle_self:
        if overload.is_static:
            oload_type = 'sipOverloadStaticMethod'
        elif overload.access_is_really_protected:
            oload_type = 'sipOverloadProtectedMethod'
        else:
            oload_type = 'sipOverloadMethod'
    else:
        oload_type = 'sipOverloadOther'

    sf.write(f'            {oload_type}\n        }};\n\n')

    # Generate the call to the parser function.
    args = [
        'sipMS',
        'sipPStateP',
        'sipArgs',
        'sipNrArgs',
        'sipKwdNames',
        '&sipAPSpec',
        'sipAPParams' if params else 'SIP_NULLPTR',
    ]

    if ctor is not None:
        args.append('sipSelf')
        args.append('sipUnused')
        parser_name = 'sipParseCtorArgs'
    else:
        args.append('&sipSelf')

        if scope is None or overload.is_static:
            args.append('sipType_Invalid')
            args.append('SIP_NULLPTR')
        else:
            args.append(_get_type_ref(scope))
            args.append('(void **)&sipCpp')

        if type_hint_arg is None:
            parser_name = 'sipParseArgs'
        else:
            parser_name = 'sipParseArgsC'
            args.append(type_hint_arg)

    args = ', '.join(args)

    sf.write(f'        if ({parser_name}({args}))\n')


def _g_callables_table(sf, members, scope, attrs, nr_callables=0):
    """ Generate a table of callable specs for a list of functions and update
    the attributes generated.  An attribute is a 3-tuple of the type, name and
    specification reference.
    """

    if isinstance(scope, Module):
        default_attr_type = 'm'
        scope_name = scope.py_name
    else:
        default_attr_type = 'c'

        if isinstance(scope, WrappedEnum):
            scope_name = scope.fq_cpp_name
        else:
            scope_name = scope.iface_file.fq_cpp_name.as_word

    overloads = scope.overloads

    for member_nr, member in enumerate(members):
        callable_name = _get_callable_name(scope, member)

        if nr_callables == 0:
            sf.write(f'\nstatic const sipCallableSpec sipCallables_{scope_name}[] = {{\n')

        has_docs = _has_documentation(member, overloads)
        docs_ref = get_optional_ptr(has_docs, 'sipDocs_' + callable_name)

        sf.write(f'    {{callable_{callable_name}, {docs_ref}}},\n')

        if member.py_slot is None:
            attr_type = default_attr_type
        else:
            attr_type = 'e' if is_extendable_slot(member.py_slot) else 's'

        spec_name = f'&sipCallables_{scope_name}[{nr_callables}]'
        nr_callables += 1

        attrs.append((attr_type, member.py_name.name, 'callable', spec_name))

    return nr_callables


def _g_class_method_table(sf, attrs, klass):
    """ Generate the table of methods for a class and update the attributes
    generated.
    """

    if klass.iface_file.type is IfaceFileType.NAMESPACE:
        members = get_function_table(klass.members)
    else:
        members = get_method_table(klass)

    _g_py_method_table(sf, members, klass, attrs)


def _g_py_method_table(sf, members, scope, attrs):
    """ Generate a Python method table for a class or mapped type and update
    the attributes generated.
    """

    if _g_callables_table(sf, members, scope, attrs) != 0:
        sf.write('};\n')


def _add_format_flags(flags, arg):
    """ Add some common format flags for an argument. """

    if arg.transfer is Transfer.TRANSFER:
        flags.append('TRANSFER')

    if arg.transfer is Transfer.TRANSFER_BACK:
        flags.append('TRANSFER_BACK')


def _add_string_flags(flags, arg):
    """ Add the format flags for a string-like argument and return True if it
    was a string and False if it was a single character.
    """

    if is_string(arg):
        # None is allowed by default.
        if not arg.disallow_none:
            flags.append('ALLOW_NONE')

        return True

    return False


def _get_callable_name(scope, member):
    """ Return the name of a callable to be embedded in generated
    specifications.
    """

    # See if it is a ctor.
    if member is None:
        return scope.iface_file.fq_cpp_name.as_word

    member_name = member.py_name.name

    # See if it is an unscoped callable.  Note that we include mapped types and
    # namespaces because the argument parser does.
    if scope is None or isinstance(scope, (Module, MappedType)) or (isinstance(scope, WrappedClass) and scope.iface_file.type is IfaceFileType.NAMESPACE):
        return member_name

    if isinstance(scope, WrappedEnum):
        scope_name = scope.fq_cpp_name
    else:
        scope_name = scope.iface_file.fq_cpp_name

    return scope_name.as_word + '_' + member_name


def _get_class_flags(module, klass, py_debug):
    """ Return the flags for a class. """

    flags = []

    if klass.is_abstract:
        flags.append('SIP_TYPE_ABSTRACT')

    if klass.subclass_base is not None:
        flags.append('SIP_TYPE_SCC')

    if klass.handles_none:
        flags.append('SIP_TYPE_ALLOW_NONE')

    if module.call_super_init:
        flags.append('SIP_TYPE_SUPER_INIT')

    flags.append('SIP_TYPE_NAMESPACE' if klass.iface_file.type is IfaceFileType.NAMESPACE else 'SIP_TYPE_CLASS')

    return '|'.join(flags)


def _get_enum_base_type_as_type_id_suffix(enum):
    """ Return the type ID suffix for an enum base type. """

    if enum.enum_base_type is None:
        return 'int'

    return enum.enum_base_type.type.name.lower().replace('string', 'byte')


def _get_optional_arg(name, is_needed):
    """ Return the name of an argument to be used in a function definition that
    may or may not be needed.
    """

    return name if is_needed else f'Py_UNUSED({name})'


def _get_type_ref(wrapped_object):
    """ Return the reference to the type of a wrapped object. """

    fq_cpp_name = wrapped_object.fq_cpp_name if isinstance(wrapped_object, WrappedEnum) else wrapped_object.iface_file.fq_cpp_name

    return 'sipType_' + fq_cpp_name.as_word


def _get_typed_docstring_text(docstring):
    """ Return the text of a docstring with a prefix that determines what to
    do with the related signature.
    """

    if docstring.signature is DocstringSignature.APPENDED:
        sig_action = 'a'
    elif docstring.signature is DocstringSignature.PREPENDED:
        sig_action = 'p'
    else:
        sig_action = 'd'

    text = get_docstring_text(docstring)

    return sig_action + get_docstring_text(docstring)


def _has_documentation(member, overloads):
    """ Return true if a member has either type hints or the member has a user
    supplied docstring.
    """

    type_hints = False
    user_docstring = False

    for overload in callable_overloads(member, overloads):
        type_hints = True

        if overload.docstring is not None:
            user_docstring = True
            break

    if member.no_arg_parser:
        type_hints = False

    return type_hints or user_docstring


# The mapping of an enum base type to the details needed to initialise a member
# specification.
_ENUM_MEMBER_TYPE_MAP = {
    ArgumentType.BOOL: ('bool_t', 'bool'),
    ArgumentType.BYTE: ('byte_t', 'char'),
    ArgumentType.STRING: ('byte_t', 'char'),
    ArgumentType.SBYTE: ('sbyte_t', 'signed char'),
    ArgumentType.SSTRING: ('sbyte_t', 'signed char'),
    ArgumentType.UBYTE: ('ubyte_t', 'unsigned char'),
    ArgumentType.USTRING: ('ubyte_t', 'unsigned char'),
    ArgumentType.SHORT: ('short_t', 'short'),
    ArgumentType.USHORT: ('ushort_t', 'unsigned short'),
    ArgumentType.INT: ('int_t', 'int'),
    ArgumentType.UINT: ('uint_t', 'unsigned'),
    ArgumentType.LONG: ('long_t', 'long'),
    ArgumentType.ULONG: ('ulong_t', 'unsigned long'),
    ArgumentType.LONGLONG: ('longlong_t', 'long long'),
    ArgumentType.ULONGLONG: ('ulonglong_t', 'unsigned long long'),
}


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
