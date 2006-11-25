/*
 * The SIP library code that implements the interface to the optional module
 * supplied Qt support.
 *
 * @BS_LICENSE@
 */


#include <Python.h>
#include <string.h>

#include "sip.h"
#include "sipint.h"


/* This is how Qt "types" signals and slots. */
#define isQtSlot(s) (*(s) == '1')
#define isQtSignal(s)   (*(s) == '2')


static PyObject *py_sender = NULL;  /* The last Python signal sender. */


static int isSameSlot(sipSlot *,PyObject *,const char *);
static int emitQtSig(sipWrapper *,const char *,PyObject *);
static int emitToSlotList(sipPySigRx *,PyObject *);
static int addSlotToPySigList(sipWrapper *,const char *,PyObject *,const char *);
static void removeSlotFromPySigList(sipWrapper *,const char *,PyObject *,const char *);
static PyObject *getWeakRef(PyObject *obj);
static sipPySig *findPySignal(sipWrapper *,const char *);
static char *sipStrdup(const char *);
static int saveSlot(sipSlot *sp, PyObject *rxObj, const char *slot);
static sipSignature *parseSignature(const char *sig);
static void *createUniversalSlot(sipWrapper *txSelf, const char *sig, PyObject *rxObj, const char *slot, const char **member);
static void *findSignal(void *txrx, const char **sig);
static void *newSignal(void *txrx, const char **sig);
static void freeSlot(sipSlot *slot);


/*
 * Return the most recent signal sender.
 */
PyObject *sip_api_get_sender()
{
    PyObject *sender;
    const void *qt_sender;

    /*
     * If there is a Qt sender then it is more recent than the last Python
     * sender, so use it instead.
     */
    if ((qt_sender = sipQtSupport->qt_get_sender()) != NULL)
        sender = sip_api_convert_from_instance((void *)qt_sender, sipQObjectClass, NULL);
    else
    {
        if ((sender = py_sender) == NULL)
            sender = Py_None;

        Py_INCREF(sender);
    }

    return sender;
}


/*
 * Release the resources held by a connection.
 */
void sip_api_free_connection(sipSlotConnection *conn)
{
    freeSlot(&conn->sc_slot);
}


/*
 * Compare two connections and return TRUE if they are the same.
 */
int sip_api_same_connection(sipSlotConnection *conn, void *tx, const char *sig,
                PyObject *rxObj, const char *slot)
{
    return (conn->sc_transmitter == tx &&
        sipQtSupport->qt_same_name(conn->sc_signature->sg_signature, sig) &&
        isSameSlot(&conn->sc_slot, rxObj, slot));
}


/*
 * Parse the signal arguments for a connection.
 */
