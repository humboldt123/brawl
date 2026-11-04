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
#include <nw4r/math/math_arithmetic.h>
#include <ogws/nw4r/snd/snd_SeqSoundHandle.h>

// ---- 0x80072968-0x80073DB0 ----

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

// ---- 0x80073DB0-0x800754E8 ----

// HYPOTHESIS: nw4r::snd::detail::BasicSound::GetRemainingFadeFrames (map name at 0x801BD2C8): the frames of the
// current fade (MoveValue at +0x44) that are still to run. Declared as a stand-in until nw4r names it.
extern "C" int fn_801BD2C8(nw4r::snd::detail::BasicSound* sound);

// HYPOTHESIS: inline used by playBGM / prepareBGM; asks the fade state of the sound in the BGM handle.
static inline int getRemainingFadeFrames(nw4r::snd::SoundHandle& handle) {
    if (handle.IsAttachedSound()) {
        return fn_801BD2C8(handle.detail_GetAttachedSound());
    }
    return 0;
}

// Stop/FadeIn through the BasicSound vtable (nw4r::snd::SoundHandle has no FadeIn).
static inline void fadeInHandle(nw4r::snd::SoundHandle& handle, int frames) {
    if (handle.IsAttachedSound()) {
        handle.detail_GetAttachedSound()->FadeIn(frames);
    }
}

// HYPOTHESIS: ducks the music while a queued voice (user param 1 / 2) plays: kind 1 (the loudest voices) mutes it,
// kind 2 lowers it to 0.2. The new volume reaches the music over `frames`.
static inline void duckBGM(sndSystem* sys, u32 kind, int frames) {
    if (sys->m_loadFinished) {
        if (kind == 1) {
            sys->m_bgmDuck = 0.0f;
        } else {
            sys->m_bgmDuck = 0.2f;
        }
        if (sys->m_bgmHandle.IsAttachedSound()) {
            sys->m_bgmHandle.SetVolume(sys->m_bgmVolume * sys->m_bgmFade * sys->m_bgmDuck * sys->m_bgmBalance, frames);
        }
    }
}

// Starts the music with a fade-in of `fade` frames; `interrupt` cuts the music that is still playing. The id is stored
// only when it started.
bool sndSystem::playBGM(int bgmId, u32 fade, bool interrupt) {
    if (!m_loadFinished) {
        return false;
    }
    if (bgmId == m_bgmId) {
        return true;
    }
    if (m_bgmHandle.IsAttachedSound() && getRemainingFadeFrames(m_bgmHandle)) {
        m_bgmHandle.Stop(0);
        m_bgmId = -1;
    }
    if (m_bgmId != -1 && interrupt && m_loadFinished && m_bgmEnable) {
        m_bgmHandle.Stop(0);
        m_bgmId = -1;
    }
    m_bgmId = -1;
    // BYTES: unsigned test against -1 (addis/cmplwi form), the music id behaves like a SndID.
    if ((u32)bgmId == (u32)-1) {
        return false;
    }
    if (!m_bgmEnable) {
        return false;
    }
    // BYTES: the original tests the StartResult itself (cmpwi r3,0); the bool wrapper StartSound compiles to cntlzw.
    // detail_StartSound is public through the shadow include/nw4r/snd/snd_SoundStartable.h.
    if (m_archivePlayer.detail_StartSound(&m_bgmHandle, bgmId, NULL, NULL, NULL) ==
        nw4r::snd::SoundStartable::START_SUCCESS) {
        fadeInHandle(m_bgmHandle, fade);
        m_bgmHandle.SetVolume(m_bgmVolume * m_bgmFade * m_bgmBalance * m_bgmDuck, 0);
        m_bgmId = bgmId;
        return true;
    }
    return false;
}

