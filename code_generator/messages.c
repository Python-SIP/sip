/*
 * Functions that display messages to the user.
 *
 * Copyright (c) 2019 Riverbank Computing Limited <info@riverbankcomputing.com>
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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sip.h"


/* The text of the current error message. */
static char error_text[1000];
static int reset_error_text = TRUE;


/*
 * Display a warning message.
 */
void warning(Warning w, const char *fmt, ...)
{
    static int start = TRUE;

    va_list ap;

    /* Don't allow deprecation warnings to be suppressed. */
    if (!warnings && w != DeprecationWarning)
        return;

    if (start)
    {
        const char *wstr;

        switch (w)
        {
        case ParserWarning:
            wstr = "Parser warning";
            break;

        case DeprecationWarning:
            wstr = "Deprecation warning";
            break;
        }

        fprintf(stderr, "sip5: %s: ", wstr);
        start = FALSE;
    }

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    if (strchr(fmt, '\n') != NULL)
    {
        if (warnings_are_fatal)
            exit(1);

        start = TRUE;
    }
}


/*
 * Display a one line error message describing a fatal error.  This does not
 * return.
 */
void fatal(const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(error_text);
    size_t room = sizeof (error_text) - used - 1;

    fatalStart();

    va_start(ap,fmt);
    vsnprintf(&error_text[used], room, fmt, ap);
    va_end(ap);

    /*
     * The error text buffer may be used more than once as it used to raise a
     * Python exception which could be ignored.
     */
    reset_error_text = TRUE;

    /* TODO: longjmp() to 'raise' an exception. */
    exit(1);
}


/*
 * Append to the current error message.
 */
void fatalAppend(const char *fmt, ...)
{
    va_list ap;
    size_t used = strlen(error_text);
    size_t room = sizeof (error_text) - used - 1;

    va_start(ap, fmt);
    vsnprintf(&error_text[used], room, fmt, ap);
    va_end(ap);
}


/*
 * Make sure the start of a fatal message is handled.
 */
void fatalStart()
{
    if (reset_error_text)
    {
        error_text[0] = '\0';
        reset_error_text = FALSE;
    }
}
