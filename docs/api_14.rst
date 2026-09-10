API v14 for Handwritten Code
============================

In this section we describe the v14 API, provided by the :mod:`sip` module,
that can be used by handwritten code in specification files.

In order to be able to support multiple interpreters and free-threading ABI v14
uses modern Python API calls to create and manage extension modules.  In
particular this means using multi-phase initialisation and using the module
state to store module-specific data.  Older ABIs used global variables which
made it very difficult, if not impossible, to have a module imported more than
once.

As a consequence many API calls now require a pointer to a mutable opaque
:c:type:`sipModuleState` structure as their first argument.  In order to make
porting from older APIs easier, this argument is handled automatically and can
be ignored when API calls are made directly from handwritten code specified in
one of the code directives.

However if you are making API calls from helper functions you have written then
you must explicitly pass the pointer to the module state to your helper
functions and the pointer must be called ``sipMS``.


API Reference
-------------

.. c:macro:: SIP_ABI_MAJOR_VERSION

    This is a C preprocessor symbol that defines the major number of the SIP
    ABI.


.. c:macro:: SIP_ABI_MINOR_VERSION

    This is a C preprocessor symbol that defines the minor number of the SIP
    ABI.


.. c:macro:: SIP_BLOCK_THREADS

    This is a C preprocessor macro that will make sure that the current thread
    has an attached thread state.  Python API calls must only be made when a
    thread state is attached.  There must be a corresponding
    :c:macro:`SIP_UNBLOCK_THREADS` at the same lexical scope.


.. c:macro:: SIP_NO_CONVERTORS

    This is a flag used by various type convertors that suppresses the use of a
    type's :directive:`%ConvertToTypeCode`.


.. c:macro:: SIP_NOT_NONE

    This is a flag used by various type convertors that causes the conversion
    to fail if the Python object being converted is ``Py_None``.


.. c:macro:: SIP_NULLPTR

    This is a C preprocessor macro that can be used instead of ``NULL`` or
    ``nullptr``.  It ensures the correct value is used depending on whether C
    or C++ is being generated and which language standard the compiler
    supports.


.. c:macro:: SIP_OWNS_MEMORY

    This is a flag used by various array constructors that species that the
    array owns the memory that holds the array's contents.


.. c:macro:: SIP_PROTECTED_IS_PUBLIC

    This is a C preprocessor symbol that is defined automatically by the build
    system to specify that the generated code is being compiled with
    ``protected`` redefined as ``public``.  This allows handwritten code to
    determine if the generated helper functions for accessing protected C++
    functions are available (see :directive:`%MethodCode`).


.. c:macro:: SIP_READ_ONLY

    This is a flag used by various array constructors that species that the
    array is read-only.


.. c:function:: void SIP_RELEASE_GIL(sip_gilstate_t sipGILState)

    .. note::
        This is provided to ease the support for multiple versions of the ABI.
        Code targeting ABI v14 only should use :c:func:`PyThreadState_Release`
        instead.

    This is called from the handwritten code specified with the
    :directive:`VirtualErrorHandler` in order to release the attached thread
    state prior to changing the execution path (e.g. by throwing a C++
    exception).  It should not be called under any other circumstances.

    :param sipGILState:
        an opaque value provided to the handwritten code by SIP.


.. c:macro:: SIP_UNBLOCK_THREADS

    This is a C preprocessor macro that will restore the thread state of the
    current thread to the state it was prior to the corresponding
    :c:macro:`SIP_BLOCK_THREADS`.


.. c:macro:: SIP_VERSION

    This is a C preprocessor symbol that defines the SIP version number
    represented as a 3 part hexadecimal number (e.g. v6.0.0 is represented as
    ``0x060000``).


.. c:macro:: SIP_VERSION_STR

    This is a C preprocessor symbol that defines the SIP version number
    represented as a string.  For development versions it will contain
    ``.dev``.


.. c:function:: sipErrorState sipBadCallableArg(int arg_nr, PyObject *arg)

    This is called from :directive:`%MethodCode` to raise a Python exception
    when an argument to a function, a C++ constructor or method is found to
    have an unexpected type.  This should be used when the
    :directive:`%MethodCode` does additional type checking of the supplied
    arguments.

    :param arg_nr:
        the number of the argument.  Arguments are numbered from 0 but are
        numbered from 1 in the detail of the exception.
    :param arg:
        the argument.
    :return:
        the value that should be assigned to ``sipError``.


.. c:function:: void sipBadCatcherResult(PyObject *method)

    This raises a Python exception when the result of a Python reimplementation
    of a C++ method doesn't have the expected type.  It is normally called by
    handwritten code specified with the :directive:`%VirtualCatcherCode`
    directive.

    :param method:
        the Python method and would normally be the supplied ``sipMethod``.


.. c:function:: void sipBadLengthForSlice(Py_ssize_t seqlen, Py_ssize_t slicelen)

    This raises a Python exception when the length of a slice object is
    inappropriate for a sequence-like object.  It is normally called by
    handwritten code specified for :meth:`__setitem__` methods.

    :param seqlen:
        the length of the sequence.
    :param slicelen:
        the length of the slice.


