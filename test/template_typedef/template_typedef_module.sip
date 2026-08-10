// The SIP implementation of the template_typedef_module test module.


%Module(name=template_typedef_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"


%ExportedHeaderCode
// The need for this, ie. to resolve the type as a typedef, is a SIP bug.
#if SIP_ABI_MAJOR_VERSION >= 14
#define getSipType(name)    sipFindType(sipResolveTypedef(name))
#else
#define getSipType(name)    (sipResolveTypedef(name) ? sipFindType(sipResolveTypedef(name)) : sipFindType(name))
#endif
%End


template<TYPE>
%MappedType std::vector<TYPE> /TypeHint="List[TYPE]"/ {
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
    //sipTypeID kp_type_id = sipFindType("TYPE");
    sipTypeID kp_type_id = getSipType("TYPE");

    if (kp_type_id == sipType_Invalid)
        return NULL;

    // Create the Python list of the correct length.
    PyObject *l;

    if ((l = PyList_New(sipCpp->size())) == NULL) {
        return NULL;
    }

    // Go through each element in the C++ instance and convert it to a SIP
    // wrapper.
    for (size_t i = 0; i < sipCpp->size(); ++i) {
        TYPE *cpp = new TYPE(sipCpp->at(i));

        PyObject *pobj = sipConvertFromNewType(cpp, kp_type_id,
                sipTransferObj);

        if (pobj == NULL) {
            // There was an error so garbage collect the Python list.
            Py_XDECREF(l);
            return NULL;
        }

        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, pobj);
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    //sipTypeID kp_type_id = sipFindType("TYPE");
    sipTypeID kp_type_id = getSipType("TYPE");

    if (kp_type_id == sipType_Invalid)
        return 0;

    // Check if type is compatible
    if (sipIsErr == NULL) {
        return PyList_Check(sipPy);
    }

    // Convert Python list of TYPE to  std::vector<TYPE>
    std::vector<TYPE> *v = new std::vector<TYPE>();
    v->reserve(PyList_GET_SIZE(sipPy));

    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
        int state;

        TYPE *p = static_cast<TYPE *>(sipConvertToType(
                PyList_GET_ITEM(sipPy, i), kp_type_id, sipTransferObj,
                SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr) {
            sipReleaseType(p, kp_type_id, state);
            delete v;
            return 0;
        }

        v->push_back(*p);

        sipReleaseType(p, kp_type_id, state);
    }

    *sipCppPtr = v;

    return sipGetState(sipTransferObj);
%End
};


%MappedType std::vector<int> /TypeHint="List[int]"/ {
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp->size())) == NULL) {
        return NULL;
    }

    // Go through each element in the C++ instance and convert it to a wrapped
    // object.
    for (size_t i = 0; i < sipCpp->size(); ++i) {
        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, PyLong_FromLong(sipCpp->at(i)));
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    // Check if type is compatible
    if (sipIsErr == NULL) {
        return PyList_Check(sipPy);
    }

    // Convert Python list of integers to a std::vector<int>
    std::vector<int> *v = new std::vector<int>();
    v->reserve(PyList_GET_SIZE(sipPy));

    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
        v->push_back(int(PyLong_AsLong(PyList_GET_ITEM(sipPy, i))));
    }

    *sipCppPtr = v;

    return sipGetState(sipTransferObj);
%End
};


%ModuleHeaderCode
#include <vector>
std::vector<int> incrementVectorInt(const std::vector<int> &v);
std::vector<std::vector<int> > incrementVectorVectorInt(
        const std::vector<std::vector<int> > &v);
typedef std::vector<int> VecInt;
%End


%ModuleCode
std::vector<int> incrementVectorInt(const std::vector<int> &v) {
    std::vector<int> ret;
    for (std::vector<int>::const_iterator i = v.cbegin(); i != v.cend(); ++i) {
        ret.push_back(*i+1);
    }
    return ret;
}

std::vector<std::vector<int> > incrementVectorVectorInt(
        const std::vector<std::vector<int> > &v) {
    std::vector<std::vector<int> > ret;
    for (std::vector<std::vector<int> >::const_iterator i = v.cbegin(); i != v.cend(); ++i) {
        ret.push_back(incrementVectorInt(*i));
    }
    return ret;
}
%End


typedef std::vector<int> VecInt;
std::vector<int> incrementVectorInt(const std::vector<int> &v);
std::vector<VecInt> incrementVectorVectorInt(const std::vector<VecInt> &v);
