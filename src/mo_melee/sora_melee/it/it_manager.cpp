#include <types.h>
#include <it/it_manager.h>
#include <gf/gf_task_scheduler.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <gm/gm_global_mode_melee.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/area/so_area_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {



// itManager::checkCreatableItem (0x2A5EE0, unnamed): whether the item kind may be created right now (match state, free
// instance slots, per-kind limits)
int fn_27_2A5EE0(itManager* self, int kind, u32 variation, bool unk);

// itArchive::isComp (0x2B23F8, unnamed): whether the archive finished loading
int fn_27_2B23F8(itArchive* archive);

// itManager::initializeSmashBallFlags (0x2A95B4, unnamed) / itManager::initializeDragoonFlags (0x2A97A4, unnamed)
// itFigureManager (0x2C1674, unnamed): whether all figure textures finished preloading
bool fn_27_2C1674(void* figureManager);

// itArchive::isRemovable (0x2B24C4, unnamed): whether the archive may be freed now (nothing uses it, load finished)
int fn_27_2B24C4(itArchive* archive);

// gm (main module): whether a Dragoon is already in play / mark that one has been used (unnamed)
int fn_800526E4();
void fn_80052694();

// itManager::findLowPriorityItemInstance (0x2A8054, unnamed): an item that may be recycled when the pool is full
// itManager::removeItemAfter (0x2A7114, unnamed): bookkeeping after an item left (kind counters, lottery state)
void fn_27_2A7114(itManager* self, int kind, int variation, int lotKind, int lotCreateKind);
// BaseItem::activate(itCreate*) (0x2829D0, unnamed)
void fn_27_2829D0(BaseItem* item, itCreate* create);
// rodata table: per kind lottery group (-3 and out-of-range kinds map to themselves)
extern int lbl_27_rodata_50D8[];
// per kind tables: item group (int), uses an item model (u8), name (char*), variation count (int, sign = flag), weapon
// item cache (0 unknown, 1 yes, 2 no), the weapon id lists (7 shorts per kind)
extern int lbl_27_data_71BF0[];
// the four item kinds a basic-gen lottery pulls in when the "all items" flag (0x14A0) is clear
extern u32 lbl_27_data_737A0[];
// per kind: the kind whose Brres this kind borrows (-1 none), may chain
extern int lbl_27_rodata_4B48[];
// per kind: archive group index (char)
extern u8 lbl_27_rodata_4A90[];
// the two generator sheets that are never picked as the random basic sheet (stage kind, level)
extern itGenSheetKind lbl_27_bss_6490;
extern itGenSheetKind lbl_27_bss_6498;
extern u8 lbl_27_data_71F6C[];
extern char* lbl_27_data_731C0[];
extern char lbl_27_data_73488[];
extern int lbl_27_rodata_47C8[];
extern int lbl_27_bss_64A0[];
// the all-clear base item switch of the Tabemono (food) lottery
extern u32 lbl_27_data_7419C;
extern u8 lbl_27_data_71EB8[];
extern s16 lbl_27_rodata_53A0[];

void fn_803F86E4(void* unk1, void* unk2, ...);
void fn_803FC984();
extern u8 __files[];
extern u8 lbl_27_data_7908C[];

// HYPOTHESIS: the item manager singleton (ftManager reads the same word as an "items enabled" flag).
extern itManager* g_ftItemEnabled;

} // extern "C"

bool itManager::isExclusiveSpecialItem(int unk1, bool unk2) {
    if ((u32)unk1 >= NUM_IT_KINDS) {
        fn_803F86E4(__files + 0xA0, lbl_27_data_7908C);
        fn_803FC984();
    }
    return (((u32*)&m_itemSwitch)[(u32)unk1 >> 5] & (1 << (unk1 & 31))) != 0;
}

BaseItem* itManager::createItem(Vec3f* safePos, Vec3f* pos, float lr, itKind kind, u32 variation, int createOwnerTaskId, soResourceModule* resourceModule, u8 groupNo, int brresId, int texResIndex, int texResId) {
    return createBaseItem(safePos, pos, lr, kind, variation, createOwnerTaskId, -1, resourceModule, groupNo, brresId, texResIndex, texResId, 20);
}

BaseItem* itManager::createItem(Vec3f* pos, float lr, itKind kind, u32 variation, int createOwnerTaskId, soResourceModule* resourceModule, u8 groupNo, int brresId, int texResIndex, int texResId) {
    return createBaseItem(pos, pos, lr, kind, variation, createOwnerTaskId, -1, resourceModule, groupNo, brresId, texResIndex, texResId, 20);
}

BaseItem* itManager::createItem(itKind kind, u32 variation, int createOwnerTaskId, soResourceModule* resourceModule, u8 groupNo, int brresId, int texResIndex, int texResId) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    return createBaseItem(&pos, &pos, 1.0f, kind, variation, createOwnerTaskId, -1, resourceModule, groupNo, brresId, texResIndex, texResId, 20);
}

void itManager::removeItem(BaseItem* item) {
    if (item != NULL) {
        for (s32 i = 0; i < m_itemArrayList.size(); i++) {
            if (m_itemArrayList.at(i) == item) {
                m_itemArrayList.erase(i);
                removeItemSub(item, 0, 1);
                return;
            }
        }
    }
}

void itManager::removeItem1(int createOwnerTaskId) {
    if ((u32)createOwnerTaskId != -1U) {
        for (s32 i = 0; i < m_itemArrayList.size(); i++) {
            BaseItem*& item = m_itemArrayList.at(i);
            if (item != NULL && (u32)createOwnerTaskId == (u32)item->m_createOwnerTaskId) {
                m_itemArrayList.erase(i);
                removeItemSub(item, 0, 1);
                i--;
            }
        }
    }
}

u32 itManager::getItemNum(itKind kind, int variation, int taskId, int genId) {
    u32 count = 0;
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        // MATCH-ONLY: the original calls the const at() overload on the member each time
        BaseItem* item = static_cast<const soArrayList<BaseItem*, 64>&>(m_itemArrayList).at(i);
        if (item != NULL && (((s32) kind == (s32) item->m_kind) || ((s32) kind == -1)) &&
            ((variation == 0) || (variation == (s32) item->m_variation)) &&
            (((u32) (taskId + 0x10000) == 0xFFFF) || ((u32) taskId == (u32) item->m_createOwnerTaskId)) &&
            (((u32) (genId + 0x10000) == 0xFFFF) || ((u32) genId == (u32) item->m_genId))) {
            count++;
        }
    }
    return count;
}

BaseItem* itManager::getItemFromInstanceId(int instanceId) {
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem* item = m_itemArrayList.at(i);
        if (item != NULL && (u32)instanceId == (u32)item->m_instanceId) {
            return item;
        }
    }
    return NULL;
}