.. c:function:: PyObject *sipBuildResult(int *is_err_p, const char *format, ...)

    This creates a Python object based on a format string and associated
    values in a similar way to the Python :c:func:`Py_BuildValue` function.

    :param is_err_p:
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
        Convert a C/C++ ``char`` to a Python ``str`` object.

    ``b`` (boolean) [bool]
        Convert a C/C++ ``bool`` to a Python boolean.

    ``c`` (string/bytes) [char]
        Convert a C/C++ ``char`` to a Python ``bytes`` object.

    ``d`` (float) [double]
        Convert a C/C++ ``double`` to a Python floating point number.

    ``e`` (integer) [enum]
        Convert an anonymous C/C++ ``enum`` to a Python integer.

    ``f`` (float) [float]
        Convert a C/C++ ``float`` to a Python floating point number.

    ``g`` (string/bytes) [char \*, :c:type:`Py_ssize_t`]
        Convert a C/C++ character array and its length to a Python ``bytes``
        object.  If the array is ``NULL`` then the length is ignored and the
        result is ``Py_None``.

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

    ``r`` (wrapped instance) [*type* \*, :c:type:`Py_ssize_t`, :c:type:`sipTypeID`]
        Convert an array of C structures, C++ classes or mapped type instances
        to a Python tuple.  Note that copies of the array elements are made.

    ``s`` (string/bytes) [char \*]
        Convert a C/C++ ``'\0'`` terminated string to a Python ``bytes``
        object.  If the string pointer is ``NULL`` then the result is
        ``Py_None``.

    ``t`` (long) [unsigned short]
        Convert a C/C++ ``unsigned short`` to a Python long.

    ``u`` (long) [unsigned int]
        Convert a C/C++ ``unsigned int`` to a Python long.

    ``w`` (unicode/string) [wchar_t]
        Convert a C/C++ wide character to a Python ``str`` object.

    ``x`` (unicode/string) [wchar_t \*]
        Convert a C/C++ ``L'\0'`` terminated wide character string to a Python
        ``str`` object.  If the string pointer is ``NULL`` then the result is
        ``Py_None``.

    ``A`` (string) [char \*]
        Convert a C/C++ ``'\0'`` terminated string to a Python ``str`` object.
        If the string pointer is ``NULL`` then the result is ``Py_None``.

    ``D`` (wrapped instance) [*type* \*, :c:type:`sipTypeID`, PyObject \*]
        Convert a C structure, C++ class or mapped type instance to a Python
        object.  If the instance has already been wrapped then the result is a
        new reference to the existing object.  Ownership of the instance is
        determined by the :c:type:`PyObject` argument.  If it is ``NULL`` and
        the instance has already been wrapped then the ownership is unchanged.
        If it is ``NULL`` and the instance is newly wrapped then ownership will
        be with C/C++.  If it is ``Py_None`` then ownership is transferred to
        Python via a call to :c:func:`sipTransferBack`.  Otherwise ownership
        is transferred to C/C++ and the instance associated with the
        :c:type:`PyObject` argument via a call to :c:func:`sipTransferTo`.  The
        Python class is influenced by any applicable
        :directive:`%ConvertToSubClassCode` code.

    ``F`` (wrapped enum) [enum, :c:type:`sipTypeID`]
        Convert a named C/C++ ``enum`` to an instance of the corresponding
        Python named enum type.

    ``G`` (unicode) [wchar_t \*, :c:type:`Py_ssize_t`]
        Convert a C/C++ wide character array and its length to a Python unicode
        object.  If the array is ``NULL`` then the length is ignored and the
        result is ``Py_None``.

    ``L`` (integer) [char]
        Convert a C/C++ ``char`` to a Python integer.

    ``M`` (long) [unsigned char]
        Convert a C/C++ ``unsigned char`` to a Python long.

    ``N`` (wrapped instance) [*type* \*, :c:type:`sipTypeID`, PyObject \*]
        Convert a new C structure, C++ class or mapped type instance to a
        Python object.  Ownership of the instance is determined by the
        :c:type:`PyObject` argument.  If it is ``NULL`` and the instance has
        already been wrapped then the ownership is unchanged.  If it is
        ``NULL`` or ``Py_None`` then ownership will be with Python.  Otherwise
        ownership will be with C/C++ and the instance associated with the
        :c:type:`PyObject` argument.  The Python class is influenced by any
        applicable :directive:`%ConvertToSubClassCode` code.

    ``R`` (object) [PyObject \*]
        The result is value passed without any conversions.  The reference
        count is unaffected, i.e. a reference is taken.

    ``S`` (object) [PyObject \*]
        The result is value passed without any conversions.  The reference
        count is incremented.

    ``V`` (sip.voidptr) [void \*]
        Convert a C/C++ ``void *`` to a Python :class:`sip.voidptr` object.

    ``z`` (object) [const char \*, void \*]
        Convert a C/C++ ``void *`` to a Python named capsule object.

    ``=`` (long) [size_t]
        Convert a C/C++ ``size_t`` to a Python long.


.. c:function:: PyObject *sipCallMethod(int *is_err_p, PyObject *method, const char *format, ...)

    This calls a Python method passing a tuple of arguments based on a format
    string and associated values in a similar way to the Python
    :c:func:`PyObject_CallObject` function.

    :param is_err_p:
        if this is not ``NULL`` then the location it points to is set to a
        non-zero value if there was an error.
    :param method:
        the Python bound method to call.
    :param format:
        the string of format characters (see :c:func:`sipBuildResult`).
    :return:
        If there was an error then ``NULL`` is returned and a Python exception
        is raised.

    It is normally called by handwritten code specified with the
    :directive:`%VirtualCatcherCode` directive with method being the supplied
    ``sipMethod``.


.. c:function:: int sipCanConvertToType(PyObject *obj, sipTypeID type_id, int flags)

    This checks if a Python object can be converted to an instance of a C
    structure, C++ class or mapped type.

    :param obj:
        the Python object.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param flags:
        any combination of the :c:macro:`SIP_NOT_NONE` and
        :c:macro:`SIP_NO_CONVERTORS` flags.
    :return:
        a non-zero value if the object can be converted.


.. c:type:: sipCFunctionDef

    This C structure is used with :c:func:`sipGetCFunction` and encapsulates
    the components parts of a Python C function.  The structure elements are as
    follows.

    .. c:member:: PyMethodDef *cf_function

        The C function.

    .. c:member:: PyObject *cf_self

        The optional bound object.


.. c:function:: PyObject *sipConvertFromConstVoidPtr(const void *cpp)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will not be writeable and has no associated size.

    :param cpp:
        the memory address.
    :return:
        the :class:`sip.voidptr` object.


.. c:function:: PyObject *sipConvertFromConstVoidPtrAndSize(const void *cpp, Py_ssize_t size)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will not be writeable and can be used as an immutable buffer object.

    :param cpp:
        the memory address.
    :param size:
        the size associated with the address.
    :return:
        the :class:`sip.voidptr` object.


.. c:function:: PyObject *sipConvertFromEnum(void *eval_p, sipTypeID type_id)

    This converts a named C/C++ ``enum`` to a Python object.

    :param eval_p:
        a pointer to the enumerated value to convert.
    :param type_id:
        the enum's :ref:`generated type specification <ref-type-specs>`.
    :return:
        the Python object.


.. c:function:: PyObject *sipConvertFromNewPyType(void *cpp, PyTypeObject *py_type, sipWrapper *owner, sipSimpleWrapper **self_p, const char *format, ...)

    This converts a new C structure or a C++ class instance to an instance of a
    corresponding Python type (as opposed to the corresponding generated Python
    type).  This is useful when the C/C++ library provides some sort of
    mechanism whereby handwritten code has some control over the exact type of
    structure or class being created.  Typically it would be used to create an
    instance of the generated derived class which would then allow Python
    re-implementations of C++ virtual methods to function properly.

    :param cpp:
        the C/C++ instance.
    :param py_type:
        the Python type object.  This is called to create the Python object and
        is passed the arguments defined by the string of format characters.
    :param owner:
        is the optional owner of the Python object.
    :param self_p:
        is an optional pointer to the ``sipPySelf`` instance variable of the
        C/C++ instance if that instance's type is a generated derived class.
        Otherwise it should be ``NULL``.
    :param format:
        the string of format characters (see :c:func:`sipBuildResult`).
    :return:
        the Python object.  If there was an error then ``NULL`` is returned and
        a Python exception is raised.


.. c:function:: PyObject *sipConvertFromNewType(void *cpp, sipTypeID type_id, PyObject *transferObj)

    This converts a new C structure or a C++ class instance to an instance of
    the corresponding generated Python type.  Note that care should be taken to
    ensure that the instance really is new.

    :param cpp:
        the C/C++ instance.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param transferObj:
        this controls the ownership of the returned value.
    :return:
        the Python object.

    If *transferObj* is ``NULL`` or ``Py_None`` then ownership will be with
    Python.
    
    Otherwise ownership will be with C/C++ and the instance associated with
    *transferObj*.
    
    The Python type is influenced by any applicable
    :directive:`%ConvertToSubClassCode` code.


.. c:function:: Py_ssize_t sipConvertFromSequenceIndex(Py_ssize_t idx, Py_ssize_t len)

    This converts a Python sequence index (i.e. where a negative value refers
    to the offset from the end of the sequence) to a C/C++ array index.  If the
    index was out of range then a negative value is returned and a Python
    exception raised.

    :param idx:
        the sequence index.
    :param len:
        the length of the sequence.
    :return:
        the unsigned array index.


