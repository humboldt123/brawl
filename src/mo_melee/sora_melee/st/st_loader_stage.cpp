#include <cstring>
#include <ec/ec_mgr.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_pool.h>
#include <gf/gf_module.h>
#include <gm/gm_global.h>
#include <if/if_smash_appear.h>
#include <it/it_gen_archive.h>
#include <it/it_manager.h>
#include <revolution/CX/CXCompression.h>
#include <revolution/OS/OSCache.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_stage.h>
#include <st/module.h>
#include <st/st_common_gimmick.h>
#include <types.h>

extern "C" {
bool fn_27_23ABA0(stLoader* loader);
bool fn_27_23AC10(stLoader* loader);
void fn_27_23AC24(stLoader* loader);
void fn_27_23AC30(stLoader* loader);
Stage* fn_27_221A24(srStageKind stageKind);
u32 fn_80015530(gfArchive* archive, const char* path, void* buffer, int unk);
void fn_8005EA4C(ecMgr* mgr, int resourceId, void* data, u32 size, int unk1, int unk2);
void fn_8005EB68(ecMgr* mgr, int resourceId);
void fn_80060B1C(ecMgr* mgr, u32 group, u32 mask);
void fn_80060BB8(ecMgr* mgr, u32 group, u32 mask);
}

// MATCH-ONLY: the native code reads the 16-bit stage kind unsigned, not through the signed enum bitfield.
static inline u32 getStageKind(gmGlobalModeMelee* melee) {
    return *reinterpret_cast<u16*>(reinterpret_cast<u8*>(&melee->m_meleeInitData) + 0x12);
}

extern gfModuleManager* lbl_805A0038;
extern const char* lbl_8042BC18[];
extern u8* lbl_8049DEC8;

static itGenSheetKind g_unkItemSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_unkItemSheet1(Stages::srStageKind(0xff), 1);

stLoaderStage::~stLoaderStage() {
    if (m_archive) {
        delete m_archive;
    }
    if (m_secondaryArchive) {
        delete m_secondaryArchive;
    }
    if (m_fileArchive) {
        delete m_fileArchive;
    }
    if (m_effectArchive) {
        delete m_effectArchive;
    }
    if (m_secondaryData) {
        gfHeapManager::free(m_secondaryData);
    }
    if (m_fileData) {
        gfHeapManager::free(m_fileData);
    }
    if (m_effectData) {
        gfHeapManager::free(m_effectData);
    }
    m_heapBuffer = nullptr;
    m_archive = nullptr;
    m_secondaryArchive = nullptr;
    m_fileArchive = nullptr;
    m_effectArchive = nullptr;
    m_secondaryData = nullptr;
    m_fileData = nullptr;
    m_effectData = nullptr;
    m_readBuffer = nullptr;
}

