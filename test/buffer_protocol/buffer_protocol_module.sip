// The SIP implementation of the buffer_protocol_module test module.


%Module(name=buffer_protocol_module, gil_use="NotUsed",
        multi_interpreter_support="PerInterpreterGILSupported")


%ModuleHeaderCode
#include <string.h>

class BufferClass
{
public:
    BufferClass(const char *s) : m_s(s) {}

    const char *data() const {return m_s;}
    size_t size() const {return strlen(m_s);}

private:
    const char *m_s;
};

class NonbufferClass {};
%End


class BufferClass
{
public:
    BufferClass(const char *s);

%BIGetBufferCode
    sipRes = PyBuffer_FillInfo(sipBuffer, sipSelf, (void *)sipCpp->data(),
            sipCpp->size(), 1, sipFlags);
%End
};

class NonbufferClass {};
