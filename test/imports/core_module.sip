// The SIP implementation of the core_module test module.


%Module(name=imports.core_module)


class BaseClass
{
%TypeHeaderCode
#include "base_class.h"
%End

public:
    BaseClass();
};


%ModuleCode
int NameSpace::get_core_value()
{
    return 10;
}
%End

namespace NameSpace
{
%TypeHeaderCode
#include "base_class.h"
%End

    int get_core_value();
};
