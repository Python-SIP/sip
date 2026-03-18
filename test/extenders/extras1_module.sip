// The SIP implementation of the extras1_module test module.


%Module(name=extenders.extras1_module)

%Import core_module.sip


%ModuleHeaderCode
namespace NameSpace
{
    float twice(float n);
    float thrice(float n);
};
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
%End


namespace NameSpace
{
    float twice(float n);
    float thrice(float n);
};
