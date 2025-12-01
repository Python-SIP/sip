// The SIP implementation of the module_attrs_getset_module test module.


%Module(name=module_attrs_getset_module)


%ModuleHeaderCode
// Read only attributes.
int int_attr_no_setter = 10;

// /PyName/ annotation.
int int_attr_py_name = 10;

// %GetCode.
int int_attr_getter = 10;

// %SetCode.
int int_attr_setter = 10;

// %SetCode that raises an exception.
int int_attr_bad_setter = 10;

// Utility to confirm an attribute has changed.
int get_int_attr() {return int_attr;}
%End


// Read only attributes.
int int_attr_no_setter /NoSetter/;

// %GetCode.
int int_attr_getter {
%GetCode
    sipPy = PyLong_FromLong(2 * int_attr_getter);
%End
};

// %SetCode.
int int_attr_setter {
%SetCode
    int_attr_setter = 2 * (int)PyLong_AsLong(sipPy);
%End
};

// %SetCode that raises an exception.
int int_attr_bad_setter {
%SetCode
    PyErr_SetNone(PyExc_NameError);
    sipErr = 1;
%End
};
