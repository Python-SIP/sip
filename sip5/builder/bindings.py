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
import sys

from ..code_generator import (set_globals, parse, generateCode,
        generateExtracts, generateAPI, generateXML, generateTypeHints)
from ..exceptions import UserException
from ..module.module import copy_nonshared_sources
from ..version import SIP_VERSION, SIP_VERSION_STR

from .configuration import Configurable, Option


class Bindings:
    """ The encapsulation of a module's bindings. """

    def __init__(self, package, sip_file, *, backstops=None, concatenate=0,
            debug=False, define_macros=None, disabled_features=None,
            docstrings=True, exceptions=False, generate_api=False,
            generate_extracts=None, generate_pyi=False, include_dirs=None,
            libraries=None, library_dirs=None, protected_is_public=None,
            release_gil=False, sip_include_dirs=None, source_suffix=None,
            tags=None, tracing=False):
        """ Initialise the bindings. """

        if define_macros is None:
            define_macros = []

        if include_dirs is None:
            include_dirs = []

        if libraries is None:
            libraries = []

        if library_dirs is None:
            library_dirs = []

        if protected_is_public is None:
            protected_is_public = (sys.platform != 'win32')

        self.package = package
        self.sip_file = sip_file

        self.backstops = backstops
        self.concatenate = concatenate
        self.debug = debug
        self.define_macros = define_macros
        self.disabled_features = disabled_features
        self.docstrings = docstrings
        self.exceptions = exceptions
        self.generate_api = generate_api
        self.generate_extracts = generate_extracts
        self.generate_pyi = generate_pyi
        self.include_dirs = include_dirs
        self.libraries = libraries
        self.library_dirs = library_dirs
        self.protected_is_public = protected_is_public
        self.release_gil = release_gil
        self.source_suffix = source_suffix
        self.sip_include_dirs = sip_include_dirs
        self.tags = tags
        self.tracing = tracing

    def generate(self):
        """ Generate the bindings source code and optional additional extracts.
        Return a GeneratedBindings instance containing the details of
        everything that was generated.
        """

        # Set the globals.
        set_globals(SIP_VERSION, SIP_VERSION_STR, UserException,
                self.sip_include_dirs)

        # Parse the input file.
        cwd = os.getcwd()
        os.chdir(self.package.root_dir)

        pt, name = parse(self.sip_file, True, self.tags, self.backstops,
                self.disabled_features, self.protected_is_public)

        os.chdir(cwd)

        # The details of things that will have been generated.  Note that we
        # don't include anything for generic extracts as the arguments include
        # a file name.
        generated = GeneratedBindings(name)

        # Get the module name.
        module_name = name.split('.')[-1]

        # Make sure the module's sub-directory exists.
        sources_dir = os.path.join(self.package.build_dir, module_name)
        os.makedirs(sources_dir, exist_ok=True)

        # Generate any API file.
        if self.generate_api:
            api_extract = os.path.join(sources_dir, module_name + '.api')
            generateAPI(pt, api_extract)
            generated.api_file = os.path.relpath(api_extract)

        # Generate any extracts.
        if self.generate_extracts:
            generateExtracts(pt, extracts)

        # Generate any type hints file.
        if self.generate_pyi:
            pyi_extract = os.path.join(sources_dir, module_name + '.pyi')
            generateTypeHints(pt, pyi_extract)
            generated.pyi_file = os.path.relpath(pyi_extract)

        # Generate the bindings.
        sources = generateCode(pt, sources_dir, self.source_suffix,
                self.exceptions, self.tracing, self.release_gil,
                self.concatenate, self.tags, self.disabled_features,
                self.docstrings, self.debug, self.package.sip_module)

        # Add the sip module code if it is not shared.
        include_dirs = [sources_dir]

        if self.package.sip_module is None:
            sources.extend(copy_nonshared_sources(sources_dir))
        else:
            include_dirs.append(sip_h_dir)

        include_dirs.extend(self.include_dirs)

        generated.sources_dir = os.path.relpath(sources_dir)
        generated.sources = [os.path.relpath(fn) for fn in sources]
        generated.include_dirs = [os.path.relpath(fn) for fn in include_dirs]

        return generated


class ConfigurableBindings(Bindings, Configurable):
    """ The user-configurable encapsulation of a module's bindings. """

    # The user-configurable options.
    _options = (
        Option('concatenate', option_type=int,
                help="concatenate the generated bindings into N source files",
                metavar="N"),
        Option('debug', option_type=bool, help="build with debugging symbols"),
        Option('docstrings', option_type=bool, inverted=True,
                help="disable the generation of docstrings"),
        Option('generate_api', option_type=bool,
                help="generate a QScintilla .api file"),
        Option('generate_extracts', option_type=list,
                help="generate an extract file", metavar="ID:FILE"),
        Option('generate_pyi', option_type=bool,
                help="generate a PEP 484 .pyi file"),
        Option('protected_is_public', option_type=bool,
                help="enable the protected/public hack (default on non-Windows"),
        Option('protected_is_public', option_type=bool, inverted=True,
                help="disable the protected/public hack (default on Windows)"),
        Option('tracing', option_type=bool, help="build with tracing support"),
    )

    @classmethod
    def get_options(cls):
        """ Get the user-configurable options. """

        return cls._options


class GeneratedBindings:
    """ The bindings created by Bindings generate(). """

    def __init__(self, name):
        """ Initialise the generated bindings. """

        self.name = name
        self.api_file = None
        self.pyi_file = None
        self.sources = None
        self.sources_dir = None
        self.include_dirs = None
