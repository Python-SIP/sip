# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


# Publish the public API.
from .annotations import AnnotationContext, AnnotationNoValue, AnnotationType
from .model import (Argument, Class, Constructor, Enum, EnumMember, Function,
        MappedType, Overload, Signature, Specification)
from .plugin import Plugin