static sipSignature *parseSignature(const char *sig)
{
    static sipSignature *psig_list = NULL;
    sipSignature *psig;
    const char *sp, *ep;

    /*
     * First see if it has already been parsed.  Note that both sides of a
     * connection will probably be parsed twice because the function names will
     * be different even though the signatures will probably be the same.  We
     * could be more clever, the most saving is when repeatedly emitting a
     * signal for which this is sufficient.
     */
    for (psig = psig_list; psig != NULL; psig = psig->sg_next)
        if (sipQtSupport->qt_same_name(psig->sg_signature, sig))
            return psig;

    /* Create a new one including space for the copy of the signature. */
    if ((psig = (sipSignature *)sip_api_malloc(sizeof (sipSignature) + strlen(sig) + 1)) == NULL)
        return NULL;

    psig->sg_signature = (char *)&psig[1];
    psig->sg_nrargs = 0;
    psig->sg_args = 0;

    /* Find the start and end of the arguments. */
    sp = strchr(sig, '(');
    ep = strrchr(sig, ')');

    /* If the signal isn't well formed we assume Qt will pick it up. */
    if (sp && ep && sp < ep)
    {
        /*
         * Copy the signature arguments while counting them and
         * removing non-significant spaces.  Each argument is left as a
         * '\0' terminated string.
         */
        char *dp = psig->sg_signature;
        int depth = 0, nrcommas = 0, argstart = TRUE;

        for (;;)
        {
            char ch = *++sp;

            if (strchr(",*&)<>", ch))
            {
                /* Backup over any previous trailing space. */
                if (dp > psig->sg_signature && dp[-1] == ' ')
                    --dp;

                if (sp == ep)
                {
                    *dp = '\0';
                    break;
                }

                if (ch == ',' && depth == 0)
                {
                    *dp++ = '\0';
                    ++nrcommas;
                    argstart = TRUE;
                }
                else
                {
                    *dp++ = ch;

                    /*
                     * Make sure commas in template
                     * arguments are ignored.
                     */
                    if (ch == '<')
                        ++depth;
                    else if (ch == '>')
                        --depth;
                }
            }
            else if (ch == ' ')
            {
                /* Ignore leading and multiple spaces. */
                if (!argstart && dp[-1] != ' ')
                    *dp++ = ch;
            }
            else
            {
                *dp++ = ch;
                argstart = FALSE;
            }
        }

        /* Handle the arguments now they are in a normal form. */
        if (*psig->sg_signature)
        {
            char *arg = psig->sg_signature;
            int a;

            /* Allocate the space. */
            psig->sg_nrargs = nrcommas + 1;

            if ((psig->sg_args = (sipSigArg *)sip_api_malloc(sizeof (sipSigArg) * psig->sg_nrargs)) == NULL)
            {
                sip_api_free(psig);
                return NULL;
            }

            for (a = 0; a < psig->sg_nrargs; ++a)
            {
                size_t btlen = 0;
                int unsup, isref = FALSE, indir = 0;
                sipSigArgType sat = unknown_sat;

                /* Find the start of the significant part of the type. */
                dp = arg;

                if (strncmp(dp, "const ", 6) == 0)
                    dp += 6;

                /*
                 * Find the length of the base type, the number of indirections
                 * and if it is a reference.
                 */
                for (ep = dp; *ep; ++ep)
                    if (*ep == '&')
                        isref = TRUE;
                    else if (*ep == '*')
                        ++indir;
                    else
                        ++btlen;

                /*
                 * Assume that anything other than a base type is unsupported.
                 */
                unsup = (isref || indir);

                /* Parse the base type. */
                switch (btlen)
                {
                case 3:
                    if (strncmp(dp, "int", 3) == 0)
                        sat = int_sat;
                    break;

                case 4:
                    if (strncmp(dp, "bool", 4) == 0)
                        sat = bool_sat;
                    else if (strncmp(dp, "long", 4) == 0)
                        sat = long_sat;
                    else if (strncmp(dp, "char", 4) == 0)
                    {
                        sat = (indir ? string_sat : char_sat);
                        unsup = (isref || indir > 1);
                    }
                    else if (strncmp(dp, "void", 4) == 0)
                    {
                        sat = void_sat;
                        unsup = (isref || indir != 1);
                    }
                    break;

                case 5:
                    if (strncmp(dp, "float", 5) == 0)
                        sat = float_sat;
                    else if (strncmp(dp, "short", 5) == 0)
                        sat = short_sat;
                    break;

                case 6:
                    if (strncmp(dp, "double", 6) == 0)
                        sat = double_sat;
                    break;

                case 7:
                    if (strncmp(dp, "__int64", 7) == 0)
                        sat = longlong_sat;
                    break;

                case 8:
                    if (strncmp(dp, "unsigned", 8) == 0)
                        sat = uint_sat;
                    else if (strncmp(dp, "QVariant", 8) == 0)
                    {
                        if (indir == 0)
                        {
                            sat = qvariant_sat;
                            unsup = FALSE;
                        }
                        else if (indir == 1)
                        {
                            sat = qvariantp_sat;
                            unsup = FALSE;
                        }
                    }
                    else if (strncmp(dp, "PyObject", 8) == 0 && indir == 1)
                    {
                        sat = pyobject_sat;
                        unsup = FALSE;
                    }
                    break;

                case 9:
                    if (strncmp(dp, "long long", 9) == 0)
                        sat = longlong_sat;
                    break;

                case 11:
                    if (strncmp(dp, "signed char", 11) == 0)
                    {
                        sat = (indir ? sstring_sat : schar_sat);
                        unsup = (isref || indir > 1);
                    }
                    break;

                case 12:
                    if (strncmp(dp, "unsigned int", 12) == 0)
                        sat = uint_sat;
                    break;

                case 13:
                    if (strncmp(dp, "unsigned long", 13) == 0)
                        sat = ulong_sat;
                    else if (strncmp(dp, "unsigned char", 13) == 0)
                    {
                        sat = (indir ? ustring_sat : uchar_sat);
                        unsup = (isref || indir > 1);
                    }
                    break;

                case 14:
                    if (strncmp(dp, "unsigned short", 14) == 0)
                        sat = ushort_sat;
                    break;

                case 16:
                    if (strncmp(dp, "unsigned __int64", 16) == 0)
                        sat = ulonglong_sat;
                    break;

                case 18:
                    if (strncmp(dp, "unsigned long long", 18) == 0)
                        sat = ulonglong_sat;
                    break;
                }

                if (sat == unknown_sat)
                    sipFindSigArgType(dp, btlen, &psig->sg_args[a], indir);
                else
                {
                    if (unsup)
                        sat = unknown_sat;

                    psig->sg_args[a].atype = sat;
                }

                /* Move to the start of the next argument. */
                arg += strlen(arg) + 1;
            }
        }
    }

    /* Make a deep copy of the signal. */
    strcpy(psig->sg_signature, sig);

    /* Add it to the list so it can be re-used. */
    psig->sg_next = psig_list;
    psig_list = psig;

    return psig;
}


