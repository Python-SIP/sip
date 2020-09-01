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


from enum import IntEnum

from utils import SIPTestCase


class EnumsTestCase(SIPTestCase):
    """ Test the support for enums. """

    def test_ModuleAnon(self):
        """ Test a module level anonymous enum. """

        from .enums import AnonMember

        self.assertEqual(AnonMember, 10)

    def test_ModuleNamed(self):
        """ Test a module level named enum. """

        from .enums import NamedEnum

        self.assertTrue(issubclass(NamedEnum, IntEnum))
        self.assertEqual(NamedEnum.NamedMember, 20)

    def test_ModuleScoped(self):
        """ Test a module level C++11 scoped enum. """

        from .enums import ScopedEnum

        self.assertTrue(issubclass(ScopedEnum, IntEnum))

    def test_ClassAnon(self):
        """ Test a class level anonymous enum. """

        from .enums import EnumClass

        self.assertEqual(EnumClass.ClassAnonMember, 40)

    def test_ClassNamed(self):
        """ Test a class level named enum. """

        from .enums import EnumClass

        self.assertTrue(issubclass(EnumClass.ClassNamedEnum, IntEnum))
        self.assertEqual(EnumClass.ClassNamedEnum.ClassNamedMember, 50)

    def test_ClassScoped(self):
        """ Test a class level C++11 scoped enum. """

        from .enums import EnumClass

        self.assertTrue(issubclass(EnumClass.ClassScopedEnum, IntEnum))
        self.assertEqual(EnumClass.ClassScopedEnum.ClassScopedMember, 60)
