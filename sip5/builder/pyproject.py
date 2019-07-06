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


from collections import OrderedDict
import toml

from ..exceptions import UserFileException, UserParseException


class PyProjectException(UserFileException):
    """ An exception related to a pyproject.toml file. """

    def __init__(self, text, *, detail=''):
        """ Initialise the exception. """

        super().__init__("pyproject.toml", text, detail=detail)


class PyProjectOptionException(PyProjectException):
    """ An exception related to a specific option of a pyproject.toml file. """

    def __init__(self, name, text, *, section_name=None, detail=''):
        """ Initialise the exception. """

        if section_name is None:
            section_name = 'tool.sip.project'

        super().__init__("'{0}.{1}' {2}".format(section_name, name, text),
                detail=detail)


class PyProjectUndefinedOptionException(PyProjectOptionException):
    """ An exception related to an undefined option of a pyproject.toml file.
    """

    def __init__(self, name, *, section_name=None):
        """ Initialise the exception. """

        super().__init__(section_name, name, "must be defined")


class PyProject:
    """ Encapsulate a parsed pyproject.toml file. """

    def __init__(self):
        """ Initialise the object. """

        try:
            self._pyproject = toml.load('pyproject.toml', _dict=OrderedDict)
        except Exception as e:
            raise UserParseException('pyproject.toml', detail=str(e))

    def get_section(self, section_name, required=False):
        """ Return a sub-section with a dotted name. """

        section = self._pyproject

        for part in section_name.split('.'):
            try:
                section = section[part]
            except KeyError:
                if required:
                    raise PyProjectException(
                            "the '[{0}]' section is missing".format(
                                    section_name))

                return None

        if not self.is_section(section):
            raise PyProjectException(
                    "'{0}' is not a section".format(section_name))

        return section

    @staticmethod
    def is_section(value):
        """ Returns True if a section value is itself a section. """

        return isinstance(value, (OrderedDict, list))
