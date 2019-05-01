The :program:`sip5-module` Command Line
=======================================

The syntax of the :program:`sip5-module` command line is::

    sip5-module [options] name

``name`` is the fully qualified name of the ``sip`` module (i.e. including the
package name).

The full set of command line options is:

.. program:: sip5-module

.. cmdoption:: -h

    Display a help message.

.. cmdoption:: -V

    Display the SIP version number.

.. cmdoption:: --documentation-dir <DIR>

    The directory in which a copy of the :file:`sip.rst` file is placed.

.. cmdoption:: --include-dir <DIR>

    The directory in which a copy of the :file:`sip.h` file is placed.

.. cmdoption:: --module-dir <DIR>

    The directory in which a copy of the module's sdist ``.tar.gz`` file is
    placed.  This file may then be installed by ``pip``, converted to a wheel
    by ``pip``, or uploaded to PyPI.  The name of the file will be derived from
    the fully qualified name of the module and the version number of the ABI
    that the module implements.  If this and the
    :option:`--documentation-dir <sip5-module --documentation-dir>` and
    :option:`--include-dir <sip-module --include-dir>` options are not
    specified then the file is created in the current directory.

.. cmdoption:: --no-sdist

    Instead of creating an sdist ``.tar.gz`` file, the module source is left as
    a directory where the contents can be subsequently modified if required.

.. cmdoption:: --setup-cfg <FILE>

    The :file:`setup.cfg` file added to the sdist ``.tar.gz`` file instead of
    the default version.  This allows the meta-data included in the sdist to be
    customised.  A number of macros may be specified in the :file:`setup.cfg`
    file:

        ``@SIP_MODULE_NAME@`` is replaced by the fully qualified module name.

        ``@SIP_MODULE_PACKAGE@`` is replaced by the name of the module's
        package.

        ``@SIP_MODULE_VERSION@`` is replaced by the version number of the
        module.
