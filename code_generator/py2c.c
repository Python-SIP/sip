/*
 * The transitional conversion from the output of the Python-based parser to
 * that required by the rest of the C-based code generator.
 *
 * Copyright (c) 2022 Riverbank Computing Limited <info@riverbankcomputing.com>
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


#if !defined(Py_LIMITED_API)
#define Py_LIMITED_API      0x03070000
#endif

#include <Python.h>

#undef NDEBUG
#include <assert.h>

#include <limits.h>
#include <string.h>

#include "sip.h"


/* The values of the ArrayArgument enum members. */
#define ARRAY_ARRAY             0
#define ARRAY_SIZE              1
#define ARRAY_NONE              2

/* The values of the ClassKey enum members. */
#define CLASS_KEY_CLASS         0
#define CLASS_KEY_STRUCT        1
#define CLASS_KEY_UNION         2

/* The values of the EnumBaseType enum members. */
#define EBT_ENUM                0
#define EBT_FLAG                1
#define EBT_INT_ENUM            2
#define EBT_INT_FLAG            3
#define EBT_UNSIGNED_INT_ENUM   4

/* The values of the GILAction enum members. */
#define GIL_ACTION_DEFAULT      0
#define GIL_ACTION_HOLD         1
#define GIL_ACTION_RELEASE      2

/* The values of the Transfer enum members. */
#define TRANSFER_NONE           0
#define TRANSFER_TRANSFER       1
#define TRANSFER_BACK           2
#define TRANSFER_THIS           3


/* Support for caches. */
typedef struct _objectCache {
    PyObject *py_obj;           /* The original Python object. */
    void *c_struct;             /* The converted C structure. */
    struct _objectCache *next;  /* The next in the list. */
} objectCache;

typedef struct _strCache {
    char *py_str;               /* The original Python string. */
    void *c_struct;             /* The converted C structure. */
    struct _strCache *next;     /* The next in the list. */
} strCache;

static void clear_caches(void);
static void cache_object(objectCache **head, PyObject *py_obj, void *c_struct);
static void clear_object_cache(objectCache **head);
static void *search_object_cache(const objectCache *cache, PyObject *py_obj);
static void cache_str(strCache **head, char *py_str, void *c_struct);
static void clear_str_cache(strCache **head);
static void *search_str_cache(const strCache *cache, char *py_str);


/*
 * The caches.
 */
static objectCache *cache_cachedname = NULL;
static objectCache *cache_class = NULL;
static objectCache *cache_codeblock = NULL;
static objectCache *cache_constructor = NULL;
static objectCache *cache_exception = NULL;
static objectCache *cache_ifacefile = NULL;
static objectCache *cache_mappedtype = NULL;
static objectCache *cache_member = NULL;
static objectCache *cache_module = NULL;
static objectCache *cache_qual = NULL;
static objectCache *cache_virtualerrorhandler = NULL;
static objectCache *cache_wrappedenum = NULL;
static objectCache *cache_wrappedtypedef = NULL;
static strCache *cache_typehint = NULL;


/*
 * Clear all the caches.
 */
static void clear_caches(void)
{
    clear_object_cache(&cache_cachedname);
    clear_object_cache(&cache_class);
    clear_object_cache(&cache_codeblock);
    clear_object_cache(&cache_constructor);
    clear_object_cache(&cache_exception);
    clear_object_cache(&cache_ifacefile);
    clear_object_cache(&cache_mappedtype);
    clear_object_cache(&cache_member);
    clear_object_cache(&cache_module);
    clear_object_cache(&cache_qual);
    clear_object_cache(&cache_virtualerrorhandler);
    clear_object_cache(&cache_wrappedenum);
    clear_object_cache(&cache_wrappedtypedef);
    clear_str_cache(&cache_typehint);
}


/*
 * Clear an object cache.
 */
static void clear_object_cache(objectCache **head)
{
    objectCache *entry = *head;

    while (entry != NULL)
    {
        objectCache *next = entry->next;

        free(entry->c_struct);
        free(entry);

        entry = next;
    }

    *head = NULL;
}


/*
 * Add a Python object/C structure pair to an object cache.
 */
static void cache_object(objectCache **head, PyObject *py_obj, void *c_struct)
{
    objectCache *entry = sipMalloc(sizeof (objectCache));

    entry->py_obj = py_obj;
    entry->c_struct = c_struct;
    entry->next = *head;

    *head = entry;
}


/*
 * Search an object cache for a Python object and return the already converted
 * C structure.
 */
static void *search_object_cache(const objectCache *cache, PyObject *py_obj)
{
    while (cache != NULL)
    {
        if (cache->py_obj == py_obj)
            return cache->c_struct;

        cache = cache->next;
    }

    return NULL;
}


/*
 * Clear a str cache.
 */
static void clear_str_cache(strCache **head)
{
    strCache *entry = *head;

    while (entry != NULL)
    {
        strCache *next = entry->next;

        free(entry->c_struct);
        free(entry);

        entry = next;
    }

    *head = NULL;
}


/*
 * Add a Python str/C structure pair to a str cache.
 */
static void cache_str(strCache **head, char *py_str, void *c_struct)
{
    strCache *entry = sipMalloc(sizeof (strCache));

    entry->py_str = py_str;
    entry->c_struct = c_struct;
    entry->next = *head;

    *head = entry;
}


/*
 * Search a str cache for a Python string and return the already converted
 * C structure.
 */
static void *search_str_cache(const strCache *cache, char *py_str)
{
    while (cache != NULL)
    {
        if (strcmp(cache->py_str, py_str) == 0)
            return cache->c_struct;

        cache = cache->next;
    }

    return NULL;
}


/* Forward declarations of convertors. */
static void argument(sipSpec *pt, PyObject *obj, const char *encoding,
        argDef *value);
static void argument_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding, argDef *value);
static argList *argument_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static int bool_attr(PyObject *obj, const char *name);
static nameDef *cachedname(PyObject *obj, const char *encoding);
static nameDef *cachedname_attr(PyObject *obj, const char *name,
        const char *encoding);
static nameDef *cachedname_list_attr(PyObject *obj, const char *name,
        const char *encoding);
static classDef *class(sipSpec *pt, PyObject *obj, const char *encoding);
static classDef *class_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static classDef *class_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static classList *classlist_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static codeBlock *codeblock(PyObject *obj, const char *encoding);
static codeBlockList *codeblock_list_attr(PyObject *obj, const char *name,
        const char *encoding);
static ctorDef *constructor(sipSpec *pt, PyObject *obj, const char *encoding);
static ctorDef *constructor_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static ctorDef *constructor_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static docstringDef *docstring(PyObject *obj, const char *encoding);
static docstringDef *docstring_attr(PyObject *obj, const char *name,
        const char *encoding);
static int enum_attr(PyObject *obj, const char *name);
static exceptionDef *exception(sipSpec *pt, PyObject *obj,
        const char *encoding);
static exceptionDef *exception_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static exceptionDef *exception_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static valueDef *expr(sipSpec *pt, PyObject *obj, const char *encoding);
static valueDef *expr_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static fcallDef *functioncall(sipSpec *pt, PyObject *obj,
        const char *encoding);
static ifaceFileDef *ifacefile(sipSpec *pt, PyObject *obj,
        const char *encoding);
static ifaceFileDef *ifacefile_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static ifaceFileDef *ifacefile_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static ifaceFileList *ifacefilelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static int int_attr(PyObject *obj, const char *name);
static licenseDef *license_attr(PyObject *obj, const char *name,
        const char *encoding);
static mappedTypeDef *mappedtype(sipSpec *pt, PyObject *obj,
        const char *encoding);
static mappedTypeDef *mappedtype_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static memberDef *member(sipSpec *pt, PyObject *obj, const char *encoding);
static memberDef *member_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static memberDef *member_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static moduleDef *module(sipSpec *pt, PyObject *obj, const char *encoding);
static moduleDef *module_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static moduleDef *module_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static moduleListDef *modulelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static mroDef *mro_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static overDef *over(sipSpec *pt, PyObject *obj, const char *encoding);
static overDef *over_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static overDef *over_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static propertyDef *property(PyObject *obj, const char *encoding);
static propertyDef *property_list_attr(PyObject *obj, const char *name,
        const char *encoding);
