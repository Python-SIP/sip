// The SIP implementation of the extras_module test module.


%Module(name=imports.extras_module)

%Import core_module.sip


%ModuleHeaderCode
#include "base_class.h"

class Subclass : public BaseClass
{
public:
    Subclass() {}
};
%End


class Subclass : public BaseClass
{
public:
    Subclass();
};
