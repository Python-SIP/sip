import sys

from sip5 import UserException
from sip5.builder import Option, Project


class CoreProject(Project):
    """ A project that adds an additional configuration option and introspects
    the system to determine its value.
    """

    def get_options(self):
        """ Return the sequence of configurable options. """

        # Get the standard options.
        options = super().get_options()

        # Add our new option.
        options.append(Option('platform'))

        return options

    def update(self):
        """ Update the project. """

        if self.platform:
            # The option was set in pyproject.toml so we just verify the value.
            if self.platform not in ('Linux', 'macOS', 'Windows'):
                raise UserException(
                        "'{0}' is not a valid platform".format(self.platform))
        else:
            # The option wasn't specified in # pyproject.toml so we introspect
            # the system.
            if sys.platform == 'linux':
                self.platform = 'Linux'
            elif sys.platform == 'darwin':
                self.platform = 'macOS'
            elif sys.platform == 'win32':
                self.platform = 'Windows'
            else:
                raise UserException(
                        "the '{0}' platform is not supported".format(
                                sys.platform))

        # Get the 'core' bindings and add the platform to the list of tags.
        core_bindings = self.bindings['core']
        core_bindings.tags.append(self.platform)
