// The declaration of the class test class defined in the core_module test
// module.


#if !defined(_KLASS_H)
#define _KLASS_H

class Klass
{
public:
    Klass() : int_attr(0) {}

    int get_int_attr() const {return int_attr;}
    void set_int_attr(int value) {int_attr = value;}

    int operator+(int n) const {return int_attr + n;}

    int int_attr;
};

#endif
