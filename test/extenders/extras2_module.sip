// The SIP implementation of the extras2_module test module.


%Module(name=extenders.extras2_module)

%Import core_module.sip


%ModuleHeaderCode
namespace NameSpace
{
    int thrice(int n);
};
%End


%ModuleCode
int NameSpace::thrice(int n)
{
    return 3 * n;
}
%End


namespace NameSpace
{
    int thrice(int n /Constrained/);
};
