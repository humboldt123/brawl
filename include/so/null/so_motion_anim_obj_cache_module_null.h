#pragma once

#include <so/so_null.h>
#include <types.h>

class soMotionAnimObjCacheModuleNull : public soNullable {
public:
    virtual void* getCacheAnimChr(int);
    virtual bool setCacheAnimChr(int, void*, int);
    virtual bool isContainAnimChr(int);
};