.. c:function:: int sipConvertFromSliceObject(PyObject *slice, Py_ssize_t length, Py_ssize_t *start, Py_ssize_t *stop, Py_ssize_t *step, Py_ssize_t *slicelength)

    This is a thin wrapper around Python's :c:func:`PySlice_Unpack` and
    :c:func:`PySlice_AdjustIndices` functions.


.. c:function:: PyObject *sipConvertFromType(void *cpp, sipTypeID type_id, PyObject *transferObj)

    This converts a C structure or a C++ class instance to an instance of the
    corresponding generated Python type.

    :param cpp:
        the C/C++ instance.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param transferObj:
        this controls the ownership of the returned value.
    :return:
        the Python object.

    If the C/C++ instance has already been wrapped then the result is a new
    reference to the existing object.

    If *transferObj* is ``NULL`` and the instance has already been wrapped then
    the ownership is unchanged.
    
    If *transferObj* is ``NULL`` and the instance is newly wrapped then
    ownership will be with C/C++.
    
    If *transferObj* is ``Py_None`` then ownership is transferred to Python via
    a call to :c:func:`sipTransferBack`.
    
    Otherwise ownership is transferred to C/C++ and the instance associated
    with *transferObj* via a call to :c:func:`sipTransferTo`.
    
    The Python class is influenced by any applicable
    :directive:`%ConvertToSubClassCode` code.


.. c:function:: PyObject *sipConvertFromVoidPtr(void *addr)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will be writeable but has no associated size.

    :param addr:
        the memory address.
    :return:
        the :class:`sip.voidptr` object.


.. c:function:: PyObject *sipConvertFromVoidPtrAndSize(void *addr, Py_ssize_t size)

    This creates a :class:`sip.voidptr` object for a memory address.  The
    object will be writeable and can be used as a mutable buffer object.
    
    :param addr:
        the memory address.
    :param size:
        the size associated with the address.
    :return:
        the :class:`sip.voidptr` object.


.. c:function:: PyObject *sipConvertToArray(void *data, const char *format, Py_ssize_t len, int flags)

    This converts a one dimensional array of fundamental types to a
    :class:`sip.array` object.

    An array is very like a Python :class:`memoryview` object.  The underlying
    memory is not copied and may be modified in situ.  Arrays support the
    buffer protocol and so can be passed to other modules, again without the
    underlying memory being copied.

    :param data:
        the address of the start of the C/C++ array.
    :param format:
        the format, as defined by the :mod:`struct` module, of an array
        element.  At the moment only ``b`` (char), ``B`` (unsigned char),
        ``h`` (short), ``H`` (unsigned short), ``i`` (int),
        ``I`` (unsigned int), ``f`` (float) and ``d`` (double) are supported.
    :param len:
        the number of elements in the array.
    :param readonly:
        is non-zero if the array is read-only.
    :param flags:
        any combination of the :c:macro:`SIP_READ_ONLY` and
        :c:macro:`SIP_OWNS_MEMORY` flags.
    :return:
        the :class:`sip.array` object.


.. c:function:: bool sipConvertToBool(PyObject *obj)

    This converts a Python object to a C++ ``bool``.

    :param obj:
        the Python object to convert.
    :return:
        the boolean value.  An exception will have been raised (to be tested by
        calling :c:func:`PyErr_Occurred`) if the conversion failed.


.. c:function:: int sipConvertToEnum(PyObject *obj, void *eval_p, sipTypeID type_id)

    This converts a Python object to the value of a named C/C++ ``enum``
    member.

    :param obj:
        the Python object to convert.
    :param eval_p:
        a pointer to the location where the converted enumerated value is
        returned.
    :param type_id:
        the enum's :ref:`generated type specification <ref-type-specs>`.
    :return:
        ``-1`` is returned, and a Python exception raised, if there was an
        error.  Otherwise ``0`` is returned.


.. c:function:: void *sipConvertToType(PyObject *obj, sipTypeID type_id, PyObject *transferObj, int flags, int *state_p, int *is_err_p)

    This converts a Python object to an instance of a C structure, C++ class or
    mapped type similar to :c:func:`sipConvertToTypeUS` but without support for
    any user state.

    :param obj:
        the Python object.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param transferObj:
        this controls any ownership changes to *obj*.
    :param flags:
        any combination of the :c:macro:`SIP_NOT_NONE` and
        :c:macro:`SIP_NO_CONVERTORS` flags.
    :param state_p:
        the state of the returned C/C++ instance is returned via this pointer.
    :param is_err_p:
        the error flag is passed and updated via this pointer.
    :return:
        the C/C++ instance.

    See :c:func:`sipConvertToTypeUS` for a full description of the arguments.


.. c:function:: void *sipConvertToTypeUS(PyObject *obj, sipTypeID type_id, PyObject *transferObj, int flags, int *state_p, void **user_state_p, int *is_err_p)

    This converts a Python object to an instance of a C structure, C++ class or
    mapped type assuming that a previous call to :c:func:`sipCanConvertToType`
    has been successful.

    :param obj:
        the Python object.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param transferObj:
        this controls any ownership changes to *obj*.
    :param flags:
        any combination of the :c:macro:`SIP_NOT_NONE` and
        :c:macro:`SIP_NO_CONVERTORS` flags.
    :param state_p:
        the state of the returned C/C++ instance is returned via this pointer.
    :param user_state_p:
        any additional state of the returned C/C++ instance is returned via
        this pointer.
    :param is_err_p:
        the error flag is passed and updated via this pointer.
    :return:
        the C/C++ instance.
    
    If *transferObj* is ``NULL`` then the ownership is unchanged.  If it is
    ``Py_None`` then ownership is transferred to Python via a call to
    :c:func:`sipTransferBack`.
    
    Otherwise ownership is transferred to C/C++ and *obj* associated with
    *transferObj* via a call to :c:func:`sipTransferTo`.

    Note that *obj* can also be managed by the C/C++ instance itself, but this
    can only be achieved by using :c:func:`sipTransferTo`.

    If *state_p* is not ``NULL`` then the location it points to is set to
    describe the state of the returned C/C++ instance and is the value returned
    by any :directive:`%ConvertToTypeCode`.  The calling code must then release
    the value at some point to prevent a memory leak by calling
    :c:func:`sipReleaseType`.

    If *user_state_p* is not ``NULL`` then the location it points to may be
    used by the type convertor for any purpose, typically to store a pointer to
    additional state on the heap.  Any such pointer is passed to the type's
    corresponding :c:func:`sipReleaseTypeUS` function.

    If there is an error then the location *is_err_p* points to is set to a
    non-zero value.  If it was initially a non-zero value then the conversion
    isn't attempted in the first place.  (This allows several calls to be made
    that share the same error flag so that it only needs to be tested once
    rather than after each call.)


