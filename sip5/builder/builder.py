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
from ..py_versions import FIRST_SUPPORTED_MINOR, LAST_SUPPORTED_MINOR
from ..version import SIP_VERSION, SIP_VERSION_STR

from .abstract_builder import AbstractBuilder
from .installable import Installable


class Builder(AbstractBuilder):
    """ The default base implementation of a project builder. """

    def build(self):
        """ Build the project in-situ. """

        self._generate_bindings()
        self.build_project(self.project.target_dir)

    @abstractmethod
    def build_project(self, target_dir):
        """ Build the project. """

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

        # Get a list of all excluded files.
        excluded = []
        for patt in project.sdist_excludes:
            excluded.extend(glob.glob(os.path.join(project.root_dir, patt)))

        for dname, dirnames, filenames in os.walk(project.root_dir):
            # Always ignore certain directories.
            if dname == project.build_dir:
                del dirnames[:]
                continue

            try:
                dirnames.remove('__pycache__')
            except ValueError:
                pass

            # Copy each file non-excluded file.
            for s_fn in filenames:
                s_fn_path = os.path.join(dname, s_fn)

                if s_fn_path in excluded:
                    continue

                d_fn_path = os.path.join(sdist_root,
                        os.path.relpath(s_fn_path, project.root_dir))
                os.makedirs(os.path.dirname(d_fn_path), exist_ok=True)

                shutil.copy2(s_fn_path, d_fn_path)

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
        self._generate_bindings()
        self.build_project(wheel_build_dir)

        # If all enabled bindings use the limited API then the wheel does.
        all_use_limited_api = True
        for bindings in project.bindings.values():
            if not bindings.build_sources.uses_limited_api:
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
        self.install_project(opaque, wheel_build_dir, wheel_tag=wheel_tag)

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

    def install(self):
        """ Install the project. """

        target_dir = self.project.target_dir

        self._generate_bindings()
        self.build_project(target_dir)
        self.install_project(target_dir)

    @abstractmethod
    def install_project(self, target_dir, wheel_tag=None):
        """ Install the project into a target directory. """

    def _generate_bindings(self):
        """ Generate the bindings for all enabled modules. """

        project = self.project

        # Get the list of directories to search for .sip files.
        sip_include_dirs = list(project.sip_include_dirs)

        if project.sip_module:
            # Add the project's sip directory.
            sip_include_dirs.append(project.sip_files_dir)

            # Add the local bindings directory to pick up the .toml files for
            # any other bindings in this package.
            local_bindings_dir = os.path.join(project.build_dir, 'bindings')
            sip_include_dirs.append(local_bindings_dir)

            # Add any bindings from previously installed packages.
            sip_include_dirs.append(
                    os.path.join(project.target_dir,
                            project.get_bindings_dir()))

            # Generate the sip.h file for the shared sip module.
            copy_sip_h(project.abi_version, project.build_dir,
                    project.sip_module)

        abi_major, abi_minor = project.abi_version.split('.')
        set_globals(SIP_VERSION, SIP_VERSION_STR, int(abi_major),
                int(abi_minor), UserException, sip_include_dirs)

        # Generate the code for each set of bindings.
        api_files = []

        for bindings in project.bindings.values():
            project.progress(
                    "Generating the bindings from {0}".format(
                            bindings.sip_file))

            # Generate the source code.
            buildable = bindings.generate()

            if not bindings.internal:
                api_files.append(
                        os.path.join(project.build_dir,
                                buildable.name + '.api'))

            # Generate the bindings configuration file.
            if project.sip_module:
                buildable.write_configuration(local_bindings_dir)

            project.buildables.append(buildable)

        # Create the .api file if required.
        if project.api_dir:
            api_fn = project.name + '.api'

            project.progress("Generating the {0} file".format(api_fn))

            # Concatanate the individual .api files.
            api_path = os.path.join(project.build_dir, api_fn)
            api_f = project.open_for_writing(api_path)

            for part_fn in api_files:
                with open(part_fn) as part_f:
                    api_f.write(part_f.read())

            api_f.close()

            # Add an Installable for the requested API file.
            installable = Installable('api',
                    target_subdir=os.path.abspath(project.api_dir))
            installable.files.append(api_path)
            project.installables.append(installable)
