// The SIP implementation of the core_module test module.


%Module(name=extenders.core_module)


%ModuleHeaderCode
namespace NameSpace
{
    int twice(int n);
};
%End


%ModuleCode
int NameSpace::twice(int n)
{
    return 2 * n;
}
%End


namespace NameSpace
{
    int twice(int n /Constrained/);
};
