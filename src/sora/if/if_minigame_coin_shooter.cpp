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
#include <nw4r/g3d/g3d_scnroot.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
#include <ty/ty_fig_listmng.h>

// The weak copy of utList::init that the static initializer of the unit calls (as in em_weapon_manager.cpp).
inline void utList::init() {
    m_tail = NULL;
    m_head = NULL;
    m_length = 0;
}

// HYPOTHESIS: the list of the label objects that exist (the static of AutoList<IfCoinShooterLabelImpl>, built by the static
// initializer of the unit; the unit's .bss word at 0x8049E560 is owned by part A).
struct IfCoinShooterLabelList : utList {
    IfCoinShooterLabelList() { init(); }
};
template <class T>
struct IfCoinShooterListStorage {
    static IfCoinShooterLabelList s_list;
};
template <class T>
IfCoinShooterLabelList IfCoinShooterListStorage<T>::s_list;

template <>
inline utList& utility::AutoList<IfCoinShooterLabelImpl>::getList() {
    return IfCoinShooterListStorage<IfCoinShooterLabelImpl>::s_list;
}

// BYTES: the scale of a vector is the inline paired-single form (MT_VEC3F_MUL_INLINE, mt_vector.h), as in the original.

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049E570[0xc] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
IfCoinShooter* lbl_805A0310 = NULL;
unsigned char lbl_805A0314[0x1] __attribute__((aligned(4))) = { 0 };

// The label classes of this unit (RTTI "@unnamed@if_minigame_coin_shooter_cpp@::..."). Layouts from the constructors
// (fn_800FF558, fn_800FFE6C, fn_80100900), the destructors and the vtables lbl_8045CC40 (Simple), lbl_8045CBC0 (Light),
// lbl_8045CB40 (Counter); the base IfCoinShooterLabelImpl is 0x18 bytes (see the header).
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
    virtual MuObject* model() const { return m_obj.get(); } // fn_801048A4
    virtual void detach() { m_obj->m_sceneModel->DetachFromParent(); } // fn_80104898
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
    virtual MuObject* model() const { return m_obj.get(); } // fn_80104890
    virtual void update();              // fn_80100774: curved path m_from -> m_to by the animation progress
    virtual void detach() { m_obj->m_sceneModel->DetachFromParent(); } // fn_80104884
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
    virtual MuObject* model() const { return m_number.m_baseObj.get(); } // fn_80101370
    virtual void detach() { m_number.m_expand->DetachFromParent(); } // fn_8010487C: on the expand group (not on the model)
};
static_assert(sizeof(IfCoinShooterLabelCounter) == 0x50, "Class is the wrong size!");

}

// HYPOTHESIS: stand-ins: IfRewardListTask::isLoadFinish, and srReportLessMastar (a report printf, fn_80101810).
extern "C" bool fn_800FA740(IfRewardListTask* task);
extern "C" void fn_80101810(...);
extern "C" void fn_800FA7B8(IfRewardListTask* task, u32 id, int count, char* buf1, int size, char* buf2, const char* title);

namespace {

// The name of a model or of an animation: it is built by copying and adding to it (the copies are what the compiler
// makes of a 0x41 byte structure passed around; same shape as muCollectionName in mu_collection_viewer.h).
struct IfCoinShooterNameA {
    char m_str[0x41];

    void addNumber(int number) { sprintf(m_str + strlen(m_str), "%d", number); }
    void addSeparator() { strcat(m_str, "__"); }

    IfCoinShooterNameA() { }
    IfCoinShooterNameA(const char* str) {
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
    IfCoinShooterNameA name3;
    IfCoinShooterNameA name2;
    IfCoinShooterNameA name1;
    IfCoinShooterNameA name0(getCoinMdlName(object));
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
    position = start * (1.0f - slide) + m_to * slide;
    obj->setTrans(&position);
}



// HYPOTHESIS: draft of the constructor to pin the layout. The member initializers are in declaration order, as the
// stores of fn at 0x80101C0C are.
// ---- 0x80100900-0x80102808 ----

// BYTES: the scale of the seal thumbnail is the inline paired-single Vec3f::operator* (opt-in macro of mt_vector.h).

// PART B of the temporarily split unit (0x80100900 .. 0x80102808). The data of the unit is defined by part A (the .bss
// list and the .sbss instance, the strings and the two ints of .sdata); the label classes are declared exactly as there.
extern IfCoinShooter* lbl_805A0310;
// HYPOTHESIS: the two .sdata ints (3 and 4) that name the node of a digit / the gauge ("pos0" + number): they are
// read from memory, so they are variables of the unit and not constants.
extern int lbl_8059E558;

// The label classes of this unit (RTTI "@unnamed@if_minigame_coin_shooter_cpp@::..."); see part A for the notes.
namespace {


// The name of an animation of a label model: the model's name, the separator and the number of the animation (the
// copies are the by-value passes of the name). Same shape as getAnimName in mu_vc_trial.cpp.
template <int N>
struct IfCoinShooterName {
    char m_str[N];

