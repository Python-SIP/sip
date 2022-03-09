Introduction
============

SIP is a tool for automatically generating `Python <https://www.python.org>`__
bindings for C and C++ libraries.  SIP was originally developed in 1998 for
`PyQt <https://www.riverbankcomputing.com/software/pyqt>`__ - the Python
bindings for the Qt GUI toolkit - but is suitable for generating bindings for
any C or C++ library.  SIP can also be used write self contained extension
modules, i.e. without a library to be wrapped.

This version of SIP generates bindings for Python v3.7 and later.

SIP can be installed from the `Python Package Index
<https://pypi.org/project/SIP/>`__ using :program:`pip`.

The homepage is https://www.riverbankcomputing.com/software/sip.  Here you can
find the latest development source packages and the latest version of this
documentation.

SIP can also be downloaded from the
`Mercurial <https://www.mercurial-scm.org>`__ repository at
https://www.riverbankcomputing.com/hg/sip.


License
-------

SIP is licensed under similar terms as Python itself.  SIP is also licensed
under the GPL (both v2 and v3).  It is your choice as to which license you
use.  If you choose the GPL then any bindings you create must be distributed
under the terms of the GPL.


Features
--------

SIP, and the bindings it produces, have the following features:

- bindings run under Linux, Windows, macOS, Android and iOS

- bindings can be built to use the `PEP 384
  <https://www.python.org/dev/peps/pep-0384/>`__ stable ABI so that they do not
  need to be built for each supported version of Python

- an extendable, `PEP 517
  <https://www.python.org/dev/peps/pep-0517/>`__-compliant build system that
  will build and install your bindings and create sdist and wheel files that
  you can upload to PyPI

- bindings are fast to load and minimise memory consumption especially when
  only a small sub-set of a large library is being used

- automatic conversion between standard Python and C/C++ data types

- overloading of functions and methods with different argument signatures

- support for Python's keyword argument syntax

- support for both explicitly specified and automatically generated docstrings

- access to a C++ class's protected methods

- the ability to define a Python class that is a sub-class of a C++ class,
  including abstract C++ classes

- Python sub-classes can implement the :meth:`__dtor__` method which will be
  called from the C++ class's virtual destructor

- support for ordinary C++ functions, class methods, static class methods,
  virtual class methods and abstract class methods

- the ability to re-implement C++ virtual and abstract methods in Python

- support for global and class variables

- support for global and class operators

- support for C++ namespaces

- support for C++ templates

- support for C++ exceptions and wrapping them as Python exceptions

- the automatic generation of complementary rich comparison slots

- support for deprecation warnings

- the ability to define mappings between C++ classes and similar Python data
  types that are automatically invoked

- the ability to automatically exploit any available run time type information
  to ensure that the class of a Python instance object matches the class of the
  corresponding C++ instance

- the ability to change the type and meta-type of the Python object used to
  wrap a C/C++ data type

- full support of the Python global interpreter lock, including the ability to
  specify that a C++ function of method may block, therefore allowing the lock
  to be released and other Python threads to run

