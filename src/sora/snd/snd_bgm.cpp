#include <revolution/OS/OSError.h>
#include <mt/mt_prng.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_relocate.h>

// HYPOTHESIS: stand-ins for gmCheckBGMIDUseEnable (gm_lib.o) and sndSystem BGM entries, which are still fn_.
extern "C" bool fn_8004F978(u32 bgmId);
extern "C" void fn_80073F84(sndSystem* system, int fade);
extern "C" bool fn_80074018(sndSystem* system, u32 bgmId, int unk);
extern "C" bool fn_80074184(sndSystem* system, u32 bgmId);
extern "C" void fn_800741EC(sndSystem* system, u32 bgmId, u32 param);

sndBgmRateSystem* lbl_805A01D8;

sndBgmRateSystem::sndBgmRateSystem() {
    m_rateCount = 0;
    m_currentRate = NULL;
    m_bgmId = 0;
    unk17C = 0;
    m_frame = 0;
    unk184 = 0;
    unk188 = 0;
    unk18C = false;
    unk190 = 0x26FA;
    m_rateArchive = NULL;
    m_rateArchiveSize = 0;
    m_titleArchive = NULL;
    m_titleArchiveSize = 0;
    for (int i = 0; i < 24; i++) {
        m_seriesCount[i] = 0;
        for (int j = 0; j < 50; j++) {
            m_seriesList[i][j] = 0;
        }
    }
    m_enabledTitleCount = 0;
    m_state = 0;
}

sndBgmRateSystem* sndBgmRateSystem::create() {
    if (lbl_805A01D8 == NULL) {
        lbl_805A01D8 = new (Heaps::Sound) sndBgmRateSystem;
    }
    return lbl_805A01D8;
}

// HYPOTHESIS: still configured as fn_80078990; only the clear loop's register allocation differs.
void sndBgmRateSystem::setupBGMList() {
    for (int i = 0; i < 24; i++) {
        m_seriesCount[i] = 0;
        for (int j = 0; j < 50; j++) {
            m_seriesList[i][j] = 0;
        }
    }
    m_enabledTitleCount = 0;
    utRelocate relocate(static_cast<u8*>(m_titleArchive), m_titleArchiveSize);
    m_titleData = static_cast<sndBgmTitleData*>(relocate.getPublicAddress("sndBgmTitleData"));
    for (m_titleCount = 0; m_titleData[m_titleCount].unk8 != 0xFFFFFFFF; m_titleCount++) {
    }
    for (u32 i = 0; i < m_titleCount; i++) {
        sndBgmTitleData* data = &m_titleData[i];
        switch (data->m_playId) {
            case 0x2709:
            case 0x27B9:
            case 0x27BA:
            case 0x27BB:
            case 0x27BC:
            case 0x27BD:
            case 0x27BE:
                break;
            default:
                if (fn_8004F978(data->m_playId)) {
                    m_enabledTitleCount++;
                    u32 series = data->m_series;
                    if (series < 21) {
                        m_seriesList[series][m_seriesCount[series]] = i;
                        m_seriesCount[series]++;
                    }
                }
                break;
        }
    }
}

sndBgmRateSystem::~sndBgmRateSystem() { }

void sndBgmRateSystem::setRate(int category, u32 bgmId, u8 rate) {
    for (u32 i = m_categoryStart[category]; m_rateData[i].m_category == category; i++) {
        if (bgmId == m_rateData[i].m_bgmId) {
            m_rateData[i].m_rate = rate;
            return;
        }
    }
}

u8 sndBgmRateSystem::getRate(int category, u32 bgmId) {
    for (u32 i = m_categoryStart[category]; m_rateData[i].m_category == category; i++) {
        if (bgmId == m_rateData[i].m_bgmId) {
            return m_rateData[i].m_rate;
        }
    }
    return 0;
}

u32 sndBgmRateSystem::getBgmId(int category, int index) {
    return m_rateData[m_categoryStart[category] + index].m_bgmId;
}

