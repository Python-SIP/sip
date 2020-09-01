// The bindings for testing support for enums.

%Module(name=enums, language="C++")

%ModuleHeaderCode
enum {
    AnonMember = 10
};

enum NamedEnum {
    NamedMember = 20
};

enum class ScopedEnum {
    ScopedMember = 30
};


class EnumClass {
public:
    enum {
        ClassAnonMember = 40
    };

    enum ClassNamedEnum {
        ClassNamedMember = 50
    };

    enum class ClassScopedEnum {
        ClassScopedMember = 60
    };
};
%End


enum {
    AnonMember
};

enum NamedEnum {
    NamedMember
};

enum class ScopedEnum {
    ScopedMember
};


class EnumClass {
public:
    enum {
        ClassAnonMember
    };

    enum ClassNamedEnum {
        ClassNamedMember
    };

    enum class ClassScopedEnum {
        ClassScopedMember
    };
};
