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


# Publish the API.  This is private to the rest of sip.
from .argument import (fmt_argument_as_cpp_type, fmt_argument_as_name,
        fmt_argument_as_py_default_value, fmt_argument_as_py_type,
        fmt_argument_as_rest_ref, fmt_argument_as_type_hint)
from .enum import (fmt_enum_as_cpp_type, fmt_enum_as_rest_ref,
        fmt_enum_as_type_hint)
from .klass import (fmt_class_as_rest_ref, fmt_class_as_scoped_name,
        fmt_class_as_type_hint)
from .misc import fmt_copying, fmt_scoped_py_name
from .signature import (fmt_signature_as_cpp_declaration,
        fmt_signature_as_cpp_definition, fmt_signature_as_type_hint)
from .value_list import (fmt_value_list_as_cpp_expression,
        fmt_value_list_as_rest_ref)
