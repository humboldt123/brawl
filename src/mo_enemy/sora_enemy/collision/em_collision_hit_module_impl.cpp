// soCollisionHitModuleImpl declares its anim cmd handler with an int index; the observer base must take the same one for
// the override to replace it (the original has a single handler, with its thunk in the observer vtable).
#define WN_ARTICLE_ANIMCMD_INT
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
// MATCH-ONLY: getDurabilityMulFromAttackData reads soCollisionAttackData's attribute unsigned and its direct flag from
// the whole word (u32 bitfields; the layout is the same).
#define SO_COLLISION_ATTACK_DATA_WORD_FIELDS
#include <ac/ac_anim_cmd_impl.h>
#include <em/collision/em_collision_hit_module_impl.h>
#include <types.h>

extern char g_soTeamModuleNullArg[];

// The argument at index (a virtual call through the array interface, as in the Enemy unit).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

emCollisionHitModuleImpl::emCollisionHitModuleImpl(soModuleAccesser* acc, int taskId, u8 category,
                                                   soArray<soCollisionHitPart>* parts, soArray<soCollisionGroup>* groups,
                                                   soArray<soCollisionHitGroup>* hitGroups, void* hitGroupData,
                                                   void* hitData)
    : soCollisionHitModuleImpl(acc, taskId, static_cast<gfTask::Category>(category), parts, groups, hitGroups,
                               reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg), true) {
    m_weakness = static_cast<emCollisionHitWeakness*>(hitGroupData);
    m_durability = static_cast<emCollisionHitDurability*>(hitData);
}

emCollisionHitModuleImpl::~emCollisionHitModuleImpl() { }

bool emCollisionHitModuleImpl::isHitWeakness(u32 partId) {
    emCollisionHitWeakness* weakness = m_weakness;
    if (weakness == NULL) {
        return false;
    }
    u32* partIds = weakness->m_partIds;
    if (partIds == NULL) {
        return false;
    }
    int num = weakness->m_num;
    if (num < 0) {
        return false;
    }
    for (int i = 0; i < num; i++) {
        if (partId == partIds[i]) {
            return true;
        }
    }
    return false;
}

void emCollisionHitModuleImpl::setWeaknessCollisionStatus(int status) {
    emCollisionHitWeakness* weakness = m_weakness;
    if (weakness == NULL) {
        return;
    }
    u32* partIds = weakness->m_partIds;
    if (partIds == NULL) {
        return;
    }
    int num = weakness->m_num;
    if (num < 0) {
        return;
    }
    for (int i = 0; i < num; i++) {
        setStatus(partIds[i], status, 0);
    }
}

u32 emCollisionHitModuleImpl::getDurabilityFromAttackAttr(int attr) {
    // The attributes with a multiplier of their own; classes 9 and 10 have none (9 takes every other attribute).
    int attrs[] = {
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Attribute_Cutup,
        soCollisionAttackData::Attribute_Electric, soCollisionAttackData::Attribute_Ice,
        soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Attribute_Grass,
        soCollisionAttackData::Attribute_Water, soCollisionAttackData::Attribute_Sleep,
        soCollisionAttackData::Attribute_None2,
    };
    for (u32 i = 0; i < 11; i++) {
        if (i == 9 || i == 10) {
            continue;
        }
        if (attr == attrs[i]) {
            return i;
        }
    }
    return 9;
}

float emCollisionHitModuleImpl::getDurabilityMulFromAttackData(soCollisionAttackData* attackData, u32 partId) {
    float mul = 1.0f;
    if (m_durability == NULL) {
        return mul;
    }
    u32 index = getDurabilityFromAttackAttr(attackData->m_attribute);
    if (isHitWeakness(partId) == true) {
        mul = m_durability->m_weakAttrMul[index];
    } else {
        mul = m_durability->m_attrMul[index];
    }
    if (!attackData->m_isDirect) {
        if (isHitWeakness(partId) == true) {
            mul *= m_durability->m_weakIndirectMul;
        } else {
            mul *= m_durability->m_indirectMul;
        }
    }
    return mul;
}

bool emCollisionHitModuleImpl::isObserv(char group) {
    return group == 6 || group == 'm';
}

bool emCollisionHitModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    if (acmd->getGroup() != 'm') {
        return soCollisionHitModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 1) {
        return false;
    }
    switch (type) {
    case 0: {
        // Set the hit status of the weak spots.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        setWeaknessCollisionStatus(getCmdArg(args, 0).getIntData());
        return true;
    }
    }
    return false;
}

void soCollisionHitModuleImpl::renderDebug() { }

void soCollisionHitModuleImpl::setReactionFrame(short reactionFrame) {
    m_reactionFrame = reactionFrame;
}

int soCollisionHitModuleImpl::getReactionFrame() {
    return m_reactionFrame;
}

bool soCollisionHitModuleImpl::isReactionFrame() {
    return m_reactionFrame >= 0;
}
