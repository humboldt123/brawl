// Local BrawlHeaders shadow: add the verified Final sound-state cleanup entry and the
// SE volume factor read by sndDoubutsugoTask. The layout pass (RSBE01_02 sndSystem, 0xBD0 bytes) names every
// member at its proven offset; names without proof are `unkNNN` (hex offset). Offsets come from the constructor
// (fn_800729B0 = __ct), the per-frame update and the SE/BGM functions of snd_system.o.
#pragma once

#include <StaticAssert.h>
#include <stddef.h>
#include <snd/snd_id.h>
#include <types.h>
#include <nw4r/snd/snd_SoundHeap.h>
#include <nw4r/snd/snd_Sound3DManager.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <nw4r/snd/snd_DvdSoundArchive.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <nw4r/snd/snd_FxBase.h>
#include <ogws/nw4r/snd/snd_SoundHandle.h>

class sndInitThread;
class sndLoadThread;

// HYPOTHESIS: layout stand-ins for the two effect classes of nw4r (snd_FxReverbHi.o / snd_FxDelay.o). Neither header
// is on the include path of this tree (only include/ogws has them), so only the parts sndSystem touches are
// declared: the sizes (0x190 / 0x88), the virtual slots (0x8 dtor, 0xC StartUp, 0x10 Shutdown, 0x14 UpdateBuffer,
// 0x18 AssignWorkBuffer, 0x1C ReleaseWorkBuffer; they are what the weak destructors fn_80073028 / fn_800730A4 and the
// constructor call) and the parameter structs. The destructors are inline in the OGWS headers, which is why the
// weak copies live in snd_system.o; the vtables (lbl_804666F8 / lbl_804666D8) belong to the nw4r objects.
namespace nw4r {
namespace snd {

class FxReverbHi : public FxBase {
public:
    struct ReverbHiParam {
        f32 preDelayTime; // 0x0
        f32 fusedTime;    // 0x4
        f32 coloration;   // 0x8
        f32 damping;      // 0xC
        f32 crossTalk;    // 0x10
        f32 outGain;      // 0x14
    };

    FxReverbHi();
    virtual ~FxReverbHi() {
        Shutdown();
        ReleaseWorkBuffer();
    }                                                          // 0x8
    virtual bool StartUp();                                    // 0xC
    virtual void Shutdown();                                   // 0x10
    virtual void UpdateBuffer(int channels, void** ppBuffer, u32 size, SampleFormat format, f32 sampleRate,
                              OutputMode mode);                // 0x14
    virtual bool AssignWorkBuffer(void* pBuffer, u32 size);    // 0x18
    virtual void ReleaseWorkBuffer();                          // 0x1C

    u32 GetRequiredMemSize();
    bool SetParam(const ReverbHiParam& rParam);
    // HYPOTHESIS: fn_801C07AC (8 bytes: this + 0x18) is the out-of-line accessor of the parameter block at 0x18.
    const ReverbHiParam& GetParam() const { return *reinterpret_cast<const ReverbHiParam*>(reinterpret_cast<const char*>(this) + 0x18); }

private:
    char unkC[0x190 - 0xC]; // AxfxImpl at 0xC, ReverbHiParam at 0x18, AXFX_REVERBHI at 0x30
};
static_assert(sizeof(FxReverbHi) == 0x190, "Class is wrong size!");

class FxDelay : public FxBase {
public:
    struct DelayParam {
        f32 delay;    // 0x0
        f32 feedback; // 0x4
        f32 outGain;  // 0x8
    };

    FxDelay();
    virtual ~FxDelay() {
        Shutdown();
        ReleaseWorkBuffer();
    }                                                          // 0x8
    virtual bool StartUp();                                    // 0xC
    virtual void Shutdown();                                   // 0x10
    virtual void UpdateBuffer(int channels, void** ppBuffer, u32 size, SampleFormat format, f32 sampleRate,
                              OutputMode mode);                // 0x14
    virtual bool AssignWorkBuffer(void* pBuffer, u32 size);    // 0x18
    virtual void ReleaseWorkBuffer();                          // 0x1C

    u32 GetRequiredMemSize();
    bool SetParam(const DelayParam& rParam);
    // HYPOTHESIS: fn_801C0390 (8 bytes: this + 0x18) is the out-of-line accessor of the parameter block at 0x18.
    const DelayParam& GetParam() const { return *reinterpret_cast<const DelayParam*>(reinterpret_cast<const char*>(this) + 0x18); }

private:
    char unkC[0x88 - 0xC]; // AxfxImpl at 0xC, DelayParam at 0x18, AXFX_DELAY at 0x24
};
static_assert(sizeof(FxDelay) == 0x88, "Class is wrong size!");

} // namespace snd
} // namespace nw4r

