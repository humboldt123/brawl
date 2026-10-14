#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

// The Pikmin article of Olimar's final smash (End of Day): a Dolfin-type article that carries Olimar and the Pikmin through the
// stage. Its status flow is 1 (rise), 2 (final attack), 3 (descent). HYPOTHESIS: all field names; offsets follow the native code.

// The parameter block of the Dolfin article. HYPOTHESIS: eight floats and three ints (ids 4000-4010), laid out like the native block.
struct wnPikminDolfinParam {
    float unk0, unk4, unk8;
    s32 unkC, unk10;
    float unk14, unk18, unk1C, unk20;
    s32 unk24;
    float unk28;
};
static_assert(sizeof(wnPikminDolfinParam) == 0x2C, "Dolfin parameter block extent");

class wnPikminDolfinParamAccesser : public wnExtendParamAccesser<3999, 8, 23999, 3, 0, 0> {
public:
    wnPikminDolfinParamAccesser(const wnPikminDolfinParam* param) : wnExtendParamAccesser<3999, 8, 23999, 3, 0, 0>(0x81) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unk14;
        m_floats[4] = &param->unk18;
        m_floats[5] = &param->unk1C;
        m_floats[6] = &param->unk20;
        m_floats[7] = &param->unk28;
        m_ints[0] = &param->unkC;
        m_ints[1] = &param->unk10;
        m_ints[2] = &param->unk24;
    }
};
static_assert(sizeof(wnPikminDolfinParamAccesser) == 0x54, "Dolfin parameter accesser extent");

struct wnPikminDolfinModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig> : public Weapon {
    // The module storage of the generic article builder (the builder is reconstructed with the constructors).
    u8 unkCC[0x23C4 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig>) == 0x23C4, "Dolfin builder extent");

class wnPikminDolfin : public wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig> {
public:
    const wnPikminDolfinParam* m_param;           // 0x23C4
    wnPikminDolfinParamAccesser m_paramAccesser;  // 0x23C8
    Vec3f m_startPos;                              // 0x241C: position saved when the article rises

    void activate(int founderTaskId, int kind, int team, Vec3f* pos);
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void processFixPosition();
    virtual bool notifyEventCollisionAttackCheck();
};
static_assert(sizeof(wnPikminDolfin) == 0x2428, "Dolfin article extent");

// Kinetic transactor of the Dolfin article. HYPOTHESIS: like the other article transactors, the original passes an empty
// one-byte object as the implicit this of the changeKineticSub* helpers; `pools` is the opaque energy-pool storage (the
// normal energy sits at +0x10).
class wnPikminDolfinKineticTransactor {
    u8 unk0;

public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);

    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub1(void* pools, soModuleAccesser* moduleAccesser);
};
