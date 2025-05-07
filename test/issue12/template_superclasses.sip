// The bindings for testing support for template super-classes.

%Module(name=template_superclasses)


%ModuleHeaderCode

class BaseClass
{
};

template <class T1>
class ValueWrapper : public BaseClass
{
public:
    T1 getValue() const {
        return T1();
    }
};

template <class T2>
class ExtendedValueWrapper : public ValueWrapper<T2>
{
};

class AValue
{
};

class BValue
{
};

typedef ExtendedValueWrapper<AValue> AValueWrapper;
typedef ExtendedValueWrapper<BValue> BValueWrapper;

%End

class BaseClass
{
};

template <T1>
class ValueWrapper : public BaseClass
{
public:
    T1 getValue() const;
};

template <T2>
class ExtendedValueWrapper : public ValueWrapper<T2>
{
};

class AValue
{
};

class BValue
{
};

typedef ExtendedValueWrapper<AValue> AValueWrapper;
typedef ExtendedValueWrapper<BValue> BValueWrapper;
