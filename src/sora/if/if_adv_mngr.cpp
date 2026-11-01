#include <types.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <if/if_adv_mngr.h>
#include <if/if_adv_task.h>
#include <if/if_center.h>
#include <if/if_figure_loader.h>
#include <if/if_player.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/menu.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <sr/sr_common.h>
#include <string.h>
#include <ty/ty_fig_listmng.h>

namespace nw4r {
namespace g3d {
class ScnChoice : public ScnGroup {
public:
    bool SetChoice(ScnObj* obj);
    bool SetChoice(int idx);
};
} // namespace g3d
} // namespace nw4r

// HYPOTHESIS: the 0x14-byte display request that dispZako and dispBoss pass on by value.
struct IfAdvDispInfo {
    u32 unk0[5];
};

extern "C" s32 fn_80109C88(s32 state);
extern "C" void fn_800DE360(s32 value);
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);

extern "C" {
IfAdvPause* fn_80109600(int arg1, int arg2);
void fn_80109774(IfAdvPause* pause, int flag);
void fn_80109850(IfAdvPause* pause);
void fn_80109BEC(IfAdvPause* pause);
void fn_8010A800(IfAdvPause* pause, nw4r::g3d::ResFile* resFile1, nw4r::g3d::ResFile* resFile2);
void fn_8010A874(IfAdvPause* pause);
IfAdvRadarData* fn_8010CF44(IfAdvRadarData* data, void* arg1, void* arg2, void* arg3, void* arg4);
void fn_8010CF88(IfAdvRadarData* data, int arg);
void fn_8010D00C();
nw4r::g3d::ScnChoice* fn_801B3440(MEMAllocator* allocator, u32* size, int capacity);
nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, int capacity);
void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn* anm);
void fn_800DFAB4(nw4r::g3d::ResFile* resFile, int numPlayer);
IfPlayer* fn_800DFCB4(IfPlayer* player, int index);
void fn_800E023C(IfPlayer* player, int arg1, int arg2, nw4r::g3d::ResFile* resFile1, nw4r::g3d::ResFile* resFile2);
void fn_800E244C(IfPlayer* player);
void fn_800E26B8(IfPlayer* player);
void fn_800DD6E4(IfCenter* center, nw4r::g3d::ResFile* resFile1, nw4r::g3d::ResFile* resFile2, int arg1, int arg2);
// HYPOTHESIS: the fourth parameter is a bool (its argument is loaded out of order like a narrower type).
void fn_800DDA50(IfCenter* center, nw4r::g3d::ScnGroup* group, int arg1, bool arg2, int arg3, int arg4);
void fn_8010BDD0(nw4r::g3d::ResFile* resFile);
void fn_8010BF3C(nw4r::g3d::ResFile* resFile);
IfAdvGauge* fn_8010B76C();
void fn_8010BC58(IfAdvGauge* gauge, nw4r::g3d::ResFile* resFile);
void fn_8010BD28(IfAdvGauge* gauge, nw4r::g3d::ResFile* resFile);
void fn_8010BD74(IfAdvGauge* gauge, nw4r::g3d::ResFile* resFile1, nw4r::g3d::ResFile* resFile2);
void fn_8010B9E4(IfAdvGauge* gauge, int flag);
void fn_8010BE98();
void fn_8010C004(nw4r::g3d::ScnGroup* group);
void fn_8010C0DC(IfAdvGauge* gauge, nw4r::g3d::ScnMdlExpand* locator);
void fn_8010C9D8(IfAdvGauge* gauge);
void fn_8010BDCC(IfAdvGauge* gauge);
void fn_8010C6C8(IfAdvGauge* gauge, s32 value);
void fn_8010C0FC(IfAdvGauge* gauge, u32 value);
void fn_8010C060(IfAdvGauge* gauge);
void fn_8010C3D4(IfAdvGauge* gauge);
void fn_8010C4D4(IfAdvGauge* gauge, s32 num);
void fn_8010C52C(IfAdvGauge* gauge, s32 num);
void fn_8010CA20(IfAdvGauge* gauge, int flag);
void fn_8010CA94(IfAdvGauge* gauge, s32 num, u8* stock);
void fn_8010CC80(IfAdvGauge* gauge);
void fn_8010CCA0(IfAdvGauge* gauge, u8 count);
void fn_8010CCE0(IfAdvGauge* gauge, float scale, u8 count, IfPlayer* player, u8 arg);
void fn_80108D1C(nw4r::g3d::ResFile* resFile);
void* fn_8010890C(void* data);
void fn_80108AE0(void* boss, nw4r::g3d::ResFile* resFile);
void fn_80108A04(void* boss, int flag);
void fn_80108DE4();
void fn_80108E44(nw4r::g3d::ScnGroup* group);
void fn_80108E5C(void* boss);
void fn_801092E0(void* boss, IfAdvDispInfo info);
void fn_80109540(void* boss);
void fn_80108404(nw4r::g3d::ResFile* resFile);
void* fn_80108014(int arg, void* data);
void fn_801081C8(void* zako, nw4r::g3d::ResFile* resFile);
void fn_801080EC(void* zako, int flag);
void fn_801084CC();
void fn_8010852C(nw4r::g3d::ScnGroup* group);
void fn_80108544(void* zako);
void fn_80108548(void* zako, IfAdvDispInfo info);
void fn_80108798(void* zako, int index);
void fn_80108858(void* zako, int arg1, int arg2);
}

