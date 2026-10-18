#pragma once

// Local shadow of BrawlHeaders' it/it_manager.h: the original plus the two queries the fighter manager uses for Final Smash availability.

#include <StaticAssert.h>
#include <it/it_archive.h>
#include <it/it_customizer.h>
#include <it/it_gen_archive.h>
#include <it/item.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/so_dispose_instance_manager.h>
#include <types.h>
#include <bitset>

// HYPOTHESIS: the startup archive description is seven words (set once by setStartupArchive).
struct itItemStartupArchive {
    int words[7];
};

class itManager : public gfTask, public soDisposeInstanceEventObserver {
public:
    struct ItemSwitch {
        std::bitset<NUM_IT_KINDS> m_itemSwitch;

        ItemSwitch() : m_itemSwitch(0) { }
        ItemSwitch(bool val) : m_itemSwitch(val) { }
    };

    struct ItemInfo {
        u16 m_variation;
        itKind m_kind : 16;
    };

    char _76[116];
    u32 m_frameCount;
    int m_numItems;
    soArrayList<itArchive*, 128> m_itArchiveArrayList;
    bool m_drawDone;
    char _1749[3];
    soArrayList<BaseItem*, 64> m_itemArrayList;
    soArrayList<BaseItem*, 64> m_itemArrayList2;
    soArrayList<BaseItem*, 64> m_itemArrayList3;
    char _4080[25];
    u8 m_itemNums[NUM_IT_KINDS];
    char _4283[1];
    ItemInfo m_nextAssistInfo; // Note: Just uses itKind in vanilla
    int m_numAssists;
    int m_preloadAssistTimer;
    soArrayList<ItemInfo, 12> m_pokemonInfoArrayList; // Note: Just uses itKind in vanilla
    u32 m_nextPokemonIndex;
    int m_numPokemon;
    int m_numPokeballs;
    int m_preloadPokemonTimer;
    soArrayList<itGenArchive*, 3> m_genArchiveArrayList;
    ItemSwitch m_itemSwitch;
    float m_itemLotRates[NUM_IT_KINDS];
    int m_crateVariation;
    int m_smashBallDropTimer;
    char _5252[8];
    int m_smashBallDropFrame1;
    int m_smashBallDropFrame2;
    char _5268[28];
    itGenParam::Level m_genLevel;
    bool m_needsToProcess;
    char _5301[3];
    void* m_figureManager;
    char _5308[4];

    virtual void processDefault();
    virtual void processBegin();
    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void processDebug();
    virtual void renderDebug();
    virtual ~itManager();

    virtual void notifyDisposeInstance(bool, int, int taskId);
    virtual void notifyDrawDone();

