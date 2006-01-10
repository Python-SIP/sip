# This module is intended to be used by the build/installation scripts of
# extension modules created with SIP.  It provides information about file
# locations, version numbers etc., and provides some classes and functions.
#
# @BS_LICENSE@


import sys
import os
import string
import types
import stat


# These are installation specific values created when SIP was configured.
# @SIP_CONFIGURATION@

# The stack of configuration dictionaries.
_config_stack = []


class Configuration(object):
    """The class that represents SIP configuration values.
    """
    def __init__(self, sub_cfg=None):
        """Initialise an instance of the class.

        sub_cfg is the list of sub-class configurations.  It should be None
        when called normally.
        """
        # Find the build macros in the closest imported module from where this
        # was originally defined.
        self._macros = None

        for cls in self.__class__.__mro__:
            if cls is object:
                continue

            mod = sys.modules[cls.__module__]

            if hasattr(mod, "_default_macros"):
                self._macros = mod._default_macros
                break

        if sub_cfg:
            cfg = sub_cfg
        else:
            cfg = []

        cfg.append(_pkg_config)

        global _config_stack
        _config_stack = cfg

    def __getattr__(self, name):
        """Allow configuration values and user options to be handled as
        instance variables.

        name is the name of the configuration value or user option.
        """
        for cfg in _config_stack:
            try:
                return cfg[name]
            except KeyError:
                pass

        raise AttributeError, "\"%s\" is not a valid configuration value or user option" % name

    def build_macros(self):
        """Return the dictionary of platform specific build macros.
        """
        return self._macros

    def set_build_macros(self, macros):
        """Set the dictionary of build macros to be use when generating
        Makefiles.

        macros is the dictionary of platform specific build macros.
        """
        self._macros = macros


class _UniqueList:
    """A limited list that ensures all its elements are unique.
    """
    def __init__(self, value=None):
        """Initialise the instance.

        value is the initial value of the list.
        """
        if value is None:
            self._list = []
        else:
            self._list = value

    def append(self, value):
        """Append a value to the list if it isn't already present.

        value is the value to append.
        """
        if value not in self._list:
            self._list.append(value)

    def extend(self, value):
        """Append each element of a value to a list if it isn't already
        present.

        value is the list of elements to append.
        """
        for el in value:
            self.append(el)

    def as_list(self):
        """Return the list as a raw list.
        """
        return self._list


class _Macro:
    """A macro that can be manipulated as a list.
    """
    def __init__(self, name, value):
        """Initialise the instance.

        name is the name of the macro.
        value is the initial value of the macro.
        """
        self._name = name
        self.set(value)

    def set(self, value):
        """Explicitly set the value of the macro.

        value is the new value.  It may be a string, a list of strings or a
        _UniqueList instance.
        """
        self._macro = []

        if isinstance(value, _UniqueList):
            value = value.as_list()

        if type(value) == types.ListType:
            self.extend(value)
        else:
            self.append(value)

    def append(self, value):
        """Append a value to the macro.

        value is the value to append.
        """
        if value:
            self._macro.append(value)

    def extend(self, value):
        """Append each element of a value to the macro.

        value is the list of elements to append.
        """
        for el in value:
            self.append(el)

    def as_list(self):
        """Return the macro as a list.
        """
        return self._macro


