:file:`pyproject.toml` Reference
================================

Note that, for the build tools, the keys described in this section are the
standard keys.  Any of these keys could be removed, or new keys added, by build
system extensions including project-specific :file:`project.py` files.

Some string values are interpreted as a Python callable.  A callable can be
interpreted in three different ways.  If the value has a :file:`.py` extension
then it is assumed to be the name of a Python script which is then imported.
The imported module is then searched for an object that is of a type (or
sub-type) expected by the particular key.  If the value is a simple name then
it is assumed to be the name of a module which is then imported and searched as
before.  Finally if the value contains an embedded ``:`` then the part to the
left of the ``:`` is taken to be the name of a module and the part to the right
is taken to be the name of a factory callable within the module.  The factory
is then called to create the required object.


``[build-system]`` Section
--------------------------

The content of this section is defined in `PEP 518
<https://www.python.org/dev/peps/pep-0518/>`__ and should be similar to the
following::

    [build-system]
    requires = ["sip >=5, <6"]
    build-backend = "sipbuild.api"

This specifies that v5 of the ``sip`` package at PyPI should be used.  You may
want to adjust the ``requires`` value if you use features introduced in a later
version of SIP.


``[tool.sip]`` Section
----------------------

The key/values in this section apply to the build system as a whole.  Unless
stated otherwise, all values are strings.

**project-factory**
    The value is used to identify a callable that will return an object that is
    a sub-class of :class:`~sipbuild.AbstractProject`.  If the value is the
    name of a :file:`.py` file then that file is evaluated and the resulting
    module is searched for a type object that is sub-classed from
    :class:`~sipbuild.AbstractProject`.  Otherwise the value must be the name
    of a module to be imported which is then searched for an appropriate type
    object.  The name of the module may have the name of the callable appended
    (and separated by ``:``) in which case the type of the object is ignored.
    The default project factory is :file:`project.py`.


``[tool.sip.builder]`` Section
------------------------------

Unless stated otherwise, the values of all list options may contain environment
markers as defined in `PEP 508 <https://www.python.org/dev/peps/pep-0508/>`__.

The SIP build system does not define any key/values in this section but build
system extensions may do so.


``[tool.sip.metadata]`` Section
-------------------------------

The key/values in this section are interpreted as meta-data describing the
project as specified in `PEP 566
<https://www.python.org/dev/peps/pep-0566/>`__.  SIP will provide default
values for ``metadata-version``, ``requires-python`` and ``version``.  The only
value that must be specified is ``name``.

SIP also implements the additional ``description-file`` key.  The value of this
is the name of a file (relative to the directory containing
:file:`pyproject.toml`) that contains a description of the project.  The
description is read and appended to the meta-data.

If a value is a list of individual values then the key will be repeated in the
generated meta-data for each of those values.

Note that SIP does not check the validity of the key/values in this section.


``[tool.sip.project]`` Section
------------------------------

The key/values in this section apply to the project as a whole.  Unless stated
otherwise, all values are strings.  Unless stated otherwise, the values of all
list options may contain environment markers as defined in `PEP 508
<https://www.python.org/dev/peps/pep-0508/>`__.

**abi-version**
    The minimum version number of the ABI of the :mod:`sip` module being used.
    If only the major version number is specified then the minor version
    defaults to 0.  By the default the latest major version is used.

**api-dir**
    The value is the name of a the directory in which a QScintilla :file:`.api`
    file is created.  By default no :file:`.api` file is created.  There is
    also a corresponding command line option.

**bindings-factory**
    The value is a callable that will return an object that is a sub-class
    of :class:`~sipbuild.Bindings`.  The default bindings factory is
    :class:`~sipbuild.Bindings`.  Bindings factories can also be specified
    programmatically using :attr:`~sipbuild.Project.bindings_factories`.

**build-dir**
    The value is the name of a directory in which all generated files will be
    created.  The directory will not be removed after the build has been
    completed.  The default depends on which build tool is being used.  There
    is also a corresponding command line option.

**build-tag**
    The value is the build tag to be used in the name of a wheel.  There is
    also a corresponding command line option.

**builder-factory**
    The value is a callable that will return an object that is a sub-class
    of :class:`~sipbuild.AbstractBuilder`.  For Python v3.10 and later the
    default builder factory is :class:`~sipbuild.SetuptoolsBuilder`.  For
    earlier versions of Python the default builder factory is
    :class:`~sipbuild.DistutilsBuilder`.

**compile**
    The boolean value specifies if the generated code is to be compiled.
    By default it is compiled.  There is also a corresponding command line
    option.

**console-scripts**
    The value is a list of entry points that defines one or more console
    scripts to be installed as part of the project.

