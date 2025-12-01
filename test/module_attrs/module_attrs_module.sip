// The SIP implementation of the module_attrs_module test module.


%Module(name=module_attrs_module)


%ModuleHeaderCode
// POD types.
bool bool_attr = 1;
_Bool _Bool_attr = 1;
char byte_attr = 10;
signed char sbyte_attr = -10;
unsigned char ubyte_attr = 10;
short short_attr = -10;
unsigned short ushort_attr = 10;
int int_attr = -10;
unsigned uint_attr = 10;
long long_attr = -10;
unsigned long ulong_attr = 10;
long long longlong_attr = -10;
unsigned long long ulonglong_attr = 10;
Py_hash_t pyhasht_attr = -10;
Py_ssize_t pyssizet_attr = -10;
size_t sizet_attr = 10;
float float_attr = 10.;
double double_attr = 10.;
char char_attr = 10;
char char_ascii_attr = 'A';
char char_latin1_attr = 0xa3;
char char_utf8_attr = 'A';
signed char schar_attr = 10;
unsigned char uchar_attr = 10;
wchar_t wchar_attr = L'β';
char *string_attr = NULL;
const char *string_attr_const = "str";
char *string_ascii_attr = NULL;
const char *string_ascii_attr_const = "str";
char *string_latin1_attr = NULL;
const char *string_latin1_attr_const = "\xa3\xa3";
char *string_utf8_attr = NULL;
const char *string_utf8_attr_const = "2H₂ + O₂ ⇌ 2H₂O";
signed char *sstring_attr = NULL;
const signed char *sstring_attr_const = (signed char *)"str";
unsigned char *ustring_attr = NULL;
const unsigned char *ustring_attr_const = (unsigned char *)"str";
char buf[] = "bytes";
void *voidptr_attr = buf;
const char buf_const[] = "bytes const";
const void *voidptr_const_attr = buf_const;
PyObject *pyobject_attr = NULL;

// Read only attributes.
const int int_attr_const = 10;
int int_attr_no_setter = 10;

// /PyName/ annotation.
int int_attr_py_name = 10;

// Utility to confirm an attribute has changed.
int get_int_attr() {return int_attr;}
%End


// POD types.
bool bool_attr;
_Bool _Bool_attr;
char byte_attr /PyInt/;
signed char sbyte_attr /PyInt/;
unsigned char ubyte_attr /PyInt/;
short short_attr;
unsigned short ushort_attr;
int int_attr;
unsigned uint_attr;
long long_attr;
unsigned long ulong_attr;
long long longlong_attr;
unsigned long long ulonglong_attr;
Py_hash_t pyhasht_attr;
Py_ssize_t pyssizet_attr;
size_t sizet_attr;
float float_attr;
double double_attr;
char char_attr /Encoding="None"/;
char char_ascii_attr /Encoding="ASCII"/;
char char_latin1_attr /Encoding="Latin-1"/;
char char_utf8_attr /Encoding="UTF-8"/;
signed char schar_attr;
unsigned char uchar_attr;
wchar_t wchar_attr;
char *string_attr /Encoding="None"/;
const char *string_attr_const /Encoding="None"/;
char *string_ascii_attr /Encoding="ASCII"/;
const char *string_ascii_attr_const /Encoding="ASCII"/;
char *string_latin1_attr /Encoding="Latin-1"/;
const char *string_latin1_attr_const /Encoding="Latin-1"/;
char *string_utf8_attr /Encoding="UTF-8"/;
const char *string_utf8_attr_const /Encoding="UTF-8"/;
signed char *sstring_attr;
const signed char *sstring_attr_const;
unsigned char *ustring_attr;
const unsigned char *ustring_attr_const;
void *voidptr_attr;
const void *voidptr_const_attr;
PyObject *pyobject_attr;

// Read only attributes.
const int int_attr_const;
int int_attr_no_setter /NoSetter/;

// /PyName/ annotation.
int int_attr_py_name /PyName=py_int_attr/;

// Utility to confirm an attribute has changed.
int get_int_attr();
