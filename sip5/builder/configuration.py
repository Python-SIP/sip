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
from argparse import ArgumentParser

from .exceptions import ConfigurationError


class Configurable(ABC):
    """ An abstract mixin for an object that can be configured by the user. """

    def configure(self, configuration):
        """ Apply the user supplied configuration. """

        for option in self.get_options():
            value = configuration[option.name]
            if value is not None:
                setattr(self, option.name, value)

    @classmethod
    @abstractmethod
    def get_options(cls):
        """ Return the configurable's options. """


class Option:
    """ Encapsulate a configuration option. """

    # The map of option types and their default values.  All other types have a
    # default of None.
    _type_defaults = {
        str:    '',
        int:    0,
        bool:   False
    }

    def __init__(self, name, *, argument_name=None, option_type=str, choices=None, default=None, help='', metavar=None, action=False, command=''):
        """ Initialise the object. """

        # The argument name defaults to the transformed option name.
        if argument_name is None:
            argument_name = name.replace('_', '-')

            # Remove any plural.
            if option_type is list and argument_name.endswith('s'):
                argument_name = argument_name[:-1]

        # Determine the default value based on the type.
        if default is None:
            default = self._type_defaults.get(option_type)

        self.name = name
        self.argument_name = argument_name
        self.option_type = option_type
        self.choices = choices
        self.default = default
        self.help = help
        self.metavar = metavar
        self.action = action
        self.command = command

    def get_argument(self, negated=False):
        """ Return the command line argument name. """

        prefix = '' if self.action else '--'

        if negated:
            prefix += 'no-'

        return prefix + self.argument_name


class ConfigurationParser:
    """ The implementation of a configuration parser that handles user supplied
    command line arguments and an optional configuration file.
    """

    def __init__(self, version, enable_configuration_file):
        """ Initialise the object. """

        self._parser = ArgumentParser()

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
                if option.command != 'no':
                    parser.add_argument(option.get_argument(),
                            dest=option.name, choices=option.choices,
                            action='store_true', help=option.help)

                if option.command != '':
                    parser.add_argument(option.get_argument(negated=True),
                            dest=option.name, choices=option.choices,
                            action='store_false', help=option.help)
            elif option.option_type is list:
                parser.add_argument(option.get_argument(), dest=option.name,
                        choices=option.choices, action='append',
                        help=option.help, metavar=option.metavar)
            else:
                parser.add_argument(option.get_argument(),
                        type=option.option_type, dest=option.name,
                        choices=option.choices, help=option.help,
                        metavar=option.metavar)

            # Save non-action options in case we have a configuration file.
            if not option.action:
                self._options.append(option)

    def parse(self):
        """ Parse the configuration and return a mapping of name/value pairs.
        """

        configuration = self._parser.parse_args()

        if self._enable_configuration_file:
            configuration_file = configuration.configuration_file
            del configuration.configuration_file

            if configuration_file:
                self._update_from_configuration_file(configuration,
                        configuration_file)

        return configuration

    def _update_from_configuration_file(self, configuration, configuration_file):
        """ Update a configuration from the contents of a configuration file.
        """

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
                    if option.name == name:
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

                # Only override the existing configuration if no value has been
                # set.
                if getattr(configuration, name) is None:
                    setattr(configuration, name, cfg_value)
