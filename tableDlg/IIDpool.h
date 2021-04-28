#pragma once
#ifndef IID_POOL_H
#define IID_POOL_H

#include <cstdint>
#include<stack>
#include<set>
using _m_UID = uint32_t;

extern const _m_UID IID_MAX;
 extern std::set < _m_UID,std::greater<_m_UID> >IIDSTACK;
extern _m_UID getNewIID();
extern bool releaseIID(_m_UID iid);
#endif
