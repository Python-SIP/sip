// The SIP implementation of the mixins_module test module.


%Module(name=mixins_module, call_super_init=True, keyword_arguments="Optional")


%ModuleHeaderCode
class Mixin
{
public:
    Mixin(int mixin_value=0) : mixin_attr(mixin_value) {}

    int get_mixin_attr() const {return mixin_attr;}
    void set_mixin_attr(int value) {mixin_attr = value;}
    int mixin_attr;
};

class Klass
{
public:
    Klass(int value) : attr(value) {}

    int get_attr() const {return attr;}
    void set_attr(int value) {attr = value;}
    int attr;
};
%End


class Mixin /Mixin/
{
public:
    Mixin(int mixin_value=0);

    int get_mixin_attr() const;
    void set_mixin_attr(int value);
    int mixin_attr;
};

class Klass
{
public:
    Klass(int value);

%FinalisationCode
    // Handle the trivial case.
    if (sipKwds == NULL)
        return 0;

    // If the 'extra_kwarg' is given then save it as an attribute.
    PyObject *dict;

    if (sipUnused == NULL)
    {
        dict = sipKwds;
    }
    else
    {
        dict = PyDict_Copy(sipKwds);
        if (dict == NULL)
            return -1;

        *sipUnused = dict;
    }

    PyObject *value;
    int rc = PyDict_PopString(dict, "extra_kwarg", &value);

    if (rc < 0)
        return -1;

    if (rc == 1)
    {
        rc = PyObject_SetAttrString(sipSelf, "extra_kwarg", value);
        Py_DECREF(value);
    }

    return rc;
%End

    int get_attr() const;
    void set_attr(int value);
    int attr;
};
