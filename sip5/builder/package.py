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


from distutils.sysconfig import get_python_lib
import os
import shutil
import sys
import warnings

from ..exceptions import handle_exception, UserException
from ..module.module import copy_nonshared_sources

from .bindings import Bindings
from .builder import DistutilsBuilder
from .configurable import Configurable, Option
from .distinfo import create_distinfo
from .install import install_module
from .pyproject import (PyProject, PyProjectException,
        PyProjectOptionException, PyProjectUndefinedOptionException)


class Package(Configurable):
    """ Encapsulate a package containing one or more sets of bindings. """

    # The configurable options.
    _options = (
        Option('sip_h_dir'),
        Option('sip_module'),

        Option('verbose', option_type=bool,
                help="enable verbose progress messages"),
        Option('build_dir', default='build', help="the build directory",
                metavar="DIR"),
        Option('target_dir', default=get_python_lib(plat_specific=1),
                help="the target installation directory", metavar="DIR"),
        Option('_action', choices=('build', 'install', 'sdist', 'wheel'),
                default='build', action=True)
    )

    def __init__(self):
        """ Initialise the package. """

        super().__init__()

        # The current directory should contain pyproject.toml.
        self.root_dir = os.getcwd()

        self._bindings = []

    def build(self):
        """ Build the package as specified by the user. """

        try:
            # Configure the package.
            self.setup(PyProject(), enable_command_line_options=True)

            # Perform the required action.
            if self._action == 'build':
                self._build()
            elif self._action == 'install':
                self._install()
            elif self._action == 'sdist':
                self._create_sdist()
            elif self._action == 'wheel':
                self.build_wheel()

        except Exception as e:
            handle_exception(e)

    def build_wheel(self, wheel_directory='.'):
        """ Build a wheel for the package and return the name of the wheel
        file.
        """

        self.progress("Creating a wheel")

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

        # Install the wheel contents.
        installed = []
        for module, module_fn in self._build_modules():
            installed.append(install_module(module, module_fn, wheel_build_dir))

        create_distinfo(self, installed, wheel_build_dir, wheel_tag=wheel_tag)

        wheel_file = '{}-{}-{}.whl'.format(self.name, self.version, wheel_tag)
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

        self.information("The wheel has been created.")

        return wheel_file

    def get_options(self):
        """ Return a sequence of configurable options. """

        return self._options

    def information(self, message):
        """ Print an informational message if verbose messages are enabled. """

        if self.verbose:
            print(message)

    def progress(self, message):
        """ Print a progress message if verbose messages are enabled. """

        self.information(message + '...')

    def setup(self, pyproject, enable_command_line_options=False):
        """ Perform all the setup prior to performing the required action. """

        # Create the initial configuration from the pyproject.toml file.
        self._set_initial_configuration(pyproject)

        # If enabled, allow the user to supply command line options for (so far
        # unspecified) parts of the configuration.
        if enable_command_line_options:
            self._configure_from_command_line()

        # Make sure the configuration is complete.
        self._finalise_configuration()

        if not self.verbose:
            warnings.simplefilter('ignore', UserWarning)

        # Make sure we have a clean build directory and make it current.
        self.progress("Creating the build directory")

        self.build_dir = os.path.abspath(self.build_dir)
        shutil.rmtree(self.build_dir, ignore_errors=True)
        os.mkdir(self.build_dir)
        os.chdir(self.build_dir)

        # Allow a sub-class (in a user supplied script) to make any updates to
        # the configuration.
        self.update()

        os.chdir(self.root_dir)

        # Make sure the configuration is correct after any user supplied script
        # has messed with it.
        self._verify()

    def update(self):
        """ This should be re-implemented by any user supplied sub-class to
        carry out any updates to the configuration (including the removal of
        entire sets of bindings) as required.  The current directory will be
        the temporary build directory.
        """

        # This default implementation does nothing.

    def verify_configuration(self):
        """ Verify that the configuration is complete and consistent. """

        if len(self._bindings) > 1 and not self.sip_module:
            raise PyProjectOptionException('tool.sip.package', 'sip_module',
                    "must be define when the package contains multiple sets "
                    "of bindings")

        # Check we have the sip.h file for any shared sip module.
        if self.sip_module:
            if not self.sip_h_dir:
                raise PyProjectOptionException('tool.sip.package', 'sip_h_dir',
                        "must be define when using a shared sip module")

            self.sip_h_dir = os.path.abspath(self.sip_h_dir)

    def _build(self):
        """ Build the package.  This is really only for debugging purposes. """

        self.progress("Building the package")

        self._build_modules()

        self.information("The package has been built.")

    def _create_sdist(self):
        """ Create an sdist for the package. """

        self.progress("Creating an sdist")

        # The sdist name.
        sdist_name = '{}-{}'.format(self.name, self.version)

        # Create the sdist root directory.
        sdist_dir = os.path.join(self.build_dir, sdist_name)
        os.mkdir(sdist_dir)

        # Copy the pyproject.toml file.
        shutil.copy(os.path.join(self.root_dir, 'pyproject.toml'), sdist_dir)

        # Create the PKG-INFO file.
        #with open(os.path.join(sdist_dir, 'PKG-INFO'), 'wt') as pi:
        #    # TODO
        #    pi.write('Metadata-Version: 1.0\n')
        #    pi.write('Name: {}\n'.format(self.name))
        #    pi.write('Version: {}\n'.format(self.version))

        # Copy in the build script.
        # TODO: the name should be worked out from pyproject.toml (but what if
        # the script imports other modules?)
        #if os.path.basename(sys.argv[0]) != 'build.py':
        #    raise UserException(
        #            "this script must be called 'build.py' when creating an "
        #            "sdist")

        #shutil.copy(os.path.join(self.root_dir, 'build.py'), sdist_dir)

        # Copy in the .sip files for each set of bindings.
        for bindings in self._bindings:
            for sip_file in bindings.get_sip_files():
                # Make sure any sub-directories exist.
                sip_dir = os.path.dirname(sip_file)
                if sip_dir != '':
                    os.makedirs(sip_dir, exist_ok=True)
                else:
                    sip_dir = sdist_dir

                shutil.copy(os.path.join(self.root_dir, sip_file), sip_dir)

        # Copy in anything else the user has asked for.
        # TODO

        # Create the tarball.
        sdist_file = os.path.abspath(sdist_name + '.tar.gz')

        saved_cwd = os.getcwd()
        os.chdir(self.build_dir)

        import tarfile

        tf = tarfile.open(sdist_file, 'w:gz', format=tarfile.PAX_FORMAT)
        tf.add(sdist_name)
        tf.close()

        os.chdir(saved_cwd)

        self._remove_build_dir()

        self.information("The sdist has been created.")

    def _install(self):
        """ Install the package. """

        self.progress("Installing the package")

        target_dir = self.target_dir

        installed = []
        for module, module_fn in self._build_modules():
            installed.append(install_module(module, module_fn, target_dir))

        create_distinfo(self, installed, target_dir)

        self._remove_build_dir()

        self.information("The package has been installed.")

    def _build_modules(self):
        """ Build the extension modules and return ia 2-tuple of the fully
        qualified module name and the pathname.
        """

        modules = []

        for bindings in self._bindings:
            self.progress(
                    "Generating the bindings from {0}".format(
                            bindings.sip_file))

            # Generate the source code.
            generated = bindings.generate()

            # Compile the generated code.
            # TODO: allow the package to define a default builder and the
            # bindings to specify a specific builder.
            builder = DistutilsBuilder(generated.sources_dir,
                    generated.sources, debug=bindings.debug,
                    define_macros=bindings.define_macros,
                    include_dirs=generated.include_dirs,
                    libraries=bindings.libraries,
                    library_dirs=bindings.library_dirs)

            self.progress(
                    "Building the bindings for {0}".format(generated.name))

            modules.append(
                    (generated.name,
                            builder.build_extension_module(self,
                                    generated.name)))

        return modules

    def _configure_from_command_line(self):
        """ Update the configuration from the user supplied command line. """

        from argparse import ArgumentParser, SUPPRESS

        parser = ArgumentParser(argument_default=SUPPRESS)

        parser.add_argument('-V', '--version', action='version',
                version=self.version)

        # Add the user configurable options to the parser.
        all_options = {}
        
        self.add_command_line_options(parser, all_options)

        for bindings in self._bindings:
            bindings.add_command_line_options(parser, all_options)

        # Parse the arguments and update the corresponding configurables.
        args = parser.parse_args()

        for option, configurables in all_options.items():
            for configurable in configurables:
                if hasattr(args, option.dest):
                    setattr(configurable, option.name,
                            getattr(args, option.dest))

    def _finalise_configuration(self):
        """ Finalise the package's configuration. """

        self.apply_defaults()

        for bindings in self._bindings:
            bindings.apply_defaults()

    def _remove_build_dir(self):
        """ Remove the build directory. """

        self.progress("Removing the build directory")

        shutil.rmtree(self.build_dir)

    def _set_initial_configuration(self, pyproject):
        """ Set the package's initial configuration. """

        # Get the metadata and extract the name and version.
        self.metadata = pyproject.get_section('tool.sip.metadata',
                required=True)

        self.name = None
        self.version = '1.0'

        for md_name, md_value in self.metadata.items():
            md_name = md_name.lower()
            if md_name == 'name':
                # Normalise the PyPI name.
                self.name = md_value.replace('-', '_')
            elif md_name == 'version':
                self.version = md_value

        if self.name is None:
            raise PyProjectUndefinedOptionException('tool.sip.metadata',
                    'Name')

        self.configure(pyproject, 'tool.sip.package')

        # Get the bindings.
        for bindings_name in pyproject.get_all_subsections('tool.sip.bindings'):
            section_name = bindings_name + '.' + bindings_name

            bindings = Bindings(bindings_name, self)
            bindings.configure(pyproject, section_name)

            self._bindings.append(bindings)

        # See if we can add a default set of bindings.
        if not self._bindings:
            # If there is a .sip file that might create bindings with the same
            # name as the package then use it.
            sip_file = self.name + '.sip'
            if os.path.isfile(sip_file):
                bindings = Bindings(self.name, self)
                bindings.sip_file = sip_file
                self._bindings.append(bindings)
            else:
                raise PyProjectException(
                        "no bindings have been specified and there is no file "
                        "'{0}'".format(sip_file))

    def _verify(self):
        """ Verify that the configuration is complete and consistent. """

        self.verify_configuration()

        for bindings in self._bindings:
            bindings.verify_configuration()
