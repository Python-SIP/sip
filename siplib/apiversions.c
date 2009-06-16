/*
 * The implementation of the supprt for setting API versions.
 *
 * @BS_LICENSE@
 */


#include <Python.h>

#include <string.h>

#include "sip.h"
#include "sipint.h"


/*
 * The structure that defines the version number of an API.
 */
typedef struct _apiVersionDef {
    /* The name of the API. */
    const char *api_name;

    /*
     * The version number of the API.  This will either be set explicitly via
     * a call to sip.setapi() or implicitly by an imported module.
     */
    int version_nr;

    /* The next in the list of APIs. */
    struct _apiVersionDef *next;
} apiVersionDef;


/*
 * The list of API versions.
 */
static apiVersionDef *api_versions = NULL;


/*
 * Forward declarations.
 */
static int add_api(const char *api, int version_nr);
static apiVersionDef *find_api(const char *api);


/*
 * See if a range of versions of a particular API is enabled.
 */
int sip_api_is_api_enabled(const char *name, int from, int to)
{
    const apiVersionDef *avd;

    if ((avd = find_api(name)) == NULL)
        return FALSE;

    if (from > 0 && avd->version_nr < from)
        return FALSE;

    if (to > 0 && avd->version_nr >= to)
        return FALSE;

    return TRUE;
}


/*
 * Get the version number for an API.
 */
PyObject *sipGetAPI(PyObject *self, PyObject *args)
{
    const char *api;
    const apiVersionDef *avd;

    if (!PyArg_ParseTuple(args, "s:getapi", &api))
        return NULL;

    if ((avd = find_api(api)) == NULL)
    {
        PyErr_Format(PyExc_ValueError, "unknown API '%s'", api);
        return NULL;
    }

#if PY_MAJOR_VERSION >= 3
    return PyLong_FromLong(avd->version_nr);
#else
    return PyInt_FromLong(avd->version_nr);
#endif
}


/*
 * Set the version number for an API.
 */
PyObject *sipSetAPI(PyObject *self, PyObject *args)
{
    const char *api;
    int version_nr;
    const apiVersionDef *avd;

    if (!PyArg_ParseTuple(args, "si:setapi", &api, &version_nr))
        return NULL;

    if (version_nr < 1)
    {
        PyErr_Format(PyExc_ValueError,
                "API version numbers must be greater or equal to 1, not %d",
                version_nr);
        return NULL;
    }

    if ((avd = find_api(api)) == NULL)
    {
        const char *api_copy;

        /* Make a deep copy of the name. */
        if ((api_copy = sip_api_malloc(strlen(api))) == NULL)
            return NULL;

        strcpy(api_copy, api);

        if (add_api(api_copy, version_nr) < 0)
            return NULL;
    }
    else if (avd->version_nr != version_nr)
    {
        PyErr_Format(PyExc_ValueError,
                "API '%s' has already been set to version %d", api,
                avd->version_nr);
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}


/*
 * Add a new API to the global list returning a negative value on error.
 */
static int add_api(const char *api, int version_nr)
{
    apiVersionDef *avd;

    if ((avd = sip_api_malloc(sizeof (apiVersionDef))) == NULL)
        return -1;

    avd->api_name = api;
    avd->version_nr = version_nr;
    avd->next = api_versions;

    api_versions = avd;

    return 0;
}


/*
 * Return the definition for the given API, or NULL if there was none.
 */
static apiVersionDef *find_api(const char *api)
{
    apiVersionDef *avd;

    for (avd = api_versions; avd != NULL; avd = avd->next)
        if (strcmp(avd->api_name, api) == 0)
            break;

    return avd;
}
