// The SIP implementation of the handler_module test module.


%Module(name=handler_module)


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
