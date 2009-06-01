.. _ref-c-api:

C API for Handwritten Code
==========================

In this section we describe the API that can be used by handwritten code in
specification files.


.. cmacro:: SIP_API_MAJOR_NR

    This is a C preprocessor symbol that defines the major number of the SIP
    API.  Its value is a number.  There is no direct relationship between this
    and the SIP version number.


.. cmacro:: SIP_API_MINOR_NR

    This is a C preprocessor symbol that defines the minor number of the SIP
    API.  Its value is a number.  There is no direct relationship between this
    and the SIP version number.


.. cmacro:: SIP_BLOCK_THREADS

    This is a C preprocessor macro that will make sure the Python Global
    Interpreter Lock (GIL) is acquired.  Python API calls must only be made
    when the GIL has been acquired.  There must be a corresponding
    :cmacro:`SIP_UNBLOCK_THREADS` at the same lexical scope.


.. cmacro:: SIP_NO_CONVERTORS

    This is a flag used by various type convertors that suppresses the use of a
    type's :directive:`%ConvertToTypeCode`.


.. cmacro:: SIP_NOT_NONE

    This is a flag used by various type convertors that causes the conversion
    to fail if the Python object being converted is ``Py_None``.


.. cmacro:: SIP_SSIZE_T

    This is a C preprocessor macro that is defined as ``Py_ssize_t`` for Python
    v2.5 and later, and as ``int`` for earlier versions of Python.  It makes it
    easier to write PEP 353 compliant handwritten code.


.. cmacro:: SIP_UNBLOCK_THREADS

    This is a C preprocessor macro that will restore the Python Global
    Interpreter Lock (GIL) to the state it was prior to the corresponding
    :cmacro:`SIP_BLOCK_THREADS`.


.. cmacro:: SIP_VERSION

    This is a C preprocessor symbol that defines the SIP version number
    represented as a 3 part hexadecimal number (e.g. v4.0.0 is represented as
    ``0x040000``).


.. cmacro:: SIP_VERSION_STR

    This is a C preprocessor symbol that defines the SIP version number
    represented as a string.  For development snapshots it will start with
    ``snapshot-``.


.. cfunction:: void sipBadCatcherResult(PyObject *method)

    This raises a Python exception when the result of a Python reimplementation
    of a C++ method doesn't have the expected type.  It is normally called by
    handwritten code specified with the :directive:`%VirtualCatcherCode`
    directive.

    :param method:
        the Python method and would normally be the supplied
        :ctype:`sipMethod`.


.. cfunction:: void sipBadLengthForSlice(SIP_SSIZE_T seqlen, SIP_SSIZE_T slicelen)

    This raises a Python exception when the length of a slice object is
    inappropriate for a sequence-like object.  It is normally called by
    handwritten code specified for :meth:`__setitem__` methods.
    
    :param seqlen:
        the length of the sequence.
    :param slicelen:
        the length of the slice.


