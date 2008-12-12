/*
 * This file defines the SIP library internal interfaces.
 *
 * @BS_LICENSE@
 */


#ifndef _SIPINT_H
#define _SIPINT_H


#ifdef __cplusplus
extern "C" {
#endif

#undef  TRUE
#define TRUE        1

#undef  FALSE
#define FALSE       0


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


extern PyInterpreterState *sipInterpreter;  /* The interpreter. */


extern sipQtAPI *sipQtSupport;  /* The Qt support API. */
extern sipWrapperType sipSimpleWrapper_Type;    /* The simple wrapper type. */
extern sipWrapperType *sipQObjectClass; /* The Python QObject class. */

void *sipGetRx(sipSimpleWrapper *txSelf, const char *sigargs, PyObject *rxObj,
        const char *slot, const char **memberp);
int sip_api_emit_signal(PyObject *self, const char *sig, PyObject *sigargs);
PyObject *sip_api_get_sender();
PyObject *sip_api_connect_rx(PyObject *txObj, const char *sig, PyObject *rxObj,
        const char *slot, int type);
PyObject *sip_api_disconnect_rx(PyObject *txObj, const char *sig,
        PyObject *rxObj,const char *slot);
sipSignature *sip_api_parse_signature(const char *sig);


/*
 * These are part of the SIP API but are also used within the SIP module.
 */
void *sip_api_malloc(size_t nbytes);
void sip_api_free(void *mem);
void *sip_api_get_cpp_ptr(sipSimpleWrapper *w, sipWrapperType *type);
PyObject *sip_api_convert_from_instance(void *cppPtr, sipWrapperType *type,
        PyObject *transferObj);
void sip_api_common_dtor(sipSimpleWrapper *sipSelf);
void sip_api_start_thread(void);
void sip_api_end_thread(void);
PyObject *sip_api_convert_from_named_enum(int eval, PyTypeObject *et);
void sip_api_free_connection(sipSlotConnection *conn);
int sip_api_emit_to_slot(const sipSlot *slot, PyObject *sigargs);
int sip_api_same_connection(sipSlotConnection *conn, void *tx, const char *sig,
        PyObject *rxObj, const char *slot);
PyObject *sip_api_invoke_slot(const sipSlot *slot, PyObject *sigargs);
void sip_api_parse_type(const char *type, sipSigArg *arg);


/*
 * These are not part of the SIP API but are used within the SIP module.
 */
void sipFreeSlotList(sipSlotList *rx);
void sipSaveMethod(sipPyMethod *pm,PyObject *meth);
void *sipGetPending(sipWrapper **op, int *fp);
PyObject *sipWrapSimpleInstance(void *cppPtr, sipTypeDef *td,
        sipWrapper *owner, int initflags);
void *sipConvertRxEx(sipWrapper *txSelf, const char *sigargs,
        PyObject *rxObj, const char *slot, const char **memberp, int flags);

void sipOMInit(sipObjectMap *om);
void sipOMFinalise(sipObjectMap *om);
sipSimpleWrapper *sipOMFindObject(sipObjectMap *om, void *key,
        sipWrapperType *type);
void sipOMAddObject(sipObjectMap *om, sipSimpleWrapper *val);
int sipOMRemoveObject(sipObjectMap *om, sipSimpleWrapper *val);

void sipSetBool(void *ptr,int val);

void *sipGetAddress(sipSimpleWrapper *w);
void sipFindSigArgType(const char *name, size_t len, sipSigArg *at, int indir);


#ifdef __cplusplus
}
#endif

#endif