.. c:function:: PyObject *sipConvertToTypedArray(void *data, sipTypeID type_id, const char *format, size_t stride, Py_ssize_t len, int flags)

    This converts a one dimensional array of instances of a C structure, C++
    class or mapped type to a :class:`sip.array` object.

    An array is very like a Python :class:`memoryview` object but it's elements
    correspond to C structures or C++ classes.  The underlying memory is not
    copied and may be modified in situ.  Arrays support the buffer protocol and
    so can be passed to other modules, again without the underlying memory
    being copied.

    :param data:
        the address of the start of the C/C++ array.
    :param type_id:
        an element's type's :ref:`generated type specification
        <ref-type-specs>`.
    :param format:
        the format, as defined by the :mod:`struct` module, of an array
        element.
    :param stride:
        the size of an array element, including any padding.
    :param len:
        the number of elements in the array.
    :param flags:
        the optional :c:macro:`SIP_READ_ONLY` flag.
    :return:
        the :class:`sip.array` object.


.. c:function:: void *sipConvertToVoidPtr(PyObject *obj)

    This converts a Python object to a memory address.
    :c:func:`PyErr_Occurred` must be used to determine if the conversion was
    successful.

    :param obj:
        the Python object which may be ``Py_None``, a :class:`sip.voidptr` or a
        :c:type:`PyCObject`.
    :return:
        the memory address.


.. c:type:: sipDateDef

    This C structure is used with :c:func:`sipGetDate`, :c:func:`sipFromDate`,
    :c:func:`sipGetDateTime` and :c:func:`sipFromDateTime` and encapsulates the
    components parts of a Python date.  The structure elements are as follows.

    .. c:member:: int pd_year

        The year.

    .. c:member:: int pd_month

        The month (1-12).

    .. c:member:: int pd_day

        The day (1-31).


.. c:function:: int sipEnableAutoconversion(PyTypeObject *py_type, int enable)

    Instances of some classes may be automatically converted to other Python
    objects even though the class has been wrapped.  This allows that behaviour
    to be suppressed so that an instances of the wrapped class is returned
    instead.

    :param py_type:
        the type object of the type.  This must refer to a wrapped class.
    :param enable:
        is non-zero if auto-conversion should be enabled for the type.  This is
        the default behaviour.
    :return:
        ``1`` or ``0`` depending on whether or not auto-conversion was
        previously enabled for the type.  This allows the previous state to be
        restored later on.  ``-1`` is returned, and a Python exception raised,
        if there was an error.


.. c:function:: int sipEnableGC(int enable)

    This enables or disables the Python garbarge collector.

    :param enable:
        is greater than ``0`` if the garbage collector should be enabled.
    :return:
        ``1`` or ``0`` depending on whether or not the garbage collector was
        previously enabled.  This allows the previous state to be restored
        later on.  ``-1`` is returned if there was an error.


.. c:enum:: sipErrorState

    This enum defines the different error states when parsing function
    signatures.

    .. c:enumerator:: sipErrorNone

        There is no error.

    .. c:enumerator:: sipErrorFail

        There was an error and signature parsing should stop.

    .. c:enumerator:: sipErrorContinue

        There was an error but signature parsing should continue.


.. c:type:: sipEventHandlerSpec

    An array of this C structure is used with
    :c:func:`sipRegisterEventHandlers` to
    define a set of event handlers registered by a module.  The structure
    elements are as follows.

    .. c:member:: sipEventType event_type

        The event type.  :c:enumerator:`sipEventNone`  is used to denote the
        end of the the array.

    .. c:member:: sipTypeID type_id

        The type specification of the type the handler handles.

    .. c:member:: void *handler

        The handler function.


.. c:enum:: sipEventType

    This enum defines the different event types.  Event handlers may be
    registered using :c:func:`sipRegisterEventHandlers`.

    .. c:enumerator:: sipEventCollectingModule

        This event is triggered whenever a wrapped module is being garbage
        collected.  The handler is not passed any arguments.

    .. c:enumerator:: sipEventCollectingWrapper

        This event is triggered whenever a Python wrapper object is being
        garbage collected.  The handler is passed a pointer to the opaque
        module state, the type's type identifier and a pointer to the
        :c:type:`sipSimpleWrapper` being garbage collected.

    .. c:enumerator:: sipEventFinalisingAddress

        This event is triggered before a C/C++ instance is wrapped as a Python
        object to allow the bindings to replace it with another instance (e.g.
        a proxy).  The handler is passed a pointer to the opaque module state,
        the type's type identifier and a ``void *`` which is the address of the
        C/C++ instance.  The handler should return the ``void *`` address to be
        used by the wrapper.  The handler should raise a Python exception and
        return ``NULL`` if there was an error.

    .. c:enumerator:: sipEventFinalisingType

        This event is triggered when a new wrapped type has been created to
        allow additional changes to be made to the type.  The handler is passed
        a pointer to the opaque module state, the type's type identifier and a
        pointer to a :c:type:`PyTypeObject` which is the new type object.  The
        handler should raise a Python exception and return ``-1`` if there was
        an error, otherwise ``0`` should be returned.

    .. c:enumerator:: sipEventNone

        This is used as a sentinal marking the last entry in an array of
        :c:type:`sipEventHandlerSpec`.

    .. c:enumerator:: sipEventWrappedInstance

        This event is triggered whenever a C/C++ instance that is created by
        C/C++ (and not by Python) is wrapped.  The handler is passed a pointer
        to the opaque module state, the type's type identifier and a ``void *``
        which is the address of the C/C++ instance.


.. c:function:: void sipExportSymbols(const sipSymbolSpec *const symbols)

    Python does not allow extension modules to directly access symbols in
    another extension module.  This exports a number of symbols, each
    referenced by a name, that can subsequently be imported, using
    :c:func:`sipImportSymbol`, by another module.

    :param symbols:
        the array of :c:type:`sipSymbolSpec` specifications of symbol names and
        their corresponding values.


.. c:function:: sipTypeID sipFindType(const char *type)

    This returns the type identifier of a C/C++ type.

    :param type:
        the C/C++ declaration of the type.
    :return:
        the type identifier.  ``sipType_Invalid`` is returned if the C/C++ type
        doesn't exist.


.. c:function:: void *sipForceConvertToType(PyObject *obj, sipTypeID type_id, PyObject *transferObj, int flags, int *state, int *iserr)

    This converts a Python object to an instance of a C structure, C++ class or
    mapped type similar to :c:func:`sipForceConvertToTypeUS()` but without
    support for any user state.

    See :c:func:`sipForceConvertToType()` for a full description of the
    arguments.


.. c:function:: void *sipForceConvertToTypeUS(PyObject *obj, sipTypeID type_id, PyObject *transferObj, int flags, int *state, void **user_state, int *iserr)

    This converts a Python object to an instance of a C structure, C++ class or
    mapped type by calling :c:func:`sipCanConvertToType()` and, if it is
    successfull, calling :c:func:`sipConvertToTypeUS()`.

    See :c:func:`sipConvertToTypeUS()` for a full description of the arguments.


.. c:function:: void sipFree(void *mem)

    This returns an area of memory allocated by :c:func:`sipMalloc()` to the
    heap.

    :param mem:
        the memory address.


.. c:function:: PyObject *sipFromDate(const sipDateDef *date)

    This creates a Python date object from its component parts.

    :param date:
        the component parts of the date.
    :return:
        the Python date object.


