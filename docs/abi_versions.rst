.. _ref-abi-versions:

ABI Versions
============

.. note::
    ABI v14 is currently considered to be experimental.  It will continue to be
    developed and refined and its full version number will remain at v14.0.0
    during this process.

SIP implements a set of ABIs which have differing behaviours and are used by
the generated extension modules of package projects to allow them to interact.
A bindings author must choose which ABI version they are going to target.
Extension modules that are part of the same package must target the same ABI
version.

Each ABI has a corresponding :ref:`C API <ref-c-api>` that is used by bindings
authors in handwritten code.


.. _ref-abi-to-target:

Specifying the ABI to Target
----------------------------

The ABI version to target can be specified explicitly or, in some cases, can be
infered.

A project may specify the :directive:`%MinimumABIVersion` directive once for
each ABI version the project supports.  If the project supports multiple ABI
versions then the directive **must** be specified for **each** one.  The
default ABI is that specified by the first use of
:directive:`%MinimumABIVersion`.

The :ref:`abi-version <ref-pyproject-abi-version>` key of the
`tool.sip.project` section of the `pyproject.toml` file can be used to specify
the major version, thereby overriding the order of multiple
:directive:`%MinimumABIVersion` directives.  (It may also specify the minor
version but this is usually omitted so that the latest minor version is used.)
The value may be overridden by the corresponding ``--abi-version`` command line
options of the build tools.

If neither :directive:`%MinimumABIVersion` noor :ref:`abi-version
<ref-pyproject-abi-version>` are specified then a default ABI is used.  For SIP
v6 this is ABI v13.

Unless a minor ABI version number is specified the latest available minor
version of the major version will be used.

.. note::
    The best practice is to use the :directive:`%MinimumABIVersion` directive
    for each supported major version to specify the corresponding full minimum
    version number (using ``.0`` if any minor version will do) placing the
    default ABI version first.

    The :ref:`abi-version <ref-pyproject-abi-version>` key should not be used
    except via the ``--abi-version`` command line option when overriding the
    default ABI.


ABI v14 Features
----------------

This ABI is intended to replace the older ABIs.  Existing projects that use an
older ABI should plan to migrate to this ABI.  However bindings authors should
be aware that their bindings may behave in slightly different ways which should
be communicated to their users.  (These changes are enhancements and, arguably,
bug fixes.)

- Python v3.15 or later is required.
- C++20 support is required when wrapping C++ libraries.
- Multiple interpreters are supported.
- Free-threading (i.e. no-GIL) is supported.
- Extension modules can be unimported and reimported (assuming the library
  being wrapped allows this).
- The implementation of enums can be configured to match the behavior of either
  v12 or v13.
- The naming of :mod:`sip` module types can be configured to match the
  behaviour of v12.
- The behaviour when setting the value of wrapped static variables (of either
  modules or classes) has changed.  With ABI v14 the value of the variable is
  changed appropriately and may involve a type conversion of the new value (if
  the new value has a different type to the C/C++ variable).  With older ABIs
  the Python attribute would be changed to the new value but the underlying
  C/C++ variable would not be change.  This change also means that ``const``
  global variables can no longer be modified.
- The legacy pickle format is not supported.  Applications need to rewrite any
  pickled data using ABI v12.19 (or later) or ABI v13.12 (or later) before
  using ABI v14.
- Any wrapped type can now be used as a mixin and there is no need to identify
  mixin classes using the :canno:`Mixin` class annotation.
- The :py:func:`~sip.cast` function of the :mod:`sip` module is not supported.
- The :directive:`%GetCode` and :directive:`%SetCode` directives can now also
  be used for global variables.
- The support for namespaces defined across modules is now complete.  For
  example if a function is defined in a namespace in one module it is now
  possible to define a new overload in another module so that it is visible as
  such in the original module.
- The :aanno:`DisallowNone` argument annotation is now supported for string and
  array types, capsules, structs, unions and void pointers.
- Inplace operators now return a value with the type specified as the return
  type in the :file:`.sip` file.  Older ABIs assumed that such operators
  returned ``self`` and ignored the return type in the :file:`.sip` file.  This
  means that the reference to the wrapped object is correctly rebound to the
  object returned by the operator.  For example, an operator declared as
  ``void operator+=(int)`` and called as ``klass += 5`` will result in
  ``klass`` having a value of ``None`` as it has been rebound to the value
  returned by the operator (ie. ``None``).  This behaviour is consistent with
  that of a Python implementation of ``__iadd__()``.

The internal design of this ABI means that significant new features can be
added without affecting binary compatibility.  Therefore a newer ABI should
only be needed if new versions of Python itself imposes major changes (as it
did to support free-threading).


ABI v13 Features
----------------

This ABI was introduced primarily to make the handling of enums consistent
once the :py:mod:`enum` module had been added to Python's standard library.

- C++11 support is required when wrapping C++ libraries.
- All enums are implemented using the standard :py:class:`~enum.Enum` and
  related Python types.

This ABI will be supported until October 2030 (when Python v3.14 is no longer
supported).


ABI v12 Features
----------------

This is the oldest ABI that is still supported.

- Traditional enums are implemented as a custom Python type.
- C++ scoped enums are implemented using the standard :py:class:`~enum.Enum`
  Python type.
- The module part of the fully qualified name of :mod:`sip` module types (e.g.
  :class:`sip.simplewrapper`) is always just ``sip`` even if the module is part
  of a package.  (Note that this is a bug that won't be fixed because user
  applications may depend on it.)

This ABI will be supported until October 2030 (when Python v3.14 is no longer
supported).
