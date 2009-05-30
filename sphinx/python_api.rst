.. _ref-python-api:

Using the SIP Module in Applications
====================================

The main purpose of the SIP module is to provide functionality common to all
SIP generated bindings.  It is loaded automatically and most of the time you
will completely ignore it.  However, it does expose some functionality that can
be used by applications.

cast(obj, type)
    This does the Python equivalent of casting a C++ instance to one of its
    sub or super-class types.  *obj* is the Python object and *type* is the
    type.  A new Python object is returned that wraps the same C++ instance as
    *obj*, but has the type *type*.

delete(obj)
    For C++ instances this calls the C++ destructor.  For C structures it
    returns the structure's memory to the heap.  *obj* is the Python object.

dump(obj)
    This writes various bits of useful information about the internal state of
    the Python object that wraps a C++ instance or C structure.

isdeleted(obj)
    This returns True if the C++ instance or C structure has been destroyed or
    returned to the heap.  *obj* is the Python object.

setdeleted(obj)
    This marks the C++ instance or C structure as having been destroyed or
    returned to the heap so that future references to it raise an exception
    rather than cause a program crash.  Normally SIP handles such things
    automatically, but there are circumstances where this isn't possible.
    *obj* is the Python object.

settracemask(mask)
    If the bindings have been created with SIP's ``-r`` command line option
    then the generated code will produce debugging statements that trace the
    execution of the code.  (It is particularly useful when trying to
    understand the operation of a C++ library's virtual function calls.)

    Debugging statements are generated at the following points:

    - in a C++ virtual function (*mask* is ``0x0001``)
    - in a C++ constructor (*mask* is ``0x0002``)
    - in a C++ destructor (*mask* is ``0x0004``)
    - in a Python type's __init__ method (*mask* is ``0x0008``)
    - in a Python type's __del__ method (*mask* is ``0x0010``)
    - in a Python type's ordinary method (*mask* is ``0x0020``).

    By default the trace mask is zero and all debugging statements are
    disabled.

SIP_VERSION
    This is a Python integer object that represents the SIP version number as
    a 3 part hexadecimal number (e.g. v4.0.0 is represented as ``0x040000``).
    It was first implemented in SIP v4.2.

SIP_VERSION_STR
    This is a Python string object that defines the SIP version number as
    represented as a string.  For development snapshots it will start with
    ``snapshot-``.  It was first implemented in SIP v4.3.

transferback(obj)
    This function is a wrapper around `sipTransferBack()`_.

transferto(obj, owner)
    This function is a wrapper around `sipTransferTo()`_.

unwrapinstance(obj)
    Return the address, as a number, of the wrapped C/C++ structure or class
    instance *obj*.

voidptr
    This is the type object for the type SIP uses to represent a C/C++
    ``void *``.  It may have a size associated with the address in which case
    the Python buffer protocol is supported.  This means that the memory can
    be treated as a mutable array of bytes when wrapped with the ``buffer()``
    builtin.  The type has the following methods:

    __init__(self, address, size=-1, writeable=True)
        ``address`` is either another ``voidptr``, ``None``, a Python CObject,
        or an integer.  ``size`` is the optional associated size of the block
        of memory and is negative if the size is not known.  ``writeable`` is
        set if the memory is writeable.  If it is not specified, and
        ``address`` is a ``voidptr`` then its value will be used.

    __int__(self)
        This returns the address as an integer.

    __hex__(self)
        This returns the address as a hexadecimal string.

    ascobject(self)
        This returns the address as a Python CObject.

    asstring(self, size=-1)
        This returns a copy of the block of memory as a Python v2 string object
        or a Python v3 bytes object.  If ``size`` is given then it specifies
        the number of bytes to copy.  If it is not given (or is negative) then
        the size associated with the address is used.  If there is no
        associated size then an exception is raised.

    getsize(self)
        This returns the size associated with the address.  It will be
        negative if there is no associated size.

    setsize(self, size)
        This sets the size associated with the address.  If ``size`` is
        negative then no size is associated.

    getwriteable(self)
        This returns the writeable state of the memory.

    setwriteable(self, writeable)
        writeable state of the memory.

wrapinstance(addr, type)
    A C/C++ structure or class instance is wrapped and the Python object
    created is returned.  If the instance has already been wrapped then a new
    reference to the existing object is returned.  *addr* is the address of
    the instance represented as a number.  *type* is the type of the object
    (e.g. ``qt.QWidget``).

wrapper
    This is the type object of the base type of all instances wrapped by SIP.

wrappertype
    This is the type object of the metatype of the ``wrapper`` type.
