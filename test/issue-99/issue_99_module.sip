// The SIP implementation of the issue_99_module test module.  Note that the
// issue meant that this module wouldn't build.  We also test the correct
// behaviour once built.


%Module(name=issue_99_module)


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
