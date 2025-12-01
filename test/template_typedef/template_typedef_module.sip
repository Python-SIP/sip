// The SIP implementation of the template_typedef_module test module.


%Module(name=template_typedef_module)


%ExportedHeaderCode
// The need for this, ie. to resolve the type as a typedef, is a SIP bug.
#define getSipType(sipTypename) (sipResolveTypedef(sipTypename) ? sipFindType(sipResolveTypedef(sipTypename)) : sipFindType(sipTypename))
%End


template<TYPE>
%MappedType std::vector<TYPE> /TypeHint="List[TYPE]"/ {
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
    //const sipTypeDef *kpTypeDef = sipFindType("TYPE");
    const sipTypeDef *kpTypeDef = getSipType("TYPE");

    if (!kpTypeDef) {
        return NULL;
    }

    // Create the Python list of the correct length.
    PyObject *l;

    if ((l = PyList_New(sipCpp->size())) == NULL) {
        return NULL;
    }

    // Go through each element in the C++ instance and convert it to a SIP
    // wrapper.
    for (size_t i = 0; i < sipCpp->size(); ++i) {
        TYPE *cpp = new TYPE(sipCpp->at(i));
        PyObject *pobj;

        // Get the Python wrapper for the Type instance, creating a new one if
        // necessary, and handle any ownership transfer.
        if ((pobj = sipConvertFromNewType(cpp, kpTypeDef, sipTransferObj)) == NULL) {
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
    //const sipTypeDef *kpTypeDef = sipFindType("TYPE");
    const sipTypeDef *kpTypeDef = getSipType("TYPE");

    if (!kpTypeDef) {
        return 0;
    }

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
                PyList_GET_ITEM(sipPy, i), kpTypeDef, sipTransferObj,
                SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr) {
            sipReleaseType(p, kpTypeDef, state);
            delete v;
            return 0;
        }

        v->push_back(*p);
        sipReleaseType(p, kpTypeDef, state);
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