    IfCoinShooterName(const char* str) {
        m_str[0] = '\0';
        strcat(m_str, str);
    }
    IfCoinShooterName separated() const {
        IfCoinShooterName name = *this;
        strcat(name.m_str, "__");
        return name;
    }
    friend IfCoinShooterName operator+(IfCoinShooterName name, int no) {
        sprintf(name.m_str + strlen(name.m_str), "%d", no);
        return name;
    }
};

static inline const char* getMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    return data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL;
}

static inline IfCoinShooterName<65> getAnimName(MuObject* object, int no) {
    IfCoinShooterName<65> name(getMdlName(object));
    return name.separated() + no;
}

}

// HYPOTHESIS: stand-ins of main.dol functions of the reward list task (IfRewardListTask, the pause menu with the
// trophies / stickers; the same functions that muAdvResultTask calls): isLoadFinish() and add(id, count, ...).
extern "C" bool fn_800FA740(IfRewardListTask* task);
// srReportLessMastar(fmt, ...): the error report of the minigame (a no-op in the retail build).
extern "C" void fn_80101810(...);

// A Counter shows a number on the model of its Type (digits InfShooter0007 on the nodes of an expand group). The group goes
// to the end of the HUD's group (drawn last). Type_4 / Type_5 play the node and texture-SRT animations of the model.
IfCoinShooterLabelCounter::IfCoinShooterLabelCounter(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type, int value, u32 sealId)
    : IfCoinShooterLabelImpl(owner), m_type(type), m_value(value), m_sealId(sealId) {
    IfCoinShooter* shooter = lbl_805A0310;
    const char* modelName = sLabelModels[type];
    int priority = shooter->m_objPriority + 1;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    m_number.m_digits = 2;
    m_number.m_ascending = true;
    m_number.m_baseObj.reset(MuObject::create(&shooter->m_infoRes, modelName, priority, NULL, Heaps::MenuInstance));
    m_number.m_expand.reset(nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, 2, m_number.m_baseObj->m_sceneModel));
    for (int i = 0; i < 2; i++) {
        m_number.m_digitObjs[i].reset(MuObject::create(&shooter->m_infoRes, "InfShooter0007_TopN", priority + 0xA, NULL, Heaps::MenuInstance));
        MuObject* digit = m_number.m_digitObjs[i].get();
        digit->changeTexPatAnimNIf(getAnimName(digit, 0).m_str);
        m_number.m_digitObjs[i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    }
    shooter->m_scnGroup->Insert(shooter->m_scnGroup->sceneItemsCount, m_number.m_expand.get());
    m_number.set(value);
    for (int i = 0; i < m_number.m_digits; i++) {
        MuObject* digit = m_number.m_digitObjs[i].get();
        digit->changeVisAnimNIf(getAnimName(digit, 0).m_str);
        m_number.m_digitObjs[i]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        m_number.m_digitObjs[i]->setFrameVisible(type == IfCoinShooterLabel::Type_5);
    }
    switch (type) {
    case IfCoinShooterLabel::Type_4:
    case IfCoinShooterLabel::Type_5: {
        MuObject* base = m_number.m_baseObj.get();
        base->changeNodeAnimNIf(getAnimName(base, 0).m_str);
        base = m_number.m_baseObj.get();
        base->changeTexSrtAnimNIf(getAnimName(base, 0).m_str);
        break;
    }
    default:
        break;
    }
    unk10_2 = m_number.m_baseObj->isNodeAnimLoop();
}

// When a Counter ends it hands its value over to a Light that flies from the Counter's node "pos02" to the HUD's total:
// the coin total (m_root "pos00") for Type_4, the seal total (m_root "pos01") for Type_5. HYPOTHESIS: the node roles.
IfCoinShooterLabelCounter::~IfCoinShooterLabelCounter() {
    IfCoinShooter* shooter = lbl_805A0310;
    if (shooter != NULL) {
        Vec3f from = model()->getGlobalPosition("pos02");
        if (m_type == IfCoinShooterLabel::Type_4) {
            // A number of coins: the Light adds them to the coin total.
            int value = m_value;
            Vec3f to = shooter->m_root->getGlobalPosition("pos00");
            new (Heaps::MenuInstance) IfCoinShooterLabelLight(IfCoinShooterLabel::Type_7, value, 0, from, to);
        } else {
            // A seal: the Light adds it to the seal total, which is brought to the front and shown again.
            int value;
            u32 sealId = m_sealId;
            value = m_value;
            Vec3f to = shooter->m_root->getGlobalPosition("pos01");
            new (Heaps::MenuInstance) IfCoinShooterLabelLight(IfCoinShooterLabel::Type_8, value, sealId, from, to);
            shooter->m_scnGroup->Remove(shooter->m_sealNumber.m_expand.get());
            shooter->m_scnGroup->Insert(shooter->m_scnGroup->sceneItemsCount, shooter->m_sealNumber.m_expand.get());
            MuObject* base = shooter->m_sealNumber.m_baseObj.get();
            base->changeNodeAnimNIf(getAnimName(base, 0).m_str);
            shooter->m_sealNumber.m_expand->Remove(shooter->m_unk31C->m_sceneModel);
            for (int i = 0; i < shooter->m_sealNumber.m_digits; i++) {
                shooter->m_sealNumber.m_expand->Remove(shooter->m_sealNumber.m_digitObjs[i]->m_sceneModel);
            }
        }
    }
}

IfCoinShooterLabel::IfCoinShooterLabel() {
    unk0 = NULL;
}

IfCoinShooterLabel::~IfCoinShooterLabel() {
    reset();
}

// HYPOTHESIS: the view of the header of a TEX0 resource that holds the size of the picture.
// The check of IfCoinShooterLabel::create: without a loaded HUD the request is reported (and ignored).
static inline bool checkHudLoaded(IfCoinShooterLabel::Type type, int value, u32 sealId) {
    if (isHudLoaded(lbl_805A0310)) {
        return true;
    }
    // "[IfCoinShooter] Error: the request was ignored IfCoinShooterLabel::create(%d,%d,%d)\n"
    fn_80101810("[IfCoinShooter] Error: "
                "\x83\x8A\x83\x4E\x83\x47\x83\x58\x83\x67\x82\xCD\x96\xB3\x8E\x8B\x82\xB3\x82\xEA\x82\xDC\x82\xB5\x82\xBD"
                " IfCoinShooterLabel::create(%d,%d,%d)\n",
                type, value, sealId);
    return false;
}

// Makes the label of a given kind (see Type) at the HUD. Without a loaded HUD the request is reported and ignored.
void IfCoinShooterLabel::create(Type type, int value, u32 sealId) {
    if (!checkHudLoaded(type, value, sealId)) {
        return;
    }
    reset();
    switch (type) {
    case Type_0:
    case Type_1:
    case Type_2:
    case Type_3:
    case Type_6:
        unk0 = new (Heaps::MenuInstance) IfCoinShooterLabelSimple(this, type);
        break;
    case Type_5: {
        char texName[0x40];
        // A number and a seal: the reward list gets the seal (its "stickers" list), the Counter shows the seal's picture.
        IfRewardListTask* task = lbl_805A0310->m_rewardTask.get();
        fn_800FA7B8(task, sealId, value, reinterpret_cast<char*>(task) + 0x13A6, 0x400, reinterpret_cast<char*>(task) + 0x23A6,
                    "\x83\x56\x81\x5B\x83\x8B");
        IfCoinShooterLabelCounter* counter = new (Heaps::MenuInstance) IfCoinShooterLabelCounter(this, type, value, sealId);
        IfCoinShooter* shooter = lbl_805A0310;
        MuObject* seal = MuObject::create(&shooter->m_infoRes, "InfShooter0023_TopN", shooter->m_objPriority + 2, NULL, Heaps::MenuInstance);
        void* sealData = shooter->m_sealMgr->getSealDataId(sealId);
        if (sealData != NULL) {
            strcpy(texName, *reinterpret_cast<const char**>(reinterpret_cast<char*>(sealData) + 8));
            texName[strlen(texName) - 6] = '\0';
            const IfCoinShooterTexSize* tex = reinterpret_cast<const IfCoinShooterTexSize*>(shooter->m_sealRes.GetResTex(texName).ptr());
            Vec3f scale = Vec3f(tex->m_width, tex->m_height, 10.0f) * 0.1f;
            seal->setObjScale(&scale);
            seal->changeMaterialTex(seal->m_resMdl.GetResMat("Seal_Sammy_Mat")->m_id, texName, &shooter->m_sealRes);
        }
        counter->m_sealObj.reset(seal);
        IfCoinShooterName<33> pos("pos0");
        counter->m_number.m_expand->PushBack(counter->m_sealObj->m_sceneModel, (pos + lbl_8059E558).m_str);
        unk0 = counter;
        break;
    }
    case Type_4:
        unk0 = new (Heaps::MenuInstance) IfCoinShooterLabelCounter(this, type, value, sealId);
        break;
    default:
        break;
    }
}

extern "C" void fn_80101810(...) {}

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

void IfCoinShooterLabel::setPos(const Vec3f* position) {
    if (unk0) {
        Vec3f converted = Vec3f::IfCommon_exchangeStageObjPosToInfoPos(position, 0);
        unk0->model()->setTrans(&converted);
    }
}

// Shows the number: clips it to 0 .. 10^m_digits - 1, puts the digit models that are needed on the group and takes the
// others off it; the digit goes to the texture frame of its model.
void IfCoinShooter::Number::set(int value) {
    IfCoinShooterName<33> pos("pos0");
    m_value = value;
    int limit = 1;
    u32 e = m_digits;
    int base = 10;
    for (; e != 0; e >>= 1) {
        if (e & 1) {
            limit *= base;
        }
        base *= base;
    }
    int top = limit - 1;
    int shown = value > 0 ? value : 0;
    if (shown < top) {
        top = shown;
    }
    value = top;
    std::auto_ptr<MuObject>* digit = m_digitObjs;
    int i = 0;
    int node;
    for (; i < m_digits; digit++, i++) {
        if ((*digit)->m_sceneModel->m_parent == NULL) {
            node = m_ascending ? i : m_digits - 1 - i;
            m_expand->PushBack((*digit)->m_sceneModel, (pos + node).m_str);
        }
        (*digit)->setFrameTex(value % 10);
        value /= 10;
        if (value <= 0) {
            i++;
            break;
        }
    }
    for (; i < m_digits; i++) {
        m_expand->Remove(m_digitObjs[i]->m_sceneModel);
    }
}

void IfCoinShooter::create() {
    lbl_805A0310 = new (Heaps::MenuInstance) IfCoinShooter;
}

void IfCoinShooter::destroy() {
    if (lbl_805A0310) {
        lbl_805A0310->exit();
    }
    lbl_805A0310 = NULL;
}

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
    // HYPOTHESIS: unk44 is the root of the menu (Info layer) scene; its fogs are reset so that no stage fog stays.
    nw4r::g3d::ScnRoot* scnRoot = g_gfSceneRoot->unk44;
    for (int i = 0;; i++) {
        nw4r::g3d::Fog fog = scnRoot->GetFog(i);
        if (!fog.IsValid()) {
            break;
        }
        fog.Init();
    }
}

