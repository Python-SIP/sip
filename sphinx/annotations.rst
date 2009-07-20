Annotations
===========

In this section we describe each of the annotations that can be used in
specification files.

Annotations can either be :ref:`argument annotations <ref-arg-annos>`,
:ref:`class annotations <ref-class-annos>`, :ref:`mapped type annotations
<ref-mapped-type-annos>`, :ref:`enum annotations <ref-enum-annos>`,
:ref:`exception annotations <ref-exception-annos>`, :ref:`function annotations
<ref-function-annos>`, :ref:`license annotations <ref-license-annos>`,
:ref:`typedef annotations <ref-typedef-annos>` or :ref:`variable annotations
<ref-variable-annos>` depending on the context in which they can be used.

Annotations are placed between forward slashes (``/``).  Multiple annotations
are comma separated within the slashes.

Annotations have a type and, possibly, a value.  The type determines the
format of the value.  The name of an annotation and its value are separated by
``=``.

Annotations can have one of the following types:

*boolean*
    This type of annotation has no value and is implicitly true.

*name*
    The value is a name that is compatible with a C/C++ identifier.  In some
    cases the value is optional.

*dotted name*
    The value is a name that is compatible with an identifier preceded by a
    Python scope.

*string*
    The value is a double quoted string.

The following example shows argument and function annotations::

    void exec(QWidget * /Transfer/) /ReleaseGIL, PyName=call_exec/;

Note that the current version of SIP does not complain about unknown
annotations, or annotations used out of their correct context.


.. _ref-arg-annos:

Argument Annotations
--------------------

.. argument-annotation:: AllowNone

    This boolean annotation specifies that the value of the corresponding
    argument (which should be either :stype:`SIP_PYCALLABLE`,
    :stype:`SIP_PYDICT`, :stype:`SIP_PYLIST`, :stype:`SIP_PYSLICE`,
    :stype:`SIP_PYTUPLE` or :stype:`SIP_PYTYPE`) may be ``None``.


.. argument-annotation:: Array

    This boolean annotation specifies that the corresponding argument (which
    should be either ``char *`` or ``unsigned char *``) refers to an array
    rather than a ``'\0'`` terminated string.  There must be a corresponding
    argument with the :aanno:`ArraySize` annotation specified.  The annotation
    may only be specified once in a list of arguments.

    The annotation also implies the :aanno:`Encoding` annotation with an
    encoding of ``"None"``.


.. argument-annotation:: ArraySize

    This boolean annotation specifies that the corresponding argument (which
    should be either ``short``, ``unsigned short``, ``int``, ``unsigned``,
    ``long`` or ``unsigned long``) refers to the size of an array.  There must
    be a corresponding argument with the :aanno:`Array` annotation specified.
    The annotation may only be specified once in a list of arguments.


.. argument-annotation:: Constrained

    Python will automatically convert between certain compatible types.  For
    example, if a floating pointer number is expected and an integer supplied,
    then the integer will be converted appropriately.  This can cause problems
    when wrapping C or C++ functions with similar signatures.  For example::

        // The wrapper for this function will also accept an integer argument
        // which Python will automatically convert to a floating point number.
        void foo(double);

        // The wrapper for this function will never get used.
        void foo(int);

    This boolean annotation specifies that the corresponding argument (which
    should be either ``bool``, ``int``, ``float``, ``double``, ``enum`` or a
    wrapped class) must match the type without any automatic conversions.  In
    the context of a wrapped class the invocation of any
    :directive:`%ConvertToTypeCode` is suppressed.

    The following example gets around the above problem::

        // The wrapper for this function will only accept floating point
        // numbers.
        void foo(double /Constrained/);

        // The wrapper for this function will be used for anything that Python
        // can convert to an integer, except for floating point numbers.
        void foo(int);