    bool isCompItemKindArchive(itKind kind, u32 variation, bool);
    bool isCompItemKindArchivePreloadAll();
    float getRandBasicItemValue(int kind, ItemSwitch* itemSwitch);
    int getRandBasicItemVariation(int kind);
    bool isRemovedItemArchive(int archiveType);
    bool removeItemOld(int kind, int variation, u32 createOwnerTaskId, u32 genId);
    void removeItemArchive(int archiveType);
    itGenArchive* preloadItemGenArchive(itGenSheetKind sheet, int unk);
    const char* getGenArchiveLoadPathFromStepID(itGenSheetKind sheet);
    itArchive* getItemKindArchive(int kind, int variation);
    bool preloadPokemon(int kind);
    int getItemKindArchiveRefBrresItKind(int kind, bool unk);
    u8 getItemKindArchiveGroup(int kind, u8 group);
    void finish();
    void removeItemAllTempArchive();
    int removeRequestTrainingItem(int kind);
    // the item resource file that holds a common animation set (the mushroom size animations use it)
    nw4r::g3d::ResFile getResFileData(int set);
    static void setStartupArchive(itItemStartupArchive* archive);
    bool preloadItemKindArchiveFromGenArchive(itGenSheetKind sheet, int archiveType);
    void start(int mode);
    float getPerBase(itGenParam* param, ItemSwitch* itemSwitch);
    bool isCompItemGenArchive(itGenSheetKind sheet);
    bool isPassedProcessUpdate();
    bool isCompFigureTexPreloadAll();
    void startSuddenDeath();
    void initializeSmashBallFlags();
    void initializeDragoonFlags(int unk, bool isSuddenDeath);
    bool isExclusiveManaphy(bool checkPokemonList);
    BaseItem* getItemFromAreaId(int areaId);
    bool isCompItemKindArchiveFromGenArchive(itGenSheetKind sheet);
    itArchive* preloadItemKindArchive(itKind kind, u32 variation, itArchive::Type archiveType, bool);
    itGenSheetKind getRandBasicItemSheet();
    ItemKind getLotOneItemKind(itGenSheetKind* sheetKind, itGenId genKind, ItemSwitch* itemSwitch = NULL, bool unk = false);
    BaseItem* createItem(Vec3f* safePos, Vec3f* pos, float lr, itKind kind, u32 variation = 0, int createOwnerTaskId = -1,
                         soResourceModule* resourceModule = NULL, u8 groupNo = 0, int brresId = 0xffff,
                         int texResIndex = 0, int texResId = 0xffff);
    BaseItem* createItem(Vec3f* pos, float lr, itKind kind, u32 variation = 0, int createOwnerTaskId = -1,
                         soResourceModule* resourceModule = NULL, u8 groupNo = 0, int brresId = 0xffff,
                         int texResIndex = 0, int texResId = 0xffff);
    BaseItem* createItem(itKind kind, u32 variation = 0, int createOwnerTaskId = -1, soResourceModule* resourceModule = NULL,
                         u8 groupNo = 0, int brresId = 0xffff, int texResIndex = 0, int texResId = 0xffff);
    BaseItem* createBaseItem(Vec3f* safePos, Vec3f* pos, float lr, itKind kind, u32 variation, int createOwnerTaskId, int ownerTaskId, soResourceModule*, u8 groupNo, int brresId, int texResIndex, int texResId, int teamNo);
    BaseItem* createItemInstance(itCreate* create);
    int createMoney(char* unk1, Vec3f *pos, Vec2f *unk2, u32 amount, u32 unk4, u32 unk5);
    u32 getItemNum(itKind kind);
    u32 getItemNum(itKind kind, int variation, int taskId, int);
    BaseItem* getItemFromInstanceId(int instanceId);
    bool isCreatableItem(itKind kind, int variation); // Added locally: sora_melee 0x2A5B84 (st_heal)
    void removeItem(BaseItem*);
    // Removes all items with this creation-owner task ID; -1 leaves them alone.
    void removeItem1(int createOwnerTaskId);
    bool preloadAssist(itKind, int variation = 0); // custom parameter


    bool isExclusiveSpecialItem(int kind, bool unk);
    bool isItemSwitch(int itemKind); // is the item enabled in the item switch

    // Added locally (sora_melee 0x2A0D38 ... 0x2AB4D4): per-kind tables and small accessors.
    void setItemGenLevel(itGenParam::Level level);
    void preStartSettingSmashBallDropFrame(int frame1, int frame2);
    const char* getItemKindName(int kind);
    void setLotRate(int kind, float rate);
    float getLotRate(int kind);
    u8 isUseItemModel(int kind);
    int getItemGroup(int kind);
    int getItemVariationNum(int kind);
    BaseItem* getItemFromIndex(u32 index);
    bool isWeaponItem(int kind);

    BaseItem* getSafeItemFromIndex(u32 index);
    BaseItem* getItemFromKind(int kind);
    int getGenItemNum();
    int getItemShotNum();
    void removeItemAll();
    BaseItem* findLowPriorityItemInstance();

    int getItemKindArchiveId(int kind);
    void setExistDragoonParts(int part, int exist);
    int createManaphyHeart(Vec3f* pos, int createOwnerTaskId, int targetTaskId);

    int getPriorityItemInstance(BaseItem* item);

    void removeItemSub(BaseItem* item, int immediate, int returnToPool);

    float getLotValue(float base, int kind);

    ItemKind getLotKirbyNormalItemKind();
    ItemKind getLotKirbyTabemonoItemKind();

    static itManager* getInstance();
};
static_assert(sizeof(itManager) == 0x14c0, "Class is wrong size!");
