// The SIP implementation of the class_misc_module test module.


%Module(name=class_misc_module)


%ModuleHeaderCode
class Klass
{
public:
    class Nested
    {
    };
};

class SimpleKlass
{
};
%End


class Klass
{
public:
    class Nested
    {
    };
};

%If (SIP_ABI_13 -)
class SimpleKlass /Supertype=class_misc_module.simplewrapper/
{
};
%End

// ABI v12 hard codes the fully qualified name of the types as 'sip.wrapper'
// etc.  This is a bug but one we choose not to fix in order to maintain
// backwards compatibility.
%If (- SIP_ABI_13)
class SimpleKlass /Supertype=sip.simplewrapper/
{
};
%End