.. c:function:: PyObject *sipFromDateTime(const sipDateDef *date, const sipTimeDef *time)

    This creates a Python datetime object from its component parts.

    :param date:
        the date related component parts of the datetime.
    :param time:
        the time related component parts of the datetime.
    :return:
        the Python datetime object.


.. c:function:: PyObject *sipFromMethod(const sipMethodDef *method)

    This creates a Python method object from its component parts.

    :param method:
        the component parts of the method.
    :return:
        the Python method object.


.. c:function:: PyObject *sipFromTime(const sipTimeDef *time)

    This creates a Python time object from its component parts.

    :param time:
        the component parts of the time.
    :return:
        the Python time object.


.. c:function:: void *sipGetAddress(sipSimpleWrapper *obj)

    This returns the address of the C structure or C++ class instance wrapped
    by a Python object.

    :param obj:
        the Python object.
    :return:
        the address of the C/C++ instance.


.. c:function:: int sipGetCFunction(PyObject *obj, sipCFunctionDef *c_function)

    This checks to see if an object is a Python C function object and, if so,
    optionally returns its component parts.

    :param obj:
        the Python object.
    :param c_function:
        if this is not ``NULL``, and the object is a C function object, then
        the component parts are returned in this structure.
    :return:
        a non-zero value if the object is a Python C function object.


.. c:function:: int sipGetDate(PyObject *obj, sipDateDef *date)

    This checks to see if an object is a Python date object and, if so,
    optionally returns its component parts.

    :param obj:
        the Python object.
    :param date:
        if this is not ``NULL``, and the object is a date object, then the
        component parts are returned in this structure.
    :return:
        ``1`` if the object is a Python date object, ``0`` if not and ``-1`` if
        there was an error.


.. c:function:: int sipGetDateTime(PyObject *obj, sipDateDef *date, sipTimeDef *time)

    This checks to see if an object is a Python datetime object and, if so,
    optionally returns its component parts.

    :param obj:
        the Python object.
    :param date:
        if this is not ``NULL``, and the object is a datetime object, then the
        date related component parts are returned in this structure.
    :param time:
        if this is not ``NULL``, and the object is a datetime object, then the
        time related component parts are returned in this structure.
    :return:
        ``1`` if the object is a Python datetime object, ``0`` if not and
        ``-1`` if there was an error.


.. c:function:: PyFrameObject *sipGetFrameRef(int depth)

    This retrieves a new reference to a frame object from the current
    execution stack.

    .. note::
        On PyPy this will always return ``NULL``.

    :param depth:
        the depth of frame to retrieve where ``0`` is the current frame, ``1``
        is the previous frame etc.
    :return:
        the opaque frame or ``NULL`` if there wasn't one at the given depth.


.. c:function:: PyInterpreterView *sipGetInterpreterView()

    This returns the current interpreter view.  Call
    :c:func:`PyInterpreterGuard_FromView` to determine if the interpreter is
    usable.

    :return:
        the current interpreter view.


.. c:function:: int sipGetMethod(PyObject *obj, sipMethodDef *method)

    This checks to see if an object is a Python method object and, if so,
    optionally returns its component parts.

    :param obj:
        the Python object.
    :param method:
        if this is not ``NULL``, and the object is a method object, then the
        component parts are returned in this structure.
    :return:
        a non-zero value if the object is a Python method object.


.. c:function:: void *sipGetModuleUserState()

    This returns the address of the user supplied module state structure set
    by :c:func:`sipSetModuleUserState`.

    :return:
        the address of the state structure.


.. c:function:: PyObject *sipGetPyObjectRef(void *cppptr, sipTypeID type_id)

    This returns a new reference to the Python object for a C structure or
    C++ class instance.

    :param cppptr:
        the pointer to the C/C++ instance.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        the Python object or ``NULL`` (and no exception is raised) if the
        C/C++ instance hasn't been wrapped.


.. c:function:: PyTypeObject *sipGetPyTypeRef(sipTypeID type_id)

    This returns a new reference to the Python type object that SIP creates for
    a wrapped type.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        the Python type object.  If the type structure refers to a mapped type
        then ``NULL`` will be returned.


.. c:function:: PyTypeObject *sipGetSimpleWrapperType()

    This returns the type of the :c:type:`PyObject` structure used to wrap
    **simple** types (i.e. those that do not support parent/child
    relationships).  It is exposed to Python code as
    :class:`sip.simplewrapper`.

    :return:
        a borrowed reference to the type object.


.. c:function:: int sipGetState(PyObject *transferObj)

    The :directive:`%ConvertToTypeCode` directive requires that the provided
    code returns an ``int`` describing the state of the converted value.  The
    state usually depends on any transfers of ownership that have been
    requested.  This is a convenience function that returns the correct state
    when the converted value is a temporary.

    :param transferObj:
        the object that describes the requested transfer of ownership.
    :return:
        the state of the converted value.


.. c:function:: int sipGetTime(PyObject *obj, sipTimeDef *time)

    This checks to see if an object is a Python time object and, if so,
    optionally returns its component parts.

    :param obj:
        the Python object.
    :param time:
        if this is not ``NULL``, and the object is a time object, then the
        component parts are returned in this structure.
    :return:
        ``1`` if the object is a Python time object, ``0`` if not and ``-1`` if
        there was an error.


.. c:function:: PyObject *sipGetTypeUserObjectRef(sipWrapperType *type)

    Each generated type corresponding to a wrapped C/C++ type, or a user
    sub-class of such a type, contains an optional reference to a Python object
    that can be used for any purpose by handwritten code and will automatically
    be garbage collected at the appropriate time.  This returns a new reference
    to that object.

    :param type:
        the wrapped type object.
    :return:
        a new reference to the type-specific user object.


.. c:function:: PyObject *sipGetUserObjectRef(sipSimpleWrapper *obj)

    Each wrapped instance contains an optional reference to a Python object
    that can be used for any purpose by handwritten code and will automatically
    be garbage collected at the appropriate time.  This returns a new reference
    to that object.

    :param obj:
        the wrapped instance.
    :return:
        a new reference to the instance-specific user object.


.. c:function:: PyTypeObject *sipGetVoidPtrType()

    This returns the type of the :c:type:`PyObject` structure that is used to
    wrap a ``void *``.  It is exposed to Python code as :class:`sip.voidptr`.

    :return:
        a borrowed reference to the type object.


.. c:function:: PyTypeObject *sipGetWrapperType()

    This returns the type of the :c:type:`PyObject` structure used to wrap
    types.  It is exposed to Python code as :class:`sip.wrapper`.

    :return:
        a borrowed reference to the type object.


.. c:function:: PyTypeObject *sipGetWrapperTypeType()

    This returns the metatype of the :c:type:`PyObject` structure used to wrap
    types (both simple and non-simple).  It is exposed to Python code as
    :class:`sip.wrappertype`.

    :return:
        a borrowed reference to the type object.


.. c:function:: void *sipImportSymbol(const char *name)

    Python does not allow extension modules to directly access symbols in
    another extension module.  This imports a symbol, referenced by a name,
    that has previously been exported, using :c:func:`sipExportSymbols`, by
    another module.

    :param name:
        the name of the symbol.
    :return:
        the value of the symbol.  ``NULL`` is returned (but no exception set)
        if there is no such symbol.


