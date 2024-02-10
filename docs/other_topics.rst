Other Topics
============

Wrapping Enums
--------------

SIP wraps C/C++ enums using a dedicated Python type and implements behaviour
that mimics the C/C++ behaviour regarding the visibility of the enum's members.
In other words, an enum's members have the same visibility as the enum itself.
For example::

    class MyClass
    {
    public:
        enum MyEnum
        {
            Member
        }
    }

In Python the ``Member`` member is referenced as ``MyClass.Member``.  This
behaviour makes it easier to translate C/C++ code to Python.

In more recent times C++11 has introduced scoped enums and Python has
introduced the :mod:`enum` module.  In both cases a member is only visible in
the scope of the enum.  In other words, the ``Member`` member is referenced as
``MyClass.MyEnum.Member``.

SIP generates bindings for C++11 scoped enums and implements them as Python
:class:`enum.Enum` objects.

A disadvantage of the above is that the Python programmer needs to know the
nature of the C/C++ enum in order to access its members.  In order to avoid
this, SIP makes the members of traditional C/C++ enums visible from the scope
of the enum as well.

It is recommended that Python code should always specify the enum scope when
referencing an enum member.  A future version of SIP will remove support for
the traditional behaviour.


.. _ref-object-ownership:

Ownership of Objects
--------------------

When a C++ instance is wrapped a corresponding Python object is created.  The
Python object behaves as you would expect in regard to garbage collection - it
is garbage collected when its reference count reaches zero.  What then happens
to the corresponding C++ instance?  The obvious answer might be that the
instance's destructor is called.  However the library API may say that when the
instance is passed to a particular function, the library takes ownership of the
instance, i.e. responsibility for calling the instance's destructor is
transferred from the SIP generated module to the library.

Ownership of an instance may also be associated with another instance.  The
implication being that the owned instance will automatically be destroyed if
the owning instance is destroyed.  SIP keeps track of these relationships to
ensure that Python's cyclic garbage collector can detect and break any
reference cycles between the owning and owned instances.  The association is
implemented as the owning instance taking a reference to the owned instance.

The :aanno:`TransferThis`, :aanno:`Transfer` and :aanno:`TransferBack` argument
annotations are used to specify where, and it what direction, transfers of
ownership happen.  It is very important that these are specified correctly to
avoid crashes (where both Python and C++ call the destructor) and memory leaks
(where neither Python and C++ call the destructor).

This applies equally to C structures where the structure is returned to the
heap using the :c:func:`free` function.

See also :c:func:`sipTransferTo()` and :c:func:`sipTransferBack()`.


.. _ref-types-metatypes:

Types and Meta-types
--------------------

Every Python object (with the exception of the :class:`object` object itself)
has a meta-type and at least one super-type.  By default an object's meta-type
is the meta-type of its first super-type.

SIP implements two super-types, :class:`sip.simplewrapper` and
:class:`sip.wrapper`, and a meta-type, :class:`sip.wrappertype`.

:class:`sip.simplewrapper` is the super-type of :class:`sip.wrapper`.  The
super-type of :class:`sip.simplewrapper` is :class:`object`.

:class:`sip.wrappertype` is the meta-type of both :class:`sip.simplewrapper`
and :class:`sip.wrapper`.  The super-type of :class:`sip.wrappertype` is
:class:`type`.

:class:`sip.wrapper` supports the concept of object ownership described in
:ref:`ref-object-ownership` and, by default, is the super-type of all the types
that SIP generates.

:class:`sip.simplewrapper` does not support the concept of object ownership but
SIP generated types that are sub-classed from it have Python objects that take
less memory.

SIP allows a class's meta-type and super-type to be explicitly specified using
the :canno:`Metatype` and :canno:`Supertype` class annotations.

SIP also allows the default meta-type and super-type to be changed for a module
using the :directive:`%DefaultMetatype` and :directive:`%DefaultSupertype`
directives.  Unlike the default super-type, the default meta-type is inherited
by importing modules.

