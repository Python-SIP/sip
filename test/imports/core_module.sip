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
