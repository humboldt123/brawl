#define FT_ENTRY_UNIT
#define WN_EVENT_UNIT_DTOR_INLINE
#include <ft/ft_entry.h>
#include <ft/ft_entry_manager.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_slot.h>
#include <ft/ft_slot_manager.h>
#include <ft/fighter.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <types.h>

// TODO: the larger ftEntry and ftEntryManager members.

extern "C" {
// HYPOTHESIS: ftEntryManager::setSwap (unnamed in sora_melee).
void fn_27_11967C(ftEntryManager* manager, int unk, int entryId1, int entryId2, bool unk2, int unk3);
// HYPOTHESIS: ftInput::setContNo (unnamed in sora_melee).
void fn_27_124830(ftInput* input, int contNo);
// HYPOTHESIS: ftInput::standbyAdvFollow (unnamed in sora_melee).
void fn_27_1241D8(ftInput* input, int unk);
// HYPOTHESIS: Fighter::setZoom (unnamed in sora_melee).
void fn_27_13231C(Fighter* fighter, float unk1, int unk2, int unk3);
// HYPOTHESIS: ftEntry::process (unnamed in sora_melee).
void fn_27_1186D4(ftEntry* entry);
// HYPOTHESIS: the index of the area instance `areaId` of an object (-1 if it has none).
int fn_27_8CC68(StageObject* object, int areaId);
// HYPOTHESIS: Fighter::addDamage (unnamed in sora_melee).
void fn_27_13C1F0(Fighter* fighter, float damage);
}

// The head word of gfTaskScheduler (its process type is private there).
struct ftEntrySchedulerView {
    s32 unk0 : 8;
    s32 m_processType : 8;
};

static inline ftEntry* getEntry(int entryId) {
    return g_ftEntryManager->getEntity(entryId);
}

static inline Fighter* getActiveFighter(ftEntry* entry) {
    return entry->m_instances[(u8)entry->m_activeInstanceIndex].m_fighter;
}

void ftEntry::standby(int unk) {
    for (int i = 0; i < m_entryCount; i++) {
        Fighter* fighter = m_instances[i].m_fighter;
        if (fighter != NULL) {
            fighter->standby(unk);
        }
    }
    fn_27_1241D8(m_input, 0);
    unkF = 1;
    unkC = 0;
}

// The sub fighter (instance 1) starts at the owner's start position with the owner's damage.
void ftEntry::startSubFighter() {
    if (_0xe & 2) {
        Vec3f pos = m_owner->getStartPos();
        float lr = m_owner->getStartLr();
        Fighter* sub = m_instances[1].m_fighter;
        sub->start(&pos, lr, m_owner->getDamage(), 1, g_ftManager->m_mode == 1);
    }
}

void ftEntry::standbyAdvFollow() {
    for (int i = 0; i < m_entryCount; i++) {
        Fighter* fighter = m_instances[i].m_fighter;
        if (fighter != NULL) {
            fighter->standbyAdvFollow();
        }
    }
    fn_27_1241D8(m_input, 0);
}

// HYPOTHESIS: the word at +0x28 of the fighter is its task id.
int ftEntry::isExistFighter(int kind) {
    for (int i = 0; (u8)i < m_entryCount; i++) {
        Fighter* fighter = m_instances[(u8)i].m_fighter;
        if (fighter != NULL && (u32)kind == *reinterpret_cast<u32*>(reinterpret_cast<u8*>(fighter) + 0x28)) {
            return i;
        }
    }
    return 0xFF;
}

// The character kind of the active instance: the forms of the characters that change (HYPOTHESIS: the trainer's Pokemon,
// Zelda/Sheik, Samus/Zero Suit) report the form's own kind.
int ftEntry::getCurrentInstanceGmKind() {
    if (_0xe & 1) {
        switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
        case 0x1d:
            return 0x1d;
        case 0x1e:
            return 0x1f;
        case 0x1f:
            return 0x21;
        }
    } else {
        switch (m_characterKind) {
        case 0xe:
        case 0xf:
            switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
            case 0xd:
                return 0xe;
            case 0xe:
                return 0xf;
            }
            break;
        case 3:
        case 4:
            switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
            case 3:
                return 3;
            case 0x18:
                return 4;
            }
            break;
        }
    }
    return m_characterKind;
}

