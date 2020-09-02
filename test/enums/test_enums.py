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

    @classmethod
    def setUpClass(cls):
        """ Set up the test case. """

        super().setUpClass()

        from .enums import EnumClass

        class ScopedEnumFixture(EnumClass):
            """ A fixture for testing scoped enum values. """

            def scoped_virt(self):
                return EnumClass.ClassScopedEnum.ClassScopedMember

        cls.scoped_enum_fixture = ScopedEnumFixture()

        class NamedEnumFixture(EnumClass):
            """ A fixture for testing named enum values. """

            def __init__(self, value):
                """ Initialise the object. """

                super().__init__()

                self._value = value

            def named_virt(self):
                return self._value

        cls.member_fixture = NamedEnumFixture(
                EnumClass.ClassNamedEnum.ClassNamedMember)
        cls.int_fixture = NamedEnumFixture(0)

    @classmethod
    def tearDownClass(cls):
        """ Tear down the test case. """

        # Remove all references to the extension module so that the superclass
        # can unload it.
        del cls.scoped_enum_fixture

        del cls.member_fixture
        del cls.int_fixture

        super().tearDownClass()

    ###########################################################################
    # The following test module level enums.
    ###########################################################################

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

    ###########################################################################
    # The following test anonymous enum convertors.
    ###########################################################################

    def test_ClassAnon(self):
        """ Test a class level anonymous enum. """

        from .enums import EnumClass

        self.assertEqual(EnumClass.ClassAnonMember, 40)

    ###########################################################################
    # The following test named enum convertors.
    ###########################################################################

    def test_ClassNamed(self):
        """ Test a class level named enum. """

        from .enums import EnumClass

        self.assertTrue(issubclass(EnumClass.ClassNamedEnum, IntEnum))
        self.assertEqual(EnumClass.ClassNamedEnum.ClassNamedMember, 50)

    def test_named_get_member(self):
        """ named enum virtual result with a member value. """

        from .enums import EnumClass

        self.assertEqual(self.member_fixture.named_get(),
                EnumClass.ClassNamedEnum.ClassNamedMember)

    def test_named_set_member(self):
        """ named enum function argument with a member value. """

        from .enums import EnumClass

        self.member_fixture.named_set(
                EnumClass.ClassNamedEnum.ClassNamedMember)

    def test_named_var_member(self):
        """ named enum instance variable with a member value. """

        from .enums import EnumClass

        self.member_fixture.named_var = EnumClass.ClassNamedEnum.ClassNamedMember

    def test_named_overload_set(self):
        """ overloaded named enum function argument. """

        from .enums import EnumClass

        self.member_fixture.named_overload_set(
                EnumClass.ClassNamedEnum.ClassNamedMember)
        self.assertIs(self.member_fixture.named_overload, True)

    def test_named_get_int(self):
        """ named enum virtual result with an integer value. """

        self.assertEqual(self.int_fixture.named_get(), 0)

    def test_named_set_int(self):
        """ named enum function argument with an integer value. """

        self.int_fixture.named_set(0)

    def test_named_var_int(self):
        """ named enum instance variable with an integer value. """

        self.int_fixture.named_var = 0

    ###########################################################################
    # The following test scoped enum convertors.
    ###########################################################################

    def test_ClassScoped(self):
        """ Test a class level C++11 scoped enum. """

        from .enums import EnumClass

        self.assertTrue(issubclass(EnumClass.ClassScopedEnum, IntEnum))
        self.assertEqual(EnumClass.ClassScopedEnum.ClassScopedMember, 60)

    def test_scoped_get_member(self):
        """ scoped enum virtual result with a member value. """

        from .enums import EnumClass

        self.assertIs(self.scoped_enum_fixture.scoped_get(),
                EnumClass.ClassScopedEnum.ClassScopedMember)

    def test_scoped_set_member(self):
        """ scoped enum function argument with a member value. """

        from .enums import EnumClass

        self.scoped_enum_fixture.scoped_set(
                EnumClass.ClassScopedEnum.ClassScopedMember)

    def test_scoped_var_member(self):
        """ scoped enum instance variable with a member value. """

        from .enums import EnumClass

        self.scoped_enum_fixture.scoped_var = EnumClass.ClassScopedEnum.ClassScopedMember