.. c:function:: void sipInstanceDestroyed(sipSimpleWrapper **obj_p)

    This should be called by handwritten code if it is able to detect that a
    wrapped C++ instance has been destroyed from C++.  It should not be called
    if SIP is able to detect this itself, i.e. when the instance was created
    from Python and the class has a virtual destructor.

    :param obj_p:
        a pointer to the Python object that wraps the destroyed instance.  It
        will be set to ``NULL`` when the call returns.


.. c:function:: int sipIsEnumFlag(PyObject *obj)

    This determines if an object is a sub-class of :py:class:`enum.Flag`.

    :param obj:
        the object.
    :return:
        a non-zero value if the object is a :py:class:`enum.Flag` sub-class.


.. c:function:: int sipIsOwnedByPython(sipSimpleWrapper *obj)

    This determines if a wrapped object is currently owned by Python.

    :param obj:
        the wrapped object.
    :return:
        a non-zero value if the object is currently owned by Python.


.. c:function:: int sipIsUserType(const sipWrapperType *type)

    This checks if a type corresponds to a wrapped C/C++ type or a user
    sub-class of such a type.

    :param type:
        the type object.
    :return:
        a non-zero value if the type is a user defined type.


.. c:function:: char sipLong_AsChar(PyObject *obj)

    This converts a Python object to a C/C++ char.  If the value is too large
    then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: signed char sipLong_AsSignedChar(PyObject *obj)

    This converts a Python object to a C/C++ signed char.  If the value is too
    large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: unsigned char sipLong_AsUnsignedChar(PyObject *obj)

    This converts a Python object to a C/C++ unsigned char.  If the value is
    too large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: short sipLong_AsShort(PyObject *obj)

    This converts a Python object to a C/C++ short.  If the value is too large
    then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: unsigned short sipLong_AsUnsignedShort(PyObject *obj)

    This converts a Python object to a C/C++ unsigned short.  If the value is
    too large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: int sipLong_AsInt(PyObject *obj)

    This converts a Python object to a C/C++ int.  If the value is too large
    then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: unsigned int sipLong_AsUnsignedInt(PyObject *obj)

    This converts a Python object to a C/C++ unsigned int.  If the value is too
    large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: size_t sipLong_AsSizeT(PyObject *obj)

    This converts a Python object to a C/C++ size_t.  If the value is too large
    then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: long sipLong_AsLong(PyObject *obj)

    This converts a Python object to a C/C++ long.  If the value is too large
    then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: unsigned long sipLong_AsUnsignedLong(PyObject *obj)

    This converts a Python object to a C/C++ unsigned long.  If the value is
    too large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: long long sipLong_AsLongLong(PyObject *obj)

    This converts a Python object to a C/C++ long long.  If the value is too
    large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: unsigned long long sipLong_AsUnsignedLongLong(PyObject *obj)

    This converts a Python object to a C/C++ unsigned long long.  If the value
    is too large then an exception is raised.

    :param obj:
        the Python object.
    :return:
        the converted C/C++ value.


.. c:function:: void *sipMalloc(size_t nbytes)

    This allocates an area of memory on the heap using the Python
    :c:func:`PyMem_RawMalloc` function.  The memory is freed by calling
    :c:func:`sipFree`.

    :param nbytes:
        the number of bytes to allocate.
    :return:
        the memory address.  If there was an error then ``NULL`` is returned
        and a Python exception raised.


.. c:type:: sipMethodDef

    This C structure is used with :c:func:`sipGetMethod` and
    :c:func:`sipFromMethod` and encapsulates the components parts of a Python
    method.  The structure elements are as follows.

    .. c:member:: PyObject *pm_function

        The function that implements the method.

    .. c:member:: PyObject *pm_self

        The bound object.

.. c:type:: sipModuleState

    This mutable opaque C structure is the state data of a generated extension
    module.  A pointer to this (called ``sipMS``) is made available to all
    handwritten code and should be passed to any helper functions that make API
    calls themselves.

.. c:function:: void sipObjectDump(PyObject *obj)

    This is a thin wrapper around :c:func:`PyObject_Dump` that is typically
    used when debugging when the limited Python API is enabled.

    :param obj:
        the Python object.


