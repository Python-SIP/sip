// The SIP implementation of the core_module test module.


%Module(name=imports.core_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


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
