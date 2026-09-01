# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from enum import auto, Enum


class AnnotationContext(Enum):
    """ The different annotation contexts. """

    ARGUMENT = auto()
    CLASS = auto()
    CONSTRUCTOR = auto()
    DESTRUCTOR = auto()
    ENUM = auto()
    ENUM_MEMBER = auto()
    EXCEPTION = auto()
    FUNCTION = auto()
    MAPPED_TYPE = auto()
    TYPEDEF = auto()
    VARIABLE = auto()


# This is the value of an optional annotation when the value part is missing.
AnnotationNoValue = object()


class AnnotationType(Enum):
    """ The different annotation types and how they are parsed.  Note that an
    _OPTIONAL suffix means that the value part of the annotation is optional,
    not the annotation itself.
    """

    # The value is True if the annotation is specified and False if not.
    BOOLEAN = auto()

    # The value is a str compatible with an identifier preceded by a Python
    # scope.
    DOTTED_NAME = auto()

    # The value is a str compatible with an identifier preceded by a Python
    # scope or AnnotationNoValue if the value part was omitted.
    DOTTED_NAME_OPTIONAL = auto()

    # The value is a signed int.
    INTEGER = auto()

    # The value is a signed int or AnnotationNoValue if the value part was
    # omitted.
    INTEGER_OPTIONAL = auto()

    # The value is a str compatible with a C/C++ identifier.
    NAME = auto()

    # The value is a str compatible with a C/C++ identifier or
    # AnnotationNoValue if the value part was omitted.
    NAME_OPTIONAL = auto()

    # The value is a str extracted from a quoted string with embedded
    # selectors or None if no selector evaluated to True.
    STRING = auto()

    # The value is a list[str] extracted from a quoted string.
    STRING_LIST = auto()

    # The value is a str extracted from a quoted string with embedded
    # selectors, None if no selector evaluated to True or AnnotationNoValue if
    # the value part was omitted.
    STRING_OPTIONAL = auto()
