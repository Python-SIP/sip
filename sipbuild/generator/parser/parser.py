# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from .parser_manager import ParserManager


def parse(hex_version, bindings, include_dirs, is_strict=True):
    """ Parse a .sip file and return a 3-tuple of a Specification object, a
    list of Module objects and a list of the .sip files that specify the module
    to be generated.  A UserException is raised if there was an error.
    """

    return ParserManager(hex_version, bindings, include_dirs,
            is_strict).parse()
