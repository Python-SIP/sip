/*
 * This file defines the SIP library internal interfaces.
 *
 * Copyright (c) 2016 Riverbank Computing Limited <info@riverbankcomputing.com>
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


#ifndef _SIPINT_H
#define _SIPINT_H


#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif

#undef  TRUE
#define TRUE        1

#undef  FALSE
#define FALSE       0


/*
 * Access to the wrapper object flags.  Note that the public SIP_TEMPORARY and
 * SIP_DERIVED_CLASS values are part of the flag "namespace".
 */
#define SIP_PY_OWNED        0x0004  /* If owned by Python. */
#define SIP_INDIRECT        0x0008  /* If there is a level of indirection. */
#define SIP_ACCFUNC         0x0010  /* If there is an access function. */
#define SIP_NOT_IN_MAP      0x0020  /* If Python object is not in the map. */
#define SIP_SHARE_MAP       0x0040  /* If the map slot might be occupied. */
#define SIP_CPP_HAS_REF     0x0080  /* If C/C++ has a reference. */
#define SIP_POSSIBLE_PROXY  0x0100  /* If there might be a proxy slot. */
#define SIP_ALIAS           0x0200  /* If it is an alias. */
#define SIP_CREATED         0x0400  /* If the C/C++ object has been created. */

#define sipIsPyOwned(sw)    ((sw)->sw_flags & SIP_PY_OWNED)
#define sipSetPyOwned(sw)   ((sw)->sw_flags |= SIP_PY_OWNED)
#define sipResetPyOwned(sw) ((sw)->sw_flags &= ~SIP_PY_OWNED)
#define sipIsDerived(sw)    ((sw)->sw_flags & SIP_DERIVED_CLASS)
#define sipIsIndirect(sw)   ((sw)->sw_flags & SIP_INDIRECT)
#define sipIsAccessFunc(sw) ((sw)->sw_flags & SIP_ACCFUNC)
#define sipNotInMap(sw)     ((sw)->sw_flags & SIP_NOT_IN_MAP)
#define sipSetNotInMap(sw)  ((sw)->sw_flags |= SIP_NOT_IN_MAP)
#define sipCppHasRef(sw)    ((sw)->sw_flags & SIP_CPP_HAS_REF)
#define sipSetCppHasRef(sw) ((sw)->sw_flags |= SIP_CPP_HAS_REF)
#define sipResetCppHasRef(sw)   ((sw)->sw_flags &= ~SIP_CPP_HAS_REF)
#define sipPossibleProxy(sw)    ((sw)->sw_flags & SIP_POSSIBLE_PROXY)
#define sipSetPossibleProxy(sw) ((sw)->sw_flags |= SIP_POSSIBLE_PROXY)
#define sipIsAlias(sw)      ((sw)->sw_flags & SIP_ALIAS)
#define sipWasCreated(sw)   ((sw)->sw_flags & SIP_CREATED)


/*
 * This defines a single entry in an object map's hash table.
 */
typedef struct
{
    void *key;                  /* The C/C++ address. */
    sipSimpleWrapper *first;    /* The first object at this address. */
} sipHashEntry;


/*
 * This defines the interface to a hash table class for mapping C/C++ addresses
 * to the corresponding wrapped Python object.
 */
typedef struct
{
    int primeIdx;               /* Index into table sizes. */
    unsigned long size;         /* Size of hash table. */
    unsigned long unused;       /* Nr. unused in hash table. */
    unsigned long stale;        /* Nr. stale in hash table. */
    sipHashEntry *hash_array;   /* Current hash table. */
} sipObjectMap;


/*
 * Support for the descriptors.
 */
extern PyTypeObject sipMethodDescr_Type;
PyObject *sipMethodDescr_New(PyMethodDef *pmd);
PyObject *sipMethodDescr_Copy(PyObject *orig, PyObject *mixin_name);

extern PyTypeObject sipVariableDescr_Type;
PyObject *sipVariableDescr_New(sipVariableDef *vd, const sipTypeDef *td,
    const sipContainerDef *cod);
PyObject *sipVariableDescr_Copy(PyObject *orig, PyObject *mixin_name);


/*
 * Support for API versions.
 */
