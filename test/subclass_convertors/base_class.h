// The declaration of the BaseClass test class.


#if !defined(_BASE_CLASS_H)
#define _BASE_CLASS_H

class BaseClass
{
public:
    BaseClass(int type = 0) : m_type(type), m_inst(0) {}
    virtual ~BaseClass() {}

    int get_type() const {return m_type;}
    BaseClass *get_instance() const {return m_inst;}
    void init_instance() {m_inst = create_instance();}

    virtual BaseClass *create_instance() = 0;

private:
    int m_type;
    BaseClass *m_inst;
};

#endif
