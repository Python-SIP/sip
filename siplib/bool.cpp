// This contains all the C++ code that is needed by the sip module.
//
// @BS_LICENSE@


// Set a C++ bool for the main C implementation of the module.
extern "C" void sipSetBool(void *ptr, int val)
{
	*reinterpret_cast<bool *>(ptr) = val;
}
