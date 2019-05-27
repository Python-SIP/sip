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

from ..exceptions import handle_exception, UserException

from .configuration import Configurable, ConfigurationParser


class Package:
    """ Encapsulate a package containing one or more sets of bindings. """

    def __init__(self, name, *, version='1.0', enable_configuration_file=True, sip_module=None, sip_h_dir=None, context_factory=None, bindings_factory=None, builder_factory=None):
        """ Initialise the package.

        :param name:
            is the name of the package as it will appear to PyPI.
        :param version:
            is the version number of the pckage as it will appear to PyPI.
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

        self._name = name
        self._version = version
        self.sip_module = sip_module
        self.sip_h_dir = sip_h_dir
        self._context = context_factory()
        self._bindings_factory = bindings_factory
        self._builder_factory = builder_factory

        self._bindings = []

        # Get the configuration.
        try:
            self._configuration = self._configure(enable_configuration_file)
        except Exception as e:
            handle_exception(e)

        # Configure the context.
        if isinstance(self._context, Configurable):
            self._context.configure(self._configuration)

        # The build directory is relative to the current directory.
        self.build_dir = os.path.abspath(self._context.build_dir)

        # The root directory is the one containing this script.
        self.root_dir = os.path.dirname(os.path.abspath(sys.argv[0]))

        # We don't expose the context in the public API.
        self.verbose = self._context.verbose

        if not self.verbose:
            warnings.simplefilter('ignore', UserWarning)

    def add_bindings(self, sip_file):
        """ Add the set of bindings defined by a .sip file to the package. """

        self._bindings.append(sip_file)

    def build(self):
        """ Build the package. """

        try:
            if self._context.action == 'install':
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

    def _configure(self, enable_configuration_file):
        """ Return a mapping of user supplied configuration names and values.
        """

        parser = ConfigurationParser(self._version, enable_configuration_file)

        if isinstance(self._context, Configurable):
            parser.add_options(self._context)

        if issubclass(self._bindings_factory, Configurable):
            parser.add_options(self._bindings_factory)

        if issubclass(self._builder_factory, Configurable):
            parser.add_options(self._builder_factory)

        # Parse the configuration.
        return parser.parse()

    def _create_sdist(self):
        """ Create an sdist for the package. """

        raise NotImplementedError

    def _create_wheel(self):
        """ Create a wheel for the package. """

        modules = self._build_modules()

        raise NotImplementedError

    def _install(self):
        """ Install the package. """

        modules = self._build_modules()

        print("Built:", modules)

    def _build_modules(self):
        """ Build the extension modules and return their pathnames. """

        # If no bindings were explicitly defined then see if there is a .sip
        # file that might define a set that matches the name of the package.
        nr_bindings = len(self._bindings)

        if nr_bindings == 0:
            sip_file = os.path.join(self.root_dir,
                    self._name.split('.')[-1] + '.sip')

            if not os.path.isfile(sip_file):
                raise UserException(
                        "no bindings have been specified and there is no file '{}'".format(sip_file))

            self._bindings = [os.path.relpath(sip_file, self.root_dir)]
        elif nr_bindings > 1 and self.sip_module is None:
            raise UserException(
                    "the name of a shared sip module must be specified when the package contains multiple sets of bindings")

        # Check we have the sip.h file for any shared sip module.
        if self.sip_module is not None and self.sip_h_dir is None:
            raise UserException(
                    "the directory containing sip.h must be specified when using a shared sip module")

        # Make sure we have a clean build directory.
        shutil.rmtree(self.build_dir, ignore_errors=True)
        os.mkdir(self.build_dir)

        # Create sip.h if we aren't using a shared sip module.
        if self.sip_module is None:
            from ..module.module import module

            module(self.sip_module, include_dir=self.build_dir)
            sip_h_dir = self.build_dir
        else:
            sip_h_dir = os.path.abspath(self.sip_h_dir)

        sip_h_dir = os.path.relpath(sip_h_dir)

        # Build each set of bindings.
        modules = []

        for sip_file in self._bindings:
            bindings = self._bindings_factory(sip_file)

            # Generate the source code.
            locations = bindings.generate(self)

            # Add the sip module code if it is not shared.
            if self.sip_module is None:
                # TODO: implement this in the module sub-package
                module_dir = os.path.join(
                        os.path.dirname(os.path.dirname(__file__)), 'module',
                        'source')

                for fn in os.listdir(module_dir):
                    if fn.endswith('.c') or fn.endswith('.cpp') or fn.endswith('.h'):
                        src_fn = os.path.join(module_dir, fn)
                        dst_fn = os.path.join(locations.sources_dir, fn)
                        shutil.copyfile(src_fn, dst_fn)

                        if not fn.endswith('.h'):
                            locations.sources.append(dst_fn)

            include_dirs = [locations.sources_dir]
            if sip_h_dir != locations.sources_dir:
                include_dirs.append(sip_h_dir)

            # Compile the generated code.
            builder = self._builder_factory(locations.sources_dir,
                    locations.sources, include_dirs)

            modules.append(builder.build_extension_module(bindings, self))

        return modules