.. argument-annotation:: Encoding

    This string annotation specifies that the corresponding argument (which
    should be either ``char``, ``const char``, ``char *`` or ``const char *``)
    refers to an encoded character or ``'\0'`` terminated encoded string with
    the specified encoding.  The encoding can be either ``"ASCII"``,
    ``"Latin-1"``, ``"UTF-8"`` or ``"None"``.  An encoding of ``"None"`` means
    that the corresponding argument refers to an unencoded character or string.

    The default encoding is specified by the :directive:`%DefaultEncoding`
    directive.  If the directive is not specified then ``None`` is used.

    Python v3 will use the ``bytes`` type to represent the argument if the
    encoding is ``"None"`` and the ``str`` type otherwise.

    Python v2 will use the ``str`` type to represent the argument if the
    encoding is ``"None"`` and the ``unicode`` type otherwise.


.. argument-annotation:: GetWrapper

    This boolean annotation is only ever used in conjunction with handwritten
    code specified with the :directive:`%MethodCode` directive.  It causes an
    extra variable to be generated for the corresponding argument which is a
    pointer to the Python object that wraps the argument.

    See the :directive:`%MethodCode` directive for more detail.


.. argument-annotation:: In

    This boolean annotation is used to specify that the corresponding argument
    (which should be a pointer type) is used to pass a value to the function.

    For pointers to wrapped C structures or C++ class instances, ``char *`` and
    ``unsigned char *`` then this annotation is assumed unless the :aanno:`Out`
    annotation is specified.

    For pointers to other types then this annotation must be explicitly
    specified if required.  The argument will be dereferenced to obtain the
    actual value.

    Both :aanno:`In` and :aanno:`Out` may be specified for the same argument.


.. argument-annotation:: KeepReference

    This boolean annotation is used to specify that a reference to the
    corresponding argument should be kept to ensure that the object is not
    garbage collected.  If the method is called again with a new argument then
    the reference to the previous argument is discarded.  Note that ownership
    of the argument is not changed.


.. argument-annotation:: Out

    This boolean annotation is used to specify that the corresponding argument
    (which should be a pointer type) is used by the function to return a value
    as an element of a tuple.

    For pointers to wrapped C structures or C++ class instances, ``char *`` and
    ``unsigned char *`` then this annotation must be explicitly specified if
    required.

    For pointers to other types then this annotation is assumed unless the
    :aanno:`In` annotation is specified.

    Both :aanno:`In` and :aanno:`Out` may be specified for the same argument.


.. argument-annotation:: ResultSize

    This boolean annotation is used with functions or methods that return a
    ``void *`` or ``const void *``.  It identifies an argument that defines the
    size of the block of memory whose address is being returned.  This allows
    the ``sip.voidptr`` object that wraps the address to support the Python
    buffer protocol and allows the memory to be read and updated when wrapped
    by the Python ``buffer()`` builtin.


.. argument-annotation:: SingleShot

    This boolean annotation is used only with arguments of type
    :stype:`SIP_RXOBJ_CON` to specify that the signal connected to the slot
    will only ever be emitted once.  This prevents a certain class of memory
    leaks.


.. argument-annotation:: Transfer

    This boolean annotation is used to specify that ownership of the
    corresponding argument (which should be a wrapped C structure or C++ class
    instance) is transferred from Python to C++.  In addition, if the argument
    is of a class method, then it is associated with the class instance with
    regard to the cyclic garbage collector.

    See :ref:`ref-object-ownership` for more detail.


.. argument-annotation:: TransferBack

    This boolean annotation is used to specify that ownership of the
    corresponding argument (which should be a wrapped C structure or C++ class
    instance) is transferred back to Python from C++.  In addition, any
    association of the argument with regard to the cyclic garbage collector
    with another instance is removed.

    See :ref:`ref-object-ownership` for more detail.


