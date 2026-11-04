#include <types.h>
#include <snd/snd_system.h>
#include <snd/snd_init_thread.h>
#include <snd/snd_load_thread.h>
#include <gf/gf_camera.h>
#include <gf/gf_error_check.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/math/math_types.h>
#include <nw4r/snd/snd_AxManager.h>
#include <nw4r/snd/snd_RemoteSpeaker.h>
#include <nw4r/snd/snd_RemoteSpeakerManager.h>
#include <revolution/AI.h>
#include <revolution/AX.h>
#include <revolution/OS.h>
#include <revolution/WPAD.h>

// The units here are written in address order: the .sdata2 float pool and the weak functions follow the order of
// first use in the text.

namespace nw4r {
namespace snd {
// HYPOTHESIS: the two entry points of nw4r::snd::SoundSystem this file uses (header not on the include path).
class SoundSystem {
public:
    static void InitSoundSystem(s32 soundThreadPrio, s32 dvdThreadPriority);
    static void ShutdownSoundSystem();
};
} // namespace snd
} // namespace nw4r

// HYPOTHESIS: RVL SDK entry points that the include tree does not declare yet (AI / AX).
extern "C" {
BOOL AICheckInit(void);
void AXInitSpecifyMem(u32 num, void* mem);
void AXQuit(void);
}

// HYPOTHESIS: sndHeapSys / sndLoadThread / sndBgmRateSystem members that are still unnamed in the symbols.
extern "C" bool fn_8007AB34(sndHeapSys* heap, SndGroupID groupId);
extern "C" s32 fn_8007A324(sndHeapSys* heap, int heapLevel, SndGroupID groupId, s32 size);
extern "C" s32 fn_8007A5EC(sndHeapSys* heap, s32 slot, SndGroupID groupId, s32 size);
extern "C" nw4r::snd::SoundMemoryAllocatable* fn_8007A314(sndHeapSys* heap, s32 slot);
extern "C" void fn_8007A78C(sndHeapSys* heap, SndGroupID groupId, s32 slot);
extern "C" void fn_8007A79C(sndHeapSys* heap);
extern "C" void fn_8007AA48(sndHeapSys* heap, int heapLevel);
extern "C" void fn_80079AA4(sndLoadThread* loadThread, SndGroupID groupId, s32 slot, s32 result);
extern "C" void fn_80078648(void* bgmRateSystem);
extern void* lbl_805A01D8;

// Zero-initialized AX work memory (defined at the end of the file).
extern unsigned char lbl_80497F00[0x5d00];

// Creates the one sound manager (0xBD0 bytes in the sound heap) the first time it is asked for.
sndSystem* sndSystem::create() {
    if (g_sndSystem == NULL) {
        g_sndSystem = new (Heaps::Sound) sndSystem();
    }
    return g_sndSystem;
}

