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

    static unsigned int_sizeof();
    int int_get();
    virtual int int_virt();
    static void int_set(int);
    int int_var;

    static unsigned long_sizeof();
    long long_get();
    virtual long long_virt();
    static void long_set(long);
    long long_var;

    static unsigned long_long_sizeof();
    long long long_long_get();
    virtual long long long_long_virt();
    static void long_long_set(long long);
    long long long_long_var;

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
