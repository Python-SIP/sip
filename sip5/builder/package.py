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
import shutil
import sys
import warnings
import zipfile

from ..exceptions import handle_exception, UserException
from ..module.module import copy_nonshared_sources

from .configuration import Configurable, ConfigurationParser
from .distinfo import create_distinfo
from .install import install_module


class Package:
    """ Encapsulate a package containing one or more sets of bindings. """

    def __init__(self, name, *, version='1.0', command_line_configuration=True, enable_configuration_file=True, sip_module=None, sip_h_dir=None, context_factory=None, bindings_factory=None, builder_factory=None):
        """ Initialise the package.

        :param name:
            is the name of the package as it will appear to PyPI.
        :param version:
            is the version number of the pckage as it will appear to PyPI.
        :param command_line_configuration:
            is set the build can be configured using command line options.
            Command line options may not work as expected when using other
            build systems (e.g. setuptools).
        :param enable_configuration_file:
            is set if the user can specify the name of a configuration file on
            the command line that provides default values of all other command
            line options.
        :param sip_module:
            is the fully qualified name of the sip module shared by all sets of
            bindings that are part of this package.  By default it is assumed
            that the package will contain a single set of bindings and there
            will be no shared sip module.
        :param sip_h_dir:
            is the name of the directory containing the sip.h file that defines
            the ABI implemented by a shared sip module.  It is ignored if there
            is no shared sip module, otherwise it must be specified.
        :param context_factory:
            is a callable that must return a Context instance.  If the callable
            is a sub-class of the Configurable type then it may define
            additional command line options.  The default callable returns a
            ConfigurableContext instance.
        :param bindings_factory:
            is a callable that must return a Bindings instance.  If the
            callable is a sub-class of the Configurable type then it may define
            additional command line options.  The default callable returns a
            BindingsContext instance.
        :param builder_factory:
            is a callable that must return a Context instance.  If the callable
            is a sub-class of the Configurable type then it may define
            additional command line options.  The default callable returns a
            ConfigurableContext instance.
        """

        # Provide default factories.
        if context_factory is None:
            from .context import ConfigurableContext as context_factory

        if bindings_factory is None:
            from .bindings import ConfigurableBindings as bindings_factory

        if builder_factory is None:
            from .builder import DistutilsBuilder as builder_factory

        # Normalise the PyPI name.
        name = name.replace('-', '_')

        self.name = name
        self.version = version
        self.sip_module = sip_module
        self.sip_h_dir = sip_h_dir
        self._context = context_factory()
        self._bindings_factory = bindings_factory
        self._builder_factory = builder_factory

        self._bindings = []

        # Get the configuration.
        if command_line_configuration:
            try:
                self._configuration = self._parse_configuration(
                        enable_configuration_file)
            except Exception as e:
                handle_exception(e)
        else:
            self._configuration = None

        # Configure the context.
        self._configure(self._context)

        # The build directory is relative to the current directory.
        self.build_dir = os.path.abspath(self._context.build_dir)

        # The root directory is the one containing this script.
        self.root_dir = os.path.dirname(os.path.abspath(sys.argv[0]))

        # We don't expose the context in the public API.
        self.verbose = self._context.verbose

        if not self.verbose:
            warnings.simplefilter('ignore', UserWarning)

    def add_bindings(self, sip_file, **bindings_args):
        """ Add the set of bindings defined by a .sip file to the package. """

        bindings = self._bindings_factory(self, sip_file, **bindings_args)
        self._configure(bindings)

        self._bindings.append(bindings)

        return bindings

    def build(self):
        """ Build the package. """

        try:
            if self._context.action == 'build':
                self._build()
            elif self._context.action == 'install':
                self._install()
            elif self._context.action == 'sdist':
                self._create_sdist()
            elif self._context.action == 'wheel':
                self._create_wheel()

        except Exception as e:
            handle_exception(e)

    def information(self, message):
        """ Print an informational message if verbose messages are enabled. """

        if self.verbose:
            print(message)

    def progress(self, message):
        """ Print a progress message if verbose messages are enabled. """

        self.information(message + '...')

    def _build(self):
        """ Build the package.  This is really only for debugging purposes. """

        self.progress("Building the package")

        self._create_build_dir()
        self._build_modules()

        self.information("The package has been built.")

    def _create_sdist(self):
        """ Create an sdist for the package. """

        raise NotImplementedError

    def _create_wheel(self):
        """ Create a wheel for the package. """

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
        self._create_build_dir()
        wheel_dir = os.path.join(self.build_dir, 'wheel')
        shutil.rmtree(wheel_dir, ignore_errors=True)
        os.mkdir(wheel_dir)

        # Install the wheel contents.
        installed = []
        for module, module_fn in self._build_modules():
            installed.append(install_module(module, module_fn, wheel_dir))

        create_distinfo(self, installed, wheel_dir, wheel_tag=wheel_tag)

        wheel_file = os.path.abspath(
                '{}-{}-{}.whl'.format(self.name, self.version, wheel_tag))

        # Create the .whl file.
        saved_cwd = os.getcwd()
        os.chdir(wheel_dir)

        with zipfile.ZipFile(wheel_file, 'w', compression=zipfile.ZIP_DEFLATED) as zf:
            for dirpath, _, filenames in os.walk('.'):
                for filename in filenames:
                    # This will result in a name with no leading '.'.
                    name = os.path.relpath(os.path.join(dirpath, filename))

                    zf.write(name)

        os.chdir(saved_cwd)

        self._remove_build_dir()

        self.information("The wheel has been created.")

    def _install(self):
        """ Install the package. """

        self.progress("Installing the package")

        self._create_build_dir()

        target_dir = self._context.target_dir

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
            builder = self._builder_factory(generated.sources_dir,
                    generated.sources, debug=bindings.debug,
                    define_macros=bindings.define_macros,
                    include_dirs=generated.include_dirs,
                    libraries=bindings.libraries,
                    library_dirs=bindings.library_dirs)
            self._configure(builder)

            self.progress(
                    "Building the bindings for {0}".format(generated.name))

            modules.append(
                    (generated.name,
                            builder.build_extension_module(self,
                                    generated.name)))

        return modules

    def _configure(self, obj):
        """ Configure an object if it is configurable. """

        if self._configuration is not None and isinstance(obj, Configurable):
            obj.configure(self._configuration)

    def _create_build_dir(self):
        """ Create the build directory, first checking that all the
        prerequisites have been met.
        """

        # If no bindings were explicitly defined then see if there is a .sip
        # file that might define a set that matches the name of the package.
        nr_bindings = len(self._bindings)

        if nr_bindings == 0:
            sip_file = os.path.join(self.root_dir,
                    self.name.split('.')[-1] + '.sip')

            if not os.path.isfile(sip_file):
                raise UserException(
                        "no bindings have been specified and there is no file '{0}'".format(sip_file))

            sip_file = os.path.relpath(sip_file, self.root_dir)
            bindings = self._bindings_factory(self, sip_file)
            self._configure(bindings)
            self._bindings.append(bindings)

        elif nr_bindings > 1 and self.sip_module is None:
            raise UserException(
                    "the name of a shared sip module must be specified when the package contains multiple sets of bindings")

        # Check we have the sip.h file for any shared sip module.
        if self.sip_module is not None:
            if self.sip_h_dir is None:
                raise UserException(
                        "the directory containing sip.h must be specified when using a shared sip module")

            self.sip_h_dir = os.path.abspath(self.sip_h_dir)

        # Make sure we have a clean build directory.
        self.progress("Creating the build directory")

        shutil.rmtree(self.build_dir, ignore_errors=True)
        os.mkdir(self.build_dir)

    def _parse_configuration(self, enable_configuration_file):
        """ Return a mapping of user supplied configuration names and values.
        """

        parser = ConfigurationParser(self.version, enable_configuration_file)

        if isinstance(self._context, Configurable):
            parser.add_options(self._context)

        if issubclass(self._bindings_factory, Configurable):
            parser.add_options(self._bindings_factory)

        if issubclass(self._builder_factory, Configurable):
            parser.add_options(self._builder_factory)

        # Parse the configuration.
        return parser.parse()

    def _remove_build_dir(self):
        """ Remove the build directory. """

        self.progress("Removing the build directory")

        shutil.rmtree(self.build_dir)
