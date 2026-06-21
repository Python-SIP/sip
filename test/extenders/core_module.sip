// The SIP implementation of the core_module test module.


%Module(name=extenders.core_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


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

class Klass
{
%TypeHeaderCode
#include "klass.h"
%End

public:
    Klass();

    int get_int_attr() const;
    void set_int_attr(int value);

    int operator+(int n /Constrained/) const;
};
