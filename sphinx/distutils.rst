Building Your Extension with distutils
======================================

To build the example in `A Simple C++ Example`_ using distutils, it is
sufficient to create a standard ``setup.py``, listing ``word.sip`` among the
files to build, and hook-up SIP into distutils::

    from distutils.core import setup, Extension
    import sipdistutils

    setup(
      name = 'word',
      versione = '1.0',
      ext_modules=[
        Extension("word", ["word.sip", "word.cpp"]),
        ],

      cmdclass = {'build_ext': sipdistutils.build_ext}
    )

As we can see, the above is a normal distutils setup script, with just a
special line which is needed so that SIP can see and process ``word.sip``.
Then, running ``setup.py build`` will build our extension module.
