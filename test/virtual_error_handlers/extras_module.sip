// The SIP implementation of the extras_module test module.


%Module(name=virtual_error_handlers.extras_module)

%Import core_module.sip


%ModuleHeaderCode
class UseDefault
{
public:
    UseDefault() {}
    virtual ~UseDefault() {}

    virtual void use_default() const {}
    void trigger_default() const {use_default();}
};

class Klass
{
public:
    Klass() {}
    virtual ~Klass() {}

    virtual void use_klass() const {}
    void trigger_klass() const {use_klass();}

    virtual void use_inst() const {}
    void trigger_inst() const {use_inst();}

    virtual void use_none() const {}
    void trigger_none() const {use_none();}
};
%End


class UseDefault
{
public:
    UseDefault();
    virtual ~UseDefault();

    virtual void use_default() const;
    void trigger_default() const;
};

class Klass /VirtualErrorHandler=klass/
{
public:
    Klass();
    virtual ~Klass();

    virtual void use_klass() const;
    void trigger_klass() const;

    virtual void use_inst() const /VirtualErrorHandler=inst/;
    void trigger_inst();

    virtual void use_none() const /NoVirtualErrorHandler/;
    void trigger_none() const;
};

%VirtualErrorHandler(name=klass)
    PyErr_Clear();
    PyObject_SetAttrString((PyObject *)sipPySelf, "klass_used", Py_True);
%End

%VirtualErrorHandler(name=inst)
    PyErr_Clear();
    PyObject_SetAttrString((PyObject *)sipPySelf, "inst_used", Py_True);
%End
