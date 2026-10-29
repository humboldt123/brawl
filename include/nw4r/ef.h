#pragma once
// HYPOTHESIS: minimal nw4r::ef declarations for the game's ef units (the BrawlHeaders have no
// nw4r ef headers); only the fields the game reads are spelled out.
#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <nw4r/math.h>
#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/ut/ut_list.h>
#include <types.h>
#include <new>

inline void* operator new[](size_t size, void* ptr) {
#pragma unused(size)
    return ptr;
}

namespace nw4r {
namespace ef {

class Effect;
class Emitter;
class Particle;
class ParticleManager;

typedef void (*ForEachFunc)(void* pObject, u32 param);

class LinkedObject {
public:
    ut::Link mMemoryLink; // 0x00
    u32 mObjectID;        // 0x08
};

class HandleBase {
public:
    u32 mObjectID;         // 0x00
    LinkedObject* mObject; // 0x04

    HandleBase();
    HandleBase& operator=(const HandleBase& rOther);
    bool IsValid() const;
    LinkedObject* GetPtr() const;
};

class Effect : public LinkedObject {
public:
    char _0C[0xA0 - 0x0C];

    Effect();
    ~Effect();
    void SetRootMtx(const math::MTX34& rMtx);
    u16 GetNumEmitter() const;
    Emitter* GetEmitter(u16 idx);
    void RetireEmitterAll();
    void RetireParticleAll();
    void ForeachParticleManager(ForEachFunc pFunc, u32 param, bool ignoreLifeStatus);
};
static_assert(sizeof(Effect) == 0xA0, "nw4r::ef::Effect size");

class Emitter : public LinkedObject {
public:
    s32 mLifeStatus;           // 0x0C
    char _10[0x20 - 0x10];
    u32 unk20;                 // 0x20
    char _24[0x90 - 0x24];
    Vec3f mRotate;             // 0x90
    Vec3f mScale;              // 0x9C
    Vec3f mTranslate;          // 0xA8
    char _B4[0x138 - 0xB4];

    Emitter();
    ~Emitter();
    void SetMtxDirty();
};
static_assert(sizeof(Emitter) == 0x138, "nw4r::ef::Emitter size");

class ParticleManager : public LinkedObject {
public:
    char _0C[0x40 - 0x0C];
    u16 mNumParticle; // 0x40
    char _42[0xBC - 0x42];

    ParticleManager();
    ~ParticleManager();
};
static_assert(sizeof(ParticleManager) == 0xBC, "nw4r::ef::ParticleManager size");

class Particle : public LinkedObject {
public:
    char _0C[0xE4 - 0x0C];

    Particle();
    ~Particle();
};
static_assert(sizeof(Particle) == 0xE4, "nw4r::ef::Particle size");

class MemoryManagerBase;

class EffectSystem {
public:
    MemoryManagerBase* mMemoryManager; // 0x00

