# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import os

from ....version import SIP_VERSION_STR

from ...specification import IfaceFileType

from ..formatters import fmt_copying

from .backends import AbstractBackend
from .snippets import (g_composite_module_code, g_iface_file_code,
        g_module_code, g_module_header_file)


def output_code(spec, bindings, project, buildable):
    """ Output the C/C++ code and add it to the given buildable. """

    module = spec.module
    py_debug = project.py_debug
    backend = AbstractBackend.factory(spec)

    if spec.is_composite:
        source_name = os.path.join(buildable.build_dir,
                'sip' + module.py_name + 'cmodule.c')

        with CompilationUnit(source_name, "Composite module code.", module, project, buildable, sip_api_file=False) as sf:
            g_composite_module_code(backend, sf, py_debug)
    else:
        _module_code(backend, bindings, project, py_debug, buildable)


def _empty_iface_file(spec, iface_file):
    """ See if an interface file has any content. """

    for klass in spec.classes:
        if klass.iface_file is iface_file and not klass.is_hidden_namespace and not klass.is_protected and not klass.external:
            return False

    for mapped_type in spec.mapped_types:
        if mapped_type.iface_file is iface_file:
            return False

    return True


def _make_part_name(buildable, module_name, part_nr, source_suffix):
    """ Return the filename of a source code part on the heap. """

    return os.path.join(buildable.build_dir,
            f'sip{module_name}part{part_nr}{source_suffix}')


def _module_code(backend, bindings, project, py_debug, buildable):
    """ Generate the C/C++ code for a module. """

    spec = backend.spec
    module = spec.module
    module_name = module.py_name
    parts = bindings.concatenate

    source_suffix = bindings.source_suffix
    if source_suffix is None:
        source_suffix = '.c' if spec.c_bindings else '.cpp'

    # Calculate the number of files in each part.
    if parts:
        nr_files = 1

        for iface_file in spec.iface_files:
            if iface_file.module is module and iface_file.type is not IfaceFileType.EXCEPTION:
                nr_files += 1

        max_per_part = (nr_files + parts - 1) // parts
        files_in_part = 1
        this_part = 0

        source_name = _make_part_name(buildable, module_name, 0, source_suffix)
    else:
        source_name = os.path.join(buildable.build_dir,
                'sip' + module_name + 'cmodule' + source_suffix)

    sf = CompilationUnit(source_name, "Module code.", module, project,
            buildable)

    closure = g_module_code(backend, sf, bindings, project, py_debug, buildable)

    # Generate the interface source files.
    for iface_file in spec.iface_files:
        if iface_file.module is module and iface_file.type is not IfaceFileType.EXCEPTION:
            need_postinc = False
            use_sf = None

            if parts:
                if files_in_part == max_per_part:
                    # Close the old part.
                    sf.close()

                    # Create a new one.
                    files_in_part = 1
                    this_part += 1

                    source_name = _make_part_name(buildable, module_name,
                            this_part, source_suffix)
                    sf = CompilationUnit(source_name, "Module code.", module,
                            project, buildable)

                    need_postinc = True
                else:
                    files_in_part += 1

                if iface_file.file_extension is None:
                    # The interface file should use this source file rather
                    # than create one of its own.
                    use_sf = sf

            # Check that there will be something in the file so that we don't
            # get warning messages from ranlib.
            if not _empty_iface_file(spec, iface_file):
                if use_sf is None:
                    source_name = os.path.join(buildable.build_dir,
                            'sip' + iface_file.module.py_name)

                    for part in iface_file.fq_cpp_name:
                        source_name += part

                    if iface_file.file_extension is not None:
                        source_suffix = iface_file.file_extension

                    source_name += source_suffix

                    iface_sf = CompilationUnit(source_name,
                            "Interface wrapper code.", iface_file.module,
                            project, buildable)

                    need_postinc = True
                else:
                    iface_sf = use_sf

                g_iface_file_code(backend, iface_sf, bindings, project,
                        buildable, py_debug, iface_file, need_postinc)

    sf.close()

    header_name = os.path.join(buildable.build_dir, f'sipAPI{module_name}.h')

    with SourceFile(header_name, "Internal module API header file.", module, project, buildable.headers) as sf:
        g_module_header_file(backend, sf, bindings, py_debug, closure)


class SourceFile:
    """ The encapsulation of a source file. """

    def __init__(self, source_name, description, module, project, generated):
        """ Initialise the object. """

        self._description = description
        self._module = module

        self.open(source_name, project)

        generated.append(source_name)

    def __enter__(self):
        """ Implement a context manager for the file. """

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """ Implement a context manager for the file. """

        self.close()

    def close(self):
        """ Close the source file. """

        self._f.close()

    def open(self, source_name, project):
        """ Open a source file and make it current. """

        self._f = open(source_name, 'w', encoding='UTF-8')

        self._line_nr = 1

        self._write_header_comments(self._description, self._module,
                project.version_info)

    def write(self, s):
        """ Write a string while tracking the current line number. """

        # Older C++ standards (pre-C++17) get confused with digraphs (usually
        # when the default setuptools is being used to build C++ extensions).
        # The easiest solution is to hack the string for the most common case
        # and hope it doesn't have unintended consequences.
        self._f.write(s.replace('_cast<::', '_cast< ::'))
        self._line_nr += s.count('\n')

    def write_code(self, code):
        """ Write some handwritten code. """

        # A trivial case.
        if code is None:
            return

        # The code may be a single code block or a list of them.
        code_blocks = code if isinstance(code, list) else [code]

        # Another trivial case.
        if not code_blocks:
            return

        for code_block in code_blocks:
            self.write(f'#line {code_block.line_nr} "{self._posix_path(code_block.sip_file)}"\n')
            self.write(code_block.text)

        self.write(f'#line {self._line_nr + 1} "{self._posix_path(self._f.name)}"\n')

    @staticmethod
    def _posix_path(path):
        """ Return the POSIX format of a path. """

        return path.replace('\\', '/')

    def _write_header_comments(self, description, module, version_info):
        """ Write the comments at the start of the file. """

        version_info_s = f' *\n * Generated by SIP {SIP_VERSION_STR}\n' if version_info else ''

        copying_s = fmt_copying(module.copying, ' *')

        self.write(
f'''/*
 * {description}
{version_info_s}{copying_s} */

''')


class CompilationUnit(SourceFile):
    """ Encapsulate a compilation unit, ie. a C or C++ source file. """

    def __init__(self, source_name, description, module, project, buildable,
            sip_api_file=True):
        """ Initialise the object. """

        super().__init__(source_name, description, module, project,
                buildable.sources)

        self.write_code(module.unit_code)

        if sip_api_file:
            self.write(f'#include "sipAPI{module.py_name}.h"\n')
