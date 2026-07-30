// Define the SIP wrapper to the extras library.

%Module(name=examples.extras, use_limited_api=True, gil_use="NotUsed",
        multi_interpreter_support="Supported")

%Import core/core.sip

%If (!Windows)
bool am_i_posix();
%MethodCode
    sipRes = true;
%End
%End

%If (Windows)
bool am_i_posix();
%MethodCode
    sipRes = false;
%End
%End
