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


import argparse
import collections
import sys

from .exceptions import Sip5ConfigurationError


# A configuration option.
Option = collections.namedtuple('Option',
        ['name', 'type', 'default', 'command', 'help', 'metavar'])

options = (
    Option('bindings_dir', type=str, default='', command='',
            help="the directory where the generated bindings will be placed",
            metavar="DIR"),
    Option('concatenate', type=int, default=0, command='',
            help="concatenate the generated bindings into N source files",
            metavar="N"),
    Option('debug', type=bool, default=False, command='',
            help="build with debugging symbols", metavar=None),
    Option('docstrings', type=bool, default=True, command='no',
            help="disable the generation of docstrings", metavar=None),
    Option('generate_api', type=bool, default=False, command='',
            help="generate a QScintilla .api file", metavar=None),
    Option('generate_extracts', type=list, default=None, command='',
            help="generate an extract file", metavar="ID:FILE"),
    Option('generate_pyi', type=bool, default=False, command='',
            help="generate a PEP 484 .pyi file", metavar=None),
    Option('protected_is_public', type=bool, default=(sys.platform != 'win32'),
            command='both', help="disable the generation of docstrings",
            metavar=None),
    Option('tracing', type=bool, default=False, command='',
            help="build with tracing support", metavar=None),
    Option('warnings', type=bool, default=False, command='',
            help="enable warning messages", metavar=None),
    Option('warnings_are_errors', type=bool, default=False, command='',
            help="handle warnings as fatal errors", metavar=None),
)


class BindingsConfiguration:
    """ The configuration of the bindings generator. """

    def __init__(self, *, enable_command_line_options=True, option_prefix='', enable_configuration_file=True):
        """ Initialise the object. """

        # Set the configuration defaults.
        for option in options:
            setattr(self, option.name, option.default)

        if enable_command_line_options:
            self._parse_command_line(option_prefix, enable_configuration_file)

    def parse_configuration_file(self, configuration_file):
        """ Parse a configuration file and update the object accordingly. """

        with open(configuration_file) as f:
            line_nr = 0
            for line in f:
                line_nr += 1

                # Ignore blank lines and comments.
                line = line.strip()
                if line == '' or line.startswith('#'):
                    continue

                # Parse the line.
                parts = line.split('=')
                if len(parts) != 2:
                    raise Sip5ConfigurationError(configuration_file, line_nr,
                            "name = value format expected")

                name = parts[0].strip()
                value = parts[1].strip()

                if name == '' or value == '':
                    raise Sip5ConfigurationError(configuration_file, line_nr,
                            "empty name or value")

                attribute_name = self._option_attribute(name)

                for option in options:
                    if option.name == attribute_name:
                        break
                else:
                    raise Sip5ConfigurationError(configuration_file, line_nr,
                            "unknown name '{}'".format(name))

                if option.type is bool:
                    cfg_value = value.lower()
                    if cfg_value == 'true':
                        cfg_value = True
                    elif cfg_value == 'false':
                        cfg_value = False
                    else:
                        raise Sip5ConfigurationError(configuration_file,
                            line_nr, "error converting '{}'".format(value))
                elif option.type is list:
                    cfg_value = value.split()
                else:
                    try:
                        cfg_value = option.type(value)
                    except Exception as e:
                        raise Sip5ConfigurationError(configuration_file,
                            line_nr, "error converting '{}'".format(value)) from e

                setattr(self, option.name, cfg_value)

    @staticmethod
    def _option_attribute(name):
        """ Return the name of the attribute corresponding to a command line
        option or configuration file entry.
        """

        return name.replace('-', '_')

    def _parse_command_line(self, option_prefix, enable_configuration_file):
        """ Parse the command line and any optional configuration file for the
        standard bindings generator configuration.
        """

        # Create the parder.
        arg_parser = argparse.ArgumentParser(add_help=False)
        self._add_configuration_command_line_options(arg_parser, option_prefix,
                enable_configuration_file)
        args, unknown = arg_parser.parse_known_args()

        # Parse any configuration file.
        if enable_configuration_file and args.configuration:
            self.parse_configuration_file(args.configuration)

        # Update the configuration with anything passed on the command line.
        for option in options:
            arg_name = option.name
            if option.type is bool:
                if option.command == 'no':
                    arg_name = 'no_' + arg_name
            elif option.type is list:
                # Assume the option name is plural.
                arg_name = arg_name[:-1]

            cfg_value = getattr(args, arg_name)

            if cfg_value is not None:
                setattr(self, option.name, cfg_value)
            elif option.type is bool and option.command == 'both':
                cfg_value = getattr(args, 'no_' + option.name)

                if cfg_value is not None:
                    setattr(self, option.name, cfg_value)

        # Update sys.argv for the actual build system.
        sys.argv = [sys.argv[0]] + unknown

    @classmethod
    def _add_configuration_command_line_options(cls, parser, option_prefix, enable_configuration_file):
        """ Add the standard command line arguments for configuring the
        bindings generator to an argument parser.
        """

        def command_option(name):
            """ Return a command line option name accounting for any prefix. """

            return '--' + option_prefix + name.replace('_', '-')

        # The configuration file contains values for all options which can then
        # be overridden on the command line.
        if enable_configuration_file:
            parser.add_argument(command_option('configuration'),
                    help="the configuration file containing default options",
                    metavar="FILE")

        # Add the standard options.
        for option in options:
            if option.type is bool:
                if option.command != 'no':
                    parser.add_argument(command_option(option.name),
                            action='store_true', help=option.help)

                if option.command != '':
                    parser.add_argument(command_option('no-' + option.name),
                            action='store_false', help=option.help)
            elif option.type is list:
                # Assume the option name is plural.
                parser.add_argument(command_option(option.name[:-1]),
                        action='append', help=option.help,
                        metavar=option.metavar)
            else:
                parser.add_argument(command_option(option.name),
                        type=option.type, help=option.help,
                        metavar=option.metavar)