void sndSystem::stopBGM(int fade) {
    if (m_loadFinished && m_bgmEnable) {
        m_bgmHandle.Stop(fade);
        m_bgmId = -1;
    }
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

// Loads the music into the BGM handle without starting it (startBGM does); `unk` cuts the old music like playBGM's flag.
bool sndSystem::prepareBGM(int bgmId, int unk) {
    if (!m_loadFinished) {
        return false;
    }
    if (bgmId == m_bgmId) {
        return true;
    }
    if (m_bgmHandle.IsAttachedSound() && getRemainingFadeFrames(m_bgmHandle)) {
        m_bgmHandle.Stop(0);
        m_bgmId = -1;
    }
    if (m_bgmId != -1 && unk != 0 && m_loadFinished && m_bgmEnable) {
        m_bgmHandle.Stop(0);
        m_bgmId = -1;
    }
    m_bgmId = -1;
    if (!m_bgmEnable) {
        return false;
    }
    if ((u32)bgmId == (u32)-1) {
        return false;
    }
    if (m_archivePlayer.detail_PrepareSound(&m_bgmHandle, bgmId, NULL, NULL, NULL) ==
        nw4r::snd::SoundStartable::START_SUCCESS) {
        m_bgmId = bgmId;
        return true;
    }
    return false;
}

bool sndSystem::isPreparedBGM(int bgmId) {
    if (!m_bgmEnable) {
        return true;
    }
    bool prepared = false;
    if (m_bgmHandle.IsAttachedSound()) {
        if (m_bgmHandle.detail_GetAttachedSound()->IsPrepared()) {
            prepared = true;
        }
    }
    return prepared;
}

// Starts the prepared music, fading in over `startFrame` frames.
void sndSystem::startBGM(int bgmId, int startFrame) {
    if (m_loadFinished && m_bgmEnable) {
        m_bgmHandle.StartPrepared();
        fadeInHandle(m_bgmHandle, startFrame);
        m_bgmHandle.SetVolume(m_bgmVolume * m_bgmFade * m_bgmBalance * m_bgmDuck, 0);
    }
}

// Plays a sound effect; returns the handle id of the slot (-1: not started). Voices (user param 1 / 2) are queued:
// only one plays at a time, it ducks the music, the others wait in m_seQueue.
s32 sndSystem::playSE(SndID sndID, int tag, int fadeIn, int unique, int group) {
    if (!m_loadFinished) {
        return -1;
    }
    // BYTES: the id is tested as an unsigned value (addis/cmplwi form); SndID is a signed enum in the header.
    if ((u32)sndID == (u32)-1) {
        return -1;
    }
    if (!m_seEnable) {
        return -1;
    }
    if (!isSeLoaded(sndID, group)) {
        return -1;
    }
    u32 kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
    if (kind == 0xB) {
        unique = 1;
    }
    u32 slot;
    if (kind == 1 || kind == 2) {
        // HYPOTHESIS: a queued voice never carries an owner tag (stopGroup must not stop it); BYTES: the original
        // tests the tag before clearing it.
        if (tag != -1) {
            tag = -1;
        }
        slot = findSE(sndID);
        if (slot != -1) {
            if (m_seHandle[slot].m_kind == 1) {
                freeSE(slot);
            } else {
                m_seHandle[slot].m_count++;
                return m_seHandle[slot].m_id;
            }
        } else {
            slot = allocSE();
        }
    } else if (unique) {
        slot = findSE(sndID);
        if (slot == -1 || kind == 1 || kind == 2) {
            slot = allocSE();
        } else {
            freeSE(slot);
        }
    } else {
        slot = allocSE();
    }
    if (slot == -1) {
        // No free slot: evict the one with the lowest live priority (not above that of this sound).
        slot = findLowestSE(sndID);
        freeSE(slot);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (kind == 1 || kind == 2) {
            int duckFrames = 0;
            if (kind == 1) {
                duckFrames = 0x2D;
                if ((u32)sndID == 0x1B4C) {
                    duckFrames = 6;
                }
            }
            // HYPOTHESIS: the original calls getPlayQueIndex with the id still stored in the (just freed) slot and drops
            // the result, probably meant to be sndID; the call itself is in the bytes.
            getPlayQueIndex((SndID)m_seHandle[slot].m_sndId, kind);
            m_seHandle[slot].m_count++;
            if (m_curSeKind == 0) {
                duckBGM(this, kind, duckFrames);
                m_curSeKind = kind;
                m_curSeId = m_seHandle[slot].m_id;
            } else if ((u32)m_curSeKind < kind) {
                // A louder voice is playing: wait in the queue.
                u32 q = getPlayQueIndex(sndID, kind);
                if (q == -1) {
                    m_seHandle[slot].m_count--;
                } else {
                    m_seQueue[q] = m_seHandle[slot].m_id;
                    m_seHandle[slot].m_sndId = sndID;
                    m_seHandle[slot].m_kind = kind;
                    m_seHandle[slot].m_tag = tag;
                    int priority = getSEPriority(sndID);
                    m_seHandle[slot].m_priority = priority;
                    m_seHandle[slot].m_curPriority = priority;
                    return m_seHandle[slot].m_id;
                }
            } else {
                // This voice takes over: the one playing goes back to its queue place.
                u32 cur = findSEId(m_curSeId);
                if (cur != -1) {
                    m_seHandle[cur].m_handle.Stop(duckFrames);
                    u32 q = getPlayQueIndex((SndID)m_seHandle[cur].m_sndId, m_curSeKind);
                    if (q != -1) {
                        m_seQueue[q] = m_curSeId;
                    }
                }
                m_curSeKind = kind;
                m_curSeId = m_seHandle[slot].m_id;
                if (kind == 1) {
                    duckBGM(this, kind, duckFrames);
                }
            }
        }
        if (m_archivePlayer.detail_StartSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            fadeInHandle(m_seHandle[slot].m_handle, fadeIn);
            m_seHandle[slot].m_handle.SetVolume(unk70C * m_seBalance, 0);
            m_seHandle[slot].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = kind;
            m_seHandle[slot].m_tag = tag;
            int priority = getSEPriority(sndID);
            m_seHandle[slot].m_priority = priority;
            m_seHandle[slot].m_curPriority = priority;
            if (kind == 1) {
                m_seqSpeed = 1.0f;
            }
            return m_seHandle[slot].m_id;
        }
        m_seHandle[slot].m_count = 0;
    }
    return -1;
}

bool sndSystem::isPlaySEID(SndID sndID) {
    for (int i = 0; i < 32; i++) {
        if (m_seHandle[i].m_sndId == sndID && m_seHandle[i].isUsed()) {
            return true;
        }
    }
    return false;
}

// Only the layout of the early return differs (the original returns -1 before the tail call).
s32 sndSystem::playSEwithFixEffect(SndID sndID, float volume, float pitch) {
    if (!m_loadFinished) {
        return -1;
    }
    return playSE(sndID, -1, 0, 0, -1);
}

// playSE on the SoundStartable of a Sound3DActor (the startable starts the sound with the actor's 3D parameters).
// Same slot handling as playSE (flag = unique), without the queue of voices.
s32 sndSystem::play3DSE(SndID sndID, nw4r::snd::SoundStartable* startable, int tag, int fadeIn, int flag, int groupId) {
    if (!m_loadFinished) {
        return -1;
    }
    // BYTES: unsigned test, as in playSE.
    if ((u32)sndID == (u32)-1) {
        return -1;
    }
    if (!m_seEnable) {
        return -1;
    }
    if (!isSeLoaded(sndID, groupId)) {
        return -1;
    }
    u32 kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
    if (kind == 0xB) {
        flag = 1;
    }
    u32 slot;
    if (flag) {
        slot = findSE(sndID);
        if (slot == -1) {
            slot = allocSE();
        } else {
            freeSE(slot);
        }
    } else {
        slot = allocSE();
    }
    if (slot == -1) {
        slot = findLowestSE(sndID);
        freeSE(slot);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (startable->detail_StartSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            fadeInHandle(m_seHandle[slot].m_handle, fadeIn);
            m_seHandle[slot].m_handle.SetVolume(unk70C * m_seBalance, 0);
            m_seHandle[slot].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = kind;
            m_seHandle[slot].m_tag = tag;
            int priority = getSEPriority(sndID);
            m_seHandle[slot].m_priority = priority;
            m_seHandle[slot].m_curPriority = priority;
            return m_seHandle[slot].m_id;
        }
    }
    return -1;
}

// ---- 0x800754E8-0x80076C78 ----

// The slot helpers shared with playSE & co (findSEId, allocSE, freeSE, findLowestSE, getSEPriority) are inline members
// declared at the end of snd/snd_system.h. Handle ids of the sound effects are the id handed out by allocSE; a slot
// index is `id & 0xFF`.

// Forgets everything about the sound in the slot (the handle itself is not touched).
static inline void seResetSlot(sndSystem::sndHandle* h) {
    h->m_id = -1;
    h->m_tag = -1;
    h->m_sndId = -1;
    h->m_kind = 0;
    h->m_priority = 0x40;
    h->m_curPriority = 0;
    h->m_count = 0;
}

// Sets the fade-in of a playing sound effect (SoundHandle has no FadeIn of its own).
static inline void seFadeIn(nw4r::snd::SoundHandle& handle, int frames) {
    if (handle.IsAttachedSound()) {
        handle.detail_GetAttachedSound()->FadeIn(frames);
    }
}

s32 sndSystem::prepareSE(SndID sndID, s32 tag, s32 groupId) {
    if ((u32)sndID == -1) {
        return -1;
    }
    if (!m_seEnable) {
        return -1;
    }
    if (!isSeLoaded(sndID, groupId)) {
        return -1;
    }
    u32 slot = allocSE();
    if (slot == -1) {
        // No free slot: the quietest sound that is not a queued voice and not louder than this one makes room.
        slot = findLowestSE(sndID);
        freeSE(slot);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (m_archivePlayer.detail_PrepareSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
            m_seHandle[slot].m_tag = tag;
            int priority = getSEPriority(sndID);
            m_seHandle[slot].m_priority = priority;
            m_seHandle[slot].m_curPriority = priority;
            return m_seHandle[slot].m_id;
        }
    }
    return -1;
}

// HYPOTHESIS (map prepare3DSE): prepareSE on the SoundStartable of a Sound3DActor.
s32 sndSystem::prepare3DSE(SndID sndID, nw4r::snd::SoundStartable* startable, int tag, int groupId) {
    if ((u32)sndID == -1) {
        return -1;
    }
    if (!m_seEnable) {
        return -1;
    }
    if (!isSeLoaded(sndID, groupId)) {
        return -1;
    }
    u32 slot = allocSE();
    if (slot == -1) {
        // No free slot: the quietest sound that is not a queued voice and not louder than this one makes room.
        slot = findLowestSE(sndID);
        freeSE(slot);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (startable->detail_PrepareSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
            m_seHandle[slot].m_tag = tag;
            int priority = getSEPriority(sndID);
            m_seHandle[slot].m_priority = priority;
            m_seHandle[slot].m_curPriority = priority;
            return m_seHandle[slot].m_id;
        }
    }
    return -1;
}

// True while the handle id is stale or the sound effects are off (nothing to wait for), else whether the prepared
// sound is ready to start.
bool sndSystem::isPreparedSE(s32 handleId) {
    u32 slot = findSEId(handleId);
    bool prepared;
    if (!m_seEnable) {
        prepared = true;
    } else if (slot == -1) {
        prepared = true;
    } else {
        sndHandle* h = &m_seHandle[slot];
        prepared = false;
        // HYPOTHESIS: SoundHandle::IsPrepared expanded as an && chain (the if/else form of the OGWS header compiles
        // differently).
        if (h->m_handle.IsAttachedSound() && h->m_handle.detail_GetAttachedSound()->IsPrepared()) {
            prepared = true;
        }
    }
    return prepared;
}

void sndSystem::startSE(s32 handleId, s32 fadeIn, s32 unique) {
    startSE1(findSEId(handleId), fadeIn, unique);
}

// Starts the prepared sound of a slot. Sounds of kind 0xB (or every sound when `unique` is set) are unique: the
// other slots playing the same sound id are freed first.
void sndSystem::startSE1(unsigned int slot, int fadeIn, int unique) {
    if (slot == -1) {
        return;
    }
    u32 sndId = m_seHandle[slot].m_sndId;
    if (sndId != -1 && m_seHandle[slot].m_kind == 0xB) {
        unique = 1;
    }
    if (unique) {
        for (u32 i = (slot + 1) & 0x1F; slot != i; i = (i + 1) & 0x1F) {
            if (sndId == m_seHandle[i].m_sndId && m_seHandle[i].isUsed()) {
                freeSE(i);
            }
        }
    }
    if (m_seEnable) {
        m_seHandle[slot].m_handle.StartPrepared();
        seFadeIn(m_seHandle[slot].m_handle, fadeIn);
        m_seHandle[slot].m_handle.SetVolume(unk70C * m_seBalance, 0);
        m_seHandle[slot].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
    }
}

void sndSystem::stopSE(s32 handleId, s32 fade) {
    stopSE1(findSEId(handleId), fade);
}

// Stops the sound of a slot. A queued voice (kind 1 / 2) is only stopped when its last reservation goes; the next
// waiting voice then starts and the music is brought back up once nothing is waiting.
void sndSystem::stopSE1(unsigned int slot, int fade) {
    if (slot == -1) {
        return;
    }
    s32 id = m_seHandle[slot].m_id;
    u32 sndId = m_seHandle[slot].m_sndId;
    if (!m_seEnable) {
        return;
    }
    u32 kind = m_seHandle[slot].m_kind;
    u32 queIdx = getPlayQueIndex((SndID)sndId, kind);
    if (kind == 1 || kind == 2) {
        m_seHandle[slot].m_count--;
        if (m_seHandle[slot].m_count != 0) {
            return;
        }
        if (queIdx != -1) {
            m_seQueue[queIdx] = -1;
        }
    }
    m_seHandle[slot].m_handle.Stop(fade);
    m_seHandle[slot].m_sndId = -1;
    m_seHandle[slot].m_id = -1;
    if (kind == 1 || kind == 2) {
        if (id != m_curSeId) {
            return;
        }
        m_curSeKind = 0;
        m_curSeId = -1;
        u32 i;
        for (i = 1; i < 6; i++) {
            if (m_seQueue[i] == -1) {
                continue;
            }
            u32 next = findSEId(m_seQueue[i]);
            if (next != -1) {
                u32 nextSndId = m_seHandle[next].m_sndId;
                m_curSeKind = 2;
                m_curSeId = m_seQueue[i];
                m_seQueue[i] = -1;
                if (m_archivePlayer.detail_StartSound(&m_seHandle[next].m_handle, nextSndId, NULL, NULL, NULL) ==
                    nw4r::snd::SoundStartable::START_SUCCESS) {
                    seFadeIn(m_seHandle[next].m_handle, fade);
                    m_seHandle[next].m_handle.SetVolume(unk70C * m_seBalance, 0);
                    m_seHandle[next].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
                }
                break;
            }
            m_seQueue[i] = -1;
        }
        if (i >= 6 && m_loadFinished) {
            m_bgmDuck = 1.0f;
            if (m_bgmHandle.IsAttachedSound()) {
                m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), fade);
            }
        }
    }
}

