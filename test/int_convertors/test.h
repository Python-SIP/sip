#if !defined(_TEST_H)
#define _TEST_H

class Test
{
public:
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

    enum class Willy {
        willy = 10
    };

    enum Poo {
        poo = 20
    };

    //void test(Willy w) {printf("Got Willy: %d\n", static_cast<int>(w));}
    //void test(Poo p) {printf("Got Poo: %d\n", static_cast<int>(p));}
    bool var_bool;
    int var_int;
};

#endif
