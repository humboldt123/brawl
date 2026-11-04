#include <snd/snd_system.h>
#include <types.h>
#include <nw4r/math/math_arithmetic.h>

// Sound-effect slot helpers. The original source inlines them everywhere (playSE, prepareSE, startSE1, ...), the
// cluster-B copies in snd_system_b.cpp are the same functions; when the four temporary files are merged these
// belong to the top of the single file.

// Slot of a handle id (see sndSystem::sndHandle::m_id): the low byte of the id, -1 when the id is stale or free.
static inline u32 seSlotFromId(sndSystem* sys, s32 handleId) {
    if (handleId == -1) {
        return -1;
    }
    bool ok = false;
    int slot = handleId & 0xFF;
    sndSystem::sndHandle* h = &sys->m_seHandle[slot];
    if (h->m_id == handleId) {
        if (h->isUsed()) {
            ok = true;
        }
    }
    return ok ? slot : -1;
}

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

// Priority stored with a freshly prepared sound (0x40 when the archive is not loaded yet or has no entry).
static inline int seNewPriority(sndSystem* sys, SndID sndID) {
    if (!sys->m_loadFinished) {
        return 0x40;
    }
    nw4r::snd::SoundArchive::SoundInfo info;
    if (sys->m_dvdSoundArchive.ReadSoundInfo(sndID, &info)) {
        return info.playerPriority;
    }
    return 0x40;
}

// Sets the fade-in of a playing sound effect (SoundHandle has no FadeIn of its own).
static inline void seFadeIn(nw4r::snd::SoundHandle& handle, int frames) {
    if (handle.IsAttachedSound()) {
        handle.detail_GetAttachedSound()->FadeIn(frames);
    }
}

static inline s32 seGetPriority(sndSystem* sys, SndID sndID) {
    s32 priority = 0x7F;
    if ((u32)sndID != -1) {
        priority = seNewPriority(sys, sndID);
    }
    return priority;
}

static inline u32 seStealSlot(sndSystem* sys, SndID sndID) {
    int lowest = -1;
    int best = seGetPriority(sys, sndID);
    for (int i = 0; i < 32; i++) {
        if (sys->m_seHandle[i].m_count == 0 && sys->m_seHandle[i].m_curPriority <= best) {
            best = sys->m_seHandle[i].m_curPriority;
            lowest = i;
        }
    }
    sys->freeSE(lowest);
    return lowest;
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
        slot = seStealSlot(this, sndID);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (m_archivePlayer.detail_PrepareSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
            m_seHandle[slot].m_tag = tag;
            int priority = seNewPriority(this, sndID);
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
        slot = seStealSlot(this, sndID);
    }
    if (slot < 32) {
        m_seHandle[slot].m_count = 0;
        if (startable->detail_PrepareSound(&m_seHandle[slot].m_handle, sndID, NULL, NULL, NULL) ==
            nw4r::snd::SoundStartable::START_SUCCESS) {
            m_seHandle[slot].m_sndId = sndID;
            m_seHandle[slot].m_kind = m_dvdSoundArchive.GetSoundUserParam(sndID);
            m_seHandle[slot].m_tag = tag;
            int priority = seNewPriority(this, sndID);
            m_seHandle[slot].m_priority = priority;
            m_seHandle[slot].m_curPriority = priority;
            return m_seHandle[slot].m_id;
        }
    }
    return -1;
}

bool sndSystem::isPreparedSE(s32 handleId) {
    u32 slot = seSlotFromId(this, handleId);
    bool prepared;
    if (!m_seEnable) {
        prepared = true;
    } else if (slot == -1) {
        prepared = true;
    } else {
        sndHandle* h = &m_seHandle[slot];
        prepared = false;
        if (h->m_handle.IsAttachedSound() && h->m_handle.detail_GetAttachedSound()->IsPrepared()) {
            prepared = true;
        }
    }
    return prepared;
}

void sndSystem::startSE(s32 handleId, s32 fadeIn, s32 unique) {
    startSE1(seSlotFromId(this, handleId), fadeIn, unique);
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
        if (m_seHandle[slot].m_handle.IsAttachedSound()) {
            m_seHandle[slot].m_handle.detail_GetAttachedSound()->FadeIn(fadeIn);
        }
        m_seHandle[slot].m_handle.SetVolume(unk70C * m_seBalance, 0);
        m_seHandle[slot].m_handle.SetFxSend(nw4r::snd::AUX_A, m_effectVolume);
    }
}

void sndSystem::stopSE(s32 handleId, s32 fade) {
    stopSE1(seSlotFromId(this, handleId), fade);
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
            u32 next = seSlotFromId(this, m_seQueue[i]);
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
        u32 cur = seSlotFromId(this, m_curSeId);
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
        u32 next = seSlotFromId(this, m_seQueue[i]);
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
                int priority = seNewPriority(this, (SndID)sndId);
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

// Stops every sound effect that was played with this tag (a fighter's or a status' sound group).
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

// Stops the playing sounds with this tag and sound id.
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

// Lets every sound effect (except the tag 0x10000 ones) pause with the game; optionally lowers the music to half.
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