.. argument-annotation:: TransferThis

    This boolean annotation is only used in C++ constructors or methods.  In
    the context of a constructor or factory method it specifies that ownership
    of the instance being created is transferred from Python to C++ if the
    corresponding argument (which should be a wrapped C structure or C++ class
    instance) is not ``None``.  In addition, the newly created instance is
    associated with the argument with regard to the cyclic garbage collector.

    In the context of a non-factory method it specifies that ownership of
    ``this`` is transferred from Python to C++ if the corresponding argument is
    not ``None``.  If it is ``None`` then ownership is transferred to Python.

    The annotation may be used more that once, in which case ownership is
    transferred to last instance that is not ``None``.

    See :ref:`ref-object-ownership` for more detail.


.. _ref-class-annos:

Class Annotations
-----------------

.. class-annotation:: Abstract

    This boolean annotation is used to specify that the class has additional
    pure virtual methods that have not been specified and so it cannot be
    instantiated or sub-classed from Python.


.. class-annotation:: AllowNone

    .. versionadded:: 4.8.2

    Normally when a Python object is converted to a C/C++ instance ``None``
    is handled automatically before the class's
    :directive:`%ConvertToTypeCode` is called.  This boolean annotation
    specifies that the handling of ``None`` will be left to the
    :directive:`%ConvertToTypeCode`.  The annotation is ignored if the class
    does not have any :directive:`%ConvertToTypeCode`.


.. class-annotation:: DelayDtor

    This boolean annotation is used to specify that the class's destructor
    should not be called until the Python interpreter exits.  It would normally
    only be applied to singleton classes.

    When the Python interpreter exits the order in which any wrapped instances
    are garbage collected is unpredictable.  However, the underlying C or C++
    instances may need to be destroyed in a certain order.  If this annotation
    is specified then when the wrapped instance is garbage collected the C or
    C++ instance is not destroyed but instead added to a list of delayed
    instances.  When the interpreter exits then the function
    :cfunc:`sipDelayedDtors()` is called with the list of delayed instances.
    :cfunc:`sipDelayedDtors()` can then choose to call (or ignore) the
    destructors in any desired order.

    The :cfunc:`sipDelayedDtors()` function must be specified using the
    :directive:`%ModuleCode` directive.

.. cfunction:: void sipDelayedDtors(const sipDelayedDtor *dd_list)

    :param dd_list:
        the linked list of delayed instances.

.. ctype:: sipDelayedDtor

    This structure describes a particular delayed destructor.

    .. cmember:: const char *dd_name

        This is the name of the class excluding any package or module name.

    .. cmember:: void *dd_ptr

        This is the address of the C or C++ instance to be destroyed.  It's
        exact type depends on the value of :cmember:`dd_isderived`.

    .. cmember:: int dd_isderived

        This is non-zero if the type of :cmember:`dd_ptr` is actually the
        generated derived class.  This allows the correct destructor to be
        called.  See :ref:`ref-derived-classes`.

    .. cmember:: sipDelayedDtor *dd_next

        This is the address of the next entry in the list or zero if this is
        the last one.

    Note that the above applies only to C and C++ instances that are owned by
    Python.


.. class-annotation:: Deprecated

    This boolean annotation is used to specify that the class is deprecated.
    It is the equivalent of annotating all the class's constructors, function
    and methods as being deprecated.


.. class-annotation:: External

    This boolean annotation is used to specify that the class is defined in
    another module.  Declarations of external classes are private to the module
    in which they appear.


.. class-annotation:: Metatype

    This dotted name annotation specifies the name of the Python type object
    (i.e. the value of the ``tp_name`` field) used as the meta-type used when
    creating the type object for this C structure or C++ type.

    See the section :ref:`ref-types-metatypes` for more details.


.. class-annotation:: NoDefaultCtors

    This boolean annotation is used to suppress the automatic generation of
    default constructors for the class.


.. class-annotation:: PyName

    This name annotation specifies an alternative name for the class being
    wrapped which is used when it is referred to from Python.  It is required
    when a class name is the same as a Python keyword.  It may also be used to
    avoid name clashes with other objects (e.g. enums, exceptions, functions)
    that have the same name in the same C++ scope.


.. class-annotation:: Supertype

    This dotted name annotation specifies the name of the Python type object
    (i.e. the value of the ``tp_name`` field) used as the super-type used when
    creating the type object for this C structure or C++ type.

    See the section :ref:`ref-types-metatypes` for more details.


