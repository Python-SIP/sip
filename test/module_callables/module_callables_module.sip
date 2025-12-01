// The SIP implementation of the module_callables_module test module.


%Module(name=module_callables_module)


%ModuleHeaderCode
bool module_procedure_called;

void module_void_ret_no_args()
{
    module_procedure_called = true;
}

void reset_module_procedure_called()
{
    module_procedure_called = false;
}

bool was_module_procedure_called()
{
    return module_procedure_called;
}

int module_doubler(int value)
{
    return 2 * value;
}

int module_sum_diff(int value_1, int value_2, int *diff_p)
{
    *diff_p = value_1 - value_2;

    return value_1 + value_2;
}

PyObject *module_var_args(int ignore, PyObject *var_args)
{
    (void)ignore;

    Py_INCREF(var_args);
    return var_args;
}

int incr_optional_kwd_args(int value, int incr=1)
{
    return value + incr;
}

int incr_all_kwd_args(int value, int incr=1)
{
    return value + incr;
}
%End


void module_void_ret_no_args();
int module_doubler(int value);
int module_sum_diff(int value_1, int value_2, int *diff_p);
SIP_PYTUPLE module_var_args(int ignore, ...);
int incr_optional_kwd_args(int value, int incr=1) /KeywordArgs="Optional"/;
int incr_all_kwd_args(int value, int incr=1) /KeywordArgs="All"/;
void reset_module_procedure_called();
bool was_module_procedure_called();
