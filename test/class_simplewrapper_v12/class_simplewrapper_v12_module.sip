// The SIP implementation of the class_simplewrapper_v12_module test module.


%Module(name=class_simplewrapper_v12_module)


%ModuleHeaderCode
class SimpleKlass
{
};
%End


// ABI v12 hard codes the fully qualified name of the types as 'sip.wrapper'
// etc.  This is a bug but one we choose not to fix in order to maintain
// backwards compatibility.
class SimpleKlass /Supertype=sip.simplewrapper/
{
};
