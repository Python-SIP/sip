// The SIP implementation of the visit_wrappers_module test module.


%Module(name=visit_wrappers_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


%ModuleHeaderCode
class Klass
{
public:
    Klass(int id) : _id(id) {}
    int id() const {return _id;}

private:
    int _id;
};
%End


%ModuleCode
struct VisitorState
{
    PyObject *visitor;
    int failed;
};


// Call a Python visitor for a wrapper.
static void call_visitor(sipSimpleWrapper *sw, void *closure)
{
    VisitorState *state = (VisitorState *)closure;
    PyObject *res;

    if (state->failed)
        return;

    res = PyObject_CallFunctionObjArgs(state->visitor, (PyObject *)sw, NULL);

    if (res == NULL)
        state->failed = 1;
    else
        Py_DECREF(res);
}
%End


class Klass
{
public:
    Klass(int id);
    int id() const;
};


void visit_wrappers(SIP_PYCALLABLE visitor);
%MethodCode
    VisitorState state = {a0, 0};

    sipVisitWrappers(call_visitor, &state);

    if (state.failed)
        sipError = sipErrorFail;
%End
