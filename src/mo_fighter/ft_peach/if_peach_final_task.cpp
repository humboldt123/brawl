#include <ft/peach/ft_peach.h>
#include <ft/ft_value_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_rot_utility.h>
#include <ft/peach/ft_peach_status_uniq_process_final.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_slow_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <it/it_manager.h>
#include <mt/mt_prng.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/so_external_value_accesser.h>
#include <so/so_slow.h>
#include <so/so_module_accesser.h>
#include <st/stage.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <snd/snd_system.h>
#include <types.h>

// Declaration-only call into the existing scene-group constructor (unnamed DOL function, as in if_marth_final.cpp).
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator*, u32*, u32);

struct PeachFinalModelData {
    const char* name;
    u8 firstIndex;
    u8 endIndex;
    u8 nodeIndex;
    u8 padding;
};

static const PeachFinalModelData s_peachFinalModelData[] = {
    { "InfWeapon0013_TopN", 0, 0, 0x80, 0 }
};

// BYTES: the original keeps these as the function-local static `actions` of setAction, but its data lands before the
// task name string "IfPeachFinal" of the constructor, which only a file-scope definition above the constructor reproduces.
// Indexed by the action: 0 intro, 1 outro, 2 intro (wide), 3 outro (wide); HYPOTHESIS: the first two floats are the frame range.
static MuAnimNameData s_peachFinalActions[4] = {
    {0.0f, 150.0f, 0.0f, "InfWeapon0013_TopN_in", 1},
    {0.0f, 60.0f, 0.0f, "InfWeapon0013_TopN_out", 1},
    {0.0f, 150.0f, 0.0f, "InfWeapon0013_TopN_in_wide", 1},
    {0.0f, 60.0f, 0.0f, "InfWeapon0013_TopN_out_wide", 1}
};

// The original inlines the scene-group helpers of IfMngr into every caller (no out-of-line copy in this module).
inline void IfMngr::addGame2DObj(nw4r::g3d::ScnObj* object) {
    nw4r::g3d::ScnGroup* group = m_game2DGroup;
    group->Insert(group->sceneItemsCount, object);
}

inline void IfMngr::removeGame2DObj(nw4r::g3d::ScnObj* object) {
    if (m_game2DGroup != NULL) m_game2DGroup->Remove(object);
}

IfPeachFinalTask* IfPeachFinalTask::create(void* resourceData, s32 heap, u32 fighterTaskId, bool initialize) {
    IfPeachFinalTask* task = new (Heaps::HeapType(heap)) IfPeachFinalTask(resourceData, fighterTaskId);
    nw4r::g3d::ResFile::Init(&task->m_resource);
    nw4r::g3d::ScnGroup* group = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::HeapType(heap)), NULL, 1);
    task->initProc(&task->m_resource, group, heap, initialize);
    return task;
}

IfPeachFinalTask::IfPeachFinalTask(void* resourceData, u32 fighterTaskId)
    : gfTask("IfPeachFinal", Category_Info, 14, 6, true), m_resource(resourceData), unk44(-1),
      m_soundHandle(-1), m_fighterTaskId(fighterTaskId), m_actionActive(false), m_registered(false) {}

void IfPeachFinalTask::initProc(nw4r::g3d::ResFile* resource, void* group, s32 heap, bool priorityOffset) {
    unk2C_b1 = false;
    initWork();
    createModel(resource, heap, priorityOffset);
    m_group = group;
    g_IfMngr->addGame2DObj(reinterpret_cast<nw4r::g3d::ScnObj*>(m_group));
    m_registered = true;
    nw4r::g3d::ScnObj* model = reinterpret_cast<nw4r::g3d::ScnObj*>(m_model->getSceneModel());
    reinterpret_cast<nw4r::g3d::ScnGroup*>(m_group)->Insert(reinterpret_cast<nw4r::g3d::ScnGroup*>(m_group)->sceneItemsCount, model);
    // TODO(nonmatching): the original passes the two zero arguments from one register (li r6; mr r7,r6); this form loads each separately.
    // MWCC shares one zero register only for narrow (bool/u8) parameters or for a value it does not constant-propagate:
    // an enum-typed local used for both (as em_resource_module_impl.cpp does) gives the mr but defines the zero before the
    // g_sndSystem load instead of after li r5; int/bool/u8/u16 locals, casts, inline wrappers (int, bool or enum params),
    // default arguments and enum constants all fold to two li.
    m_soundHandle = g_sndSystem->playSE(SndID(0x1B4A), -1, 0, 0, -1);
}