// HYPOTHESIS: the state word of ifAdvTask (private in its header).
struct IfAdvTaskView {
    u8 unk0[0x40];
    u32 m_state;
};

// HYPOTHESIS: the IfAdvPause fields that the manager reads and writes.
struct IfAdvPauseView {
    u8 unk0;
    u8 unk1[0x103];
    void* unk104;
    void* unk108;
    void* unk10C;
    nw4r::g3d::ScnObj* unk110;
    u8 unk114[0x28];
    void* unk13C;
    u8 unk140[4];
    u8 unk144;
    u8 unk145[2];
    bool unk147;
};

// HYPOTHESIS: the IfAdvBoss display flags that the manager sets.
struct IfAdvBossView {
    u8 unk0[0x32];
    u8 unk32[3];
    u8 unk35[2];
    u8 unk37[2];
    u8 unk39;
    u8 unk3A[2];
    s32 unk3C;
    s32 unk40;
};

// HYPOTHESIS: the menu record bit that selects the camera file.
struct IfAdvMenuDataView {
    u8 unk0[0x28];
    u8 unk28_80 : 1;
};


// HYPOTHESIS: the IfFigureLoader instance; the second word is the padding of the original object's .sbss.
IfFigureLoader* lbl_805A0318[2];

bool IfFigureLoader::Entry::isReady() {
    if (m_state == 2) {
        if (!m_handle.isReady() && m_handle.isCanceled()) {
            return false;
        }
        m_state = 1;
    }
    if (m_state == 1) {
        if (--m_wait != 0) {
            return false;
        }
        m_state = 0;
        tyFigListData* data = lbl_805A0318[0]->m_listMgr->getFigDataId(m_figId);
        IfFigurePath path;
        path = IfFigurePath("/toy/fig/") + data->name + ".brres";
        m_handle.readRequest(path.m_str, m_buffer, 0x400000, 0);
        m_resFile = nw4r::g3d::ResFile();
        return false;
    }
    if (!m_handle.isReady()) {
        return false;
    }
    if (m_resFile.IsValid()) {
        return true;
    }
    if (m_handle.getReturnStatus() == 0) {
        m_resFile = nw4r::g3d::ResFile(m_handle.getBuffer());
        nw4r::g3d::ResFile::Init(&m_resFile);
    }
    return true;
}

void IfFigureLoader::Entry::destroy() {
    m_refCount--;
}

inline IfFigureLoader::Entry::Entry() : m_refCount(0), m_wait(0), m_state(0), m_figId(-1), m_data(NULL), m_path("") {
    Heaps::HeapType heap = Heaps::InfoExtraResource;
    if (gfHeapManager::getMaxFreeSize(Heaps::InfoExtraResource) < 0x400000u) {
        heap = Heaps::MenuResource;
    }
    m_buffer = gfHeapManager::alloc(heap, 0x400000);
}

