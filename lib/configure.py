# This script handles the SIP configuration and generates the Makefiles.
#
# @BS_LICENSE@


import sys
import os
import string
import glob
import getopt

import siputils


# Initialise the globals.
sip_version = 0x@RM_HEXVERSION@
sip_version_str = "@RM_LATEST@"
py_version = sys.hexversion >> 8
plat_py_site_dir = None
plat_py_inc_dir = None
plat_py_lib_dir = None
plat_sip_dir = None
plat_bin_dir = None
platform_specs = []
default_platform = None

# Command line options.
opt_platform = None
opt_sipbindir = None
opt_sipmoddir = None
opt_sipincdir = None
opt_sipsipdir = None
opt_static = 0
opt_debug = 0
opt_export_all = 0

# The names of build macros extracted from the platform specific configuration
# files.
build_macro_names = [
    "DEFINES",
    "CC",
    "CFLAGS",
    "CFLAGS_RELEASE", "CFLAGS_DEBUG",
    "CFLAGS_CONSOLE", "CFLAGS_SHLIB", "CFLAGS_THREAD",
    "CFLAGS_MT", "CFLAGS_MT_DBG", "CFLAGS_MT_DLL", "CFLAGS_MT_DLLDBG",
    "CFLAGS_EXCEPTIONS_ON", "CFLAGS_EXCEPTIONS_OFF",
    "CFLAGS_RTTI_ON", "CFLAGS_RTTI_OFF",
    "CFLAGS_STL_ON", "CFLAGS_STL_OFF",
    "CFLAGS_WARN_ON", "CFLAGS_WARN_OFF",
    "CHK_DIR_EXISTS", "COPY",
    "CXX",
    "CXXFLAGS",
    "CXXFLAGS_RELEASE", "CXXFLAGS_DEBUG",
    "CXXFLAGS_CONSOLE", "CXXFLAGS_SHLIB", "CXXFLAGS_THREAD",
    "CXXFLAGS_MT", "CXXFLAGS_MT_DBG", "CXXFLAGS_MT_DLL", "CXXFLAGS_MT_DLLDBG",
    "CXXFLAGS_EXCEPTIONS_ON", "CXXFLAGS_EXCEPTIONS_OFF",
    "CXXFLAGS_RTTI_ON", "CXXFLAGS_RTTI_OFF",
    "CXXFLAGS_STL_ON", "CXXFLAGS_STL_OFF",
    "CXXFLAGS_WARN_ON", "CXXFLAGS_WARN_OFF",
    "DEL_FILE",
    "EXTENSION_SHLIB", "EXTENSION_PLUGIN",
    "INCDIR", "INCDIR_X11", "INCDIR_OPENGL",
    "LINK", "LINK_SHLIB", "AIX_SHLIB", "LINK_SHLIB_CMD",
    "LFLAGS", "LFLAGS_CONSOLE", "LFLAGS_CONSOLE_DLL", "LFLAGS_DEBUG",
    "LFLAGS_PLUGIN", "LFLAGS_RELEASE", "LFLAGS_SHLIB", "LFLAGS_SONAME",
    "LFLAGS_THREAD", "LFLAGS_WINDOWS", "LFLAGS_WINDOWS_DLL", "LFLAGS_OPENGL",
    "LIBDIR", "LIBDIR_X11", "LIBDIR_OPENGL",
    "LIBS", "LIBS_CONSOLE", "LIBS_OPENGL", "LIBS_OPENGL", "LIBS_RT",
    "LIBS_RTMT", "LIBS_THREAD", "LIBS_WINDOWS", "LIBS_X11",
    "MAKEFILE_GENERATOR",
    "MKDIR",
    "RPATH",
    "AR", "RANLIB", "LIB", "STRIP"
]