sndSystem::sndSystem() : m_initFinished(false), m_loadFinished(false) {
    if (!AICheckInit()) {
        // The audio interface was not initialised yet: it gets its stack from the sound heap.
        AIInit((u8*)gfHeapManager::alloc(Heaps::Sound, 0x1000) + 0x1000);
    }
    AXInitSpecifyMem(0x20, lbl_80497F00);
    nw4r::snd::SoundSystem::InitSoundSystem(4, 3);
    if (nw4r::snd::detail::AxManager::GetInstance().GetOutputMode() == nw4r::snd::OUTPUT_MODE_DPL2) {
        // Dolby Pro Logic II output is not used: fall back to plain stereo.
        nw4r::snd::detail::AxManager::GetInstance().SetOutputMode(nw4r::snd::OUTPUT_MODE_STEREO);
    }

    m_bgmVolume = 1.0f;
    m_bgmDuck = 1.0f;
    m_bgmFade = 1.0f;
    unk70C = 1.0f;
    m_bgmBalance = 1.0f;
    m_seBalance = 1.0f;
    m_seqSpeed = 1.0f;
    m_bgmEnable = true;
    m_seEnable = true;
    for (int i = 0; i < 32; i++) {
        m_seHandle[i].m_id = -1;
        m_seHandle[i].m_tag = -1;
        m_seHandle[i].m_sndId = -1;
        m_seHandle[i].m_kind = 0;
        m_seHandle[i].m_priority = 0x40;
        m_seHandle[i].m_curPriority = 0;
        m_seHandle[i].m_count = 0;
        m_seHandle[i].m_index = i;
    }
    // BYTES: the three zeroing stores are only emitted in this order (descending offsets) by the compiler.
    unk6DC = 0;
    unk6D8 = 0;
    m_seSearchIdx = 0;
    m_bgmId = -1;

    // Reverb and delay of the main output (aux bus 0): music and effects share them.
    nw4r::snd::FxReverbHi::ReverbHiParam reverbParam = {0.02f, 3.0f, 0.6f, 0.4f, 0.1f, 0.5f};
    nw4r::snd::FxDelay::DelayParam delayParam = {320.0f, 0.4f, 1.0f};
    m_effectFlags = 0;
    m_effectVolume = 0.0f;
    m_reverb[0].SetParam(reverbParam);
    u32 reverbSize = m_reverb[0].GetRequiredMemSize();
    m_reverbType = 0;
    m_delay[0].SetParam(delayParam);
    u32 delaySize = m_delay[0].GetRequiredMemSize();
    m_reverbBuffer[0] = gfHeapManager::alloc(Heaps::Sound, reverbSize);
    m_delayBuffer[0] = gfHeapManager::alloc(Heaps::Sound, delaySize);
    m_reverb[0].AssignWorkBuffer(m_reverbBuffer[0], reverbSize);
    m_delay[0].AssignWorkBuffer(m_delayBuffer[0], delaySize);

    setReverbPreDelay(20);
    setDelayTime(160.0f);

    // Reverb and delay of the second and third aux bus.
    nw4r::snd::FxReverbHi::ReverbHiParam reverbParam1 = {0.0f, 1.5f, 0.6f, 0.4f, 0.1f, 1.0f};
    nw4r::snd::FxDelay::DelayParam delayParam1 = {200.0f, 0.3f, 1.0f};
    m_reverb[1].SetParam(reverbParam1);
    reverbSize = m_reverb[1].GetRequiredMemSize();
    m_delay[1].SetParam(delayParam1);
    delaySize = m_delay[1].GetRequiredMemSize();
    m_reverbBuffer[1] = gfHeapManager::alloc(Heaps::Sound, reverbSize);
    m_delayBuffer[1] = gfHeapManager::alloc(Heaps::Sound, delaySize);
    m_reverb[1].AssignWorkBuffer(m_reverbBuffer[1], reverbSize);
    m_delay[1].AssignWorkBuffer(m_delayBuffer[1], delaySize);
    nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_B, &m_reverb[1]);
    nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_C, &m_delay[1]);

    m_archiveHeader = NULL;
    // The init thread opens the archive and sets up the buffers (loadArchiveHeader / setupBuffers); the load thread
    // streams the sound groups. Neither runs before startLoadThread / update resume them.
    m_initThread = new (Heaps::Sound) sndInitThread();
    m_initThread->setPriority(0xF);
    m_loadThread = new (Heaps::Sound) sndLoadThread();
    m_loadThread->setPriority(0xF);
    init();
    m_sndHeapSys = new (Heaps::Sound) sndHeapSys();
}

// TEMPORARY (four-way split of the sndSystem text): the constructor expands setReverbPreDelay / setDelayTime, whose
// real (out-of-line) definitions live in snd_system_d.cpp. They are repeated here as inline copies so that the
// expansion matches and no second strong symbol is emitted; delete both when the four units are merged back.
inline void sndSystem::setReverbPreDelay(int ms) {
    if (m_loadFinished) {
        nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
        param.preDelayTime = ms / 1000.0f;
        m_reverb[0].SetParam(param);
    }
}

inline void sndSystem::setDelayTime(float delay) {
    if (m_loadFinished) {
        nw4r::snd::FxDelay::DelayParam param = m_delay[0].GetParam();
        param.delay = delay;
        m_delay[0].SetParam(param);
    }
}

