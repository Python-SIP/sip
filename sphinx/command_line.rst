The SIP Command Line
====================

The syntax of the SIP command line is::

    sip [options] [specification]

``specification`` is the name of the specification file for the module.  If it
is omitted then ``stdin`` is used.

The full set of command line options is:

-h      Display a help message.
-V      Display the SIP version number.
-a file
        The name of the QScintilla API file to generate.  This file contains a
        description of the module API in a form that the QScintilla editor
        component can use for auto-completion and call tips.  (The file may
        also be used by the SciTE editor but must be sorted first.)  By default
        the file is not generated.
-b file
        The name of the build file to generate.  This file contains the
        information about the module needed by the SIP build system to generate
        a platform and compiler specific Makefile for the module.  By default
        the file is not generated.
-c dir  The name of the directory (which must exist) into which all of the
        generated C or C++ code is placed.  By default no code is generated.
-d file
        The name of the documentation file to generate.  Documentation is
        included in specification files using the `%Doc`_ and `%ExportedDoc`_
        directives.  By default the file is not generated.
-e      Support for C++ exceptions is enabled.  This causes all calls to C++
        code to be enclosed in ``try``/``catch`` blocks and C++ exceptions to
        be converted to Python exceptions.  By default exception support is
        disabled.
-g      The Python GIL is released before making any calls to the C/C++ library
        being wrapped and reacquired afterwards.  See `The Python Global
        Interpreter Lock`_ and the ReleaseGIL_ and HoldGIL_ annotations.
-I dir  The directory is added to the list of directories searched when looking
        for a specification file given in an `%Include`_ or `%Import`_
        directive.  This option may be given any number of times.
-j number
        The generated code is split into the given number of files.  This make
        it easier to use the parallel build facility of most modern
        implementations of ``make``.  By default 1 file is generated for each C
        structure or C++ class.
-p module
        The name of the `%ConsolidatedModule`_ which will contain the wrapper
        code for this component module.
-r      Debugging statements that trace the execution of the bindings are
        automatically generated.  By default the statements are not generated.
-s suffix
        The suffix to use for generated C or C++ source files.  By default
        ``.c`` is used for C and ``.cpp`` for C++.
-t tag  The SIP version tag (declared using a `%Timeline`_ directive) or the
        SIP platform tag (declared using the `%Platforms`_ directive) to
        generate code for.  This option may be given any number of times so
        long as the tags do not conflict.
-w      The display of warning messages is enabled.  By default warning
        messages are disabled.
-x feature
        The feature (declared using the `%Feature`_ directive) is disabled.
-z file
        The name of a file containing more command line options.