class Makefile:
    """The base class for the different types of Makefiles.
    """
    def __init__(self, configuration, console=0, qt=0, opengl=0, python=0,
                 threaded=0, warnings=1, debug=0, dir=None,
                 makefile="Makefile", installs=None):
        """Initialise an instance of the target.  All the macros are left
        unchanged allowing scripts to manipulate them at will.

        configuration is the current configuration.
        console is set if the target is a console (rather than windows) target.
        qt is set if the target uses Qt.  For Qt v4 a list of Qt libraries may
        be specified and a simple non-zero value implies QtCore and QtGui.
        opengl is set if the target uses OpenGL.
        python is set if the target #includes Python.h.
        debug is set to generated a debugging version of the target.
        threaded is set if the target requires thread support.  It is
        automatically set if the target uses Qt and Qt has thread support
        enabled.
        warnings is set if compiler warning messages are required.
        debug is set if debugging symbols should be generated.
        dir is the directory for build files and Makefiles.
        makefile is the name of the Makefile.
        installs is a list of extra install targets.  Each element is a two
        part list, the first of which is the source and the second is the
        destination.  If the source is another list then it is a set of source
        files and the destination is a directory.
        """
        if qt:
            if not hasattr(configuration, "qt_version"):
                error("The target uses Qt but pyqtconfig has not been imported.")

            # For Qt v4 interpret Qt support as meaning link against the core
            # and GUI libraries (which corresponds to the default qmake
            # configuration).  Also allow a list of Qt v4 modules to be
            # specified.
            if configuration.qt_version >= 0x040000:
                if type(qt) != types.ListType:
                    qt = ["QtCore", "QtGui"]

            self._threaded = configuration.qt_threaded
        else:
            self._threaded = threaded

        self.config = configuration
        self.console = console
        self._qt = qt
        self._opengl = opengl
        self._python = python
        self._warnings = warnings
        self._debug = debug
        self._dir = dir
        self._makefile = makefile
        self._installs = installs

        self._finalised = 0

        # Copy the macros and convert them all to instance lists.
        macros = configuration.build_macros()

        for m in macros.keys():
            # Allow the user to override the default.
            try:
                val = getattr(configuration, m)
            except AttributeError:
                val = macros[m]

            # These require special handling as they are (potentially) a set of
            # space separated values rather than a single value that might
            # contain spaces.
            if m == "DEFINES" or m[:6] in ("INCDIR", "LIBDIR"):
                val = string.split(val)

            # We also want to treat lists of libraries in the same way so that
            # duplicates get eliminated.
            if m[:4] == "LIBS":
                val = string.split(val)

            self.__dict__[m] = _Macro(m, val)

        # This is used to alter the configuration more significantly than can
        # be done with just configuration files.
        self.generator = self.optional_string("MAKEFILE_GENERATOR", "UNIX")

        # These are what configuration scripts normally only need to change.
        self.extra_cflags = []
        self.extra_cxxflags = []
        self.extra_defines = []
        self.extra_include_dirs = []
        self.extra_lflags = []
        self.extra_lib_dirs = []
        self.extra_libs = []

        # Get these once and make them available to sub-classes.
        if sys.platform == "win32":
            def_copy = "copy"
            def_rm = "del"
            def_mkdir = "mkdir"
            def_chk_dir_exists = "if not exist"
        else:
            def_copy = "cp -f"
            def_rm = "rm -f"
            def_mkdir = "mkdir -p"
            def_chk_dir_exists = "test -d"

        self.copy = self.optional_string("COPY", def_copy)
        self.rm = self.optional_string("DEL_FILE", def_rm)
        self.mkdir = self.optional_string("MKDIR", def_mkdir)
        self.chkdir = self.optional_string("CHK_DIR_EXISTS", def_chk_dir_exists)


    def finalise(self):
        """Finalise the macros by doing any consolidation that isn't specific
        to a Makefile.
        """
        # Extract the things we might need from the Windows Qt configuration.
        if self._qt:
            wcfg = string.split(self.config.qt_winconfig)
            win_shared = ("shared" in wcfg)
            win_exceptions = ("exceptions" in wcfg)
            win_rtti = ("rtti" in wcfg)
            win_stl = ("stl" in wcfg)
        else:
            win_shared = 1
            win_exceptions = 0
            win_rtti = 0
            win_stl = 0

        # Get what we are going to transform.
        cflags = _UniqueList()
        cflags.extend(self.extra_cflags)
        cflags.extend(self.optional_list("CFLAGS"))

        cxxflags = _UniqueList()
        cxxflags.extend(self.extra_cxxflags)
        cxxflags.extend(self.optional_list("CXXFLAGS"))

        defines = _UniqueList()
        defines.extend(self.extra_defines)
        defines.extend(self.optional_list("DEFINES"))

        incdir = _UniqueList(["."])
        incdir.extend(self.extra_include_dirs)
        incdir.extend(self.optional_list("INCDIR"))

        lflags = _UniqueList()
        lflags.extend(self.extra_lflags)
        lflags.extend(self.optional_list("LFLAGS"))

        libdir = _UniqueList()
        libdir.extend(self.extra_lib_dirs)
        libdir.extend(self.optional_list("LIBDIR"))

        # Don't use a unique list as libraries may need to be searched more
        # than once.  Also MacOS/X uses the form "-framework lib" so we don't
        # want to lose the multiple "-framework".
        libs = []

        for l in self.extra_libs:
            libs.append(self.platform_lib(l))

            if self._qt:
                libs.extend(self._dependent_libs(l))

        libs.extend(self.optional_list("LIBS"))

        rpaths = _UniqueList()

        for l in self.extra_lib_dirs:
            # Ignore relative directories.  This is really a hack to handle
            # SIP v3 inter-module linking.
            if os.path.dirname(l) not in ("", ".", ".."):
                rpaths.append(l)

        if self._python:
            incdir.append(self.config.py_inc_dir)

            if sys.platform == "win32":
                libdir.append(self.config.py_lib_dir)

                py_lib = "python%u%u" % ((self.config.py_version >> 16), ((self.config.py_version >> 8) & 0xff))

                # For Borland use the OMF version of the Python library if it
                # exists, otherwise assume that Python was built with Borland
                # and use the normal library.
                if self.generator == "BMAKE":
                    bpy_lib = py_lib + "_bcpp"
                    bpy_lib_path = os.path.join(self.config.py_lib_dir, self.platform_lib(bpy_lib))

                    if os.access(bpy_lib_path, os.F_OK):
                        py_lib = bpy_lib

                if self._debug:
                    py_lib = py_lib + "_d"

                    cflags.append("/D_DEBUG")
                    cxxflags.append("/D_DEBUG")

                libs.append(self.platform_lib(py_lib))

        if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
            if win_exceptions:
                cflags_exceptions = "CFLAGS_EXCEPTIONS_ON"
                cxxflags_exceptions = "CXXFLAGS_EXCEPTIONS_ON"
            else:
                cflags_exceptions = "CFLAGS_EXCEPTIONS_OFF"
                cxxflags_exceptions = "CXXFLAGS_EXCEPTIONS_OFF"

            cflags.extend(self.optional_list(cflags_exceptions))
            cxxflags.extend(self.optional_list(cxxflags_exceptions))

            if win_rtti:
                cflags_rtti = "CFLAGS_RTTI_ON"
                cxxflags_rtti = "CXXFLAGS_RTTI_ON"
            else:
                cflags_rtti = "CFLAGS_RTTI_OFF"
                cxxflags_rtti = "CXXFLAGS_RTTI_OFF"

            cflags.extend(self.optional_list(cflags_rtti))
            cxxflags.extend(self.optional_list(cxxflags_rtti))

            if win_stl:
                cflags_stl = "CFLAGS_STL_ON"
                cxxflags_stl = "CXXFLAGS_STL_ON"
            else:
                cflags_stl = "CFLAGS_STL_OFF"
                cxxflags_stl = "CXXFLAGS_STL_OFF"

            cflags.extend(self.optional_list(cflags_stl))
            cxxflags.extend(self.optional_list(cxxflags_stl))

        if self._debug:
            if win_shared:
                cflags_mt = "CFLAGS_MT_DLLDBG"
                cxxflags_mt = "CXXFLAGS_MT_DLLDBG"
            else:
                cflags_mt = "CFLAGS_MT_DBG"
                cxxflags_mt = "CXXFLAGS_MT_DBG"

            cflags_debug = "CFLAGS_DEBUG"
            cxxflags_debug = "CXXFLAGS_DEBUG"
            lflags_debug = "LFLAGS_DEBUG"
        else:
            if win_shared:
                cflags_mt = "CFLAGS_MT_DLL"
                cxxflags_mt = "CXXFLAGS_MT_DLL"
            else:
                cflags_mt = "CFLAGS_MT"
                cxxflags_mt = "CXXFLAGS_MT"

            cflags_debug = "CFLAGS_RELEASE"
            cxxflags_debug = "CXXFLAGS_RELEASE"
            lflags_debug = "LFLAGS_RELEASE"

        if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
            if self._threaded:
                cflags.extend(self.optional_list(cflags_mt))
                cxxflags.extend(self.optional_list(cxxflags_mt))

            if self.console:
                cflags.extend(self.optional_list("CFLAGS_CONSOLE"))
                cxxflags.extend(self.optional_list("CXXFLAGS_CONSOLE"))

        cflags.extend(self.optional_list(cflags_debug))
        cxxflags.extend(self.optional_list(cxxflags_debug))
        lflags.extend(self.optional_list(lflags_debug))

        if self._warnings:
            cflags_warn = "CFLAGS_WARN_ON"
            cxxflags_warn = "CXXFLAGS_WARN_ON"
        else:
            cflags_warn = "CFLAGS_WARN_OFF"
            cxxflags_warn = "CXXFLAGS_WARN_OFF"

        cflags.extend(self.optional_list(cflags_warn))
        cxxflags.extend(self.optional_list(cxxflags_warn))

        if self._threaded:
            cflags.extend(self.optional_list("CFLAGS_THREAD"))
            cxxflags.extend(self.optional_list("CXXFLAGS_THREAD"))
            lflags.extend(self.optional_list("LFLAGS_THREAD"))

        if self._qt:
            if self.generator != "UNIX" and win_shared:
                defines.append("QT_DLL")

            if not self._debug:
                defines.append("QT_NO_DEBUG")

            if self.config.qt_version >= 0x040000:
                # This is really just a help for PyQt's configure.py when it is
                # detecting what Qt modules are available.
                qtmods = self.config.pyqt_modules[:]
                qtmods.extend(self._qt)

                for mod in qtmods:
                    if mod == "QtCore":
                        defines.append("QT_CORE_LIB")
                    elif mod == "QtGui":
                        defines.append("QT_GUI_LIB")
                    elif mod == "QtNetwork":
                        defines.append("QT_NETWORK_LIB")
                    elif mod == "QtOpenGL":
                        defines.append("QT_OPENGL_LIB")
                    elif mod == "QtSql":
                        defines.append("QT_SQL_LIB")
                    elif mod == "QtXml":
                        defines.append("QT_XML_LIB")
            elif self._threaded:
                defines.append("QT_THREAD_SUPPORT")

            # This is unlikely to be missing, or to contain more than one
            # directory, but you never know.
            qtincdir = self.optional_list("INCDIR_QT")

            if qtincdir:
                incdir.extend(qtincdir)

                if self.config.qt_version >= 0x040000:
                    for mod in qtmods:
                        incdir.append(os.path.join(qtincdir[0], mod))

            specd = os.path.join(self.config.qt_dir, "mkspecs", "default")

            if not os.access(specd, os.F_OK):
                specd = os.path.join(self.config.qt_dir, "mkspecs", self.config.platform)

            incdir.append(specd)

            libdir_qt = self.optional_list("LIBDIR_QT")
            libdir.extend(libdir_qt)
            rpaths.extend(libdir_qt)

            if self.config.qt_version >= 0x040000:
                # For Windows: the macros that define the dependencies on
                # Windows libraries.
                wdepmap = {
                    "QtCore":       "LIBS_CORE",
                    "QtGui":        "LIBS_GUI",
                    "QtNetwork":    "LIBS_NETWORK",
                    "QtOpenGL":     "LIBS_OPENGL"
                }

                # For Windows: the dependencies between Qt libraries.
                qdepmap = {
                    "QtAssistant":  ("QtCore", "QtGui", "QtNetwork"),
                    "QtGui":        ("QtCore", ),
                    "QtNetwork":    ("QtCore", ),
                    "QtOpenGL":     ("QtCore", "QtGui"),
                    "QtSql":        ("QtCore", ),
                    "QtSvg":        ("QtCore", "QtGui", "QtXml"),
                    "QtXml":        ("QtCore", )
                }

                # The QtSql .prl file doesn't include QtGui as a dependency (at
                # least on Linux) so we explcitly set the dependency here for
                # everything.
                if "QtSql" in self._qt and "QtGui" not in self._qt:
                    self._qt.append("QtGui")

                for mod in self._qt:
                    lib = self._qt4_module_to_lib(mod)
                    libs.append(self.platform_lib(lib))

                    if sys.platform == "win32":
                        # On Windows the dependent libraries seem to be in
                        # qmake.conf rather than the .prl file and the
                        # inter-dependencies between Qt libraries don't seem to
                        # be anywhere.
                        deps = _UniqueList()

                        if mod in wdepmap.keys():
                            deps.extend(self.optional_list(wdepmap[mod]))

                        if mod in qdepmap.keys():
                            for qdep in qdepmap[mod]:
                                # Ignore the dependency if it is explicitly
                                # linked.
                                if qdep not in self._qt:
                                    libs.append(self.platform_lib(self._qt4_module_to_lib(qdep)))

                                    if qdep in wdepmap.keys():
                                        deps.extend(self.optional_list(wdepmap[qdep]))

                        libs.extend(deps.as_list())
                    else:
                        libs.extend(self._dependent_libs(lib))
            else:
                # Windows needs the version number appended if Qt is a DLL.
                qt_lib = self.config.qt_lib

                if self.generator in ("MSVC", "MSVC.NET", "BMAKE") and win_shared:
                    qt_lib = qt_lib + string.replace(version_to_string(self.config.qt_version), ".", "")

                    if self.config.qt_edition == "non-commercial":
                        qt_lib = qt_lib + "nc"

                libs.append(self.platform_lib(qt_lib))
                libs.extend(self._dependent_libs(self.config.qt_lib))

        if self._opengl:
            incdir.extend(self.optional_list("INCDIR_OPENGL"))
            lflags.extend(self.optional_list("LFLAGS_OPENGL"))
            libdir.extend(self.optional_list("LIBDIR_OPENGL"))
            libs.extend(self.optional_list("LIBS_OPENGL"))

        if self._qt or self._opengl:
            incdir.extend(self.optional_list("INCDIR_X11"))
            libdir.extend(self.optional_list("LIBDIR_X11"))
            libs.extend(self.optional_list("LIBS_X11"))

        if self._threaded:
            libs.extend(self.optional_list("LIBS_THREAD"))
            libs.extend(self.optional_list("LIBS_RTMT"))
        else:
            libs.extend(self.optional_list("LIBS_RT"))

        if self.console:
            libs.extend(self.optional_list("LIBS_CONSOLE"))

        libs.extend(self.optional_list("LIBS_WINDOWS"))

        lflags.extend(self._platform_rpaths(rpaths.as_list()))

        # Save the transformed values.
        self.CFLAGS.set(cflags)
        self.CXXFLAGS.set(cxxflags)
        self.DEFINES.set(defines)
        self.INCDIR.set(incdir)
        self.LFLAGS.set(lflags)
        self.LIBDIR.set(libdir)
        self.LIBS.set(libs)

        # Don't do it again because it has side effects.
        self._finalised = 1

    def _qt4_module_to_lib(self, mname):
        """Return the name of the Qt4 library corresponding to a module.

        mname is the name of the module.
        """
        if mname == "QtAssistant":
            lib = "QtAssistantClient"
        else:
            lib = mname

        if self._debug:
            if sys.platform == "win32":
                lib = lib + "d"
            else:
                lib = lib + "_debug"

        if sys.platform == "win32" and mname in ("QtCore", "QtGui", "QtNetwork", "QtOpenGL", "QtSql", "QtSvg", "QtXml"):
            lib = lib + "4"

        return lib

    def optional_list(self, name):
        """Return an optional Makefile macro as a list.

        name is the name of the macro.
        """
        return self.__dict__[name].as_list()

    def optional_string(self, name, default=""):
        """Return an optional Makefile macro as a string.

        name is the name of the macro.
        default is the default value
        """
        s = string.join(self.optional_list(name))

        if not s:
            s = default

        return s

    def required_string(self, name):
        """Return a required Makefile macro as a string.

        name is the name of the macro.
        """
        s = self.optional_string(name)

        if not s:
            raise ValueError, "\"%s\" must have a non-empty value" % name

        return s

    def _platform_rpaths(self, rpaths):
        """Return a list of platform specific rpath flags.

        rpaths is the cannonical list of rpaths.
        """
        flags = []
        prefix = self.optional_string("RPATH")

        if prefix:
            for r in rpaths:
                flags.append(_quote(prefix + r))

        return flags

    def platform_lib(self, clib):
        """Return a library name in platform specific form.

        clib is the library name in cannonical form.
        """
        if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
            plib = clib + ".lib"
        else:
            plib = "-l" + clib

        return plib

    def _dependent_libs(self, clib):
        """Return a list of additional libraries (in platform specific form)
        that must be linked with a library.

        clib is the library name in cannonical form.
        """
        prl_libs = []

        if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
            prl_name = os.path.join(self.config.qt_lib_dir, clib + ".prl")
        else:
            prl_name = os.path.join(self.config.qt_lib_dir, "lib" + clib + ".prl")

        if os.access(prl_name, os.F_OK):
            try:
                f = open(prl_name, "r")
            except IOError, detail:
                error("Unable to open \"%s\": %s" % (prl_name, detail))

            line = f.readline()
            while line:
                line = string.strip(line)
                if line and line[0] != "#":
                    eq = string.find(line, "=")
                    if eq > 0 and string.strip(line[:eq]) == "QMAKE_PRL_LIBS":
                        prl_libs = string.split(line[eq + 1:])
                        break

                line = f.readline()

            f.close()

        return prl_libs


    def parse_build_file(self, filename):
        """
        Parse a build file and return the corresponding dictionary.

        filename is the name of the build file.  If it is a dictionary instead
        then its contents are validated.
        """
        if type(filename) is types.DictType:
            bfname = "dictionary"
            dict = filename
        else:
            if self._dir:
                bfname = os.path.join(self._dir, filename)
            else:
                bfname = filename

            dict = {}

            try:
                f = open(bfname, "r")
            except IOError, detail:
                error("Unable to open \"%s\": %s" % (bfname, detail))

            line_nr = 1
            line = f.readline()

            while line:
                line = string.strip(line)

                if line and line[0] != "#":
                    eq = string.find(line, "=")

                    if eq <= 0:
                        error("\"%s\" line %d: Line must be in the form 'name = value value...'." % (bfname, line_nr))

                    dict[string.strip(line[:eq])] = string.strip(line[eq + 1:])

                line_nr = line_nr + 1
                line = f.readline()

            f.close()

        # Check the compulsory values.
        for i in ("target", "sources"):
            try:
                dict[i]
            except KeyError:
                error("\"%s\" is missing from \"%s\"." % (i, bfname))

        # Get the optional values.
        for i in ("headers", "moc_headers"):
            try:
                dict[i]
            except KeyError:
                dict[i] = ""

        # Generate the list of objects.
        if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
            ext = ".obj"
        else:
            ext = ".o"

        olist = []

        for f in string.split(dict["sources"]):
            root, discard = os.path.splitext(f)
            olist.append(root + ext)

        for f in string.split(dict["moc_headers"]):
            if not self._qt:
                error("\"%s\" defines \"moc_headers\" for a non-Qt module." % bfname)

            root, discard = os.path.splitext(f)
            olist.append("moc_" + root + ext)

        dict["objects"] = string.join(olist)

        return dict

    def clean_build_file_objects(self, mfile, build):
        """Generate the clean target.

        mfile is the file object.
        build is the dictionary created from the build file.
        """
        mfile.write("\t-%s $(TARGET)\n" % self.rm)

        for f in string.split(build["objects"]):
            mfile.write("\t-%s %s\n" % (self.rm, f))

        for f in string.split(build["moc_headers"]):
            root, discard = os.path.splitext(f)
            mfile.write("\t-%s moc_%s.cpp\n" % (self.rm, root))

    def ready(self):
        """The Makefile is now ready to be used.
        """
        if not self._finalised:
            self.finalise()

    def generate(self):
        """Generate the Makefile.
        """
        self.ready()

        if self._dir:
            mfname = os.path.join(self._dir, self._makefile)
        else:
            mfname = self._makefile

        try:
            mfile = open(mfname, "w")
        except IOError, detail:
            error("Unable to create \"%s\": %s" % (mfname, detail))

        self.generate_macros_and_rules(mfile)
        self.generate_target_default(mfile)
        self.generate_target_install(mfile)

        if self._installs:
            if type(self._installs) != types.ListType:
                self._installs = [self._installs]

            for src, dst in self._installs:
                self.install_file(mfile, src, dst)

        self.generate_target_clean(mfile)

        mfile.close()

    def generate_macros_and_rules(self, mfile):
        """The default implementation of the macros and rules generation.

        mfile is the file object.
        """
        mfile.write("CC = %s\n" % self.required_string("CC"))
        mfile.write("CXX = %s\n" % self.required_string("CXX"))
        mfile.write("LINK = %s\n" % self.required_string("LINK"))

        cppflags = []

        for f in self.optional_list("DEFINES"):
            cppflags.append("-D" + f)

        for f in self.optional_list("INCDIR"):
            cppflags.append("-I" + _quote(f))

        mfile.write("CPPFLAGS = %s\n" % string.join(cppflags))

        mfile.write("CFLAGS = %s\n" % self.optional_string("CFLAGS"))
        mfile.write("CXXFLAGS = %s\n" % self.optional_string("CXXFLAGS"))
        mfile.write("LFLAGS = %s\n" % self.optional_string("LFLAGS"))

        libs = []

        if self.generator in ("MSVC", "MSVC.NET"):
            libdir_prefix = "/LIBPATH:"
        else:
            libdir_prefix = "-L"

        for ld in self.optional_list("LIBDIR"):
            libs.append(libdir_prefix + _quote(ld))

        libs.extend(self.optional_list("LIBS"))

        mfile.write("LIBS = %s\n" % string.join(libs))

        if self._qt:
            mfile.write("MOC = %s\n" % _quote(self.required_string("MOC")))

        # These probably don't matter.
        if self.generator == "MINGW":
            mfile.write(".SUFFIXES: .cpp .cxx .cc .C .c\n\n")
        elif self.generator == "UNIX":
            mfile.write(".SUFFIXES: .c .o .cpp .cc .cxx .C\n\n")
        else:
            mfile.write(".SUFFIXES: .c .cpp .cc .cxx .C\n\n")

        if self.generator in ("MSVC", "MSVC.NET"):
            mfile.write("""
{.}.cpp{}.obj::
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
\t$<
<<

{.}.cc{}.obj::
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
\t$<
<<

{.}.cxx{}.obj::
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
\t$<
<<

{.}.C{}.obj::
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -Fo @<<
\t$<
<<

{.}.c{}.obj::
\t$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fo @<<
\t$<
<<
""")
        elif self.generator == "BMAKE":
            mfile.write("""
.cpp.obj:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o$@ $<

.cc.obj:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o$@ $<

.cxx.obj:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o$@ $<

.C.obj:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o$@ $<

.c.obj:
\t$(CC) -c $(CFLAGS) $(CPPFLAGS) -o$@ $<
""")
        else:
            mfile.write("""
.cpp.o:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cc.o:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cxx.o:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.C.o:
\t$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.c.o:
\t$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
""")

    def generate_target_default(self, mfile):
        """The default implementation of the default target.

        mfile is the file object.
        """
        mfile.write("\nall:\n")

    def generate_target_install(self, mfile):
        """The default implementation of the install target.

        mfile is the file object.
        """
        mfile.write("\ninstall:\n")

    def generate_target_clean(self, mfile):
        """The default implementation of the clean target.

        mfile is the file object.
        """
        mfile.write("\nclean:\n")

    def install_file(self, mfile, src, dst, strip=0):
        """Install one or more files in a directory.

        mfile is the file object.
        src is the name of a single file to install, or the list of a number of
        files to install.
        dst is the name of the destination directory.
        strip is set if the files should be stripped after been installed.
        """
        # Help package builders.
        if self.generator == "UNIX":
            dst = "$(DESTDIR)" + dst

        mfile.write("\t@%s %s " % (self.chkdir, _quote(dst)))

        if self.generator == "UNIX":
            mfile.write("|| ")

        mfile.write("%s %s\n" % (self.mkdir, _quote(dst)))

        if type(src) != types.ListType:
            src = [src]

        # Get the strip command if needed.
        if strip:
            strip_cmd = self.optional_string("STRIP")

            if not strip_cmd:
                strip = 0

        for sf in src:
            target = _quote(os.path.join(dst, os.path.basename(sf)))

            mfile.write("\t%s %s %s\n" % (self.copy, _quote(sf), target))

            if strip:
                mfile.write("\t%s %s\n" % (strip_cmd, target))


