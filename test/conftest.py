# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


import glob
import importlib
import os
import shutil
import subprocess
import sys
import tarfile

import pytest


# The different ABI versions.
ABI_VERSIONS = (12, 13)


def pytest_addoption(parser):
    """ Add the required command line option to specify the ABI version to
    test.
    """

    parser.addoption("--sip-abi-version",
            help="Specify the sip module ABI version", type=int,
            choices=ABI_VERSIONS, default=ABI_VERSIONS[-1])


@pytest.fixture
def abi_version(module):
    """ This fixture extracts the ABI version used to build a test module.  The
    value is the ABI major version.
    """

    return module.SIP_ABI_VERSION >> 16


@pytest.fixture(scope='module')
def code(request):
    """ This fixture generates the C/C++ code of a test module but does not try
    to compile it.  The value is True if the code was successfully generated
    and False otherwise.
    """

    # Get the configuration details.
    abi_version, exceptions, package, sip_module_configuration, tags, test_dir = _get_configuration_details(request)

    # Try to generate the code of the test module.
    try:
        _build_test_module(_get_only_sip_file(test_dir), test_dir, abi_version,
                None, exceptions, tags, no_compile=True)
    except subprocess.CalledProcessError:
        return False

    return True


@pytest.fixture(scope='module')
def module(request):
    """ The fixture is an appropriately built and imported wrapped module. """

    # Get the configuration details.
    abi_version, exceptions, package, sip_module_configuration, tags, test_dir = _get_configuration_details(request)

    # Build the test module.
    module_name = _build_test_module(_get_only_sip_file(test_dir), test_dir,
            abi_version, package, exceptions, tags)

    # Build a sip module if required.
    sip_module_name = _build_sip_module(test_dir, abi_version, package,
            sip_module_configuration) if package else None

    # Import the module.
    importlib.import_module(module_name)

    # The fixture is the module object.
    yield sys.modules[module_name]

    # Unload each module.
    del sys.modules[module_name]

    if sip_module_name is not None:
        try:
            del sys.modules[sip_module_name]
        except KeyError:
            pass


@pytest.fixture(scope='module')
def package(request):
    """ The fixture is a package containing one or more appropriately built
    wrapped modules and a separate sip module.
    """

    # Get the configuration details.
    abi_version, exceptions, package, sip_module_configuration, tags, test_dir = _get_configuration_details(request)

    if not package:
        raise ValueError("a value for 'cfg_package' must be specified")

    module_names = [package]

    # Build each module in the package.
    for sip_file in _get_sip_files(test_dir):
        module_name = _build_test_module(sip_file, test_dir, abi_version,
                package, exceptions, tags)
        module_names.append(module_name)

    # Build the sip module.
    sip_module_name = _build_sip_module(test_dir, abi_version, package,
            sip_module_configuration)

    # Import the modules.
    for module_name in module_names:
        importlib.import_module(module_name)

    # The fixture is the package object.
    yield sys.modules[package]

    # Unload each module.
    for module_name in module_names:
        del sys.modules[module_name]

    del sys.modules[sip_module_name]


class VirtualHook:
    """ The implementation of a virtual hook. """

    def __init__(self):
        """ Initialise the hook. """

        self._exc = None

    def catch(self, xtype, xvalue, xtb):
        """ The replacement exceptionhook. """

        # Save the exception for later.
        self._exc = xvalue

    def reraise(self):
        """ Restore the original exception hook and re-raise any exception
        raised by a virtual reimplementation.
        """

        if self._exc is not None:
            raise self._exc


@pytest.fixture
def virtual_hook():
    """ This fixture installs an exception hook that will catch any exceptions
    raised in a Python reimplementation of a C++ virtual.  The value is an
    object that has a reraise() method.  When it is called it will re-raise any
    any caught exception.
    """

    hook = VirtualHook()

    # Save the old hook and install the new one.
    old_hook = sys.excepthook
    sys.excepthook = hook.catch

    yield hook

    # Restore the original hook.
    sys.excepthook = old_hook


# The prototype pyproject.toml file.
_PYPROJECT_TOML = """
[build-system]
requires = ["sip >=6"]
build-backend = "sipbuild.api"

[project]
name = "{module_name}"

[tool.sip.project]
abi-version = "{abi_version}"
minimum-macos-version = "10.9"
sip-files-dir = ".."
"""

_SIP_MODULE = """
sip-module = "{sip_module}"
"""


def _build_module(module_name, package, build_args, src_dir, test_dir,
        impl_subdirs=None, no_compile=False):
    """ Build a module and move any implementation to the test directory. """

    os.chdir(src_dir)

    pkg_dir = test_dir

    # Do the build.
    args = [sys.executable] + build_args

    if no_compile:
        args.append('--no-compile')

    subprocess.run(args).check_returncode()

    if no_compile:
        return

    # Find the implementation.  Note that we only support setuptools and not
    # distutils.
    impl_pattern = ['build']

    if impl_subdirs is not None:
        impl_pattern.extend(impl_subdirs)

    impl_pattern.append('lib*')

    if package:
        pkg_subdirs = package.split('.')
        impl_pattern.extend(pkg_subdirs)

        pkg_dir = os.path.join(pkg_dir, os.path.join(*pkg_subdirs))
        os.makedirs(pkg_dir, exist_ok=True)

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

    os.chdir(pkg_dir)

    try:
        os.remove(impl)
    except:
        pass

    os.rename(impl_path, impl)