PyObject *sipGetAPI(PyObject *self, PyObject *args);
PyObject *sipSetAPI(PyObject *self, PyObject *args);
int sip_api_is_api_enabled(const char *name, int from, int to);
int sipIsRangeEnabled(sipExportedModuleDef *em, int range_index);
int sipInitAPI(sipExportedModuleDef *em, PyObject *mod_dict);


/*
 * Support for void pointers.
 */
extern PyTypeObject sipVoidPtr_Type;
void *sip_api_convert_to_void_ptr(PyObject *obj);
PyObject *sip_api_convert_from_void_ptr(void *val);
PyObject *sip_api_convert_from_const_void_ptr(const void *val);
PyObject *sip_api_convert_from_void_ptr_and_size(void *val, SIP_SSIZE_T size);
PyObject *sip_api_convert_from_const_void_ptr_and_size(const void *val,
        SIP_SSIZE_T size);


extern sipQtAPI *sipQtSupport;  /* The Qt support API. */
extern sipWrapperType sipSimpleWrapper_Type;    /* The simple wrapper type. */
extern sipTypeDef *sipQObjectType;  /* The QObject type. */

void *sipGetRx(sipSimpleWrapper *txSelf, const char *sigargs, PyObject *rxObj,
        const char *slot, const char **memberp);
PyObject *sip_api_connect_rx(PyObject *txObj, const char *sig, PyObject *rxObj,
        const char *slot, int type);
PyObject *sip_api_disconnect_rx(PyObject *txObj, const char *sig,
        PyObject *rxObj,const char *slot);


/*
 * These are part of the SIP API but are also used within the SIP module.
 */
void *sip_api_malloc(size_t nbytes);
void sip_api_free(void *mem);
void *sip_api_get_address(sipSimpleWrapper *w);
void *sip_api_get_cpp_ptr(sipSimpleWrapper *w, const sipTypeDef *td);
PyObject *sip_api_convert_from_type(void *cppPtr, const sipTypeDef *td,
        PyObject *transferObj);
void sip_api_common_dtor(sipSimpleWrapper *sipSelf);
void sip_api_end_thread(void);
void *sip_api_force_convert_to_type(PyObject *pyObj, const sipTypeDef *td,
        PyObject *transferObj, int flags, int *statep, int *iserrp);
void sip_api_free_sipslot(sipSlot *slot);
unsigned long sip_api_long_as_unsigned_long(PyObject *o);
int sip_api_same_slot(const sipSlot *sp, PyObject *rxObj, const char *slot);
PyObject *sip_api_invoke_slot(const sipSlot *slot, PyObject *sigargs);
PyObject *sip_api_invoke_slot_ex(const sipSlot *slot, PyObject *sigargs,
        int no_receiver_check);
void *sip_api_convert_rx(sipWrapper *txSelf, const char *sigargs,
        PyObject *rxObj, const char *slot, const char **memberp, int flags);
int sip_api_save_slot(sipSlot *sp, PyObject *rxObj, const char *slot);


/*
 * These are not part of the SIP API but are used within the SIP module.
 */
sipClassTypeDef *sipGetGeneratedClassType(const sipEncodedTypeDef *enc,
        const sipClassTypeDef *ctd);
void sipSaveMethod(sipPyMethod *pm,PyObject *meth);
int sipGetPending(void **pp, sipWrapper **op, int *fp);
int sipIsPending();
PyObject *sipWrapInstance(void *cpp,  PyTypeObject *py_type, PyObject *args,
        sipWrapper *owner, int flags);
void *sipConvertRxEx(sipWrapper *txSelf, const char *sigargs,
        PyObject *rxObj, const char *slot, const char **memberp, int flags);

void sipOMInit(sipObjectMap *om);
void sipOMFinalise(sipObjectMap *om);
sipSimpleWrapper *sipOMFindObject(sipObjectMap *om, void *key,
        const sipTypeDef *td);
void sipOMAddObject(sipObjectMap *om, sipSimpleWrapper *val);
int sipOMRemoveObject(sipObjectMap *om, sipSimpleWrapper *val);

void sipSetBool(void *ptr,int val);


#ifdef __cplusplus
}
#endif

#endif