static qualDef *qual(sipSpec *pt, PyObject *obj, const char *encoding);
static qualDef *qual_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static scopedNameDef *scopedname(PyObject *obj, const char *encoding);
static scopedNameDef *scopedname_attr(PyObject *obj, const char *name,
        const char *encoding);
static signatureDef *signature(sipSpec *pt, PyObject *obj,
        const char *encoding, signatureDef *value);
static signatureDef *signature_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding, signatureDef *value);
static sourceLocation sourcelocation(PyObject *obj, const char *encoding);
static sourceLocation sourcelocation_attr(PyObject *obj, const char *name,
        const char *encoding);
static char *str(PyObject *obj, const char *encoding);
static char *str_attr(PyObject *obj, const char *name, const char *encoding);
static stringList *str_list_attr(PyObject *obj, const char *name,
        const char *encoding);
static templateDef *template(sipSpec *pt, PyObject *obj, const char *encoding);
static templateDef *template_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static throwArgs *throw_arguments(sipSpec *pt, PyObject *obj,
        const char *encoding);
static throwArgs *throw_arguments_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static typeHintDef *typehint_attr(PyObject *obj, const char *name,
        const char *encoding);
static void typehints_attr(PyObject *obj, const char *name,
        const char *encoding, typeHintDef **th_in, typeHintDef **th_out,
        const char **th_value);
static valueDef *value(sipSpec *pt, PyObject *obj, const char *encoding);
static virtErrorHandler *virtualerrorhandler(sipSpec *pt, PyObject *obj,
        const char *encoding);
static virtErrorHandler *virtualerrorhandler_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static virtErrorHandler *virtualerrorhandler_list_attr(sipSpec *pt,
        PyObject *obj, const char *name, const char *encoding);
static virtHandlerDef *virtualhandler(sipSpec *pt, PyObject *obj,
        const char *encoding);
static virtHandlerDef *virtualhandler_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static virtHandlerDef *virtualhandler_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static virtOverDef *virtualoverload(sipSpec *pt, PyObject *obj,
        const char *encoding);
static virtOverDef *virtualoverloadlist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static visibleList *visible(sipSpec *pt, PyObject *obj, const char *encoding);
static visibleList *visiblelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static enumDef *wrappedenum(sipSpec *pt, PyObject *obj, const char *encoding);
static enumDef *wrappedenum_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding);
static enumDef *wrappedenum_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static enumMemberDef *wrappedenummember(sipSpec *pt, PyObject *obj,
        const char *encoding);
static enumMemberDef *wrappedenummember_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static typedefDef *wrappedtypedef(sipSpec *pt, PyObject *obj,
        const char *encoding);
static typedefDef *wrappedtypedef_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static typedefDef *wrappedtypedef_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);
static varDef *wrappedvariable(sipSpec *pt, PyObject *obj,
        const char *encoding);
static varDef *wrappedvariable_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding);

/* Other forward declarations. */
static void appendCodeBlock(codeBlockList **headp, codeBlock *cb);
static scopedNameDef *text2scopePart(char *text);


/*
 * Convert the Specification Python object to the C structures defined in
 * sip.h.  Note that no error checking is done.
 */
sipSpec *py2c(PyObject *spec, const char *encoding)
{
    sipSpec *pt;

    clear_caches();

    pt = sipMalloc(sizeof (sipSpec));

    pt->module = module_attr(pt, spec, "module", encoding);
    pt->namecache = cachedname_list_attr(spec, "name_cache", encoding);
    pt->ifacefiles = ifacefile_list_attr(pt, spec, "iface_files", encoding);
    pt->classes = class_list_attr(pt, spec, "classes", encoding);
    pt->exceptions = exception_list_attr(pt, spec, "exceptions", encoding);
    pt->mappedtypes = mappedtype_list_attr(pt, spec, "mapped_types", encoding);
    pt->enums = wrappedenum_list_attr(pt, spec, "enums", encoding);
    pt->vars = wrappedvariable_list_attr(pt, spec, "variables", encoding);
    pt->typedefs = wrappedtypedef_list_attr(pt, spec, "typedefs", encoding);
    pt->virthandlers = virtualhandler_list_attr(pt, spec, "virtual_handlers",
            encoding);
    pt->errorhandlers = virtualerrorhandler_list_attr(pt, spec,
            "virtual_error_handlers", encoding);
    pt->exphdrcode = codeblock_list_attr(spec, "exported_header_code",
            encoding);
    pt->exptypehintcode = codeblock_list_attr(spec, "exported_type_hint_code",
            encoding);
    pt->genc = bool_attr(spec, "c_bindings");
    pt->plugins = str_list_attr(spec, "plugins", encoding);
    pt->nrvirthandlers = int_attr(spec, "nr_virtual_handlers");
    pt->qobject_cd = class_attr(pt, spec, "pyqt_qobject", encoding);

    return pt;
}


/*
 * Convert an optional Argument object.
 */
static void argument(sipSpec *pt, PyObject *obj, const char *encoding,
        argDef *value)
{
    PyObject *derefs_obj, *definition;
    int key, xfer, array;
    Py_ssize_t i;

    if (obj == Py_None)
    {
        value->atype = no_type;
        return;
    }

    value->atype = (argType)enum_attr(obj, "type");
    value->name = cachedname_attr(obj, "name", encoding);
    typehints_attr(obj, "type_hints", encoding, &value->typehint_in,
            &value->typehint_out, &value->typehint_value);

    if (bool_attr(obj, "is_reference"))
        setIsReference(value);

    if (bool_attr(obj, "is_const"))
        setIsConstArg(value);

    xfer = enum_attr(obj, "transfer");

    if (xfer == TRANSFER_TRANSFER)
        setIsTransferred(value);

    if (xfer == TRANSFER_THIS)
        setIsThisTransferred(value);

    if (xfer == TRANSFER_BACK)
        setIsTransferredBack(value);

    array = enum_attr(obj, "array");

    if (array == ARRAY_ARRAY)
        setArray(value);

    if (array == ARRAY_SIZE)
        setArraySize(value);

    if (bool_attr(obj, "allow_none"))
        setAllowNone(value);

    if (bool_attr(obj, "get_wrapper"))
        setGetWrapper(value);

    if (bool_attr(obj, "is_in"))
        setIsInArg(value);

    if (bool_attr(obj, "is_out"))
        setIsOutArg(value);

    if (bool_attr(obj, "is_constrained"))
        setIsConstrained(value);

    if (bool_attr(obj, "result_size"))
        setResultSize(value);

    if (bool_attr(obj, "no_copy"))
        setNoCopy(value);

    if (bool_attr(obj, "disallow_none"))
        setDisallowNone(value);

    derefs_obj = PyObject_GetAttrString(obj, "derefs");
    assert(derefs_obj != NULL);

    for (i = 0; i < PyList_Size(derefs_obj) && i < MAX_NR_DEREFS; ++i)
        value->derefs[i] = (PyList_GetItem(derefs_obj, i) == Py_True);

    value->nrderefs = i;

    value->defval = expr_attr(pt, obj, "default_value", encoding);
    value->scopes_stripped = int_attr(obj, "scopes_stripped");

    key = int_attr(obj, "key");

    if (key != INT_MIN)
    {
        setKeepReference(value);
        value->key = key;
    }

    value->original_type = wrappedtypedef_attr(pt, obj, "original_typedef",
            encoding);

    definition = PyObject_GetAttrString(obj, "definition");
    assert(definition != NULL);

    switch (value->atype)
    {
    case function_type:
        value->u.sa = signature(pt, definition, encoding, NULL);
        break;

    case template_type:
        value->u.td = template(pt, definition, encoding);
        break;

    case defined_type:
        value->u.snd = scopedname(definition, encoding);
        break;

    case class_type:
        value->u.cd = class(pt, definition, encoding);
        break;

    case mapped_type:
        value->u.mtd = mappedtype(pt, definition, encoding);
        break;

    case enum_type:
        value->u.ed = wrappedenum(pt, definition, encoding);
        break;

    case struct_type:
    case union_type:
        value->u.sname = scopedname(definition, encoding);
        break;

    case capsule_type:
        value->u.cap = scopedname(definition, encoding);
        break;

    default:
        break;
    }

    Py_DECREF(definition);
}


