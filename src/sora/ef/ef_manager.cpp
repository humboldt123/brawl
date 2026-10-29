#include <ef/ef_manager.h>
#include <ef/ef_particle.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <gm/gm_global.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8005D558();
extern "C" void fn_8005D5A8();
extern "C" float fn_8004E94C(GameGlobal* global);

using namespace nw4r::ef;

efPostRenderCallback lbl_805A0108;
efManager* lbl_805A0110;

void efManager::create() {
    lbl_805A0110 = new (Heaps::System) efManager();
}

efManager::efManager() : gfTask("EffectManager", Category_Effect, 0xc, 7, true) {
    unk138 = 0;
    unk40.m_effectSystem = EffectSystem::GetInstance();
    void* heap = gfHeapManager::alloc(Heaps::Effect, 0x456e0);
    unk12C = heap;
    MemoryManager* memoryManager = new (Heaps::System) MemoryManager(heap, 0x456e0, 0x80, 0x80, 0x100, 0x300);
    unk40.m_effectSystem->SetMemoryManager(memoryManager, 1);
    unk40.m_memoryManager = memoryManager;
    u8* buf = static_cast<u8*>(gfHeapManager::alloc(Heaps::Effect, 0x6000));
    unk130 = buf;
    gfMemFill(buf, 0, 0x6000);
    unk40.unk6C = buf;
    unk40.unk70 = buf + 0x3000;
    buf = static_cast<u8*>(gfHeapManager::alloc(Heaps::Effect, 0x2800));
    unk134 = buf;
    gfMemFill(buf, 0, 0x2800);
    unk40.unk64 = buf;
    unk40.unk68 = buf + 0x1400;
    unk40.unk74 = buf;
    unk40.unk78 = buf + 0x1400;
    unk40.unk7C = unk40.unk6C;
    unk40.unk80 = unk40.unk70;
    g_gfApplication->unk118.add(&lbl_805A0108);
    fn_8005D558();
    efParticle::setSlowParam(unk40.unk74, unk40.unk78, unk40.unk7C, unk40.unk80);
}

efManager::~efManager() {
    fn_8005D5A8();
    g_gfApplication->unk118.remove(&lbl_805A0108);
    gfHeapManager::free(unk12C);
    gfHeapManager::free(unk130);
    gfHeapManager::free(unk134);
    unk12C = NULL;
    unk130 = NULL;
    unk134 = NULL;
    lbl_805A0110 = NULL;
}

void* MemoryManager::AllocHeap(u32 size) {
    u32 headerSize = nw4r::ut::RoundUp(sizeof(MemInfo), 32);
    size = nw4r::ut::RoundUp(size, 32);

    for (MemInfo* pIt = mFreeMem; pIt != NULL; pIt = pIt->next) {
        if (pIt->size < size) {
            continue;
        }

        if (pIt->size < size + headerSize + 32) {
            if (pIt->prev != NULL) {
                pIt->prev->next = pIt->next;
            } else {
                mFreeMem = pIt->next;
            }

            if (pIt->next != NULL) {
                pIt->next->prev = pIt->prev;
            }

            if (mActiveMem != NULL) {
                mActiveMem->prev = pIt;
            }

            pIt->next = mActiveMem;
            pIt->prev = NULL;
            pIt->active = true;
            mActiveMem = pIt;
        } else {
            MemInfo* pNewFree = reinterpret_cast<MemInfo*>(
                (reinterpret_cast<u8*>(pIt) + headerSize + size));

            pNewFree->prev = pIt->prev;
            pNewFree->next = pIt->next;
            pNewFree->size = pIt->size - size - headerSize;
            pNewFree->active = false;
            pNewFree->chainNext = pIt->chainNext;

            if (pIt->chainNext != NULL) {
                pIt->chainNext->chainPrev = pNewFree;
            }

            pNewFree->chainPrev = pIt;
            pIt->chainNext = pNewFree;

            if (pIt->prev != NULL) {
                pIt->prev->next = pNewFree;
            } else {
                mFreeMem = pNewFree;
            }

            if (pIt->next != NULL) {
                pNewFree->next->prev = pNewFree;
            }

            if (mActiveMem != NULL) {
                mActiveMem->prev = pIt;
            }

            pIt->next = mActiveMem;
            pIt->prev = NULL;
            pIt->size = size;
            pIt->active = true;
            mActiveMem = pIt;
        }

        return reinterpret_cast<u8*>(pIt) + headerSize;
    }

    return NULL;
}

void efManager::processBegin() {
    unk40.m_isJustGameFrame = false;
    if (g_GameGlobal->isJustGameFrame()) {
        unk40.m_isJustGameFrame = true;
    }
    lbl_805A0100 = 1.0f / g_GameGlobal->getSlowRate();
    unk40.m_gameFrame = g_GameGlobal->getGameFrame();
    unk40.m_gameFrameDelta = fn_8004E94C(g_GameGlobal);
    if (0.0f == unk40.m_gameFrame) {
        unk40.unk40_b6 = true;
    }
    unk40.unk40_b4 = false;
    if (unk40.unk40_b6) {
        unk40.unk40_b4 = true;
        unk40.unk54 = -1.0f;
        unk40.unk40_b5 = false;
        unk40.unk48 = unk40.m_gameFrame;
        unk40.unk40_b6 = false;
    } else if (unk40.m_isJustGameFrame) {
        if ((u32)unk40.m_gameFrame > unk40.unk48) {
            unk40.unk40_b4 = true;
            unk40.unk48 = unk40.m_gameFrame;
        }
        unk40.unk40_b5 = false;
    } else if (unk40.unk48 != (u32)(1.0f + unk40.m_gameFrame)) {
        unk40.unk40_b4 = true;
        unk40.unk48++;
    }
}