void ftEntry::entryEnd() {
    for (u8 i = 0; i < m_entryCount; i++) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(i, 0x2000, 0);
    }
}

void ftEntry::resultEnd() {
    for (u8 i = 0; i < m_entryCount; i++) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(i, 0x4000, 0);
    }
}

void ftEntry::exitFinal(int unk) {
    if ((u32)unk == 1) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(0xFF, 0x1000, 0);
        _0x10 = 0;
    } else {
        _0x10 = 2;
    }
}

int ftEntry::getDragoonCount(bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_owner->getDragoonCount();
    }
    return m_owner->getDragoonCount();
}

int ftEntry::getDragoonVariation(int index, bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_owner->getDragoonVariation(index);
    }
    return m_owner->getDragoonVariation(index);
}

void ftEntry::toKnockOut(soDamageAttackerInfo* attackerInfo) {
    if (unkF == 6) {
        getActiveFighter(this)->toKnockOut(attackerInfo);
    }
}

void ftEntry::setContNo(int contNo) {
    if (m_heartSwapEntryId != -1 && g_ftEntryManager->m_isProcessHeartSwap == true) {
        ftEntryManager* manager = g_ftEntryManager;
        fn_27_11967C(manager, 0, *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x44),
                     *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x48), false, -1);
    }
    fn_27_124830(m_input, contNo);
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x70) = contNo;
}

void ftEntry::setSlow(bool setStatus, int strength, int duration, bool isTimer) {
    if (unkF == 6) {
        getActiveFighter(this)->setSlow(setStatus, strength, duration, isTimer);
        if (_0xe & 2) {
            m_instances[1].m_fighter->setSlow(setStatus, strength, duration, isTimer);
        }
    }
}

void ftEntry::setWarp(Vec3f* pos, float lr, u32 flags) {
    if (unkF == 6) {
        getActiveFighter(this)->warp(pos, lr, flags);
        if (_0xe & 2) {
            m_instances[1].m_fighter->warp(pos, lr, flags);
        }
    }
}

// HYPOTHESIS: the pending scaling request (kind at +0x20, -1 when none; type at +0x24), applied outside the passes 2..5.
void ftEntry::setScaling(int kind, int type) {
    if (unkF == 6) {
        int* request = reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x20);
        request[0] = kind;
        request[1] = type;
        gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
        if (scheduler != NULL) {
            int processType = reinterpret_cast<ftEntrySchedulerView*>(scheduler)->m_processType;
            if ((processType <= 1 || processType >= 6) && request[0] != -1) {
                if (request[0] == 0) {
                    if (_0xe & 2) {
                        m_instances[0].m_fighter->restoreScaling();
                        m_instances[1].m_fighter->restoreScaling();
                    } else {
                        getActiveFighter(this)->restoreScaling();
                    }
                } else if (_0xe & 2) {
                    m_instances[0].m_fighter->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                    m_instances[1].m_fighter->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                } else {
                    getActiveFighter(this)->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                }
                request[0] = -1;
            }
        }
    }
}

void ftEntry::setCurry() {
    if (unkF == 6) {
        getActiveFighter(this)->setCurry(true, -1);
    }
}

void ftEntry::setSuperStar() {
    if (unkF == 6) {
        getActiveFighter(this)->setSuperStar(true, -1, true);
    }
}

void ftEntry::setZoom(float unk1, int unk2, int unk3) {
    if (unkF == 6) {
        fn_27_13231C(getActiveFighter(this), unk1, unk2, unk3);
    }
}

int ftEntry::getTeam(bool useTeamPoint, bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        ftEntry* entry = getEntry(m_heartSwapEntryId);
        if (useTeamPoint == true) {
            return entry->m_playerNo;
        }
        return entry->m_owner->getTeam();
    }
    if (useTeamPoint == true) {
        return m_playerNo;
    }
    return m_owner->getTeam();
}

int ftEntry::getTeam2nd(bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_playerNo;
    }
    return m_playerNo;
}

// The trainer (instance 3) leaves with the rest of the match unless only one player is left.
void ftEntry::disappearTrainer() {
    if (_0xe & 1) {
        m_instances[3].m_fighter->disappear(g_ftManager->m_mode == 1);
    }
}

bool ftEntry::isProcessTechnique() {
    if ((u8)((u8)unk11_08 != 0) == 1) {
        return true;
    }
    if ((u8)_0x10 != 0) {
        return true;
    }
    if (unkF == 3 && *(reinterpret_cast<u8*>(this) + 0x170) == 1) {
        return true;
    }
    return false;
}

