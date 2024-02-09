// SPDX-License-Identifier: BSD-2-Clause

// This contains the support for C++ bools.
//
// Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


// Set a C++ bool for the main C implementation of the module.  This is only
// used on Windows.
extern "C" void sip_set_bool(void *ptr, int val)
{
    *reinterpret_cast<bool *>(ptr) = !!val;
}
