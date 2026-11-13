#pragma once

#include <havok/hkMemory.h>

// Per-thread performance monitor stream (timer begin/end commands).
struct hkMonitorStream {
    char* m_start;        // 0x00
    char* unk4;           // 0x04
    char* unk8;           // 0x08
    char* unkC;           // 0x0C
    hkBool m_isBufferAllocatedOnHeap; // 0x10

    hkMonitorStream() {}

    // Timer command written at the current end of the stream (0x0C bytes): the name string and the low
    // time-base word. HYPOTHESIS: the third word is unused on this target (never written by the timers).
    struct TimerCommand {
        const char* m_name; // 0x00
        u32 m_time;         // 0x04
        u32 unk8;           // 0x08

        // Stamps the command with the low time-base word (map name hkMonitorStream::TimerCommand::setTime).
        void setTime();
    };
    // Begin command of a timer list (0x10 bytes): a timer command plus the name of the list's first split.
    struct TimerBeginListCommand : TimerCommand {
        const char* m_firstSplitName; // 0x0C
    };

    // True while there is room for another command (end of data below the 0x0C limit word).
    hkBool memoryAvailable() const { return unk4 < unkC; }
    // The current end of the stream, where the next command goes, and its update after a command is written.
    char* getEnd() const { return unk4; }
    void setEnd(char* end) { unk4 = end; }

    static hkMonitorStream& getInstance() { return s_instance; }

    static void init();
    void quit();

    static hkMonitorStream s_instance;
};

// Reads the low word of the time base (mftb, TBL).
inline u32 hkMonitorStream_getTicks() {
    register u32 ticks;
    asm { mftb ticks, 268 }
    return ticks;
}

inline void hkMonitorStream::TimerCommand::setTime() {
    m_time = hkMonitorStream_getTicks();
}

// The timer bracket of the collision and world functions: a name command stamped with the time base, written
// only while the stream has room. A macro: the name is materialized inside the branch, after the test, and the
// end of the stream is read once (before the limit word). HK_TIMER_BEGIN takes the bare name and prefixes "Tt";
// HK_TIMER_END writes "Et". Asm-verified on hkWorld::stepDeltaTime.
// MATCH-ONLY: the direct hkBool(...) test instead of memoryAvailable(); the member call reloads the end pointer.
// The limit word is read into a local first. With copy propagation on (most functions) it is folded back and the
// end word loads first with an extsb. test; in functions where CodeWarrior turns propagation off (see hkSimdReal
// in hkVector4.h) the limit word loads first and the hkBool becomes a bool before the branch
// (extsb; neg; or; srwi.). One spelling gives both forms of the image.
#define HK_MONITOR_TIMER_COMMAND(NAME)                                                       \
    {                                                                                        \
        hkMonitorStream& mStream = hkMonitorStream::getInstance();                           \
        char* limit = mStream.unkC;                                                          \
        if (hkBool(mStream.unk4 < limit)) {                                                  \
            hkMonitorStream::TimerCommand* h = (hkMonitorStream::TimerCommand*)mStream.unk4; \
            h->m_name = NAME;                                                                \
            h->m_time = hkMonitorStream_getTicks();                                          \
            mStream.unk4 = (char*)(h + 1);                                                   \
        }                                                                                    \
    }
#define HK_TIMER_BEGIN(NAME) HK_MONITOR_TIMER_COMMAND("Tt" NAME)
#define HK_TIMER_END() HK_MONITOR_TIMER_COMMAND("Et")

// The same brackets written with the stream accessors (the Havok macro form). Functions compiled without inlining
// (hkBvTreeStreamAgent::processCollision) call each accessor out of line, which shows this form in the image.
#define HK_TIMER_COMMAND_ACCESSORS(NAME)                                                     \
    {                                                                                        \
        hkMonitorStream& mStream = hkMonitorStream::getInstance();                           \
        if (mStream.memoryAvailable()) {                                                     \
            hkMonitorStream::TimerCommand* h = (hkMonitorStream::TimerCommand*)mStream.getEnd(); \
            h->m_name = NAME;                                                                \
            h->setTime();                                                                    \
            mStream.setEnd((char*)(h + 1));                                                  \
        }                                                                                    \
    }
