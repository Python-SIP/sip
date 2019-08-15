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

from ..distinfo import create_distinfo
from ..exceptions import UserException

from .buildable import BuildableBindings
from .builder import Builder


class DistutilsBuilder(Builder):
    """ The implementation of a distutils-based project builder. """

    def compile(self, buildables, target_dir):
        """ Compile the project.  The returned opaque object is a sequence of
        2-tuples of the buildable object and the pathname of the compiled
        module.
        """

        modules = []

        for buildable in buildables:
            if not isinstance(buildable, BuildableBindings):
                raise UserException(
                        "DistutilsBuilder can only build BuildableBindings "
                        "buildables")

            if buildable.static:
                raise UserException(
                        "DistutilsBuilder cannot build static modules")

            extension_module = self._build_extension_module(buildable)
            modules.append((buildable, extension_module))

        return modules

    def install_into(self, opaque, target_dir, wheel_tag=None):
        """ Install the project into a target directory.  The opaque object
        contains the project's files to be installed.
        """

        project = self.project

        installed = []

        for buildable, module_fn in opaque:
            # Get the name of the individual module's directory.
            module_name_parts = buildable.fq_name.split('.')
            parts = [target_dir]
            parts.extend(module_name_parts[:-1])
            module_dir = os.path.join(*parts)
            os.makedirs(module_dir, exist_ok=True)

            # Copy the extension module.
            installed.append(self._install_file(module_fn, module_dir))

            # Copy any .pyi file.
            if buildable.pyi_file:
                installed.append(
                        self._install_file(buildable.pyi_file, module_dir))

            # Write the configuration file and copy the .sip files.
            if project.sip_module:
                bindings_dir = self.get_bindings_dir(target_dir)

                installed.append(
                        buildable.bindings.write_configuration(buildable,
                                bindings_dir))

                installed.extend(
                        self._install_sip_files(buildable.bindings,
                                bindings_dir))

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

        distinfo_dir = os.path.join(target_dir,
                '{}-{}.dist-info'.format(project.name.replace('-', '_'),
                project.version))

        create_distinfo(distinfo_dir, installed, project.metadata,
                wheel_tag=wheel_tag)

    @staticmethod
    def _install_file(fname, module_dir):
        """ Install a file into a module-specific directory and return the
        pathname of the installed file.
        """

        target_fn = os.path.join(module_dir, os.path.basename(fname))
        shutil.copyfile(fname, target_fn)

        return target_fn

    def _build_extension_module(self, buildable):
        """ Build an extension module from the sources and return its full
        pathname.
        """

        project = self.project

        set_threshold(INFO if project.verbose else ERROR)

        distribution = Distribution()

        module_builder = ExtensionCommand(distribution, buildable)
        module_builder.build_lib = buildable.sources_dir
        module_builder.debug = buildable.debug
        module_builder.ensure_finalized()

        # Convert the #defines.
        define_macros = []
        for macro in buildable.define_macros:
            parts = macro.split('=', maxsplit=1)
            name = parts[0]
            try:
                value = parts[1]
            except IndexError:
                value = None

            define_macros.append((name, value))

        module_builder.extensions = [
            Extension(buildable.fq_name, buildable.sources,
                    define_macros=define_macros,
                    include_dirs=buildable.include_dirs,
                    libraries=buildable.libraries,
                    library_dirs=buildable.library_dirs)]

        project.progress(
                "Compiling the '{0}' module".format(buildable.fq_name))

        saved_cwd = os.getcwd()
        os.chdir(buildable.sources_dir)

        try:
            module_builder.run()
        except Exception as e:
            raise UserException(
                    "Unable to compile the '{0}' module".format(
                            buildable.fq_name),
                    detail=str(e))

        extension_module = module_builder.get_ext_fullpath(buildable.fq_name)

        os.chdir(saved_cwd)

        return extension_module


class ExtensionCommand(build_ext):
    """ Extend the distutils command to build an extension module. """

    def __init__(self, distribution, buildable):
        """ Initialise the object. """

        super().__init__(distribution)

        self._buildable = buildable

    def get_ext_filename(self, ext_name):
        """ Reimplemented to handle modules that use the limited API. """

        return os.path.join(*ext_name.split('.')) + self._buildable.get_module_extension()
