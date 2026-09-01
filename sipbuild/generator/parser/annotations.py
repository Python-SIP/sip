# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from collections import namedtuple

from ...plugin import (AnnotationNoValue, AnnotationType,
        Argument as PluginArgument, Class, Constructor as PluginConstructor,
        Enum as PluginEnum, EnumMember, MappedType as PluginMappedType,
        Overload as PluginOverload, Specification)

from ..specification import (Argument, Constructor, MappedType, Overload,
        WrappedClass, WrappedEnum, WrappedEnumMember)


# An encapsulation of an annotation value and its context.
Annotation = namedtuple('Annotation', ('production', 'symbol', 'value'))


class Annotations(dict):
    """ Encapsulate a collection of annotations. """

    def pop(self, name):
        """ A reimplementation of dict.pop() that validates the value. """

        # Note that an unknown name is an internal error.
        return self.validate(name, super().pop(name, None),
                _ANNOTATION_TYPES[name])

    def run_plugins(self, pm, item, annotation_context):
        """ Invoke any plugins for any remaining annotations. """

        # Handle the trivial case.
        if not self:
            return

        spec = pm.spec
        plugins = spec.bindings.project.plugins

        if plugins:
            plugin_spec = Specification(spec)

            if isinstance(item, Argument):
                # It should be safe to pass a 'None' scope as we are not
                # generating code at this stage.
                plugin_item = PluginArgument(item, None, spec)
            elif isinstance(item, Constructor):
                plugin_item = PluginConstructor(item)
            elif isinstance(item, MappedType):
                plugin_item = PluginMappedType(item, spec)
            elif isinstance(item, Overload):
                # It should be safe to pass a 'None' scope as we are not
                # generating code at this stage.
                plugin_item = PluginOverload(item, None, spec)
            elif isinstance(item, WrappedClass):
                plugin_item = Class(item, spec)
            elif isinstance(item, WrappedEnum):
                plugin_item = PluginEnum(item, spec)
            elif isinstance(item, WrappedEnumMember):
                plugin_item = EnumMember(item)

            for plugin in plugins:
                plugin.sip_handle_annotations(plugin_spec, plugin_item, self,
                        annotation_context)

        # Any remaining annotations are an error.
        for name, anno in self.items():
            _error(anno, name, "unknown annotation")

    @staticmethod
    def validate(name, anno, annotation_type):
        """ Validate an annotation and return the validated value. """

        match annotation_type:
            case AnnotationType.BOOLEAN:
                return _validate_boolean(name, anno)

            case AnnotationType.DOTTED_NAME:
                return _validate_name(name, anno, allow_dots=True)

            case AnnotationType.DOTTED_NAME_OPTIONAL:
                return _validate_name(name, anno, allow_dots=True,
                        optional=True)

            case AnnotationType.INTEGER:
                return _validate_integer(name, anno)

            case AnnotationType.INTEGER_OPTIONAL:
                return _validate_integer(name, anno, optional=True)

            case AnnotationType.NAME:
                return _validate_name(name, anno)

            case AnnotationType.NAME_OPTIONAL:
                return _validate_name(name, anno, optional=True)

            case AnnotationType.STRING:
                return _validate_string(name, anno)

            case AnnotationType.STRING_LIST:
                return _validate_string_list(name, anno)

            case AnnotationType.STRING_OPTIONAL:
                return _validate_string(name, anno, optional=True)

        _error(anno, name, f"unknown annotation type {annotation_type}")


class DottedName(str):
    """ Encapsulate a dotted name.  A dedicated type is used (rather than a
    str) because we need to be able to distinguish it from a quoted string when
    used as the value of an annotation.
    """

    pass


def _error(anno, name, message):
    """ Handle an annotation error. """

    p = anno.production

    p.parser.pm.parser_error(p, anno.symbol, f"'{name}' {message}")


def _required_error(anno, name):
    """ Handle an error for a missing annotation. """

    _error(anno, name, "must have a value")


def _validate_boolean(name, anno):
    """ Return a valid boolean value. """

    if anno is None:
        return False

    if anno.value is None:
        return True

    _error(anno, name, "cannot have a value")
    return False


def _validate_integer(name, anno, *, optional=False):
    """ Return a valid, possibly optional, integer. """

    if anno is None:
        return None

    value = anno.value

    if value is None:
        if optional:
            return AnnotationNoValue

        _required_error(anno, name)
        return 0

    if not isinstance(value, int):
        _error(anno, name, "must be an integer")
        return 0

    return value


def _validate_name(name, anno, *, allow_dots=False, optional=False):
    """ Return a valid, possibly optional, possibly dotted name. """

    if anno is None:
        return None

    value = anno.value

    if value is None:
        if optional:
            return AnnotationNoValue

        _required_error(anno, name)
        return ''

    if not isinstance(value, DottedName):
        _error(anno, name, "must be an unquoted name")
        return ''

    if '.' in value and not allow_dots:
        _error(anno, name, "cannot contain '.'")
        return ''

    return value


