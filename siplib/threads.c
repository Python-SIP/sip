/*
 * Thread support for the SIP library.  This module provides the hooks for
 * C++ classes that provide a thread interface to interact properly with the
 * Python threading infrastructure.
 *
 * @BS_LICENSE@
 */


#include "sip.h"
#include "sipint.h"


#ifdef WITH_THREAD

#include <pythread.h>


/*
 * The per thread data we need to maintain.
 */
typedef struct _threadDef {
	long thr_ident;				/* The thread identifier. */
	void *cppPending;
	int cppPendingFlags;
	sipWrapper *cppPendingOwner;
	struct _threadDef *next;		/* Next in the list. */
} threadDef;


static threadDef *threads = NULL;		/* Linked list of threads. */


static threadDef *currentThreadDef(void);

#endif


static void *cppPending = NULL;
static int cppPendingFlags;
static sipWrapper *cppPendingOwner;


/*
 * Get the address of any C/C++ object waiting to be wrapped.
 */
void *sipGetPending(sipWrapper **op, int *fp)
{
	void *pend;
	int pendFlags;
	sipWrapper *pendOwner;

#ifdef WITH_THREAD
	threadDef *td;

	if ((td = currentThreadDef()) != NULL)
	{
		pend = td->cppPending;
		pendOwner = td->cppPendingOwner;
		pendFlags = td->cppPendingFlags;
	}
	else
	{
		pend = cppPending;
		pendOwner = cppPendingOwner;
		pendFlags = cppPendingFlags;
	}
#else
	pend = cppPending;
	pendOwner = cppPendingOwner;
	pendFlags = cppPendingFlags;
#endif

	if (pend != NULL)
	{
		if (op != NULL)
			*op = pendOwner;

		if (fp != NULL)
			*fp = pendFlags;
	}

	return pend;
}


/*
 * Convert a new C/C++ pointer to a Python instance.
 */
PyObject *sipWrapSimpleInstance(void *cppPtr, sipWrapperType *type,
		sipWrapper *owner, int flags)
{
	static PyObject *nullargs = NULL;

	PyObject *self;
#ifdef WITH_THREAD
	threadDef *td;
#endif

	if (nullargs == NULL && (nullargs = PyTuple_New(0)) == NULL)
		return NULL;

	if (cppPtr == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

#ifdef WITH_THREAD
	if ((td = currentThreadDef()) != NULL)
	{
		td->cppPending = cppPtr;
		td->cppPendingOwner = owner;
		td->cppPendingFlags = flags;
	}
	else
	{
		cppPending = cppPtr;
		cppPendingOwner = owner;
		cppPendingFlags = flags;
	}
#else
	cppPending = cppPtr;
	cppPendingOwner = owner;
	cppPendingFlags = flags;
#endif

	self = PyObject_Call((PyObject *)type,nullargs,NULL);

#ifdef WITH_THREAD
	if (td != NULL)
		td -> cppPending = NULL;
	else
		cppPending = NULL;
#else
	cppPending = NULL;
#endif

	return self;
}


/*
 * This is called from a newly created thread to initialise some thread local
 * storage.
 */
void sip_api_start_thread(void)
{
#ifdef WITH_THREAD
	threadDef *td;

	/* Save the thread ID.  First, find an empty slot in the list. */
	for (td = threads; td != NULL; td = td -> next)
		if (td -> thr_ident == 0)
			break;

	if (td == NULL)
	{
		td = sip_api_malloc(sizeof (threadDef));
		td -> next = threads;
		threads = td;
	}

	if (td != NULL)
	{
		td -> thr_ident = PyThread_get_thread_ident();
		td -> cppPending = NULL;
	}
#endif
}


/*
 * Handle the termination of a thread.  The thread state should already have
 * been handled by the last call to PyGILState_Release().
 */
void sip_api_end_thread(void)
{
#ifdef WITH_THREAD
	threadDef *td;

	/* We have the GIL at this point. */
	if ((td = currentThreadDef()) != NULL)
		td -> thr_ident = 0;
#endif
}


#ifdef WITH_THREAD

/*
 * Return the thread data for the current thread or NULL if it wasn't
 * recognised.
 */
static threadDef *currentThreadDef(void)
{
	threadDef *td;
	long ident = PyThread_get_thread_ident();

	for (td = threads; td != NULL; td = td -> next)
		if (td -> thr_ident == ident)
			break;

	return td;
}

#endif
