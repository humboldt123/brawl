// BYTES: the scale of a vector is the inline paired-single form (MT_VEC3F_MUL_INLINE, mt_vector.h), as in the original.
#define MT_VEC3F_MUL_INLINE
#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <if/if_minigame_coin_shooter.h>
#include <if/if_figure_loader.h>
#include <gf/gf_application.h>
#include <gf/gf_3d_scene.h>
#include <gm/gm_common.h>
#include <if/if_reward_list_task.h>
#include <mu/mu_msg.h>
#include <std/auto_ptr.h>
#include <ty/ty_seal_listmng.h>
#include <mt/mt_matrix.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
// HYPOTHESIS: lbl_8049E560 is the list of every label that exists (AutoList<IfCoinShooterLabelImpl>); it is a typed
// list here so that the label constructors and destructors call utList. The original builds it with a static
// initializer (fn_801048AC, guard lbl_805A0314) that the part of the unit with the .ctors entry carries.
utList lbl_8049E560 __attribute__((aligned(8)));
unsigned char lbl_8049E570[0xc] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
IfCoinShooter* lbl_805A0310 = NULL;
unsigned char lbl_805A0314[0x1] __attribute__((aligned(4))) = { 0 };

// The label classes of this unit (RTTI "@unnamed@if_minigame_coin_shooter_cpp@::..."). Layouts from the constructors
// (fn_800FF558, fn_800FFE6C, fn_80100900), the destructors and the vtables lbl_8045CC40 (Simple), lbl_8045CBC0 (Light),
// lbl_8045CB40 (Counter); the base IfCoinShooterLabelImpl is 0x18 bytes (see the header).
template <>
inline utList& utility::AutoList<IfCoinShooterLabelImpl>::getList() {
    return lbl_8049E560;
}

inline IfCoinShooterLabelImpl::IfCoinShooterLabelImpl(IfCoinShooterLabel* owner) : unkC(owner) {
    unk10 = 0;
    unk10_3 = false;
    unk10_2 = true;
}

namespace {

// A flat model that plays its animation once (Type_0 .. Type_3 and Type_6). fn_800FF558 / fn_800FFD50.
class IfCoinShooterLabelSimple : public IfCoinShooterLabelImpl {
public:
    std::auto_ptr<MuObject> m_obj;      // 0x18 the model InfShooter000x_TopN (table lbl_8045C8C4[m_type]), draw priority = shooter->m_objPriority
    int m_type;                         // 0x1C IfCoinShooterLabel::Type

    IfCoinShooterLabelSimple(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type);
    virtual ~IfCoinShooterLabelSimple();
    virtual MuObject* model() const;    // fn_801048A4: m_obj
    virtual void detach();              // fn_80104898: the parent of m_obj's scene model is told to remove it (virtual +0xC, 0x10001)
};
static_assert(sizeof(IfCoinShooterLabelSimple) == 0x20, "Class is the wrong size!");

// The coin / seal that flies to the totals (Type_7 and Type_8), made by the destructor of a Counter. fn_800FFE6C / fn_801002F8 / fn_80100774.
// The destructor adds m_value to the coin total (Type_7) or m_value to the seal total (Type_8, with the thumbnail of m_sealId).
class IfCoinShooterLabelLight : public IfCoinShooterLabelImpl {
public:
    std::auto_ptr<MuObject> m_obj;      // 0x18 InfShooter0006_TopN
    int m_type;                         // 0x1C Type_7 or Type_8
    int m_value;                        // 0x20 coins / seals that it carries
    u32 m_sealId;                       // 0x24 the seal (Type_8; getSealDataId)
    Vec3f m_from;                       // 0x28 where it starts (global position of the Counter's model)
    Vec3f m_to;                         // 0x34 where it flies to (global position of the shooter's m_root)

