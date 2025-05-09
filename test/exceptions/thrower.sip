// The bindings for the module invoking the exception support.


%Module(name=thrower)

%Import handler.sip


%ModuleHeaderCode

void throwException()
{
    throw std::exception();
};

%End


void throwException();