class ParentMakefile(Makefile):
    """The class that represents a parent Makefile.
    """
    def __init__(self, configuration, subdirs, dir=None, makefile="Makefile",
                 installs=None):
        """Initialise an instance of a parent Makefile.

        subdirs is the sequence of subdirectories.
        """
        Makefile.__init__(self, configuration, dir=dir, makefile=makefile, installs=installs)

        self._subdirs = subdirs

    def generate_macros_and_rules(self, mfile):
        """Generate the macros and rules.

        mfile is the file object.
        """
        # We don't want them.
        pass

    def generate_target_default(self, mfile):
        """Generate the default target.

        mfile is the file object.
        """
        self._subdir_target(mfile)

    def generate_target_install(self, mfile):
        """Generate the install target.

        mfile is the file object.
        """
        self._subdir_target(mfile, "install")

    def generate_target_clean(self, mfile):
        """Generate the clean target.

        mfile is the file object.
        """
        self._subdir_target(mfile, "clean")

    def _subdir_target(self, mfile, target="all"):
        """Create a target for a list of sub-directories.

        mfile is the file object.
        target is the name of the target.
        """
        if target == "all":
            tname = ""
        else:
            tname = " " + target

        mfile.write("\n" + target + ":\n")

        for d in self._subdirs:
            if self.generator == "MINGW":
                mfile.write("\t@$(MAKE) -C %s%s\n" % (d, tname))
            elif self.generator == "UNIX":
                mfile.write("\t@(cd %s; $(MAKE)%s)\n" % (d, tname))
            else:
                mfile.write("\tcd %s\n" % d)
                mfile.write("\t$(MAKE)%s\n" % tname)
                mfile.write("\t@cd ..\n")