/*
 * Find an existing signal.
 */
static void *findSignal(void *txrx, const char **sig)
{
    sipSignature *psig;

    /*
     * Handle the trivial case where the Qt implementation doesn't support
     * universal signals.
     */
    if (sipQtSupport->qt_is_qt_signal == NULL)
        return txrx;

    /* See if this a shortcircuited Python signal. */
    if (strchr(*sig, '(') == NULL)
        return sipQtSupport->qt_find_universal_signal_shortcut(txrx, *sig, sig);

    /* See if the existing object can be used itself. */
    if (sipQtSupport->qt_is_qt_signal(txrx, *sig))
        return txrx;

    if ((psig = parseSignature(*sig)) == NULL)
        return NULL;

    /* Find an ordinary universal signal. */
    return sipQtSupport->qt_find_universal_signal(txrx, psig);
}


/*
 * Return a usable signal, creating a new universal signal if needed.
 */
static void *newSignal(void *txrx, const char **sig)
{
    sipSignature *psig;

    /*
     * Handle the trivial case where the Qt implementation doesn't support
     * universal signals.
     */
    if (sipQtSupport->qt_is_qt_signal == NULL)
        return txrx;

    /* See if this a shortcircuited Python signal. */
    if (strchr(*sig, '(') == NULL)
        return sipQtSupport->qt_create_universal_signal_shortcut(txrx, *sig, sig);

    /* See if the existing object can be used itself. */
    if (sipQtSupport->qt_is_qt_signal(txrx, *sig))
        return txrx;

    if ((psig = parseSignature(*sig)) == NULL)
        return NULL;

    /* Create an ordinary universal signal. */
    return sipQtSupport->qt_create_universal_signal(txrx, psig);
}


/*
 * Create a universal slot.  Returns a pointer to it or 0 if there was an
 * error.
 */
static void *createUniversalSlot(sipWrapper *txSelf, const char *sig,
                 PyObject *rxObj, const char *slot,
                 const char **member)
{
    sipSlotConnection conn;
    void *us;

    /* Initialise the connection. */
    conn.sc_transmitter = (txSelf ? sipGetAddress(txSelf) : 0);

    /* Save the real slot. */
    if (saveSlot(&conn.sc_slot, rxObj, slot) < 0)
        return 0;

    /* Parse the signature and create the universal slot. */
    if ((conn.sc_signature = parseSignature(sig)) == NULL || (us = sipQtSupport->qt_create_universal_slot(txSelf, &conn, member)) == NULL)
    {
        sip_api_free_connection(&conn);
        return 0;
    }

    return us;
}


/*
 * Emit a Python or Qt signal.
 */
