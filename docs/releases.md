# Release Notes


## v6.10.0

### Introspection of the `sip` module ABI version

`SIP_ABI_VERSION` has been added as a module attribute of the `sip` module.
This is an integer that represents the ABI version that the module implements
as a 3 part hexadecimal number.

This has been implemented in ABIs v12.17 and v13.10.

Resolves [#62](https://github.com/Python-SIP/sip/issues/62)

### `sip-module` support for older ABI versions

The ability of `sip-module` to generate sdists for older ABI versions has been
restored.  The oldest that can be generated are v12.9 and v13.1 which are the
oldest non-deprecated versions.

The documentation now contains brief histories of these versions.

Resolves [#61](https://github.com/Python-SIP/sip/issues/61)

### Default ABI version

The default minor ABI version to use is now the latest minor version for a
particular major version (rather than `0` as it was previously).

Resolves [#63](https://github.com/Python-SIP/sip/issues/63)

### Specify the ABI version on the command line

The `--abi-version` command line option has been added to `sip-build`,
`sip-install` and `sip-wheel` to specify the ABI version of the `sip` module to
generate code for.

Resolves [#64](https://github.com/Python-SIP/sip/issues/64)

### Added the `%MinimumABIVersion` directive

The `%MinimumABIVersion` directive is used to specify the minimum `sip` module
ABI to target in order to ensure that all calls to the public API from
handwritten code are available.

A consequence of this change is that passing string values to the
`/Deprecated/` annotation no longer requires an ABI version that supports them
to be targeted.  If the targeted ABI version does not support them they are
simply ignored.

Resolves [#65](https://github.com/Python-SIP/sip/issues/65)

### Escaping special characters in string annotations

The value of a string annotation uses the `;` and `:` characters to allow
platform- and feature-specific values to be defined.  These special characters
can now be escaped using a leading `\\`.  This capability has now been
documented.

Resolves [#59](https://github.com/Python-SIP/sip/issues/59)

### Bug fixes

- The license designator in `setup.py` for the latest versions of the `sip`
  module are now correctly specified as `BSD-2-Clause`.
- The code generation has been fixed for classes created by instantiating
  mapped type templates using a `typedef` with the `/NoTypeName/` annotation.
  Resolves [#66](https://github.com/Python-SIP/sip/issues/66)


## v6.9.1

### `sip-sdist` creates PEP 625 compatible file names

The sdists created by `sip-sdist` are now compatible with PEP 625 in that
they have lower case names.  This ensures that they will still be able to
be uploaded to PyPI.

Resolves [#23](https://github.com/Python-SIP/sip/issues/23)

### Update the minimum version of `setuptools`

The minimum version of `setuptools` used by SIP and by the generated `sip`
module sdists has been set to v69.5.  This is the oldest version that
supports PEP 625.

Resolves [#55](https://github.com/Python-SIP/sip/issues/55)

### `sip` module sdist `Requires-Python` is incorrect

The minimum Python version in the generated `sip` module metadata is now
set to v3.9.

Resolves [#56](https://github.com/Python-SIP/sip/issues/56)


## v6.9.0

### Removal of support for Python v3.8

- Generated bindings now require Python v3.9 or later.
- Type hints now conform to PEP 585.
- The latest `sip` module ABI versions are v12.16 and v13.9.

Resolves [#37](https://github.com/Python-SIP/sip/issues/37)

### Added support for the `deprecated()` decorator in `.pyi` files

- `.pyi` files now use the `deprecated()` decorator when the `/Deprecated/`
  annotation is specified.
- The `/Deprecated/` annotation may now specify an optional string which will
  be appended to the default deprecation warning.

Resolves [#8](https://github.com/Python-SIP/sip/issues/8)

### PyQt-specific support for registering `QFlags` types

Calls to `qMetaTypeId()` for all `QFlags` mapped types are now automatically
generated for PyQt using ABI v13.

Resolves [#43](https://github.com/Python-SIP/sip/issues/43)

### Bug fixes

- The handling of the SIP versions timeline was fixed.  (Resolves
  [#47](https://github.com/Python-SIP/sip/issues/47))
- Annotations are now only validated if they are known to be needed.
- The buffer protocol support for byte objects was fixed. (Resolves
  [#36](https://github.com/Python-SIP/sip/issues/36))
- All outstanding compiler warnings were eliminated when building the `sip`
  module.  (Resolves [#32](https://github.com/Python-SIP/sip/issues/32))
- Redundant `%` in trace output of `this` pointer in were removed.  (Pull
  request [#33](https://github.com/Python-SIP/sip/pull/33))


## v6.8.6

### Handle single number macOS deployment targets

If the macOS deployment target (as returned by `sysconfig.get_platform()`)
was just a major version number then SIP would crash.

Resolves [#31](https://github.com/Python-SIP/sip/issues/31)

### Support for architectures where `char` is unsigned

Conversions to and from `char` and Python integer objects on architectures
where `char` was unsigned (eg. Linux on ARM) have been fixed.

The latest `sip` module ABI versions are v12.15 and v13.8.

Resolves [#29](https://github.com/Python-SIP/sip/issues/29)

### Support for building from git archives

`.git_archival.txt` and `.gitattributes` were added so that git archives
contain the necessary metadata for [setuptools-scm](https://setuptools-scm.readthedocs.io/en/stable/usage#git-archives).

Pull request [#30](https://github.com/Python-SIP/sip/pull/30)

### Run the tests using the current Python version

The tests are run using the current Python version instead of the default
one to make it easier to test using multiple Python versions.

Pull request [#27](https://github.com/Python-SIP/sip/pull/27)


## v6.8.5

### Missing dependency in `pyproject.toml`

Added `setuptools` as a project dependency.

Resolves [#26](https://github.com/Python-SIP/sip/issues/26)

### V6.8.4 release notes are incorrect

The incorrect entries in the v6.8.4 release notes regarding the latest ABI
version numbers were removed.

Resolves [#24](https://github.com/Python-SIP/sip/issues/24)


## v6.8.4

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
