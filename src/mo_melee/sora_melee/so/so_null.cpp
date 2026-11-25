#define SO_NULL_CONTROLLER_FULL
#define SO_PHYSICS_MODULE_IMPL_FULL
#define SO_TERRITORY_MODULE_IMPL_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define SO_ARTICLE_DTOR_INLINE
#define IT_RESOURCE_ACCESSER_DTOR_INLINE
#define SO_KINETIC_ENERGY_INLINE_CTOR
#define SO_KINETIC_ENERGY_INLINE_VIRTUALS
#include <so/null/so_camera_module_null.h>
#include <so/team/so_team_module.h>
#include <so/article/so_article_mediator.h>
#include <so/null/so_glow_module_null.h>
#include <so/null/so_item_manage_module_null.h>
#include <so/null/so_model_module_null.h>
#include <so/null/so_jostle_module_null.h>
#include <so/model/so_model_event_presenter.h>
#include <so/null/so_reflect_module_null.h>
#include <so/null/so_slow_module_null.h>
#include <so/null/so_transition_module_null.h>
#include <so/null/so_item_pick_transactor_null.h>
#include <so/null/so_item_search_null.h>
#include <so/shadow/so_shadow_module_impl.h>
#include <so/null/so_slope_module_null.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/null/so_physics_module_null.h>
#include <so/null/so_target_search_module_null.h>
#include <so/null/so_territory_module_null.h>
#include <so/null/so_area_enviroment_element_checker_null.h>
#include <so/null/so_area_module_null.h>
#include <so/null/so_combo_module_null.h>
#include <so/null/so_debug_module_null.h>
#include <so/null/so_work_manage_module_null.h>
#include <so/so_value_accesser.h>
#include <so/null/so_effect_module_null.h>
#include <so/null/so_generate_article_manage_module_null.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/null/so_motion_module_null.h>
#include <so/null/so_visibility_module_null.h>
#include <so/null/so_sound_module_null.h>
#include <so/null/so_shake_module_null.h>
#include <so/null/so_stop_module_null.h>
#include <so/null/so_capture_module_null.h>
#include <so/null/so_catch_module_null.h>
#include <so/null/so_damage_module_null.h>
#include <so/null/so_collision_search_module_null.h>
#include <so/null/so_collision_catch_module_null.h>
#include <so/null/so_collision_shield_module_null.h>
#include <so/null/so_collision_hit_module_null.h>
#include <so/null/so_collision_attack_module_null.h>
#include <so/null/so_situation_module_null.h>
#include <so/null/so_posture_module_null.h>
#include <so/null/so_ground_tracer_null.h>
#include <so/null/so_ground_module_null.h>
#include <so/null/so_camera_module_null.h>
#include <so/null/so_controller_module_null.h>
#include <so/event/so_event_presenter.h>
#include <so/null/so_event_manage_module_null.h>
#include <so/null/so_event_system_null.h>
#include <so/null/so_link_module_null.h>
#include <so/null/so_article_null.h>
#include <so/null/so_motion_anim_obj_cache_module_null.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/so_heap_module.h>
#include <so/null/so_abnormal_module_null.h>
#include <so/null/so_team_module_null.h>
#include <so/article/so_article_mediator_null.h>
#include <so/null/so_kinetic_mediator_null.h>

int soSoundIdExchangerNull::getLandingId(int landingId, soModuleAccesser* moduleAccesser) { return landingId; }
int soSoundIdExchangerNull::getStepId(int stepId, soModuleAccesser* moduleAccesser) { return stepId; }
SndID soSoundIdExchangerNull::getId(SndID sndID, soModuleAccesser* moduleAccesser) { return sndID; }
soSoundIdExchangerNull::~soSoundIdExchangerNull() { }

bool soMotionAnimObjCacheModuleNull::isContainAnimChr(int) { return false; }
bool soMotionAnimObjCacheModuleNull::setCacheAnimChr(int, void*, int) { return false; }
void* soMotionAnimObjCacheModuleNull::getCacheAnimChr(int) { return NULL; }

void soGlowModuleNull::dbgReport() { }
void soGlowModuleNull::setRefLinkNo(s32) { }
float soGlowModuleNull::getPower(float power, soCollisionAttackData*, soGlowModule*) { return power; }
float soGlowModuleNull::getPower(float power, soCollisionAttackData*, int) { return power; }
void soGlowModuleNull::setGlowDefenseAttr(int) { }
void soGlowModuleNull::setWeaponGlowAttack(int) { }
int soGlowModuleNull::getGlowAttackAdd(soCollisionAttackData*) { return 0; }
void soGlowModuleNull::addGlowAttackStatus(int) { }
void soGlowModuleNull::setGlowAttackStatus(int) { }
void soGlowModuleNull::clearGlowAttackStatus() { }
void soGlowModuleNull::setGlowAttackRegion(int) { }
void soGlowModuleNull::setGlowAttackAttr(int) { }
int soGlowModuleNull::getGlowAttackMax() { return 999; }
void soGlowModuleNull::setGlowAttackMax(int) { }
float soGlowModuleNull::getGlowPowerMax() { return 0.0f; }
float soGlowModuleNull::getGlowPowerMin() { return 0.0f; }
float soGlowModuleNull::getGlowPowerEff() { return 0.0f; }
void soGlowModuleNull::setGlowPower(float, float, float) { }
void soGlowModuleNull::setGlowDefense(s32) { }
s32 soGlowModuleNull::getGlowDefense() { return 0; }
s32 soGlowModuleNull::getGlowDefense(int) { return 0; }
bool soGlowModuleNull::isGlow() { return false; }
void soGlowModuleNull::setGlowAttack(s32 attack) { }
s32 soGlowModuleNull::getGlowAttack() { return 0; }
s32 soGlowModuleNull::getGlowAttack(soCollisionAttackData*) { return 0; }
void soGlowModuleNull::setup() { }
void soGlowModuleNull::activate() { }
soGlowModuleNull::~soGlowModuleNull() { }

static soParamCustomizeModuleNull s_soParamCustomizeModuleNull;
static soHeapModuleNull s_soHeapModuleNull;

void soReflectModuleNull::setNoSpeedMul(bool) {  }
bool soReflectModuleNull::isCountMax() { return false; }
bool soReflectModuleNull::isReflect() { return false; }
float soReflectModuleNull::getLifeMul() { return 1.0f; }
float soReflectModuleNull::getSpeedMul() { return 1.0f; }
void soReflectModuleNull::setAttackMul(float) {  }
float soReflectModuleNull::getAttackMul() { return 1.0f; }
int soReflectModuleNull::getTeam() { return -1; }
int soReflectModuleNull::getTaskId() { return -1; }
void soReflectModuleNull::resetInfo() {  }
void soReflectModuleNull::initInfo() {  }
void soReflectModuleNull::deactivate() {  }
void soReflectModuleNull::activate() {  }
soReflectModuleNull::~soReflectModuleNull() { }

static soStatusUniqProcessNull s_soStatusUniqProcessNull;

int soSlowModuleNull::getWholeFrame() { return (int)0; }
void soSlowModuleNull::setWholeFrame(int wholeFrame) {  }
u8 soSlowModuleNull::getWholeMag() { return 1; }
void soSlowModuleNull::clearWhole() {  }
void soSlowModuleNull::setWhole(int, int wholeFrame) {  }
bool soSlowModuleNull::isSkip() { return false; }
float soSlowModuleNull::getRate() { return 1.0f; }
void soSlowModuleNull::clear() {  }
int soSlowModuleNull::getFrame() { return (int)0; }
int soSlowModuleNull::getMag() { return 1; }
void soSlowModuleNull::set(int mag, int frame) {  }
bool soSlowModuleNull::isSlow() { return false; }
void soSlowModuleNull::resetSkip() {  }
void soSlowModuleNull::update(bool) {  }
void soSlowModuleNull::deactivate() {  }
void soSlowModuleNull::activate() {  }
soSlowModuleNull::~soSlowModuleNull() { }

soAbnormalModule::~soAbnormalModule() { }
void soAbnormalModuleNull::clearAll() { }
void soAbnormalModuleNull::clear(int) { }
void soAbnormalModuleNull::set(int) { }
bool soAbnormalModuleNull::isStatus(int) { return false; }
float soAbnormalModuleNull::getDamageReactionMul() { return 1.0f; }
float soAbnormalModuleNull::getAttackPowerMul() { return 1.0f; }
void soAbnormalModuleNull::update() { }
void soAbnormalModuleNull::activate() { }
soAbnormalModuleNull::~soAbnormalModuleNull() { }

void soJostleModuleNull::setForceGap(float) { }
void soJostleModuleNull::setJostleData(void*) { }
void soJostleModuleNull::setPriority(int) { }
void soJostleModuleNull::setPushSpeedX(float) { }
void soJostleModuleNull::setRefer(int) { }
void soJostleModuleNull::update() { }
void soJostleModuleNull::sleep(bool) { }
void soJostleModuleNull::setIgnoreSpeedX(u8) { }
void soJostleModuleNull::setFix(bool) { }
void soJostleModuleNull::setLayer(int) { }
void soJostleModuleNull::setStatus(u8) { }
void soJostleModuleNull::activate() { }
float soJostleModuleNull::getPosZ() { return 0.0f; }
float soJostleModuleNull::getSpeedX() { return 0.0f; }
soJostleModuleNull::~soJostleModuleNull() {  }

soJostleModule::~soJostleModule() { }

soTransitionInfo* soTransitionModuleNull::getLastTransitionInfo() {
    soTransitionInfo info;
    info.m_groupId = -1;
    info.m_unitId = -1;
    info._unk08 = 0;
    return &info;
}
soTransitionModuleNull::~soTransitionModuleNull() {  }
int soTransitionModuleNull::notifyEventAnimCmd(int commandType, soArrayContractibleTable<acCmdArgConv> commandArgList, u8* option, soModuleAccesser* accesser) { return 1; }
void soTransitionModuleNull::clearTransitionTermAll(int groupID) {  }
void soTransitionModuleNull::addGeneralTermLastTerm(int groupID, soGeneralTerm* term) {  }
void soTransitionModuleNull::addGeneralTerm(int groupID, int unitID, soGeneralTerm* term) {  }
int soTransitionModuleNull::addTerm(int groupID, soTransitionTerm* term, int unitID, u32 targetKind, u16* option) { return -1; }
bool soTransitionModuleNull::isEnableTermGroup(int groupID) { return false; }
void soTransitionModuleNull::unableTermGroup(int groupID) {  }
void soTransitionModuleNull::enableTermGroup(int groupID) {  }
void soTransitionModuleNull::unableTermAll(int groupID) {  }
void soTransitionModuleNull::enableTermAll(int groupID) {  }
void soTransitionModuleNull::unableTerm(int unitID, int groupID) {  }
void soTransitionModuleNull::enableTerm(int unitID, int groupID) {  }
int soTransitionModuleNull::checkEstablish(soModuleAccesser* accesser, u32* targetKindOut, int groupID, u16* attrMask, soGeneralTermCache* generalTermCache) { return (int)0; }

void soColorBlendModuleNull::setPri() { }
void soColorBlendModuleNull::setLightDir() { }
void soColorBlendModuleNull::setLightColorFrame() { }
void soColorBlendModuleNull::setLightColor() { }
void soColorBlendModuleNull::setLight() { }
void soColorBlendModuleNull::setFlashColorFrame() { }
void soColorBlendModuleNull::setFlash() { }
s8 soColorBlendModuleNull::getLightSet() { return 0; }
soColorBlend * soColorBlendModuleNull::getColorBlend() { return nullptr; }
GXColor soColorBlendModuleNull::getSubColor() { GXColor color = { 0, 0, 0, 0 }; return color; }
bool soColorBlendModuleNull::isEnableSubColor() { return false; }
void soColorBlendModuleNull::setSubColor(GXColor color, char enabled) {  }
void soColorBlendModuleNull::offLight(bool) {  }
void soColorBlendModuleNull::offFlash(bool) {  }
void soColorBlendModuleNull::offAll(bool) {  }
void soColorBlendModuleNull::updateMaterial() {  }
void soColorBlendModuleNull::update() {  }
void soColorBlendModuleNull::deactivate() {  }
void soColorBlendModuleNull::activate() {  }
soColorBlendModuleNull::~soColorBlendModuleNull() {  }

soColorBlendModule::~soColorBlendModule() { }

bool soItemPickTransactorNull::pickup(soItemSearch*, soModuleAccesser*, u8, itParam::TraitMask, u32, u32 index) { return false; }
bool soItemPickTransactorNull::check(soItemSearch*, soItemInfo*, soModuleAccesser*) { return false; }
soItemPickTransactorNull::~soItemPickTransactorNull() {  }

soItemPickTransactor::~soItemPickTransactor() { }

bool soItemSearchNull::search(int*, u32, soModuleAccesser*) { return false; }
soItemSearchNull::~soItemSearchNull() {  }

soItemSearch::~soItemSearch() { }

void soItemManageModuleNull::setHaveItemTeam(int, u32 index) {  }
void soItemManageModuleNull::scaleItem(BaseItem*, float) {  }
void soItemManageModuleNull::scaleAttach(itKind, float) {  }
void soItemManageModuleNull::dropAttachGroup(itParam::AttachGroup, float, float) {  }
void soItemManageModuleNull::dropAttach(itKind, float, float) {  }
void soItemManageModuleNull::ejectAttach(itKind, bool, bool) {  }
void soItemManageModuleNull::eject(int taskId) {  }
void soItemManageModuleNull::ejectAttachItem(u32 index, bool, bool) {  }
void soItemManageModuleNull::ejectHaveItem(u32 index, bool, bool) {  }
u8 soItemManageModuleNull::getAttachItemVisibility() { return 1; }
void soItemManageModuleNull::setAttachItemVisibility(bool, itParam::AttachGroup) {  }
void soItemManageModuleNull::getAttachItemInfo(soItemInfo*, u32 index) const {  }
u32 soItemManageModuleNull::getAttachItemCount() { return (u32)0; }
bool soItemManageModuleNull::isAttachItem(itKind) { return false; }
void soItemManageModuleNull::resetHaveItemConstraintNode(u32 index) {  }
void soItemManageModuleNull::setHaveItemConstraintNode(u32, u32 index) {  }
void soItemManageModuleNull::setHaveItemHoldAnim(bool, u32 index) {  }
void soItemManageModuleNull::setHaveItemVisibility(bool, int index) {  }
void soItemManageModuleNull::setHaveItemAction(u32, float, u32 index) {  }
void soItemManageModuleNull::setHaveItemScaleAnim(float, u32, u32 index) {  }
void soItemManageModuleNull::updateHaveItemActionInfo(u32 index) {  }
u32 soItemManageModuleNull::getShootItemBullet(u32 index) { return (u32)0; }
void soItemManageModuleNull::shootItemBulletBlanks(u32, u32 index) {  }
void soItemManageModuleNull::shootItemBullet(u32, float, u32 index) {  }
void soItemManageModuleNull::bornItem(u32 index) {  }
void soItemManageModuleNull::warpItem(Vec3f* pos, u32 index) {  }
void soItemManageModuleNull::dropItem(float, float, u32 index) {  }
void soItemManageModuleNull::throwItem(float angle, float speed, float power, u32 index) {  }
void soItemManageModuleNull::throwItem(float angle, float speed, float power, Vec2f* pos, u32 index) {  }
void soItemManageModuleNull::removeAll() {  }
bool soItemManageModuleNull::removeItem(BaseItem*) { return false; }
void soItemManageModuleNull::removeItem(u32 index) {  }
void soItemManageModuleNull::getPickableItemInfo(soItemInfo*) const {  }
itParam::TraitFlag soItemManageModuleNull::getPickableItemTrait() { return (itParam::TraitFlag)0; }
itKind soItemManageModuleNull::getPickableItemKind() { return static_cast<itKind>(-1); }
itParam::SizeKind soItemManageModuleNull::getPickableItemSize() { return itParam::Size_Invalid; }
void soItemManageModuleNull::getHaveItemInfo(soItemInfo*, u32 index) const {  }
float soItemManageModuleNull::getHaveItemParamFloat(u32, u32 index) const { return 0.0f; }
int soItemManageModuleNull::getHaveItemParamInt(u32, u32 index) const { return (int)0; }
itParam::HaveKind soItemManageModuleNull::getHaveItemHoldKind(u32 index) { return static_cast<itParam::HaveKind>(-1); }
itParam::TraitFlag soItemManageModuleNull::getHaveItemTrait(u32 index) const { return (itParam::TraitFlag)0; }
itKind soItemManageModuleNull::getHaveItemKind(u32 index) const { return static_cast<itKind>(-1); }
itParam::SizeKind soItemManageModuleNull::getHaveItemSize(u32 index) const { return itParam::Size_Invalid; }
bool soItemManageModuleNull::isHaveItem(u32 index) const { return false; }
bool soItemManageModuleNull::isSuccessPickupItem() { return false; }
bool soItemManageModuleNull::pickupItem(u8, u32, u32, u32 index) { return false; }
bool soItemManageModuleNull::attachItem(BaseItem* item, bool) { return false; }
bool soItemManageModuleNull::attachItem(itKind, u32 variation, bool) { return false; }
bool soItemManageModuleNull::useItem(BaseItem* item) { return false; }
void soItemManageModuleNull::useItem(u32 index) {  }
bool soItemManageModuleNull::createThrowItem(itKind kind, u32 variation, u32 nodeIndex, float angle, float speed, float power) { return false; }
bool soItemManageModuleNull::haveItem(itKind kind, u32 variation, u32 index, bool) { return false; }
bool soItemManageModuleNull::haveItem(BaseItem* item, u32 index, bool, bool) { return false; }
bool soItemManageModuleNull::haveItem(u32 nodeIndex, BaseItem* item, u32 index, bool, bool) { return false; }
void soItemManageModuleNull::check() {  }
void soItemManageModuleNull::begin() {  }
void soItemManageModuleNull::deactivate() {  }
void soItemManageModuleNull::activate() {  }
soItemManageModuleNull::~soItemManageModuleNull() {  }

