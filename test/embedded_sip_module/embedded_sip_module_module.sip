// The SIP implementation of the embedded_sip_module test module.


%Module(name=embedded_sip_module_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%ModuleHeaderCode
class Klass
{
public:
    class Nested
    {
    };
};
%End


class Klass
{
public:
    class Nested
    {
    };
};
