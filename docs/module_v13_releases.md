# `sip` Module v13 History


## v13.10

- Added the `SIP_ABI_VERSION` module attribute.


## v13.9

- Added support for the `/Deprecated/` annotation accepting an optional string.
- The buffer protocol support for byte objects was fixed. (Resolves
  [#36](https://github.com/Python-SIP/sip/issues/36))
- Removed support for Python v3.8.


## v13.8

- Conversions to and from `char` and Python integer objects on architectures
  where `char` was unsigned (eg. Linux on ARM) have been fixed.  (Resolves
  [#29](https://github.com/Python-SIP/sip/issues/29))


## v13.7

- Relicensed under the BSD-2-Clause license.
- Potential overflow problems on Windows 64 were fixed.  (Resolves
  [#14](https://github.com/Python-SIP/sip/issues/14))
- C99 support is required.  `_Bool`, `stdbool.h` and `va_copy` are assumed to
  be available on all supported platforms.  (Resolves
  [#13](https://github.com/Python-SIP/sip/issues/13))
- Added support for Python v3.13.
- Removed support for Python v3.7.


## v13.6

- Added support for Python v3.12.
- Added `sipPyTypeDictRef()` to the public API.
- Deprecated `sipPyTypeDict()` in the public API.


## v13.5

- A bug related to constructors with the `/Transfer/` annotation was fixed.
- A garbage collection bug was fixed.


## v13.4

- Added support for `sip.array`.
- Enums with unexpected values are supported by automatically creating
  corresponding pseudo-members.


## v13.3

- Added support for Python v3.11.
- Removed support for Python v3.6.


## v13.2

- Added support for enums with unsigned values.