itManager* itManager::getInstance() {
    return g_ftItemEnabled;
}

bool itManager::isCreatableItem(itKind kind, int variation) {
    return fn_27_2A5EE0(this, kind, variation, false) != 0;
}

u32 itManager::getItemNum(itKind kind) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (valid) {
        return m_itemNums[kind];
    }
    return 0;
}

// HYPOTHESIS: virtual slots of BaseItem past the declared ones (0x1F8: place the item, 0x258: finish before it is reused);
// the item vtable pointer sits at +0x3C, the first virtual at vtable offset 8.
#define IT_V4(n) virtual void n##0(); virtual void n##1(); virtual void n##2(); virtual void n##3();
#define IT_V16(n) IT_V4(n##a) IT_V4(n##b) IT_V4(n##c) IT_V4(n##d)
struct ItemSlotPad {
    u8 pad[0x3C];
};
struct ItemSlot1F8 : ItemSlotPad {
    IT_V16(a0) IT_V16(a1) IT_V16(a2) IT_V16(a3) IT_V16(a4) IT_V16(a5) IT_V16(a6) IT_V4(b0) IT_V4(b1) IT_V4(b2)
    virtual void unk1F8(Vec3f* pos, float lr, float unk);
};
struct ItemSlot258 : ItemSlotPad {
    IT_V16(a0) IT_V16(a1) IT_V16(a2) IT_V16(a3) IT_V16(a4) IT_V16(a5) IT_V16(a6) IT_V16(a7) IT_V16(a8) IT_V4(b0)
    virtual void unk258();
};

template <class T> static inline T& itManagerField(itManager* manager, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(manager) + offset);
}

BaseItem* itManager::createItemInstance(itCreate* create) {
    BaseItem* pooled;
    BaseItem* created;
    if (itManagerField<u8>(this, 0x14A2) != 0) {
        if (m_itemArrayList3.size() <= 0) {
            int kind, variation, group, lotKind;
            BaseItem* old = findLowPriorityItemInstance();
            if (old == NULL) {
                return NULL;
            }
            kind = old->m_kind;
            variation = old->m_variation;
            if (kind == -3) {
                group = kind;
            } else if (kind < 0 || kind >= 0xB2) {
                group = kind;
            } else {
                group = lbl_27_rodata_50D8[kind];
            }
            lotKind = (kind == 0x2A) ? old->getLotCreateItKind() : -1;
            old->reqLostEffect();
            if (kind == 0x37) {
                itManagerField<int>(this, 0x1480) = itManagerField<int>(this, 0x1490);
            } else if (kind == 0x14) {
                setExistDragoonParts(variation, 0);
            } else if (kind == 0x15) {
                u32 flags = itManagerField<u32>(this, 0x1494) & ~0x10;
                itManagerField<u32>(this, 0x1494) = flags;
                if (itManagerField<u8>(this, 0x149C) != 0) {
                    itManagerField<u32>(this, 0x1494) = flags | 1;
                }
            }
            reinterpret_cast<ItemSlot258*>(old)->unk258();
            fn_27_2A7114(this, kind, variation, group, lotKind);
            fn_27_2829D0(old, create);
            return old;
        }
        pooled = m_itemArrayList3.at(0);
        m_itemArrayList3.erase(0);
        fn_27_2829D0(pooled, create);
        m_itemArrayList.push(pooled);
        return pooled;
    }
    created = NULL;
    created = new (Heaps::ItemInstance) BaseItem(create);
    if (created == NULL) {
        return NULL;
    }
    created->init();
    created->activate(create->m_lr, 0.0f, create->m_pos, false);
    Vec3f* source = create->m_safePos;
    Vec2f safePos;
    float y = source->m_y;
    float x = source->m_x;
    safePos.m_x = x;
    safePos.m_y = y;
    created->setSafePos(&safePos);
    reinterpret_cast<ItemSlot1F8*>(created)->unk1F8(create->m_pos, create->m_lr, 0.0f);
    m_itemArrayList.push(created);
    return created;
}

void itManager::setItemGenLevel(itGenParam::Level level) {
    m_genLevel = level;
}

void itManager::processDefault() {
    m_needsToProcess = false;
}

void itManager::processUpdate() {
    m_needsToProcess = true;
}

void itManager::preStartSettingSmashBallDropFrame(int frame1, int frame2) {
    if (frame1 < 0) {
        frame1 = -1;
    }
    if (frame2 < 0) {
        frame2 = -1;
    }
    m_smashBallDropFrame1 = frame1;
    m_smashBallDropFrame2 = frame2;
}

const char* itManager::getItemKindName(int kind) {
    if (kind < 0 || kind >= NUM_IT_KINDS) {
        return lbl_27_data_73488;
    }
    return lbl_27_data_731C0[kind];
}

void itManager::setLotRate(int kind, float rate) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (!valid) {
        return;
    }
    m_itemLotRates[kind] = rate;
}

float itManager::getLotRate(int kind) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (valid) {
        return m_itemLotRates[kind];
    }
    return 0.0f;
}

u8 itManager::isUseItemModel(int kind) {
    if (kind == -3) {
        return 1;
    }
    if (kind < 0 || kind >= NUM_IT_KINDS) {
        return 0;
    }
    return lbl_27_data_71F6C[kind];
}

int itManager::getItemGroup(int kind) {
    if (kind == -3) {
        return 1;
    }
    if (kind < 0 || kind >= NUM_IT_KINDS) {
        return -1;
    }
    return lbl_27_data_71BF0[kind];
}

int itManager::getItemVariationNum(int kind) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (!valid) {
        return 0;
    }
    int count = lbl_27_rodata_47C8[kind];
    return count < 0 ? -count : count;
}

BaseItem* itManager::getItemFromIndex(u32 index) {
    if (index >= static_cast<u32>(m_itemArrayList.size())) {
        return NULL;
    }
    return m_itemArrayList.at(index);
}

bool itManager::isWeaponItem(int kind) {
    if (lbl_27_bss_64A0[kind] != 0) {
        return lbl_27_bss_64A0[kind] == 1;
    }
    s16* weapon = reinterpret_cast<s16*>(reinterpret_cast<char*>(lbl_27_rodata_53A0) + kind * 14);
    for (int i = 0; i < 7 && *weapon != -1; weapon++, i++) {
        if (*weapon == -2) {
            lbl_27_bss_64A0[kind] = 1;
            return true;
        }
    }
    lbl_27_bss_64A0[kind] = 2;
    return false;
}

void itManager::notifyDrawDone() {
    m_drawDone = true;
}