    IfCoinShooterLabelLight(IfCoinShooterLabel::Type type, int value, u32 sealId, const Vec3f& from, const Vec3f& to);
    virtual ~IfCoinShooterLabelLight();
    virtual MuObject* model() const;    // fn_80104890: m_obj
    virtual void update();              // fn_80100774: curved path m_from -> m_to by the animation progress
    virtual void detach();              // fn_80104884
};
static_assert(sizeof(IfCoinShooterLabelLight) == 0x40, "Class is the wrong size!");

// A number (and for Type_5 the seal thumbnail) that is shown at a position and then turns into a Light. fn_80100900 / fn_80100FB4.
class IfCoinShooterLabelCounter : public IfCoinShooterLabelImpl {
public:
    IfCoinShooter::Number m_number;     // 0x18 the base model is InfShooter0003 / 0004 (table lbl_8045C8C4[m_type]); 2 digits, m_ascending
    int m_type;                         // 0x40 Type_4 or Type_5
    int m_value;                        // 0x44 the number (Number::set(m_value) in the ctor); the Light's value
    u32 m_sealId;                       // 0x48 the seal (Type_5)
    std::auto_ptr<MuObject> m_sealObj;  // 0x4C HYPOTHESIS: the seal thumbnail model (InfShooter0023, Type_5); freed by the dtor, not touched here

    IfCoinShooterLabelCounter(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type, int value, u32 sealId);
    virtual ~IfCoinShooterLabelCounter();
    virtual MuObject* model() const;    // fn_80101370: m_number.m_baseObj
    virtual void detach();              // fn_8010487C: on m_number.m_expand (not on the model)
};
static_assert(sizeof(IfCoinShooterLabelCounter) == 0x50, "Class is the wrong size!");

}

// TEMPORARY (part A only, to be removed at the merge): the label classes have internal linkage, so their members are
// only emitted when something uses them. IfCoinShooterLabel::create (part B) makes them in the real unit.
void fn_tmp_part_a_users(IfCoinShooterLabel* label, const Vec3f& from, const Vec3f& to) {
    new (Heaps::MenuInstance) IfCoinShooterLabelSimple(label, IfCoinShooterLabel::Type_0);
    new (Heaps::MenuInstance) IfCoinShooterLabelLight(IfCoinShooterLabel::Type_7, 1, 0, from, to);
}

// HYPOTHESIS: stand-ins: IfRewardListTask::isLoadFinish, and srReportLessMastar (a report printf, fn_80101810).
extern "C" bool fn_800FA740(IfRewardListTask* task);
extern "C" void fn_80101810(...);

namespace {

// The name of a model or of an animation: it is built by copying and adding to it (the copies are what the compiler
// makes of a 0x41 byte structure passed around; same shape as muCollectionName in mu_collection_viewer.h).
struct IfCoinShooterName {
    char m_str[0x41];

    void addNumber(int number) { sprintf(m_str + strlen(m_str), "%d", number); }
    void addSeparator() { strcat(m_str, "__"); }

    IfCoinShooterName() { }
    IfCoinShooterName(const char* str) {
        m_str[0] = '\0';
        strcat(m_str, str);
    }
};

static inline const char* getCoinMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    return data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL;
}

// The kinds of animation of a model (the animations of a label are all named <model>__<no>).
enum IfCoinShooterAnimKind {
    AnimKind_Node,
    AnimKind_Clr,
    AnimKind_TexSrt,
    AnimKind_Vis,
};

// Starts the animation of a model that is named after the model and the number of the animation: <model>__<no>.
template <int Kind>
static inline void changeCoinAnim(MuObject* object, int no) {
    IfCoinShooterName name3;
    IfCoinShooterName name2;
    IfCoinShooterName name1;
    IfCoinShooterName name0(getCoinMdlName(object));
    name1 = name0;
    name1.addSeparator();
    name2 = name1;
    name2.addNumber(no);
    name3 = name2;
    if (Kind == AnimKind_Node) {
        object->changeNodeAnimNIf(name3.m_str);
    } else if (Kind == AnimKind_Clr) {
        object->changeClrAnimNIf(name3.m_str);
    } else if (Kind == AnimKind_TexSrt) {
        object->changeTexSrtAnimNIf(name3.m_str);
    } else {
        object->changeVisAnimNIf(name3.m_str);
    }
}