inline IfFigureLoader::Entry::~Entry() {
    gfHeapManager::free(m_buffer);
}

inline IfFigureLoader::IfFigureLoader(tyFigListDataManager* listMgr, int num) : m_listMgr(listMgr), m_num(num) {
    m_entries = static_cast<Entry*>(gfHeapManager::alloc(Heaps::MenuInstance, num * sizeof(Entry)));
    m_next = 0;
    for (int i = 0; i < m_num; i++) {
        new (&m_entries[i]) Entry;
    }
}

inline IfFigureLoader::~IfFigureLoader() {
    for (int i = 0; i < m_num; i++) {
        Entry* entry = &m_entries[i];
        entry->~Entry();
    }
    if (m_entries) {
        gfHeapManager::free(m_entries);
    }
}

IfFigureLoader* IfFigureLoader::create(tyFigListDataManager* listMgr, int num) {
    IfFigureLoader* loader = new (Heaps::MenuInstance) IfFigureLoader(listMgr, num);
    lbl_805A0318[0] = loader;
    return loader;
}

void IfFigureLoader::destroy() {
    delete lbl_805A0318[0];
    lbl_805A0318[0] = NULL;
}

IfFigureLoader* IfFigureLoader::getInstance() {
    return lbl_805A0318[0];
}

bool IfFigureLoader::isLoadFinish() {
    return m_listMgr->isLoadFinish();
}

IfFigureLoader::EntryRef IfFigureLoader::readRequest(u32 id) {
    tyFigListData* data = m_listMgr->getFigDataId(id);
    if (data == NULL) {
        return EntryRef(NULL);
    }
    for (int i = 0; i < m_num; i++) {
        if (m_entries[i].m_data == data) {
            m_entries[i].m_refCount++;
            return EntryRef(&m_entries[i]);
        }
    }
    s32 start = m_next + 1;
    for (int i = 0; i < m_num + 1; i++) {
        s32 index = (start + i) % m_num;
        m_next = index;
        Entry* entry = &m_entries[index];
        if (entry->m_refCount == 0) {
            entry->m_refCount++;
            entry->m_figId = id;
            entry->m_data = data;
            entry->m_wait = 3;
            if (entry->m_state == 0) {
                if (entry->m_handle.m_request != NULL && !entry->m_handle.isReady()) {
                    entry->m_state = 2;
                    entry->m_handle.cancelRequest();
                } else {
                    entry->m_state = 1;
                }
            }
            return EntryRef(entry);
        }
    }
    return EntryRef(NULL);
}

bool IfFigureLoader::waitForIdle() {
    for (int i = 0; i < m_num; i++) {
        Entry* entry = &m_entries[m_next];
        if (entry->m_handle.m_request != NULL && !entry->m_handle.isReady() && !entry->m_handle.isCanceled()) {
            return false;
        }
    }
    return true;
}

void IfFigureLoader::adjustModel(u32 id, MuObject* model, MuObject* base) {
    tyFigListData* data = m_listMgr->getFigDataId(id);
    Vec3f pos;
    model->getPos(&pos, "transN");
    Vec3f rot;
    model->getRotate(&rot, "transN");
    Vec3f scale;
    model->getAnimScale(&scale, "transN");
    pos += Vec3f(data->unk28.m_x, data->unk28.m_y, data->unk28.m_z);
    model->setPos(&pos, "transN");
    rot += Vec3f(0.0f, data->unk3C, 0.0f);
    model->setScale("transN", &rot);
    float s = data->unk34;
    scale = Vec3f(scale.m_x * s, scale.m_y * s, scale.m_z * s);
    model->setObjScale(&scale);
    float baseScale = data->unk38;
    if (baseScale < 1.1920929e-7f) {
        baseScale = 1.0f;
    }
    Vec3f baseVec(baseScale, baseScale, baseScale);
    base->setObjScale(&baseVec);
}