void itManager::processDebug() { }

void itManager::renderDebug() { }

void itManager::notifyDisposeInstance(bool, int, int taskId) { }

BaseItem* itManager::getSafeItemFromIndex(u32 index) {
    if (index >= static_cast<u32>(m_itemArrayList.size())) {
        return NULL;
    }
    BaseItem* item = m_itemArrayList.at(index);
    if (item == NULL || item->isSafe() == 0) {
        return NULL;
    }
    return item;
}

BaseItem* itManager::getItemFromKind(int kind) {
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem* item = m_itemArrayList.at(i);
        if (item != NULL && kind == item->m_kind) {
            return item;
        }
    }
    return NULL;
}

int itManager::getGenItemNum() {
    int count = 0;
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem* item = static_cast<const soArrayList<BaseItem*, 64>&>(m_itemArrayList).at(i);
        if (item != NULL && (u32)item->m_genId != 0xFFFFFFFF) {
            count++;
        }
    }
    return count;
}

int itManager::getItemShotNum() {
    int count = 0;
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem* item = static_cast<const soArrayList<BaseItem*, 64>&>(m_itemArrayList).at(i);
        if (item != NULL && isWeaponItem(item->m_kind)) {
            count++;
        }
    }
    return count;
}

void itManager::removeItemAll() {
    for (s32 i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem*& item = m_itemArrayList.at(i);
        if (item != NULL) {
            m_itemArrayList.erase(i);
            removeItemSub(item, 0, 1);
            i--;
        }
    }
    m_itemArrayList.clear();
}

BaseItem* itManager::findLowPriorityItemInstance() {
    BaseItem* best = NULL;
    int bestPriority = 10000;
    int count = m_itemArrayList.size();
    for (int i = 0; i < count; i++) {
        BaseItem* item = static_cast<const soArrayList<BaseItem*, 64>&>(m_itemArrayList).at(i);
        int priority = getPriorityItemInstance(item);
        if (priority < 10000) {
            if (priority == bestPriority) {
                if (best == NULL || (u32)best->m_instanceId > (u32)item->m_instanceId) {
                    bestPriority = priority;
                    best = item;
                }
            } else if (priority < bestPriority) {
                bestPriority = priority;
                best = item;
            }
        }
    }
    return best;
}

int itManager::getItemKindArchiveId(int kind) {
    if (itManagerField<u8>(this, 0x14A0) != 0) {
        if (kind == -3) {
            return -1;
        }
        u8 usesArchive;
        if (kind < 0 || kind >= NUM_IT_KINDS) {
            usesArchive = 0;
        } else {
            usesArchive = lbl_27_data_71EB8[kind];
        }
        if (usesArchive == 0) {
            return -1;
        }
        int group = getItemGroup(kind);
        if (group < 6) {
            if (group == 1) {
                return kind;
            }
        } else if (group < 8) {
            return kind;
        }
    }
    return -1;
}

void itManager::setExistDragoonParts(int part, int exist) {
    u32& flags = itManagerField<u32>(this, 0x1494);
    if (exist != 0) {
        switch (part) {
        case 0:
            flags |= 2;
            break;
        case 1:
            flags |= 4;
            break;
        case 2:
            flags |= 8;
            break;
        }
    } else {
        switch (part) {
        case 0:
            flags &= ~2;
            break;
        case 1:
            flags &= ~4;
            break;
        case 2:
            flags &= ~8;
            break;
        }
    }
}

int itManager::createManaphyHeart(Vec3f* pos, int createOwnerTaskId, int targetTaskId) {
    BaseItem* item = createBaseItem(pos, pos, 1.0f, static_cast<itKind>(0x23), 0, createOwnerTaskId, -1, NULL, 0, 0xFFFF, 0, 0xFFFF, 20);
    int result;
    if (item != NULL) {
        item->setTarget(targetTaskId);
        result = item->m_instanceId;
    } else {
        result = 0;
    }
    return result;
}

// How important an item is to keep when the instance pool is full: lower values are recycled first, 10000 and above never.
int itManager::getPriorityItemInstance(BaseItem* item) {
    int priority;
    if (item == NULL) {
        return -1;
    }
    priority = 0;
    if (item->isHaved() != 0) {
        return 10000;
    }
    if (item->isVanishMode() == 0) {
        return 10000;
    }
    int kind = item->m_kind;
    int group = getItemGroup(kind);
    switch (group) {
    case 0:
    case 5:
        return 10000;
    case 2:
    case 3: {
        int mapped;
        if (kind == -3) {
            mapped = kind;
        } else if (kind < 0 || kind >= 0xB2) {
            mapped = kind;
        } else {
            mapped = lbl_27_rodata_50D8[kind];
        }
        if (mapped == kind) {
            return 10000;
        }
        priority = 5000;
        break;
    }
    case 4:
    case 6:
    case 7:
        return 10000;
    case 8:
        priority = 4000;
        break;
    default:
        if (kind == 0x37) {
            return 10000;
        }
        if (kind == 0x17 || kind == 0x33 || kind == 0xB) {
            priority = 1000;
        }
        // HYPOTHESIS: the situation module sits in slot 5 of the item's module enumeration (item + 0x1A4).
        soSituationModule* situation = *reinterpret_cast<soSituationModule**>(*reinterpret_cast<char**>(reinterpret_cast<char*>(item) + 0x1A4) + 0x14);
        if (situation->getKind() == 2) {
            priority += 1000;
        }
        if (isWeaponItem(kind)) {
            priority += 1000;
        }
        int param = item->getParamInt(0x10000009);
        if (param < 0 || param > 1000) {
            priority += 1000;
        } else {
            priority += param;
        }
        break;
    }
    return priority;
}

// HYPOTHESIS: the match phase of the task scheduler is a 9-bit field behind its first byte.
struct gfTaskSchedulerView {
    s32 unk0 : 8;
    s32 phase : 9;
};

