// The SIP implementation of the movable_module test module.


%Module(name=movable_module)


template <TYPE>
%MappedType std::unique_ptr<TYPE> /Movable, NoRelease, AllowNone/
{
%TypeHeaderCode
#include <memory>
#include <utility>      // For std::move().
%End

%ConvertFromTypeCode
#if SIP_ABI_MAJOR_VERSION >= 14
    sipTypeID type_id = sipFindTypeID(sipMS, "TYPE*");

    return sipConvertFromType(sipMS, sipCpp->release(), type_id, Py_None);
#else
    const sipTypeDef *td = sipFindType("TYPE*");

    return sipConvertFromType(sipCpp->release(), td, Py_None);
#endif
%End

%ConvertToTypeCode
#if SIP_ABI_MAJOR_VERSION >= 14
    sipTypeID type_id = sipFindTypeID(sipMS, "TYPE");

    if (sipIsErr == NULL)
        return sipCanConvertToType(sipMS, sipPy, type_id, 0);

    if (sipPy == Py_None)
        return 1;

    int state;
    TYPE *t = reinterpret_cast<TYPE *>(sipConvertToType(sipMS, sipPy, type_id, sipTransferObj, 0, &state, sipIsErr));

    sipReleaseType(sipMS, t, type_id, state);

    if (*sipIsErr)
        return 0;

    *sipCppPtr = new std::unique_ptr<TYPE>(t);

    return sipGetState(sipTransferObj);
#else
    const sipTypeDef *td = sipFindType("TYPE");

    if (sipIsErr == NULL)
        return sipCanConvertToType(sipPy, td, 0);

    if (sipPy == Py_None)
        return 1;

    int state;
    TYPE *t = reinterpret_cast<TYPE *>(sipConvertToType(sipPy, td, sipTransferObj, 0, &state, sipIsErr));

    sipReleaseType(t, td, state);

    if (*sipIsErr)
        return 0;

    *sipCppPtr = new std::unique_ptr<TYPE>(t);

    return sipGetState(sipTransferObj);
#endif
%End
};


%ModuleHeaderCode
class AnObject
{
public:
    AnObject(int i) : mI(i) {}
    virtual ~AnObject() {}

    int getValue() const
    {
        return mI;
    }

    void increment()
    {
        mI++;
    }

private:
    int mI;
};
%End


class AnObject
{
public:
    AnObject(int i);
    virtual ~AnObject();

    int getValue() const;
};


%ModuleHeaderCode
#include <memory>


class ObjectWrapper
{
public:
    ObjectWrapper() {};
    ObjectWrapper(const ObjectWrapper& other) : mObject(new AnObject(*other.mObject)) {}
    ObjectWrapper& operator=(const ObjectWrapper& other)
    {
        mObject.reset(new AnObject(*other.mObject));
        return *this;
    };
    virtual ~ObjectWrapper() {}

    int getObjectValue() const
    {
        return mObject ? mObject->getValue() : -1000;
    }

    std::unique_ptr<AnObject> takeObject()
    {
        return std::move(mObject);
    }

    void setObject(std::unique_ptr<AnObject> anObject)
    {
        mObject = std::move(anObject);
        mObject->increment();
    }

private:
    std::unique_ptr<AnObject> mObject;
};
%End


class ObjectWrapper
{
public:
    ObjectWrapper();
    virtual ~ObjectWrapper();

    int getObjectValue() const;

    std::unique_ptr<AnObject> takeObject();

    void setObject(std::unique_ptr<AnObject> anObject);
};