soItemManageModule::~soItemManageModule() { }

int soKineticMediatorNull::getMediateNum() { return 0; }
void soKineticMediatorNull::notifyEventChangeStatus(void*, void*, void*, void*) { }
void soKineticMediatorNull::addSpeedOutside(int, void*, soModuleAccesser*) { }
void soKineticMediatorNull::addSpeed(void*, soModuleAccesser*) { }
void soKineticMediatorNull::postUpdateEnergy() { }
void soKineticMediatorNull::updateEnergy(soArray<soKineticEnergy**>*, soModuleAccesser*) { }
void soKineticMediatorNull::updateEnergy(soModuleAccesser*, u32) { }
void soKineticMediatorNull::updateEnergy(soModuleAccesser*) { }
void soKineticMediatorNull::changeKinetic(int, soModuleAccesser*) { }

soTeamModule::~soTeamModule() { }
static soTeamNull s_soTeamNull;
void soTeamModuleNull::setEnableOwnerEvent(bool) { }
void soTeamModuleNull::reset() { }
void soTeamModuleNull::deactivate() { }
void soTeamModuleNull::activate() { }
void soTeamModuleNull::setHit2nd(s32) { }
void soTeamModuleNull::set2nd(s32) { }
s32 soTeamModuleNull::getTeamOwnerId() { return -1; }
void soTeamModuleNull::setTeamOwnerId(s32) { }
void soTeamModuleNull::setHitTeam(s32) { }
soTeam* soTeamModuleNull::getIndirectTeam() { return &s_soTeamNull; }
soTeam* soTeamModuleNull::getHitTeam() { return &s_soTeamNull; }
void soTeamModuleNull::setTeam(s32, bool) { }
soTeam* soTeamModuleNull::getTeam() { return &s_soTeamNull; }
soTeamModuleNull::~soTeamModuleNull() { }

void soTeamNull::reset() { }
void soTeamNull::set2nd(int) { }
int soTeamNull::get2nd() const { return -1; }
void soTeamNull::setIndirectNo(int) { }
int soTeamNull::getIndirectNo() const { return -1; }
void soTeamNull::setNo(int) { }
int soTeamNull::getNo() const { return -1; }
soTeamNull::~soTeamNull() { }

soArticleMediator::~soArticleMediator() { }
static soArticleMediatorNull s_soArticleMediatorNull;

static s8 s_loupeNo = -1;
s8 soModelModuleNull::getLoupeNo() { return s_loupeNo; }
soModelModuleNull::~soModelModuleNull() {  }
void soModelModuleNull::setDisposeInstanceImm(bool) {  }
void soModelModuleNull::setShadowNode(int) {  }
void soModelModuleNull::setNodeConvertData(u32** convertData) {  }
Matrix * soModelModuleNull::getWorldMtxArray() { return nullptr; }
void soModelModuleNull::preShadowCalcView() {  }
bool soModelModuleNull::isRefFB() { return false; }
void soModelModuleNull::switchEnvMap(bool) {  }
bool soModelModuleNull::isEnvMap() { return false; }
bool soModelModuleNull::isAddEnvMapMat() { return false; }
void soModelModuleNull::setVirtualNodeUpdate(u32 virtualNodeIndex, bool) {  }
void soModelModuleNull::initVirtualNode(int, u32, u32, void*, Matrix*) {  }
void soModelModuleNull::changeMaterialTex(soResourceModule*, const char*resMat, const char*resPlt, nw4r::g3d::ResFile) {  }
void soModelModuleNull::setNodeVisibility(u32 nodeId, bool visibility) {  }
void soModelModuleNull::setNodeVisibility(u32 nodeId, bool visibility, bool) {  }
void soModelModuleNull::setNodeVisibility(s32 numIds, const u32* nodeIds, bool visibility) {  }
void soModelModuleNull::setNodeVisibility(s32 numIds, const u32* nodeIds, bool visibility, bool) {  }
void soModelModuleNull::setVisibility(bool visibility) {  }
void soModelModuleNull::setVisibility(bool visibility, bool) {  }
bool soModelModuleNull::getResMdl(nw4r::g3d::ResMdl* resMdl) { return false; }
bool soModelModuleNull::setOption(u32 option, u32 value) { return false; }
bool soModelModuleNull::removeAnmObj(nw4r::g3d::ScnMdlSimple::AnmObjType type) { return false; }
bool soModelModuleNull::setAnmObj(nw4r::g3d::AnmObj* obj) { return false; }
gfSceneRoot::LayerType soModelModuleNull::getLayerType() { return static_cast<gfSceneRoot::LayerType>(2); }
void soModelModuleNull::setScaleZ(float) {  }
void soModelModuleNull::setScale(float) {  }
float soModelModuleNull::getScale() { return 1.0f; }
void soModelModuleNull::renderNodeAxis(u32 nodeIndex, float ) {  }
Vec3f soModelModuleNull::getTopNodeGlobalPosFromNode(u32 p1, Vec3f* p2) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soModelModuleNull::getNodeGlobalOffsetFromTop(u32 p1) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soModelModuleNull::getResFileNodeRotation(u32 nodeIndex) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soModelModuleNull::getResFileNodeTranslate(u32 nodeIndex) { return Vec3f(0.0f, 0.0f, 0.0f); }
int soModelModuleNull::getTransNNodeId() { return -1; }
const char * soModelModuleNull::getTransNNodeName() { return nullptr; }
void soModelModuleNull::setTransNNodeId(int) {  }
u32 soModelModuleNull::getNodeNum() const { return (u32)0; }
u32 soModelModuleNull::getNodeNum(bool) const { return (u32)0; }
bool soModelModuleNull::isNode(u32 id) { return false; }
Vec3f soModelModuleNull::getNodeGlobalRotation(u32 nodeIndex, bool ) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soModelModuleNull::setNodeGlobalMtx(u32 index, Matrix* , bool ) {  }
void soModelModuleNull::getNodeGlobalMtx(u32 idx, Matrix* outMtx, bool) const { outMtx->setIdentity(); }
void soModelModuleNull::getNodeLocalMtxFromNode(u32 node1, u32 node2, Matrix* outMtx, bool ) {  }
Vec3f soModelModuleNull::getNodeLocalPosition(u32 correctNodeId, bool ) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soModelModuleNull::getNodeGlobalPosition(u32 correctNodeId, bool ) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soModelModuleNull::getNodeGlobalPosition(u32 correctNodeId, const Vec3f* p2, bool, bool ) { return Vec3f(0.0f, 0.0f, 0.0f); }
const char* soModelModuleNull::getNodeName(u32 p1) { return nullptr; }
int soModelModuleNull::getRealNodeId(u32) const { return -1; }
int soModelModuleNull::getCorrectNodeId(u32) { return -1; }
int soModelModuleNull::getNodeId(const char *nodeName) { return -1; }
void soModelModuleNull::setNodeTranslate(u32 , Vec3f* xlate) {  }
Vec3f soModelModuleNull::getNodeTranslate(u32 p1) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soModelModuleNull::setNodeRotate(u32 , Vec3f* rot) {  }
void soModelModuleNull::setNodeRotateZ(u32 , float z) {  }
void soModelModuleNull::setNodeRotateY(u32 , float y) {  }
void soModelModuleNull::setNodeRotateX(u32 , float x) {  }
Vec3f soModelModuleNull::getNodeRotate(u32 nodeIndex) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soModelModuleNull::setNodeScale(u32 , Vec3f* ) {  }
Vec3f soModelModuleNull::getNodeScale(u32 nodeIndex) { return Vec3f(1.0f, 1.0f, 1.0f); }
void soModelModuleNull::setNodeSRT(u32 , Vec3f* scale, Vec3f* rot, Vec3f* xlate) {  }
void soModelModuleNull::clearNodeSRT(u32 p1) {  }
void soModelModuleNull::clearNodeSetUp() {  }
void soModelModuleNull::setLockMtx(bool) {  }
void soModelModuleNull::clearSetUpMtx() {  }
void soModelModuleNull::setUpSkinningMtx() {  }
void soModelModuleNull::setUpMtx(u32 nodeId) {  }
void soModelModuleNull::setUpMtx(u32* nodeIds, s32 numNodeIds) {  }
void soModelModuleNull::setUpMtxFinal() {  }
void soModelModuleNull::resetLayerType(gfSceneRoot::LayerType lt) {  }
nw4r::g3d::ScnMdl* soModelModuleNull::detachScnMdl(void*) { return nullptr; }
void soModelModuleNull::setScnMdlExtern(nw4r::g3d::ScnMdl *) {  }
void soModelModuleNull::setScnMdl(nw4r::g3d::ScnMdl*, void*, gfSceneRoot::LayerType) {  }
void soModelModuleNull::setLoupeNo(s8) {  }
nw4r::g3d::ScnMdl* soModelModuleNull::getScnMdl() const { return nullptr; }
nw4r::g3d::ScnMdl* soModelModuleNull::getScnMdl(bool) const { return nullptr; }
void soModelModuleNull::begin(bool) {  }
void soModelModuleNull::activate() {  }
void soModelModuleNull::deactivate() {  }
void soModelModuleNull::destructInstance() {  }
void soModelModuleNull::constructInstance(gfSceneRoot::LayerType, u32 flags, bool, s8, nw4r::g3d::ResFile, const char*, const char*, s16 mdlResId, void*, u8 loupeNo, int priorityDrawXlu) {  }

bool soShadowModuleNull::isDispProjection() { return false; }
void soShadowModuleNull::setIgnoreMode(int) {  }
int soShadowModuleNull::getMode() { return (int)0; }
void soShadowModuleNull::setGroundPokemon(int) {  }
void soShadowModuleNull::setSizeMul(float) {  }
void soShadowModuleNull::setGroundRef(int) {  }
void soShadowModuleNull::setYOffset(float) {  }
void soShadowModuleNull::setSizeMinMax(float, float) {  }
void soShadowModuleNull::setNode(int node) {  }
void soShadowModuleNull::setSize(float) {  }
void soShadowModuleNull::update() {  }
void soShadowModuleNull::setActiveStatus(int status) {  }
void soShadowModuleNull::setActive(bool) {  }
void soShadowModuleNull::deactivate() {  }
void soShadowModuleNull::activate() {  }
soShadowModuleNull::~soShadowModuleNull() {  }
static soShadowModuleNull s_soShadowModuleNull;

void soSlopeModuleNull::setInvalidStatus(u32 statusMask) {  }
void soSlopeModuleNull::setPartNode(int) {  }
void soSlopeModuleNull::setTopAngleUnlimit(bool) {  }
void soSlopeModuleNull::updateModelTopAngle() {  }
void soSlopeModuleNull::update(bool) {  }
soSlopeStatusParam::StatusMask soSlopeModuleNull::getStatus() { return (soSlopeStatusParam::StatusMask)0; }
void soSlopeModuleNull::setStatus(u32 statusMask, int, bool) {  }
void soSlopeModuleNull::activate() {  }


soSlopeModule::~soSlopeModule() { }
soSlopeModuleNull::~soSlopeModuleNull() { }

void soPhysicsModuleNull::setToeAngleLimit(int ikId, float limit) { }
const char* soPhysicsModuleNull::getIKEndNodeName(int ikId, int index) { return NULL; }
void soPhysicsModuleNull::setIKNormal(int ikId, const Vec3f* normal) { }
void soPhysicsModuleNull::setIKTargetPos(int ikId, const Vec3f* pos, const Vec3f* pos2, const Vec3f* pos3) { }
void soPhysicsModuleNull::setIK(int ikId, float value) { }
bool soPhysicsModuleNull::isIK() { return false; }
bool soPhysicsModuleNull::isIK(int ikId) { return false; }
void soPhysicsModuleNull::sync2ndSavedPosition() { }
void soPhysicsModuleNull::set2ndCliffDir(float lr) { }
void soPhysicsModuleNull::set2ndWeaponScale(float scale) { }
void soPhysicsModuleNull::set2ndWind(const Vec3f* wind) { }
bool soPhysicsModuleNull::is2ndHitCollision() { return false; }
void soPhysicsModuleNull::set2ndPosSplit(int split) { }
void soPhysicsModuleNull::set2ndLinePos(const Vec3f* pos) { }
u32 soPhysicsModuleNull::get2ndNodeId(int lineId) { return 0; }
void soPhysicsModuleNull::set2ndPower(float power) { }
void soPhysicsModuleNull::set2ndBothReverseFlag(int flag) { }
void soPhysicsModuleNull::set2ndBothHitStat(int hitStat) { }
void soPhysicsModuleNull::set2ndBothWaver(int waver) { }
void soPhysicsModuleNull::set2ndBothRewind(int rewind) { }
void soPhysicsModuleNull::set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag) { }
int soPhysicsModuleNull::get2ndBothNodeMax() { return 0; }
int soPhysicsModuleNull::get2ndBothSituation() { return 0; }
void soPhysicsModuleNull::set2ndBothStatus(int status) { }
bool soPhysicsModuleNull::is2ndBothReverse() { return false; }
void soPhysicsModuleNull::set2ndBoth(int on) { }
bool soPhysicsModuleNull::is2ndBoth() { return false; }
void soPhysicsModuleNull::off2ndWind() { }
void soPhysicsModuleNull::set2ndRewind(int rewind) { }
void soPhysicsModuleNull::set2ndActiveNodeNum(int num) { }
int soPhysicsModuleNull::get2ndActiveNodeNum() { return 0; }
void soPhysicsModuleNull::set2ndLineLengthBothAll(float length) { }
void soPhysicsModuleNull::set2ndLineInertiaValue(int lineId, float inertia) { }
void soPhysicsModuleNull::set2ndLineInertia(int lineId, float inertia) { }
void soPhysicsModuleNull::set2ndLineLength(int lineId, float length) { }
float soPhysicsModuleNull::get2ndLineLength(int lineId) { return 0.0f; }
int soPhysicsModuleNull::get2ndLineCount() { return 0; }
void soPhysicsModuleNull::set2ndPreCollisionNode(int nodeId) { }
void soPhysicsModuleNull::set2ndEndPos(const Vec3f* pos) { }
void soPhysicsModuleNull::set2ndStatus(int status) { }
void soPhysicsModuleNull::set2ndType(int type) { }
void soPhysicsModuleNull::set2ndSleep(bool sleep) { }
void soPhysicsModuleNull::set2ndGravity(float gravity) { }
void soPhysicsModuleNull::set2ndDefault(int on) { }
void soPhysicsModuleNull::set2ndDefault() { }
void soPhysicsModuleNull::set2nd(int on) { }
bool soPhysicsModuleNull::is2nd() { return false; }
void soPhysicsModuleNull::setUpVisibility() { }
void soPhysicsModuleNull::setUpMtx2nd() { }
void soPhysicsModuleNull::setUpMtx2ndPreCollision() { }
bool soPhysicsModuleNull::isNcessarySetUpMtx2ndBoth() { return false; }
bool soPhysicsModuleNull::correct2ndBoth() { return false; }
void soPhysicsModuleNull::setUpMtx2ndBoth() { }
void soPhysicsModuleNull::setUpMtxIK() { }
void soPhysicsModuleNull::begin() { }
void soPhysicsModuleNull::activate() { }
void soPhysicsModuleNull::deactivate() { }
void soPhysicsModuleNull::destructController() { }
void soPhysicsModuleNull::constructController() { }
soPhysicsModuleNull::~soPhysicsModuleNull() { }