inline IfAdvRadarDataOwner::~IfAdvRadarDataOwner() {
    delete m_ptr;
}

void IfAdvMngr::create(int arg1, int arg2) {
    g_IfAdvMngr = new (Heaps::System) IfAdvMngr(arg1, arg2);
}

inline IfAdvMngr::~IfAdvMngr() {
    destroyModel();
    fn_80109774(m_pause, 1);
    freeResource();
}

void IfAdvMngr::destroy() {
    delete g_IfAdvMngr;
    g_IfAdvMngr = NULL;
}

IfAdvMngr::IfAdvMngr(int arg1, int arg2) :
    m_task(NULL), unk4_80(true), m_choice(NULL), unkC(0), m_archive(NULL), unk38(0), unk3C(0), unk40(NULL),
    m_group(NULL), unk48_80(false), unk48_40(false), unk48_20(false), unk48_10(false), unk4C(0), unk50(0), unk54(0),
    unk55(0), unk56(2), unk57(0), unk58(NULL), unk5C(NULL), m_zako(NULL), m_boss(NULL), unk70(0), m_pause(NULL),
    m_stockNum(0), m_stockIndex(0), m_gauge(NULL), m_center(NULL), m_corps(0), unk110(1), m_isPause(0),
    m_numPlayer(2), unk118(1) {
    for (int i = 0; i < 99; i++) {
        m_stock[i] = 0;
    }
    for (int i = 0; i < 7; i++) {
        m_players[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        unk68[i] = 0;
        unk6A[i] = 0;
        unk64[i] = 0;
        unk66[i] = 0;
    }
    m_pause = fn_80109600(arg1, arg2);
}

void IfAdvMngr::loadResource(gfArchive* archive) {
    m_archive = archive;
}

extern "C" void fn_80106A64(IfAdvMngr* mngr, void* arg1, void* arg2, void* arg3, void* arg4) {
    mngr->unk14 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk14);
    mngr->unk18 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk18);
    mngr->unk34 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 2, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk34);
    mngr->unk1C = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 3, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk1C);
    mngr->unk20 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 4, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk20);
    mngr->unk24 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 5, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk24);
    mngr->unk2C = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 8, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk2C);
    mngr->unk28 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 6, 0xFFFE));
    nw4r::g3d::ResFile::Init(&mngr->unk28);
    mngr->unk40 = mngr->m_archive->getData(0, Data_Type_Misc, 7, 0xFFFE);
    if (reinterpret_cast<IfAdvMenuDataView*>(g_GameGlobal->getGlobalRecordMenuDatap())->unk28_80) {
        mngr->unk30 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 0xC, 0xFFFE));
    } else {
        mngr->unk30 = nw4r::g3d::ResFile(mngr->m_archive->getData(0, Data_Type_Misc, 0xB, 0xFFFE));
    }
    nw4r::g3d::ResFile::Init(&mngr->unk30);
    mngr->unk58 = mngr->m_archive->getData(0, Data_Type_Misc, 9, 0xFFFE);
    mngr->unk5C = mngr->m_archive->getData(0, Data_Type_Misc, 0xA, 0xFFFE);
    if (arg1 && arg2 && arg3 && arg4) {
        IfAdvRadarData* data = static_cast<IfAdvRadarData*>(operator new(8, Heaps::System));
        if (data) {
            data = fn_8010CF44(data, arg1, arg2, arg3, arg4);
        }
        if (mngr->m_radarData.m_ptr != data) {
            delete mngr->m_radarData.m_ptr;
            mngr->m_radarData.m_ptr = data;
        }
    }
}

