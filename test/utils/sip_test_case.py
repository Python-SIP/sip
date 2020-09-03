# Copyright (c) 2020, Riverbank Computing Limited
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


import glob
import inspect
import os
import shutil
import subprocess
import sys
import unittest


class SIPTestCase(unittest.TestCase):
    """ Encapsulate a test case that tests a set of standalone bindings. """

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

        # Build the extension module.
        cwd = os.getcwd()
        subprocess.run(['sip-build', '--verbose'], cwd=test_dir).check_returncode()
        os.chdir(cwd)

        # Move the extension module to the test directory.
        build_dir = os.path.join(test_dir, 'build')

        module_path = glob.glob(
                os.path.join(build_dir, module_name,
                        module_name + '*.pyd' if sys.platform == 'win32' else '*.so'))

        if len(module_path) == 0:
            raise Exception(
                    "no '{0}' extension module was built".format(module_name))

        if len(module_path) != 1:
            raise Exception(
                    "unable to determine file name of the '{0}' extension module".format(module_name))

        module_path = module_path[0]
        module_impl = os.path.join(test_dir, os.path.basename(module_path))
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

        os.remove(cls._module_impl)

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
requires = ["sip >=5"]
build-backend = "sipbuild.api"

[tool.sip.metadata]
name = "{module_name}"
"""
