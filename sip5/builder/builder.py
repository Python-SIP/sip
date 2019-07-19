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
import sys

from ..code_generator import set_globals
from ..exceptions import UserException
from ..module import copy_sip_h
from ..version import SIP_VERSION, SIP_VERSION_STR

from .abstract_builder import AbstractBuilder
from .project import FIRST_SUPPORTED_MINOR, LAST_SUPPORTED_MINOR


class Builder(AbstractBuilder):
    """ The default base implementation of a project builder. """

    def build(self):
        """ Build the project in-situ. """

        self._generate_and_compile()

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
        for bindings in project.bindings:
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

        project = self.project

        # Create a temporary directory for the wheel.
        wheel_build_dir = os.path.join(project.build_dir, 'wheel')
        os.mkdir(wheel_build_dir)

        # Build the wheel contents.
        opaque = self._generate_and_compile()

        # If all enabled bindings use the limited API then the wheel does.
        all_use_limited_api = True
        for bindings in project.get_enabled_bindings():
            if not bindings.generated.uses_limited_api:
                all_use_limited_api = False
                break

        # Create the wheel tag.
        if all_use_limited_api:
            vtags = ['cp3{}'.format(v)
                    for v in range(FIRST_SUPPORTED_MINOR,
                            LAST_SUPPORTED_MINOR + 1)]
            wheel_tag = '.'.join(vtags)

            wheel_tag += '-none' if sys.platform == 'win32' else '-abi3'
        else:
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

        # Copy the wheel contents.
        self.install_into(opaque, wheel_build_dir, wheel_tag=wheel_tag)

        wheel_file = '{}-{}-{}.whl'.format(project.name.replace('-', '_'),
                project.version, wheel_tag)
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
        install_into().
        """

    def get_bindings_dir(self, target_dir):
        """ Return the name of the bindings directory for a target directory.
        """

        name_parts = self.project.sip_module.split('.')
        name_parts.insert(0, target_dir)
        name_parts[-1] = 'bindings'

        return os.path.join(*name_parts)

    def install(self):
        """ Install the project. """

        self.install_into(self._generate_and_compile(),
                self.project.target_dir)

    @abstractmethod
    def install_into(self, opaque, target_dir, wheel_tag=None):
        """ Install the project into a target directory.  The opaque object was
        returned by compile().
        """

    @staticmethod
    def _ensure_subdirs_exist(file_path):
        """ Ensure that the parent directories of a file path exist. """

        file_dir = os.path.dirname(file_path)
        if file_dir != '':
            os.makedirs(file_dir, exist_ok=True)

    def _generate_and_compile(self):
        """ Generate the bindings for all enable modules, pass to compile() and
        return the opaque object from compile().
        """

        project = self.project

        # Get the list of directories to search for .sip files.
        sip_include_dirs = list(project.sip_include_dirs)

        if project.sip_module:
            # Add the project's sip directory.
            sip_include_dirs.append(project.sip_files_dir)

            # Add the bindings (specifically for the bindings .toml file) for
            # this package.
            local_bindings_dir = os.path.join(project.build_dir, 'bindings')
            sip_include_dirs.append(local_bindings_dir)

            # Add any bindings from previously installed packages.
            sip_include_dirs.append(self.get_bindings_dir(project.target_dir))

            # Generate the sip.h file for the shared sip module.
            copy_sip_h(project.abi_version, project.build_dir,
                    project.sip_module)

        abi_major, abi_minor = project.abi_version.split('.')
        set_globals(SIP_VERSION, SIP_VERSION_STR, int(abi_major),
                int(abi_minor), UserException, sip_include_dirs)

        # Generate the code for each enabled set of bindings.
        for bindings in project.get_enabled_bindings():
            project.progress(
                    "Generating the bindings from {0}".format(
                            bindings.sip_file))

            # Generate the bindings .toml file.
            if project.sip_module:
                bindings.write_configuration(local_bindings_dir)

            # Generate the source code.
            bindings.generate()

        # Compile the generated code.
        return self.compile()

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
