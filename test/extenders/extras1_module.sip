// The SIP implementation of the extras1_module test module.


%Module(name=extenders.extras1_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%Import core_module.sip


%ModuleHeaderCode
#include "klass.h"

namespace NameSpace
{
    float twice(float n);
    float thrice(float n);

    extern int in_extras1;
};

int get_in_extras1();

double operator+(const Klass &lhs, double rhs);

class Cast
{
public:
    Cast(int n) : int_attr(n) {}

    operator Klass() const {
        Klass klass;
        klass.int_attr = int_attr;
        return klass;
    };

private:
    int int_attr;
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

int get_in_extras1()
{
    return NameSpace::in_extras1;
}

int NameSpace::in_extras1 = 33;

double operator+(const Klass &lhs, double rhs)
{
    return lhs.int_attr + rhs;
}
%End


namespace NameSpace
{
    float twice(float n);
    float thrice(float n);

    int in_extras1;
};

int get_in_extras1();

double operator+(const Klass &lhs, double rhs);

class Cast
{
public:
    Cast(int n);

    operator Klass() const;
};
