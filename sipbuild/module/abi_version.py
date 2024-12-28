# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os

from ..exceptions import UserException


# The directory containing the different module implementations.
_module_source_dir = os.path.join(os.path.dirname(__file__), 'source')


def get_module_source_dir(abi_major_version):
    """ Return the name of the directory containing the latest source of the
    sip module that implements the given ABI version.
    """

    return os.path.join(_module_source_dir, abi_major_version)


def get_sip_module_version(abi_major_version):
    """ Return the version number of the latest implementation of the sip
    module with the given ABI as a string.
    """

    abi_minor_version = patch_version = None

    # Read the version from the header file shared with the code generator.
    with open(os.path.join(get_module_source_dir(abi_major_version), 'sip.h.in')) as vf:
        for line in vf:
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == '#define':
                name = parts[1]
                value = parts[2]

                if name == 'SIP_ABI_MINOR_VERSION':
                    abi_minor_version = value
                elif name == 'SIP_MODULE_PATCH_VERSION':
                    patch_version = value

    # These are internal errors and should never happen.
    if abi_minor_version is None:
        raise ValueError(
                f"'SIP_ABI_MINOR_VERSION' not found for ABI {abi_major_version}")

    if patch_version is None:
        raise ValueError(
                f"'SIP_MODULE_PATCH_VERSION' not found for ABI {abi_major_version}")

    return f'{abi_major_version}.{abi_minor_version}.{patch_version}'


def resolve_abi_version(abi_version):
    """ Return a valid ABI version defaulting to the latest. """

    # Assume we are using the default minor version.
    minor_version = -1

    # Make sure we have a valid major version.
    if abi_version:
        parts = abi_version.split('.')
        major_version = parts[0]

        if not os.path.isdir(get_module_source_dir(major_version)):
            raise UserException(
                    f"'{abi_version}' is not a supported ABI version")

        if len(parts) != 1:
            try:
                minor_version = int(parts[1])
            except ValueError:
                pass

            if len(parts) > 2 or minor_version < 0:
                raise UserException(
                        f"'{abi_version}' is not a valid ABI version")
    else:
        # Default to the latest major version.
        major_version = sorted(os.listdir(_module_source_dir), key=int)[-1]

    # Get the latest minor version of the major version.
    latest_version = get_sip_module_version(major_version)
    _, latest_minor_version, _ = latest_version.split('.')
    latest_minor_version = int(latest_minor_version)

    # Get or validate the minor version to use.
    if minor_version < 0:
        minor_version = latest_minor_version
    elif minor_version > latest_minor_version:
        raise UserException(f"'{abi_version}' is not a supported ABI version")

    # Return the version.
    return f'{major_version}.{minor_version}'
