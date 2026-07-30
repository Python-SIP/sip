// The SIP implementation of the handler_module test module.


%Module(name=imported_exceptions.handler_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%Exception std::exception(SIP_Exception) /PyName=StdException, Default/
{
%TypeHeaderCode
#include <exception>
%End

%RaiseCode
    const char *detail = sipExceptionRef.what();

#if SIP_ABI_MAJOR_VERSION >= 14
    PyObject *py_type = (PyObject *)sipGetPyTypeRef(sipType_std_exception);
    PyErr_SetString(py_type, detail);
    Py_DECREF(py_type);
#else
    PyErr_SetString(sipException_std_exception, detail);
#endif
%End
};
