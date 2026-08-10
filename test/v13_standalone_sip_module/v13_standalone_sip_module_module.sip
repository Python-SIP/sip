// The SIP implementation of the v13_ns.v13_standalone_sip_module test module.


%Module(name=v13_ns.v13_standalone_sip_module_module, use_limited_api=True,
        gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


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
    class Nested /Supertype=v13_ns.sip.simplewrapper/
    {
    };
};