def _build_test_module(sip_file, test_dir, abi_version, package, exceptions,
        tags, no_compile=False):
    """ Build a test extension module and return its name. """

    build_dir = sip_file[:-4]
    module_name = os.path.basename(build_dir)

    # Remove any previous build directory.
    shutil.rmtree(build_dir, ignore_errors=True)

    os.mkdir(build_dir)

    # Create a pyproject.toml file.
    pyproject_toml = os.path.join(build_dir, 'pyproject.toml')

    with open(pyproject_toml, 'w') as f:
        f.write(
                _PYPROJECT_TOML.format(module_name=module_name,
                        abi_version=abi_version))

        if package:
            f.write(
                    _SIP_MODULE.format(
                            sip_module=_get_full_module_name(package)))

        if exceptions or tags is not None:
            f.write(f'\n[tool.sip.bindings.{module_name}]\n')

            if exceptions:
                f.write('exceptions = true\n')

            if tags is not None:
                tags_s = ', '.join([f'"{t}"' for t in tags])
                f.write(f'tags = [{tags_s}]\n')

    # Build and move the test module.
    _build_module(module_name, package,
            ['-m', 'sipbuild.tools.build', '--verbose'], build_dir, test_dir,
            impl_subdirs=[module_name, 'build'], no_compile=no_compile)

    return _get_full_module_name(package, module_name)


def _build_sip_module(test_dir, abi_version, package,
        sip_module_configuration):
    """ Build the sip module and return its name. """

    sip_module_name = _get_full_module_name(package)

    sdist_glob = os.path.join(test_dir,
            sip_module_name.replace('.', '_') + '-*.tar.gz')

    # Remove any existing sdists.
    for old in glob.glob(sdist_glob):
        os.remove(old)

    # Create the sdist.
    args = [sys.executable, '-m', 'sipbuild.tools.module', '--sdist',
        '--target-dir', test_dir, '--abi-version', str(abi_version)
    ]

    if sip_module_configuration is not None:
        for option in sip_module_configuration:
            args.append('--option')
            args.append(option)

    args.append(sip_module_name)

    subprocess.run(args).check_returncode()

    # Find the sdist and unpack it.
    sdists = glob.glob(sdist_glob)

    if len(sdists) != 1:
        raise Exception(
            "unable to determine the name of the sip module sdist file")

    sdist = sdists[0]

    with tarfile.open(sdist) as tf:
        tf.extractall(path=test_dir, filter='data')

    # Build and move the module.
    src_dir = os.path.join(test_dir, os.path.basename(sdist)[:-7])

    _build_module(sip_module_name, package, ['setup.py', 'build'], src_dir,
            test_dir)

    return sip_module_name


def _get_configuration_details(request):
    """ Return a 6-tuple of the ABI version, the exception support,
    the package, the sip module configuration, the tags and the test directory.
    """

    # Get the ABI version.
    abi_version = request.config.getoption('sip_abi_version')

    # Determine if the tests should be skipped depending on the ABI version.
    if hasattr(request.module, 'cfg_enabled_for'):
        # If the value is a non-empty sequence then it defines those ABIs for
        # which the tests are enabled.
        enabled_for = getattr(request.module, 'cfg_enabled_for')

        if enabled_for:
            if abi_version not in enabled_for:
                pytest.skip(f"Skipping non-enabled test for ABI v{abi_version}")
        else:
            pytest.skip("Skipping disabled test")

    elif hasattr(request.module, 'cfg_disabled_for'):
        # If the value is a non-empty sequence then it defines those ABIs for
        # which the tests are disabled.
        disabled_for = getattr(request.module, 'cfg_disabled_for')

        if disabled_for:
            if abi_version in disabled_for:
                pytest.skip(f"Skipping disabled test for ABI v{abi_version}")

    # See if exception support should be enabled.
    exceptions = getattr(request.module, 'cfg_exceptions', False)

    # The optional package in which generated modules will be placed.  This
    # will also cause a seperate sip module to be built.
    package = getattr(request.module, 'cfg_package', None)

    # The optional configuration of a sip module.
    sip_module_configuration = getattr(request.module,
            'cfg_sip_module_configuration', None)

    # Get the optional list of tags to be used to configure the test module.
    tags = getattr(request.module, 'cfg_tags', None)

    # Get the directory containing the particular test.
    test_dir = request.path.parent

    return abi_version, exceptions, package, sip_module_configuration, tags, test_dir


def _get_full_module_name(package, module_name='sip'):
    """ Return the full name of module taking an optional package into account.
    """

    return package + '.' + module_name if package else module_name


def _get_only_sip_file(test_dir):
    """ Return the name of the .sip file in a test directory. """

    sip_files = _get_sip_files(test_dir)

    if len(sip_files) != 1:
        raise ValueError("Exactly one .sip file expected")

    return sip_files[0]


def _get_sip_files(test_dir):
    """ Return the names of the .sip files in a test directory. """

    return glob.glob(os.path.join(test_dir, '*.sip'))
