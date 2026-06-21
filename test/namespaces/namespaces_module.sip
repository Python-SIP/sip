// The SIP implementation of the namespaces_module test module.


%Module(name=namespaces_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%ModuleHeaderCode
namespace NS
{
    extern int attr;
    int get_attr();
};
%End


%ModuleCode
int NS::attr = 0;

int NS::get_attr()
{
    return NS::attr;
}
%End


namespace NS
{
    int attr;
    int get_attr();
};
