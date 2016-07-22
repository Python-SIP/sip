SIP v5 Roadmap
==============

The next major release of SIP will be v5 and is currently being planned.  While
we make no committments to release dates, we expect that the development will
be done during 2016.

The major focus of v5 will be to:

- eliminate inconsistencies in the syntax of specification files

- fill in some gaps in the C/C++ support

- restructure, refactor and rewrite the code as appropriate to ensure that it
  is easy to test, maintain and enhance over the long term.

There is no plan to introduce any significant new functionality.

Any feedback on the roadmap is very welcome.


Migration from SIP v4 to v5
---------------------------

SIP v4.18.x will be the final series of SIP v4 releases.  All features that
will be removed in SIP v5 will trigger a deprecation warning.  These will be
Use of the old syntax will trigger deprecation warning messages.  These will be
disabled by default and will be enabled by passing the ``-w`` command line
option to the code generator.

A set of specification files that does not trigger any deprecation warnings
with SIP v4.18.x should work unchanged with SIP v5.


Roadmap
-------

Here we list specific changes that are planned.  Note that no changes are
planned for the ``sip`` extension module.

- The syntax of directives will be revised to follow a standard pattern that
  supports arguments and sub-directives in a consistent manner.

- All directives will behave consistently when enclosed in ``%If``/``%End``
  blocks.

- Support will be added for fixed sized arrays of any type.

- Support for optionally detecting overflows when converting from Python
  integers to C/C++ types will be investigated.

- Invalid annotations will trigger an error message rather than being silently
  ignored.

- Error messages will be improved and will always include a reference to the
  originating file and line number.

- Support for the generation of QScintilla API files will be removed.  A
  utility to create these files from the XML export file will be added to
  QScintilla.

- The code generator's ``-I`` command line option will support Windows style
  path names.

- The code generator may be extended using plugins.

- All PyQt specific support will be removed and implemented in appropriate
  plugins that will be distributed as part of PyQt.

- The design of the code generator will allow for the implementation of plugins
  to support generating bindings for languages other than Python.

- The code generator will be reimplemented using Python v3.  It will be able to
  be used as a standalone application or a package.

- The build system will be removed.  SIP itself will be distributed as a
  ``distutils`` package.
