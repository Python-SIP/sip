Command Line Tools
==================

Note that, for the build tools, the command line options described in this
section are the standard options.  Any of these options could be removed, or
new options added, by build system extensions including project-specific
:file:`project.py` files.


:program:`sip-build`
--------------------

:program:`sip-build` builds a project but does not install it.  This is useful
when developing a set of bindings.

The syntax of the :program:`sip-build` command line is::

    sip-build [options]

The full set of command line options is:

.. program:: sip-build

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --quiet

    All progress messages are disabled.

.. option:: --verbose

    Verbose progress messages are enabled.

.. option:: --api-dir <DIR>

    A QScintilla :file:`.api` file is created in ``DIR``.

.. option:: --build-dir <DIR>

    ``DIR`` is created as a build directory in which all generated files will
    be created.  The build directory is not removed after the build has been
    completed.  The default value is ``build``.

.. option:: --concatenate <N>

    The generated code is split into ``N`` files.  By default one file is
    generated for each C structure or C++ class.  Specifying a low value of
    ``N`` can significantly speed up the build of large projects.

.. option:: --disable <NAME>

    The ``NAME`` bindings are disabled and will not be built.  This option may
    be specified multiple times.  It is only available if the project contains
    multiple sets of bindings.

.. option:: --enable <NAME>

    The ``NAME`` bindings are enabled and will be built.  Any associated
    configuration tests that would normally be run to determine if the bindings
    should be built are suppressed.  This option may be specified multiple
    times.  It is only available if the project contains multiple sets of
    bindings.

.. option:: --debug

    A build with debugging symbols is performed.

.. option:: --no-docstrings

    The generation of docstrings that describe the signature of all functions,
    methods and constructors is disabled.

.. option:: --pep484-pyi

    The generation of Python type hints stub files is enabled.  These files
    contain a description of a module's API that is compliant with `PEP 484
    <https://www.python.org/dev/peps/pep-0484/>`__.

.. option:: --protected-is-public

    SIP can generate code to provide access to protected C++ functions from
    Python.  On non-Windows platforms this code can be avoided if the
    ``protected`` keyword is redefined as ``public`` during compilation.  This
    can result in a significant reduction in the size of a generated Python
    module.  This option enables the redefinition of ``protected`` and is the
    default on all platforms except Windows.

.. option:: --no-protected-is-public

    This option disables the redefinition of ``protected`` to access protected
    C++ functions from Python and is the default on Windows.

.. option:: --target-dir <DIR>

    The project will eventually be installed in ``DIR``.  By default it is the
    :file:`site-packages` directory of the Python installation.

.. option:: --tracing

    Debugging statements that trace the execution of the bindings are
    automatically generated.  By default the statements are not generated.


:program:`sip-distinfo`
-----------------------

:program:`sip-distinfo` creates and populates a :file:`.dist-info` directory of
an installation or a wheel.  It is provided for build systems that extend the
SIP build system and need to create the :file:`.dist-info` directory from an
external tool such as :program:`make`.

The syntax of the :program:`sip-distinfo` command line is::

    sip-distinfo [options] directory

``directory`` is the full path name of the directory to create.

The full set of command line options is:

.. program:: sip-distinfo

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --console-script <ENTRY-POINT>

    The console entry point ``ENTRY-POINT`` is added to the wheel.  It is
    ignored if the :option:`--wheel-tag` option is not specified.  This option
    may be specified multiple times.

.. option:: --generator <NAME>

    If the :option:`--wheel-tag` option is specified then ``NAME`` is written
    as the ``Generator`` in the :file:`WHEEL` file in the :file:`.dist-info`
    directory.  Otherwise ``NAME`` is written to the :file:`INSTALLER` file.
    By default ``sip-distinfo`` is written.

.. option:: --inventory <FILE>

    ``FILE`` contains a list of the relative names of the files, one per line, 
    that comprise the installation or wheel contents.  This option must be
    specified.

.. option:: --prefix <DIR>

    This option is provided as an aid to Linux package builders.  ``DIR`` is
    used to pass the commonly used values of ``DESTDIR`` or ``INSTALL_ROOT``.
    If specified it should have a trailing native path separator.

