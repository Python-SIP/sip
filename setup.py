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


import glob
import os

from setuptools import Extension, setup


# Get the version number.
version_file_name = os.path.join('sip5', 'version.py')
try:
    version_file = open(version_file_name)
    version = version_file.read().strip().split('\n')[1].split()[-1][1:-1]
    version_file.close()
except FileNotFoundError:
    # Provide a minimal version file.
    version = '0.0.dev0'
    version_file = open(version_file_name, 'w')
    version_file.write('SIP_VERSION = 0\nSIP_VERSION_STR = \'%s\'\n' % version)
    version_file.close()

# Build the code generator extension module.
code_gen_src = glob.glob(os.path.join('code_generator', '*.c'))
code_gen_module = Extension('sip5.code_generator', code_gen_src,
        include_dirs=['code_generator'])

# Do the setup.
setup(
        name='sip5',
        version=version,
        license='SIP',
        python_requires='>=3.5',
        packages=['sip5'],
        ext_modules=[code_gen_module],
        entry_points={
            'console_scripts': [
                'sip5-bindings = sip5.bindings_main:main',
                'sip5-module = sip5.module_main:main']
        }
     )