#define HK_TIMER_BEGIN_LIST_ACCESSORS(NAME, FIRST)                                           \
    {                                                                                        \
        hkMonitorStream& mStream = hkMonitorStream::getInstance();                           \
        if (mStream.memoryAvailable()) {                                                     \
            hkMonitorStream::TimerBeginListCommand* h =                                      \
                (hkMonitorStream::TimerBeginListCommand*)mStream.getEnd();                   \
            h->m_name = "Lt" NAME;                                                           \
            h->m_firstSplitName = "St" FIRST;                                                \
            h->setTime();                                                                    \
            mStream.setEnd((char*)(h + 1));                                                  \
        }                                                                                    \
    }
#define HK_TIMER_SPLIT_LIST_ACCESSORS(NAME) HK_TIMER_COMMAND_ACCESSORS("St" NAME)
#define HK_TIMER_END_LIST_ACCESSORS() HK_TIMER_COMMAND_ACCESSORS("lt")
#define HK_TIMER_BEGIN_ACCESSORS(NAME) HK_TIMER_COMMAND_ACCESSORS("Tt" NAME)
#define HK_TIMER_END_ACCESSORS() HK_TIMER_COMMAND_ACCESSORS("Et")

// Timer lists: the begin command (0x10 bytes) carries the name of the first split at 0x0C, the splits and the end
// are ordinary 0x0C-byte commands ("St" + split name, "lt"). Store order from hkBvAgent::processCollision.
#define HK_TIMER_BEGIN_LIST(NAME, FIRST)                                                     \
    {                                                                                        \
        hkMonitorStream& mStream = hkMonitorStream::getInstance();                           \
        char* limit = mStream.unkC;                                                          \
        if (hkBool(mStream.unk4 < limit)) {                                                  \
            hkMonitorStream::TimerBeginListCommand* h =                                      \
                (hkMonitorStream::TimerBeginListCommand*)mStream.unk4;                       \
            h->m_name = "Lt" NAME;                                                           \
            h->m_firstSplitName = "St" FIRST;                                                \
            h->m_time = hkMonitorStream_getTicks();                                          \
            mStream.unk4 = (char*)(h + 1);                                                   \
        }                                                                                    \
    }
#define HK_TIMER_SPLIT_LIST(NAME) HK_MONITOR_TIMER_COMMAND("St" NAME)
#define HK_TIMER_END_LIST() HK_MONITOR_TIMER_COMMAND("lt")

// Scoped timer: begins a timer on construction and ends it when it goes out of scope. Its out-of-line destructor
// sits in hkSimulation.o (map: hkTimeFunctionHelper::__dt), and hkContinuousSimulation's exception tables name it
// as a cleanup action, so it is an inline member shared by several units. The "Et" literal of the inline
// destructor is the weak .sdata object @STRING@__dt__20hkTimeFunctionHelperFv (MWCC names the literals of inline
// functions after the function and the linker merges the copies), which is why hkSimulation and
// hkContinuousSimulation load the same "Et" while plain HK_TIMER_END sites use their unit's string pool.
// The begin is the plain macro at the call site, not a constructor taking the name: hkSimulation::stepDeltaTime
// materializes the name inside the room test, which an argument evaluated before an inlined body would not give.
struct hkTimeFunctionHelper {
    ~hkTimeFunctionHelper() { HK_TIMER_END(); }
};
#define HK_TIME_CODE_BLOCK(NAME)                                                                 HK_TIMER_BEGIN(NAME);                                                                        hkTimeFunctionHelper timeFunctionHelper