// Takes an item out of play. Items normally go back to the pool for reuse (returnToPool) unless the match is in a phase
// where they are destroyed; immediate skips the bookkeeping and exits the task right away.
void itManager::removeItemSub(BaseItem* item, int immediate, int returnToPool) {
    BaseItem* removed;
    if (item == NULL) {
        return;
    }
    removed = item;
    int kind = item->m_kind;
    int variation = item->m_variation;
    int group;
    if (kind == -3) {
        group = kind;
    } else if (kind < 0 || kind >= 0xB2) {
        group = kind;
    } else {
        group = lbl_27_rodata_50D8[kind];
    }
    int lotKind;
    if (kind == 0x2A) {
        lotKind = item->getLotCreateItKind();
    } else {
        lotKind = -1;
    }
    bool destroy = false;
    if (itManagerField<u8>(this, 0x14B5) != 0 || immediate != 0) {
        destroy = true;
    }
    if (returnToPool != 0) {
        if (kind == 0x37) {
            itManagerField<int>(this, 0x1480) = itManagerField<int>(this, 0x1490);
        } else if (kind == 0x14) {
            setExistDragoonParts(variation, 0);
        } else if (kind == 0x15) {
            u32 flags = itManagerField<u32>(this, 0x1494) & ~0x10;
            itManagerField<u32>(this, 0x1494) = flags;
            if (itManagerField<u8>(this, 0x149C) != 0) {
                itManagerField<u32>(this, 0x1494) = flags | 1;
            }
        }
        if (!destroy) {
            if (removed->isUseOuterRes() != 0 || getItemGroup(kind) == 6) {
                gfTaskSchedulerView* scheduler = reinterpret_cast<gfTaskSchedulerView*>(gfTaskScheduler::getInstance());
                if (scheduler != NULL) {
                    int phase = scheduler->phase;
                    if (phase < 2 || phase > 5) {
                        destroy = true;
                    }
                }
            }
        }
    }
    if (!destroy) {
        m_itemArrayList2.push(removed);
    } else {
        reinterpret_cast<ItemSlot258*>(removed)->unk258();
        if (immediate == 0 && itManagerField<u8>(this, 0x14A2) != 0) {
            m_itemArrayList3.push(removed);
        } else {
            removed->exit();
        }
        if (immediate == 0) {
            fn_27_2A7114(this, kind, variation, group, lotKind);
        }
    }
}

// The lottery weight of an item kind: the base weight plus the per-kind lot rate. During a versus match the weight of some
// kinds is cut to zero once the field already holds too many items of the same group (the counters at 0xFF8..0x1000).
float itManager::getLotValue(float base, int kind) {
    if (itManagerField<int>(this, 0x50) == 2 && itManagerField<u8>(this, 0x1008) != 0) {
        bool valid = false;
        if (kind > -1 && kind < NUM_IT_KINDS) {
            valid = true;
        }
        if (valid && kind != 0x46) {
            int category;
            switch (kind) {
            case 0xB:
            case 0x17:
            case 0x33:
                category = 1;
                break;
            default: {
                int group;
                if (kind == -3) {
                    group = 1;
                } else if (kind < 0 || kind >= NUM_IT_KINDS) {
                    group = -1;
                } else {
                    group = lbl_27_data_71BF0[kind];
                }
                switch (group) {
                case 0:
                case 1:
                case 8:
                    category = 0;
                    break;
                case 5:
                    category = 2;
                    break;
                default:
                    category = 3;
                    break;
                }
                break;
            }
            }
            if (category == 1 || category == 0) {
                if (category == 0) {
                    if (5 < itManagerField<int>(this, 0xFF8) + itManagerField<int>(this, 0x1000)) {
                        return 0.0f;
                    }
                }
                if (8 < itManagerField<int>(this, 0xFFC) + itManagerField<int>(this, 0x1000) + itManagerField<int>(this, 0xFF8)) {
                    return 0.0f;
                }
            }
        }
    }
    float result = 0.0f;
    if (0.0f < base) {
        bool valid = false;
        if (kind > -1 && kind < NUM_IT_KINDS) {
            valid = true;
        }
        float rate;
        if (valid) {
            rate = m_itemLotRates[kind];
        } else {
            rate = 0.0f;
        }
        result = base + rate;
    }
    if (kind == 0x37) {
        result = result * itManagerField<float>(this, 0x1484);
    }
    return result;
}

// The item Kirby gets from a Pokemon-less basic lottery: the item switch minus the kinds Kirby cannot copy.
ItemKind itManager::getLotKirbyNormalItemKind() {
    u32 words[6];
    words[1] = itManagerField<u32>(this, 0x11A0);
    words[1] &= ~0x800;
    words[1] &= ~0x80000;
    words[3] = itManagerField<u32>(this, 0x11A8);
    words[0] = itManagerField<u32>(this, 0x119C);
    words[0] &= ~0x800;
    words[0] &= ~0x800000;
    words[4] = itManagerField<u32>(this, 0x11AC);
    words[2] = itManagerField<u32>(this, 0x11A4) & ~1;
    words[5] = itManagerField<u32>(this, 0x11B0);
    itGenSheetKind sheet = getRandBasicItemSheet();
    itGenSheetKind sheets[3] = { sheet, sheet, sheet };
    return getLotOneItemKind(sheets, static_cast<itGenId>(10000), reinterpret_cast<ItemSwitch*>(words), false);
}

// The food item Kirby gets for his Tabemono (eat) lottery: only the food kinds the item switch allows.
ItemKind itManager::getLotKirbyTabemonoItemKind() {
    u32 words[6];
    words[0] = lbl_27_data_7419C;
    words[1] = lbl_27_data_7419C;
    words[2] = lbl_27_data_7419C;
    words[3] = lbl_27_data_7419C;
    words[4] = lbl_27_data_7419C;
    words[5] = lbl_27_data_7419C;
    if ((itManagerField<u32>(this, 0x11A4) & 1) != 0) {
        words[2] = lbl_27_data_7419C | 1;
    }
    if ((itManagerField<u32>(this, 0x11A0) & 1) != 0) {
        words[1] = lbl_27_data_7419C | 1;
    }
    if ((itManagerField<u32>(this, 0x119C) & 0x10000) != 0) {
        words[0] = lbl_27_data_7419C | 0x10000;
    }
    if ((itManagerField<u32>(this, 0x11A0) & 0x10) != 0) {
        words[1] = words[1] | 0x10;
    }
    itGenSheetKind sheet = getRandBasicItemSheet();
    itGenSheetKind sheets[3] = { sheet, sheet, sheet };
    return getLotOneItemKind(sheets, static_cast<itGenId>(10000), reinterpret_cast<ItemSwitch*>(words), false);
}

static inline bool isSameSheetStage(itGenArchive* cur, const itGenSheetKind& sheet) {
    itGenSheetKind kind = cur->m_sheetKind;
    return sheet.m_stageKind == kind.m_stageKind;
}

static inline bool isSameSheetLevel(itGenArchive* cur, const itGenSheetKind& sheet) {
    itGenSheetKind kind = cur->m_sheetKind;
    return (u32)sheet.m_levelId == (u32)kind.m_levelId;
}

static inline itGenArchive* findGenArchive(const soArrayFixed<itGenArchive*>& list, const itGenSheetKind& sheet) {
    itGenArchive* cur;
    for (int i = 0; i < list.size(); i++) {
        cur = list.at(i);
        bool stageMatches = false;
        bool found = false;
        if (cur != NULL && isSameSheetStage(cur, sheet)) {
            stageMatches = true;
        }
        if (stageMatches && isSameSheetLevel(cur, sheet)) {
            found = true;
        }
        if (found) {
            return cur;
        }
    }
    return NULL;
}

