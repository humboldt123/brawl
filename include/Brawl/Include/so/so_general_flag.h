#pragma once

#include <StaticAssert.h>

template <typename T>
struct soGeneralFlag {
    T flag;
#ifdef IT_GENERAL_FLAG_OOL
    soGeneralFlag(T init);
#ifdef IT_GENERAL_FLAG_DEFAULT
    soGeneralFlag();
#endif
#else
    soGeneralFlag(T init) : flag(init) { }
#ifdef IT_GENERAL_FLAG_DEFAULT
    soGeneralFlag() : flag(0) { }
#endif
#endif
};
