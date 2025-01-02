# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os
import shutil
import subprocess
import sys

from ..exceptions import UserException
from ..py_versions import MINIMUM_SETUPTOOLS, OLDEST_SUPPORTED_MINOR
from ..version import SIP_VERSION, SIP_VERSION_STR

from .abi_version import (get_latest_version, get_module_source_dir,
        get_source_version_range, parse_abi_version)


def module(sip_module, abi_version, project, sdist, setup_cfg, sip_h, sip_rst,
        target_dir):
    """ Create the various elements of a sip module. """

    # Check we have the required source.
    if abi_version:
        major_version, minor_version = parse_abi_version(abi_version)
    else:
        major_version = get_latest_version()
        minor_version = None

    oldest_source, latest_source = get_source_version_range(major_version)

    if minor_version is None:
        minor_version = latest_source
    elif minor_version < oldest_source:
        raise UserException(
                f"ABI v{major_version}.{oldest_source} is the oldest available")

    module_source_dir = get_module_source_dir((major_version, minor_version))

    if project is None:
        project = sip_module.replace('.', '_')

    # Create the patches.
    patches = _create_patches(sip_module, module_source_dir, project)

    # The names of generated files.
    sdist_dir = project + '-' + patches['@SIP_MODULE_VERSION@']
    sip_h_fn = 'sip.h'
    sip_rst_fn = 'sip.rst'

    if target_dir:
        sdist_dir = os.path.join(target_dir, sdist_dir)
        sip_h_fn = os.path.join(target_dir, sip_h_fn)
        sip_rst_fn = os.path.join(target_dir, sip_rst_fn)

    # Generate the required files.
    if sdist:
        _create_sdist(sdist_dir, module_source_dir, patches, setup_cfg)

    if sip_h:
        _create_sip_file(sip_h_fn, module_source_dir, patches)

    if sip_rst:
        _create_sip_file(sip_rst_fn, module_source_dir, patches)


def copy_sip_h(abi_version, target_dir, sip_module='', version_info=True):
    """ Copy the sip.h file. """

    module_source_dir = get_module_source_dir(abi_version)
    patches = _create_patches(sip_module, module_source_dir,
            version_info=version_info)
    _install_source_file('sip.h', module_source_dir, target_dir, patches)


def copy_sip_pyi(abi_version, target_dir):
    """ Copy the sip.pyi file. """

    module_source_dir = get_module_source_dir(abi_version)
    shutil.copy(os.path.join(module_source_dir, 'sip.pyi'), target_dir)


def copy_nonshared_sources(abi_version, target_dir):
    """ Copy the module sources as a non-shared module. """

    # Copy the patched sip.h.
    copy_sip_h(abi_version, target_dir)

    # Copy the remaining source code.
    sources = []

    module_source_dir = get_module_source_dir(abi_version)

    for fn in os.listdir(module_source_dir):
        if fn.endswith('.c') or fn.endswith('.cpp') or fn.endswith('.h'):
            src_fn = os.path.join(module_source_dir, fn)
            dst_fn = os.path.join(target_dir, fn)
            shutil.copyfile(src_fn, dst_fn)

            if not fn.endswith('.h'):
                sources.append(dst_fn)

    return sources


def _create_patches(sip_module, module_source_dir, project='',
        version_info=True):
    """ Return a dict of the patches. """

    sip_module_parts = sip_module.split('.')
    sip_module_package_name = '.'.join(sip_module_parts[:-1])
    sip_module_name = sip_module_parts[-1]

    # We special case this because this should be the only package requiring
    # the support.
    legacy = (sip_module == 'PyQt5.sip')

    if version_info:
        sip_version = SIP_VERSION
        sip_version_str = SIP_VERSION_STR
    else:
        sip_version = 0
        sip_version_str = ''

    # Get the full version number of the sip module.
    abi_major = abi_minor = abi_patch = '???'

    with open(os.path.join(module_source_dir, 'sip.h.in')) as vf:
        for line in vf:
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == '#define':
                name = parts[1]
                value = parts[2]

                if name == 'SIP_ABI_MAJOR_VERSION':
                    abi_major = value
                elif name == 'SIP_ABI_MINOR_VERSION':
                    abi_minor = value
                elif name == 'SIP_MODULE_PATCH_VERSION':
                    abi_patch = value

    sip_module_version = f'{abi_major}.{abi_minor}.{abi_patch}'

    return {
        # The public patches are those that might be needed in setup.cfg or any
        # automatically generated user documentation.
        '@SIP_MODULE_FQ_NAME@':                 sip_module,
        '@SIP_MODULE_PROJECT_NAME@':            project,
        '@SIP_MODULE_PACKAGE_NAME@':            sip_module_package_name,
        '@SIP_MODULE_VERSION@':                 sip_module_version,

        # These are internal.
        '@_SIP_MINIMUM_SETUPTOOLS@':            MINIMUM_SETUPTOOLS,
        '@_SIP_MODULE_FQ_NAME@':                sip_module,
        '@_SIP_MODULE_NAME@':                   sip_module_name,
        '@_SIP_MODULE_SHARED@':                 '1' if sip_module else '0',
        '@_SIP_MODULE_ENTRY@':                  'PyInit_' + sip_module_name,
        '@_SIP_MODULE_LEGACY@':                 "1" if legacy else "0",
        '@_SIP_OLDEST_SUPPORTED_MINOR@':        str(OLDEST_SUPPORTED_MINOR),
        '@_SIP_OLDEST_SUPPORTED_MINOR_HEX@':    format(OLDEST_SUPPORTED_MINOR,
                                                        '02x'),
        '@_SIP_VERSION@':                       hex(sip_version),
        '@_SIP_VERSION_STR@':                   sip_version_str
    }


def _create_sdist(sdist_dir, module_source_dir, patches, setup_cfg):
    """ Create the sdist. """

    # Remove any existing source directory.
    shutil.rmtree(sdist_dir, ignore_errors=True)

    os.mkdir(sdist_dir)

    # The source directory doesn't have sub-directories.
    for name in os.listdir(module_source_dir):
        if name in ('setup.cfg.in', 'sip.pyi', 'sip.rst.in'):
            continue

        if name != 'MANIFEST.in' and name.endswith('.in'):
            name = name[:-3]

            # Don't install the default README if we are not using the default
            # setup.cfg.
            if name != 'README' or setup_cfg is None:
                _install_source_file(name, module_source_dir, sdist_dir,
                        patches)
        else:
            shutil.copy(os.path.join(module_source_dir, name), sdist_dir)

    # Write setup.cfg as required.
    if setup_cfg is None:
        setup_cfg = os.path.join(module_source_dir, 'setup.cfg.in')

    _install_file(setup_cfg, os.path.join(sdist_dir, 'setup.cfg'), patches)

    # Create the sdist file using setuptools.  This means any user supplied
    # setup.cfg should be handled correctly.
    saved_cwd = os.getcwd()
    os.chdir(sdist_dir)

    subprocess.run(
            [sys.executable, 'setup.py', '--quiet', 'sdist', '--dist-dir',
                    '..'])

    os.chdir(saved_cwd)

    # Tidy up.
    shutil.rmtree(sdist_dir)


def _create_sip_file(sip_file_fn, module_source_dir, patches):
    """ Create a patched file from the module source directory. """

    dname, fname = os.path.split(os.path.abspath(sip_file_fn))
    _install_source_file(fname, module_source_dir, dname, patches)


def _install_source_file(name, module_source_dir, target_dir, patches):
    """ Install a source file in a target directory. """

    _install_file(os.path.join(module_source_dir, name) + '.in',
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
