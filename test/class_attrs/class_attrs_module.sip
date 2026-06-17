// The SIP implementation of the class_attrs_module test module.


%Module(name=class_attrs_module)


%ModuleHeaderCode
class Value
{
public:
    Value(int v = 0) : m_value(v) {}

    int get_value() const {return m_value;}

private:
    int m_value;
};

class Klass
{
public:
    Klass() : attr(0), class_attr(0) {}

    static int s_attr;
    static int get_s_attr() {return s_attr;}

    static int s_attr_with_code;

    int attr;
    int get_attr() {return attr;}

    Value *class_attr;
    Value *get_class_attr() const {return class_attr;}

    Value value_attr;
};
%End


%ModuleCode
int Klass::s_attr = 0;
int Klass::s_attr_with_code = 0;
%End


class Value
{
public:
    Value(int v = 0);

    int get_value() const;
};

class Klass
{
public:
    Klass();

    static int s_attr;
    static int get_s_attr();

    static int s_attr_with_code
    {
        %GetCode
            sipPy = PyLong_FromLong(Klass::s_attr_with_code);
        %End
        %SetCode
            Klass::s_attr_with_code = PyLong_AsLong(sipPy);

            if (PyErr_Occurred())
                sipErr = 1;
        %End
    };

    int attr;
    int get_attr();

    Value *class_attr;
    Value *get_class_attr() const;

    Value value_attr;
};