If you want to use your own meta-type or super-type then they must be
sub-classed from one of the SIP provided types.  Your types must be registered
using :c:func:`sipRegisterPyType()`.  This is normally done in code specified
using the :directive:`%InitialisationCode` directive.

.. note::

    It is not possible to define new super-types or meta-types if the limited
    Python API is enabled.


.. _ref-lazy-type-attributes:

Lazy Type Attributes
--------------------

Instead of populating a wrapped type's dictionary with its attributes (or
descriptors for those attributes) SIP only creates objects for those attributes
when they are actually needed.  This is done to reduce the memory footprint and
start up time when used to wrap large libraries with hundreds of classes and
tens of thousands of attributes.

SIP allows you to extend the handling of lazy attributes to your own attribute
types by allowing you to register an attribute getter handler (using
:c:func:`sipRegisterAttributeGetter()`).  This will be called just before a
type's dictionary is accessed for the first time.


Support for Python's Buffer Interface
-------------------------------------

SIP supports Python's buffer interface in that whenever C/C++ requires a
``char`` or ``char *`` type then any Python type that supports the buffer
interface (including ordinary Python strings) can be used.


Support for Wide Characters
---------------------------

SIP supports the use of wide characters (i.e. the ``wchar_t`` type).  Python's
C API includes support for converting between ``str`` objects and wide
character strings and arrays.  When converting from a ``str`` object to wide
characters SIP creates the string or array on the heap (using memory allocated
using :c:func:`sipMalloc()`).  This then raises the problem of how this memory
is subsequently freed.

The following describes how SIP handles this memory in the different situations
where this is an issue.

    - When a wide string or array is passed to a function or method then the
      memory is freed (using :c:func:`sipFree()`) after that function or method
      returns.

    - When a wide string or array is returned from a virtual method then SIP
      does not free the memory until the next time the method is called.

    - When an assignment is made to a wide string or array instance variable
      then SIP does not first free the instance's current string or array.


.. _ref-gil:

The Python Global Interpreter Lock
----------------------------------

Python's Global Interpretor Lock (GIL) must be acquired before calls can be
made to the Python API.  It should also be released when a potentially
blocking call to C/C++ library is made in order to allow other Python threads
to be executed.  In addition, some C/C++ libraries may implement their own
locking strategies that conflict with the GIL causing application deadlocks.
SIP provides ways of specifying when the GIL is released and acquired to
ensure that locking problems can be avoided.

SIP always ensures that the GIL is acquired before making calls to the Python
API.  By default SIP does not release the GIL when making calls to the C/C++
library being wrapped.  The :fanno:`ReleaseGIL` annotation can be used to
override this behaviour when required.

If the ``release-gil`` key is set to ``true`` in the bindings-specific section
of the ``pyproject.toml`` file then (for that set of bindings) then the default
behaviour is changed and SIP releases the GIL every time is makes calls to the
C/C++ library being wrapped.  The :fanno:`HoldGIL` annotation can be used to
override this behaviour when required.


.. _ref-subclass-convertors:

Writing :directive:`%ConvertToSubClassCode`
-------------------------------------------

When SIP needs to wrap a C++ class instance it first checks to make sure it
hasn't already done so.  If it has then it just returns a new reference to the
corresponding Python object.  Otherwise it creates a new Python object of the
appropriate type.  In C++ a function may be defined to return an instance of a
certain class, but can often return a sub-class instead.

The :directive:`%ConvertToSubClassCode` directive is used to specify
handwritten code that exploits any available real-time type information (RTTI)
to see if there is a more specific Python type that can be used when wrapping
the C++ instance.  The RTTI may be provided by the compiler or by the C++
instance itself.

The directive is included in the specification of one of the classes that the
handwritten code handles the type conversion for.  It doesn't matter which
one, but a sensible choice would be the one at the root of that class
hierarchy in the module.

