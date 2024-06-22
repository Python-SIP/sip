# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import glob
import inspect
import os
import shutil
import subprocess
import sys
import unittest


class SIPTestCase(unittest.TestCase):
    """ Encapsulate a test case that tests a set of standalone bindings. """

    # The ABI version to use.  None implies the latest version.
    abi_version = None

    @classmethod
    def setUpClass(cls):
        """ Build a test extension module. """

        # Get the name of the test directory from the file implementing the
        # test case.
        test_dir = os.path.dirname(inspect.getfile(cls))

        # Look for a .sip file in the test directory.
        sip_files = glob.glob(os.path.join(test_dir, '*.sip'))

        if len(sip_files) != 1:
            raise Exception(
                    "expected to find a single .sip file in '{0}'".format(
                            test_dir))

        module_name = os.path.basename(sip_files[0])[:-4]

        # Create a pyproject.toml file.
        pyproject_toml = os.path.join(test_dir, 'pyproject.toml')

        with open(pyproject_toml, 'w') as f:
            f.write(_PYPROJECT_TOML.format(module_name=module_name))

            if cls.abi_version is not None:
                f.write(_ABI_VERSION.format(abi_version=cls.abi_version))

        # Build the extension module.
        cwd = os.getcwd()
        subprocess.run([sys.executable, '-m', 'sipbuild.tools.build', '--verbose'], cwd=test_dir).check_returncode()
        os.chdir(cwd)

        # Move the extension module to the test directory.
        build_dir = os.path.join(test_dir, 'build')

        # The distutils and setuptools builders leave the module in different
        # places.
        for subdirs in ((module_name, ), (module_name, 'build', 'lib*')):
            module_pattern = [build_dir]
            module_pattern.extend(subdirs)
            module_pattern.append(
                    module_name + '*.pyd' if sys.platform == 'win32' else '*.so')

            module_path = glob.glob(os.path.join(*module_pattern))
            if len(module_path) != 0:
                break
        else:
            raise Exception(
                    "no '{0}' extension module was built".format(module_name))

        if len(module_path) != 1:
            raise Exception(
                    "unable to determine file name of the '{0}' extension module".format(module_name))

        module_path = module_path[0]
        module_impl = os.path.join(test_dir, os.path.basename(module_path))

        # On Windows the module may be lying around from a previous run.
        try:
            os.remove(module_impl)
        except:
            pass

        os.rename(module_path, module_impl)

        # Remove the build artifacts.
        os.remove(pyproject_toml)
        shutil.rmtree(build_dir, ignore_errors=True)

        # Provide tearDownClass() with the values it needs to tidy up.
        cls._module_name = module_name
        cls._module_impl = module_impl

    @classmethod
    def tearDownClass(cls):
        """ Unload the module and remove the extension module. """

        try:
            del sys.modules[cls._module_name]
        except KeyError:
            pass

        # This will fail on Windows (probably because the module is loaded).
        try:
            os.remove(cls._module_impl)
        except:
            pass

    def install_hook(self):
        """ Install a hook that will catch any exceptions raised by a Python
        reimplementation of a C++ virtual.
        """

        # Clear the saved exception.
        self._exc = None

        # Save the old hook and install the new one.
        self._old_hook = sys.excepthook
        sys.excepthook = self._hook

    def uninstall_hook(self):
        """ Restore the original exception hook and re-raise any exception
        raised by a virtual reimplementation.
        """

        sys.excepthook = self._old_hook

        if self._exc is not None:
            raise self._exc

    def _hook(self, xtype, xvalue, xtb):
        """ The replacement exceptionhook. """

        # Save the exception for later.
        self._exc = xvalue


# The prototype pyproject.toml file.
_PYPROJECT_TOML = """
[build-system]
requires = ["sip >=6"]
build-backend = "sipbuild.api"

[tool.sip.metadata]
name = "{module_name}"

[tool.sip.project]
minimum-macos-version = "10.9"
"""

_ABI_VERSION = """
abi-version = "{abi_version}"
"""
