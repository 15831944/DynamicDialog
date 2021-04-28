#include "pch.h"
#include "IIDpool.h"
//#include<xstddef>
const _m_UID IID_MAX = 1e6;
std::set < _m_UID,std::greater<_m_UID> > IIDSTACK;
_m_UID getNewIID()
{
    if (IIDSTACK.size() <= 1)
    {
        _m_UID tp; 
        if (IIDSTACK.size() == 0)
        {
            tp = IID_MAX;
        }
        else
        {
            tp = *IIDSTACK.begin();
            IIDSTACK.erase(IIDSTACK.begin());
        };
         for (_m_UID iid = tp - 1000; iid <= tp; ++iid)
                IIDSTACK.insert(iid);
    }
    _m_UID ret = *IIDSTACK.begin();
    IIDSTACK.erase(IIDSTACK.begin());
    return ret;
}

bool releaseIID(_m_UID iid)
{
    //check if there is iid in iid_stack
    auto p = IIDSTACK.insert(iid);
    //report error if you like
    return  (p.second == false);
}
