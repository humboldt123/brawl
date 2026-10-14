// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <so/anim/so_anim_obj.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_physics_iface.h>
#include <ft/ft_class_info_impl.h>
#include <ft/gamewatch/ft_gamewatch.h>
#include <ft/gamewatch/ft_gamewatch_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <so/team/so_team.h>
#include <so/param/so_param_customize_module_impl.h>

#define FT_NO_REFLECTOR_INSTANTIATION
#define FT_BC ftGameWatchBuildConfig
#include <ft/builder/ft_builder_noinline.h>

#pragma dont_inline on
template soCollisionShieldModuleBuilder<ftGameWatchCollisionReflectorBaseModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
template soCollisionShieldModuleBuilder<ftGameWatchCollisionAbsorberModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
#pragma dont_inline off

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_107_bss_15C;
extern soStatusUniqProcess lbl_107_bss_16C;
extern soStatusUniqProcess lbl_107_bss_1BC;
extern soStatusUniqProcess lbl_107_bss_17C;
extern soStatusUniqProcess lbl_107_bss_19C;
extern soStatusUniqProcess lbl_107_bss_1AC;
extern soStatusUniqProcess lbl_107_bss_1CC;
extern soStatusUniqProcess lbl_107_bss_1DC;
extern soStatusUniqProcess lbl_107_bss_1EC;
extern soStatusUniqProcess lbl_107_bss_1FC;
extern soStatusUniqProcess lbl_107_bss_20C;
extern soStatusUniqProcess lbl_107_bss_21C;
extern soStatusUniqProcess lbl_107_bss_22C;
extern soStatusUniqProcess lbl_107_bss_23C;
}
extern soStatusUniqProcess g_ftGamewatchStatusUniqProcessSpecialLw;

// HYPOTHESIS: the turn module's second interface is not declared yet; the constructor calls slot 0x30 of its vtable with 1
// (MATCH-ONLY placeholder virtuals put the called slot where the original has it).
struct ftTurnIface {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10(int arg);
};
static inline void ftTurnModuleCall30(void* module, int arg) {
    static_cast<ftTurnIface*>(module)->slot10(arg);
}

ftGameWatchExtendParamAccesser g_ftGameWatchExtendParamAccesser;

ftClassInfoImpl<Fighter_GameWatch, ftGameWatch> g_ftClassInfoGameWatch;

ftGameWatch::ftGameWatch(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftGameWatchBuildConfig>(entryId,
                                         Fighter_GameWatch,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_GameWatch)), m_animObj() {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[39] = {0};
    processes[1] = &lbl_107_bss_15C;
    processes[2] = &lbl_107_bss_16C;
    processes[4] = &lbl_107_bss_1BC;
    processes[5] = &lbl_107_bss_17C;
    processes[7] = &g_ftGamewatchStatusUniqProcessSpecialLw;
    processes[8] = &lbl_107_bss_19C;
    processes[9] = &lbl_107_bss_1AC;
    processes[12] = &lbl_107_bss_1CC;
    processes[13] = &lbl_107_bss_1DC;
    processes[14] = &lbl_107_bss_1EC;
    processes[15] = &lbl_107_bss_1FC;
    processes[16] = &lbl_107_bss_20C;
    processes[17] = &lbl_107_bss_20C;
    processes[18] = &lbl_107_bss_21C;
    processes[25] = &lbl_107_bss_22C;
    processes[26] = &lbl_107_bss_22C;
    processes[27] = &lbl_107_bss_23C;
    processes[28] = &lbl_107_bss_23C;
    processes[29] = &lbl_107_bss_23C;
    processes[30] = &lbl_107_bss_1CC;
    processes[31] = &lbl_107_bss_1DC;
    processes[32] = &lbl_107_bss_1EC;
    processes[33] = &lbl_107_bss_1FC;
    processes[34] = &lbl_107_bss_20C;
    processes[35] = &lbl_107_bss_20C;
    processes[36] = &lbl_107_bss_21C;
    processes[37] = &lbl_107_bss_23C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 39);
    // HYPOTHESIS: the common data record (+0x8c) holds a sphere (node index as a float, centre, radius) that is installed as the
    // shape of the absorber (the bucket of Oil Panic), group 0.
    const float* sphere = *reinterpret_cast<const float**>(reinterpret_cast<u8*>(m_data) + 0x8c);
    soCollisionShieldData shape;
    soCollisionShieldGroupData group;
    shape.setOffset(0, sphere[1]);
    shape.setOffset(1, sphere[2]);
    shape.setOffset(2, sphere[3]);
    shape.setOffset(3, sphere[1]);
    shape.setOffset(4, sphere[2]);
    shape.setOffset(5, sphere[3]);
    shape.m_size = sphere[4];
    shape.m_nodeIndex = (u32)sphere[0];
    shape.m_shapeType = 1;
    group.m_flags = 0;
    group.m_shieldDataSet = soSet<soCollisionShieldData>(&shape, 1);
    soCollisionShieldModule& absorber = m_moduleAccesser->getCollisionAbsorberModule();
    absorber.add(&group, 0);
    absorber.setStatus(0, 0, 0);
    ftTurnModuleCall30(m_moduleAccesser->m_enumerationStart->m_turnModule, 1);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftGameWatch is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftGameWatchInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftGameWatchInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;



// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

// ftGameWatch::getExtendParam: pointer loaded from the object tail, plus 0x7C.
u8* fn_107_A8C4(u8* p) { return *(u8**)(p + 0x1FDB8) + 0x7C; }

// ftGameWatch::onDeactivate: two sub-object teardown calls on the same member.
void fn_107_AB10(u8* p) {
    reinterpret_cast<soAnimObj*>(p + 0x1FDBC)->remove();
    reinterpret_cast<soAnimObj*>(p + 0x1FDBC)->destroy();
}

u8 fn_107_8720(u8* p) { return *(u8*)(p + 0x4); }
void fn_107_A810() {}
void fn_107_B648() {}
int fn_107_BEAC() { return 0x0; }
void fn_107_BFB8() {}
void fn_107_C140() {}
u8 fn_107_C144(u8* p) { return *(u8*)(p + 0x44); }
void fn_107_C14C() {}
void fn_107_C18C() {}
void fn_107_C190() {}
void fn_107_C220() {}
void fn_107_C24C() {}
void fn_107_C250() {}
void fn_107_C254() {}
void fn_107_C258() {}
void fn_107_C25C() {}
void fn_107_C260() {}
void fn_107_C264() {}
void fn_107_C268() {}
void fn_107_C26C() {}
void fn_107_C270() {}
void fn_107_C274() {}
void fn_107_C278() {}
void fn_107_C27C() {}
void fn_107_C280() {}
void fn_107_C284() {}
int fn_107_C288() { return 0x0; }
void fn_107_C290() {}
void fn_107_C294() {}
void fn_107_C298() {}
void fn_107_C29C() {}
void fn_107_C2C8() {}
void fn_107_C2CC() {}
void fn_107_C2D0() {}
void fn_107_C2D4() {}
void fn_107_C2D8() {}
int fn_107_C2DC(u8* p) { return *(int*)(p + 0x110); }
int fn_107_C2F4() { return 0x0; }
void fn_107_C2FC() {}
void fn_107_C300() {}
int fn_107_C304() { return 0x0; }
int fn_107_C30C() { return 0x0; }
void fn_107_C314() {}
void fn_107_C318() {}
void fn_107_C31C() {}
void fn_107_C320() {}
void fn_107_C324() {}
void fn_107_C328() {}
int fn_107_C49C(u8* p) { return *(int*)(p + 0x20); }
int fn_107_C578(u8* p) { return *(int*)(p + 0x18); }
int fn_107_C610(u8* p) { return *(int*)(p + 0x10); }
int fn_107_C6D0() { return 0x0; }
int fn_107_DFFC() { return 0x7; }
void fn_107_EEC8() {}
void fn_107_F5D4() {}
int fn_107_F610() { return 0x8; }
void fn_107_F658() {}
int fn_107_F704() { return 0x0; }
void fn_107_F740() {}
int fn_107_F7EC() { return 0x0; }
void fn_107_F828() {}
int fn_107_F8D4() { return 0x0; }
void fn_107_F910() {}
int fn_107_F9BC() { return 0x0; }
void fn_107_F9F8() {}
int fn_107_FAA4() { return 0x0; }
void fn_107_FAE0() {}
int fn_107_FB8C() { return 0x0; }
void fn_107_FBC8() {}
int fn_107_FC74() { return 0x0; }
void fn_107_FCB0() {}
int fn_107_FD5C() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_107_FFAC(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_107_FFB4(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_107_FFBC(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
void fn_107_FFCC(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
bool fn_107_10004(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_107_1000C(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_107_10014(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_107_1001C(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_107_10034(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_107_1004C(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_107_1005C(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_107_10064(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_107_1006C(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_107_10074(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_107_1007C(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_107_10084(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_107_1008C(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_107_10094(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_107_1009C(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_107_100A4(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_107_100AC(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_107_100B4(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_107_100BC(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_107_100C4(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_107_100D4(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_107_100DC(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_107_100E4(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_107_100EC(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_107_100F4(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
void fn_107_FFC4(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
bool fn_107_FFDC(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_107_10024(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_107_1002C(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_107_1003C(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_107_10044(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_107_1F34(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
extern "C" {
void* fn_107_3DC4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_107_4158(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
// Article specialization also declared by the weapon-builder header, which
// cannot coexist with this Fighter-based builder translation unit.
template <>
class soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> > {
    soParamCustomizeModuleImpl m_paramCustomizeModule;
public:
    soParamCustomizeModuleBuilder(soModuleAccesser*) { }
    ~soParamCustomizeModuleBuilder();
};
soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> >::~soParamCustomizeModuleBuilder() { }
soArticleMediator::~soArticleMediator() { }
extern "C" {
void* fn_107_6800(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_107_6840(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"