// HYPOTHESIS: the Final Smash sound state ends: the queued voice that was playing is cleared (its reservation is
// given back), the next waiting voice starts with a 45 frame fade-in and the music comes back up.
void sndSystem::finalOffState(int) {
    if (!m_loadFinished) {
        return;
    }
    if (m_curSeId != -1) {
        u32 cur = findSEId(m_curSeId);
        if (cur != -1) {
            m_seHandle[cur].m_count--;
            m_seHandle[cur].m_handle.Stop(0x2D);
            m_seHandle[cur].m_sndId = -1;
            m_seHandle[cur].m_id = -1;
        }
        m_curSeKind = 0;
        m_curSeId = -1;
    }
    u32 i;
    for (i = 1; i < 6; i++) {
        if (m_seQueue[i] == -1) {
            continue;
        }
        u32 next = findSEId(m_seQueue[i]);
        m_seQueue[i] = -1;
        if (next != -1) {
            u32 sndId = m_seHandle[next].m_sndId;
            m_curSeKind = m_seHandle[next].m_kind;
            m_curSeId = m_seHandle[next].m_id;
            if (m_archivePlayer.detail_StartSound(&m_seHandle[next].m_handle, sndId, NULL, NULL, NULL) ==
                nw4r::snd::SoundStartable::START_SUCCESS) {
                seFadeIn(m_seHandle[next].m_handle, 0x2D);
                m_seHandle[next].m_handle.SetVolume(unk70C * m_seBalance, 0);
                m_seHandle[next].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
                int priority = getSEPriority((SndID)sndId);
                m_seHandle[next].m_priority = priority;
                m_seHandle[next].m_curPriority = priority;
            }
            break;
        }
    }
    if (i >= 6 && m_loadFinished) {
        m_bgmDuck = 1.0f;
        if (m_bgmHandle.IsAttachedSound()) {
            m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), 0x2D);
        }
    }
}

