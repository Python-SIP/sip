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


from abc import ABC, abstractmethod
from argparse import ArgumentParser, SUPPRESS

from ..exceptions import UserException


class Configurable(ABC):
    """ An abstract mixin for an object that can be configured by the user. """

    def configure(self, configuration):
        """ Apply the user supplied configuration. """

        for option in self.get_options():
            name = option.name

            if hasattr(configuration, name):
                setattr(self, name, getattr(configuration, name))

    @classmethod
    @abstractmethod
    def get_options(cls):
        """ Get the user-configurable options. """


class Option:
    """ Encapsulate a configuration option. """

    # This is used to make sure each option (even if they are handling the same
    # attribute) has a unique 'dest'.
    option_nr = 0

    def __init__(self, name, *, argument_name=None, option_type=str, choices=None, help='', metavar=None, action=False, inverted=False):
        """ Initialise the object. """

        # The argument name defaults to the transformed option name.
        if argument_name is None:
            argument_name = name.replace('_', '-')

            if inverted:
                argument_name = 'no-' + argument_name

            if not action:
                argument_name = '--' + argument_name

            # Remove any plural.
            if option_type is list and argument_name.endswith('s'):
                argument_name = argument_name[:-1]

        if action:
            # argparse doesn't allow a 'dest' to be specified for positional
            # arguments.
            self.dest = name
        else:
            self.dest = 'd' + str(type(self).option_nr)
            type(self).option_nr += 1

        self.name = name
        self.argument_name = argument_name
        self.option_type = option_type
        self.choices = choices
        self.help = help
        self.metavar = metavar
        self.action = action
        self.inverted = inverted


class ConfigurationError(UserException):
    """ An exception that encapsulates an error in a configuration file. """

    def __init__(self, configuration, line_nr, text, detail=''):
        """ Initialise the object. """

        exception_text = "{}:{}:{}".format(configuration, line_nr, text)

        super().__init__(exception_text, detail)


class ConfigurationParser:
    """ The implementation of a configuration parser that handles user supplied
    command line arguments and an optional configuration file.
    """

    def __init__(self, version, enable_configuration_file):
        """ Initialise the object. """

        self._parser = ArgumentParser(argument_default=SUPPRESS)

        self._parser.add_argument('-V', '--version', action='version',
                version=version)

        if enable_configuration_file:
            self._parser.add_argument('--configuration',
                    help="the configuration file containing default options",
                    metavar='FILE')

        self._enable_configuration_file = enable_configuration_file
        self._options = []

    def add_options(self, configurable):
        """ Add a configurable's options to the parser. """

        for option in configurable.get_options():
            if option.option_type is bool:
                self._parser.add_argument(option.argument_name,
                        dest=option.dest,
                        action=('store_false' if option.inverted
                                else 'store_true'),
                        help=option.help)
            elif option.option_type is list:
                self._parser.add_argument(option.argument_name,
                        dest=option.dest, choices=option.choices,
                        action='append', help=option.help,
                        metavar=option.metavar)
            elif option.action:
                self._parser.add_argument(option.argument_name,
                        type=option.option_type, choices=option.choices,
                        help=option.help, metavar=option.metavar)
            else:
                self._parser.add_argument(option.argument_name,
                        dest=option.dest, type=option.option_type,
                        choices=option.choices, help=option.help,
                        metavar=option.metavar)

            self._options.append(option)

    def parse(self):
        """ Parse the configuration and return a mapping of name/value pairs.
        """

        args = self._parser.parse_args()

        # Convert the parsed arguments to the option names.
        configuration = type(args)()

        for option in self._options:
            if hasattr(args, option.dest):
                if hasattr(configuration, option.name):
                    raise UserException(
                            "error: the '{}' option is incompatible with another "
                            "specified option".format(option.argument_name))

                setattr(configuration, option.name, getattr(args, option.dest))

        if hasattr(configuration, 'configuration'):
            self._update_from_configuration_file(configuration)
            del configuration.configuration

        return configuration

    def _update_from_configuration_file(self, configuration):
        """ Update a configuration from the contents of a configuration file.
        """

        configuration_file = configuration.configuration

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
                    raise ConfigurationError(configuration_file, line_nr,
                            "name = value format expected")

                name = parts[0].strip()
                value = parts[1].strip()

                if name == '' or value == '':
                    raise ConfigurationError(configuration_file, line_nr,
                            "empty name or value")

                for option in self._options:
                    if option.name == name and not option.action:
                        break
                else:
                    raise ConfigurationError(configuration_file, line_nr,
                            "unknown name '{}'".format(name))

                if option.type is bool:
                    cfg_value = value.lower()
                    if cfg_value == 'true':
                        cfg_value = True
                    elif cfg_value == 'false':
                        cfg_value = False
                    else:
                        raise ConfigurationError(configuration_file, line_nr,
                                "error converting '{}'".format(value))
                elif option.type is list:
                    cfg_value = value.split()
                else:
                    try:
                        cfg_value = option.option_type(value)
                    except Exception as e:
                        raise ConfigurationError(configuration_file, line_nr,
                                "error converting '{}'".format(value)) from e

                # Only override the existing configuration if no value had been
                # set.
                if not hasattr(configuration, name) is None:
                    setattr(configuration, name, cfg_value)
