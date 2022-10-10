# Copyright (c) 2022, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ('AS IS'
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


from ..scoped_name import STRIP_NONE

from .scoped import EmbeddedScopeFormatter
from .template import TemplateFormatter


class ClassFormatter(EmbeddedScopeFormatter):
    """ This creates various string representations of a class. """

    def scoped_name(self, *, scope=None, strip=STRIP_NONE, as_xml=False):
        """ Return an appropriately scoped class name. """

        klass = self.object

        if klass.no_type_name:
            return TemplateFormatter(self.spec, klass.template).cpp_type(
                    scope=scope, strip=strip, as_xml=as_xml)

        # Protected classes have to be explicitly scoped.
        if klass.is_protected:
            # This should never happen.
            if scope is None:
                scope = klass.iface_file

            return 'sip{scope.fq_cpp_name.as_word}::sip{klass.iface_file.fq_cpp_name.base_name}'

        return klass.iface_file.fq_cpp_name.cpp_stripped(strip)
