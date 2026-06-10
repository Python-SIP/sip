// The SIP implementation of the core_module test module.


%Module(name=imports.core_module)


%ModuleCode
class ExternalClass;
ExternalClass *get_external() {return 0;}

class UndefinedExternalClass;
void check_undefined(const UndefinedExternalClass *u) {(void)u;}
%End


class ExternalClass /External/;
ExternalClass *get_external();

class UndefinedExternalClass /External/;
void check_undefined(const UndefinedExternalClass *u);

class BaseClass
{
%TypeHeaderCode
#include "base_class.h"
%End

public:
    BaseClass();
};
