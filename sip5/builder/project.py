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
from distutils.sysconfig import get_python_lib
import glob
import importlib
import os
import shutil
import sys
import tempfile
import warnings

from ..code_generator import set_globals
from ..exceptions import UserException
from ..module import copy_sip_h, resolve_abi_version
from ..version import SIP_VERSION, SIP_VERSION_STR

from .bindings import Bindings
from .builder import Builder
from .configurable import Configurable, Option
from .distinfo import create_distinfo
from .pyproject import (PyProject, PyProjectException,
        PyProjectOptionException, PyProjectUndefinedOptionException)


class Project(Configurable):
    """ Encapsulate a project containing one or more sets of bindings. """

    # The configurable options.
    _options = (
        # The ABI version that the sip module should use.
        Option('abi_version'),

        # The module:object of a callable that will return a Builder instance.
        Option('builder', default='sip5.builder:DistutilsBuilder'),

        # The list of extra files and directories to be copied to an
        # installation or a wheel.  An item can be a name (copied to the root
        # of the installation) or a 2 element list of name and directory
        # relative to the root of the installation.
        Option('install_extras', option_type=list),

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
        Option('verbose', option_type=bool,
                help="enable verbose progress messages"),
        Option('build_dir', help="the build directory", metavar="DIR"),
        Option('target_dir', default=get_python_lib(plat_specific=1),
                help="the target installation directory", metavar="DIR",
                tools='install')
    )

    # The configurable options for multiple bindings.
    _multibindings_options = (
        Option('disable', help="disable the NAME bindings", metavar="NAME",
                tools='build install wheel'),
        Option('enable', help="enable the NAME bindings", metavar="NAME",
                tools='build install wheel'),
    )

    def __init__(self):
        """ Initialise the project. """

        super().__init__()

        # The current directory should contain pyproject.toml.
        self.root_dir = os.getcwd()
        self.bindings = {}

        self._temp_build_dir = None

    def build(self):
        """ Build the project in-situ. """

        self._build_modules()

    def build_sdist(self, sdist_directory='.'):
        """ Build an sdist for the project and return the name of the sdist
        file.
        """

        # The sdist name.
        sdist_name = '{}-{}'.format(self.name.replace('-', '_'), self.version)

        # Create the sdist root directory.
        sdist_root = os.path.join(self.build_dir, sdist_name)
        os.mkdir(sdist_root)

        # Copy the pyproject.toml file.
        shutil.copy(os.path.join(self.root_dir, 'pyproject.toml'), sdist_root)

        # Copy in any project.py.
        project_py = os.path.join(self.root_dir, 'project.py')
        if os.path.isfile(project_py):
            shutil.copy(project_py, sdist_root)

        # Copy in any sip.h file.
        if self.sip_module and self.sip_h not in ('generate', 'installed'):
            rel_sip_h = os.path.relpath(self.sip_h, self.root_dir)
            target_sip_h = os.path.join(sdist_root, rel_sip_h)
            self._ensure_subdirs_exist(target_sip_h)
            shutil.copyfile(self.sip_h, target_sip_h)

        # Copy in the .sip files for each set of bindings.
        for bindings in self.bindings.values():
            self._install_sip_files(bindings, sdist_root)

        # Copy in anything else the user has asked for.
        for extra in self.sdist_extras:
            extra = os.path.abspath(extra)

            if os.path.commonprefix([extra, self.root_dir]) != self.root_dir:
                raise PyProjectOptionException('tool.sip.project',
                        'sdist-extras',
                        "must all be in the '{0}' directory or a sub-directory".format(self.root_dir))

            extra = os.path.relpath(extra, self.root_dir)

            for src in glob.glob(extra):
                dst = os.path.join(sdist_root, src)

                if os.path.isfile(src):
                    shutil.copyfile(src, dst)
                elif os.path.isdir(src):
                    shutil.copytree(src, dst)
                else:
                    raise UserException("unable to copy '{0}'".format(src))

        # Create the tarball.
        sdist_file = sdist_name + '.tar.gz'
        sdist_path = os.path.abspath(os.path.join(sdist_directory, sdist_file))

        saved_cwd = os.getcwd()
        os.chdir(self.build_dir)

        import tarfile

        tf = tarfile.open(sdist_path, 'w:gz', format=tarfile.PAX_FORMAT)
        tf.add(sdist_name)
        tf.close()

        os.chdir(saved_cwd)

        self._remove_build_dir()

        return sdist_file

    def build_wheel(self, wheel_directory='.'):
        """ Build a wheel for the project and return the name of the wheel
        file.
        """

        # Create the wheel tag.
        major_minor = '{}{}'.format((sys.hexversion >> 24) & 0xff,
                (sys.hexversion >> 16) & 0xff)
        wheel_tag = 'cp{}'.format(major_minor)

        try:
            wheel_tag += '-cp' + major_minor + sys.abiflags
        except AttributeError:
            wheel_tag += '-none'

        if sys.platform == 'win32':
            wheel_tag += '-win32' if is_32 else '-win_amd64'
        elif sys.platform == 'darwin':
            wheel_tag += '-macosx_10_6_intel'
        else:
            # We assume that Linux wheels are PEP 513 compatible so that it can
            # be uploaded to PyPI.
            wheel_tag += '-manylinux1_x86_64'

        # Create a temporary directory for the wheel.
        wheel_build_dir = os.path.join(self.build_dir, 'wheel')
        os.mkdir(wheel_build_dir)

        # Build and copy the wheel contents.
        self._build_and_install_modules(wheel_build_dir, wheel_tag=wheel_tag)

        wheel_file = '{}-{}-{}.whl'.format(self.name.replace('-', '_'),
                self.version, wheel_tag)
        wheel_path = os.path.abspath(os.path.join(wheel_directory, wheel_file))

        # Create the .whl file.
        saved_cwd = os.getcwd()
        os.chdir(wheel_build_dir)

        from zipfile import ZipFile, ZIP_DEFLATED

        with ZipFile(wheel_path, 'w', compression=ZIP_DEFLATED) as zf:
            for dirpath, _, filenames in os.walk('.'):
                for filename in filenames:
                    # This will result in a name with no leading '.'.
                    name = os.path.relpath(os.path.join(dirpath, filename))

                    zf.write(name)

        os.chdir(saved_cwd)

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
        project._finalise_configuration(tool)

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
        project.update()

        os.chdir(project.root_dir)

        # Make sure the configuration is correct after any user supplied script
        # has messed with it.
        project._verify()

        return project

    def get_options(self):
        """ Return a sequence of configurable options. """

        return self._options + self._multibindings_options

    def information(self, message):
        """ Print an informational message if verbose messages are enabled. """

        if self.verbose:
            print(message)

    def install(self):
        """ Install the project. """

        self._build_and_install_modules(self.target_dir)

        self._remove_build_dir()

    def progress(self, message):
        """ Print a progress message if verbose messages are enabled. """

        self.information(message + '...')

    def update(self):
        """ This should be re-implemented by any user supplied sub-class to
        carry out any updates to the configuration (including the removal of
        entire sets of bindings) as required.  The current directory will be
        the temporary build directory.
        """

        # This default implementation does nothing.

    def verify_configuration(self):
        """ Verify that the configuration is complete and consistent. """

        # Make sure we have a valid ABI version.
        self.abi_version = resolve_abi_version(self.abi_version)

        # Get the builder factory.
        self.builder = self._import_callable(self.builder, 'tool.sip.project',
                'builder')

        # Check we have the name of the sip module if it is shared.
        if len(self.bindings) > 1 and not self.sip_module:
            raise PyProjectOptionException('tool.sip.project', 'sip-module',
                    "must be defined when the project contains multiple sets "
                    "of bindings")

        # Check we will have the sip.h file for any shared sip module.
        if self.sip_module:
            if self.sip_h not in ('generate', 'installed'):
                self.sip_h = os.path.abspath(self.sip_h)

                if os.path.commonprefix([self.sip_h, self.root_dir]) != self.root_dir:
                    raise PyProjectOptionException('tool.sip.project',
                            'sip-h',
                            "the sip.h file must be in the '{0}' directory or "
                            "a sub-directory".format(self.root_dir))

        # Verify the types of any install extras.
        def bad_extras():
            raise PyProjectOptionException('tool.sip.project',
                    'install-extras',
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

        # Do this again in case any user script has modified it.
        self._validate_enabled_bindings()

    def _build_and_install_modules(self, target_dir, wheel_tag=None):
        """ Build and install the extension modules and create the .dist-info
        directory.
        """

        installed = []

        # If we are using a copy of sip.h that is not already installed then
        # install it.
        if self.sip_module:
            bindings_dir = self._get_installed_bindings_dir(target_dir)
            os.makedirs(bindings_dir, exist_ok=True)
        else:
            bindings_dir = None

        for bindings, module, module_fn, pyi_file in self._build_modules():
            # Get the name of the individual module's directory.
            module_name_parts = module.split('.')
            parts = [target_dir]
            parts.extend(module_name_parts[:-1])
            module_dir = os.path.join(*parts)
            os.makedirs(module_dir, exist_ok=True)

            # Copy the extension module.
            installed.append(self._install_file(module_fn, module_dir))

            # Copy any .pyi file.
            if pyi_file is not None:
                installed.append(self._install_file(pyi_file, module_dir))

            # Copy the .sip files.
            if bindings_dir is not None:
                installed.extend(
                        self._install_sip_files(bindings, bindings_dir))

        # Install anything else the user has specified.
        for extra in self.install_extras:
            src = extra[0]

            if len(extra) == 1:
                dst_dir = target_dir
            else:
                dst_dir = extra[1]

                if os.path.isabs(dst_dir):
                    # Quietly ignore absolute pathnames when creating a wheel.
                    if wheel_tag is not None:
                        continue
                else:
                    dst_dir = os.path.join(target_dir, dst_dir)

            os.makedirs(dst_dir, exist_ok=True)

            if os.path.isfile(src):
                installed.append(self._install_file(src, dst_dir))
            elif os.path.isdir(src):
                dst = os.path.join(dst_dir, os.path.basename(src))

                shutil.copytree(src, dst,
                        copy_function=lambda s, d: installed.append(
                                shutil.copy2(s, d)))
            else:
                raise UserException("unable to install '{0}'".format(src))

        create_distinfo(self, installed, target_dir, wheel_tag=wheel_tag)

    def _build_modules(self):
        """ Build the enabled extension modules and return a 4-tuple of the
        bindings object, the fully qualified module name, its pathname and the
        pathname of any .pyi file.
        """

        # Generate the sip.h file for any shared sip module.
        if self.sip_module:
            copy_sip_h(self.abi_version, self.build_dir, self.sip_module)

        # Get the list of directories to search for .sip files.
        sip_include_dirs = list(self.sip_include_dirs)

        if self.sip_module:
            sip_include_dirs.append(self.sip_files_dir)
            sip_include_dirs.append(
                    self._get_installed_bindings_dir(self.target_dir))

        set_globals(SIP_VERSION, SIP_VERSION_STR, UserException,
                sip_include_dirs)

        # Build each enabled set of bindings.
        modules = []

        for bindings_name in self.enable:
            bindings = self.bindings[bindings_name]

            self.progress(
                    "Generating the bindings from {0}".format(
                            bindings.sip_file))

            # Generate the source code.
            generated = bindings.generate()

            if generated.pyi_file is None:
                pyi_file = None
            else:
                pyi_file = os.path.join(generated.sources_dir,
                        generated.pyi_files)

            # Compile the generated code.
            builder = self.builder(generated.sources_dir, generated.sources,
                    debug=bindings.debug, define_macros=bindings.define_macros,
                    include_dirs=generated.include_dirs,
                    libraries=bindings.libraries,
                    library_dirs=bindings.library_dirs)

            if not isinstance(builder, Builder):
                raise PyProjectOptionException('tool.sip.project', 'builder',
                        "did not return a Builder instance")

            self.progress(
                    "Building the bindings for {0}".format(generated.name))

            saved_cwd = os.getcwd()
            os.chdir(generated.sources_dir)
            extension_module = builder.build_extension_module(self,
                    generated.name)
            os.chdir(saved_cwd)

            modules.append(
                    (bindings, generated.name, extension_module, pyi_file))

        return modules

    def _configure_from_command_line(self, tool, description):
        """ Update the configuration from the user supplied command line. """

        from argparse import SUPPRESS
        from ..argument_parser import ArgumentParser

        parser = ArgumentParser(description, argument_default=SUPPRESS)

        # Add the user configurable options to the parser.
        all_options = {}
        
        options = self._options
        if len(self.bindings) > 1:
            options += self._multibindings_options

        self.add_command_line_options(parser, tool, all_options,
                options=options)

        for bindings in self.bindings.values():
            bindings.add_command_line_options(parser, tool, all_options)

        # Parse the arguments and update the corresponding configurables.
        args = parser.parse_args()

        for option, configurables in all_options.items():
            for configurable in configurables:
                if hasattr(args, option.dest):
                    setattr(configurable, option.name,
                            getattr(args, option.dest))

    @staticmethod
    def _ensure_subdirs_exist(file_path):
        """ Ensure that the parent directories of a file path exist. """

        file_dir = os.path.dirname(file_path)
        if file_dir != '':
            os.makedirs(file_dir, exist_ok=True)

    def _finalise_configuration(self, tool):
        """ Finalise the project's configuration. """

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

        self.apply_defaults()

        for bindings in self.bindings.values():
            bindings.apply_defaults()

        self._validate_enabled_bindings()

    def _get_installed_bindings_dir(self, target_dir):
        """ Return the name of the installed bindings directory. """

        name_parts = self.sip_module.split('.')
        name_parts.insert(0, target_dir)
        name_parts[-1] = 'bindings'

        return os.path.join(*name_parts)

    @staticmethod
    def _import_callable(callable_name, section_name, name):
        """ Return a callable imported from a location specified as a value in
        the pyproject.toml file.
        """

        # Extract the module and object names.
        parts = callable_name.split(':')
        if len(parts) != 2:
            raise PyProjectOptionException(section_name, name,
                    "must be defined as 'module:name'")

        module_name, obj_name = parts

        # Try and import the module.
        try:
            module = importlib.import_module(module_name)
        except ImportError as e:
            raise PyProjectOptionException(section_name, name,
                    "unable to import '{0}'".format(module_name),
                    detail=str(e))

        # Get the callable.
        obj = getattr(module, obj_name)
        if obj is None:
            raise PyProjectOptionException(section_name, name,
                    "'{0}' module has no callable '{1}'".format(module_name,
                            obj_name))

        return obj

    @staticmethod
    def _install_file(fname, module_dir):
        """ Install a file into a module-specific directory and return the
        pathname of the installed file.
        """

        target_fn = os.path.join(module_dir, os.path.basename(fname))
        shutil.copyfile(fname, target_fn)

        return target_fn

    def _install_sip_files(self, bindings, target_dir):
        """ Install the .sip files for a set of bindings in a target directory
        and return a list of the installed files.
        """

        installed = []

        rel_sip_files_dir = os.path.relpath(self.sip_files_dir, self.root_dir)
        target_sip_files_dir = os.path.join(target_dir, rel_sip_files_dir)

        for sip_file in bindings.get_sip_files():
            source_sip_file = os.path.join(self.sip_files_dir, sip_file)
            target_sip_file = os.path.join(target_sip_files_dir, sip_file)

            self._ensure_subdirs_exist(target_sip_file)
            shutil.copyfile(source_sip_file, target_sip_file)

            installed.append(target_sip_file)

        return installed

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

        metadata = pyproject.get_section('tool.sip', required=True)
        for md_name, md_value in metadata.items():
            # Ignore sub-sections.
            if pyproject.is_section(md_value):
                continue

            if not isinstance(md_value, str):
                raise PyProjectOptionException('tool.sip', md_name,
                        "must be a string")

            md_name = md_name.lower()
            if md_name == 'name':
                if not md_value.isidentifier():
                    raise PyProjectOptionException('tool.sip', 'name',
                            "'{0}' is an invalid project name".format(
                                    md_value))

                self.name = md_value
            elif md_name == 'version':
                self.version = md_value
            elif md_name == 'metadata-version':
                metadata_version = md_value

            self.metadata[md_name] = md_value

        if self.name is None:
            raise PyProjectUndefinedOptionException('tool.sip', 'name')

        if self.version is None:
            self.version = '0.1'
            self.metadata['version'] = self.version

        if metadata_version is None:
            # Default to PEP 566.
            self.metadata['metadata-version'] = '2.1'

        # This is cosmetic.
        for name in ('version', 'name', 'metadata-version'):
            self.metadata.move_to_end(name, last=False)

        # Get the project configuration.
        project_section = pyproject.get_section('tool.sip.project')
        if project_section is not None:
            self.configure(project_section, 'tool.sip.project')

        # Get configuration for each set of bindings.
        bindings_sections = pyproject.get_section('tool.sip.bindings')
        if bindings_sections is None:
            bindings_sections = []

        for section in bindings_sections:
            bindings = Bindings(self)
            bindings.configure(section, 'tool.sip.bindings')
            self.bindings[bindings.name] = bindings

        # Add a default set of bindings if none were defined.
        if not self.bindings:
            bindings = Bindings(self)
            bindings.name = self.name
            self.bindings[bindings.name] = bindings

    def _validate_enabled_bindings(self):
        """ Check the enabled bindings are valid and remove any disabled ones.
        """

        if self.enable:
            # Check that the enable bindings are valid.
            for bindings_name in self.enable:
                if bindings_name not in self.bindings:
                    raise UserException(
                            "unknown bindings '{0}'".format(bindings_name))
        else:
            self.enable = list(self.bindings.keys())

        for disabled in self.disable:
            try:
                self.enable.remove(disabled)
            except ValueError:
                pass

    def _verify(self):
        """ Verify that the configuration is complete and consistent. """

        self.verify_configuration()

        for bindings in self.bindings.values():
            bindings.verify_configuration()
