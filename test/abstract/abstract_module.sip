// The SIP implementation of the abstract_module test module.


%Module(name=abstract_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%ModuleHeaderCode
class AbstractBaseClass
{
public:
    virtual ~AbstractBaseClass() {}
    virtual int get_value() const = 0;
};

class ConcreteClass : public AbstractBaseClass
{
public:
    virtual int get_value() const {return 10;}
};
%End


class AbstractBaseClass
{
public:
    virtual ~AbstractBaseClass();
    virtual int get_value() const = 0;
};

class ConcreteClass : public AbstractBaseClass
{
public:
    virtual int get_value() const;
};