void stLoaderStage::buildFileName() {
    static const char* s_levelNames[] = { "1", "2", "3", "4", "5" };
    static const char* s_editNames[] = { "0", "1", "2" };

    m_effectResourceId = getStageKind(m_globalModeMelee) + 0x31;
    m_stageKind = getStageKind(m_globalModeMelee);
    m_subStageKind = m_globalModeMelee->m_meleeInitData.m_subStageKind;
    memset(m_fileName, 0, sizeof(m_fileName));
    strcpy(m_fileName, "/STAGE/MELEE/STG");
    switch (getStageKind(m_globalModeMelee)) {
    case Stages::MetalGear:
        strcat(m_fileName, lbl_8042BC18[getStageKind(m_globalModeMelee)]);
        switch (m_globalModeMelee->m_meleeInitData.m_subStageKind) {
        case 0:
            strcat(m_fileName, "_00");
            break;
        case 1:
            strcat(m_fileName, "_01");
            break;
        case 2:
            strcat(m_fileName, "_02");
            break;
        default:
            strcat(m_fileName, "_00");
            break;
        }
        break;
    case Stages::Village:
        strcat(m_fileName, lbl_8042BC18[getStageKind(m_globalModeMelee)]);
        switch (m_globalModeMelee->m_meleeInitData.m_subStageKind) {
        case 0:
            strcat(m_fileName, "_00");
            break;
        case 1:
            strcat(m_fileName, "_01");
            break;
        case 2:
            strcat(m_fileName, "_02");
            break;
        case 3:
            strcat(m_fileName, "_03");
            break;
        case 4:
            strcat(m_fileName, "_03");
            break;
        case 5:
            strcat(m_fileName, "_03");
            break;
        case 6:
            strcat(m_fileName, "_04");
            break;
        case 7:
            strcat(m_fileName, "_04");
            break;
        default:
            strcat(m_fileName, "_00");
            break;
        }
        break;
    case Stages::Tengan: {
        strcat(m_fileName, "TENGAN_");
        int level = m_subStageKind;
        if (level > 2) {
            level = 2;
        }
        if (level < 0) {
            level = 0;
        }
        strcat(m_fileName, s_levelNames[level]);
        break;
    }
    case Stages::MarioPast:
        strcat(m_fileName, lbl_8042BC18[getStageKind(m_globalModeMelee)]);
        if (m_globalModeMelee->m_meleeInitData.m_subStageKind != 0) {
            strcat(m_fileName, "_01");
        } else {
            strcat(m_fileName, "_00");
        }
        break;
    case Stages::StarFox:
        strcat(m_fileName, "STARFOX_GDIFF");
        break;
    case Stages::TargetBreak: {
        strcat(m_fileName, "TARGETLV");
        // HYPOTHESIS: the target level is the top three bits of the byte at init data offset 6.
        int level = (u8)m_globalModeMelee->m_meleeInitData._0x5[1] >> 5;
        if (level > 4) {
            level = 4;
        }
        if (level < 0) {
            level = 0;
        }
        strcat(m_fileName, s_levelNames[level]);
        m_effectResourceId = 0x12B;
        break;
    }
    case Stages::Edit:
        strcat(m_fileName, "EDIT_");
        strcat(m_fileName, s_editNames[m_globalModeMelee->m_meleeInitData.m_subStageKind & 3]);
        break;
    case Stages::CharaRoll:
        m_effectResourceId = 0x133;
    default:
        strcat(m_fileName, lbl_8042BC18[getStageKind(m_globalModeMelee)]);
        break;
    }
    strcat(m_fileName, ".PAC");
}

u32 stLoaderStage::getMemoryBlankSize() {
    u32 stageKind = getStageKind(m_globalModeMelee);
    if (stageKind == 0x27) {
        return 0x100000;
    }
    if (stageKind == Stages::StarFox) {
        return 0x366640;
    }
    if (stageKind == Stages::Emblem) {
        return 0x4B3300;
    }
    if (stageKind == Stages::Edit) {
        return 0x100000;
    }
    if (stageKind == Stages::Adventure) {
        // HYPOTHESIS: an adventure state byte inside the unmapped part of gmAdvData.
        u8 state = reinterpret_cast<u8*>(g_GameGlobal->m_advData)[0x604];
        // MATCH-ONLY: the negated conjunction keeps two compares where `state == 3 || state == 4` becomes a range test.
        if (!(state != 3 && state != 4)) {
            return 0x2840;
        }
        return 0x400000;
    }
    if (stageKind == Stages::Famicom) {
        return 0x100000;
    }
    return 0x2000;
}

