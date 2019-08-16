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


from collections import OrderedDict
from distutils.sysconfig import get_python_inc, get_python_lib
import importlib
import os
import shutil
import sys
import tempfile
import warnings

from ..exceptions import UserException
from ..module import resolve_abi_version

from .abstract_builder import AbstractBuilder
from .bindings import Bindings
from .configurable import Configurable, Option
from .pyproject import (PyProject, PyProjectOptionException,
        PyProjectUndefinedOptionException)


# The first and last supported minor versions of Python v3.
FIRST_SUPPORTED_MINOR = 5
LAST_SUPPORTED_MINOR = 8


class Project(Configurable):
    """ Encapsulate a project containing one or more sets of bindings. """

    # The configurable options.
    _options = (
        # The ABI version that the sip module should use.
        Option('abi_version'),

        # The module:object of a callable that will return an AbstractBuilder
        # instance.
        Option('builder'),

        # The list of extra files and directories to be copied to an
        # installation or a wheel.  An item can be a name (copied to the root
        # of the installation) or a 2 element list of name and directory
        # relative to the root of the installation.
        Option('install_extras', option_type=list),

        # Set if building for a debug version of Python.
        Option('py_debug', option_type=bool),

        # The name of the directory containing Python.h.
        Option('py_include_dir', default=get_python_inc()),

        # The name of the target Python platform.
        Option('py_platform'),

        # The major version number of the target Python installation.
        Option('py_major_version', option_type=int),

        # The minor version number of the target Python installation.
        Option('py_minor_version', option_type=int),

        # The name of the directory containing the .sip files.  If the sip
        # module is shared then each set of bindings is in its own
        # sub-directory.
        Option('sip_files_dir', default=os.getcwd()),

        # The list of extra files and directories, specified as glob patterns,
        # to be copied to an sdist.
        Option('sdist_extras', option_type=list),

        # The list of additional directories to search for .sip files.
        Option('sip_include_dirs', option_type=list),

        # The fully qualified name of the sip module.
        Option('sip_module'),

        # The user-configurable options.
        Option('quiet', option_type=bool,
                help="disable all progress messages"),
        Option('verbose', option_type=bool,
                help="enable verbose progress messages"),
        Option('build_dir', help="the build directory", metavar="DIR"),
        Option('target_dir', default=get_python_lib(plat_specific=1),
                help="the target installation directory", metavar="DIR",
                tools='install')
    )

    # The configurable options for multiple bindings.
    _multibindings_options = (
        Option('disable', option_type=list, help="disable the NAME bindings",
                metavar="NAME", tools='build install wheel'),
        Option('enable', option_type=list, help="enable the NAME bindings",
                metavar="NAME", tools='build install wheel'),
    )

    def __init__(self, **kwargs):
        """ Initialise the project. """

        super().__init__(**kwargs)

        # The current directory should contain pyproject.toml.
        self.root_dir = os.getcwd()
        self.bindings = []
        self.builder = None

        self._temp_build_dir = None

    def apply_defaults(self, tool):
        """ Set default values for options that haven't been set yet. """

        # For the build tool we want build_dir to default to a local 'build'
        # directory (which we won't remove).  However, for other tools (and for
        # PEP 517 frontends) we want to use a temporary directory in case the
        # current directory is read-only.
        if self.build_dir is None:
            if tool == 'build':
                self.build_dir = 'build'
            else:
                self._temp_build_dir = tempfile.TemporaryDirectory()
                self.build_dir = self._temp_build_dir.name

        if self.py_major_version is None or self.py_minor_version is None:
            self.py_major_version = sys.hexversion >> 24
            self.py_minor_version = (sys.hexversion >> 16) & 0x0ff

        if self.py_platform is None:
            self.py_platform = sys.platform

        if self.py_debug is None:
            self.py_debug = hasattr(sys, 'gettotalrefcount')

        super().apply_defaults(tool)

        # Adjust the list of bindings according to what has been explicitly
        # enabled and disabled.
        self._enable_disable_bindings()

        # Set the defaults for the builder and bindings.
        self.builder.apply_defaults(tool)

        for bindings in self.bindings:
            bindings.apply_defaults(tool)

    def build(self):
        """ Build the project in-situ. """

        self.builder.build()

    def build_sdist(self, sdist_directory):
        """ Build an sdist for the project and return the name of the sdist
        file.
        """

        sdist_file = self.builder.build_sdist(sdist_directory)
        self._remove_build_dir()

        return sdist_file

    def build_wheel(self, wheel_directory):
        """ Build a wheel for the project and return the name of the wheel
        file.
        """

        wheel_file = self.builder.build_wheel(wheel_directory)
        self._remove_build_dir()

        return wheel_file

    @classmethod
    def factory(cls, tool='', description=''):
        """ Return a Project instance fully configured for a particular command
        line tool.  If no tool is specified then it is configured for a PEP 517
        frontend.
        """

        # Get the contents of the pyproject.toml file.
        pyproject = PyProject()

        # Try and import a project.py file.
        spec = importlib.util.spec_from_file_location('project', 'project.py')
        project_module = importlib.util.module_from_spec(spec)

        try:
            spec.loader.exec_module(project_module)
        except FileNotFoundError:
            project_factory = cls
        except Exception as e:
            raise UserException("unable to import project.py", detail=str(e))
        else:
            # Look for a class that is a sub-class of Project.
            for project_factory in project_module.__dict__.values():
                if isinstance(project_factory, type):
                    if issubclass(project_factory, Project):
                        # Make sure the type is defined in project.py and not
                        # imported by it.
                        if project_factory.__module__ == 'project':
                            break
            else:
                raise UserException(
                        "project.py does not define a Project sub-class")

        # Create the project.
        project = project_factory()

        # Set the initial configuration from the pyproject.toml file.
        project._set_initial_configuration(pyproject)

        # Add any tool-specific command line options for (so far unspecified)
        # parts of the configuration.
        if tool:
            project._configure_from_command_line(tool, description)

        # Make sure the configuration is complete.
        project.apply_defaults(tool)

        # Configure the warnings module.
        if not project.verbose:
            warnings.simplefilter('ignore', UserWarning)

        # Make sure we have a clean build directory and make it current.
        project.progress("Creating the build directory")

        if project._temp_build_dir is None:
            project.build_dir = os.path.abspath(project.build_dir)
            shutil.rmtree(project.build_dir, ignore_errors=True)
            os.mkdir(project.build_dir)

        os.chdir(project.build_dir)

        # Allow a sub-class (in a user supplied script) to make any updates to
        # the configuration.
        project.update(tool)

        os.chdir(project.root_dir)

        # Make sure the configuration is correct after any user supplied script
        # has messed with it.
        project.verify_configuration(tool)

        return project

    def get_builder(self):
        """ Get the builder. """

        # This default implementation gets the factory from the .toml file.
        if self.builder:
            builder_factory = self._import_callable(self.builder,
                    'tool.sip.project', 'builder')
        else:
            from .distutils_builder import DistutilsBuilder as builder_factory

        return builder_factory(self)

    def get_options(self):
        """ Return the list of configurable options. """

        options = super().get_options()
        options.extend(self._options)
        options.extend(self._multibindings_options)

        return options

    def install(self):
        """ Install the project. """

        self.builder.install()
        self._remove_build_dir()

    @staticmethod
    def open_for_writing(fname):
        """ Open a file for writing while handling any errors. """

        try:
            return open(fname, 'w')
        except IOError as e:
            raise UserException(
                    "There was an error creating '{0}' - make sure you have "
                    " write permission on the parent directory".format(fname),
                    detail=str(e))

    def progress(self, message):
        """ Print a progress message unless they are disabled. """

        if not self.quiet:
            if message[-1] != '.':
                message += '...'

            print(message)

    def update(self, tool):
        """ This should be re-implemented by any user supplied sub-class to
        carry out any updates to the configuration as required.  The current
        directory will be the temporary build directory.
        """

        # This default implementation does nothing.

    def update_buildable_bindings(self):
        """ Update the list of bindings to ensure they are either buildable or
        have been explicitly enabled.
        """

        # Explicitly enabled bindings are assumed to be buildable.
        if self.enable:
            return

        self.bindings = [b for b in self.bindings if b.is_buildable()]

    def verify_configuration(self, tool):
        """ Verify that the configuration is complete and consistent. """

        # Make sure we support the targeted version of Python.
        py_version = (self.py_major_version, self.py_minor_version)
        first_version = (3, FIRST_SUPPORTED_MINOR)
        last_version = (3, LAST_SUPPORTED_MINOR)

        if py_version < first_version or py_version > last_version:
            raise UserException(
                    "Python v{}.{} is not supported".format(
                            self.py_major_version, self.py_minor_version))

        # Make sure we have a valid ABI version.
        self.abi_version = resolve_abi_version(self.abi_version)

        # Check we have the name of the sip module if it is shared.
        if len(self.bindings) > 1 and not self.sip_module:
            raise PyProjectOptionException('sip-module',
                    "must be defined when the project contains multiple sets "
                            "of bindings")

        # If we use a shared sip module then add the dependency to the
        # meta-data.
        if self.sip_module:
            requires_dist = self.metadata.get('requires-dist')
            if requires_dist is None:
                requires_dist = []
            elif isinstance(requires_dist, str):
                requires_dist = [requires_dist]

            # Ignore if the module is already defined.
            sip_project_name = self.sip_module.replace('.', '-')

            for rd in requires_dist:
                if rd.split()[0] == sip_project_name:
                    break
            else:
                next_abi_major = int(self.abi_version.split('.')[0]) + 1

                requires_dist.insert(0,
                        '{} (>={}, <{})'.format(sip_project_name,
                                self.abi_version, next_abi_major))

                self.metadata['requires-dist'] = requires_dist

        # Verify the types of any install extras.
        def bad_extras():
            raise PyProjectOptionException('install-extras',
                    "each element must be a sub-list of a source file or "
                            "directory and an optional destination directory")

        for extra in self.install_extras:
            if not isinstance(extra, list):
                bad_extras()

            if len(extra) == 1:
                src = extra[0]
            elif len(extra) == 2:
                src, dst = extra

                if not isinstance(dst, str):
                    bad_extras()
            else:
                bad_extras()

            if not isinstance(src, str):
                bad_extras()

        # Verify the configuration of the builder and bindings.
        self.builder.verify_configuration(tool)

        for bindings in self.bindings:
            bindings.verify_configuration(tool)

    def _configure_from_command_line(self, tool, description):
        """ Update the configuration from the user supplied command line. """

        from argparse import SUPPRESS
        from ..argument_parser import ArgumentParser

        parser = ArgumentParser(description, argument_default=SUPPRESS)

        # Add the user configurable options to the parser.
        all_options = {}
        
        options = self.get_options()
        if len(self.bindings) < 2:
            # Remove the options that only make sense where the project has
            # multiple bindings.
            for multi in self._multibindings_options:
                options.remove(multi)

        self.add_command_line_options(parser, tool, all_options,
                options=options)

        self.builder.add_command_line_options(parser, tool, all_options)

        for bindings in self.bindings:
            bindings.add_command_line_options(parser, tool, all_options)

        # Parse the arguments and update the corresponding configurables.
        args = parser.parse_args()

        for option, configurables in all_options.items():
            for configurable in configurables:
                if hasattr(args, option.dest):
                    setattr(configurable, option.name,
                            getattr(args, option.dest))

    def _enable_disable_bindings(self):
        """ Check the enabled bindings are valid and remove any disabled ones.
        """

        names = [bindings.name for bindings in self.bindings]

        # Check that any explicitly enabled bindings are valid.
        if self.enable:
            for enabled in self.enable:
                if enabled not in names:
                    raise UserException(
                            "unknown enabled bindings '{0}'".format(enabled))

            # Only include explicitly enabled bindings.
            self.bindings = [b for b in self.bindings if b.name in self.enable]

        # Check that any explicitly disabled bindings are valid.
        if self.disable:
            for disabled in self.disable:
                if disabled not in names:
                    raise UserException(
                            "unknown disabled bindings '{0}'".format(disabled))

            # Remove any explicitly disabled bindings.
            self.bindings = [b for b in self.bindings
                    if b.name not in self.disable]

    @staticmethod
    def _import_callable(callable_name, section_name, name):
        """ Return a callable imported from a location specified as a value in
        the pyproject.toml file.
        """

        # Extract the module and object names.
        parts = callable_name.split(':')
        if len(parts) != 2:
            raise PyProjectOptionException(name,
                    "must be defined as 'module:name'",
                    section_name=section_name)

        module_name, obj_name = parts

        # Try and import the module.
        try:
            module = importlib.import_module(module_name)
        except ImportError as e:
            raise PyProjectOptionException(name,
                    "unable to import '{0}'".format(module_name),
                    section_name=section_name, detail=str(e))

        # Get the callable.
        obj = getattr(module, obj_name)
        if obj is None:
            raise PyProjectOptionException(name,
                    "'{0}' module has no callable '{1}'".format(module_name,
                            obj_name),
                    section_name=section_name)

        return obj

    def _remove_build_dir(self):
        """ Remove the build directory. """

        self.progress("Removing the build directory")

        self._temp_build_dir = None

    def _set_initial_configuration(self, pyproject):
        """ Set the project's initial configuration. """

        # Get the metadata and extract the name and version.
        self.metadata = OrderedDict()
        self.name = None
        self.version = None
        metadata_version = None
        requires_python = None

        metadata = pyproject.get_section('tool.sip', required=True)
        for md_name, md_value in metadata.items():
            # Ignore sub-sections.
            if pyproject.is_section(md_value):
                continue

            if not isinstance(md_value, str):
                raise PyProjectOptionException(md_name, "must be a string",
                        section_name='tool.sip')

            md_name = md_name.lower()
            if md_name == 'name':
                if not md_value.replace('-', '_').isidentifier():
                    raise PyProjectOptionException('name',
                            "'{0}' is an invalid project name".format(
                                    md_value),
                            section_name='tool.sip')

                self.name = md_value
            elif md_name == 'version':
                self.version = md_value
            elif md_name == 'metadata-version':
                metadata_version = md_value
            elif md_name == 'requires-python':
                requires_python = md_value

            self.metadata[md_name] = md_value

        if self.name is None:
            raise PyProjectUndefinedOptionException('name',
                    section_name='tool.sip')

        if self.version is None:
            self.version = '0.1'
            self.metadata['version'] = self.version

        if metadata_version is None:
            # Default to PEP 566.
            self.metadata['metadata-version'] = '2.1'

        if requires_python is None:
            # The minimal version of Python we support.
            self.metadata['requires-python'] = '>=3.{}'.format(
                    FIRST_SUPPORTED_MINOR)

        # This is cosmetic.
        for name in ('requires-python', 'version', 'name', 'metadata-version'):
            self.metadata.move_to_end(name, last=False)

        # Get the project configuration.
        project_section = pyproject.get_section('tool.sip.project')
        if project_section is not None:
            self.configure(project_section, 'tool.sip.project')

        # Create the builder.
        self.builder = self.get_builder()

        if not isinstance(self.builder, AbstractBuilder):
            raise UserException(
                    "The project builder is not a AbstractBuilder sub-class")

        builder_section = pyproject.get_section('tool.sip.builder')
        if builder_section is not None:
            self.builder.configure(builder_section, 'tool.sip.builder')

        # Get configuration for each set of bindings.
        bindings_sections = pyproject.get_section('tool.sip.bindings')
        if bindings_sections is None:
            bindings_sections = []

        for section in bindings_sections:
            # If the bindings have already been defined then update them,
            # otherwise create a new set.
            name = section.get('name')
            for bindings in self.bindings:
                if bindings.name == name:
                    break
            else:
                bindings = Bindings(self)
                self.bindings.append(bindings)

            bindings.configure(section, 'tool.sip.bindings')

        # Add a default set of bindings if none were defined.
        if not self.bindings:
            bindings = Bindings(self, name=self.name)
            self.bindings.append(bindings)
