#include <types.h>
#include <snd/snd_system.h>
#include <gf/gf_error_check.h>
#include <gf/gf_heap_manager.h>

extern "C" s32 fn_8007AB34(sndHeapSys* heap, SndGroupID groupId);
extern "C" s32 fn_8007A324(sndHeapSys* heap, int heapLevel, SndGroupID groupId, s32 size);
extern "C" s32 fn_8007A5EC(sndHeapSys* heap, s32 slot, SndGroupID groupId, s32 size);
extern "C" nw4r::snd::SoundMemoryAllocatable* fn_8007A314(sndHeapSys* heap, s32 slot);
extern "C" void fn_8007A78C(sndHeapSys* heap, SndGroupID groupId, s32 slot);
extern "C" void fn_80079AA4(sndLoadThread* loadThread, SndGroupID groupId, s32 slot, s32 result);

int sndSystem::loadSoundGroup(SndGroupID sndGroupId, int heapLevel, bool isRequest) {
    nw4r::snd::SoundArchive::GroupInfo groupInfo;
    s32 size;
    s32 slot;
    s32 result;

    if (!m_initFinished) {
        return -5;
    }
    m_dvdSoundArchive.detail_ReadGroupInfo(sndGroupId, &groupInfo);
    size = groupInfo.size + groupInfo.waveDataSize;
    if (size == 0) {
        return -3;
    }
    if (fn_8007AB34(m_sndHeapSys, sndGroupId) != 0) {
        return -1;
    }
    slot = fn_8007A324(m_sndHeapSys, heapLevel, sndGroupId, size);
    if (slot == 0x1C) {
        return -2;
    }
    result = fn_8007A5EC(m_sndHeapSys, slot, sndGroupId, size);
    if (result == -1) {
        return -2;
    }
    if (isRequest) {
        fn_80079AA4(m_loadThread, sndGroupId, slot, result);
    } else {
        m_archivePlayer.LoadGroup((u32)sndGroupId, fn_8007A314(m_sndHeapSys, slot), 0x10000U);
        fn_8007A78C(m_sndHeapSys, sndGroupId, slot);
    }
    return result;
}

int sndSystem::getBGMId() {
    if (!m_loadFinished) {
        return -1;
    }
    if (!m_bgmHandle.IsAttachedSound()) {
        m_bgmId = -1;
    }
    return m_bgmId;
}

bool sndSystem::setupBuffers() {
    u32 playerSize = m_archivePlayer.GetRequiredMemSize(&m_dvdSoundArchive);
    u32 managerSize = m_3DManager.GetRequiredMemSize(&m_dvdSoundArchive);
    m_playerBuffer = gfHeapManager::alloc(Heaps::Sound, playerSize);
    m_strmBuffer = gfHeapManager::alloc(Heaps::Sound, 0xE6000);
    m_3DBuffer = gfHeapManager::alloc(Heaps::Sound, managerSize);
    if (!m_archivePlayer.Setup(&m_dvdSoundArchive, m_playerBuffer, playerSize, m_strmBuffer, 0xE6000)) {
        gfErrorCheck::check();
        return false;
    }
    if (!m_3DManager.Setup(&m_dvdSoundArchive, m_3DBuffer, managerSize)) {
        gfErrorCheck::check();
        return false;
    }
    m_3DManager.SetMaxPriorityReduction(0x20);
    m_3DManager.AddListener(&m_3DListener);
    return true;
}

bool sndSystem::loadArchiveHeader() {
    u32 headerSize;
    void* header;

    if (!m_dvdSoundArchive.Open("/sound/smashbros_sound.brsar")) {
        gfErrorCheck::check();
        return false;
    }
    headerSize = m_dvdSoundArchive.GetHeaderSize();
    header = gfHeapManager::alloc(Heaps::Sound, headerSize);
    m_archiveHeader = header;
    if (!m_dvdSoundArchive.LoadHeader(header, headerSize)) {
        gfErrorCheck::check();
        return false;
    }
    return true;
}

extern "C" void fn_80076A6C(sndSystem* p1, s32 p2);
extern "C" void fn_80076860(sndSystem* p1, s32 p2);

void sndSystem::specialPause() {
    if (!m_paused) {
        fn_80076860(this, 0);
        m_specialPaused = true;
    }
    if (m_hbmState != 1) {
        m_bgmHandle.Pause(true, 0);
    }
}

void sndSystem::specialResume() {
    if (m_hbmState != 1) {
        m_bgmHandle.Pause(false, 0);
    }
    if (m_specialPaused) {
        fn_80076A6C(this, 0);
        m_specialPaused = false;
    }
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80497F00[0x5d00] __attribute__((aligned(8))) = { 0 };

// Sound-system globals (sbss 0x805A01D0): the singleton and the SE handle id sequence.
sndSystem* g_sndSystem;
s32 lbl_805A01D4;

// HYPOTHESIS: .rodata tables of this unit (0x80407718 - 0x80407880), typed from the code that reads them.
// lbl_80407718: SetOutputLine bit of Wii Remote 0-3 (playSERem).
extern const u32 lbl_80407718[4];
const u32 lbl_80407718[4] = { 2, 4, 8, 0x10 };

// lbl_80407728: [low, high] sound-id range of the voices for sound group (category) 1..0x26, 8 bytes each (isVoice).
extern const u32 lbl_80407728[76];
const u32 lbl_80407728[76] = {
    0x179, 0x1AB, 0x229, 0x252, 0x2AA, 0x2D6, 0x344, 0x372, 0x373, 0x3A7, 0x40F, 0x44A, 0x44B, 0x46C, 0x46D,
    0x499, 0x49A, 0x4C5, 0x4C6, 0x4F1, 0xCA9, 0xCD6, 0x4F2, 0x521, 0x522, 0x570, 0xE16, 0xE41, 0x571, 0x5BF,
    0x5C0, 0x5EF, 0x5F0, 0x617, 0x640, 0x66A, 0x696, 0x6BC, 0x6BD, 0x6DE, 0x6DF, 0x72D, 0x72E, 0x75D, 0x75E,
    0x795, 0x7C2, 0x810, 0x811, 0x83E, 0x83F, 0x865, 0x866, 0x879, 0x87A, 0x8A9, 0x8AA, 0x8D4, 0x8D5, 0x906,
    0x907, 0x955, 0x956, 0x988, 0x989, 0x9AF, 0x9B0, 0x9DD, 0x9DE, 0xA05, 0xA06, 0xA33, 0xA34, 0xA62, 0xA63,
    0xA8D,
};

// lbl_80407858: sound ids (33, zero padded) that isVoice accepts for the groups 0x29 and 0x2A - 0x4B (read in threes).
extern const u32 lbl_80407858[34];
const u32 lbl_80407858[34] = {
    0x1AA9, 0x1AAA, 0x1AB5, 0x1AC5, 0x1AC6, 0x1ACD, 0x1AD1, 0x1AD2, 0x1AD5, 0x1ADD, 0x1ADE, 0x1ADF,
    0x1AE1, 0x1AED, 0x1AF5, 0x1AFD, 0x1B02, 0x1B03, 0x1B04, 0x1B05, 0x1B09, 0x1B15, 0x1B19, 0x1B1A,
    0x1B21, 0x1B29, 0x1B35, 0x1B36, 0x1B39, 0x1B3A, 0x1B41, 0x1B42, 0x1B45, 0,
};
