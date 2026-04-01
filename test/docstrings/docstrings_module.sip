// The SIP implementation of the docstrings_module test module.


%Module(name=docstrings_module)
{
%Docstring
Module
%End
};


%ModuleHeaderCode
void pod_ptr_arg(const char *arg) {}
void pod_ptr_opt_arg(const char *arg = NULL) {}
void bool_opt_arg(bool arg = true) {}
%End


void pod_ptr_arg(const char *arg);
void pod_ptr_opt_arg(const char *arg = NULL);
void bool_opt_arg(bool arg = true);
