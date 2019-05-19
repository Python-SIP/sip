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


from .exceptions import UserException
from .code_generator import (set_globals, parse, generateCode,
        generateExtracts, generateAPI, generateXML, generateTypeHints)
from .version import SIP_VERSION, SIP_VERSION_STR


def sip5(specification, sip_module=None, sources_dir=None, include_dirs=None, tags=None, backstops=None, disabled_features=None, exceptions=False, parts=0, source_suffix=None, docstrings=False, protected_is_public=False, py_debug=False, release_gil=False, tracing=False, extracts=None, pyi_extract=None, api_extract=None, warnings=False, warnings_are_errors=False):
    """ Create the bindings for a C/C++ library. """

    # The code generator requires the name of the sip module.
    if sources_dir is not None and sip_module is None:
        raise UserException("the name of the sip module must be given")

    # Set the globals.
    # TODO: deprecate 'warnings' and 'warnings_are_errors' and use the warnings
    # module to issue warnings.
    set_globals(SIP_VERSION, SIP_VERSION_STR, include_dirs, warnings,
            warnings_are_errors)

    # Parse the input file.
    # TODO: return the name of the module.
    pt, module_name = parse(specification, True, tags, backstops,
            disabled_features, protected_is_public)

    # Generate the bindings.
    if sources_dir is not None:
        # TODO: return the names of all generated files.
        generateCode(pt, sources_dir, source_suffix, exceptions, tracing,
                release_gil, parts, tags, disabled_features, docstrings,
                py_debug, sip_module)

    # Generate any extracts.
    generateExtracts(pt, extracts)

    # Generate the API file.
    if api_extract is not None:
        generateAPI(pt, api_extract)

    # Generate the type hints file.
    if pyi_extract is not None:
        generateTypeHints(pt, pyi_extract)

    return module_name
