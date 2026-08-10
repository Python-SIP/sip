// The SIP implementation of the c_module test module.


%Module(name=c_module_module, language="C", gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


int echo(int n);
%MethodCode
    sipRes = a0;
%End
