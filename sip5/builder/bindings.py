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

from ..sip5 import sip5

from .configuration import Configurable, Option


class Bindings:
    """ The encapsulation of a module's bindings. """

    def __init__(self, sip_file, *, backstops=None, concatenate=0, debug=False, disabled_features=None, docstrings=True, exceptions=False, generate_api=False, generate_extracts=False, generate_pyi=False, include_dirs=None, protected_is_public=None, release_gil=False, source_suffix=None, tags=None, tracing=False):
        """ Initialise the object. """

        if protected_is_public is None:
            protected_is_public = (sys.platform != 'win32')

        self.sip_file = sip_file
        self.backstops = backstops
        self.concatenate = concatenate
        self.debug = debug
        self.disabled_features = disabled_features
        self.docstrings = docstrings
        self.exceptions = exceptions
        self.generate_api = generate_api
        self.generate_extracts = generate_extracts
        self.generate_pyi = generate_pyi
        self.include_dirs = include_dirs
        self.protected_is_public = protected_is_public
        self.release_gil = release_gil
        self.source_suffix = source_suffix
        self.tags = tags
        self.tracing = tracing

    def generate(self, package):
        """ Generate the bindings source code and optional additional extracts.
        Return a BindingsLocation instance containing the absolute file
        locations of everything that was generated.
        """

        # Derive the module name from the name of the .sip file.
        # TODO - this should be returned by the .sip parser.  The code
        # generator should be given the name of the higher-level directory in
        # which a module-specific sub-directory is created and the generated
        # code placed in that.
        module_name = os.path.basename(sip_file)
        if module_name.endswith('.sip'):
            module_name = module_name[:-4]

        sources_dir = os.path.join(package.build_dir, module_name)

        if self.generate_pyi:
            pyi_extract = os.path.join(sources_dir, module_name + '.pyi')
        else:
            pyi_extract = None

        if self.generate_api:
            api_extract = os.path.join(sources_dir, module_name + '.api')
        else:
            api_extract = None

        # Make sure the module's sub-directory exists.
        os.makedirs(sources_dir, exist_ok=True)

        sip5(self.sip_file, sip_module=package.sip_module,
                sources_dir=sources_dir, include_dirs=self.include_dirs,
                tags=self.tags, backstops=self.backstops,
                disabled_features=self.disabled_features,
                exceptions=self.exceptions, parts=self.concatenate,
                source_suffix=self.source_suffix, docstrings=self.docstrings,
                protected_is_public=self.protected_is_public,
                py_debug=self.debug, release_gil=self.release_gil,
                tracing=self.tracing, extracts=self.generate_extracts,
                pyi_extract=pyi_extract, api_extract=api_extract)

        # The locations of things that will have been generated.  Note that we
        # don't include anything for generic extracts as the arguments include
        # a file name.
        locations = BindingsLocations()

        locations.api_file = api_extract
        locations.include_dirs = [package.installed_sip_dir, sources_dir]
        locations.pyi_file = pyi_extract

        # Assume anything in the sources directory that looks like a C/C++
        # source file should be compiled.
        # TODO - this should be returned by the code generator.
        sources = []
        for fn in os.listdir(sources_dir):
            if self._source_suffix:
                if fn.endswith(self._source_suffix):
                    sources.append(fn)
            elif fn.endswith('.c') or fn.endswith('.cpp'):
                sources.append(fn)

        locations.sources = [os.path.join(sources_dir, fn) for fn in sources]

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
                help="disable the generation of docstrings")
        Option('generate_api', option_type=bool,
                help="generate a QScintilla .api file"),
        Option('generate_extracts', option_type=list,
                help="generate an extract file", metavar="ID:FILE"),
        Option('generate_pyi', option_type=bool,
                help="generate a PEP 484 .pyi file")
        Option('protected_is_public', option_type=bool,
                help="enable the protected/public hack"),
        Option('protected_is_public', option_type=bool, inverted=True,
                help="disable the protected/public hack"),
        Option('tracing', type=bool, help="build with tracing support"),
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
        self.include_dirs = None
        self.pyi_file = None
        self.sources = None