// HYPOTHESIS: the model of each label Type (lbl_8045C8C4): InfShooter0000 .. 0006.
const char* sLabelModels[9] = {
    "InfShooter0000_TopN", "InfShooter0000_TopN", "InfShooter0001_TopN", "InfShooter0002_TopN", "InfShooter0003_TopN",
    "InfShooter0004_TopN", "InfShooter0005_TopN", "InfShooter0006_TopN", "InfShooter0006_TopN",
};

// Makes the model of a label on the HUD (the next draw priority is the shooter's) and hangs it on the HUD's group.
static inline MuObject* createLabelModel(IfCoinShooter* shooter, int type) {
    MuObject* obj = MuObject::create(&shooter->m_infoRes, sLabelModels[type], shooter->m_objPriority, NULL, Heaps::MenuInstance);
    if (obj != NULL) {
        shooter->m_scnGroup->Insert(shooter->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    }
    return obj;
}

}

IfCoinShooterLabelSimple::IfCoinShooterLabelSimple(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type)
    : IfCoinShooterLabelImpl(owner), m_type(type) {
    m_obj.reset(createLabelModel(lbl_805A0310, type));
    switch (type) {
    case IfCoinShooterLabel::Type_0:
    case IfCoinShooterLabel::Type_1:
        changeCoinAnim<AnimKind_Node>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_Clr>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_TexSrt>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_Vis>(m_obj.get(), 0);
        m_obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        int visibleFrame = type != IfCoinShooterLabel::Type_0;
        m_obj->setFrameVisible(visibleFrame);
        break;
    case IfCoinShooterLabel::Type_6:
        lbl_805A0310->m_unk1C4 = true;
        // fall through
    case IfCoinShooterLabel::Type_2:
    case IfCoinShooterLabel::Type_3:
        changeCoinAnim<AnimKind_Node>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_Clr>(m_obj.get(), 0);
        break;
    default:
        break;
    }
    unk10_2 = m_obj->isNodeAnimLoop();
}

// Cuts the link between a label and the shape that shows it (each one points to the other).
// BYTES: the destructor calls this with the label from the member read first; as an inline function taking the
// pointers, MWCC keeps the vtable pointer store of the destructor and reads the owner before testing the object.
static inline void breakLabelLink(IfCoinShooterLabelImpl* impl, IfCoinShooterLabel* owner) {
    if (impl && owner) {
        impl->unkC = NULL;
        owner->unk0 = NULL;
    }
}

IfCoinShooterLabelImpl::~IfCoinShooterLabelImpl() {
    breakLabelLink(this, unkC);
}

static inline void clearType6Flag(IfCoinShooterLabelSimple* label) {
    if (lbl_805A0310 != NULL && label->m_type == IfCoinShooterLabel::Type_6) {
        lbl_805A0310->m_unk1C4 = false;
    }
}

IfCoinShooterLabelSimple::~IfCoinShooterLabelSimple() {
    clearType6Flag(this);
}

IfCoinShooterLabelLight::IfCoinShooterLabelLight(IfCoinShooterLabel::Type type, int value, u32 sealId, const Vec3f& from,
                                                 const Vec3f& to)
    : IfCoinShooterLabelImpl(NULL), m_type(type), m_value(value), m_sealId(sealId), m_from(from), m_to(to) {
    m_obj.reset(createLabelModel(lbl_805A0310, type));
    switch (type) {
    case IfCoinShooterLabel::Type_7:
    case IfCoinShooterLabel::Type_8:
        changeCoinAnim<AnimKind_Node>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_Clr>(m_obj.get(), 0);
        changeCoinAnim<AnimKind_Vis>(m_obj.get(), 0);
        m_obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        int visibleFrame = 0;
        m_obj->setFrameVisible(visibleFrame);
        break;
    }
    unk10_2 = m_obj->isNodeAnimLoop();
}

