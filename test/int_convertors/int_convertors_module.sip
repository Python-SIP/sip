// The SIP implementation of the int_convertors_module tets module.


%Module(name=int_convertors_module)


%PostInitialisationCode
#if SIP_API_MAJOR_NR == 12
sipEnableOverflowChecking(1);
#endif
%End


%ModuleHeaderCode

#include <limits.h>


class IntConvertors
{
public:
    virtual ~IntConvertors() {}

    bool bool_get() {return bool_virt();}
    virtual bool bool_virt() {return false;}
    static void bool_set(bool) {}
    bool bool_var;

    static int char_lower() {return CHAR_MIN;}
    static int char_upper() {return CHAR_MAX;}
    char char_get() {return char_virt();}
    virtual char char_virt() {return 0;}
    static void char_set(char) {}
    char char_var;

    static unsigned signed_char_sizeof() {return sizeof (signed char);}
    signed char signed_char_get() {return signed_char_virt();}
    virtual signed char signed_char_virt() {return 0;}
    static void signed_char_set(signed char) {}
    signed char signed_char_var;

    static unsigned short_sizeof() {return sizeof (short);}
    short short_get() {return short_virt();}
    virtual short short_virt() {return 0;}
    static void short_set(short) {}
    short short_var;

    static unsigned int_sizeof() {return sizeof (int);}
    int int_get() {return int_virt();}
    virtual int int_virt() {return 0;}
    static void int_set(int) {}
    int int_var;

    static unsigned long_sizeof() {return sizeof (long);}
    long long_get() {return long_virt();}
    virtual long long_virt() {return 0;}
    static void long_set(long) {}
    long long_var;

    static unsigned long_long_sizeof() {return sizeof (long long);}
    long long long_long_get() {return long_long_virt();}
    virtual long long long_long_virt() {return 0;}
    static void long_long_set(long long) {}
    long long long_long_var;

    static unsigned unsigned_char_sizeof() {return sizeof (unsigned char);}
    unsigned char unsigned_char_get() {return unsigned_char_virt();}
    virtual unsigned char unsigned_char_virt() {return 0;}
    static void unsigned_char_set(unsigned char) {}
    unsigned char unsigned_char_var;

    static unsigned unsigned_short_sizeof() {return sizeof (unsigned short);}
    unsigned short unsigned_short_get() {return unsigned_short_virt();}
    virtual unsigned short unsigned_short_virt() {return 0;}
    static void unsigned_short_set(unsigned short) {}
    unsigned short unsigned_short_var;

    static unsigned unsigned_int_sizeof() {return sizeof (unsigned int);}
    unsigned int unsigned_int_get() {return unsigned_int_virt();}
    virtual unsigned int unsigned_int_virt() {return 0;}
    static void unsigned_int_set(unsigned int) {}
    unsigned int unsigned_int_var;

    static unsigned unsigned_long_sizeof() {return sizeof (unsigned long);}
    unsigned long unsigned_long_get() {return unsigned_long_virt();}
    virtual unsigned long unsigned_long_virt() {return 0;}
    static void unsigned_long_set(unsigned long) {}
    unsigned long unsigned_long_var;

    static unsigned unsigned_long_long_sizeof() {
        return sizeof (unsigned long long);
    }
    unsigned long long unsigned_long_long_get() {
        return unsigned_long_long_virt();
    }
    virtual unsigned long long unsigned_long_long_virt() {return 0;}
    static void unsigned_long_long_set(unsigned long long) {}
    unsigned long long unsigned_long_long_var;
};

%End


class IntConvertors
{
public:
    virtual ~IntConvertors();

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