/*
 * Convert an optional Argument attribute.
 */
static void argument_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding, argDef *value)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);

    assert(attr != NULL);

    argument(pt, attr, encoding, value);

    Py_DECREF(attr);
}


/*
 * Convert an Argument list attribute to a C argList.
 */
static argList *argument_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    argList *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        argList *item = sipMalloc(sizeof (argList));

        argument(pt, PyList_GetItem(attr, i), encoding, &item->arg);

        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a bool attribute.
 */
static int bool_attr(PyObject *obj, const char *name)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    int value;

    assert(attr != NULL);

    value = (attr == Py_True);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional CachedName object.
 */
static nameDef *cachedname(PyObject *obj, const char *encoding)
{
    nameDef *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_cachedname, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (nameDef));

    cache_object(&cache_cachedname, obj, value);

    value->text = str_attr(obj, "name", encoding);
    value->len = strlen(value->text);
    value->offset = int_attr(obj, "offset");

    if (bool_attr(obj, "is_substring"))
        setIsSubstring(value);

    if (bool_attr(obj, "used"))
        setIsUsedName(value);

    return value;
}


/*
 * Convert an optional CachedName attribute.
 */
static nameDef *cachedname_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    nameDef *value;

    assert(attr != NULL);

    value = cachedname(attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a CachedName list attribute.
 */
static nameDef *cachedname_list_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    nameDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);
    assert(PyList_Check(attr));

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        nameDef *item = cachedname(PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional Class object.
 */
static classDef *class(sipSpec *pt, PyObject *obj, const char *encoding)
{
    classDef *value;
    int gil_action, class_key, dtor_access;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_class, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (classDef));

    cache_object(&cache_class, obj, value);

    value->iff = ifacefile_attr(pt, obj, "iface_file", encoding);
    value->docstring = docstring_attr(obj, "docstring", encoding);

    if (bool_attr(obj, "is_abstract"))
        setIsAbstractClass(value);

    if (bool_attr(obj, "is_opaque"))
        setIsOpaque(value);

    if (bool_attr(obj, "is_protected"))
        setIsProtectedClass(value);

    if (bool_attr(obj, "is_incomplete"))
        setIsIncomplete(value);

    if (bool_attr(obj, "can_create"))
        setCanCreate(value);

    if (bool_attr(obj, "external"))
        setIsExternal(value);

    if (bool_attr(obj, "delay_dtor"))
        setIsDelayedDtor(value);

    if (bool_attr(obj, "no_default_ctors"))
        setNoDefaultCtors(value);

    if (bool_attr(obj, "pyqt_no_qmetaobject"))
        setPyQtNoQMetaObject(value);

    if (bool_attr(obj, "deprecated"))
        setIsDeprecatedClass(value);

    if (bool_attr(obj, "cannot_assign"))
        setCannotAssign(value);

    if (bool_attr(obj, "cannot_copy"))
        setCannotCopy(value);

    if (bool_attr(obj, "handles_none"))
        setClassHandlesNone(value);

    if (bool_attr(obj, "has_nonlazy_method"))
        setHasNonlazyMethod(value);

    if (bool_attr(obj, "has_variable_handlers"))
        setHasVarHandlers(value);

    gil_action = enum_attr(obj, "dtor_gil_action");
    if (gil_action == GIL_ACTION_HOLD)
        setIsHoldGILDtor(value);
    else if (gil_action == GIL_ACTION_RELEASE)
        setIsReleaseGILDtor(value);

    if ((dtor_access = enum_attr(obj, "dtor")) != -1)
        value->classflags |= enum_attr(obj, "dtor");

    if (bool_attr(obj, "is_qobject"))
        setIsQObjectSubClass(value);

    if (bool_attr(obj, "mixin"))
        setMixin(value);

    if (bool_attr(obj, "needs_array_helper"))
        setArrayHelper(value);

    if (bool_attr(obj, "needs_copy_helper"))
        setCopyHelper(value);

    if (bool_attr(obj, "export_derived"))
        setExportDerived(value);

    if (bool_attr(obj, "is_hidden_namespace"))
        setHiddenNamespace(value);

    if (bool_attr(obj, "no_type_name"))
        setUseTemplateName(value);

    if (bool_attr(obj, "has_shadow"))
        setHasShadow(value);

    class_key = enum_attr(obj, "class_key");
    if (class_key == CLASS_KEY_UNION)
        setUnion(value);

    value->pyqt_flags = int_attr(obj, "pyqt_flags");
    value->pyqt_flags_enums = str_list_attr(obj, "pyqt_flags_enums", encoding);
    value->pyqt_interface = str_attr(obj, "pyqt_interface", encoding);
    value->pyname = cachedname_attr(obj, "py_name", encoding);
    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->ecd = class_attr(pt, obj, "scope", encoding);
    value->real = class_attr(pt, obj, "real_class", encoding);
    value->subbase = class_attr(pt, obj, "subclass_base", encoding);
    value->mro = mro_list_attr(pt, obj, "mro", encoding);
    value->supers = classlist_attr(pt, obj, "superclasses", encoding);
    value->metatype = cachedname_attr(obj, "metatype", encoding);
    value->supertype = cachedname_attr(obj, "supertype", encoding);
    value->td = template_attr(pt, obj, "template", encoding);
    value->ctors = constructor_list_attr(pt, obj, "ctors", encoding);
    value->defctor = constructor_attr(pt, obj, "default_ctor", encoding);
    value->dealloccode = codeblock_list_attr(obj, "dealloc_code", encoding);
    value->dtorcode = codeblock_list_attr(obj, "dtor_virtual_catcher_code",
            encoding);
    value->dtorexceptions = throw_arguments_attr(pt, obj, "dtor_throw_args",
            encoding);
    value->members = member_list_attr(pt, obj, "members", encoding);
    value->overs = over_list_attr(pt, obj, "overloads", encoding);
    value->vmembers = virtualoverloadlist_attr(pt, obj, "virtual_overloads",
            encoding);
    value->visible = visiblelist_attr(pt, obj, "visible_members", encoding);
    value->casts = argument_list_attr(pt, obj, "casts", encoding);
    value->cppcode = codeblock_list_attr(obj, "type_code", encoding);
    value->convtosubcode = codeblock_list_attr(obj, "convert_to_subclass_code",
            encoding);
    value->instancecode = codeblock_list_attr(obj, "instance_code", encoding);
    value->convtocode = codeblock_list_attr(obj, "convert_to_type_code",
            encoding);
    value->convfromcode = codeblock_list_attr(obj, "convert_from_type_code",
            encoding);
    value->travcode = codeblock_list_attr(obj, "gc_traverse_code", encoding);
    value->clearcode = codeblock_list_attr(obj, "gc_clear_code", encoding);
    value->getbufcode = codeblock_list_attr(obj, "bi_get_buffer_code",
            encoding);
    value->releasebufcode = codeblock_list_attr(obj, "bi_release_buffer_code",
            encoding);
    value->picklecode = codeblock_list_attr(obj, "pickle_code", encoding);
    value->finalcode = codeblock_list_attr(obj, "finalisation_code", encoding);
    value->typehintcode = codeblock_list_attr(obj, "type_hint_code", encoding);
    value->properties = property_list_attr(obj, "properties", encoding);
    value->virt_error_handler = str_attr(obj, "virtual_error_handler",
            encoding);
    typehints_attr(obj, "type_hints", encoding, &value->typehint_in,
            &value->typehint_out, &value->typehint_value);

    return value;
}


/*
 * Convert an optional Class attribute.
 */
