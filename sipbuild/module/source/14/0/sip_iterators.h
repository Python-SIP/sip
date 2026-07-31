/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the iterator support.
 *
 * Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_ITERATORS_H
#define _SIP_ITERATORS_H

#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

/* A function that is called when an iterator finds an attribute. */
typedef int (*sipIterAttrFunc)(sipModuleState *, const sipAttrSpec *, void *);

/* A function that is called when an iterator finds an init extender. */
typedef int (*sipIterInitFunc)(sipModuleState *, const sipInitExtenderSpec *,
        void *);

int sip_iterate_over_attributes(sipModuleState *ms,
        const sipTypeSpec *extending_ts, const char *name,
        sipIterAttrFunc handler, void *closure);
int sip_iterate_over_init_extenders(sipModuleState *ms,
        const sipTypeSpec *extending_ts, sipIterInitFunc handler,
        void *closure);

#ifdef __cplusplus
}
#endif

#endif
