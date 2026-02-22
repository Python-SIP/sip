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

namespace NameSpace
{
    int get_extras_value();
};
%End


class Subclass : public BaseClass
{
public:
    Subclass();
};


%ModuleCode
int NameSpace::get_extras_value()
{
    return 20;
}
%End

namespace NameSpace
{
%TypeHeaderCode
#include "base_class.h"
%End

    int get_extras_value();
};