// Stops every sound effect that was played with this tag (a fighter's or a status' sound group; the soSoundModule
// calls it when a status ends) and frees the slots. Queued voices (m_count != 0) are left alone.
void sndSystem::stopGroup(int tag, int fade) {
    if (m_loadFinished && m_seEnable) {
        for (u32 i = 0; i < 32; i++) {
            if (tag == m_seHandle[i].m_tag && m_seHandle[i].m_count == 0) {
                m_seHandle[i].m_handle.Stop(fade);
                seResetSlot(&m_seHandle[i]);
            }
        }
    }
}

// Stops the playing sounds with this tag and sound id (the slots stay reserved).
void sndSystem::stopGroupId(int tag, SndID sndID, int fade) {
    if (m_loadFinished && m_seEnable) {
        for (u32 i = 0; i < 32; i++) {
            if (tag == m_seHandle[i].m_tag && (u32)sndID == m_seHandle[i].m_sndId) {
                m_seHandle[i].m_handle.Stop(fade);
            }
        }
    }
}

// Stops everything except the sounds tagged 0x10000, clears the voice queue and optionally lifts the music duck.
void sndSystem::allStopSE(int fade, bool restoreBgm) {
    if (!m_loadFinished) {
        return;
    }
    if (m_seEnable) {
        for (u32 i = 0; i < 32; i++) {
            if (m_seHandle[i].m_tag != 0x10000) {
                m_seHandle[i].m_handle.Stop(fade);
                seResetSlot(&m_seHandle[i]);
            }
        }
        m_curSeKind = 0;
        m_curSeId = -1;
        m_seQueue[0] = -1;
        m_seQueue[1] = -1;
        m_seQueue[2] = -1;
        m_seQueue[3] = -1;
        m_seQueue[4] = -1;
        m_seQueue[5] = -1;
    }
    if (restoreBgm) {
        if (!m_loadFinished) {
            return;
        }
        m_bgmDuck = 1.0f;
        if (m_bgmHandle.IsAttachedSound()) {
            m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), fade);
        }
    } else {
        m_bgmDuck = 1.0f;
    }
}

