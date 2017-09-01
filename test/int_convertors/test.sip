%Module(name=test)

//%Import QtCore/QtCoremod.sip

class Test
{
%TypeHeaderCode
#include "test.h"
%End

public:
    static unsigned signed_char_sizeof();
    signed char signed_char_get() /PyInt/;
    virtual signed char signed_char_virt() /PyInt/;
    static void signed_char_set(signed char /PyInt/);
    signed char signed_char_var /PyInt/;

    static unsigned short_sizeof();
    short short_get();
    virtual short short_virt();
    static void short_set(short);
    short short_var;

    enum class Willy {
        willy
    };

    enum Poo {
        poo
    };

    //void test(Willy);
    //void test(Poo);
    bool var_bool;
    int var_int;
};