.. option:: --project-root <DIR>

    The name of the directory containing the project's :file:`pyproject.toml`
    file is ``DIR``.  This option must be specified.

.. option:: --requires-dist <EXPR>

    ``EXPR`` is added to the list of prerequisites written to the
    :file:`METADATA` file in the :file:`.dist-info` directory.  It is normally
    used to specify a particular version of a package project's :mod:`sip`
    module.  This option may be specified multiple times.

.. option:: --wheel-tag <TAG>

    ``TAG`` is written as the ``Tag`` in the :file:`WHEEL` file in the
    :file:`.dist-info` directory.


:program:`sip-install`
----------------------

:program:`sip-install` builds and installs a project.

The syntax of the :program:`sip-install` command line is::

    sip-install [options]

The full set of command line options is:

.. program:: sip-install

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --quiet

    All progress messages are disabled.

.. option:: --verbose

    Verbose progress messages are enabled.

.. option:: --api-dir <DIR>

    A QScintilla :file:`.api` file is created in ``DIR``.

.. option:: --build-dir <DIR>

    ``DIR`` is created as a build directory in which all generated files will
    be created.  This build directory is not removed after the build has been
    completed.  By default a temporary build directory is created which is
    removed after the build has been completed.

.. option:: --concatenate <N>

    The generated code is split into ``N`` files.  By default one file is
    generated for each C structure or C++ class.  Specifying a low value of
    ``N`` can significantly speed up the build of large projects.

.. option:: --disable <NAME>

    The ``NAME`` bindings are disabled and will not be built.  This option may
    be specified multiple times.  It is only available if the project contains
    multiple sets of bindings.

.. option:: --enable <NAME>

    The ``NAME`` bindings are enabled and will be built.  Any associated
    configuration tests that would normally be run to determine if the bindings
    should be built are suppressed.  This option may be specified multiple
    times.  It is only available if the project contains multiple sets of
    bindings.

.. option:: --debug

    A build with debugging symbols is performed.

.. option:: --no-docstrings

    The generation of docstrings that describe the signature of all functions,
    methods and constructors is disabled.

.. option:: --pep484-pyi

    The generation of Python type hints stub files is enabled.  These files
    contain a description of a module's API that is compliant with `PEP 484
    <https://www.python.org/dev/peps/pep-0484/>`__.

.. option:: --protected-is-public

    SIP can generate code to provide access to protected C++ functions from
    Python.  On non-Windows platforms this code can be avoided if the
    ``protected`` keyword is redefined as ``public`` during compilation.  This
    can result in a significant reduction in the size of a generated Python
    module.  This option enables the redefinition of ``protected`` and is the
    default on all platforms except Windows.

.. option:: --no-protected-is-public

    This option disables the redefinition of ``protected`` to access protected
    C++ functions from Python and is the default on Windows.

.. option:: --target-dir <DIR>

    The project will be installed in ``DIR``.  By default it is the
    :file:`site-packages` directory of the Python installation.

.. option:: --tracing

    Debugging statements that trace the execution of the bindings are
    automatically generated.  By default the statements are not generated.


:program:`sip-module`
---------------------

:program:`sip-module` builds one of more of the elements of the :mod:`sip`
module for a set of package projects.

The syntax of the :program:`sip-module` command line is::

    sip-module [options] name

``name`` is the fully qualified name of the :mod:`sip` module (i.e. including
the package name).

The full set of command line options is:

.. program:: sip-module

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --abi-version <VERSION>

    The version of the ABI implemented by the :mod:`sip` module is ``VERSION``.
    By the default the latest version is used.

.. option:: --project <NAME>

    The name of the project as it would appear on PyPI is ``NAME``.  By default
    the name is derived from the fully qualified name of the :mod:`sip`
    module.

.. option:: --sdist

    Create an sdist which can then be installed by :program:`pip` or uploaded
    to PyPI.

