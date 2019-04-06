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

from .module_abi import ABI_MAJOR, ABI_MINOR, ABI_MAINTENANCE


# The directory containing the source code.
_src_dir = os.path.join(os.path.dirname(__file__), 'module_source')


def module(sip_module, include_dir=None, module_dir=None, no_sdist=False, setup_cfg=None):
    """ Create the sdist for a sip module. """

    # sip_module: eg. 'PyQt5.sip'
    # include_dir: the directory to create the sip.h file in, don't install it
    #    if not specified.  The name of the sip module will be #defined in the
    #    file to be used by a sip generated module that needs to import it.
    # module_dir: the directory to create the module in, don't create it if not
    #    specified.
    # no_sdist: normally an sdist .tar.gz is created in the module_dir.  If
    #    this is set then it is left as a directory and the .tar.gz is not
    #    created.
    # setup_cfg: the name of an optional setup.cfg file.

    # Create the patches.
    version = (ABI_MAJOR << 16) | (ABI_MINOR << 8) | ABI_MAINTENANCE

    version_str = '%d.%d' % (ABI_MAJOR, ABI_MINOR)
    if ABI_MAINTENANCE > 0:
        version_str = '%s.%d' % (version_str, ABI_MAINTENANCE)

    patches = {
        '@SIP_MODULE_PACKAGE@': sip_module.split('.')[0],
        '@SIP_MODULE_NAME@':    sip_module.replace('.', '_'),
        '@SIP_MODULE_VERSION@': version_str,

        # These are internal.
        '@_SIP_ABI_MAJOR@':     str(ABI_MAJOR),
        '@_SIP_ABI_MINOR@':     str(ABI_MINOR),
        '@_SIP_ABI_VERSION@':   hex(version),
        '@_SIP_FQ_NAME@':       sip_module,
    }

    # Install the sip.h file.
    if include_dir is not None:
        _install_file('sip.h', include_dir, patches)

    # There will be a default setup.cfg file with generic details (including a
    # long_description link to a generic README).

    # setup.cfg requires setuptools v30.3.0 to there needs to be a
    # pyproject.toml file to specify that as a minimum requirement.

    # The setup.cfg file determines if the sip.pyi is included.  The default
    # setup.cfg does not but should include a commented out section that does.


def _install_file(name, target_dir, patches):
    """ Install a file in a target directory. """

    # Read the file.
    with open(os.path.join(_src_dir, name + '.in')) as f:
        data = f.read()

    # Patch the file.
    for patch_name, patch in patches.items():
        data = data.replace(patch_name, patch)

    # Write the file.
    with open(os.path.join(target_dir, name), 'w') as f:
        f.write(data)