void IfAdvMngr::freeResource() {
    if (unk14.IsValid()) {
        unk14 = nw4r::g3d::ResFile();
    }
    if (unk18.IsValid()) {
        unk18 = nw4r::g3d::ResFile();
    }
    if (unk34.IsValid()) {
        unk34 = nw4r::g3d::ResFile();
    }
    if (unk1C.IsValid()) {
        unk1C = nw4r::g3d::ResFile();
    }
    if (unk20.IsValid()) {
        unk20 = nw4r::g3d::ResFile();
    }
    if (unk24.IsValid()) {
        unk24 = nw4r::g3d::ResFile();
    }
    if (unk28.IsValid()) {
        unk28 = nw4r::g3d::ResFile();
    }
    if (unk40) {
        unk40 = NULL;
    }
    if (unk2C.IsValid()) {
        unk2C = nw4r::g3d::ResFile();
    }
    if (unk30.IsValid()) {
        unk30 = nw4r::g3d::ResFile();
    }
    if (unk58) {
        unk58 = NULL;
    }
    if (unk5C) {
        unk5C = NULL;
    }
    if (m_radarData.m_ptr) {
        delete m_radarData.m_ptr;
        m_radarData.m_ptr = NULL;
    }
}

bool IfAdvMngr::isLoadFinish() {
    return true;
}

void IfAdvMngr::createModel() {
    int i;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    m_choice = fn_801B3440(allocator, NULL, 2);
    m_group = fn_801AB6CC(allocator, NULL, 0x20);
    fn_800DFAB4(&unk14, m_numPlayer);
    for (i = 0; i < m_numPlayer; i++) {
        IfPlayer* player = static_cast<IfPlayer*>(operator new(0x5D8, Heaps::InfoInstance));
        if (player) {
            player = fn_800DFCB4(player, i);
        }
        m_players[i] = player;
        fn_800E023C(player, 0, 0, &unk14, &unk2C);
    }
    m_center = new (Heaps::InfoInstance) IfCenter;
    fn_800DD6E4(m_center, &unk14, &unk34, 0, 5);
    fn_8010BDD0(&unk18);
    fn_8010BF3C(&unk14);
    m_gauge = fn_8010B76C();
    fn_8010BC58(m_gauge, &unk18);
    fn_8010BD28(m_gauge, &unk14);
    fn_8010BD74(m_gauge, &unk18, &unk2C);
    fn_80108D1C(&unk18);
    m_boss = fn_8010890C(unk58);
    fn_80108AE0(m_boss, &unk18);
    fn_80108404(&unk18);
    m_zako = fn_80108014(0, unk58);
    fn_801081C8(m_zako, &unk18);
    fn_8010A800(m_pause, &unk1C, &unk20);
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk104 = unk28.ptr();
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk108 = unk40;
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk10C = unk24.ptr();
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk13C = unk5C;
    // MATCH-ONLY: the by-value copy of the animation is declared before it, as a raw word.
    void* anm2;
    nw4r::g3d::ResAnmScn anm;
    anm = unk30.GetResAnmScn(0);
    anm2 = anm.ptr();
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, reinterpret_cast<nw4r::g3d::ResAnmScn*>(&anm2));
    g_gfSceneRoot->setInfoCamResAnm(&anm);
    if (m_radarData.m_ptr) {
        fn_8010CF88(m_radarData.m_ptr, 0);
    }
}

void IfAdvMngr::destroyModel() {
    if (m_gauge) {
        fn_8010C0DC(m_gauge, IfPlayer::getPlayerLocator());
    }
    if (m_gauge) {
        fn_8010C9D8(m_gauge);
    }
    if (m_center) {
        delete m_center;
        m_center = NULL;
    }
    for (int i = 0; i < 7; i++) {
        if (m_players[i]) {
            delete m_players[i];
            m_players[i] = NULL;
        }
    }
    IfPlayer::destroyPlayerLocator();
    if (m_gauge) {
        fn_8010B9E4(m_gauge, 1);
        m_gauge = NULL;
    }
    fn_8010BE98();
    if (m_boss) {
        fn_80108A04(m_boss, 1);
        m_boss = NULL;
    }
    fn_80108DE4();
    if (m_zako) {
        fn_801080EC(m_zako, 1);
        m_zako = NULL;
    }
    fn_801084CC();
    if (m_pause) {
        fn_8010A874(m_pause);
    }
    if (m_group) {
        m_group->Destroy();
        m_group = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
    if (m_task) {
        m_task->exit();
        m_task = NULL;
    }
    fn_8010D00C();
    if (m_choice) {
        m_choice->Destroy();
        m_choice = NULL;
    }
}

void IfAdvMngr::startMelee() {
    unk4_80 = false;
    fn_800DDA50(m_center, m_group, 0, 0, 0, 0);
    IfPlayer::startMelee(m_group);
    fn_8010C004(m_group);
    fn_80108E44(m_group);
    fn_8010852C(m_group);
    m_choice->Insert(0, m_group);
    m_choice->Insert(1, reinterpret_cast<IfAdvPauseView*>(m_pause)->unk110);
    unkC = 0;
    m_choice->SetChoice(0);
    if (unk110) {
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_choice));
    }
    m_task = ifAdvTask::create();
}

