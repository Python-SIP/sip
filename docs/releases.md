# Release Notes


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