int sip_api_emit_signal(PyObject *self,const char *sig,PyObject *sigargs)
{
    sipPySig *ps;
    void *tx;
    sipWrapper *w = (sipWrapper *)self;

    /*
     * Don't do anything if signals are blocked.  Qt signals would be blocked
     * anyway, but this blocks Python signals as well.
     */
    if ((tx = sip_api_get_cpp_ptr(w, sipQObjectClass)) == NULL || sipQtSupport->qt_signals_blocked(tx))
        return 0;

    if (isQtSignal(sig))
    {
        sipSignature *psig;

        /* Handle Qt implementations that emit using generated code. */
        if (!sipQtSupport->qt_emit_signal)
            return emitQtSig(w, sig, sigargs);

        /* See if the signal is a shortcut. */
        if (strchr(sig, '(') == NULL)
            return sipQtSupport->qt_emit_signal_shortcut(tx, sig, sigargs);

        if ((psig = parseSignature(sig)) == NULL)
            return -1;

        if (psig->sg_nrargs != PyTuple_GET_SIZE(sigargs))
            PyErr_Format(PyExc_TypeError, "Signal has %d arguments, but %d given", psig->sg_nrargs, PyTuple_GET_SIZE(sigargs));

        return sipQtSupport->qt_emit_signal(tx, psig, sigargs);
    }

    if ((ps = findPySignal(w,sig)) != NULL)
    {
        int rc;

        /* Forget the last Qt sender and remember this one. */
        sipQtSupport->qt_forget_sender();
        py_sender = self;

        rc = emitToSlotList(ps -> rxlist,sigargs);

        /* Forget this as a sender. */
        py_sender = NULL;

        return rc;
    }

    return 0;
}


/*
 * Search the Python signal list for a signal.
 */
static sipPySig *findPySignal(sipWrapper *w,const char *sig)
{
    sipPySig *ps;

    for (ps = w -> pySigList; ps != NULL; ps = ps -> next)
        if (sipQtSupport->qt_same_name(ps -> name,sig))
            return ps;

    return NULL;
}


/*
 * Search a signal table for a signal.  If found, call the emitter function
 * with the signal arguments.  Return 0 if the signal was emitted or <0 if
 * there was an error.
 */
static int emitQtSig(sipWrapper *w,const char *sig,PyObject *sigargs)
{
    sipQtSignal *tab;

    /* Search the table. */
    for (tab = ((sipWrapperType *)(w -> ob_type)) -> type -> td_emit; tab -> st_name != NULL; ++tab)
    {
        const char *sp, *tp;
        int found;

        /* Compare only the base name. */
        sp = &sig[1];
        tp = tab -> st_name;

        found = TRUE;

        while (*sp != '\0' && *sp != '(' && *tp != '\0')
            if (*sp++ != *tp++)
            {
                found = FALSE;
                break;
            }

        if (found)
            return (*tab -> st_emitfunc)(w,sigargs);
    }

    /* It wasn't found if we got this far. */
    PyErr_Format(PyExc_NameError,"Invalid signal %s",&sig[1]);

    return -1;
}


/*
 * Send a signal to a single slot (Qt or Python).
 */