// Whether every item archive referenced by the generator sheet's parameters has finished loading. The result is cached
// in the sheet archive once everything is complete.
bool itManager::isCompItemKindArchiveFromGenArchive(itGenSheetKind sheet) {
    itGenSheetKind target = sheet;
    itGenArchive* archive = findGenArchive(m_genArchiveArrayList, target);
    if (archive == NULL) {
        return false;
    }
    if (!archive->isComp()) {
        return false;
    }
    if (archive->m_itemArchivesChecked) {
        return true;
    }
    if (archive->isFailed()) {
        return true;
    }
    for (u32 level = 0; level < 5; level++) {
        u32 numParams = archive->getGenParamNum(level);
        for (u32 p = 0; p < numParams; p++) {
            itGenParam* param = archive->getGenParamFromIndex(p, level);
            if (param == NULL) {
                continue;
            }
            if ((u32)param->m_id == (u32)Item_Gen_Basic && itManagerField<u8>(this, 0x14A0) == 0) {
                continue;
            }
            u32 off = 0;
            u32 e = 0;
            for (; e < param->m_numEntries; off += 0x10, e++) {
                itGenParam::Entry* entry = (itGenParam::Entry*)((u8*)param->m_entries + off);
                int kind = entry->m_kind;
                if (kind == -1) {
                    break;
                }
                switch (kind) {
                case 0x7D1:
                    kind = 0x40;
                    break;
                case 0x7D2:
                case 0x7D3:
                    if (itManagerField<u8>(this, 0x14A0) != 0) {
                        if (!isCompItemKindArchive((itKind)0x3E, 5000, true)) {
                            return false;
                        }
                    } else {
                        for (u32 j = 0; j < 4; j++) {
                            if (!isCompItemKindArchive((itKind)lbl_27_data_737A0[j], 5000, true)) {
                                return false;
                            }
                        }
                    }
                    continue;
                case 0x7D4:
                case 0x7D5:
                    continue;
                }
                int group;
                if (kind == -3) {
                    group = 1;
                } else if (kind < 0 || kind >= 0xB2) {
                    group = -1;
                } else {
                    group = lbl_27_data_71BF0[kind];
                }
                switch (group) {
                case 5:
                    break;
                case 2:
                case 3:
                case 4:
                case 6:
                case 7:
                    continue;
                }
                if (kind == 0x46) {
                    int variation = entry->m_variation;
                    if (variation != 5000 && !isCompItemKindArchive((itKind)kind, variation, true)) {
                        return false;
                    }
                } else if (!isCompItemKindArchive((itKind)kind, 5000, true)) {
                    return false;
                }
            }
        }
    }
    archive->m_itemArchivesChecked = true;
    return true;
}

// Whether every preloaded item archive has finished loading; the answer is cached once it turns true.
bool itManager::isCompItemKindArchivePreloadAll() {
    if (itManagerField<u8>(this, 0x14AC) != 0) {
        return true;
    }
    itManagerField<u8>(this, 0x14AC) = 0;
    for (int i = 0; i < m_itArchiveArrayList.size(); i++) {
        itArchive* archive = m_itArchiveArrayList.at(i);
        if (archive == NULL || !fn_27_2B23F8(archive)) {
            return false;
        }
    }
    itManagerField<u8>(this, 0x14AC) = 1;
    return true;
}

// The live item that owns the given area id (its area module reports a valid index for it), or NULL.
BaseItem* itManager::getItemFromAreaId(int areaId) {
    for (int i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem*& item = m_itemArrayList.at(i);
        if (item != NULL && item->m_moduleAccesser->getAreaModule().getAreaIndexFromId(areaId) >= 0) {
            return item;
        }
    }
    return NULL;
}

// Whether Manaphy's heart item must be exclusive right now: a Manaphy is already queued for the Pokeball pool
// (checked only when requested), or a Heart Swap / technique-practice process is running.
bool itManager::isExclusiveManaphy(bool checkPokemonList) {
    if (checkPokemonList) {
        for (int i = 0; i < static_cast<const soArrayList<ItemInfo, 12>&>(m_pokemonInfoArrayList).size(); i++) {
            // HYPOTHESIS: the queue holds bare itKind words in vanilla, so compare the whole word
            if (*(int*)&static_cast<const soArrayList<ItemInfo, 12>&>(m_pokemonInfoArrayList).at(i) == 0x78) {
                return true;
            }
        }
    }
    int heartSwapActive = itManagerField<u8>(this, 0x1081);
    if (heartSwapActive > 0) {
        return true;
    }
    if (g_ftManager->isProcessHeartSwap()) {
        return true;
    }
    int techniqueActive = itManagerField<u8>(this, 0x1040);
    if (techniqueActive > 0 || g_ftManager->isProcessTechnique()) {
        return true;
    }
    return false;
}

// Whether the generator sheet's archive has finished loading (false if it was never requested).
bool itManager::isCompItemGenArchive(itGenSheetKind sheet) {
    itGenSheetKind target = sheet;
    itGenArchive* archive = findGenArchive(m_genArchiveArrayList, target);
    if (archive == NULL) {
        return false;
    }
    return archive->isComp();
}

// Sudden death re-arms the Smash Ball and the Dragoon parts.
void itManager::startSuddenDeath() {
    itManagerField<u8>(this, 0xBC) = 0;
    initializeSmashBallFlags();
    initializeDragoonFlags(0, true);
}

// HYPOTHESIS: the scheduler's first word packs two signed bytes; the second is the process phase counter.
struct gfTaskSchedulerHead {
    s32 unk0_1 : 8;
    s32 phase : 8;
    u32 unk2 : 16;
};

// Whether the scheduler has run past its first update passes since the manager asked to process.
bool itManager::isPassedProcessUpdate() {
    bool passed = false;
    if (itManagerField<u8>(this, 0x14B4) != 0 && ((gfTaskSchedulerHead*)gfTaskScheduler::getInstance())->phase >= 3) {
        passed = true;
    }
    return passed;
}

bool itManager::isCompFigureTexPreloadAll() {
    u32 state = itManagerField<u32>(this, 0x50) - 2;
    if (state <= 1) {
        return fn_27_2C1674(m_figureManager);
    }
    return true;
}

static inline bool isSameSheetKind(itGenSheetKind a, const itGenSheetKind& b) {
    return a.m_stageKind == b.m_stageKind && (u32)a.m_levelId == (u32)b.m_levelId;
}

