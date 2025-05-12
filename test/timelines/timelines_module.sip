// The SIP implementation of the timelines_module test module.


%Module(name=timelines_module)


%Timeline {v1 v2 v3}


%ModuleHeaderCode

// We implement them all.
void up_to_v2_disabled() {}
void v3_and_after_disabled() {}
void between_v1_and_v3_enabled() {}
void up_to_unknown_enabled() {}
void unknown_and_after_disabled() {}
void between_unknown1_and_unknown2_disabled() {}

%End


%If (- v2)
void up_to_v2_disabled();
%End


%If (v3 -)
void v3_and_after_disabled();
%End


%If (v1 - v3)
void between_v1_and_v3_enabled();
%End


%If (- unknown)
void up_to_unknown_enabled();
%End


%If (unknown -)
void unknown_and_after_disabled();
%End


%If (unknown1 - unknown2)
void between_unknown1_and_unknown2_disabled();
%End