// HYPOTHESIS: draft of the destructor to pin the member order.
IfCoinShooter::~IfCoinShooter() {
    g_gfApplication->m_114.remove(this);
    m_msg->detachMuObject(m_root.get());
    m_rewardTask->detachDialogMsg();
    // Every label that is still on the list is deleted (each one takes itself off the list).
    utList& labels = utility::AutoList<IfCoinShooterLabelImpl>::getList();
    while (labels.getLength() != 0) {
        // BYTES: the list node is the base of IfCoinShooterLabelImpl at offset 0 (the downcast would add a null test).
        IfCoinShooterLabelImpl* label = reinterpret_cast<IfCoinShooterLabelImpl*>(labels.getHead());
        delete label;
    }
    g_gfSceneRoot->setClearDepthBufferState(gfSceneRoot::Layer_Info, false);
}

void IfCoinShooter::loadResource() {
    m_infoRes.request("/info/info_coin_shooter.brres", Heaps::MenuResource, 0);
    m_standRes.request("/toy/fig/Stand.brres", Heaps::MenuResource, 0);
    m_sealRes.request("/toy/seal/ToySealThumb.brres", Heaps::MenuResource, 0);
    m_msgFile.request("/menu/msg/mini_coin_shooter.msbin", Heaps::MenuResource, 0);
    m_sealMgr.reset(new (Heaps::MenuInstance) tySealListDataManager(true));
}