static inline bool isSameSheetKindRef(const itGenSheetKind& a, const itGenSheetKind& b) {
    return a.m_stageKind == b.m_stageKind && (u32)a.m_levelId == (u32)b.m_levelId;
}

// Picks a stage's generator sheet at random from the loaded ones, skipping the two defaults, whose basic-item
// parameters exist; falls back to the default sheet.
itGenSheetKind itManager::getRandBasicItemSheet() {
    itGenSheetKind result = lbl_27_bss_6490;
    int level = m_genLevel;
    if (itManagerField<int>(this, 0x50) == 2) {
        result = lbl_27_bss_6498;
    } else {
        level = 0;
    }
    if (static_cast<const soArrayList<itGenArchive*, 3>&>(m_genArchiveArrayList).size() > 1) {
        for (int i = 0; i < static_cast<const soArrayList<itGenArchive*, 3>&>(m_genArchiveArrayList).size(); i++) {
            itGenArchive* archive = static_cast<const soArrayList<itGenArchive*, 3>&>(m_genArchiveArrayList).at(i);
            if (archive != NULL && archive->isComp()) {
                itGenSheetKind kind = archive->m_sheetKind;
                itGenSheetKind first = archive->m_sheetKind;
                if (!(first.m_stageKind == lbl_27_bss_6490.m_stageKind && (u32)first.m_levelId == (u32)lbl_27_bss_6490.m_levelId) &&
                    !(kind.m_stageKind == lbl_27_bss_6498.m_stageKind && (u32)kind.m_levelId == (u32)lbl_27_bss_6498.m_levelId) && archive->getGenParam(10000, level) != NULL) {
                    result = kind;
                    break;
                }
            }
        }
    }
    return result;
}

// Sum of the lottery values of a parameter's entries (the weight base for the percentage split); stops at the first
// terminator entry. Entries whose kind is switched off in the item switch are skipped.
float itManager::getPerBase(itGenParam* param, ItemSwitch* itemSwitch) {
    float total = 0.0f;
    u32 off = 0;
    for (u32 i = 0; i < param->m_numEntries; off += 0x10, i++) {
        itGenParam::Entry* entry = (itGenParam::Entry*)((u8*)param->m_entries + off);
        int kind = entry->m_kind;
        if (kind == -1) {
            break;
        }
        if (kind == 0x7D1) {
            kind = 0x40;
        }
        bool valid = false;
        if (kind >= 0 && kind < NUM_IT_KINDS) {
            valid = true;
        }
        if (valid) {
            if ((u32)kind >= NUM_IT_KINDS) {
                fn_803F86E4(__files + 0xA0, lbl_27_data_7908C);
                fn_803FC984();
            }
            if ((((u32*)itemSwitch)[(u32)kind >> 5] & (1 << (kind & 31))) == 0) {
                continue;
            }
        }
        total += getLotValue(entry->m_rate, kind);
    }
    return total;
}

// The lottery value of one kind in the random sheet's basic-item parameter (0 when the kind is not listed or is
// switched off).
float itManager::getRandBasicItemValue(int kind, ItemSwitch* itemSwitch) {
    itGenSheetKind sheet = getRandBasicItemSheet();
    itGenSheetKind target = sheet;
    itGenArchive* archive = findGenArchive(m_genArchiveArrayList, target);
    itGenParam* param;
    if (archive == NULL) {
        param = NULL;
    } else {
        int level = m_genLevel;
        if (itManagerField<int>(this, 0x50) != 2) {
            level = 0;
        }
        param = archive->getGenParam(10000, level);
        if (param == NULL) {
            param = archive->getGenParam(10000, 0);
            if (param == NULL) {
                param = NULL;
            }
        }
    }
    if (param != NULL) {
        u32 count = param->m_numEntries;
        u32 off = 0;
        for (u32 i = 0; i < count; i++, off += 0x10) {
            itGenParam::Entry* entry = (itGenParam::Entry*)((u8*)param->m_entries + off);
            int entryKind = entry->m_kind;
            if (entryKind == -1) {
                return 0.0f;
            }
            if (entryKind == 0x7D1) {
                entryKind = 0x40;
            }
            if (entryKind == kind) {
                bool valid = false;
                if (entryKind >= 0 && entryKind < NUM_IT_KINDS) {
                    valid = true;
                }
                if (valid) {
                    if ((u32)entryKind >= NUM_IT_KINDS) {
                        fn_803F86E4(__files + 0xA0, lbl_27_data_7908C);
                        fn_803FC984();
                    }
                    if ((((u32*)itemSwitch)[(u32)entryKind >> 5] & (1 << (entryKind & 31))) == 0) {
                        continue;
                    }
                }
                return getLotValue(entry->m_rate, entryKind);
            }
        }
    }
    return 0.0f;
}

// The variation the random sheet's basic-item parameter lists for a kind (0 when the kind is not listed).
int itManager::getRandBasicItemVariation(int kind) {
    itGenSheetKind sheet = getRandBasicItemSheet();
    itGenSheetKind target = sheet;
    itGenArchive* archive = findGenArchive(m_genArchiveArrayList, target);
    itGenParam* param;
    if (archive == NULL) {
        param = NULL;
    } else {
        int level = m_genLevel;
        if (itManagerField<int>(this, 0x50) != 2) {
            level = 0;
        }
        param = archive->getGenParam(10000, level);
        if (param == NULL) {
            param = archive->getGenParam(10000, 0);
            if (param == NULL) {
                param = NULL;
            }
        }
    }
    if (param != NULL) {
        u32 off = 0;
        for (u32 remaining = param->m_numEntries; remaining > 0; remaining--) {
            int entryKind = *(int*)((u8*)param->m_entries + off);
            if (entryKind == -1) {
                return 0;
            }
            if (entryKind == 0x7D1) {
                entryKind = 0x40;
            }
            if (entryKind == kind) {
                return *(int*)((u8*)param->m_entries + off + 4);
            }
            off += 0x10;
        }
    }
    return 0;
}

// Whether no loaded item archive or generator archive of the given type is left (true once the type is fully removed).
bool itManager::isRemovedItemArchive(int archiveType) {
    for (int i = 0; i < m_itArchiveArrayList.size(); i++) {
        itArchive*& archive = m_itArchiveArrayList.at(i);
        if (archive != NULL && archiveType == archive->m_type) {
            return false;
        }
    }
    for (int i = 0; i < m_genArchiveArrayList.size(); i++) {
        itGenArchive*& archive = m_genArchiveArrayList.at(i);
        if (archive != NULL && archiveType == archive->m_itArchiveType) {
            return false;
        }
    }
    return true;
}

