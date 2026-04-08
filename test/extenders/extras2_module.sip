// The SIP implementation of the extras2_module test module.


%Module(name=extenders.extras2_module)

%Import core_module.sip


%ModuleHeaderCode
#include "klass.h"

namespace NameSpace
{
    int thrice(int n);
};

int operator*(const Klass &lhs, int rhs);
%End


%ModuleCode
int NameSpace::thrice(int n)
{
    return 3 * n;
}

int operator*(const Klass &lhs, int rhs)
{
    return lhs.int_attr * rhs;
}
%End


namespace NameSpace
{
    int thrice(int n /Constrained/);
};

int operator*(const Klass &lhs, int rhs);
