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


from abc import ABC, abstractmethod


class Builder(ABC):
    """ The encapsulation of a code builder. """

    def __init__(self, sources_dir, sources, include_dirs):
        """ Initialise the object. """

        self.sources_dir = sources_dir
        self.sources = sources
        self.include_dirs = include_dirs

    @abstractmethod
    def build_extension_module(self, bindings, package):
        """ Build an extension module from the sources and return its full
        pathname.
        """


class DistutilsBuilder(Builder):
    """ The implementation of a distutils-based code builder. """

    def build_extension_module(self, bindings, package):
        """ Build an extension module from the sources and return its full
        pathname.
        """

        # TODO: support package.verbose
        from distutils.command.build_ext import build_ext
        from distutils.dist import Distribution
        from distutils.extension import Extension

        dist = Distribution()

        builder = build_ext(dist)
        builder.build_lib = self.sources_dir
        builder.debug = bindings.debug
        builder.ensure_finalized()

        builder.extensions = [
            Extension(bindings.name, self.sources,
                    include_dirs=self.include_dirs)]

        builder.run()

        return builder.get_ext_fullpath(bindings.name)
