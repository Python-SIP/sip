Porting to ABI v14
==================

.. note::
    This section is a work-in-progress and will be continually updated while
    ABI v14 is considered experimental.

This section is intended to help bindings authors port their projects to target
ABI v14.  The strongest motivation for doing so is to be able to create
extension modules that are able to take advantage of multiple interpreters and
free-threading.


Overview
--------

- PEP 489 (multi-phase initialisation)
- PEP 573 (module states)
- opaque and mutable vs public and immutable
- API porting definitions.


:file:`.sip` Specification File Changes from v13 and v12
--------------------------------------------------------

This section summarises the differences in :file:`.sip` specification files
(including handwritten code) between v14 and older versions.

- All handwritten implementations of ``__hash__()`` must return
  :c:type:`Py_hash_t`.

- All handwritten implementations of ``__len__()`` must return
  :c:type:`Py_ssize_t`.

- :directive:`%AccessCode` is not supported.  :directive:`%GetCode` should be
  used instead.

- The concept of delayed dtors is no longer supported.  The :canno:`DelayDtor`
  class annotation is ignored.

- The deprecated ``sipIsErr`` error flag is not supported.


API Changes from v13 and v12
----------------------------

This section summarises the differences between the v14 API and older versions.

- All calls that used to take or return a pointer to a :c:type:`sipWrapperType`
  now take or return a pointer to a :c:type:`PyTypeObject`.

- All calls that used to take or return a pointer to a :c:type:`sipWrapper` or
  :c:type:`sipSimpleWrapper` now take or return a pointer to a
  :c:type:`PyObject`.

- :c:func:`sipGetBufferInfo`, :c:func:`sipReleaseBufferInfo` and
  :c:type:`sipBufferInfoDef` are no longer supported as the corresponding
  Python support is now part of the stable ABI.

- :c:func:`sipConvertFromEnum` is passed a pointer to the enum's value rather
  than the value itself.

- :c:func:`sipConvertToBool` now returned a C/C++ ``bool`` rather than an
  ``int``.  :c:func:`PyErr_Occurred` must be called to check if the conversion
  was successful.

- :c:func:`sipConvertToEnum` is passed a pointer to return the enum's value
  rather than returning the value from the function itself.

- :c:func:`sipEnableAutoconversion` is passed the Python type object rather
  than the generated type structure.

- :c:func:`sipExportSymbol` has been replaced by :c:func:`sipExportSymbols`.

- :c:func:`sipGetDate`, :c:func:`sipGetDateTime` and :c:func:`sipGetTime` now
  return ``-1`` if there was an error.

- :c:func:`sipGetFrame` has been replaced by :c:func:`sipGetFrameRef`.

- :c:func:`sipGetInterpreter` has been replaced by
  :c:func:`sipGetInterpreterView`.

- :c:func:`sipGetPyObject` has been replaced by :c:func:`sipGetPyObjectRef`.

- :c:func:`sipGetTypeUserData` has been replaced by
  :c:func:`sipGetTypeUserObject`.

- :c:func:`sipInstanceDestroyed` now takes a pointer to a ``PyObject *``
  (rather than just a ``PyObject *``) which will be set to ``NULL`` when the
  call returns.

- :c:func:`sipIsUserType` now returns ``-1`` (and raises a Python exception) if
  there was an error.

- :c:func:`sipParseResult` has additional arguments that are provided by
  :directive:`%VirtualCatcherCode`.  The ``S`` format character is no longer
  supported.

- :c:func:`sipPrintObject` has been replaced by :c:func:`sipObjectDump`.

- :c:func:`sipPyTypeDict` has been removed, use :c:func:`sipPyTypeDictRef`
  instead.

- :c:func:`sipPyTypeName` has been removed, use
  ``(const char *)PyType_GetSlot(type, Py_tp_name)`` instead.

- :c:func:`sipRegisterAttributeGetter` is no longer supported.  A handler for
  the :c:enumerator:`sipEventFinalisingType` event type should be used instead.

- :c:func:`sipRegisterProxyResolver` is no longer supported.  A handler for the
  :c:enumerator:`sipEventFinalisingAddress` event type should be used instead.

- :c:func:`sipRegisterEventHandler` has been replaced by
  :c:func:`sipRegisterEventHandlers`.

- :c:func:`sipRegisterExitNotifier` is no longer supported.  A handler for the
  :c:enumerator:`sipEventCollectingModule` event type should be used instead.

- :c:func:`sipResolveTypedef` returns a pointer to the name being resolved
  (rather than `NULL`) if the name doesn't correspond to the name of a typedef.

- :c:func:`sipSetDestroyOnExit` is no longer supported.

- :c:func:`sipSetTypeUserData` has been replaced by
  :c:func:`sipSetTypeUserObject`.

- :c:var:`sipSimpleWrapper_Type` is no longer supported, call
  :c:func:`sipGetSimpleWrapperType` instead.

- :c:func:`sipTypeAsPyTypeObject` has been replaced by
  :c:func:`sipGetPyTypeRef`.

- :c:func:`sipTypeIsNamespace` is no longer supported.

- :c:func:`sipTypeName` is no longer supported.  Calls to
  :c:func:`sipGetPyTypeRef` and :c:func:`PyType_GetSlot` should be used
  instead.

- :c:var:`sipVoidPtr_Type` is no longer supported, call
  :c:func:`sipGetVoidPtrType` instead.

- :c:var:`sipWrapper_Type` is no longer supported, call
  :c:func:`sipGetWrapperType` instead.

- :c:var:`sipWrapperType_Type` is no longer supported, call
  :c:func:`sipGetWrapperTypeType` instead.


A Case Study
------------

In this section we outline the steps taken to add ABI v14 as a target to two
large projects, specifically PyQt5 (which currently targets ABI v12) and PyQt6
(which currently targets ABI v13).  We are adding the new target, rather than
replacing the existing one, because we still need to build the project against
versions of Python prior to v3.15.
