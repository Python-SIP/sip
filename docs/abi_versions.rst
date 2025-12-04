ABI Versions, Multiple Interpreters and Free Threading
======================================================

As mentioned in the previous section, SIP implements a C API used by bindings
authors in hand-written code and an associated ABI used by the bindings of
package projects that allows them to interact.  SIP goes to great lengths to
maintain backwards compatibility in order to minimise changes to existing
bindings when new features are added.  When incompatible changes are made then
the major version number of the ABI is increased.  However older versions of
the ABI continue to be maintained and receive bug fixes and (compatible) new
features.  A bindings author will adopt a particular major ABI version to use
(normally the latest one available at the time) and continue to use it even if
a newer ABI becomes available.

Support for an old ABI version will be removed eventually but this will only be
done after a long deprecation period and when equivalent functionality is
available in the latest ABI.

The available ABIs and their distinguishing features are as follows:

**v12** 
    - Traditional enums are implemented as a custom Python type.
    - C++ scoped enums are implemented using the standard ``Enum`` Python type.
    - The module part of the fully qualified name of :mod:`sip` module types
      (e.g. :class:`sip.simplewrapper`) is always just ``sip`` even if the
      module is part of a package.  (Note that this is a bug that won't be
      fixed because user applications may depend on it.)

**v13**
    - All enums are implemented using the standard ``Enum`` and related Python
      types.

**v14**
    - Multiple interpreters are supported.
    - Free threading (i.e. no GIL) is supported.
    - The implementation of enums can be configured to match the behavior of
      either v12 or v13.
    - The naming of :mod:`sip` module types can be configured to match the
      behaviour of v12.
    - Python v3.15 or later is required.

ABI v14 is currently under development and not yet available for testing.

As ABI v14 requires Python v3.15 then ABIs v12 and v13 will be supported at
least until October 2030 (when Python v3.14 becomes unsupported).