**disable**
    The value is a list of the names of the bindings that are disabled and will
    not be built.  There is also a corresponding command line option.

**dunder-init**
    The boolean value specifies if a :file:`__init__.py` file should be
    installed in the top level package directory.  By default it is not
    installed and the value is ignored for standalone projects.  If it is set
    and at least one set of bindings specify that a Python type hints stub file
    be generated (by setting the ``pep484-pyi`` option) then the stub file for
    the ``sip`` module and a PEP 561-compatible ``py.typed`` marker file is
    also installed.

**distinfo**
    The boolean value specifies if a :file:`.dist-info` directory is to be
    created when a project is installed.  By default it is enabled.  There is
    also a corresponding command line option.

**enable**
    The value is a list of the names of the bindings that are enabled and will
    be built.  Any associated configuration tests that would normally be run to
    determine if the bindings should be built are suppressed.  There is also a
    corresponding command line option.

**gui-scripts**
    The value is a list of entry points that defines one or more GUI scripts to
    be installed as part of the project.

**manylinux**
    The boolean value specifies if support for ``manylinux`` in the platform
    tag of a name of a wheel is enabled.  By default ``manylinux`` support is
    enabled.  It should only be disabled if support for older versions of
    :program:`pip` is required.  There is also a corresponding command line
    option.

**minimum-glibc-version**
    The minimum GLIBC version required by the project specified as a 2-tuple of
    the major and minor version numbers.  This is used to determine the correct
    platform tag to use for Linux wheels.  The default version of GLIBC is v2.5
    which corresponds to ``manylinux1``.  It is ignored if the ``manylinux``
    option is False.

**minimum-macos-version**
    The minimum macOS version required by the project specified as a 2-tuple of
    the major and minor version numbers.  This is used to determine the correct
    platform tag to use for macOS wheels.  The default version is that required
    by the Python interpreter.

**name**
    The value is used instead of the value of the ``name`` key in the
    ``[tool.sip.metadata]`` section in the name of an sdist or wheel.  There is
    also a corresponding command line option.

**py-debug**
    The boolean value specifies if a debug build of Python is being used.  By
    default this is determined dynamically from the Python installation.

**py-include-dir**
    The value is the name of the directory containing the :file:`Python.h`
    header file.  By default this is determined dynamically from the Python
    installation.

**py-platform**
    The value is the target Python platform.  By default this is determined
    dynamically from the Python installation.

**py-major-version**
    The value is the major version number of the version of Python being
    targetted.  By default this is determined dynamically from the Python
    installation.

**py-minor-version**
    The value is the minor version number of the version of Python being
    targetted.  By default this is determined dynamically from the Python
    installation.

**quiet**
    The boolean value enables or disables the display of progress messages.  By
    default progress messages are displayed.  There is also a corresponding
    command line option.

**scripts-dir**
    The value is the name of the directory where any project scripts will be
    installed in.  If name is relative then it is taken as relative to the
    target directory.  By default the directory containing the Python
    interpreter is used.  There is also a corresponding command line option.

**sdist-excludes**
    The value is a list of files and directories, expressed as *glob* patterns
    and relative to the directory containing the :file:`pyproject.toml` file,
    that should be excluded from an sdist.

**sip-files-dir**
    The value is the name of the directory containing the :file:`.sip`
    specification files.  If the project is a package project then the
    :file:`.sip` files of each set of bindings are assumed to be in their own
    bindings-specific sub-directory.  The default value is the name of the
    directory containing the :file:`pyproject.toml` file.

**sip-include-dirs**
    The value is a list of additional directories that should be searched for
    :file:`.sip` files.

**sip-module**
    The value is the fully qualified package name of the :mod:`sip` module.  If
    it is not specified then the project is assumed to be a standalone project.

**target-dir**
    The value is the name of the directory where the project will be installed
    in.  By default it is the :file:`site-packages` directory of the Python
    installation.  There is also a corresponding command line option.

**verbose**
    The boolean value enables or disables the display of verbose progress
    messages.  By default verbose progress messages are not displayed.  There
    is also a corresponding command line option.

**version-info**
    The boolean value determines if the generated code includes a reference to
    the SIP version number.  By default a reference is included.  There is also
    a corresponding command line option.

**wheel-includes**
    The values is a list of files and directories, specified as *glob* patterns
    and relative to the project directory, that should be included in a wheel.
    If an element of list is a string then it is a pattern and files and
    directories are installed in the target directory.  If an element is a
    2-tuple then the first part is the pattern and the # second part is the
    name of a sub-directory relative to the target directory where the files
    and directories are installed.


