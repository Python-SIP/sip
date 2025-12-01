// The SIP implementation of the class_callables_module test module.


%Module(name=class_callables_module)


%ModuleHeaderCode
#define NR_ELEMS 5

class Klass
{
public:
    Klass() : attr_int(0) {
        for (int i = 0; i < NR_ELEMS; i++)
            int_list[i] = i;

        list_len = NR_ELEMS;
    }

    static int get_s_attr_int() {return s_attr_int;}
    static void set_s_attr_int(int value) {s_attr_int = value;}

    int get_attr_int() const {return attr_int;}
    void set_attr_int(int value) {attr_int = value;}

    int operator-() {return -attr_int;}
    bool operator==(const Klass &other) {return attr_int == other.attr_int;}
    int operator()(int multiplier) {return attr_int * multiplier;}

    int count() const {return list_len;}
    int& operator[](int idx) {return int_list[idx];}
    void del_item(int idx) {
        if (idx < 0 || list_len == 0)
            return;

        if (idx + 1 < list_len)
            while (idx < list_len)
            {
                int_list[idx] = int_list[idx + 1];
                idx++;
            }

        list_len--;
    }

private:
    static int s_attr_int;
    int attr_int;
    int int_list[NR_ELEMS];
    int list_len;
};
%End


%ModuleCode
int Klass::s_attr_int = 0;
%End


class Klass
{
public:
    Klass();

    static int get_s_attr_int();
    static void set_s_attr_int(int value);

    int get_attr_int() const;
    void set_attr_int(int value);

    int operator-();
    bool operator==(const Klass &other);
    int operator()(int multiplier);

    int count() const /__len__/;
    // Return a reference once it is supported.
    int operator[](int idx);
    // Remove this once returned references are supported and __setitem__ is
    // generated automatically.
    void __setitem__(int idx, int value);
%MethodCode
    if (a0 < 0 || a0 >= sipCpp->count())
    {
        PyErr_SetNone(PyExc_IndexError);
        return -1;
    }

    (*sipCpp)[a0] = a1;
%End
    void __delitem__(int idx);
%MethodCode
    sipCpp->del_item(a0);
%End
};