void stLoaderStage::processBegin() {
    if ((u8)_0x45[1] == 0) {
        return;
    }
    switch (m_state) {
    case 0:
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        } else if (getStageKind(m_globalModeMelee) != 0) {
            if (m_commonLoader == nullptr || m_commonLoader->isLoad()) {
                m_state = 1;
            }
        }
        break;
    case 1:
        // HYPOTHESIS: the first word of the stage-resource heap entry is zero until that heap exists.
        if (g_HeapInfos[Heaps::StageResource].m_name == nullptr) {
            if ((u8)_0x45[2] == 1) {
                m_state = 6;
            } else {
                m_state = 0;
            }
            break;
        }
        if (unk99 == 1 && getStageKind(m_globalModeMelee) != Stages::Result &&
            getStageKind(m_globalModeMelee) != Stages::Config)
        {
            if ((u8)_0x45[2] == 1) {
                m_state = 6;
            } else {
                m_state = 0;
            }
            break;
        }
        buildFileName();
        if (m_archive == nullptr) {
            m_archive = new (Heaps::StageResource) gfArchive;
        }
        if (unk98 == 0) {
            m_moduleName = moGetStageModuleName((srStageKind)getStageKind(m_globalModeMelee));
            if (m_moduleName) {
                gfModuleManager::LoadRequestResult result;
                lbl_805A0038->setTempolaryLoadHeap(Heaps::StageResource);
                gfModuleManager::loadModuleRequest(&result, lbl_805A0038, m_moduleName, (Heaps::HeapType)0x33, true, false);
                lbl_805A0038->setTempolaryLoadHeap(Heaps::StageResource);
            }
        }
        m_state = 7;
        break;
    case 7: {
        if (m_moduleName && unk98 == 0 && !lbl_805A0038->isLoaded(m_moduleName)) {
            break;
        }
        int fileSize = gfFileIO::getFileSize(m_fileName);
        int freeSize = gfHeapManager::getMaxFreeSize(Heaps::StageResource);
        freeSize -= getMemoryBlankSize();
        u8* buffer = static_cast<u8*>(gfHeapManager::alloc(Heaps::StageResource, freeSize));
        m_heapBuffer = buffer;
        m_heapCursor = buffer;
        m_readBuffer = buffer + freeSize - fileSize;
        m_archive->m_Heap = gfHeapManager::getHeap(Heaps::StageResource);
        if (fn_80015530(m_archive, m_fileName, m_readBuffer, 0) == 1) {
            m_secondaryArchive = new (Heaps::StageResource) gfArchive;
            m_fileArchive = new (Heaps::StageResource) gfArchive;
            m_effectArchive = new (Heaps::StageResource) gfArchive;
            m_state = 2;
        } else {
            m_state = 5;
        }
        break;
    }
    case 2: {
        if (!m_archive->unk9) {
            break;
        }
        void* data = m_archive->getData(Data_Type_Misc, 0, 0xFFFE);
        u32 size = CXGetUncompressedSize(data);
        if (size > 0x20) {
            m_secondaryData = m_heapCursor;
            m_heapCursor += size;
            CXUncompressLZ(data, m_secondaryData);
            DCFlushRange(m_secondaryData, size);
            m_secondaryArchive->setFileImage(m_secondaryData, size, Heaps::StageResource);
        }
        data = m_archive->getData(Data_Type_Misc, 1, 0xFFFE);
        size = CXGetUncompressedSize(data);
        if (size > 0x20) {
            m_fileData = m_heapCursor;
            m_heapCursor += size;
            CXUncompressLZ(data, m_fileData);
            DCFlushRange(m_fileData, size);
            m_fileArchive->setFileImage(m_fileData, size, Heaps::StageResource);
        }
        data = m_archive->getData(Data_Type_Misc, 2, 0xFFFE);
        if (data) {
            size = CXGetUncompressedSize(data);
            if (size > 0x20) {
                m_effectData = m_heapCursor;
                m_heapCursor += size;
                CXUncompressLZ(data, m_effectData);
                DCFlushRange(m_effectData, size);
                m_effectArchive->setFileImage(m_effectData, size, Heaps::StageResource);
                fn_8005EA4C(g_ecMgr, m_effectResourceId, m_effectData, size, 0, 0xFF);
            }
        }
        delete m_archive;
        m_archive = nullptr;
        m_state = 3;
        break;
    }
    case 3:
        if (isRemove()) {
            m_state = 5;
        }
        break;
    case 5:
        if (m_moduleName) {
            lbl_805A0038->destroy(m_moduleName);
        }
        fn_8005EB68(g_ecMgr, m_effectResourceId);
        if (m_archive) {
            delete m_archive;
        }
        if (m_secondaryArchive) {
            delete m_secondaryArchive;
        }
        if (m_fileArchive) {
            delete m_fileArchive;
        }
        if (m_effectArchive) {
            delete m_effectArchive;
        }
        gfHeapManager::free(m_heapBuffer);
        m_archive = nullptr;
        m_heapBuffer = nullptr;
        m_secondaryArchive = nullptr;
        m_fileArchive = nullptr;
        m_effectArchive = nullptr;
        m_secondaryData = nullptr;
        m_fileData = nullptr;
        m_effectData = nullptr;
        if ((u8)_0x45[2] == 1) {
            m_state = 6;
        } else {
            m_state = 0;
        }
        break;
    }
}

