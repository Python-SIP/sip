// The SIP implementation of the movable_module test module.


%Module(name=movable_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


template <TYPE>
%MappedType std::unique_ptr<TYPE> /Movable, AllowNone/
{
%TypeHeaderCode
#include <memory>
#include <utility>      // For std::move().
%End

%ConvertFromTypeCode
    /* This must result in ownership being with Python. */

    return sipConvertFromType(sipCpp->release(), sipFindType("TYPE*"),
            Py_None);
%End

%ConvertToTypeCode
    /* This must result in ownership being with C++. */

    sipTypeID type_id = sipFindType("TYPE");

    if (sipIsErr == NULL)
        return sipCanConvertToType(sipPy, type_id, 0);

    if (sipPy == Py_None)
        return 1;

    /* Get the C++ pointer with no convertors or ownership changes. */
    TYPE *t = reinterpret_cast<TYPE *>(sipConvertToType(sipPy, type_id, NULL, SIP_NO_CONVERTORS, NULL, sipIsErr));

    if (*sipIsErr)
        return 0;

    /* Give ownership to C++ but don't lose the Python object. */
    sipTransferTo(sipPy, Py_None);

    *sipCppPtr = new std::unique_ptr<TYPE>(t);

    return 0;
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

    virtual std::unique_ptr<AnObject> createObject()
    {
        return std::unique_ptr<AnObject>(new AnObject(100));
    }

    void recreateObject()
    {
        mObject = std::move(createObject());
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

    virtual std::unique_ptr<AnObject> createObject();

    void recreateObject();
};