s32 IfAdvMngr::main() {
    s32 result = 0;
    if (m_isPause != 0) {
        result = 0;
        s32 state = fn_80109C88(reinterpret_cast<s32>(m_pause));
        switch (state) {
        case 1:
            result = 1;
            break;
        case 3:
            result = 2;
            break;
        case 2:
            result = 3;
            break;
        case 4:
            result = 4;
            break;
        }
    } else {
        meleeMain();
        fn_800DE360(reinterpret_cast<s32>(m_center));
    }
    g_gfSceneRoot->remove(reinterpret_cast<nw4r::g3d::ScnMdl*>(m_choice));
    if (unk118 != 0) {
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_choice));
    }
    if (unk118 == 1) {
        fn_8000DB40(g_gfSceneRoot, 8);
    }
    return result;
}

void IfAdvMngr::meleeMain() {
    for (int i = 0; i < 7; i++) {
        if (m_players[i]) {
            fn_800E244C(m_players[i]);
        }
    }
    IfPlayer::arrowMain(m_players, 7);
    if (m_zako) {
        fn_80108544(m_zako);
    }
    if (m_zako) {
        fn_80108E5C(m_boss);
    }
    if (m_gauge) {
        fn_8010BDCC(m_gauge);
    }
    if (unk48_80 && m_gauge) {
        fn_8010C6C8(m_gauge, unk4C);
    }
    u32 score = unk50;
    if (score > 999999999) {
        score = 999999999;
    }
    if (m_gauge) {
        fn_8010C0FC(m_gauge, score);
    }
    if (unk48_40 && m_gauge) {
        fn_8010C060(m_gauge);
    }
    if (m_corps >= 0 && m_gauge) {
        fn_8010C3D4(m_gauge);
    }
    if (unk48_20) {
        if (++unk54 == 0x10) {
            unk54 = 0;
            unk48_20 = false;
        }
        float scale = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(m_stock[0])) * 10 + 1;
        if (m_gauge) {
            fn_8010CCE0(m_gauge, scale, unk54, m_players[unk55], unk56);
        }
    }
    if (unk48_10) {
        if (++unk57 == 10) {
            unk48_10 = false;
            unk57 = 0;
            if (m_gauge) {
                fn_8010CA94(m_gauge, m_stockNum, m_stock);
            }
        }
        if (unk57 != 0 && unk57 < 10 && m_gauge) {
            fn_8010CC80(m_gauge);
        }
        if (m_gauge) {
            fn_8010CCA0(m_gauge, unk57);
        }
    }
    if (unk68[0]) {
        if (unk68[0]) {
            if (++unk64[0] == 1 && m_zako) {
                fn_80108858(m_zako, 1, 0);
            }
            if (unk64[0] == 10) {
                unk64[0] = 0;
                unk68[0] = 0;
            }
        }
    }
    if (unk6A[0]) {
        if (unk6A[0]) {
            if (++unk66[0] == 1 && m_zako) {
                fn_80108858(m_zako, 0, 0);
            }
            if (unk66[0] == 0x1F) {
                unk66[0] = 0;
                unk6A[0] = 0;
                if (m_zako) {
                    fn_80108798(m_zako, 0);
                }
            }
        }
    }
    if (unk68[1]) {
        if (unk68[1]) {
            if (++unk64[1] == 1 && m_zako) {
                fn_80108858(m_zako, 1, 1);
            }
            if (unk64[1] == 10) {
                unk64[1] = 0;
                unk68[1] = 0;
            }
        }
    }
    if (unk6A[1]) {
        if (unk6A[1]) {
            if (++unk66[1] == 1 && m_zako) {
                fn_80108858(m_zako, 0, 1);
            }
            if (unk66[1] == 0x1F) {
                unk66[1] = 0;
                unk6A[1] = 0;
                if (m_zako) {
                    fn_80108798(m_zako, 1);
                }
            }
        }
    }
}

