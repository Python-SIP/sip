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
 * Enable or disable overflow checking.
 */
static int sip_api_enable_overflow_checking(int enable)
{
    int was_enabled = overflow_checking;

    overflow_checking = enable;

    return was_enabled;
}


/*
 * Enable or disable overflow checking.
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
