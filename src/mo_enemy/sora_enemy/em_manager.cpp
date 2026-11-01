#include <em/em_archive.h>
#include <em/em_create.h>
#include <em/em_info.h>
#include <em/em_manager.h>
#include <em/enemy.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <so/so_archive_db.h>
#include <types.h>

// The enemy manager: the registry of live enemies and of the archives preloaded for each enemy kind, and emArchive,
// the resource ids of one kind's archives. No boundary separates the emArchive code from the manager's (no data, no
// static initializer), so both stay in this unit, emArchive last as in the module.

extern "C" {

// HYPOTHESIS: soArchiveDb::remove(index), the counterpart of soArchiveDb::create (it destroys manager `index`).
void fn_27_6FDE0(u32 index);
// HYPOTHESIS: the index of the area instance `areaId` of an object (-1 if it has none).
int fn_27_8CC68(StageObject* object, int areaId);
// HYPOTHESIS: utArchiveManager's registration of an archive; returns its resource id.
u32 fn_800455E0(utArchiveManager* manager, gfArchive* archive);
// The load state of a resource id (0: not loaded), as in itGenArchive::isComp.
s32 fn_80045DC0(utArchiveManager* manager, u32 resourceId, s32 flag);
// HYPOTHESIS: utArchiveManager's release of a resource id.
void fn_80045E64(utArchiveManager* manager, u32 resourceId);
// HYPOTHESIS: sndSystem::isSoundGroupLoaded (declared in snd/snd_system.h; the symbol is unnamed).
bool fn_80073D08(sndSystem* system, u32 groupId);

} // extern "C"

// HYPOTHESIS: the name (the instance pointer of the unit's .bss; getInstance only returns it).
emManager* g_emManager;

void emManager::create(u32 enemyEntryNum, u32 archiveEntryNum, u8 primidFaceType) {
    if (g_emManager != NULL) {
        return;
    }
    Entry* enemyEntries = new (Heaps::EnemyInstance) Entry[enemyEntryNum];
    if (enemyEntries == NULL) {
        return;
    }
    Entry* archiveEntries = new (Heaps::EnemyInstance) Entry[archiveEntryNum];
    if (archiveEntries == NULL) {
        delete[] enemyEntries;
        return;
    }
    // Room for the archives of every preloaded kind (two each) and one more.
    if (soArchiveDb::create(1, archiveEntryNum * 2 + 1) == false) {
        delete[] enemyEntries;
        delete[] archiveEntries;
        return;
    }
    g_emManager = new (Heaps::EnemyInstance)
        emManager(enemyEntryNum, enemyEntries, archiveEntryNum, archiveEntries, primidFaceType);
    if (g_emManager == NULL) {
        delete[] enemyEntries;
        delete[] archiveEntries;
        fn_27_6FDE0(1);
    }
}

void emManager::remove() {
    delete g_emManager;
    g_emManager = NULL;
}

emManager* emManager::getInstance() {
    return g_emManager;
}

emManager::emManager(u32 enemyEntryNum, Entry* enemyEntries, u32 archiveEntryNum, Entry* archiveEntries, u8 primidFaceType) :
    m_enemyEntryNum(enemyEntryNum), m_enemyEntries(enemyEntries) {
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        m_enemyEntries[i].clear();
    }
    m_nextEnemyCreateId = 0;
    m_archiveEntryNum = archiveEntryNum;
    m_archiveEntries = archiveEntries;
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        m_archiveEntries[i].clear();
    }
    m_nextArchiveCreateId = 0;
    m_primidFaceType = primidFaceType;
}

void emManager::Entry::clear() {
    m_enemy = NULL;
    m_isUsed = false;
    m_kind = static_cast<EnemyKind>(emInfo::getInstance()->getInvalidKind());
    m_parentCreateId = InvalidCreateId;
    m_createId = InvalidCreateId;
    m_soundHeapLevel = -2;
}

emManager::~emManager() {
    removeEnemyAll();
    removeArchiveAll();
    delete[] m_enemyEntries;
    delete[] m_archiveEntries;
    fn_27_6FDE0(1);
    m_enemyEntryNum = 0;
    m_enemyEntries = NULL;
    m_nextEnemyCreateId = 0;
    m_archiveEntryNum = 0;
    m_archiveEntries = NULL;
    m_nextArchiveCreateId = 0;
    m_primidFaceType = 0;
}

