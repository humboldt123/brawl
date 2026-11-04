#include <snd/snd_system.h>
#include <types.h>
#include <nw4r/snd/snd_AxManager.h>
#include <nw4r/snd/snd_RemoteSpeaker.h>
#include <nw4r/snd/snd_RemoteSpeakerManager.h>
#include <ogws/nw4r/snd/snd_SeqSoundHandle.h>
#include <snd/snd_load_thread.h>
#include <nw4r/math/math_arithmetic.h>

// Tables defined by snd_system.cpp (cluster A).
extern const u32 lbl_80407718[4];
extern const u32 lbl_80407728[76];
extern const u32 lbl_80407858[34];

// Clamp done with two fsel and no branches (upper bound first).
// HYPOTHESIS: the register allocation of the target differs a little (constants and the temporary swap registers),
// so the callers are not exact yet.
static inline float clampf(float x, float hi, float lo) {
    x = nw4r::math::FSelect(hi - x, x, hi);
    return nw4r::math::FSelect(lo - x, lo, x);
}

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