.. c:function:: int sipParseResult(PyThreadStateToken *tst, const char *error_handler, PyObject *self, PyObject *method, PyObject *result, const char *format, ...)

    This converts a Python object (usually returned by a method) to C/C++ based
    on a format string and associated values in a similar way to the Python
    :c:func:`PyArg_ParseTuple` function.

    :param tst:
        the thread state token.
    :param error_handler:
        the name of the error handler.
    :param self:
        the Python self object.
    :param method:
        the Python method that returned *result*.
    :param result:
        the Python object returned by *method*.
    :param format:
        the format string.
    :return:
        0 if there was no error.  Otherwise a negative value is returned, and
        an exception raised.

    This is normally called by handwritten code specified with the
    :directive:`%VirtualCatcherCode` directive with *tst* being the supplied
    ``sipTST``, *error_handler* being the the supplied ``sipErrorHandler``,
    *self* being the supplied ``sipPySelf``, *method* being the supplied
    ``sipMethod`` and *result* being the value returned by
    :c:func:`sipCallMethod`.

    If *format* begins and ends with parentheses then *result* must be a Python
    tuple and the rest of *format* is applied to the tuple contents.

    In the following description the first letter is the format character, the
    entry in parentheses is the Python object type that the format character
    will convert, and the entry in brackets are the types of the C/C++ values
    to be passed. 

    ``ae`` (object) [char \*]
        Convert a Python string-like object of length 1 to a C/C++ ``char``
        according to the encoding ``e``.  ``e`` can either be ``A`` for ASCII,
        ``L`` for Latin-1, or ``8`` for UTF-8.  The object may either be a
        ``bytes`` object or a ``str`` object that can be encoded.  An object
        that supports the buffer protocol may also be used.

    ``b`` (integer) [bool \*]
        Convert a Python bool or integer to a C/C++ ``bool``.

    ``c`` (bytes) [char \*]
        Convert a Python ``bytes`` object of length 1 to a C/C++ ``char``.

    ``d`` (float) [double \*]
        Convert a Python floating point number to a C/C++ ``double``.

    ``e`` (integer) [enum \*]
        Convert a Python integer to an anonymous C/C++ ``enum``.

    ``f`` (float) [float \*]
        Convert a Python floating point number to a C/C++ ``float``.

    ``g`` (bytes) [const char \*\*, :c:type:`Py_ssize_t` \*]
        Convert a Python ``bytes`` object to a C/C++ character array and its
        length.  If the Python object is ``Py_None`` then the array and length
        are ``NULL`` and zero respectively.

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

    ``t`` (long) [unsigned short \*]
        Convert a Python long to a C/C++ ``unsigned short``.

    ``u`` (long) [unsigned int \*]
        Convert a Python long to a C/C++ ``unsigned int``.

    ``w`` (string) [wchar_t \*]
        Convert a Python ``str`` object of length 1 to a C/C++ wide character.

    ``x`` (string) [wchar_t \*\*]
        Convert a Python ``str`` object to a C/C++ ``L'\0'`` terminated wide
        character string.  If the Python object is ``Py_None`` then the string
        is ``NULL``.

    ``Ae`` (object) [int, const char \*\*]
        Convert a Python string-like object to a C/C++ ``'\0'`` terminated
        string according to the encoding ``e``.  ``e`` can either be ``A`` for
        ASCII, ``L`` for Latin-1, or ``8`` for UTF-8.  If the Python object is
        ``Py_None`` then the string is ``NULL``.  The integer uniquely
        identifies the object in the context defined by the ``S`` format
        character and allows an extra reference to the object to be kept to
        ensure that the string remains valid.  The object may either be a
        ``bytes`` object or a ``str`` object that can be encoded.  An object
        that supports the buffer protocol may also be used.

    ``B`` (bytes) [int, const char \*\*]
        Convert a Python ``bytes`` object to a C/C++ ``'\0'`` terminated
        string.  If the Python object is ``Py_None`` then the string is
        ``NULL``.  The integer uniquely identifies the object in the context
        defined by the ``S`` format character and allows an extra reference to
        the object to be kept to ensure that the string remains valid.

    ``F`` (wrapped enum) [:c:type:`sipTypeDef` \*, enum \*]
        Convert a Python named enum type to the corresponding C/C++ ``enum``.

    ``G`` (string) [wchar_t \*\*, :c:type:`Py_ssize_t` \*]
        Convert a Python ``str`` object to a C/C++ wide character array and its
        length.  If the Python object is ``Py_None`` then the array and length
        are ``NULL`` and zero respectively.

    ``Hf`` (wrapped instance) [const :c:type:`sipTypeDef` \*, int \*, void \*\*]
        Convert a Python object to a C structure, C++ class or mapped type
        instance as described in :c:func:`sipConvertToType()`.  ``f`` is a
        combination of the following flags encoded as an ASCII character by
        adding ``0`` to the combined value:

        0x01 disallows the conversion of ``Py_None`` to ``NULL``

        0x02 implements the :fanno:`Factory` and :fanno:`TransferBack` annotations

        0x04 returns a copy of the C/C++ instance.

    ``L`` (integer) [signed char \*]
        Convert a Python integer to a C/C++ ``signed char``.

    ``M`` (long) [unsigned char \*]
        Convert a Python long to a C/C++ ``unsigned char``.

    ``N`` (object) [PyTypeObject \*, PyObject \*\*]
        A Python object is checked to see if it is a certain type and then
        returned without any conversions.  The reference count is incremented.
        The Python object may be ``Py_None``.

    ``O`` (object) [PyObject \*\*]
        A Python object is returned without any conversions.  The reference
        count is incremented.

    ``T`` (object) [PyTypeObject \*, PyObject \*\*]
        A Python object is checked to see if it is a certain type and then
        returned without any conversions.  The reference count is incremented.
        The Python object may not be ``Py_None``.

    ``V`` (:class:`sip.voidptr`) [void \*\*]
        Convert a Python :class:`sip.voidptr` object to a C/C++ ``void *``.

    ``z`` (object) [const char \*, void \*\*]
        Convert a Python named capsule object to a C/C++ ``void *``.

    ``Z`` (object) []
        Check that a Python object is ``Py_None``.  No value is returned.

    ``!`` (object) [PyObject \*\*]
        A Python object is checked to see if it implements the buffer protocol
        and then returned without any conversions.  The reference count is
        incremented.  The Python object may not be ``Py_None``.

    ``$`` (object) [PyObject \*\*]
        A Python object is checked to see if it implements the buffer protocol
        and then returned without any conversions.  The reference count is
        incremented.  The Python object may be ``Py_None``.

    ``=`` (long) [size_t \*]
        Convert a Python long to a C/C++ ``size_t``.


.. c:function:: PyObject *sipPyTypeDictRef(PyTypeObject *py_type)

    This is a thin wrapper around :c:func:`PyType_GetDict` and is typically
    used when the limited Python API is enabled.

    :param py_type:
        the type object.
    :return:
        a new reference to type object's read-only type dictionary.


.. c:function:: int sipRegisterEventHandlers(const sipEventHandlerSpec *const handler_specs)

    This registers a number of event handlers which will be called whenever the
    corresponding event is triggered.

    :param handler_specs:
        the array of :c:type:`sipEventHandlerSpec` specifications of event
        types are their corresponding handlers.
    :return:
        0 if there was no error, otherwise -1 is returned (and a Python
        exception is raised).


.. c:function:: int sipRegisterPyType(PyTypeObject *type)

    This registers a Python type object that can be used as the meta-type or
    super-type of a wrapped C++ type.
    
    :param type:
        the type object.
    :return:
        0 if there was no error, otherwise -1 is returned.

    See the section :ref:`ref-types-metatypes` for more details.


.. c:function:: void sipReleaseType(void *cpp, sipTypeID type_id, int state)

    This releases a wrapped C/C++ or mapped type instance to the heap if it was
    a temporary instance similar to :c:func:`sipReleaseTypeUS` but without
    support for any user state.
    
    :param cpp:
        the C/C++ instance.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param state:
        describes the state of the C/C++ instance.
    
    See :c:func:`sipReleaseTypeUS` for a full description of the arguments.


.. c:function:: void sipReleaseTypeUS(void *cpp, sipTypeID type_id, int state, void *user_state)

    This releases a wrapped C/C++ or mapped type instance to the heap if it was
    a temporary instance.  It is called after a call to either
    :c:func:`sipConvertToTypeUS` or :c:func:`sipForceConvertToTypeUS`.
    
    :param cpp:
        the C/C++ instance.
    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :param state:
        describes the state of the C/C++ instance.
    :param user_state:
        the value set by the corresponding call to
        :c:func:`sipConvertToTypeUS` or :c:func:`sipForceConvertToTypeUS`.


.. c:function:: const char *sipResolveTypedef(const char *name)

    This returns the value of a C/C++ typedef.

    :param name:
        the name of the typedef.
    :return:
        the value of the typedef. If there was no such typedef then *name* will
        be returned.


.. c:function:: void sipSetModuleUserState(void *user_state, sipModuleUserStateClearFunc clear, sipModuleUserStateFreeFunc free, sipModuleUserStateTraverseFunc traverse)

    This sets the address of the user supplied module state structure.  It can
    be obtained by calling :c:func:`sipGetModuleUserState`.  It also sets a
    number of functions that are called to manage the lifecycle of the
    structure.

    This structure is defined and used by bindings authors to avoid using
    static variables in their handwritten code so that multiple interpreters
    can be supported.  This function is normally called from
    :directive:`%InitialisationCode`.

    :param user_state:
        the address of the state structure.
    :param clear:
        the optional function that is called by the module's clear slot.  It is
        passed a pointer to the opaque module state and the address of the
        state structure.  It should return an ``int`` that is normally ``0`` or
        ``-1`` to indicate an error.
    :param free:
        the optional function that is called by the module's free slot.  It is
        passed a pointer to the opaque module state and the address of the
        state structure.
    :param traverse:
        the optional function that is called by the module's traverse slot.  It
        is passed a pointer to the opaque module state, the address of the
        state structure and the ``visitproc`` and ``arg`` arguments that were
        passed to the module's traverse slot.  It should return an ``int`` that
        is normally ``0`` or the result returned by any calls to ``visitproc``.
        This function must only call the *DuringGC* Python functions.


