# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os

from ..exceptions import UserException


# The directory containing the different module implementations.
_module_source_dir = os.path.join(os.path.dirname(__file__), 'source')


def get_module_source_dir(abi_version):
    """ Return the name of the directory containing the source of the sip
    module that implements the given ABI version.
    """

    major_version, minor_version = abi_version

    return os.path.join(_module_source_dir, str(major_version),
            str(minor_version))


def get_source_version_range(abi_major_version):
    """ Return a 2-tuple of the earliest and latest minor versions, for a
    particular major version, for which source code is available.
    """

    source_dir = _module_source_dir

    if abi_major_version is not None:
        source_dir = os.path.join(source_dir, str(abi_major_version))

    subdirs = sorted(os.listdir(source_dir), key=int)

    return int(subdirs[0]), int(subdirs[-1])


def parse_abi_version(abi_version):
    """ Return a 2-tuple of the valid major ABI version (defaulting to the
    latest) and the valid minor ABI version (or None if it was omitted).  Both
    values are integers.
    """

    if abi_version:
        # Extract the major and optional minor versions.
        try:
            parts = [int(p) for p in abi_version.split('.')]
            nr_parts = len(parts)

            if nr_parts > 2:
                raise ValueError

            major_version = parts[0]
            minor_version = parts[1] if nr_parts == 2 else None

        except ValueError:
            raise UserException(f"'{abi_version}' is not a valid ABI version")

        try:
            # This will raise an exception if the major version is unsupported.
            latest_minor_version = _latest_version(major_version)

            if minor_version is None:
                minor_version = latest_minor_version
            elif minor_version > latest_minor_version:
                raise Exception()
        except:
            raise UserException(
                    f"'{abi_version}' is not a supported ABI version")
    else:
        # The defaults.
        major_version = _latest_version()
        minor_version = None

    return major_version, minor_version


def _latest_version(abi_major_version=None):
    """ Return the latest minor version for a major version (if given)
    otherwise return the latest major version.
    """

    _, latest = get_source_version_range(abi_major_version)

    return latest
