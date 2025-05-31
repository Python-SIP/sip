// The SIP implementation of the templates_module test module.


%Module(name=templates_module)


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


template <class T3>
class SimpleWrapper : public T3
{
};

class CValue
{
};

typedef SimpleWrapper<CValue> CWrapper;


namespace Scope
{
    class Nested
    {
    };
};

typedef SimpleWrapper<Scope::Nested> NestedWrapper;

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


template <T3>
class SimpleWrapper : public T3
{
};

class CValue
{
};

typedef SimpleWrapper<CValue> CWrapper;


namespace Scope
{
    class Nested
    {
    };
};

typedef SimpleWrapper<Scope::Nested> NestedWrapper;