// HYPOTHESIS: the record that tySealListDataManager::getSealDataId returns; only the name of the seal's thumbnail
// (+8) is read here.
struct IfCoinShooterSealData {
    u32 unk0;
    u32 unk4;
    const char* m_texName;
};

// HYPOTHESIS: the size in pixels that follows the fixed fields of a TEX0 resource (ResTexData does not declare them).
struct IfCoinShooterTexSize {
    u8 unk0[0x1C];
    u16 m_width;
    u16 m_height;
};

// The HUD takes requests only when its models, the reward task and the figure loader are loaded.
static inline bool isHudLoaded(IfCoinShooter* shooter) {
    bool loaded = false;
    if (shooter->m_unk42_6 && fn_800FA740(shooter->m_rewardTask.get()) && IfFigureLoader::getInstance()->isLoadFinish()) {
        loaded = true;
    }
    return loaded;
}

// The request to change the totals is only taken when the HUD is loaded; otherwise it is reported (the total is then
// only stored, nothing is shown).
static inline bool takesRequest(IfCoinShooter* shooter, int delta) {
    if (isHudLoaded(shooter)) {
        return true;
    }
    fn_80101810("[IfCoinShooter] Error: リクエストは無視されました setCoinCount(%d)\n", delta);
    return false;
}

// Adds a coin count to the coin total of the HUD (not below 0) and starts the animation of its base model.
static inline void addCoinsToTotal(IfCoinShooter* shooter, int delta) {
    int total = shooter->m_coinNumber.m_value + delta;
    int coins = total > 0 ? total : 0;
    if (!takesRequest(shooter, delta)) {
        shooter->m_coinNumber.m_value = coins;
        return;
    }
    changeCoinAnim<AnimKind_Node>(shooter->m_coinNumber.m_baseObj.get(), 1);
    changeCoinAnim<AnimKind_Clr>(shooter->m_coinNumber.m_baseObj.get(), 1);
    shooter->m_coinNumber.set(coins);
}

// Puts the thumbnail model of a seal into the seal total of the HUD, textured with the thumbnail of the seal, and adds
// the count of seals to the total.
static inline void addSealToTotal(IfCoinShooter* shooter, u32 sealId, int count) {
    shooter->m_sealNumber.m_expand->PushBack(shooter->m_unk31C->m_sceneModel, "pos02");
    MuObject* seal = shooter->m_unk31C.get();
    IfCoinShooterSealData* data = reinterpret_cast<IfCoinShooterSealData*>(shooter->m_sealMgr->getSealDataId(sealId));
    if (data != NULL) {
        char texName[0x3C];
        strcpy(texName, data->m_texName);
        texName[strlen(texName) - 6] = '\0';
        nw4r::g3d::ResTex tex = shooter->m_sealRes.GetResTex(texName);
        const IfCoinShooterTexSize* texSize = reinterpret_cast<const IfCoinShooterTexSize*>(&tex.ref());
        Vec3f scale = Vec3f(texSize->m_width, texSize->m_height, 10.0f) * 0.1f;
        seal->setObjScale(&scale);
        seal->changeMaterialTex(seal->m_resMdl.GetResMat("Seal_Sammy_Mat")->m_id, texName, &shooter->m_sealRes);
    }
    shooter->m_sealNumber.set(shooter->m_sealNumber.m_value + count);
}

IfCoinShooterLabelLight::~IfCoinShooterLabelLight() {
    IfCoinShooter* shooter = lbl_805A0310;
    if (shooter != NULL) {
        if (m_type == IfCoinShooterLabel::Type_7) {
            addCoinsToTotal(shooter, m_value);
        } else {
            addSealToTotal(shooter, m_sealId, m_value);
        }
    }
}

