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


from ..specification import CodeBlock


# The lexer states.
states = (
    ('code',            'exclusive'),
    ('ccomment',        'exclusive'),
    ('directive',       'inclusive'),
    ('needeol',         'inclusive'),
)


# The single character tokens.
literals = '(){}.,;:=!-+*/&|~<>[]%^'


# The map of non-code directives and corresponding tokens.
directives = {
    'AutoPyName':                   'AUTOPYNAME',
    'CompositeModule':              'COMPOMODULE',
    'DefaultDocstringFormat':       'DEFDOCSTRFMT',
    'DefaultDocstringSignature':    'DEFDOCSTRSIG',
    'DefaultEncoding':              'DEFENCODING',
    'DefaultMetatype':              'DEFMETATYPE',
    'DefaultSupertype':             'DEFSUPERTYPE',
    'End':                          'END',
    'Exception':                    'EXCEPTION',
    'Feature':                      'FEATURE',
    'HideNamespace':                'HIDE_NS',
    'If':                           'IF',
    'Import':                       'IMPORT',
    'Include':                      'INCLUDE',
    'License':                      'LICENSE',
    'MappedType':                   'MAPPEDTYPE',
    'Module':                       'MODULE',
    'Platforms':                    'PLATFORMS',
    'Plugin':                       'PLUGIN',       # Remove in SIP v7.
    'Property':                     'PROPERTY',
    'Timeline':                     'TIMELINE',
}


# The map of code directives and corresponding tokens.
code_directives = {
    'AccessCode':               'ACCESSCODE',
    'BIGetBufferCode':          'GETBUFFERCODE',
    'BIGetCharBufferCode':      'CHARBUFFERCODE',   # Remove in SIP v7.
    'BIGetReadBufferCode':      'READBUFFERCODE',   # Remove in SIP v7.
    'BIGetSegCountCode':        'SEGCOUNTCODE',     # Remove in SIP v7.
    'BIGetWriteBufferCode':     'WRITEBUFFERCODE',  # Remove in SIP v7.
    'BIReleaseBufferCode':      'RELEASEBUFFERCODE',
    'ConvertFromTypeCode':      'FROMTYPE',
    'ConvertToSubClassCode':    'TOSUBCLASS',
    'ConvertToTypeCode':        'TOTYPE',
    'Copying':                  'COPYING',
    'Docstring':                'DOCSTRING',
    'ExportedHeaderCode':       'EXPHEADERCODE',
    'ExportedTypeHintCode':     'EXPTYPEHINTCODE',
    'Extract':                  'EXTRACT',
    'FinalisationCode':         'FINALCODE',
    'GCClearCode':              'CLEARCODE',
    'GCTraverseCode':           'TRAVERSECODE',
    'GetCode':                  'GETCODE',
    'InitialisationCode':       'INITCODE',
    'InstanceCode':             'INSTANCECODE',
    'MethodCode':               'METHODCODE',
    'ModuleCode':               'MODCODE',
    'ModuleHeaderCode':         'MODHEADERCODE',
    'PickleCode':               'PICKLECODE',
    'PostInitialisationCode':   'POSTINITCODE',
    'PreInitialisationCode':    'PREINITCODE',
    'PreMethodCode':            'PREMETHODCODE',
    'RaiseCode':                'RAISECODE',
    'ReleaseCode':              'RELEASE',
    'SetCode':                  'SETCODE',
    'TypeCode':                 'TYPECODE',
    'TypeHeaderCode':           'TYPEHEADERCODE',
    'TypeHintCode':             'TYPEHINTCODE',
    'UnitCode':                 'UNITCODE',
    'UnitPostIncludeCode':      'UNITPOSTINCLUDECODE',
    'VirtualCallCode':          'VIRTUALCALLCODE',
    'VirtualCatcherCode':       'VIRTUALCATCHERCODE',
    'VirtualErrorHandler':      'VIRTERRORHANDLER',
}


