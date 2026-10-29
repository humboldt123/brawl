#include <ef/ef_manager.h>
#include <ef/ef_particle.h>
#include <gm/gm_global.h>
#include <math.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

using namespace nw4r::ef;

extern "C" Emitter* fn_801643C8(const Effect* effect);

void* lbl_805A0118[2];
void* lbl_805A0120[2];
efParticle::EmitterBackup* lbl_805A0128;
efParticle::EmitterBackup* lbl_805A012C;
// HYPOTHESIS: untyped placeholders for the rest of this unit's .sbss, sizes and order from the original layout.
unsigned char lbl_805A0130[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0134[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0138[0x8] __attribute__((aligned(4))) = { 0 };

void efParticle::setSlowParam(void* p1, void* p2, void* p3, void* p4) {
    lbl_805A0118[0] = p1;
    lbl_805A0118[1] = p2;
    lbl_805A0120[0] = p3;
    lbl_805A0120[1] = p4;
}

efParticle* efParticle::create(const char* name, u8 group) {
    return new (Heaps::Effect) efParticle(name, group);
}

efParticle::efParticle(const char* name, u8 group) : efEffect(name, group) {
    unk54_b7 = false;
    unk54_b6 = false;
    unk54_b5 = false;
    unk54_b4 = false;
    unk54_b3 = false;
    unk55 = 0;
    unk58 = 0;
    unk5C = 0.0f;
    unk54_b6 = true;
    if (g_GameGlobal->isJustGameFrame()) {
        float frame = g_GameGlobal->getGameFrame();
        m_frame = frame - (float)floor(frame);
    }
    if (strcmp(name, "PtcFinIkeCutFire") == 0) {
        unk54_b3 = true;
        efManager::getInstance()->unk138++;
    }
}

efParticle::~efParticle() {
    if (m_handle.IsValid()) {
        Effect* effect;
        EffectSystem* system = EffectSystem::GetInstance();
        if (unk54_b3 == true) {
            efManager* manager = efManager::getInstance();
            if (manager->unk138 > 0) {
                manager->unk138--;
            }
        }
        effect = static_cast<Effect*>(m_handle.GetPtr());
        effect->RetireParticleAll();
        effect->RetireEmitterAll();
        system->RetireEffect(effect);
        HandleBase empty;
        m_handle = empty;
    }
}

void efParticle::endCallback() {
    if (m_handle.IsValid()) {
        Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
        if (effect->GetNumEmitter()) {
            effect->RetireEmitterAll();
        }
    }
}

bool efParticle::isAlive() {
    bool alive = false;
    if (m_handle.IsValid()) {
        Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
        u16 num = effect->GetNumEmitter();
        for (u16 i = 0; i < num; i++) {
            if (effect->GetEmitter(i)->mLifeStatus == 1) {
                alive = true;
                break;
            }
        }
        if (!alive) {
            efParticleCounter counter;
            if (counter.count(effect)) {
                alive = true;
            }
        }
    }
    return alive;
}

void efParticle::stopEmitter() {
    if (m_handle.IsValid()) {
        if (getState() == 4) {
            m_state = 2;
        }
        static_cast<Effect*>(m_handle.GetPtr())->RetireEmitterAll();
    }
}

void efParticle::updateRootMatrix() {
    Effect* effect = static_cast<Effect*>(m_handle.GetPtr());
    if (fn_801643C8(effect) == NULL) {
        m_state = 3;
        if (unk54_b7) {
            end();
        }
    } else {
        Matrix mtx(true);
        getWorldMatrix(&mtx);
        effect->SetRootMtx(mtx);
    }
}

void efParticle::foreachEmitterBackup(void* emitter, u32 param) {
    Emitter* em = static_cast<Emitter*>(emitter);
    EmitterBackup* backup = &lbl_805A012C[static_cast<u16>(em->mObjectID)];
    backup->unk24 = em->unk20;
    backup->m_trans = em->mTranslate;
    backup->m_rot = em->mRotate;
    backup->m_scale = em->mScale;
}

void efParticle::foreachEmitterRestore(void* emitter, u32 param) {
    Emitter* em = static_cast<Emitter*>(emitter);
    EmitterBackup* backup = &lbl_805A012C[static_cast<u16>(em->mObjectID)];
    em->unk20 = backup->unk24;
    em->mTranslate = backup->m_trans;
    em->mRotate = backup->m_rot;
    em->mScale = backup->m_scale;
    em->SetMtxDirty();
}

void efParticleCounter::foreachPM(void* particleManager, u32 counter) {
    reinterpret_cast<efParticleCounter*>(counter)->m_count +=
        static_cast<ParticleManager*>(particleManager)->mNumParticle;
}

u32 efParticleCounter::count(Effect* effect) {
    m_count = 0;
    effect->ForeachParticleManager(foreachPM, reinterpret_cast<u32>(this), true);
    return m_count;
}