class PythonModuleMakefile(Makefile):
    """The class that represents a Python module Makefile.
    """
    def __init__(self, configuration, srcdir, dstdir, dir=None,
                 makefile="Makefile", installs=None):
        """Initialise an instance of a parent Makefile.

        srcdir is the name of the directory (relative to the directory in which
        the Makefile will be created) containing the module's Python code.
        dstdir is the name of the directory where the module's Python code will
        be installed.
        """
        Makefile.__init__(self, configuration, dir=dir, makefile=makefile, installs=installs)

        if dir:
            self._moddir = os.path.join(dir, srcdir)
        else:
            self._modir = srcdir

        self._srcdir = srcdir
        self._dstdir = dstdir

    def generate_macros_and_rules(self, mfile):
        """Generate the macros and rules.

        mfile is the file object.
        """
        # We don't want them.
        pass

    def generate_target_install(self, mfile):
        """Generate the install target.

        mfile is the file object.
        """
        Makefile.generate_target_install(self, mfile)

        os.path.walk(self._moddir, self._visit, mfile)

    def _visit(self, mfile, dirname, names):
        """Install the files from a particular directory.

        mfile is the file object.
        dirname is the sub-directory.
        names is the list of files to install from the sub-directory.
        """
        tail = dirname[len(self._moddir):]

        flist = []
        for f in names:
            if os.path.isfile(os.path.join(dirname, f)):
                flist.append(os.path.join(self._srcdir + tail, f))

        self.install_file(mfile, flist, self._dstdir + tail)