.. cfunction:: PyObject *sipBuildResult(int *iserr, const char *format, ...)

    This creates a Python object based on a format string and associated
    values in a similar way to the Python :cfunc:`Py_BuildValue()` function.
    
    :param iserr:
        if this is not ``NULL`` then the location it points to is set to a
        non-zero value.
    :param format:
        the string of format characters.
    :return:
        If there was an error then ``NULL`` is returned and a Python exception
        is raised.
        
    If the format string begins and ends with parentheses then a tuple of
    objects is created.  If it contains more than one format character then
    parentheses must be specified.

    In the following description the first letter is the format character, the
    entry in parentheses is the Python object type that the format character
    will create, and the entry in brackets are the types of the C/C++ values
    to be passed. 

    ``a`` (string) [char]
        Convert a C/C++ ``char`` to a Python v2 or v3 string object.

    ``b`` (boolean) [int]
        Convert a C/C++ ``int`` to a Python boolean.

    ``c`` (string/bytes) [char]
        Convert a C/C++ ``char`` to a Python v2 string object or a Python v3
        bytes object.

    ``d`` (float) [double]
        Convert a C/C++ ``double`` to a Python floating point number.

    ``e`` (integer) [enum]
        Convert an anonymous C/C++ ``enum`` to a Python integer.

    ``f`` (float) [float]
        Convert a C/C++ ``float`` to a Python floating point number.

    ``g`` (string/bytes) [char \*, :cmacro:`SIP_SSIZE_T`]
        Convert a C/C++ character array and its length to a Python v2 string
        object or a Python v3 bytes object.  If the array is ``NULL`` then the
        length is ignored and the result is ``Py_None``.

    ``h`` (integer) [short]
        Convert a C/C++ ``short`` to a Python integer.

    ``i`` (integer) [int]
        Convert a C/C++ ``int`` to a Python integer.

    ``l`` (long) [long]
        Convert a C/C++ ``long`` to a Python integer.

    ``m`` (long) [unsigned long]
        Convert a C/C++ ``unsigned long`` to a Python long.

    ``n`` (long) [long long]
        Convert a C/C++ ``long long`` to a Python long.

    ``o`` (long) [unsigned long long]
        Convert a C/C++ ``unsigned long long`` to a Python long.

    ``s`` (string/bytes) [char \*]
        Convert a C/C++ ``'\0'`` terminated string to a Python v2 string object
        or a Python v3 bytes object.  If the string pointer is ``NULL`` then
        the result is ``Py_None``.

    ``t`` (long) [unsigned short]
        Convert a C/C++ ``unsigned short`` to a Python long.

    ``u`` (long) [unsigned int]
        Convert a C/C++ ``unsigned int`` to a Python long.

    ``w`` (unicode/string) [wchar_t]
        Convert a C/C++ wide character to a Python v2 unicode object or a
        Python v3 string object.

    ``x`` (unicode/string) [wchar_t \*]
        Convert a C/C++ ``L'\0'`` terminated wide character string to a Python
        v2 unicode object or a Python v3 string object.  If the string pointer
        is ``NULL`` then the result is ``Py_None``.

    ``A`` (string) [char \*]
        Convert a C/C++ ``'\0'`` terminated string to a Python v2 or v3 string
        object.  If the string pointer is ``NULL`` then the result is
        ``Py_None``.

    ``B`` (wrapped instance) [*type* \*, :ctype:`sipWrapperType` \*, PyObject \*]
        Convert a new C structure or a new C++ class instance to a Python class
        instance object.  Ownership of the structure or instance is determined
        by the ``PyObject *`` argument.  If it is ``NULL`` and the instance has
        already been wrapped then the ownership is unchanged.  If it is
        ``NULL`` or ``Py_None`` then ownership will be with Python.  Otherwise
        ownership will be with C/C++ and the instance associated with the
        ``PyObject *`` argument.  The Python class is influenced by any
        applicable :directive:`%ConvertToSubClassCode` code.

        .. note::
            This is deprecated from SIP v4.8.  Instead you should use ``N``.

    ``C`` (wrapped instance) [*type* \*, :ctype:`sipWrapperType` \*, PyObject \*]
        Convert a C structure or a C++ class instance to a Python class
        instance object.  If the structure or class instance has already been
        wrapped then the result is a new reference to the existing class
        instance object.  Ownership of the structure or instance is determined
        by the ``PyObject *`` argument.  If it is ``NULL`` and the instance has
        already been wrapped then the ownership is unchanged.  If it is
        ``NULL`` and the instance is newly wrapped then ownership will be with
        C/C++.  If it is ``Py_None`` then ownership is transferred to Python
        via a call to :cfunc:`sipTransferBack()`.  Otherwise ownership is
        transferred to C/C++ and the instance associated with the
        ``PyObject *`` argument via a call to :cfunc:`sipTransferTo()`.  The
        Python class is influenced by any applicable
        :directive:`%ConvertToSubClassCode` code.

        .. note::
            This is deprecated from SIP v4.8.  Instead you should use ``D``.

    ``D`` (wrapped instance) [*type* \*, const :ctype:`sipTypeDef` \*, PyObject \*]
        Convert a C structure, C++ class or mapped type instance to a Python
        object.  If the instance has already been wrapped then the result is a
        new reference to the existing object.  Ownership of the instance is
        determined by the ``PyObject *`` argument.  If it is ``NULL`` and the
        instance has already been wrapped then the ownership is unchanged.  If
        it is ``NULL`` and the instance is newly wrapped then ownership will be
        with C/C++.  If it is ``Py_None`` then ownership is transferred to
        Python via a call to :cfunc:`sipTransferBack()`.  Otherwise ownership
        is transferred to C/C++ and the instance associated with the
        ``PyObject *`` argument via a call to :cfunc:`sipTransferTo()`.  The
        Python class is influenced by any applicable
        :directive:`%ConvertToSubClassCode` code.

    ``E`` (wrapped enum) [enum, PyTypeObject \*]
        Convert a named C/C++ ``enum`` to an instance of the corresponding
        Python named enum type.

        .. note::
            This is deprecated from SIP v4.8.  Instead you should use ``F``.

    ``F`` (wrapped enum) [enum, :ctype:`sipTypeDef` \*]
        Convert a named C/C++ ``enum`` to an instance of the corresponding
        Python named enum type.

    ``G`` (unicode) [wchar_t \*, :cmacro:`SIP_SSIZE_T`]
        Convert a C/C++ wide character array and its length to a Python unicode
        object.  If the array is ``NULL`` then the length is ignored and the
        result is ``Py_None``.

    ``N`` (wrapped instance) [*type* \*, :ctype:`sipTypeDef` \*, PyObject \*]
        Convert a new C structure, C++ class or mapped type instance to a
        Python object.  Ownership of the instance is determined by the
        ``PyObject *`` argument.  If it is ``NULL`` and the instance has
        already been wrapped then the ownership is unchanged.  If it is
        ``NULL`` or ``Py_None`` then ownership will be with Python.  Otherwise
        ownership will be with C/C++ and the instance associated with the
        ``PyObject *`` argument.  The Python class is influenced by any
        applicable :directive:`%ConvertToSubClassCode` code.

    ``R`` (object) [PyObject \*]
        The result is value passed without any conversions.  The reference
        count is unaffected, i.e. a reference is taken.

    ``S`` (object) [PyObject \*]
        The result is value passed without any conversions.  The reference
        count is incremented.

    ``V`` (sip.voidptr) [void \*]
        Convert a C/C++ ``void *`` Python :class:`sip.voidptr` object.


.. cfunction:: PyObject *sipCallMethod(int *iserr, PyObject *method, const char *format, ...)

    This calls a Python method passing a tuple of arguments based on a format
    string and associated values in a similar way to the Python
    :cfunc:`PyObject_CallObject()` function.
    
    :param iserr:
        if this is not ``NULL`` then the location it points to is set to a
        non-zero value.
    :param method:
        the Python bound method to call.
    :param format:
        the string of format characters (see :cfunc:`sipBuildResult()`).
    :return:
        If there was an error then ``NULL`` is returned and a Python exception
        is raised.

    It is normally called by handwritten code specified with the
    :directive:`%VirtualCatcherCode` directive with method being the supplied
    :ctype:`sipMethod`.


.. cfunction:: int sipCanConvertToEnum(PyObject *obj, const sipTypeDef *td)

    This checks if a Python object can be converted to a named enum.
    
    :param obj:
        the Python object.
    :param td:
        the enum's :ref:`generated type structure <ref-type-structures>`.
    :return:
        a non-zero value if the object can be converted.


.. cfunction:: int sipCanConvertToInstance(PyObject *obj, sipWrapperType *type, int flags)

    This checks if a Python object can be converted to an instance of a C
    structure or C++ class.
    
    :param obj:
        the Python object.
    :param type:
        the C/C++ type's :ref:`generated type object <ref-type-objects>`.
    :param flags:
        any combination of the :cmacro:`SIP_NOT_NONE` and
        :cmacro:`SIP_NO_CONVERTORS` flags.
    :return:
        a non-zero value if the object can be converted.

    .. note::
        This is deprecated from SIP v4.8.  Instead you should use
        :cfunc:`sipCanConvertToType()`.


.. cfunction:: int sipCanConvertToMappedType(PyObject *obj, const sipMappedType *mt, int flags)

    This checks if a Python object can be converted to an instance of a C
    structure or C++ class which has been implemented as a mapped type.
    
    :param obj:
        the Python object.
    :param mt:
        the an opaque structure returned by :cfunc:`sipFindMappedType()`.
    :param flags:
        this may be the :cmacro:`SIP_NOT_NONE` flag.
    :return:
        a non-zero value if the object can be converted.

    .. note::
        This is deprecated from SIP v4.8.  Instead you should use
        :cfunc:`sipCanConvertToType()`.


.. cfunction:: int sipCanConvertToType(PyObject *obj, const sipTypeDef *td, int flags)

    This checks if a Python object can be converted to an instance of a C
    structure, C++ class or mapped type.
    
    :param obj:
        the Python object.
    :param td:
        the C/C++ type's :ref:`generated type structure <ref-type-structures>`.
    :param flags:
        any combination of the :cmacro:`SIP_NOT_NONE` and
        :cmacro:`SIP_NO_CONVERTORS` flags.
    :return:
        a non-zero value if the object can be converted.


