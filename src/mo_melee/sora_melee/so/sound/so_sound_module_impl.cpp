#define SO_SOUND_3D_GENERATOR_ACCESSER_FULL
#define SO_COLLISION_ATTACK_DATA_SOUND_WORD_FIELDS
#include <mt/mt_vector.h>
#include <snd/snd_system.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_log.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/sound/so_sound_util.h>
#include <types.h>

// HYPOTHESIS: parameter lists of the unnamed main.dol sndSystem stopGroupId, setVol, setPitch, setSeqSpeed and getVol
// (sora map); the stage units still call them through their own stand-ins.
extern "C" void fn_800765B0(sndSystem* system, int groupId, int unk);
extern "C" void fn_800777DC(sndSystem* system, int handle, float volume, int unk);
extern "C" void fn_800778CC(sndSystem* system, int handle, float pitch);
extern "C" void fn_80076E30(sndSystem* system, int handle, float speed);
extern "C" float fn_80077A64(sndSystem* system, int handle);

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soSoundStatusDataView {
    char _0[0xc];
    u32 : 27;
    u32 m_keepStatusSE : 1;
};

// HYPOTHESIS: one SE per damage sound attribute (rows) and sound level (columns).
static const SndID s_damageSEs[29][4] = {
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0x3B, (SndID)0x3C, (SndID)0x3D, (SndID)-1},
    {(SndID)0x3E, (SndID)0x3F, (SndID)0x40, (SndID)-1},
    {(SndID)0x41, (SndID)0x42, (SndID)0x43, (SndID)-1},
    {(SndID)0xE8, (SndID)0xE8, (SndID)0xE8, (SndID)0xE8},
    {(SndID)0x1F84, (SndID)0x1F83, (SndID)0x1F82, (SndID)-1},
    {(SndID)0x1F8C, (SndID)0x1F8B, (SndID)0x1F89, (SndID)-1},
    {(SndID)0x79, (SndID)0x7A, (SndID)0x7B, (SndID)0x7C},
    {(SndID)0x4B, (SndID)0x4C, (SndID)0x4D, (SndID)-1},
    {(SndID)0xAA, (SndID)0xAB, (SndID)0xAC, (SndID)0xAD},
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0xC6, (SndID)0xC7, (SndID)0xC8, (SndID)0xC9},
    {(SndID)-1, (SndID)-1, (SndID)-1, (SndID)-1},
    {(SndID)0x190A, (SndID)0x1909, (SndID)0x1907, (SndID)0x1908},
    {(SndID)0x16E8, (SndID)0x16E7, (SndID)0x16E5, (SndID)0x16E5},
    {(SndID)0x3B5, (SndID)0x3B4, (SndID)0x3B2, (SndID)0x3B3},
    {(SndID)0x75, (SndID)0x76, (SndID)0x77, (SndID)0x78},
    {(SndID)0x1FA6, (SndID)0x1FA6, (SndID)0x1FA6, (SndID)0x1FA6},
    {(SndID)0xF1B, (SndID)0xF1B, (SndID)0xF1B, (SndID)0xF1B},
    {(SndID)0xF1C, (SndID)0xF1C, (SndID)0xF1C, (SndID)0xF1C},
    {(SndID)0xF1D, (SndID)0xF1D, (SndID)0xF1D, (SndID)0xF1D},
    {(SndID)0xF1E, (SndID)0xF1E, (SndID)0xF1E, (SndID)0xF1E},
    {(SndID)0x168D, (SndID)0x168C, (SndID)0x168A, (SndID)0x168B},
    {(SndID)0x112A, (SndID)0x112A, (SndID)0x112A, (SndID)0x112A},
    {(SndID)0x136, (SndID)0x136, (SndID)0x136, (SndID)0x136},
    {(SndID)0x120, (SndID)0x120, (SndID)0x120, (SndID)0x120},
    {(SndID)0xD79, (SndID)0xD79, (SndID)0xD79, (SndID)0xD79},
    {(SndID)0xE75, (SndID)0xE75, (SndID)0xE75, (SndID)0xE75},
    {(SndID)0xCC, (SndID)0xCD, (SndID)0xCE, (SndID)0xCF},
};

soSoundModule::~soSoundModule() { }

soSoundModuleImpl::~soSoundModuleImpl() { }

void soSoundModuleImpl::activate(Vec3f* pos) {
    m_soundGeneratorAccesser->activate(pos);
}

void soSoundModuleImpl::deactivate() {
    fn_800765B0(g_sndSystem, m_seGroupId, 0);
    fn_800765B0(g_sndSystem, m_statusSEGroupId, 0);
    m_sePitch = 1.0f;
    m_soundGeneratorAccesser->deactivate();
    m_checkSoundGroup = -1;
}

void soSoundModuleImpl::update(Vec3f* pos) {
    if (!m_generatorStatus) {
        setPos(pos, 0);
    }
}

void soSoundModuleImpl::setPos(Vec3f* pos, int index) {
    soSoundUtil::setPos(m_soundGeneratorAccesser, pos, index);
}