Bindings Sections
-----------------

Each set of bindings has its own section called ``[tool.sip.bindings.name]``
where ``name`` is the name of the bindings.  If no bindings are explicitly
defined then SIP will look in the directory containing the
:file:`pyproject.toml` file for a :file:`.sip` file with the same name as the
value of the ``name`` key in the ``[tool.sip.metadata]`` section and, if found,
will assume that it defines the bindings of a standalone project.

Unless stated otherwise, all values are strings.  Unless stated otherwise, the
values of all list options may contain environment markers as defined in `PEP
508 <https://www.python.org/dev/peps/pep-0508/>`__.

**builder-settings**
    The value is a list of values that are passed to the builder.  It is up to
    the builder to determine how these values are used.

**concatenate**
    The value, interpreted as a number, specifies that the generated code is
    split into that number of source files.  By default one file is generated
    for each C structure or C++ class.  Specifying a low value can
    significantly speed up the build of large projects.  There is also a
    corresponding command line option.

**debug**
    The boolean value specifies if a build with debugging symbols is performed.
    By default a debug build is not performed.  There is also a corresponding
    command line option.

**define-macros**
    The value is a list of ``#define`` names and values in the form ``"NAME"``
    or ``"NAME=VALUE"``.

**disabled-features**
    The value is a list of disabled :directive:`%Feature` tags.  There is also
    a corresponding command line option.

**docstrings**
    The boolean value specifies if docstrings that describe the signature of
    all functions, methods and constructors should be generated.  By default
    docstrings are generated.  There is also a corresponding command line
    option.

**exceptions**
    The boolean values specifies if support for C++ exceptions in the library
    being wrapped is enabled.  By default exception support is disabled.

**extra-compile-args**
    The value is a list of extra command line arguments to pass to the
    compiler.

**extra-link-args**
    The value is a list of extra command line arguments to pass to the linker.

**extra-objects**
    The value is a list of extra compiled object files to link.

**generate-extracts**
    The value is a list of extracts (defined by the :directive:`%Extract`
    directive).  Each value is the identifier of the extract and the name of
    the file that the extract is written to separated by a ``:``.

**headers**
    The value is a list of additional :file:`.h` header files needed to build
    the bindings.

**include-dirs**
    The value is a list of additional directories to search for :file:`.h`
    header files.

**internal**
    The boolean value specifies if the set of bindings are internal.  Internal
    bindings never have :file:`.sip`, :file:`.pyi` or :file:`.api` files
    installed.  By default the bindings are not internal.

**libraries**
    The value is a list of libraries to link the source code with and should
    include any library being wrapped.

**library-dirs**
    The value is a list of directories that will be searched, in addition to
    the standard system directories, for any libraries.

**pep484-pyi**
    The boolean value specifies if a Python type hints stub file is generated.
    This file contains a description of the module's API that is compliant with
    `PEP 484 <https://www.python.org/dev/peps/pep-0484/>`__.  By default the
    stub file is not generated.  There is also a corresponding command line
    option.

**protected-is-public**
    The boolean value specifies if SIP redefines the ``protected`` keyword as
    ``public`` during compilation.  On non-Windows platforms this can result in
    a significant reduction in the size of a generated Python module.  By
    default SIP redefines the keyword on non-Windows platforms.  There is also
    a corresponding command line option.

**release-gil**
    The boolean value specifies if the Python GIL is always released when
    calling a function in the library being wrapped irrespective of any
    :fanno:`ReleaseGIL` annotation.  By default the GIL is only released as
    determined by :fanno:`ReleaseGIL`.

**static**
    The boolean value specifies that the bindings should be built as a static
    library.  By default the bindings are built as a dynamically loaded
    library.  Note that not all builders (including the default
    :class:`~sipbuild.DistutilsBuilder` builder) can build static libraries.

**sip-file**
    The name of the :file:`.sip` specification file that defines the set of
    bindings.  If it is a relative name then it is assumed to be relative to
    the value of the ``sip-files-dir`` key of the ``[tool.sip.project]``
    section.  By default it is the name of the bindings with :file:`.sip`
    appended.

**source-suffix**
    The value is the extension used for the generated source files.  By default
    this is :file:`.c` for C bindings and :file:`.cpp` for C++ bindings.

**sources**
    The value is a list of additional C/C++ source files needed to build the
    bindings.

**tags**
    The value is a list of :directive:`%Platforms` and :directive:`%Timeline`
    tags used to configure the bindings.

**tracing**
    The boolean value specifies that debugging statements that trace the
    execution of the bindings are automatically generated.  Be default the
    statements are not generated.  There is also a corresponding command line
    option.
