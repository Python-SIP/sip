# Copyright (c) 2019, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


import os
import sys

from ..code_generator import (parse, generateCode, generateExtracts,
        generateAPI, generateXML, generateTypeHints)
from ..exceptions import UserException
from ..module import copy_nonshared_sources

from .configurable import Configurable, Option
from .pyproject import PyProjectUndefinedOptionException


class Bindings(Configurable):
    """ The encapsulation of a module's bindings. """

    # The configurable options.
    _options = (
        # The list of backstop tags.
        Option('backstops', option_type=list),

        # The list of #define names and values in the format "NAME" or
        # "NAME=VALUE".
        Option('define_macros', option_type=list),

        # The list of disabled feature tags.
        Option('disabled_features', option_type=list),

        # Set if exception support is enabled.
        Option('exceptions', option_type=bool),

        # The list of C/C++ include directories to search (using POSIX path
        # separators).
        Option('include_dirs', option_type=list),

        # The list of library names to link against.
        Option('libraries', option_type=list),

        # The list of C/C++ library directories to search.
        Option('library_dirs', option_type=list),

        # The name of the bindings.  This never appears in generated code but
        # is used in the default directory structure and to identify the
        # bindings to the user.
        Option('name'),

        # Set to always release the Python GIL.
        Option('release_gil', option_type=bool),

        # The name of the .sip file that specifies the bindings.
        Option('sip_file'),

        # The filename extension to use for generated source files.
        Option('source_suffix'),

        # The list of tags to enable.
        Option('tags', option_type=list),

        # The user-configurable options.
        Option('concatenate', option_type=int,
                help="concatenate the generated bindings into N source files",
                metavar="N", tools='build install wheel'),
        Option('debug', option_type=bool, help="build with debugging symbols",
                tools='build install wheel'),
        Option('docstrings', option_type=bool, inverted=True,
                help="disable the generation of docstrings",
                tools='build install wheel'),
        Option('generate_api', help="generate a QScintilla .api file",
                metavar="FILE"),
        Option('generate_extracts', option_type=list,
                help="generate an extract file", metavar="ID:FILE"),
        Option('pep484_stubs', option_type=bool,
                help="generate a PEP 484 .pyi file"),
        Option('protected_is_public', option_type=bool,
                help="enable the protected/public hack (default on non-Windows)",
                tools='build install wheel'),
        Option('protected_is_public', option_type=bool, inverted=True,
                help="disable the protected/public hack (default on Windows)",
                tools='build install wheel'),
        Option('tracing', option_type=bool, help="build with tracing support",
                tools='build install wheel'),
    )

    def __init__(self, project):
        """ Initialise the bindings. """

        super().__init__()

        self.project = project

        self._sip_files = None

    def configure(self, section, section_name):
        """ Perform the initial configuration of the bindings. """

        super().configure(section, section_name)

        # We need to ensure the bindings have a name as soon as possible.
        if not self.name:
            raise PyProjectUndefinedOptionException('tool.sip.bindings',
                    'name')

    def generate(self):
        """ Generate the bindings source code and optional additional extracts.
        Return a GeneratedBindings instance containing the details of
        everything that was generated.  The current directory is set to the
        directory containing the defining .sip file.
        """

        # Parse the input file.
        pt, name, sip_files = self._parse()

        name_parts = name.split('.')

        if self.project.sip_module and len(name_parts) == 1:
            raise UserException(
                    "module '{0}' must be part of a project when used with a "
                            "shared 'sip' module".format(name))

        # Only save the .sip files if they haven't already been obtained
        # (possibly by a sub-class).
        if self._sip_files is None:
            self._sip_files = sip_files

        # The details of things that will have been generated.  Note that we
        # don't include anything for .api files or generic extracts as the
        # arguments include a file name.
        generated = GeneratedBindings(name)

        # Make sure the module's sub-directory exists.
        sources_dir = os.path.join(self.project.build_dir, self.name)
        os.makedirs(sources_dir, exist_ok=True)

        # Generate any API file.
        if self.generate_api:
            generateAPI(pt, generate_api)

        # Generate any extracts.
        if self.generate_extracts:
            generateExtracts(pt, extracts)

        # Generate any type hints file.
        if self.pep484_stubs:
            pyi_extract = os.path.join(sources_dir, name_parts[-1] + '.pyi')
            generateTypeHints(pt, pyi_extract)
            generated.pyi_file = os.path.relpath(pyi_extract, sources_dir)

        # Generate the bindings.
        abi_major, abi_minor = self.project.abi_version.split('.')

        sources = generateCode(pt, int(abi_major), int(abi_minor), sources_dir,
                self.source_suffix, self.exceptions, self.tracing,
                self.release_gil, self.concatenate, self.tags,
                self.disabled_features, self.docstrings, self.debug,
                self.project.sip_module)

        # Add the sip module code if it is not shared.
        include_dirs = [sources_dir]

        if self.project.sip_module:
            # sip.h will already be in the build directory.
            include_dirs.append(self.project.build_dir)
        else:
            sources.extend(
                    copy_nonshared_sources(self.project.abi_version,
                            sources_dir))

        include_dirs.extend(self.include_dirs)

        generated.sources_dir = sources_dir
        generated.sources = [os.path.relpath(fn, sources_dir)
                for fn in sources]
        generated.include_dirs = [os.path.relpath(fn, sources_dir)
                for fn in include_dirs]

        return generated

    def get_options(self):
        """ Return a sequence of configurable options. """

        return self._options

    def get_sip_files(self):
        """ Return a list of .sip files that define the bindings.  These should
        all be relative to the project's sip-files-dir directory.
        """

        # If there is a shared sip module then we assume that all the relevant
        # files are in the bindings' sub-directory so we just walk that tree.
        # of that directory.
        if self.project.sip_module:
            sip_files = []

            sip_files_dir = os.path.join(self.project.sip_files_dir, self.name)
            for dirpath, _, filenames in os.walk(sip_files_dir):
                for fn in filenames:
                    sip_files.append(
                            os.path.relpath(os.path.join(dirpath, fn),
                                    self.project.sip_files_dir))

            return sip_files

        # Otherwise (without a defined directory structure) we use the list of
        # files that were parsed.
        if self._sip_files is None:
            # We haven't called the parser yet so do it now.
            _, _, self._sip_files = self._parse()

        # Check that the .sip file names are relative to the root directory and
        # are within the root directory.
        sip_files = []
        root_dir = self.project.root_dir

        for fn in self._sip_files:
            fn = os.path.abspath(fn)

            if os.path.commonprefix([fn, root_dir]) != root_dir:
                raise UserException(
                        "the .sip files that define the bindings must all be "
                        "in the '{0}' directory or a sub-directory".format(
                                root_dir))

            sip_files.append(os.path.relpath(fn, root_dir))

        return sip_files

    def verify_configuration(self):
        """ Verify that the configuration is complete and consistent. """

        # Provide a default .sip file name if needed.
        if not self.sip_file:
            if self.project.sip_module:
                sip_file = os.path.join(self.name, self.name)
            else:
                sip_file = self.name

            self.sip_file = sip_file + '.sip'

        # Check the .sip file exists.
        sip_path = os.path.join(self.project.sip_files_dir, self.sip_file)
        if not os.path.isfile(sip_path):
            raise PyProjectOptionException(
                    'tool.sip.bindings', 'sip-file',
                    "the file '{0}' for the '{1}' bindings does not exist".format(os.path.relpath(sip_path, self.project.root_dir), self.name))

        if not self.source_suffix:
            self.source_suffix = None

    def _parse(self):
        """ Invoke the parser and return its results. """

        sip_path = os.path.join(self.project.sip_files_dir, self.sip_file)
        sip_dir, sip_file = os.path.split(sip_path)

        cwd = os.getcwd()
        os.chdir(sip_dir)
        results = parse(sip_file, True, self.tags, self.backstops,
                self.disabled_features, self.protected_is_public)
        os.chdir(cwd)

        return results


class GeneratedBindings:
    """ The bindings created by Bindings generate(). """

    def __init__(self, name):
        """ Initialise the generated bindings. """

        self.name = name
        self.pyi_file = None
        self.sources = None
        self.sources_dir = None
        self.include_dirs = None
