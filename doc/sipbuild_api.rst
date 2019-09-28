.. module:: sipbuild
    :synopsis: The SIP build system.


:mod:`sipbuild` Module Reference
================================

The :mod:`sipbuild` module implements the SIP build system.  The module may be
used by project-specific :program:`project.py` scripts or extended by other
build systems.

.. note::
    Most of the classes described here have additional methods and attributes.
    Unless they are documented here they are **not** part of the public API and
    may be changed in any future release.


:class:`~sipbuild.AbstractBuilder`
----------------------------------

.. class:: AbstractBuilder(project, \*\*kwargs)

    An abstract class that defines the API of a builder.

    :param Project project: is the project.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :class:`~sipbuild.Option` defined by the builder.  An
        :class:`~sipbuild.Option` value set in this way cannot be overridden in
        the :file:`pyproject.toml` file or by using a tool command line option.

    .. method:: build()
        :abstractmethod:

        Build the project but do not install it.

    .. method:: build_sdist(sdist_directory)
        :abstractmethod:

        Build an sdist for the project.

        :param str sdist_directory: is the name of the directory in which the
            sdist is created.
        :return: the name of the sdist file (excluding any path).

    .. method:: build_wheel(wheel_directory)
        :abstractmethod:

        Build a wheel for the project.

        :param str wheel_directory: is the name of the directory in which the
            wheel is created.
        :return: the name of the wheel file (excluding any path).

    .. method:: install()
        :abstractmethod:

        Build and install the project.

    .. attribute:: project

        The :class:`~sipbuild.Project` object.


:class:`~sipbuild.AbstractProject`
----------------------------------

.. class:: AbstractProject

    An abstract class that defines the API of a project.

    .. method:: build()
        :abstractmethod:

        Build the project but do not install it.

    .. method:: build_sdist(sdist_directory)
        :abstractmethod:

        Build an sdist for the project.

        :param str sdist_directory: is the name of the directory in which the
            sdist is created.
        :return: the name of the sdist file (excluding any path).

    .. method:: build_wheel(wheel_directory)
        :abstractmethod:

        Build a wheel for the project.

        :param str wheel_directory: is the name of the directory in which the
            wheel is created.
        :return: the name of the wheel file (excluding any path).

    .. method:: import_callable(name, base_type)
        :staticmethod:

        Import a callable from a script or module.  The callable is identified
        either by its name (if specified) or its type.

        :param str name: is a script (with a :file:`.py` extension), a module
            or the name of an object in a module (specified as
            ``module:name``).
        :param type base_type: is the type of the callable and is ignored if
            the name of the callable is explicitly specified.
        :return: the callable.

    .. method:: install()
        :abstractmethod:

        Build and install the project.

    .. method:: setup(pyproject, tool, tool_description)
        :abstractmethod:

        Setup the project from the :file:`pyproject.toml` file.

        :param PyProject pyproject: is the parsed :file:`pyproject.toml` file.
        :param str tool: is the name of the tool, either ``'build'``,
            ``'install'``, ``'pep517'``, ``'sdist'`` or ``'wheel'``.
        :param str tool_description: is a short description of the tool.


:class:`~sipbuild.Bindings`
---------------------------

.. class:: Bindings(project, name, \*\*kwargs)

    The encapsulation of a set of bindings.

    :param Project project: is the project.
    :param str name: is the name of the bindings.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :class:`~sipbuild.Option` defined by the bindings.
        An :class:`~sipbuild.Option` value set in this way cannot be overridden
        in the :file:`pyproject.toml` file or by using a tool command line
        option.

    .. method:: apply_nonuser_defaults(tool)

        Called by the bindings to set the default values of any non-user
        options (i.e. those that cannot be set from a tool command line).  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :param str tool: is the name of the tool being used.

    .. method:: apply_user_defaults(tool)

        Called by the bindings to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. method:: get_options()

        Called by the bindings to get the list of the bindings's options.  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :class:`~sipbuild.Option` objects.

    .. method:: is_buildable()

        Called by the builder to determine if the bindings are buildable.  This
        will not be called if the bindings have been explicitly enabled.  The
        default implementation returns ``True``.

        :return: ``True`` if the bindings are buildable.

    .. attribute:: project

        The :class:`~sipbuild.Project` object.


:class:`~sipbuild.Buildable`
----------------------------

