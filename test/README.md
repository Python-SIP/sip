# Unit Tests

Unit tests are written using the `pytest` package.  Each sub-directory
implements a related set of tests.  The tests are run for one ABI version using
the version of Python for which `pytest` is installed.


## Running Unit Tests

To run the complete suite of tests for the latest ABI version, run:

    pytest

To run the complete suite of tests for a specific ABI version, run:

    pytest --sip-abi-version ABI

Note that the tests for different ABI versions must **not** be run
simultaneously.


## Writing Unit Tests

Each test sub-directory should contain a one or more `.sip` files and a Python
test script with the prefix `test_`.

Each `.sip` file should define a single module to build.  The C/C++
implementation being wrapped would normally be embedded in the `.sip` file
using appropriate code directives.

The test script may set the following module attributes to configure the tests:

- `cfg_enabled_for` specifies a sequence of ABI versions for which the tests
  are enabled.  If the sequence is empty or `None` then the tests will be
  disabled.
- `cfg_disabled_for` specifies a sequence of ABI versions for which the tests
  are disabled.  If this and the `cfg_enabled_for` attributes are missing (ie.
  the default) then the tests are enabled.
- `cfg_exceptions` is `True` if the test module should be built with support
  for C++ exceptions.
- `cfg_package` is the name of a package into which generated test modules are
  placed.  This also causes a separate `sip` module to be generated.  Normally
  the `sip` module is embedded in the (only) generated test module.
- `cfg_sip_module_configuration` is an optional sequence of values used to
  configure the `sip` module.  This only applies to ABI v14.
- `cfg_tags` is an optional sequence of tags to be applied when building the
  test module.

The following fixtures are provided:

- `abi_version` is the major version of the ABI version being tested as an
  integer.
- `code` is `True` if the test module successfully compiled and `False`
  otherwise.  The module is not imported.
- `module` is an imported test module.
- `package` is an imported package containing one or more imported test modules
  and an imported `sip` module.
- `virtual_hook` is an object that will capture any exception raised during the
  execution of a Python reimplementation of a C++ virtual.  The object has a
  `reraise()` method that will reraise any captured exception so that it can be
  caught by `pytest.raises()`.
