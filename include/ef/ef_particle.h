#pragma once
#include <StaticAssert.h>
#include <ef/ef_effect.h>
#include <nw4r/ef.h>
#include <types.h>

class efParticle : public efEffect {
public:
    nw4r::ef::HandleBase m_handle; // 0x4C
    bool unk54_b7 : 1;             // 0x54
    bool unk54_b6 : 1;
    bool unk54_b5 : 1;
    bool unk54_b4 : 1;
    bool unk54_b3 : 1;             
    u8 unk55;                      // 0x55
    u32 unk58;                     // 0x58
    float unk5C;                   // 0x5C

    // HYPOTHESIS: type name; one record per emitter object ID in the lbl_805A012C buffer.
    struct EmitterBackup {
        Vec3f m_trans;  // 0x00
        Vec3f m_rot;    // 0x0C
        Vec3f m_scale;  // 0x18
        u32 unk24;      // 0x24
    };

    static efParticle* create(const char* name, u8 group);
    efParticle(const char* name, u8 group);
    virtual ~efParticle();
    virtual void update(float frames);
    virtual void updateRootMatrix();
    virtual void render();
    virtual void endCallback();

    // HYPOTHESIS: parameter types; the four buffers come from efManager's effect heaps.
    static void setSlowParam(void* p1, void* p2, void* p3, void* p4);
    bool isAlive();
    void stopEmitter();
    static void foreachEmitterBlend(void* emitter, u32 param);
    static void foreachEmitterBackup(void* emitter, u32 param);
    static void foreachEmitterRestore(void* emitter, u32 param);
    static void foreachParticleManager(void* particleManager, u32 param);
};
static_assert(sizeof(efParticle) == 0x60, "efParticle size");

class efParticleCounter {
public:
    u32 m_count; // 0x00

    static void foreachPM(void* particleManager, u32 counter);
    // MATCH-ONLY: efParticleCounter is its own original object (ef_particle_counter.o),
    // so isAlive calls count out of line.
    u32 count(nw4r::ef::Effect* effect) __attribute__((never_inline));
};

extern void* lbl_805A0118[2];
extern void* lbl_805A0120[2];
extern efParticle::EmitterBackup* lbl_805A0128; // HYPOTHESIS: the blend source records
extern efParticle::EmitterBackup* lbl_805A012C;