.. class:: Buildable(project, name)

    Encapsulate a generic buildable.

    :param Project project: is the project.
    :param str name: is the name of the buildable.

    .. attribute:: build_dir

        The name of the buildable-specific build directory.  This will be
        created automatically.

    .. attribute:: build_settings

        A list of values that are passed to the builder. It is up to the
        builder to determine how these values are used.

    .. attribute:: installables

        The list of :class:`~sipbuild.Installable` objects created by the
        builder to describe what was built.

    .. attribute:: name

        The name of the buildable.

    .. attribute:: project

        The :class:`~sipbuild.Project` object.


:class:`~sipbuild.BuildableBindings`
------------------------------------

.. class:: BuildableBindings(bindings, fq_name, \*, uses_limited_api=False)

    A :class:`~sipbuild.BuildableModule` sub-class that encapsulates the Python
    extension module for a set of bindings.

    :param Bindings bindings: is the bindings.
    :param str fq_name: is the fully qualified name of the bindings module.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. attribute:: bindings

        The :class:`~sipbuild.Bindings` object.


:class:`~sipbuild.BuildableExecutable`
--------------------------------------

.. class:: BuildableExecutable(project, name, target, \*, uses_limited_api=False)

    A :class:`~sipbuild.BuildableFromSources` sub-class that encapsulates an
    executable.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str target: is the platform-independent name of the executable being
        built.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.


:class:`~sipbuild.BuildableFromSources`
---------------------------------------

.. class:: BuildableFromSources(project, name, target, \*, uses_limited_api=False)

    A :class:`~sipbuild.Buildable` sub-class that encapsulates a target that is
    built from source code.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str target: is the name of the target being built.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. attribute:: debug

        ``True`` if a build with debugging symbols should be performed.

    .. attribute:: define_macros

        The list of ``#define`` names and values in the form ``"NAME"`` or
        ``"NAME=VALUE"``.

    .. attribute:: headers

        The list of :file:`.h` header files.

    .. attribute:: include_dirs

        The list of directories that will be searched, in additional to the
        standard system directores, for :file:`.h` header files.

    .. attribute:: libraries

        The list of libraries to link the source code with.

    .. attribute:: library_dirs

        The list of directories that will be searched, in addition to the
        standard system directories, for any libraries.

    .. method:: make_names_relative()

        Make all the file names relative to the build directory.  This isn't
        necessary but can make any build files easier to read by the user.

    .. attribute:: sources

        The list of source files.

    .. attribute:: target

        The name of the target being built.

    .. attribute:: uses_limited_api

        ``True`` if the source code uses only the limited Python API.


:class:`~sipbuild.BuildableModule`
----------------------------------

.. class:: BuildableModule(project, name, fq_name, \*, uses_limited_api=False)

    A :class:`~sipbuild.BuildableFromSources` sub-class that encapsulates a
    Python extension module.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str fq_name: is the fully qualified name of the module.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. attribute:: fq_name

        The fully qualified name of the module.

    .. method:: get_install_subdir()

        Get the name of the sub-directory (relative to any future target
        installation directory) that the module should be installed in.

        :return: the name of the sub-directory.

    .. method:: get_module_extension()

        Get the platform-specific file name extension that a module should
        have.

        :return: the extension.

    .. attribute:: static

        ``True`` if the module should be built as a static library.


:class:`~sipbuild.Builder`
--------------------------

