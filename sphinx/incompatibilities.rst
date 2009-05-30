Potential Incompatibilities with Earlier Versions
=================================================

SIP v4.8
--------

Prior to this version the ``__div__`` special method implicitly defined the
``__truediv__`` special method.  From this version the ``__truediv__`` special
method must be explicitly defined.

Prior to this version the `sipWrapper`_ structure had a member called ``user``
which is available for handwritten code to use.  From this version ``user`` is
a member of the `sipSimpleWrapper`_ structure.  `sipWrapper`_ pointers can be
safely cast to `sipSimpleWrapper`_ pointers, so if your code does something
like::

    ((sipWrapper *)obj)->user = an_object_reference;

then you just need to change it to::

    ((sipSimpleWrapper *)obj)->user = an_object_reference;

The generated class names are now ``const char *`` rather than ``char *``.

The following deprecated parts of the API have been removed.

    - The ``a``, ``A``, ``M``, ``N``, ``O``, ``P`` and ``T`` format characters
      from `sipBuildResult()`_ and `sipCallMethod()`_.

    - The ``a``, ``A``, ``L`` and ``M`` format characters from
      `sipParseResult()`_.

    - ``sipConvertToCpp()``, ``sipIsSubClassInstance()`` and ``sipTransfer()``.

    - The old-style generated type convertors.

    - The ``transfer()`` function of the ``sip`` module.

In addition the deprecated ``-a`` command line option to ``configure.py`` has
been removed.

The following parts of the API are now deprecated (but still supported).

    - The `Generated Type Objects`_.

    - The `Generated Named Enum Type Objects`_.

    - `sipConvertFromInstance()`_

    - `sipConvertFromMappedType()`_

    - `sipConvertFromNamedEnum()`_

    - `sipConvertFromNewInstance()`_

    - `sipCanConvertToInstance()`_

    - `sipCanConvertToMappedType()`_

    - `sipConvertToInstance()`_

    - `sipConvertToMappedType()`_

    - `sipForceConvertToInstance()`_

    - `sipForceConvertToMappedType()`_

    - `sipClassName()`_

    - `sipFindClass()`_

    - `sipFindNamedEnum()`_

    - `sipFindMappedType()`_

    - `sipGetWrapper()`_

    - `sipReleaseInstance()`_

    - `sipReleaseMappedType()`_

    - `sipWrapper_Check()`_

    - The ``B``, ``C`` and ``E`` format characters of `sipBuildResult()`_ and
      `sipCallMethod()`_.

    - The ``s``, ``C`` and ``E`` format character of `sipParseResult()`_.

The following PyQt-specific support functions have been removed.

    - ``sipConnectRx()``

    - ``sipDisconnectRx()``

    - ``sipEmitSlot()``

    - ``sipGetSender()``


SIP v4.7.8
----------

This version allows a Python int object to be passed whenever an enum is
expected.  This can mean that two signatures that were different with prior
versions are now the same as far as Python is concerned.  The `Constrained`_
annotation can now be applied to an enum argument to revert to the earlier
behaviour.


SIP v4.7.3
----------

Prior to this version SIP did not automatically generate missing complementary
comparison operators.  Typically this was worked around by adding them
explicitly to the .sip files, even though they weren't implemented in C++ and
relied on the C++ compiler calling the complementary operator that was
implemented.

A necessary change to the code generator meant that this not longer worked and
so SIP was changed to automatically generate any missing complementary
operators.  If you have added such operators explicitly then you should remove
them or make them dependent on the particular version of SIP.


SIP v4.4
--------

    - The ``SIP_BUILD`` C preprocessor symbol has been removed.

    - ``sipConvertToCpp()``, ``sipIsSubClassInstance()`` and the old-style
      generated type convertors have been deprecated.  The functions
      `sipCanConvertToType()`_, `sipConvertToType()`_,
      `sipForceConvertToType()`_, `sipConvertFromType()`_ and
      `sipConvertFromNewType()`_ should be used instead.  Handwritten
      `%ConvertFromTypeCode`_ and `%ConvertToTypeCode`_ now has the
      responsibility for using these to implement the ``Transfer`` and
      ``TransferBack`` annotations.