int sip_api_emit_to_slot(sipSlot *slot, PyObject *sigargs)
{
    PyObject *sa, *oxtype, *oxvalue, *oxtb, *sfunc, *newmeth, *sref;

    /* Keep some compilers quiet. */
    oxtype = oxvalue = oxtb = NULL;

    /* Fan out Qt signals. */
    if (slot -> name != NULL && slot -> name[0] != '\0')
        return sip_api_emit_signal(slot -> pyobj,slot -> name,sigargs);

    /* Get the object to call, resolving any weak references. */
    if (slot -> weakSlot == NULL)
        sref = NULL;
    else if ((sref = PyWeakref_GetObject(slot -> weakSlot)) == NULL)
        return -1;
    else
        Py_INCREF(sref);

    if (sref == Py_None)
    {
        /*
         * If the real object has gone then we pretend everything is Ok.  This
         * mimics the Qt behaviour of not caring if a receiving object has been
         * deleted.
         */
        Py_DECREF(sref);
        return 0;
    }

    if (slot -> pyobj == NULL)
    {
        PyObject *self = (sref != NULL ? sref : slot->meth.mself);

        /* See if any underlying C++ instance has gone. */
        if (self != NULL && sip_api_wrapper_check(self) && ((sipWrapper *)self)->u.cppPtr == NULL)
        {
            Py_XDECREF(sref);
            return 0;
        }

        if ((sfunc = PyMethod_New(slot->meth.mfunc, self, slot->meth.mclass)) == NULL)
        {
            Py_XDECREF(sref);
            return -1;
        }

        /* Make sure we garbage collect the new method. */
        newmeth = sfunc;
    }
    else if (slot -> name != NULL)
    {
        char *mname = slot -> name + 1;
        PyObject *self = (sref != NULL ? sref : slot->pyobj);

        /* See if any underlying C++ instance has gone. */
        if (self != NULL && sip_api_wrapper_check(self) && ((sipWrapper *)self)->u.cppPtr == NULL)
        {
            Py_XDECREF(sref);
            return 0;
        }

        if ((sfunc = PyObject_GetAttrString(self, mname)) == NULL || !PyCFunction_Check(sfunc))
        {
            /*
             * Note that in earlier versions of SIP this error would be
             * detected when the slot was connected.
             */
            PyErr_Format(PyExc_NameError,"Invalid slot %s",mname);

            Py_XDECREF(sref);
            return -1;
        }

        /* Make sure we garbage collect the new method. */
        newmeth = sfunc;
    }
    else
    {
        sfunc = slot -> pyobj;
        newmeth = NULL;
    }

    /*
     * We make repeated attempts to call a slot.  If we work out that it failed
     * because of an immediate type error we try again with one less argument.
     * We keep going until we run out of arguments to drop.  This emulates the
     * Qt ability of the slot to accept fewer arguments than a signal provides.
     */
    sa = sigargs;
    Py_INCREF(sa);

    for (;;)
    {
        PyObject *nsa, *xtype, *xvalue, *xtb, *resobj;

        if ((resobj = PyEval_CallObject(sfunc,sa)) != NULL)
        {
            Py_DECREF(resobj);

            Py_XDECREF(newmeth);
            Py_XDECREF(sref);

            /* Remove any previous exception. */

            if (sa != sigargs)
            {
                Py_XDECREF(oxtype);
                Py_XDECREF(oxvalue);
                Py_XDECREF(oxtb);
                PyErr_Clear();
            }

            Py_DECREF(sa);

            return 0;
        }

        /* Get the exception. */
        PyErr_Fetch(&xtype,&xvalue,&xtb);

        /*
         * See if it is unacceptable.  An acceptable failure is a type error
         * with no traceback - so long as we can still reduce the number of
         * arguments and try again.
         */
        if (!PyErr_GivenExceptionMatches(xtype,PyExc_TypeError) ||
            xtb != NULL ||
            PyTuple_GET_SIZE(sa) == 0)
        {
            /*
             * If there is a traceback then we must have called the slot and
             * the exception was later on - so report the exception as is.
             */
            if (xtb != NULL)
            {
                if (sa != sigargs)
                {
                    Py_XDECREF(oxtype);
                    Py_XDECREF(oxvalue);
                    Py_XDECREF(oxtb);
                }

                PyErr_Restore(xtype,xvalue,xtb);
            }
            else if (sa == sigargs)
                PyErr_Restore(xtype,xvalue,xtb);
            else
            {
                /*
                 * Discard the latest exception and restore the original one.
                 */
                Py_XDECREF(xtype);
                Py_XDECREF(xvalue);
                Py_XDECREF(xtb);

                PyErr_Restore(oxtype,oxvalue,oxtb);
            }

            break;
        }

        /* If this is the first attempt, save the exception. */
        if (sa == sigargs)
        {
            oxtype = xtype;
            oxvalue = xvalue;
            oxtb = xtb;
        }
        else
        {
            Py_XDECREF(xtype);
            Py_XDECREF(xvalue);
            Py_XDECREF(xtb);
        }

        /* Create the new argument tuple. */
        if ((nsa = PyTuple_GetSlice(sa,0,PyTuple_GET_SIZE(sa) - 1)) == NULL)
        {
            /* Tidy up. */
            Py_XDECREF(oxtype);
            Py_XDECREF(oxvalue);
            Py_XDECREF(oxtb);

            break;
        }

        Py_DECREF(sa);
        sa = nsa;
    }

    Py_XDECREF(newmeth);
    Py_XDECREF(sref);

    Py_DECREF(sa);

    return -1;
}