void soTargetSearchModuleNull::rayCheckCacheSet() { }
bool soTargetSearchModuleNull::rayCheckCacheCheck() { return false; }
void soTargetSearchModuleNull::rayCheckCacheInit() { }
void soTargetSearchModuleNull::update() { }
bool soTargetSearchModuleNull::isLostTarget() { return false; }
bool soTargetSearchModuleNull::isFoundTarget() { return false; }
void soTargetSearchModuleNull::activate() { }
void soTargetSearchModuleNull::resetIntervalCount() { }
void soTargetSearchModuleNull::initInterval() { }
void soTargetSearchModuleNull::createTargetSearchUnitAll() { }
soTargetSearchModuleNull::~soTargetSearchModuleNull() { }
soTargetSearchModule::~soTargetSearchModule() { }

bool soTerritoryModuleNull::isDoCreate() { return false; }
bool soTerritoryModuleNull::checkArea(u32 area) { return false; }
void soTerritoryModuleNull::setRect(soHyp::soTerritoryRect* rect) { }
soHyp::soTerritoryRect* soTerritoryModuleNull::getRect() { return &m_rect; }
bool soTerritoryModuleNull::isOutSide() { return false; }
bool soTerritoryModuleNull::isTouch(int area) { return false; }
void soTerritoryModuleNull::setEnable(int) { }
void soTerritoryModuleNull::createTerritory(int) { }
void soTerritoryModuleNull::activate() { }
void soTerritoryModuleNull::initAreaInstance() { }
void soTerritoryModuleNull::createAreaDataSetAll() { }
soTerritoryModuleNull::~soTerritoryModuleNull() { }
soTerritoryModule::~soTerritoryModule() { }

void soAreaModuleNull::createWind(u32, u32, stMovementData*) { }
void soAreaModuleNull::eraseWind(int) { }
void soAreaModuleNull::setLogOverThrough(bool) { }
soAreaEnviromentElementChecker* soAreaModuleNull::getEnviromentElementChecker() { return &g_soAreaEnviromentElementCheckerNull; }
soAreaContactLog* soAreaModuleNull::getAreaContactLog(int areaId, int index) { return NULL; }
u32 soAreaModuleNull::getAreaContactCount(int) { return 0; }
int soAreaModuleNull::getAreaIndexFromId(int) { return -1; }
gfArea* soAreaModuleNull::getAreaInstance(u32 index) { return NULL; }
bool soAreaModuleNull::isExistAreaInstance(int) { return false; }
int soAreaModuleNull::getAreaId(u32 index) { return 0; }
void soAreaModuleNull::setAreaDbgColor(u32 index, GXColor) { }
void soAreaModuleNull::setCenterX0(u32 index, bool) { }
void soAreaModuleNull::setAreaShapeType(u32 index, soAreaInstance::ShapeType) { }
void soAreaModuleNull::setAreaShape(u32 index, Vec2f*, float) { }
void soAreaModuleNull::setAreaShape(u32 index, Vec2f*, Vec2f*) { }
void soAreaModuleNull::setAreaTargetLocalGroupMask(u32 index, u32 mask) { }
void soAreaModuleNull::setAreaTargetGroupMask(u32 index, u32 mask) { }
void soAreaModuleNull::resetArea(int) { }
bool soAreaModuleNull::isEnableArea(int) { return false; }
void soAreaModuleNull::enableArea(int, bool, int) { }
gfArea::Group soAreaModuleNull::getGroup(u32 index) { return static_cast<gfArea::Group>(0); }
void soAreaModuleNull::setWhole(bool) { }
void soAreaModuleNull::addArea(soSet<soAreaData>*) { }
void soAreaModuleNull::setup(soSet<soAreaData>*) { }
void soAreaModuleNull::clean() { }
void soAreaModuleNull::check() { }
void soAreaModuleNull::clearLog() { }
void soAreaModuleNull::update() { }
void soAreaModuleNull::deactivate() { }
void soAreaModuleNull::activate(int ownerId) { }
soAreaModuleNull::~soAreaModuleNull() { }
soAreaModule::~soAreaModule() { }

bool soAreaEnviromentElementCheckerNull::testWater(Vec2f*, float* surfaceY) { return false; }
u32 soAreaEnviromentElementCheckerNull::getOnMovementGroundArea() { return 0; }
float soAreaEnviromentElementCheckerNull::getWaterSurfaceY() { return 0.0f; }
int soAreaEnviromentElementCheckerNull::getWaterAreaId() { return -1; }
bool soAreaEnviromentElementCheckerNull::isWaterAreaIndex(int) { return false; }
bool soAreaEnviromentElementCheckerNull::isWater() { return false; }
bool soAreaEnviromentElementCheckerNull::isEnable() { return false; }
void soAreaEnviromentElementCheckerNull::unable() { }
void soAreaEnviromentElementCheckerNull::enable() { }
void soAreaEnviromentElementCheckerNull::clear() { }
void soAreaEnviromentElementCheckerNull::clearWaterInfo() { }
void soAreaEnviromentElementCheckerNull::check(soModuleAccesser*) { }
soAreaEnviromentElementCheckerNull::~soAreaEnviromentElementCheckerNull() { }
static soAreaEnviromentElementCheckerNull s_soAreaEnviromentElementCheckerNull;

int soComboModuleNull::getCount() { return 0; }
void soComboModuleNull::clearSetting() { }
void soComboModuleNull::update() { }
void soComboModuleNull::set(int kind) { }
void soComboModuleNull::activate() { }
soComboModuleNull::~soComboModuleNull() { }
static soComboModuleNull s_soComboModuleNull;

bool soDebugModule::isSubFlag() { return false; }
bool soDebugModule::isDebugTarget() { return false; }
bool soDebugModuleNull::isDamageOrbit() { return false; }
bool soDebugModuleNull::isPrintChangeMotion() { return false; }
bool soDebugModuleNull::isPrintChangeStatus() { return false; }
void soDebugModuleNull::debugPrintInsideEventInfo() { }
const char* soDebugModuleNull::getGroundCorrectName() { return NULL; }
const char* soDebugModuleNull::getSituationName() { return NULL; }
const char* soDebugModuleNull::getKineticTypeName() { return NULL; }
const char* soDebugModuleNull::getMotionName() { return NULL; }
const char* soDebugModuleNull::getStatusName() { return NULL; }
int soDebugModuleNull::getColorBlendStatus() { return 0; }
void soDebugModuleNull::addDebugDamage() { }
void soDebugModuleNull::addDebugRay() { }
bool soDebugModuleNull::isSleep() { return true; }
void soDebugModuleNull::sleep() { }
bool soDebugModuleNull::isDisplayModel() { return true; }
bool soDebugModuleNull::isDamageLock() { return false; }
void soDebugModuleNull::render() { }
void soDebugModuleNull::update() { }
void soDebugModuleNull::proc() { }
soDebugModuleNull::~soDebugModuleNull() { }

void* soWorkManageModuleNull::getParamAccesser() { return &g_soParamAccesserNull; }
void soWorkManageModuleNull::clearAll(u32 index) { }
u32 soWorkManageModuleNull::turnOffFlag(u32 index) { return 0; }
void soWorkManageModuleNull::setFlag(bool on, u32 index) { }
void soWorkManageModuleNull::offFlag(u32 index) { }
void soWorkManageModuleNull::onFlag(u32 index) { }
bool soWorkManageModuleNull::isFlag(u32 index) { return false; }
void soWorkManageModuleNull::subFloat(float subtractValue, u32 index) { }
void soWorkManageModuleNull::addFloat(float addValue, u32 index) { }
void soWorkManageModuleNull::rndFloat(float minValue, float maxValue, u32 index) { }
void soWorkManageModuleNull::setFloat(float value, u32 index) { }
float soWorkManageModuleNull::getFloat(u32 index) { return 0.0f; }
u32 soWorkManageModuleNull::countDownInt(u32 index, int threshold) { return 0; }
void soWorkManageModuleNull::subInt(int subtractValue, u32 index) { }
void soWorkManageModuleNull::addInt(int addValue, u32 index) { }
void soWorkManageModuleNull::rndInt(int minValue, int maxValue, u32 index) { }
void soWorkManageModuleNull::incInt(u32 index) { }
void soWorkManageModuleNull::decInt(u32 index) { }
void soWorkManageModuleNull::setInt(int value, u32 index) { }
int soWorkManageModuleNull::getInt(u32 index) { return 0; }
void soWorkManageModuleNull::setWork(u32 index, soGeneralWorkAbstract* generalWork) { }
void soWorkManageModuleNull::activate() { }
soWorkManageModuleNull::~soWorkManageModuleNull() { }
soWorkManageModule::~soWorkManageModule() { }

void* soParamAccesserNull::getParamIndefinite(soModuleAccesser* moduleAccesser, s32 p2, s32 p3) { return NULL; }
s32 soParamAccesserNull::getParamInt(soModuleAccesser* moduleAccesser, s32 p2, s32 p3) { return 0; }
float soParamAccesserNull::getParamFloat(soModuleAccesser* moduleAccesser, s32 p2, s32 p3) { return 0.0f; }
soParamAccesserNull::~soParamAccesserNull() { }
soParamAccesser::~soParamAccesser() { }

void soEffectModuleNull::ignoreScaleZ(bool) { }
void soEffectModuleNull::setShieldEffect(int) { }
bool soEffectModuleNull::isSyncScale() { return false; }
void soEffectModuleNull::setSyncScale(bool) { }
bool soEffectModuleNull::isSyncVisibility() { return false; }
void soEffectModuleNull::setSyncVisibility(bool) { }
void soEffectModuleNull::removeScreen(int index) { }
void soEffectModuleNull::reqScreen(int index) { }
bool soEffectModuleNull::isExistCommon(int) { return false; }
bool soEffectModuleNull::isEndCommon() { return false; }
void soEffectModuleNull::resetCommon() { }
void soEffectModuleNull::removeCommon(int) { }
void soEffectModuleNull::reqCommon(int index, float frame) { }
float soEffectModuleNull::getDeadEffectRotZ(Vec3f*, bool* out) { return 0.0f; }
void soEffectModuleNull::clearScreen(int index, u32) { }
#ifdef FT_FIGHTER_FIELDS
void soEffectModuleNull::fillScreen(int index, u32 frames, GXColor* color, int, int flags) { }
#else
void soEffectModuleNull::fillScreen(int index, u32, int, int) { }
#endif
void soEffectModuleNull::reqPause(bool) { }
void soEffectModuleNull::setWhole(bool) { }
void soEffectModuleNull::setVisible(int, bool) { }
bool soEffectModuleNull::isExistEffect(int) { return false; }
void soEffectModuleNull::setScale(int, Vec3f* scale) { }
void soEffectModuleNull::setRot(int, Vec3f* rot) { }
void soEffectModuleNull::setPos(int, Vec3f* pos) { }
void soEffectModuleNull::removeAllAfterImage(u32, int) { }
void soEffectModuleNull::removeAfterImage(int, int) { }
int soEffectModuleNull::reqAfterImage(int nodeID1, int nodeID2, int, short, Vec3f*, Vec3f*, u32, EfID effectId, int, Vec3f* pos, Vec3f*, float scale, u8, u8, float) { return 0; }
void soEffectModuleNull::killAll(u32, bool, bool) { }
void soEffectModuleNull::killKind(int effectKind, bool, bool) { }
void soEffectModuleNull::kill(int, bool, bool) { }
void soEffectModuleNull::removeAll(u32) { }
void soEffectModuleNull::remove(int) { }
void soEffectModuleNull::reqEmit(EfID effectID, u32) { }
void soEffectModuleNull::removeAllTime() { }
void soEffectModuleNull::removeTime(int) { }
void soEffectModuleNull::reqTimeFollow(EfID effectID, int, int, Vec3f*, Vec3f*, float scale, u32) { }
void soEffectModuleNull::reqTime(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, u32) { }
void soEffectModuleNull::removeAllContinual() { }
void soEffectModuleNull::removeContinual(u32 index) { }
int soEffectModuleNull::reqContinual(EfID effectID, float, int, bool, u32 index) { return 0; }
u32 soEffectModuleNull::reqFollow(EfID effectID, int, Vec3f*, Vec3f*, float scale, bool, u32, u32 effectIDAdd, int) { return 0; }
u32 soEffectModuleNull::reqAttachGround(EfID effectID, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* rotRange, u32) { return 0; }
u32 soEffectModuleNull::req2D(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32) { return 0; }
bool soEffectModuleNull::req(EfID effectID, int, int*, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32) { return false; }
u32 soEffectModuleNull::req(EfID effectID, int, Vec3f* pos, Vec3f* rot, float scale, Vec3f* posRange, Vec3f* rotRange, bool, u32) { return 0; }
u32 soEffectModuleNull::req(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32, int) { return 0; }
void soEffectModuleNull::update(bool) { }
void soEffectModuleNull::begin() { }
void soEffectModuleNull::activate() { }
void soEffectModuleNull::deactivate() { }
soEffectModuleNull::~soEffectModuleNull() { }
static soEffectModule s_soEffectModule;

void soGenerateArticleManageModuleNull::setDynamicArticleMediator(soArticleMediator*) { }
void soGenerateArticleManageModuleNull::getArticleList(soArray<soArticle*>*, int, int) { }
soArticle* soGenerateArticleManageModuleNull::getArticleFromNo(int, int) { return NULL; }
soArticle* soGenerateArticleManageModuleNull::getArticle(int) { return NULL; }
void soGenerateArticleManageModuleNull::setAutoRecycle(bool) { }
int soGenerateArticleManageModuleNull::getGenerateRestNum(int) { return 0; }
int soGenerateArticleManageModuleNull::getGenerateMaxNum(int) { return 0; }
int soGenerateArticleManageModuleNull::getActiveNum(int) { return 0; }
bool soGenerateArticleManageModuleNull::isGeneratable(int) { return false; }
int soGenerateArticleManageModuleNull::getNum(int) { return 0; }
bool soGenerateArticleManageModuleNull::isExist(int) { return false; }
void soGenerateArticleManageModuleNull::removeExistTaskId(int) { }
void soGenerateArticleManageModuleNull::removeExist(int, int) { }
void soGenerateArticleManageModuleNull::remove(int, int) { }
void soGenerateArticleManageModuleNull::eject(int) { }
void soGenerateArticleManageModuleNull::entry(soArticle*) { }
void soGenerateArticleManageModuleNull::setSituationKind(int, SituationKind) { }
void soGenerateArticleManageModuleNull::set2nd(int, u8, int) { }
void soGenerateArticleManageModuleNull::setVisibilityWhole(int, u8, int) { }
void soGenerateArticleManageModuleNull::changeStatus(int, int, int) { }
void soGenerateArticleManageModuleNull::setRate(int, float) { }
void soGenerateArticleManageModuleNull::setFrame(int, float) { }
void soGenerateArticleManageModuleNull::changeMotion(int, int, bool) { }
void soGenerateArticleManageModuleNull::shoot(int, int, bool) { }
void soGenerateArticleManageModuleNull::have(int, int, int, int) { }
soArticle* soGenerateArticleManageModuleNull::generate(int, void*, bool) { return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage); }
void soGenerateArticleManageModuleNull::deactivate(bool) { }
void soGenerateArticleManageModuleNull::activate() { }
soGenerateArticleManageModuleNull::~soGenerateArticleManageModuleNull() { }
soGenerateArticleManageModule::~soGenerateArticleManageModule() { }