// Creates the enemy described by `create` from its kind's preloaded archive. Returns its create id, or -2 (invalid
// kind), -5 (archive missing or not loaded yet), -3 (no free entry), -4 (creation failed).
int emManager::createEnemy(emCreate* create) {
    // BYTES: declared up front, the register order of the original.
    u32 createId;
    EnemyKind kind;
    if (emInfo::getInstance()->isInvalidKind(create->m_enemyKind) == true) {
        return -2;
    }
    // The original calls it and drops the result.
    getPreloadArchiveCountFromKind(create->m_enemyKind);
    int archiveId = getPreloadArchiveCreateIdFromKind(create->m_enemyKind);
    if (archiveId < 0) {
        return -5;
    }
    if (isCompArchive(archiveId) == false) {
        return -5;
    }
    int index = getEntryEnemyEmpty();
    if (index < 0) {
        return -3;
    }
    Entry* entry = getEntryEnemyPtrFromIndex(index);
    if (entry == NULL) {
        return -3;
    }
    createId = issueEnemyCreateId();
    Enemy* enemy = Enemy::create(createId, create);
    if (enemy == NULL) {
        return -4;
    }
    kind = create->m_enemyKind;
    entry->clear();
    entry->m_enemy = enemy;
    entry->m_isUsed = true;
    entry->m_kind = kind;
    entry->m_parentCreateId = create->m_parentCreateId;
    entry->m_createId = createId;
    Enemy::createPost(enemy, create);
    return createId;
}

bool emManager::isChild(int parentCreateId, int childCreateId) {
    int childParentId = getInstance()->getEntryEnemyPtrFromId(childCreateId)->m_parentCreateId;
    return childParentId == parentCreateId;
}

bool emManager::resetParentEnemyCreateId(int parentCreateId, int childCreateId) {
    if (getInstance()->isChild(parentCreateId, childCreateId) == true) {
        Entry* entry = getInstance()->getEntryEnemyPtrFromId(childCreateId);
        if (entry == NULL) {
            return true;
        }
        entry->m_parentCreateId = InvalidCreateId;
    }
    return true;
}

void emManager::removeEnemy(int createId) {
    Entry* entry = getEntryEnemyPtrFromId(createId);
    if (entry != NULL && entry->m_enemy != NULL) {
        Enemy::remove(entry->m_enemy);
        doFuncOtherEnemy(createId, resetParentEnemyCreateId);
        entry->clear();
    }
}

void emManager::doFuncOtherEnemy(int createId, bool (*func)(int createId, int otherCreateId)) {
    if (createId == InvalidCreateId) {
        return;
    }
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        // HYPOTHESIS: the original skips by entry index, not by create id.
        if (i == createId) {
            continue;
        }
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed) {
            if (func(createId, entry->m_createId) == false) {
                return;
            }
        }
    }
}

void emManager::removeEnemyAll() {
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed == true) {
            removeEnemy(entry->m_createId);
        }
    }
}

emManager::Entry* emManager::getEntryEnemyPtrFromIndex(u32 index) {
    if (index >= m_enemyEntryNum) {
        return NULL;
    }
    return &m_enemyEntries[index];
}

emManager::Entry* emManager::getEntryEnemyPtrFromId(int createId) {
    if (createId == InvalidCreateId) {
        return NULL;
    }
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_createId == createId) {
            return entry;
        }
    }
    return NULL;
}

int emManager::getEntryEnemyEmpty() {
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && !entry->m_isUsed) {
            return i;
        }
    }
    return -1;
}

Enemy* emManager::getEnemyPtrFromId(int createId) {
    Entry* entry = getEntryEnemyPtrFromId(createId);
    if (entry == NULL) {
        return NULL;
    }
    // `== false`: `!entry->m_isUsed` inverts the branch.
    if (entry->m_isUsed == false) {
        return NULL;
    }
    return entry->m_enemy;
}

Enemy* emManager::getEnemyRefFromId(int createId) {
    return getEntryEnemyPtrFromId(createId)->m_enemy;
}

bool emManager::isEntryEnemyEmpty() {
    return getEntryEnemyEmpty() >= 0;
}

u32 emManager::getEnemyCount() {
    u32 count = 0;
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed) {
            count++;
        }
    }
    return count;
}

