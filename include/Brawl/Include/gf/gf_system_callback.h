#pragma once

#include <StaticAssert.h>
#include <types.h>

class gfReturnStatusCallback {
public:
    gfReturnStatusCallback* m_next;
#ifdef MU_NAME_CPP
    gfReturnStatusCallback() : m_next(NULL) {}
#endif
    virtual bool returnStatus() const = 0;
};
static_assert(sizeof(gfReturnStatusCallback) == 0x8, "Class is the wrong size!");

class gfReturnStatusCallbackList {
    gfReturnStatusCallback* m_head;
public:
    gfReturnStatusCallbackList() : m_head(nullptr) { }
    void add(gfReturnStatusCallback* node);
    bool remove(gfReturnStatusCallback* node);
    bool process() const;
};
static_assert(sizeof(gfReturnStatusCallbackList) == 0x4, "Class is the wrong size!");
