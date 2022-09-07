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


from weakref import WeakKeyDictionary

from .specification import TypeHint


class TypeHintManager:
    """ A manager for type hints on behalf of a Specification object. """

    # The map of specification objects and the corresponding manager object.
    _spec_manager_map = WeakKeyDictionary()

    def __new__(cls, spec):
        """ Return the existing manager for a specification or create a new one
        if necessary.
        """

        try:
            manager = cls._spec_manager_map[spec]
        except KeyError:
            manager = object.__new__(cls)
            cls._spec_manager_map[spec] = manager

        return manager

    def __init__(self, spec):
        """ Initialise the manager. """

        self._type_hints = {}

    def get_type_hint(self, text):
        """ Return a unique (to the specification) TypeHint object for the text
        of a hint.
        """

        try:
            type_hint = self._type_hints[text]
        except KeyError:
            type_hint = TypeHint(text)
            self._type_hints[text] = type_hint

        return type_hint


def get_type_hint(spec, text):
    """ Return a unique TypeHint object for the text of a hint. """

    return TypeHintManager(spec).get_type_hint(text)
