// The SIP implementation of the py_properties_module test module.


%Module(name=py_properties_module)


%ModuleHeaderCode
class AnObject
{
public:
    AnObject() : _int_val(-1) {}

    int get_int_val() const
    {
        return _int_val;
    }

    void set_int_val(int int_val)
    {
        _int_val = int_val;
    }

private:
    int _int_val;
};
%End


class AnObject
{
public:
    AnObject();

    int get_int_val() const;
    void set_int_val(int int_val);
    %Property(name=int_val, get=get_int_val, set=set_int_val)
};
