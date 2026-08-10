// Define the SIP wrapper to the (theoretical) fib library.

%Module(name=fib, language="C", gil_use="NotUsed",
        multi_interpreter_support="Supported")

%MinimumABIVersion "12.0"
%MinimumABIVersion "13.0"
%MinimumABIVersion "14.0"

int fib_n(int n);
%MethodCode
    if (a0 <= 0)
    {
        sipRes = 0;
    }
    else
    {
        int a = 0, b = 1, c, i;

        for (i = 2; i <= a0; i++)
        {
            c = a + b;
            a = b;
            b = c;
        }

        sipRes = b;
    }
%End
