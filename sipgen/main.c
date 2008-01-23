/*
 * The main module for SIP.
 *
 * @BS_LICENSE@
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "sip.h"


#ifndef PACKAGE
#define PACKAGE "sip"
#endif

#define VERSION "@RM_LATEST@"


/* Global variables - see sip.h for their meaning. */
char *sipVersion;
stringList *includeDirList;

static char *sipPackage = PACKAGE;
static int warnings = FALSE;


static void help(void);
static void version(void);
static void usage(void);
static char parseopt(int,char **,char *,char **,int *,char **);
static int parseInt(char *,char);


int main(int argc, char **argv)
{
    char *filename, *docFile, *codeDir, *srcSuffix, *flagFile, *consModule;
    char arg, *optarg, *buildFile, *apiFile, *xmlFile;
    int optnr, exceptions, tracing, releaseGIL, parts;
    FILE *file;
    sipSpec spec;
    stringList *versions, *xfeatures;

    /* Initialise. */
    sipVersion = VERSION;
    includeDirList = NULL;
    versions = NULL;
    xfeatures = NULL;
    buildFile = NULL;
    codeDir = NULL;
    docFile = NULL;
    srcSuffix = NULL;
    flagFile = NULL;
    apiFile = NULL;
    xmlFile = NULL;
    consModule = NULL;
    exceptions = FALSE;
    tracing = FALSE;
    releaseGIL = FALSE;
    parts = 0;

    /* Parse the command line. */
    optnr = 1;

    while ((arg = parseopt(argc, argv, "hVa:b:ec:d:gI:j:m:p:rs:t:wx:z:", &flagFile, &optnr, &optarg)) != '\0')
        switch (arg)
        {
        case 'p':
            /* The name of the consolidated module. */
            consModule = optarg;
            break;

        case 'a':
            /* Where to generate the API file. */
            apiFile = optarg;
            break;

        case 'm':
            /* Where to generate the XML file. */
            xmlFile = optarg;
            break;

        case 'b':
            /* Generate a build file. */
            buildFile = optarg;
            break;

        case 'e':
            /* Enable exceptions. */
            exceptions = TRUE;
            break;

        case 'g':
            /* Always release the GIL. */
            releaseGIL = TRUE;
            break;

        case 'j':
            /* Generate the code in this number of parts. */
            parts = parseInt(optarg,'j');
            break;

        case 'z':
            /* Read a file for the next flags. */
            if (flagFile != NULL)
                fatal("The -z flag cannot be specified in an argument file\n");

            flagFile = optarg;
            break;

        case 'c':
            /* Where to generate the code. */
            codeDir = optarg;
            break;

        case 'd':
            /* Where to generate the documentation. */
            docFile = optarg;
            break;

        case 't':
            /* Which platform or version to generate code for. */
            appendString(&versions,optarg);
            break;

        case 'x':
            /* Which features are disabled. */
            appendString(&xfeatures,optarg);
            break;

        case 'I':
            /* Where to get included files from. */
            appendString(&includeDirList,optarg);
            break;

        case 'r':
            /* Enable tracing. */
            tracing = TRUE;
            break;

        case 's':
            /* The suffix to use for source files. */
            srcSuffix = optarg;
            break;

        case 'w':
            /* Enable warning messages. */
            warnings = TRUE;
            break;

        case 'h':
            /* Help message. */
            help();
            break;

        case 'V':
            /* Display the version number. */
            version();
            break;

        default:
            usage();
        }

    if (optnr < argc)
    {
        file = NULL;
        filename = argv[optnr++];

        if (optnr < argc)
            usage();
    }
    else
    {
        file = stdin;
        filename = "stdin";
    }

    /* Parse the input file. */
    parse(&spec, file, filename, versions, xfeatures);

    /* Verify and transform the parse tree. */
    transform(&spec);

    /* Generate code. */
    generateCode(&spec, codeDir, buildFile, docFile, srcSuffix, exceptions,
            tracing, releaseGIL, parts, xfeatures, consModule);

    /* Generate the API file. */
    if (apiFile != NULL)
        generateAPI(&spec, spec.module, apiFile);

    /* Generate the XML export. */
    if (xmlFile != NULL)
        generateXML(&spec, spec.module,  xmlFile);

    /* All done. */
    return 0;
}


/*
 * Parse the next command line argument - similar to UNIX getopts().  Allow a
 * flag to specify that a file contains further arguments.
 */