def _validate_string(name, anno, *, optional=False):
    """ Return a valid, possibly optional, string value. """

    if anno is None:
        return None

    value = anno.value

    if value is None:
        if optional:
            return AnnotationNoValue

        _required_error(anno, name)
        return ''

    if not isinstance(value, str):
        _error(anno, name, "must be a quoted string")
        return ''

    # Handle any embedded selectors.
    fields = []

    # Break into a list of ';' separated fields allowing for escaped ';'s.
    field = ''
    needs_appending = True
    for ch in value:
        if ch == ';':
            if field and field[-1] == '\\':
                # Remove the escape.
                field = field[:-1]
            else:
                fields.append(field.strip())
                field = ''
                needs_appending = False
                continue

        field += ch
        needs_appending = True

    if needs_appending:
        fields.append(field.strip())

    # Go through each field looking for ':' separated selector/value pairs.
    for fields in fields:
        # A missing selector is treated as 'true'.
        parts = field.split(':', maxsplit=1)
        if len(parts) != 2:
            return field

        selector, field_value = parts

        # An escaped ':' means the selector is missing.
        if selector and selector[-1] == '\\':
            # Remove the escape.
            return field.replace('\\', '', 1)

        # See if the selector is inverted.
        if selector.startswith('!'):
            selector = selector[1:]
            inverted = True
        else:
            inverted = False

        if anno.pm.evaluate_feature_or_platform(anno.p, anno.symbol, selector, inverted):
            return field_value.strip()

    # No value was selected so ignore the annotation completely.
    return None


def _validate_string_list(name, anno):
    """ Return a valid string list value. """

    if anno is None:
        return None

    value = anno.value

    if not isinstance(value, str):
        _error(anno, name, "must be a quoted string")
        return []

    return value.split(' ')


# The annotations and the type of their values.
_ANNOTATION_TYPES = {
    '__imatmul__':              AnnotationType.BOOLEAN,
    '__len__':                  AnnotationType.BOOLEAN,
    '__matmul__':               AnnotationType.BOOLEAN,
    'AbortOnException':         AnnotationType.BOOLEAN,
    'Abstract':                 AnnotationType.BOOLEAN,
    'AllowNone':                AnnotationType.BOOLEAN,
    'Array':                    AnnotationType.BOOLEAN,
    'ArraySize':                AnnotationType.BOOLEAN,
    'AutoGen':                  AnnotationType.NAME_OPTIONAL,
    'BaseType':                 AnnotationType.NAME,
    'Capsule':                  AnnotationType.BOOLEAN,
    'Constrained':              AnnotationType.BOOLEAN,
    'Deprecated':               AnnotationType.STRING_OPTIONAL,
    'Default':                  AnnotationType.BOOLEAN,
    'DelayDtor':                AnnotationType.BOOLEAN,
    'DisallowNone':             AnnotationType.BOOLEAN,
    'ExportDerived':            AnnotationType.BOOLEAN,
    'ExportDerivedLocally':     AnnotationType.BOOLEAN,
    'External':                 AnnotationType.BOOLEAN,
    'Encoding':                 AnnotationType.STRING,
    'Factory':                  AnnotationType.BOOLEAN,
    'FileExtension':            AnnotationType.STRING,
    'GetWrapper':               AnnotationType.BOOLEAN,
    'HoldGIL':                  AnnotationType.BOOLEAN,
    'In':                       AnnotationType.BOOLEAN,
    'KeepReference':            AnnotationType.INTEGER_OPTIONAL,
    'KeywordArgs':              AnnotationType.STRING,
    'Metatype':                 AnnotationType.DOTTED_NAME,
    'Mixin':                    AnnotationType.BOOLEAN,
    'Movable':                  AnnotationType.BOOLEAN,
    'NewThread':                AnnotationType.BOOLEAN,
    'NoArgParser':              AnnotationType.BOOLEAN,
    'NoAssignmentOperator':     AnnotationType.BOOLEAN,
    'NoCopy':                   AnnotationType.BOOLEAN,
    'NoCopyCtor':               AnnotationType.BOOLEAN,
    'NoDefaultCtor':            AnnotationType.BOOLEAN,
    'NoDefaultCtors':           AnnotationType.BOOLEAN,
    'NoDerived':                AnnotationType.BOOLEAN,
    'NoRaisesPyException':      AnnotationType.BOOLEAN,
    'NoRelease':                AnnotationType.BOOLEAN,
    'NoScope':                  AnnotationType.BOOLEAN,
    'NoSetter':                 AnnotationType.BOOLEAN,
    'NoTypeHint':               AnnotationType.BOOLEAN,
    'NoTypeName':               AnnotationType.BOOLEAN,
    'NoVirtualErrorHandler':    AnnotationType.BOOLEAN,
    'Numeric':                  AnnotationType.BOOLEAN,
    'Out':                      AnnotationType.BOOLEAN,
    'PostHook':                 AnnotationType.NAME,
    'PreHook':                  AnnotationType.NAME,
    'PyInt':                    AnnotationType.BOOLEAN,
    'PyName':                   AnnotationType.NAME,
    'RaisesPyException':        AnnotationType.BOOLEAN,
    'ReleaseGIL':               AnnotationType.BOOLEAN,
    'ResultSize':               AnnotationType.BOOLEAN,
    'Sequence':                 AnnotationType.BOOLEAN,
    'Supertype':                AnnotationType.DOTTED_NAME,
    'Transfer':                 AnnotationType.BOOLEAN,
    'TransferBack':             AnnotationType.BOOLEAN,
    'TransferThis':             AnnotationType.BOOLEAN,
    'TypeHint':                 AnnotationType.STRING,
    'TypeHintIn':               AnnotationType.STRING,
    'TypeHintOut':              AnnotationType.STRING,
    'TypeHintValue':            AnnotationType.STRING,
    'VirtualErrorHandler':      AnnotationType.NAME,
}
