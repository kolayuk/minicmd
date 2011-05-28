
#ifndef __PARAMETER_H__
#define	__PARAMETER_H__

#include <e32base.h>

enum AttStatus
{
    not_set = 0,    //not define
    add_to,         //+
    rm_it           //-
};

struct Parameter
{
    AttStatus s;    //sys attribute
    AttStatus h;    //hidden
    AttStatus r;    //read only
    
    TBool is;       //include subfolder?
    TBool ow;       //overwrite exists file?
    
    Parameter()
    {
        s = h = r = not_set;
        is = ow = EFalse;
    }
    
    TBool NeedToSetAtt() const{
        return s + h + r > 0;
    }
    
    Parameter & operator = (const Parameter &other)
    {
        this->s = other.s;
        this->h = other.h;
        this->r = other.r;
        this->is = other.is;
        this->ow = other.ow;
        
        return *this;
    }
};

#endif  /* __PARAMETER_H__ */