.. _ref-mapped-type-annos:

Mapped Type Annotations
-----------------------

.. mapped-type-annotation:: NoRelease

    This boolean annotation is used to specify that the mapped type does not
    support the :cfunc:`sipReleaseType()` function.  Any
    :directive:`%ConvertToTypeCode` should not create temporary instances of
    the mapped type, i.e. it should not return :cmacro:`SIP_TEMPORARY`.


.. _ref-enum-annos:

Enum Annotations
----------------

.. enum-annotation:: PyName

    This name annotation specifies an alternative name for the enum or enum
    member being wrapped which is used when it is referred to from Python.  It
    is required when an enum or enum member name is the same as a Python
    keyword.  It may also be used to avoid name clashes with other objects
    (e.g. classes, exceptions, functions) that have the same name in the same
    C++ scope.


.. _ref-exception-annos:

Exception Annotations
---------------------

.. exception-annotation:: PyName

    This name annotation specifies an alternative name for the exception being
    defined which is used when it is referred to from Python.  It is required
    when an exception name is the same as a Python keyword.  It may also be
    used to avoid name clashes with other objects (e.g. classes, enums,
    functions) that have the same name.


.. _ref-function-annos:

Function Annotations
--------------------

.. function-annotation:: AutoGen

    This optional name annotation is used with class methods to specify that
    the method be automatically included in all sub-classes.  The value is the
    name of a feature (specified using the :directive:`%Feature` directive)
    which must be enabled for the method to be generated.


.. function-annotation:: Default

    This boolean annotation is only used with C++ constructors.  Sometimes SIP
    needs to create a class instance.  By default it uses a constructor with no
    compulsory arguments if one is specified.  (SIP will automatically generate
    a constructor with no arguments if no constructors are specified.)  This
    annotation is used to explicitly specify which constructor to use.  Zero is
    passed as the value of any arguments to the constructor.


.. function-annotation:: Deprecated

    This boolean annotation is used to specify that the constructor or function
    is deprecated.  A deprecation warning is issued whenever the constructor or
    function is called.


.. function-annotation:: Factory

    This boolean annotation specifies that the value returned by the function
    (which should be a wrapped C structure or C++ class instance) is a newly
    created instance and is owned by Python.

    See :ref:`ref-object-ownership` for more detail.


.. function-annotation:: HoldGIL

    This boolean annotation specifies that the Python Global Interpreter Lock
    (GIL) is not released before the call to the underlying C or C++ function.
    See :ref:`ref-gil` and the :fanno:`ReleaseGIL` annotation.


.. function-annotation:: NewThread

    This boolean annotation specifies that the function will create a new
    thread.


.. function-annotation:: NoArgParser

    This boolean annotation is used with global functions to specify that the
    supplied :directive:`%MethodCode` will handle the parsing of the arguments.


.. function-annotation:: NoDerived

    This boolean annotation is only used with C++ constructors.  In many cases
    SIP generates a derived class for each class being wrapped (see
    :ref:`ref-derived-classes`).  This derived class contains constructors with
    the same C++ signatures as the class being wrapped.  Sometimes you may want
    to define a Python constructor that has no corresponding C++ constructor.
    This annotation is used to suppress the generation of the constructor in
    the derived class.


.. function-annotation:: Numeric

    This boolean annotation specifies that the operator should be interpreted
    as a numeric operator rather than a sequence operator.  Python uses the
    ``+`` operator for adding numbers and concatanating sequences, and the
    ``*`` operator for multiplying numbers and repeating sequences.  SIP tries
    to work out which is meant by looking at other operators that have been
    defined for the type.  If it finds either ``-``, ``-=``, ``/``, ``/=``,
    ``%`` or ``%=`` defined then it assumes that ``+``, ``+=``, ``*`` and
    ``*=`` should be numeric operators.  Otherwise, if it finds either ``[]``,
    :meth:`__getitem__`, :meth:`__setitem__` or :meth:`__delitem__` defined
    then it assumes that they should be sequence operators.  This annotation is
    used to force SIP to treat the operator as numeric.