.. option:: --setup-cfg <FILE>

    ``FILE`` is copied to the sdist as :file:`setup.cfg` instead of the default
    version.  This allows the meta-data included in the sdist to be customised.
    A number of macros may be specified in the :file:`setup.cfg` file:

        ``@SIP_MODULE_FQ_NAME@`` is replaced by the fully qualified name
        of the :mod:`sip` module.

        ``@SIP_MODULE_PACKAGE_NAME@`` is replaced by the module's project
        top-level package name.

        ``@SIP_MODULE_PROJECT_NAME@`` is replaced by the module's project name
        as it would appear on PyPI.

        ``@SIP_MODULE_VERSION@`` is replaced by the version number of the
        module.

.. option:: --sip-h

    Create a :file:`sip.h` header file that defines the C ABI implemented by
    the :mod:`sip` module.

.. option:: --sip-rst

    Create a :file:`sip.rst` file that documents the Python API implemented by
    the :mod:`sip` module.

.. option:: --target-dir <DIR>

    Each of the module's elements will be created in ``DIR``.


:program:`sip-sdist`
--------------------

:program:`sip-sdist` creates an sdist (a source distribution) than be uploaded
to PyPI.

The syntax of the :program:`sip-sdist` command line is::

    sip-sdist [options]

The full set of command line options is:

.. program:: sip-sdist

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --name <NAME>

    ``NAME`` is used instead of the PyPI project name in the
    :file:`pyproject.toml` file in the name of the sdist file.


:program:`sip-wheel`
--------------------

:program:`sip-wheel` creates a wheel (a binary distribution) than be uploaded
to PyPI.

The syntax of the :program:`sip-wheel` command line is::

    sip-wheel [options]

The full set of command line options is:

.. program:: sip-wheel

.. option:: -h, --help

    Display a help message.

.. option:: -V, --version

    Display the SIP version number.

.. option:: --quiet

    All progress messages are disabled.

.. option:: --verbose

    Verbose progress messages are enabled.

.. option:: --api-dir <DIR>

    A QScintilla :file:`.api` file is created in ``DIR``.

.. option:: --build-dir <DIR>

    ``DIR`` is created as a build directory in which all generated files will
    be created.  This build directory is not removed after the build has been
    completed.  By default a temporary build directory is created which is
    removed after the build has been completed.

.. option:: --concatenate <N>

    The generated code is split into ``N`` files.  By default one file is
    generated for each C structure or C++ class.  Specifying a low value of
    ``N`` can significantly speed up the build of large projects.

.. option:: --disable <NAME>

    The ``NAME`` bindings are disabled and will not be built.  This option may
    be specified multiple times.  It is only available if the project contains
    multiple sets of bindings.

.. option:: --enable <NAME>

    The ``NAME`` bindings are enabled and will be built.  Any associated
    configuration tests that would normally be run to determine if the bindings
    should be built are suppressed.  This option may be specified multiple
    times.  It is only available if the project contains multiple sets of
    bindings.

.. option:: --name <NAME>

    ``NAME`` is used instead of the PyPI project name in the
    :file:`pyproject.toml` file in the name of the wheel file.

.. option:: --debug

    A build with debugging symbols is performed.

.. option:: --no-docstrings

    The generation of docstrings that describe the signature of all functions,
    methods and constructors is disabled.

.. option:: --pep484-pyi

    The generation of Python type hints stub files is enabled.  These files
    contain a description of a module's API that is compliant with `PEP 484
    <https://www.python.org/dev/peps/pep-0484/>`__.

.. option:: --protected-is-public

    SIP can generate code to provide access to protected C++ functions from
    Python.  On non-Windows platforms this code can be avoided if the
    ``protected`` keyword is redefined as ``public`` during compilation.  This
    can result in a significant reduction in the size of a generated Python
    module.  This option enables the redefinition of ``protected`` and is the
    default on all platforms except Windows.

.. option:: --no-protected-is-public

    This option disables the redefinition of ``protected`` to access protected
    C++ functions from Python and is the default on Windows.

.. option:: --tracing

    Debugging statements that trace the execution of the bindings are
    automatically generated.  By default the statements are not generated.
