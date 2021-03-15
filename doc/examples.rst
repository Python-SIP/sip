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

First of all is the project's :file:`pyproject.toml` file (downloadable from
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
    # Specify sip v6 as the build system for the package.
    [build-system]
    requires = ["sip >=6, <7"]
    build-backend = "sipbuild.api"

    # Specify the PEP 566 metadata for the project.
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
library has been installed in a non-standard location?  There are a couple of
possible approaches:

- the user modifies :file:`pyproject.toml` to set the values of
  ``include-dirs`` and ``library-dirs`` appropriately

- the project provides command line options to SIP's build tools (i.e.
  :program:`sip-build`, :program:`sip-install` and :program:`sip-wheel`) that
  allows the user to specify the locations.

The first approach, while not particularly user friendly, is legitimate so long
as you document it.  However note that it cannot work when building and
installing directly from an sdist because :program:`pip` does not currently
fully implement `PEP 517 <https://www.python.org/dev/peps/pep-0517/>`__.

The second approach is the most flexible but requires code to implement it.  If
SIP finds a file called (by default) :file:`project.py` in the same directory
as :file:`pyproject.toml` then it is assumed to be an extension to the build
system.  Specifically it is expected to implement a class that is a sub-class
of SIP's :mod:`~sipbuild.AbstractProject` class.

Below is a complete :file:`project.py` that adds options to allow the user to
specify the locations of the ``fib`` header file and library.

.. parsed-literal::
    import os

    from sipbuild import Option, Project


    class FibProject(Project):
        """ A project that adds an additional configuration options to specify
        the locations of the fib header file and library.
        """

        def get_options(self):
            """ Return the sequence of configurable options. """

            # Get the standard options.
            options = super().get_options()

            # Add our new options.
            inc_dir_option = Option('fib_include_dir',
                    help="the directory containing fib.h", metavar="DIR")
            options.append(inc_dir_option)

            lib_dir_option = Option('fib_library_dir',
                    help="the directory containing the fib library",
                    metavar="DIR")
            options.append(lib_dir_option)

            return options

        def apply_user_defaults(self, tool):
            """ Apply any user defaults. """

            # Ensure any user supplied include directory is an absolute path.
            if self.fib_include_dir is not None:
                self.fib_include_dir = os.path.abspath(self.fib_include_dir)

            # Ensure any user supplied library directory is an absolute path.
            if self.fib_library_dir is not None:
                self.fib_library_dir = os.path.abspath(self.fib_library_dir)

            # Apply the defaults for the standard options.
            super().apply_user_defaults(tool)

        def update(self, tool):
            """ Update the project configuration. """

            # Get the fib bindings object.
            fib_bindings = self.bindings['fib']

            # Use any user supplied include directory.
            if self.fib_include_dir is not None:
                fib_bindings.include_dirs = [self.fib_include_dir]

            # Use any user supplied library directory.
            if self.fib_library_dir is not None:
                fib_bindings.library_dirs = [self.fib_library_dir]


The :meth:`~sipbuild.Project.get_options` method is reimplemented to add two
new :class:`~sipbuild.Option` instances.  An :class:`~sipbuild.Option` defines
a key that can be used in :file:`pyproject.toml`.  Because these
:class:`~sipbuild.Option`\s are defined as part of the
:class:`~sipbuild.Project` then the keys are used in the ``[tool.sip.project]``
section of :file:`pyproject.toml`.  In addition, because each
:class:`~sipbuild.Option` has help text, these are defined as *user options*
and therefore are also added as command line options to each of SIP's build
tools.  Note that in both :file:`pyproject.toml` and the command line any
``_`` in the :class:`~sipbuild.Option` name is converted to ``-``.

The :meth:`~sipbuild.Project.apply_user_defaults` method is reimplemented to
provide a default value for an :class:`~sipbuild.Option`.  Note that the value
is accessed as an instance attribute of the object for which the
:class:`~sipbuild.Option` is defined.  In this case there are no default values
but we want to make sure that any values that are provided are absolute path
names.

The :meth:`~sipbuild.Project.update` method is reimplemented to update the
:class:`~sipbuild.Bindings` object for the ``fib`` bindings with any values
provided by the user from the command line.


Package Projects
----------------

We now describe two package projects.  The ``examples-core`` project contains a
single set of bindings called :mod:`~examples.core`.  The ``examples-extras``
project contains a single set of bindings called :mod:`~examples.extras`.  The
:mod:`~examples.extras` module imports the :mod:`~examples.core` module.  Both
modules are part of the top-level :mod:`examples` package.  The
:mod:`~examples.sip` module required by all related package projects is also
part of the top-level :mod:`examples` package.

