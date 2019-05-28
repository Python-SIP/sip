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
import hashlib
import os
import shutil
import sys

from ..exceptions import UserException
from ..version import SIP_VERSION_STR


# The meta-data format defined in PEP345.
METADATA_VERSION = '1.2'

# The wheel format defined in PEP427.
WHEEL_VERSION = '1.0'


def create_distinfo(package, installed, target_dir, wheel_tag=None):
    """ Create the dist-info directory for a list of installed files. """

    # Make sure we have an empty dist-info directory.  Handle exceptions as the
    # user may be trying something silly with a system directory.
    distinfo_dir = os.path.join(target_dir,
            '{}-{}.dist-info'.format(package.name, package.version))

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

    if wheel_tag is None:
        # Create the INSTALLER file.
        installer_fn = os.path.join(distinfo_dir, 'INSTALLER')
        installer_f = open(installer_fn, 'w')
        installer_f.write('sip {}\n'.format(SIP_VERSION_STR))
        installer_f.close()

        installed.append(installer_fn)
    else:
        # Create the WHEEL file.
        WHEEL = '''Wheel-Version: {}
Generator: sip {}
Root-Is_Purelib: false
Tag: {}
'''

        wheel_fn = os.path.join(distinfo_dir, 'WHEEL')
        wheel_f = open(wheel_fn, 'w')
        wheel_f.write(WHEEL.format(WHEEL_VERSION, SIP_VERSION_STR, wheel_tag))
        wheel_f.close()

        installed.append(wheel_fn)

    # Create the METADATA file.
    METADATA = '''Metadata-Version: {}
Name: {}
Version: {}
'''

    metadata_fn = os.path.join(distinfo_dir, 'METADATA')
    metadata_f = open(metadata_fn, 'w')
    metadata_f.write(
            METADATA.format(METADATA_VERSION, package.name, package.version))
    metadata_f.close()

    installed.append(metadata_fn)

    # Create the RECORD file.
    record_fn = os.path.join(distinfo_dir, 'RECORD')
    record_f = open(record_fn, 'w')

    for fn in installed:
        if fn.startswith(target_dir):
            record_fn = fn[len(target_dir) + 1:]
        elif fn.startswith(sys.prefix):
            record_fn = os.path.relpath(fn, target_dir)
        else:
            record_fn = fn

        record_fn = record_fn.replace(os.sep, '/')

        fn_f = open(fn, 'rb')
        data = fn_f.read()
        fn_f.close()

        digest = base64.urlsafe_b64encode(
                hashlib.sha256(data).digest()).rstrip(b'=').decode('ascii')

        record_f.write(
                '{},sha256={},{}\n'.format(record_fn, digest, len(data)))

    record_f.write('{}/RECORD,,\n'.format(os.path.basename(distinfo_dir)))

    record_f.close()