.. class:: Builder(project, \*\*kwargs)

    The default base implementation of a builder.

    :param Project project: is the project
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :class:`~sipbuild.Option` defined by the builder.  An
        :class:`~sipbuild.Option` value set in this way cannot be overridden in
        the :file:`pyproject.toml` file or by using a tool command line option.

    .. method:: apply_nonuser_defaults(tool)

        Called by the builder to set the default values of any non-user options
        (i.e. those that cannot be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. method:: apply_user_defaults(tool)

        Called by the builder to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. method:: build_executable(buildable, \*, fatal=True)
        :abstractmethod:

        Build an executable from a buildable.

        :param BuildableExecutable buildable: is the buildable.
        :param bool fatal: is ``True`` if a :exc:`~sipbuild.UserException`
            should be raised if the build failed.
        :return: the relative path name of the built executable.

    .. method:: build_project(target_dir, \*, wheel_tag=None)
        :abstractmethod:

        Build the project either to be installed for use or to create a wheel.

        :param str target_dir: is the directory in which the project will be
            installed in.
        :param str wheel_tag: is the wheel tag if a wheel is being created.

    .. method:: get_options()

        Called by the builder to get the list of the builder's options.  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :class:`~sipbuild.Option` objects.

    .. method:: install_project(target_dir, \*, wheel_tag=None)
        :abstractmethod:

        Install a built project either for use or to create a wheel.

        :param str target_dir: is the directory in which the project will be
            installed in.
        :param str wheel_tag: is the wheel tag if a wheel is being created.


:class:`~sipbuild.DistutilsBuilder`
-----------------------------------

.. class:: DistutilsBuilder(project, \*\*kwargs)

    A :class:`~sipbuild.Builder` that uses the Python :mod:`distutils` package
    to perform builds.

    :param Project project: is the project.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :class:`~sipbuild.Option` defined by the builder.  An
        :class:`~sipbuild.Option` value set in this way cannot be overridden in
        the :file:`pyproject.toml` file or by using a tool command line option.


:func:`~sipbuild.handle_exception`
----------------------------------

.. function:: handle_exception(e)

    Handle an exception by displaying an appropriate error message to
    ``stdout``.  The process is then terminated with a non-zero exit code.

    :param exception e: is the exception to be handled.


:class:`~sipbuild.Installable`
------------------------------

.. class:: Installable(name, \*, target_subdir=None)

    Encapsulate a list of files that will be installed in the same directory.

    :param str name: is the name of the installable.
    :param str target_subdir: is the relative path name of a sub-directory in
        which the installable's files will be installed.  If it is an absolute
        path name then it is used as the eventual full target directory.

    .. attribute:: files

        The list of file names to be installed.

    .. method:: get_full_target_dir(target_dir)

        Get the full path name of the directory where the installable's file
        will be installed.

        :param str target_dir: is the name of target directory.
        :return: the full path name of the sub-directory within the target
            directory where the files will be installed.

    .. method:: install(target_dir, installed, \*, do_install=True)

        Install the installable's files in a target directory.

        :param str target_dir: is the name of the target directory.
        :param list[str] installed: is a list of installed files which is
            updated with the newly installed files.  The list is always updated
            even if the files are not actually installed.
        :param bool do_install: is ``True`` if the files are actually to be
            installed.

    .. attribute:: name

        The name of the installable.

    .. attribute:: target_subdir

        The name of the target sub-directory.


:class:`~sipbuild.Option`
-------------------------

.. class:: Option(name, \*, option_type=str, choices=None, default=None, help=None, metavar=None, inverted=False, tools=None)

    Encapsulate a configurable option.  Option values may be specified in code,
    in the :file:`pyproject.toml` file or on the command line of SIP's tools.
    The value of an option is accessed as an attribute of the object for which
    the option is defined.

    :param str name: is the name of the option.  Any '_' in the name will be
        replaced by '-' in the context of a :file:`pyproject.toml` key or a
        command line option.
    :param type option_type: is the type of the value, either ``bool``,
        ``int``, ``list`` or ``str`` (the default).
    :param list choices: is a list of values that are the valid for the option.
    :param default: is the default value.
    :param str help: is the short help text.  This must be specified if the
        option is to be used as a tool command line option.
    :param str metavar: is the name of the option's value when used in tool
        usage messages.
    :param bool inverted: is ``True`` if, when used as a tool command
        line option, the name should be preceded by ``no-``.
    :param list[str] tools: is the list of tools that use the option as a
        command line option.  If it isn't specified then the list of build
        tools is used, i.e. ``['build', 'install', 'pep517', 'wheel']``.


:class:`~sipbuild.Project`
--------------------------

.. class:: Project(\*\*kwargs)

    The default implementation of a project.  It has an associated builder
    which it uses to build a set of buildables.  Building a buildable may
    create one or more installables.

    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :class:`~sipbuild.Option` defined by the project.  An
        :class:`~sipbuild.Option` value set in this way cannot be overridden in
        the :file:`pyproject.toml` file or by using a tool command line option.

    .. method:: apply_nonuser_defaults(tool)

        Called by the project to set the default values of any non-user options
        (i.e. those that cannot be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. method:: apply_user_defaults(tool)

        Called by the project to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. attribute:: bindings

        The :class:`~collections.OrderedDict` of :class:`~sipbuild.Bindings`
        objects keyed by the name of the bindings.

    .. attribute:: bindings_factories

        The list of bindings factories which when called will return a
        :class:`~sipbuild.Bindings` object.  There may or may not be a
        corresponding section in the :file:`pyproject.toml` file.

    .. attribute:: builder

        The :class:`~sipbuild.AbstractBuilder` implementation that the project
        uses to build buildables.

    .. attribute:: buildables

        The list of :class:`~sipbuild.Buildable` objects that the project will
        use the builder to build.

    .. method:: get_distinfo_dir(target_dir)

        Get the path name of the project's :file:`.dist-info` directory.

        :param str target_dir: is the name of the directory that should contain
            the :file:`.dist-info` directory.
        :return: the path name of the :file:`.dist-info` directory.

    .. method:: get_dunder_init()

        Called by the project to get the contents of of the top-level
        :file:`__init__.py` file to install.  The default implementation
        returns an empty string.

        :return: the contents of the :file:`__init__.py` file.

    .. method:: get_options()

        Called by the project to get the list of the project's options.  If it
        is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :class:`~sipbuild.Option` objects.

    .. method:: get_requires_dists()

        Get the list of any implicit ``requires-dist`` expressions that should
        be added to any explicit expressions specified in the
        ``[tool.sip.metadata]`` section of the :file:`pyproject.toml` file.

        :return: the list of ``requires-dist`` expressions.

    .. attribute:: installables

        The list of :class:`~sipbuild.Installable` objects that the project
        will use the builder to install.

    .. method:: open_for_writing(fname)
        :staticmethod:

        Open a text file for writing.  This is a wrapper around :c:func:`open`
        that handles common user errors.

        :param str fname: is the name of the file.
        :return: the open file object.

    .. method:: progress(message)

        A progress message is written to ``stdout`` if progress messages have
        not been disabled.  If the message does not end with ``.`` then ``...``
        is appended.

        :param str message: is the text of the message.

    .. method:: read_command_pipe(args, \*, and_stderr=False, fatal=True)

        Create a generator that will return each line of a command's
        ``stdout``.

        :param list[str] args: is the list of arguments that make up the
            command.
        :param bool and_stderr: is ``True`` if the output from ``stderr``
            should be included.
        :param bool fatal: is ``True`` if a :exc:`~sipbuild.UserException`
            should be raised if the command returns a non-zero exit code.
        :return: the generator.

    .. method:: run_command(args, \*, fatal=True)

        Run a command and display any output from ``stdout`` or ``stderr`` if
        verbose progress messages are enabled.

        :param list[str] args: is the list of arguments that make up the
            command.
        :param bool fatal: is ``True`` if a :exc:`~sipbuild.UserException`
            should be raised if the command returns a non-zero exit code.

    .. attribute:: root_dir

        The name of the directory containing the :file:`pyproject.toml` file.

    .. method:: update(tool)

        Called by the project to carry out any required updates to the project.
        The current directory will be the build directory.  The default
        implementation will call
        :meth:`~sipbuild.Project.update_buildable_bindings` if the tool is a
        build tool.

        :param str tool: is the name of the tool being used.

    .. method:: update_buildable_bindings()

        Update :attr:`~sipbuild.Project.bindings` to ensure all bindings are
        buildable or have been explicitly enabled.


:class:`~sipbuild.PyProject`
----------------------------

.. class:: PyProject

    An encapsulation of a parsed :file:`pyproject.toml` file.

    .. method:: get_metadata()

        Get an :class:`~collections.OrderedDict` containing the contents of the
        ``[tool.sip.metadata]`` section.  The ``name``, ``version``,
        ``metadata-version`` and ``requires-python`` keys will be defined.

        :return: the meta-data.

    .. method:: get_section(section_name: str, \*, required=False)

        Get a section as either an :class:`~collections.OrderedDict`, if the
        section is a table, or a ``list`` if the section is a list.

        :param str section_name: is the name of the section.
        :param bool required: is ``True`` if the section must be defined.
        :return: the section.


:exc:`~sipbuild.PyProjectOptionException`
-----------------------------------------

.. exception:: PyProjectOptionException(name, text, \*, section_name=None, detail=None)

    The exception raised to describe an error with a particular option (i.e.
    key/value) in a particular section of a :file:`pyproject.toml` file.

    :param str name: is the name of the option.
    :param str text: is the text describing the error.
    :param str section_name: is the name of the section, defaulting to
        ``[tool.sip.project]``.
    :param str detail: is additional detail about the error.


:exc:`~sipbuild.PyProjectUndefinedOptionException`
--------------------------------------------------

.. exception:: PyProjectUndefinedOptionException(name, \*, section_name=None)

    The exception raised to when a particular option (i.e.  key/value) in a
    particular section of a :file:`pyproject.toml` file has not been defined.

    :param str name: is the name of the option.
    :param str section_name: is the name of the section, defaulting to
        ``[tool.sip.project]``.


:exc:`~sipbuild.UserException`
------------------------------

.. exception:: UserException(text, \*, detail=None)

    The exception raised to describe an anticipated error to the user.

    :param str text: is the text describing the error.
    :param str detail: is additional detail about the error.
