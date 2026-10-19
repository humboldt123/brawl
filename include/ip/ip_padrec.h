#pragma once

// Local BrawlHeaders shadow: recover the recorder's fields without changing its
// existing 0x80-byte layout or the public pause-status offset.
#include <StaticAssert.h>
#include <gm/gm_global_mode_melee.h>
#include <ip/ip_pad_status.h>
#include <sr/sr_common.h>
#include <stddef.h>
#include <types.h>

struct ipReplayChunk {
    u32 offset;
    u32 size;
    u16 frames;
    u16 unkA;
};

// HYPOTHESIS: descriptive names; offsets and access widths are verified in the
// header setters, encoder, decoder and replay-menu caller.
struct ipReplayHeader {
    u32 magic;
    u16 chunkCount;
    u16 frames;
    u16 idleFrames[4];
    u32 dataSize;
    u32 unk14;
    gmGlobalModeMelee baseInfo;
    u16 advOffset;
    u16 advSize;
    u16 stageOffset;
    u16 stageSize;
    ipReplayChunk chunks[4];
    u16 initialProcessCount;
    u8 unk372[0x16];
};

class ipKeyRecorder {
public:
    enum Mode { Idle = 0, Playing = 1, Recording = 2 };
    // HYPOTHESIS: interface/method spelling. Native playback calls the first
    // virtual slot with a byte command and halfword argument, stopping at the
    // first observer that returns nonzero. No callback lifetime is owned here.
    struct CallBack { virtual int process(u8 command, u16 value) = 0; };
    struct SyncCommand { u8 command; u8 unk1; u16 value; };

    int m_mode;
    CallBack* m_callbacks[8];
    ipReplayHeader* m_buffer; // 24
    u8* m_cursor;
    u8* m_end;
    u16 m_frame;
    u16 unk32;
    u32 m_capacity;
    u16 m_chunk;
    bool m_hasRecording;
    bool m_overflow;
    int m_pauseStatus; // 3C, retained for existing users
    int m_suppressFileSync;
    u32 m_applicationTick;
    u8 m_phase;
    u8 unk49;
    u16 unk4A;
    ipPadStatus m_pads[4];
    u8 m_padMask;
    u8 unk5D;
    SyncCommand m_commands[7]; // 5E, two-byte aligned
    u8 m_commandCount;
    u8 unk7B[5];

    ipKeyRecorder(Heaps::HeapType heap, u32 capacity);
    void clear(u8 value);
    gmGlobalModeMelee* getBaseInfo();
    bool setBaseInfo(const gmGlobalModeMelee* info);
    bool setAdvData(const void* data);
    void* getStageData();
    bool setStageData(const void* data, u16 size);
    void getBufferData(void** data, u32* size);
    bool setBufferData(const void* data, u32 size);
    void setPadStatus(const ipPadStatus* pads, u32 count);
    u16 getMeleeInitialProcessCount();
    void setMeleeInitialProcessCount(u16 count);
    void startFileReadSyncMode();
    bool startRec(u32 chunk);
    bool endRec();
    bool startPlay(int chunk);
    bool endPlay();
    bool addCallBack(CallBack* callback);
    bool removeCallBack(CallBack* callback);
    int getCurrentFrameNo();
    bool setSyncCommand(u32 command, int value);
    // HYPOTHESIS: spelling; the input-switch caller and mode guard prove this
    // unnamed native entry writes a recording packet.
    bool record();
    bool play(u8** packet, int* groupFrames, int* phaseFrames);
    bool extractPadStatus(ipPadStatus* pads, const u8* packet);
    static void clearOnamae(ipReplayHeader* data);
    static bool isSafeData(const ipReplayHeader* data, u32 size);

private:
    // HYPOTHESIS: source-level names for repeated inline packet/seek operations.
    static inline int packetSize(u8 flags) {
        u8 pads = 0;
        for (u8 i = 0; i < 4; ++i) if (flags & (1u << i)) ++pads;
        return 1 + pads * 4 + ((flags >> 4) & 7) * 4;
    }
    inline bool advancePacket() {
        int size = packetSize(*m_cursor);
        if (m_cursor + size > m_end) return false;
        m_cursor += size;
        return true;
    }
    inline void seekFrame(int frame) {
        m_frame = 0;
        m_cursor = (u8*)m_buffer + m_buffer->chunks[m_chunk].offset;
        u16 count = m_buffer->chunks[m_chunk].frames;
        for (int i = 0; i < frame; ++i) {
            if (!advancePacket() || i >= count) break;
            ++m_frame;
        }
    }
};
static_assert(sizeof(ipReplayChunk) == 12, "Replay chunk header size");
static_assert(sizeof(ipReplayHeader) == 0x388, "Replay header size");
static_assert(sizeof(ipKeyRecorder) == 0x80, "Recorder size");
static_assert(offsetof(ipKeyRecorder, m_pauseStatus) == 0x3C, "Recorder pause offset");
static_assert(offsetof(ipKeyRecorder, m_commands) == 0x5E, "Recorder command queue offset");
static_assert(offsetof(ipKeyRecorder, m_commandCount) == 0x7A, "Recorder command count offset");

static_assert(offsetof(ipReplayHeader, chunks) == 0x340, "Replay chunk table offset");
static_assert(offsetof(ipReplayHeader, initialProcessCount) == 0x370, "Replay initial process offset");
static_assert(offsetof(ipKeyRecorder, m_buffer) == 0x24, "Recorder buffer offset");
