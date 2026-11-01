#pragma once

// wnExtendParamAccesser<FloatBase, NumFloats, IntBase, NumInts, IndefiniteBase, NumIndefinites>: an article's parameter
// accesser. Parameter ids above a base select one of the pointers into the article's parameter block (ids start at
// base + 1); the second index selects the element.

#include <so/so_value_accesser.h>
#include <types.h>

template <s32 FloatBase, s32 NumFloats, s32 IntBase, s32 NumInts, s32 IndefiniteBase, s32 NumIndefinites>
class wnExtendParamAccesser : public soParamAccesser {
    s32 m_kind;           // +0xC
    s32 m_floatBase;      // +0x10
    s32 m_intBase;        // +0x14
    s32 m_indefiniteBase; // +0x18
protected:
    // HYPOTHESIS: one spare slot per table (the original reserves N + 1 entries).
    const float* m_floats[NumFloats + 1];         // +0x1C
    const s32* m_ints[NumInts + 1];
    void* m_indefinites[NumIndefinites + 1];
public:
    wnExtendParamAccesser(s32 kind) :
        m_kind(kind), m_floatBase(FloatBase), m_intBase(IntBase), m_indefiniteBase(IndefiniteBase) { }
#ifdef WN_EXTEND_PARAM_ACCESSER_NOINLINE
    // BYTES: an owning unit whose derived accesser overrides the queries keeps the base ones out of line.
    virtual float getParamFloat(soModuleAccesser*, s32 id, s32 index) __attribute__((never_inline)) {
        return m_floats[id - m_floatBase - 1][index];
    }
    virtual s32 getParamInt(soModuleAccesser*, s32 id, s32 index) __attribute__((never_inline)) {
        return m_ints[id - m_intBase - 1][index];
    }
    virtual void* getParamIndefinite(soModuleAccesser*, s32 id, s32) __attribute__((never_inline)) {
        return m_indefinites[id - m_indefiniteBase - 1];
    }
#else
    virtual float getParamFloat(soModuleAccesser*, s32 id, s32 index) {
        return m_floats[id - m_floatBase - 1][index];
    }
    virtual s32 getParamInt(soModuleAccesser*, s32 id, s32 index) {
        return m_ints[id - m_intBase - 1][index];
    }
    virtual void* getParamIndefinite(soModuleAccesser*, s32 id, s32) {
        return m_indefinites[id - m_indefiniteBase - 1];
    }
#endif
};

#ifdef WN_OCTOPUS_PARAM_ACCESSER_OUT_OF_LINE
// This instance's virtual queries belong to the Octopus collision unit.
template <>
class wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> : public soParamAccesser {
    s32 m_kind;
    s32 m_floatBase;
    s32 m_intBase;
    s32 m_indefiniteBase;
protected:
    const float* m_floats[4];
    const s32* m_ints[1];
    void* m_indefinites[1];
public:
    wnExtendParamAccesser(s32 kind) :
        m_kind(kind), m_floatBase(3999), m_intBase(23999), m_indefiniteBase(0) { }
    virtual float getParamFloat(soModuleAccesser*, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser*, s32 id, s32 index);
    virtual void* getParamIndefinite(soModuleAccesser*, s32 id, s32 index);
};
#endif
