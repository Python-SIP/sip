Legacy Command Line Tools
=========================

The command line tools described in this section are provided as an aid in
moving projects from the SIP v4 build system (such as it is) to the SIP v5
build system.  They will be removed in SIP v6.


:program:`sip5`
---------------

:program:`sip5` is a drop-in replacement for the :program:`sip` code generator
included with SIP v4.

The syntax of the :program:`sip5` command line is::

    sip5 [options] [specification]

``specification`` is the name of the specification file for the module.  If it
is omitted then ``stdin`` is used.

The full set of command line options is:

.. program:: sip5

.. option:: -h

    Display a help message.

.. option:: -V

    Display the SIP version number.

.. option:: -a <FILE>

    The name of the QScintilla API file to generate.  This file contains a
    description of the module API in a form that the QScintilla editor
    component can use for auto-completion and call tips.  (The file may also be
    used by the SciTE editor but must be sorted first.)  By default the file is
    not generated.

.. option:: --abi-version <VERSION>

    The version of the ABI implemented by the :mod:`sip` module is ``VERSION``.
    By the default the latest version is used.

.. option:: -B <TAG>

    The tag is added to the list of *backstops*.  The option may be given more
    than once if multiple timelines have been defined.

.. option:: -c <DIR>

    The name of the directory (which must exist) into which all of the
    generated C or C++ code is placed.  By default no code is generated.

.. option:: -D

    Code is generated for a debug build of Python.

.. option:: -e

    Support for C++ exceptions is enabled.  This causes all calls to C++ code
    to be enclosed in ``try``/``catch`` blocks and C++ exceptions to be
    converted to Python exceptions.  By default exception support is disabled.

.. option:: -f

    Warnings are handled as if they were errors and the program terminates.

.. option:: -g

    The Python GIL is released before making any calls to the C/C++ library
    being wrapped and reacquired afterwards.  See :ref:`ref-gil` and the
    :fanno:`ReleaseGIL` and :fanno:`HoldGIL` annotations.

.. option:: -I <DIR>

    The directory is added to the list of directories searched when looking for
    a specification file given in an :directive:`%Include` or
    :directive:`%Import` directive.  Directory separators must always be ``/``.
    This option may be given any number of times.

.. option:: -j <FILES>

    The generated code is split into the given number of files.  This makes it
    easier to use the parallel build facility of most modern implementations of
    ``make``.  By default 1 file is generated for each C structure or C++
    class.

.. option:: -n <NAME>

    The fully qualified name of the :mod:`sip` module (i.e. including the
    package name).

.. option:: -o

    Docstrings will be automatically generated that describe the signature of
    all functions, methods and constructors.

.. option:: -P

    By default SIP generates code to provide access to protected C++ functions
    from Python.  On some platforms (notably Linux, but not Windows) this code
    can be avoided if the ``protected`` keyword is redefined as ``public``
    during compilation.  This can result in a significant reduction in the size
    of a generated Python module.  This option disables the generation of the
    extra code.

.. option:: -r

    Debugging statements that trace the execution of the bindings are
    automatically generated.  By default the statements are not generated.

.. option:: -s <SUFFIX>

    The suffix to use for generated C or C++ source files.  By default ``.c``
    is used for C and ``.cpp`` for C++.

.. option:: -t <TAG>

    The SIP version tag (declared using a :directive:`%Timeline` directive) or
    the SIP platform tag (declared using the :directive:`%Platforms` directive)
    to generate code for.  This option may be given any number of times so long
    as the tags do not conflict.

.. option:: -w

    The display of warning messages is enabled.  By default warning messages
    are disabled.

.. option:: -x <FEATURE>

    The feature (declared using the :directive:`%Feature` directive) is
    disabled.

.. option:: -X <ID:FILE>

    The extract (defined with the :directive:`%Extract` directive) with the
    identifier ``ID`` is written to the file ``FILE``.

.. option:: -y <FILE>

    The name of the Python type hints stub file to generate.  This file
    contains a description of the module API that is compliant with PEP 484.
    By default the file is not generated.

Command line options can also be placed in a file and passed on the command
line using the ``@`` prefix.


:program:`sip5-header`
----------------------

:program:`sip5-header` installs a local copy of the :file:`sip.h` header file
included with SIP v4.

The syntax of the :program:`sip5-header` command line is::

    sip5-header [options] name

``name`` is the fully qualified name of the :mod:`sip` module.

The full set of command line options is:

.. program:: sip5-header

.. option:: -h

    Display a help message.

.. option:: -V

    Display the SIP version number.

.. option:: --abi-version <VERSION>

    The version of the ABI defined by the :file:`sip.h` file is ``VERSION``.
    By the default the latest version is used.

.. option:: --include-dir <DIR>

    The :file:`sip.h` file is written to the directory ``DIR``.  The default is
    the current directory.
