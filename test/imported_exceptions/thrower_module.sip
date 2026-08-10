// The SIP implementation of the thrower_module test module.


%Module(name=imported_exceptions.thrower_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"

%Import handler_module.sip


%ModuleHeaderCode

void throwException()
{
    throw std::exception();
};

%End


void throwException() /ReleaseGIL/;