u32 emManager::getEnemyCountFromKind(EnemyKind enemyKind) {
    u32 count = 0;
    if (emInfo::getInstance()->isInvalidKind(enemyKind) == true) {
        return 0;
    }
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_enemy != NULL && enemyKind == entry->m_enemy->getEnemyKind()) {
            count++;
        }
    }
    return count;
}

int emManager::getEnemyParentCreateId(int createId) {
    if (createId == InvalidCreateId) {
        return InvalidCreateId;
    }
    Entry* entry = getEntryEnemyPtrFromId(createId);
    if (entry == NULL) {
        return InvalidCreateId;
    }
    if (entry->m_isUsed == false) {
        return InvalidCreateId;
    }
    return entry->m_parentCreateId;
}

bool emManager::isEnemyExist(int createId) {
    if (createId == InvalidCreateId) {
        return false;
    }
    Entry* entry = getEntryEnemyPtrFromId(createId);
    if (entry == NULL) {
        return false;
    }
    if (entry->m_isUsed == false) {
        return false;
    }
    return entry->m_enemy != NULL;
}

// The create id of the first enemy that has the area `areaId`.
int emManager::getCreateIdFromAreaId(int areaId) {
    for (u32 i = 0; i < m_enemyEntryNum; i++) {
        Entry* entry = getEntryEnemyPtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_enemy != NULL && fn_27_8CC68(entry->m_enemy, areaId) != -1) {
            return entry->m_createId;
        }
    }
    return InvalidCreateId;
}

void emManager::Entry::entryPreloadArchive(emArchive* archive, EnemyKind kind, u32 createId, int soundHeapLevel) {
    clear();
    m_archive = archive;
    m_isUsed = true;
    m_kind = kind;
    m_parentCreateId = InvalidCreateId;
    m_createId = createId;
    m_soundHeapLevel = soundHeapLevel;
}

// Registers the archives of one enemy kind and loads its sound group. Returns the archive's create id, or -2 (invalid
// kind), -1 (kind already preloaded), -3 (no free entry), -4 (allocation or sound group load failed).
int emManager::preloadArchive(gfArchive* param, gfArchive* brres, gfArchive* enmCommon, gfArchive* primFaceBrres,
                              EnemyKind enemyKind, bool isSoundRequest) {
    if (emInfo::getInstance()->isInvalidKind(enemyKind) == true) {
        return -2;
    }
    if (getPreloadArchiveCountFromKind(enemyKind) >= 1) {
        return -1;
    }
    int index = getEntryPreloadArchiveEmpty();
    if (index < 0) {
        return -3;
    }
    Entry* entry = getEntryPreloadArchivePtrFromIndex(index);
    if (entry == NULL) {
        return -3;
    }
    u32 createId = issueArchiveCreateId();
    emArchive* archive = new (Heaps::EnemyInstance) emArchive(param, brres, enmCommon, primFaceBrres, enemyKind);
    if (archive == NULL) {
        return -4;
    }
    u32 seId = emInfo::getInstance()->getSEId(enemyKind);
    int soundHeapLevel = -2;
    if (seId != -1) {
        soundHeapLevel = g_sndSystem->loadSoundGroup(static_cast<SndGroupID>(seId), 0xC, isSoundRequest);
        if (soundHeapLevel == -2) {
            return -4;
        }
    }
    entry->entryPreloadArchive(archive, enemyKind, createId, soundHeapLevel);
    return createId;
}

bool emManager::isCompArchive(int createId) {
    Entry* entry = getEntryPreloadArchivePtrFromId(createId);
    if (entry == NULL) {
        return false;
    }
    if (entry->m_archive == NULL) {
        return false;
    }
    if (entry->m_archive->checkLoadFiles() == false) {
        return false;
    }
    if (entry->m_soundHeapLevel >= 0) {
        u32 seId = emInfo::getInstance()->getSEId(entry->m_kind);
        if (seId != -1 && fn_80073D08(g_sndSystem, seId) == false) {
            return false;
        }
    }
    return true;
}

bool emManager::isCompArchiveAll() {
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && isCompArchive(entry->m_createId) == false) {
            return false;
        }
    }
    return true;
}