class sndHeapSys
{
public:
    char _0[0x258];
    nw4r::snd::SoundHeap m_heapArr[0xC];
    char _468[0xC];
    nw4r::snd::SoundHeap* m_heapAddresses[0xC];
    u32 m_heapMaxSizeArr[0xC];
    u32 m_heapCurrSizeArr[0xC];

    // HYPOTHESIS: names follow the map order of sndHeapSys (RSBE01_02 sora.map); signatures are those of the stand-ins
    // snd_system.cpp calls through fn_xxxx today (heap, slot / level, group id, size). Not defined in snd_system.o
    // (they are 0x80079EB8-0x8007AD48); declared so the callers can use member calls once symbols.txt names them:
    //   __ct__10sndHeapSysFv 80079EB8, __dt__10sndHeapSysFv 8007A1EC (called as fn_8007A1EC(heap, 1))
    //   getHeapP__10sndHeapSysFl 8007A314, getUseHeapId__10sndHeapSysFl10SndGroupIDl 8007A324,
    //   reserveNextHeapLevel__10sndHeapSysFl10SndGroupIDl 8007A5EC, saveState__10sndHeapSysF10SndGroupIDl 8007A78C,
    //   reserveFreeGroup__10sndHeapSysFv 8007A79C, freeGroup__10sndHeapSysFl 8007AA48, isLoaded__10sndHeapSysF10SndGroupID 8007AB34
    nw4r::snd::SoundMemoryAllocatable* getHeapP(s32 slot);
    s32 getUseHeapId(int heapLevel, SndGroupID groupId, s32 size);
    s32 reserveNextHeapLevel(s32 slot, SndGroupID groupId, s32 size);
    void saveState(SndGroupID groupId, s32 slot);
    void reserveFreeGroup();
    void freeGroup(int heapLevel);
    s32 isLoaded(SndGroupID groupId);
};
static_assert(sizeof(sndHeapSys) == 0x504, "Class is wrong size!");

class sndSystem {
public:
    // HYPOTHESIS: one slot of the sound-effect table (32 of them at 0x2D4). Map: sndSystem::sndHandle (nested class,
    // constructor fn_80073120 only clears the handle, destructor fn_8007312C detaches it). Field meanings from
    // playSE / prepareSE / stopSE1 / allStopSE:
    class sndHandle {
    public:
        nw4r::snd::SoundHandle m_handle; // 0x00 the playing sound
        // The id handed to callers: (m_handleSeq & 0x7FFFFF00) + m_index when the slot is taken, -1 when freed.
        // Callers address a slot by `id & 0xFF` and compare the whole id (stale ids fail).
        s32 m_id;            // 0x04
        // Owner tag given to playSE (0x10000 survives allStopSE, -1 none, otherwise a group id for stopGroup).
        s32 m_tag;           // 0x08
        s32 m_sndId;         // 0x0C SndID (-1 none)
        s32 m_kind;          // 0x10 nw4r::snd::SoundArchive::GetSoundUserParam (1/2: queued voices, 0xB: unique)
        s32 m_priority;      // 0x14 SoundInfo playerPriority, 0x40 when unknown
        s32 m_curPriority;   // 0x18 m_priority + the live priority of the sound (refreshed by update)
        u16 m_count;         // 0x1C HYPOTHESIS: reservation count of a queued voice (kind 1/2) before the handle attaches
        s16 m_index;         // 0x1E the slot's own index

        sndHandle() {}
        ~sndHandle() {}

        // HYPOTHESIS: inlined everywhere ("slot in use"): an id is reserved and counted, or a sound is attached.
        bool isUsed() const {
            if (m_sndId != -1 && m_count != 0) {
                return true;
            }
            return m_handle.IsAttachedSound();
        }
    };
    static_assert(sizeof(sndHandle) == 0x20, "Class is wrong size!");

