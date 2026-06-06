// The SIP implementation of the v12_ns.v12_standalone_sip_module test module.


%Module(name=v12_ns.v12_standalone_sip_module_module)

%SipModuleConfiguration {BrokenTypeNames}


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
    class Nested /Supertype=sip.simplewrapper/
    {
    };
};
