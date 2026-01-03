// The SIP implementation of the virtuals_module test module.


%Module(name=virtuals_module)


%ModuleHeaderCode
class Base
{
public:
    Base(const char *class_name = nullptr) : m_class_name(class_name) {}
    virtual ~Base() {}

    virtual int default_value_int() const {return 10;}
    int get_value_int() const {return default_value_int();}

    virtual Base *default_value_class() const {return new Base("Base");}
    Base *get_value_class() const {return default_value_class();}
    const char *get_class_name() const {return m_class_name;}

private:
    const char *m_class_name;
};
%End


class Base
{
public:
    Base(const char *class_name = 0);
    virtual ~Base();

    virtual int default_value_int() const;
    int get_value_int() const;

    virtual Base *default_value_class() const /Factory/;
    Base *get_value_class() const /Factory/;
    const char *get_class_name() const;
};