void soMotionModuleNull::changeMotion(soMotionChangeParam* changeParam) { }
bool soMotionModuleNull::isForceImmediatelyStatusInterpret() { return false; }
bool soMotionModuleNull::isProcessFixPositionDone() { return false; }
void soMotionModuleNull::setTransMoveSpeedNoScale(bool) { }
void soMotionModuleNull::setNoComp(bool) { }
void soMotionModuleNull::setShareAnimObjPartialInfo(int, int) { }
void soMotionModuleNull::setShareAnimObjKind(int shareAnimObjKind) { }
void soMotionModuleNull::setKindOffset(int kindOffset) { }
void soMotionModuleNull::clearResourceId() { }
void soMotionModuleNull::setResourceId(int, int) { }
void soMotionModuleNull::setAnmObjChrBlend(void*) { }
void* soMotionModuleNull::detachAnmObjChrBlend() { return NULL; }
void soMotionModuleNull::setCheckTexBind() { }
bool soMotionModuleNull::setAnimObjCacheModule(void*) { return false; }
void soMotionModuleNull::cacheAnimChrForce(int kind) { }
void* soMotionModuleNull::getLastMotionTransitionInfo() {
    soTransitionInfo info;
    info.m_groupId = -1;
    info.m_unitId = -1;
    info._unk08 = 0;
    return &info;
}
void soMotionModuleNull::setOnlyChr(bool) { }
void soMotionModuleNull::setShareCancelNode(int, int) { }
void soMotionModuleNull::addTransitionTerm(void*, int numTerms, int) { }
void soMotionModuleNull::addDefaultTransitionTerm(void*, int numTerms) { }
bool soMotionModuleNull::checkTransition(int) { return false; }
void soMotionModuleNull::turnBack() { }
void soMotionModuleNull::rewind() { }
void soMotionModuleNull::begin() { }
Vec3f soMotionModuleNull::getNodeTranslate(float, int kind, int nodeId, bool) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soMotionModuleNull::getNodeTranslate(int nodeId, bool) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soMotionModuleNull::getNodeRotate(int nodeId) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soMotionModuleNull::enablePartialCancelNode(bool) { }
void soMotionModuleNull::setPartialCancelNodeData(void*) { }
#ifdef FT_FIGHTER_FIELDS
void soMotionModuleNull::setShareAdjustNode(int, float) { }
#else
void soMotionModuleNull::setShareAdjustNode(float, void*) { }
#endif
int soMotionModuleNull::getNum() { return 0; }
Vec3f soMotionModuleNull::getBlendTransMoveSpeed() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soMotionModuleNull::get2ndTransMoveSpeed() { return Vec3f(0.0f, 0.0f, 0.0f); }
bool soMotionModuleNull::is2ndTransMove() { return false; }
void soMotionModuleNull::update2ndTransMoveSpeed() { }
Vec3f soMotionModuleNull::get2ndTransNTranslate() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soMotionModuleNull::getTransMoveSpeed() { return Vec3f(0.0f, 0.0f, 0.0f); }
bool soMotionModuleNull::isTransMove() { return false; }
void soMotionModuleNull::updateTransMoveSpeed() { }
Vec3f soMotionModuleNull::getTransNTranslate() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soMotionModuleNull::getTransNTranslateEndFrame(int kind) { return Vec3f(0.0f, 0.0f, 0.0f); }
bool soMotionModuleNull::isLoopedOtherAnim(int) { return false; }
bool soMotionModuleNull::isEndOtherAnim(int) { return false; }
float soMotionModuleNull::getOtherAnimEndFrame(int) { return 0.0f; }
float soMotionModuleNull::getOtherAnimFrame(int) { return 0.0f; }
void soMotionModuleNull::setOtherAnimFrame(float frame, int) { }
float soMotionModuleNull::getOtherAnimRate(int) { return 0.0f; }
void soMotionModuleNull::setOtherAnimRate(int, float rate) { }
void soMotionModuleNull::removeOtherAnim(int) { }
void soMotionModuleNull::addOtherAnim(float, float, int, int, int, bool) { }
void soMotionModuleNull::addOtherAnim(int, u32, bool, float, float) { }
int soMotionModuleNull::getOtherAnimKind(int) { return -1; }
int soMotionModuleNull::getOtherAnimSize() { return 0; }
bool soMotionModuleNull::isLoopedPartial(int) { return false; }
bool soMotionModuleNull::isEndPartial(int) { return false; }
float soMotionModuleNull::getPartialEndFrame(int) { return 0.0f; }
float soMotionModuleNull::getPartialFrame(int) { return 0.0f; }
void soMotionModuleNull::setPartialFrameSyncAnimCmd(float frame, int) { }
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
void soMotionModuleNull::setPartialFrame(int part, float frame) { }
#else
void soMotionModuleNull::setPartialFrame(float frame, int) { }
#endif
float soMotionModuleNull::getPartialRate(int) { return 0.0f; }
#ifdef SO_MOTION_PARTIAL_INDEX_FIRST
void soMotionModuleNull::setPartialRate(int part, float rate) { }
#else
void soMotionModuleNull::setPartialRate(float rate, int) { }
#endif
int soMotionModuleNull::getPartialKind(int) { return -1; }
int soMotionModuleNull::getPartialSize() { return 0; }
void soMotionModuleNull::attachPartialAnimChr(u32) { }
void soMotionModuleNull::detachPartialAnimChr(u32, bool) { }
void soMotionModuleNull::compRemovePartialAnimChr(int) { }
void soMotionModuleNull::removePartialAnimChr(int) { }
void soMotionModuleNull::addPartialAnimChr(int partKind, int kind, int nodeSet, bool loop, float frame, float rate, bool sync) { }
bool soMotionModuleNull::is2ndEnd() { return false; }
int soMotionModuleNull::get2ndKind() { return -1; }
float soMotionModuleNull::get2ndFrame() { return 0.0f; }
void soMotionModuleNull::set2ndRate(float rate) { }
void soMotionModuleNull::remove2ndAnimChr() { }
void soMotionModuleNull::add2ndAnimChr(int kind, bool, float frame, float rate) { }
void soMotionModuleNull::processFixCamera() { }
void soMotionModuleNull::processFixPosition() { }
bool soMotionModuleNull::changeMotionRequest(soMotionChangeParam* changeParam) { return false; }
Vec3f soMotionModuleNull::calcNode(int, int) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soMotionModuleNull::calcNode(int, void*, void*) { }
void soMotionModuleNull::disableNode(int) { }
bool soMotionModuleNull::isBlend() { return false; }
void soMotionModuleNull::setWeightRate(float weightRate) { }
void soMotionModuleNull::setWeight(float weight) { }
float soMotionModuleNull::getPrevWeight() { return 0.0f; }
float soMotionModuleNull::getWeight() { return 0.0f; }
int soMotionModuleNull::getKind() { return -1; }
char* soMotionModuleNull::getName() { return NULL; }
char* soMotionModuleNull::getName(int kind, bool) { return NULL; }
bool soMotionModuleNull::isAnimCmd(int) { return false; }
bool soMotionModuleNull::isLooped() { return false; }
bool soMotionModuleNull::isEnd() { return false; }
float soMotionModuleNull::getEndFrame() { return 0.0f; }
float soMotionModuleNull::getEndFrame(int kind) { return 0.0f; }
bool soMotionModuleNull::isAnimResFile(int kind) { return false; }
float soMotionModuleNull::getFrame() { return 0.0f; }
void soMotionModuleNull::setFrameSyncAnimCmd(float frame) { }
void soMotionModuleNull::setFrame(float frame) { }
bool soMotionModuleNull::isLoopFlag() { return false; }
void soMotionModuleNull::setLoopFlag(bool loopFlag) { }
void soMotionModuleNull::setRate(float rate) { }
float soMotionModuleNull::getRate() { return 0.0f; }
void soMotionModuleNull::updateNode() { }
void soMotionModuleNull::updateFrame(float) { }
void soMotionModuleNull::setData(void*) { }
void soMotionModuleNull::deactivate() { }
void soMotionModuleNull::activate() { }
soMotionModuleNull::~soMotionModuleNull() { }


// soVisibilityModuleNull
bool soVisibilityModuleNull::isVisible() { return false; }
u8 soVisibilityModuleNull::getWhole() { return 0; }
void soVisibilityModuleNull::setWhole(u8) { }
int soVisibilityModuleNull::get(u32) { return 0; }
void soVisibilityModuleNull::hideAll() { }
void soVisibilityModuleNull::update() { }
void soVisibilityModuleNull::setDefaultAll() { }
void soVisibilityModuleNull::setStatusDefault(u32, int, bool) { }
void soVisibilityModuleNull::set(u32, int) { }
void soVisibilityModuleNull::setData(soVisibilityData*) { }
void soVisibilityModuleNull::activate() { }
soVisibilityModuleNull::~soVisibilityModuleNull() { }

// soSoundModuleNull
bool soSoundModuleNull::isPlay(int) { return false; }
int soSoundModuleNull::getCheckSoundGroup() { return -1; }
void soSoundModuleNull::setCheckSoundGroup(int) { }
void soSoundModuleNull::setFixGeneratorStatus() { }
soSoundModule::SoundIdData* soSoundModuleNull::getSoundIdData() { return 0; }
void soSoundModuleNull::setSoundIdData(SoundIdData*) { }
void soSoundModuleNull::playLandingSE() { }
void soSoundModuleNull::setSESpeed(int, float) { }
void soSoundModuleNull::stopSEHandle(int, int) { }
void soSoundModuleNull::stopSE(int, int) { }
void soSoundModuleNull::stopStatusSE() { }
float soSoundModuleNull::getSEPitch() { return 1.0f; }
void soSoundModuleNull::setSEPitch(u32, double) { }
void soSoundModuleNull::setSEPitch(float) { }
float soSoundModuleNull::getSEVol(int) { return 0.0f; }
void soSoundModuleNull::setSEVol(int, float, int) { }
void soSoundModuleNull::playDamageSE(u32, u32) { }
void soSoundModuleNull::playHitSE(float, void*) { }
bool soSoundModuleNull::getPlayHitSEFlag() { return false; }
void soSoundModuleNull::setPlayHitSEFlag(bool) { }
int soSoundModuleNull::playStatusSE(SndID, bool, int) { return -1; }
int soSoundModuleNull::playSEPos(SndID, Vec3f*, bool, bool, int) { }
int soSoundModuleNull::playSENo3d(SndID, bool) { return -1; }
int soSoundModuleNull::playSE(SndID, bool, bool, int) { return -1; }
void soSoundModuleNull::setPos(Vec3f*, int) { }
void soSoundModuleNull::update(Vec3f*) { }
void soSoundModuleNull::activate(Vec3f*) { }
void soSoundModuleNull::deactivate() { }
soSoundModuleNull::~soSoundModuleNull() { }

// soShakeTerm & soShakeModuleNull
soShakeTerm::~soShakeTerm() { }
bool soShakeModuleNull::getOffset(Vec2f*) { return false; }
bool soShakeModuleNull::isShake() { return false; }
void soShakeModuleNull::update(bool) { }
void soShakeModuleNull::stopKind(int) { }
void soShakeModuleNull::stop() { }
void soShakeModuleNull::extend(int, int) { }
void soShakeModuleNull::req(int, int, bool, const Vec2f*) { }
void soShakeModuleNull::activate() { }
soShakeModuleNull::~soShakeModuleNull() { }

// soStopModuleNull
void soStopModuleNull::setHitStopFrameFix(s32) { }
void soStopModuleNull::setHitStopFrame(u32, int) { }
s32 soStopModuleNull::getHitStopRealFrame() { return 0; }
s32 soStopModuleNull::getHitStopFrame() { return 0; }
void soStopModuleNull::update() { }
bool soStopModuleNull::isDamage() { return false; }
bool soStopModuleNull::isHit() { return false; }
bool soStopModuleNull::isSpecialStop() { return false; }
bool soStopModuleNull::isStop() { return false; }
void soStopModuleNull::setLinkStop(u32) { }
void soStopModuleNull::cancelOtherStop() { }
void soStopModuleNull::setOtherStop(u32) { }
void soStopModuleNull::cancelHitStop() { }
void soStopModuleNull::setHitStop(float, int) { }
void soStopModuleNull::end(s32) { }
void soStopModuleNull::activate() { }
void soStopModuleNull::setSpecialStop(u32) { }
soStopModuleNull::~soStopModuleNull() { }

// soCaptureModuleNull
bool soCaptureModuleNull::isIgnoreCatching() { return false; }
void soCaptureModuleNull::setIgnoreCatching(bool) { }
const Vec3f* soCaptureModuleNull::getMotionOffset() { return 0; }
bool soCaptureModuleNull::isReaction() { return false; }
void soCaptureModuleNull::setUpdatePos(bool) { }
void soCaptureModuleNull::setCatchNode(int) { }
void soCaptureModuleNull::setSendCutEvent(bool) { }
bool soCaptureModuleNull::check() { return false; }
void soCaptureModuleNull::thrownCut(bool) { }
void soCaptureModuleNull::updateLRThrown() { }
void soCaptureModuleNull::updatePosThrown() { }
bool soCaptureModuleNull::isThrown() { return false; }
void soCaptureModuleNull::thrown() { }
bool soCaptureModuleNull::checkDamageThrown() { return false; }
bool soCaptureModuleNull::checkDamage() { return false; }
void soCaptureModuleNull::captureCut(bool, bool) { }
float soCaptureModuleNull::getCatchPosY() { return 0.0f; }
Vec2f soCaptureModuleNull::getNodeOffset() { return Vec2f(0.0f, 0.0f); }
bool soCaptureModuleNull::capture(int, int, bool) { return false; }
int soCaptureModuleNull::getCatchTaskId() { return -1; }
bool soCaptureModuleNull::isCapture() { return false; }
void soCaptureModuleNull::initInfo() { }
void soCaptureModuleNull::setNodes(int, int, float) { }
void soCaptureModuleNull::deactivate() { }
void soCaptureModuleNull::activate() { }
soCaptureModuleNull::~soCaptureModuleNull() { }

// soCatchModuleNull
bool soCatchModuleNull::check() { return false; }
void soCatchModuleNull::setSendCutEvent(bool) { }
bool soCatchModuleNull::checkDamage() { return false; }
void soCatchModuleNull::clingCut(bool) { }
void soCatchModuleNull::updateLRCling() { }
void soCatchModuleNull::updatePosCling() { }
void soCatchModuleNull::catchCut(bool, bool) { }
void soCatchModuleNull::offCatchFlag() { }
void soCatchModuleNull::onCatchFlag() { }
bool soCatchModuleNull::isCatch() { return false; }
void soCatchModuleNull::setCatch(int) { }
void soCatchModuleNull::initInfo() { }
void soCatchModuleNull::setNodes(int, int, float) { }
void soCatchModuleNull::deactivate() { }
void soCatchModuleNull::activate() { }
soCatchModuleNull::~soCatchModuleNull() { }

// soDamageModuleNull
static soDamageEffectorNull s_soDamageEffectorNull;

