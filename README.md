# SIP - A Python Bindings Generator for C and C++ Libraries

One of the features of Python that makes it so powerful is the ability to take
existing libraries, written in C or C++, and make them available as Python
extension modules.  Such extension modules are often called bindings for the
library.

SIP is a collection of tools that makes it very easy to create Python bindings
for C and C++ libraries.  It was originally developed in 1998 to create
[PyQt](https://pypi.org/project/PyQt6/), the Python bindings for the Qt
toolkit, but can be used to create bindings for any C or C++ library.  For
example it is also used to generate [wxPython](https://wxpython.org/), the
Python bindings for [wxWidgets](https://wxwidgets.org/).

SIP comprises a set of build tools and a `sip` module. The build tools process
a set of `.sip` specification files and generates C or C++ code which is then
compiled to create the bindings extension module.  Several extension modules
may be installed in the same Python package.  Extension modules can be built so
that they are are independent of the version of Python being used.

The specification files contain a description of the interface of the C or C++
library, i.e. the classes, methods, functions and variables.  The format of a
specification file is almost identical to a C or C++ header file, so much so
that the easiest way of creating a specification file is to edit a copy of the
corresponding header file.

The `sip` module provides support functions to the automatically generated
code.  The `sip` module is installed as part of the same Python package as the
generated extension modules.  Unlike the extension modules the `sip` module is
specific to a particular version of Python (e.g. v3.8, v3.9, v3.10, v3.11,
v3.12).

SIP makes it easy to exploit existing C or C++ libraries in a productive
interpretive programming environment.  SIP also makes it easy to take a Python
application (maybe a prototype) and selectively implement parts of the
application (maybe for performance reasons) in C or C++.

[MetaSIP](https://github.com/Python-SIP/metasip/) is a GUI development for SIP
that can take the header files of a C/C++ library into a project from which API
items can be managed, compared with new versions etc. and from which the `.sip`
specification files can be generated from.


## Documentation

The documentation can be found at
[Read the Docs](https://python-sip.readthedocs.io).


## License

SIP is licensed under the BSD 2 clause license.

SIP includes a copy of [ply](https://github.com/dabeaz/ply/) which is licensed
under the BSD 3 clause license.


## Installation

To install SIP, run:

    pip install sip


## Creating Packages for Distribution

Python sdists and wheels can be created with any standard Python build
frontend.

For example, using [build](https://pypi.org/project/build/) an sdist and wheel
will be created from a checkout in the current directory by running:

    python -m build --outdir .


## Building the Documentation

The documentation is built using [Sphinx](https://pypi.org/project/Sphinx/),
[myst_parser](https://pypi.org/project/myst-parser/) and the
[sphinx-rtd-theme](https://pypi.org/project/sphinx-rtd-theme/) theme.

Change to the `docs` directory of a checkout and run:

    make html

The HTML documentation can then be found in the `_build/html` subdirectory.
