// The SIP implementation of the extras_module test module.


%Module(name=subclass_convertors.extras_module)

%Import core_module.sip


%ModuleHeaderCode
#include "base_class.h"

class Subclass2 : public BaseClass
{
public:
    Subclass2() : BaseClass(2) {}

    virtual BaseClass *create_instance() {return new Subclass2();}
};
%End


class Subclass2 : public BaseClass
{
public:
    Subclass2();

%ConvertToSubClassCode
#if SIP_ABI_MAJOR_VERSION >= 14
    if (sipCpp->get_type() == 2)
        sipType = sipTypeID_Subclass2;
    else
        sipType = sipTypeID_Invalid;
#else
    if (sipCpp->get_type() == 2)
        sipType = sipType_Subclass2;
    else
        sipType = NULL;
#endif
%End
};