// HYPOTHESIS: the change target (position at +0x78, facing at +0x84, pose at +0x14).
void ftEntry::prepareChange(float lr, Vec3f* pos, int changePose) {
    *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(this) + 0x78) = *pos;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x84) = lr;
    unkC = 7;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x14) = changePose;
    unk11_20 = true;
    if (unkF != 3) {
        unkF = 5;
    }
}

void ftEntry::setHeal(float heal) {
    if (unkF == 6) {
        fn_27_13C1F0(getActiveFighter(this), -heal);
        if (_0xe & 2) {
            fn_27_13C1F0(m_instances[1].m_fighter, -heal);
        }
    }
}

void ftEntry::setVisibilityTrainer(bool visible) {
    if (unkF == 6 && (_0xe & 1)) {
        m_instances[3].m_fighter->setVisibility(visible);
    }
}

// HYPOTHESIS: +0x38 is the swap counter (starts at 1), +0x40/+0x44/+0x48 the swap entry ids.
ftEntryManager::ftEntryManager(int entryCapacity) : m_entries(NULL), m_entryCount(entryCapacity) {
    *reinterpret_cast<int*>(_0x38) = 1;
    m_isProcessHeartSwap = false;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x40) = -1;
    m_entries = new (static_cast<Heaps::HeapType>(2)) ftEntry[entryCapacity];
    m_entryArrayVector.clear();
    g_ftEntryManager = this;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x44) = -1;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x48) = -1;
}

ftEntryManager::~ftEntryManager() {
    delete[] m_entries;
    m_entries = NULL;
    g_ftEntryManager = NULL;
}

ftEntry* ftEntryManager::getEntity(u32 entryId) {
    return &m_entries[(u8)entryId];
}

// The instance of the entry whose fighter task has the id.
static inline bool findInstanceFromTaskId(ftEntry* entry, int taskId, int* outInstanceIndex) {
    Fighter* fighter;
    int i;
    for (i = 0; i < entry->m_entryCount; i++) {
        fighter = entry->m_instances[i].m_fighter;
        if (fighter != NULL && (u32)taskId == *reinterpret_cast<u32*>(reinterpret_cast<u8*>(fighter) + 0x28)) {
            if (outInstanceIndex != NULL) {
                *outInstanceIndex = i;
            }
            return true;
        }
    }
    return false;
}

static inline bool findInstanceFromAreaId(ftEntry* entry, int areaId, int* outInstanceIndex) {
    int count = entry->m_entryCount;
    for (int i = 0; i < count; i++) {
        Fighter* fighter = entry->m_instances[i].m_fighter;
        if (fighter != NULL && fn_27_8CC68(fighter, areaId) != -1) {
            if (outInstanceIndex != NULL) {
                *outInstanceIndex = i;
            }
            return true;
        }
    }
    return false;
}

int ftEntryManager::getEntryIdFromAreaId(int areaId, int* outInstanceIndex) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (findInstanceFromAreaId(static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i), areaId, outInstanceIndex) == true) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

int ftEntryManager::getEntryIdFromTaskId(int taskId, int* outInstanceIndex) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (findInstanceFromTaskId(static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i), taskId, outInstanceIndex) == true) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

int ftEntryManager::getEntryIdFromPlayerNo(int playerNo) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (playerNo == static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_playerNo) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

// HYPOTHESIS: registers the first free entry (state 7) from the start slot (0 for unk 1, else 4) and returns its id
// (a wrapping counter in the high half, the index in the low half), or -1.
int ftEntryManager::pushEntity(int unk) {
    for (int i = (unk == 1) ? 0 : 4; i < (int)m_entryCount; i++) {
        if (m_entries[i].unkF == 7) {
            int counter = *reinterpret_cast<int*>(_0x38);
            int id = (counter << 16) | i;
            counter++;
            *reinterpret_cast<int*>(_0x38) = counter;
            if (counter > 0xFFFF) {
                *reinterpret_cast<int*>(_0x38) = 0;
            }
            ftEntry* entry = &m_entries[i];
            m_entryArrayVector.push(entry);
            m_entries[i].m_entryId = id;
            return id;
        }
    }
    return -1;
}