static char parseopt(int argc, char **argv, char *opts, char **flags,
        int *optnrp, char **optargp)
{
    char arg, *op, *fname;
    int optnr;
    static FILE *fp = NULL;

    /* Deal with any file first. */

    fname = *flags;

    if (fname != NULL && fp == NULL && (fp = fopen(fname,"r")) == NULL)
        fatal("Unable to open %s\n",fname);

    if (fp != NULL)
    {
        char buf[200], *cp, *fname;
        int ch;

        fname = *flags;
        cp = buf;

        while ((ch = fgetc(fp)) != EOF)
        {
            /* Skip leading whitespace. */

            if (cp == buf && isspace(ch))
                continue;

            if (ch == '\n')
                break;

            if (cp == &buf[sizeof (buf) - 1])
                fatal("A flag in %s is too long\n",fname);

            *cp++ = (char)ch;
        }

        *cp = '\0';

        if (ch == EOF)
        {
            fclose(fp);
            fp = NULL;
            *flags = NULL;
        }

        /*
         * Get the option character and any optional argument from the
         * line.
         */

        if (buf[0] != '\0')
        {
            if (buf[0] != '-' || buf[1] == '\0')
                fatal("An non-flag was given in %s\n",fname);

            arg = buf[1];

            /* Find any optional argument. */

            for (cp = &buf[2]; *cp != '\0'; ++cp)
                if (!isspace(*cp))
                    break;

            if (*cp == '\0')
                cp = NULL;
            else
                cp = sipStrdup(cp);

            *optargp = cp;

            if ((op = strchr(opts,arg)) == NULL)
                fatal("An invalid flag was given in %s\n",fname);

            if (op[1] == ':' && cp == NULL)
                fatal("Missing flag argument in %s\n",fname);

            if (op[1] != ':' && cp != NULL)
                fatal("Unexpected flag argument in %s\n",fname);

            return arg;
        }
    }

    /* Check there is an argument and it is a switch. */

    optnr = *optnrp;

    if (optnr >= argc || argv[optnr] == NULL || argv[optnr][0] != '-')
        return '\0';

    /* Check it is a valid switch. */

    arg = argv[optnr][1];

    if (arg == '\0' || (op = strchr(opts,arg)) == NULL)
        usage();

    /* Check for the switch parameter, if any. */

    if (op[1] == ':')
    {
        if (argv[optnr][2] != '\0')
        {
            *optargp = &argv[optnr][2];
            ++optnr;
        }
        else if (optnr + 1 >= argc || argv[optnr + 1] == NULL)
            usage();
        else
        {
            *optargp = argv[optnr + 1];
            optnr += 2;
        }
    }
    else if (argv[optnr][2] != '\0')
        usage();
    else
    {
        *optargp = NULL;
        ++optnr;
    }

    *optnrp = optnr;

    return arg;
}


/*
 * Parse an integer option.
 */
static int parseInt(char *arg, char opt)
{
    char *endptr;
    int val;

    val = strtol(arg, &endptr, 10);

    if (*arg == '\0' || *endptr != '\0')
        fatal("Invalid integer argument for -%c flag\n", opt);

    return val;
}


/*
 * Append a string to a list of them.
 */
void appendString(stringList **headp, const char *s)
{
    stringList *sl;

    /* Create the new entry. */

    sl = sipMalloc(sizeof (stringList));

    sl -> s = s;
    sl -> next = NULL;

    /* Append it to the list. */

    while (*headp != NULL)
        headp = &(*headp) -> next;

    *headp = sl;
}


/*
 * Display a warning message.
 */
void warning(char *fmt,...)
{
    static int start = TRUE;

    va_list ap;

    if (!warnings)
        return;

    if (start)
    {
        fprintf(stderr,"%s: Warning: ",sipPackage);
        start = FALSE;
    }

    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    va_end(ap);

    if (strchr(fmt,'\n') != NULL)
        start = TRUE;
}


/*
 * Display all or part of a one line error message describing a fatal error.
 * If the message is complete (it has a newline) then the program exits.
 */
void fatal(char *fmt,...)
{
    static int start = TRUE;

    va_list ap;

    if (start)
    {
        fprintf(stderr,"%s: ",sipPackage);
        start = FALSE;
    }

    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    va_end(ap);

    if (strchr(fmt,'\n') != NULL)
        exit(1);
}


/*
 * Display the SIP version number on stdout and exit with zero exit status.
 */
static void version(void)
{
    printf("%s\n",sipVersion);
    exit(0);
}


/*
 * Display the help message on stdout and exit with zero exit status.
 */
static void help(void)
{
    printf(
"Usage:\n"
"    %s [-h] [-V] [-a file] [-c dir] [-d file] [-e] [-g] [-I dir] [-j #] [-m file] [-p module] [-r] [-s suffix] [-t tag] [-w] [-x feature] [-z file] [file]\n"
"where:\n"
"    -h          display this help message\n"
"    -V          display the %s version number\n"
"    -a file     the name of the QScintilla API file [default not generated]\n"
"    -b file     the name of the build file [default none generated]\n"
"    -c dir      the name of the code directory [default not generated]\n"
"    -d file     the name of the documentation file [default not generated]\n"
"    -e          enable support for exceptions [default disabled]\n"
"    -g          always release and reacquire the GIL [default only when specified]\n"
"    -I dir      look in this directory when including files\n"
"    -j #        split the generated code into # files [default 1 per class]\n"
"    -m file     the name of the XML export file [default not generated]\n"
"    -p module   the name of the consoldated module that this is a component of\n"
"    -r          generate code with tracing enabled [default disabled]\n"
"    -s suffix   the suffix to use for C or C++ source files [default \".c\" or \".cpp\"]\n"
"    -t tag      the version/platform to generate code for\n"
"    -w          enable warning messages\n"
"    -x feature  this feature is disabled\n"
"    -z file     the name of a file containing more command line flags\n"
"    file        the name of the specification file [default stdin]\n"
        , sipPackage, sipPackage);

    exit(0);
}


/*
 * Display the usage message.
 */
static void usage(void)
{
    fatal("Usage: %s [-h] [-V] [-a file] [-c dir] [-d file] [-e] [-g] [-I dir] [-j #] [-m file] [-p module] [-r] [-s suffix] [-t tag] [-w] [-x feature] [-z file] [file]\n", sipPackage);
}