// ---- 0x80102808-0x801048EC ----

// Part C of if_minigame_coin_shooter (0x80102808 .. 0x801048EC): createModel, update and the per-frame work of the HUD,
// the coin counter, the bonus gauge, the trophy, and the tail of the unit (stubs, static initializer, thunks).

// HYPOTHESIS: data and helpers that part A owns (.bss / .sbss of the unit) and stand-ins for the callees of other units.
extern IfCoinShooter* lbl_805A0310;                        // the HUD instance
extern "C" bool fn_800FA740(IfRewardListTask* task);       // IfRewardListTask::isLoadFinish
extern "C" void fn_800FA6CC(IfRewardListTask* task);       // IfRewardListTask::show
// IfRewardListTask::create: the parameters of the notice (see IfCoinShooterRewardParam) and the heaps of the task and its files
struct IfCoinShooterRewardParam {
    nw4r::g3d::ResFile m_infoRes;       // 0x00 the HUD resource (info_coin_shooter)
    nw4r::g3d::ResFile m_sealRes;       // 0x04 the seal thumbnails
    const char* m_name;                 // 0x08 the name of the notice model
    int m_priority;                     // 0x0C the draw priority of the notice
    tyFigListDataManager* m_figMgr;     // 0x10 the trophy list (the one of the figure loader)
    tySealListDataManager* m_sealMgr;   // 0x14
    void* m_msgBinary;                  // 0x18 the message file
    int m_unk1C;                        // 0x1C
    u8 m_decisionPad;                   // 0x20 the pad that pauses
    u8 m_unk21[0xF];
};
static_assert(sizeof(IfCoinShooterRewardParam) == 0x30, "Class is the wrong size!");
extern "C" IfRewardListTask* fn_800F7CCC(IfCoinShooterRewardParam* param, Heaps::HeapType heap, Heaps::HeapType resHeap);
// ScnGroup::Construct: a group of scene objects for at most `capacity` children
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, int capacity);
// IfRewardListTask::add: an entry for the notice (the id, how many, two work buffers inside the task and their size, the title)

