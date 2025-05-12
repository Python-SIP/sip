# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import glob
import inspect
import os
import shutil
import subprocess
import sys
import tarfile
import unittest


class SIPTestCase(unittest.TestCase):
    """ Encapsulate a test case that tests a set of standalone bindings. """

    # The ABI version to use.  None implies the latest major version.
    abi_version = None

    # Set if exception support should be enabled.
    exceptions = False

    # The list of tags to be used to configure the test modules.
    tags = None

    # Set if a separate sip module should be generated.  It will be built
    # automatically if more than one module is being built.
    use_sip_module = False

    @classmethod
    def setUpClass(cls):
        """ Build a test extension module. """

        # Get the name of the test's root directory from the file implementing
        # the test case.
        root_dir = os.path.dirname(inspect.getfile(cls))

        cls._modules = []

        sip_files = glob.glob(os.path.join(root_dir, '*.sip'))
        use_sip_module = cls.use_sip_module or len(sip_files) > 1

        # Build a sip module if required.
        if use_sip_module:
            cls._modules.append(cls._build_sip_module(root_dir))

        for sip_file in sip_files:
            cls._modules.append(
                    cls._build_test_module(sip_file, use_sip_module, root_dir))

        # Fix the path.
        sys.path.insert(0, root_dir)

    @classmethod
    def tearDownClass(cls):
        """ Unload the module. """

        for module_name in cls._modules:
            try:
                del sys.modules[module_name]
            except KeyError:
                pass

        # Restore the path.
        del sys.path[0]

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

    @classmethod
    def _build_test_module(cls, sip_file, use_sip_module, root_dir):
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
        cls._build_module(module_name,
                ['-m', 'sipbuild.tools.build', '--verbose'], build_dir,
                root_dir, impl_subdirs=[module_name, 'build'])

        return module_name

    @classmethod
    def _build_sip_module(cls, root_dir):
        """ Build the sip module and return its name. """

        sip_module_name = 'sip'

        # Create the sdist.
        args = [sys.executable, '-m', 'sipbuild.tools.module', '--sdist',
            '--target-dir', root_dir
        ]

        if cls.abi_version is not None:
            args.append('--abi-version')
            args.append(cls.abi_version)

        args.append(sip_module_name)

        subprocess.run(args).check_returncode()

        # Find the sdist and unpack it.
        sdists = glob.glob(
                os.path.join(root_dir, sip_module_name + '-*.tar.gz'))

        if len(sdists) != 1:
            raise Exception(
                "unable to determine the name of the sip module sdist file")

        sdist = sdists[0]

        with tarfile.open(sdist) as tf:
            tf.extractall(path=root_dir, filter='data')

        # Build and move the module.
        src_dir = os.path.join(root_dir, os.path.basename(sdist)[:-7])

        cls._build_module(sip_module_name, ['setup.py', 'build'], src_dir,
                root_dir)

        return sip_module_name

    @classmethod
    def _build_module(cls, module_name, build_args, src_dir, root_dir,
            impl_subdirs=None):
        """ Build a module and move the implementation to the test's root
        directory.
        """

        cwd = os.getcwd()
        os.chdir(src_dir)

        # Do the build.
        args = [sys.executable] + build_args
        subprocess.run(args).check_returncode()

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
