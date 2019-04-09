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


from .bindings import bindings
from .exceptions import handle_exception
from .standard_arguments import StandardArgumentParser


def main():
    """ Create the bindings for a C/C++ library. """

    # Parse the command line.
    parser = StandardArgumentParser(
            description="Generate Python extension modules for C/C++ "
                    "libraries.",
            fromfile_prefix_chars='@')

    parser.add_argument('specification',
            help="the name of the specification file [default stdin]",
            metavar="FILE", nargs='?')

    parser.add_argument('-a', dest='api_extract',
            help="the name of the QScintilla API file [default not generated]",
            metavar="FILE")

    parser.add_argument('-B', dest='backstops', action='append',
            help="add <TAG> to the list of timeline backstops",
            metavar="TAG")

    parser.add_argument('-c', dest='bindings_dir',
            help="the name of the code directory [default not generated]",
            metavar="DIR")

    parser.add_argument('-D', dest='py_debug', action='store_true',
            default=False,
            help="generate code for a debug build of Python")

    parser.add_argument('-e', dest='exceptions', action='store_true',
            default=False,
            help="enable support for exceptions [default disabled]")

    parser.add_argument('-f', dest='warnings_are_errors', action='store_true',
            default=False,
            help="warnings are handled as errors")

    parser.add_argument('-g', dest='release_gil', action='store_true',
            default=False,
            help="always release and reacquire the GIL [default only when "
                    "specified]")

    parser.add_argument('-I', dest='include_dirs', action='append',
            help="add <DIR> to the list of directories to search when "
                    "including files",
            metavar="DIR")

    parser.add_argument('-j', dest='parts', type=int, default=0,
            help="split the generated code into <FILES> files [default 1 per "
                    "class]",
            metavar="FILES")

    parser.add_argument('-m', dest='xml_extract',
            help="the name of the XML export file [default not generated]",
            metavar="FILE")

    parser.add_argument('-n', dest='sip_module',
            help="the qualified name of the sip module",
            metavar="NAME")

    parser.add_argument('-o', dest='docstrings', action='store_true',
            default=False,
            help="enable the automatic generation of docstrings [default "
                    "disabled]")

    parser.add_argument('-P', dest='protected_is_public', action='store_true',
            default=False,
            help="enable the protected/public hack [default disabled]")

    parser.add_argument('-r', dest='tracing', action='store_true',
            default=False,
            help="generate code with tracing enabled [default disabled]")

    parser.add_argument('-s', dest='source_suffix',
            help="the suffix to use for C or C++ source files [default \".c\" "
                    "or \".cpp\"]",
            metavar="SUFFIX")

    parser.add_argument('-t', dest='tags', action='append',
            help="add <TAG> to the list of versions/platforms to generate "
                    "code for",
            metavar="TAG")

    parser.add_argument('-w', dest='warnings', action='store_true',
            default=False,
            help="enable warning messages [default disabled]")

    parser.add_argument('-x', dest='disabled_features', action='append',
            help="add <FEATURE> to the list of disabled features",
            metavar="FEATURE")

    parser.add_argument('-X', dest='extracts', action='append',
            help="add <ID:FILE> to the list of extracts to generate",
            metavar="ID:FILE")

    parser.add_argument('-y', dest='pyi_extract',
            help="the name of the .pyi stub file [default not generated]",
            metavar="FILE")

    args = parser.parse_args()

    try:
        bindings(specification=args.specification, sip_module=args.sip_module,
                bindings_dir=args.bindings_dir, include_dirs=args.include_dirs,
                tags=args.tags, backstops=args.backstops,
                disabled_features=args.disabled_features,
                exceptions=args.exceptions, parts=args.parts,
                source_suffix=args.source_suffix, docstrings=args.docstrings,
                protected_is_public=args.protected_is_public,
                py_debug=args.py_debug, release_gil=args.release_gil,
                tracing=args.tracing, extracts=args.extracts,
                pyi_extract=args.pyi_extract, api_extract=args.api_extract,
                xml_extract=args.xml_extract, warnings=args.warnings,
                warnings_are_errors=args.warnings_are_errors)
    except Exception as e:
        handle_exception(e)

    return 0
