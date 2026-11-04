#include <snd/snd_system.h>
#include <types.h>

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