.. function-annotation:: PostHook

    This name annotation is used to specify the name of a Python builtin that
    is called immediately after the call to the underlying C or C++ function or
    any handwritten code.  The builtin is not called if an error occurred.  It
    is primarily used to integrate with debuggers.


.. function-annotation:: PreHook

    This name annotation is used to specify the name of a Python builtin that
    is called immediately after the function's arguments have been successfully
    parsed and before the call to the underlying C or C++ function or any
    handwritten code.  It is primarily used to integrate with debuggers.


.. function-annotation:: PyName

    This name annotation specifies an alternative name for the function being
    wrapped which is used when it is referred to from Python.  It is required
    when a function or method name is the same as a Python keyword.  It may
    also be used to avoid name clashes with other objects (e.g. classes, enums,
    exceptions) that have the same name in the same C++ scope.


.. function-annotation:: ReleaseGIL

    This boolean annotation specifies that the Python Global Interpreter Lock
    (GIL) is released before the call to the underlying C or C++ function and
    reacquired afterwards.  It should be used for functions that might block or
    take a significant amount of time to execute.  See :ref:`ref-gil` and the
    :fanno:`HoldGIL` annotation.


.. function-annotation:: Transfer

    This boolean annotation specifies that ownership of the value returned by
    the function (which should be a wrapped C structure or C++ class instance)
    is transferred to C++.  It is only used in the context of a class
    constructor or a method.

    In the case of methods returned values (unless they are new references to
    already wrapped values) are normally owned by C++ anyway.  However, in
    addition, an association between the returned value and the instance
    containing the method is created with regard to the cyclic garbage
    collector.

    See :ref:`ref-object-ownership` for more detail.


.. function-annotation:: TransferBack

    This boolean annotation specifies that ownership of the value returned by
    the function (which should be a wrapped C structure or C++ class instance)
    is transferred back to Python from C++.  Normally returned values (unless
    they are new references to already wrapped values) are owned by C++.  In
    addition, any association of the returned value with regard to the cyclic
    garbage collector with another instance is removed.

    See :ref:`ref-object-ownership` for more detail.


.. function-annotation:: TransferThis

    This boolean annotation specifies that ownership of ``this`` is transferred
    from Python to C++.

    See :ref:`ref-object-ownership` for more detail.


.. _ref-license-annos:

License Annotations
-------------------

.. license-annotation:: Licensee

    This optional string annotation specifies the license's licensee.  No
    restrictions are placed on the contents of the string.

    See the :directive:`%License` directive.


.. license-annotation:: Signature

    This optional string annotation specifies the license's signature.  No
    restrictions are placed on the contents of the string.

    See the :directive:`%License` directive.


.. license-annotation:: Timestamp

    This optional string annotation specifies the license's timestamp.  No
    restrictions are placed on the contents of the string.

    See the :directive:`%License` directive.


.. license-annotation:: Type

    This string annotation specifies the license's type.  No restrictions are
    placed on the contents of the string.

    See the :directive:`%License` directive.


.. _ref-typedef-annos:

Typedef Annotations
-------------------

.. typedef-annotation:: NoTypeName

    This boolean annotation specifies that the definition of the type rather
    than the name of the type being defined should be used in the generated
    code.

    Normally a typedef would be defined as follows::

        typedef bool MyBool;

    This would result in ``MyBool`` being used in the generated code.

    Specifying the annotation means that ``bool`` will be used in the generated
    code instead.


.. _ref-variable-annos:

Variable Annotations
--------------------

.. variable-annotation:: PyName

    This name annotation specifies an alternative name for the variable being
    wrapped which is used when it is referred to from Python.  It is required
    when a variable name is the same as a Python keyword.  It may also be used
    to avoid name clashes with other objects (e.g. classes, functions) that
    have the same name in the same C++ scope.