# The map of plain keywords and corresponding tokens.
keywords = {
    'bool':             'BOOL',
    'char':             'CHAR',
    'class':            'CLASS',
    'const':            'CONST',
    'double':           'DOUBLE',
    'enum':             'ENUM',
    'explicit':         'EXPLICIT',
    'false':            'FALSE',
    'final':            'FINAL',
    'float':            'FLOAT',
    'int':              'INT',
    'long':             'LONG',
    'namespace':        'NAMESPACE',
    'noexcept':         'NOEXCEPT',
    'NULL':             'NULL',
    'operator':         'OPERATOR',
    'private':          'PRIVATE',
    'protected':        'PROTECTED',
    'public':           'PUBLIC',
    'Py_hash_t':        'PYHASHT',
    'Py_ssize_t':       'PYSSIZET',
    'Q_SIGNAL':         'SIGNAL_METHOD',
    'Q_SIGNALS':        'SIGNALS',
    'Q_SLOT':           'SLOT_METHOD',
    'Q_SLOTS':          'SLOTS',
    'short':            'SHORT',
    'signals':          'SIGNALS',
    'signed':           'SIGNED',
    'SIP_PYBUFFER':     'PYBUFFER',
    'SIP_PYCALLABLE':   'PYCALLABLE',
    'SIP_PYDICT':       'PYDICT',
    'SIP_PYENUM':       'PYENUM',
    'SIP_PYLIST':       'PYLIST',
    'SIP_PYOBJECT':     'PYOBJECT',
    'SIP_PYSLICE':      'PYSLICE',
    'SIP_PYTUPLE':      'PYTUPLE',
    'SIP_PYTYPE':       'PYTYPE',
    'SIP_SSIZE_T':      'PYSSIZET',         # Remove in SIP v7.
    'size_t':           'SIZET',
    'slots':            'SLOTS',
    'static':           'STATIC',
    'struct':           'STRUCT',
    'template':         'TEMPLATE',
    'throw':            'THROW',
    'true':             'TRUE',
    'typedef':          'TYPEDEF',
    'union':            'UNION',
    'unsigned':         'UNSIGNED',
    'virtual':          'VIRTUAL',
    'void':             'VOID',
    'wchar_t':          'WCHAR_T',
}


# The map of directive keywords and corresponding tokens.
directive_keywords = {
    'all_raise_py_exception':       'ALLRAISEPYEXC',
    'call_super_init':              'CALLSUPERINIT',
    'default_VirtualErrorHandler':  'DEFERRORHANDLER',
    'False':                        'FALSE',
    'format':                       'FORMAT',
    'get':                          'GET',
    'id':                           'ID',
    'keyword_arguments':            'KWARGS',
    'language':                     'LANGUAGE',
    'licensee':                     'LICENSEE',
    'name':                         'NAMEKW',
    'optional':                     'OPTIONAL',
    'order':                        'ORDER',
    'remove_leading':               'REMOVELEADING',
    'set':                          'SET',
    'signature':                    'SIGNATURE',
    'timestamp':                    'TIMESTAMP',
    'True':                         'TRUE',
    'type':                         'TYPE',
    'py_ssize_t_clean':             'PYSSIZETCLEAN',
    'use_argument_names':           'USEARGNAMES',
    'use_limited_api':              'USELIMITEDAPI',
}


# The lexer tokens.
tokens = [
    'CODEBLOCK',
    'ELLIPSIS',
    'END',
    'EOL',
    'LOGICALOR',
    'NAME',
    'NUMBER',
    'QCHAR',
    'REAL',
    'SCOPE',
    'STRING',
]

tokens = set(tokens)
tokens.update(directives.values())
tokens.update(code_directives.values())
tokens.update(keywords.values())
tokens.update(directive_keywords.values())
tokens = list(tokens)


# Handle EOF.
def t_eof(t):

    try:
        t.lexer.pm.pop_file()
    except IndexError:
        return None

    return t.lexer.token()


# Handle errors.
def t_ANY_error(t):

    t.lexer.pm.lexer_error(t, "'{0}' is unexpected".format(t.value[0]))
    t.lexer.skip(1)


# Ignore whitespace except when reading code blocks.
t_ANY_ignore = ' \t\r'
t_code_ignore = ''


# Handle newlines outside of code blocks and comments.
def t_newline(t):
    r'\n'

    lexer = t.lexer
    pm = lexer.pm

    # Maintain the line number.
    lexer.lineno += 1

    # Enter the 'code' state if we are at the end of a code directive name and
    # arguments.
    if pm.code_block is not None and pm.paren_depth == 0:
        pm.code_block.line_nr = lexer.lineno
        pm.set_lexer_state('code')


# Maintain the parenthesis depth.
def t_LPAREN(t):
    r'\('

    t.lexer.pm.paren_depth += 1
    t.type = '('

    return t


def t_RPAREN(t):
    r'\)'

    t.lexer.pm.paren_depth -= 1
    t.type = ')'

    return t


# Handle directives.
def t_DIRECTIVE(t):
    r'(?m)^\s*%[a-zA-Z][a-zA-Z]*'

    # Get the name of the directive.
    name = t.value[t.value.index('%') + 1:]

    t.type = code_directives.get(name)
    if t.type is not None:
        t.lexer.pm.code_block = CodeBlock(t.lexer.pm.sip_file)
    else:
        t.type = directives.get(name)

    return t