/*
 * Send a signal to the slots (Qt or Python) in a Python list.
 */
static int emitToSlotList(sipPySigRx *rxlist,PyObject *sigargs)
{
    int rc;

    /* Apply the arguments to each slot method. */
    rc = 0;

    while (rxlist != NULL && rc >= 0)
    {
        sipPySigRx *next;

        /*
         * We get the next in the list before calling the slot in case the list
         * gets changed by the slot - usually because the slot disconnects
         * itself.
         */
        next = rxlist -> next;
        rc = sip_api_emit_to_slot(&rxlist -> rx, sigargs);
        rxlist = next;
    }

    return rc;
}


/*
 * Add a slot to a transmitter's Python signal list.  The signal is a Python
 * signal, the slot may be either a Qt signal, a Qt slot, a Python signal or a
 * Python slot.
 */
static int addSlotToPySigList(sipWrapper *txSelf,const char *sig,
                  PyObject *rxObj,const char *slot)
{
    sipPySig *ps;
    sipPySigRx *psrx;

    /* Create a new one if necessary. */
    if ((ps = findPySignal(txSelf,sig)) == NULL)
    {
        if ((ps = (sipPySig *)sip_api_malloc(sizeof (sipPySig))) == NULL)
            return -1;

        if ((ps -> name = sipStrdup(sig)) == NULL)
        {
            sip_api_free(ps);
            return -1;
        }

        ps -> rxlist = NULL;
        ps -> next = txSelf -> pySigList;

        txSelf -> pySigList = ps;
    }

    /* Create the new receiver. */
    if ((psrx = (sipPySigRx *)sip_api_malloc(sizeof (sipPySigRx))) == NULL)
        return -1;

    if (saveSlot(&psrx->rx, rxObj, slot) < 0)
    {
        sip_api_free(psrx);
        return -1;
    }

    psrx -> next = ps -> rxlist;
    ps -> rxlist = psrx;

    return 0;
}


/*
 * Compare two slots to see if they are the same.
 */
static int isSameSlot(sipSlot *slot1,PyObject *rxobj2,const char *slot2)
{
    /* See if they are signals or Qt slots, ie. they have a name. */
    if (slot1 -> name != NULL)
        return (slot2 != NULL &&
            sipQtSupport->qt_same_name(slot1 -> name,slot2) &&
            slot1 -> pyobj == rxobj2);

    /* Both must be Python slots. */
    if (slot2 != NULL)
        return 0;

    /* See if they are Python methods. */
    if (slot1 -> pyobj == NULL)
        return (PyMethod_Check(rxobj2) &&
            slot1 -> meth.mfunc == PyMethod_GET_FUNCTION(rxobj2) &&
            slot1 -> meth.mself == PyMethod_GET_SELF(rxobj2) &&
            slot1 -> meth.mclass == PyMethod_GET_CLASS(rxobj2));

    if (PyMethod_Check(rxobj2))
        return 0;

    /* The objects must be the same. */
    return (slot1 -> pyobj == rxobj2);
}


/*
 * Convert a valid Python signal or slot to an existing universal slot.
 */
void *sipGetRx(sipWrapper *txSelf,const char *sigargs,PyObject *rxObj,
           const char *slot,const char **memberp)
{
    if (slot != NULL)
        if (isQtSlot(slot) || isQtSignal(slot))
        {
            void *rx;

            *memberp = slot;

            if ((rx = sip_api_get_cpp_ptr((sipWrapper *)rxObj, sipQObjectClass)) == NULL)
                return NULL;

            if (isQtSignal(slot))
                rx = findSignal(rx, memberp);

            return rx;
        }

    /*
     * The slot was either a Python callable or PyQt3 Python signal so there
     * should be a universal slot.
     */
    return sipQtSupport->qt_find_slot(sipGetAddress(txSelf), sigargs, rxObj, slot, memberp);
}


/*
 * Convert a Python receiver (either a Python signal or slot or a Qt signals or
 * slot) to a Qt receiver.  It is only ever called when the signal is a Qt
 * signal.  Return NULL is there was an error.
 */