void emManager::removeArchive(int createId) {
    Entry* entry = getEntryPreloadArchivePtrFromId(createId);
    if (entry != NULL && entry->m_archive != NULL) {
        delete entry->m_archive;
        if (entry->m_soundHeapLevel >= 0) {
            g_sndSystem->freeGroup(entry->m_soundHeapLevel, 0xC);
        }
        entry->clear();
    }
}

void emManager::removeArchiveAll() {
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed == true) {
            removeArchive(entry->m_createId);
        }
    }
}

emManager::Entry* emManager::getEntryPreloadArchivePtrFromIndex(u32 index) {
    if (index >= m_archiveEntryNum) {
        return NULL;
    }
    return &m_archiveEntries[index];
}

emManager::Entry* emManager::getEntryPreloadArchivePtrFromId(int createId) {
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_createId == createId) {
            return entry;
        }
    }
    return NULL;
}

int emManager::getEntryPreloadArchiveEmpty() {
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && !entry->m_isUsed) {
            return i;
        }
    }
    return -1;
}

u32 emManager::getPreloadArchiveCountFromKind(EnemyKind enemyKind) {
    u32 count = 0;
    if (emInfo::getInstance()->isInvalidKind(enemyKind) == true) {
        return 0;
    }
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_archive != NULL && entry->m_kind == enemyKind) {
            count++;
        }
    }
    return count;
}

emArchive* emManager::getPreloadArchivePtrFromKind(EnemyKind enemyKind) {
    if (emInfo::getInstance()->isInvalidKind(enemyKind) == true) {
        return NULL;
    }
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_archive != NULL && entry->m_kind == enemyKind) {
            return entry->m_archive;
        }
    }
    return NULL;
}

int emManager::getPreloadArchiveCreateIdFromKind(EnemyKind enemyKind) {
    if (emInfo::getInstance()->isInvalidKind(enemyKind) == true) {
        return 0;
    }
    for (u32 i = 0; i < m_archiveEntryNum; i++) {
        Entry* entry = getEntryPreloadArchivePtrFromIndex(i);
        if (entry != NULL && entry->m_isUsed && entry->m_kind == enemyKind) {
            return entry->m_createId;
        }
    }
    return -5;
}

emArchive::emArchive(gfArchive* param, gfArchive* brres, gfArchive* enmCommon, gfArchive* primFaceBrres, EnemyKind kind) :
    enmCommonResourceId(InvalidResourceId),
    brresResourceId(InvalidResourceId),
    paramResourceId(InvalidResourceId),
    weaponResourceId(InvalidResourceId),
    faceTexBrresResourceId(InvalidResourceId),
    extendBrresResourceId(InvalidResourceId) {
    paramResourceId = fn_800455E0(soArchiveDb::getManager(1), param);
    brresResourceId = fn_800455E0(soArchiveDb::getManager(1), brres);
    enmCommonResourceId = fn_800455E0(soArchiveDb::getManager(1), enmCommon);
    weaponResourceId = faceTexBrresResourceId = brresResourceId;
    if (emInfo::getInstance()->isPrimKind(kind) == true) {
        extendBrresResourceId = fn_800455E0(soArchiveDb::getManager(1), primFaceBrres);
    }
}

emArchive::~emArchive() {
    unloadFiles();
}

bool emArchive::checkLoadFiles() {
    if (paramResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        if (fn_80045DC0(manager, paramResourceId, 0) == 0) {
            return false;
        }
    }
    if (brresResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        if (fn_80045DC0(manager, brresResourceId, 0) == 0) {
            return false;
        }
    }
    if (enmCommonResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        if (fn_80045DC0(manager, enmCommonResourceId, 0) == 0) {
            return false;
        }
    }
    if (extendBrresResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        if (fn_80045DC0(manager, extendBrresResourceId, 0) == 0) {
            return false;
        }
    }
    return true;
}

void emArchive::unloadFiles() {
    if (paramResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        fn_80045E64(manager, paramResourceId);
    }
    if (brresResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        fn_80045E64(manager, brresResourceId);
    }
    if (enmCommonResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        fn_80045E64(manager, enmCommonResourceId);
    }
    if (extendBrresResourceId != InvalidResourceId) {
        utArchiveManager* manager = soArchiveDb::getManager(1);
        fn_80045E64(manager, extendBrresResourceId);
    }
}

int emArchive::getWeaponResourceID(u32 id) {
    return weaponResourceId;
}
