#define WN_KINETIC_MEDIATOR_INLINE_DTOR // BYTES: the null mediator's destructor is inlined into the kinetic module builder's
#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/peach/wn_peach_kassar.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnPeachKassar::wnPeachKassar(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKassarData* data) :
    wnWeaponBuilder<wnPeachKassarModuleAccesserBuildConfig>(
        "PeachKassar", articleId, info,
        wnModuleAccesserBuildData(0x5C, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// TODO(nonmatching): activate is 99.27%: only the callee-saved register numbers differ (the original gives unkC r31, this
// r30, the float constant's address r29 and team r28; this unit gives this r31, the constant r30, team r29 and unkC r28,
// i.e. the original colors unkC first). Statement order, a desc constructor, a local copy of unkC, a u32 parameter and
// two permuter runs did not change the allocation; wnMetaknightMantle::activate has the same body and the same problem.
// Also ruled out (identical bytes): an inline setter for unkC, desc declared before position, a static_cast<u32>, a
// comma temporary, a local copy of unkC declared first/after position/after desc, `register int unkC`, and
// `#pragma opt_propagation off` / `opt_lifetimes off` over the function. Swapping which parameter is stored last showed
// the colouring follows the parameter slot (r7 -> r29, r6 -> r28), not the live range, so the original must hold unkC in
// something MWCC colours before the parameters (a temporary or a non-coalesced local).
// TODO(link): besides activate, the unit cannot be linked yet. Linking the REL with this object instead of the original
// gives the same .text size but a different .data order: the original lays out the vtables of wnPeachKassar,
// wnWeaponBuilder<>, the parameter accesser, soParamCustomizeModuleImpl, soArticleMediatorNull, soKineticMediatorNull,
// soTeamImpl, soResourceIdAccesserImpl, IpNull and Input in that order and has no vtable of wnExtendParamAccesser<> or of
// the other bases; this unit emits the base destructors of the empty classes (wnExtendParamAccesser<>, wnKineticModuleImpl,
// soSound3dGeneratorAccesserImpl, the instance manager vectors, ...) out of line with their vtables, which shifts the order.
// The 9 weak vtable/RTTI objects of ft_peach.o that this unit also emits are named in symbols.txt so the linker merges them.
// Measured: the .data vtable order always equals the order of the weak virtual-member batches in .text (original:
// ParamCustomizeModuleImpl, ArticleMediatorNull, KineticMediatorNull, TeamImpl, ResourceIdAccesserImpl, IpNull, Input;
// ours: KineticMediatorNull, TeamImpl, ArticleMediatorNull, IpNull, ParamCustomizeModuleImpl, ResourceIdAccesserImpl,
// Input), and it is neither the constructor's vptr-store order nor the header include order (including the seven impl
// headers first changes nothing). The original also has no __vt__22soParamCustomizeModule (only its RTTI), which ours emits.
// A scratch link of ft_peach with this object (2026-11-10) has equal section sizes; .text first differs at the weak
// batches after soGetKind. Besides the batch order, the order inside two batches differs: this unit emits in-class
// virtuals in reverse declaration order, while the original has soParamCustomizeModuleImpl as
// getAttackPowerMulArticle, getParamIndefinite, getParamFloat, getParamInt, getWalkSpeedMul, getAttackReactionMul,
// isCustomize, removeCustomize, addCustomize, setDefault, activate, getAttackPowerMulScale, setAttackPowerMulScale,
// getAttackPowerMulPattern, setAttackPowerMulPattern, getAttackPowerMul, and soKineticMediatorNull with
// updateEnergy, updateEnergy1, updateEnergy2 in forward order. Both classes only override, so a unit-gated
// declaration order in their shared headers can give this without changing a vtable (the method that fixed the
// vector order of Peach's down special).
void wnPeachKassar::activate(int founderTaskId, int resourceId, int unkC, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = position;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unkC = unkC;
    Weapon::activate(&desc);
}