void sndSystem::startLoadThread() {
    m_initThread->resume();
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

// HYPOTHESIS: names from the map. The HOME Button Menu runs its own sound: the reverb and delay of the main output
// are saved and cleared from all three aux buses while it is up (startHBMState) and restored afterwards (endHBMState).
void sndSystem::startHBMState() {
    if (m_loadFinished) {
        const nw4r::snd::FxReverbHi::ReverbHiParam& reverb = m_reverb[0].GetParam();
        m_hbmReverbParam.preDelayTime = reverb.preDelayTime;
        m_hbmReverbParam.fusedTime = reverb.fusedTime;
        m_hbmReverbParam.coloration = reverb.coloration;
        m_hbmReverbParam.damping = reverb.damping;
        m_hbmReverbParam.crossTalk = reverb.crossTalk;
        m_hbmReverbParam.outGain = reverb.outGain;
        const nw4r::snd::FxDelay::DelayParam& delay = m_delay[0].GetParam();
        m_hbmDelayParam.delay = delay.delay;
        m_hbmDelayParam.feedback = delay.feedback;
        m_hbmDelayParam.outGain = delay.outGain;
        nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_A, 0xFA);
        nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_B, 0xFA);
        nw4r::snd::detail::AxManager::GetInstance().ClearEffect(nw4r::snd::AUX_C, 0xFA);
    }
}

void sndSystem::endHBMState() {
    if (m_loadFinished) {
        m_reverb[0].SetParam(m_hbmReverbParam);
        m_delay[0].SetParam(m_hbmDelayParam);
        nw4r::snd::FxReverbHi::ReverbHiParam reverbParam1 = {0.0f, 1.5f, 0.6f, 0.4f, 0.1f, 1.0f};
        nw4r::snd::FxDelay::DelayParam delayParam1 = {200.0f, 0.3f, 1.0f};
        m_reverb[1].SetParam(reverbParam1);
        m_delay[1].SetParam(delayParam1);
        nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_B, &m_reverb[1]);
        nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_C, &m_delay[1]);
        if (m_effectFlags & 1) {
            nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_A, &m_reverb[0]);
        }
        if (m_effectFlags & 2) {
            nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_A, &m_delay[0]);
        }
    }
}

sndSystem::~sndSystem() {
    delete m_initThread;
    delete m_loadThread;
    delete m_sndHeapSys;
    m_archivePlayer.Shutdown();
    m_dvdSoundArchive.Close();
    gfHeapManager::free(m_archiveHeader);
    gfHeapManager::free(m_playerBuffer);
    gfHeapManager::free(m_strmBuffer);
    gfHeapManager::free(m_3DBuffer);
    for (int i = 0; i < 2; i++) {
        gfHeapManager::free(m_reverbBuffer[i]);
        gfHeapManager::free(m_delayBuffer[i]);
    }
    nw4r::snd::SoundSystem::ShutdownSoundSystem();
    AXQuit();
}

// Resets what the constructor does not: the listener looks along the default direction from the origin, the Wii
// Remote speakers and the voice queue are cleared.
void sndSystem::init() {
    m_paused = false;
    m_specialPaused = false;
    m_hbmState = 0;
    m_3DListener.SetInteriorSize(50.0f);
    m_3DListener.SetMaxVolumeDistance(60.0f);
    m_3DListener.SetUnitDistance(150.0f);

    nw4r::math::VEC3 eye(0.0f, 0.0f, 0.0f);
    nw4r::math::VEC3 up(0.0f, 1.0f, 0.0f);
    nw4r::math::VEC3 direction(nw4r::math::SinFIdx(0.0f), 0.0f, -nw4r::math::CosFIdx(0.0f));
    nw4r::math::VEC3 target = eye + direction;
    nw4r::math::MTX34 matrix;
    nw4r::math::MTX34LookAt(&matrix, &eye, &up, &target);
    m_3DListener.SetMatrix(matrix);

    unk6E8 = 0;
    for (int i = 0; i < 4; i++) {
        m_remoteState[i] = 0;
    }
    m_curSeKind = 0;
    m_curSeId = -1;
    for (int i = 0; i < 6; i++) {
        m_seQueue[i] = -1;
    }
}