int soSoundModuleImpl::playSE(SndID sndId, bool setPitch, bool noGroup, int index) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    int handle;
    if (noGroup == true) {
        handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, -1, m_checkSoundGroup);
    } else {
        handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, m_seGroupId, m_checkSoundGroup);
    }
    if (m_sePitch != 1.0f && setPitch == true && handle >= 0) {
        fn_800778CC(g_sndSystem, handle, m_sePitch);
    }
    return handle;
}

int soSoundModuleImpl::playSENo3d(SndID sndId, bool noGroup) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    if (noGroup == true) {
        return g_sndSystem->playSE(id, -1, 0, 0, m_checkSoundGroup);
    }
    return g_sndSystem->playSE(id, m_seGroupId, 0, 0, m_checkSoundGroup);
}

int soSoundModuleImpl::playSEPos(SndID sndId, Vec3f* pos, bool setPitch, bool noGroup, int index) {
    setPos(pos, index);
    return playSE(sndId, setPitch, noGroup, index);
}

int soSoundModuleImpl::playStatusSE(SndID sndId, bool setPitch, int index) {
    SndID id = m_soundIdExchanger->getId(sndId, m_moduleAccesser);
    if ((u32)id == 0xFFFFFFFF) {
        return -1;
    }
    int handle = soSoundUtil::playSE(m_soundGeneratorAccesser, index, id, m_statusSEGroupId, m_checkSoundGroup);
    if (m_sePitch != 1.0f && setPitch == true && handle >= 0) {
        fn_800778CC(g_sndSystem, handle, m_sePitch);
    }
    return handle;
}

void soSoundModuleImpl::setSEVol(int handle, float volume, int unk) {
    fn_800777DC(g_sndSystem, handle, volume, unk);
}

float soSoundModuleImpl::getSEVol(int handle) {
    return fn_80077A64(g_sndSystem, handle);
}

void soSoundModuleImpl::setSEPitch(u32 handle, double pitch) {
    fn_800778CC(g_sndSystem, handle, pitch);
}

void soSoundModuleImpl::stopStatusSE() {
    fn_800765B0(g_sndSystem, m_statusSEGroupId, 0);
}

void soSoundModuleImpl::stopSE(int unk, int index) {
    soSoundUtil::stopSE(m_soundGeneratorAccesser, index, unk, m_seGroupId);
}

void soSoundModuleImpl::stopSEHandle(int handle, int unk) {
    g_sndSystem->stopSE(static_cast<s32>(handle), unk);
}

void soSoundModuleImpl::playDamageSE(u32 index1, u32 index2) {
    SndID id = s_damageSEs[index1][index2];
    if ((u32)id != 0xFFFFFFFF) {
        playSE(id, false, false, 0);
    }
}

void soSoundModuleImpl::setSESpeed(int handle, float speed) {
    fn_80076E30(g_sndSystem, handle, speed);
}

bool soSoundModuleImpl::isPlay(int handle) {
    return g_sndSystem->isPlay(handle);
}

bool soSoundModuleImpl::isObserv(char unk1) {
    return unk1 == 10;
}

void soSoundModuleImpl::setPlayHitSEFlag(bool flag) {
    m_playHitSEFlag = flag;
}

void soSoundModuleImpl::setFixGeneratorStatus() {
    m_generatorStatus = 1;
}

void soSoundModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    m_generatorStatus = 0;
    if (!reinterpret_cast<soSoundStatusDataView*>(statusData)->m_keepStatusSE) {
        stopStatusSE();
    }
    m_sndID = (SndID)-1;
}

void soSoundModuleImpl::playHitSE(float power, void* data) {
    soCollisionAttackData* attackData = static_cast<soCollisionAttackData*>(data);
    if (attackData->m_soundAttribute != 0) {
        if (power > 0.0f) {
            playDamageSE(attackData->m_soundAttribute, attackData->m_soundLevel);
        } else {
            playSE((SndID)0xF, false, false, 0);
        }
    }
}

void soSoundModuleImpl::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk, soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    if (getPlayHitSEFlag() && attackData->m_soundAttribute != 0) {
        if (collisionLog->_41 == true) {
            playSE((SndID)0xBD, false, false, 0);
        } else {
            playHitSE(power, attackData);
        }
    }
}

bool soSoundModuleImpl::getPlayHitSEFlag() {
    return m_playHitSEFlag;
}

void soSoundModuleImpl::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    if (kind == Situation_Ground) {
        playLandingSE();
    }
}

void soSoundModuleImpl::playLandingSE() {
    if ((u32)m_sndID != 0xFFFFFFFF) {
        playSE(m_sndID, false, false, 0);
    }
}

int soSoundModuleImpl::getCheckSoundGroup() {
    return m_checkSoundGroup;
}

void soSoundModuleImpl::setCheckSoundGroup(int group) {
    m_checkSoundGroup = group;
}

soSoundModule::SoundIdData* soSoundModuleImpl::getSoundIdData() {
    return m_soundIdData;
}

void soSoundModuleImpl::setSoundIdData(SoundIdData* data) {
    m_soundIdData = data;
}

float soSoundModuleImpl::getSEPitch() {
    return m_sePitch;
}

void soSoundModuleImpl::setSEPitch(float pitch) {
    m_sePitch = pitch;
}
