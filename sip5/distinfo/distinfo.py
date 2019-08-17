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


import base64
import collections
import hashlib
import os
import shutil
import sys

from ..exceptions import UserException
from ..pyproject import PyProject
from ..version import SIP_VERSION_STR


# The wheel format defined in PEP427.
WHEEL_VERSION = '1.0'


def distinfo(name, inventory, prefix, generator, project_root):
    """ Create and populate a .dist-info directory from an inventory file. """

    # Read the list of installed files.
    with open(inventory) as inventory_f:
        installed = inventory_f.read().strip().split('\n')

    if prefix is None:
        prefix = ''

    if project_root is None:
        # Default to what we can extract from the name of the .dist-info
        # directory.
        distinfo_base = os.path.basename(name)
        pkg_name, version = os.path.splitext(distinfo_base)[0].split('-')

        metadata = collections.OrderedDict()
        metadata['name'] = pkg_name
        metadata['version'] = version
    else:
        # Get the metadata from the pyproject.toml file.
        saved = os.getcwd()
        os.chdir(project_root)
        pyproject = PyProject()
        os.chdir(saved)

        metadata = pyproject.get_metadata()

    create_distinfo(name, installed, metadata, prefix_dir=prefix,
            generator=generator)


def create_distinfo(distinfo_dir, installed, metadata, prefix_dir='',
        wheel_tag=None, generator=None):
    """ Create and populate a .dist-info directory. """

    if generator is None:
        generator = os.path.basename(sys.argv[0])

    # The prefix directory corresponds to DESTDIR or INSTALL_ROOT.
    real_distinfo_dir = prefix_dir + distinfo_dir

    # Make sure we have an empty dist-info directory.  Handle exceptions as the
    # user may be trying something silly with a system directory.
    if os.path.exists(distinfo_dir):
        try:
            shutil.rmtree(distinfo_dir)
        except Exception as e:
            raise UserException(
                    "unable remove old dist-info directory '{}'".format(
                            distinfo_dir),
                    str(e))

    try:
        os.mkdir(distinfo_dir)
    except Exception as e:
        raise UserException(
                "unable create dist-info directory '{}'".format(distinfo_dir),
                str(e))

    # Reproducable builds.
    installed.sort()

    if wheel_tag is None:
        # Create the INSTALLER file.
        installer_fn = os.path.join(distinfo_dir, 'INSTALLER')
        installed.append(installer_fn)

        with open(prefix_dir + installer_fn, 'w') as installer_f:
            print(generator, file=installer_f)
    else:
        # Create the WHEEL file.
        WHEEL = '''Wheel-Version: {}
Generator: {} {}
Root-Is_Purelib: false
Tag: {}
'''

        wheel_fn = os.path.join(distinfo_dir, 'WHEEL')
        installed.append(wheel_fn)

        with open(prefix_dir + wheel_fn, 'w') as wheel_f:
            wheel_f.write(
                    WHEEL.format(WHEEL_VERSION, generator, SIP_VERSION_STR,
                            wheel_tag))

    # Create the METADATA file.
    metadata_fn = os.path.join(distinfo_dir, 'METADATA')
    installed.append(metadata_fn)

    with open(prefix_dir + metadata_fn, 'w') as metadata_f:
        for name, value in metadata.items():
            if isinstance(value, str):
                value = [value]

            for v in value:
                metadata_f.write('{}: {}\n'.format(name.title(), v))

    # Create the RECORD file.
    record_fn = os.path.join(distinfo_dir, 'RECORD')

    distinfo_path, distinfo_base = os.path.split(distinfo_dir)

    with open(prefix_dir + record_fn, 'w') as record_f:
        for name in installed:
            native_name = prefix_dir + name.replace('/', os.sep)
            if os.path.isdir(native_name):
                all_fns = []

                for root, dirs, files in os.walk(native_name):
                    # Reproducable builds.
                    dirs.sort()
                    files.sort()

                    for f in files:
                        all_fns.append(
                                os.path.join(root, f).replace(os.sep, '/'))

                    if '__pycache__' in dirs:
                        dirs.remove('__pycache__')
            else:
                all_fns = [prefix_dir + name]

            for fn in all_fns:
                real_distinfo_path = prefix_dir + distinfo_path

                if fn.startswith(real_distinfo_path):
                    fn_name = fn[len(real_distinfo_path) + 1:].replace('\\', '/')
                elif fn.startswith(prefix_dir + sys.prefix):
                    fn_name = os.path.relpath(
                            fn, real_distinfo_path).replace('\\', '/')
                else:
                    fn_name = fn[len(prefix_dir):]

                fn_f = open(fn, 'rb')
                data = fn_f.read()
                fn_f.close()

                digest = base64.urlsafe_b64encode(
                        hashlib.sha256(data).digest()).rstrip(b'=').decode('ascii')

                record_f.write(
                        '{},sha256={},{}\n'.format(fn_name, digest, len(data)))

        record_f.write('{}/RECORD,,\n'.format(distinfo_base))