static classDef *class_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    classDef *value;

    assert(attr != NULL);

    value = class(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a Class list attribute.
 */
static classDef *class_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    classDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        classDef *item = class(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Class list attribute to a classList.
 */
static classList *classlist_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    classList *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        classList *item = sipMalloc(sizeof (classList));

        item->cd = class(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a CodeBlock object.
 */
static codeBlock *codeblock(PyObject *obj, const char *encoding)
{
    codeBlock *value;

    if ((value = search_object_cache(cache_codeblock, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (codeBlock));

    cache_object(&cache_codeblock, obj, value);

    value->frag = str_attr(obj, "text", encoding);
    value->filename = str_attr(obj, "sip_file", encoding);
    value->linenr = int_attr(obj, "line_nr");

    return value;
}


/*
 * Convert a CodeBlock list or an optional CodeBlock attribute.
 */
static codeBlockList *codeblock_list_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    codeBlockList *head;

    assert(attr != NULL);

    head = NULL;

    if (PyList_Check(attr))
    {
        Py_ssize_t i;

        for (i = 0; i < PyList_Size(attr); ++i)
            appendCodeBlock(&head,
                    codeblock(PyList_GetItem(attr, i), encoding));
    }
    else if (attr != Py_None)
    {
        appendCodeBlock(&head, codeblock(attr, encoding));
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional Constructor object.
 */
static ctorDef *constructor(sipSpec *pt, PyObject *obj, const char *encoding)
{
    ctorDef *value;
    int gil_action;
    PyObject *py_sig_obj, *cpp_sig_obj;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_constructor, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (ctorDef));

    cache_object(&cache_constructor, obj, value);

    value->docstring = docstring_attr(obj, "docstring", encoding);

    value->ctorflags |= enum_attr(obj, "access_specifier");

    if (bool_attr(obj, "is_cast"))
        value->ctorflags |= CTOR_CAST;

    gil_action = enum_attr(obj, "gil_action");
    if (gil_action == GIL_ACTION_HOLD)
        setIsHoldGILCtor(value);
    else if (gil_action == GIL_ACTION_RELEASE)
        setIsReleaseGILCtor(value);

    if (enum_attr(obj, "transfer") == TRANSFER_TRANSFER)
        setIsResultTransferredCtor(value);

    if (bool_attr(obj, "deprecated"))
        setIsDeprecatedCtor(value);

    if (bool_attr(obj, "raises_py_exception"))
        setRaisesPyExceptionCtor(value);

    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->kwargs = (KwArgs)enum_attr(obj, "kw_args");

    py_sig_obj = PyObject_GetAttrString(obj, "py_signature");
    assert(py_sig_obj != NULL);

    signature(pt, py_sig_obj, encoding, &value->pysig);

    cpp_sig_obj = PyObject_GetAttrString(obj, "cpp_signature");
    assert(cpp_sig_obj != NULL);

    if (cpp_sig_obj != Py_None)
    {
        if (cpp_sig_obj == py_sig_obj)
            value->cppsig = &value->pysig;
        else
            value->cppsig = signature(pt, cpp_sig_obj, encoding, NULL);
    }

    Py_DECREF(py_sig_obj);
    Py_DECREF(cpp_sig_obj);

    value->exceptions = throw_arguments_attr(pt, obj, "throw_args", encoding);
    value->methodcode = codeblock_list_attr(obj, "method_code", encoding);
    value->premethodcode = codeblock_list_attr(obj, "premethod_code",
            encoding);
    value->prehook = str_attr(obj, "prehook", encoding);
    value->posthook = str_attr(obj, "posthook", encoding);

    return value;
}


/*
 * Convert an optional Constructor attribute.
 */
static ctorDef *constructor_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    ctorDef *value;

    assert(attr != NULL);

    value = constructor(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a Constructor list attribute.
 */
static ctorDef *constructor_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    ctorDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        ctorDef *item = constructor(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Docstring object.
 */
static docstringDef *docstring(PyObject *obj, const char *encoding)
{
    docstringDef *value = sipMalloc(sizeof (docstringDef));

    value->signature = (Signature)enum_attr(obj, "signature");
    value->text = str_attr(obj, "text", encoding);

    return value;
}


/*
 * Convert an optional Docstring attribute.
 */
static docstringDef *docstring_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    docstringDef *value;

    assert(attr != NULL);

    if (attr == Py_None)
        value = NULL;
    else
        value = docstring(attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional enum attribute.
 */
static int enum_attr(PyObject *obj, const char *name)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    int value;

    assert(attr != NULL);

    if (attr == Py_None)
    {
        value = -1;
    }
    else
    {
        PyObject *value_obj = PyObject_GetAttrString(attr, "value");

        value = PyLong_AsLong(value_obj);

        Py_DECREF(value_obj);
    }

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional WrappedException object.
 */
static exceptionDef *exception(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    exceptionDef *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_exception, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (exceptionDef));

    cache_object(&cache_exception, obj, value);

    value->exceptionnr = int_attr(obj, "exception_nr");
    value->iff = ifacefile_attr(pt, obj, "iface_file", encoding);
    value->pyname = str_attr(obj, "py_name", encoding);
    value->cd = class_attr(pt, obj, "class_exception", encoding);
    value->bibase = str_attr(obj, "builtin_base_exception", encoding);
    value->base = exception_attr(pt, obj, "defined_base_exception", encoding);
    value->raisecode = codeblock_list_attr(obj, "raise_code", encoding);
    value->needed = bool_attr(obj, "needed");

    return value;
}


/*
 * Convert an optional WrappedException attribute.
 */
static exceptionDef *exception_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    exceptionDef *value;

    assert(attr != NULL);

    value = exception(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a WrappedException list attribute.
 */
static exceptionDef *exception_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    exceptionDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        exceptionDef *item = exception(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a FunctionCall object.
 */
static fcallDef *functioncall(sipSpec *pt, PyObject *obj, const char *encoding)
{
    fcallDef *value = sipMalloc(sizeof (fcallDef));
    PyObject *args_obj;
    Py_ssize_t i;

    argument_attr(pt, obj, "result", encoding, &value->type);

    args_obj = PyObject_GetAttrString(obj, "args");
    assert(args_obj != NULL);
    assert(PyList_Check(args_obj));

    for (i = 0; i < PyList_Size(args_obj) && i < MAX_NR_ARGS; ++i)
        value->args[i] = expr(pt, PyList_GetItem(args_obj, i), encoding);

    value->nrArgs = i;

    Py_DECREF(args_obj);

    return value;
}


/*
 * Convert an optional IfaceFile object.
 */
static ifaceFileDef *ifacefile(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    ifaceFileDef *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_ifacefile, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (ifaceFileDef));

    cache_object(&cache_ifacefile, obj, value);

    value->name = cachedname_attr(obj, "cpp_name", encoding);
    value->needed = bool_attr(obj, "needed");
    value->type = (ifaceFileType)enum_attr(obj, "type");
    value->ifacenr = int_attr(obj, "type_nr");
    value->fqcname = scopedname_attr(obj, "fq_cpp_name", encoding);
    value->module = module_attr(pt, obj, "module", encoding);
    value->hdrcode = codeblock_list_attr(obj, "type_header_code", encoding);
    value->file_extension = str_attr(obj, "file_extension", encoding);
    value->used = ifacefilelist_attr(pt, obj, "used", encoding);

    return value;
}


/*
 * Convert an optional IfaceFile attribute.
 */
static ifaceFileDef *ifacefile_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    ifaceFileDef *value;

    assert(attr != NULL);

    value = ifacefile(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an IfaceFile list attribute.
 */
static ifaceFileDef *ifacefile_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    ifaceFileDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        ifaceFileDef *item = ifacefile(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an IfaceFile list attribute to an ifaceFileList.
 */
static ifaceFileList *ifacefilelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    ifaceFileList *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        ifaceFileList *item = sipMalloc(sizeof (ifaceFileList));

        item->iff = ifacefile(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional int attribute.
 */
static int int_attr(PyObject *obj, const char *name)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    int value;

    assert(attr != NULL);

    if (attr == Py_None)
        value = INT_MIN;
    else
        value = PyLong_AsLong(attr);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional License attribute.
 */
static licenseDef *license_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    licenseDef *value;

    assert(attr != NULL);

    if (attr == Py_None)
        return NULL;

    value = sipMalloc(sizeof (licenseDef));

    value->type = str_attr(attr, "type", encoding);
    value->licensee = str_attr(attr, "licensee", encoding);
    value->timestamp = str_attr(attr, "timestamp", encoding);
    value->sig = str_attr(attr, "signature", encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional MappedType object.
 */
static mappedTypeDef *mappedtype(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    mappedTypeDef *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_mappedtype, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (mappedTypeDef));

    cache_object(&cache_mappedtype, obj, value);

    if (bool_attr(obj, "no_assignment_operator"))
        setNoAssignOp(value);

    if (bool_attr(obj, "no_copy_ctor"))
        setNoCopyCtor(value);

    if (bool_attr(obj, "no_default_ctor"))
        setNoDefaultCtor(value);

    if (bool_attr(obj, "no_release"))
        setNoRelease(value);

    if (bool_attr(obj, "handles_none"))
        setHandlesNone(value);

    if (bool_attr(obj, "needs_user_state"))
        setNeedsUserState(value);

    argument_attr(pt, obj, "type", encoding, &value->type);
    value->pyname = cachedname_attr(obj, "py_name", encoding);
    value->cname = cachedname_attr(obj, "cpp_name", encoding);
    typehints_attr(obj, "type_hints", encoding, &value->typehint_in,
            &value->typehint_out, &value->typehint_value);
    value->pyqt_flags = int_attr(obj, "pyqt_flags");
    value->iff = ifacefile_attr(pt, obj, "iface_file", encoding);
    value->members = member_list_attr(pt, obj, "members", encoding);
    value->overs = over_list_attr(pt, obj, "overloads", encoding);
    value->instancecode = codeblock_list_attr(obj, "instance_code", encoding);
    value->typecode = codeblock_list_attr(obj, "type_code", encoding);
    value->convfromcode = codeblock_list_attr(obj, "convert_from_type_code",
            encoding);
    value->convtocode = codeblock_list_attr(obj, "convert_to_type_code",
            encoding);
    value->releasecode = codeblock_list_attr(obj, "release_code", encoding);

    value->real = value;

    return value;
}


/*
 * Convert a MappedType list attribute.
 */
static mappedTypeDef *mappedtype_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    mappedTypeDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        mappedTypeDef *item = mappedtype(pt, PyList_GetItem(attr, i),
                encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Member object.
 */
static memberDef *member(sipSpec *pt, PyObject *obj, const char *encoding)
{
    memberDef *value;
    int slot;

    if ((value = search_object_cache(cache_member, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (memberDef));

    cache_object(&cache_member, obj, value);

    value->pyname = cachedname_attr(obj, "py_name", encoding);

    if (bool_attr(obj, "is_numeric"))
        setIsNumeric(value);

    if (bool_attr(obj, "is_numeric"))
        setIsSequence(value);

    if (bool_attr(obj, "no_arg_parser"))
        setNoArgParser(value);

    if (bool_attr(obj, "allow_keyword_args"))
        setUseKeywordArgs(value);

    if (bool_attr(obj, "has_protected"))
        setHasProtected(value);

    slot = enum_attr(obj, "py_slot");

    if (slot < 0)
        value->slot = no_slot;
    else
        value->slot = (slotType)slot;

    value->module = module_attr(pt, obj, "module", encoding);
    value->ns_scope = ifacefile_attr(pt, obj, "namespace_iface_file",
            encoding);

    return value;
}


/*
 * Convert a Member attribute.
 */
static memberDef *member_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    memberDef *value;

    assert(attr != NULL);

    value = member(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a Member list attribute.
 */
static memberDef *member_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    memberDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        memberDef *item = member(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional Module object.
 */
static moduleDef *module(sipSpec *pt, PyObject *obj, const char *encoding)
{
    moduleDef *value;
    PyObject *needed_types_obj;
    Py_ssize_t nr_needed_types;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_module, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (moduleDef));

    cache_object(&cache_module, obj, value);

    value->fullname = cachedname_attr(obj, "fq_py_name", encoding);
    value->name = str_attr(obj, "py_name", encoding);
    value->docstring = docstring_attr(obj, "docstring", encoding);

    if (bool_attr(obj, "call_super_init"))
        setCallSuperInitYes(value);
    else
        setCallSuperInitNo(value);

    if (bool_attr(obj, "has_delayed_dtors"))
        setHasDelayedDtors(value);

    if (bool_attr(obj, "is_composite"))
    {
        setIsComposite(value);
        value->modflags &= ~MOD_SUPER_INIT_MASK;
    }

    if (bool_attr(obj, "use_arg_names"))
        setUseArgNames(value);

    if (bool_attr(obj, "use_limited_api"))
        setUseLimitedAPI(value);

    if (bool_attr(obj, "py_ssize_t_clean"))
        setPY_SSIZE_T_CLEAN(value);

    value->othfuncs = member_list_attr(pt, obj, "global_functions", encoding);
    value->overs = over_list_attr(pt, obj, "overloads", encoding);
    value->defdocstringfmt = (Format)enum_attr(obj,
            "default_docstring_format");
    value->defdocstringsig = (Signature)enum_attr(obj,
            "default_docstring_signature");
    value->defexception = exception_attr(pt, obj, "default_exception",
            encoding);
    value->hdrcode = codeblock_list_attr(obj, "module_header_code", encoding);
    value->cppcode = codeblock_list_attr(obj, "module_code", encoding);
    value->copying = codeblock_list_attr(obj, "copying", encoding);
    value->preinitcode = codeblock_list_attr(obj, "preinitialisation_code",
            encoding);
    value->initcode = codeblock_list_attr(obj, "initialisation_code",
            encoding);
    value->postinitcode = codeblock_list_attr(obj, "postinitialisation_code",
            encoding);
    value->unitcode = codeblock_list_attr(obj, "unit_code", encoding);
    value->unitpostinccode = codeblock_list_attr(obj, "unit_postinclude_code",
            encoding);
    value->typehintcode = codeblock_list_attr(obj, "type_hint_code", encoding);
    value->virt_error_handler = str_attr(obj, "default_virtual_error_handler",
            encoding);
    value->qualifiers = qual_list_attr(pt, obj, "qualifiers", encoding);
    value->nrexceptions = int_attr(obj, "nr_exceptions");
    value->nrtypedefs = int_attr(obj, "nr_typedefs");
    value->nrvirterrorhandlers = int_attr(obj, "nr_virtual_error_handlers");
    value->next_key = int_attr(obj, "next_key");
    value->license = license_attr(obj, "license", encoding);
    value->proxies = class_list_attr(pt, obj, "proxies", encoding);
    value->container = module_attr(pt, obj, "composite", encoding);
    value->used = ifacefilelist_attr(pt, obj, "used", encoding);
    value->imports = modulelist_attr(pt, obj, "imports", encoding);
    value->allimports = modulelist_attr(pt, obj, "all_imports", encoding);

    needed_types_obj = PyObject_GetAttrString(obj, "needed_types");
    assert(needed_types_obj != NULL);

    if ((nr_needed_types = PyList_Size(needed_types_obj)) > 0)
    {
        Py_ssize_t i;

        value->needed_types = sipCalloc(nr_needed_types, sizeof (argDef));

        for (i = 0; i < nr_needed_types; ++i)
            argument(pt, PyList_GetItem(needed_types_obj, i), encoding,
                    &value->needed_types[i]);

        value->nr_needed_types = nr_needed_types;
    }

    Py_DECREF(needed_types_obj);

    return value;
}


/*
 * Convert an optional Module attribute.
 */
static moduleDef *module_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    moduleDef *value;

    assert(attr != NULL);

    value = module(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a Module list attribute.
 */
static moduleDef *module_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    moduleDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        moduleDef *item = module(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Module list attribute to a moduleListDef.
 */
static moduleListDef *modulelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    moduleListDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        moduleListDef *item = sipMalloc(sizeof (moduleListDef));

        item->module = module(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Class list attribute to a list of mroDef.
 */
static mroDef *mro_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    mroDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        mroDef *item = sipMalloc(sizeof (mroDef));

        item->cd = class(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an Overload object.
 */
static overDef *over(sipSpec *pt, PyObject *obj, const char *encoding)
{
    overDef *value = sipMalloc(sizeof (overDef));
    PyObject *py_sig_obj, *cpp_sig_obj;
    int access, xfer, gil_action;

    value->sloc = sourcelocation_attr(obj, "source_location", encoding);
    value->cppname = str_attr(obj, "cpp_name", encoding);
    value->docstring = docstring_attr(obj, "docstring", encoding);

    if ((access = enum_attr(obj, "access_specifier")) >= 0)
        value->overflags |= access;

    if ((access = enum_attr(obj, "pyqt_method_specifier")) >= 0)
        value->overflags |= access;

    if (bool_attr(obj, "is_virtual"))
        setIsVirtual(value);

    if (bool_attr(obj, "is_virtual_reimplementation"))
        setIsVirtualReimp(value);

    if (bool_attr(obj, "is_abstract"))
        setIsAbstract(value);

    if (bool_attr(obj, "is_const"))
        setIsConst(value);

    if (bool_attr(obj, "is_static"))
        setIsStatic(value);

    if (bool_attr(obj, "is_auto_generated"))
        setIsAutoGen(value);

    if (bool_attr(obj, "is_complementary"))
        setIsComplementary(value);

    if (bool_attr(obj, "is_reflected"))
        setIsReflected(value);

    if (bool_attr(obj, "is_global"))
        setIsGlobal(value);

    if (bool_attr(obj, "dont_deref_self"))
        setDontDerefSelf(value);

    if (bool_attr(obj, "new_thread"))
        setIsNewThread(value);

    if (bool_attr(obj, "factory"))
        setIsFactory(value);

    xfer = enum_attr(obj, "transfer");

    if (xfer == TRANSFER_TRANSFER)
        setIsResultTransferred(value);

    if (xfer == TRANSFER_THIS)
        setIsThisTransferredMeth(value);

    if (xfer == TRANSFER_BACK)
        setIsResultTransferredBack(value);

    gil_action = enum_attr(obj, "gil_action");
    if (gil_action == GIL_ACTION_HOLD)
        setIsHoldGIL(value);
    else if (gil_action == GIL_ACTION_RELEASE)
        setIsReleaseGIL(value);

    if (bool_attr(obj, "deprecated"))
        setIsDeprecated(value);

    if (bool_attr(obj, "access_is_really_protected"))
        setIsReallyProtected(value);

    if (bool_attr(obj, "is_delattr"))
        setIsDelattr(value);

    if (bool_attr(obj, "raises_py_exception"))
        setRaisesPyException(value);

    if (bool_attr(obj, "no_virtual_error_handler"))
        setNoErrorHandler(value);

    if (bool_attr(obj, "abort_on_exception"))
        setAbortOnException(value);

    if (bool_attr(obj, "is_final"))
        setIsFinal(value);

    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->kwargs = (KwArgs)enum_attr(obj, "kw_args");
    value->common = member_attr(pt, obj, "common", encoding);

    py_sig_obj = PyObject_GetAttrString(obj, "py_signature");
    assert(py_sig_obj != NULL);

    signature(pt, py_sig_obj, encoding, &value->pysig);

    cpp_sig_obj = PyObject_GetAttrString(obj, "cpp_signature");
    assert(cpp_sig_obj != NULL);

    if (cpp_sig_obj == py_sig_obj)
        value->cppsig = &value->pysig;
    else
        value->cppsig = signature(pt, cpp_sig_obj, encoding, NULL);

    Py_DECREF(py_sig_obj);
    Py_DECREF(cpp_sig_obj);

    value->exceptions = throw_arguments_attr(pt, obj, "throw_args", encoding);
    value->methodcode = codeblock_list_attr(obj, "method_code", encoding);
    value->premethodcode = codeblock_list_attr(obj, "premethod_code",
            encoding);
    value->virtcallcode = codeblock_list_attr(obj, "virtual_call_code",
            encoding);
    value->virtcode = codeblock_list_attr(obj, "virtual_catcher_code",
            encoding);
    value->prehook = str_attr(obj, "prehook", encoding);
    value->posthook = str_attr(obj, "posthook", encoding);
    value->virt_error_handler = str_attr(obj, "virtual_error_handler",
            encoding);

    return value;
}


/*
 * Convert an Overload attribute.
 */
static overDef *over_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    overDef *value;

    assert(attr != NULL);

    value = over(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an Overload list attribute.
 */
static overDef *over_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    overDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        overDef *item = over(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Property object.
 */
static propertyDef *property(PyObject *obj, const char *encoding)
{
    propertyDef *value;

    value = sipMalloc(sizeof (propertyDef));

    value->name = cachedname_attr(obj, "name", encoding);
    value->docstring = docstring_attr(obj, "docstring", encoding);
    value->get = str_attr(obj, "getter", encoding);
    value->set = str_attr(obj, "setter", encoding);

    return value;
}


/*
 * Convert a Property list attribute.
 */
static propertyDef *property_list_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    propertyDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        propertyDef *item = property(PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a Qualifier object.
 */
static qualDef *qual(sipSpec *pt, PyObject *obj, const char *encoding)
{
    qualDef *value;

    if ((value = search_object_cache(cache_qual, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (qualDef));

    cache_object(&cache_qual, obj, value);

    value->name = str_attr(obj, "name", encoding);
    value->qtype = (qualType)enum_attr(obj, "type");
    value->module = module_attr(pt, obj, "module", encoding);
    value->line = int_attr(obj, "timeline");
    value->order = int_attr(obj, "order");
    value->default_enabled = bool_attr(obj, "enabled_by_default");

    return value;
}


/*
 * Convert a Qualifier list attribute.
 */
static qualDef *qual_list_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    qualDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        qualDef *item = qual(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional ScopedName object.
 */
static scopedNameDef *scopedname(PyObject *obj, const char *encoding)
{
    PyObject *name_obj;
    Py_ssize_t i;
    scopedNameDef *value, **tail;

    if (obj == Py_None)
        return NULL;

    name_obj = PyObject_GetAttrString(obj, "_name");
    assert(name_obj != NULL);

    value = NULL;
    tail = &value;

    for (i = 0; i < PyList_Size(name_obj); ++i)
    {
        scopedNameDef *snd = text2scopePart(
                str(PyList_GetItem(name_obj, i), encoding));

        *tail = snd;
        tail = &snd->next;
    }

    Py_DECREF(name_obj);

    return value;
}


/*
 * Convert an optional ScopedName attribute.
 */
static scopedNameDef *scopedname_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    scopedNameDef *value;

    assert(attr != NULL);

    value = scopedname(attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional Signature object.
 */
static signatureDef *signature(sipSpec *pt, PyObject *obj,
        const char *encoding, signatureDef *value)
{
    PyObject *args_obj;
    Py_ssize_t i;

    if (obj == Py_None)
        return NULL;

    if (value == NULL)
        value = sipMalloc(sizeof (signatureDef));

    argument_attr(pt, obj, "result", encoding, &value->result);

    args_obj = PyObject_GetAttrString(obj, "args");
    assert(args_obj != NULL);
    assert(PyList_Check(args_obj));

    for (i = 0; i < PyList_Size(args_obj) && i < MAX_NR_ARGS; ++i)
        argument(pt, PyList_GetItem(args_obj, i), encoding, &value->args[i]);

    value->nrArgs = i;

    Py_DECREF(args_obj);

    return value;
}


/*
 * Convert an optional Signature attribute.
 */
static signatureDef *signature_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding, signatureDef *value)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);

    assert(attr != NULL);

    value = signature(pt, attr, encoding, value);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional SourceLocation object.
 */
static sourceLocation sourcelocation(PyObject *obj, const char *encoding)
{
    sourceLocation value;

    if (obj != Py_None)
    {
        value.linenr = int_attr(obj, "line");
        value.name = str_attr(obj, "sip_file", encoding);
    }

    return value;
}


/*
 * Convert an optional SourceLocation attribute.
 */
static sourceLocation sourcelocation_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    sourceLocation value;

    assert(attr != NULL);

    value = sourcelocation(attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional str.
 */
static char *str(PyObject *obj, const char *encoding)
{
    PyObject *bytes;
    char *value;

    if (obj == Py_None)
        return NULL;

    bytes = PyUnicode_AsEncodedString(obj, encoding, "strict");
    assert(bytes != NULL);

    value = sipStrdup(PyBytes_AsString(bytes));

    Py_DECREF(bytes);

    return value;
}


/*
 * Convert an optional str attribute.
 */
static char *str_attr(PyObject *obj, const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    char *value;

    assert(attr != NULL);

    value = str(attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional str list attribute.
 */
static stringList *str_list_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    stringList *head = NULL;

    assert(attr != NULL);

    if (attr != Py_None)
    {
        stringList **tail = &head;
        Py_ssize_t i;

        for (i = 0; i < PyList_Size(attr); ++i)
        {
            stringList *item = sipMalloc(sizeof (stringList));
            item->s = str(PyList_GetItem(attr, i), encoding);
            *tail = item;
            tail = &item->next;
        }
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional Template object.
 */
static templateDef *template(sipSpec *pt, PyObject *obj, const char *encoding)
{
    templateDef *value;

    if (obj == Py_None)
        return NULL;

    value = sipMalloc(sizeof (templateDef));

    value->fqname = scopedname_attr(obj, "cpp_name", encoding);
    signature_attr(pt, obj, "types", encoding, &value->types);

    return value;
}


/*
 * Convert an optional Template attribute.
 */
static templateDef *template_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    templateDef *value;

    assert(attr != NULL);

    value = template(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional ThrowArguments object.
 */
static throwArgs *throw_arguments(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    throwArgs *value;
    PyObject *args_obj;

    if (obj == Py_None)
        return NULL;

    value = sipMalloc(sizeof (throwArgs));

    args_obj = PyObject_GetAttrString(obj, "arguments");
    assert(args_obj != NULL);

    if (args_obj != Py_None)
    {
        Py_ssize_t i;

        for (i = 0; i < PyList_Size(args_obj) && i < MAX_NR_ARGS; ++i)
            value->args[i] = exception(pt, PyList_GetItem(args_obj, i), encoding);

        value->nrArgs = i;
    }
    else
    {
        value->nrArgs = -1;
    }

    Py_DECREF(args_obj);

    return value;
}


/*
 * Convert an optional ThrowArguments attribute.
 */
static throwArgs *throw_arguments_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    throwArgs *value;

    assert(attr != NULL);

    value = throw_arguments(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional str attribute as a typeHintDef.
 */
static typeHintDef *typehint_attr(PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    typeHintDef *value;
    char *raw_hint;

    assert(attr != NULL);

    if ((raw_hint = str(attr, encoding)) != NULL)
    {
        if ((value = search_str_cache(cache_typehint, raw_hint)) == NULL)
        {
            value = sipMalloc(sizeof (typeHintDef));

            cache_str(&cache_typehint, raw_hint, value);

            value->status = needs_parsing;
            value->raw_hint = raw_hint;
        }
    }
    else
    {
        value = NULL;
    }

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional TypeHints attribute.
 */
static void typehints_attr(PyObject *obj, const char *name,
        const char *encoding, typeHintDef **th_in, typeHintDef **th_out,
        const char **th_value)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);

    assert(attr != NULL);

    if (attr != Py_None)
    {
        *th_in = typehint_attr(attr, "hint_in", encoding);
        *th_out = typehint_attr(attr, "hint_out", encoding);
        *th_value = str_attr(attr, "default_value", encoding);
    }

    Py_DECREF(attr);
}


/*
 * Convert a Value object.
 */
static valueDef *value(sipSpec *pt, PyObject *obj, const char *encoding)
{
    valueDef *value = sipMalloc(sizeof (valueDef));
    char *op;
    PyObject *val_obj;

    value->vtype = (valueType)enum_attr(obj, "value_type");

    op = str_attr(obj, "unary_operator", encoding);
    if (op != NULL)
    {
        value->vunop = op[0];
        free(op);
    }

    op = str_attr(obj, "binary_operator", encoding);
    if (op != NULL)
    {
        value->vbinop = op[0];
        free(op);
    }

    value->cast = scopedname_attr(obj, "cast", encoding);

    val_obj = PyObject_GetAttrString(obj, "value");
    assert(val_obj != NULL);

    if (val_obj != Py_None)
    {
        char *val_str;

        switch (value->vtype)
        {
        case qchar_value:
            val_str = str(val_obj, encoding);
            value->u.vqchar = val_str[0];
            free(val_str);
            break;

        case numeric_value:
            value->u.vnum = PyLong_AsLong(val_obj);
            break;

        case real_value:
            value->u.vreal = PyFloat_AsDouble(val_obj);
            break;

        case string_value:
            value->u.vstr = str(val_obj, encoding);
            break;

        case scoped_value:
            value->u.vscp = scopedname(val_obj, encoding);
            break;

        case fcall_value:
            value->u.fcd = functioncall(pt, val_obj, encoding);
            break;

        case empty_value:
            break;
        }
    }

    Py_DECREF(val_obj);

    return value;
}


/*
 * Convert an expression.
 */
static valueDef *expr(sipSpec *pt, PyObject *obj, const char *encoding)
{
    valueDef *head = NULL;
    valueDef **tail = &head;
    Py_ssize_t i;

    assert (PyList_Check(obj));

    for (i = 0; i < PyList_Size(obj); ++i)
    {
        valueDef *item = value(pt, PyList_GetItem(obj, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    return head;
}


/*
 * Convert an optional expression attribute.
 */
static valueDef *expr_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    valueDef *value = NULL;

    assert(attr != NULL);

    if (attr != Py_None)
        value = expr(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert an optional VirtualErrorHandler object.
 */
static virtErrorHandler *virtualerrorhandler(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    virtErrorHandler *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_virtualerrorhandler, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (virtErrorHandler));

    cache_object(&cache_virtualerrorhandler, obj, value);

    value->name = str_attr(obj, "name", encoding);
    value->code = codeblock_list_attr(obj, "code", encoding);
    value->mod = module_attr(pt, obj, "module", encoding);
    value->index = int_attr(obj, "handler_nr");

    return value;
}


/*
 * Convert an optional VirtualErrorHandler attribute.
 */
static virtErrorHandler *virtualerrorhandler_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    virtErrorHandler *value;

    assert(attr != NULL);

    value = virtualerrorhandler(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a VirtualErrorHandler list attribute.
 */
static virtErrorHandler *virtualerrorhandler_list_attr(sipSpec *pt,
        PyObject *obj, const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    virtErrorHandler *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        virtErrorHandler *item = virtualerrorhandler(pt,
                PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional VirtualHandler object.
 */
static virtHandlerDef *virtualhandler(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    virtHandlerDef *value;

    if (obj == Py_None)
        return NULL;

    value = sipMalloc(sizeof (virtHandlerDef));

    value->cppsig = signature_attr(pt, obj, "cpp_signature", encoding, NULL);
    value->pysig = signature_attr(pt, obj, "py_signature", encoding, NULL);
    value->virtcode = codeblock_list_attr(obj, "virtual_catcher_code",
            encoding);
    value->veh = virtualerrorhandler_attr(pt, obj, "virtual_error_handler",
            encoding);
    value->virthandlernr = int_attr(obj, "handler_nr");

    if (bool_attr(obj, "abort_on_exception"))
        setAbortOnExceptionVH(value);

    if (bool_attr(obj, "transfer_result"))
        setIsTransferVH(value);

    return value;
}


/*
 * Convert an optional VirtualHandler attribute.
 */
static virtHandlerDef *virtualhandler_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    virtHandlerDef *value;

    assert(attr != NULL);

    value = virtualhandler(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a VirtualHandler list attribute.
 */
static virtHandlerDef *virtualhandler_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    virtHandlerDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        virtHandlerDef *item = virtualhandler(pt, PyList_GetItem(attr, i),
                encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a VirtualOverload object.
 */
static virtOverDef *virtualoverload(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    virtOverDef *value = sipMalloc(sizeof (virtOverDef));

    value->od = over_attr(pt, obj, "overload", encoding);
    value->virthandler = virtualhandler_attr(pt, obj, "handler", encoding);

    return value;
}


/*
 * Convert a VirtualOverload list attribute.
 */
static virtOverDef *virtualoverloadlist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    virtOverDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        virtOverDef *item = virtualoverload(pt, PyList_GetItem(attr, i),
                encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a VisibleMember object.
 */
static visibleList *visible(sipSpec *pt, PyObject *obj, const char *encoding)
{
    visibleList *value = sipMalloc(sizeof (visibleList));

    value->m = member_attr(pt, obj, "member", encoding);
    value->cd = class_attr(pt, obj, "scope", encoding);

    return value;
}


/*
 * Convert a VisibleMember list attribute.
 */
static visibleList *visiblelist_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    visibleList *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        visibleList *item = visible(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a WrappedEnum object.
 */
static enumDef *wrappedenum(sipSpec *pt, PyObject *obj, const char *encoding)
{
    enumDef *value;
    int base_type;
    PyObject *scope_obj;

    if ((value = search_object_cache(cache_wrappedenum, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (enumDef));

    cache_object(&cache_wrappedenum, obj, value);

    if (bool_attr(obj, "is_protected"))
        setIsProtectedEnum(value);

    if (bool_attr(obj, "no_scope"))
        setIsNoScope(value);

    if (bool_attr(obj, "is_scoped"))
        setIsScopedEnum(value);

    if (bool_attr(obj, "needed"))
        setNeedsEnum(value);

    base_type = enum_attr(obj, "base_type");

    if (base_type == EBT_ENUM)
        setIsEnumEnum(value);

    if (base_type == EBT_FLAG)
        setIsEnumFlag(value);

    if (base_type == EBT_INT_ENUM)
        setIsEnumIntEnum(value);

    if (base_type == EBT_INT_FLAG)
        setIsEnumIntFlag(value);

    if (base_type == EBT_UNSIGNED_INT_ENUM)
        setIsEnumUIntEnum(value);

    value->fqcname = scopedname_attr(obj, "fq_cpp_name", encoding);
    value->cname = cachedname_attr(obj, "cached_fq_cpp_name", encoding);
    value->pyname = cachedname_attr(obj, "py_name", encoding);
    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->enumnr = int_attr(obj, "type_nr");

    scope_obj = PyObject_GetAttrString(obj, "scope");
    assert(scope_obj != NULL);

    if (scope_obj != Py_None)
    {
        PyObject *test_obj = PyObject_GetAttrString(scope_obj, "mro");

        if (test_obj != NULL)
        {
            Py_DECREF(test_obj);

            value->ecd = class(pt, scope_obj, encoding);
        }
        else
        {
            value->emtd = mappedtype(pt, scope_obj, encoding);
        }
    }

    Py_DECREF(scope_obj);

    value->module = module_attr(pt, obj, "module", encoding);
    value->members = wrappedenummember_list_attr(pt, obj, "members", encoding);

    value->slots = member_list_attr(pt, obj, "slots", encoding);
    value->overs = over_list_attr(pt, obj, "overloads", encoding);

    return value;
}


/*
 * Convert a WrappedEnum attribute.
 */
static enumDef *wrappedenum_attr(sipSpec *pt, PyObject *obj, const char *name,
        const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    enumDef *value;

    assert(attr != NULL);

    value = wrappedenum(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a WrappedEnum list attribute.
 */
static enumDef *wrappedenum_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    enumDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        enumDef *item = wrappedenum(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a WrappedEnumMember object.
 */
static enumMemberDef *wrappedenummember(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    enumMemberDef *value;

    value = sipMalloc(sizeof (enumMemberDef));

    value->pyname = cachedname_attr(obj, "py_name", encoding);
    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->cname = str_attr(obj, "cpp_name", encoding);
    value->ed = wrappedenum_attr(pt, obj, "scope", encoding);

    return value;
}


/*
 * Convert a WrappedEnumMember list attribute.
 */
static enumMemberDef *wrappedenummember_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    enumMemberDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        enumMemberDef *item = wrappedenummember(pt, PyList_GetItem(attr, i),
                encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert an optional WrappedTypedef object.
 */
static typedefDef *wrappedtypedef(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    typedefDef *value;

    if (obj == Py_None)
        return NULL;

    if ((value = search_object_cache(cache_wrappedtypedef, obj)) != NULL)
        return value;

    value = sipMalloc(sizeof (typedefDef));

    cache_object(&cache_wrappedtypedef, obj, value);

    if (bool_attr(obj, "no_type_name"))
        setNoTypeName(value);

    value->fqname = scopedname_attr(obj, "fq_cpp_name", encoding);
    value->ecd = class_attr(pt, obj, "scope", encoding);
    value->module = module_attr(pt, obj, "module", encoding);
    argument_attr(pt, obj, "type", encoding, &value->type);

    return value;
}


/*
 * Convert an optional WrappedTypedef attribute.
 */
static typedefDef *wrappedtypedef_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    typedefDef *value;

    assert(attr != NULL);

    value = wrappedtypedef(pt, attr, encoding);

    Py_DECREF(attr);

    return value;
}


/*
 * Convert a WrappedTypedef list attribute.
 */
static typedefDef *wrappedtypedef_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    typedefDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        typedefDef *item = wrappedtypedef(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Convert a WrappedVariable object.
 */
static varDef *wrappedvariable(sipSpec *pt, PyObject *obj,
        const char *encoding)
{
    varDef *value = sipMalloc(sizeof (varDef));

    value->fqcname = scopedname_attr(obj, "fq_cpp_name", encoding);
    value->pyname = cachedname_attr(obj, "py_name", encoding);
    value->no_typehint = bool_attr(obj, "no_type_hint");
    value->ecd = class_attr(pt, obj, "scope", encoding);
    value->module = module_attr(pt, obj, "module", encoding);

    if (bool_attr(obj, "is_static"))
        setIsStaticVar(value);

    if (bool_attr(obj, "no_setter"))
        setNoSetter(value);

    if (bool_attr(obj, "needs_handler"))
        setNeedsHandler(value);

    argument_attr(pt, obj, "type", encoding, &value->type);
    value->accessfunc = codeblock_list_attr(obj, "access_code", encoding);
    value->getcode = codeblock_list_attr(obj, "get_code", encoding);
    value->setcode = codeblock_list_attr(obj, "set_code", encoding);

    return value;
}


/*
 * Convert a WrappedVariable list attribute.
 */
static varDef *wrappedvariable_list_attr(sipSpec *pt, PyObject *obj,
        const char *name, const char *encoding)
{
    PyObject *attr = PyObject_GetAttrString(obj, name);
    varDef *head, **tail;
    Py_ssize_t i;

    assert(attr != NULL);

    head = NULL;
    tail = &head;

    for (i = 0; i < PyList_Size(attr); ++i)
    {
        varDef *item = wrappedvariable(pt, PyList_GetItem(attr, i), encoding);
        *tail = item;
        tail = &item->next;
    }

    Py_DECREF(attr);

    return head;
}


/*
 * Append a code block to a list of them.
 */
static void appendCodeBlock(codeBlockList **headp, codeBlock *cb)
{
    codeBlockList *cbl;

    /* Handle the trivial case. */
    if (cb == NULL)
        return;

    /* Find the end of the list. */
    while (*headp != NULL)
    {
        /* Ignore if the block is already in the list. */
        if ((*headp)->block == cb)
            return;

        headp = &(*headp)->next;
    }

    cbl = sipMalloc(sizeof (codeBlockList));
    cbl->block = cb;

    *headp = cbl;
}


/*
 * Convert a text string to a scope part structure.
 */
static scopedNameDef *text2scopePart(char *text)
{
    scopedNameDef *snd;

    snd = sipMalloc(sizeof (scopedNameDef));

    snd->name = text;
    snd->next = NULL;

    return snd;
}
