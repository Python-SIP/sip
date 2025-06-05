# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import glob
import inspect
import os
import shutil
import subprocess
import sys
import unittest


class SIPBaseTestCase(unittest.TestCase):
    """ The base class for all test cases. """

    # The ABI version to use.  None implies the latest major version.
    abi_version = None

    # Set if exception support should be enabled.
    exceptions = False

    # The list of tags to be used to configure the test modules.
    tags = None

    @classmethod
    def get_test_root_directory(cls):
        """ Get the name of the test's root directory from the file
        implementing the test case.
        """

        return os.path.dirname(inspect.getfile(cls))

    @classmethod
    def build_test_module(cls, sip_file, root_dir, use_sip_module=False,
            no_compile=False):
        """ Build a test extension module and return its name. """

        build_dir = sip_file[:-4]
        module_name = os.path.basename(build_dir)

        # Remove any previous build directory.
        shutil.rmtree(build_dir, ignore_errors=True)

        os.mkdir(build_dir)

        # Create a pyproject.toml file.
        pyproject_toml = os.path.join(build_dir, 'pyproject.toml')

        with open(pyproject_toml, 'w') as f:
            f.write(_PYPROJECT_TOML.format(module_name=module_name))

            if cls.abi_version is not None:
                f.write(_ABI_VERSION.format(abi_version=cls.abi_version))

            if use_sip_module:
                f.write(_SIP_MODULE)

            if cls.tags is not None or cls.exceptions:
                f.write(f'\n[tool.sip.bindings.{module_name}]\n')

                if cls.tags is not None:
                    tags_s = ', '.join([f'"{t}"' for t in cls.tags])
                    f.write(f'tags = [{tags_s}]\n')

                if cls.exceptions:
                    f.write('exceptions = true\n')

        # Build and move the test module.
        cls.build_module(module_name,
                ['-m', 'sipbuild.tools.build', '--verbose'], build_dir,
                root_dir, impl_subdirs=[module_name, 'build'],
                no_compile=no_compile)

        return module_name

    @classmethod
    def build_module(cls, module_name, build_args, src_dir, root_dir,
            impl_subdirs=None, no_compile=False):
        """ Build a module and move any implementation to the test's root
        directory.
        """

        cwd = os.getcwd()
        os.chdir(src_dir)

        # Do the build.
        args = [sys.executable] + build_args

        if no_compile:
            args.append('--no-compile')

        subprocess.run(args).check_returncode()

        if no_compile:
            return

        # Find the implementation.  Note that we only support setuptools and
        # not distutils.
        impl_pattern = ['build']

        if impl_subdirs is not None:
            impl_pattern.extend(impl_subdirs)

        impl_pattern.append('lib*')
        impl_pattern.append(
                module_name + '*.pyd' if sys.platform == 'win32' else '*.so')

        impl_paths = glob.glob(os.path.join(*impl_pattern))
        if len(impl_paths) == 0:
            raise Exception(
                    "no '{0}' extension module was built".format(module_name))

        if len(impl_paths) != 1:
            raise Exception(
                    "unable to determine file name of the '{0}' extension module".format(module_name))

        impl_path = os.path.abspath(impl_paths[0])
        impl = os.path.basename(impl_path)

        os.chdir(root_dir)

        try:
            os.remove(impl)
        except:
            pass

        os.rename(impl_path, impl)

        os.chdir(cwd)

    @classmethod
    def get_test_root_directory(cls):
        """ Get the name of the test's root directory from the file
        implementing the test case.
        """

        return os.path.dirname(inspect.getfile(cls))


# The prototype pyproject.toml file.
_PYPROJECT_TOML = """
[build-system]
requires = ["sip >=6"]
build-backend = "sipbuild.api"

[project]
name = "{module_name}"

[tool.sip.project]
minimum-macos-version = "10.9"
sip-files-dir = ".."
"""

_ABI_VERSION = """
abi-version = "{abi_version}"
"""

_SIP_MODULE = """
sip-module = "sip"
"""
