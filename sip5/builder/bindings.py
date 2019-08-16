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
from ..version import SIP_VERSION_STR

from .buildable import BuildableBindings
from .configurable import Configurable, Option
from .pyproject import (PyProjectOptionException,
        PyProjectUndefinedOptionException)


class Bindings(Configurable):
    """ The encapsulation of a module's bindings. """

    # The configurable options.
    _options = (
        # The list of backstop tags.
        Option('backstops', option_type=list),

        # Any builder-specific settings.
        Option('builder_settings', option_type=list),

        # The list of #define names and values in the format "NAME" or
        # "NAME=VALUE".
        Option('define_macros', option_type=list),

        # The list of disabled feature tags.
        Option('disabled_features', option_type=list),

        # Set if exception support is enabled.
        Option('exceptions', option_type=bool),

        # The list of additional .h files.
        Option('headers', option_type=list),

        # The list of additional C/C++ include directories to search.
        Option('include_dirs', option_type=list),

        # The list of library names to link against.
        Option('libraries', option_type=list),

        # The list of C/C++ library directories to search.
        Option('library_dirs', option_type=list),

        # The name of the bindings.  This never appears in generated code and
        # is used to identify the bindings to the user.
        Option('name'),

        # Set to always release the Python GIL.
        Option('release_gil', option_type=bool),

        # Set to compile the module as a static library.
        Option('static', option_type=bool),

        # The name of the .sip file that specifies the bindings relative to the
        # project's 'sip_files_dir'.
        Option('sip_file'),

        # The filename extension to use for generated source files.
        Option('source_suffix'),

        # The list of additional C/C++ source files to compile and link.
        Option('sources', option_type=list),

        # The list of tags to enable.
        Option('tags', option_type=list),

        # The user-configurable options.  Although the use of a corresponding
        # command line option will affect all sets of bindings, putting them
        # here (as opposed to in Builder) means they can have individual
        # values specified in pyproject.toml.
        Option('concatenate', option_type=int,
                help="concatenate the generated bindings into N source files",
                metavar="N", tools='build install wheel'),
        Option('debug', option_type=bool, help="build with debugging symbols",
                tools='build install wheel'),
        Option('docstrings', option_type=bool, inverted=True,
                help="disable the generation of docstrings",
                tools='build install wheel'),
        Option('generate_api', help="generate a QScintilla .api file",
                metavar="FILE", tools='build install wheel'),
        Option('generate_extracts', option_type=list,
                help="generate an extract file", metavar="ID:FILE",
                tools='build install wheel'),
        Option('pep484_stubs', option_type=bool,
                help="generate a PEP 484 .pyi file",
                tools='build install wheel'),
        Option('protected_is_public', option_type=bool,
                help="enable the protected/public hack (default on non-Windows)",
                tools='build install wheel'),
        Option('protected_is_public', option_type=bool, inverted=True,
                help="disable the protected/public hack (default on Windows)",
                tools='build install wheel'),
        Option('tracing', option_type=bool, help="build with tracing support",
                tools='build install wheel'),
    )

    def __init__(self, project, **kwargs):
        """ Initialise the bindings. """

        super().__init__(**kwargs)

        self.project = project
        self.build_sources = None

        self._sip_files = None

    def apply_defaults(self, tool):
        """ Set default values for options that haven't been set yet. """

        # We need to call the super-class first because we need py_platform, so
        # remember if this had been initialised.
        protected_is_public = self.protected_is_public

        super().apply_defaults(tool)

        if protected_is_public is None:
            self.protected_is_public = (self.project.py_platform != 'win32')

    def configure(self, section, section_name):
        """ Perform the initial configuration of the bindings. """

        super().configure(section, section_name)

        # We need to ensure the bindings have a name as soon as possible.
        if not self.name:
            raise PyProjectUndefinedOptionException('name',
                    section_name='tool.sip.bindings')

    def generate(self):
        """ Generate the bindings source code and optional additional extracts.
        and return a BuildableBindings instance containing the details of
        everything needed to build the bindings.  When called the current
        directory is set to the directory containing the defining .sip file.
        """

        project = self.project

        # Parse the input file.
        pt, fq_name, uses_limited_api, sip_files = self._parse()

        name_parts = fq_name.split('.')
        buildable_name = name_parts[-1]

        uses_limited_api = bool(uses_limited_api)
        if project.py_debug:
            uses_limited_api = False

        if project.sip_module:
            if len(name_parts) == 1:
                raise UserException(
                        "module '{0}' must be part of a project when used "
                        "with a shared 'sip' module".format(fq_name))
        elif uses_limited_api:
            raise UserException(
                    "module '{0}' cannot use the limited API without using a "
                    "shared 'sip' module".format(fq_name))

        # Only save the .sip files if they haven't already been obtained
        # (possibly by a sub-class).
        if self._sip_files is None:
            self._sip_files = sip_files

        # Make sure the module's sub-directory exists.
        sources_dir = os.path.join(project.build_dir, buildable_name)
        os.makedirs(sources_dir, exist_ok=True)

        # The details of things that will have been generated.  Note that we
        # don't include anything for .api files or generic extracts as the
        # arguments include a file name.
        build_sources = BuildableBindings(self, fq_name, buildable_name,
                sources_dir, uses_limited_api)

        build_sources.builder_settings.extend(self.builder_settings)
        build_sources.debug = self.debug
        build_sources.static = self.static

        # Generate any API file.
        if self.generate_api:
            generateAPI(pt, generate_api)

        # Generate any extracts.
        if self.generate_extracts:
            generateExtracts(pt, extracts)

        # Generate any type hints file.
        if self.pep484_stubs:
            build_sources.pyi_file = os.path.join(sources_dir,
                    buildable_name + '.pyi')
            generateTypeHints(pt, build_sources.pyi_file)

        # Generate the bindings.
        header, sources = generateCode(pt, sources_dir, self.source_suffix,
                self.exceptions, self.tracing, self.release_gil,
                self.concatenate, self.tags, self.disabled_features,
                self.docstrings, project.py_debug, project.sip_module)

        if header:
            build_sources.headers.append(header)

        build_sources.headers.extend(self.headers)

        build_sources.sources.extend(sources)

        # Add the sip module code if it is not shared.
        build_sources.include_dirs.append(sources_dir)

        if project.sip_module:
            # sip.h will already be in the build directory.
            build_sources.include_dirs.append(project.build_dir)
        else:
            build_sources.sources.extend(
                    copy_nonshared_sources(project.abi_version, sources_dir))

        build_sources.include_dirs.extend(self.include_dirs)
        build_sources.sources.extend(self.sources)

        if self.protected_is_public:
            build_sources.define_macros.append('SIP_PROTECTED_IS_PUBLIC')
            build_sources.define_macros.append('protected=public')

        build_sources.define_macros.extend(self.define_macros)

        build_sources.libraries.extend(self.libraries)
        build_sources.library_dirs.extend(self.library_dirs)

        build_sources.make_names_relative()

        return build_sources

    def get_options(self):
        """ Return the list of configurable options. """

        options = super().get_options()
        options.extend(self._options)

        return options

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
            _, _, _, self._sip_files = self._parse()

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

    def is_buildable(self):
        """ Return True if the bindings are buildable.  This will not be called
        if the bindings have been explicitly enabled.
        """

        return True

    def verify_configuration(self, tool):
        """ Verify that the configuration is complete and consistent. """

        project = self.project

        super().verify_configuration(tool)

        # On Windows the interpreter must be a debug build if a debug version
        # is to be built and vice versa.
        if sys.platform == 'win32':
            if self.debug:
                if not project.py_debug:
                    raise UserException(
                            "A debug version of Python must be used when "
                            "building a debug version of the {0} "
                            "bindings".format(self.name))
            elif project.py_debug:
                raise UserException(
                        "A debug version of the {0} bindings must be built "
                        "when a debug version of Python is used".format(
                                self.name))

        # Provide a default .sip file name if needed.
        if not self.sip_file:
            self.sip_file = self.name + '.sip'

        if not os.path.isabs(self.sip_file):
            self.sip_file = os.path.join(project.sip_files_dir, self.sip_file)

        # Check the .sip file exists.
        if not os.path.isfile(self.sip_file):
            raise PyProjectOptionException('sip-file',
                    "the file '{0}' for the '{1}' bindings does not "
                            "exist".format(self.sip_file self.name),
                    section_name='tool.sip.bindings')

        if not self.source_suffix:
            self.source_suffix = None

    def write_configuration(self, buildable, bindings_dir):
        """ Write the configuration of the bindings and return the name of the
        file.
        """

        # Create a bindings-specific sub-directory.
        bindings_dir = os.path.join(bindings_dir, buildable.name)
        os.makedirs(bindings_dir, exist_ok=True)

        config_file = os.path.join(bindings_dir, buildable.name + '.toml')

        with open(config_file, 'w') as cf:
            tags = ', '.join(['"{}"'.format(t) for t in self.tags])
            disabled = ', '.join(
                    ['"{}"'.format(f) for f in self.disabled_features])

            cf.write("# Automatically generated configuration for '{0}'.\n".format(self.name))
            cf.write('''
sip-version = "{}"
sip-abi-version = "{}"
module-tags = [{}]
module-disabled-features = [{}]
'''.format(SIP_VERSION_STR, self.project.abi_version, tags, disabled))

        return config_file

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