.. cfunction:: PyObject *sipClassName(PyObject *obj)

    This gets the class name of a wrapped instance as a Python string.  It
    comes with a reference.

    :param obj:
        the wrapped instance.
    :return:
        the name of the instance's class.

    .. note::
        This is deprecated from SIP v4.8.  Instead you should use the
        following::

            PyString_FromString(obj->ob_type->tp_name)


.. cfunction:: PyObject *sipConvertFromConstVoidPtr(const void *cpp)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will not be writeable and has no associated size.
    
    :param cpp:
        the memory address.
    :return:
        the :class:`sip.voidptr` object.


.. cfunction:: PyObject *sipConvertFromConstVoidPtrAndSize(const void *cpp, SIP_SSIZE_T size)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will not be writeable and can be used as an immutable buffer object.
    
    :param cpp:
        the memory address.
    :param size:
        the size associated with the address.
    :return:
        the :class:`sip.voidptr` object.


.. cfunction:: PyObject *sipConvertFromEnum(int eval, const sipTypeDef *td)

    This converts a named C/C++ ``enum`` to an instance of the corresponding
    generated Python type.
    
    :param eval:
        the enumerated value to convert.
    :param td:
        the enum's :ref:`generated type structure <ref-type-structures>`.
    :return:
        the Python object.


.. cfunction:: PyObject *sipConvertFromInstance(void *cpp, sipWrapperType *type, PyObject *transferObj)

    This converts a C structure or a C++ class instance to an instance of the
    corresponding generated Python type.

    :param cpp:
        the C/C++ instance.
    :param type:
        the type's :ref:`generated type object <ref-type-objects>`.
    :param transferObj:
        this controls the ownership of the returned value.
    :return:
        the Python object.

    If the C/C++ instance has already been wrapped then the result is a
    new reference to the existing class instance object.
    
    If *transferObj* is ``NULL`` and the instance has already been wrapped then
    the ownership is unchanged.
    
    If *transferObj* is ``NULL`` and the instance is newly wrapped then
    ownership will be with C/C++.
    
    If *transferObj* is ``Py_None`` then ownership is transferred to Python via
    a call to :cfunc:`sipTransferBack()`.
    
    Otherwise ownership is transferred to C/C++ and the instance associated
    with *transferObj* via a call to :cfunc:`sipTransferTo()`.
    
    The Python type is influenced by any applicable
    :directive:`%ConvertToSubClassCode` code.

    .. note::
        This is deprecated from SIP v4.8.  Instead you should use
        :cfunc:`sipConvertFromType()`.


sipConvertFromMappedType()
--------------------------

PyObject \*sipConvertFromMappedType(void \*cpp, const sipMappedType \*mt, PyObject \*transferObj)
    This converts a C structure or a C++ class instance wrapped as a mapped
    type to a Python object.  *cpp* is the C/C++ instance.  *mt* is the opaque
    structure returned by `sipFindMappedType()`_.  *transferObj* controls any
    ownership changes to *obj*.  If it is ``NULL`` then the ownership is
    unchanged.  If it is ``Py_None`` then ownership is transferred to Python
    via a call to `sipTransferBack()`_.  Otherwise ownership is transferred to
    C/C++ and the instance associated with the ``PyObject *`` argument via a
    call to `sipTransferTo()`_.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipConvertFromType()`_.


sipConvertFromNamedEnum()
-------------------------

PyObject \*sipConvertFromNamedEnum(int eval, PyTypeObject \*type)
    This converts a named C/C++ ``enum`` to an instance of the corresponding
    Python named enum type.  *eval* is the enumerated value to convert.  *type*
    is the generated Python type object (see `Generated Named Enum Type
    Objects`_).

    This is deprecated from SIP v4.8.  Instead you should use
    `sipConvertFromEnum()`_.


sipConvertFromNewInstance()
---------------------------

PyObject \*sipConvertFromNewInstance(void \*cpp, sipWrapperType \*type, PyObject \*transferObj)
    This converts a new C structure or a new C++ class instance to a Python
    class instance object.  *cpp* is the C/C++ instance.  *type* is the
    generated type corresponding to the C/C++ type.  *transferObj* controls the
    ownership of the returned value.  If it is ``NULL`` or ``Py_None`` then
    ownership will be with Python.  Otherwise ownership will be with C/C++ and
    the instance associated with *transferObj*.  The Python class is influenced
    by any applicable `%ConvertToSubClassCode`_ code.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipConvertFromNewType()`_.


sipConvertFromNewType()
-----------------------

PyObject \*sipConvertFromNewType(void \*cpp, const sipTypeDef \*td, PyObject \*transferObj)
    This converts a new C structure, C++ class or mapped type instance to a
    Python object.  *cpp* is the C/C++ instance.  *td* is the generated type
    structure corresponding to the C/C++ type.  *transferObj* controls the
    ownership of the returned value.  If it is ``NULL`` or ``Py_None`` then
    ownership will be with Python.  Otherwise ownership will be with C/C++ and
    the instance associated with *transferObj*.  The Python class is influenced
    by any applicable `%ConvertToSubClassCode`_ code.


sipConvertFromSequenceIndex()
-----------------------------

SIP_SSIZE_T sipConvertFromSequenceIndex(SIP_SSIZE_T idx, SIP_SSIZE_T len)
    This converts a Python sequence index (i.e. where a negative value refers
    to the offset from the end of the sequence) to a C/C++ array index.  If the
    index was out of range then a negative value is returned and a Python
    exception raised.  With versions of Python prior to v2.5 the result and the
    arguments have type ``int``.


sipConvertFromSliceObject()
---------------------------

int sipConvertFromSliceObject(PyObject \*slice, SIP_SSIZE_T length, SIP_SSIZE_T \*start, SIP_SSIZE_T \*stop, SIP_SSIZE_T \*step, SIP_SSIZE_T \*slicelength)
    This is a thin wrapper around the Python ``PySlice_GetIndicesEx()``
    function provided to make it easier to write handwritten code that is
    compatible with SIP v3.x and versions of Python earlier that v2.3.


sipConvertFromType()
--------------------