    static EffectSystem* GetInstance();
    bool Initialize(u32 maxGroupID);
    void SetMemoryManager(MemoryManagerBase* pManager, u32 maxGroupID) {
        mMemoryManager = pManager;
        if (mMemoryManager != NULL) {
            Initialize(maxGroupID);
        }
    }
    void SetProcessCamera(const math::VEC3& rPos, const math::MTX34& rViewMtx, float near, float far);
    void RetireEffect(Effect* pEffect);
};

class MemoryManagerBase {
public:
    MemoryManagerBase() { }
    virtual ~MemoryManagerBase() { }
    virtual void GarbageCollection() = 0;
    virtual Effect* AllocEffect() = 0;
    virtual void FreeEffect(void* pObject) = 0;
    virtual u32 GetNumAllocEffect() const = 0;
    virtual u32 GetNumActiveEffect() const = 0;
    virtual u32 GetNumFreeEffect() const = 0;
    virtual Emitter* AllocEmitter() = 0;
    virtual void FreeEmitter(void* pObject) = 0;
    virtual u32 GetNumAllocEmitter() const = 0;
    virtual u32 GetNumActiveEmitter() const = 0;
    virtual u32 GetNumFreeEmitter() const = 0;
    virtual ParticleManager* AllocParticleManager() = 0;
    virtual void FreeParticleManager(void* pObject) = 0;
    virtual u32 GetNumAllocParticleManager() const = 0;
    virtual u32 GetNumActiveParticleManager() const = 0;
    virtual u32 GetNumFreeParticleManager() const = 0;
    virtual Particle* AllocParticle() = 0;
    virtual void FreeParticle(void* pObject) = 0;
    virtual u32 GetNumAllocParticle() const = 0;
    virtual u32 GetNumActiveParticle() const = 0;
    virtual u32 GetNumFreeParticle() const = 0;
    virtual void* AllocHeap(u32 size) = 0;
    virtual void FreeHeap(void* pPtr) = 0;
};

template <typename T> class MemoryManagerTmp {
public:
    T* mHead;             // 0x00
    ut::List mFreeList;   // 0x04
    ut::List mLeasedList; // 0x10
    int mStructSize;      // 0x1C

    static const u32 OBJECT_ID_BASE = 0x10000;

    MemoryManagerTmp(int count, T* heap) {
        ut::List_Init(&mFreeList, 0);
        ut::List_Init(&mLeasedList, 0);
        mStructSize = sizeof(T);
        mHead = heap;
        for (u32 i = 0; i < count; i++) {
            heap[i].mObjectID = i;
            ut::List_Append(&mFreeList, &heap[i]);
        }
    }

    virtual void AssignObjectID(void* pObj) {
        static_cast<LinkedObject*>(pObj)->mObjectID += OBJECT_ID_BASE;
    }

    T* Alloc() {
        void* pPtr = ut::List_GetFirst(&mFreeList);
        if (pPtr == NULL) {
            return NULL;
        }
        AssignObjectID(pPtr);
        ut::List_Remove(&mFreeList, pPtr);
        ut::List_Append(&mLeasedList, pPtr);
        return static_cast<T*>(pPtr);
    }

    void Free(void* pObj) {
        ut::List_Remove(&mLeasedList, pObj);
        ut::List_Append(&mFreeList, pObj);
    }

    u32 GetNumAllocObject() const { return GetNumActiveObject() + GetNumFreeObject(); }
    u32 GetNumActiveObject() const { return ut::List_GetSize(&mLeasedList); }
    u32 GetNumFreeObject() const { return ut::List_GetSize(&mFreeList); }
};

// HYPOTHESIS: member names from the Wii Sports decompilation (ogws ef_memorymanagerimpl.h).
class MemoryManager : public MemoryManagerBase {
public:
    struct MemInfo {
        MemInfo* prev;      // 0x00
        MemInfo* next;      // 0x04
        MemInfo* chainPrev; // 0x08
        MemInfo* chainNext; // 0x0C
        u32 size;           // 0x10
        bool active;        // 0x14
        u8 PADDING_0x15[3]; // 0x15
    };

    int mLeastEffect;                                      // 0x04
    int mMaxEffect;                                        // 0x08
    MemoryManagerTmp<Effect>* mEffectOM;                   // 0x0C
    int mLeastEmitter;                                     // 0x10
    int mMaxEmitter;                                       // 0x14
    MemoryManagerTmp<Emitter>* mEmitterOM;                 // 0x18
    int mLeastParticleManager;                             // 0x1C
    int mMaxParticleManager;                               // 0x20
    MemoryManagerTmp<ParticleManager>* mParticleManagerOM; // 0x24
    int mLeastParticle;                                    // 0x28
    int mMaxParticle;                                      // 0x2C
    MemoryManagerTmp<Particle>* mParticleOM;               // 0x30
    void* mHeapStartAddr;                                  // 0x34
    void* mHeapEndAddr;                                    // 0x38
    MemInfo* mActiveMem;                                   // 0x3C
    MemInfo* mFreeMem;                                     // 0x40
    MemInfo* mFreeMemTail;                                 // 0x44
    MemInfo* mAllChain;                                    // 0x48