Again with this example, in order to make it self-contained, we are not
creating bindings for real libraries but instead embedding the implementation
within the :file:`.sip` files.


:mod:`examples.sip`
...................

In order to create an sdist for the :mod:`~examples.sip` module, run::

    sip-module --sdist examples.sip

If you want to create a wheel from the sdist then run::

    pip wheel examples_sip-X.Y.Z.tar.gz

``X.Y.Z`` is the version number of the ABI implemented by the
:mod:`~examples.sip` module and it will default to the latest version.


:mod:`examples.core`
....................

We now look at the :file:`pyproject.toml` file for the ``examples-core``
project (downloadable from 
:download:`here <../examples/package/core/pyproject.toml>`) below.

.. literalinclude:: ../examples/package/core/pyproject.toml

Compared to the standalone project's version of the file we have added the
``[tool.sip.bindings.core]`` section to specify that the project contains a
single set of bindings called :mod:`~examples.core`.  We need to do this
because the name is no longer the same as the name of the project itself as
defined by the ``name`` key of the ``[tool.sip.metadata]`` section.  The
bindings section is empty because all the default values are appropriate in
this case.

We have also added the ``[tool.sip.project]`` section containing the
``sip-module`` key, which specifies the full package name of the
:mod:`~examples.sip` module and the ``dunder-init`` key, which specifies that
an :file:`__init__.py` file (empty by default) is created in the top-level
:mod:`examples` package.

We next look at the :file:`core.sip` file (downloadable from 
:download:`here <../examples/package/core/core.sip>`) below.

.. literalinclude:: ../examples/package/core/core.sip

The :directive:`%Module` directive, as well as specifying the full package name
of the :mod:`~examples.core` module, specifies that the bindings will use the
`PEP 384 <https://www.python.org/dev/peps/pep-0384/>`__ stable ABI.

The :directive:`%DefaultEncoding` directive specifies that any character
conversions between C/C++ and Python ``str`` objects will default to the ASCII
codec.

The :directive:`%Platforms` directive defines three mutually exclusive *tags*
that can be used by :directive:`%If` directives to select platform-specific
parts of the bindings.

The remaining parts of the file are three different platform-specific
implementations of a function called :c:func:`what_am_i` which just returns a
name for the platform.

We are then left will the question as to how we specify which of the platform
tags should be selected for a particular build.  For this we need the
:file:`project.py` file file (downloadable from
:download:`here <../examples/package/core/project.py>`) shown below.

.. literalinclude:: ../examples/package/core/project.py

As before we reimplement the :meth:`~sipbuild.Project.get_options` method to
add a new :class:`~sipbuild.Option` instance to specify the platform tag.
Because no help text has been specified the :class:`~sipbuild.Option` can only
be used as a key in the ``[tool.sip.project]`` section of
:file:`pyproject.toml` and will not be added to the command line options of the
build tools.

We reimplement the :meth:`~sipbuild.Project.apply_nonuser_defaults` method
provide a default value for the new :class:`~sipbuild.Option` if it hasn't been
specified in :file:`pyproject.toml`.  If it has been specified in
:file:`pyproject.toml` then we validate it.  (Why would be want to specify the
platform in :file:`pyproject.toml`?  Perhaps we are cross-compiling and
introspecting the host platform would be inappropriate.)

The :meth:`~sipbuild.Project.update` method is reimplemented to update the
:class:`~sipbuild.Bindings` object for the ``core`` bindings with the validated
platform tag.


:mod:`examples.extras`
......................

We now look at the :file:`pyproject.toml` file for the ``example-extras``
project (downloadable from
:download:`here <../examples/package/extras/pyproject.toml>`) below.

.. literalinclude:: ../examples/package/extras/pyproject.toml

Compared to the ``examples-core`` project's version of the file we have added
the ``requires-dist`` key to the ``[tool.sip.metadata]`` section which will
ensure that the ``examples-core`` project will be automatically installed as a
prerequisite of the ``examples-extras`` project.  SIP will automatically add a
similar line to ensure the :mod:`~examples.sip` module is also installed.

Of course we have specifed an appropriately named bindings section.

We have also removed the ``dunder-init`` key from the
``[tool.sip.project.section]`` section.

We next look at the :file:`extras.sip` file (downloadable from 
:download:`here <../examples/package/extras/extras.sip>`) below.

.. literalinclude:: ../examples/package/extras/extras.sip

This is very similar to the :mod:`~examples.core` module in that it implements
simple platform-specific functions.  The key thing to notice is that there is
no need to specify the platform tag as part of the configuration as it is
obtained automatically from the installed ``examples-core`` project.

The ``examples-extras`` project has no need for a :file:`project.py` file.
