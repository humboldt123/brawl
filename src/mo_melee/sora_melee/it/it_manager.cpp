#include <types.h>
#include <it/it_manager.h>
#include <gf/gf_task_scheduler.h>
#include <so/situation/so_situation_module_impl.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {



// itManager::checkCreatableItem (0x2A5EE0, unnamed): whether the item kind may be created right now (match state, free
// instance slots, per-kind limits)
int fn_27_2A5EE0(itManager* self, int kind, u32 variation, bool unk);

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
extern u8 lbl_27_data_71F6C[];
extern char* lbl_27_data_731C0[];
extern char lbl_27_data_73488[];
extern int lbl_27_rodata_47C8[];
extern int lbl_27_bss_64A0[];
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