// Removes the most recently created live item that matches the filters (kind -1, variation 0, owner task -1 and
// generator id -1 mean "any"); returns whether one was removed.
bool itManager::removeItemOld(int kind, int variation, u32 createOwnerTaskId, u32 genId) {
    BaseItem* target = NULL;
    int i;
    for (i = 0; i < m_itemArrayList.size(); i++) {
        BaseItem*& item = m_itemArrayList.at(i);
        if (item != NULL && (kind == item->m_kind || kind == -1) && (variation == 0 || variation == (int)item->m_variation) &&
            (createOwnerTaskId == 0xFFFFFFFF || createOwnerTaskId == item->m_createOwnerTaskId) &&
            (genId == 0xFFFFFFFF || genId == item->m_genId) &&
            (target == NULL || (u32)target->m_instanceId < (u32)item->m_instanceId)) {
            target = item;
        }
    }
    if (target == NULL) {
        return false;
    }
    if (target != NULL) {
        for (i = 0; i < m_itemArrayList.size(); i++) {
            if (m_itemArrayList.at(i) == target) {
                m_itemArrayList.erase(i);
                removeItemSub(target, 0, 1);
                break;
            }
        }
    }
    return true;
}

// Frees every item archive and generator archive of the given type; archives still in use are only flagged for
// removal. Archives of other types lose their "checked" marks, and the preload-complete cache is cleared.
void itManager::removeItemArchive(int archiveType) {
    for (int i = 0; i < m_itArchiveArrayList.size(); i++) {
        itArchive*& archive = m_itArchiveArrayList.at(i);
        if (archive != NULL && archiveType == archive->m_type) {
            if (fn_27_2B24C4(archive)) {
                delete archive;
                m_itArchiveArrayList.erase(i);
                i--;
            } else {
                archive->m_4 = 1;
            }
        }
    }
    for (int i = 0; i < m_genArchiveArrayList.size(); i++) {
        itGenArchive*& archive = m_genArchiveArrayList.at(i);
        if (archive != NULL) {
            if (archiveType != archive->m_itArchiveType) {
                archive->m_17 = 0;
                archive->m_itemArchivesChecked = 0;
            } else {
                if (archive->isRemovable()) {
                    delete archive;
                    m_genArchiveArrayList.erase(i);
                    i--;
                } else {
                    archive->m_16 = 1;
                }
            }
        }
    }
    itManagerField<u8>(this, 0x14AC) = 0;
}

// Starts loading the generator archive for a stage sheet (unless the manager is in a state that skips it) and returns
// it; an archive that is already queued is returned as is unless it has been flagged for removal.
itGenArchive* itManager::preloadItemGenArchive(itGenSheetKind sheet, int unk) {
    itGenArchive* archive;
    if ((u32)(itManagerField<int>(this, 0x4C) - 3) <= 1) {
        archive = NULL;
    } else {
        itGenSheetKind target = sheet;
        archive = findGenArchive(m_genArchiveArrayList, target);
        if (archive == NULL) {
            itGenArchive* created = new itGenArchive(sheet, getGenArchiveLoadPathFromStepID(sheet), unk);
            if (created == NULL) {
                archive = NULL;
            } else {
                archive = created;
                m_genArchiveArrayList.push(archive);
            }
        } else if (archive->m_16 != 0) {
            archive = NULL;
        }
    }
    return archive;
}

// getItemKindArchiveId as a file-local inline (the original inlines it into its callers)
static inline int archiveIdOf(itManager* manager, int kind) {
    if (itManagerField<u8>(manager, 0x14A0) != 0) {
        if (kind == -3) {
            return -1;
        }
        u8 usesArchive;
        if (kind < 0 || kind >= NUM_IT_KINDS) {
            usesArchive = 0;
        } else {
            usesArchive = lbl_27_data_71EB8[kind];
        }
        if (usesArchive == 0) {
            return -1;
        }
        int group = manager->getItemGroup(kind);
        if (group < 6) {
            if (group == 1) {
                return kind;
            }
        } else if (group < 8) {
            return kind;
        }
    }
    return -1;
}

// The loaded item archive for a kind and variation (NULL if none): kinds that share another kind's archive are looked
// up under the shared id (-3), and the variation is zeroed for kinds that do not have variation archives.
itArchive* itManager::getItemKindArchive(int kind, int variation) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (!valid) {
        variation = 0;
    } else if (variation == 5000) {
        variation = 5000;
    } else {
        variation = variation & ~(lbl_27_rodata_47C8[kind] >> 31);
    }
    if (archiveIdOf(this, kind) >= 0) {
        kind = -3;
    }
    for (int i = 0; i < static_cast<const soArrayList<itArchive*, 128>&>(m_itArchiveArrayList).size(); i++) {
        itArchive* archive = static_cast<const soArrayList<itArchive*, 128>&>(m_itArchiveArrayList).at(i);
        if (archive != NULL && kind == archive->m_itKind && variation == (int)archive->m_itVariation) {
            return archive;
        }
    }
    return NULL;
}

// Kinds that share another kind's lottery group map to that group's kind (-3 and out-of-range kinds map to themselves).
static inline int lotGroupKindOf(int kind) {
    int group;
    if (kind == -3) {
        group = kind;
    } else if (kind < 0 || kind >= NUM_IT_KINDS) {
        group = kind;
    } else {
        group = lbl_27_rodata_50D8[kind];
    }
    return group;
}

// Queues a Pokemon kind for preloading (Pokeball pool): at most 5 stay queued; when the queue is full the oldest
// one is dropped, together with its live items and loaded archives, and the preload is retried later.
bool itManager::preloadPokemon(int kind) {
    int queued = kind;
    int state = itManagerField<int>(this, 0x4C);
    if ((u32)(state - 3) <= 1) {
        return false;
    }
    if (state == 7) {
        return false;
    }
    if (getItemGroup(kind) != 3) {
        return false;
    }
    if (m_pokemonInfoArrayList.size() < 5) {
        int capacity = m_pokemonInfoArrayList.capacity();
        if (m_pokemonInfoArrayList.size() < capacity) {
            goto preload;
        }
    }
    {
        int oldest = *(int*)&m_pokemonInfoArrayList.at(0);
        if (itManagerField<int>(this, 0x1160) > 0) {
            if (itManagerField<int>(this, 0x1160) > 0) {
                itManagerField<int>(this, 0x1160)--;
            }
        }
        m_pokemonInfoArrayList.erase(0);
        for (int i = 0; i < m_itemArrayList.size(); i++) {
            BaseItem*& item = m_itemArrayList.at(i);
            if (item != NULL) {
                int itemKind = item->m_kind;
                if (oldest == lotGroupKindOf(itemKind) || itemKind == 0x2A) {
                    m_itemArrayList.erase(i);
                    removeItemSub(item, 0, 1);
                    i--;
                }
            }
        }
        bool flagged = false;
        for (int i = 0; i < m_itArchiveArrayList.size(); i++) {
            itArchive*& archive = m_itArchiveArrayList.at(i);
            if (archive != NULL) {
                if (oldest == lotGroupKindOf(archive->m_itKind)) {
                    archive->m_4 = 1;
                    flagged = true;
                }
            }
        }
        if (flagged) {
            if (!(m_preloadPokemonTimer > 0)) {
                m_preloadPokemonTimer = 16;
            }
            return false;
        }
    }
preload:
    itArchive* archive = preloadItemKindArchive((itKind)queued, 5000, (itArchive::Type)1, true);
    if (archive == NULL) {
        return false;
    }
    m_pokemonInfoArrayList.push(*(ItemInfo*)&queued);
    return true;
}

