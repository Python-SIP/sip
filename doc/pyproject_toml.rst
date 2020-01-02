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
    The value is a callable that will return an object that is a sub-class
    of :class:`~sipbuild.AbstractProject`.  The default builder factory is
    :file:`project.py`.


``[tool.sip.builder]`` Section
------------------------------

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
otherwise, all values are strings.

**abi-version**
    The value is the version number of the ABI of the :mod:`sip` module being
    used.  By default the latest version is used.

**api-dir**
    The value is the name of a the directory in which a QScintilla :file:`.api`
    file is created.  By default no :file:`.api` file is created.  This is also
    a user option.

**bindings-factory**
    The value is a callable that will return an object that is a sub-class
    of :class:`~sipbuild.Bindings`.  The default bindings factory is
    :class:`~sipbuild.Bindings`.  Bindings factories can also be specified
    programmatically using :attr:`~sipbuild.Project.bindings_factories`.

**build-dir**
    The value is the name of a directory in which all generated files will be
    created.  The directory will not be removed after the build has been
    completed.  The default depends on which build tool is being used.  This is
    also a user option.

**build-tag**
    The value is the build tag to be used in the name of a wheel.  This is also
    a user option.

**builder-factory**
    The value is a callable that will return an object that is a sub-class
    of :class:`~sipbuild.AbstractBuilder`.  The default builder factory is
    :class:`~sipbuild.DistutilsBuilder`.

**console-scripts**
    The value is a list of entry points that defines one or more console
    scripts to be installed as part of the project.

**disable**
    The value is a list of the names of the bindings that are disabled and will
    not be built.  This is also a user option.

**enable**
    The value is a list of the names of the bindings that are enabled and will
    be built.  Any associated configuration tests that would normally be run to
    determine if the bindings should be built are suppressed.  This is also a
    user option.

**dunder-init**
    The boolean value specifies if a :file:`__init__.py` file should be
    installed in the top level package directory.  By default it is not
    installed and the value is ignored for standalone projects.

**minimum-glibc-version**
    The minimum GLIBC version required by the project specified as a 2-tuple of
    the major and minor version numbers.  This is used to determine the correct
    platform tag to use for Linux wheels.  The default version of GLIBC is v2.5
    which corresponds to the ``manylinux1_x86_64`` platform tag.

**name**
    The value is used instead of the value of the ``name`` key in the
    ``[tool.sip.metadata]`` section in the name of an sdist or wheel.  This is
    also a user option.

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
    default progress messages are displayed.  This is also a user option.

**sip-files-dir**
    The value is the name of the directory containing the :file:`.sip`
    specification files.  If the project is a package project then the
    :file:`.sip` files of each set of bindings are assumed to be in their own
    bindings-specific sub-directory.  The default value is the name of the
    directory containing the :file:`pyproject.toml` file.

**sdist-excludes**
    The value is a list of files and directories, expressed as *glob* patterns
    and relative to the directory containing the :file:`pyproject.toml` file,
    that should be excluded from an sdist.

**sip-include-dirs**
    The valus is a list of additional directories that should be searched for
    :file:`.sip` files.

**sip-module**
    The value is the fully qualified package name of the :mod:`sip` module.  If
    it is not specified then the project is assumed to be a standalone project.

**target-dir**
    The value is the name of the directory where the project will be installed
    in.  By default it is the :file:`site-packages` directory of the Python
    installation.  This is also a user option.

**verbose**
    The boolean value enables or disables the display of verbose progress
    messages.  By default verbose progress messages are not displayed.  This is
    also a user option.


Bindings Sections
-----------------

Each set of bindings has its own section called ``[tool.sip.bindings.name]``
where ``name`` is the name of the bindings.  If no bindings are explicitly
defined then SIP will look in the directory containing the
:file:`pyproject.toml` file for a :file:`.sip` file with the same name as the
value of the ``name`` key in the ``[tool.sip.metadata]`` section and, if found,
will assume that it defines the bindings of a standalone project.

Unless stated otherwise, all values are strings.

**builder-settings**
    The value is a list of values that are passed to the builder.  It is up to
    the builder to determine how these values are used.

**concatenate**
    The value, interpreted as a number, specifies that the generated code is
    split into that number of source files.  By default one file is generated
    for each C structure or C++ class.  Specifying a low value can
    significantly speed up the build of large projects.  This is also a user
    option.

**debug**
    The boolean value specifies if a build with debugging symbols is performed.
    By default a debug build is not performed.  This is also a user option.

**define-macros**
    The value is a list of ``#define`` names and values in the form ``"NAME"``
    or ``"NAME=VALUE"``.

**disabled-features**
    The value is a list of disabled :directive:`%Feature` tags.

**docstrings**
    The boolean value specifies if docstrings that describe the signature of
    all functions, methods and constructors should be generated.  By default
    docstrings are generated.  This is also a user option.

**exceptions**
    The boolean values specifies if support for C++ exceptions in the library
    being wrapped is enabled.  By default exception support is disabled.

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
    stub file is not generated.  This is also a user option.

**protected-is-public**
    The boolean value specifies if SIP redefines the ``protected`` keyword as
    ``public`` during compilation.  On non-Windows platforms this can result in
    a significant reduction in the size of a generated Python module.  By
    default SIP redefines the keyword on non-Windows platforms.  This is also a
    user option.

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
    statements are not generated.  This is also a user option.
