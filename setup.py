# Copyright (c) 2019, Riverbank Computing Limited
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
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
    version = version_file.read().strip().split('\n')[0].split()[-1][1:-1]
    version_file.close()
except FileNotFoundError:
    # Provide a minimal version file.
    version = '0.0.dev0'
    version_file = open(version_file_name, 'w')
    version_file.write(
            'SIP5_RELEASE = \'%s\'\nSIP5_HEXVERSION = 0\n' %
                    version)
    version_file.close()

# Get the long description for PyPI.
with open('README') as readme:
    long_description = readme.read()

# Build the code generator extension module.
code_gen_src = glob.glob(os.path.join('sipgen', '*.c'))
code_gen_module = Extension('sip5.code_generator', code_gen_src,
        include_dirs=['sipgen'])

# Do the setup.
setup(
        name='sip5',
        version=version,
        description="A Python module generator for wrapping C/C++ libraries",
        long_description=long_description,
        author='Riverbank Computing Limited',
        author_email='info@riverbankcomputing.com',
        url='https://www.riverbankcomputing.com/software/sip/',
        license='SIP',
        platforms=['X11', 'macOS', 'Windows'],
        python_requires='>=3.5',
        packages=['sip5'],
        ext_modules=[code_gen_module],
        zip_safe=False,
        entry_points={
            'console_scripts': [
                'sip5 = sip5.sip5_main:main']
        }
     )
