#pragma once
// NOTE: shadows the BrawlHeaders copy to add the constructor used by soGeneralWorkBuilder.

#include <StaticAssert.h>
#include <so/work/so_general_work_abstract.h>
#include <types.h>

class soGeneralWorkSimple : public soGeneralWorkAbstract {
    void clearInts(const u32& count);
    void clearFloats(const u32& count);
    void clearFlags(const u32& count);
    // (clearWorkAll: Marth's copy and the Enemy's (SO_GENERAL_WORK_CLEAR_RELOADS_SIZE) load each size twice and take the
    // minimum; the other fighters' copies load it once.)
    void clearInts() {
#if defined(FT_MARTH_RUNTIME_HELPERS) || defined(SO_GENERAL_WORK_CLEAR_RELOADS_SIZE)
        u32 intWorkSize = m_intWorkSize;
        // MATCH-ONLY: retain the original second size load before taking the minimum.
        u32 currentSize = *(volatile u32*)&m_intWorkSize;
        if (intWorkSize >= currentSize) intWorkSize = currentSize;
#else
        s32 intWorkSize = m_intWorkSize;
        intWorkSize = (intWorkSize != m_intWorkSize) ? m_intWorkSize : intWorkSize;
#endif
        for (s32 i = 0; i < (s32)intWorkSize; i++) {
            m_intWorks[i] = 0;
        }
    }

    void clearFloats() {
#if defined(FT_MARTH_RUNTIME_HELPERS) || defined(SO_GENERAL_WORK_CLEAR_RELOADS_SIZE)
        u32 floatWorkSize = m_floatWorkSize;
        // MATCH-ONLY: retain the original second size load before taking the minimum.
        u32 currentSize = *(volatile u32*)&m_floatWorkSize;
        if (floatWorkSize >= currentSize) floatWorkSize = currentSize;
#else
        s32 floatWorkSize = m_floatWorkSize;
        floatWorkSize = (floatWorkSize != m_floatWorkSize) ? m_floatWorkSize : floatWorkSize;
#endif
        for (s32 i = 0; i < (s32)floatWorkSize; i++) {
            m_floatWorks[i] = 0;
        }
    }

    void clearFlags() {
#if defined(FT_MARTH_RUNTIME_HELPERS) || defined(SO_GENERAL_WORK_CLEAR_RELOADS_SIZE)
        u32 flagWorkSize = m_flagWorkSize;
        // MATCH-ONLY: retain the original second size load before taking the minimum.
        u32 currentSize = *(volatile u32*)&m_flagWorkSize;
        if (flagWorkSize >= currentSize) flagWorkSize = currentSize;
#else
        s32 flagWorkSize = m_flagWorkSize;
        flagWorkSize = (flagWorkSize != m_flagWorkSize) ? m_flagWorkSize : flagWorkSize;
#endif
        for (s32 i = 0; i < (s32)flagWorkSize; i++) {
            m_flagWorks[i] = 0;
        }
    }
public:
    s32* m_intWorks;
    u32 m_intWorkSize;
    float* m_floatWorks;
    u32 m_floatWorkSize;
    u32* m_flagWorks;
    u32 m_flagWorkSize;

    soGeneralWorkSimple() { }
#ifdef SO_BUILDER_HELPERS_WEAK
    // MATCH-ONLY: the Enemy unit calls its own weak copy of the work-area constructor (the fighter RELs define it in
    // ft_builder_noinline.h); the copy brings the class's vtable and every accessor with it.
    soGeneralWorkSimple(s32* ints, u32 numInts, float* floats, u32 numFloats, u32* flags, u32 numFlags)
        __attribute__((never_inline))
        : soGeneralWorkAbstract(false), m_intWorks(ints), m_intWorkSize(numInts), m_floatWorks(floats),
          m_floatWorkSize(numFloats), m_flagWorks(flags), m_flagWorkSize(numFlags) { }
#else
    soGeneralWorkSimple(s32* ints, u32 numInts, float* floats, u32 numFloats, u32* flags, u32 numFlags);
#endif

