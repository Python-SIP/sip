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


# The directory containing the source code.
_src_dir = os.path.join(os.path.dirname(__file__), 'source')


def module(sip_module, documentation_dir=None, include_dir=None, module_dir=None, no_sdist=False, setup_cfg=None):
    """ Create the sdist for a sip module. """

    # If no locations are specified then create the sdist in the current
    # directory.
    if documentation_dir is None and include_dir is None and module_dir is None:
        module_dir = '.'

    # Create the patches.
    pypi_name = sip_module.replace('.', '_')

    abi_major, abi_minor, abi_maintenance = _read_abi_version()
    version = (abi_major << 16) | (abi_minor << 8) | abi_maintenance

    version_str = '%d.%d' % (abi_major, abi_minor)
    if abi_maintenance > 0:
        version_str = '%s.%d' % (version_str, abi_maintenance)

    sip_module_parts = sip_module.split('.')

    patches = {
        '@SIP_MODULE_NAME@':    pypi_name,
        '@SIP_MODULE_PACKAGE@': sip_module_parts[0],
        '@SIP_MODULE_VERSION@': version_str,

        # These are internal.
        '@_SIP_ABI_MAJOR@':     str(abi_major),
        '@_SIP_ABI_MINOR@':     str(abi_minor),
        '@_SIP_ABI_VERSION@':   hex(version),
        '@_SIP_FQ_NAME@':       sip_module,
        '@_SIP_BASE_NAME@':     sip_module_parts[-1],
        '@_SIP_MODULE_ENTRY@':  'PyInit_' + sip_module_parts[-1],
    }

    # Install the sip.rst file.
    if documentation_dir is not None:
        _install_source_file('sip.rst', documentation_dir, patches)

    # Install the sip.h file.
    if include_dir is not None:
        _install_source_file('sip.h', include_dir, patches)

    # Create the source directory.
    if module_dir is not None:
        full_pypi_name = pypi_name + '-' + version_str
        pkg_dir = os.path.join(module_dir, full_pypi_name)

        _install_code(pkg_dir, patches, setup_cfg)

        if not no_sdist:
            # Created the sdist.
            tf = tarfile.open(pkg_dir + '.tar.gz', 'w:gz',
                    format=tarfile.PAX_FORMAT)

            # Make sure that the names in the archive don't have a leading path
            # component.
            old_cwd = os.getcwd()
            os.chdir(module_dir)
            tf.add(full_pypi_name)
            os.chdir(old_cwd)

            tf.close()

            shutil.rmtree(pkg_dir)


def _install_code(target_dir, patches, setup_cfg):
    """ Install the module code in a target directory. """

    # Remove any existing directory.
    shutil.rmtree(target_dir, ignore_errors=True)

    os.mkdir(target_dir)

    # The source directory doesn't have sub-directories.
    for name in os.listdir(_src_dir):
        if name in ('sip.pyi', 'sip.rst.in'):
            continue

        if name.endswith('.in'):
            name = name[:-3]

            # Don't install the default README if we are not using the default
            # setup.cfg.
            if name != 'README' or setup_cfg is None:
                _install_source_file(name, target_dir, patches)
        else:
            shutil.copy(os.path.join(_src_dir, name), target_dir)

    # Overwrite setup.cfg is required.
    if setup_cfg is not None:
        setup_cfg_text = _install_source_file(setup_cfg,
                os.path.join(target_dir, 'setup.cfg'), patches)

        # If the user's setup.cfg mentions sip.pyi then assume it is needed.
        if 'sip.pyi' in setup_cfg_text:
            shutil.copy(os.path.join(_src_dir, 'sip.pyi'), target_dir)


def _install_source_file(name, target_dir, patches):
    """ Install a source file in a target directory and return a copy of the
    contents of the file.
    """

    return _install_file(os.path.join(_src_dir, name) + '.in',
            os.path.join(target_dir, name), patches)


def _install_file(name_in, name_out, patches):
    """ Install a file and return a copy of its contents. """

    # Read the file.
    with open(name_in) as f:
        data = f.read()

    # Patch the file.
    for patch_name, patch in patches.items():
        data = data.replace(patch_name, patch)

    # Write the file.
    with open(name_out, 'w') as f:
        f.write(data)

    return data


def _read_abi_version():
    """ Return a 3-tuple of the major, minor and maintenance version numbers of
    the current ABI.
    """

    abi_major = abi_minor = abi_maintenance = -1

    # Read the version from the header file shared with the code generator.
    with open(os.path.join()) as vf:
        for line in vf.read():
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == '#define':
                name = parts[1]
                value = parts[2]

                if name == 'SIP5_ABI_MAJOR':
                    abi_major = int(value)
                elif name == 'SIP5_ABI_MINOR':
                    abi_minor = int(value)
                elif name == 'SIP5_ABI_MAINTENANCE':
                    abi_maintenance = int(value)

    return abi_major, abi_minor, abi_maintenance
