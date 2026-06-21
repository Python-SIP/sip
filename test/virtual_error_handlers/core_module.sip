// The SIP implementation of the core_module test module.


%Module(name=virtual_error_handlers.core_module,
        default_VirtualErrorHandler=default, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%VirtualErrorHandler(name=default)
    PyErr_Clear();
    PyObject_SetAttrString((PyObject *)sipPySelf, "default_used", Py_True);
%End
