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

from ..sip5 import sip5


class Bindings:
    """ Encapsulate the bindings defined by a .sip specification file. """

    def __init__(self, sip_file, context, *, backstops=None, disabled_features=None, exceptions=False, module_name=None, release_gil=False, source_suffix=None, tags=None):
        """ Initialise the object. """

        self._sip_file = sip_file
        self._context = context

        # Derive the module name from the name of the .sip file if it wasn't
        # explicitly specified.
        if module_name is None:
            module_name = os.path.basename(sip_file)
            if module_name.endswith('.sip'):
                module_name = module_name[:-4]

        self._module_name = module_name

        self._backstops = backstops
        self._disabled_features = disabled_features
        self._exceptions = exceptions
        self._release_gil = release_gil
        self._source_suffix = source_suffix
        self._tags = tags

    def generate(self):
        """ Generate the bindings source code and optional additional extracts.
        Return a BindingsLocation instance containing the absolute file
        locations of everything that was generated.
        """

        context = self._context
        configuration = context.configuration

        # Make sure the context is initialised.
        context.initialise()

        # Generate the bindings.
        sources_dir = os.path.join(context.bindings_dir, self._module_name)
        include_dirs = [context.bindings_dir, sources_dir]
        parts = 0 if configuration.concatenate is None else configuration.concatenate

        if configuration.generate_pyi:
            pyi_extract = os.path.join(sources_dir, self._module_name + '.pyi')
        else:
            pyi_extract = None

        if configuration.generate_api:
            api_extract = os.path.join(sources_dir, self._module_name + '.api')
        else:
            api_extract = None

        # Make sure the module's sub-directory exists.
        os.makedirs(sources_dir, exist_ok=True)

        sip5(self._sip_file, sip_module=context.sip_module,
                sources_dir=sources_dir, include_dirs=include_dirs,
                tags=self._tags, backstops=self._backstops,
                disabled_features=self._disabled_features,
                exceptions=self._exceptions, parts=parts,
                source_suffix=self._source_suffix,
                docstrings=configuration.docstrings,
                protected_is_public=configuration.protected_is_public,
                py_debug=configuration.debug, release_gil=self._release_gil,
                tracing=configuration.tracing,
                extracts=configuration.generate_extracts,
                pyi_extract=pyi_extract, api_extract=api_extract,
                warnings=configuration.warnings_are_errors,
                warnings_are_errors=configuration.warnings_are_errors)

        # The locations of things that will have been generated.  Note that we
        # don't include anything for generic extracts as the arguments include
        # a file name.
        locations = BindingsLocations()

        locations.api_file = api_extract
        locations.include_dirs = include_dirs
        locations.pyi_file = pyi_extract

        # Assume anything in the sources directory that looks like a C/C++
        # source file should be compiled.
        sources = []
        for fn in os.listdir(sources_dir):
            if self._source_suffix:
                if fn.endswith(self._source_suffix):
                    sources.append(fn)
            elif fn.endswith('.c') or fn.endswith('.cpp'):
                sources.append(fn)

        locations.sources = [os.path.join(sources_dir, fn) for fn in sources]

        return locations


class BindingsLocations:
    """ The locations of the various files created by the bindings generator.
    """

    def __init__(self):
        """ Initialise the object. """

        self.api_file = None
        self.include_dirs = None
        self.pyi_file = None
        self.sources = None
