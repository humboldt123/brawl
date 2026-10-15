#include <types.h>
#include <it/it_manager.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

void fn_27_2A7970(itManager* self, BaseItem* item, s32 unk1, s32 unk2);

// itManager::checkCreatableItem (0x2A5EE0, unnamed): whether the item kind may be created right now (match state, free
// instance slots, per-kind limits)
int fn_27_2A5EE0(itManager* self, int kind, u32 variation, bool unk);

// itManager::findLowPriorityItemInstance (0x2A8054, unnamed): an item that may be recycled when the pool is full
BaseItem* fn_27_2A8054(itManager* self);
// itManager::setExistDragoonParts (0x2A96F4, unnamed)
void fn_27_2A96F4(itManager* self, int variation, int exist);
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
                fn_27_2A7970(this, item, 0, 1);
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
                fn_27_2A7970(this, item, 0, 1);
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
            BaseItem* old = fn_27_2A8054(this);
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
                fn_27_2A96F4(this, variation, 0);
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
