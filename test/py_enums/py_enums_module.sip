// The SIP implementation of the py_enums_module test module.


%Module(name=py_enums_module)


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


enum EnumBase {
    EnumBaseMember = 100
};

enum FlagBase {
    FlagBaseMember = 110
};

enum IntEnumBase {
    IntEnumBaseMember = 120
};

enum IntFlagBase {
    IntFlagBaseMember = 130
};


class EnumClass {
public:
    EnumClass() : named_overload(false) {}
    virtual ~EnumClass() {}

    enum {
        ClassAnonMember = 40
    };

    enum ClassNamedEnum {
        ClassNamedMember = 50
    };

    ClassNamedEnum named_get() {
        return named_virt();
    }
    virtual ClassNamedEnum named_virt() {
        return ClassNamedMember;
    }
    static void named_set(ClassNamedEnum) {}
    ClassNamedEnum named_var;

    enum ClassNamedEnum2 {
        ClassNamedMember2 = 60
    };

    void named_overload_set(ClassNamedEnum2) {named_overload = false;}
    void named_overload_set(ClassNamedEnum) {named_overload = true;}
    bool named_overload;

    enum class ClassScopedEnum {
        ClassScopedMember = 70
    };

    ClassScopedEnum scoped_get() {
        return scoped_virt();
    }
    virtual ClassScopedEnum scoped_virt() {
        return ClassScopedEnum::ClassScopedMember;
    }
    static void scoped_set(ClassScopedEnum) {}
    ClassScopedEnum scoped_var;
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


enum EnumBase /BaseType=Enum/ {
    EnumBaseMember
};

enum FlagBase /BaseType=Flag/ {
    FlagBaseMember
};

enum IntEnumBase /BaseType=IntEnum/ {
    IntEnumBaseMember
};

enum IntFlagBase /BaseType=IntFlag/ {
    IntFlagBaseMember
};


class EnumClass {
public:
    EnumClass();
    virtual ~EnumClass();

    enum {
        ClassAnonMember
    };

    enum ClassNamedEnum {
        ClassNamedMember
    };

    ClassNamedEnum named_get();
    virtual ClassNamedEnum named_virt();
    static void named_set(ClassNamedEnum);
    ClassNamedEnum named_var;

    enum ClassNamedEnum2 {
        ClassNamedMember2
    };

    void named_overload_set(ClassNamedEnum2);
    void named_overload_set(ClassNamedEnum);
    bool named_overload;

    enum class ClassScopedEnum {
        ClassScopedMember
    };

    ClassScopedEnum scoped_get();
    virtual ClassScopedEnum scoped_virt();
    static void scoped_set(ClassScopedEnum);
    ClassScopedEnum scoped_var;
};
