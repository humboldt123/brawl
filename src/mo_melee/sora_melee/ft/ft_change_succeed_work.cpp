#include <ec/ec_mgr.h>
#include <ft/ft_change_succeed_work.h>
#include <ft/ft_common_data_accesser.h>
#include <it/it_manager.h>
#include <snd/snd_system.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_heap_module_impl.h>
#include <wn/dummy/wn_dummy_fighter.h>
#include <wn/wn_article_build_info.h>
#include <types.h>

extern "C" const wnDummyFighterData* fn_27_149B64(const ftCommonDataAccesser* accesser);
extern "C" void fn_27_1E3C78(wnDummyFighter* dummy, Vec3f* pos, Vec2f* speed, float* unk1, float* unk2, int flags, int unk6,
                             int unk7, float scale, float lr);

// HYPOTHESIS: two (0xFF, flag) pairs that the unit's static initializer fills (nothing in this unit reads them).
struct ftChangeSucceedWorkPair {
    int m_unk0;
    int m_unk4;
    ftChangeSucceedWorkPair(int unk0, int unk4) : m_unk0(unk0), m_unk4(unk4) { }
};
static ftChangeSucceedWorkPair g_ftChangeSucceedWorkPair0(0xFF, 0);
static ftChangeSucceedWorkPair g_ftChangeSucceedWorkPair1(0xFF, 1);

// HYPOTHESIS: the dummy fighter is built like a fighter article, from a kind (0, -1) and the work's own heap module.
struct ftChangeSucceedWorkArticleKind : wnArticleKindInfo {
    ftChangeSucceedWorkArticleKind() {
        kind = 0;
        subKind = -1;
    }
};
struct ftChangeSucceedWorkArticleBuildInfo : wnArticleBuildInfo {
    ftChangeSucceedWorkArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

ftChangeSucceedWork::ftChangeSucceedWork() :
    unk0(0.0f), unk4(0.0f), unk8(0.0f), unkC(0.0f), unk10(0), unk14(0.0f), unk18(0.0f), unk1C(0.0f), unk20(0.0f),
    unk4C(0), unk50(0), unk54(0), unk58(0.0f), unk5C(0.0f), unk60(0), unk64(0.0f), unk68(0), unk6C(0), unk70(0),
    unk74(0), unk78(0), unk7C(0), unk80(-1), unk84(0), unk88(0), unk8C(0.0f), unk90(0), unk94(0), unk98(-1), unk9C(0),
    unkA0(0.0f), unkA4(0.0f), unkA8(0), unkAC(1), unkB0(0), unkB4(0), unkB8(1), unkBC(0), unkC0(0), unkC4(0), unkC8(0),
    unkEC(0.0f), unkF0(0.0f), unkF4(0.0f), unkF8(0), unkFC(0), m_heapModule(NULL), m_dummyFighter(NULL)
{
    m_attackerInfo.clear();
    for (int i = 0; i < 4; i++) {
        unkCC[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
        unk100[i] = 0;
    }
}

ftChangeSucceedWork::~ftChangeSucceedWork() { }

void ftChangeSucceedWork::init() {
    unk80 = -1;
    unk98 = -1;
    unkC4 = 0;
    unkC8 = 0;
    unkFC = 0;
    unkF8 = 0;
    unk78 = 0;
    unk84 = 0;
    unk90 = 0;
    unk9C = 0;
    unkA8 = 0;
    unkAC = 1;
    unkB4 = 0;
    unkB8 = 1;
    unk68 = 0;
    unk4C = 0;
    unk50 = 0;
    unk54 = 0;
    unk64 = 0.0f;
}

void ftChangeSucceedWork::exit() {
    if (unk80 != -1) {
        g_sndSystem->stopSE(unk80, 0);
        unk80 = -1;
    }
    if (unk98 != -1) {
        g_sndSystem->stopSE(unk98, 0);
        unk98 = -1;
    }
    int instanceId = unkC4;
    if (instanceId != 0) {
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
        if (item != NULL) {
            itManager::getInstance()->removeItem(item);
        }
    }
    for (int i = 0; i < unkC8; i++) {
        BaseItem* item = itManager::getInstance()->getItemFromInstanceId(unkCC[i]);
        if (item != NULL) {
            itManager::getInstance()->removeItem(item);
        }
    }
    if (unkFC != 0) {
        g_ecMgr->endEffect(unkFC);
        unkFC = 0;
    }
    if (m_dummyFighter != NULL) {
        m_dummyFighter->deactivate(false);
        delete m_dummyFighter;
        m_dummyFighter = NULL;
    }
    if (m_heapModule != NULL) {
        delete m_heapModule;
        m_heapModule = NULL;
    }
}

void ftChangeSucceedWork::createDummyFighter(Heaps::HeapType instHeap, Heaps::HeapType modelInstHeap, Heaps::HeapType motionInstHeap) {
    if (m_heapModule == NULL && m_dummyFighter == NULL) {
        m_heapModule = new (instHeap) soHeapModuleImpl(instHeap, modelInstHeap, motionInstHeap, 0);
        m_dummyFighter = new (instHeap) wnDummyFighter(-1, ftChangeSucceedWorkArticleBuildInfo(ftChangeSucceedWorkArticleKind(), m_heapModule),
                                                       fn_27_149B64(&g_ftCommonDataAccesser));
    }
}

void ftChangeSucceedWork::activateDummyFighter(Vec3f* pos, Vec2f* speed, float* unk1, float* unk2, int flags, float scale, float lr) {
    if (m_dummyFighter != NULL) {
        fn_27_1E3C78(m_dummyFighter, pos, speed, unk1, unk2, flags, unkAC, unkB8, scale, lr);
    }
}

void ftChangeSucceedWork::deactivateDummyFighter() {
    if (m_dummyFighter != NULL) {
        m_dummyFighter->deactivate(false);
    }
}

bool ftChangeSucceedWork::isActiveDummyFighter() {
    if (m_heapModule == NULL || m_dummyFighter == NULL) {
        return false;
    }
    return m_dummyFighter->isActive();
}

Vec3f ftChangeSucceedWork::getDummyFighterPos() {
    if (m_dummyFighter != NULL) {
        return soExternalValueAccesser::getPos(m_dummyFighter);
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec2f ftChangeSucceedWork::getDummyFighterSpeed() {
    if (m_dummyFighter != NULL) {
        soKineticModule* kinetic = soExternalValueAccesser::getKineticModule(m_dummyFighter);
        return kinetic->getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    }
    return Vec2f(0.0f, 0.0f);
}

SituationKind ftChangeSucceedWork::getDummyFighterSituationKind() {
    if (m_dummyFighter != NULL) {
        return soExternalValueAccesser::getSituationKind(m_dummyFighter);
    }
    return Situation_None;
}

float ftChangeSucceedWork::getDummyFighterLr() {
    if (m_dummyFighter != NULL) {
        return soExternalValueAccesser::getLr(m_dummyFighter);
    }
    return 0.0f;
}

bool ftChangeSucceedWork::isIncludeDummyFighter(float* rect, Vec3f* out) {
    if (m_dummyFighter != NULL) {
        return soExternalValueAccesser::isIncludeRect(m_dummyFighter, rect, out, 4.0f, 10.0f);
    }
    return false;
}
