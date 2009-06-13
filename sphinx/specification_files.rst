SIP Specification Files
=======================

A SIP specification consists of some C/C++ type and function declarations and
some directives.  The declarations may contain annotations which provide SIP
with additional information that cannot be expressed in C/C++.  SIP does not
include a full C/C++ parser.

It is important to understand that a SIP specification describes the Python
API, i.e. the API available to the Python programmer when they ``import`` the
generated module.  It does not have to accurately represent the underlying
C/C++ library.  There is nothing wrong with omitting functions that make
little sense in a Python context, or adding functions implemented with
handwritten code that have no C/C++ equivalent.  It is even possible (and
sometimes necessary) to specify a different super-class hierarchy for a C++
class.  All that matters is that the generated code compiles properly.

In most cases the Python API matches the C/C++ API.  In some cases handwritten
code (see :directive:`%MethodCode`) is used to map from one to the other
without SIP having to know the details itself.  However, there are a few cases
where SIP generates a thin wrapper around a C++ method or constructor (see
:ref:`ref-derived-classes`) and needs to know the exact C++ signature.  To deal
with these cases SIP allows two signatures to be specified.  For example::

    class Klass
    {
    public:
        // The Python signature is a tuple, but the underlying C++ signature
        // is a 2 element array.
        Klass(SIP_PYTUPLE) [(int *)];
    %MethodCode
            int iarr[2];

            if (PyArg_ParseTuple(a0, "ii", &iarr[0], &iarr[1]))
            {
                // Note that we use the SIP generated derived class
                // constructor.
                Py_BEGIN_ALLOW_THREADS
                sipCpp = new sipKlass(iarr);
                Py_END_ALLOW_THREADS
            }
    %End
    };


Syntax Definition
-----------------

The following is a semi-formal description of the syntax of a specification
file.

