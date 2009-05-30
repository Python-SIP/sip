.. _ref-build-system:

The SIP Build System
====================

The purpose of the build system is to make it easy for you to write
configuration scripts in Python for your own bindings.  The build system takes
care of the details of particular combinations of platform and compiler.  It
supports over 50 different platform/compiler combinations.

The build system is implemented as a pure Python module called ``sipconfig``
that contains a number of classes and functions.  Using this module you can
write bespoke configuration scripts (e.g. PyQt's ``configure.py``) or use it
with other Python based build systems (e.g.
`Distutils <http://www.python.org/sigs/distutils-sig/distutils.html>`_ and
`SCons <http://www.scons.org>`_).

An important feature of SIP is the ability to generate bindings that are built
on top of existing bindings.  For example, both
`PyKDE <http://www.riverbankcomputing.com/software/pykde/>`_ and
`PyQwt <http://pyqwt.sourceforge.net/>`_ are built on top of PyQt but all three
packages are maintained by different developers.  To make this easier PyQt
includes its own configuration module, ``pyqtconfig``, that contains additional
classes intended to be used by the configuration scripts of bindings built on
top of PyQt.  The SIP build system includes facilities that do a lot of the
work of creating these additional configuration modules.


``sipconfig`` Functions
-----------------------

create_config_module(module, template, content, macros=None)
    This creates a configuration module (e.g. ``pyqtconfig``) from a template
    file and a string.

    ``module`` is the name of the configuration module file to create.

    ``template`` is the name of the template file.

    ``content`` is a string which replaces every occurence of the pattern
    ``@SIP_CONFIGURATION@`` in the template file.  The content string is
    usually created from a Python dictionary using
    ``sipconfig.create_content()``.  ``content`` may also be a dictionary, in
    which case ``sipconfig.create_content()`` is automatically called to
    convert it to a string.

    ``macros`` is an optional dictionary of platform specific build macros.  It
    is only used if ``sipconfig.create_content()`` is called automatically to
    convert a ``content`` dictionary to a string.

create_content(dict, macros=None)
    This converts a Python dictionary to a string that can be parsed by the
    Python interpreter and converted back to an equivalent dictionary.  It is
    typically used to generate the content string for
    ``sipconfig.create_config_module()``.

    ``dict`` is the Python dictionary to convert.

    ``macros`` is the optional dictionary of platform specific build macros.

    Returns the dictionary as a string.

create_wrapper(script, wrapper, gui=0)
    This creates a platform dependent executable wrapper around a Python
    script.

    ``script`` is the full pathname of the script.

    ``wrapper`` is the pathname of the wrapper to create.

    ``gui`` is non-zero if a GUI enabled version of the interpreter should be
    used on platforms that require it.

    Returns the platform specific name of the wrapper.

error(msg)
    This displays an error message on ``stderr`` and calls ``sys.exit()`` with
    a value of 1.

    ``msg`` is the text of the message and should not include any newline
    characters.

format(msg, leftmargin=0, rightmargin=78)
    This formats a message by inserting newline characters at appropriate
    places.

    ``msg`` is the text of the message and should not include any newline
    characters.

    ``leftmargin`` is the optional position of the left margin.

    ``rightmargin`` is the optional position of the right margin.

inform(msg)
    This displays an information message on ``stdout``.

    ``msg`` is the text of the message and should not include any newline
    characters.

parse_build_macros(filename, names, overrides=None, properties=None)
    This parses a qmake compatible file of build system macros and converts it
    to a dictionary.  A macro is a name/value pair.  The dictionary is returned
    or None if any of the overrides was invalid.

    ``filename`` is the name of the file to parse.

    ``names`` is a list of the macro names to extract from the file.

    ``overrides`` is an optional list of macro names and values that modify
    those found in the file.  They are of the form *name=value* (in which case
    the value replaces the value found in the file) or *name+=value* (in which
    case the value is appended to the value found in the file).

    ``properties`` is an optional dictionary of property name and values that
    are used to resolve any expressions of the form ``$[name]`` in the file.

read_version(filename, description, numdefine=None, strdefine=None)
    This extracts version information for a package from a file, usually a C or
    C++ header file.  The version information must each be specified as a
    ``#define`` of a numeric (hexadecimal or decimal) value and/or a string
    value.

    ``filename`` is the name of the file to read.

    ``description`` is a descriptive name of the package used in error
    messages.

    ``numdefine`` is the optional name of the ``#define`` of the version as a
    number.  If it is ``None`` then the numeric version is ignored.

    ``strdefine`` is the optional name of the ``#define`` of the version as a
    string.  If it is ``None`` then the string version is ignored.

    Returns a tuple of the numeric and string versions.  ``sipconfig.error()``
    is called if either were required but could not be found.

version_to_sip_tag(version, tags, description)
    This converts a version number to a SIP version tag.  SIP uses the
    `%Timeline`_ directive to define the chronology of the different versions
    of the C/C++ library being wrapped.  Typically it is not necessary to
    define a version tag for every version of the library, but only for those
    versions that affect the library's API as SIP sees it.

    ``version`` is the numeric version number of the C/C++ library being
    wrapped.  If it is negative then the latest version is assumed.  (This is
    typically useful if a snapshot is indicated by a negative version number.)

    ``tags`` is the dictionary of SIP version tags keyed by the corresponding
    C/C++ library version number.  The tag used is the one with the smallest
    key (i.e. earliest version) that is greater than ``version``.

    ``description`` is a descriptive name of the C/C++ library used in error
    messages.

    Returns the SIP version tag.  ``sipconfig.error()`` is called if the C/C++
    library version number did not correspond to a SIP version tag.

version_to_string(v)
    This converts a 3 part version number encoded as a hexadecimal value to a
    string.

    ``v`` is the version number.

    Returns a string.


``sipconfig`` Classes
---------------------

Configuration
    This class encapsulates configuration values that can be accessed as
    instance objects.  A sub-class may provide a dictionary of additional
    configuration values in its constructor the elements of which will have
    precedence over the super-class's values.

    The following configuration values are provided:

        default_bin_dir
            The name of the directory where executables should be installed by
            default.

        default_mod_dir
            The name of the directory where SIP generated modules should be
            installed by default.

        default_sip_dir
            The name of the base directory where the ``.sip`` files for SIP
            generated modules should be installed by default.  A sub-directory
            with the same name as the module should be created and its ``.sip``
            files should be installed in the sub-directory.  The ``.sip``
            files only need to be installed if you might want to build other
            bindings based on them.

        platform
            The name of the platform/compiler for which the build system has
            been configured for.

        py_conf_inc_dir
            The name of the directory containing the ``pyconfig.h`` header
            file.

        py_inc_dir
            The name of the directory containing the ``Python.h`` header file.

        py_lib_dir
            The name of the directory containing the Python interpreter
            library.

        py_version
            The Python version as a 3 part hexadecimal number (e.g. v2.3.3 is
            represented as ``0x020303``).

        sip_bin
            The full pathname of the SIP executable.

        sip_config_args
            The command line passed to ``configure.py`` when SIP was
            configured.

        sip_inc_dir
            The name of the directory containing the ``sip.h`` header file.

        sip_mod_dir
            The name of the directory containing the SIP module.

        sip_version
            The SIP version as a 3 part hexadecimal number (e.g. v4.0.0 is
            represented as ``0x040000``).

        sip_version_str
            The SIP version as a string.  For development snapshots it will
            start with ``snapshot-``.

        universal
            The name of the MacOS/X SDK used when creating universal binaries.

    __init__(self, sub_cfg=None)
        Initialise the instance.

        ``sub_cfg`` is an optional list of sub-class configurations.  It should
        only be used by the ``__init__()`` method of a sub-class to append its
        own dictionary of configuration values before passing the list to its
        super-class.

    build_macros(self)
        Return the dictionary of platform specific build macros.

    set_build_macros(self, macros)
        Set the dictionary of platform specific build macros to be use when
        generating Makefiles.  Normally there is no need to change the default
        macros.

Makefile
    This class encapsulates a Makefile.  It is intended to be sub-classed to
    generate Makefiles for particular purposes.  It handles all platform and
    compiler specific flags, but allows them to be adjusted to suit the
    requirements of a particular module or program.  These are defined using a
    number of macros which can be accessed as instance objects.

    The following instance objects are provided to help in fine tuning the
    generated Makefile:

        chkdir
            A string that will check for the existence of a directory.

        config
            A reference to the ``configuration`` argument that was passed to
            the constructor.

        console
            A reference to the ``console`` argument that was passed to the
            constructor.

        copy
            A string that will copy a file.

        extra_cflags
            A list of additional flags passed to the C compiler.

        extra_cxxflags
            A list of additional flags passed to the C++ compiler.

        extra_defines
            A list of additional macro names passed to the C/C++ preprocessor.

        extra_include_dirs
            A list of additional include directories passed to the C/C++
            preprocessor.

        extra_lflags
            A list of additional flags passed to the linker.

        extra_lib_dirs
            A list of additional library directories passed to the linker.

        extra_libs
            A list of additional libraries passed to the linker.  The names of
            the libraries must be in platform neutral form (i.e. without any
            platform specific prefixes, version numbers or extensions).

        generator
            A string that defines the platform specific style of Makefile.  The
            only supported values are ``UNIX`` and something else that is not
            ``UNIX``.

        mkdir
            A string that will create a directory.

        rm
            A string that will remove a file.

    __init__(self, configuration, console=0, qt=0, opengl=0, python=0, threaded=0, warnings=None, debug=0, dir=None, makefile="Makefile", installs=None, universal='')
        Initialise the instance.

        ``configuration`` is the current configuration and is an instance of
        the ``Configuration`` class or a sub-class.

        ``console`` is set if the target is a console (rather than GUI) target.
        This only affects Windows and is ignored on other platforms.

        ``qt`` is set if the target uses Qt.  For Qt v4 a list of Qt libraries
        may be specified and a simple non-zero value implies QtCore and QtGui.

        ``opengl`` is set if the target uses OpenGL.

        ``python`` is set if the target uses Python.h.

        ``threaded`` is set if the target requires thread support.  It is set
        automatically if the target uses Qt and Qt has thread support enabled.

        ``warnings`` is set if compiler warning messages should be enabled.
        The default of ``None`` means that warnings are enabled for SIP v4.x
        and disabled for SIP v3.x.

        ``debug`` is set if debugging symbols should be generated.

        ``dir`` is the name of the directory where build files are read from
        and Makefiles are written to.  The default of ``None`` means the
        current directory is used.

        ``makefile`` is the name of the generated Makefile.

        ``installs`` is a list of extra install targets.  Each element is a two
        part list, the first of which is the source and the second is the
        destination.  If the source is another list then it is a list of source
        files and the destination is a directory.

        ``universal`` is the name of the SDK if universal binaries are to be
        created under MacOS/X.

    clean_build_file_objects(self, mfile, build)
        This generates the Makefile commands that will remove any files
        generated during the build of the default target.

        ``mfile`` is the Python file object of the Makefile.

        ``build`` is the dictionary created from parsing the build file.

    finalise(self)
        This is called just before the Makefile is generated to ensure that it
        is fully configured.  It must be reimplemented by a sub-class.

    generate(self)
        This generates the Makefile.

    generate_macros_and_rules(self, mfile)
        This is the default implementation of the Makefile macros and rules
        generation.

        ``mfile`` is the Python file object of the Makefile.

    generate_target_clean(self, mfile)
        This is the default implementation of the Makefile clean target
        generation.

        ``mfile`` is the Python file object of the Makefile.

    generate_target_default(self, mfile)
        This is the default implementation of the Makefile default target
        generation.

        ``mfile`` is the Python file object of the Makefile.

    generate_target_install(self, mfile)
        This is the default implementation of the Makefile install target
        generation.

        ``mfile`` is the Python file object of the Makefile.

    install_file(self, mfile, src, dst, strip=0)
        This generates the Makefile commands to install one or more files to a
        directory.

        ``mfile`` is the Python file object of the Makefile.

        ``src`` is the name of a single file to install or a list of a number
        of files to install.

        ``dst`` is the name of the destination directory.

        ``strip`` is set if the files should be stripped of unneeded symbols
        after having been installed.

    optional_list(self, name)
        This returns an optional Makefile macro as a list.

        ``name`` is the name of the macro.

        Returns the macro as a list.

    optional_string(self, name, default="")
        This returns an optional Makefile macro as a string.

        ``name`` is the name of the macro.

        ``default`` is the optional default value of the macro.

        Returns the macro as a string.

    parse_build_file(self, filename)
        This parses a build file (created with the ``-b`` SIP command line
        option) and converts it to a dictionary.  It can also validate an
        existing dictionary created through other means.

        ``filename`` is the name of the build file, or is a dictionary to be
        validated.  A valid dictionary will contain the name of the target to
        build (excluding any platform specific extension) keyed by ``target``;
        the names of all source files keyed by ``sources``; and, optionally,
        the names of all header files keyed by ``headers``.

        Returns a dictionary corresponding to the parsed build file.

    platform_lib(self, clib, framework=0)
        This converts a library name to a platform specific form.

        ``clib`` is the name of the library in cannonical form.

        ``framework`` is set if the library is implemented as a MacOS
        framework.

        Return the platform specific name.

    ready(self)
        This is called to ensure that the Makefile is fully configured.  It is
        normally called automatically when needed.

    required_string(self, name)
        This returns a required Makefile macro as a string.

        ``name`` is the name of the macro.

        Returns the macro as a string.  An exception is raised if the macro
        does not exist or has an empty value.

ModuleMakefile(Makefile)
    This class encapsulates a Makefile to build a generic Python extension
    module.

    __init__(self, configuration, build_file, install_dir=None, static=0, console=0, opengl=0, threaded=0, warnings=None, debug=0, dir=None, makefile="Makefile", installs=None, strip=1, export_all=0, universal='')
        Initialise the instance.

        ``configuration`` - see ``sipconfig.Makefile.__init__()``.

        ``build_file`` is the name of the build file.  Build files are
        generated using the ``-b`` SIP command line option.

        ``install_dir`` is the name of the directory where the module will be
        optionally installed.

        ``static`` is set if the module should be built as a static library
        (see `Builtin Modules and Custom Interpreters`_).

        ``console`` - see ``sipconfig.Makefile.__init__()``.

        ``qt`` - see ``sipconfig.Makefile.__init__()``.

        ``opengl`` - see ``sipconfig.Makefile.__init__()``.

        ``threaded`` - see ``sipconfig.Makefile.__init__()``.

        ``warnings`` - see ``sipconfig.Makefile.__init__()``.

        ``debug`` - see ``sipconfig.Makefile.__init__()``.

        ``dir`` - see ``sipconfig.Makefile.__init__()``.

        ``makefile`` - see ``sipconfig.Makefile.__init__()``.

        ``installs`` - see ``sipconfig.Makefile.__init__()``.

        ``strip`` is set if the module should be stripped of unneeded symbols
        after installation.  It is ignored if either ``debug`` or ``static`` is
        set, or if the platform doesn't support it.

        ``export_all`` is set if all of the module's symbols should be exported
        rather than just the module's initialisation function.  Exporting all
        symbols increases the size of the module and slows down module load
        times but may avoid problems with modules that use C++ exceptions.  All
        symbols are exported if either ``debug`` or ``static`` is set, or if
        the platform doesn't support it.

    finalise(self)
        This is a reimplementation of ``sipconfig.Makefile.finalise()``.

    generate_macros_and_rules(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_macros_and_rules()``.

    generate_target_clean(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_clean()``.

    generate_target_default(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_default()``.

    generate_target_install(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_install()``.

    module_as_lib(self, mname)
        This returns the name of a SIP v3.x module for when it is used as a
        library to be linked against.  An exception will be raised if it is
        used with SIP v4.x modules.

        ``mname`` is the name of the module.

        Returns the corresponding library name.

ParentMakefile(Makefile)
    This class encapsulates a Makefile that sits above a number of other
    Makefiles in sub-directories.

    __init__(self, configuration, subdirs, dir=None, makefile="Makefile", installs=None)
        Initialise the instance.

        ``configuration`` - see ``sipconfig.Makefile.__init__()``.

        ``subdirs`` is the sequence of sub-directories.

        ``dir`` - see ``sipconfig.Makefile.__init__()``.

        ``makefile`` - see ``sipconfig.Makefile.__init__()``.

        ``installs`` - see ``sipconfig.Makefile.__init__()``.

    generate_macros_and_rules(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_macros_and_rules()``.

    generate_target_clean(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_clean()``.

    generate_target_default(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_default()``.

    generate_target_install(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_install()``.

ProgramMakefile(Makefile)
    This class encapsulates a Makefile to build an executable program.

    __init__(self, configuration, build_file=None, install_dir=None, console=0, qt=0, opengl=0, python=0, threaded=0, warnings=None, debug=0, dir=None, makefile="Makefile", installs=None, universal='')
        Initialise the instance.

        ``configuration`` - see ``sipconfig.Makefile.__init__()``.

        ``build_file`` is the name of the optional build file.  Build files are
        generated using the ``-b`` SIP command line option.

        ``install_dir`` is the name of the directory where the executable
        program will be optionally installed.

        ``console`` - see ``sipconfig.Makefile.__init__()``.

        ``qt`` - see ``sipconfig.Makefile.__init__()``.

        ``opengl`` - see ``sipconfig.Makefile.__init__()``.

        ``python`` - see ``sipconfig.Makefile.__init__()``.

        ``threaded`` - see ``sipconfig.Makefile.__init__()``.

        ``warnings`` - see ``sipconfig.Makefile.__init__()``.

        ``debug`` - see ``sipconfig.Makefile.__init__()``.

        ``dir`` - see ``sipconfig.Makefile.__init__()``.

        ``makefile`` - see ``sipconfig.Makefile.__init__()``.

        ``installs`` - see ``sipconfig.Makefile.__init__()``.

    build_command(self, source)
        This creates a single command line that will create an executable
        program from a single source file.

        ``source`` is the name of the source file.

        Returns a tuple of the name of the executable that will be created and
        the command line.

    finalise(self)
        This is a reimplementation of ``sipconfig.Makefile.finalise()``.

    generate_macros_and_rules(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_macros_and_rules()``.

    generate_target_clean(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_clean()``.

    generate_target_default(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_default()``.

    generate_target_install(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_install()``.

PythonModuleMakefile(Makefile)
    This class encapsulates a Makefile that installs a pure Python module.

    __init__(self, configuration, dstdir, srcdir=None, dir=None, makefile="Makefile", installs=None)
        Initialise the instance.

        ``configuration`` - see ``sipconfig.Makefile.__init__()``.

        ``dstdir`` is the name of the directory in which the module's Python
        code will be installed.

        ``srcdir`` is the name of the directory (relative to ``dir``)
        containing the module's Python code.  It defaults to the same
        directory.

        ``dir`` - see ``sipconfig.Makefile.__init__()``.

        ``makefile`` - see ``sipconfig.Makefile.__init__()``.

        ``installs`` - see ``sipconfig.Makefile.__init__()``.

    generate_macros_and_rules(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_macros_and_rules()``.

    generate_target_install(self, mfile)
        This is a reimplementation of
        ``sipconfig.Makefile.generate_target_install()``.

SIPModuleMakefile(ModuleMakefile)
    This class encapsulates a Makefile to build a SIP generated Python
    extension module.

    finalise(self)
        This is a reimplementation of ``sipconfig.Makefile.finalise()``.
