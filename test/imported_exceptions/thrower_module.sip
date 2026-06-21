// The SIP implementation of the thrower_module test module.


%Module(name=imported_exceptions.thrower_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%Import handler_module.sip


%ModuleHeaderCode

void throwException()
{
    throw std::exception();
};

%End


void throwException() /ReleaseGIL/;
