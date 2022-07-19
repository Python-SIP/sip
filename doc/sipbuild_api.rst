.. py:module:: sipbuild
    :synopsis: The SIP build system.


:py:mod:`sipbuild` Module Reference
===================================

The :py:mod:`sipbuild` module implements the SIP build system.  The module may
be used by project-specific :program:`project.py` scripts or extended by other
build systems.

.. note::
    Most of the classes described here have additional methods and attributes.
    Unless they are documented here they are **not** part of the public API and
    may be changed in any future release.

.. py:data:: SIP_VERSION
    :type: int

    The major.minor.patch version number encoded as an integer.  For example
    v6.5.0 would be encoded as 0x060500.

.. py:data:: SIP_VERSION_STR
    :type: str

    The version number as it should be displayed to the user.


:py:class:`~sipbuild.AbstractBuilder`
-------------------------------------

.. py:class:: AbstractBuilder(project, **kwargs)

    An abstract class that defines the API of a builder.

    :param Project project: is the project.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the builder.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.

    .. py:method:: build()
        :abstractmethod:

        Build the project but do not install it.

    .. py:method:: build_sdist(sdist_directory)
        :abstractmethod:

        Build an sdist for the project.

        :param str sdist_directory: is the name of the directory in which the
            sdist is created.
        :return: the name of the sdist file (excluding any path).

    .. py:method:: build_wheel(wheel_directory)
        :abstractmethod:

        Build a wheel for the project.

        :param str wheel_directory: is the name of the directory in which the
            wheel is created.
        :return: the name of the wheel file (excluding any path).

    .. py:method:: install()
        :abstractmethod:

        Build and install the project.

    .. py:attribute:: project

        The :py:class:`~sipbuild.Project` object.


:py:class:`~sipbuild.AbstractProject`
-------------------------------------

.. py:class:: AbstractProject

    An abstract class that defines the API of a project.

    .. py:method:: build()
        :abstractmethod:

        Build the project but do not install it.

    .. py:method:: build_sdist(sdist_directory)
        :abstractmethod:

        Build an sdist for the project.

        :param str sdist_directory: is the name of the directory in which the
            sdist is created.
        :return: the name of the sdist file (excluding any path).

    .. py:method:: build_wheel(wheel_directory)
        :abstractmethod:

        Build a wheel for the project.

        :param str wheel_directory: is the name of the directory in which the
            wheel is created.
        :return: the name of the wheel file (excluding any path).

    .. py:method:: import_callable(name, base_type)
        :staticmethod:

        Import a callable from a script or module.  The callable is identified
        either by its name (if specified) or its type.

        :param str name: is a script (with a :file:`.py` extension), a module
            or the name of an object in a module (specified as
            ``module:name``).
        :param type base_type: is the type of the callable and is ignored if
            the name of the callable is explicitly specified.
        :return: the callable.

    .. py:method:: install()
        :abstractmethod:

        Build and install the project.

    .. py:method:: setup(pyproject, tool, tool_description)
        :abstractmethod:

        Setup the project from the :file:`pyproject.toml` file.

        :param PyProject pyproject: is the parsed :file:`pyproject.toml` file.
        :param str tool: is the name of the tool, either ``'build'``,
            ``'install'``, ``'sdist'`` or ``'wheel'``.
        :param str tool_description: is a short description of the tool.


:py:class:`~sipbuild.Bindings`
------------------------------

.. py:class:: Bindings(project, name, **kwargs)

    The encapsulation of a set of bindings.

    :param Project project: is the project.
    :param str name: is the name of the bindings.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the bindings.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.

    .. py:method:: apply_nonuser_defaults(tool)

        Called by the bindings to set the default values of any non-user
        options (i.e. those that cannot be set from a tool command line).  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :param str tool: is the name of the tool being used.

    .. py:method:: apply_user_defaults(tool)

        Called by the bindings to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. py:method:: generate()

        Called by the project to generate the source code of the bindings and
        return a :py:class:`~sipbuild.BuildableBindings` object containing the
        details needed by the builder to build the bindings.

        :return: the :py:class:`~sipbuild.BuildableBindings` object.

    .. py:method:: get_options()

        Called by the bindings to get the list of the bindings's options.  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :py:class:`~sipbuild.Option` objects.

    .. py:method:: is_buildable()

        Called by the builder to determine if the bindings are buildable.  This
        will not be called if the bindings have been explicitly enabled.  The
        default implementation returns ``True``.

        :return: ``True`` if the bindings are buildable.

    .. py:attribute:: project

        The :py:class:`~sipbuild.Project` object.