    nw4r::snd::DvdSoundArchive m_dvdSoundArchive;           // 0x000
    nw4r::snd::SoundArchivePlayer m_archivePlayer;          // 0x18C
    nw4r::snd::Sound3DManager m_3DManager;                  // 0x26C
    nw4r::snd::Sound3DListener m_3DListener;                // 0x294 (matrix 0x294; interior size 0x2C4, max volume
                                                            // distance 0x2C8, unit distance 0x2CC: 50, 60, 150 in init)
    sndHeapSys* m_sndHeapSys;                               // 0x2D0
    sndHandle m_seHandle[32];                               // 0x2D4
    s32 m_seSearchIdx;                                      // 0x6D4 round-robin start of the free-slot search
    s32 unk6D8;                                             // 0x6D8 cleared by the constructor, never read
    s32 unk6DC;                                             // 0x6DC cleared by the constructor, never read
    nw4r::snd::SoundHandle m_bgmHandle;                     // 0x6E0 (vtable calls of playBGM / stopBGM / pause)
    s32 m_bgmId;                                            // 0x6E4 id of the music in m_bgmHandle (-1 none)
    s32 unk6E8;                                             // 0x6E8 cleared by init
    bool m_bgmEnable;                                       // 0x6EC 1 from the constructor on; BGM functions check it
    bool m_seEnable;                                        // 0x6ED 1 from the constructor on; SE functions check it
    bool m_initFinished;                                    // 0x6EE update() saw sndInitThread::isFinishInitialize (archive header and buffers set up)
    bool m_loadFinished;                                    // 0x6EF update() saw sndInitThread::isFinishLoad (common groups loaded; it then resumes m_loadThread); gate of the play functions
    void* m_archiveHeader;                                  // 0x6F0 gfHeapManager block, size from the brsar header (+0x124)
    void* m_playerBuffer;                                   // 0x6F4 SoundArchivePlayer work memory
    void* m_strmBuffer;                                     // 0x6F8 SoundArchivePlayer stream buffer (0xE6000)
    void* m_3DBuffer;                                       // 0x6FC Sound3DManager work memory
    f32 m_bgmVolume;                                        // 0x700 setBGMVol (0..1)
    f32 m_bgmDuck;                                          // 0x704 ducks the music while a voice (kind 1/2) plays: 0 / 0.2 / 1.0
    f32 m_bgmFade;                                          // 0x708 1.0 normally, 0.5 while the game is paused (pause / resume)
    // HYPOTHESIS: SE volume factor; sndDoubutsugoTask scales its voice volume by it.
    float unk70C;                                           // 0x70C setSEVol (0..1)
    f32 m_effectVolume;                                     // 0x710 setEffectVol: aux (FX) send of the effects
    f32 m_bgmBalance;                                       // 0x714 setVolBalance: 2 * bgm / (bgm + se), capped at 1
    f32 m_seBalance;                                        // 0x718 setVolBalance: 2 * se / (bgm + se), capped at 1
    f32 m_seqSpeed;                                         // 0x71C tempo ratio set by setFinalSeqSpeed
    bool m_paused;                                          // 0x720 pause() / resume()
    bool m_specialPaused;                                   // 0x721 specialPause() paused it
    s32 m_hbmState;                                         // 0x724 HYPOTHESIS: 1 while the HOME Button Menu holds the sound (pause variant)
    nw4r::snd::FxReverbHi m_reverb[2];                      // 0x728 0x190 each (bus 1 effect is [1])
    nw4r::snd::FxDelay m_delay[2];                          // 0xA48 0x88 each
    void* m_reverbBuffer[2];                                // 0xB58 work memory of m_reverb[i]
    void* m_delayBuffer[2];                                 // 0xB60 work memory of m_delay[i]
    s32 m_reverbType;                                       // 0xB68 setReverbType
    nw4r::snd::FxReverbHi::ReverbHiParam m_hbmReverbParam;  // 0xB6C saved by startHBMState, restored by endHBMState
    nw4r::snd::FxDelay::DelayParam m_hbmDelayParam;         // 0xB84
    u8 m_effectFlags;                                       // 0xB90 bit 0: m_reverb[0] appended to bus 0, bit 1: m_delay[0] (appendEffect)
    sndInitThread* m_initThread;                            // 0xB94 resumed by startLoadThread (map name)
    sndLoadThread* m_loadThread;                            // 0xB98 loadGroup / freeGroup requests
    s32 m_remoteState[4];                                   // 0xB9C HYPOTHESIS: per Wii Remote speaker setup state machine in update(): 0 idle, 1 setup requested, 2 speaker enabled
    s32 m_curSeKind;                                        // 0xBAC kind (SoundUserParam 1/2) of the queued voice playing now, 0 none
    s32 m_curSeId;                                          // 0xBB0 handle id of that voice (-1 none)
    s32 m_seQueue[6];                                       // 0xBB4 waiting voice handle ids by getPlayQueIndex (-1 none)
    virtual ~sndSystem();                                   // 0xBCC vptr (lbl_80454470: typeinfo, 0, fn_800734EC)

