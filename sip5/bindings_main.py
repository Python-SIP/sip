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
import sys

from . import SIP5_HEXVERSION, SIP5_RELEASE
from .code_generator import (set_globals, parse, transform, generateCode,
        generateExtracts, generateAPI, generateXML, generateTypeHints)


def main():
    """ The entry point for the setuptools generated CLI wrapper. """

    # Parse the command line.
    parser = argparse.ArgumentParser(
            description="Generate Python extension modules for C/C++ "
                    "libraries.",
            fromfile_prefix_chars='@')

    parser.add_argument('-V', action='version', version=SIP5_RELEASE)

    parser.add_argument('filename',
            help="the name of the specification file [default stdin]",
            metavar="FILE", nargs='?')

    parser.add_argument('-a', dest='apiFile',
            help="the name of the QScintilla API file [default not generated]",
            metavar="FILE")

    parser.add_argument('-B', dest='backstops', action='append',
            help="add <TAG> to the list of timeline backstops",
            metavar="TAG")

    parser.add_argument('-c', dest='codeDir',
            help="the name of the code directory [default not generated]",
            metavar="DIR")

    parser.add_argument('-D', dest='py_debug', action='store_true',
            default=False,
            help="generate code for a debug build of Python")

    parser.add_argument('-e', dest='exceptions', action='store_true',
            default=False,
            help="enable support for exceptions [default disabled]")

    parser.add_argument('-f', dest='warnings_are_fatal', action='store_true',
            default=False,
            help="warnings are handled as errors")

    parser.add_argument('-g', dest='releaseGIL', action='store_true',
            default=False,
            help="always release and reacquire the GIL [default only when "
                    "specified]")

    parser.add_argument('-I', dest='includeDirList', action='append',
            help="add <DIR> to the list of directories to search when "
                    "including files",
            metavar="DIR")

    parser.add_argument('-j', dest='parts', type=int, default=0,
            help="split the generated code into <FILES> files [default 1 per "
                    "class]",
            metavar="FILES")

    parser.add_argument('-m', dest='xmlFile',
            help="the name of the XML export file [default not generated]",
            metavar="FILE")

    parser.add_argument('-n', dest='sipName',
            help="the qualified name of the sip module",
            metavar="NAME")

    parser.add_argument('-o', dest='docs', action='store_true', default=False,
            help="enable the automatic generation of docstrings [default "
                    "disabled]")

    parser.add_argument('-P', dest='protHack', action='store_true',
            default=False,
            help="enable the protected/public hack [default disabled]")

    parser.add_argument('-r', dest='tracing', action='store_true',
            default=False,
            help="generate code with tracing enabled [default disabled]")

    parser.add_argument('-s', dest='srcSuffix',
            help="the suffix to use for C or C++ source files [default \".c\" "
                    "or \".cpp\"]",
            metavar="SUFFIX")

    parser.add_argument('-t', dest='versions', action='append',
            help="add <TAG> to the list of versions/platforms to generate "
                    "code for",
            metavar="TAG")

    parser.add_argument('-w', dest='warnings', action='store_true',
            default=False,
            help="enable warning messages [default disabled]")

    parser.add_argument('-x', dest='xfeatures', action='append',
            help="add <FEATURE> to the list of disabled features",
            metavar="FEATURE")

    parser.add_argument('-X', dest='extracts', action='append',
            help="add <ID:FILE> to the list of extracts to generate",
            metavar="ID:FILE")

    parser.add_argument('-y', dest='pyiFile',
            help="the name of the .pyi stub file [default not generated]",
            metavar="FILE")

    args = parser.parse_args()

    # The XML extract requires a non-strict parser.
    strict = (args.xmlFile is None)

    # Check for option conflicts.
    if args.codeDir is not None:
        # The code generator requires a strict parser.
        if not strict:
            print("The -m option cannot be given if the -c option is given",
                    file=sys.stderr)
            sys.exit(1)

        # The code generator requires the name of the sip module.
        if args.sipName is None:
            print("The -n option must be given if the -c option is given",
                    file=sys.stderr)
            sys.exit(1)

    # Set the globals.
    set_globals(SIP5_HEXVERSION, SIP5_RELEASE, args.includeDirList,
            args.warnings, args.warnings_are_fatal)

    # Parse the input file.
    pt = parse(args.filename, strict, args.versions, args.backstops,
            args.xfeatures, args.protHack)

    # Verify and transform the parse tree.
    transform(pt, strict)

    # Generate the code.
    if args.codeDir is not None:
        generateCode(pt, args.codeDir, args.srcSuffix, args.exceptions,
                args.tracing, args.releaseGIL, args.parts, args.versions,
                args.xfeatures, args.docs, args.py_debug, args.sipName)

    # Generate any extracts.
    generateExtracts(pt, args.extracts)

    # Generate the API file.
    if args.apiFile is not None:
        generateAPI(pt, args.apiFile)

    # Generate the XML export.
    if args.xmlFile is not None:
        generateXML(pt, args.xmlFile)

    # Generate the type hints file.
    if args.pyiFile is not None:
        generateTypeHints(pt, args.pyiFile)

    # All done.
    return 0