void IfAdvMngr::renderPre() {
    IfPlayer::loupeRenderPreInit();
    for (int i = 0; i < 7; i++) {
        if (m_players[i]) {
            fn_800E26B8(m_players[i]);
            m_players[i]->loupeRenderPre();
        }
    }
}

u32 IfAdvMngr::getState() {
    return reinterpret_cast<IfAdvTaskView*>(m_task)->m_state;
}

void IfAdvMngr::setNumPlayer(int numPlayer) {
    m_numPlayer = numPlayer;
}

extern "C" void fn_801077D8(IfAdvMngr* mngr, IfAdvDispInfo info) {
    if (mngr->m_zako) {
        fn_80108548(mngr->m_zako, info);
    }
}

void IfAdvMngr::erasesZako(int index) {
    if (m_zako) {
        fn_80108798(m_zako, index);
    }
}

extern "C" void fn_80107848(IfAdvMngr* mngr, IfAdvDispInfo info) {
    if (mngr->m_boss) {
        fn_801092E0(mngr->m_boss, info);
    }
}

void IfAdvMngr::eraseBoss() {
    if (m_boss) {
        fn_80109540(m_boss);
    }
}

void IfAdvMngr::setBossDamageDisp(int index) {
    IfAdvBossView* boss = static_cast<IfAdvBossView*>(m_boss);
    if (boss) {
        boss->unk35[index] = 1;
        boss->unk3C = index;
    }
}

void IfAdvMngr::setBossDeathDisp(int index) {
    IfAdvBossView* boss = static_cast<IfAdvBossView*>(m_boss);
    if (boss) {
        boss->unk37[index] = 1;
        boss->unk32[index] = 0;
        boss->unk40 = index;
    }
}

void IfAdvMngr::setBossMoveDispFlag(bool flag) {
    IfAdvBossView* boss = static_cast<IfAdvBossView*>(m_boss);
    if (boss) {
        boss->unk39 = flag;
    }
}

// HYPOTHESIS: the second parameter is not read.
extern "C" bool fn_80107914(IfAdvMngr* mngr, u8 kind, int arg) {
    if (!mngr->m_gauge) {
        return false;
    }
    if (mngr->m_gauge) {
        fn_8010CA20(mngr->m_gauge, 1);
    }
    if (++mngr->m_stockNum >= 99) {
        return false;
    }
    mngr->m_stock[mngr->m_stockNum] = kind;
    if (mngr->m_gauge) {
        reinterpret_cast<u8*>(mngr->m_gauge)[0x360] = 1;
    }
    if (mngr->m_gauge) {
        fn_8010CA94(mngr->m_gauge, mngr->m_stockNum, mngr->m_stock);
    }
    return true;
}


bool IfAdvMngr::addStockSmb() {
    // HYPOTHESIS: a function-local counter of the added Smash Balls (the .sbss word after g_IfAdvMngr).
    static int lbl_805A0324;
    if (!m_gauge) {
        return false;
    }
    if (m_gauge) {
        fn_8010CA20(m_gauge, 1);
    }
    if (++m_stockNum >= 99) {
        return false;
    }
    m_stock[m_stockNum] = 0x2E;
    lbl_805A0324++;
    if (m_gauge) {
        reinterpret_cast<u8*>(m_gauge)[0x360] = 1;
    }
    if (m_gauge) {
        fn_8010CA94(m_gauge, m_stockNum, m_stock);
    }
    return true;
}

