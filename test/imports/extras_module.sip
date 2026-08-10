// The SIP implementation of the extras_module test module.


%Module(name=imports.extras_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"

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