class ModuleMakefile(Makefile):
    """The class that represents a Python extension module Makefile
    """
    def __init__(self, configuration, build_file, install_dir=None, static=0,
                 console=0, qt=0, opengl=0, threaded=0, warnings=1, debug=0,
                 dir=None, makefile="Makefile", installs=None, strip=1):
        """Initialise an instance of a module Makefile.

        build_file is the file containing the target specific information.  If
        it is a dictionary instead then its contents are validated.
        install_dir is the directory the target will be installed in.
        static is set if the module should be built as a static library.
        strip is set if the module should be stripped of unneeded symbols when
        installed.
        """
        Makefile.__init__(self, configuration, console, qt, opengl, 1, threaded, warnings, debug, dir, makefile, installs)

        self._build = self.parse_build_file(build_file)
        self._install_dir = install_dir
        self._dir = dir
        self.static = static

        # Don't strip if this is a debug or static build.
        if debug or static:
            self._strip = 0
        else:
            self._strip = strip

        # Save the target name for later.
        self._target = self._build["target"]

        if sys.platform != "win32" and static:
            self._target = "lib" + self._target

        if sys.platform == "win32" and debug:
            self._target = self._target + "_d"

        # See if we should only export the module initialisation function (to
        # reduce the size of the module and speed up module load times).
        if self.config.export_all or debug or static:
            self._export_init = 0
        else:
            self._export_init = 1

    def finalise(self):
        """Finalise the macros common to all module Makefiles.
        """
        if self.console:
            lflags_console = "LFLAGS_CONSOLE"
        else:
            lflags_console = "LFLAGS_WINDOWS"

        if self.static:
            self.DEFINES.append("SIP_STATIC_MODULE")
        else:
            self.CFLAGS.extend(self.optional_list("CFLAGS_SHLIB"))
            self.CXXFLAGS.extend(self.optional_list("CXXFLAGS_SHLIB"))

            lflags_dll = self.optional_list("LFLAGS_DLL")

            if lflags_dll:
                self.LFLAGS.extend(lflags_dll)
            elif self.console:
                lflags_console = "LFLAGS_CONSOLE_DLL"
            else:
                lflags_console = "LFLAGS_WINDOWS_DLL"

            lflags_plugin = self.optional_list("LFLAGS_PLUGIN")

            if not lflags_plugin:
                lflags_plugin = self.optional_list("LFLAGS_SHLIB")

            self.LFLAGS.extend(lflags_plugin)

        self.LFLAGS.extend(self.optional_list(lflags_console))

        if sys.platform == "darwin":
            self.LFLAGS.append("-framework Python")

        Makefile.finalise(self)

        if not self.static:
            if self.optional_string("AIX_SHLIB"):
                # AIX needs a lot of special handling.
                if self.required_string('LINK') == 'g++':
                    # g++ is used for linking.
                    # For SIP v4 and g++:
                    # 1.) Import the python symbols
                    aix_lflags = ['-Wl,-bI:%s/python.exp' % self.config.py_lib_dir]

                    if self._export_init:
                        aix_lflags.append('-Wl,-bnoexpall')
                        aix_lflags.append('-Wl,-bnoentry')
                        aix_lflags.append('-Wl,-bE:%s.exp' % self._target)
                else:
                    # IBM VisualAge C++ is used for linking.
                    # For SIP v4 and xlC:
                    # 1.) Create a shared object
                    # 2.) Import the python symbols
                    aix_lflags = ['-qmkshrobj',
                                  '-bI:%s/python.exp' % self.config.py_lib_dir]

                    if self._export_init:
                        aix_lflags.append('-bnoexpall')
                        aix_lflags.append('-bnoentry')
                        aix_lflags.append('-bE:%s.exp' % self._target)

                self.LFLAGS.extend(aix_lflags)
            else:
                if self._export_init:
                    if sys.platform[:5] == 'linux':
                        self.LFLAGS.extend(['-Wl,--version-script=%s.exp' % self._target])
                    elif sys.platform[:5] == 'hp-ux':
                        self.LFLAGS.extend(['-Wl,+e,init%s' % self._target])
                    elif sys.platform[:5] == 'irix' and self.required_string('LINK') != 'g++':
                        # Doesn't work when g++ is used for linking on IRIX.
                        self.LFLAGS.extend(['-Wl,-exported_symbol,init%s' % self._target])

                # Force the shared linker if there is one.
                link_shlib = self.optional_list("LINK_SHLIB")

                if link_shlib:
                    self.LINK.set(link_shlib)

    def module_as_lib(self, mname):
        """Return the name of a SIP v3.x module when it is used as a library.
        This will raise an exception when used with SIP v4.x modules.

        mname is the name of the module.
        """
        raise ValueError, "module_as_lib() can only be used with SIP v3.x"

    def generate_macros_and_rules(self, mfile):
        """Generate the macros and rules generation.

        mfile is the file object.
        """
        if self.static:
            if sys.platform == "win32":
                ext = "lib"
            else:
                ext = "a"
        else:
            if sys.platform == "win32":
                ext = "pyd"
            elif sys.platform == "darwin":
                ext = "so"
            else:
                ext = self.optional_string("EXTENSION_PLUGIN")
                if not ext:
                    ext = self.optional_string("EXTENSION_SHLIB", "so")

        mfile.write("TARGET = %s\n" % (self._target + "." + ext))
        mfile.write("OFILES = %s\n" % self._build["objects"])
        mfile.write("HFILES = %s %s\n" % (self._build["headers"], self._build["moc_headers"]))
        mfile.write("\n")

        if self.static:
            if self.generator in ("MSVC", "MSVC.NET", "BMAKE"):
                mfile.write("LIB = %s\n" % self.required_string("LIB"))
            else:
                mfile.write("AR = %s\n" % self.required_string("AR"))

                self._ranlib = self.optional_string("RANLIB")

                if self._ranlib:
                    mfile.write("RANLIB = %s\n" % self._ranlib)

        Makefile.generate_macros_and_rules(self, mfile)

    def generate_target_default(self, mfile):
        """Generate the default target.

        mfile is the file object.
        """
        mfile.write("\n$(TARGET): $(OFILES)\n")

        if self.generator in ("MSVC", "MSVC.NET"):
            if self.static:
                mfile.write("\t$(LIB) /OUT:$(TARGET) @<<\n")
                mfile.write("\t  $(OFILES)\n")
                mfile.write("<<\n")
            else:
                mfile.write("\t$(LINK) $(LFLAGS) /OUT:$(TARGET) @<<\n")
                mfile.write("\t  $(OFILES) $(LIBS)\n")
                mfile.write("<<\n")
        elif self.generator == "BMAKE":
            if self.static:
                mfile.write("\t-%s $(TARGET)\n" % (self.rm))
                mfile.write("\t$(LIB) $(TARGET) @&&|\n")

                for of in string.split(self._build["objects"]):
                    mfile.write("+%s \\\n" % (of))

                mfile.write("|\n")
            else:
                mfile.write("\t$(LINK) @&&|\n")
                mfile.write("\t$(LFLAGS) $(OFILES) ,$(TARGET),,$(LIBS),%s\n" % (self._target))
                mfile.write("|\n")

                # Create the .def file that renames the entry point.
                defname = self._target + ".def"

                if self._dir:
                    defname = os.path.join(self._dir, defname)

                try:
                    dfile = open(defname, "w")
                except IOError, detail:
                    error("Unable to create \"%s\": %s" % (defname, detail))

                dfile.write("EXPORTS\n")
                dfile.write("init%s=_init%s\n" % (self._target, self._target))

                dfile.close()

        else:
            if self.static:
                mfile.write("\t-%s $(TARGET)\n" % self.rm)
                mfile.write("\t$(AR) $(TARGET) $(OFILES)\n")

                if self._ranlib:
                    mfile.write("\t$(RANLIB) $(TARGET)\n")
            else:
                if self._export_init:
                    # Create an export file for AIX and Linux.
                    if sys.platform[:5] == 'linux':
                        mfile.write("\t@echo '{ global: init%s; local: *; };' > %s.exp\n" % (self._target, self._target))
                    elif sys.platform[:3] == 'aix':
                        mfile.write("\t@echo '#!' >%s.exp" % self._target)
                        mfile.write("; \\\n\t echo 'init%s' >>%s.exp\n" % (self._target, self._target))

                mfile.write("\t$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)\n")

        mfile.write("\n$(OFILES): $(HFILES)\n")

        for mf in string.split(self._build["moc_headers"]):
            root, discard = os.path.splitext(mf)
            cpp = "moc_" + root + ".cpp"

            mfile.write("\n%s: %s\n" % (cpp, mf))
            mfile.write("\t$(MOC) -o %s %s\n" % (cpp, mf))

    def generate_target_install(self, mfile):
        """Generate the install target.

        mfile is the file object.
        """
        if self._install_dir is None:
            self._install_dir = self.config.default_mod_dir

        mfile.write("\ninstall: $(TARGET)\n")
        self.install_file(mfile, "$(TARGET)", self._install_dir, self._strip)

    def generate_target_clean(self, mfile):
        """Generate the clean target.

        mfile is the file object.
        """
        mfile.write("\nclean:\n")
        self.clean_build_file_objects(mfile, self._build)

        # Remove any export file on AIX and Linux.
        if self._export_init and (sys.platform[:5] == 'linux' or sys.platform[:3] == 'aix'):
            mfile.write("\t-%s %s.exp\n" % (self.rm, self._target))


