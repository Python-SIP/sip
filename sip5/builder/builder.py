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


from abc import abstractmethod
import glob
import os
import shutil

from ..code_generator import set_globals
from ..exceptions import UserException
from ..module import copy_sip_h
from ..version import SIP_VERSION, SIP_VERSION_STR

from .abstract_builder import AbstractBuilder
from .configurable import Option
from .distinfo import create_distinfo


class Builder(AbstractBuilder):
    """ The default base implementation of a project builder. """

    # The configurable options.
    _options = (
    )

    def build(self):
        """ Build the project in-situ. """

        # self.compile()
        self._build_modules()

    def build_sdist(self, sdist_directory):
        """ Build an sdist for the project and return the name of the sdist
        file.
        """

        project = self.project

        # The sdist name.
        sdist_name = '{}-{}'.format(project.name.replace('-', '_'),
                project.version)

        # Create the sdist root directory.
        sdist_root = os.path.join(project.build_dir, sdist_name)
        os.mkdir(sdist_root)

        # Copy the pyproject.toml file.
        shutil.copy(os.path.join(project.root_dir, 'pyproject.toml'),
                sdist_root)

        # Copy in any project.py.
        project_py = os.path.join(project.root_dir, 'project.py')
        if os.path.isfile(project_py):
            shutil.copy(project_py, sdist_root)

        # Copy in the .sip files for each set of bindings.
        for bindings in project.bindings.values():
            self._install_sip_files(bindings, sdist_root)

        # Copy in anything else the user has asked for.
        for extra in project.sdist_extras:
            extra = os.path.abspath(extra)

            if os.path.commonprefix([extra, project.root_dir]) != project.root_dir:
                raise PyProjectOptionException('sdist-extras',
                        "must all be in the '{0}' directory or a "
                                "sub-directory".format(project.root_dir))

            extra = os.path.relpath(extra, project.root_dir)

            for src in glob.glob(extra):
                dst = os.path.join(sdist_root, src)

                if os.path.isfile(src):
                    shutil.copyfile(src, dst)
                elif os.path.isdir(src):
                    shutil.copytree(src, dst)
                else:
                    raise UserException("unable to copy '{0}'".format(src))

        # Create the tarball.
        sdist_file = sdist_name + '.tar.gz'
        sdist_path = os.path.abspath(os.path.join(sdist_directory, sdist_file))

        saved_cwd = os.getcwd()
        os.chdir(project.build_dir)

        import tarfile

        tf = tarfile.open(sdist_path, 'w:gz', format=tarfile.PAX_FORMAT)
        tf.add(sdist_name)
        tf.close()

        os.chdir(saved_cwd)

        return sdist_file

    def build_wheel(self, wheel_directory):
        """ Build a wheel for the project and return the name of the wheel
        file.
        """

        # inventory = self.compile()
        # self.install_into_wheel(wheel_dir, inventory)
        # create distinfo dir
        # create wheel.

        # Create the wheel tag.
        # TODO: If all bindings use the limited API (need to extract that from
        #   the parser) then include supported Python versions in the tag.  The
        #   minimum version can be extracted from the metadata
        #   'requires-python' and the maximum defined in pyproject.toml (and
        #   default to a hardcoded value - the latest stable version + 1).
        major_minor = '{}{}'.format((sys.hexversion >> 24) & 0xff,
                (sys.hexversion >> 16) & 0xff)
        wheel_tag = 'cp{}'.format(major_minor)

        try:
            wheel_tag += '-cp' + major_minor + sys.abiflags
        except AttributeError:
            wheel_tag += '-none'

        if sys.platform == 'win32':
            wheel_tag += '-win32' if is_32 else '-win_amd64'
        elif sys.platform == 'darwin':
            wheel_tag += '-macosx_10_6_intel'
        else:
            # We assume that Linux wheels are PEP 513 compatible so that it can
            # be uploaded to PyPI.
            wheel_tag += '-manylinux1_x86_64'

        # Create a temporary directory for the wheel.
        wheel_build_dir = os.path.join(self.build_dir, 'wheel')
        os.mkdir(wheel_build_dir)

        # Build and copy the wheel contents.
        self._build_and_install_modules(wheel_build_dir, wheel_tag=wheel_tag)

        wheel_file = '{}-{}-{}.whl'.format(self.name.replace('-', '_'),
                self.version, wheel_tag)
        wheel_path = os.path.abspath(os.path.join(wheel_directory, wheel_file))

        # Create the .whl file.
        saved_cwd = os.getcwd()
        os.chdir(wheel_build_dir)

        from zipfile import ZipFile, ZIP_DEFLATED

        with ZipFile(wheel_path, 'w', compression=ZIP_DEFLATED) as zf:
            for dirpath, _, filenames in os.walk('.'):
                for filename in filenames:
                    # This will result in a name with no leading '.'.
                    name = os.path.relpath(os.path.join(dirpath, filename))

                    zf.write(name)

        os.chdir(saved_cwd)

        return wheel_file

    @abstractmethod
    def compile(self):
        """ Compile the project.  The returned opaque object will be passed to
        install_into_target() and install_into_wheel().
        """

    def get_options(self):
        """ Return the list of configurable options. """

        options = super().get_options()
        options.extend(self._options)

        return options

    def install(self):
        """ Install the project. """

        # inventory = self.compile()
        # NOTE: QScintilla will also install to the Qt directory but not when
        # creating a wheel (that will be part of the bundling process when the
        # Qt directory is in the wheel).  Therefore need a hook to finalise an
        # ordinary installation.
        # self.install_into_target(target_dir, inventory)
        self._build_and_install_modules(self.target_dir)

    @abstractmethod
    def install_into_target(self, target_dir, opaque):
        """ Install the project into a target directory.  The opaque object was
        returned by compile().
        """

    @abstractmethod
    def install_into_wheel(self, wheel_dir, opaque):
        """ Install the project into a wheel directory.  The opaque object was
        returned by compile().
        """

    def _build_and_install_modules(self, target_dir, wheel_tag=None):
        """ Build and install the extension modules and create the .dist-info
        directory.
        """

        installed = []

        for bindings, module, module_fn, pyi_file in self._build_modules():
            # Get the name of the individual module's directory.
            module_name_parts = module.split('.')
            parts = [target_dir]
            parts.extend(module_name_parts[:-1])
            module_dir = os.path.join(*parts)
            os.makedirs(module_dir, exist_ok=True)

            # Copy the extension module.
            installed.append(self._install_file(module_fn, module_dir))

            # Copy any .pyi file.
            if pyi_file is not None:
                installed.append(self._install_file(pyi_file, module_dir))

            # Write the configuration file and copy the .sip files.
            if self.sip_module:
                bindings_dir = self._get_bindings_dir(target_dir)

                installed.append(bindings.write_configuration(bindings_dir))

                installed.extend(
                        self._install_sip_files(bindings, bindings_dir))

        # Install anything else the user has specified.
        for extra in self.install_extras:
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

        create_distinfo(self, installed, target_dir, wheel_tag=wheel_tag)

    def _build_modules(self):
        """ Build the enabled extension modules and return a 4-tuple of the
        bindings object, the fully qualified module name, its pathname and the
        pathname of any .pyi file.
        """

        # Get the list of directories to search for .sip files.
        sip_include_dirs = list(self.sip_include_dirs)

        if self.sip_module:
            # Add the project's sip directory.
            sip_include_dirs.append(self.sip_files_dir)

            # Add the bindings (specifically for the bindings .toml file) for
            # this package.
            local_bindings_dir = os.path.join(self.build_dir, 'bindings')
            sip_include_dirs.append(local_bindings_dir)

            # Add any bindings from previously installed packages.
            sip_include_dirs.append(self._get_bindings_dir(self.target_dir))

            # Generate the sip.h file for the shared sip module.
            if self.sip_module:
                copy_sip_h(self.abi_version, self.build_dir, self.sip_module)

        abi_major, abi_minor = self.abi_version.split('.')
        set_globals(SIP_VERSION, SIP_VERSION_STR, int(abi_major),
                int(abi_minor), UserException, sip_include_dirs)

        # Build each enabled set of bindings.
        modules = []

        for bindings_name in self.enable:
            bindings = self.bindings[bindings_name]

            self.progress(
                    "Generating the bindings from {0}".format(
                            bindings.sip_file))

            # Generate the bindings .toml file.
            if self.sip_module:
                bindings.write_configuration(local_bindings_dir)

            # Generate the source code.
            generated = bindings.generate()

            if generated.pyi_file is None:
                pyi_file = None
            else:
                pyi_file = os.path.join(generated.sources_dir,
                        generated.pyi_files)

            # Compile the generated code.
            # TODO
            builder_factory = self.get_builder_factory()
            builder = builder_factory(generated.sources_dir,
                    generated.sources, debug=bindings.debug,
                    define_macros=bindings.define_macros,
                    include_dirs=generated.include_dirs,
                    libraries=bindings.libraries,
                    library_dirs=bindings.library_dirs)

            self.progress(
                    "Building the bindings for {0}".format(generated.name))

            saved_cwd = os.getcwd()
            os.chdir(generated.sources_dir)
            extension_module = builder.build_extension_module(self,
                    generated.name)
            os.chdir(saved_cwd)

            modules.append(
                    (bindings, generated.name, extension_module, pyi_file))

        return modules

    @staticmethod
    def _ensure_subdirs_exist(file_path):
        """ Ensure that the parent directories of a file path exist. """

        file_dir = os.path.dirname(file_path)
        if file_dir != '':
            os.makedirs(file_dir, exist_ok=True)

    def _get_bindings_dir(self, target_dir):
        """ Return the name of the bindings directory for a target directory.
        """

        name_parts = self.sip_module.split('.')
        name_parts.insert(0, target_dir)
        name_parts[-1] = 'bindings'

        return os.path.join(*name_parts)

    @staticmethod
    def _install_file(fname, module_dir):
        """ Install a file into a module-specific directory and return the
        pathname of the installed file.
        """

        target_fn = os.path.join(module_dir, os.path.basename(fname))
        shutil.copyfile(fname, target_fn)

        return target_fn

    def _install_sip_files(self, bindings, target_dir):
        """ Install the .sip files for a set of bindings in a target directory
        and return a list of the installed files.
        """

        project = self.project

        installed = []

        rel_sip_files_dir = os.path.relpath(project.sip_files_dir,
                project.root_dir)
        target_sip_files_dir = os.path.join(target_dir, rel_sip_files_dir)

        for sip_file in bindings.get_sip_files():
            source_sip_file = os.path.join(project.sip_files_dir, sip_file)
            target_sip_file = os.path.join(target_sip_files_dir, sip_file)

            self._ensure_subdirs_exist(target_sip_file)
            shutil.copyfile(source_sip_file, target_sip_file)

            installed.append(target_sip_file)

        return installed

    def z__init__(self, sources_dir, sources, *, debug=False,
            define_macros=None, include_dirs=None, libraries=None,
            library_dirs=None):
        """ Initialise the object. """

        if define_macros is None:
            define_macros = []

        if include_dirs is None:
            include_dirs = []

        if libraries is None:
            libraries = []

        if library_dirs is None:
            library_dirs = []

        self.sources_dir = sources_dir
        self.sources = sources
        self.debug = debug
        self.define_macros = define_macros
        self.include_dirs = include_dirs
        self.libraries = libraries
        self.library_dirs = library_dirs

    def build_extension_module(self, project, name):
        """ Build an extension module from the sources and return its full
        pathname.
        """