- support for the concept of ownership of a C++ instance (i.e. what part of the
  code is responsible for calling the instance's destructor) and how the
  ownership may change during the execution of an application

- the ability to generate bindings for a C++ class library that itself is built
  on another C++ class library which also has had bindings generated so that
  the different bindings integrate and share code properly

- a sophisticated versioning system that allows the full lifetime of a C++
  class library, including any platform specific or optional features, to be
  described in a single set of specification files 

- support for the automatic generation of `PEP 484
  <https://www.python.org/dev/peps/pep-0484/>`__ type hint stub files

- the ability to include documentation in the specification files which can be
  extracted and subsequently processed by external tools

- the ability to include copyright notices and licensing information in the
  specification files that is automatically included in all generated source
  code.

SIP supports C++ compilers that implement the C++11 standard as a minimum.
Older compilers may work depending on what features are used.


Overview
--------

At its simplest a SIP project contains a :ref:`specification file
<ref-specification>` (:file:`.sip` file) that describes the API that the
generated bindings will wrap, and a :file:`pyproject.toml` file that describes
how the bindings will be built.  A specification file is very like a C/C++
header file with embedded :ref:`directives <ref-directives>` and
:ref:`annotations <ref-annotations>`.  The format of a :file:`pyproject.toml`
file is described in `PEP 518 <https://www.python.org/dev/peps/pep-0518/>`__.

A SIP project can either be a *standalone* project or a *package* project.  A
standalone project implements a single set of bindings (i.e. a single extension
module) that cannot be extended by another set of bindings.  A package project
implements one or more sets of mutually dependent bindings (i.e. one set of
bindings will import another set of bindings).  Such bindings may be defined in
the same project or a completely different package project (possibly with a
different maintainer).  Often the bindings of all related package projects
will be installed as part of a single top-level Python package.  For example,
the whole of PyQt5 is current implemented as 6 separate package projects each
containing between 1 and 52 sets of bindings all installed as part of the
:mod:`PyQt5` top-level package.  However there are also 3rd-party packages that
extend PyQt5 but are not installed in the :mod:`PyQt5` top-level package.

SIP also generates a :mod:`sip` module which performs the following functions:

- it implements a private C ABI used by the bindings of package projects that
  allows them to interact

- it implements a public C API used by bindings authors in hand-written code in
  situations where SIP's normal behaviour is insufficient and also when
  embedding Python in C/C++ applications

- it implements a public Python API used by application authors typically to
  configure the behaviour of bindings and to aid debugging.

The :mod:`sip` module does not use the `PEP 384
<https://www.python.org/dev/peps/pep-0384/>`__ stable ABI and so must be built
for each supported version of Python.

The version number of the :mod:`sip` module is the version number of the ABI
that the module implements. Like SIP itself, this uses `semantic versioning
<https://semver.org/spec/v2.0.0.html>`__.

When used with standalone projects the :mod:`sip` module is not a separate
module and is instead embedded in the single set of bindings.  When used with
package projects the :mod:`sip` module is a separate extension module installed
somewhere under the top-level package.

`PEP 517 <https://www.python.org/dev/peps/pep-0517/>`__ describes the concepts
of a *build frontend* and a *build backend*.  SIP implements a compliant
backend and provides a number of frontends each performing a specific type of
build.

:program:`sip-build`
    This builds the project but does not install it.  This is useful when
    developing a set of bindings.

:program:`sip-install`
    This builds and installs a project.

:program:`sip-sdist`
    This creates an sdist (a source distribution) that can be uploaded to PyPI.

:program:`sip-wheel`
    This creates a wheel (a binary distribution) that can be uploaded to PyPI.

Collectively the above are SIP's *build tools*.

:program:`pip` can also be used as a build frontend.  This has the advantage
that the user does not need to explicitly install SIP, :program:`pip` will do
that automatically.  However it has the disadvantage that :program:`pip` does
not (yet) allow the user to configure the backend using command line options.

SIP also includes some additional command line tools.

:program:`sip-distinfo`
    This creates and populates a :file:`.dist-info` directory of an
    installation or a wheel.  It is provided for build systems that extend the
    SIP build system and need to create the :file:`.dist-info` directory from
    an external tool such as :program:`make`.

:program:`sip-module`
    This builds one or more elements of the :mod:`sip` module for a set of
    package projects:

    - an sdist of the module which can be installed by :program:`pip` or
      uploaded to PyPI

    - a :file:`sip.h` header file which defines the module's ABI.  Normally you
      do not need to worry about this file but this will install a local copy
      of it if required

    - a :file:`sip.rst` file that documents the Python API of the module for
      inclusion in your project's documentation.


Installation
------------

To install SIP from PyPI, run::

    pip install sip

SIP is also included with all of the major Linux distributions.  However, it
may be a version or two out of date.


Support for Old Versions of Python
----------------------------------

When a Python version reaches it's end-of-life, support for it will be removed
in the next minor release of SIP.  For example, if the current version of SIP
is v5.x.y then the support will be removed in v5.x+1.0.
