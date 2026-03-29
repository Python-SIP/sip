// The SIP implementation of the extras1_module test module.


%Module(name=extenders.extras1_module)

%Import core_module.sip


%ModuleHeaderCode
namespace NameSpace
{
    float twice(float n);
    float thrice(float n);

    extern int in_extras1;
};

int get_in_extras1();
%End


%ModuleCode
float NameSpace::twice(float n)
{
    return 2 * n;
}

float NameSpace::thrice(float n)
{
    return 3 * n;
}

int get_in_extras1()
{
    return NameSpace::in_extras1;
}

int NameSpace::in_extras1 = 33;
%End


namespace NameSpace
{
    float twice(float n);
    float thrice(float n);

    int in_extras1;
};

int get_in_extras1();