// Clamp done with two fsel and no branches (upper bound first).
static inline float clampf(float x, float hi, float lo) {
    x = nw4r::math::FSelect(hi - x, x, hi);
    return nw4r::math::FSelect(lo - x, lo, x);
}

// Pauses the game's sound: every sound effect that is in use except the ones tagged 0x10000 is paused, and the music is
// either paused too (m_hbmState == 1, HYPOTHESIS: HOME Button Menu) or, when lowerBgm is set, ducked to half volume.
// Returns false when the sound system is not ready or is already paused.
bool sndSystem::pause(bool lowerBgm) {
    if (!m_loadFinished) {
        return false;
    }
    if (m_paused) {
        return false;
    }
    m_paused = true;
    if (m_hbmState == 1) {
        m_bgmHandle.Pause(true, 0);
    } else if (lowerBgm) {
        m_bgmVolume = clampf(m_bgmVolume, 1.0f, 0.0f);
        m_bgmFade = 0.5f;
        m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), 0);
    }
    for (u32 i = 0; i < 32; i++) {
        if (m_seHandle[i].m_tag != 0x10000 && m_seHandle[i].isUsed()) {
            m_seHandle[i].m_handle.Pause(true, 0);
        }
    }
    return true;
}

// Undoes pause(): sound effects resume and the music comes back to full volume (or is un-paused).
bool sndSystem::resume(bool lowerBgm) {
    if (!m_loadFinished) {
        return false;
    }
    if (!m_paused) {
        return false;
    }
    m_paused = false;
    if (m_hbmState == 1) {
        m_bgmHandle.Pause(false, 0);
    } else if (lowerBgm) {
        m_bgmVolume = clampf(m_bgmVolume, 1.0f, 0.0f);
        m_bgmFade = 1.0f;
        m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), 0);
    }
    for (u32 i = 0; i < 32; i++) {
        if (m_seHandle[i].m_tag != 0x10000 && m_seHandle[i].isUsed()) {
            m_seHandle[i].m_handle.Pause(false, 0);
        }
    }
    return true;
}

// Pause used while a Final Smash cut-in / similar freeze runs (HYPOTHESIS): pauses everything including the music,
// remembering in m_specialPaused whether it was the one that paused the game.
void sndSystem::specialPause() {
    if (!m_paused) {
        pause(false);
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
        resume(false);
        m_specialPaused = false;
    }
}

// ---- 0x80076C78-0x800783EC ----

// Tables defined by snd_system.cpp (cluster A).
extern const u32 lbl_80407718[4];
extern const u32 lbl_80407728[76];
extern const u32 lbl_80407858[34];


// Looks up a handle id given out by playSE & co: the low byte is the slot, the whole id has to match and the slot
// has to be in use. -1 when the id is stale or -1.
static inline u32 findSlot(sndSystem* sys, s32 handleId) {
    if (handleId == -1) {
        return -1;
    }
    bool found = false;
    u32 idx = handleId & 0xFF;
    sndSystem::sndHandle& slot = sys->m_seHandle[idx];
    if (slot.m_id == handleId) {
        if (slot.isUsed()) {
            found = true;
        }
    }
    return found ? idx : -1;
}

