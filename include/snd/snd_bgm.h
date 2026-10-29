#pragma once

#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: field names from the accesses of sndBgmRateSystem (snd_bgmsys.o): one selectable
// track of a BGM category ("snd_bgmSysData", ended by category 0xFF).
struct sndBgmRateData {
    u32 m_bgmId;       // 0x0
    u16 m_playTiming;  // 0x4
    u8 m_category;     // 0x6
    u8 m_rate;         // 0x7
};
static_assert(sizeof(sndBgmRateData) == 0x8, "Class is wrong size!");

// HYPOTHESIS: one track title entry ("sndBgmTitleData", ended by unk8 == 0xFFFFFFFF).
struct sndBgmTitleData {
    u32 m_playId;    // 0x0
    u32 m_series;    // 0x4
    u32 unk8;        // 0x8
    char _C[0x4];
    u32 m_msgIndex;  // 0x10
    char _14[0x1C];
};
static_assert(sizeof(sndBgmTitleData) == 0x30, "Class is wrong size!");

class sndBgmRateSystem {
public:
    sndBgmRateData* m_rateData;          // 0x0
    u32 m_categoryCount[45];             // 0x4
    u32 m_categoryStart[45];             // 0xB8
    u32 m_rateCount;                     // 0x16C
    sndBgmRateData* m_currentRate;       // 0x170
    int m_category;                      // 0x174
    u32 m_bgmId;                         // 0x178
    u32 unk17C;                          // 0x17C
    u32 m_frame;                         // 0x180
    u32 unk184;                          // 0x184
    u32 unk188;                          // 0x188
    bool unk18C;                         // 0x18C
    u32 unk190;                          // 0x190
    sndBgmTitleData* m_titleData;        // 0x194
    u32 m_titleCount;                    // 0x198
    u32 m_seriesCount[24];               // 0x19C
    u32 m_seriesList[24][50];            // 0x1FC
    u32 m_enabledTitleCount;             // 0x14BC
    int m_state;                         // 0x14C0
    virtual ~sndBgmRateSystem();         // 0x14C4
    void* m_rateArchive;                 // 0x14C8
    u32 m_rateArchiveSize;               // 0x14CC
    void* m_titleArchive;                // 0x14D0
    u32 m_titleArchiveSize;              // 0x14D4

    sndBgmRateSystem();
    static sndBgmRateSystem* create();
    void setupBGMList();
    // HYPOTHESIS: parameter types from the register use (signed category, unsigned ids).
    void setRate(int category, u32 bgmId, u8 rate);
    u8 getRate(int category, u32 bgmId);
    u32 getBgmId(int category, int index);
    int prepareBGM(int category, u32 id, int bgmId);
    bool isPreparedBGM();
    void startBGM(u32 param);
    u16 getPlayTiming();
    int setBgmId(int category, u32 id, int bgmId);
    bool getSeriesAndId(u32 playId, u32* series, u32* index);
    u32 getSeriesId(u32 key);
};
static_assert(sizeof(sndBgmRateSystem) == 0x14D8, "Class is wrong size!");

extern sndBgmRateSystem* lbl_805A01D8;
