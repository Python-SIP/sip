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

from sip5.builder import Package


# Define the package as it would appear in PyPI.  We also specify the name of
# the sip module it uses.
package = Package('age', sip_module='examples.sip')

# Add the extension module created from the .sip specification file.
package.add_module('age.sip')

# Build the package according to the user supplied command line options..
package.build()


from setuptools import Extension, setup
from sip5.bindings import Bindings, BindingsConfiguration, BindingsContext


# Get a bindings configuration instance that contains the user supplied
# bindings configuration.
configuration = BindingsConfiguration()

# Create a bindings context instance and give it the name of the package's sip
# module.
context = BindingsContext('examples.sip', configuration)

# Create a bindings instance and give it the name of the SIP specification file
# that defines the bindings.
bindings = Bindings('age.sip', context)

# Generate the bindings code.  It returns a BindingsLocation instance
# containing the absolute file names of everything that was generated.
locations = bindings.generate()

age_module = Extension('examples.age', locations.sources,
        include_dirs=locations.include_dirs)

setup(name='age', version='1.0', ext_modules=[age_module])