.. c:function:: void sipSetTypeUserObject(sipWrapperType *type, PyObject *user)

    Each generated type corresponding to a wrapped C/C++ type, or a user
    sub-class of such a type, can contain a reference to a single Python object
    that can be used for any purpose by handwritten code and will automatically
    be garbage collected at the appropriate time.  This sets that object using
    a new reference and replacing any existing object.

    :param type:
        the type object.
    :param user:
        the type-specific user object.


.. c:function:: void sipSetUserObject(sipSimpleWrapper *obj, PyObject *user)

    Each wrapped object can contain a reference to a single Python object that
    can be used for any purpose by handwritten code and will automatically be
    garbage collected at the appropriate time.  This sets that object using a
    new reference and replacing any existing object.

    :param obj:
        the wrapped object.
    :param user:
        the instance-specific user object.


.. c:type:: sipSimpleWrapper

    This is an opaque C structure that implements a Python wrapped instance
    whose type is :class:`sip.simplewrapper`.  It is an extension of the
    :c:type:`PyObject` structure and so may be safely cast to it.


.. c:type:: sipSymbolSpec

    An array of this C structure is used with :c:func:`sipExportSymbols` to
    define a set of symbols exported by a module.  The structure elements are
    as follows.

    .. c:member:: const char *name

        The name of the symbol.  ``NULL`` is used to denote the end of the
        array.

    .. c:member:: void *symbol

        The value of the symbol.


.. c:type:: sipTimeDef

    This C structure is used with :c:func:`sipGetTime`, :c:func:`sipFromTime`,
    :c:func:`sipGetDateTime` and :c:func:`sipFromDateTime` and encapsulates the
    components parts of a Python time.  The structure elements are as follows.

    .. c:member:: int pt_hour

        The hour (0-23).

    .. c:member:: int pt_minute

        The minute (0-59).

    .. c:member:: int pt_second

        The second (0-59).

    .. c:member:: int pt_microsecond

        The microsecond (0-999999).


.. c:function:: void sipTransferBack(PyObject *obj)

    This transfers ownership of a Python wrapped instance to Python (see
    :ref:`ref-object-ownership`).

    :param obj:
        the wrapped instance.
        
    In addition, any association of the instance with regard to the cyclic
    garbage collector with another instance is removed.


.. c:function:: void sipTransferTo(PyObject *obj, PyObject *owner)

    This transfers ownership of a Python wrapped instance to C++ (see
    :ref:`ref-object-ownership`).

    :param obj:
        the wrapped instance.
    :param owner:
        an optional wrapped instance that *obj* becomes associated with with
        regard to the cyclic garbage collector.  If *owner* is ``NULL`` then no
        such association is made.  If *owner* is ``Py_None`` then *obj* is
        given an extra reference which is removed when the C++ instance's
        destructor is called.  If *owner* is the same value as *obj* then any
        reference cycles involving *obj* can never be detected or broken by the
        cyclic garbage collector.  Responsibility for calling the C++
        instance's destructor is always transfered to C++.


.. c:type:: sipTypeID

    This opaque C type is the ABI's representation of a
    :ref:`generated type specification <ref-type-specs>`.


.. c:function:: int sipTypeIsClass(sipTypeID type_id)

    This checks if a type specification refers to a C structure or C++ class or
    namespace.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        a non-zero value if the type specification refers to a structure, class
        or namespace.


.. c:function:: int sipTypeIsEnum(sipTypeID type_id)

    If the ``CustomEnums`` option was specified in the
    :directive:`SipModuleConfiguration` directive then this checks if a type
    specification refers to a C-style named enum.

    If the ``PyEnums`` option was specified in the
    :directive:`SipModuleConfiguration` directive then this checks if a type
    specification refers to any style of named enum.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        a non-zero value if the type specification refers to an appropriate
        enum.


.. c:function:: int sipTypeIsMapped(sipTypeID type_id)

    This checks if a type specification refers to a mapped type.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        a non-zero value if the type specification refers to a mapped type.


.. c:function:: int sipTypeIsScopedEnum(sipTypeID type_id)

    This checks if a type specification refers to a C++11 scoped enum.  It is
    only present if the ``CustomEnums`` option was specified in the
    :directive:`SipModuleConfiguration` directive.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        a non-zero value if the type specification refers to a C++11 scoped
        enum.


.. c:function:: sipTypeID sipTypeScope(sipTypeID type_id)

    This returns the type specification of the enclosing scope of a wrapped
    type.

    :param type_id:
        the C/C++ type's :ref:`generated type specification <ref-type-specs>`.
    :return:
        the type specification of the scope or ``sipType_Invalid`` if the type
        has no scope.


.. c:function:: void *sipUnicodeData(PyObject *obj, int *char_size, Py_ssize_t *len)

    This returns information about the contents of a Python unicode object.

    :param obj:
        the unicode object.
    :param char_size:
        a pointer which will be updated with the number of bytes (either 1, 2
        or 4) used to store a character.  If there was an error then this will
        be a negative value.
    :param len:
        a pointer which will be updated with the number of characters (not
        bytes) in the unicode object.
    :return:
        the address of the buffer where the characters are stored.  It will be
        undefined if the returned character size is a negative value.


.. c:function:: PyObject *sipUnicodeNew(Py_ssize_t len, unsigned maxchar, int *kind, void **data)

    This creates a Python unicode object that will hold a set number of
    characters, each character being of a certain size.

    :param len:
        the number of characters.
    :param maxchar:
        the largest code point that will be placed in the object.
    :param kind:
        a pointer which will be updated with a value that represents the number
        of bytes (either 1, 2 or 4) used to store a character.
    :param data:
        a pointer which will be updated with the address of the buffer where
        the characters will be stored.
    :return:
        the unicode object or ``NULL`` if there was an error.


.. c:function:: void sipUnicodeWrite(int kind, void *data, int index, unsigned value)

    This updates the buffer of a Python unicode object with a character at a
    particular position.

    :param kind:
        the value that represents the number of bytes (either 1, 2 or 4) used
        to store a character.
    :param data:
        the address of the buffer where the characters are stored.
    :param index:
        the character (not byte) index of the character to be updated.
    :param value:
        the value of the new character.


.. c:function:: void sipVisitWrappers(sipWrapperVisitorFunc visitor, void *closure)

    This calls a visitor function for every wrapper object.

    :param visitor:
        the visitor function.
    :param closure:
        a pointer that is passed to the visitor.

    A visitor has the following signature.

    void visitor(sipSimpleWrapper \*obj, void \*closure)

        *obj* is the wrapped object being visited.

        *closure* is the pointer passed to :c:func:`sipVisitWrappers`.


.. c:type:: sipWrapper

    This is an opaque C structure that implements a Python wrapped instance
    whose type is :class:`sip.wrapper`.  It is an extension of the
    :c:type:`sipSimpleWrapper` and :c:type:`PyObject` structures and so may be
    safely cast to both.


.. c:type:: sipWrapperType

    This is an opaque C structure that implements a SIP generated type object.
    It is an extension of the :c:type:`PyTypeObject` structure (which is itself
    an extension of the :c:type:`PyObject` structure) and so may be safely cast
    to :c:type:`PyTypeObject` (and :c:type:`PyObject`).


Version History
---------------

v14.0
.....

The initial version.
