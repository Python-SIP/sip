// The SIP implementation of the mapped_type_attrs_module test module.


%Module(name=mapped_type_attrs_module)


%MappedType EnumAttrs
{
    enum MTEnum {
        MTEnumMember
    };

%ConvertFromTypeCode
    (void)sipCpp;

    Py_INCREF(Py_None);

    return Py_None;
%End

%ConvertToTypeCode
    if (sipIsErr == NULL)
        return 1;

    return sipGetState(sipTransferObj);
%End
};


%ModuleHeaderCode
class EnumAttrs
{
public:
    enum MTEnum {
        MTEnumMember = 10
    };
};
%End


%MappedType Callables
{
    static int callable();

%ConvertFromTypeCode
    (void)sipCpp;

    Py_INCREF(Py_None);

    return Py_None;
%End

%ConvertToTypeCode
    if (sipIsErr == NULL)
        return 1;

    return sipGetState(sipTransferObj);
%End
};


%ModuleHeaderCode
class Callables
{
public:
    static int callable() {return 42;}
};
%End
