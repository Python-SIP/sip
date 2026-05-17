// The SIP implementation of the core_module test module.


%Module(name=imports.core_module)


%ModuleCode
class ExternalClass;

ExternalClass *get_external() {return 0;}
%End


class ExternalClass /External/;
ExternalClass *get_external();

class BaseClass
{
%TypeHeaderCode
#include "base_class.h"
%End

public:
    BaseClass();
};
