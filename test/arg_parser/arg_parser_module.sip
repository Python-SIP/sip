// The SIP implementation of the arg_parser_module test module.


%Module(name=arg_parser_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


%ModuleHeaderCode
class OwnedClass
{
};

class Klass
{
public:
    Klass() : m_sub(0), m_inst(0), m_prot_inst(0) {}
    Klass(OwnedClass *sub) : m_sub(sub), m_inst(0), m_prot_inst(0) {}
    ~Klass() {
        if (m_sub) {
            delete m_sub;
            m_sub = 0;
        }
    }

    static void set_static(int value) {m_static = value;}
    static int get_static() {return m_static;}

    void set_inst(int value) {m_inst = value;}
    int get_inst() const {return m_inst;}

protected:
    void set_prot_inst(int value) {m_prot_inst = value;}
    int get_prot_inst() const {return m_prot_inst;}

private:
    OwnedClass *m_sub;
    static int m_static;
    int m_inst;
    int m_prot_inst;
};
%End

%ModuleCode
int Klass::m_static = 0;
%End


class Klass
{
public:
    Klass();
    Klass(OwnedClass *sub /Transfer/);
    ~Klass();

    static void set_static(int value);
    static int get_static();

    void set_inst(int value);
    int get_inst() const;

protected:
    void set_prot_inst(int value);
    int get_prot_inst() const;
};

class OwnedClass
{
};
