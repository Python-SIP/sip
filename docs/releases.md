# Release Notes


## v6.8.4

### ABI v13

The latest ABI v13 is v13.7.1:

- added support for Python v3.13
- C99 support is assumed.

### ABI v12

The latest ABI v12 is v12.4.1:

- added support for Python v3.13
- C99 support is assumed.

### Added support for Python v3.13

Python v3.13 raises the minimum macOS version to 10.13.  Ensure that this
minimum is used for wheel names for projects where all modules use the limited
ABI, no matter what the minimum requirement of the version of Python being used
to build the wheel is.

Eliminated all compiler warnings on all platforms when building the `sip`
module.

Removed calls to all deprecated parts of the Python API.

Resolves [#22](https://github.com/Python-SIP/sip/issues/22)

### Make all tools accessable using `python -m`

`sip-distinfo` can now also be run using `python -m sipbuild.tools.distinfo`.

`sip-module` can now also be run using `python -m sipbuild.tools.module`.

Resolves [#21](https://github.com/Python-SIP/sip/issues/21)

### Assume C99 support

`_Bool` and `stdbool.h` are assumed to be available on all supported platforms.

`va_copy` is assumed to be available on all supported platforms.

Resolves [#13](https://github.com/Python-SIP/sip/issues/13)

### Object map incorrect size assumptions for Windows 64

`uintptr_t` is now used as the hash key in the object map rather than
`unsigned long`.

Resolves [#14](https://github.com/Python-SIP/sip/issues/14)

### `%MappedType` documentation error

The documentation for the `MappedType` directive incorrectly stated that the
type was `type` rather than `base-type`.

Resolves [#10](https://github.com/Python-SIP/sip/issues/10)

### Missing import affecting XML generation

The generation of XML (used by PyQt documentation) failed because of a missing
import.

Resolves [#18](https://github.com/Python-SIP/sip/issues/18)

### Fixed the generation of module-level attributes

This is a regression in SIP v6.8 and only affects attributes defined in hidden
namespaces.

Resolves [#19](https://github.com/Python-SIP/sip/issues/19)

### Documentation updates

The documentation is now hosted at
[Read the Docs](https://python-sip.readthedocs.io).

Resolves [#2](https://github.com/Python-SIP/sip/issues/2)

### Completion of the migration from Mercurial

SIP is now licensed under the BSD-2-Clause license.

`README.md` now reflects the state of the migration.

The project has now been migrated from `setup.py` to `setuptools_scm` and
`pyproject.toml`.

Resolves [#1](https://github.com/Python-SIP/sip/issues/1)