:py:class:`~sipbuild.Buildable`
-------------------------------

.. py:class:: Buildable(project, name)

    Encapsulate a generic buildable.

    :param Project project: is the project.
    :param str name: is the name of the buildable.

    .. py:attribute:: build_dir

        The name of the buildable-specific build directory.  This will be
        created automatically.

    .. py:attribute:: build_settings

        A list of values that are passed to the builder. It is up to the
        builder to determine how these values are used.

    .. py:attribute:: installables

        The list of :py:class:`~sipbuild.Installable` objects created by the
        builder to describe what was built.

    .. py:attribute:: name

        The name of the buildable.

    .. py:attribute:: project

        The :py:class:`~sipbuild.Project` object.


:py:class:`~sipbuild.BuildableBindings`
---------------------------------------

.. py:class:: BuildableBindings(bindings, fq_name, *, uses_limited_api=False)

    A :py:class:`~sipbuild.BuildableModule` sub-class that encapsulates the
    Python extension module for a set of bindings.

    :param Bindings bindings: is the bindings.
    :param str fq_name: is the fully qualified name of the bindings module.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. py:attribute:: bindings

        The :py:class:`~sipbuild.Bindings` object.


:py:class:`~sipbuild.BuildableExecutable`
-----------------------------------------

.. py:class:: BuildableExecutable(project, name, target, *, uses_limited_api=False)

    A :py:class:`~sipbuild.BuildableFromSources` sub-class that encapsulates an
    executable.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str target: is the platform-independent name of the executable being
        built.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.


:py:class:`~sipbuild.BuildableFromSources`
------------------------------------------

.. py:class:: BuildableFromSources(project, name, target, *, uses_limited_api=False)

    A :py:class:`~sipbuild.Buildable` sub-class that encapsulates a target that
    is built from source code.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str target: is the name of the target being built.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. py:attribute:: debug

        ``True`` if a build with debugging symbols should be performed.

    .. py:attribute:: define_macros

        The list of ``#define`` names and values in the form ``"NAME"`` or
        ``"NAME=VALUE"``.

    .. py:attribute:: headers

        The list of :file:`.h` header files.

    .. py:attribute:: include_dirs

        The list of directories that will be searched, in additional to the
        standard system directores, for :file:`.h` header files.

    .. py:attribute:: libraries

        The list of libraries to link the source code with.

    .. py:attribute:: library_dirs

        The list of directories that will be searched, in addition to the
        standard system directories, for any libraries.

    .. py:method:: make_names_relative()

        Make all the file names relative to the build directory.  This isn't
        necessary but can make any build files easier to read by the user.

    .. py:attribute:: sources

        The list of source files.

    .. py:attribute:: target

        The name of the target being built.

    .. py:attribute:: uses_limited_api

        ``True`` if the source code uses only the limited Python API.


:py:class:`~sipbuild.BuildableModule`
-------------------------------------

.. py:class:: BuildableModule(project, name, fq_name, *, uses_limited_api=False)

    A :py:class:`~sipbuild.BuildableFromSources` sub-class that encapsulates a
    Python extension module.

    :param Project project: is the project.
    :param str name: is the name of the buildable.
    :param str fq_name: is the fully qualified name of the module.
    :param bool uses_limited_api: is ``True`` if the source code uses only the
        limited Python API.

    .. py:attribute:: exceptions

        ``True`` if the module should be built with support for C++ exceptions.

    .. py:attribute:: fq_name

        The fully qualified name of the module.

    .. py:method:: get_install_subdir()

        Get the name of the sub-directory (relative to any future target
        installation directory) that the module should be installed in.

        :return: the name of the sub-directory.

    .. py:method:: get_module_extension()

        Get the platform-specific file name extension that a module should
        have.

        :return: the extension.

    .. py:attribute:: static

        ``True`` if the module should be built as a static library.


