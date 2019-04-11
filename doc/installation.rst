Installation
============

To install SIP from PyPI, run::

    pip install sip

SIP is also included with all of the major Linux distributions.  However, it
may be a version or two out of date.


.. _ref-configuration-files:

Configuring with Configuration Files
------------------------------------

The :program:`configure.py` script normally introspects the Python installation
of the interpreter running it in order to determine the names of the various
files and directories it needs.  This is fine for a native build of SIP but
isn't appropriate when cross-compiling.  In this case it is possible to supply
a configuration file, specified using the
:option:`--configuration <configure.py --configuration>` option, which contains
definitions of all the required values.

The format of a configuration file is as follows:

- a configuration item is a single line containing a name/value pair separated
  by ``=``

- a value may include another value by embedding the name of that value
  surrounded by ``%(`` and ``)``

- comments begin with ``#`` and continue to the end of the line

- blank lines are ignored.

:program:`configure.py` provides the following preset values for a
configuration:

``py_major``
    is the major version number of the target Python installation.

``py_minor``
    is the minor version number of the target Python installation.

``sysroot``
    is the name of the system root directory.  This is specified with the
    :option:`--sysroot <configure.py --sysroot>` option.

The following is an example configuration file::

    # The target Python installation.
    py_platform = linux
    py_inc_dir = %(sysroot)/usr/include/python%(py_major)%(py_minor)

    # Where SIP will be installed.
    sip_bin_dir = %(sysroot)/usr/bin
    sip_module_dir = %(sysroot)/usr/lib/python%(py_major)/dist-packages

The following values can be specified in the configuration file:

``py_platform``
    is the target Python platform.

``py_inc_dir``
    is the target Python include directory containing the ``Python.h`` file.

``py_conf_inc_dir``
    is the target Python include directory containing the ``pyconfig.h`` file.
    If this isn't specified then it defaults to the value of ``py_inc_dir``.

``py_pylib_dir``
    is the target Python library directory.

``sip_bin_dir``
    is the name of the target directory where the SIP code generator will be
    installed.  It can be overridden by the
    :option:`--bindir <configure.py -b>` option.

``sip_inc_dir``
    is the name of the target directory where the ``sip.h`` file will be
    installed.  If this isn't specified then it defaults to the value of
    ``py_inc_dir``.  It can be overridden by the
    :option:`--incdir <configure.py -e>` option.

``sip_module_dir``
    is the target directory where the :mod:`sip` module will be installed.  It
    can be overridden by the :option:`--destdir <configure.py -d>` option.

``sip_sip_dir``
    is the name of the target directory where generated ``.sip`` files will be
    installed by default.  It is only used when creating the :mod:`sipconfig`
    module.  It can be overridden by the :option:`--sipdir <configure.py -v>`
    option.