void IfPeachFinalTask::initWork() {
    m_group = NULL;
    for (int i = 0; i < 1; ++i) (&m_model)[i] = NULL;
}

IfPeachFinalTask::~IfPeachFinalTask() {
    if (m_soundHandle >= 0) {
        g_sndSystem->stopSE(m_soundHandle, 0);
        m_soundHandle = -1;
    }
    destroyModel();
    if (m_group != NULL) reinterpret_cast<nw4r::g3d::G3dObj*>(m_group)->Destroy();
    gfTask* fighter = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, m_fighterTaskId);
    ftPeach* peach = dynamic_cast<ftPeach*>(fighter);
    if (peach != NULL) peach->endFinalRequest();
}

void IfPeachFinalTask::createModel(nw4r::g3d::ResFile* resource, s32 heap, bool priorityOffset) {
    int dataIndex;
    int modelIndex;
    const PeachFinalModelData* data = s_peachFinalModelData;
    for (dataIndex = 0; dataIndex < 1; dataIndex++, data++) {
        int modelCount = data->firstIndex < data->endIndex ? data->endIndex - data->firstIndex : 1;
        for (modelIndex = 0; modelIndex < modelCount; ++modelIndex) {
            (&m_model)[data->firstIndex + modelIndex] = MuObject::create(resource, data->name,
                data->nodeIndex + priorityOffset, NULL, Heaps::HeapType(heap));
            (&m_model)[data->firstIndex + modelIndex]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

void IfPeachFinalTask::destroyModel() {
    for (int i = 0; i < 1; ++i) {
        if ((&m_model)[i] != NULL) {
            delete (&m_model)[i];
            (&m_model)[i] = NULL;
        }
    }
}

void IfPeachFinalTask::setAction(int action) {
    m_model->setAnimName(&s_peachFinalActions[action], true);
}

void IfPeachFinalTask::setRate(float rate) {
    for (int i = 0; i < 1; ++i) {
        MuObject* model = (&m_model)[i];
        if (model != NULL) model->m_modelAnim->setUpdateRate(rate);
    }
}

void IfPeachFinalTask::processFixPosition() {
    float rate = soSlow::getInstance()->getRate();
    rate *= gfSlowManager::getQuickRate();
    float fighterSlowRate = 1.0f;
    gfTask* fighterTask = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, m_fighterTaskId);
    ftPeach* peach = dynamic_cast<ftPeach*>(fighterTask);
    if (peach != NULL) fighterSlowRate = soExternalValueAccesser::getSlowRate(peach);
    setRate(rate * fighterSlowRate);
    // HYPOTHESIS: isNodeAnimFinished returns bool in the original; the shadow header declares int, so the result is compared unsigned.
    if (soSlow::getInstance()->isAdjust() == true && m_actionActive == true && static_cast<u32>(m_model->isNodeAnimFinished()) == 1) {
        gfTask* fighter = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, m_fighterTaskId);
        ftPeach* finalFighter = dynamic_cast<ftPeach*>(fighter);
        if (finalFighter != NULL) finalFighter->endFinalRequest();
        exit();
    }
}

void IfPeachFinalTask::setVisibilityWhole(bool visible) {
    if (visible == true && m_registered == 0) {
        g_IfMngr->addGame2DObj(reinterpret_cast<nw4r::g3d::ScnObj*>(m_group));
        m_registered = true;
    } else if (visible == false && m_registered == 1) {
        g_IfMngr->removeGame2DObj(reinterpret_cast<nw4r::g3d::ScnObj*>(m_group));
        m_registered = false;
    }
}
