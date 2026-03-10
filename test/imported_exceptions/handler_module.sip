// The SIP implementation of the handler_module test module.


%Module(name=imported_exceptions.handler_module)


%Exception std::exception(SIP_Exception) /PyName=StdException, Default/
{
%TypeHeaderCode
#include <exception>
%End

%RaiseCode
    const char *detail = sipExceptionRef.what();

    SIP_BLOCK_THREADS

#if SIP_ABI_MAJOR_VERSION >= 14
    PyErr_SetString((PyObject *)sipGetPyType(sipMS, sipTypeID_std_exception), detail);
#else
    PyErr_SetString(sipException_std_exception, detail);
#endif

    SIP_UNBLOCK_THREADS
%End
};