// Leaves the pause state set by pause(): music and effects back to full level. In the HOME Button Menu state the
// music was only paused by it, so it is stopped here.
void sndSystem::resumeClear() {
    m_paused = false;
    m_bgmFade = 1.0f;
    m_bgmVolume = 1.0f;
    if (m_hbmState == 1 && m_loadFinished && m_bgmEnable) {
        m_bgmHandle.Stop(0);
        m_bgmId = -1;
    }
}

void sndSystem::setFinalSeqSpeed(float speed) {
    // BYTES: m_curSeKind is compared unsigned (cmplwi); the header declares it s32.
    if ((u32)m_curSeKind == 1 && m_seqSpeed != speed) {
        u32 idx = findSlot(this, m_curSeId);
        if (idx != -1 && m_seEnable) {
            nw4r::snd::SeqSoundHandle seqHandle(&m_seHandle[idx].m_handle);
            seqHandle.SetTempoRatio(speed);
        }
        m_seqSpeed = speed;
    }
}

void sndSystem::setSeqSpeed(s32 handleId, float speed) {
    u32 idx = findSlot(this, handleId);
    if (idx != -1 && m_seEnable) {
        nw4r::snd::SeqSoundHandle seqHandle(&m_seHandle[idx].m_handle);
        seqHandle.SetTempoRatio(speed);
    }
}

// Sets how loud the music and the effects are relative to each other (the pair is normalised so that an equal
// pair gives 1.0 for both), then reapplies the levels to the music and to every sound effect slot.
void sndSystem::setVolBalance(float bgm, float se) {
    float total = bgm + se;
    m_bgmBalance = 2.0f * (bgm / total);
    m_seBalance = 2.0f * (se / total);
    if (m_bgmBalance > 1.0f) {
        m_bgmBalance = 1.0f;
    }
    m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmVolume * m_bgmFade), 0);
    m_bgmDuck = 1.0f;
    if (m_seBalance > 1.0f) {
        m_seBalance = 1.0f;
    }
    unk70C = clampf(unk70C, 1.0f, 0.0f);
    float seVolume = m_seBalance * unk70C;
    for (u32 i = 0; i < 32; i++) {
        m_seHandle[i].m_handle.SetVolume(seVolume, 0);
    }
}

float sndSystem::getBalanceSE() {
    // MATCH-ONLY: a plain `m_seBalance * 0.5f` loads the constant first; the local and *= give the target's order.
    float balance = m_seBalance;
    balance *= 0.5f;
    return balance;
}

void sndSystem::setBGMVol(bool setImmediately, float volume) {
    m_bgmVolume = clampf(volume, 1.0f, 0.0f);
    if (setImmediately) {
        m_bgmHandle.SetVolume(m_bgmBalance * (m_bgmDuck * (m_bgmVolume * m_bgmFade)), 0);
    }
}

void sndSystem::setSEVol(bool setImmediately, float volume) {
    unk70C = clampf(volume, 1.0f, 0.0f);
    if (setImmediately) {
        float seVolume = m_seBalance * unk70C;
        for (u32 i = 0; i < 32; i++) {
            m_seHandle[i].m_handle.SetVolume(seVolume, 0);
        }
    }
}

void sndSystem::setEffectVol(bool unused, bool setImmediately, float volume) {
    m_effectVolume = clampf(volume, 1.0f, 0.0f);
    if (setImmediately) {
        for (u32 i = 0; i < 32; i++) {
            m_seHandle[i].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
        }
    }
}

// Appends one of the effects to AX bus 0 (0: reverb, 1: delay); endHBMState re-appends them from m_effectFlags.
void sndSystem::appendEffect(int unused, int effect) {
    if (!m_loadFinished) {
        return;
    }
    switch (effect) {
    case 0:
        nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_A, &m_reverb[0]);
        m_effectFlags |= 1;
        break;
    case 1:
        nw4r::snd::detail::AxManager::GetInstance().AppendEffect(nw4r::snd::AUX_A, &m_delay[0]);
        m_effectFlags |= 2;
        break;
    }
}

// Reverb time in milliseconds (HYPOTHESIS: the fused time of the reverb parameters).
void sndSystem::setReverbTime(int ms) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    param.fusedTime = (f32)ms / 1000.0f;
    m_reverb[0].SetParam(param);
}

// Wet level of the reverb in percent.
void sndSystem::setReverbBalance(int percent) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    param.outGain = (f32)percent / 100.0f;
    m_reverb[0].SetParam(param);
}

void sndSystem::setReverbPreDelay(int ms) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    param.preDelayTime = (f32)ms / 1000.0f;
    m_reverb[0].SetParam(param);
}

void sndSystem::setReverbLPF(float) {
}

// Reverb presets 0..2: coloration and damping from two small tables.
void sndSystem::setReverbType(int type) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    float coloration[3] = { 0.3f, 0.6f, 1.0f };
    float damping[3] = { 0.2f, 0.4f, 0.4f };
    param.coloration = coloration[type];
    param.damping = damping[type];
    m_reverbType = type;
    m_reverb[0].SetParam(param);
}

void sndSystem::setReverbColoration(float coloration) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    param.coloration = coloration;
    m_reverb[0].SetParam(param);
}