    virtual s32 getIntWork(u32 index) const {
        return m_intWorks[index];
    }

    virtual void setIntWork(s32 value, u32 index) {
        m_intWorks[index] = value;
    }

    virtual void addIntWork(s32 value, u32 index) {
        m_intWorks[index] += value;
    }

    virtual void subIntWork(s32 value, u32 index) {
        m_intWorks[index] -= value;
    }

    virtual void mulIntWork(s32 value, u32 index) {
        m_intWorks[index] *= value;
    }

    virtual void divIntWork(s32 value, u32 index) {
        if (0 != value) {
            m_intWorks[index] /= value;
        }
    }

    virtual void incIntWork(u32 index) {
        m_intWorks[index]++;
    }

    virtual void decIntWork(u32 index) {
        m_intWorks[index]--;
    }

    virtual u32 getIntWorkSize() {
        return m_intWorkSize;
    }

    virtual float getFloatWork(u32 index) const {
        return m_floatWorks[index];
    }

    virtual void setFloatWork(float value, u32 index) {
        m_floatWorks[index] = value;
    }

    virtual void addFloatWork(float value, u32 index) {
        m_floatWorks[index] += value;
    }

    virtual void subFloatWork(float value, u32 index) {
        m_floatWorks[index] -= value;
    }

    virtual void mulFloatWork(float value, u32 index) {
        m_floatWorks[index] *= value;
    }

    virtual void divFloatWork(float value, u32 index) {
        if (0.0f != value) {
            m_floatWorks[index] /= value;
        }
    }

    virtual u32 getFloatWorkSize() {
        return m_floatWorkSize;
    }

    virtual void onFlag(u32 flag, u32 index) {
        m_flagWorks[index] |= flag;
    }

    virtual void offFlag(u32 flag, u32 index) {
        m_flagWorks[index] &= ~flag;
    }

    virtual void clearFlag(u32 index) {
        m_flagWorks[index] = 0;
    }

    virtual bool isFlag(u32 flag, u32 index) const {
        return m_flagWorks[index] & flag;
    }

    virtual u32 turnOffFlag(u32 flag, u32 index) {
        u32 result;
        if ((result = isFlag(flag, index)) == 1) {
            offFlag(flag, index);
        }
        return result;
    }

    virtual u32 getFlagWorkSize() {
        return m_flagWorkSize;
    }

    virtual void clearWorkAll() {
#ifdef SO_GENERAL_WORK_BOUNDED_CLEAR
        clearInts(m_intWorkSize);
        clearFloats(m_floatWorkSize);
        clearFlags(m_flagWorkSize);
#else
        clearInts();
        clearFloats();
        clearFlags();
#endif
    }
};
static_assert(sizeof(soGeneralWorkSimple) == 0x24, "Class is wrong size!");


extern soGeneralWorkSimple g_soGeneralWorkSimple;

// HYPOTHESIS: the bounded helper boundary explains the capacity clamp in Mario.
// Define these after their caller so MWCC preserves the two input snapshots.
inline void soGeneralWorkSimple::clearInts(const u32& count) {
    u32 size = count;
    u32 capacity = m_intWorkSize;
    if (size >= capacity) size = capacity;
    for (s32 i = 0; i < (s32)size; ++i) m_intWorks[i] = 0;
}

inline void soGeneralWorkSimple::clearFloats(const u32& count) {
    u32 size = count;
    u32 capacity = m_floatWorkSize;
    if (size >= capacity) size = capacity;
    for (s32 i = 0; i < (s32)size; ++i) m_floatWorks[i] = 0.0f;
}

inline void soGeneralWorkSimple::clearFlags(const u32& count) {
    u32 size = count;
    u32 capacity = m_flagWorkSize;
    if (size >= capacity) size = capacity;
    for (s32 i = 0; i < (s32)size; ++i) m_flagWorks[i] = 0;
}