void ftEntryManager::process() {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        fn_27_1186D4(m_entryArrayVector.at(i));
    }
}

bool ftEntryManager::isValid(int entryId) {
    if (entryId != -1 && (int)(u8)entryId < (int)m_entryCount && m_entries[(u8)entryId].m_entryId == entryId) {
        return true;
    }
    return false;
}

void ftEntryManager::endSwap(bool unk) {
    if (m_isProcessHeartSwap == true) {
        fn_27_11967C(this, 0, *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x44),
                     *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x48), unk, -1);
    }
}

ftEntry::~ftEntry() {
}

void ftEntry::startResult() {
    Vec3f pos = m_owner->getStartPos();
    int startMode = 5;
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    gmGlobalModeMelee* modeMelee = g_GameGlobal->m_modeMelee;
    if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
        ftOutsideEventPresenter presenter(m_entryId, g_ftManager->m_eventManageModule.getManageId());
        presenter.notifyOutsideEventResultWin(0);
    } else if ((bool)g_ftManager->m_isTeams == true) {
        if (resultInfo->m_winningTeam == (u8)modeMelee->m_playersInitData[m_playerNo].m_teamNo) {
            startMode = 4;
        }
    } else {
        u8 idx = modeMelee->m_playersInitData[m_playerNo].m_playerNo;
        if (resultInfo->m_playersResultInfo[idx].m_rank == 0) {
            startMode = 4;
        }
    }
    if (_0xe & 1) {
        getActiveFighter(this)->start(&pos, 1.0f, 0.0f, startMode, false);
        m_instances[3].m_fighter->trainerStart(1.0f, &pos, &pos, startMode, false);
    } else if (_0xe & 2) {
        m_instances[0].m_fighter->start(&pos, 1.0f, 0.0f, startMode, false);
        if (!m_owner->isSubFighterStartStandby()) {
            m_instances[1].m_fighter->start(&pos, 1.0f, 0.0f, startMode, false);
        }
    } else {
        getActiveFighter(this)->start(&pos, 1.0f, 0.0f, startMode, false);
    }
}