// Plays an animation of an object by its number: the name is the name of the model, the separator and the number.
static inline void changeNodeAnim(MuObject* object, int no) {
    object->changeNodeAnimNIf((IfCoinShooterName<65>(getMdlName(object)).separated() + no).m_str);
}
static inline void changeVisAnim(MuObject* object, int no) {
    object->changeVisAnimNIf((IfCoinShooterName<65>(getMdlName(object)).separated() + no).m_str);
}
static inline void changeClrAnim(MuObject* object, int no) {
    object->changeClrAnimNIf((IfCoinShooterName<65>(getMdlName(object)).separated() + no).m_str);
}

// A group of scene objects on the nodes of a model (HYPOTHESIS: the slots are the capacity of the group).
static inline nw4r::g3d::ScnMdlExpand* createExpand(MuObject* object, int slots) {
    return nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, slots,
                                              object->getSceneModel());
}

// HYPOTHESIS: the node of the bonus gauge on the coin counter (an .sdata word of the unit that is read, not folded).
static int sBonusGaugeNode = 4;

// One of the counters of the HUD (the seal / figure / coin totals): the model that carries the digits, a group on it, one
// digit model for every place, and for the totals that are shown at first the scaler of the base and the first frame of the
// digits. HYPOTHESIS: the names and the parameters; the order of the work follows the original.
static inline void createNumber(IfCoinShooter* hud, IfCoinShooter::Number& number, bool ascending, int digits, const char* baseName,
                                const char* digitName, bool scaled, float visFrame) {
    int priority = hud->m_objPriority++;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    number.m_digits = digits;
    number.m_ascending = ascending;
    number.m_baseObj.reset(MuObject::create(&hud->m_infoRes, baseName, priority, NULL, Heaps::MenuInstance));
    number.m_expand.reset(nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, digits + 1, number.m_baseObj->m_sceneModel));
    for (int i = 0; i < digits; i++) {
        number.m_digitObjs[i].reset(MuObject::create(&hud->m_infoRes, digitName, priority + 10, NULL, Heaps::MenuInstance));
        MuObject* digit = number.m_digitObjs[i].get();
        digit->changeTexPatAnimNIf((IfCoinShooterName<65>(getMdlName(digit)).separated() + 0).m_str);
        number.m_digitObjs[i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    }
    if (scaled) {
        number.m_scaler.reset(new (Heaps::MenuInstance) utility::DepthScaler(*number.m_baseObj.get(), 0.01f, 0.99f));
    }
    hud->m_scnGroup->Insert(hud->m_scnGroup->sceneItemsCount, number.m_expand.get());
    if (scaled) {
        for (int i = 0; i < number.m_digits; i++) {
            changeVisAnim(number.m_digitObjs[i].get(), 0);
            number.m_digitObjs[i]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            number.m_digitObjs[i]->setFrameVisible(visFrame);
        }
    }
}

