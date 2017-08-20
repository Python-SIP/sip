/*
 * The implementation of the Python object to C/C++ integer convertors.
 *
 * Copyright (c) 2017 Riverbank Computing Limited <info@riverbankcomputing.com>
 *
 * This file is part of SIP.
 *
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 *
 * This copy of SIP may also used under the terms of the GNU General Public
 * License v2 or v3 as published by the Free Software Foundation which can be
 * found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
 *
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <Python.h>

#include "sipint.h"


static int overflow_checking = FALSE;   /* Check for overflows. */


/*
 * Enable or disable overflow checking (Python API).
 */
PyObject *sipEnableOverflowChecking(PyObject *self, PyObject *args)
{
    int enable;

    (void)self;

    if (PyArg_ParseTuple(args, "i:enableoverflowchecking", &enable))
    {
        PyObject *res;

        res = (sip_api_enable_overflow_checking(enable) ? Py_True : Py_False);

        Py_INCREF(res);
        return res;
    }

    return NULL;
}


/*
 * Enable or disable overflow checking (C API).
 */
static int sip_api_enable_overflow_checking(int enable)
{
    int was_enabled = overflow_checking;

    overflow_checking = enable;

    return was_enabled;
}


/*
 * Convert a Python object to a C signed char.
 */
signed char sip_api_long_as_char(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C unsigned char.
 */
unsigned char sip_api_long_as_unsigned_char(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C short.
 */
short sip_api_long_as_short(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C unsigned short.
 */
unsigned short sip_api_long_as_unsigned_short(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C int.
 */
int sip_api_long_as_int(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C unsigned int.
 */
unsigned sip_api_long_as_unsigned_int(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C long.
 */
long sip_api_long_as_long(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C unsigned long.
 */
unsigned long sip_api_long_as_unsigned_long(PyObject *o)
{
#if PY_VERSION_HEX < 0x02040000
    /*
     * Work around a bug in Python versions prior to v2.4 where an integer (or
     * a named enum) causes an error.
     */
    if (o != NULL && !PyLong_Check(o) && PyInt_Check(o))
    {
        long v = PyInt_AsLong(o);

        /*
         * Strictly speaking this should be changed to be consistent with the
         * use of PyLong_AsUnsignedLongMask().  However as it's such an old
         * version of Python we choose to leave it as it is.
         */

        if (v < 0)
        {
            PyErr_SetString(PyExc_OverflowError,
                    "can't convert negative value to unsigned long");

            return (unsigned long)-1;
        }

        return v;
    }
#endif

    /*
     * Note that we now ignore any overflow so that (for example) a negative
     * integer will be converted to an unsigned as C/C++ would do.  We don't
     * bother to check for overflow when converting to small C/C++ types (short
     * etc.) so at least this is consistent.
     */
    return PyLong_AsUnsignedLongMask(o);
}


#if defined(HAVE_LONG_LONG)
/*
 * Convert a Python object to a C long long.
 */
PY_LONG_LONG sip_api_long_as_long_long(PyObject *o)
{
    return 0;
}


/*
 * Convert a Python object to a C unsigned long long.
 */
unsigned PY_LONG_LONG sip_api_long_as_unsigned_long_long(PyObject *o)
{
    return 0;
}
#endif