// HYPOTHESIS: sndBgmRateSystem::stopBGM; st_madein.cpp still calls it by this address name.
extern "C" void fn_800790FC(sndBgmRateSystem* system, int fade) {
    fn_80073F84(g_sndSystem, fade);
}

int sndBgmRateSystem::prepareBGM(int category, u32 id, int bgmId) {
    if (id != 0xFFFFFFFF) {
        if (bgmId == 0) {
            int selected = setBgmId(category, id, 0);
            if (selected == -1) {
                return -1;
            }
            m_bgmId = selected;
        } else {
            m_bgmId = bgmId;
            setBgmId(category, id, bgmId);
        }
    } else if (category == 0x16) {
        if (m_bgmId >= 0x27A9 && m_bgmId <= 0x27B2) {
            m_bgmId = 0x27A1;
        }
    }
    if (m_bgmId == 0) {
        return -1;
    }
    if (fn_80074018(g_sndSystem, m_bgmId, 1)) {
        return m_bgmId;
    }
    return -1;
}

bool sndBgmRateSystem::isPreparedBGM() {
    return fn_80074184(g_sndSystem, m_bgmId);
}

void sndBgmRateSystem::startBGM(u32 param) {
    unk17C = param;
    m_frame = 0;
    fn_800741EC(g_sndSystem, m_bgmId, param);
}

u16 sndBgmRateSystem::getPlayTiming() {
    return m_currentRate->m_playTiming;
}

// HYPOTHESIS: sndBgmRateSystem::getBgmTitleDataPlayId; if_center.cpp still calls it by this address name.
extern "C" sndBgmTitleData* fn_800791FC(sndBgmRateSystem* system, u32 playId) {
    for (u32 i = 0; i < system->m_titleCount; i++) {
        sndBgmTitleData* data = &system->m_titleData[i];
        if (data->m_playId == playId) {
            return data;
        }
    }
    return NULL;
}

int sndBgmRateSystem::setBgmId(int category, u32 id, int bgmId) {
    int total = 0;
    m_category = category;
    unk184 = id;
    if (bgmId == 0) {
        if (category == 0x2C) {
            return 0x26F9;
        }
        for (u32 i = m_categoryStart[category]; m_rateData[i].m_category == category; i++) {
            if (fn_8004F978(m_rateData[i].m_bgmId)) {
                total += m_rateData[i].m_rate;
            }
        }
        if (total == 0) {
            bgmId = m_rateData[m_categoryStart[category]].m_bgmId;
            m_currentRate = &m_rateData[m_categoryStart[category]];
        } else {
            int roll = randi(total);
            for (u32 i = m_categoryStart[category]; m_rateData[i].m_category == category; i++) {
                if (fn_8004F978(m_rateData[i].m_bgmId)) {
                    if (roll < m_rateData[i].m_rate) {
                        bgmId = m_rateData[i].m_bgmId;
                        m_currentRate = &m_rateData[i];
                        OSReport("PlayID[%5d] Index[%d]\n", bgmId, i);
                        break;
                    }
                    roll -= m_rateData[i].m_rate;
                }
            }
        }
        switch (bgmId) {
            case 0x2751:
                if (id == 2 || id == 3) {
                    bgmId = 0x2756;
                }
                break;
        }
        return bgmId;
    }
    m_currentRate = NULL;
    for (u32 i = m_categoryStart[category]; m_rateData[i].m_category == category; i++) {
        if (bgmId == m_rateData[i].m_bgmId) {
            m_currentRate = &m_rateData[i];
            break;
        }
    }
    return bgmId;
}

bool sndBgmRateSystem::getSeriesAndId(u32 playId, u32* series, u32* index) {
    for (u32 i = 0; i < 24; i++) {
        for (u32 j = 0; j < m_seriesCount[i]; j++) {
            if (playId == m_titleData[m_seriesList[i][j]].m_playId) {
                *series = i;
                *index = j;
                return true;
            }
        }
    }
    return false;
}

u32 sndBgmRateSystem::getSeriesId(u32 key) {
    u32 i;
    for (i = 0; i < 24; i++) {
        if (m_titleData[m_seriesList[i][0]].m_series >= key) {
            break;
        }
    }
    return i;
}