.. note::

    In a future version of SIP this use of the directive will be deprecated and
    it will instead be placed outside any class specification.

If a class hierarchy extends over a number of modules then this directive
should be used in each of those modules to handle the part of the hierarchy
defined in that module.  SIP will ensure that the different pieces of code are
called in the right order to determine the most specific Python type to use.

A class has at least one convertor if it or any super-class defines
:directive:`%ConvertToSubClassCode`.  A convertor has a base class.  If a class
that defines :directive:`%ConvertToSubClassCode` does not have a super-class
that defines :directive:`%ConvertToSubClassCode` then that class is the base
class.  Otherwise the base class is that of the right-most super-class that has
a convertor.  In this case the :directive:`%ConvertToSubClassCode` extends all
other convertors with the same base class.

Consider the following class hierarchy::

    A
      \
       B*     C*
         \  /   \
          D      E
        /   \
      F       G*

The classes marked with an asterisk define :directive:`%ConvertToSubClassCode`.

Classes ``A`` to ``F`` are implemented in module ``X``.  Class ``G`` is
implemented in module ``Y``.

We can say the following:

- ``A`` has no convertor, all other classes do.

- The base class of ``B``'s :directive:`%ConvertToSubClassCode` is ``B``.

- The base class of ``C``'s :directive:`%ConvertToSubClassCode` is ``C``.

- ``D`` and ``F`` have two convertors ``B`` and ``C``.

- ``E``'s convertor is ``C``.

- The base class of ``G``'s :directive:`%ConvertToSubClassCode` is ``C``.

- ``B``'s :directive:`%ConvertToSubClassCode` must handle instances of ``B``,
  ``D`` and ``F`` (i.e. those sub-classes of its base class defined in the same
  module).

- ``C``'s :directive:`%ConvertToSubClassCode` must handle instances of ``C``,
  ``D``, ``E`` and ``F``.

- ``G``'s :directive:`%ConvertToSubClassCode` must handle instances of ``G``.

A convertor is invoked when SIP needs to wrap a C++ instance and the type of
that instance is a sub-class of the convertor's base class.  The convertor is
passed a pointer to the instance cast to the base class.  The convertor then,
if possible, casts that pointer to an instance of a sub-class of its original
class.  It also returns a pointer to the corresponding
:ref:`generated type structure <ref-type-structures>`.

It is possible for a convertor to switch to another convertor.  This can avoid
duplication of convertor code where there is multiple inheritance.

When more than one convertor may be invoked they are done so in the order that
reflects the module hierarchy.  When the convertors are defined in the same
module then the order is undefined.  Convertors must be written with this mind.

Given the class hierarchy shown above, lets say that SIP needs to wrap an
instance of known to be of class ``D`` but is actually of class ``F``.  We want
the conversion mechanism to recognise that fact and return a Python object of
type ``F``.  The following steps are taken:

- ``G``'s :directive:`%ConvertToSubClassCode` is invoked and passed the pointer
  to ``D`` cast to ``C``.  This convertor only recognises instances of class
  ``G`` and so returns a value that indicates it was unable to perform a
  conversion.

- SIP will now invoke either ``B``'s :directive:`%ConvertToSubClassCode` or
  ``C``'s :directive:`%ConvertToSubClassCode`.  As they are defined in the same
  module which is chosen is undefined.  Let's assume it is the ``C`` convertor
  that is invoked.

- The convertor recognises that the instance is of class ``D`` (rather than
  ``C`` or ``E``).  It must also determine whether this really is ``D`` or
  whether it is actually ``F``.  Of course ``B``'s
  :directive:`%ConvertToSubClassCode` must also make the same distinction.
  Rather than possibly duplicating the required code in both convertors the
  ``C`` convertor switches to the ``B`` convertor.  It does this by casting the
  pointer it is trying to convert to ``B`` and returns ``B``'s
  :ref:`generated type structure <ref-type-structures>`.
