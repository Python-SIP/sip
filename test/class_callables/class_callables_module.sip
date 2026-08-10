// The SIP implementation of the class_callables_module test module.


%Module(name=class_callables_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


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
    int operator-(int rhs) {return attr_int - rhs;}
    int operator+(int rhs) {return attr_int + rhs;}
    int operator*(int rhs) {return attr_int * rhs;}
    int operator/(int rhs) {return attr_int / rhs;}
    int operator%(int rhs) {return attr_int % rhs;}
    int operator&(int rhs) {return attr_int & rhs;}
    int operator|(int rhs) {return attr_int | rhs;}
    int operator^(int rhs) {return attr_int ^ rhs;}
    int operator~() {return ~attr_int;}
    int operator<<(int rhs) {return attr_int << rhs;}
    int operator>>(int rhs) {return attr_int >> rhs;}
    Klass &operator-=(int rhs) {attr_int -= rhs; return *this;}
    Klass &operator+=(int rhs) {attr_int += rhs; return *this;}
    Klass &operator*=(int rhs) {attr_int *= rhs; return *this;}
    Klass &operator/=(int rhs) {attr_int /= rhs; return *this;}
    Klass &operator%=(int rhs) {attr_int %= rhs; return *this;}
    Klass &operator&=(int rhs) {attr_int &= rhs; return *this;}
    Klass &operator|=(int rhs) {attr_int |= rhs; return *this;}
    Klass &operator^=(int rhs) {attr_int ^= rhs; return *this;}
    Klass &operator<<=(int rhs) {attr_int <<= rhs; return *this;}
    Klass &operator>>=(int rhs) {attr_int >>= rhs; return *this;}

    bool operator==(int rhs) {return attr_int == rhs;}
    bool operator!=(int rhs) {return attr_int != rhs;}
    bool operator<(int rhs) {return attr_int < rhs;}
    bool operator<=(int rhs) {return attr_int <= rhs;}
    bool operator>(int rhs) {return attr_int > rhs;}
    bool operator>=(int rhs) {return attr_int >= rhs;}

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


class Reverse
{
public:
    Reverse() : attr_int(0) {}

    void set_attr_int(int value) {attr_int = value;}
    int attr_int;
};

int operator-(int lhs, const Reverse &rhs);
int operator+(int lhs, const Reverse &rhs);
int operator*(int lhs, const Reverse &rhs);
int operator/(int lhs, const Reverse &rhs);
int operator%(int lhs, const Reverse &rhs);
int operator&(int lhs, const Reverse &rhs);
int operator|(int lhs, const Reverse &rhs);
int operator^(int lhs, const Reverse &rhs);
int operator<<(int lhs, const Reverse &rhs);
int operator>>(int lhs, const Reverse &rhs);
%End


%ModuleCode
int Klass::s_attr_int = 0;

int operator-(int lhs, const Reverse &rhs) {return lhs - rhs.attr_int;}
int operator+(int lhs, const Reverse &rhs) {return lhs + rhs.attr_int;}
int operator*(int lhs, const Reverse &rhs) {return lhs * rhs.attr_int;}
int operator/(int lhs, const Reverse &rhs) {return lhs / rhs.attr_int;}
int operator%(int lhs, const Reverse &rhs) {return lhs % rhs.attr_int;}
int operator&(int lhs, const Reverse &rhs) {return lhs & rhs.attr_int;}
int operator|(int lhs, const Reverse &rhs) {return lhs | rhs.attr_int;}
int operator^(int lhs, const Reverse &rhs) {return lhs ^ rhs.attr_int;}
int operator<<(int lhs, const Reverse &rhs) {return lhs << rhs.attr_int;}
int operator>>(int lhs, const Reverse &rhs) {return lhs >> rhs.attr_int;}
%End


class Klass
{
public:
    Klass();
    Klass(int v);
%MethodCode
        PyErr_SetString(PyExc_ValueError, "bad ctor");
        sipError = sipErrorFail;
%End

    void error_method();
%MethodCode
        PyErr_SetString(PyExc_ValueError, "bad method");
        sipError = sipErrorFail;
%End

    static int get_s_attr_int();
    static void set_s_attr_int(int value);

    int get_attr_int() const;
    void set_attr_int(int value);

    int operator-();
    int operator-(int rhs);
    int operator+(int rhs);
    int operator*(int rhs);
    int operator/(int rhs);
    int operator%(int rhs);
    int operator&(int rhs);
    int operator|(int rhs);
    int operator^(int rhs);
    int operator~();
    int operator<<(int rhs);
    int operator>>(int rhs);
    Klass &operator-=(int rhs);
    Klass &operator+=(int rhs);
    Klass &operator*=(int rhs);
    Klass &operator/=(int rhs);
    Klass &operator%=(int rhs);
    Klass &operator&=(int rhs);
    Klass &operator|=(int rhs);
    Klass &operator^=(int rhs);
    Klass &operator<<=(int rhs);
    Klass &operator>>=(int rhs);

    bool operator==(int rhs);
    bool operator!=(int rhs);
    bool operator<(int rhs);
    bool operator<=(int rhs);
    bool operator>(int rhs);
    bool operator>=(int rhs);

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
#if SIP_ABI_MAJOR_VERSION >= 14
        sipSetParserError(sipPStateP);
        return NULL;
#else
        return -1;
#endif
    }

    (*sipCpp)[a0] = a1;
%End
    void __delitem__(int idx);
%MethodCode
    sipCpp->del_item(a0);
%End
};


class Reverse
{
public:
    Reverse();

    void set_attr_int(int value);
};

int operator-(int lhs, const Reverse &rhs);
int operator+(int lhs, const Reverse &rhs);
int operator*(int lhs, const Reverse &rhs);
int operator/(int lhs, const Reverse &rhs);
int operator%(int lhs, const Reverse &rhs);
int operator&(int lhs, const Reverse &rhs);
int operator|(int lhs, const Reverse &rhs);
int operator^(int lhs, const Reverse &rhs);
int operator<<(int lhs, const Reverse &rhs);
int operator>>(int lhs, const Reverse &rhs);
