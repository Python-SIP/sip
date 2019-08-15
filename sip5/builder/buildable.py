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


import importlib
import os
import sys


class Buildable:
    """ Encapsulate the sources etc. used to build an extension module,
    executable etc.
    """

    def __init__(self, name, sources_dir, uses_limited_api=False):
        """ Initialise the sources. """

        self.name = name
        self.sources_dir = sources_dir
        self.uses_limited_api = uses_limited_api

        self.define_macros = []
        self.sources = []
        self.headers = []
        self.include_dirs = []
        self.libraries = []
        self.library_dirs = []
        self.debug = False
        self.builder_settings = []

        if self.uses_limited_api:
            self.define_macros.append('Py_LIMITED_API=0x03040000')

    def make_names_relative(self):
        """ Make the names of source file, header files and include directories
        relative to the sources directory.
        """

        self.include_dirs = self._relative_names(self.include_dirs)
        self.headers = self._relative_names(self.headers)
        self.sources = self._relative_names(self.sources)
        self.library_dirs = self._relative_names(self.library_dirs)

    def _relative_names(self, names):
        """ Return a list of times made relative to the sources directory.
        Note that we only really do this for cosmetic reasons when doing a
        simple build.
        """

        rel_names = []

        for fn in names:
            try:
                common = os.path.commonpath([fn, self.sources_dir])
                _, common = os.path.splitdrive(common)

                if len(common) > 1:
                    # Only convert to a relative name if there is at least one
                    # parent directory in common.
                    fn = os.path.relpath(fn, self.sources_dir)
            except ValueError:
                # This is most likely to happen if the build directory is on a
                # different Windows drive.
                pass

            rel_names.append(fn)

        return rel_names


class BuildableModule(Buildable):
    """ Encapsulate the sources etc. used to build an extension module. """

    def __init__(self, fq_name, *args, **kwargs):
        """ Initialise the sources. """

        super().__init__(*args, **kwargs)

        self.fq_name = fq_name
        self.pyi_file = None
        self.static = False

    def get_module_extension(self):
        """ Return the filename extension that a module should have. """

        if sys.platform == 'win32':
            return '.pyd'

        suffixes = importlib.machinery.EXTENSION_SUFFIXES

        if self.uses_limited_api:
            for s in suffixes:
                if '.abi3' in s:
                    return s

        return suffixes[0]


class BuildableBindings(BuildableModule):
    """ Encapsulate the sources etc. used to build the extension module for a
    set of bindings.
    """

    def __init__(self, bindings, *args, **kwargs):
        """ Initialise the sources. """

        super().__init__(*args, **kwargs)

        self.bindings = bindings