// HYPOTHESIS: a position in the label list; the end of the list is the null position.
struct IfCoinShooterLabelIter {
    IfCoinShooterLabelImpl* m_node;

    IfCoinShooterLabelIter(IfCoinShooterLabelImpl* node) : m_node(node) { }
    static IfCoinShooterLabelIter begin() {
        return IfCoinShooterLabelIter(static_cast<IfCoinShooterLabelImpl*>(utility::AutoList<IfCoinShooterLabelImpl>::getList().getHead()));
    }
    static IfCoinShooterLabelIter end() { return IfCoinShooterLabelIter(NULL); }
    bool operator!=(const IfCoinShooterLabelIter& other) const { return m_node != other.m_node; }
    bool operator==(const IfCoinShooterLabelIter& other) const { return m_node == other.m_node; }
    IfCoinShooterLabelImpl* operator->() const { return m_node; }
    IfCoinShooterLabelIter next() const { return IfCoinShooterLabelIter(static_cast<IfCoinShooterLabelImpl*>(m_node->m_next)); }
};

void IfCoinShooter::update() {
    if (m_started) {
        if (g_gfApplication->m_pauseState) {
            IfRewardListTask* task = m_rewardTask.get();
            if (!task->unk100) {
                if (!task->unk100_6) {
                    g_gfApplication->setPause(0, 0);
                    g_sndSystem->setBGMVol(true, 1.0f);
                } else if (isQuit()) {
                    g_gfApplication->setPause(0, 0);
                }
            }
        }
        onUpdate();
    }
}

void IfCoinShooter::onUpdate() {
    if (m_clearMsgTimer <= 0 && !m_rewardTask->unk100 && !m_rewardTask->unk100_6) {
        gfPadStatus pad;
        g_gfPadSystem->getSysPadStatus(m_decisionPad, &pad);
        u32 pressed = pad.m_buttonsPressedThisFrame.bits;
        if ((pressed & GF_PAD_BUTTON_BIT_B) || (pressed & GF_PAD_BUTTON_BIT_START)) {
            fn_800FA6CC(m_rewardTask.get());
            g_gfApplication->setPause(1, 0);
            g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
            g_sndSystem->setBGMVol(true, 0.75f);
        }
    }

    // HYPOTHESIS: a label that asks for its removal (unk10_3) is deleted; the loop then goes on with the node after the
    // next one, as the original does.
    IfCoinShooterLabelIter end = IfCoinShooterLabelIter::end();
    for (IfCoinShooterLabelIter it = IfCoinShooterLabelIter::begin(); it != end; it = it.next()) {
        it->update();
        if (it->unk10_3) {
            IfCoinShooterLabelIter next = it.next();
            delete it.m_node;
            it = next;
            if (it == IfCoinShooterLabelIter::end()) {
                break;
            }
        }
    }

    updateBonusPoint(m_unk1C4 ? 100 : m_bonusTarget);

    if (m_clearMsgTimer > 0 && --m_clearMsgTimer <= 0) {
        m_msg->printf(2, "");
    }
    if (!isQuit() && m_figureTimer == 0) {
        showFigureCore();
    }
}

void IfCoinShooterLabelImpl::update() {
}

bool IfCoinShooter::isLoadFinish() {
    bool loaded = false;
    if (m_unk42_6 && fn_800FA740(m_rewardTask.get()) && IfFigureLoader::getInstance()->isLoadFinish()) {
        loaded = true;
    }
    return loaded;
}

void IfCoinShooter::start() {
    m_started = true;
}

bool IfCoinShooter::isQuit() {
    if (!m_rewardTask.get() || m_rewardTask->unk100 || !m_rewardTask->unk100_6) {
        return false;
    }
    return IfFigureLoader::getInstance()->waitForIdle();
}