class SIPModuleMakefile(ModuleMakefile):
    """The class that represents a SIP generated module Makefile.
    """
    def finalise(self):
        """Finalise the macros for a SIP generated module Makefile.
        """
        self.INCDIR.append(self.config.sip_inc_dir)

        ModuleMakefile.finalise(self)


class ProgramMakefile(Makefile):
    """The class that represents a program Makefile.
    """
    def __init__(self, configuration, build_file=None, install_dir=None,
                 console=0, qt=0, opengl=0, python=0, threaded=0, warnings=1,
                 debug=0, dir=None, makefile="Makefile", installs=None):
        """Initialise an instance of a program Makefile.

        build_file is the file containing the target specific information.  If
        it is a dictionary instead then its contents are validated.
        install_dir is the directory the target will be installed in.
        """
        Makefile.__init__(self, configuration, console, qt, opengl, python, threaded, warnings, debug, dir, makefile, installs)

        self._install_dir = install_dir

        if build_file:
            self._build = self.parse_build_file(build_file)
        else:
            self._build = None

    def build_command(self, source):
        """Create a command line that will build an executable.  Returns a
        tuple of the name of the executable and the command line.

        source is the name of the source file.
        """
        self.ready()

        # The name of the executable.
        exe, ignore = os.path.splitext(source)

        if sys.platform == "win32":
            exe = exe + ".exe"

        # The command line.
        build = []

        build.append(self.required_string("CXX"))

        for f in self.optional_list("DEFINES"):
            build.append("-D" + f)

        for f in self.optional_list("INCDIR"):
            build.append("-I" + _quote(f))

        build.extend(self.optional_list("CXXFLAGS"))

        # Borland requires all flags to precede all file names.
        if self.generator != "BMAKE":
            build.append(source)

        if self.generator in ("MSVC", "MSVC.NET"):
            build.append("-Fe")
            build.append("/link")
            libdir_prefix = "/LIBPATH:"
        elif self.generator == "BMAKE":
            build.append("-e" + exe)
            libdir_prefix = "-L"
        else:
            build.append("-o")
            build.append(exe)
            libdir_prefix = "-L"

        for ld in self.optional_list("LIBDIR"):
            build.append(libdir_prefix + _quote(ld))

        lflags = self.optional_list("LFLAGS")

        # This is a huge hack demonstrating my lack of understanding of how the
        # Borland compiler works.
        if self.generator == "BMAKE":
            blflags = []

            for lf in lflags:
                for f in string.split(lf):
                    # Tell the compiler to pass the flags to the linker.
                    if f[-1] == "-":
                        f = "-l-" + f[1:-1]
                    elif f[0] == "-":
                        f = "-l" + f[1:]

                    # Remove any explicit object files otherwise the compiler
                    # will complain that they can't be found, but they don't
                    # seem to be needed.
                    if string.lower(f[-4:]) != ".obj":
                        blflags.append(f)

            lflags = blflags

        build.extend(lflags)

        build.extend(self.optional_list("LIBS"))

        if self.generator == "BMAKE":
            build.append(source)

        return (exe, string.join(build))

    def finalise(self):
        """Finalise the macros for a program Makefile.
        """
        if self.generator in ("MSVC", "MSVC.NET"):
            self.LFLAGS.append("/INCREMENTAL:NO")

        if self.console:
            lflags_console = "LFLAGS_CONSOLE"
        else:
            lflags_console = "LFLAGS_WINDOWS"

        self.LFLAGS.extend(self.optional_list(lflags_console))

        Makefile.finalise(self)

    def generate_macros_and_rules(self, mfile):
        """Generate the macros and rules generation.

        mfile is the file object.
        """
        if not self._build:
            raise ValueError, "pass a filename as build_file when generating a Makefile"

        target = self._build["target"]

        if sys.platform == "win32":
            target = target + ".exe"

        mfile.write("TARGET = %s\n" % target)
        mfile.write("OFILES = %s\n" % self._build["objects"])
        mfile.write("HFILES = %s\n" % self._build["headers"])
        mfile.write("\n")

        Makefile.generate_macros_and_rules(self, mfile)

    def generate_target_default(self, mfile):
        """Generate the default target.

        mfile is the file object.
        """
        mfile.write("\n$(TARGET): $(OFILES)\n")

        if self.generator in ("MSVC", "MSVC.NET"):
            mfile.write("\t$(LINK) $(LFLAGS) /OUT:$(TARGET) @<<\n")
            mfile.write("\t  $(OFILES) $(LIBS)\n")
            mfile.write("<<\n")
        elif self.generator == "BMAKE":
            mfile.write("\t$(LINK) @&&|\n")
            mfile.write("\t$(LFLAGS) $(OFILES) ,$(TARGET),,$(LIBS),,\n")
            mfile.write("|\n")
        else:
            mfile.write("\t$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)\n")

        mfile.write("\n$(OFILES): $(HFILES)\n")

        for mf in string.split(self._build["moc_headers"]):
            root, discard = os.path.splitext(mf)
            cpp = "moc_" + root + ".cpp"

            mfile.write("\n%s: %s\n" % (cpp, mf))
            mfile.write("\t$(MOC) -o %s %s\n" % (cpp, mf))

    def generate_target_install(self, mfile):
        """Generate the install target.

        mfile is the file object.
        """
        if self._install_dir is None:
            self._install_dir = self.config.default_bin_dir

        mfile.write("\ninstall: $(TARGET)\n")
        self.install_file(mfile, "$(TARGET)", self._install_dir)

    def generate_target_clean(self, mfile):
        """Generate the clean target.

        mfile is the file object.
        """
        mfile.write("\nclean:\n")
        self.clean_build_file_objects(mfile, self._build)


