Examples
========

In this section we walk through two simple examples, one a standalone project
and the other a pair of package projects.  These will introduce the basic
features of SIP.  Other sections of this documentation will contain complete
descriptions of all available features.


A Standalone Project
--------------------

This project implements a module called :mod:`fib` that contains a function
:func:`fib_n` which takes a single integer argument ``n`` and returns the
n'th value of the Fibonacci series.

Note that the example does not wrap a separate C/C++ library that implements
:c:func:`fib_n`.  Instead it provides the C implementation within the
:file:`.sip` specification file itself.  While this is not the way SIP is
normally used it means that the example is entirely self contained.  Later in
this section we will describe the changes to the project that would be needed
if a separate library was being wrapped.

First of all in the project's :file:`pyproject.toml` file (downloadable from
:download:`here <../examples/standalone/pyproject.toml>`) which we show in its
entirety below.

.. literalinclude:: ../examples/standalone/pyproject.toml

The file is in `TOML <https://github.com/toml-lang/toml>`__ format and the
structure is defined in `PEP 518
<https://www.python.org/dev/peps/pep-0518/>`__.

The ``[build-system]`` section is used by build frontends to determine what
version of what build backend is to be used.  :program:`pip`, for example, will
download, install and invoke an appropriate version automatically.

The ``[tool.sip.metadata]`` section specified the name of the project (as it
would appear on PyPI).  This is the minimum information needed to build a
standalone project.

Next is the module's :file:`.sip` specification file (downloadable from
:download:`here <../examples/standalone/fib.sip>`) which we also show in its
entirety below.

.. literalinclude:: ../examples/standalone/fib.sip

The first line of interest is the :directive:`%Module` directive.  This defines
the name of the extension module that will be created.  In the case of
standalone projects this would normally be the same as the name defined in the
:file:`pyproject.toml` file.  It also specifies that the code being wrapped is
implemented in C (as opposed to C++).

The next line of interest is the declaration of the :c:func:`fib_n` function to
be wrapped.

The remainder of the file is the :directive:`%MethodCode` directive attached to
the function declaration.  This is used to provide the actual implementation of
the :c:func:`fib_n` function and would not be needed if we were wrapping a
separate library.  In this code ``a0`` is the value of the first argument
passed to the function and converted from a Python ``int`` object, and
``sipRes`` is the value that will be converted to a Python ``int`` object and
returned by the function.

The project may be built and installed by running::

    sip-install

It may also be built and installed by running::

    pip install .

An sdist (to be installed by :program:`pip`) may be created for the project by
running::

    sip-sdist

A wheel (to be installed by :program:`pip`) may be created for the project by
running::

    sip-wheel

An installed project may be uninstalled by running::

    pip uninstall fib


Using a Real Library
....................

If there was a real ``fib`` library to be wrapped, with a corresponding
:file:`fib.h` header file, then the :file:`pyproject.toml` file would look more
like that shown below.

.. parsed-literal::
    # Specify sip v5 as the build system for the package.
    [build-system]
    requires = ["sip >=5, <6"]
    build-backend = "sipbuild.api"

    # Specify the PEP 522 metadata for the project.
    [tool.sip.metadata]
    name = "fib"

    # Configure the building of the fib bindings.
    [tool.sip.bindings.fib]
    headers = ["fib.h"]
    include-dirs = ["/path/to/headers"]
    libraries = ["fib"]
    library-dirs = ["/path/to/libraries"]

The ``include-dirs`` and ``library-dirs`` would only need to be specified if
they are not installed in standard locations and found automatically by the
compiler.

Note that POSIX path separators are used.  SIP will automatically convert these
to native path separators when required.

The :file:`.sip` file would look more like that shown below.

.. parsed-literal::
    // Define the SIP wrapper to the (actual) fib library.

    %Module(name=fib, language="C")

    %ModuleCode
    #include <fib.h>
    %End

    int fib_n(int n);

The :directive:`%MethodCode` directive has been removed and the
:directive:`%ModuleCode` directive has been added.


Configuring a Build
...................

How should a project deal with the situation where, for example, the ``fib``
library has been installed in a non-standard location?

TODO - user modifications to pyproject.toml

TODO - project.py to search a list of locations

TODO - project.py providing a command line options to specify the locations


Package Projects
----------------

TODO