# Handle the %End of a code directive.
def t_code_END(t):
    r'%End'

    t.type = 'CODEBLOCK'
    t.value = t.lexer.pm.code_block
    t.lexer.pm.code_block = None
    t.lexer.begin('INITIAL')

    return t


# Handle a newline when an end-of-line needs to be reported to the parser.
def t_needeol_newline(t):
    r'\n'

    # Maintain the line number.
    t.lexer.lineno += 1

    t.lexer.pm.set_lexer_state()
    t.type = 'EOL'

    return t


# Handle a newline in a code directive.
def t_code_newline(t):
    r'\n'

    # Maintain the line number.
    t.lexer.lineno += 1

    t.lexer.pm.code_block.text += t.value

    # Discard the token.
    return None


# Handle a character in a code directive.
def t_code_CH(t):
    r'.'

    t.lexer.pm.code_block.text += t.value

    # Discard the token.
    return None


# Handle keywords and simple identifiers.
def t_KEYWORD(t):
    r'[_A-Za-z][_A-Za-z\d]*'

    t.type = keywords.get(t.value)

    if t.type is None:
        t.type = 'NAME'
    else:
        t.value = t.type

    return t


# Handle directive keywords, ie. keywords that are only recognised in the
# context of a directive.
def t_directive_KEYWORD(t):
    r'[_A-Za-z][_A-Za-z\d]*'

    t.type = directive_keywords.get(t.value)

    if t.type is None:
        t.type = 'NAME'
    else:
        t.value = t.type

    return t


# Handle a C++-style comment.
def t_CPPCOMMENT(t):
    r'//.*'

    # Discard the token.
    return None


# Handle the start of a C-style comment.
def t_COMMENTSTART(t):
    r'/\*'

    t.lexer.push_state('ccomment')

    # Discard the token.
    return None


# Handle the end of a C-style comment.
def t_ccomment_COMMENTEND(t):
    r'\*/'

    t.lexer.pop_state()

    # Discard the token.
    return None


# Handle a newline in a C-style comment.
def t_ccomment_newline(t):
    r'\n'

    # Maintain the line number.
    t.lexer.lineno += 1

    # Discard the token.
    return None


# Handle the content of a C-style comment.
def t_ccomment_CH(t):
    r'.'

    # Maintain the line number.
    if t.value == '\n':
        t.lexer.lineno += 1

    # Discard the token.
    return None


# Handle a signed decimal number.
def t_NUMBER(t):
    r'-?\d+[lLuU]?'

    # Remove any suffix character.
    value = t.value
    if not value[-1].isdigit():
        value = value[:-1]

    t.type = 'NUMBER'
    t.value = int(value)

    return t


# Handle an unsigned hexadecimal number.
def t_HEXNUMBER(t):
    r'0x[\da-fA-F]+'

    t.type = 'NUMBER'
    t.value = int(t.value, base=16)

    return t


# Handle a real number.
def t_REAL(t):
    r'-?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?[fF]?'

    # Remove any suffix character.
    value = t.value
    if not value[-1].isdigit():
        value = value[:-1]

    t.type = 'REAL'
    t.value = float(value)

    return t


# Handle a double-quoted string.
def t_STRING(t):
    r'"(\\.|[^\\"])*"'

    # Strip the quotes and handle any standard escape characters.
    value = t.value.strip('"')
    value = value.replace(r'\n', '\n')
    value = value.replace(r'\r', '\r')
    value = value.replace(r'\t', '\t')
    value = value.replace(r'\\', '\\')

    t.type = 'STRING'
    t.value = value

    return t


# Handle a single-quoted hex encoded character.
def t_QHEXCH(t):
    r"'\\x[\da-fA-F]+'"

    t.type = 'QCHAR'
    t.value = int(t.value.strip("'")[2:], base=16)

    return t


# Handle a single-quoted character.
def t_QCH(t):
    r"'[^'\n]*['\n]"

    # Make sure these is only one quoted character.  If not then report the
    # error and carry on with a fudged value.
    n_ch = len(t.value)

    if n_ch != 3:
        t.lexer.pm.lexer_error(t,
                "exactly one character expected between single quotes")

        if n_ch == 0:
            t.value = '?'

    t.type = 'QCHAR'
    t.value = t.value[1]

    return t


# The remaining trivial token definitions.
t_ELLIPSIS = r'\.\.\.'
t_LOGICALOR = r'\|\|'
t_SCOPE = r'::'
