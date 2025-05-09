// The bindings for the module implementing the exception support.


%Module(name=handler)


%Exception std::exception(SIP_Exception) /PyName=StdException, Default/
{
%TypeHeaderCode
#include <exception>
%End

%RaiseCode
    const char *detail = sipExceptionRef.what();
    SIP_BLOCK_THREADS
    PyErr_SetString(sipException_std_exception, detail);
    SIP_UNBLOCK_THREADS
%End
};