:py:class:`~sipbuild.Builder`
-----------------------------

.. py:class:: Builder(project, **kwargs)

    The default base implementation of a builder.

    :param Project project: is the project
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the builder.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.

    .. py:method:: apply_nonuser_defaults(tool)

        Called by the builder to set the default values of any non-user options
        (i.e. those that cannot be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. py:method:: apply_user_defaults(tool)

        Called by the builder to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. py:method:: build_executable(buildable, *, fatal=True)
        :abstractmethod:

        Build an executable from a buildable.

        :param BuildableExecutable buildable: is the buildable.
        :param bool fatal: is ``True`` if a :py:exc:`~sipbuild.UserException`
            should be raised if the build failed.
        :return: the relative path name of the built executable.

    .. py:method:: build_project(target_dir, *, wheel_tag=None)
        :abstractmethod:

        Build the project either to be installed for use or to create a wheel.

        :param str target_dir: is the directory in which the project will be
            installed in.
        :param str wheel_tag: is the wheel tag if a wheel is being created.

    .. py:method:: get_options()

        Called by the builder to get the list of the builder's options.  If
        it is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :py:class:`~sipbuild.Option` objects.

    .. py:method:: install_project(target_dir, *, wheel_tag=None)
        :abstractmethod:

        Install a built project either for use or to create a wheel.

        :param str target_dir: is the directory in which the project will be
            installed in.
        :param str wheel_tag: is the wheel tag if a wheel is being created.


:py:class:`~sipbuild.DistutilsBuilder`
--------------------------------------

.. py:class:: DistutilsBuilder(project, **kwargs)

    A :py:class:`~sipbuild.Builder` that uses the Python :py:mod:`distutils`
    package to perform builds.  This is the default builder for Python v3.9 and
    earlier.

    :param Project project: is the project.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the builder.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.


:py:func:`~sipbuild.handle_exception`
-------------------------------------

.. py:function:: handle_exception(e)

    Handle an exception by displaying an appropriate error message to
    ``stdout``.  The process is then terminated with a non-zero exit code.

    :param exception e: is the exception to be handled.


:py:class:`~sipbuild.Installable`
---------------------------------

.. py:class:: Installable(name, *, target_subdir=None)

    Encapsulate a list of files that will be installed in the same directory.

    :param str name: is the name of the installable.
    :param str target_subdir: is the relative path name of a sub-directory in
        which the installable's files will be installed.  If it is an absolute
        path name then it is used as the eventual full target directory.

    .. py:attribute:: files

        The list of file names to be installed.

    .. py:method:: get_full_target_dir(target_dir)

        Get the full path name of the directory where the installable's file
        will be installed.

        :param str target_dir: is the name of target directory.
        :return: the full path name of the sub-directory within the target
            directory where the files will be installed.

    .. py:method:: install(target_dir, installed, *, do_install=True)

        Install the installable's files in a target directory.

        :param str target_dir: is the name of the target directory.
        :param list[str] installed: is a list of installed files which is
            updated with the newly installed files.  The list is always updated
            even if the files are not actually installed.
        :param bool do_install: is ``True`` if the files are actually to be
            installed.

    .. py:attribute:: name

        The name of the installable.

    .. py:attribute:: target_subdir

        The name of the target sub-directory.


:py:class:`~sipbuild.Option`
----------------------------

.. py:class:: Option(name, *, option_type=str, choices=None, default=None, help=None, metavar=None, inverted=False, tools=None)

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
        tools is used, i.e. ``['build', 'install', 'wheel']``.


:py:class:`~sipbuild.Project`
-----------------------------

.. py:class:: Project(**kwargs)

    The default implementation of a project.  It has an associated builder
    which it uses to build a set of buildables.  Building a buildable may
    create one or more installables.

    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the project.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.

    .. py:method:: apply_nonuser_defaults(tool)

        Called by the project to set the default values of any non-user options
        (i.e. those that cannot be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. py:method:: apply_user_defaults(tool)

        Called by the project to set the default values of any user options
        (i.e. those that can be set from a tool command line).  If it is
        re-implemented in a sub-class then the super-class version should be
        called.

        :param str tool: is the name of the tool being used.

    .. py:attribute:: bindings

        The :py:class:`~collections.OrderedDict` of
        :py:class:`~sipbuild.Bindings` objects keyed by the name of the
        bindings.

    .. py:attribute:: bindings_factories

        The list of bindings factories which when called will return a
        :py:class:`~sipbuild.Bindings` object.  There may or may not be a
        corresponding section in the :file:`pyproject.toml` file.

    .. py:attribute:: builder

        The :py:class:`~sipbuild.AbstractBuilder` implementation that the
        project uses to build buildables.

    .. py:attribute:: buildables

        The list of :py:class:`~sipbuild.Buildable` objects that the project
        will use the builder to build.

    .. py:method:: get_distinfo_dir(target_dir)

        Get the path name of the project's :file:`.dist-info` directory.

        :param str target_dir: is the name of the directory that should contain
            the :file:`.dist-info` directory.
        :return: the path name of the :file:`.dist-info` directory.

    .. py:method:: get_dunder_init()

        Called by the project to get the contents of of the top-level
        :file:`__init__.py` file to install.  The default implementation
        returns an empty string.

        :return: the contents of the :file:`__init__.py` file.

    .. py:method:: get_metadata_overrides()

        Called by the project to get a mapping of `PEP 566
        <https://www.python.org/dev/peps/pep-0566/>`__ metadata names and
        values that will override any corresponding values defined in the
        pyproject.toml file.  A typical use is to determine a project's version
        dynamically.

        :return: the mapping.

    .. py:method:: get_options()

        Called by the project to get the list of the project's options.  If it
        is re-implemented in a sub-class then the super-class version should
        be called.

        :return: the list of :py:class:`~sipbuild.Option` objects.

    .. py:method:: get_platform_tag()

        Get the platform tag to use in a wheel name.  This default
        implementation uses the platform name and applies PEP defined
        conventions depending on OS version and GLIBC version as appropriate.

        :return: the platform tag.

    .. py:method:: get_requires_dists()

        Get the list of any implicit ``requires-dist`` expressions that should
        be added to any explicit expressions specified in the
        ``[tool.sip.metadata]`` section of the :file:`pyproject.toml` file.

        :return: the list of ``requires-dist`` expressions.

    .. py:method:: get_sip_distinfo_command_line(sip_distinfo, inventory, generator=None, wheel_tag=None, generator_version=None)

        Get a sequence of command line arguments to invoke
        :program:`sip-distinfo`.  The :option:`sip-distinfo --console-script`,
        :option:`sip-distinfo --gui-script`, :option:`sip-distinfo --metadata`,
        :option:`sip-distinfo --prefix`, :option:`sip-distinfo --project-root`
        and :option:`sip-distinfo --requires-dist` command line options are
        handled automatically.  The arguments do not contain the name of the
        :file:`.dist-info` directory to create.

        :param str sip_distinfo: is the name of the :program:`sip-distinfo`
            executable.
        :param str inventory: is the value of the
            :option:`sip-distinfo --inventory` command line option.
        :param str generator: is the value of the
            :option:`sip-distinfo --generator` command line option.
        :param str wheel_tag: is the value of the
            :option:`sip-distinfo --wheel-tag` command line option.
        :param str generator_version: is the value of the
            :option:`sip-distinfo --generator-version` command line option.
        :return: the sequence of command line arguments.

    .. py:attribute:: installables

        The list of :py:class:`~sipbuild.Installable` objects that the project
        will use the builder to install.

    .. py:method:: open_for_writing(fname)
        :staticmethod:

        Open a text file for writing.  This is a wrapper around :c:func:`open`
        that handles common user errors.

        :param str fname: is the name of the file.
        :return: the open file object.

    .. py:method:: progress(message)

        A progress message is written to ``stdout`` if progress messages have
        not been disabled.  If the message does not end with ``.`` then ``...``
        is appended.

        :param str message: is the text of the message.

    .. py:method:: project_path(path, relative_to=None)

        A file or directory path, possibly using POSIX separators and possibly
        relative to another directory is converted to an absolute path with
        native separators.

        :param str path: is the path.
        :param str relative_to: is the absolute path of a directory that the
            supplied path is relative to.  The default is the project
            directory.
        :return: the converted path.

    .. py:method:: read_command_pipe(args, *, and_stderr=False, fatal=True)

        Create a generator that will return each line of a command's
        ``stdout``.

        :param list[str] args: is the list of arguments that make up the
            command.
        :param bool and_stderr: is ``True`` if the output from ``stderr``
            should be included.
        :param bool fatal: is ``True`` if a :py:exc:`~sipbuild.UserException`
            should be raised if the command returns a non-zero exit code.
        :return: the generator.

    .. py:method:: run_command(args, *, fatal=True)

        Run a command and display any output from ``stdout`` or ``stderr`` if
        verbose progress messages are enabled.

        :param list[str] args: is the list of arguments that make up the
            command.
        :param bool fatal: is ``True`` if a :py:exc:`~sipbuild.UserException`
            should be raised if the command returns a non-zero exit code.

    .. py:attribute:: root_dir

        The name of the directory containing the :file:`pyproject.toml` file.

    .. py:method:: update(tool)

        Called by the project to carry out any required updates to the project.
        The current directory will be the build directory.  The default
        implementation will call
        :meth:`~sipbuild.Project.update_buildable_bindings` if the tool is a
        build tool.

        :param str tool: is the name of the tool being used.

    .. py:method:: update_buildable_bindings()

        Update :py:attr:`~sipbuild.Project.bindings` to ensure all bindings are
        buildable or have been explicitly enabled.


:py:class:`~sipbuild.PyProject`
-------------------------------

.. py:class:: PyProject

    An encapsulation of a parsed :file:`pyproject.toml` file.

    .. py:method:: get_metadata()

        Get an :py:class:`~collections.OrderedDict` containing the contents of
        the ``[tool.sip.metadata]`` section.  The ``name``, ``version``,
        ``metadata-version`` and ``requires-python`` keys will be defined.

        :return: the meta-data.

    .. py:method:: get_section(section_name: str, *, required=False)

        Get a section as either an :py:class:`~collections.OrderedDict`, if the
        section is a table, or a ``list`` if the section is a list.

        :param str section_name: is the name of the section.
        :param bool required: is ``True`` if the section must be defined.
        :return: the section.


:py:exc:`~sipbuild.PyProjectOptionException`
--------------------------------------------

.. py:exception:: PyProjectOptionException(name, text, *, section_name=None, detail=None)

    The exception raised to describe an error with a particular option (i.e.
    key/value) in a particular section of a :file:`pyproject.toml` file.

    :param str name: is the name of the option.
    :param str text: is the text describing the error.
    :param str section_name: is the name of the section, defaulting to
        ``[tool.sip.project]``.
    :param str detail: is additional detail about the error.


:py:exc:`~sipbuild.PyProjectUndefinedOptionException`
-----------------------------------------------------

.. py:exception:: PyProjectUndefinedOptionException(name, *, section_name=None)

    The exception raised to when a particular option (i.e.  key/value) in a
    particular section of a :file:`pyproject.toml` file has not been defined.

    :param str name: is the name of the option.
    :param str section_name: is the name of the section, defaulting to
        ``[tool.sip.project]``.


:py:class:`~sipbuild.SetuptoolsBuilder`
---------------------------------------

.. py:class:: SetuptoolsBuilder(project, **kwargs)

    A :py:class:`~sipbuild.Builder` that uses the Python :py:mod:`setuptools`
    package to perform builds.  This is the default builder for Python v3.10
    and later.

    :param Project project: is the project.
    :param \*\*kwargs: are keyword arguments that define the initial values of
        any corresponding :py:class:`~sipbuild.Option` defined by the builder.
        An :py:class:`~sipbuild.Option` value set in this way cannot be
        overridden in the :file:`pyproject.toml` file or by using a tool
        command line option.


:py:exc:`~sipbuild.UserException`
---------------------------------

.. py:exception:: UserException(text, *, detail=None)

    The exception raised to describe an anticipated error to the user.

    :param str text: is the text describing the error.
    :param str detail: is additional detail about the error.