def _quote(s):
    """Return a string surrounded by double quotes it if contains a space.

    s is the string.
    """
    if string.find(s, " ") >= 0:
        s = '"' + s + '"'

    return s


def version_to_string(v):
    """Convert a 3 part version number encoded as a hexadecimal value to a
    string.
    """
    return "%u.%u.%u" % (((v >> 16) & 0xff), ((v >> 8) & 0xff), (v & 0xff))


def read_version(filename, description, numdefine=None, strdefine=None):
    """Read the version information for a package from a file.  The information
    is specified as #defines of a numeric (hexadecimal or decimal) value and/or
    a string value.

    filename is the name of the file.
    description is the descriptive name of the package.
    numdefine is the name of the #define of the numeric version.  It is ignored
    if it is None.
    strdefine is the name of the #define of the string version.  It is ignored
    if it is None.

    Returns a tuple of the version as a number and as a string.
    """
    need_num = numdefine is not None
    need_str = strdefine is not None

    vers = None
    versstr = None

    f = open(filename)
    l = f.readline()

    while l and (need_num or need_str):
        wl = string.split(l)
        if len(wl) >= 3 and wl[0] == "#define":
            if need_num and wl[1] == numdefine:
                v = wl[2]

                if v[0:2] == "0x":
                    vers = string.atoi(v,16)
                else:
                    dec = int(v)
                    maj = dec / 100
                    min = (dec % 100) / 10
                    bug = (dec % 10)
                    vers = (maj << 16) + (min << 8) + bug

                need_num = 0

            if need_str and wl[1] == strdefine:
                # Take account of embedded spaces.
                versstr = string.join(wl[2:])[1:-1]
                need_str = 0

        l = f.readline()

    f.close()

    if need_num or need_str:
        error("The %s version number could not be determined by parsing %s." % (description, filename))

    return (vers, versstr)


def create_content(dict, macros=None):
    """Convert a dictionary to a string (typically to use as the content to a
    call to create_config_module()).  Dictionary values that are strings are
    quoted.  Dictionary values that are lists are converted to quoted strings.

    dict is the dictionary.
    macros is the optional dictionary of platform specific build macros.
    """
    content = "_pkg_config = {\n"

    keys = dict.keys()
    keys.sort()

    # Format it nicely.
    width = 0

    for k in keys:
        klen = len(k)

        if width < klen:
            width = klen

    for k in keys:
        val = dict[k]
        vtype = type(val)

        if val is None:
            val = "None"
        elif vtype == types.ListType:
            val = "'" + string.join(val) + "'"
        elif vtype == types.StringType:
            val = "'" + val + "'"
        elif vtype == types.IntType:
            if string.find(k, "version") >= 0:
                # Assume it's a hexadecimal version number.  It doesn't matter
                # if it isn't, we are just trying to make it look pretty.
                val = "0x%06x" % val
            else:
                val = str(val)
        else:
            val = "'" + str(val) + "'"

        content = content + "    '" + k + "':" + (" " * (width - len(k) + 2)) + string.replace(val, "\\", "\\\\")

        if k != keys[-1]:
            content = content + ","

        content = content + "\n"

    content = content + "}\n\n"

    # Format the optional macros.
    content = content + "_default_macros = "

    if macros:
        content = content + "{\n"

        names = macros.keys()
        names.sort()

        width = 0
        for c in names:
            clen = len(c)
            if width < clen:
                width = clen

        for c in names:
            if c == names[-1]:
                sep = ""
            else:
                sep = ","

            k = "'" + c + "':"
            content = content + "    %-*s  '%s'%s\n" % (1 + width + 2, k, string.replace(macros[c], "\\", "\\\\"), sep)

        content = content + "}\n"
    else:
        content = content + "None\n"

    return content


def create_config_module(module, template, content, macros=None):
    """Create a configuration module by replacing "@" followed by
    "SIP_CONFIGURATION" followed by "@" in a template file with a content
    string.

    module is the name of the module file.
    template is the name of the template file.
    content is the content string.  If it is a dictionary it is first converted
    to a string using create_content().
    macros is an optional dictionary of platform specific build macros.  It is
    only used if create_content() is called to convert the content to a string.
    """
    if type(content) == types.DictType:
        content = create_content(content, macros)

    # Allow this file to used as a template.
    key = "@" + "SIP_CONFIGURATION" + "@"

    df = open(module, "w")
    sf = open(template, "r")

    line = sf.readline()
    while line:
        if string.find(line, key) >= 0:
            line = content

        df.write(line)

        line = sf.readline()


def version_to_sip_tag(version, tags, description):
    """Convert a version number to a SIP tag.

    version is the version number.  If it is negative then the latest version
    is assumed.  (This is typically useful if a snapshot is indicated by a
    negative version number.)
    tags is the dictionary of tags keyed by version number.  The tag used is
    the one with the smallest key (ie. earliest version) that is greater than
    the given version number.
    description is the descriptive name of the package used for error messages.

    Returns the corresponding tag.
    """
    tag = None

    vl = tags.keys()
    vl.sort()

    # For a snapshot use the latest tag.
    if version < 0:
        tag = tags[vl[-1]]
    else:
        for v in vl:
            if version < v:
                tag = tags[v]
                break

        if tag is None:
            error("Unsupported %s version: 0x%06x." % (description, version))

    return tag


