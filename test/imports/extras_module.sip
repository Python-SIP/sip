// The SIP implementation of the extras_module test module.


%Module(name=imports.extras_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%Import core_module.sip


%ModuleHeaderCode
#include "base_class.h"

class Subclass : public BaseClass
{
public:
    Subclass() {}
};

class ExternalClass
{
public:
    ExternalClass() {}
};
%End


class Subclass : public BaseClass
{
public:
    Subclass();
};

class ExternalClass
{
public:
    ExternalClass();
};
