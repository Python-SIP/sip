# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import glob
import os
import subprocess
import sys
import tarfile

from .sip_base_test_case import SIPBaseTestCase


class SIPTestCase(SIPBaseTestCase):
    """ Encapsulate a test case that tests the execution of  bindings. """

    # Set if a separate sip module should be generated.  It will be built
    # automatically if more than one module is being built.
    use_sip_module = False

    @classmethod
    def setUpClass(cls):
        """ Build a test extension module. """

        root_dir = cls.get_test_root_directory()

        cls._modules = []

        sip_files = glob.glob(os.path.join(root_dir, '*.sip'))
        use_sip_module = cls.use_sip_module or len(sip_files) > 1

        # Build a sip module if required.
        if use_sip_module:
            cls._modules.append(cls._build_sip_module(root_dir))

        for sip_file in sip_files:
            cls._modules.append(
                    cls.build_test_module(sip_file, root_dir,
                            use_sip_module=use_sip_module))

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

        cls.build_module(sip_module_name, ['setup.py', 'build'], src_dir,
                root_dir)

        return sip_module_name
