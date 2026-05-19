// The SIP implementation of the core_module test module.


%Module(name=subclass_convertors.core_module)


%ModuleHeaderCode
#include "base_class.h"

class Subclass1 : public BaseClass
{
public:
    Subclass1() : BaseClass(1) {}

    virtual BaseClass *create_instance() {return new Subclass1();}
};
%End


class BaseClass /Abstract/
{
public:
    BaseClass(int type = 0);
    virtual ~BaseClass();

    BaseClass *get_instance() const;
    void init_instance();

%ConvertToSubClassCode
#if SIP_ABI_MAJOR_VERSION >= 14
    if (sipCpp->get_type() == 1)
        sipType = sipTypeID_Subclass1;
    else
        sipType = sipTypeID_Invalid;
#else
    if (sipCpp->get_type() == 1)
        sipType = sipType_Subclass1;
    else
        sipType = NULL;
#endif
%End
};

class Subclass1 : public BaseClass
{
public:
    Subclass1();
};