#include <mt/mt_vector.h>
// TODO: clean up, give variables real names
namespace jj_113C0C {
struct Vec3 { float x, y, z; };
struct Vec4 { float x, y, z, w; };
struct Inner {
    char pad0[8];
    u32 top : 3;
    u32 mode : 3;
    u32 type : 3;
    u32 sub : 3;
    u32 flag : 1;
    u32 rest : 19;
    char padC[0x10];
    Vec3 pos;
    float value;
    Vec4 a;
    Vec4 b;
    char pad4C[0x14];
    Vec3 zero;
};
struct Self {
    char pad0[0x28];
    void* work;
    char pad2C[0x2c];
    u32 type;
    char pad5C[0x134];
    Inner* inner;
    s32 result;
};
extern "C" {
s32 getAdventureUnitKind__7ftOwnerFv(void*);
void regist__9cmSubjectFv(Inner*);
void setPos__9cmSubjectFP5Vec3f(Inner*, const Vec3*);
void setRange__9cmSubjectFP6Rect2D(Inner*, const Vec4*);
void updateDispStatus__9cmSubjectFv(Inner*);
}
extern float lbl_27_rodata_FB8;

extern "C" void setTemporaryCamera__7ftEntryFfiii(Self* self, Vec4* arg1, Vec3* arg2, s32 arg3, float farg0) {
    if (self->inner != 0) {
        regist__9cmSubjectFv(self->inner);
        self->inner->mode = 3;
        if (getAdventureUnitKind__7ftOwnerFv(self->work) == 0)
            self->inner->type = 5;
        else
            self->inner->type = 4;
        self->inner->sub = self->type;
        setRange__9cmSubjectFP6Rect2D(self->inner, arg1);
        {
            Inner* p = self->inner;
            float w, z, y, x;
            x = arg1->x;
            y = arg1->y;
            p->b.x = x;
            z = arg1->z;
            p->b.y = y;
            w = arg1->w;
            p->b.z = z;
            p->b.w = w;
        }
        self->inner->value = farg0;
        setPos__9cmSubjectFP5Vec3f(self->inner, arg2);
        Inner* p = self->inner;
        float value, z, y, x;
        x = arg2->x;
        y = arg2->y;
        p->pos.x = x;
        z = arg2->z;
        p->pos.y = y;
        value = lbl_27_rodata_FB8;
        p->pos.z = z;
        Vec3 zero = {value, value, value};
        self->inner->zero = zero;
        self->inner->flag = 0;
        self->inner->top = 0;
        updateDispStatus__9cmSubjectFv(self->inner);
        self->result = arg3;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_113D94 {
struct Vec3f {
    float x;
    float y;
    float z;
};

struct Vec4f {
    float x;
    float y;
    float z;
    float w;
};

struct Inner {
    char pad0[0x28];
    float unk28;
    Vec4f unk2C;
    Vec4f unk3C;
};

struct Self {
    char pad0[0x190];
    Inner* unk190;
    s32 unk194;
    u32 unk198;
    Vec3f unk19C;
    float unk1A8;
};

extern "C" void updateDispStatus__9cmSubjectFv(Inner* p);

extern "C" void setIntarpolateTemporaryCamera__7ftEntryFffii(Self* self, Vec3f* v, u32 flags, float f1, float f2) {
    if (self->unk190 != 0 && f1 != self->unk190->unk28) {
        Vec4f a = self->unk190->unk2C;
        self->unk190->unk2C.x = -a.x;
        self->unk190->unk2C.y = -a.y;
        Vec4f b = self->unk190->unk3C;
        self->unk190->unk3C.x = -b.x;
        self->unk190->unk3C.y = -b.y;
        self->unk190->unk28 = f1;
        updateDispStatus__9cmSubjectFv(self->unk190);
    }
    float vz;
    float vy;
    float vx;
    vx = v->x;
    vy = v->y;
    vz = v->z;
    self->unk19C.x = vx;
    self->unk19C.y = vy;
    self->unk19C.z = vz;
    self->unk198 = flags;
    self->unk1A8 = f2;
    self->unk194 = -1;
}
}

// TODO: clean up, give variables real names
namespace jj_114308 {
extern "C" {
extern void* getInstance__15gfTaskSchedulerFv();
extern void fn_27_1143B0(char*);
extern unsigned int isJustGameFrame__10GameGlobalCFv(void*);
extern void* g_GameGlobal;
}

struct SchedHeader {
    unsigned int x : 8;
    signed int y : 8;
    unsigned int z : 16;
};

extern "C" void toStartSequence__7ftEntryFUc(char* self, unsigned char arg1) {
    *(unsigned char*)(self + 0x0c) = arg1;
    if (*(unsigned char*)(self + 0x0f) == 3) {
        if (*(unsigned char*)(self + 0x0e) & 1) {
            fn_27_1143B0(self);
            *(unsigned char*)(self + 0x0f) = 3;
        }
    } else {
        void* scheduler = getInstance__15gfTaskSchedulerFv();
        if (scheduler != 0) {
            if (isJustGameFrame__10GameGlobalCFv(g_GameGlobal) == 1) {
                SchedHeader hdr;
                *(int*)&hdr = *(int*)scheduler;
                if (hdr.y <= 1) {
                    fn_27_1143B0(self);
                    return;
                }
            }
            *(unsigned char*)(self + 0x0f) = 5;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_114ABC {
extern "C" void fn_27_114ABC(void) {}
}

// TODO: clean up, give variables real names
namespace jj_114D80 {
extern "C" void change__7FighterFPvi(void) {}
}

// TODO: clean up, give variables real names
namespace jj_115358 {
extern "C" void fn_27_115358(void) {}
}

// TODO: clean up, give variables real names
namespace jj_11535C {
extern void *g_GameGlobal;

extern "C" {
unsigned char getRank__7ftEntryFv(char *p)
{
    int idx = *(int *)(p + 0x58);
    char *g = (char *)g_GameGlobal;
    char *a = *(char **)(g + 0x8);
    char *b = *(char **)(g + 0x18);
    unsigned char sub = *(unsigned char *)(a + idx * 0x5c + 0x9b);
    return *(unsigned char *)(b + sub * 0x2ac + 0x32);
}
}
}

// TODO: clean up, give variables real names
namespace jj_11538C {
typedef unsigned char u8;
typedef int s32;

extern "C" {
extern char* g_GameGlobal;
}

extern "C" void* getRankPoint__7ftEntryFv(char* p) {
    s32 idx = *(s32*)(p + 0x58);
    char* g = g_GameGlobal;
    char* a = *(char**)(g + 0x8) + idx * 0x5c;
    s32 sub = *(u8*)(a + 0x9b);
    char* b = *(char**)(g + 0x18);
    return *(void**)(b + sub * 0x2ac + 0x27c);
}
}

// TODO: clean up, give variables real names
namespace jj_11581C {
struct fn_27_Obj {
    char pad[0xc];
    unsigned char unk_c;
};

extern "C" {
int fn_27_11581C(fn_27_Obj* p) {
    return p->unk_c == 6;
}
}
}

// TODO: clean up, give variables real names
namespace jj_115964 {
extern "C" void disappear__7FighterFb(void) {}
}

// TODO: clean up, give variables real names
namespace jj_115E3C {


extern "C" {
void getHipPos__23ftExternalValueAccesserFP7Fighter(void*, void*);
extern const float lbl_27_rodata_FB8[4];

void* fn_27_115E3C(void* self, float* target, int idx, float* out_min, float f) {
    const float* constants = lbl_27_rodata_FB8;
    Vec3f hip;
    float high;
    float base;
    float min;
    void* ent;
    void* result = 0;
    unsigned char count;

    min = constants[2];

    if (idx == 0)
        count = 1;
    else
        count = *(unsigned char*)((char*)self + 8);

    high = constants[3];
    int counter = 0;
    base = constants[0];

    for (; counter < (int)count; counter++) {
        if (counter == 0)
            ent = *(void**)((char*)self + *(unsigned char*)((char*)self + 0xa) * 8 + 0x34);
        else
            ent = *(void**)((char*)self + 0x3c);

        getHipPos__23ftExternalValueAccesserFP7Fighter(&hip, ent);

        if (base != f) {
            if (high == f) {
                if (hip.m_x > target[0])
                    break;
            } else {
                if (hip.m_x < target[0])
                    break;
            }
        }

        Vec2f goal;
        Vec2f current;
        Vec2f delta;
        current.m_x = hip.m_x;
        current.m_y = hip.m_y;
        goal.m_x = target[0];
        goal.m_y = target[1];
        delta.m_x = goal.m_x - current.m_x;
        delta.m_y = goal.m_y - current.m_y;
        float dist2 = delta.m_x * delta.m_x + delta.m_y * delta.m_y;

        if (dist2 < min) {
            min = dist2;
            result = ent;
        }
    }

    *out_min = min;
    return result;
}
}
}

// TODO: clean up, give variables real names
namespace jj_116CCC {
extern "C" void activateController__7FighterFb(void* self, u32 x);
extern "C" void setOperationCpu__7ftOwnerFb(void* self, u32 x);

extern "C" void fn_27_116CCC(u8* self, u32 a, u32 b) {
    void* p;
    u8* e = self;
    for (s32 i = 0; i < self[8]; ) {
        p = *(void**)(e + 0x34);
        if (p != 0) {
            activateController__7FighterFb(p, a);
            setOperationCpu__7ftOwnerFb(p, b);
        }
        e += 8;
        i++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_116DD4 {
extern "C" unsigned int fn_27_13FF7C(void*,u32,u32);
class Entry { public: char pad[0x3C]; virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31(); virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35(); virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39(); virtual void v40(); virtual void v41(); virtual void v42(); virtual void v43(); virtual void v44(); virtual void v45(); virtual void v46(); virtual void v47(); virtual void v48(); virtual void v49(); virtual void v50(); virtual void v51(); virtual void v52(); virtual void v53(); virtual void v54(); virtual void v55(); virtual void v56(); virtual void v57(); virtual void v58(); virtual void v59(); virtual void v60(); virtual void v61(); virtual void v62(); virtual void v63(); virtual void v64(); virtual void v65(); virtual void v66(); virtual void v67(); virtual void v68(); virtual void v69(); virtual void v70(); virtual void v71(); virtual void v72(); virtual void v73(); virtual void v74(); virtual void v75(); virtual void v76(); virtual void v77(); virtual void v78(); virtual void v79(); virtual void v80(); virtual void v81(); virtual void v82(); virtual void v83(); virtual void v84(); virtual void v85(); virtual void v86(); virtual void v87(); virtual void v88(); virtual void v89(); virtual void v90(); virtual void v91(); virtual void v92(); virtual void v93(); virtual void v94(); virtual void v95(); virtual void v96(); virtual void v97(); virtual void v98(); virtual void v99(); virtual void v100(); virtual void v101(); virtual void v102(); virtual void v103(); virtual void v104(); virtual void v105(); virtual void v106(); virtual void v107(); virtual void v108(); virtual void v109(); virtual void v110(); virtual void v111(); virtual void v112(); virtual void v113(); virtual void v114(); virtual void v115(); virtual void v116(); virtual void v117(); virtual void v118(); virtual void v119(); virtual void v120(); virtual void v121(); virtual void v122(); virtual void v123(); virtual void v124(); virtual void v125(); virtual void v126(); virtual void v127(); virtual void v128(); virtual void v129(); virtual void v130(); virtual void v131(); virtual void v132(); virtual void v133(); virtual void v134(); virtual void v135(); virtual void v136(); virtual void v137(); virtual void v138(); virtual void v139(); virtual void v140(); virtual void v141(); virtual void v142(); virtual void v143(); virtual void v144(); virtual void v145(); virtual void v146(); virtual void v147(); virtual void v148(); virtual void v149(); virtual void v150(); virtual void v151(); virtual void v152(); virtual void v153(); virtual void v154(); virtual void v155(); virtual void v156(); virtual void v157(); virtual void v158(); virtual void v159(); virtual void v160(); virtual void v161(); virtual void v162(); virtual void v163(); virtual void v164(); virtual void v165(); virtual void v166(); virtual void v167(); virtual void v168(); virtual void v169(); virtual void v170(); virtual void v171(); virtual void v172(); virtual void v173(); virtual void v174(); virtual void v175(); virtual void v176(); virtual void v177(); virtual void v178(); virtual void v179(); virtual void v180(); virtual void v181(); virtual void v182(); virtual void v183(); virtual void v184(); virtual void v185(); virtual void v186(); virtual unsigned int check(u32,u32); };
struct Slot { Entry* ptr; u32 pad; };
struct Obj { unsigned char pad[10]; unsigned char index; unsigned char pad2[3]; unsigned char flags; unsigned char type; char pad3[36]; Slot entries[9]; };
extern "C" s32 fn_27_116DD4(const Obj* self,s32 a,s32 b,u32 c) {
    if (self->type != 6) {
        if (self->flags&1) { Entry* p=self->entries[3].ptr; if(p&&p->check(a,b)==1)return 1; }
        if(fn_27_13FF7C((char*)self+0x78,a,b)==1)return 1;
    } else {
        Entry* p=self->entries[self->index].ptr;
        if(p&&p->check(a,b)==1)return 1;
        if((self->flags&2)&&c==1){Entry* p2=self->entries[1].ptr;if(p2&&p2->check(a,b)==1)return 1;}
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_116F5C {
extern "C" void fn_27_116F5C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_117340 {
extern "C" void notifyEventDeadPartner__20ftEntryEventObserverFi(void) {}
}

// TODO: clean up, give variables real names
namespace jj_117674 {
extern "C" void fn_27_117674(void) {}
}

// TODO: clean up, give variables real names
namespace jj_1179B8 {
extern "C" void notifyEventPokeTrainerReplace__20ftEntryEventObserverFUc(void) {}
}

// TODO: clean up, give variables real names
namespace jj_117D0C {
extern "C" void fn_27_117D0C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_118050 {
extern "C" void fn_27_118050(void) {}
}

// TODO: clean up, give variables real names
namespace jj_118394 {
extern "C" void fn_27_118394(void) {}
}

// TODO: clean up, give variables real names
namespace jj_1186D0 {
extern "C" void fn_27_1186D0(void) {}
}

// TODO: clean up, give variables real names
namespace jj_118E6C {
extern "C" void notifyEventPartnerResourcePrepared__20ftEntryEventObserverFv(void) {}
}

// TODO: clean up, give variables real names
namespace jj_119214 {
struct Data {
    unsigned int unused;
    unsigned int value;
};

struct Container {
    virtual void slot08();
    virtual void slot0C();
    virtual Data** at(int index);
    virtual int size();
};

struct Object {
    unsigned char unused[8];
    Container array;
};

extern "C" int enumEntryId__14ftEntryManagerFi(Object* self, unsigned int* output) {
    int count = 0;
    int size = self->array.size();

    for (int i = 0; i < size; i++) {
        *output++ = (*self->array.at(i))->value;
        count++;
    }

    return count;
}
}

