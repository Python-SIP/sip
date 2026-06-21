// The SIP implementation of the extras_module test module.


%Module(name=subclass_convertors.extras_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%Import core_module.sip


%ModuleHeaderCode
#include "base_class.h"

class Subclass2 : public BaseClass
{
public:
    Subclass2() : BaseClass(2) {}

    virtual BaseClass *create_instance() {return new Subclass2();}
};

class AnotherBase
{
public:
    AnotherBase() {}
};

class Subclass3 : public BaseClass, public AnotherBase
{
public:
    Subclass3(int t = 3) : BaseClass(t) {}

    virtual BaseClass *create_instance() {return new Subclass3();}
};

class Subclass4 : public Subclass3
{
public:
    Subclass4() : Subclass3(4) {}

    virtual BaseClass *create_instance() {return new Subclass4();}
};
%End

class Subclass2 : public BaseClass
{
public:
    Subclass2();

%ConvertToSubClassCode
    if (sipCpp->get_type() == 2)
        sipType = sipType_Subclass2;
    else
        sipType = sipType_Invalid;
%End
};

class AnotherBase
{
public:
    AnotherBase();
};

class Subclass3 : public BaseClass, public AnotherBase
{
public:
    Subclass3();

%ConvertToSubClassCode
    if (sipCpp->get_type() == 3)
        sipType = sipType_Subclass3;
    else
        sipType = sipType_Invalid;
%End
};

class Subclass4 : public Subclass3
{
public:
    Subclass4();

%ConvertToSubClassCode
    if (sipCpp->get_type() == 4)
        sipType = sipType_Subclass4;
    else
        sipType = sipType_Invalid;
%End
};