    MemoryManager(void* pStartAddr, u32 size, int maxEffect, int maxEmitter,
                  int maxParticleManager, int maxParticle) {
        u32 headerSize = ut::RoundUp<u32>(sizeof(MemInfo), 32);

        mLeastEffect = 0;
        mLeastEmitter = 0;
        mLeastParticleManager = 0;
        mLeastParticle = 0;

        mMaxEffect = 0;
        mMaxEmitter = 0;
        mMaxParticleManager = 0;
        mMaxParticle = 0;

        mEffectOM = NULL;
        mEmitterOM = NULL;
        mParticleManagerOM = NULL;
        mParticleOM = NULL;

        mActiveMem = NULL;
        mFreeMem = NULL;
        mFreeMemTail = NULL;
        mAllChain = NULL;

        mHeapEndAddr = static_cast<u8*>(pStartAddr) + size;
        mHeapStartAddr = reinterpret_cast<u8*>(ut::RoundUp<u32>(reinterpret_cast<u32>(pStartAddr), 32));

        mLeastEffect = maxEffect;
        mLeastEmitter = maxEmitter;
        mLeastParticleManager = maxParticleManager;
        mLeastParticle = maxParticle;

        mMaxEffect = mLeastEffect;
        mMaxEmitter = mLeastEmitter;
        mMaxParticleManager = mLeastParticleManager;
        mMaxParticle = mLeastParticle;

        mFreeMem = static_cast<MemInfo*>(mHeapStartAddr);
        mFreeMem->prev = NULL;
        mFreeMem->next = NULL;
        mFreeMem->chainPrev = NULL;
        mFreeMem->chainNext = NULL;
        mFreeMem->active = false;
        mFreeMem->size = static_cast<u8*>(mHeapEndAddr) - static_cast<u8*>(mHeapStartAddr) - headerSize;

        mAllChain = mFreeMem;
        mActiveMem = NULL;

        int numEffects = mLeastEffect;
        Effect* pEffects = NULL;
        if (numEffects > 0) {
            pEffects = new (AllocHeap(sizeof(Effect) * numEffects + 32)) Effect[numEffects];
        }
        mEffectOM = new (AllocHeap(sizeof(MemoryManagerTmp<Effect>)))
            MemoryManagerTmp<Effect>(mLeastEffect, pEffects);

        int numEmitters = mLeastEmitter;
        Emitter* pEmitters = NULL;
        if (numEmitters > 0) {
            pEmitters = new (AllocHeap(sizeof(Emitter) * numEmitters + 32)) Emitter[numEmitters];
        }
        mEmitterOM = new (AllocHeap(sizeof(MemoryManagerTmp<Emitter>)))
            MemoryManagerTmp<Emitter>(mLeastEmitter, pEmitters);

        int numParticleManagers = mLeastParticleManager;
        ParticleManager* pParticleManagers = NULL;
        if (numParticleManagers > 0) {
            pParticleManagers = new (AllocHeap(sizeof(ParticleManager) * numParticleManagers + 32))
                ParticleManager[numParticleManagers];
        }
        mParticleManagerOM = new (AllocHeap(sizeof(MemoryManagerTmp<ParticleManager>)))
            MemoryManagerTmp<ParticleManager>(mLeastParticleManager, pParticleManagers);

        int numParticles = mLeastParticle;
        Particle* pParticles = NULL;
        if (numParticles > 0) {
            pParticles = new (AllocHeap(sizeof(Particle) * numParticles + 32)) Particle[numParticles];
        }
        mParticleOM = new (AllocHeap(sizeof(MemoryManagerTmp<Particle>)))
            MemoryManagerTmp<Particle>(mLeastParticle, pParticles);
    }

    virtual ~MemoryManager();
    virtual void GarbageCollection();
    virtual Effect* AllocEffect();
    virtual void FreeEffect(void* pObject);
    virtual u32 GetNumAllocEffect() const;
    virtual u32 GetNumActiveEffect() const;
    virtual u32 GetNumFreeEffect() const;
    virtual Emitter* AllocEmitter();
    virtual void FreeEmitter(void* pObject);
    virtual u32 GetNumAllocEmitter() const;
    virtual u32 GetNumActiveEmitter() const;
    virtual u32 GetNumFreeEmitter() const;
    virtual ParticleManager* AllocParticleManager();
    virtual void FreeParticleManager(void* pObject);
    virtual u32 GetNumAllocParticleManager() const;
    virtual u32 GetNumActiveParticleManager() const;
    virtual u32 GetNumFreeParticleManager() const;
    virtual Particle* AllocParticle();
    virtual void FreeParticle(void* pObject);
    virtual u32 GetNumAllocParticle() const;
    virtual u32 GetNumActiveParticle() const;
    virtual u32 GetNumFreeParticle() const;
    virtual void* AllocHeap(u32 size);
    virtual void FreeHeap(void* pPtr);
};
static_assert(sizeof(MemoryManager) == 0x4C, "nw4r::ef::MemoryManager size");

} // namespace ef
} // namespace nw4r