def error(msg):
    """Display an error message and terminate.

    msg is the text of the error message.
    """
    sys.stderr.write(format("Error: " + msg) + "\n")
    sys.exit(1)

 
def inform(msg):
    """Display an information message.

    msg is the text of the error message.
    """
    sys.stdout.write(format(msg) + "\n")


def format(msg, leftmargin=0, rightmargin=78):
    """Format a message by inserting line breaks at appropriate places.

    msg is the text of the message.
    leftmargin is the position of the left margin.
    rightmargin is the position of the right margin.

    Return the formatted message.
    """
    curs = leftmargin
    fmsg = " " * leftmargin

    for w in string.split(msg):
        l = len(w)
        if curs != leftmargin and curs + l > rightmargin:
            fmsg = fmsg + "\n" + (" " * leftmargin)
            curs = leftmargin

        if curs > leftmargin:
            fmsg = fmsg + " "
            curs = curs + 1

        fmsg = fmsg + w
        curs = curs + l

    return fmsg


def parse_build_macros(filename, names, overrides=None, properties=None):
    """Parse a qmake compatible file of build system macros and convert it to a
    dictionary.  A macro is a name/value pair.  The dictionary is returned or
    None if any of the overrides was invalid.

    filename is the name of the file to parse.
    names is a list of the macro names to extract from the file.
    overrides is an optional list of macro names and values that modify those
    found in the file.  They are of the form "name=value" (in which case the
    value replaces the value found in the file) or "name+=value" (in which case
    the value is appended to the value found in the file).
    properties is an optional dictionary of property name and values that are
    used to resolve any expressions of the form "$[name]" in the file.
    """
    # Validate and convert the overrides to a dictionary.
    orides = {}

    if overrides is not None:
        for oride in overrides:
            prefix = ""
            name_end = string.find(oride, "+=")

            if name_end >= 0:
                prefix = "+"
                val_start = name_end + 2
            else:
                name_end = string.find(oride, "=")

                if name_end >= 0:
                    val_start = name_end + 1
                else:
                    return None

            name = oride[:name_end]

            if name not in names:
                return None

            orides[name] = prefix + oride[val_start:]

    try:
        f = open(filename, "r")
    except IOError, detail:
        error("Unable to open %s: %s" % (filename, detail))

    # Get everything into a dictionary.
    raw = {
        "DIR_SEPARATOR":        os.sep,
        "LITERAL_WHITESPACE":   " ",
        "LITERAL_DOLLAR":       "$",
        "LITERAL_HASH":         "#"
    }

    line = f.readline()
    while line:
        # Handle line continuations.
        while len(line) > 1 and line[-2] == "\\":
            line = line[:-2]

            next = f.readline()

            if next:
                line = line + next
            else:
                break

        line = string.strip(line)

        # Ignore comments.
        if line and line[0] != "#":
            assstart = string.find(line, "+")
            if assstart > 0 and line[assstart + 1] == '=':
                assend = assstart + 1
            else:
                assstart = string.find(line, "=")
                assend = assstart

            if assstart > 0:
                lhs = string.strip(line[:assstart])
                rhs = string.strip(line[assend + 1:])

                raw[lhs] = rhs

        line = f.readline()

    # Go through the raw dictionary extracting the macros we need and
    # resolving any macro expansions.  First of all, make sure every macro has
    # a value.
    refined = {}

    for m in names:
        refined[m] = ""

    macro_prefix = "QMAKE_"

    for lhs in raw.keys():
        # Strip any prefix.
        if string.find(lhs, macro_prefix) == 0:
            reflhs = lhs[len(macro_prefix):]
        else:
            reflhs = lhs

        # See if we are interested in this one.
        if reflhs not in names:
            continue

        rhs = raw[lhs]

        # Resolve any references.
        estart = string.find(rhs, "$$(")
        mstart = string.find(rhs, "$$")

        while mstart >= 0 and mstart != estart:
            rstart = mstart + 2
            if rstart < len(rhs) and rhs[rstart] == "{":
                rstart = rstart + 1
                term = "}"
            elif rstart < len(rhs) and rhs[rstart] == "[":
                rstart = rstart + 1
                term = "]"
            else:
                term = string.whitespace

            mend = rstart
            while mend < len(rhs) and rhs[mend] not in term:
                mend = mend + 1

            lhs = rhs[rstart:mend]

            if term in "}]":
                mend = mend + 1

            if term == "]":
                try:
                    value = properties[lhs]
                except KeyError:
                    error("%s: property '%s' is not defined." % (filename, lhs))
            else:
                try:
                    value = raw[lhs]
                except KeyError:
                    error("%s: macro '%s' is not defined." % (filename, lhs))

            rhs = rhs[:mstart] + value + rhs[mend:]
            estart = string.find(rhs, "$$(")
            mstart = string.find(rhs, "$$")

        # Expand any POSIX style environment variables.
        pleadin = ["$$(", "$("]

        for pl in pleadin:
            estart = string.find(rhs, pl)

            if estart >= 0:
                nstart = estart + len(pl)
                break
        else:
            estart = -1

        while estart >= 0:
            eend = string.find(rhs[nstart:], ")")

            if eend < 0:
                break

            eend = nstart + eend

            name = rhs[nstart:eend]

            try:
                env = os.environ[name]
            except KeyError:
                env = ""

            rhs = rhs[:estart] + env + rhs[eend + 1:]

            for pl in pleadin:
                estart = string.find(rhs, pl)

                if estart >= 0:
                    nstart = estart + len(pl)
                    break
            else:
                estart = -1

        # Expand any Windows style environment variables.
        estart = string.find(rhs, "%")

        while estart >= 0:
            eend = string.find(rhs[estart + 1:], "%")

            if eend < 0:
                break

            eend = estart + 1 + eend

            name = rhs[estart + 1:eend]

            try:
                env = os.environ[name]
            except KeyError:
                env = ""

            rhs = rhs[:estart] + env + rhs[eend + 1:]

            estart = string.find(rhs, "%")

        refined[reflhs] = rhs

    # Handle the user overrides.
    for lhs in orides.keys():
        rhs = refined[lhs]
        oride = orides[lhs]

        if string.find(oride, "+") == 0:
            if rhs:
                rhs = rhs + " " + oride[1:]
            else:
                rhs = oride[1:]
        else:
            rhs = oride

        refined[lhs] = rhs

    return refined


def create_wrapper(script, wrapper, gui=0):
    """Create a platform dependent executable wrapper around a Python script.

    script is the full pathname of the script.
    wrapper is the name of the wrapper file to create.
    gui is non-zero if a GUI enabled version of the interpreter should be used.

    Returns the platform specific name of the wrapper.
    """
    if sys.platform == "win32":
        wrapper = wrapper + ".bat"

    wf = open(wrapper, "w")

    if sys.platform == "win32":
        exe = sys.executable

        if gui:
            exe = exe[:-4] + "w.exe"

        wf.write("@\"%s\" \"%s\" %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9\n" % (exe, script))
    else:
        wf.write("exec %s %s ${1+\"$@\"}\n" % (sys.executable, script))

    wf.close()

    if sys.platform != "win32":
        sbuf = os.stat(wrapper)
        mode = sbuf.st_mode
        mode |= (stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

        os.chmod(wrapper, mode)

    return wrapper
