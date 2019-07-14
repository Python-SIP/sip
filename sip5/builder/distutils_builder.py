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


from distutils.command.build_ext import build_ext
from distutils.dist import Distribution
from distutils.extension import Extension
from distutils.log import ERROR, INFO, set_threshold

import os
import shutil

from ..exceptions import UserException

from .builder import Builder
from .distinfo import create_distinfo


# TODO: Make sure the correctly named .so files are created when the limited
#   API is specified.
class DistutilsBuilder(Builder):
    """ The implementation of a distutils-based project builder. """

    def compile(self):
        """ Compile the project.  The returned opaque object is a sequence of
        4-tuples of the bindings object, the fully qualified module name, its
        pathname and the pathname of any .pyi file.
        """

        project = self.project

        # Compile each enabled set of bindings.
        modules = []

        set_threshold(INFO if project.verbose else ERROR)

        for bindings_name in project.enable:
            bindings = project.bindings[bindings_name]

            self.project.progress(
                    "Compiling the bindings for {0}".format(
                            bindings.generated.name))

            saved_cwd = os.getcwd()
            os.chdir(bindings.generated.sources_dir)
            extension_module = self._build_extension_module(bindings)
            os.chdir(saved_cwd)

            modules.append((bindings, extension_module))

        return modules

    def install_into(self, opaque, target_dir, wheel_tag=None):
        """ Install the project into a target directory.  The opaque object
        contains the project's files to be installed.
        """

        project = self.project

        installed = []

        for bindings, module_fn in opaque:
            # Get the name of the individual module's directory.
            module_name_parts = bindings.generated.name.split('.')
            parts = [target_dir]
            parts.extend(module_name_parts[:-1])
            module_dir = os.path.join(*parts)
            os.makedirs(module_dir, exist_ok=True)

            # Copy the extension module.
            installed.append(self._install_file(module_fn, module_dir))

            # Copy any .pyi file.
            if bindings.generated.pyi_file:
                installed.append(
                        self._install_file(generated.pyi_file, module_dir))

            # Write the configuration file and copy the .sip files.
            if project.sip_module:
                bindings_dir = self.get_bindings_dir(target_dir)

                installed.append(bindings.write_configuration(bindings_dir))

                installed.extend(
                        self._install_sip_files(bindings, bindings_dir))

        # Install anything else the user has specified.
        for extra in project.install_extras:
            src = extra[0]

            if len(extra) == 1:
                dst_dir = target_dir
            else:
                dst_dir = extra[1]

                if os.path.isabs(dst_dir):
                    # Quietly ignore absolute pathnames when creating a wheel.
                    if wheel_tag is not None:
                        continue
                else:
                    dst_dir = os.path.join(target_dir, dst_dir)

            os.makedirs(dst_dir, exist_ok=True)

            if os.path.isfile(src):
                installed.append(self._install_file(src, dst_dir))
            elif os.path.isdir(src):
                dst = os.path.join(dst_dir, os.path.basename(src))

                shutil.copytree(src, dst,
                        copy_function=lambda s, d: installed.append(
                                shutil.copy2(s, d)))
            else:
                raise UserException("unable to install '{0}'".format(src))

        create_distinfo(project, installed, target_dir, wheel_tag=wheel_tag)

    @staticmethod
    def _install_file(fname, module_dir):
        """ Install a file into a module-specific directory and return the
        pathname of the installed file.
        """

        target_fn = os.path.join(module_dir, os.path.basename(fname))
        shutil.copyfile(fname, target_fn)

        return target_fn

    def _build_extension_module(self, bindings):
        """ Build an extension module from the sources and return its full
        pathname.
        """

        dist = Distribution()

        module_builder = build_ext(dist)
        module_builder.build_lib = bindings.generated.sources_dir
        module_builder.debug = bindings.debug
        module_builder.ensure_finalized()

        # Convert the #defines.
        define_macros = []
        for macro in bindings.define_macros:
            parts = macro.split('=', maxsplit=1)
            name = parts[0]
            try:
                value = parts[1]
            except IndexError:
                value = None

            define_macros.append((name, value))

        module_builder.extensions = [
            Extension(bindings.generated.name, bindings.generated.sources,
                    define_macros=define_macros,
                    include_dirs=bindings.include_dirs,
                    libraries=bindings.libraries,
                    library_dirs=bindings.library_dirs)]

        try:
            module_builder.run()
        except Exception as e:
            raise UserException(
                    "Unable to compile the bindings for '{0}'".format(
                            bindings.generated.name),
                    detail=str(e))

        return module_builder.get_ext_fullpath(bindings.generated.name)