void *sip_api_convert_rx(sipWrapper *txSelf,const char *sig,PyObject *rxObj,
             const char *slot,const char **memberp)
{
    if (slot == NULL)
        return createUniversalSlot(txSelf, sig, rxObj, NULL, memberp);

    if (isQtSlot(slot) || isQtSignal(slot))
    {
        void *rx;

        *memberp = slot;

        if ((rx = sip_api_get_cpp_ptr((sipWrapper *)rxObj, sipQObjectClass)) == NULL)
            return NULL;

        if (isQtSignal(slot))
            rx = newSignal(rx, memberp);

        return rx;
    }

    /* The slot is a Python signal so we need a universal slot to catch it. */
    return createUniversalSlot(txSelf, sig, rxObj, slot, memberp);
}


/*
 * Connect a Qt signal or a Python signal to a Qt slot, a Qt signal, a Python
 * slot or a Python signal.  This is all possible combinations.
 */
PyObject *sip_api_connect_rx(PyObject *txObj,const char *sig,PyObject *rxObj,
                 const char *slot, int type)
{
    sipWrapper *txSelf = (sipWrapper *)txObj;

    /* Handle Qt signals. */
    if (isQtSignal(sig))
    {
        void *tx, *rx;
        const char *member, *real_sig;
        int res;

        if ((tx = sip_api_get_cpp_ptr(txSelf, sipQObjectClass)) == NULL)
            return NULL;

        real_sig = sig;

        if ((tx = newSignal(tx, &real_sig)) == NULL)
            return NULL;

        if ((rx = sip_api_convert_rx(txSelf, sig, rxObj, slot, &member)) == NULL)
            return NULL;

        Py_BEGIN_ALLOW_THREADS
        res = sipQtSupport->qt_connect(tx, real_sig, rx, member, type);
        Py_END_ALLOW_THREADS

        return PyBool_FromLong(res);
    }

    /* Handle Python signals. */
    if (addSlotToPySigList(txSelf, sig, rxObj, slot) < 0)
        return NULL;

    Py_INCREF(Py_True);
    return Py_True;
}


/*
 * Disconnect a signal to a signal or a Qt slot.
 */
PyObject *sip_api_disconnect_rx(PyObject *txObj,const char *sig,
                PyObject *rxObj,const char *slot)
{
    sipWrapper *txSelf = (sipWrapper *)txObj;

    /* Handle Qt signals. */
    if (isQtSignal(sig))
    {
        void *tx, *rx;
        const char *member;
        PyObject *res;

        if ((tx = sip_api_get_cpp_ptr(txSelf, sipQObjectClass)) == NULL)
            return NULL;

        if ((rx = sipGetRx(txSelf, sig, rxObj, slot, &member)) == NULL)
        {
            Py_INCREF(Py_False);
            return Py_False;
        }

        /* Handle Python signals. */
        tx = findSignal(tx, &sig);

        res = PyBool_FromLong(sipQtSupport->qt_disconnect(tx, sig, rx, member));

        /*
         * Delete it if it is a universal slot as this will be it's only
         * connection.  If the slot is actually a universal signal then it
         * should leave it in place.
         */
        sipQtSupport->qt_destroy_universal_slot(rx);

        return res;
    }

    /* Handle Python signals. */
    removeSlotFromPySigList(txSelf,sig,rxObj,slot);

    Py_INCREF(Py_True);
    return Py_True;
}


/*
 * Remove a slot from a transmitter's Python signal list.
 */
static void removeSlotFromPySigList(sipWrapper *txSelf,const char *sig,
                    PyObject *rxObj,const char *slot)
{
    sipPySig *ps;

    if ((ps = findPySignal(txSelf,sig)) != NULL)
    {
        sipPySigRx **psrxp;

        for (psrxp = &ps -> rxlist; *psrxp != NULL; psrxp = &(*psrxp) -> next)
        {
            sipPySigRx *psrx = *psrxp;

            if (isSameSlot(&psrx -> rx,rxObj,slot))
            {
                *psrxp = psrx -> next;
                sipFreePySigRx(psrx);
                break;
            }
        }
    }
}


/*
 * Free a sipSlot structure.
 */