def usage(rcode = 2):
    """Display a usage message and exit.

    rcode is the return code passed back to the calling process.
    """
    print "Usage:"
    print "  python configure.py [-h] [-b dir] [-d dir] [-e dir] [-k] [-p plat] [-u] [-v dir] option=value option+=value ..."
    print "where:"
    print "  -h       display this help message"
    print "  -b dir   where the SIP code generator will be installed [default %s]" % opt_sipbindir
    print "  -d dir   where the SIP module will be installed [default %s]" % opt_sipmoddir
    print "  -e dir   where the SIP header file will be installed [default %s]" % opt_sipincdir
    print "  -k       build the SIP module as a static library"
    print "  -p plat  the platform/compiler configuration [default %s]" % default_platform
    print "  -u       build with debugging symbols"
    print "  -v dir   where .sip files are normally installed [default %s]" % opt_sipsipdir

    # Display the different platform/compilers.
    print
    print "The following platform/compiler configurations are supported:"
    print

    platform_specs.sort()

    ss = ""
    sep = ""
    for s in platform_specs:
        ss = ss + sep + s
        sep = ", "

    print siputils.format(ss, 2)

    # Display the different build macros.
    print
    print "The following options may be used to adjust the compiler configuration:"
    print

    build_macro_names.sort()

    ss = ""
    sep = ""
    for s in build_macro_names:
        ss = ss + sep + s
        sep = " "

    print siputils.format(ss, 2)

    sys.exit(rcode)


def set_defaults():
    """Set up the defaults for values that can be set on the command line.
    """
    global default_platform
    global opt_sipbindir, opt_sipmoddir, opt_sipincdir, opt_sipsipdir

    # Set the platform specific default specification.
    platdefaults = {
        "aix":          "aix-xlc",
        "bsd":          "bsdi-g++",
        "cygwin":       "cygwin-g++",
        "darwin":       "macx-g++",
        "dgux":         "dgux-g++",
        "freebsd":      "freebsd-g++",
        "gnu":          "hurd-g++",
        "hp-ux":        "hpux-acc",
        "irix":         "irix-cc",
        "linux":        "linux-g++",
        "lynxos":       "lynxos-g++",
        "netbsd":       "netbsd-g++",
        "openbsd":      "openbsd-g++",
        "openunix":     "unixware-cc",
        "osf1":         "tru64-cxx",
        "qnx":          "qnx-g++",
        "reliantunix":  "reliant-cds",
        "sco_sv":       "sco-cc",
        "sinix":        "reliant-cds",
        "sunos5":       "solaris-cc",
        "ultrix":       "ultrix-g++",
        "unix_sv":      "unixware-g++",
        "unixware":     "unixware-cc",
        "win32":        "win32-msvc"
    }

    default_platform = "none"

    for pd in platdefaults.keys():
        if sys.platform[:len(pd)] == pd:
            default_platform = platdefaults[pd]
            break

    opt_sipbindir = plat_bin_dir
    opt_sipmoddir = plat_py_site_dir
    opt_sipincdir = plat_py_inc_dir
    opt_sipsipdir = plat_sip_dir


def inform_user():
    """Tell the user the option values that are going to be used.
    """
    siputils.inform("The SIP code generator will be installed in %s." % opt_sipbindir)
    siputils.inform("The SIP module will be installed in %s." % opt_sipmoddir)
    siputils.inform("The SIP header file will be installed in %s." % opt_sipincdir)
    siputils.inform("The default directory to install .sip files in is %s." % opt_sipsipdir)
    siputils.inform("The platform/compiler configuration is %s." % opt_platform)


def set_platform_directories():
    """Initialise the global variables relating to platform specific
    directories.
    """
    global plat_py_site_dir, plat_py_inc_dir, plat_bin_dir, plat_sip_dir
    global plat_py_lib_dir

    if sys.platform == "win32":
        plat_py_site_dir = sys.prefix + "\\Lib"
        if py_version >= 0x020200:
            plat_py_site_dir = plat_py_site_dir + "\\site-packages"

        plat_py_inc_dir = sys.prefix + "\\include"
        plat_py_lib_dir = sys.prefix + "\\libs"
        plat_bin_dir = sys.exec_prefix
        plat_sip_dir = sys.prefix + "\\sip"
    else:
        vers = "%d.%d" % ((py_version >> 16) & 0xff, (py_version >> 8) & 0xff)

        plat_py_site_dir = sys.prefix + "/lib/python" + vers
        if py_version >= 0x020000:
            plat_py_site_dir = plat_py_site_dir + "/site-packages"

        plat_py_inc_dir = sys.prefix + "/include/python" + vers
        plat_py_lib_dir = sys.prefix + "/lib/python" + vers + "/config"
        plat_bin_dir = sys.exec_prefix + "/bin"
        plat_sip_dir = sys.prefix + "/share/sip"


