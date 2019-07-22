from sip5.builder import Option, Project, PyProjectOptionException


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

    def update(self, tool):
        """ Update the project. """

        if self.platform:
            # The option was set in pyproject.toml so we just verify the value.
            if self.platform not in ('Linux', 'macOS', 'Windows'):
                raise PyProjectOptionException('platform',
                        "'{0}' is not a valid platform".format(self.platform))
        else:
            # The option wasn't specified in pyproject.toml so we introspect
            # the system.

            from sys import platform

            if platform == 'linux':
                self.platform = 'Linux'
            elif platform == 'darwin':
                self.platform = 'macOS'
            elif platform == 'win32':
                self.platform = 'Windows'
            else:
                raise PyProjectOptionException('platform',
                        "the '{0}' platform is not supported".format(platform))

        # Get the 'core' bindings and add the platform to the list of tags.
        core_bindings = self.bindings['core']
        core_bindings.tags.append(self.platform)