void efManager::renderPre() { }

void efManager::processDebug() {
    EffectSystem::GetInstance();
}

void efManager::addEffect(efEffect* effect) {
    m_effectLists[effect->m_group].PushBack(effect);
}

void efManager::removeEffect(efEffect* effect) {
    m_effectLists[effect->m_group].Erase(effect);
}

void MemoryManager::FreeHeap(void* pPtr) {
    u32 headerSize = nw4r::ut::RoundUp(sizeof(MemInfo), 32);

    MemInfo* pInfo =
        reinterpret_cast<MemInfo*>(static_cast<u8*>(pPtr) - headerSize);

    if (pInfo->prev != NULL) {
        pInfo->prev->next = pInfo->next;
    } else {
        mActiveMem = pInfo->next;
    }

    if (pInfo->next != NULL) {
        pInfo->next->prev = pInfo->prev;
    }

    MemInfo* pPrev = NULL;
    if (pInfo->chainPrev != NULL && !pInfo->chainPrev->active) {
        pPrev = pInfo->chainPrev;
    }

    MemInfo* pNext = NULL;
    if (pInfo->chainNext != NULL && !pInfo->chainNext->active) {
        pNext = pInfo->chainNext;
    }

    if (pPrev != NULL) {
        if (pNext != NULL) {
            if (pNext->prev != NULL) {
                pNext->prev->next = pNext->next;
            } else {
                mFreeMem = pNext->next;
            }

            if (pNext->next != NULL) {
                pNext->next->prev = pNext->prev;
            }

            pPrev->size +=
                pNext->size + pInfo->size + headerSize + headerSize;

            pPrev->chainNext = pNext->chainNext;

            if (pNext->chainNext != NULL) {
                pNext->chainNext->chainPrev = pPrev;
            }
        } else {
            pPrev->size += pInfo->size + headerSize;
            pPrev->chainNext = pInfo->chainNext;

            if (pInfo->chainNext != NULL) {
                pInfo->chainNext->chainPrev = pPrev;
            }
        }
    } else if (pNext != NULL) {
        if (pNext->prev != NULL) {
            pNext->prev->next = pInfo;
        } else {
            mFreeMem = pInfo;
        }

        if (pNext->next != NULL) {
            pNext->next->prev = pInfo;
        }

        pInfo->prev = pNext->prev;
        pInfo->next = pNext->next;
        pInfo->size += pNext->size + headerSize;
        pInfo->active = false;
        pInfo->chainNext = pNext->chainNext;

        if (pNext->chainNext != NULL) {
            pNext->chainNext->chainPrev = pPrev;
        }
    } else {
        if (mFreeMem != NULL) {
            mFreeMem->prev = pInfo;
        }

        pInfo->next = mFreeMem;
        pInfo->prev = 0;
        pInfo->active = false;
        mFreeMem = pInfo;
    }
}

u32 MemoryManager::GetNumFreeParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocParticle() const {
    if (mParticleOM == NULL) {
        return 0;
    }
    return mParticleOM->GetNumAllocObject();
}

void MemoryManager::FreeParticle(void* pObject) {
    if (mParticleOM != NULL) {
        mParticleOM->Free(pObject);
    }
}

Particle* MemoryManager::AllocParticle() {
    if (mParticleOM == NULL) {
        return NULL;
    }
    return mParticleOM->Alloc();
}

u32 MemoryManager::GetNumFreeParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocParticleManager() const {
    if (mParticleManagerOM == NULL) {
        return 0;
    }
    return mParticleManagerOM->GetNumAllocObject();
}

void MemoryManager::FreeParticleManager(void* pObject) {
    if (mParticleManagerOM != NULL) {
        mParticleManagerOM->Free(pObject);
    }
}

ParticleManager* MemoryManager::AllocParticleManager() {
    if (mParticleManagerOM == NULL) {
        return NULL;
    }
    return mParticleManagerOM->Alloc();
}

u32 MemoryManager::GetNumFreeEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocEmitter() const {
    if (mEmitterOM == NULL) {
        return 0;
    }
    return mEmitterOM->GetNumAllocObject();
}

void MemoryManager::FreeEmitter(void* pObject) {
    if (mEmitterOM != NULL) {
        mEmitterOM->Free(pObject);
    }
}

Emitter* MemoryManager::AllocEmitter() {
    if (mEmitterOM == NULL) {
        return NULL;
    }
    return mEmitterOM->Alloc();
}

u32 MemoryManager::GetNumFreeEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumFreeObject();
}

u32 MemoryManager::GetNumActiveEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumActiveObject();
}

u32 MemoryManager::GetNumAllocEffect() const {
    if (mEffectOM == NULL) {
        return 0;
    }
    return mEffectOM->GetNumAllocObject();
}

void MemoryManager::FreeEffect(void* pObject) {
    if (mEffectOM != NULL) {
        mEffectOM->Free(pObject);
    }
}

Effect* MemoryManager::AllocEffect() {
    if (mEffectOM == NULL) {
        return NULL;
    }
    return mEffectOM->Alloc();
}

MemoryManager::~MemoryManager() { }

