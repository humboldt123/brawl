#pragma once

// SHADOW of BrawlHeaders em/em_manager.h (-I include comes first): the full emManager of the Enemy unit's em_manager.cpp
// (map names), with the entry type and the layout typed from its code.

#include <StaticAssert.h>
#include <em/em_archive.h>
#include <em/enemy.h>
#include <types.h>

class gfArchive;

// The registry of the live enemies and of the archives preloaded for them. Each enemy and each preloaded archive gets a
// create id from its own counter; the entries are fixed arrays allocated by create().
class emManager {
public:
    // The create id of no enemy / no archive (and the id of a resource that is not loaded).
    static const u32 InvalidCreateId = 0xFFFF;

    // One slot of either array: an enemy (m_enemy) or a preloaded archive (m_archive). Not a template: the map names
    // its methods emManager::Entry::clear and emManager::Entry::entryPreloadArchive.
    class Entry {
    public:
        bool m_isUsed;          // 0x0
        EnemyKind m_kind;       // 0x4
        int m_parentCreateId;   // 0x8, emCreate::m_parentCreateId for an enemy; InvalidCreateId for an archive
        u32 m_createId;         // 0xC
        int m_soundHeapLevel;   // 0x10, the sndSystem::loadSoundGroup result of an archive (-2: no sound group)
        union {
            Enemy* m_enemy;
            emArchive* m_archive;
        };                      // 0x14

        void clear();
        void entryPreloadArchive(emArchive* archive, EnemyKind kind, u32 createId, int soundHeapLevel);
    };
    static_assert(sizeof(Entry) == 0x18, "Class is wrong size!");

    u32 m_enemyEntryNum;        // 0x0
    Entry* m_enemyEntries;      // 0x4
    u32 m_nextEnemyCreateId;    // 0x8
    u32 m_archiveEntryNum;      // 0xC
    Entry* m_archiveEntries;    // 0x10
    u32 m_nextArchiveCreateId;  // 0x14
    // HYPOTHESIS: the Primid face variant of the stage (BrawlHeaders name); nothing in this unit reads it.
    u8 m_primidFaceType;        // 0x18

    emManager(u32 enemyEntryNum, Entry* enemyEntries, u32 archiveEntryNum, Entry* archiveEntries, u8 primidFaceType);
    virtual ~emManager();       // vptr at 0x1C

    // Hands out the next create id of an enemy / of a preloaded archive.
    u32 issueEnemyCreateId() { return m_nextEnemyCreateId++; }
    u32 issueArchiveCreateId() { return m_nextArchiveCreateId++; }

    static void create(u32 enemyEntryNum, u32 archiveEntryNum, u8 primidFaceType);
    static void remove();
    static emManager* getInstance();

    int createEnemy(emCreate* create);
    // Whether the enemy childCreateId was created as a child of parentCreateId.
    // BYTES: never_inline, with -ipa file it would be inlined into resetParentEnemyCreateId.
    bool isChild(int parentCreateId, int childCreateId) __attribute__((never_inline));
    // doFuncOtherEnemy callback of removeEnemy: detaches the child childCreateId from its removed parent.
    static bool resetParentEnemyCreateId(int parentCreateId, int childCreateId);
    void removeEnemy(int createId);
    // Calls func(createId, other) for the create id of every other live enemy, until func returns false.
    void doFuncOtherEnemy(int createId, bool (*func)(int createId, int otherCreateId));
    void removeEnemyAll();
    Entry* getEntryEnemyPtrFromIndex(u32 index);
    Entry* getEntryEnemyPtrFromId(int createId);
    // The index of a free enemy entry, or -1.
    int getEntryEnemyEmpty();
    Enemy* getEnemyPtrFromId(int createId);
    // The enemy of an entry that is known to exist (no check).
    Enemy* getEnemyRefFromId(int createId);
    bool isEntryEnemyEmpty();
    u32 getEnemyCount();
    u32 getEnemyCountFromKind(EnemyKind enemyKind);
    int getEnemyParentCreateId(int createId);
    bool isEnemyExist(int createId);
    int getCreateIdFromAreaId(int areaId);

    int preloadArchive(gfArchive* param, gfArchive* brres, gfArchive* enmCommon, gfArchive* primFaceBrres, EnemyKind enemyKind, bool isSoundRequest);
    bool isCompArchive(int createId);
    bool isCompArchiveAll();
    void removeArchive(int createId);
    void removeArchiveAll();
    Entry* getEntryPreloadArchivePtrFromIndex(u32 index);
    Entry* getEntryPreloadArchivePtrFromId(int createId);
    // The index of a free archive entry, or -1.
    int getEntryPreloadArchiveEmpty();
    u32 getPreloadArchiveCountFromKind(EnemyKind enemyKind);
    emArchive* getPreloadArchivePtrFromKind(EnemyKind enemyKind);
    int getPreloadArchiveCreateIdFromKind(EnemyKind enemyKind);
};
static_assert(sizeof(emManager) == 0x20, "Class is wrong size!");