void stLoaderStage::entryEntity() {
    if (fn_27_23AC10(this) != true && m_stage == nullptr) {
        g_GameGlobal->m_stageData->m_motionRatio = 1.0f;
        g_GameGlobal->m_stageData->m_motionSubRatio = 1.0f;
        m_stage = fn_27_221A24((srStageKind)getStageKind(m_globalModeMelee));
        if (m_commonLoader) {
            static_cast<stCommonGimmick*>(m_stage)->m_commonFileData = m_commonLoader->m_secondaryArchive;
        }
        static_cast<stCommonGimmick*>(m_stage)->unk1AC = true;
        static_cast<stCommonGimmick*>(m_stage)->m_fileData = m_fileArchive;
        static_cast<stCommonGimmick*>(m_stage)->m_secondaryFileData = m_secondaryArchive;
        static_cast<stCommonGimmick*>(m_stage)->setupItemSheet();
        m_stage->createObj();
        m_stage->unk198 = false;
        fn_27_23AC24(this);
        if (lbl_8049DEC8) {
            lbl_8049DEC8[0x5C] = true;
        }
        itGenSheetKind sheet((srStageKind)0xFF, 0);
        u32 stageKind = getStageKind(m_globalModeMelee);
        if (stageKind != Stages::Adventure) {
            sheet = itGenSheetKind((srStageKind)stageKind, 0);
        } else {
            sheet = itGenSheetKind(Stages::Adventure, *reinterpret_cast<int*>(reinterpret_cast<u8*>(g_GameGlobal->m_advData) + 0x620));
        }
        itManager::getInstance()->preloadItemGenArchive(sheet, 0);
        itManager::getInstance()->preloadItemKindArchiveFromGenArchive(sheet, 0);
    }
}

void stLoaderStage::entryEntitySuddenDeath() {
    if (fn_27_23AC10(this) != true && m_stage == nullptr) {
        entryEntity();
        m_stage->unk198 = true;
    }
}

void stLoaderStage::removeEntity() {
    if (fn_27_23AC10(this) && m_stage) {
        disablePause();
        IfSmashAppearTask* appearTask = m_stage->getAppearTask();
        if (appearTask) {
            appearTask->removeInfo();
        }
        m_stage->exit();
        fn_27_23AC30(this);
        m_stage = nullptr;
        g_GameGlobal->m_stageData->m_motionRatio = 1.0f;
        g_GameGlobal->m_stageData->m_motionSubRatio = 1.0f;
        if (lbl_8049DEC8) {
            lbl_8049DEC8[0x5C] = false;
        }
    }
}

void stLoaderStage::enablePause() {
    if (m_stage) {
        static_cast<stCommonGimmick*>(m_stage)->setPause(true);
        // HYPOTHESIS: bit 1 of the scene-root byte at 0x464 pauses scene animation.
        reinterpret_cast<u8*>(g_gfSceneRoot)[0x464] |= 2;
        fn_80060B1C(g_ecMgr, 0x2000000, 0xFF000000);
    }
}

void stLoaderStage::disablePause() {
    if (m_stage) {
        static_cast<stCommonGimmick*>(m_stage)->setPause(false);
        reinterpret_cast<u8*>(g_gfSceneRoot)[0x464] &= ~2;
        fn_80060BB8(g_ecMgr, 0x2000000, 0xFF000000);
    }
}

Stage* stLoaderStage::getStage() {
    return m_stage;
}

bool stLoaderStage::isRemove() {
    if ((u8)_0x45[2] == 1 || m_stageKind != getStageKind(m_globalModeMelee) ||
        m_subStageKind != m_globalModeMelee->m_meleeInitData.m_subStageKind)
    {
        return true;
    }
    return false;
}

bool stLoaderStage::isLoad() {
    if (m_stageKind != getStageKind(m_globalModeMelee) ||
        m_subStageKind != m_globalModeMelee->m_meleeInitData.m_subStageKind)
    {
        return false;
    }
    return fn_27_23ABA0(this);
}
