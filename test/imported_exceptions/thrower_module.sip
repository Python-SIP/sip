// The SIP implementation of the thrower_module test module.


%Module(name=imported_exceptions.thrower_module)

%Import handler_module.sip


%ModuleHeaderCode

void throwException()
{
    throw std::exception();
};

%End


void throwException();
