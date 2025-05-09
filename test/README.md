# Unit Tests

Unit tests are written using Python's `unittest` module.  Each sub-directory
implements a single test case, each of which may contain any number of tests.


## Running Unit Tests

To run the complete suite of tests, run:

    python -m unittest

To run the tests of a particular test case, run:

    python -m unittest TEST_SUBDIR/test_TEST.py


## Writing Unit Tests

Each test sub-directory should contain an empty `__init__.py` file, a Python
test script with the prefix `test_` and one or more `.sip` files.

The test script should contain a sub-class of `SIPTestCase`, which is imported
from the `utils` module.  This will automatically build any modules (defined by
the `.sip` files) needed to run the tests.  The modules will be available as
top-level imports.  A module's build is configurable to a limited extent - see
the class attributes of `SIPTestCase` for the relevant information.

Each `.sip` file should define a single module to build.  The C/C++
implementation being wrapped would normally be embedded in the `.sip` file
using appropriate code directives.