    // fn_80072968: creates g_sndSystem (0xBD0 bytes, Heaps::Sound) once
    static sndSystem* create();
    sndSystem();                  // fn_800729B0 (0x624 bytes)
    // HYPOTHESIS: the map calls it startLoadThread, it resumes m_initThread (0xB94).
    void startLoadThread();       // fn_80073184
    bool loadArchiveHeader();
    bool setupBuffers();
    // HYPOTHESIS: names from the map (startHBMState / endHBMState): save the reverb and delay settings and clear the
    // AX effect buses while the HOME Button Menu runs / restore them afterwards.
    void startHBMState();         // fn_8007331C
    void endHBMState();           // fn_800733D8
    void init();                  // fn_80073670 resets the state the constructor does not (listener, queue ids)
    void update();                // fn_800737E4 per frame (map: sndSystem::update)
    int loadSoundGroup(SndGroupID sndGroupId, int, bool isRequest);
    void freeGroup(int heapLevel, int);
    bool isFinishLoad();   // the sound groups that loadSoundGroup requested are loaded (the load thread is done)
    // HYPOTHESIS: the names of the next two follow the main map's order (freeGroup, isFinishLoad, isFinishLoad1,
    // isFinishFree, isExist); they ask the load thread (sndLoadThread::isFinish1 / isFinishFree).
    bool isFinishLoad1(u32 groupId);  // HYPOTHESIS: the group that loadSoundGroup requested is loaded
    bool isFinishFree();
    bool isSoundGroupLoaded(SndGroupID sndGroupId);
    void loadCommon();
    void loadGroupThread(SndGroupID, int);
    void freeGroupThread(int, int);

    bool playBGM(int bgmId, u32 fade, bool);
    void stopBGM(int fade);
    int getBGMId();
    // HYPOTHESIS: the name (the map skips 0x80074018); it loads the music so that startBGM can start it without a wait.
    bool prepareBGM(int bgmId, int unk);
    bool isPreparedBGM(int bgmId);
    void startBGM(int bgmId, int startFrame);   // HYPOTHESIS: the frame of the music it starts at

