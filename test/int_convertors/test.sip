%Module(name=test)

//%Import QtCore/QtCoremod.sip

class Test
{
%TypeHeaderCode
#include "test.h"
%End

public:
    enum class Scoped {
        scoped
    };
    Scoped scoped_get();
    virtual Scoped scoped_virt();
    static void scoped_set(Scoped);
    Scoped scoped_var;

    enum Named {
        named
    };
    Named named_get();
    virtual Named named_virt();
    static void named_set(Named);
    Named named_var;

    enum Named2 {
        named2 = 10
    };
    void named_overload_set(Named2);
    void named_overload_set(Named);
    bool named_overload;

    bool bool_get();
    virtual bool bool_virt();
    static void bool_set(bool);
    bool bool_var;

    static int char_lower();
    static int char_upper();
    char char_get() /PyInt/;
    virtual char char_virt() /PyInt/;
    static void char_set(char /PyInt/);
    char char_var /PyInt/;

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

    static unsigned unsigned_char_sizeof();
    unsigned char unsigned_char_get() /PyInt/;
    virtual unsigned char unsigned_char_virt() /PyInt/;
    static void unsigned_char_set(unsigned char /PyInt/);
    unsigned char unsigned_char_var /PyInt/;

    static unsigned unsigned_short_sizeof();
    unsigned short unsigned_short_get();
    virtual unsigned short unsigned_short_virt();
    static void unsigned_short_set(unsigned short);
    unsigned short unsigned_short_var;

    static unsigned unsigned_int_sizeof();
    unsigned int unsigned_int_get();
    virtual unsigned int unsigned_int_virt();
    static void unsigned_int_set(unsigned int);
    unsigned int unsigned_int_var;

    static unsigned unsigned_long_sizeof();
    unsigned long unsigned_long_get();
    virtual unsigned long unsigned_long_virt();
    static void unsigned_long_set(unsigned long);
    unsigned long unsigned_long_var;

    static unsigned unsigned_long_long_sizeof();
    unsigned long long unsigned_long_long_get();
    virtual unsigned long long unsigned_long_long_virt();
    static void unsigned_long_long_set(unsigned long long);
    unsigned long long unsigned_long_long_var;
};
