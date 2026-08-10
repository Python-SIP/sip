// The SIP implementation of the issue_99_module test module.  Note that the
// issue meant that this module wouldn't build.  We also test the correct
// behaviour once built.


%Module(name=issue_99_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


%ModuleHeaderCode
class KlassA
{
public:
    static const char *get_cpp_name() {return "KlassA";}
};

class KlassB
{
public:
    static const char *get_cpp_name() {return "KlassB";}
};
%End


class KlassA /PyName=KlassC/
{
public:
    static const char *get_cpp_name();
};

class KlassB /PyName=KlassA/
{
public:
    static const char *get_cpp_name();
};