// HYPOTHESIS: adds delta to the coin total (never below 0). While the HUD is not loaded the total is only stored (and the request
// is reported); otherwise the counter is redrawn, and with animate the base model plays its animation number 1 first.
void IfCoinShooter::setCoinCount(int delta, bool animate) {
    int total = m_coinNumber.m_value + delta;
    int coins = total > 0 ? total : 0;
    bool ready;
    if (lbl_805A0310->isLoadFinish()) {
        ready = true;
    } else {
        fn_80101810("[IfCoinShooter] Error: setCoinCount(%d)\n", delta);
        ready = false;
    }
    if (!ready) {
        m_coinNumber.m_value = coins;
        return;
    }
    if (animate) {
        changeNodeAnim(m_coinNumber.m_baseObj.get(), 1);
        changeClrAnim(m_coinNumber.m_baseObj.get(), 1);
    }
    m_coinNumber.set(coins);
}

void IfCoinShooter::updateBonusPoint(int value) {
    if (lbl_805A0310->isLoadFinish()) {
        int gauge = 100;
        int clipped = value > 0 ? value : 0;
        if (clipped < 100) {
            gauge = clipped;
        }
        if (gauge != m_bonusGauge.m_value) {
            m_bonusGauge.m_obj->setFrameVisible(gauge / 10.0f);
            m_bonusGauge.m_obj->m_modelAnim->unbindMatColAnim(m_bonusGauge.m_obj->m_sceneModel);
            if (gauge > m_bonusGauge.m_value) {
                if (gauge >= 100) {
                    changeClrAnim(m_bonusGauge.m_obj.get(), 1);
                } else if (gauge / 10 > m_bonusGauge.m_value / 10) {
                    changeClrAnim(m_bonusGauge.m_obj.get(), 0);
                }
            }
            m_bonusGauge.m_value = gauge;
        }
    }
}

void IfCoinShooter::showFigure(IfFigureLoader::EntryRef* entry) {
    bool ready;
    if (lbl_805A0310->isLoadFinish()) {
        ready = true;
    } else {
        fn_80101810("[IfCoinShooter] Error: showFigure(%s)\n", entry->m_entry->m_data->name);
        ready = false;
    }
    if (ready) {
        IfRewardListTask* task = m_rewardTask.get();
        fn_800FA7B8(task, entry->m_entry->m_figId, 1, reinterpret_cast<char*>(task) + 0x3A4, 0x400,
                    reinterpret_cast<char*>(task) + 0x13A4, "\x83\x74\x83\x42\x83\x4C\x83\x85\x83\x41");
        IfFigureLoader::Entry* taken = entry->m_entry;
        entry->m_entry = NULL;
        if (m_figEntry.m_entry) {
            m_figEntry.m_entry->destroy();
        }
        m_figEntry.m_entry = taken;
        if (m_figureNumber.m_expand.get()) {
            m_figureNumber.m_expand->DetachFromParent();
        }
        if (m_figureView.m_expand.get()) {
            m_figureView.m_expand->DetachFromParent();
        }
        m_figureTimer = 2;
    }
}

