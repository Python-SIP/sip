SIP v5 Roadmap
==============

The high level goals of SIP v5 are described in the main SIP documentation.
Here we list other specific changes that are planned.  Note that no significant
changes are planned for the ``sip`` extension module.

- Support will be added for fixed sized arrays of any type.

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

- The code generator will be reimplemented using Python v3.  It will be able to
  be used as a standalone application or a package.