void soDamageModuleNull::updateAttackerInfo() { }
void soDamageModuleNull::heal(float, u32) { }
bool soDamageModuleNull::isDamageLock() { return false; }
void soDamageModuleNull::setDamageLock(bool) { }
void soDamageModuleNull::setReactionMul2nd(float) { }
void soDamageModuleNull::setReactionMul(float) { }
void soDamageModuleNull::setDamageMul(float) { }
bool soDamageModuleNull::isCaptureCut(void*) { return false; }
bool soDamageModuleNull::isCaptureCut() { return false; }
float soDamageModuleNull::getCaptureDamage() { return 0.0f; }
bool soDamageModuleNull::isCatchCut() { return false; }
bool soDamageModuleNull::isReaction() { return false; }
soDamageEffector* soDamageModuleNull::getEffector() {
    return &s_soDamageEffectorNull;
}
void soDamageModuleNull::clearAttackerInfo() { }
void soDamageModuleNull::restoreAttackerInfo(soDamageAttackerInfo*) { }
void soDamageModuleNull::getAttackerInfo(soDamageAttackerInfo*) { }
soDamageAttackerInfo* soDamageModuleNull::getAttackerInfo() { return 0; }
void soDamageModuleNull::clearDamageLog() { }
soDamageLog* soDamageModuleNull::getDamageLog() { return 0; }
void soDamageModuleNull::leaveTurnDamage() { }
void soDamageModuleNull::toTurnDamage() { }
bool soDamageModuleNull::isNoReactionModePerfect() { return false; }
void soDamageModuleNull::setNoReactionModePerfect(bool) { }
void soDamageModuleNull::setNoReactionModeAlways(bool) { }
void soDamageModuleNull::resetNoReactionMode() { }
void soDamageModuleNull::setNoReactionMode(int) { }
void soDamageModuleNull::resetNoReactionModeStatus() { }
void soDamageModuleNull::setNoReactionMode2nd(float, float, int) { }
void soDamageModuleNull::setNoReactionModeStatus(float, float, int) { }
bool soDamageModuleNull::checkNoReaction(void*) { return false; }
float soDamageModuleNull::getPowerMax(u32) { return 0.0f; }
float soDamageModuleNull::getReaction(u32) { return 0.0f; }
float soDamageModuleNull::getDamage(u32) { return 0.0f; }
void soDamageModuleNull::addDamage(float, u32) { }
bool soDamageModuleNull::setForceDamage(StageObject*, const char*, u32, u32, bool, bool) { return false; }
bool soDamageModuleNull::setForceDamage(StageObject*, int, u32, u32, bool, bool) { return false; }
bool soDamageModuleNull::setForceDamage(StageObject*, Vec3f*, u32, u32, bool, bool) { return false; }
float soDamageModuleNull::getDamageLr(float, float, void*, void*, int, int) { return -1.0f; }
bool soDamageModuleNull::check(bool) { return false; }
void soDamageModuleNull::storeDamage(soDamage*, float) { }
void soDamageModuleNull::setInfo() { }
void soDamageModuleNull::initInfo() { }
void soDamageModuleNull::initDamage(float) { }
void soDamageModuleNull::update() { }
void soDamageModuleNull::sleep(bool) { }
void soDamageModuleNull::deactivate() { }
void soDamageModuleNull::activate(float) { }
soDamageModuleNull::~soDamageModuleNull() { }

// soCollisionSearchModuleNull
void soCollisionSearchModuleNull::renderDebug() { }
void soCollisionSearchModuleNull::setCheckInterval(int) { }
void soCollisionSearchModuleNull::invalidLog(soCollisionLog*, u32) { }
int soCollisionSearchModuleNull::isInflict() { return 0; }
bool soCollisionSearchModuleNull::check() { return false; }
void soCollisionSearchModuleNull::checkLog() { }
void soCollisionSearchModuleNull::update(float, float) { }
soCollisionSearchData* soCollisionSearchModuleNull::getData(u32) { return 0; }
void soCollisionSearchModuleNull::setPos(int, Vec3f*, Vec3f*) { }
void soCollisionSearchModuleNull::setSize(u32, float) { }
float soCollisionSearchModuleNull::getSize(u32) { return 0.0f; }
bool soCollisionSearchModuleNull::isSearch(u32) { return false; }
void soCollisionSearchModuleNull::set(u32, u32, soCollisionSearchData*) { }
void soCollisionSearchModuleNull::clear(u32) { }
void soCollisionSearchModuleNull::clearAll() { }
void soCollisionSearchModuleNull::begin() { }
void soCollisionSearchModuleNull::deactivate() { }
void soCollisionSearchModuleNull::activate(float, int) { }
soCollisionSearchModuleNull::~soCollisionSearchModuleNull() { }

