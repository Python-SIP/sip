// Define the SIP wrapper to the age library.

%Module examples.age

class Age
{
%TypeHeaderCode
class Age
{
public:
    Age(int a) : m_age(a)
    {
    }

    int months() const
    {
        return m_age * 12;
    }

private:
    int m_age;
};
%End

public:
    Age(int a);

    int months() const;
};