    s32 playSE(SndID sndID, int unk, int unk2, int unk3, int unk4);
    bool isPlaySEID(SndID sndID);
    // Plays a sound effect at a fixed volume and pitch (map: sndSystem::playSEwithFixEffect); the menus pass 1.0 and 1.0.
    // HYPOTHESIS: the two floats are the volume and the pitch.
    void playSEwithFixEffect(SndID sndID, float volume, float pitch);
    // HYPOTHESIS (map order play3DSE, prepare3DSE): playSE / prepareSE on the SoundStartable of a Sound3DActor
    // (snd3DGenerator calls them); params: id, startable, tag, fadeIn, flag, group. fn_80074EF8 / fn_80075830.
    s32 play3DSE(SndID sndID, nw4r::snd::SoundStartable* startable, int tag, int fadeIn, int flag, int groupId);
    s32 prepareSE(SndID p1, s32 p2, s32 p3);
    s32 prepare3DSE(SndID sndID, nw4r::snd::SoundStartable* startable, int tag);
    bool isPreparedSE(s32 handleId);       // fn_80075B7C
    void startSE(s32 p1, s32 p2, s32 p3);
    void startSE1(int slot, int fadeIn);   // fn_80075D1C startSE after the handle id became a slot index
    void stopSE(s32 handleId, s32 p2);
    void stopSE(s8 handleIndex, s32 p2);
    void stopSE1(int slot, int fade);      // fn_80075FAC stopSE after the handle id became a slot index
    // HYPOTHESIS: fn_800765B0 stops every slot with the tag (map stopGroup) and fn_80076674 those with the tag and the id.
    void stopGroup(int tag, int fade);
    void stopGroupId(int tag, SndID sndID, int fade);
    void allStopSE(int fade, bool restoreBgm);   // fn_80076714: slots whose tag is not 0x10000
    // HYPOTHESIS: fn_80076860 / fn_80076A6C (map pause / resume): returns false if not ready / already (un)paused.
    bool pause(bool lowerBgm);
    bool resume(bool lowerBgm);
    void resumeClear();                   // fn_80076C78
    void specialPause();
    void specialResume();
    // HYPOTHESIS: fn_80076D00 applies the tempo ratio to the voice in m_curSeId; fn_80076E30 to a handle.
    void setFinalSeqSpeed(float speed);
    void setSeqSpeed(s32 handleId, float speed);
    void setVolBalance(float bgm, float se);
    float getBalanceSE();                 // fn_80077054: m_seBalance * 0.5
    void setBGMVol(bool setImmediately, float volume);
    void setSEVol(bool setImmediately, float volume);          // fn_800770CC
    void setEffectVol(bool unused, bool setImmediately, float volume);
    // HYPOTHESIS: reverb / delay setters of the map order (fn_80077200 - fn_8007776C); r4 of appendEffect is unused.
    void appendEffect(int unused, int effect);                 // fn_80077200 effect: 0 reverb, 1 delay
    void setReverbTime(int ms);            // fn_80077288
    void setReverbBalance(int percent);    // fn_80077334
    void setReverbPreDelay(int ms);        // fn_800773E0
    void setReverbLPF(float);              // fn_8007748C (empty)
    void setReverbType(int type);          // fn_80077490 (0..2, fills m_reverbType)
    void setReverbColoration(float coloration);   // fn_8007757C
    void setReverbDamping(float damping);         // fn_80077604
    void setDelayTime(float delay);               // fn_8007768C
    void setDelayFeedback(float feedback);        // fn_800776FC
    void setDelayOutput(float outGain);           // fn_8007776C
    void setVol(s32 handleId, float volume, int frames);   // fn_800777DC
    void setPitch(s32 handleId, float pitch);              // fn_800778CC
    void setPan(s32 p1, float p2);
    float getVol(s32 handleId);                            // fn_80077A64
    // HYPOTHESIS: playSERem (map): plays on the speaker of a Wii Remote; returns a handle id.
    s32 playSERem(int remoteNo, SndID sndID, int tag, int fadeIn);   // fn_80077B98
    bool getRemEnable(int remoteNo);       // fn_80077D54 RemoteSpeaker::IsEnabledOutput
    bool isPlay(s32 handleId);
    // HYPOTHESIS: fn_80077E94 (map isVoice) tells whether an id lies in the voice ranges of a sound group (tables
    // lbl_80407728 / lbl_80407858); it uses no member, ft_sound_id_exchanger calls it with (group, id).
    static bool isVoice(int soundGroup, u32 sndID);
    // HYPOTHESIS: fn_80078068 (map isSeLoaded): whether the group of the sound (given, or looked up from id ranges) is loaded.
    bool isSeLoaded(SndID sndID, int groupId);
    // HYPOTHESIS: fn_80078380 (map getPlayQueIndex): index into m_seQueue of a queued voice, -1 / 0 .. 5.
    int getPlayQueIndex(SndID sndID, u32 kind);

    // HYPOTHESIS: formal type of the observed caller argument. The native
    // entry restores Final sound state and does not consume this argument.
    void finalOffState(int);

    static SndID constructInterceptSndID(int wavID, SndID infoSndID) { // custom
        return static_cast<SndID>(0xF0000000 + ((wavID & 0xFFF) << 16) + infoSndID);
    }
};
static_assert(sizeof(sndSystem) == 0xbd0, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_archivePlayer) == 0x18C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_3DManager) == 0x26C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_3DListener) == 0x294, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_sndHeapSys) == 0x2D0, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_seHandle) == 0x2D4, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_seSearchIdx) == 0x6D4, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_bgmHandle) == 0x6E0, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_bgmId) == 0x6E4, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_bgmEnable) == 0x6EC, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_loadFinished) == 0x6EF, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_archiveHeader) == 0x6F0, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_bgmVolume) == 0x700, "Class is wrong size!");
static_assert(offsetof(sndSystem, unk70C) == 0x70C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_seqSpeed) == 0x71C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_paused) == 0x720, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_hbmState) == 0x724, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_reverb) == 0x728, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_delay) == 0xA48, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_reverbBuffer) == 0xB58, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_reverbType) == 0xB68, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_hbmReverbParam) == 0xB6C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_hbmDelayParam) == 0xB84, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_effectFlags) == 0xB90, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_initThread) == 0xB94, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_loadThread) == 0xB98, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_remoteState) == 0xB9C, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_curSeKind) == 0xBAC, "Class is wrong size!");
static_assert(offsetof(sndSystem, m_seQueue) == 0xBB4, "Class is wrong size!");

extern sndSystem* g_sndSystem;
// HYPOTHESIS: sequence of the SE handle ids (sbss 0x805A01D4): the next id is `lbl_805A01D4 + slot index`, then it
// advances by 0x100 and keeps bits 8..30.
extern s32 lbl_805A01D4;
