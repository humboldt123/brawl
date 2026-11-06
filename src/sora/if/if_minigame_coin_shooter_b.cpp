// BYTES: the scale of the seal thumbnail is the inline paired-single Vec3f::operator* (opt-in macro of mt_vector.h).
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
#include <nw4r/g3d/g3d_scnroot.h>
#include <std/auto_ptr.h>
#include <ty/ty_seal_listmng.h>

// PART B of the temporarily split unit (0x80100900 .. 0x80102808). The data of the unit is defined by part A (the .bss
// list and the .sbss instance, the strings and the two ints of .sdata); the label classes are declared exactly as there.
extern utList lbl_8049E560;
extern IfCoinShooter* lbl_805A0310;
// HYPOTHESIS: the two .sdata ints (3 and 4) that name the node of a digit / the gauge ("pos0" + number): they are
// read from memory, so they are variables of the unit and not constants.
extern int lbl_8059E558;

template <>
inline utList& utility::AutoList<IfCoinShooterLabelImpl>::getList() {
    return lbl_8049E560;
}

// The constructor of the label base (part A); the derived constructors have it inline.
inline IfCoinShooterLabelImpl::IfCoinShooterLabelImpl(IfCoinShooterLabel* owner) : unkC(owner) {
    unk10 = 0;
    unk10_3 = false;
    unk10_2 = true;
}

// The label classes of this unit (RTTI "@unnamed@if_minigame_coin_shooter_cpp@::..."); see part A for the notes.
namespace {

// A flat model that plays its animation once (Type_0 .. Type_3 and Type_6). fn_800FF558 / fn_800FFD50.
class IfCoinShooterLabelSimple : public IfCoinShooterLabelImpl {
public:
    std::auto_ptr<MuObject> m_obj;      // 0x18
    int m_type;                         // 0x1C IfCoinShooterLabel::Type

    IfCoinShooterLabelSimple(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type);
    virtual ~IfCoinShooterLabelSimple();
    virtual MuObject* model() const;
    virtual void detach();
};
static_assert(sizeof(IfCoinShooterLabelSimple) == 0x20, "Class is the wrong size!");

// The coin / seal that flies to the totals (Type_7 and Type_8). fn_800FFE6C / fn_801002F8 / fn_80100774.
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
    virtual MuObject* model() const;
    virtual void update();
    virtual void detach();
};
static_assert(sizeof(IfCoinShooterLabelLight) == 0x40, "Class is the wrong size!");

// A number (and for Type_5 the seal thumbnail) that is shown at a position and then turns into a Light. fn_80100900 / fn_80100FB4.
class IfCoinShooterLabelCounter : public IfCoinShooterLabelImpl {
public:
    IfCoinShooter::Number m_number;     // 0x18 the base model is InfShooter0003 / 0004 (table lbl_8045C8C4[m_type]); 2 digits, m_ascending
    int m_type;                         // 0x40 Type_4 or Type_5
    int m_value;                        // 0x44 the number (Number::set(m_value) in the ctor); the Light's value
    u32 m_sealId;                       // 0x48 the seal (Type_5)
    std::auto_ptr<MuObject> m_sealObj;  // 0x4C the seal thumbnail model (InfShooter0023, Type_5), set by IfCoinShooterLabel::create

    IfCoinShooterLabelCounter(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type, int value, u32 sealId);
    virtual ~IfCoinShooterLabelCounter();
    virtual MuObject* model() const { return m_number.m_baseObj.get(); }  // fn_80101370
    virtual void detach();              // fn_8010487C: on m_number.m_expand (not on the model)
};
static_assert(sizeof(IfCoinShooterLabelCounter) == 0x50, "Class is the wrong size!");

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

// HYPOTHESIS: the model of each label Type (lbl_8045C8C4): InfShooter0000 .. 0006.
const char* sLabelModels[9] = {
    "InfShooter0000_TopN", "InfShooter0000_TopN", "InfShooter0001_TopN", "InfShooter0002_TopN", "InfShooter0003_TopN",
    "InfShooter0004_TopN", "InfShooter0005_TopN", "InfShooter0006_TopN", "InfShooter0006_TopN",
};

}

// The Impl destructor of part A (fn_800FFC90): the derived destructors of the unit have it inline.
IfCoinShooterLabelImpl::~IfCoinShooterLabelImpl() {
    if (this && unkC != NULL) {
        unkC->unk0 = NULL;
        unkC = NULL;
    }
}

// HYPOTHESIS: stand-ins of main.dol functions of the reward list task (IfRewardListTask, the pause menu with the
// trophies / stickers; the same functions that muAdvResultTask calls): isLoadFinish() and add(id, count, ...).
extern "C" bool fn_800FA740(IfRewardListTask* task);
extern "C" void fn_800FA7B8(IfRewardListTask* task, u32 id, int count, void* bufA, int sizeA, void* bufB, const char* label);
// The deleting destructor of tySealListDataManager.
extern "C" void fn_800AD87C(tySealListDataManager* manager, int flag);
// srReportLessMastar(fmt, ...): the error report of the minigame (a no-op in the retail build).
extern "C" void fn_80101810(...);

// The Light is made by the destructor of a Counter.
// HYPOTHESIS: names of the nodes of m_root that the Lights fly to (the coin total, the seal total) and the node of the
// Counter that they start from.

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
    if (type >= IfCoinShooterLabel::Type_4 && type < IfCoinShooterLabel::Type_6) {
        MuObject* base = m_number.m_baseObj.get();
        base->changeNodeAnimNIf(getAnimName(base, 0).m_str);
        base = m_number.m_baseObj.get();
        base->changeTexSrtAnimNIf(getAnimName(base, 0).m_str);
    }
    unk10_2 = m_number.m_baseObj->isNodeAnimLoop();
}

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

// HYPOTHESIS: the body of IfCoinShooter::isLoadFinish (part C) on the global instance, inline here.
static inline bool isHudLoaded(IfCoinShooter* shooter) {
    bool loaded = false;
    if (shooter->m_unk42_6 && fn_800FA740(shooter->m_rewardTask.get()) && IfFigureLoader::getInstance()->isLoadFinish()) {
        loaded = true;
    }
    return loaded;
}

// HYPOTHESIS: the view of the header of a TEX0 resource that holds the size of the picture.
struct IfCoinShooterTexSize {
    u8 m_pad[0x1C];
    u16 m_width;
    u16 m_height;
};

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