// HYPOTHESIS: the coin / seal flies from m_from to m_to along the node animation of its model: the first part swings the
// start point around m_to (a curve of up to 7/4 pi, mirrored when the target is to the right), the second part slides
// from the swung point to m_to; both are driven by the progress of the animation (frame / length).
void IfCoinShooterLabelLight::update() {
    MuObject* obj = m_obj.get();
    float frame = obj->m_modelAnim->m_anmObjChrRes->GetFrame();
    float progress = frame / (float)obj->getNodeAnimLength();
    float half = progress * 0.5f;
    float swing = half / (half + (1.0f - progress));
    if (m_from.m_x < m_to.m_x) {
        swing = -swing;
    }
    Matrix rot(true);
    rot.setRotateZ(5.4977875f * swing);
    Vec3f offset = m_from - m_to;
    Vec3f swung;
    rot.mulVec(&offset, &swung);
    Vec3f start = swung + m_to;
    float fast = progress * 12.0f;
    float slide = fast / (fast + (1.0f - progress));
    Vec3f position;
    position = m_to * slide + start * (1.0f - slide);
    obj->setTrans(&position);
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" void fn_80101810(...) {}


// HYPOTHESIS: draft of the constructor to pin the layout. The member initializers are in declaration order, as the
// stores of fn at 0x80101C0C are.
IfCoinShooter::IfCoinShooter()
    : utility::StandardMenu("IfCoinShooter", 0xF0, Heaps::MenuResource), m_decisionPad(gmGetMenuDecisionPad()),
      m_started(false), m_clearMsgTimer(0), m_objPriority(0), m_bonusTarget(0), m_unk1C4(false), m_figureTimer(-1),
      m_figEntry(NULL) {
    g_gfApplication->m_114.add(this);
    unk2C_b1 = false;
    g_gfSceneRoot->setClearDepthBufferState(gfSceneRoot::Layer_Info, true);
    m_cameraPath[0] = '\0';
    strcat(m_cameraPath, "/menu/defaultcamera/FigureList");
    // TODO: the original ends with the fog reset of the menu scene:
    //   for (int i = 0;; i++) { nw4r::g3d::Fog fog = g_gfSceneRoot->unk44->GetFog(i); if (!fog.IsValid()) break; fog.Init(); }
    // include/nw4r/g3d/g3d_scnroot.h (local shadow) has no ScnRoot::GetFog (GetFog__Q34nw4r3g3d7ScnRootFi, 0x801AC5F4).
}

// HYPOTHESIS: draft of the destructor to pin the member order. The members are destroyed by the compiler in reverse
// order (the null tests of their addresses in the original are the inlined member destructors).
IfCoinShooter::~IfCoinShooter() {
    g_gfApplication->m_114.remove(this);
    m_msg->detachMuObject(m_root.get());
    m_rewardTask->detachDialogMsg();
    // TODO: the original deletes every label that is still on the list lbl_8049E560 here:
    //   while (list.m_length != 0) { if (list.m_head != NULL) delete head; }   (the list is the unit's .bss, see sinit)
    g_gfSceneRoot->setClearDepthBufferState(gfSceneRoot::Layer_Info, false);
}

void IfCoinShooter::destroy() {
    if (lbl_805A0310) {
        lbl_805A0310->exit();
    }
    lbl_805A0310 = NULL;
}

void IfCoinShooter::create() {
    lbl_805A0310 = new (Heaps::MenuInstance) IfCoinShooter;
}

void IfCoinShooterLabel::setPos(const Vec3f* position) {
    if (unk0) {
        Vec3f converted = Vec3f::IfCommon_exchangeStageObjPosToInfoPos(position, 0);
        unk0->model()->setTrans(&converted);
    }
}

void IfCoinShooterLabel::reset() {
    IfCoinShooterLabelImpl* impl = unk0;
    if (impl && impl->unk10_2) {
        impl->detach();
        impl->unk10 = 2;
    }
    if (unk0 && this) {
        unk0->unkC = NULL;
        unk0 = NULL;
    }
}

IfCoinShooterLabel::~IfCoinShooterLabel() {
    reset();
}



void IfCoinShooter::start() {
    m_started = true;
}

void IfCoinShooterLabelImpl::update() {
}

IfCoinShooterLabel::IfCoinShooterLabel() {
    unk0 = NULL;
}

