# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


from .parser_manager import ParserManager


def parse(hex_version, spec, include_dirs):
    """ Parse a .sip file and return a 2-tuple of a list of Module objects and
    a list of the .sip files that specify the module to be generated.
    """

    return ParserManager(hex_version, spec, include_dirs).parse()