def create_config(module, template, macros):
    """Create the SIP configuration module so that it can be imported by build
    scripts.

    module is the module file name.
    template is the template file name.
    macros is the dictionary of build macros.
    """
    siputils.inform("Creating %s..." % module)

    content = {
        "sip_version":      sip_version,
        "sip_version_str":  sip_version_str,
        "sip_bin":          os.path.join(opt_sipbindir, "sip"),
        "sip_inc_dir":      opt_sipincdir,
        "sip_mod_dir":      opt_sipmoddir,
        "default_bin_dir":  plat_bin_dir,
        "default_mod_dir":  plat_py_site_dir,
        "default_sip_dir":  opt_sipsipdir,
        "export_all":       opt_export_all,
        "py_version":       py_version,
        "py_inc_dir":       plat_py_inc_dir,
        "py_lib_dir":       plat_py_lib_dir
    }

    siputils.create_config_module(module, template, content, macros)


def create_makefiles(macros):
    """Create the Makefiles.

    macros is the dictionary of platform specific build macros.
    """
    # Bootstrap.
    import sipconfig

    cfg = sipconfig.Configuration()

    cfg.set_build_macros(macros)

    sipconfig.inform("Creating top level Makefile...")

    sipconfig.ParentMakefile(
        configuration=cfg,
        subdirs=["sipgen", "siplib"],
        installs=(["sipconfig.py", "sipdistutils.py"], cfg.sip_mod_dir)
    ).generate()

    sipconfig.inform("Creating sip code generator Makefile...")

    sipconfig.ProgramMakefile(
        configuration=cfg,
        build_file="sipgen.sbf",
        dir="sipgen",
        install_dir=os.path.dirname(cfg.sip_bin),
        console=1,
        warnings=0
    ).generate()

    sipconfig.inform("Creating sip module Makefile...")

    makefile = sipconfig.ModuleMakefile(
        configuration=cfg,
        build_file="siplib.sbf",
        dir="siplib",
        install_dir=cfg.sip_mod_dir,
        installs=(["sip.h"], cfg.sip_inc_dir),
        warnings=0,
        static=opt_static,
        debug=opt_debug
    )

    makefile.generate()


def main(argv):
    """Create the configuration module module.

    argv is the list of command line arguments.
    """
    siputils.inform("This is SIP %s for Python %s on %s." % (sip_version_str, string.split(sys.version)[0], sys.platform))

    if py_version < 0x020300:
        siputils.error("This version of SIP requires Python v2.3 or later.")

    # Basic initialisation.
    set_platform_directories()

    # Build up the list of valid specs.
    for s in os.listdir("specs"):
        platform_specs.append(s)

    # Handle the command line.
    set_defaults()

    try:
        optlist, args = getopt.getopt(argv[1:], "hab:d:e:kp:uv:")
    except getopt.GetoptError:
        usage()

    global opt_sipbindir, opt_sipmoddir, opt_sipincdir, opt_sipsipdir
    global opt_platform, opt_static, opt_debug, opt_export_all

    for opt, arg in optlist:
        if opt == "-h":
            usage(0)
        elif opt == "-a":
            opt_export_all = 1
        elif opt == "-b":
            opt_sipbindir = os.path.abspath(arg)
        elif opt == "-d":
            opt_sipmoddir = os.path.abspath(arg)
        elif opt == "-e":
            opt_sipincdir = os.path.abspath(arg)
        elif opt == "-k":
            opt_static = 1
        elif opt == "-p":
            if arg not in platform_specs:
                usage()
            
            opt_platform = arg
        elif opt == "-u":
            opt_debug = 1
        elif opt == "-v":
            opt_sipsipdir = os.path.abspath(arg)

    if opt_platform is None:
        opt_platform = default_platform

    # Get the platform specific macros for building.
    macros = siputils.parse_build_macros(os.path.join("specs", opt_platform), build_macro_names, args)

    if macros is None:
        usage()

    # Tell the user what's been found.
    inform_user()

    # Install the configuration module.
    create_config("sipconfig.py", "siputils.py", macros)

    # Create the Makefiles.
    create_makefiles(macros)


###############################################################################
# The script starts here.
###############################################################################

if __name__ == "__main__":
    try:
        main(sys.argv)
    except SystemExit:
        raise
    except:
        print \
"""An internal error occured.  Please report all the output from the program,
including the following traceback, to support@riverbankcomputing.co.uk.
"""
        raise
