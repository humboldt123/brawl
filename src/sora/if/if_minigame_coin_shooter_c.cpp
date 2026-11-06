#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_common.h>
#include <if/if_figure_loader.h>
#include <if/if_minigame_coin_shooter.h>
#include <if/if_reward_list_task.h>
#include <mu/mu_msg.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
#include <std/auto_ptr.h>
#include <ty/ty_fig_listmng.h>
#include <ty/ty_seal_listmng.h>

// Part C of if_minigame_coin_shooter (0x80102808 .. 0x801048EC): createModel, update and the per-frame work of the HUD,
// the coin counter, the bonus gauge, the trophy, and the tail of the unit (stubs, static initializer, thunks).

// HYPOTHESIS: data and helpers that part A owns (.bss / .sbss of the unit) and stand-ins for the callees of other units.
extern IfCoinShooter* lbl_805A0310;                        // the HUD instance
extern "C" void fn_80101810(const char* format, ...);      // srReportLessMastar
extern "C" bool fn_800FA740(IfRewardListTask* task);       // IfRewardListTask::isLoadFinish
extern "C" void fn_800FA6CC(IfRewardListTask* task);       // IfRewardListTask::show
// IfRewardListTask::add: an entry for the notice (the id, how many, two work buffers inside the task and their size, the title)
extern "C" void fn_800FA7B8(IfRewardListTask* task, u32 id, int count, char* buf1, int size, char* buf2, const char* title);

// The label classes of this unit (the same definitions as in part A: RTTI "@unnamed@if_minigame_coin_shooter_cpp@::...").
namespace {

class IfCoinShooterLabelSimple : public IfCoinShooterLabelImpl {
public:
    std::auto_ptr<MuObject> m_obj;      // 0x18
    int m_type;                         // 0x1C IfCoinShooterLabel::Type

    IfCoinShooterLabelSimple(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type);
    virtual ~IfCoinShooterLabelSimple();
    virtual MuObject* model() const { return m_obj.get(); }
    virtual void detach();
};
static_assert(sizeof(IfCoinShooterLabelSimple) == 0x20, "Class is the wrong size!");

class IfCoinShooterLabelLight : public IfCoinShooterLabelImpl {
public:
    std::auto_ptr<MuObject> m_obj;      // 0x18
    int m_type;                         // 0x1C
    int m_value;                        // 0x20
    u32 m_sealId;                       // 0x24
    Vec3f m_from;                       // 0x28
    Vec3f m_to;                         // 0x34

    IfCoinShooterLabelLight(IfCoinShooterLabel::Type type, int value, u32 sealId, const Vec3f& from, const Vec3f& to);
    virtual ~IfCoinShooterLabelLight();
    virtual MuObject* model() const { return m_obj.get(); }
    virtual void update();
    virtual void detach();
};
static_assert(sizeof(IfCoinShooterLabelLight) == 0x40, "Class is the wrong size!");

class IfCoinShooterLabelCounter : public IfCoinShooterLabelImpl {
public:
    IfCoinShooter::Number m_number;     // 0x18
    int m_type;                         // 0x40
    int m_value;                        // 0x44
    u32 m_sealId;                       // 0x48
    std::auto_ptr<MuObject> m_sealObj;  // 0x4C

    IfCoinShooterLabelCounter(IfCoinShooterLabel* owner, IfCoinShooterLabel::Type type, int value, u32 sealId);
    virtual ~IfCoinShooterLabelCounter();
    virtual MuObject* model() const { return m_number.m_baseObj.get(); }
    virtual void detach();
};
static_assert(sizeof(IfCoinShooterLabelCounter) == 0x50, "Class is the wrong size!");

}

// A name that is put together from a text and numbers; each step works on a copy (the same helper as mu_vc_trial.cpp).
// HYPOTHESIS: the sizes (33 and 65 letters with the end) follow from how the compiler copies the names around.
template <int N>
struct IfCoinShooterName {
    char m_str[N];

    IfCoinShooterName(const char* str) {
        m_str[0] = '\0';
        strcat(m_str, str);
    }
    // The name with the separator before the number of an animation.
    IfCoinShooterName separated() const {
        IfCoinShooterName name = *this;
        strcat(name.m_str, "__");
        return name;
    }
    // The name with a number appended.
    friend IfCoinShooterName operator+(IfCoinShooterName name, int no) {
        sprintf(name.m_str + strlen(name.m_str), "%d", no);
        return name;
    }
};

// The name of the model of an object (from the resource of the model), which the names of its animations start with.
static inline const char* getMdlName(MuObject* object) {
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    return data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL;
}

// Plays an animation of an object by its number: the name is the name of the model, the separator and the number.
static inline void changeNodeAnim(MuObject* object, int no) {
    object->changeNodeAnimNIf((IfCoinShooterName<65>(getMdlName(object)).separated() + no).m_str);
}
static inline void changeClrAnim(MuObject* object, int no) {
    object->changeClrAnimNIf((IfCoinShooterName<65>(getMdlName(object)).separated() + no).m_str);
}

// HYPOTHESIS: the list of the label objects that exist (the static of AutoList<IfCoinShooterLabelImpl>, built by the static
// initializer of the unit; the unit's .bss word at 0x8049E560 is owned by part A).
struct IfCoinShooterLabelList : utList {
    IfCoinShooterLabelList() { clear(); }
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
    IfRewardListTask* task = m_rewardTask.get();
    if (!task || task->unk100 || !task->unk100_6) {
        return false;
    }
    return IfFigureLoader::getInstance()->waitForIdle();
}

// HYPOTHESIS: format of the message that the request functions report while the HUD is not loaded.
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
    if (!lbl_805A0310->isLoadFinish()) {
        return;
    }
    int gauge = 100;
    int clipped = value > 0 ? value : 0;
    if (clipped < 100) {
        gauge = clipped;
    }
    if (gauge == m_bonusGauge.m_value) {
        return;
    }
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

// HYPOTHESIS: shows the trophy that the player got: the figure counter goes up, the trophy model (made from the resource
// of the loader entry) stands on the Stand model under the group of the figure counter, and the frame plays its animation.
void IfCoinShooter::showFigureCore() {
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_figureNumber.m_expand.get());
    changeNodeAnim(m_figureNumber.m_baseObj.get(), 0);
    m_figureNumber.set(m_figureNumber.m_value + 1);

    m_figureView.m_expand.reset(NULL);
    m_figureView.m_model.reset(NULL);

    nw4r::g3d::ResFile figureRes = m_figEntry.m_entry->m_resFile;
    m_figureView.m_model.reset(MuObject::create(&figureRes, m_objPriority + 1, NULL, 0, Heaps::MenuInstance));
    m_figureView.m_modelScaler.reset(new (Heaps::MenuInstance) utility::DepthScaler(*m_figureView.m_model.get(), 0.5f, 0.5f));
    m_figureView.m_expand.reset(nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, 1,
                                                                   m_figureView.m_model->m_sceneModel));
    m_rootExpand->PushBack(m_figureView.m_expand.get(), "pos02");
    m_figureView.m_stand->m_sceneModel->DetachFromParent();
    m_figureView.m_expand->PushBack(m_figureView.m_stand->m_sceneModel, "pos00");
    IfFigureLoader::getInstance()->adjustModel(m_figEntry.m_entry->m_figId, m_figureView.m_model.get(), m_figureView.m_stand.get());
    changeNodeAnim(m_root.get(), 0);
}

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
