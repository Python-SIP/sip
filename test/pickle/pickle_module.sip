// The SIP implementation of the pickle_module test module.


%Module(name=pickle_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%ModuleHeaderCode
class PickleClass
{
public:
    PickleClass(int x, int y) : m_x(x), m_y(y) {}

    int x() const {return m_x;}
    int y() const {return m_y;}

private:
    int m_x, m_y;
};
%End


class PickleClass
{
public:
    PickleClass(int x, int y);

    int x() const;
    int y() const;

%PickleCode
    sipRes = Py_BuildValue("ii", sipCpp->x(), sipCpp->y());
%End
};
