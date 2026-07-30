/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API of the Python object to C/C++ integer convertors.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_INT_CONVERTORS_H
#define _SIP_INT_CONVERTORS_H

#include <Python.h>

#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

bool sip_api_convert_to_bool(PyObject *o);
char sip_api_long_as_char(PyObject *o);
signed char sip_api_long_as_signed_char(PyObject *o);
unsigned char sip_api_long_as_unsigned_char(PyObject *o);
short sip_api_long_as_short(PyObject *o);
unsigned short sip_api_long_as_unsigned_short(PyObject *o);
int sip_api_long_as_int(PyObject *o);
unsigned int sip_api_long_as_unsigned_int(PyObject *o);
long sip_api_long_as_long(PyObject *o);
unsigned long sip_api_long_as_unsigned_long(PyObject *o);
long long sip_api_long_as_long_long(PyObject *o);
unsigned long long sip_api_long_as_unsigned_long_long(PyObject *o);
size_t sip_api_long_as_size_t(PyObject *o);

#ifdef __cplusplus
}
#endif

#endif
