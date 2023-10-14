# Copyright (c) 2023, Riverbank Computing Limited
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


from ...scoped_name import STRIP_NONE

from .misc import fmt_scoped_py_name, iface_is_defined
from .template import fmt_template_as_cpp_type


def fmt_class_as_rest_ref(klass):
    """ Return a fully qualified Python name as a reST reference. """

    module_name = klass.iface_file.module.fq_py_name.name
    klass_name = fmt_scoped_py_name(klass.scope, klass.py_name.name)

    return f':sip:ref:`~{module_name}.{klass_name}`'


def fmt_class_as_scoped_name(spec, klass, scope=None, strip=STRIP_NONE,
        make_public=False, as_xml=False):
    """ Return a class's scoped name. """

    if klass.no_type_name:
        return fmt_template_as_cpp_type(spec, klass.template, strip=strip,
                as_xml=as_xml)

    # Protected classes have to be explicitly scoped.
    if klass.is_protected and not make_public:
        # This should never happen.
        if scope is None:
            scope = klass.iface_file

        return f'sip{scope.fq_cpp_name.as_word}::sip{klass.iface_file.fq_cpp_name.base_name}'

    return klass.iface_file.fq_cpp_name.cpp_stripped(strip)


def fmt_class_as_type_hint(spec, klass, defined):
    """ Return the type hint. """

    module = spec.module

    # We assume that an external class will be handled properly by some
    # handwritten type hint code.
    quote = '' if klass.external or iface_is_defined(klass.iface_file, klass.scope, module, defined) else "'"

    # Include the module name if it is not the current one.
    module_name = klass.iface_file.module.py_name + '.' if klass.iface_file.module is not module and defined is not None else ''
    py_name = fmt_scoped_py_name(klass.scope, klass.py_name.name)

    return f'{quote}{module_name}{py_name}{quote}'
