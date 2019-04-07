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


import os
import shutil
import tarfile

from .module_abi import ABI_MAJOR, ABI_MINOR, ABI_MAINTENANCE


# The directory containing the source code.
_src_dir = os.path.join(os.path.dirname(__file__), 'module_source')


def module(sip_module, include_dir=None, module_dir=None, no_sdist=False, setup_cfg=None):
    """ Create the sdist for a sip module. """

    # Create the patches.
    pypi_name = sip_module.replace('.', '_')

    version = (ABI_MAJOR << 16) | (ABI_MINOR << 8) | ABI_MAINTENANCE

    version_str = '%d.%d' % (ABI_MAJOR, ABI_MINOR)
    if ABI_MAINTENANCE > 0:
        version_str = '%s.%d' % (version_str, ABI_MAINTENANCE)

    patches = {
        '@SIP_MODULE_PACKAGE@': sip_module.split('.')[0],
        '@SIP_MODULE_NAME@':    pypi_name,
        '@SIP_MODULE_VERSION@': version_str,

        # These are internal.
        '@_SIP_ABI_MAJOR@':     str(ABI_MAJOR),
        '@_SIP_ABI_MINOR@':     str(ABI_MINOR),
        '@_SIP_ABI_VERSION@':   hex(version),
        '@_SIP_FQ_NAME@':       sip_module,
    }

    # Install the sip.h file.
    if include_dir is not None:
        _install_source_file('sip.h', include_dir, patches)

    # Create the source directory.
    if module_dir is not None:
        pkg_dir = os.path.join(module_dir, pypi_name + '-' + version_str)

        _install_code(pkg_dir, patches)

        # Overwrite setup.cfg is required.
        if setup_cfg is not None:
            _install_source_file(setup_cfg,
                    os.path.join(pkg_dir, 'setup.cfg'), patches)

        if not no_sdist:
            # Created the sdist.
            tf = tarfile.open(pkg_dir + '.tar.gz', 'w:gz')
            tf.add(pkg_dir)
            tf.close()

            shutil.rmtree(pkg_dir)


def _install_code(target_dir, patches):
    """ Install the module code in a target directory. """

    # Remove any existing directory.
    shutil.rmtree(target_dir, ignore_errors=True)

    os.mkdir(target_dir)

    # The source directory doesn't have sub-directories.
    for name in os.listdir(_src_dir):
        if name.endswith('.in'):
            _install_source_file(name[:-3], target_dir, patches)
        else:
            shutil.copy(os.path.join(_src_dir, name), target_dir)


def _install_source_file(name, target_dir, patches):
    """ Install a source file in a target directory. """

    _install_file(os.path.join(_src_dir, name) + '.in',
            os.path.join(target_dir, name), patches)


def _install_file(name_in, name_out, patches):
    """ Install a file. """

    # Read the file.
    with open(name_in) as f:
        data = f.read()

    # Patch the file.
    for patch_name, patch in patches.items():
        data = data.replace(patch_name, patch)

    # Write the file.
    with open(name_out, 'w') as f:
        f.write(data)