void IfCoinShooter::createModel() {
    IfCoinShooterName<33> pos("pos0");
    m_objPriority = 0x10;

    m_scnGroup.reset(fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, 0x20));
    // BYTES: gfSceneRoot::add takes a ScnMdl (the symbol says so); the original passes the group to it.
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnGroup.get()));

    m_root.reset(MuObject::create(&m_infoRes, "InfShooter0010_TopN", m_objPriority, NULL, Heaps::MenuInstance));
    m_rootExpand.reset(createExpand(m_root.get(), 3));
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_rootExpand.get());

    // The seal total.
    createNumber(this, m_sealNumber, true, 2, "InfShooter0013_TopN", "InfShooter0007_TopN", true, 1.0f);
    m_unk31C.reset(MuObject::create(&m_infoRes, "InfShooter0023_TopN", m_objPriority++, NULL, Heaps::MenuInstance));
    // The figure total.
    createNumber(this, m_figureNumber, true, 2, "InfShooter0014_TopN", "InfShooter0007_TopN", true, 0.0f);
    // The coin total.
    createNumber(this, m_coinNumber, false, 4, "InfShooter0011_TopN", "InfShooter0012_TopN", false, 0.0f);
    changeNodeAnim(m_coinNumber.m_baseObj.get(), 0);
    changeClrAnim(m_coinNumber.m_baseObj.get(), 0);
    m_coinNumber.set(m_coinNumber.m_value);

    // The bonus gauge sits on the coin total.
    m_bonusGauge.m_obj.reset(MuObject::create(&m_infoRes, "InfShooter0015_TopN", m_objPriority++, NULL, Heaps::MenuInstance));
    m_coinNumber.m_expand->PushBack(m_bonusGauge.m_obj->m_sceneModel, (pos + sBonusGaugeNode).m_str);
    changeVisAnim(m_bonusGauge.m_obj.get(), 0);
    m_bonusGauge.m_obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);

    // The stand of the trophy.
    m_figureView.m_stand.reset(MuObject::create(&m_standRes, m_objPriority + 1, NULL, 0, Heaps::MenuInstance));
    m_figureView.m_stand->changeClrAnimN("DolStand");
    m_figureView.m_stand->m_modelAnim->setUpdateRate(0.0f);
    m_figureView.m_stand->setFrameMatCol(38.0f);
    m_figureView.m_standScaler.reset(new (Heaps::MenuInstance) utility::DepthScaler(*m_figureView.m_stand.get(), 0.5f, 0.5f));

    // The message of the root model.
    m_clearMsgTimer = 90;
    m_msg.reset(MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource));
    m_msg->allocMsgBuf(0x100, 3);
    m_msg->setMsgData(m_msgFile.m_handle.getBuffer());
    m_msg->attachScnMdlSimple(2, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_root->m_sceneModel), 0, 1.0f / 14.0f);
    m_msg->setAlignMode(2, MuMsg::Align_Center);
    m_msg->setEdgeEnable(2, true);
    m_msg->setEdge(2, 8, 0xFF, 0, 1.0f);
    m_msg->printIndex(2, 2, NULL);

    // The notice of the rewards (the pause menu).
    IfCoinShooterRewardParam param = { nw4r::g3d::ResFile(), nw4r::g3d::ResFile(), "InfShooter", 0, NULL, NULL, NULL, 1, 0 };
    param.m_infoRes = m_infoRes;
    param.m_sealRes = m_sealRes;
    param.m_priority = m_objPriority + 0x14;
    param.m_figMgr = IfFigureLoader::getInstance()->m_listMgr;
    param.m_sealMgr = m_sealMgr.get();
    param.m_msgBinary = m_msg->getBinary();
    param.m_decisionPad = m_decisionPad;
    m_rewardTask.reset(fn_800F7CCC(&param, Heaps::MenuResource, Heaps::MenuResource));
}

// HYPOTHESIS: shows the trophy that the player got: the figure counter goes up, the trophy model (made from the resource
// of the loader entry) stands on the Stand model under the group of the figure counter, and the frame plays its animation.
// MATCH-ONLY: auto_inline off; the original calls showFigureCore from onUpdate (the compiler would otherwise inline the single call).
#pragma auto_inline off
void IfCoinShooter::showFigureCore() {
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_figureNumber.m_expand.get());
    changeNodeAnim(m_figureNumber.m_baseObj.get(), 0);
    m_figureNumber.set(m_figureNumber.m_value + 1);

    m_figureView.m_expand.reset(NULL);
    m_figureView.m_model.reset(NULL);

    nw4r::g3d::ResFile figureRes = m_figEntry.m_entry->m_resFile;
    m_figureView.m_model.reset(MuObject::create(&figureRes, m_objPriority + 1, NULL, 0, Heaps::MenuInstance));
    m_figureView.m_modelScaler.reset(new (Heaps::MenuInstance) utility::DepthScaler(*m_figureView.m_model.get(), 0.5f, 0.5f));
    m_figureView.m_expand.reset(createExpand(m_figureView.m_model.get(), 1));
    m_rootExpand->PushBack(m_figureView.m_expand.get(), "pos02");
    m_figureView.m_stand->m_sceneModel->DetachFromParent();
    m_figureView.m_expand->PushBack(m_figureView.m_stand->m_sceneModel, "pos00");
    IfFigureLoader::getInstance()->adjustModel(m_figEntry.m_entry->m_figId, m_figureView.m_model.get(), m_figureView.m_stand.get());
    changeNodeAnim(m_root.get(), 0);
}

#pragma auto_inline reset

void IfCoinShooter::userProc() {
    if (m_figureTimer >= 0 && m_figEntry.m_entry) {
        int timer = m_figureTimer - 1;
        m_figureTimer = timer > -1 ? timer : -1;
    }
    IfCoinShooterLabelIter end = IfCoinShooterLabelIter::end();
    for (IfCoinShooterLabelIter it = IfCoinShooterLabelIter::begin(); it != end; it = it.next()) {
        if (it->unk10 > 0) {
            it->unk10--;
            if (it->unk10 <= 0) {
                it->unk10_3 = true;
            }
        } else if (it->model()->isNodeAnimFinished()) {
            it->detach();
            it->unk10 = 2;
        }
    }
}


