Introduction
============

SIP is a tool for automatically generating `Python <https://www.python.org>`__
bindings for C and C++ libraries.  SIP was originally developed in 1998 for
`PyQt <https://www.riverbankcomputing.com/software/pyqt>`__ - the Python
bindings for the Qt GUI toolkit - but is suitable for generating bindings for
any C or C++ library.

This version of SIP generates bindings for Python v3.5 and later.

There are many other similar tools available.  One of the original such tools
is `SWIG <http://www.swig.org>`__ and, in fact, SIP is so called because it
started out as a small SWIG.  Unlike SWIG, SIP is specifically designed for
bringing together Python and C/C++ and goes to great lengths to make the
integration as tight as possible.

The homepage for SIP is https://www.riverbankcomputing.com/software/sip.  Here
you will always find the latest stable version and the latest version of this
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

- support for the automatic generation of PEP 484 type hint stub files

- the ability to include documentation in the specification files which can be
  extracted and subsequently processed by external tools

- the ability to include copyright notices and licensing information in the
  specification files that is automatically included in all generated source
  code

- a build system, written in Python, that you can extend to configure, compile
  and install your own bindings without worrying about platform specific issues

- SIP, and the bindings it produces, runs under Linux, Windows, macOS, Android
  and iOS.


SIP Components
--------------

At its simplest level the SIP code generator creates the C or C++ source code
from :file:`.sip` specification files that is compiled to produce a Python
extension module that implement Python bindings around a C or C++ library.  If
there are a number of C/C++ libraries then corresponding extension modules may
be created that make up a single Python package.  The C/C++ libraries will
typically have mutual dependencies which are reflected in the Python extension
modules.  The extension modules will automatically import each other as
required.

The SIP code generator is called :program:`sip5`.  It is covered in detail in
:ref:`ref-using`.  This is a drop-in replacement for the ``sip`` code generator
in SIP v4.

SIP will also create a ``sip`` extension module for the package that is
imported automatically by generated extension modules.  This implements a
number of utility functions used internally by the generated extension modules.
The ``sip`` module, when compiled, supports a specific version of Python (e.g.
v3.6 or v3.7) and means that generated extension modules do not have to be
compiled for a specific version of Python.  The ``sip`` module also implements
a public API that provides access to Python code of useful low-level features.

The ``sip`` extension module implements a binary ABI that is used by generated
extension module.  SIP will generate a :file:`sip.h` file that defines the ABI
and is added to the source code of the generated extension modules.

Finally SIP will generate a :file:`sip.rst` reST document that describes the
public API implemented by the ``sip`` module which can then be included as part
of any documentation for the generated extension modules.

The ``sip`` extension module, the :file:`sip.h` header file and the
:file:`sip.rst` documentation are all created by the :program:`sip5-module`
program.