void sndSystem::setReverbDamping(float damping) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxReverbHi::ReverbHiParam param = m_reverb[0].GetParam();
    param.damping = damping;
    m_reverb[0].SetParam(param);
}

void sndSystem::setDelayTime(float delay) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxDelay::DelayParam param = m_delay[0].GetParam();
    param.delay = delay;
    m_delay[0].SetParam(param);
}

void sndSystem::setDelayFeedback(float feedback) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxDelay::DelayParam param = m_delay[0].GetParam();
    param.feedback = feedback;
    m_delay[0].SetParam(param);
}

void sndSystem::setDelayOutput(float outGain) {
    if (!m_loadFinished) {
        return;
    }
    nw4r::snd::FxDelay::DelayParam param = m_delay[0].GetParam();
    param.outGain = outGain;
    m_delay[0].SetParam(param);
}

// Volume of a playing sound effect (0..1), scaled by the effect volume setting and the balance.
void sndSystem::setVol(s32 handleId, float volume, int frames) {
    u32 idx = findSlot(this, handleId);
    if (idx == -1) {
        return;
    }
    m_seHandle[idx].m_handle.SetVolume(unk70C * (m_seBalance * clampf(volume, 1.0f, 0.0f)), frames);
}

void sndSystem::setPitch(s32 handleId, float pitch) {
    u32 idx = findSlot(this, handleId);
    if (idx == -1) {
        return;
    }
    m_seHandle[idx].m_handle.SetPitch(pitch);
}

void sndSystem::setPan(s32 handleId, float pan) {
    u32 idx = findSlot(this, handleId);
    if (idx == -1) {
        return;
    }
    m_seHandle[idx].m_handle.SetPan(clampf(pan, 1.0f, -1.0f));
}

// Volume of the ambient parameters of the playing sound (BasicSound + 0x28), 1.0 when the handle id is not valid.
float sndSystem::getVol(s32 handleId) {
    u32 idx = findSlot(this, handleId);
    float* volume;
    if (idx == -1) {
        volume = NULL;
    } else if (!m_seHandle[idx].isUsed()) {
        volume = NULL;
    } else {
        nw4r::snd::detail::BasicSound* sound = m_seHandle[idx].m_handle.detail_GetAttachedSound();
        volume = sound != NULL ? &sound->GetAmbientParam().volume : NULL;
    }
    if (volume == NULL) {
        return 1.0f;
    }
    return *volume;
}

// Plays a sound effect through the speaker of a Wii Remote (instead of the TV).
s32 sndSystem::playSERem(int remoteNo, SndID sndID, int tag, int fadeIn) {
    u32 idx = allocSE();
    if (idx == -1) {
        return -1;
    }
    if (nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(remoteNo).EnableOutput(true)) {
        nw4r::snd::SoundHandle* handle = &m_seHandle[idx].m_handle;
        if (m_archivePlayer.StartSound(handle, (int)sndID)) {
            handle->SetOutputLine(lbl_80407718[remoteNo]);
            handle->SetRemoteOutVolume(remoteNo, unk70C * m_seBalance);
            if (handle->IsAttachedSound()) {
                handle->detail_GetAttachedSound()->FadeIn(fadeIn);
            }
            m_seHandle[idx].m_sndId = sndID;
            m_seHandle[idx].m_kind = 0;
            m_seHandle[idx].m_tag = tag;
            return m_seHandle[idx].m_id;
        }
    }
    return -1;
}

bool sndSystem::getRemEnable(int remoteNo) {
    if (!m_loadFinished) {
        return false;
    }
    return nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(remoteNo).IsEnabledOutput();
}

bool sndSystem::isPlay(s32 handleId) {
    u32 idx = findSlot(this, handleId);
    if (idx == -1) {
        return false;
    }
    return m_seHandle[idx].isUsed();
}

// HYPOTHESIS: whether a sound id is one of the voice clips of a fighter group (sound group 1..0x26 have an id range
// each, some of them a few extra ids; 0x29 and 0x2A-0x4B share a list of ids).
bool sndSystem::isVoice(u32 soundGroup, u32 sndID) {
    u32 rangeIdx = soundGroup - 1;
    if (soundGroup >= 1 && soundGroup <= 0x26) {
        if (sndID >= lbl_80407728[rangeIdx * 2] && sndID <= lbl_80407728[rangeIdx * 2 + 1]) {
            return true;
        }
        switch (soundGroup) {
        case 7:
            if (sndID >= 0x796 && sndID <= 0x7C1) {
                return true;
            }
            break;
        case 15:
            if (sndID >= 0x1A3A && sndID <= 0x1A60) {
                return true;
            }
            break;
        case 17:
            if (sndID >= 0x618 && sndID <= 0x63F) {
                return true;
            }
            break;
        case 18:
            if (sndID >= 0x66B && sndID <= 0x695) {
                return true;
            }
            break;
        // BYTES: the three ids are tested as separate comparisons (the "==" || chain is folded into a range test).
        case 28:
            if (sndID != 0x876 && sndID != 0x14D2 && sndID != 0x14D3) {
                break;
            }
            return true;
        case 29:
            if (sndID != 0x877 && sndID != 0x14D4 && sndID != 0x14D5) {
                break;
            }
            return true;
        case 30:
            if (sndID != 0x878 && sndID != 0x14D6 && sndID != 0x14D7) {
                break;
            }
            return true;
        }
        return false;
    }
    if (soundGroup == 0x28) {
        return false;
    }
    if (soundGroup == 0x29 || (soundGroup >= 0x2A && soundGroup <= 0x4B)) {
        if (sndID < 0x1AA9 || sndID > 0x1B45) {
            return false;
        }
        const u32* ids = lbl_80407858;
        for (int i = 0; i < 11; i++) {
            if (sndID == ids[0]) {
                return true;
            }
            if (sndID == ids[1]) {
                return true;
            }
            if (sndID == ids[2]) {
                return true;
            }
            ids += 3;
        }
    }
    return false;
}

