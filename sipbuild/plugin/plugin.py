# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


class Plugin:
    """ The base class of a plugin.  It provides a mechanism for a bindings
    author to extend SIP by, for example, adding bespoke annotations and
    directives.  The prefix 'sip_' is reserved in method and attribute names.
    """

    # A string that uniquely identifies the plugin.  A value based on the PyPI
    # name is recommended.
    sip_key = None

    def sip_class_generate_derived_class_decl(self, writer, spec, klass):
        """ Generate code to be included in the declaration of a class's
        derived class.  By default this will be placed in a private section.
        """

        pass

    def sip_class_generate_derived_class_impl(self, writer, spec, klass):
        """ Generate code to be included in the implementation of a class's
        derived class.
        """

        pass

    def sip_class_generate_impl(self, writer, spec, klass):
        """ Generate code that implements support for a class.  The name
        of a plugin data structure can be returned.  This is used by ABIs v12
        and v13 but ignored by v14.
        """

        return None

    def sip_class_generate_release_code(self, writer, spec, klass):
        """ Generate the body of a class's release function.  True is returned
        if the plugin generated something to replace the default
        implementation.
        """

        return False

    def sip_class_get_access_specifiers(self):
        """ Return a sequence of non-standard strings that can appear in a
        plugin-specific access specifier.
        """

        return ()

    def sip_class_parsed(self, spec, scope, klass):
        """ Invoked when a class has been parsed. """

        pass

    def sip_class_superclass_set(self, spec, klass, superclass):
        """ Invoked when a class's superclass has been set. """

        pass

    def sip_handle_access_specifier(self, spec, klass, primary, secondary):
        """ Parse a primary and optional secondary class access specifier.  If
        it was parsed return the C++ standard access specifier (ie. 'public',
        'protected' or 'private') to use, otherwise return None.
        """

        return None

    def sip_handle_annotations(self, spec, item, annotations,
            annotation_context):
        """ Handle any context-specific annotations.  All supported annotations
        must be handled.
        """

        pass

    def sip_handle_class_function_prefix(self, spec, overload, prefix):
        """ Invoked to handle a class function prefix.  Return True if the
        prefix was handled.
        """

        return False

    def sip_handle_directive(self, spec, name, args):
        """ Invoked to handle a directive.  Return True if the directive was
        handled.
        """

        return False

    def sip_instantiated_class_resolved(self, spec, klass, template_class,
            template_arguments):
        """ Invoked when a class created by instantiating a template with
        arguments has been resolved.
        """

        pass

    def sip_instantiated_mapped_type_resolved(self, spec, mapped_type,
            template_mapped_type, template_arguments):
        """ Invoked when a mapped type created by instantiating a template with
        arguments has been resolved.
        """

        pass

    def sip_mapped_type_generate_impl(self, writer, spec, mapped_type):
        """ Generate code that implements support for a mapped type.  The name
        of a plugin data structure can be returned.  This is used by ABIs v12
        and v13 but ignored by v14.
        """

        return None

    def sip_module_generate_code(self, writer, spec):
        """ Generate code to declare and/or initialise any module-specific
        data.
        """

        pass

    def sip_module_generate_header_code(self, writer, spec):
        """ Generate code to be included in the internal module API .h file.
        """

        pass

    def sip_module_generate_initialisation_code(self, writer, spec):
        """ Generate code included in the module initialisation code. """

        pass

    def sip_overload_resolved(self, spec, scope, overload):
        """ Invoked when an overload (in an optional scope) has been resolved.
        """

        pass

    def sip_overload_get_prefixes(self, spec):
        """ Return a sequence of non-standard strings that can appear in a
        plugin-specific overload prefix.
        """

        return ()

    def sip_overload_get_typing(self, spec, overload):
        """ Return a 2-tuple of the typing entry for an overload to be included
        in the module's '.pyi' file and a flag that is True if other overloads
        of the same function should be ignored.  If None is returned then a
        default is used.
        """
 
        return None, False

    def sip_overload_parsed(self, spec, scope, overload):
        """ Invoked when an overload (in an optional scope) has been parsed.
        """

        pass

    def sip_specification_resolved(self, spec):
        """ Invoked when a specification has been resolved. """

        pass

    # The XML methods are undocumented as they support the (currently
    # undocumented) documentation system.

    def sip_xml_class_add_attributes(self, attributes, klass):
        """ Add any class related attributes. """

        pass

    def sip_xml_overload_add_attributes(self, attributes, overload):
        """ Add any overload related attributes. """

        pass