// Follows the chain of "borrows another kind's Brres" links to its end (the original unrolls the recursion).
int itManager::getItemKindArchiveRefBrresItKind(int kind, bool unk) {
    if (kind == -3) {
        return kind;
    }
    if (kind < 0 || kind >= NUM_IT_KINDS) {
        return kind;
    }
    int ref = lbl_27_rodata_4B48[kind];
    if (ref == -1) {
        return kind;
    }
    return getItemKindArchiveRefBrresItKind(ref, false);
}

// The archive group of a kind. A handful of kinds with variation archives (Smash Ball style crates and barrels) add
// the caller's group to the table value; every other kind first resolves its borrowed-Brres chain.
u8 itManager::getItemKindArchiveGroup(int kind, u8 group) {
    bool valid = false;
    if (kind > -1 && kind < NUM_IT_KINDS) {
        valid = true;
    }
    if (!valid) {
        return 0;
    }
    int variationSign;
    if (kind < 0) {
        variationSign = 0;
    } else {
        int mask = lbl_27_rodata_47C8[kind] >> 31;
        variationSign = (mask ^ lbl_27_rodata_47C8[kind]) - mask;
    }
    if (variationSign < 1 || (kind != 0xF && kind != 7 && kind != 3 && kind != 10 && kind != 0x14 && kind != 0x8F)) {
        getItemKindArchiveRefBrresItKind(kind, false);
        return lbl_27_rodata_4A90[kind];
    }
    return lbl_27_rodata_4A90[kind] + group;
}

// HYPOTHESIS: the byte at +9 of the melee init data packs the game rule (time / stock / coin) in its top 3 bits.
struct gmGameRuleByte {
    u8 rule : 3;
    u8 rest : 5;
};

// Arms the Smash Ball timers at the start of a match (HYPOTHESIS: 0x148C/0x1490 are the first-drop and re-drop frame
// counts, derived from the item parameter floats at 0x64/0x70 when still unset; 0x1488 is the latest drop frame, a
// fraction (param 0x68) of the match length).
void itManager::initializeSmashBallFlags() {
    if (itManagerField<int>(this, 0x148C) < 0) {
        float seconds = itManagerField<float>(this, 0x64);
        itManagerField<int>(this, 0x148C) = (int)(60.0f * seconds);
    }
    if (itManagerField<int>(this, 0x1490) < 0) {
        float seconds = itManagerField<float>(this, 0x70);
        itManagerField<int>(this, 0x1490) = (int)(60.0f * seconds);
    }
    itManagerField<int>(this, 0x1480) = itManagerField<int>(this, 0x148C);
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee != NULL) {
        u32 timeLimit = melee->m_meleeInitData.m_timeLimitFrames;
        switch (((gmGameRuleByte*)((u8*)melee + 9))->rule) {
        case 1:
        case 2:
            timeLimit = 0x1C20;
            break;
        }
        u32 matchFrames = (u32)(itManagerField<float>(this, 0x6C) * 60.0f * 60.0f);
        if (timeLimit == 0 || timeLimit > matchFrames) {
            timeLimit = matchFrames;
        }
        float ratio = itManagerField<float>(this, 0x68);
        itManagerField<float>(this, 0x1484) = 0.0f;
        u32 latest = (u32)((float)timeLimit * ratio);
        itManagerField<u32>(this, 0x1488) = latest;
        if (latest <= itManagerField<u32>(this, 0x1480)) {
            itManagerField<u32>(this, 0x1488) = 0;
        }
    }
}

// Decides whether the Dragoon (the machine from Kirby Air Ride) shows up this match and when. HYPOTHESIS: 0x1494 bit 0
// = a Dragoon is scheduled, 0x1498 = the frame it arrives, 0x149C = forced; the chance comes from the item parameter
// floats (0x74 time rule, 0x78 coin rule, 0x7C stock rule), scaled by the basic-item lottery value of kind 0x14.
void itManager::initializeDragoonFlags(int forced, bool disabled) {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee != NULL) {
        itManagerField<u32>(this, 0x1498) = 0;
        itManagerField<u32>(this, 0x1494) = 0;
        itManagerField<u8>(this, 0x149C) = 0;
        if (itManagerField<int>(this, 0x50) != 4 && !disabled && (forced != 0 || fn_800526E4() == 0)) {
            float rate = itManagerField<float>(this, 0x74);
            int rule = ((gmGameRuleByte*)((u8*)melee + 9))->rule;
            u32 frames = melee->m_meleeInitData.m_timeLimitFrames;
            switch (rule) {
            case 1: {
                rate = itManagerField<float>(this, 0x7C);
                frames = *(int*)((u8*)melee + 0x24) * (itManagerField<int>(this, 0xAC) * 60);
                u32 minFrames = itManagerField<int>(this, 0xB0) * 60;
                if (frames < minFrames) {
                    frames = minFrames;
                }
                break;
            }
            case 2:
                rate = itManagerField<float>(this, 0x78);
                break;
            }
            if (forced != 0) {
                frames = frames / 3;
                itManagerField<u8>(this, 0x149C) = 1;
            }
            u32 maxFrames = itManagerField<int>(this, 0xB4) * 3600;
            if (frames == 0 || frames > maxFrames) {
                frames = maxFrames;
            }
            float base = 0.1f;
            if (itManagerField<u8>(this, 0x149E) != 0) {
                base = getRandBasicItemValue(0x14, reinterpret_cast<ItemSwitch*>(&itManagerField<u32>(this, 0x119C)));
            }
            float chance = base * rate;
            if (forced != 0 || randf() < chance) {
                itManagerField<u32>(this, 0x1498) = frames;
                itManagerField<u32>(this, 0x1494) |= 1;
                fn_80052694();
            }
        }
    }
}