void sndSystem::update() {
    if (!m_loadFinished) {
        // Still starting up: wait for the init thread (archive header and buffers), then for the common groups.
        if (!m_initFinished && m_initThread != NULL && m_initThread->isFinishInitialize()) {
            m_initFinished = true;
        }
        if (m_initThread != NULL && m_initThread->isFinishLoad()) {
            m_loadFinished = true;
            m_loadThread->resume();
        }
        return;
    }

    // Wii Remote speakers: once a Remote is connected its speaker is set up (state 1), and is considered playing
    // (state 2) when the speaker reports it is available; losing the Remote or the speaker drops back to idle (0).
    // HYPOTHESIS: the meaning of the three states.
    for (int i = 0; i < 4; i++) {
        u32 type;
        s32 probe = WPADProbe(i, &type);
        switch (m_remoteState[i]) {
        case 0:
            if (probe == 0) {
                nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(i).Setup(NULL);
                m_remoteState[i] = 1;
            }
            break;
        case 1:
            if (probe != 0 && probe != -2) {
                m_remoteState[i] = 0;
            } else if (nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(i).IsAvailable()) {
                m_remoteState[i] = 2;
            }
            break;
        case 2:
            if (!nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(i).IsAvailable() ||
                probe != 0) {
                m_remoteState[i] = 0;
            }
            break;
        }
    }

    // The sound listener follows the game camera: position, viewing direction and up vector are copied out of
    // camera 0 and turned into the listener matrix.
    // BYTES: the intermediate Vec3f copies (declared pos, up, dir) are what the original stack frame shows; building
    // the nw4r vectors straight from the camera loads every component twice.
    gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
    Vec3f posCopy;
    Vec3f upCopy;
    Vec3f dirCopy;
    posCopy = camera->m_centerPos;
    upCopy = camera->m_up;
    dirCopy = camera->m_direction;
    nw4r::math::VEC3 eye(posCopy.m_x, posCopy.m_y, posCopy.m_z);
    nw4r::math::VEC3 upVec(upCopy.m_x, upCopy.m_y, upCopy.m_z);
    nw4r::math::VEC3 dir(dirCopy.m_x, dirCopy.m_y, dirCopy.m_z);
    nw4r::math::VEC3 target = eye + dir;
    nw4r::math::MTX34 matrix;
    nw4r::math::MTX34LookAt(&matrix, &eye, &upVec, &target);
    m_3DListener.SetMatrix(matrix);
    m_archivePlayer.Update();
    fn_80078648(lbl_805A01D8);

    // Priority of every SE slot: the live priority of its sound plus the priority it was started with.
    for (int i = 0; i < 32; i++) {
        nw4r::snd::detail::BasicSound* sound = m_seHandle[i].m_handle.detail_GetAttachedSound();
        nw4r::snd::SoundParam* param = sound != NULL ? &sound->GetAmbientParam() : NULL;
        if (param != NULL) {
            m_seHandle[i].m_curPriority = param->priority + m_seHandle[i].m_priority;
        } else {
            m_seHandle[i].m_curPriority = 0;
        }
    }
}

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

void sndSystem::freeGroup(int heapLevel, int slot) {
    if (heapLevel >= 0 && m_sndHeapSys != NULL) {
        fn_8007A79C(m_sndHeapSys);
        m_loadThread->addFreeGroup(heapLevel, slot);
    }
}

// HYPOTHESIS: the three tail calls ask the load thread whether its queue is empty / the group is loaded / the frees
// are done.
bool sndSystem::isFinishLoad() {
    return m_loadThread->isFinish();
}

bool sndSystem::isFinishLoad1(u32 groupId) {
    return m_loadThread->isFinish((SndGroupID)groupId);
}

bool sndSystem::isFinishFree() {
    return m_loadThread->isFinishFree();
}

bool sndSystem::isSoundGroupLoaded(SndGroupID sndGroupId) {
    return fn_8007AB34(m_sndHeapSys, sndGroupId);
}

void sndSystem::loadCommon() {
}

// Run by the load thread.
void sndSystem::loadGroupThread(SndGroupID sndGroupId, int slot) {
    m_archivePlayer.LoadGroup((u32)sndGroupId, fn_8007A314(m_sndHeapSys, slot), 0x10000U);
    fn_8007A78C(m_sndHeapSys, sndGroupId, slot);
}

void sndSystem::freeGroupThread(int heapLevel, int) {
    if (heapLevel >= 0 && m_sndHeapSys != NULL) {
        fn_8007AA48(m_sndHeapSys, heapLevel);
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
