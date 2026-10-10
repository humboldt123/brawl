#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_util.h>
#include <ft/zako/ft_zako.h>
#include <ft/zako/ft_zako_extend_param_accesser.h>

// The four Zako fighters share their module configurations, so one set of builder instantiations covers all of them.
#define FT_BC ftZakoBoyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftZakoBoyExtendParamAccesser g_ftZakoBoyExtendParamAccesser;
ftZakoGirlExtendParamAccesser g_ftZakoGirlExtendParamAccesser;
ftZakoChildExtendParamAccesser g_ftZakoChildExtendParamAccesser;
ftZakoBallExtendParamAccesser g_ftZakoBallExtendParamAccesser;

ftClassInfoImpl<Fighter_Zako_Boy, ftZakoBoy> g_ftClassInfoZakoBoy;
ftClassInfoImpl<Fighter_Zako_Girl, ftZakoGirl> g_ftClassInfoZakoGirl;
ftClassInfoImpl<Fighter_Zako_Child, ftZakoChild> g_ftClassInfoZakoChild;
ftClassInfoImpl<Fighter_Zako_Ball, ftZakoBall> g_ftClassInfoZakoBall;

#define FT_ZAKO_IMPL(NAME)                                                                  \
    ftZako##NAME::ftZako##NAME(s32 entryId,                                                 \
                               Heaps::HeapType instHeap,                                    \
                               Heaps::HeapType nwModelInstHeap,                             \
                               Heaps::HeapType nwMotionInstHeap) :                          \
        ftFighterBuilder<ftZako##NAME##BuildConfig>(entryId,                                \
                                                    Fighter_Zako_##NAME,                    \
                                                    instHeap,                               \
                                                    nwModelInstHeap,                        \
                                                    nwMotionInstHeap) {                     \
        m_commonData = g_ftCommonDataAccesser.getData(Fighter_Zako_##NAME);                 \
    }                                                                                       \
    bool ftZako##NAME::checkTransitionStatus(u32 status) {                                  \
        bool result = false;                                                                \
        if (Fighter::checkTransitionStatus(status)) {                                       \
            if (ftUtil::isValidStatusKindZako(m_moduleAccesser, status)) {                  \
                result = true;                                                              \
            }                                                                               \
        }                                                                                   \
        return result;                                                                      \
    }                                                                                       \
    bool ftZako##NAME::isHeartSwapEnableCondition() { return false; }                       \
    bool ftZako##NAME::setMetal(bool setStatus, float health, int unk3) { return false; }

FT_ZAKO_IMPL(Boy)
FT_ZAKO_IMPL(Girl)
FT_ZAKO_IMPL(Child)
FT_ZAKO_IMPL(Ball)

// Plays the common "other" animation layer and sets four work flags on activation.
// HYPOTHESIS: the flags are the invincibility/ground-state flags set by the common statuses; names unknown.
#define FT_ZAKO_ON_ACTIVATE(NAME)                                                           \
    void ftZako##NAME::onActivate() {                                                       \
        soModuleAccesser* acc = m_moduleAccesser;                                           \
        float one = 1.0f;                                                                   \
        acc->getMotionModule().addOtherAnim(0.0f, one, 1, 0x1CE, true);                     \
        acc->getMotionModule().setOtherAnimRate(one, 1);                                    \
        acc->getWorkManageModule().onFlag(0x12000021);                                     \
        acc->getWorkManageModule().onFlag(0x12000022);                                     \
        acc->getWorkManageModule().onFlag(0x12000023);                                     \
        acc->getWorkManageModule().onFlag(0x1200001D);                                     \
    }

FT_ZAKO_ON_ACTIVATE(Boy)
FT_ZAKO_ON_ACTIVATE(Girl)
FT_ZAKO_ON_ACTIVATE(Child)
FT_ZAKO_ON_ACTIVATE(Ball)

// MATCH-ONLY: the REL emits Fighter's empty onActivate (the vtable base entry); keep it alive.
#pragma dont_inline on
void ftZakoKeepFighterOnActivate(Fighter* fighter) { fighter->Fighter::onActivate(); }
#pragma dont_inline off

// FIXME: Test code present only to emit the shared builder functions
void testBuilder() {
    soInsideEventManageModuleBuilder<ftZakoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftZakoInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
bool fn_126_98C0(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_126_98C8(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
bool fn_126_98D8(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_126_98F0(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_126_9908(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_126_9918(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_126_9920(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_126_9930(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_126_9938(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_126_9940(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_126_9948(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_126_9950(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_126_9958(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_126_9960(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_126_9968(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_126_9970(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_126_9978(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_126_9980(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_126_9988(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_126_9990(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_126_9998(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_126_99A0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_126_99A8(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_126_98E0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_126_98E8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_126_98F8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_126_9900(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_126_9910(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_126_4EB4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void fn_126_8EFC() {}
} // extern "C"