PyObject \*sipConvertFromType(void \*cpp, const sipTypeDef \*td, PyObject \*transferObj)
    This converts a C structure, a C++ class instance or a mapped type to a
    Python object.  *cpp* is the C/C++ instance.  If the instance has already
    been wrapped then the result is a new reference to the existing object.
    *td* is the generated type structure corresponding to the C/C++ type.
    *transferObj* controls the ownership of the returned value.  If the
    structure or class instance has already been wrapped then the result is a
    new reference to the existing object.  If it is ``NULL`` and the instance
    has already been wrapped then the ownership is unchanged.  If it is
    ``NULL`` and the instance is newly wrapped then ownership will be with
    C/C++.  If it is ``Py_None`` then ownership is transferred to Python via a
    call to `sipTransferBack()`_.  Otherwise ownership is transferred to C/C++
    and the instance associated with *transferObj* via a call to
    `sipTransferTo()`_.  The Python class is influenced by any applicable
    `%ConvertToSubClassCode`_ code.


sipConvertFromVoidPtr()
-----------------------

PyObject \*sipConvertFromVoidPtr(void \*cpp)
    This creates a ``sip.voidptr`` object for a memory address.  The object
    will be writeable but has no associated size.  *cpp* is the memory address.


sipConvertFromVoidPtrAndSize()
------------------------------

PyObject \*sipConvertFromVoidPtrAndSize(void \*cpp, SIP_SSIZE_T size)
    This creates a ``sip.voidptr`` object for a memory address.  The object
    will be writeable.  *cpp* is the memory address.  *size* is the size
    associated with the address.  The object can be used as a mutable buffer
    object.


sipConvertToInstance()
----------------------

void \*sipConvertToInstance(PyObject \*obj, sipWrapperType \*type, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure or C++ class
    assuming that a previous call to `sipCanConvertToInstance()`_ has been
    successful.  *obj* is the Python object.  *type* is the generated type
    corresponding to the C/C++ type returned.  It may be any class in the
    object's class hierarchy.  *transferObj* controls any ownership changes to
    *obj*.  If it is ``NULL`` then the ownership is unchanged.  If it is
    ``Py_None`` then ownership is transferred to Python via a call to
    `sipTransferBack()`_.  Otherwise ownership is transferred to C/C++ and
    *obj* associated with *transferObj* via a call to `sipTransferTo()`_.
    *flags* is any combination of the following values used to fine tune the
    check.

        - ``SIP_NOT_NONE`` causes the check to fail if *obj* is ``None``.

        - ``SIP_NO_CONVERTORS`` suppresses the use of of any
          `%ConvertToTypeCode`_ for *type*.

    If *state* is not ``NULL`` then the location it points to is set to
    describe the state of the returned C/C++ instance and is the value returned
    by any `%ConvertToTypeCode`_.  The calling code must then release the value
    at some point to prevent a memory leak by calling `sipReleaseInstance()`_.
    If there is an error then the location *iserr* points to is set to a
    non-zero value.  If it was initially a non-zero value then the conversion
    isn't attempted in the first place.  (This allows several calls to be made
    that share the same error flag so that it only needs to be tested once
    rather than after each call.)

    This is deprecated from SIP v4.8.  Instead you should use
    `sipConvertToType()`_


sipConvertToMappedType()
------------------------

void \*sipConvertToMappedType(PyObject \*obj, const sipMappedType \*mt, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure or C++
    class that is implemented as a mapped type assuming that a previous call to
    `sipCanConvertToMappedType()`_ has been successful.  *obj* is the Python
    object.  *mt* is the opaque structure returned by `sipFindMappedType()`_.
    *transferObj* controls any ownership changes to *obj*.  If it is ``NULL``
    then the ownership is unchanged.  If it is ``Py_None`` then ownership is
    transferred to Python via a call to `sipTransferBack()`_.  Otherwise
    ownership is transferred to C/C++ and *obj* associated with *transferObj*
    via a call to `sipTransferTo()`_.  *flags* is any combination of the
    following values used to fine tune the check.

        - ``SIP_NOT_NONE`` causes the check to fail if *obj* is ``None``.

    If *state* is not ``NULL`` then the location it points to is set to
    describe the state of the returned C/C++ instance and is the value returned
    by any `%ConvertToTypeCode`_.  The calling code must then release the value
    at some point to prevent a memory leak by calling
    `sipReleaseMappedType()`_.  If there is an error then the location *iserr*
    points to is set to a non-zero value.  If it was initially a non-zero value
    then the conversion isn't attempted in the first place.  (This allows
    several calls to be made that share the same error flag so that it only
    needs to be tested once rather than after each call.)

    This is deprecated from SIP v4.8.  Instead you should use
    `sipConvertToType()`_


sipConvertToType()
------------------

