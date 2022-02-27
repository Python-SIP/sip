# Copyright (c) 2021, Riverbank Computing Limited
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


from .specification import ArgumentType, ScopedName


def append_iface_file(iface_file_list, iface_file):
    """ Append an IfaceFile object to a list of them. """

    # Make sure we don't try and add an interface file to its own list.
    if iface_file.used is iface_file_list:
        return

    # Don't bother if it is already there.
    if iface_file in iface_file_list:
        return

    iface_file_list.append(iface_file)


def argument_as_str(arg):
    """ Convert an Argument object to a string of valid C++. """

    if arg.original_typedef is None or arg.original_typedef.no_type_name:
        if arg.type is ArgumentType.TEMPLATE:
            s = str(arg.definition.cpp_name)
            s += '<'

            need_comma = False
            for sub_arg in arg.definition.types.args:
                if need_comma:
                    s += ','
                else:
                    need_comma = True

                s += argument_as_str(sub_arg)

            if s.endswith('>'):
                # For compilers earlier than C++11.
                s += ' '

            s += '>'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.DEFINED):
            s = str(arg.definition)

        elif arg.type in (ArgumentType.UBYTE, ArgumentType.USTRING):
            s = 'unsigned char'

        elif arg.type in (ArgumentType.BYTE, ArgumentType.STRING, ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
            s = 'char'

        elif arg.type in (ArgumentType.SBYTE, ArgumentType.SSTRING):
            s = 'signed char'

        elif arg.type is ArgumentType.WSTRING:
            s = 'wchar_t'

        elif arg.type is ArgumentType.USHORT:
            s = 'unsigned short'

        elif arg.type is ArgumentType.SHORT:
            s = 'short'

        elif arg.type is ArgumentType.UINT:
            s = 'uint'

        elif arg.type in (ArgumentType.INT, ArgumentType.CINT):
            s = 'int'

        elif arg.type is ArgumentType.ULONG:
            s = 'unsigned long'

        elif arg.type is ArgumentType.LONG:
            s = 'long'

        elif arg.type is ArgumentType.ULONGLONG:
            s = 'unsigned long long'

        elif arg.type is ArgumentType.LONGLONG:
            s = 'long long'

        elif arg.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            s = 'float'

        elif arg.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            s = 'double'

        elif arg.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            s = 'bool'

        elif arg.type is ArgumentType.VOID:
            s = 'void'

        elif arg.type is ArgumentType.CAPSULE:
            s = 'void *'

        elif arg.type is ArgumentType.SSIZE:
            s = 'Py_ssize_t'

        elif arg.type is ArgumentType.SIZE:
            s = 'size_t'

        elif arg.type is ArgumentType.HASH:
            s = 'Py_hash_t'

    else:
        # Use the original typedef.
        s = str(arg.original_typedef.fq_cpp_name)

    # Remove any global scope specifier (simply to make generated less
    # cluttered).
    if s.startswith('::'):
        s = s[2:]

    for _ in arg.derefs:
        s += '*'

    if arg.is_reference:
        s += '&'

    return s


def normalised_scoped_name(scoped_name, scope):
    """ Convert a scoped name to a fully qualified name. """

    # Clone the name.
    fq_scoped_name = ScopedName(scoped_name)

    if fq_scoped_name.is_absolute:
        pass
    elif scope is None:
        fq_scoped_name.make_absolute()
    elif fq_scoped_name.is_simple:
        fq_scoped_name.prepend(scope.iface_file.fq_cpp_name)
    else:
        # The relative name has a scope and appears within a scope so we need
        # lookup the name's scope within the current scope.
        names_scope = fq_scoped_name[0]
        scope_fq_cpp_name = scope.iface_file.fq_cpp_name

        while scope_fq_cpp_name is not None:
            if scope_fq_cpp_name.base_name == names_scope:
                del fq_scoped_name[0]
                fq_scoped_name.prepend(scope_fq_cpp_name)
                break

            scope_fq_cpp_name = scope_fq_cpp_name.scope
        else:
            # The lookup failed so just make the name absolute.
            fq_scoped_name.make_absolute()

    return fq_scoped_name


def same_base_type(type1, type2):
    """ Return True if two Argument objects refer to the same base type, ie.
    without taking into account const and pointers.
    """

    # The types must be the same.
    if type1.type is not type2.type:
        # If we are comparing a template with those that have already been used
        # to instantiate a class or mapped type then we need to compare with
        # the class or mapped type name.

        if type1.type is ArgumentType.CLASS and type2.type is ArgumentType.DEFINED:

            return type1.definition.iface_file.fq_cpp_name == type2.definition

        if type1.type is ArgumentType.DEFINED and type2.type is ArgumentType.CLASS:
            return type2.definition.iface_file.fq_cpp_name == type1.definition

        if type1.type is ArgumentType.MAPPED and type2.type is ArgumentType.DEFINED:
            return type1.definition.iface_file.fq_cpp_name == type2.definition

        if type1.type is ArgumentType.DEFINED and type2.type is ArgumentType.MAPPED:
            return type2.definition.iface_file.fq_cpp_name == type1.definition

        if type1.type is ArgumentType.ENUM and type2.type is ArgumentType.DEFINED:
            return type1.definition.fq_cpp_name == type2.definition

        if type1.type is ArgumentType.DEFINED and type2.type is ArgumentType.ENUM:
            return type2.definition.fq_cpp_name == type1.definition

        return False

    if type1.type is ArgumentType.CLASS:
        return type1.definition is type2.definition

    if type1.type is ArgumentType.ENUM:
        return type1.definition is type2.definition

    if type1.type is ArgumentType.TEMPLATE:
        td1 = type1.definition
        td2 = type2.definition

        if td1.cpp_name != td2.cpp_name:
            return False

        try:
            for ad1, ad2 in zip(td1.types.args, td2.types.args, strict=True):
                if len(ad1.derefs) != len(ad2.derefs):
                    return False

                if not same_base_type(ad1, ad2):
                    return False
        except ValueError:
            return False

        return True

    if type1.type in (ArgumentType.STRUCT, ArgumentType.UNION):
        return type1.definition == type2.definition

    if type1.type is ArgumentType.DEFINED:
        return type1.definition == type2.definition

    if type1.type is ArgumentType.MAPPED:
        return type1.definition is type2.definition

    # They must be the same if we've got this far.
    return True


def search_typedefs(spec, cpp_name, type):
    """ Search the typedefs and update the given type from any definition. """

    # Look for the name.
    fq_cpp_name = ScopedName(cpp_name)
    fq_cpp_name.make_absolute()

    for td in spec.typedefs:
        if td.fq_cpp_name == fq_cpp_name:
            break
    else:
        return

    # Update the type.
    type.type = td.type.type
    type.allow_none = type.allow_none or td.type.allow_none
    type.definition = td.type.definition
    type.derefs.extend(td.type.derefs)
    type.disallow_none = type.disallow_none or td.type.disallow_none
    type.is_const = type.is_const or td.type.is_const
    type.is_reference = type.is_reference or td.type.is_reference
    type.type_hints = type.type_hints or td.type.type_hints

    # Remember the original typedef.
    if type.original_typedef is None:
        type.original_typedef = td