static void freeSlot(sipSlot *slot)
{
    if (slot->name != NULL)
        sip_api_free(slot->name);

    /* Remove any weak reference. */
    Py_XDECREF(slot->weakSlot);
}


/*
 * Free a sipPySigRx structure on the heap.
 */
void sipFreePySigRx(sipPySigRx *rx)
{
    freeSlot(&rx->rx);
    sip_api_free(rx);
}


/*
 * Implement strdup() using sip_api_malloc().
 */
static char *sipStrdup(const char *s)
{
    char *d;

    if ((d = (char *)sip_api_malloc(strlen(s) + 1)) != NULL)
        strcpy(d,s);

    return d;
}


/*
 * Initialise a slot, returning 0 if there was no error.  If the signal was a
 * Qt signal, then the slot may be a Python signal or a Python slot.  If the
 * signal was a Python signal, then the slot may be anything.
 */
static int saveSlot(sipSlot *sp, PyObject *rxObj, const char *slot)
{
    sp -> weakSlot = NULL;

    if (slot == NULL)
    {
        sp -> name = NULL;

        if (PyMethod_Check(rxObj))
        {
            /*
             * Python creates methods on the fly.  We could increment the
             * reference count to keep it alive, but that would keep "self"
             * alive as well and would probably be a circular reference.
             * Instead we remember the component parts and hope they are still
             * valid when we re-create the method when we need it.
             */
            sipSaveMethod(&sp -> meth,rxObj);

            /* Notice if the class instance disappears. */
            sp -> weakSlot = getWeakRef(sp -> meth.mself);

            /* This acts a flag to say that the slot is a method. */
            sp -> pyobj = NULL;
        }
        else
        {
            PyObject *self;

            /*
             * We know that it is another type of callable, ie. a
             * function/builtin.
             */

            if (PyCFunction_Check(rxObj) &&
                (self = PyCFunction_GET_SELF(rxObj)) != NULL &&
                sip_api_wrapper_check(self))
            {
                /*
                 * It is a wrapped C++ class method.  We can't keep a copy
                 * because they are generated on the fly and we can't take a
                 * reference as that may keep the instance (ie. self) alive.
                 * We therefore treat it as if the user had specified the slot
                 * at "obj, SLOT('meth()')" rather than "obj.meth" (see below).
                 */

                const char *meth;

                /* Get the method name. */
                meth = ((PyCFunctionObject *)rxObj) -> m_ml -> ml_name;

                if ((sp -> name = (char *)sip_api_malloc(strlen(meth) + 2)) == NULL)
                    return -1;

                /*
                 * Copy the name and set the marker that it needs converting to
                 * a built-in method.
                 */
                sp -> name[0] = '\0';
                strcpy(&sp -> name[1],meth);

                sp -> pyobj = self;
                sp -> weakSlot = getWeakRef(self);
            }
            else
            {
                /*
                 * It's unlikely that we will succeed in getting a weak
                 * reference to the slot, but there is no harm in trying (and
                 * future versions of Python may support references to more
                 * object types).
                 */
                sp -> pyobj = rxObj;
                sp -> weakSlot = getWeakRef(rxObj);
            }
        }
    }
    else if ((sp -> name = sipStrdup(slot)) == NULL)
        return -1;
    else if (isQtSlot(slot))
    {
        /*
         * The user has decided to connect a Python signal to a Qt slot and
         * specified the slot as "obj, SLOT('meth()')" rather than "obj.meth".
         */

        char *tail;

        /* Remove any arguments. */
        if ((tail = strchr(sp -> name,'(')) != NULL)
            *tail = '\0';

        /*
         * A bit of a hack to indicate that this needs converting to a built-in
         * method.
         */
        sp -> name[0] = '\0';

        /* Notice if the class instance disappears. */
        sp -> weakSlot = getWeakRef(rxObj);

        sp -> pyobj = rxObj;
    }
    else
        /* It's a Qt signal. */
        sp -> pyobj = rxObj;

    return 0;
}


/*
 * Return a weak reference to the given object.
 */
static PyObject *getWeakRef(PyObject *obj)
{
    PyObject *wr;

    if ((wr = PyWeakref_NewRef(obj,NULL)) == NULL)
        PyErr_Clear();

    return wr;
}
