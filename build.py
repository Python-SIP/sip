#!/usr/bin/python

# Copyright (c) 2016 Riverbank Computing Limited <info@riverbankcomputing.com>
#
# This file is part of SIP.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


"""This script prepares a repository copy of SIP for building.  It should be
run from a Mercurial repository.  It is not part of a packaged release.
"""


import os
import sys


def _progress(message, quiet):
    """ Show a progress message to the user.

    :param message:
        The text of the message (without a newline).
    :param quiet:
        Set if progress messages should be suppressed.
    """

    if not quiet:
        sys.stdout.write(message)
        sys.stdout.write("\n")


def prepare(quiet):
    """ Prepare for configuration and building by creating all the required
    additional files.

    :param quiet:
        Set if progress messages should be suppressed.
    """

    sipgen = 'sipgen'
    metasrc = os.path.join(sipgen, 'metasrc')

    lexer_l = os.path.join(metasrc, 'lexer.l')
    lexer_c = os.path.join(sipgen, 'lexer.c')
    _progress("Running flex to create %s" % lexer_c, quiet)
    os.system('flex -o%s %s' % (lexer_c, lexer_l))

    parser_y = os.path.join(metasrc, 'parser.y')
    parser_c = os.path.join(sipgen, 'parser.c')
    _progress("Running bison to create %s" % parser_c, quiet)
    os.system('bison -y -d -o %s %s' % (parser_c, parser_y))


if __name__ == '__main__':

    def _prepare(options):
        """prepare for configuration and building"""

        prepare(options.quiet)


    actions = (_prepare, )

    import optparse

    class MyParser(optparse.OptionParser):

        def get_usage(self):
            """ Reimplemented to add the description of the actions.  We don't
            use the description because the default formatter strips newlines.
            """

            usage = optparse.OptionParser.get_usage(self)

            usage += "\n" + __doc__ + "\nActions:\n"

            for action in actions:
                usage += "  %-9s  %s\n" % (action.__name__[1:], action.__doc__)

            return usage


    action_names = [action.__name__[1:] for action in actions]

    parser = MyParser(
            usage="%%prog [options] %s" % '|'.join(action_names))

    parser.add_option("-q", "--quiet", action='store_true', default=False,
            dest='quiet', help="suppress progress messages")

    options, args = parser.parse_args()

    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    for action in actions:
        if action.__name__[1:] == args[0]:
            action(options)
            break
    else:
        parser.print_help()
        sys.exit(1)

    sys.exit()