// HYPOTHESIS: the first parameter is not read.
extern "C" int fn_80107A70(IfAdvMngr* mngr, int arg, bool flag) {
    u8 stock[100] = { 0 };
    if (!mngr->m_gauge) {
        return 0x3E;
    }
    for (int i = 0; i < 0x10; i++) {
        if (mngr->unk48_20) {
            if (++mngr->unk54 == 0x10) {
                mngr->unk54 = 0;
                mngr->unk48_20 = false;
            }
            float scale = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(mngr->m_stock[0])) * 10 + 1;
            if (mngr->m_gauge) {
                fn_8010CCE0(mngr->m_gauge, scale, mngr->unk54, mngr->m_players[mngr->unk55], mngr->unk56);
            }
        }
        if (mngr->unk48_10) {
            if (++mngr->unk57 == 10) {
                mngr->unk57 = 0;
                mngr->unk48_10 = false;
                if (mngr->m_gauge) {
                    fn_8010CA94(mngr->m_gauge, mngr->m_stockNum, mngr->m_stock);
                }
            }
            if (mngr->unk57 != 0 && mngr->unk57 < 10 && mngr->m_gauge) {
                fn_8010CC80(mngr->m_gauge);
            }
            if (mngr->m_gauge) {
                fn_8010CCA0(mngr->m_gauge, mngr->unk57);
            }
        }
    }
    for (int i = 0; i < mngr->m_stockNum; i++) {
        stock[i] = mngr->m_stock[i + 1];
    }
    memset(mngr->m_stock, 0, sizeof(mngr->m_stock));
    for (int i = 0; i < mngr->m_stockNum; i++) {
        mngr->m_stock[i] = stock[i];
    }
    if (mngr->m_stockNum >= 0 && flag) {
        mngr->unk48_20 = true;
        mngr->unk48_10 = true;
    }
    if (mngr->m_gauge && flag) {
        reinterpret_cast<u8*>(mngr->m_gauge)[0x360] = 1;
    }
    int kind;
    if (mngr->m_stockNum <= 0) {
        kind = 0x3E;
    } else {
        kind = mngr->m_stock[mngr->m_stockIndex];
    }
    if (kind != 0x3E) {
        if (++mngr->m_stockIndex >= 99) {
            mngr->m_stockIndex = 0;
        }
        mngr->m_stockNum--;
    }
    if (mngr->m_stockNum == 0 && mngr->m_gauge) {
        fn_8010CA20(mngr->m_gauge, 0);
    }
    return kind;
}

bool IfAdvMngr::addCorps() {
    if (m_corps >= 50) {
        return false;
    }
    m_corps++;
    if (m_gauge) {
        fn_8010C4D4(m_gauge, m_corps);
    }
    return true;
}

bool IfAdvMngr::removeCorps() {
    if (m_corps <= 0) {
        return false;
    }
    m_corps--;
    if (m_gauge) {
        fn_8010C52C(m_gauge, m_corps);
    }
    return true;
}

void IfAdvMngr::startPauseMenu(bool arg1, bool arg2) {
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk144 = arg1;
    reinterpret_cast<IfAdvPauseView*>(m_pause)->unk0 = arg2;
    fn_80109850(m_pause);
    m_choice->SetChoice(1);
    m_isPause = 1;
}

void IfAdvMngr::exitPauseMenu() {
    fn_80109BEC(m_pause);
    m_choice->SetChoice(0);
    m_isPause = 0;
}

bool IfAdvMngr::isEndPauseMenu() {
    return reinterpret_cast<IfAdvPauseView*>(m_pause)->unk147;
}

void IfAdvMngr::startSavePointMenu() {
    m_choice->SetChoice(static_cast<nw4r::g3d::ScnObj*>(NULL));
}

void IfAdvMngr::exitSavePointMenu() {
    m_choice->SetChoice(0);
}

void IfAdvMngr::setCameraResAnm() {
    nw4r::g3d::ResAnmScn anm;
    anm = unk30.GetResAnmScn(0);
    nw4r::g3d::ResAnmScn anm2 = anm;
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, &anm2);
    g_gfSceneRoot->setInfoCamResAnm(&anm);
}

IfAdvMngr* g_IfAdvMngr;
