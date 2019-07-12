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


from .builder import Builder


# TODO: Make sure the correctly named .so files are created when the limited
#   API is specified.
class DistutilsBuilder(Builder):
    """ The implementation of a distutils-based project builder. """

    def build_extension_module(self, project, name):
        """ Build an extension module from the sources and return its full
        pathname.
        """

        from distutils.command.build_ext import build_ext
        from distutils.dist import Distribution
        from distutils.extension import Extension
        from distutils.log import ERROR, INFO, set_threshold

        set_threshold(INFO if project.verbose else ERROR)

        dist = Distribution()

        builder = build_ext(dist)
        builder.build_lib = self.sources_dir
        builder.debug = self.debug
        builder.ensure_finalized()

        # Convert the #defines.
        define_macros = []
        for macro in self.define_macros:
            parts = macro.split('=', maxsplit=1)
            name = parts[0]
            try:
                value = parts[1]
            except IndexError:
                value = None

            define_macros.append((name, value))

        builder.extensions = [
            Extension(name, self.sources, define_macros=define_macros,
                    include_dirs=self.include_dirs, libraries=self.libraries,
                    library_dirs=self.library_dirs)]

        builder.run()

        return builder.get_ext_fullpath(name)