// soCollisionCatchModuleNull
void soCollisionCatchModuleNull::renderDebug() { }
void soCollisionCatchModuleNull::checkLog() { }
void soCollisionCatchModuleNull::renew() { }
void soCollisionCatchModuleNull::update(float, float, float) { }
soCollisionCatchData* soCollisionCatchModuleNull::getData(u32) const { return 0; }
Vec3f soCollisionCatchModuleNull::getCenterPos(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
float soCollisionCatchModuleNull::getPosX() { return 0.0f; }
void soCollisionCatchModuleNull::setSize(u32, float) { }
float soCollisionCatchModuleNull::getSize(u32) { return 0.0f; }
bool soCollisionCatchModuleNull::isCatch(u32) { return false; }
void soCollisionCatchModuleNull::set(u32, soCollisionCatchData*) { }
void soCollisionCatchModuleNull::clear(u32) { }
void soCollisionCatchModuleNull::clearAll() { }
void soCollisionCatchModuleNull::begin() { }
void soCollisionCatchModuleNull::deactivate() { }
void soCollisionCatchModuleNull::activate(float, float, float, int, bool) { }
soCollisionCatchModuleNull::~soCollisionCatchModuleNull() { }

// soCollisionShieldModuleNull
float soCollisionShieldModuleNull::getPosX(u32) const { return 0.0f; }
float soCollisionShieldModuleNull::getLifeMul(u32) const { return 0.0f; }
float soCollisionShieldModuleNull::getSpeedMul(u32) const { return 1.0f; }
float soCollisionShieldModuleNull::getAttackMul(u32) const { return 1.0f; }
void soCollisionShieldModuleNull::setTargetProperty(u32, u32) { }
u32 soCollisionShieldModuleNull::getGroupNum() { return 0; }
float soCollisionShieldModuleNull::getLr(u32) const { return 1.0f; }
Vec3f soCollisionShieldModuleNull::getCenterPos(u32, u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soCollisionShieldModuleNull::clearAll(u32) { }
void soCollisionShieldModuleNull::clearAll() { }
void soCollisionShieldModuleNull::renderDebug() { }
bool soCollisionShieldModuleNull::check() { return false; }
void soCollisionShieldModuleNull::checkLog() { }
u32 soCollisionShieldModuleNull::getTeam() { return -1; }
int soCollisionShieldModuleNull::getPartSize(u32) const { return 0; }
u8 soCollisionShieldModuleNull::isNoMBall(u32) const { return 0; }
float soCollisionShieldModuleNull::getAttackLimit(u32) const { return 0.0f; }
void soCollisionShieldModuleNull::setHop(float, bool, u32) { }
void soCollisionShieldModuleNull::setFront(bool, u32) { }
void soCollisionShieldModuleNull::setTurn(bool, u32) { }
bool soCollisionShieldModuleNull::isNoHop(u32) const { return false; }
float soCollisionShieldModuleNull::getHopAngle(u32) const { return 0.0f; }
bool soCollisionShieldModuleNull::isHop(u32) const { return false; }
u8 soCollisionShieldModuleNull::isFront(u32) const { return 0; }
u8 soCollisionShieldModuleNull::isTurn(u32) const { return 0; }
void soCollisionShieldModuleNull::setStatusAll(u32, u32) { }
void soCollisionShieldModuleNull::setStatus(u32, u32, u32) { }
bool soCollisionShieldModuleNull::isShield(u32, u32) { return false; }
void soCollisionShieldModuleNull::update(double, double, double) { }
void soCollisionShieldModuleNull::update(double, double, double, u32) { }
void soCollisionShieldModuleNull::setReflectorData(soCollisionReflectorData*, u32) { }
void soCollisionShieldModuleNull::set(u32, soCollisionShieldData*, u32) { }
void soCollisionShieldModuleNull::add(soCollisionReflectorGroupData*, u32) { }
void soCollisionShieldModuleNull::add(soCollisionShieldGroupData*, u32) { }
void soCollisionShieldModuleNull::clean() { }
void soCollisionShieldModuleNull::begin() { }
void soCollisionShieldModuleNull::deactivate() { }
void soCollisionShieldModuleNull::activate(double, double, double, u32) { }
void soCollisionShieldModuleNull::activate(double, double, double) { }
soCollisionShieldModuleNull::~soCollisionShieldModuleNull() { }

// soCollisionHitModuleNull
void soCollisionHitModuleNull::renderDebug() { }
Vec3f soCollisionHitModuleNull::getCenterPos(u16, u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
float soCollisionHitModuleNull::getPosX(u32) { return 0.0f; }
void soCollisionHitModuleNull::setPosX(float, u32) { }
void soCollisionHitModuleNull::setReactionFrame(short) { }
int soCollisionHitModuleNull::getReactionFrame() { return 0; }
bool soCollisionHitModuleNull::isReactionFrame() { return false; }
u32 soCollisionHitModuleNull::getGroupNum() { return 0; }
void soCollisionHitModuleNull::checkLog() { }
void soCollisionHitModuleNull::setSelfCategory(soCollision::Category, u32) { }
void soCollisionHitModuleNull::setOpponentCategory(u32, bool, u32) { }
void soCollisionHitModuleNull::initMultiSituation(u32) { }
void soCollisionHitModuleNull::setMultiSituation(SituationKind, u32) { }
void soCollisionHitModuleNull::setGlobalOffset(u8, u32) { }
void soCollisionHitModuleNull::setNoTeam(bool) { }
u32 soCollisionHitModuleNull::getPartHeight(u32, u32) { return 1; }
int soCollisionHitModuleNull::getPartSize(u32) { return 0; }
float soCollisionHitModuleNull::getDurabilityMulFromAttackData(soCollisionAttackData*, u32) { return 0.0f; }
int soCollisionHitModuleNull::getTotalStatus(u32) { return 0; }
int soCollisionHitModuleNull::getStatus(u32, u32) { return 0; }
int soCollisionHitModuleNull::getGlobal(u32) { return 3; }
int soCollisionHitModuleNull::getInvincibleFrameGlobal(u32) { return 0; }
void soCollisionHitModuleNull::setInvincibleFrameGlobal(int, bool, u32) { }
void soCollisionHitModuleNull::cancelInvincibleGlobal(u32) { }
void soCollisionHitModuleNull::setInvincibleGlobal(u32) { }
void soCollisionHitModuleNull::setXluFrameGlobal(int, u32) { }
void soCollisionHitModuleNull::cancelXluGlobal(u32) { }
void soCollisionHitModuleNull::setXluGlobal(u32) { }
void soCollisionHitModuleNull::resetStatusGlobal(u32) { }
void soCollisionHitModuleNull::update(float, float, float, SituationKind) { }
void soCollisionHitModuleNull::update(float, float, float, SituationKind, u32) { }
void soCollisionHitModuleNull::setCheckCatch(bool, u32) { }
int soCollisionHitModuleNull::getWhole(u32) { return 3; }
void soCollisionHitModuleNull::setWhole(int, u32) { }
void soCollisionHitModuleNull::setStatusAll(int, u32) { }
void soCollisionHitModuleNull::setStatusNodeDefault(int, int, u32) { }
void soCollisionHitModuleNull::setStatusNode(int, int, u32) { }
void soCollisionHitModuleNull::setStatus(u32, int, u32) { }
void soCollisionHitModuleNull::set(soSet<soCollisionHitData::Simple>*, u32) { }
void soCollisionHitModuleNull::set(soSet<soCollisionHitData>*, u32) { }
void soCollisionHitModuleNull::set(u32, soCollisionHitData*, u32) { }
void soCollisionHitModuleNull::resetStatusAll(u32) { }
void soCollisionHitModuleNull::add(soSet<soCollisionHitData::Simple>*, u32) { }
void soCollisionHitModuleNull::add(soSet<soCollisionHitData>*, u32) { }
void soCollisionHitModuleNull::sleep(bool) { }
void soCollisionHitModuleNull::clean() { }
void soCollisionHitModuleNull::begin() { }
void soCollisionHitModuleNull::deactivate() { }
void soCollisionHitModuleNull::reset(float, float, float, u32, u32) { }
void soCollisionHitModuleNull::activate(float, float, float, int, u32) { }
void soCollisionHitModuleNull::activate(float, float, float, u32, u32) { }
soCollisionHitModuleNull::~soCollisionHitModuleNull() { }

// soCollisionAttackModuleNull
void soCollisionAttackModuleNull::renderDebug() { }
void soCollisionAttackModuleNull::setRestrict(bool) { }
bool soCollisionAttackModuleNull::checkRebound(soModuleAccesser*, StageObject*, const soCollisionLog*, float, float*, float*) { return false; }
void soCollisionAttackModuleNull::invalidLog(soCollisionLog*, u32) { }
void soCollisionAttackModuleNull::addPastLog(void*, u32, u32) { }
soCollision* soCollisionAttackModuleNull::getCollision() { return 0; }
int soCollisionAttackModuleNull::getIndirectTeamNo() { return -1; }
int soCollisionAttackModuleNull::getIndirectTaskId() { return -1; }
void soCollisionAttackModuleNull::setIndirectInfo(int, int) { }
int soCollisionAttackModuleNull::isInflictStatus() { return 0; }
int soCollisionAttackModuleNull::isInflict() { return 0; }
void soCollisionAttackModuleNull::unRefLogGroup(u32) { }
void soCollisionAttackModuleNull::refLogGroup(u32, u32) { }
bool soCollisionAttackModuleNull::check() { return false; }
void soCollisionAttackModuleNull::checkLog() { }
void soCollisionAttackModuleNull::update(Vec2f*, float, float, SituationKind, Vec2f*) { }
u32 soCollisionAttackModuleNull::getPartSize() { return 0; }
float soCollisionAttackModuleNull::getLr() { return 1.0f; }
void soCollisionAttackModuleNull::setReactionMul(float) { }
float soCollisionAttackModuleNull::getPowerMul2nd() { return 1.0f; }
void soCollisionAttackModuleNull::setPowerMul2nd(float) { }
float soCollisionAttackModuleNull::getPowerMul() { return 1.0f; }
void soCollisionAttackModuleNull::setPowerMul(float) { }
void soCollisionAttackModuleNull::setPowerSpeedStatus(float) { }
float soCollisionAttackModuleNull::getPowerSpeedStatus() { return 0.0f; }
void soCollisionAttackModuleNull::setPowerAddStatus(float) { }
float soCollisionAttackModuleNull::getPowerMulStatus() { return 1.0f; }
void soCollisionAttackModuleNull::setPowerMulStatus(float) { }
soCollisionAttackData* soCollisionAttackModuleNull::getData(u32, bool) { return 0; }
void soCollisionAttackModuleNull::setSpeed(Vec2f*) { }
Vec2f soCollisionAttackModuleNull::getSpeed() { return Vec2f(0.0f, 0.0f); }
float soCollisionAttackModuleNull::getSpeedX() { return 0.0f; }
Vec3f soCollisionAttackModuleNull::getCenterPos(u32, bool) { return Vec3f(0.0f, 0.0f, 0.0f); }
float soCollisionAttackModuleNull::getPosX() { return 0.0f; }
float soCollisionAttackModuleNull::getPosX(u32, bool) { return 0.0f; }
void soCollisionAttackModuleNull::setPosX(u32, float) { }
float soCollisionAttackModuleNull::getReactionMul(u32) { return 1.0f; }
void soCollisionAttackModuleNull::setReactionAdd(u32, int, bool) { }
int soCollisionAttackModuleNull::getReactionAdd(u32, bool) { return 0; }
void soCollisionAttackModuleNull::setReactionFix(u32, int, bool) { }
int soCollisionAttackModuleNull::getReactionFix(u32, bool) { return 0; }
void soCollisionAttackModuleNull::setReactionEffect(u32, int, bool) { }
int soCollisionAttackModuleNull::getReactionEffect(u32, bool) { return 0; }
u32 soCollisionAttackModuleNull::getGroup(u32) { return 0; }
float soCollisionAttackModuleNull::getPower(u32, bool) { return 0.0f; }
float soCollisionAttackModuleNull::getBasePower(u32, bool) { return 0.0f; }
void soCollisionAttackModuleNull::setNode(u32, int) { }
bool soCollisionAttackModuleNull::setOffsetFromNode(u32, int) { return false; }
bool soCollisionAttackModuleNull::setOffset(u32, Vec3f*) { return false; }
void soCollisionAttackModuleNull::setTargetCategory(u32, soCollision::CategoryMask) { }
void soCollisionAttackModuleNull::setSize(u32, float) { }
float soCollisionAttackModuleNull::getSize(u32) { return 0.0f; }
void soCollisionAttackModuleNull::setVector(u32, int) { }
void soCollisionAttackModuleNull::setPower(u32, int, bool) { }
bool soCollisionAttackModuleNull::isInvalidXlu(u32, bool) { return false; }
bool soCollisionAttackModuleNull::isInvalidInvincible(u32, bool) { return false; }
bool soCollisionAttackModuleNull::isAttack(u32, bool) { return false; }
void soCollisionAttackModuleNull::setWhole(u32, bool) { }
void soCollisionAttackModuleNull::hitAbsolute(u32, const char*, int, int, int) { }
void soCollisionAttackModuleNull::hitAbsolute(u32, int, int, int, int) { }
void soCollisionAttackModuleNull::hitAbsolute(u32, Vec3f*, int, int, int) { }
void soCollisionAttackModuleNull::setAbsolute(u32, u32, soCollisionAttackAbsoluteData*) { }
void soCollisionAttackModuleNull::set(u32, u32) { }
void soCollisionAttackModuleNull::set(u32, u32, soCollisionAttackData*) { }
void soCollisionAttackModuleNull::sleep(bool) { }
void soCollisionAttackModuleNull::clear(u32) { }
void soCollisionAttackModuleNull::clearAll() { }
void soCollisionAttackModuleNull::begin() { }
void soCollisionAttackModuleNull::deactivate() { }
void soCollisionAttackModuleNull::activate(float, float, int, Vec2f*, u32, bool) { }
soCollisionAttackModuleNull::~soCollisionAttackModuleNull() { }


inline soSituationModule::~soSituationModule() { }
inline soGroundTracer::~soGroundTracer() { }
inline soGroundModule::~soGroundModule() { }
inline soEventObserverRegistrationDesc::~soEventObserverRegistrationDesc() { }
inline soLinkModule::~soLinkModule() { }

// --- soSituationModuleNull ---
void soSituationModuleNull::update() { }
void soSituationModuleNull::setKeepAir(bool) { }
void soSituationModuleNull::setKind(SituationKind, bool) { }
bool soSituationModuleNull::isSituationChanged() { return false; }
SituationKind soSituationModuleNull::getPrevKind() { return Situation_Outfield; }
SituationKind soSituationModuleNull::getKind() { return Situation_Outfield; }
void soSituationModuleNull::deactivate() { }
void soSituationModuleNull::activate() { }
soSituationModuleNull::~soSituationModuleNull() { }

// --- soPostureModuleNull ---
void soPostureModuleNull::fixRot() { }
void soPostureModuleNull::fixConstraintNodePosture(int, void*) { }
void soPostureModuleNull::setSyncConstraintNode(int) { }
bool soPostureModuleNull::setStickLR(float) { return false; }
float soPostureModuleNull::interpolate(float t) { return t; }
void soPostureModuleNull::turnBack() { }
void soPostureModuleNull::rewind() { }
void soPostureModuleNull::begin() { }
void soPostureModuleNull::setOwnerScale(float) { }
float soPostureModuleNull::getOwnerScale() { return 1.0f; }
void soPostureModuleNull::initScale() { }
void soPostureModuleNull::setScale(float) { }
float soPostureModuleNull::getScale() { return 1.0f; }
float soPostureModuleNull::getBaseScale() { return 1.0f; }
void soPostureModuleNull::reverseRotYLr() { }
void soPostureModuleNull::updateRotYLr() { }
float soPostureModuleNull::getRotYLr() { return 1.0f; }
bool soPostureModuleNull::isRotYLrDifferentInnerLr() { return false; }
void soPostureModuleNull::initRotYLr() { }
void soPostureModuleNull::setRot(Vec3f*, u32) { }
Vec3f soPostureModuleNull::getRot(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soPostureModuleNull::initRot(u32) { }
void soPostureModuleNull::initRot() { }
void soPostureModuleNull::reverseLr() { }
void soPostureModuleNull::setLr(float) { }
float soPostureModuleNull::getLr() { return 1.0f; }
void soPostureModuleNull::setPrevRoughPos(Vec3f*) { }
Vec3f soPostureModuleNull::getPrevRoughPos() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soPostureModuleNull::getPrevPos() { return Vec3f(0.0f, 0.0f, 0.0f); }
void soPostureModuleNull::setPos(Vec3f*) { }
Vec3f soPostureModuleNull::getPos() { return Vec3f(0.0f, 0.0f, 0.0f); }
void soPostureModuleNull::initPos(Vec3f*) { }
void soPostureModuleNull::init(Vec3f*, float) { }
soPostureModuleNull::~soPostureModuleNull() { }

// --- soGroundTracerNull ---
bool soGroundTracerNull::isTouchCurrentLine(soGroundModule*, u32) { return false; }
bool soGroundTracerNull::isOpenTraceLineCurrentAndNext(soGroundModule*, u32) { return true; }
bool soGroundTracerNull::isSameNormalTraceLineCurrentAndNext(soGroundModule*, u32) { return false; }
Vec2f soGroundTracerNull::getVecFromCurrentLine(soGroundModule*, u32) { return Vec2f(0.0f, 0.0f); }
float soGroundTracerNull::getDistanceFromCurrentLine(soGroundModule*, u32, bool) { return 0.0f; }
Vec2f soGroundTracerNull::getGroundMovementSpeed() { return Vec2f(0.0f, 0.0f); }
void soGroundTracerNull::update(bool, soGroundModule*, u32) { }
void soGroundTracerNull::end() { }
bool soGroundTracerNull::renew(u32, u8, soPostureModule*, soGroundModule*, u32, float, float) { return false; }
bool soGroundTracerNull::canIntercept(u32, u8, soGroundModule*, u32, float) { return false; }
bool soGroundTracerNull::isArrived(soPostureModule*, soGroundModule*, u32, float) { return false; }
Vec2f soGroundTracerNull::getTraceTargetPosturePos(soGroundModule*, u32, float) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundTracerNull::getTraceTargetVec(soGroundModule*, u32, float) { return Vec2f(0.0f, 0.0f); }
bool soGroundTracerNull::start(u32, u8, soPostureModule*, soGroundModule*, u32, u8, float, float, float) { return false; }
bool soGroundTracerNull::canStart(u32, u8, soGroundModule*, u32, float) { return false; }
bool soGroundTracerNull::isTraced() { return false; }
soGroundTracerNull::~soGroundTracerNull() { }

// --- soGroundModuleNull ---
static soGroundConditionCheckerNull s_soGroundConditionCheckerNull;

void soGroundModuleNull::setDetachChangeStatus(bool) { }
void soGroundModuleNull::setIgnoreFriction(bool) { }
soGroundConditionChecker* soGroundModuleNull::getConditionChecker() { return &s_soGroundConditionCheckerNull; }
void soGroundModuleNull::testGround(u32) { }
void soGroundModuleNull::updateTrace(bool, u32) { }
bool soGroundModuleNull::isTouchTraceLine(u32) { return true; }
bool soGroundModuleNull::isOpenTraceLineCurrentAndNext(u32) { return true; }
bool soGroundModuleNull::isSameNormalTraceLineCurrentAndNext(u32) { return false; }
Vec2f soGroundModuleNull::getTraceVecFromLine(u32) { return Vec2f(0.0f, 0.0f); }
float soGroundModuleNull::getTraceDistanceFromLine(u32) { return 0.0f; }
Vec2f soGroundModuleNull::getTraceGroundMovementSpeed() { return Vec2f(0.0f, 0.0f); }
void soGroundModuleNull::endTrace(u32) { }
bool soGroundModuleNull::renewTrace(u8, u32, u32, float, float) { return false; }
bool soGroundModuleNull::canTraceIntercept(u8, u32, u32, float) { return false; }
bool soGroundModuleNull::isTraceArrived(u32, float) { return false; }
Vec2f soGroundModuleNull::getTraceTargetPosturePos(u32, float) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getTraceTargetVec(u32, float) { return Vec2f(0.0f, 0.0f); }
bool soGroundModuleNull::startTrace(u8, u32, u32, u8, float, float, float) { return false; }
bool soGroundModuleNull::canTraceStart(u8, u32, u32, float) { return false; }
bool soGroundModuleNull::isTraced() { return false; }
void soGroundModuleNull::renderDebug() { }
void soGroundModuleNull::setGrCollisionFlagDefaultAll() { }
void soGroundModuleNull::setGrCollisionFlagAll(u16, int) { }
void soGroundModuleNull::setGrCollisionVertexPos(u16, Vec2f*) { }
void soGroundModuleNull::initGrCollisionVertexPos(u16, Vec2f*) { }
u16 soGroundModuleNull::getGrCollisionVertexNum() { return 0; }
void soGroundModuleNull::setGrCollisionMode(bool) { }
bool soGroundModuleNull::isEnableGrCollision() { return false; }
grCollision* soGroundModuleNull::getGrCollision() { return NULL; }
bool soGroundModuleNull::rayCheck(Vec3f*, Vec3f*, int*, Vec3f*, Vec3f*, bool) { return false; }
bool soGroundModuleNull::rayCheck(Vec3f*, Vec3f*, Vec3f*, Vec3f*, bool) { return false; }
bool soGroundModuleNull::rayCheck(Vec3f*, Vec3f*, bool) { return false; }
bool soGroundModuleNull::rayCheckTarget(StageObject*, bool) { return false; }
void soGroundModuleNull::test(u32) { }
void soGroundModuleNull::getAttackPointBreak(bool*, bool*, bool*, bool*, u32) { }
void soGroundModuleNull::setAttackPoint(bool, bool, bool, bool, bool, u32) { }
void soGroundModuleNull::detach(u32) { }
bool soGroundModuleNull::isAttach(u32) { return false; }
void soGroundModuleNull::attach(u8, u32) { }
bool soGroundModuleNull::isAttachable(int, u32) { return false; }
bool soGroundModuleNull::attachGround(u32) { return false; }
float soGroundModuleNull::getZ() { return 0.0f; }
void soGroundModuleNull::setZ(float) { }
bool soGroundModuleNull::isPassableGround(u32) { return false; }
void soGroundModuleNull::setShapeKind(soGroundShapeImpl::ShapeKind, u32) { }
void soGroundModuleNull::setShapeSafePos(Vec2f*, u32) { }
bool soGroundModuleNull::checkShapeFlag(soGroundShapeImpl::ShapeFlagId, u32) { return false; }
void soGroundModuleNull::setShapeFlag(soGroundShapeImpl::ShapeFlagId, bool, u32) { }
void soGroundModuleNull::setOffsetY(float, u32) { }
void soGroundModuleNull::setOffsetX(float, u32) { }
void soGroundModuleNull::calcRhombusModify(float*, float*, float*, float*, u32) { }
void soGroundModuleNull::getRhombus(clRhombus2D*, clRhombus2D*, u32) { }
void soGroundModuleNull::getRhombus(float*, float*, float*, int*, u32) { }
void soGroundModuleNull::getRhombus(float*, float*, float*, u32) { }
void soGroundModuleNull::modifyRhombusFollowNode(float, float, float, int, u32) { }
void soGroundModuleNull::modifyRhombusWidth(float, u32) { }
void soGroundModuleNull::modifyRhombusDown(float, u32) { }
void soGroundModuleNull::modifyRhombusUp(float, u32) { }
void soGroundModuleNull::modifyRhombus(float, float, float, u32) { }
void soGroundModuleNull::initRhombus(u32) { }
bool soGroundModuleNull::isCliffOverPos(Vec2f*, u8, u32) { return false; }
bool soGroundModuleNull::isCliffOver(u8, u32) { return false; }
float soGroundModuleNull::getDistanceFromUnderGrCol(float, Vec3f*, u8) { return 0.0f; }
float soGroundModuleNull::getDownFriction(u32) { return 0.0f; }
bool soGroundModuleNull::getDownCorrectPos(Vec3f*, Vec3f*, u32) { return false; }
Vec2f soGroundModuleNull::getDownMovementSpeed(u32) { return Vec2f(0.0f, 0.0f); }
float soGroundModuleNull::getWidth(u32) { return 0.0f; }
Vec2f soGroundModuleNull::getCenterPos(u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getRightPos(u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getLeftPos(u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getDownPos(u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getUpPos(u32) { return Vec2f(0.0f, 0.0f); }
bool soGroundModuleNull::isSameJoint(int*, int*) { return false; }
u32 soGroundModuleNull::getCliffId(u32) { return 0; }
u32 soGroundModuleNull::getUpRightLineId(u32) { return 0; }
u32 soGroundModuleNull::getUpLeftLineId(u32) { return 0; }
u32 soGroundModuleNull::getDownRightLineId(u32) { return 0; }
u32 soGroundModuleNull::getDownLeftLineId(u32) { return 0; }
u32 soGroundModuleNull::getUpLineId(u32) { return 0; }
u32 soGroundModuleNull::getLeftLineId(u32) { return 0; }
u32 soGroundModuleNull::getRightLineId(u32) { return 0; }
u32 soGroundModuleNull::getDownLineId(u32) { return 0; }
bool soGroundModuleNull::isOnDynamicCollision(u32) { return false; }
bool soGroundModuleNull::isCrush(float, u32) { return false; }
bool soGroundModuleNull::isDiveDown(u32) { return true; }
void soGroundModuleNull::ignoreTouchLine(grCollStatus::TouchMask, u32) { }
bool soGroundModuleNull::getTouchNextNormal(bool, Vec2f*, grCollStatus::TouchMask, u32) { return false; }
float soGroundModuleNull::getTouchLineRate(grCollStatus::TouchMask, u32) { return 0.0f; }
float soGroundModuleNull::getTouchLength(grCollStatus::TouchMask, u32) { return 0.0f; }
u32 soGroundModuleNull::getTouchCount(u32) { return 0; }
bool soGroundModuleNull::getTouchAttackData(soCollisionAttackData*, int*, int*, u32) { return false; }
Vec2f soGroundModuleNull::getTouchPos(int, u32) { return Vec2f(0.0f, 0.0f); }
grCollisionLine::MaterialType soGroundModuleNull::getTouchMaterialType(grCollStatus::TouchMask, u32) { return grCollisionLine::Material_None; }
int soGroundModuleNull::getTouchFlag(u32) { return 0; }
Vec2f soGroundModuleNull::getAttachNormal(u32) { return Vec2f(0.0f, 1.0f); }
Vec2f soGroundModuleNull::getAttachMovementSpeed(u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getTouchSpeed(grCollStatus::TouchMask, u32) { return Vec2f(0.0f, 0.0f); }
Vec2f soGroundModuleNull::getTouchNormal(int, u32) { return Vec2f(0.0f, 1.0f); }
bool soGroundModuleNull::isTouch(int, u32) { return false; }
bool soGroundModuleNull::isMissStepL(u32) { return false; }
bool soGroundModuleNull::isMissStepR(u32) { return false; }
bool soGroundModuleNull::isNearCliff(float, float, u32) { return false; }
bool soGroundModuleNull::isOttotto(float, u32) { return false; }
grCollStatus* soGroundModuleNull::getCollStatus(u32) { return NULL; }
void soGroundModuleNull::setEventToGroundStatus(bool, u32) { }
void soGroundModuleNull::setEventToGround(bool, u32) { }
void soGroundModuleNull::setTestDynamicCollision(bool, u32) { }
void soGroundModuleNull::setTestDynamicCollisionStatus(bool, u32) { }
void soGroundModuleNull::setTestPreMapStatus(bool, u32) { }
void soGroundModuleNull::setTestPreMap(bool, u32) { }
void soGroundModuleNull::setTestCollStopStatus(bool, u32) { }
void soGroundModuleNull::setTestCollStop(bool, u32) { }
void soGroundModuleNull::setStGroundForce(u32) { }
float soGroundModuleNull::getCliffDir(u32) { return 0.0f; }
Vec3f soGroundModuleNull::getCliffPos(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soGroundModuleNull::leaveCliff(u32) { }
void soGroundModuleNull::entryCliff(u32) { }
void soGroundModuleNull::setCliffTeam(u8, u32) { }
Vec3f soGroundModuleNull::getCorrectPos(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
bool soGroundModuleNull::isStCliff(u32) { return false; }
void soGroundModuleNull::setCliffBackRangeXMul(float) { }
void soGroundModuleNull::selectCliffHangData(u32, u32) { }
void soGroundModuleNull::setCliffHangData(void*) { }
void soGroundModuleNull::setCliffCheck(soGroundShapeImpl::CliffCheckKind, u32) { }
void soGroundModuleNull::setCorrectIgnoreSlope(bool, u32) { }
bool soGroundModuleNull::isCloudThroughOut(Vec2f*, u32) { return false; }
bool soGroundModuleNull::isCloudThrough(u32) { return false; }
void soGroundModuleNull::setCloudThrough(bool, u32) { }
bool soGroundModuleNull::isPassableCheck(u32) { return true; }
void soGroundModuleNull::setPassableCheck(bool, u32) { }
void soGroundModuleNull::setExclusiveLineAttr(short, u32) { }
void soGroundModuleNull::setTargetLineCategory(grCollisionLine::CategoryMask, u32) { }
void soGroundModuleNull::setCalcLineMove(bool, u32) { }
soGroundShapeImpl::CorrectKind soGroundModuleNull::getCorrect(u32) { return soGroundShapeImpl::Correct_None; }
void soGroundModuleNull::setCorrect(soGroundShapeImpl::CorrectKind, u32) { }
bool soGroundModuleNull::isReferenceModelNode() { return false; }
void soGroundModuleNull::apply() { }
void soGroundModuleNull::pause(bool) { }
void soGroundModuleNull::updateGrCollisionMatrix() { }
void soGroundModuleNull::updateCheckGrCollision() { }
float soGroundModuleNull::getRotateYForGrCollisionMatrix() { return 0.0f; }
void soGroundModuleNull::constructGrCollision(int, int, Heaps::HeapType, u8, u8, u8) { }
void soGroundModuleNull::processFixPosition() { }
void soGroundModuleNull::update(u32) { }
void soGroundModuleNull::begin() { }
void soGroundModuleNull::relocate(Vec2f*, u32) { }
void soGroundModuleNull::deactivate() { }
void soGroundModuleNull::activate(int, Vec2f*) { }
void soGroundModuleNull::clean() { }
void soGroundModuleNull::setup(void*, int, u8) { }
soGroundModuleNull::~soGroundModuleNull() { }

// --- soGroundConditionCheckerNull ---
bool soGroundConditionCheckerNull::isConditionNoCliffStop(soModuleAccesser*) { return false; }
soGroundConditionCheckerNull::~soGroundConditionCheckerNull() { }

// --- soCameraModuleNull ---
void soCameraModuleNull::setOffsetGlobalRectCenter(Rect2D*, soModuleAccesser*, u32) { }
void soCameraModuleNull::setCameraRangeGlobalRect(Rect2D*, soModuleAccesser*, u32) { }
void soCameraModuleNull::getCameraRangeGlobalRect(Rect2D*, u32) { }
void soCameraModuleNull::renderDebug() { }
soCameraSubject* soCameraModuleNull::getSubject(u32) { return NULL; }
float soCameraModuleNull::getSubjectOffsetY(u32) { return 0.0f; }
Vec3f soCameraModuleNull::getSubjectPos(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soCameraModuleNull::getSubjectRange(Rect2D*, u32) { }
void soCameraModuleNull::exitRun(u32) { }
void soCameraModuleNull::setRun(u32) { }
void soCameraModuleNull::exitDamageFly(u32) { }
void soCameraModuleNull::setDamageFly(u32) { }
void soCameraModuleNull::setDamage(short, u32) { }
u8 soCameraModuleNull::getPlayerNo(u32) { return 0; }
void soCameraModuleNull::setPlayerNo(u8, int) { }
bool soCameraModuleNull::isLocked() { return true; }
soCameraClipSphere* soCameraModuleNull::getClipSphere(u32) { return NULL; }
bool soCameraModuleNull::isClipInAll(bool) { return true; }
bool soCameraModuleNull::isClipIn(bool) { return true; }
u8 soCameraModuleNull::getClipOutStatus(bool) { return 0xff; }
void soCameraModuleNull::setDemoDistScaleMul(float, u32) { }
u8 soCameraModuleNull::getAdvPri(u32) { return 0; }
void soCameraModuleNull::setAdvPri(u8, int) { }
u8 soCameraModuleNull::getPri(u32) { return 0; }
void soCameraModuleNull::setPri(u8, int) { }
Vec3f soCameraModuleNull::getMainCameraTargetPos() { return Vec3f(0.0f, 0.0f, 0.0f); }
soCameraModule::QuakeKind soCameraModuleNull::getQuakeKind() { return Quake_None; }
void soCameraModuleNull::stopQuake(QuakeKind) { }
void soCameraModuleNull::reqQuake(QuakeKind, Vec3f*) { }
void soCameraModuleNull::reqQuake(QuakeKind, soModuleAccesser*) { }
void soCameraModuleNull::setQuake(bool) { }
void soCameraModuleNull::zoomOut() { }
void soCameraModuleNull::zoomIn(short, u32, Vec2f*, float) { }
void soCameraModuleNull::unRegistCamera(u32) { }
void soCameraModuleNull::addCameraRangeZoom(float, u32) { }
void soCameraModuleNull::addCameraRangeRect(Rect2D*, u32) { }
void soCameraModuleNull::addCameraRangeOffset(Vec3f*, u32) { }
void soCameraModuleNull::resetCameraRange(u32) { }
soCameraRange* soCameraModuleNull::getCameraRangeData(u32) { return NULL; }
void soCameraModuleNull::setEnableInterpolatePos(bool, int) { }
void soCameraModuleNull::setEnableUpdatePos(bool, int) { }
void soCameraModuleNull::setEnableCamera(bool, int) { }
void soCameraModuleNull::setWhole(bool) { }
void soCameraModuleNull::updateForce(u32, Vec3f*, Vec3f*) { }
void soCameraModuleNull::update() { }
void soCameraModuleNull::resetAll() { }
void soCameraModuleNull::deactivate() { }
void soCameraModuleNull::activate() { }
void soCameraModuleNull::setZoomParam(soCameraZoom*, u32) { }
void soCameraModuleNull::setTrainingRangeData(soCameraRange*, u32) { }
void soCameraModuleNull::setup(soSet<soCameraRange>*) { }
void soCameraModuleNull::setup() { }
u32 soCameraModuleNull::getSubjectsSize() { return 0; }
soCameraModuleNull::~soCameraModuleNull() { }
static soCameraModuleNull s_soCameraModuleNull;

// --- soControllerModuleNull ---
void soControllerModuleNull::resetFlickBonusLr() { }
void soControllerModuleNull::resetFlickBonus() { }
u8 soControllerModuleNull::getFlickBonusLr() { return 0; }
u8 soControllerModuleNull::getFlickBonus() { return 0; }
float soControllerModuleNull::getLr() { return 0.0f; }
void soControllerModuleNull::setReverseXFrame(s32, bool) { }
void soControllerModuleNull::stopRumbleAll(s32, s32) { }
void soControllerModuleNull::setRumbleAll(s32, s32, s32) { }
void soControllerModuleNull::stopRumble(bool) { }
void soControllerModuleNull::stopRumbleKind(s32, s32) { }
void soControllerModuleNull::setRumble(s32, s32, bool, s32) { }
soController* soControllerModuleNull::getController() { return &g_soControllerNull; }
float soControllerModuleNull::getClatterThreshold(u32) { return 0.0f; }
void soControllerModuleNull::endClatter(u32) { }
float soControllerModuleNull::getClatterTime(u32) { return 0.0f; }
void soControllerModuleNull::addClatterTime(float, u32) { }
void soControllerModuleNull::setClatterTime(float, u32) { }
void soControllerModuleNull::startClatter(float, float, float, u8, u8, u32, bool) { }
void soControllerModuleNull::setLogActive(bool) { }
s32 soControllerModuleNull::getLogNum() { return 0; }
void soControllerModuleNull::clearLog() { }
void soControllerModuleNull::setPrev(s32) { }
void soControllerModuleNull::setOff(bool) { }
int soControllerModuleNull::getRelease() { return 0; }
int soControllerModuleNull::getButton() { return 0; }
u8 soControllerModuleNull::getTriggerCountPrev(u8) { return 0; }
u8 soControllerModuleNull::getTriggerCount(u8) { return 0; }
int soControllerModuleNull::getTrigger() { return 0; }
bool soControllerModuleNull::isSubStickSide() { return false; }
float soControllerModuleNull::getSubStickDir() { return 0.0f; }
float soControllerModuleNull::getSubStickPrevY() { return 0.0f; }
float soControllerModuleNull::getSubStickY() { return 0.0f; }
float soControllerModuleNull::getSubStickPrevX() { return 0.0f; }
float soControllerModuleNull::getSubStickX() { return 0.0f; }
u8 soControllerModuleNull::getFlickAfterY() { return 0; }
s8 soControllerModuleNull::getFlickAfterXDir() { return 0; }
u8 soControllerModuleNull::getFlickAfterX() { return 0; }
u8 soControllerModuleNull::getFlickNoResetY() { return 0; }
u8 soControllerModuleNull::getFlickNoResetX() { return 0; }
s8 soControllerModuleNull::getFlickYDir() { return 0; }
u8 soControllerModuleNull::getFlickY() { return 0; }
s8 soControllerModuleNull::getFlickXDir() { return 0; }
u8 soControllerModuleNull::getFlickX() { return 0; }
bool soControllerModuleNull::isStickSide() { return false; }
float soControllerModuleNull::getStickDir() { return 0.0f; }
float soControllerModuleNull::getStickAngle() { return 0.0f; }
float soControllerModuleNull::getStickPrevY() { return 0.0f; }
float soControllerModuleNull::getStickY() { return 0.0f; }
float soControllerModuleNull::getStickPrevX() { return 0.0f; }
float soControllerModuleNull::getStickX() { return 0.0f; }
void soControllerModuleNull::resetFlickY() { }
void soControllerModuleNull::resetFlickX() { }
void soControllerModuleNull::update(Input*, bool) { }
void soControllerModuleNull::resetSubStick() { }
void soControllerModuleNull::resetSubStickY() { }
void soControllerModuleNull::resetSubStickX() { }
void soControllerModuleNull::resetMainStick() { }
void soControllerModuleNull::setMainStickY(float) { }
void soControllerModuleNull::resetMainStickY() { }
void soControllerModuleNull::setMainStickX(float) { }
void soControllerModuleNull::resetMainStickX() { }
void soControllerModuleNull::resetTrigger() { }
void soControllerModuleNull::resetButton() { }
void soControllerModuleNull::deactivate() { }
void soControllerModuleNull::activate() { }
soControllerModuleNull::~soControllerModuleNull() { }

// --- soEventObserverRegistrationDescNull ---
soEventObserverRegistrationDescNull g_soEventObserverRegistrationDescNull;

// --- soEventManageModuleNull ---
static soEventUnitPureNull s_soEventUnitPureNull;

s16 soEventManageModuleNull::addUnit(soEventUnit*) { return -1; }
void soEventManageModuleNull::eraseUnit(u32) { }
bool soEventManageModuleNull::isNullUnit(u32) { return true; }
void soEventManageModuleNull::eraseObserver(u32, int) { }
u32 soEventManageModuleNull::getObserverCapacity(u32) { return 0; }
u32 soEventManageModuleNull::getObserverSize(u32) { return 0; }
bool soEventManageModuleNull::isExist(short) { return false; }
s16 soEventManageModuleNull::getManageId() { return -1; }
soEventUnit* soEventManageModuleNull::getEventUnit(short) { return &s_soEventUnitPureNull; }

// --- soEventUnitPureNull ---
soEventUnitPureNull::~soEventUnitPureNull() { }
bool soEventUnitPureNull::isNull() const { return true; }
u32 soEventUnitPureNull::getObserverSize(u32) { return 0; }
u32 soEventUnitPureNull::getObserverCapacity(u32) { return 0; }
s16 soEventUnitPureNull::getEventId() { return -1; }
void soEventUnitPureNull::eraseObserver(s16, s16) { }

// --- soLinkModuleNull ---
bool soLinkModuleNull::isNodeGroundCrush(float) { return false; }
void soLinkModuleNull::getLinkParentCameraInfo(u32, u8*, u8*, u8*) { }
bool soLinkModuleNull::getLinkParentSlowInfo(float* rate, bool* isSkip, int) {
    *rate = 1.0f;
    *isSkip = false;
    return false;
}
bool soLinkModuleNull::isLinkParentSlow(int) { return false; }
void soLinkModuleNull::getCmdAddressPackParents(u32, soArray<soAnimCmdAddressPack>*, int, short*) { }
soArticle* soLinkModuleNull::generateArticleLinkParents(u32, int, soModuleAccesser*) { return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage); }
bool soLinkModuleNull::getLinkSlopeRotation(int, Vec3f* rot) {
    rot->m_x = rot->m_y = rot->m_z = 0.0f;
    return false;
}
soSlopeStatusParam::Status soLinkModuleNull::getLinkSlopeStatus(int) { return soSlopeStatusParam::Status(soSlopeStatusParam::STATUS_MASK_NONE); }
void soLinkModuleNull::setAnmFileTransactorLinkNo(int) { }
nw4r::g3d::ResFile soLinkModuleNull::getAnmFile(u16) { return nw4r::g3d::ResFile((void*)NULL); }
bool soLinkModuleNull::chkLinkedAttribute(soLinkConnection::Attribute) { return false; }
bool soLinkModuleNull::chkAttribute(int, soLinkConnection::Attribute) { return false; }
void soLinkModuleNull::setAttribute(u32, soLinkConnection::Attribute, bool) { }
void soLinkModuleNull::calcModelConstraintNodeMtx(Matrix*, bool) { }
bool soLinkModuleNull::setUpModelConstraintTargetMtx() { return false; }
float soLinkModuleNull::getConstraintModelScale() { return 1.0f; }
bool soLinkModuleNull::getNodeRotation(u32, u32, Vec3f* rot) {
    rot->m_x = rot->m_y = rot->m_z = 0.0f;
    return false;
}
Vec3f soLinkModuleNull::getModelConstraintTargetNodeRotation() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soLinkModuleNull::getModelConstraintNodeGlobalPositionRecursive(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soLinkModuleNull::getModelConstraintNodeGlobalPosition(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soLinkModuleNull::getModelConstraintTargetNodePosition() { return Vec3f(0.0f, 0.0f, 0.0f); }
void soLinkModuleNull::getModelConstraintTargetNodeWorldMtx(Matrix*, bool) { }
bool soLinkModuleNull::isModelConstraintMutual() { return false; }
bool soLinkModuleNull::isModelConstraint() { return false; }
void soLinkModuleNull::removeModelConstraint(bool) { }
void soLinkModuleNull::adjustModelConstraintPosture(u16, Vec3f*, Vec3f*) { }
void soLinkModuleNull::adjustModelConstraintPosture(Vec3f*, Vec3f*) { }
u32 soLinkModuleNull::getModelConstraintNo() { return -1; }
void soLinkModuleNull::setModelConstraintTargetNode(int) { }
void soLinkModuleNull::setModelConstraintNode(int) { }
int soLinkModuleNull::getModelConstraintNode() { return -1; }
void soLinkModuleNull::offModelConstraintFlag(u16) { }
void soLinkModuleNull::setModelConstraintFlag(u16) { }
u16 soLinkModuleNull::getModelConstraintFlag() { return 0; }
void soLinkModuleNull::setModelConstraintAttribute(u32, bool) { }
void soLinkModuleNull::setModelConstraintPosOrt(u32, int, const char*, u16, bool) { }
void soLinkModuleNull::setModelConstraintPosOrt(u32, int, int, u16, bool) { }
bool soLinkModuleNull::isNodeDamageCaptureCut(u8) { return false; }
bool soLinkModuleNull::isNodeDamageReaction(u8) { return false; }
int soLinkModuleNull::getNodeTaskId(u8) { return -1; }
StageObject* soLinkModuleNull::getNode(u8) { return NULL; }
void soLinkModuleNull::sendEventNodes(s32, int, soLinkEventArgs&) { }
void soLinkModuleNull::sendEventNodes(s32, const soLinkEventArgs&, u32) { }
void soLinkModuleNull::sendEventParents(s32, soLinkEventArgs&) { }
StageObject* soLinkModuleNull::searchNode(int) { return NULL; }
StageObject* soLinkModuleNull::searchParentAttr(soLinkConnection::Attribute) { return NULL; }
StageObject* soLinkModuleNull::searchParent(int) { return NULL; }
StageObject* soLinkModuleNull::getParent(u32) { return NULL; }
void soLinkModuleNull::setParentAttackerInfo(u32, StageObject*) { }
void soLinkModuleNull::setParentHitStopFrame(u32, u32, bool) { }
bool soLinkModuleNull::correctParent2ndBoth(u32, Vec3f*, float) { return false; }
u8 soLinkModuleNull::getParentAttachItemVisibility(u32) { return 0; }
float soLinkModuleNull::getParentWeaponGlowDamageMax(u32) { return 0.0f; }
float soLinkModuleNull::getParentWeaponGlowDamageMin(u32) { return 0.0f; }
float soLinkModuleNull::getParentWeaponGlowEff(u32) { return 0.0f; }
int soLinkModuleNull::getParentGlowAttackAdd(soCollisionAttackData*) { return 0; }
int soLinkModuleNull::getParentWeaponGlowDefense(u32) { return 0; }
int soLinkModuleNull::getParentWeaponGlowAttack(u32) { return 0; }
int soLinkModuleNull::getParentGlowAttack(u32, soCollisionAttackData*) { return 0; }
bool soLinkModuleNull::isParentGlow(u32) { return false; }
void soLinkModuleNull::sendEnvMapNodes(bool) { }
bool soLinkModuleNull::isParentEffectSyncScale(u32) { return false; }
int soLinkModuleNull::getParentTexResourceId(u32) { return 0xFFFF; }
bool soLinkModuleNull::isParentRefFB(u32) { return false; }
bool soLinkModuleNull::isParentEnableEnvMap(u32) { return false; }
float soLinkModuleNull::getOwnerAttackPowerMulArticle(u32) { return 1.0f; }
bool soLinkModuleNull::getParentShapeCenterPosTaskId(int, Vec2f*) { return false; }
Vec2f soLinkModuleNull::getParentShapeCenterPos(u32) { return Vec2f(0.0f, 0.0f); }
bool soLinkModuleNull::isValidParentShape(u32) { return false; }
nw4r::g3d::ResFile soLinkModuleNull::getParentAnmFile(u32, u16) { return nw4r::g3d::ResFile((void*)NULL); }
int soLinkModuleNull::getParentTotalHitStatus(u32) { return 0; }
bool soLinkModuleNull::isParentDamageCatchCut(u32) { return false; }
bool soLinkModuleNull::isParentDamageReaction(u32) { return false; }
void soLinkModuleNull::setNodeScalePowerMul(float) { }
float soLinkModuleNull::getParentDamagePowerMax(u32) { return 0.0f; }
float soLinkModuleNull::getParentDamageReaction(u32) { return 0.0f; }
float soLinkModuleNull::getParentDamage(u32) { return 0.0f; }
Vec3f soLinkModuleNull::getParentRot(u32) { return Vec3f(0.0f, 0.0f, 0.0f); }
int soLinkModuleNull::getParentTaskId(u32) { return -1; }
Vec2f soLinkModuleNull::getParentSumSpeed(u32) { return Vec2f(0.0f, 0.0f); }
float soLinkModuleNull::getParentLr(u32) { return 1.0f; }
SituationKind soLinkModuleNull::getParentSituationKind(u32) { return Situation_Ground; }
float soLinkModuleNull::getParentMotionFrame(u32) { return 0.0f; }
Vec3f soLinkModuleNull::getParentModelNodeGlobalPosition(u32, const char*, bool) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soLinkModuleNull::getParentModelNodeGlobalPosition(u32, u32, bool) { return Vec3f(0.0f, 0.0f, 0.0f); }
void soLinkModuleNull::setConstraint2ndWind(u8) { }
void soLinkModuleNull::adjustLinkGroundCollision(int, float*) { }
GXColor soLinkModuleNull::getParentSubColor(u32) {
    GXColor c = { 0, 0, 0, 0 };
    return c;
}
bool soLinkModuleNull::isParentEnableSubColor(u32) { return false; }
float soLinkModuleNull::getParentScale(u32) { return 1.0f; }
float soLinkModuleNull::getLinkScale() { return 1.0f; }
soController* soLinkModuleNull::getLinkController() { return &g_soControllerNull; }
bool soLinkModuleNull::chkLinkVisibility() { return true; }
s8 soLinkModuleNull::getLinkLightSet() { return 0; }
soColorBlend* soLinkModuleNull::getLinkColorBlend() { return NULL; }
bool soLinkModuleNull::chkLinkShake(Vec2f*) { return false; }
bool soLinkModuleNull::isValidLinkStop() { return false; }
void soLinkModuleNull::leaveLinkStop() { }
void soLinkModuleNull::toLinkStop() { }
bool soLinkModuleNull::chkLinkStop() { return false; }
bool soLinkModuleNull::isLinked(int) { return false; }
bool soLinkModuleNull::isLink(int) { return false; }
void soLinkModuleNull::unlink(u32) { }
void soLinkModuleNull::unlinkNodeAll() { }
void soLinkModuleNull::unlinkNode(u32) { }
void soLinkModuleNull::unlinkAll() { }
bool soLinkModuleNull::link(u32, int) { return false; }
void soLinkModuleNull::deactivate() { }
void soLinkModuleNull::activate() { }
s8 soLinkModuleNull::getParentModelLoupeNo(u32) { return s_loupeNo; }
soLinkModuleNull::~soLinkModuleNull() { }


soArticleNull::~soArticleNull() { }
void soArticle::setAttackPowerMulPattern(float) { }
float soArticle::getActiveGlobalFrame() { return -1.0f; }
void soArticle::intrudeLogAttackInfo() { }
soLogAttackInfo soArticle::getLogAttackInfo() { return soLogAttackInfo(0, 0); }
void soArticle::setLogAttackInfo(soLogAttackInfo*) { }
u32 soArticle::getFounderTaskId() { return -1; }
int soArticle::getOwnerDeactiveTreatType() { return 0; }
void soArticle::deactivateDescendantForce() { }
void soArticle::setSituationKind(SituationKind) { }
void soArticle::action(int) { }
void soArticle::set2nd(u8) { }
void soArticle::setVisibility(int, u8) { }
void soArticle::setVisibilityWhole(u8) { }
void soArticle::changeStatus(int) { }
void soArticle::setRate(float) { }
void soArticle::setFrame(float) { }
void soArticleNull::updateLogAttackInfo() { }
void soArticleNull::setSyncOwnerStatus(bool) { }
bool soArticleNull::isSyncOwnerStatus() { return false; }
void soArticleNull::unlinkOwner(int) { }
void soArticleNull::linkOwner(int, int) { }
void soArticleNull::have(int, int, int) { }
void soArticleNull::deactivateArticle() { }
bool soArticleNull::isActiveArticle() { return false; }
int soArticleNull::getTaskId() { return -1; }
s16 soArticleNull::getArticleEventManageId() { return -1; }
int soArticleNull::getArticleId() { return -1; }
void soArticleNull::changeMotion(int, bool) { }
void soArticleNull::remove() { }
static soArticleNull s_soArticleNull;
soResourceIdAccesserNull g_soResourceIdAccesserNull;
soResourceModuleNull g_soResourceModuleNull(true);

soControllerNull::~soControllerNull() { }
void soControllerNull::resetFlickBonusLr() { }
void soControllerNull::resetFlickBonus() { }
u8 soControllerNull::getFlickBonusLr() { return 0; }
u8 soControllerNull::getFlickBonus() { return 0; }
float soControllerNull::getLr() { return 0.0f; }
int soControllerNull::getRelease() { return 0; }
int soControllerNull::getButton() { return 0; }
u8 soControllerNull::getTriggerCountPrev(u8) { return 0; }
u8 soControllerNull::getTriggerCount(u8) { return 0; }
int soControllerNull::getTrigger() { return 0; }
float soControllerNull::getSubStickPrevY() { return 0.0f; }
float soControllerNull::getSubStickY() { return 0.0f; }
float soControllerNull::getSubStickPrevX() { return 0.0f; }
float soControllerNull::getSubStickX() { return 0.0f; }
u8 soControllerNull::getFlickAfterY() { return 0; }
s8 soControllerNull::getFlickAfterXDir() { return 0; }
u8 soControllerNull::getFlickAfterX() { return 0; }
u8 soControllerNull::getFlickNoResetY() { return 0; }
u8 soControllerNull::getFlickNoResetX() { return 0; }
s8 soControllerNull::getFlickYDir() { return 0; }
u8 soControllerNull::getFlickY() { return 0; }
s8 soControllerNull::getFlickXDir() { return 0; }
u8 soControllerNull::getFlickX() { return 0; }
float soControllerNull::getStickPrevY() { return 0.0f; }
float soControllerNull::getStickY() { return 0.0f; }
float soControllerNull::getStickPrevX() { return 0.0f; }
float soControllerNull::getStickX() { return 0.0f; }
void soControllerNull::setOff(bool) { }
void soControllerNull::resetFlickY() { }
void soControllerNull::resetFlickX() { }
void soControllerNull::update(Input*, bool) { }
void soControllerNull::reset() { }
void soControllerNull::resetSubStick() { }
void soControllerNull::resetSubStickY() { }
void soControllerNull::resetSubStickX() { }
void soControllerNull::resetMainStick() { }
void soControllerNull::setMainStickY(float) { }
void soControllerNull::resetMainStickY() { }
void soControllerNull::setMainStickX(float) { }
void soControllerNull::resetMainStickX() { }
void soControllerNull::resetTrigger() { }
void soControllerNull::resetButton() { }
void soControllerNull::resetFlick() { }



void soTurnModuleNull::setOmitIntermediate(bool) { }
unsigned int soTurnModuleNull::getEndFrame() { return 0; }
float soTurnModuleNull::getRYLr() { return 0.0f; }
float soTurnModuleNull::getRY() { return 0.0f; }
bool soTurnModuleNull::isExtern() { return false; }
bool soTurnModuleNull::isTurn() { return false; }
void soTurnModuleNull::update(bool) { }
void soTurnModuleNull::end() { }
void soTurnModuleNull::set(const soTurnData*, bool, bool, float) { }
void soTurnModuleNull::activate() { }
soTurnModuleNull::~soTurnModuleNull() { }

static soKineticEnergyNull s_kineticEnergyNull;

void soKineticModuleNull::addSpeedOutside(soKineticEnergy::OutsideType, Vec3f*) { }
void soKineticModuleNull::addSpeed(Vec3f*, soModuleAccesser*) { }
void soKineticModuleNull::reflectAccel(Vec3f*, soKineticEnergy::AttributeFlag) { }
void soKineticModuleNull::reflectSpeed(Vec3f*, soKineticEnergy::AttributeFlag) { }
void soKineticModuleNull::mulAccel(Vec3f*, soKineticEnergy::AttributeFlag) { }
void soKineticModuleNull::mulSpeed(Vec3f*, soKineticEnergy::AttributeFlag) { }
void soKineticModuleNull::setConsiderGroundFriction(bool, soKineticEnergy::AttributeFlag) { }
int soKineticModuleNull::getKineticType() { return -1; }
void soKineticModuleNull::changeKinetic(int, soModuleAccesser*) { }
Vec3f soKineticModuleNull::getSumRotation(soKineticEnergy::AttributeFlag) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec3f soKineticModuleNull::getSumSpeed3f(soKineticEnergy::AttributeFlag) { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec2f soKineticModuleNull::getSumSpeed(soKineticEnergy::AttributeFlag) { return Vec2f(0.0f, 0.0f); }
void soKineticModuleNull::eraseEnergyAll() { }
void soKineticModuleNull::eraseEnergy(int) { }
void soKineticModuleNull::unableEnergyAll() { }
void soKineticModuleNull::resumeEnergyAll() { }
void soKineticModuleNull::suspendEnergyAll() { }
void soKineticModuleNull::clearSpeedAll() { }
bool soKineticModuleNull::isContain(int) { return false; }
soKineticEnergy* soKineticModuleNull::getEnergy(int) { return &s_kineticEnergyNull; }
int soKineticModuleNull::addEnergy(soKineticEnergy*, int, soKineticEnergy::AttributeFlag, short) { return -1; }
void soKineticModuleNull::updateEnergy(soModuleAccesser*, soKineticEnergy::AttributeFlag) { }
void soKineticModuleNull::deactivate() { }
void soKineticModuleNull::activate() { }
int soKineticModule::getKineticType() { return -1; }

int soSwitchDecideModuleNull::decideSwitch(int, soArrayContractibleTable<const acCmdArgConv>*, soModuleAccesser*) { return 0; }
soSwitchDecideModuleNull::~soSwitchDecideModuleNull() { }

bool soGeneralTermDecideModuleNull::checkEstablish(soGeneralTerm*, soModuleAccesser*, u32*, soGeneralTermCache*) { return true; }
soGeneralTermDecideModuleNull::~soGeneralTermDecideModuleNull() { }

void soStatusModuleNull::changeStatus(int, soModuleAccesser*) { }
soStatusModuleNull::~soStatusModuleNull() { }
void soStatusModuleNull::setUniqProc(u32, soStatusUniqProcess*) { }
void soStatusModuleNull::connectStatusDataList(void*) { }
void soStatusModuleNull::unableTransitionTerm(int, int) { }
int soStatusModuleNull::getPrevStatusKind(u32) { return -1; }
u32 soStatusModuleNull::getStatusGroundCorrect(int) { return 0; }
char* soStatusModuleNull::getStatusName(int) { return "None"; }
char* soStatusModuleNull::getStatusName() { return "None"; }
void soStatusModuleNull::addRangeUniqProc(soStatusUniqProcess**, u32) { }
void soStatusModuleNull::clearTransitionTermAll(int) { }
soTransitionInfo* soStatusModuleNull::getLastStatusTransitionInfo() {
    soTransitionInfo info;
    info.m_groupId = -1;
    info.m_unitId = -1;
    info._unk08 = 0;
    return &info;
}
void soStatusModuleNull::enableTransitionTermAll(int) { }
bool soStatusModuleNull::isEnableTransitionTermGroup(int) { return false; }
void soStatusModuleNull::unableTransitionTermGroup(int) { }
void soStatusModuleNull::enableTransitionTermGroup(int) { }
bool soStatusModuleNull::checkTransition(soModuleAccesser*) { return false; }
bool soStatusModuleNull::isChanged() { return false; }
void soStatusModuleNull::startWatchChange() { }
bool soStatusModuleNull::isCollisionAttackTarget(u32*) { return false; }
bool soStatusModuleNull::isCollisionAttackOccer() { return false; }
int soStatusModuleNull::getStatusKind() { return -1; }
void soStatusModuleNull::leaveStop(soModuleAccesser*, int, bool) { }
bool soStatusModuleNull::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusModuleNull::checkAttack(soModuleAccesser*, void*, float) { }
bool soStatusModuleNull::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusModuleNull::execFixCamera(soModuleAccesser*) { }
void soStatusModuleNull::execFixPos(soModuleAccesser*) { }
void soStatusModuleNull::execFixPosCounter(soModuleAccesser*) { }
void soStatusModuleNull::execMapCorrection(soModuleAccesser*) { }
void soStatusModuleNull::execStop(soModuleAccesser*) { }
void soStatusModuleNull::execStatus(soModuleAccesser*) { }
void soStatusModuleNull::begin() { }
void soStatusModuleNull::processFixPosition(soModuleAccesser*, bool) { }
void soStatusModuleNull::changeStatusForce(int, soModuleAccesser*) { }
bool soStatusModuleNull::changeStatusRequest(int, soModuleAccesser*) { return true; }
void soStatusModuleNull::deactivate(soModuleAccesser*) { }
void soStatusModuleNull::activate(soModuleAccesser*) { }

soAnimCmdModuleNull::~soAnimCmdModuleNull() { }
int soAnimCmdModuleNull::getInterpreterNum() { return 0; }
void soAnimCmdModuleNull::setAddressPackList() { }
void soAnimCmdModuleNull::getCmdAddressPack() { }
void soAnimCmdModuleNull::deactivate(soModuleAccesser*) { }
void soAnimCmdModuleNull::activate(soModuleAccesser*) { }
float soAnimCmdModuleNull::getAnimCmdFrame() { return 0.0f; }
bool soAnimCmdModuleNull::isAnimCmdEnd() { return true; }
void soAnimCmdModuleNull::skipNextMotionGroup() { }
bool soAnimCmdModuleNull::isInterpreterAnimCmd(int) { return false; }
void soAnimCmdModuleNull::resetInterpreter(int) { }
void soAnimCmdModuleNull::resetInterpreter(const soAttributeFlag&, void*, soModuleAccesser*, int, int) { }
void soAnimCmdModuleNull::resetInterpreter(int, void*, soModuleAccesser*) { }
void soAnimCmdModuleNull::resetInterpreter(float, int, soModuleAccesser*, u32) { }
void soAnimCmdModuleNull::resetInterpreter() { }
void soAnimCmdModuleNull::interpretCmdAdjust() { }
void soAnimCmdModuleNull::interpretCmd(float, int, soModuleAccesser*) { }
void soAnimCmdModuleNull::interpretCmd(soModuleAccesser*, const soAttributeFlag&, float) { }
int soAnimCmdModuleNull::registInterpreter(const soAnimCmdControlUnit&, const soAttributeFlag&) { return -1; }