void \*sipConvertToType(PyObject \*obj, const sipTypeDef \*td, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure, C++ class or
    mapped type assuming that a previous call to `sipCanConvertToType()`_ has
    been successful.  *obj* is the Python object.  *td* is the generated type
    structure corresponding to the C/C++ type returned.  It may be any class in
    the object's class hierarchy.  *transferObj* controls any ownership changes
    to *obj*.  If it is ``NULL`` then the ownership is unchanged.  If it is
    ``Py_None`` then ownership is transferred to Python via a call to
    `sipTransferBack()`_.  Otherwise ownership is transferred to C/C++ and
    *obj* associated with *transferObj* via a call to `sipTransferTo()`_.
    *flags* is any combination of the following values used to fine tune the
    check.

        - ``SIP_NOT_NONE`` causes the check to fail if *obj* is ``None``.

        - ``SIP_NO_CONVERTORS`` suppresses the use of of any
          `%ConvertToTypeCode`_ for *td*.  It is ignored for mapped types.

    If *state* is not ``NULL`` then the location it points to is set to
    describe the state of the returned C/C++ instance and is the value returned
    by any `%ConvertToTypeCode`_.  The calling code must then release the value
    at some point to prevent a memory leak by calling `sipReleaseType()`_.  If
    there is an error then the location *iserr* points to is set to a non-zero
    value.  If it was initially a non-zero value then the conversion isn't
    attempted in the first place.  (This allows several calls to be made that
    share the same error flag so that it only needs to be tested once rather
    than after each call.)


sipConvertToVoidPtr()
---------------------

void \*sipConvertToVoidPtr(PyObject \*obj)
    This converts a Python object to a memory address.  ``obj`` may be
    ``Py_None``, a ``sip.voidptr`` or a PyCObject.  The memory address is
    returned.  ``PyErr_Occurred()`` must be used to determine if the conversion
    was successful.


sipExportSymbol()
-----------------

int sipExportSymbol(const char \*name, void \*sym)
    Python does not allow extension modules to directly access symbols in
    another extension module.  This exports a symbol, referenced by a name,
    that can subsequently be imported, using `sipImportSymbol()`_, by another
    module.  *name* is the name of the symbol and *sym* is its value.  Zero is
    returned if there was no error.  A negative value is returned if *name* is
    already associated with a symbol or there was some other error.


sipFindClass()
--------------

sipWrapperType \*sipFindClass(const char \*type)
    This returns a pointer to the generated type corresponding to a C/C++ type.
    *type* is the C/C++ declaration of the type.  ``NULL`` is returned if the
    C/C++ type doesn't exist.  The value of the pointer will not change and
    may be saved in a static cache.

    This is deprecated from SIP v4.8.  Instead you should use `sipFindType()`_.


sipFindMappedType()
-------------------

const sipMappedType \*sipFindMappedType(const char \*type)
    This returns a pointer to an opaque structure describing a mapped type.
    *type* is the C/C++ declaration of the type.  ``NULL`` is returned if the
    mapped type doesn't exist.  The value of the pointer will not change and
    may be saved in a static cache.

    This is deprecated from SIP v4.8.  Instead you should use `sipFindType()`_.


sipFindNamedEnum()
------------------

PyTypeObject \*sipFindNamedEnum(const char \*type)
    This returns a pointer to the generated type corresponding to a named C/C++
    enum.  *type* is the C/C++ declaration of the enum.  ``NULL`` is returned
    if the named C/C++ enum doesn't exist.  The value of the pointer will not
    change and may be saved in a static cache.

    This is deprecated from SIP v4.8.  Instead you should use `sipFindType()`_.


sipFindType()
-------------

const sipTypeDef \*sipFindType(const char \*type)
    This returns a pointer to the SIP generated type structure corresponding to
    a C/C++ type.  *type* is the C/C++ declaration of the type.  NULL is
    returned if the type doesn't exist.  The value of the pointer will not
    change and may be saved in a static cache.


sipForceConvertToInstance()
---------------------------

void \*sipForceConvertToInstance(PyObject \*obj, sipWrapperType \*type, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure or C++ class
    by calling `sipCanConvertToInstance()`_ and, if it is successfull, calling
    `sipConvertToInstance()`_.  See `sipConvertToInstance()`_ for a full
    description of the arguments.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipForceConvertToType()`_.


sipForceConvertToMappedType()
-----------------------------

void \*sipForceConvertToMappedType(PyObject \*obj, const sipMappedType \*mt, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure or C++ class
    which has been implemented as a mapped type by calling
    `sipCanConvertToMappedType()`_ and, if it is successfull, calling
    `sipConvertToMappedType()`_.  See `sipConvertToMappedType()`_ for a full
    description of the arguments.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipForceConvertToType()`_.


sipForceConvertToType()
-----------------------

void \*sipForceConvertToType(PyObject \*obj, const sipTypeDef \*td, PyObject \*transferObj, int flags, int \*state, int \*iserr)
    This converts a Python object to an instance of a C structure, C++ class or
    mapped type by calling `sipCanConvertToType()`_ and, if it is successfull,
    calling `sipConvertToType()`_.  See `sipConvertToType()`_ for a full
    description of the arguments.


sipFree()
---------

void sipFree(void \*mem)
    This returns an area of memory allocated by `sipMalloc()`_ to the heap.
    *mem* is a pointer to the area of memory.


sipGetPyObject()
----------------

PyObject \*sipGetPyObject(void \*cppptr, const sipTypeDef \*td)
    This returns a borrowed reference to the Python object for a C structure or
    C++ class instance.  If the structure or class instance hasn't been wrapped
    then ``NULL`` is returned (and no Python exception is raised).  *cppptr* is
    the pointer to the structure or class instance.  *td* is the generated type
    structure corresponding to the C/C++ type.


sipGetWrapper()
---------------

PyObject \*sipGetWrapper(void \*cppptr, sipWrapperType \*type)
    This returns a borrowed reference to the wrapped instance object for a C
    structure or C++ class instance.  If the structure or class instance
    hasn't been wrapped then ``NULL`` is returned (and no Python exception is
    raised).  *cppptr* is the pointer to the structure or class instance.
    *type* is the generated type corresponding to the C/C++ type.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipGetPyObject()`_.


sipImportSymbol()
-----------------

void \*sipImportSymbol(const char \*name)
    Python does not allow extension modules to directly access symbols in
    another extension module.  This imports a symbol, referenced by a name,
    that has previously been exported, using `sipExportSymbol()`_, by another
    module.  *name* is the name of the symbol.  The value of the symbol is
    returned if there was no error.  ``NULL`` is returned if there is no such
    symbol.


sipIntTypeClassMap
------------------

This C structure is used with `sipMapIntToClass()`_ to define a mapping
between integer based RTTI and `generated type objects`_.  The structure
elements are as follows.

int typeInt
    The integer RTTI.

sipWrapperType \*\*pyType.
    A pointer to the corresponding Python type object.

This is deprecated from SIP v4.8.


sipLong_AsUnsignedLong()
------------------------

unsigned long sipLong_AsUnsignedLong(PyObject \*obj)
    This function is a thin wrapper around PyLong_AsUnsignedLong() that works
    around a bug in Python v2.3.x and earlier when converting integer objects.


sipMalloc()
-----------

void \*sipMalloc(size_t nbytes)
    This allocates an area of memory of size *nytes* on the heap using the
    Python ``PyMem_Malloc()`` function.  If there was an error then ``NULL`` is
    returned and a Python exception raised.  See `sipFree()`_.


sipMapIntToClass()
------------------

sipWrapperType \*sipMapIntToClass(int type, const sipIntTypeClassMap \*map, int maplen)
    This is used in `%ConvertToSubClassCode`_ code as a convenient way of
    converting integer based RTTI to the corresponding Python type object.
    *type* is the RTTI.  *map* is the table of known RTTI and the corresponding
    type objects (see sipIntTypeClassMap_).  The entries in the table must be
    sorted in ascending order of RTTI.  *maplen* is the number of entries in
    the table.  The corresponding Python type object is returned, or ``NULL``
    if *type* wasn't in *map*.

    This is deprecated from SIP v4.8.


sipMapStringToClass()
---------------------

sipWrapperType \*sipMapStringToClass(char \*type, const sipStringTypeClassMap \*map, int maplen)
    This is used in `%ConvertToSubClassCode`_ code as a convenient way of
    converting ``'\0'`` terminated string based RTTI to the corresponding
    Python type object.  *type* is the RTTI.  *map* is the table of known RTTI
    and the corresponding type objects (see sipStringTypeClassMap_).  The
    entries in the table must be sorted in ascending order of RTTI.  *maplen*
    is the number of entries in the table.  The corresponding Python type
    object is returned, or ``NULL`` if *type* wasn't in *map*.

    This is deprecated from SIP v4.8.


sipParseResult()
----------------

int sipParseResult(int \*iserr, PyObject \*method, PyObject \*result, const char \*format, ...)
    This converts a Python object (usually returned by a method) to C/C++ based
    on a format string and associated values in a similar way to the Python
    ``PyArg_ParseTuple()`` function.  If there was an error then a negative
    value is returned and a Python exception is raised.  If *iserr* is not
    ``NULL`` then the location it points to is set to a non-zero value.
    *method* is the Python bound method that returned the *result* object.
    *format* is the string of format characters.

    This is normally called by handwritten code specified with the
    `%VirtualCatcherCode`_ directive with *method* being the supplied
    ``sipMethod`` and ``result`` being the value returned by
    `sipCallMethod()`_.

    If *format* begins and ends with parentheses then *result* must be a Python
    tuple and the rest of *format* is applied to the tuple contents.

    In the following description the first letter is the format character, the
    entry in parentheses is the Python object type that the format character
    will convert, and the entry in brackets are the types of the C/C++ values
    to be passed. 

    ``ae`` (object) [char \*]
        Convert a Python string-like object of length 1 to a C/C++ ``char``
        according to the encoding ``e``.  ``e`` can either be ``A`` for ASCII,
        ``L`` for Latin-1, or ``8`` for UTF-8.  For Python v2 the object may be
        either a string or a unicode object that can be encoded.  For Python v3
        the object may either be a bytes object or a string object that can be
        encoded.  An object that supports the buffer protocol may also be used.

    ``b`` (integer) [bool \*]
        Convert a Python integer to a C/C++ ``bool``.

    ``c`` (string/bytes) [char \*]
        Convert a Python v2 string object or a Python v3 bytes object of length
        1 to a C/C++ ``char``.

    ``d`` (float) [double \*]
        Convert a Python floating point number to a C/C++ ``double``.

    ``e`` (integer) [enum \*]
        Convert a Python integer to an anonymous C/C++ ``enum``.

    ``f`` (float) [float \*]
        Convert a Python floating point number to a C/C++ ``float``.

    ``g`` (string/bytes) [const char \*\*, SIP_SSIZE_T \*]
        Convert a Python v2 string object or a Python v3 bytes object to a
        C/C++ character array and its length.  If the Python object is
        ``Py_None`` then the array and length are ``NULL`` and zero
        respectively.

    ``h`` (integer) [short \*]
        Convert a Python integer to a C/C++ ``short``.

    ``i`` (integer) [int \*]
        Convert a Python integer to a C/C++ ``int``.

    ``l`` (long) [long \*]
        Convert a Python long to a C/C++ ``long``.

    ``m`` (long) [unsigned long \*]
        Convert a Python long to a C/C++ ``unsigned long``.

    ``n`` (long) [long long \*]
        Convert a Python long to a C/C++ ``long long``.

    ``o`` (long) [unsigned long long \*]
        Convert a Python long to a C/C++ ``unsigned long long``.

    ``s`` (string/bytes) [const char \*\*]
        Convert a Python v2 string object or a Python v3 bytes object to a
        C/C++ ``'\0'`` terminated string.  If the Python object is ``Py_None``
        then the string is ``NULL``.

        This is deprecated from SIP v4.8.  Instead you should use ``B``.

    ``t`` (long) [unsigned short \*]
        Convert a Python long to a C/C++ ``unsigned short``.

    ``u`` (long) [unsigned int \*]
        Convert a Python long to a C/C++ ``unsigned int``.

    ``w`` (unicode/string) [wchar_t \*]
        Convert a Python v2 unicode object or a Python v3 string object of
        length 1 to a C/C++ wide character.

    ``x`` (unicode/string) [wchar_t \*\*]
        Convert a Python v2 unicode object or a Python v3 string object to a
        C/C++ ``L'\0'`` terminated wide character string.  If the Python object
        is ``Py_None`` then the string is ``NULL``.

    ``Ae`` (object) [int, const char \*\*]
        Convert a Python string-like object to a C/C++ ``'\0'`` terminated
        string according to the encoding ``e``.  ``e`` can either be ``A`` for
        ASCII, ``L`` for Latin-1, or ``8`` for UTF-8.  If the Python object is
        ``Py_None`` then the string is ``NULL``.  The integer uniquely
        identifies the object in the context defined by the ``S`` format
        character and allows an extra reference to the object to be kept to
        ensure that the string remains valid.  For Python v2 the object may be
        either a string or a unicode object that can be encoded.  For Python v3
        the object may either be a bytes object or a string object that can be
        encoded.  An object that supports the buffer protocol may also be used.

    ``B`` (string/bytes) [int, const char \*\*]
        Convert a Python v2 string object or a Python v3 bytes object to a
        C/C++ ``'\0'`` terminated string.  If the Python object is ``Py_None``
        then the string is ``NULL``.  The integer uniquely identifies the
        object in the context defined by the ``S`` format character and allows
        an extra reference to the object to be kept to ensure that the string
        remains valid.

    ``Cf`` (wrapped class) [sipWrapperType \*, int \*, void \*\*]
        Convert a Python object to a C structure or a C++ class instance and
        return its state as described in `sipConvertToInstance()`_.  ``f`` is a
        combination of the following flags encoded as an ASCII character by
        adding ``0`` to the combined value:

            0x01 disallows the conversion of ``Py_None`` to ``NULL``

            0x02 implements the `Factory`_ annotation

            0x04 suppresses the return of the state of the returned C/C++
                 instance.  Note that the ``int *`` used to return the state is
                 not passed if this flag is specified.

        This is deprecated from SIP v4.8.  Instead you should use ``Df``.

    ``Df`` (wrapped instance) [const sipTypeDef \*, int \*, void \*\*]
        Convert a Python object to a C structure, C++ class or mapped type
        instance and return its state as described in `sipConvertToType()`_.
        ``f`` is a combination of the following flags encoded as an ASCII
        character by adding ``0`` to the combined value:

            0x01 disallows the conversion of ``Py_None`` to ``NULL``

            0x02 implements the `Factory`_ annotation

            0x04 suppresses the return of the state of the returned C/C++
                 instance.  Note that the ``int *`` used to return the state is
                 not passed if this flag is specified.

    ``E`` (wrapped enum) [PyTypeObject \*, enum \*]
        Convert a Python named enum type to the corresponding C/C++ ``enum``.

        This is deprecated from SIP v4.8.  Instead you should use ``F``.

    ``F`` (wrapped enum) [sipTypeDef \*, enum \*]
        Convert a Python named enum type to the corresponding C/C++ ``enum``.

    ``G`` (unicode) [wchar_t \*\*, SIP_SSIZE_T \*]
        Convert a Python unicode object to a C/C++ wide character array and its
        length.  If the Python object is ``Py_None`` then the array and length
        are ``NULL`` and zero respectively.

    ``N`` (object) [PyTypeObject \*, PyObject \*\*]
        A Python object is checked to see if it is a certain type and then
        returned without any conversions.  The reference count is incremented.
        The Python object may be ``Py_None``.

    ``O`` (object) [PyObject \*\*]
        A Python object is returned without any conversions.  The reference
        count is incremented.

    ``S`` [sipSimpleWrapper \*]
        This format character, if used, must be the first.  It is used with
        other format characters to define a context and doesn't itself convert
        an argument.

    ``T`` (object) [PyTypeObject \*, PyObject \*\*]
        A Python object is checked to see if it is a certain type and then
        returned without any conversions.  The reference count is incremented.
        The Python object may not be ``Py_None``.

    ``V`` (sip.voidptr) [void \*]
        Convert a Python ``sip.voidptr`` object to a C/C++ ``void *``.

    ``Z`` (object) []
        Check that a Python object is ``Py_None``.  No value is returned.


sipRegisterAttributeGetter()
----------------------------

int sipRegisterAttributeGetter(const sipTypeDef \*td, sipAttrGetterFunc getter)
    This registers a handler that will called just before SIP needs to get an
    attribute from a wrapped type's dictionary for the first time.  The handler
    must then populate the type's dictionary with any lazy attributes.  -1 is
    returned if there was an error registering the handler, 0 is returned
    otherwise.

    *td* is an optional generated type definition which means that the handler
    will only be called for types with that type or sub-classed from it.  If it
    is ``NULL`` then the handler will be called for all types.

    *getter* is the handler and it has the following signature.

    int handler(const sipTypeDef \*td, PyObject \*dict)
        *td* is the generated type definition of the type whose dictionary is
        to be populated.

        *dict* is the dictionary to be populated.

        -1 is returned if there is an error, 0 is returned otherwise.

    See the section `Lazy Type Attributes`_ for more details.


sipRegisterPyType()
-------------------

int sipRegisterPyType(PyTypeObject \*type)
    This registers a type object that can be used as the meta-type or
    super-type of a wrapped C++ type.  *type* is the type.  -1 is returned if
    there was an error registering the type, 0 is returned otherwise.

    See the section `Types and Meta-types`_ for more details.


sipReleaseInstance()
--------------------

void sipReleaseInstance(void \*cpp, sipWrapperType \*type, int state)
    This destroys a wrapped C/C++ instance if it was a temporary instance.  It
    is called after a call to either `sipConvertToInstance()`_ or
    `sipForceConvertToInstance()`_.  *cpp* is the wrapped C/C++ instance.
    *type* is the generated type corresponding to *cpp*.  *state* describes the
    state of the instance.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipReleaseType()`_.


sipReleaseMappedType()
----------------------

void sipReleaseMappedType(void \*cpp, const sipMappedType \*mt, int state)
    This destroys a wrapped C/C++ mapped type if it was a temporary instance.
    It is called after a call to either `sipConvertToMappedType()`_ or
    `sipForceConvertToMappedType()`_.  *cpp* is the wrapped C/C++ instance.
    *mt* is the opaque structure returned by `sipFindMappedType()`_.  *state*
    describes the state of the instance.

    This is deprecated from SIP v4.8.  Instead you should use
    `sipReleaseType()`_.


sipReleaseType()
----------------

void sipReleaseType(void \*cpp, const sipTypeDef \*td, int state)
    This destroys a wrapped C/C++ or mapped type instance if it was a temporary
    instance.  It is called after a call to either `sipConvertToType()`_ or
    `sipForceConvertToType()`_.  *cpp* is the wrapped C/C++ instance.  *td* is
    the generated type structure.  *state* describes the state of the instance.


sipResolveTypedef()
-------------------

const char \*sipResolveTypedef(const char \*name)
    If *name* refers to a C/C++ typedef then the value of that typedef is
    returned, otherwise ``NULL`` is returned.


sipSimpleWrapper
----------------

This is a C structure that represents a Python wrapped instance whose type is
``sip.simplewrapper``.  It is an extension of the ``PyObject`` structure and so
may be safely cast to it.

It includes a member called ``user`` which is of type ``PyObject *``.  This can
be used for any purpose by handwritten code and will automatically be garbage
collected at the appropriate time.


sipSimpleWrapper_Type
---------------------

This is a pointer to a ``PyTypeObject`` structure that is the type of a
`sipSimpleWrapper`_ structure and is the C implementation of
``sip.simplewrapper``.  It may be safely cast to `sipWrapperType`_.


sipStringTypeClassMap
---------------------

This C structure is used with `sipMapStringToClass()`_ to define a mapping
between ``'\0'`` terminated string based RTTI and `generated type objects`_.
The structure elements are as follows.

char \*typeString
    The ``'\0'`` terminated string RTTI.

sipWrapperType \*\*pyType.
    A pointer to the corresponding Python type object.

This is deprecated from SIP v4.8.


sipTransferBack()
-----------------

void sipTransferBack(PyObject \*obj)
    This transfers ownership of a Python wrapped instance to Python (see
    `Ownership of Objects`_).  *obj* is the wrapped instance.  In addition,
    any association of the instance with regard to the cyclic garbage
    collector with another instance is removed.


sipTransferBreak()
------------------

void sipTransferBreak(PyObject \*obj)
    Any association of a Python wrapped instance with regard to the cyclic
    garbage collector with another instance is removed.  *obj* is the wrapped
    instance.  Ownership of the instance should be with C++.


sipTransferTo()
---------------

void sipTransferTo(PyObject \*obj, PyObject \*owner)
    This transfers ownership of a Python wrapped instance to C++ (see
    `Ownership of Objects`_).  *obj* is the wrapped instance.  *owner* is an
    optional wrapped instance that *obj* becomes associated with with regard
    to the cyclic garbage collector.  If *owner* is ``NULL`` then no such
    association is made.  If *owner* is the same value as *obj* then any
    reference cycles involving *obj* can never be detected or broken by the
    cyclic garbage collector.  Responsibility for calling the C++ instance's
    destructor is always transfered to C++.


sipTypeAsPyTypeObject()
-----------------------

PyTypeObject \*sipTypeAsPyTypeObject(sipTypeDef \*td)
    This returns a pointer to the Python type object that SIP creates from a
    generated type structure.  *td* is the type structure.

    If the type structure refers to a C structure or C++ class then it may be
    safely cast to a `sipWrapperType`_.

    If the type structure refers to a mapped type then ``NULL`` will be
    returned.


sipTypeFromPyTypeObject()
-------------------------

const sipTypeDef \*sipTypeFromPyTypeObject(PyTypeObject \*py_type)
    This returns a pointer to the SIP generated type structure for a Python
    type.  *py_type* is the Python type object.

    If the Python type doesn't correspond to a SIP generated type then ``NULL``
    will be returned.


sipTypeIsClass()
----------------

int sipTypeIsClass(sipTypeDef \*td)
    This returns a non-zero value if the generated type structure refers to a
    C structure or C++ class.  *td* is the type structure.


sipTypeIsEnum()
---------------

int sipTypeIsEnum(sipTypeDef \*td)
    This returns a non-zero value if the generated type structure refers to a
    named enum.  *td* is the type structure.


sipTypeIsMapped()
-----------------

int sipTypeIsMapped(sipTypeDef \*td)
    This returns a non-zero value if the generated type structure refers to a
    mapped type.  *td* is the type structure.


sipTypeIsNamespace()
--------------------

int sipTypeIsNamespace(sipTypeDef \*td)
    This returns a non-zero value if the generated type structure refers to a
    C++ namespace.  *td* is the type structure.


sipTypeName()
-------------

const char \*sipTypeName(const sipTypeDef \*td)
    This returns the C/C++ name of a SIP generated type.  *td* is the type
    structure.


sipTypeScope()
--------------

const sipTypeDef \*sipTypeScope(const sipTypeDef \*td)
    This returns the SIP generated type structure of the scope of another
    SIP generated type structure.  ``NULL`` will be returned if the type has no
    scope.


sipVoidPtr_Type
---------------

This is a pointer to a ``PyTypeObject`` structure that is the type of a
``PyObject`` structure that is used to wrap a ``void *``.


sipWrapper
----------

This is a C structure that represents a Python wrapped instance whose type is
``sip.wrapper``.  It is an extension of the `sipSimpleWrapper`_ and
``PyObject`` structures and so may be safely cast to both.


sipWrapper_Check()
------------------

int sipWrapper_Check(PyObject \*obj)
    This returns a non-zero value if a Python object is a wrapped instance.
    *obj* is the Python object.

    This is deprecated from SIP v4.8.  Instead you should use the following::

        PyObject_TypeCheck(obj, sipWrapper_Type)


sipWrapper_Type
---------------

This is a pointer to a ``PyTypeObject`` structure that is the type of a
`sipWrapper`_ structure and is the C implementation of ``sip.wrapper``.  It may
be safely cast to `sipWrapperType`_.


sipWrapperType
--------------

This is a C structure that represents a SIP generated type object.  It is an
extension of the ``PyTypeObject`` structure (which is itself an extension of
the ``PyObject`` structure) and so may be safely cast to ``PyTypeObject`` (and
``PyObject``).


sipWrapperType_Type
-------------------

This is a pointer to a ``PyTypeObject`` structure that is the type of a
`sipWrapperType`_ structure and is the C implementation of ``sip.wrappertype``.


.. _ref-type-structures:

Generated Type Structures
-------------------------

SIP generates an opaque type structure for each C structure, C++ class, C++
namespace, named enum or mapped type being wrapped.  These are
:ctype:`sipTypeDef` structures and are used extensively by the SIP API.

The names of these structure are prefixed by ``sipType_``.

For those structures that correspond to C structures, C++ classes, C++
namespaces or named enums the remaining part of the name is the fully
qualified name of the structure, class, namespace or enum name.  Any ``::``
scope separators are replaced by an underscore.  For example, the type object
for class ``Klass`` is ``sipType_Klass``.

For those structure that correspond to mapped types the remaining part of the
name is generated by SIP.  The only way for handwritten code to obtain a
pointer to a structure for a mapped type is to use :cfunc:`sipFindType()`.

The type structures of all imported types are available to handwritten code.


.. _ref-type-objects:

Generated Type Objects
----------------------

SIP generates a :ctype:`sipWrapperType` type object for each C structure or
C++ class being wrapped.

These objects are named with the structure or class name prefixed by
``sipClass_``.  For example, the type object for class ``Klass`` is
``sipClass_Klass``.

.. note::
    Using these names is deprecated from SIP v4.8.  Instead use the
    corresponding generated type structure (see :ref:`ref-type-structures`) and
    :cfunc:`sipTypeAsPyTypeObject()`.


.. _ref-enum-type-objects:

Generated Named Enum Type Objects
---------------------------------

SIP generates a type object for each named enum being wrapped.  These are
PyTypeObject structures.  (Anonymous enums are wrapped as Python integers.)

These objects are named with the fully qualified enum name (i.e. including any
enclosing scope) prefixed by ``sipEnum_``.  For example, the type object for
enum ``Enum`` defined in class ``Klass`` is ``sipEnum_Klass_Enum``.

.. note::
    Using these names is deprecated from SIP v4.8.  Instead use the
    corresponding generated type structure (see :ref:`ref-type-structures`) and
    :cfunc:`sipTypeAsPyTypeObject()`.


.. _ref-derived-classes:

Generated Derived Classes
-------------------------

For most C++ classes being wrapped SIP generates a derived class with the same
name prefixed by ``sip``.  For example, the derived class for class ``Klass``
is ``sipKlass``.

If a C++ class doesn't have any virtual or protected methods in it or any of
it's super-class hierarchy, or does not emit any Qt signals, then a derived
class is not generated.

Most of the time handwritten code should ignore the derived classes.  The only
exception is that handwritten constructor code specified using the
:directive:`%MethodCode` directive should call the derived class's constructor
(which has the same C++ signature) rather then the wrapped class's constructor.


Generated Exception Objects
---------------------------

SIP generates a Python object for each exception defined with the
:directive:`%Exception` directive.

These objects are named with the fully qualified exception name (i.e. including
any enclosing scope) prefixed by ``sipException_``.  For example, the type
object for enum ``Except`` defined in class ``Klass`` is
``sipException_Klass_Except``.

The objects of all imported exceptions are available to handwritten code.
