// The SIP implementation of the class_simplewrapper_module test module.


%Module(name=class_simplewrapper_module)


%ModuleHeaderCode
class SimpleKlass
{
};
%End


class SimpleKlass /Supertype=class_simplewrapper_module.simplewrapper/
{
};
