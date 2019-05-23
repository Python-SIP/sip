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
from ..version import SIP_VERSION, SIP_VERSION_STR

from .configuration import Configurable, Option
from .exceptions import BuilderException


class Bindings:
    """ The encapsulation of a module's bindings. """

    def __init__(self, sip_file):
        """ Initialise the object. """

        self.sip_file = sip_file

        self.backstops = None
        self.concatenate = 0
        self.debug = False
        self.disabled_features = None
        self.docstrings = True
        self.exceptions = False
        self.generate_api = False
        self.generate_extracts = None
        self.generate_pyi = False
        self.include_dirs = None
        self.protected_is_public = (sys.platform != 'win32')
        self.release_gil = False
        self.source_suffix = None
        self.tags = None
        self.tracing = False

        # This is set by generate().
        self.name = None

    def generate(self, package):
        """ Generate the bindings source code and optional additional extracts.
        Return a BindingsLocation instance containing the absolute file
        locations of everything that was generated.
        """

        # Set the globals.
        set_globals(SIP_VERSION, SIP_VERSION_STR, BuilderException,
                self.include_dirs, package.verbose, True)

        # Parse the input file.
        pt, self.name = parse(self.sip_file, True, self.tags, self.backstops,
            self.disabled_features, self.protected_is_public)

        # Get the module name.
        module_name = self.name.split('.')[-1]

        # Make sure the module's sub-directory exists.
        sources_dir = os.path.join(package.build_dir, module_name)
        os.makedirs(sources_dir, exist_ok=True)

        # Generate any API file.
        if self.generate_api:
            api_extract = os.path.join(sources_dir, module_name + '.api')
            generateAPI(pt, api_extract)
        else:
            api_extract = None

        # Generate any extracts.
        if self.generate_extracts:
            generateExtracts(pt, extracts)

        # Generate any type hints file.
        if self.generate_pyi:
            pyi_extract = os.path.join(sources_dir, module_name + '.pyi')
            generateTypeHints(pt, pyi_extract)
        else:
            pyi_extract = None

        # Generate the bindings.
        generateCode(pt, sources_dir, self.source_suffix, self.exceptions,
                self.tracing, self.release_gil, self.concatenate, self.tags,
                self.disabled_features, self.docstrings, self.debug,
                package.sip_module)

        # The locations of things that will have been generated.  Note that we
        # don't include anything for generic extracts as the arguments include
        # a file name.
        # TODO: is this the best solution? Could have generated_sources etc but
        # to be consistent with 'name' being set here, but we would then leave
        # it to the builder to add the sip.h dir.
        locations = BindingsLocations()

        locations.api_file = api_extract
        locations.pyi_file = pyi_extract

        # Assume anything in the sources directory that looks like a C/C++
        # source file should be compiled.
        # TODO - this should be returned by the code generator.
        sources = []
        for fn in os.listdir(sources_dir):
            if self.source_suffix:
                if fn.endswith(self.source_suffix):
                    sources.append(fn)
            elif fn.endswith('.c') or fn.endswith('.cpp'):
                sources.append(fn)

        locations.sources = [os.path.join(sources_dir, fn) for fn in sources]
        locations.sources_dir = sources_dir

        return locations


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


class BindingsLocations:
    """ The locations of the various files created by the bindings generator.
    """

    def __init__(self):
        """ Initialise the object. """

        self.api_file = None
        self.pyi_file = None
        self.sources = None
        self.sources_dir = None
