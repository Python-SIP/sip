.. _ref-plugin-api:

.. py:module:: sipbuild.plugin
    :synopsis: The SIP plugin API.


:py:mod:`sipbuild.plugin` Module Reference
==========================================

The :py:mod:`sipbuild.plugin` module implements the SIP plugin API.  This API
allows bindings authors to add functionality to SIP (for example new
:ref:`directives <ref-directives>` and :ref:`annotations <ref-annotations>`)
and to generate additional code to meet the needs of a particular use case.

The API is not intended to be comprehensive but it will be added to as required
to meet the needs of new use cases.  If you, as a bindings author, need
additions to be made then `raise an issue
<https://github.com/Python-SIP/sip/issues>`__ specifying the changes you would
like to see.

The API consists of the :py:class:`~sipbuild.plugin.Plugin` class whose methods
you can reimplement to add new behaviours.  There are additional classes that
model various SIP objects such as :py:class:`~sipbuild.plugin.Class` and
:py:class:`~sipbuild.plugin.MappedType`.

The simplest way to add a plugin to a project is to create a ``project.py``
file containing a sub-class of :py:class:`~sipbuild.Project` and the
implementation of the plugin (which will be a sub-class of
:py:class:`~sipbuild.plugin.Plugin`.  In the ``__init__`` method of the project
sub-class call :py:meth:`~sipbuild.Project.install_plugin` passing an instance
of the plugin.

If you want other bindings authors to be able to extend your own bindings then
it gets a little more complicated as they need to sub-class from your
:py:class:`~sipbuild.Project` sub-class.  To do this create a separate Python
package (uploaded to PyPI) that contains your :py:class:`~sipbuild.Project`
sub-class and your plugin implementation.  In your ``pyproject.toml`` file add
the package as a dependency using the ``requires`` value in the
``[build-system]`` section and specify the full import path of the
:py:class:`~sipbuild.Project` sub-class as the value of ``project-factory`` in
the ``[tool.sip]`` section.  There is no need for a ``project.py`` file.


.. py:class:: AnnotationContext

    An enum that specifies the context of annotations passed to
    :py:meth:`~sipbuild.plugin.Plugin.sip_handle_annotations`.

    .. py:attribute:: ARGUMENT

        The annotations relate to an argument.

    .. py:attribute:: CLASS

        The annotations relate to a class.

    .. py:attribute:: CONSTRUCTOR

        The annotations relate to a constructor.

    .. py:attribute:: DESTRUCTOR

        The annotations relate to a destructor.

    .. py:attribute:: ENUM

        The annotations relate to an enum.

    .. py:attribute:: ENUM_MEMBER

        The annotations relate to a member of an enum.

    .. py:attribute:: EXCEPTION

        The annotations relate to an exception.

    .. py:attribute:: FUNCTION

        The annotations relate to a function or overload.

    .. py:attribute:: MAPPED_TYPE

        The annotations relate to a mapped type.

    .. py:attribute:: TYPEDEF

        The annotations relate to a typedef.

    .. py:attribute:: VARIABLE

        The annotations relate to a variable.


.. py:attribute:: AnnotationNoValue

    A value that is return by
    :py:meth:`~sipbuild.plugin.Specification.parse_annotation` to indicate that
    an annotation was specified but without a value.


.. py:class:: AnnotationType

    An enum that specifies the data type of an annotation passed to
    :py:meth:`~sipbuild.plugin.Specification.parse_annotation`.

    .. py:attribute:: BOOLEAN

        The value is ``True`` if the annotation is specified and ``False`` if
        not.

    .. py:attribute:: DOTTED_NAME

        The value is a ``str`` compatible with an identifier preceded by a
        Python scope.

    .. py:attribute:: DOTTED_NAME_OPTIONAL

        The value is a ``str`` compatible with an identifier preceded by a
        Python scope or :py:attr:`~sipbuild.plugin.AnnotationNoValue` if the
        value part was omitted.

    .. py:attribute:: INTEGER

        The value is a signed ``int``.

    .. py:attribute:: INTEGER_OPTIONAL

        The value is a signed ``int`` or
        :py:attr:`~sipbuild.plugin.AnnotationNoValue` if the value part was
        omitted.

    .. py:attribute:: NAME

        The value is a ``str`` compatible with a C/C++ identifier.

    .. py:attribute:: NAME_OPTIONAL

        The value is a ``str`` compatible with a C/C++ identifier or
        :py:attr:`~sipbuild.plugin.AnnotationNoValue` if the value part was
        omitted.

    .. py:attribute:: STRING

        The value is a ``str`` extracted from a quoted string with embedded
        selectors or ``None`` if no selector evaluated to ``True``.

    .. py:attribute:: STRING_LIST

        The value is a ``list[str]`` extracted from a quoted string.

    .. py:attribute:: STRING_OPTIONAL

        The value is a ``str`` extracted from a quoted string with embedded
        selectors, ``None`` if no selector evaluated to ``True`` or
        :py:attr:`~sipbuild.plugin.AnnotationNoValue` if the value part was
        omitted.


.. py:class:: Argument

    This represents an argument.  Instances are created by the API and should
    not be created by a plugin.

    .. py:method:: get_declaration(*, nr_of_scopes_stripped=None)

        Get the declaration of the argument as a string.

        :param int nr_of_scopes_stripped: is the number of scopes that will be
            be removed from the arguments type.  By default only the global
            scope (ie. the leading ``::``) is stripped.
        :return: the declaration.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:property:: is_const

        ``True`` if the argument is ``const``.

    .. py:property:: is_optional

        ``True`` if the argument is optional (i.e. has a default value).  It
        cannot be set.

    .. py:property:: is_reference

        ``True`` if the argument is a C++ reference.

    .. py:property:: nr_of_dereferences

        The number of pointer dereferences.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: Class

    This represents a class.  Instances are created by the API and should not
    be created by a plugin.

    .. py:property:: cpp_name

        The fully scoped C++ name of the class.  It cannot be set.

    .. py:property:: cpp_name_as_word

        The fully scoped C++ name of the class transformed so that it can be
        used as a C++ identifier.  It cannot be set.

    .. py:property:: dtor_is_public

        ``True`` if the class has a public destructor.  It cannot be set.

    .. py:property:: functions

        A sequence of :py:class:`~sipbuild.plugin.Function` instances of the
        class's functions.  Note that this is based on the Python name and not
        the C++ name.  It cannot be set.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:property:: needs_derived_class

        ``True`` if the class needs a :ref:`derived class
        <ref-derived-classes>` to be generated.

    .. py:property:: py_name_as_str

        The Python name of the class as a string literal (i.e. enclosed in
        double quotes).  It cannot be set.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.

    .. py:property:: unscoped_cpp_name

        The unscoped C++ name of the class.  It cannot be set.


.. py:class:: Constructor

    This represents a constructor.  Instances are created by the API and should
    not be created by a plugin.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: Enum

    This represents an enum.  Instances are created by the API and should not
    be created by a plugin.

    .. py:property:: cpp_name

        The fully scoped C/C++ name of the enum.  It will be ``None`` if the
        enum is anonymous.  It cannot be set.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:property:: is_public

        ``True`` if the enum is public.  It cannot be set.

    .. py:property:: scope

        The enclosing scope of the enum or ``None`` if it is global.  It cannot
        be set.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: EnumMember

    This represents a member of an enum.  Instances are created by the API and
    should not be created by a plugin.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: Function

    This represents a function (i.e a collection of overloads which have the
    same Python name).  Instances are created by the API and should not be
    created by a plugin.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:property:: impl_reference

        An ABI-specific reference to the implementation of a function.  It
        cannot be set.

    .. py:property:: overloads

        A sequence of :py:class:`~sipbuild.plugin.Overload` instances of the
        function's overloads.  Note that this is based on the Python name and
        not the C++ name.  It cannot be set.

    .. py:property:: py_name_as_str

        The Python name of the class as a string literal (i.e. enclosed in
        double quotes).  It cannot be set.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: MappedType

    This represents a mapped type.  Instances are created by the API and should
    not be created by a plugin.

    .. py:property:: cpp_name

        The C++ name of the mapped type.  It cannot be set.

    .. py:property:: cpp_name_as_word

        The C++ name of the mapped type transformed so that it can be used as
        a C++ identifier.  It cannot be set.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: Overload

    This represents a function overload.  Instances are created by the API and
    should not be created by a plugin.

    .. py:property:: cpp_signature

        The overload's C/C++ signature.  It cannot be set.

    .. py:property:: docstring

        The overload's docstring.  It is ``None`` if the
        :directive:`%Docstring` directive wasn't specified.  It cannot be set.

    .. py:method:: generate_signature_parser(writer, *, overload_nr, simple_call=False)

        Generate the code to parse the overload's signature.

        :param writer: is an object with a ``write()`` method.
        :param int overload_nr: is the index of the overload in the sequence
            of the function's *callable* overloads.
        :param bool simple_call: is ``True`` in order to generate a simple
            call, i.e. one that doesn't allow ``self`` being passed as the
            first argument and doesn't support keyword arguments.

    .. py:method:: generate_call_arguments(writer)

        Generate the arguments to a call that were produced from a parsed
        signature.

        :param writer: is an object with a ``write()`` method.

    .. py:method:: generate_signature_parser_tidy_up(writer)

        Generate the code to tidy up after a signature parser.

        :param writer: is an object with a ``write()`` method.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:property:: is_callable

        ``True`` if the overload is callable.  Overloads are callable by
        default.

    .. py:property:: is_public

        ``True`` if the overload is public.  It cannot be set.

    .. py:property:: is_static

        ``True`` if the overload is ``static``.  It cannot be set.

    .. py:property:: py_name

        The Python name of the overload.  It cannot be set.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.

    .. py:property:: type_hint

        The overload's type hint.  It cannot be set.

    .. py:property:: unscoped_cpp_name

        The unscoped C/C++ name of the overload.  It cannot be set.


.. py:class:: Plugin

    This is the base class for plugins.  Its methods are called at various
    points in the bindings generation process and sub-classes can reimplement
    these methods to add new features.

    If multiple plugins are installed then each will be called.  If a plugin
    method returns a value then SIP will check that those values do not
    conflict.

    The prefix ``sip_`` in the names of method and attribute names is reserved.

    .. py:attribute:: sip_key

        A string that uniquely identifies the plugin.  A value based on the
        PyPI name is recommended.

    .. py:method:: sip_class_generate_derived_class_decl(writer, spec, klass)

        Generate code to be included in the declaration of a class's
        :ref:`derived class <ref-derived-classes>`.  By default this will be
        placed in a private section.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.
        :param Class klass: is the class.

    .. py:method:: sip_class_generate_derived_class_impl(writer, spec, klass)

        Generate code to be included in the implementation of a class's
        :ref:`derived class <ref-derived-classes>`.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.
        :param Class klass: is the class.

    .. py:method:: sip_class_generate_impl(writer, spec, klass)

        Generate code that implements support for a class.  The name of an
        optional plugin-specific data structure can be returned.  This is used
        by ABIs v12 and v13 but ignored by v14.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.
        :param Class klass: is the class.
        :return: the name of a plugin-specific data structure or ``None``.

    .. py:method:: sip_class_generate_release_code(writer, spec, klass)

        Generate the body of a class's release function.  ``True`` is returned
        if the plugin generated something to replace the default
        implementation.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.
        :param Class klass: is the class.
        :return: ``True`` if something was generated.

    .. py:method:: sip_class_get_access_specifiers(spec)

        Return a sequence of non-standard strings that can appear in a
        plugin-specific access specifier.

        :param Specification spec: is the specification.
        :return: the sequence of access specifiers.

    .. py:method:: sip_class_parsed(spec, scope, klass)

        This is called when a class has been parsed.

        :param Specification spec: is the specification.
        :param Class|None scope: is the enclosing scope of the class.
        :param Class klass: is the class.

    .. py:method:: sip_class_superclass_set(spec, klass, superclass)

        This called when a class's superclass has been set.

        :param Specification spec: is the specification.
        :param Class klass: is the class.
        :param Class superclass: is the class's superclass.

    .. py:method:: sip_handle_access_specifier(spec, klass, primary, secondary)

        Parse a primary and optional secondary class access specifier.  If it
        was parsed return the C++ standard access specifier (ie. ``public``,
        ``protected`` or ``private``) to use, otherwise return ``None``.

        :param Specification spec: is the specification.
        :param Class klass: is the class.
        :param str primary: is the primary access specifier.
        :param str|None secondary: is the optional secondary access specifier.
        :return: the standard access specifier or ``None``.

    .. py:method:: sip_handle_annotations(spec, item, annotations, annotation_context)

        Handle any context-specific annotations.  All supported annotations
        must be handled.

        :param Specification spec: is the specification.
        :param item: is the item that is annotated.
        :param annotations: is an opaque annotations object that can be passed
            to :py:meth:`~sipbuild.plugin.Specification.parse_annotation`.
        :param AnnotationContext annotation_context: is the context of the
            annotations (implying the type of **item**).

    .. py:method:: sip_handle_class_function_prefix(spec, overload, prefix)

        This is called to handle a class function prefix.

        :param Specification spec: is the specification.
        :param Overload overload: is the overload.
        :param str prefix: is the prefix.
        :return: ``True`` if the prefix was handled.

    .. py:method:: sip_handle_directive(spec, name, args)

        This is called to handle a directive.

        :param Specification spec: is the specification.
        :param str name: is the name of the directive.
        :param list[str] args: is a list of the directive's arguments.
        :return: ``True`` if the directive was handled.

    .. py:method:: sip_instantiated_class_resolved(spec, klass, template_class, template_arguments)

        This is called when a class created by instantiating a template with
        arguments has been resolved.

        :param Specification spec: is the specification.
        :param Class klass: is the class.
        :param Class template_class: is the prototype template class.
        :param dict[str, str] template_arguments: is the dictionary of
            name/value pairs that are the template arguments.

    .. py:method:: sip_instantiated_mapped_type_resolved(spec, mapped_type, template_mapped_type, template_arguments)

        This is called when a mapped type created by instantiating a template
        with arguments has been resolved.

        :param Specification spec: is the specification.
        :param MappedType mapped_type: is the mapped type.
        :param MappedType template_mapped_type: is the prototype template
            mapped type.
        :param dict[str, str] template_arguments: is the dictionary of
            name/value pairs that are the template arguments.

    .. py:method:: sip_mapped_type_generate_impl(writer, spec, mapped_type)

        Generate code that implements support for a mapped type.  The name of
        an optional plugin-specific data structure can be returned.  This is
        used by ABIs v12 and v13 but ignored by v14.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.
        :param MappedType mapped_type: is the mapped type.
        :return: the name of a plugin-specific data structure or ``None``.

    .. py:method:: sip_module_generate_code(writer, spec)

        Generate code to declare and/or initialise any module-specific data.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.

    .. py:method:: sip_module_generate_header_code(writer, spec)

        Generate code to be included in the internal module API .h file.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.

    .. py:method:: sip_module_generate_initialisation_code(writer, spec)

        Generate code included in the module initialisation code.

        :param writer: is an object with a ``write()`` method.
        :param Specification spec: is the specification.

    .. py:method:: sip_overload_resolved(spec, scope, overload)

        This is called when an overload has been resolved.

        :param Specification spec: is the specification.
        :param Class|MappedType|None scope: is the enclosing scope of the
            overload.
        :param Overload overload: is the overload.

    .. py:method:: sip_overload_get_prefixes(spec)

        Return a sequence of non-standard strings that can appear in a
        plugin-specific overload prefix.

        :param Specification spec: is the specification.
        :return: the sequence of prefixes.

    .. py:method:: sip_overload_get_typing(spec, overload)

        Return a 2-tuple of the typing entry for an overload to be included in
        the module's .pyi file and a flag that is ``True`` if other overloads
        of the same function should be ignored.  If ``None`` is returned then a
        default is used.

        :param Specification spec: is the specification.
        :param Overload overload: is the overload.
        :return: the 2-tuple of typing entry and flag.

    .. py:method:: sip_overload_parsed(spec, scope, overload)

        This is called when an overload has been parsed.

        :param Specification spec: is the specification.
        :param Class|MappedType|None scope: is the enclosing scope of the
            overload.
        :param Overload overload: is the overload.

    .. py:method:: sip_specification_resolved(spec)

        This is called when a specification has been resolved.

        :param Specification spec: is the specification.


.. py:class:: Signature

    This represents an overload's signature.  Instances are created by the API
    and should not be created by a plugin.

    .. py:property:: arguments

        A sequence of :py:class:`~sipbuild.plugin.Argument` instances of the
        signature's arguments.  It cannot be set.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.


.. py:class:: Specification

    This represents a complete specification of a module and its contents.
    Instances are created by the API and should not be created by a plugin.

    .. py:property:: abi_major_version

        The major version number of the target ABI.  It cannot be set.

    .. py:property:: abi_minor_version

        The minor version number of the target ABI.  It cannot be set.

    .. py:method:: add_declaration(klass)

        The header file that declares a class is added to those #included by
        the generated module code.

        :param Class klass: is the class.

    .. py:property:: all_enums

        The sequence of all enums defined in the module.  It cannot be set.

    .. py:property:: all_mapped_types

        The sequence of all mapped types defined in the module.  It cannot be
        set.

    .. py:method:: apply_template_arguments(proto_string, template_arguments)
        :staticmethod:

        Applies the arguments of a template to a prototype string and return
        the expanded result.

        :param str proto_string: is the prototype string.
        :param dict[str, str] template_arguments: is the dictionary of
            name/value pairs that are the template arguments.
        :return: the expanded result.

    .. py:property:: c_bindings

        ``True`` if C (rather than C++) bindings are being generated.  It
        cannot be set.

    .. py:property:: docstrings

        ``True`` if docstrings should be generated.  It cannot be set.

    .. py:method:: error(message, *, annotation=None, overload=None)

        Report an error, possibly related to a particular context.

        :param str message: is the text of the error message.
        :param str annotation: is the name of an annotation to be passed if the
            error occurred in an implementation of
            :py:meth:`~sipbuild.Plugin.sip_handle_annotations`.
        :param Overload overload: is the overload to be passed if the error
            occurred in an implementation of
            :py:meth:`~sipbuild.Plugin.sip_overload_resolved`.

    .. py:method:: get_plugin_attribute(plugin, name, default=None)

        Get the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param default: is the optional default value of the attribute.
        :return: the value of the attribute.

    .. py:method:: get_type_specification(wrapped_object)

        Get the name of the :ref:`type specification <ref-type-specs>` of a
        wrapped object.

        :param Class|Enum|MappedType wrapped_object: is the wrapped object.
        :return: the type specification.

    .. py:method:: is_class(wrapped_object)
        :staticmethod:

        Check if a wrapped object is a class.

        :param wrapped_object: is the wrapped object.
        :return: ``True`` if the wrapped object is a class.

    .. py:method:: is_namespace(wrapped_object)
        :staticmethod:

        Check if a wrapped object is a namespace.

        :param wrapped_object: is the wrapped object.
        :return: ``True`` if the wrapped object is a namespace.

    .. py:property:: module_name

        The name of the module.  It cannot be set.

    .. py:method:: parse_annotation(annotations, name, annotation_type)
        :staticmethod:

        Parse an annotation of a particular type and return its value.

        :param annotations: is an opaque annotations object passed to
            :py:meth:`~sipbuild.plugin.Plugin.sip_handle_annotations`.
        :param str name: is the name of the annotation.
        :param AnnotationType annotation_type: is the expected type of the
            annotation's value.
        :return: the value of the annotation or ``None`` if it wasn't
            specified.

    .. py:method:: set_plugin_attribute(plugin, name, value)

        Set the value of a plugin-specific attribute.

        :param Plugin plugin: is the plugin.
        :param str name: is the name of the attribute.
        :param value: is the value to set.
