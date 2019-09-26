:file:`pyproject.toml` Reference
================================

Note that, for the build tools, the keys described in this section are the
standard keys.  Any of these keys could be removed, or new keys added, by build
system extensions including project-specific :file:`project.py` files.


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


``[tool.sip.metadata]`` Section
-------------------------------

The key/values in this section are interpreted as meta-data describing the
project as specified in `PEP 566
<https://www.python.org/dev/peps/pep-0566/>`__.  SIP will provide default
values for ``metadata-version``, ``requires-python`` and ``version``.  The only
value that must be specified is ``name``.

If a value is a list of individual values then the key will be repeated in the
generated meta-data for each of those values.

Note that SIP does not check the validity of the key/values in this section.


``[tool.sip.project]`` Section
------------------------------

The key/values in this section apply to the project as a whole.  Unless stated
otherwise, all values are strings.

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
    :class:`~sipbuild.Bindings`.

**build-dir**
    The value is the name of a directory in which all generated files will be
    created.  The directory will not be removed after the build has been
    completed.  The default depends on which build tool is being used.  This is
    also a user option.

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

TODO
