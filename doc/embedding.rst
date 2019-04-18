Using the C API when Embedding
==============================

The :ref:`C API <ref-c-api>` is intended to be called from handwritten code in
SIP generated modules.  However it is also often necessary to call it from C or
C++ applications that embed the Python interpreter and need to pass C or C++
instances between the application and the interpreter.

The API is exported by the SIP module as a ``sipAPIDef`` data structure
containing a set of function pointers.  The data structure is defined in the
SIP header file ``sip.h``.  When using Python v2.7, or Python v3.1 or later the
data structure is wrapped as a Python ``PyCapsule`` object.  When using other
versions of Python the data structure is wrapped as a Python ``PyCObject``
object.  It is referenced by the name ``_C_API`` in the SIP module dictionary.

Each member of the data structure is a pointer to one of the functions of the
SIP API.  The name of the member can be derived from the function name by
replacing the ``sip`` prefix with ``api`` and converting each word in the
name to lower case and preceding it with an underscore.  For example:

    ``sipExportSymbol`` becomes ``api_export_symbol``

    ``sipWrapperCheck`` becomes ``api_wrapper_check``

Note that the type objects that SIP generates for a wrapped module (see
:ref:`ref-type-structures` and :ref:`ref-exception-objects`) cannot be refered
to directly and must be obtained using the :c:func:`sipFindType()` function.
Of course, the corresponding modules must already have been imported into the
interpreter.

The following code fragment shows how to get a pointer to the ``sipAPIDef``
data structure::

    #include <sip.h>

    const sipAPIDef *get_sip_api()
    {
        return (const sipAPIDef *)PyCapsule_Import("sip._C_API", 0);
    }

``"sip._C_API"`` should be replaced by the fully qualified name of the ``sip``
module.
