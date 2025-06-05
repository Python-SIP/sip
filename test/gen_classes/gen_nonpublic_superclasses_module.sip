// The SIP implementation of the gen_nonpublic_superclasses_module test module.


%Module(name=gen_nonpublic_superclasses_module)


class Bar
{
};

class Foo : protected Bar
{
};