// Whether the sound group a sound effect lives in is loaded. The group is given by the caller or, with -1, derived from
// the id range of the sound (the three ranges below are split over character-specific groups).
bool sndSystem::isSeLoaded(u32 sndID, u32 groupId) {
    if (groupId != -1) {
        return m_loadThread->isFinish((SndGroupID)groupId);
    }
    if (sndID <= 0x906 && sndID >= 0x87A) {
        if (sndID <= 0x8A9) {
            return m_loadThread->isFinish((SndGroupID)0x1C);
        }
        if (sndID >= 0x8D5) {
            return m_loadThread->isFinish((SndGroupID)0x1E);
        }
        return m_loadThread->isFinish((SndGroupID)0x1D);
    }
    if (sndID >= 0x1523 && sndID <= 0x1657) {
        if (sndID <= 0x157C) {
            return m_loadThread->isFinish((SndGroupID)0x1C);
        }
        if (sndID >= 0x15DF) {
            return m_loadThread->isFinish((SndGroupID)0x1E);
        }
        return m_loadThread->isFinish((SndGroupID)0x1D);
    }
    if (sndID >= 0x1AA9 && sndID <= 0x1B48) {
        int group = 0x2A;
        if (sndID >= 0x1B45) {
            group = 0x44;
        } else if (sndID >= 0x1B41) {
            group = 0x4B;
        } else if (sndID >= 0x1B3D) {
            group = 0x4A;
        } else if (sndID >= 0x1B35) {
            group = 0x49;
        } else if (sndID >= 0x1B31) {
            group = 0x48;
        } else if (sndID >= 0x1B2D) {
            group = 0x47;
        } else if (sndID >= 0x1B29) {
            group = 0x46;
        } else if (sndID >= 0x1B25) {
            group = 0x45;
        } else if (sndID >= 0x1B21) {
            group = 0x44;
        } else if (sndID >= 0x1B1D) {
            group = 0x47;
        } else if (sndID >= 0x1B19) {
            group = 0x43;
        } else if (sndID >= 0x1B15) {
            group = 0x42;
        } else if (sndID >= 0x1B11) {
            group = 0x41;
        } else if (sndID >= 0x1B0D) {
            group = 0x41;
        } else if (sndID >= 0x1B09) {
            group = 0x41;
        } else if (sndID >= 0x1B05) {
            group = 0x40;
        } else if (sndID >= 0x1B01) {
            group = 0x3F;
        } else if (sndID >= 0x1AFD) {
            group = 0x3E;
        } else if (sndID >= 0x1AF9) {
            group = 0x3D;
        } else if (sndID >= 0x1AF5) {
            group = 0x3C;
        } else if (sndID >= 0x1AF1) {
            group = 0x3B;
        } else if (sndID >= 0x1AED) {
            group = 0x3A;
        } else if (sndID >= 0x1AE9) {
            group = 0x39;
        } else if (sndID >= 0x1AE5) {
            group = 0x38;
        } else if (sndID >= 0x1AE1) {
            group = 0x37;
        } else if (sndID >= 0x1ADD) {
            group = 0x36;
        } else if (sndID >= 0x1AD9) {
            group = 0x35;
        } else if (sndID >= 0x1AD5) {
            group = 0x34;
        } else if (sndID >= 0x1AD1) {
            group = 0x33;
        } else if (sndID >= 0x1ACD) {
            group = 0x32;
        } else if (sndID >= 0x1AC5) {
            group = 0x31;
        } else if (sndID >= 0x1AC1) {
            group = 0x30;
        } else if (sndID >= 0x1ABD) {
            group = 0x2F;
        } else if (sndID >= 0x1AB9) {
            group = 0x2E;
        } else if (sndID >= 0x1AB5) {
            group = 0x2D;
        } else if (sndID >= 0x1AB1) {
            group = 0x2C;
        } else if (sndID >= 0x1AAD) {
            group = 0x2B;
        }
        return m_loadThread->isFinish((SndGroupID)group);
    } else {
        return true;
    }
}

// Index in the queue of waiting voices (m_seQueue) for a queued sound: kind 1 always takes the head, kind 2 sounds
// have a fixed place per sound id, and 5 for the others.
int sndSystem::getPlayQueIndex(SndID sndID, u32 kind) {
    int index = -1;
    if (kind == 1) {
        index = 0;
    } else if (kind == 2) {
        switch (sndID) {
        case 0x1B85:
            index = 1;
            break;
        case 0x2074:
            index = 2;
            break;
        case 0x2073:
            index = 3;
            break;
        case 0x2072:
            index = 4;
            break;
        default:
            index = 5;
            break;
        }
    }
    return index;
}

// ---- data ----

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