.. parsed-literal::

    *specification* ::= {*module-statement*}

    *module-statement* ::= [*module-directive* | *statement*]

    *module-directive* ::= [
            :directive:`%CModule` |
            :directive:`%CompositeModule` |
            :directive:`%ConsolidatedModule` |
            :directive:`%Copying` |
            :directive:`%DefaultEncoding` |
            :directive:`%DefaultMetatype` |
            :directive:`%DefaultSupertype` |
            :directive:`%Doc` |
            :directive:`%ExportedDoc` |
            :directive:`%ExportedHeaderCode` |
            :directive:`%Feature` |
            :directive:`%Import` |
            :directive:`%Include` |
            :directive:`%InitialisationCode` |
            :directive:`%License` |
            :directive:`%MappedType` |
            :directive:`%Module` |
            :directive:`%ModuleCode` |
            :directive:`%ModuleHeaderCode` |
            :directive:`%OptionalInclude` |
            :directive:`%Platforms` |
            :directive:`%PreInitialisationCode` |
            :directive:`%PostInitialisationCode` |
            :directive:`%Timeline` |
            :directive:`%UnitCode` |
            *mapped-type-template*]

    *statement* :: [*class-statement* | *function* | *variable*]

    *class-statement* :: [
            :directive:`%If` |
            *class* |
            *class-template* |
            *enum* |
            *namespace* |
            *opaque-class* |
            *operator* |
            *struct* |
            *typedef* |
            *exception*]

    *class* ::= **class** *name* [**:** *super-classes*] [*class-annotations*]
            **{** {*class-line*} **};**

    *super-classes* ::= *name* [**,** *super-classes*]

    *class-line* ::= [
            *class-statement* |
            :directive:`%BIGetBufferCode` |
            :directive:`%BIGetReadBufferCode` |
            :directive:`%BIGetWriteBufferCode` |
            :directive:`%BIGetSegCountCode` |
            :directive:`%BIGetCharBufferCode` |
            :directive:`%BIReleaseBufferCode` |
            :directive:`%ConvertToSubClassCode` |
            :directive:`%ConvertToTypeCode` |
            :directive:`%GCClearCode` |
            :directive:`%GCTraverseCode` |
            :directive:`%PickleCode` |
            :directive:`%TypeCode` |
            :directive:`%TypeHeaderCode` |
            *constructor* |
            *destructor* |
            *method* |
            *static-method* |
            *virtual-method* |
            *special-method* |
            *operator* |
            *virtual-operator* |
            *class-variable* |
            **public:** |
            **public slots:** |
            **protected:** |
            **protected slots:** |
            **private:** |
            **private slots:** |
            **signals:**]

    *constructor* ::= [**explicit**] *name* **(** [*argument-list*] **)**
            [*exceptions*] [*function-annotations*]
            [*c++-constructor-signature*] **;** [:directive:`%MethodCode`]

    *c++-constructor-signature* ::= **[(** [*argument-list*] **)]**

    *destructor* ::= [**virtual**] **~** *name* **()** [*exceptions*] [**= 0**]
            [*function-annotations*] **;** [:directive:`%MethodCode`]
            [:directive:`%VirtualCatcherCode`]

    *method* ::= *type* *name* **(** [*argument-list*] **)** [**const**]
            [*exceptions*] [**= 0**] [*function-annotations*] [*c++-signature*]
            **;** [:directive:`%MethodCode`]

    *c++-signature* ::= **[** *type* **(** [*argument-list*] **)]**

    *static-method* ::= **static** *function*

    *virtual-method* ::= **virtual** *type* *name* **(** [*argument-list*] **)**
            [**const**] [*exceptions*] [**= 0**] [*function-annotations*]
            [*c++-signature*] **;** [:directive:`%MethodCode`]
            [:directive:`%VirtualCatcherCode`]

    *special-method* ::= *type* *special-method-name*
            **(** [*argument-list*] **)** [*function-annotations*] **;**
            [:directive:`%MethodCode`]

    *special-method-name* ::= [**__abs__** | **__add__** | **__and__** |
            **__bool__** | **__call__** | **__cmp__** | **__contains__** |
            **__delitem__** | **__div__** | **__eq__** | **__float__** |
            **__floordiv__** | **__ge__** | **__getitem__** | **__gt__** |
            **__hash__** | **__iadd__** | **__iand__** | **__idiv__** |
            **__ifloordiv__** | **__ilshift__** | **__imod__** | **__imul__** |
            **__index__** | **__int__** | **__invert__** | **__ior__** |
            **__irshift__** | **__isub__** | **__itruediv__** | **__ixor__** |
            **__le__** | **__len__** | **__long__** | **__lshift__** |
            **__lt__** | **__mod__** | **__mul__** | **__ne__** | **__neg__** |
            **__nonzero__** | **__or__** | **__pos__** | **__repr__** |
            **__rshift__** | **__setitem__** | **__str__** | **__sub__** |
            **__truediv__** | **__xor__**]

    *operator* ::= *operator-type*
            **(** [*argument-list*] **)** [**const**] [*exceptions*]
            [*function-annotations*] **;** [:directive:`%MethodCode`]

    *virtual-operator* ::= **virtual** *operator-type*
            **(** [*argument-list*] **)** [**const**] [*exceptions*] [**= 0**]
            [*function-annotations*] **;** [:directive:`%MethodCode`]
            [:directive:`%VirtualCatcherCode`]

    *operatator-type* ::= [ *operator-function* | *operator-cast* ]

    *operator-function* ::= *type* **operator** *operator-name*

    *operator-cast* ::= **operator** *type*

    *operator-name* ::= [**+** | **-** | ***** | **/** | **%** | **&** |
            **|** | **^** | **<<** | **>>** | **+=** | **-=** | ***=** |
            **/=** | **%=** | **&=** | **|=** | **^=** | **<<=** | **>>=** |
            **~** | **()** | **[]** | **<** | **<=** | **==** | **!=** |
            **>** | **>>=** | **=**]

    *class-variable* ::= [**static**] *variable*

    *class-template* :: = **template** **<** *type-list* **>** *class*

    *mapped-type-template* :: = **template** **<** *type-list* **>**
            :directive:`%MappedType`

    *enum* ::= **enum** [*name*] [*enum-annotations*] **{** {*enum-line*} **};**

    *enum-line* ::= [:directive:`%If` | *name* [*enum-annotations*] **,**

    *function* ::= *type* *name* **(** [*argument-list*] **)** [*exceptions*]
            [*function-annotations*] **;** [:directive:`%MethodCode`]

    *namespace* ::= **namespace** *name* **{** {*namespace-line*} **};**

    *namespace-line* ::= [:directive:`%TypeHeaderCode` | *statement*]

    *opaque-class* ::= **class** *scoped-name* **;**

    *struct* ::= **struct** *name* **{** {*class-line*} **};**

    *typedef* ::= **typedef** [*typed-name* | *function-pointer*]
            *typedef-annotations* **;**

    *variable*::= *typed-name* [*variable-annotations*] **;**
            [:directive:`%AccessCode`] [:directive:`%GetCode`]
            [:directive:`%SetCode`]

    *exception* ::= :directive:`%Exception` *exception-name* [*exception-base*]
            **{** [:directive:`%TypeHeaderCode`] :directive:`%RaiseCode` **};**

    *exception-name* ::= *scoped-name*

    *exception-base* ::= **(** [*exception-name* | *python-exception*] **)**

    *python-exception* ::= [**SIP_Exception** | **SIP_StopIteration** |
            **SIP_StandardError** | **SIP_ArithmeticError** |
            **SIP_LookupError** | **SIP_AssertionError** |
            **SIP_AttributeError** | **SIP_EOFError** |
            **SIP_FloatingPointError** | **SIP_EnvironmentError** |
            **SIP_IOError** | **SIP_OSError** | **SIP_ImportError** |
            **SIP_IndexError** | **SIP_KeyError** | **SIP_KeyboardInterrupt** |
            **SIP_MemoryError** | **SIP_NameError** | **SIP_OverflowError** |
            **SIP_RuntimeError** | **SIP_NotImplementedError** |
            **SIP_SyntaxError** | **SIP_IndentationError** | **SIP_TabError** |
            **SIP_ReferenceError** | **SIP_SystemError** | **SIP_SystemExit** |
            **SIP_TypeError** | **SIP_UnboundLocalError** |
            **SIP_UnicodeError** | **SIP_UnicodeEncodeError** |
            **SIP_UnicodeDecodeError** | **SIP_UnicodeTranslateError** |
            **SIP_ValueError** | **SIP_ZeroDivisionError** |
            **SIP_WindowsError** | **SIP_VMSError**]

    *exceptions* ::= **throw (** [*exception-list*] **)**

    *exception-list* ::= *scoped-name* [**,** *exception-list*]

    *argument-list* ::= *argument* [**,** *argument-list*] [**,** **...**]

    *argument* ::= [
            *type* [*name*] [*argument-annotations*] [*default-value*] |
            :stype:`SIP_ANYSLOT` [*default-value*] |
            :stype:`SIP_QOBJECT` |
            :stype:`SIP_RXOBJ_CON` |
            :stype:`SIP_RXOBJ_DIS` |
            :stype:`SIP_SIGNAL` [*default-value*] |
            :stype:`SIP_SLOT` [*default-value*] |
            :stype:`SIP_SLOT_CON` |
            :stype:`SIP_SLOT_DIS`]

    *default-value* ::= **=** *expression*

    *expression* ::= [*value* | *value* *binary-operator* *expression*]

    *value* ::= [*unary-operator*] *simple-value*

    *simple-value* ::= [*scoped-name* | *function-call* | *real-value* |
            *integer-value* | *boolean-value* | *string-value* |
            *character-value*]

    *typed-name*::= *type* *name*

    *function-pointer*::= *type* **(*** *name* **)(** [*type-list*] **)**

    *type-list* ::= *type* [**,** *type-list*]

    *function-call* ::= *scoped-name* **(** [*value-list*] **)**

    *value-list* ::= *value* [**,** *value-list*]

    *real-value* ::= a floating point number

    *integer-value* ::= a number

    *boolean-value* ::= [**true** | **false**]

    *string-value* ::= **"** {*character*} **"**

    *character-value* ::= **'** *character* **'**

    *unary-operator* ::= [**!** | **~** | **-** | **+**]

    *binary-operator* ::= [**-** | **+** | ***** | **/** | **&** | **|**]

    *argument-annotations* ::= see :ref:`ref-arg-annos`

    *class-annotations* ::= see :ref:`ref-class-annos`

    *enum-annotations* ::= see :ref:`ref-enum-annos`

    *function-annotations* ::= see :ref:`ref-function-annos`

    *typedef-annotations* ::= see :ref:`ref-typedef-annos`

    *variable-annotations* ::= see :ref:`ref-variable-annos`

    *type* ::= [**const**] *base-type* {*****} [**&**]

    *type-list* ::= *type* [**,** *type-list*]

    *base-type* ::= [*scoped-name* | *template* | **struct** *scoped-name* |
            **char** | **signed char** | **unsigned char** | **wchar_t** |
            **int** | **unsigned** | **unsigned int** |
            **short** | **unsigned short** |
            **long** | **unsigned long** |
            **long long** | **unsigned long long** |
            **float** | **double** |
            **bool** |
            **void** |
            :stype:`SIP_PYCALLABLE` |
            :stype:`SIP_PYDICT` |
            :stype:`SIP_PYLIST` |
            :stype:`SIP_PYOBJECT` |
            :stype:`SIP_PYSLICE` |
            :stype:`SIP_PYTUPLE` |
            :stype:`SIP_PYTYPE`]

    *scoped-name* ::= *name* [**::** *scoped-name*]

    *template* ::= *scoped-name* **<** *type-list* **>**

    *dotted-name* ::= *name* [**.** *dotted-name*]

    *name* ::= _A-Za-z {_A-Za-z0-9}

Here is a short list of differences between C++ and the subset supported by
SIP that might trip you up.

    - SIP does not support the use of ``[]`` in types.  Use pointers instead.

    - A global ``operator`` can only be defined if its first argument is a
      class or a named enum that has been wrapped in the same module.

    - Variables declared outside of a class are effectively read-only.

    - A class's list of super-classes doesn't not include any access specifier
      (e.g. ``public``).


Variable Numbers of Arguments
-----------------------------

SIP supports the use of ``...`` as the last part of a function signature.  Any
remaining arguments are collected as a Python tuple.


Additional SIP Types
--------------------

SIP supports a number of additional data types that can be used in Python
signatures.


.. sip-type:: SIP_ANYSLOT

This is both a ``const char *`` and a ``PyObject *`` that is used as the type
of the member instead of ``const char *`` in functions that implement the
connection or disconnection of an explicitly generated signal to a slot.
Handwritten code must be provided to interpret the conversion correctly.


.. sip-type:: SIP_PYCALLABLE

This is a ``PyObject *`` that is a Python callable object.


.. sip-type:: SIP_PYDICT

This is a ``PyObject *`` that is a Python dictionary object.


.. sip-type:: SIP_PYLIST

This is a ``PyObject *`` that is a Python list object.


.. sip-type:: SIP_PYOBJECT

This is a ``PyObject *`` of any Python type.


.. sip-type:: SIP_PYSLICE

This is a ``PyObject *`` that is a Python slice object.


.. sip-type:: SIP_PYTUPLE

This is a ``PyObject *`` that is a Python tuple object.


.. sip-type:: SIP_PYTYPE

This is a ``PyObject *`` that is a Python type object.


.. sip-type:: SIP_QOBJECT

This is a ``QObject *`` that is a C++ instance of a class derived from Qt's
``QObject`` class.


.. sip-type:: SIP_RXOBJ_CON

This is a ``QObject *`` that is a C++ instance of a class derived from Qt's
``QObject`` class.  It is used as the type of the receiver instead of ``const
QObject *`` in functions that implement a connection to a slot.


.. sip-type:: SIP_RXOBJ_DIS

This is a ``QObject *`` that is a C++ instance of a class derived from Qt's
``QObject`` class.  It is used as the type of the receiver instead of ``const
QObject *`` in functions that implement a disconnection from a slot.


.. sip-type:: SIP_SIGNAL

This is a ``const char *`` that is used as the type of the signal instead of
``const char *`` in functions that implement the connection or disconnection
of an explicitly generated signal to a slot.


.. sip-type:: SIP_SLOT

This is a ``const char *`` that is used as the type of the member instead of
``const char *`` in functions that implement the connection or disconnection
of an explicitly generated signal to a slot.


.. sip-type:: SIP_SLOT_CON

This is a ``const char *`` that is used as the type of the member instead of
``const char *`` in functions that implement the connection of an internally
generated signal to a slot.  The type includes a comma separated list of types
that is the C++ signature of of the signal.

To take an example, ``QAccel::connectItem()`` connects an internally generated
signal to a slot.  The signal is emitted when the keyboard accelerator is
activated and it has a single integer argument that is the ID of the
accelerator.  The C++ signature is::

    bool connectItem(int id, const QObject *receiver, const char *member);

The corresponding SIP specification is::

    bool connectItem(int, SIP_RXOBJ_CON, SIP_SLOT_CON(int));


.. sip-type:: SIP_SLOT_DIS

This is a ``const char *`` that is used as the type of the member instead of
``const char *`` in functions that implement the disconnection of an
internally generated signal to a slot.  The type includes a comma separated
list of types that is the C++ signature of of the signal.
