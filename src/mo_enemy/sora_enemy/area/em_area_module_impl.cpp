// The weak copies of soAreaModuleImpl::getEnviromentElementChecker and setLogOverThrough are this unit's.
#define SO_AREA_MODULE_INLINE_ACCESSORS
// The constructor of soAreaEnviromentElementCheckerImpl is a function of sora_melee.
#define SO_AREA_ENVIROMENT_ELEMENT_CHECKER_IMPL_CT
// MATCH-ONLY: only the situation observer's addObserver is a weak copy of this unit (the others are sora_melee's).
#define SO_SITUATION_OBSERVER_INLINE
#define SO_EVENT_OBSERVER_ADD_SUB_EXTERN
// MATCH-ONLY: the unit calls the weak out-of-line copies of Vec3f(float, float, float) and operator= and of the Vec2f
// constructor, length and operator- that other units of the REL carry (Vec2f is copied whole, word by word).
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_CTOR_OUT_OF_LINE
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_SUB_NOINLINE
// MATCH-ONLY: the paired-single Vec3f subtraction of the weak Vec3f::distance writes over the right-hand registers.
#define MT_VEC3F_SUB_ASCENDING
#define MT_VEC3F_SUB_NATIVE_ORDER
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
// MATCH-ONLY: the observer bases are built by the sora_melee copies (EM_ENEMY_BUILDER would also do it, but it conflicts
// with ft/ft_manager.h, which specializes soInstanceUnit<soEventUnit*> and so comes before the event system headers).
#define SO_EVENT_OBSERVER_CTOR_EXTERN
#define SO_EVENT_OBSERVER_DTOR_EXTERN
#include <ft/ft_manager.h>
#include <em/em_external_value_accesser.h>
#include <em/em_manager.h>
#include <em/enemy.h>
#include <ft/fighter.h>
#include <ft/ft_external_value_accesser.h>
#include <mt/mt_coll2d.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// MATCH-ONLY: the weak copy of Vec3f::distance in this unit expands the length (z squared first, square root by the
// reciprocal square root) instead of calling the DOL's Vec3f::length.
inline float Vec3f::length() {
    float zSq = m_z * m_z;
    float xSq = m_x * m_x;
    float ySq = m_y * m_y;
    float lengthSquared = zSq + (xSq + ySq);
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

// Stand-ins for functions of sora_melee that the headers do not declare (module map: unnamed).
extern "C" {
// HYPOTHESIS: ftStatus predicates of a fighter status kind (the downed-and-waiting statuses, and holding a smash).
bool fn_27_14D530(int statusKind);
bool fn_27_14D550(int statusKind);
}
// MSL's atan2f, called through its out-of-line weak copy (shared with the kinetic module unit).
extern "C" float atan2f(float y, float x);
float vec2fAngle(Vec2f* a, Vec2f* b);

// The argument types of acCmdArg::getArgType that the area anim cmds accept besides plain values.
static const s32 CmdArgType_ValueIndex = 5;

// The data of the area kinds (not const: they are in .data). Index: the common kind (0-5), or the group - 9.
static u32 sAreaCommonKindToGroup[6] = {9, 10, 10, 10, 10, 10};
static u32 sAreaGroupToGroupMask[6] = {0, 0, 2, 0, 0x10000000, 0};
static u32 sAreaGroupToGroupMaskLocal[6] = {0, 0x200, 0, 0, 0, 0};
static u8 sAreaGroupToFlag[6] = {0x00, 0x02, 0x04, 0x14, 0x04, 0x04};

// The area module of the enemy that asks (the module of every enemy is an emAreaModuleImpl).
static inline emAreaModuleImpl& getAreaModule(soModuleAccesser* acc) {
    return dynamic_cast<emAreaModuleImpl&>(acc->getAreaModule());
}

// The enemy that asks.
static inline Enemy& getEnemy(soModuleAccesser* acc) {
    return dynamic_cast<Enemy&>(acc->getStageObject());
}

// The center of the enemy that asks, written into the caller's vector. The assignment happens inside the inline
// helper so that the getCenterPos temporary is numbered after the by-value argument copies of the callers below
// (MWCC lays out expression temporaries in creation order; an assignment in the caller puts it above them).
static inline void getSelfCenterPos(soModuleAccesser* acc, Vec3f& center) {
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
}

extern char g_soTeamModuleNullArg[];

emAreaModuleImpl::emAreaModuleImpl(soModuleAccesser* acc, int areaNum, void* areaParams, void* targetRanges,
                                   soArray<soAreaInstance>* instances, void* areaWork, void* wind)
    : soAreaModuleImpl(acc, soAreaData::Category_Enemy, instances, &m_contactLogs, &m_envChecker,
                       static_cast<soArray<soAreaWind>*>(wind),
                       reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg), -1),
      m_moduleAccesser(acc), m_areaParams(static_cast<emAreaParam*>(areaParams)), m_contactLogs(0), m_envChecker() {
    m_areas.set(static_cast<soAreaData*>(areaWork), areaNum);
    m_targetRanges = static_cast<float*>(targetRanges);
}

emAreaModuleImpl::~emAreaModuleImpl() {
}

// The two sides of the cone of getTargetSide: the facing (1, 0) rotated by +/- half the opening angle.
void emAreaModuleImpl::setSideLineVec() {
    if (m_targetRanges != NULL) {
        float halfAngle = m_targetRanges[3] / 2.0f;
        Vec2f side;
        side.m_x = 1.0f;
        side.m_y = 0.0f;
        side.rot(0.017453292f * halfAngle, &m_sideLine0);
        side.rot(0.017453292f * -halfAngle, &m_sideLine1);
    }
}

// MATCH-ONLY: the original calls the validators and the table lookups out of line.
#pragma dont_inline on
bool emAreaModuleImpl::isInvalidAreaCommonKind(int kind) {
    if (kind <= -1 || kind >= 6) {
        return true;
    }
    return false;
}

// The gfArea::Group of an area kind (8, an invalid group, for an invalid kind).
u32 emAreaModuleImpl::getAreaCommonKindToGroup(u32 kind) {
    if (emAreaModuleImpl::isInvalidAreaCommonKind(kind) == true) {
        return 8;
    }
    return sAreaCommonKindToGroup[kind];
}

bool emAreaModuleImpl::isInvalidAreaGroup(int group) {
    if (group <= 8 || group >= 15) {
        return true;
    }
    return false;
}

bool emAreaModuleImpl::isInvalidAreaTarget(u32 area) {
    if ((s32)area <= -1 || (s32)area >= 8) {
        return true;
    }
    return false;
}

u32 emAreaModuleImpl::getAreaGroupToGroupMask(int group) {
    if (isInvalidAreaGroup(group) == true) {
        return 0;
    }
    return sAreaGroupToGroupMask[group - 9];
}

u32 emAreaModuleImpl::getAreaGroupToGroupMaskLocal(int group) {
    if (isInvalidAreaGroup(group) == true) {
        return 0;
    }
    return sAreaGroupToGroupMaskLocal[group - 9];
}

u8 emAreaModuleImpl::getAreaGroupToFlag(int group) {
    if (isInvalidAreaGroup(group) == true) {
        return 0;
    }
    return sAreaGroupToFlag[group - 9];
}
#pragma dont_inline reset

// The data set of an area index; NULL without data sets or past the area count (the soSet accessors are inlined).
soAreaData* emAreaModuleImpl::getAreaData(u32 index) {
    if (m_areas.elements() == NULL) {
        return NULL;
    }
    if (index >= m_areas.size()) {
        return NULL;
    }
    return &m_areas.elements()[index];
}

#pragma dont_inline on
void emAreaModuleImpl::setAreaDataFromGroup(s32 group, soAreaData* data) {
    if (emAreaModuleImpl::isInvalidAreaGroup(group) != true) {
        data->m_0x4 = emAreaModuleImpl::getAreaGroupToGroupMask(group);
        data->m_0x8 = emAreaModuleImpl::getAreaGroupToGroupMaskLocal(group);
        data->m_shapeFlag.m_mask = emAreaModuleImpl::getAreaGroupToFlag(group);
        data->m_group = group;
    }
}

#pragma dont_inline reset

bool emAreaModuleImpl::isFighterTargetKind(u32 area) {
    return area <= 3;
}

void emAreaModuleImpl::createAreaDataSetSubParam(u32 index, emAreaParam* param) {
    soAreaData* data = getAreaData(index);
    if (data != NULL) {
        s32 group = param->m_group;
        data->m_shapeType = param->m_shapeType;
        setAreaDataFromGroup(group, data);
        if (param->m_nodeIndex >= 0) {
            data->m_nodeIndex = param->m_nodeIndex;
            data->m_shapeFlag.m_mask |= soAreaData::SHAPE_MASK_FOLLOW_NODE | soAreaData::SHAPE_MASK_APPLY_OFFSET_ROT;
        } else {
            data->m_nodeIndex = 0;
        }
        data->m_offsetPos.m_x = param->m_offsetX;
        data->m_offsetPos.m_y = param->m_offsetY;
        data->m_range.m_x = param->m_rangeX;
        data->m_range.m_y = param->m_rangeY;
        float levelScale = soValueAccesser::getValueFloat(m_moduleAccesser, 0x1100000C, 0);
        if (0.0f != param->m_rangeXScale) {
            data->m_range.m_x += param->m_rangeXScale * levelScale;
        }
        if (0.0f != param->m_rangeYScale) {
            data->m_range.m_y += param->m_rangeYScale * levelScale;
        }
        // HYPOTHESIS: enemy group 12 has no area of its own and stands for the wind space of the stages.
        if (data->m_group == 12) {
            data->m_group = gfArea::Stage_Group_Space_Wind;
        }
    }
}

void emAreaModuleImpl::createAreaDataSet() {
    emAreaParam* params = m_areaParams;
    if (m_areas.elements() != NULL) {
        soAreaData* data = getAreaData(0);
        if (data != NULL) {
            u32 group = getAreaCommonKindToGroup(0);
            data->m_shapeType = soAreaInstance::Shape_Rectangle_Ref_Ground_Rhombus;
            setAreaDataFromGroup(group, data);
            data->m_nodeIndex = 0;
            data->m_offsetPos.m_x = 0.0f;
            data->m_offsetPos.m_y = 0.0f;
            data->m_range.m_x = 0.0f;
            data->m_range.m_y = 0.0f;
        }
        if (params != NULL) {
            for (u32 i = 6; i < m_areas.size(); i++) {
                createAreaDataSetSubParam(i, &params[i - 6]);
            }
        }
    }
}

void emAreaModuleImpl::createAreaInstance() {
    setup(&m_areas);
}

void emAreaModuleImpl::activate(int ownerId) {
    soAreaModuleImpl::activate(ownerId);
    setSideLineVec();
}

// Whether an object of the target kind touches the area (an area index of this module) according to the contact logs.
// HYPOTHESIS: the meaning of the kinds: 0-3 the fighters (the player's fighter and its sub fighter, player 1 then
// player 2), 4 this enemy, 5 its parent, 6 and 7 anything.
bool emAreaModuleImpl::checkArea(soModuleAccesser* acc, u32 area, u32 kind) {
    int logNum = getContactLogCount();
    if (logNum <= 0) {
        return false;
    }
    for (int i = 0; i < logNum; i++) {
        soAreaContactLog* log = getContactLog(i);
        if (area != log->m_area) {
            continue;
        }
        if (kind <= 3) {
            if (log->m_category != soAreaData::Category_Fighter) {
                continue;
            }
            int instanceIndex = 0;
            int entryId = g_ftManager->getEntryIdFromAreaId(log->m_areaId, &instanceIndex);
            if (entryId == -1) {
                continue;
            }
            Fighter* fighter = g_ftManager->getFighter(entryId, instanceIndex);
            Fighter* mainFighter = g_ftManager->getFighter(entryId, -1);
            if (kind <= 1) {
                if (g_ftManager->getPlayerNo(entryId) != 0) {
                    continue;
                }
                if ((s32)kind == 0) {
                    if (fighter == mainFighter) {
                        return true;
                    }
                } else if ((s32)kind == 1) {
                    if (fighter != mainFighter) {
                        return true;
                    }
                }
            } else if (kind - 2 <= 1) {
                if (g_ftManager->getPlayerNo(entryId) != 1) {
                    continue;
                }
                if ((s32)kind == 2) {
                    if (fighter == mainFighter) {
                        return true;
                    }
                } else if ((s32)kind == 3) {
                    if (fighter != mainFighter) {
                        return true;
                    }
                }
            }
        } else if (kind - 4 <= 1) {
            if (log->m_category != soAreaData::Category_Enemy) {
                continue;
            }
            u32 contactCreateId = emManager::getInstance()->getCreateIdFromAreaId(log->m_areaId);
            if (contactCreateId == emManager::InvalidCreateId) {
                continue;
            }
            u32 createId = getEnemy(acc).getCreateId();
            if ((s32)kind == 4) {
                if (createId == contactCreateId) {
                    return true;
                }
            } else if ((s32)kind == 5) {
                u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
                if (parentCreateId == emManager::InvalidCreateId) {
                    continue;
                }
                if (parentCreateId == contactCreateId) {
                    return true;
                }
            }
        } else if ((s32)kind == 6) {
            return true;
        } else if ((s32)kind == 7) {
            return true;
        } else {
            break;
        }
    }
    return false;
}

bool emAreaModuleImpl::isTargetFighter(soModuleAccesser* acc, u32 area) {
    return area <= 3;
}

// The fighter of a target kind 0-3, NULL when it is gone or not accessible.
Fighter* emAreaModuleImpl::getTargetFighterPtr(u32 area) {
    // MATCH-ONLY: dead local (the original builds a position here and never uses it).
    Vec3f unused(0.0f, 0.0f, 0.0f);
    if (area <= 3) {
        int entryId = -1;
        if (area <= 1) {
            entryId = g_ftManager->getEntryId(0);
            if (entryId == -1) {
                return NULL;
            }
        } else if (area - 2 <= 1) {
            entryId = g_ftManager->getEntryId(1);
            if (entryId == -1) {
                return NULL;
            }
        }
        if ((s32)area == emAreaModuleImpl::TargetPlayer1 || (s32)area == emAreaModuleImpl::TargetPlayer2) {
            if (!g_ftManager->isFighterPositionAccessable(entryId, -1)) {
                return NULL;
            }
        } else if ((s32)area == emAreaModuleImpl::TargetPlayer1Sub || (s32)area == emAreaModuleImpl::TargetPlayer2Sub) {
            if (!g_ftManager->isSubFighterPositionAccessable(entryId)) {
                return NULL;
            }
        }
        Fighter* fighter = NULL;
        if ((s32)area == emAreaModuleImpl::TargetPlayer1 || (s32)area == emAreaModuleImpl::TargetPlayer2) {
            fighter = g_ftManager->getFighter(entryId, -1);
        } else if ((s32)area == emAreaModuleImpl::TargetPlayer1Sub || (s32)area == emAreaModuleImpl::TargetPlayer2Sub) {
            fighter = g_ftManager->getSubFighter(entryId);
        }
        return fighter;
    }
    return NULL;
}

bool emAreaModuleImpl::isExistTarget(soModuleAccesser* acc, u32 area) {
    if (area <= 3) {
        return getTargetFighterPtr(area) != NULL;
    }
    if (area - 4 <= 1) {
        u32 createId = getEnemy(acc).getCreateId();
        if ((s32)area == TargetSelf) {
            return true;
        }
        if ((s32)area == TargetParent) {
            u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
            if (parentCreateId == emManager::InvalidCreateId) {
                return false;
            }
            if (emManager::getInstance()->getEnemyPtrFromId(parentCreateId) == NULL) {
                return false;
            }
        }
        return true;
    }
    if ((s32)area == TargetCamera) {
        return true;
    }
    return (s32)area == TargetUnk7;
}

Vec3f emAreaModuleImpl::getTargetPos(soModuleAccesser* acc, u32 area, bool center) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    if (area <= 3) {
        Fighter* fighter = getTargetFighterPtr(area);
        if (fighter == NULL) {
            return pos;
        }
        if (!center) {
            pos = soExternalValueAccesser::getPos(fighter);
        } else {
            pos = ftExternalValueAccesser::getHipPos(fighter);
        }
    } else if (area - 4 <= 1) {
        Enemy& self = getEnemy(acc);
        u32 createId = self.getCreateId();
        if ((s32)area == TargetSelf) {
            if (!center) {
                pos = soExternalValueAccesser::getPos(&self);
            } else {
                pos = emExternalValueAccesser::getCenterPos(&self);
            }
        } else if ((s32)area == TargetParent) {
            u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
            if (parentCreateId == emManager::InvalidCreateId) {
                return pos;
            }
            Enemy* parent = emManager::getInstance()->getEnemyPtrFromId(parentCreateId);
            if (parent == NULL) {
                return pos;
            }
            if (!center) {
                pos = soExternalValueAccesser::getPos(parent);
            } else {
                pos = emExternalValueAccesser::getCenterPos(parent);
            }
        }
    } else if ((s32)area == TargetCamera) {
        pos = acc->getCameraModule().getMainCameraTargetPos();
    }
    return pos;
}

s32 emAreaModuleImpl::getTargetStatusKind(soModuleAccesser* acc, u32 area) {
    s32 statusKind = -1;
    if (area <= 3) {
        Fighter* fighter = getTargetFighterPtr(area);
        if (fighter == NULL) {
            return -1;
        }
        return soExternalValueAccesser::getStatusKind(fighter);
    }
    if (area - 4 <= 1) {
        Enemy& self = getEnemy(acc);
        u32 createId = self.getCreateId();
        if ((s32)area == TargetSelf) {
            statusKind = soExternalValueAccesser::getStatusKind(&self);
        } else if ((s32)area == TargetParent) {
            u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
            if (parentCreateId == emManager::InvalidCreateId) {
                return -1;
            }
            Enemy* parent = emManager::getInstance()->getEnemyPtrFromId(parentCreateId);
            if (parent == NULL) {
                return -1;
            }
            statusKind = soExternalValueAccesser::getStatusKind(parent);
        }
        return statusKind;
    }
    if ((s32)area == TargetCamera) {
        // MATCH-ONLY: the camera case (also -1) keeps the original's compare only through the narrowing cast (found by the permuter).
        return (s8)statusKind;
    }
    return -1;
}

float emAreaModuleImpl::getTargetPostureLr(soModuleAccesser* acc, u32 area) {
    float lr = 0.0f;
    if (area <= 3) {
        Fighter* fighter = getTargetFighterPtr(area);
        if (fighter == NULL) {
            return lr;
        } else {
            lr = soExternalValueAccesser::getLr(fighter);
            return lr;
        }
    }
    if (area - 4 <= 1) {
        Enemy& self = getEnemy(acc);
        u32 createId = self.getCreateId();
        if ((s32)area == TargetSelf) {
            lr = soExternalValueAccesser::getLr(&self);
        } else if ((s32)area == TargetParent) {
            u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
            if (parentCreateId == emManager::InvalidCreateId) {
                return lr;
            }
            Enemy* parent = emManager::getInstance()->getEnemyPtrFromId(parentCreateId);
            if (parent == NULL) {
                return lr;
            }
            lr = soExternalValueAccesser::getLr(parent);
        }
        return lr;
    }
    if ((s32)area == TargetCamera) {
        return 1.0f;
    }
    if ((s32)area == TargetUnk7) {
        return 1.0f;
    }
    return lr;
}

float emAreaModuleImpl::getTargetPostureScl(soModuleAccesser* acc, u32 area) {
    float scl = 1.0f;
    if (area <= 3) {
        Fighter* fighter = getTargetFighterPtr(area);
        if (fighter == NULL) {
            return scl;
        } else {
            scl = soExternalValueAccesser::getScl(fighter);
            return scl;
        }
    }
    if (area - 4 <= 1) {
        Enemy& self = getEnemy(acc);
        u32 createId = self.getCreateId();
        if ((s32)area == TargetSelf) {
            scl = soExternalValueAccesser::getScl(&self);
        } else if ((s32)area == TargetParent) {
            u32 parentCreateId = emManager::getInstance()->getEnemyParentCreateId(createId);
            if (parentCreateId == emManager::InvalidCreateId) {
                return scl;
            }
            Enemy* parent = emManager::getInstance()->getEnemyPtrFromId(parentCreateId);
            if (parent == NULL) {
                return scl;
            }
            scl = soExternalValueAccesser::getScl(parent);
        }
        return scl;
    }
    if ((s32)area == TargetCamera) {
        return 1.0f;
    }
    if ((s32)area == TargetUnk7) {
        return 1.0f;
    }
    return scl;
}

// The radians to degrees factor of the angle queries.
static const float RadToDeg = 57.29578f;

// MATCH-ONLY: getTargetAxisSideSub takes the 2D position of the enemy's center through an inline (x, y) setter: its
// arguments load y before x, which neither the Vec2f(const Vec3f&) constructor nor member stores do. (A second caller
// would keep it out of line.)
static inline void setVec2f(Vec2f& v, float x, float y) {
    v.m_x = x;
    v.m_y = y;
}

// The dot product (taking the difference temporary by reference, so it is not copied).
static inline float dotVec2f(const Vec2f& a, const Vec2f& b) {
    return a.m_x * b.m_x + a.m_y * b.m_y;
}

// The angle (in frames of a rotation motion of frameNum frames) from the direction dir to the target at pos + offset,
// as seen from the center of this enemy; -1 when there is nothing to measure, 0 when rotSign is neither 1 nor -1.
float emAreaModuleImpl::getTargetAngleToMotionFrameSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f dir,
                                                       float rotSign, float frameNum) {
    if (dir.length() == 0.0f) {
        return -1.0f;
    }
    if (frameNum <= 0.0f) {
        return -1.0f;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    // MATCH-ONLY: the 2D center is read y first (as the right-to-left arguments of an inlined Vec2f(x, y) constructor
    // would be); Vec2f(const Vec3f&) and member stores read x first, and an inline (x, y) helper stays out of line once
    // more than one function calls it. Same in getTargetSideSub, getTargetRotSpeedDirForAxisAdjustSub and
    // getTargetAngleFromAxisSub.
    float x, y;
    y = center.m_y;
    x = center.m_x;
    Vec2f self;
    self.m_x = x;
    self.m_y = y;
    Vec2f target(pos->m_x + offset.m_x, pos->m_y + offset.m_y);
    Vec2f diff = target - self;
    if (diff.length() == 0.0f) {
        return -1.0f;
    }
    float dirAngle = RadToDeg * atan2f(dir.m_y, dir.m_x);
    float diffAngle = RadToDeg * atan2f(diff.m_y, diff.m_x);
    float angle = dirAngle - diffAngle;
    if (1.0f != rotSign) {
        if (-1.0f == rotSign) {
            angle *= -1.0f;
        } else {
            return 0.0f;
        }
    }
    if (angle < 0.0f) {
        angle = 360.0f - angle;
    }
    return frameNum * angle / 360.0f;
}

// The offset of a target flipped for the facing lr, turned by rot (radians) and scaled by scl (a weak copy).
inline Vec2f soPostureModule::adjustPosOffset(Vec2f offset, float lr, float scl, float rot) {
    offset.m_x = offset.m_x * lr;
    offset.rot(rot, &offset);
    offset.m_x = offset.m_x * scl;
    offset.m_y = offset.m_y * scl;
    return offset;
}

float emAreaModuleImpl::getTargetAngleToMotionFrame(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f dir,
                                                    float rotSign, float frameNum, u32 useLr) {
    Vec3f pos = getTargetPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetPostureScl(acc, area), 0.0f);
    return getTargetAngleToMotionFrameSub(acc, &pos, offset, dir, rotSign, frameNum);
}

// The distance class (0-2: the first range the target is inside, 3: farther, or no ranges) of a position.
// MATCH-ONLY: the guarded while loop (found by the permuter) is the form that allocates the area module and the
// counter the way the original does; the plain for / while loops swap r30 and r31.
s32 emAreaModuleImpl::getTargetDistanceSub(soModuleAccesser* acc, Vec3f* pos) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (areaModule.m_targetRanges == NULL) {
        return 3;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    s32 i = 0;
    if (i < 3) {
        while (i < 3) {
            if (center.distance(pos) <= areaModule.m_targetRanges[i]) {
                return i;
            }
            i++;
        }
    }
    return 3;
}

s32 emAreaModuleImpl::getTargetDistance(soModuleAccesser* acc, u32 area) {
    Vec3f pos = getTargetPos(acc, area, true);
    return getTargetDistanceSub(acc, &pos);
}

float emAreaModuleImpl::getTargetRealDistanceSub(soModuleAccesser* acc, Vec3f* pos) {
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    return center.distance(pos);
}

float emAreaModuleImpl::getTargetRealDistance(soModuleAccesser* acc, u32 area) {
    Vec3f pos = getTargetPos(acc, area, true);
    return getTargetRealDistanceSub(acc, &pos);
}

// 0 when the target is to the right of this enemy (or in line with it), 1 when it is to the left.
s32 emAreaModuleImpl::getTargetDirSub(soModuleAccesser* acc, Vec3f* pos) {
    Vec3f selfPos(0.0f, 0.0f, 0.0f);
    selfPos = soExternalValueAccesser::getPos(&getEnemy(acc));
    // MATCH-ONLY: the complementary else-if keeps the original branches (a plain if / return folds into mfcr / cntlzw).
    if (selfPos.m_x <= pos->m_x) {
        return 0;
    } else if (selfPos.m_x > pos->m_x) {
        return 1;
    }
    return 1;
}

// Whether the direction dir (getTargetDirSub) is the asked kind: 0 right, 1 left, 2 in front of this enemy, 3 behind.
bool emAreaModuleImpl::isTargetDirSub(soModuleAccesser* acc, s32 dir, s32 kind) {
    if (kind == 0) {
        if (dir == kind) {
            return true;
        }
    } else if (kind == 1) {
        if (dir == kind) {
            return true;
        }
    } else if (kind == 2) {
        float lr = acc->getPostureModule().getLr();
        if (1.0f == lr) {
            if (dir == 0) {
                return true;
            }
        } else if (-1.0f == lr) {
            if (dir == 1) {
                return true;
            }
        } else {
            return false;
        }
    } else if (kind == 3) {
        float lr = acc->getPostureModule().getLr();
        if (1.0f == lr) {
            if (dir == 1) {
                return true;
            }
        } else if (-1.0f == lr) {
            if (dir == 0) {
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    return false;
}

bool emAreaModuleImpl::isTargetDir(soModuleAccesser* acc, u32 area, s32 kind) {
    Vec3f pos = getTargetPos(acc, area, true);
    return isTargetDirSub(acc, getTargetDirSub(acc, &pos), kind);
}

// The side of the cone in front of this enemy (getTargetSide's two side lines) the position is on: 0 inside both
// halves, 1 outside both, 2 and 3 between them; 6 without ranges or in no case.
s32 emAreaModuleImpl::getTargetSideSub(soModuleAccesser* acc, Vec3f* pos) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (areaModule.m_targetRanges == NULL) {
        return 6;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    // MATCH-ONLY: y read first (see getTargetAngleToMotionFrameSub).
    float x, y;
    y = center.m_y;
    x = center.m_x;
    Vec2f origin;
    origin.m_x = x;
    origin.m_y = y;
    clRay2D ray0(origin, areaModule.m_sideLine0);
    clLine2DParam side0(ray0, false);
    float dist0 = side0.distance(Vec2f(pos->m_x, pos->m_y));
    clRay2D ray1(origin, areaModule.m_sideLine1);
    clLine2DParam side1(ray1, false);
    float dist1 = side1.distance(Vec2f(pos->m_x, pos->m_y));
    if (dist0 >= 0.0f && dist1 >= 0.0f) {
        return 0;
    }
    if (dist0 <= 0.0f && dist1 <= 0.0f) {
        return 1;
    }
    if (dist0 <= 0.0f && dist1 >= 0.0f) {
        return 2;
    }
    if (dist0 >= 0.0f && dist1 <= 0.0f) {
        return 3;
    }
    return 6;
}

// Whether the side (getTargetSideSub) is the asked kind: 0-3 the side itself, 4 and 5 the side in front of / behind
// this enemy for its facing.
bool emAreaModuleImpl::isTargetSideSub(soModuleAccesser* acc, s32 side, s32 kind) {
    if (kind == 0) {
        if (side == kind) {
            return true;
        }
    } else if (kind == 1) {
        if (side == kind) {
            return true;
        }
    } else if (kind == 2) {
        if (side == kind) {
            return true;
        }
    } else if (kind == 3) {
        if (side == kind) {
            return true;
        }
    } else if (kind == 4) {
        float lr = acc->getPostureModule().getLr();
        if (1.0f == lr) {
            if (side == 2) {
                return true;
            }
        } else if (-1.0f == lr) {
            if (side == 3) {
                return true;
            }
        } else {
            return false;
        }
    } else if (kind == 5) {
        float lr = acc->getPostureModule().getLr();
        if (1.0f == lr) {
            if (side == 3) {
                return true;
            }
        } else if (-1.0f == lr) {
            if (side == 2) {
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    return false;
}

bool emAreaModuleImpl::isTargetSide(soModuleAccesser* acc, u32 area, s32 kind) {
    Vec3f pos = getTargetPos(acc, area, true);
    return isTargetSideSub(acc, getTargetSideSub(acc, &pos), kind);
}

// Which side of the axis (a direction through this enemy) the target at pos + offset is on: 1 on the side the axis
// points to, 2 on the other, 0 on the axis, 3 without an axis.
s32 emAreaModuleImpl::getTargetAxisSideSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis) {
    getAreaModule(acc);
    if (axis.length() == 0.0f) {
        return 3;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    Vec2f self;
    setVec2f(self, center.m_x, center.m_y);
    Vec2f target(pos->m_x + offset.m_x, pos->m_y + offset.m_y);
    float dot = dotVec2f(axis, target - self);
    if (0.0f == dot) {
        return 0;
    }
    if (dot > 0.0f) {
        return 1;
    }
    if (dot < 0.0f) {
        return 2;
    }
    return 3;
}

s32 emAreaModuleImpl::getTargetAxisSide(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr) {
    Vec3f pos = getTargetPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetPostureScl(acc, area), 0.0f);
    return getTargetAxisSideSub(acc, &pos, offset, axis);
}

// Which way the axis has to turn to point at the target at pos + offset: 0 and 1 the two directions, 2 already
// pointing at it, 3 without an axis.
s32 emAreaModuleImpl::getTargetRotSpeedDirForAxisAdjustSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis) {
    getAreaModule(acc);
    if (axis.length() == 0.0f) {
        return 3;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    // MATCH-ONLY: y read first (see getTargetAngleToMotionFrameSub).
    float x, y;
    y = center.m_y;
    x = center.m_x;
    Vec2f self;
    self.m_x = x;
    self.m_y = y;
    Vec2f target(pos->m_x + offset.m_x, pos->m_y + offset.m_y);
    Vec2f diff = target - self;
    float axisAngle = RadToDeg * atan2f(axis.m_y, axis.m_x);
    float diffAngle = RadToDeg * atan2f(diff.m_y, diff.m_x);
    float turn = diffAngle - axisAngle;
    // MATCH-ONLY: the complementary else-ifs keep the original branches (see getTargetDirSub).
    if (turn > 0.0f) {
        if ((float)fabs(turn) <= 180.0f) {
            return 0;
        } else if ((float)fabs(turn) > 180.0f) {
            return 1;
        }
        return 1;
    }
    if (turn < 0.0f) {
        if ((float)fabs(turn) <= 180.0f) {
            return 1;
        } else if ((float)fabs(turn) > 180.0f) {
            return 0;
        }
        return 0;
    }
    return 2;
}

s32 emAreaModuleImpl::getTargetRotSpeedDirForAxisAdjust(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis,
                                                        u32 useLr) {
    Vec3f pos = getTargetPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetPostureScl(acc, area), 0.0f);
    return getTargetRotSpeedDirForAxisAdjustSub(acc, &pos, offset, axis);
}

// The angle in degrees between the axis and the direction to the target at pos + offset; 3 without an axis.
float emAreaModuleImpl::getTargetAngleFromAxisSub(soModuleAccesser* acc, Vec3f* pos, Vec2f offset, Vec2f axis) {
    getAreaModule(acc);
    if (axis.length() == 0.0f) {
        return 3.0f;
    }
    Vec3f center(0.0f, 0.0f, 0.0f);
    center = emExternalValueAccesser::getCenterPos(&getEnemy(acc));
    // MATCH-ONLY: y read first (see getTargetAngleToMotionFrameSub).
    float x, y;
    y = center.m_y;
    x = center.m_x;
    Vec2f self;
    self.m_x = x;
    self.m_y = y;
    Vec2f target(pos->m_x + offset.m_x, pos->m_y + offset.m_y);
    Vec2f diff = target - self;
    return RadToDeg * vec2fAngle(&axis, &diff);
}

float emAreaModuleImpl::getTargetAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr) {
    Vec3f pos = getTargetPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetPostureScl(acc, area), 0.0f);
    return getTargetAngleFromAxisSub(acc, &pos, offset, axis);
}

// Whether the angle of getTargetAngleFromAxis is within +/- limit. The code is exact; symbols.txt still calls it (and
// isTargetFoundAngleFromAxis) fn_41_2536C / fn_41_25B54 because the general term unit calls them by those names.
bool emAreaModuleImpl::isTargetAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, float limit,
                                             u32 useLr) {
    float angle = getTargetAngleFromAxis(acc, area, offset, axis, useLr);
    float absLimit = fabs(limit);
    float absAngle = fabs(angle);
    return absAngle <= absLimit;
}

// Keeps the position of a target (area index, or TargetAll for every existing target) as its found position.
void emAreaModuleImpl::setTargetFoundPos(soModuleAccesser* acc, s32 area) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (area == TargetAll) {
        for (s32 i = 0; i < 8; i++) {
            if (isExistTarget(acc, i)) {
                areaModule.m_foundPos[0][i] = getTargetPos(acc, i, false);
                areaModule.m_foundPos[1][i] = getTargetPos(acc, i, true);
            }
        }
    } else {
        if (area <= -1 || area >= 8) {
            return;
        }
        areaModule.m_foundPos[0][area] = getTargetPos(acc, area, false);
        areaModule.m_foundPos[1][area] = getTargetPos(acc, area, true);
    }
}

Vec3f emAreaModuleImpl::getTargetFoundPos(soModuleAccesser* acc, s32 area, bool center) {
    if (area <= -1 || area >= 8) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (!center) {
        return areaModule.m_foundPos[0][area];
    }
    return areaModule.m_foundPos[1][area];
}

float emAreaModuleImpl::getTargetFoundAngleToMotionFrame(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f dir,
                                                         float rotSign, float frameNum, u32 useLr) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetFoundPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetFoundPostureScl(acc, area), 0.0f);
    return getTargetAngleToMotionFrameSub(acc, &pos, offset, dir, rotSign, frameNum);
}

s32 emAreaModuleImpl::getTargetFoundDistance(soModuleAccesser* acc, s32 area) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    return getTargetDistanceSub(acc, &pos);
}

// The distance to the found position of a target, the found counterpart of getTargetRealDistance (only the anim cmds
// use it, inline). HYPOTHESIS: name.
inline float emAreaModuleImpl::getTargetFoundRealDistance(soModuleAccesser* acc, s32 area) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    return getTargetRealDistanceSub(acc, &pos);
}

bool emAreaModuleImpl::isTargetFoundDir(soModuleAccesser* acc, s32 area, s32 kind) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    return isTargetDirSub(acc, getTargetDirSub(acc, &pos), kind);
}

bool emAreaModuleImpl::isTargetFoundSide(soModuleAccesser* acc, s32 area, s32 kind) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    return isTargetSideSub(acc, getTargetSideSub(acc, &pos), kind);
}

s32 emAreaModuleImpl::getTargetFoundAxisSide(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis, u32 useLr) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetFoundPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetFoundPostureScl(acc, area), 0.0f);
    return getTargetAxisSideSub(acc, &pos, offset, axis);
}

s32 emAreaModuleImpl::getTargetFoundRotSpeedDirForAxisAdjust(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis,
                                                             u32 useLr) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetFoundPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetFoundPostureScl(acc, area), 0.0f);
    return getTargetRotSpeedDirForAxisAdjustSub(acc, &pos, offset, axis);
}

float emAreaModuleImpl::getTargetFoundAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis,
                                                    u32 useLr) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetFoundPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetFoundPostureScl(acc, area), 0.0f);
    return getTargetAngleFromAxisSub(acc, &pos, offset, axis);
}

bool emAreaModuleImpl::isTargetFoundAngleFromAxis(soModuleAccesser* acc, u32 area, Vec2f offset, Vec2f axis,
                                                  float limit, u32 useLr) {
    float angle = getTargetFoundAngleFromAxis(acc, area, offset, axis, useLr);
    float absLimit = fabs(limit);
    float absAngle = fabs(angle);
    return absAngle <= absLimit;
}

// The angle of the kinetic speed that moves self toward target; 0 when they are at the same place.
float emAreaModuleImpl::getTargetFoundKineticAngleSub(float lr, Vec3f self, Vec3f target) {
    Vec2f speed(target.m_x - self.m_x, target.m_y - self.m_y);
    if (speed.length() == 0.0f) {
        return 0.0f;
    }
    return emKineticModuleImpl::getKineticAngleFromSpeed(lr, speed);
}

float emAreaModuleImpl::getTargetFoundKineticAngle(soModuleAccesser* acc, u32 area, Vec2f offset, u32 useLr) {
    Vec3f pos = getTargetFoundPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetFoundPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetFoundPostureScl(acc, area), 0.0f);
    pos.m_x += offset.m_x;
    pos.m_y += offset.m_y;
    Vec3f self(0.0f, 0.0f, 0.0f);
    getSelfCenterPos(acc, self);
    float selfLr = acc->getPostureModule().getLr();
    return getTargetFoundKineticAngleSub(selfLr, self, pos);
}

float emAreaModuleImpl::getTargetKineticAngle(soModuleAccesser* acc, u32 area, Vec2f offset, u32 useLr) {
    Vec3f pos = getTargetPos(acc, area, true);
    float lr = 1.0f;
    if (useLr == 1) {
        lr = getTargetPostureLr(acc, area);
    }
    offset = soPostureModule::adjustPosOffset(offset, lr, getTargetPostureScl(acc, area), 0.0f);
    pos.m_x += offset.m_x;
    pos.m_y += offset.m_y;
    Vec3f self(0.0f, 0.0f, 0.0f);
    getSelfCenterPos(acc, self);
    float selfLr = acc->getPostureModule().getLr();
    return getTargetFoundKineticAngleSub(selfLr, self, pos);
}

void emAreaModuleImpl::setTargetFoundStatusKind(soModuleAccesser* acc, s32 area) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (area == TargetAll) {
        for (s32 i = 0; i < 8; i++) {
            if (isExistTarget(acc, i)) {
                areaModule.m_foundStatusKind[i] = getTargetStatusKind(acc, i);
            }
        }
    } else {
        if (area <= -1 || area >= 8) {
            return;
        }
        areaModule.m_foundStatusKind[area] = getTargetStatusKind(acc, area);
    }
}

s32 emAreaModuleImpl::getTargetFoundStatusKind(soModuleAccesser* acc, s32 area) {
    if (area <= -1 || area >= 8) {
        return -1;
    }
    return getAreaModule(acc).m_foundStatusKind[area];
}

void emAreaModuleImpl::setTargetFoundPostureLr(soModuleAccesser* acc, s32 area) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (area == TargetAll) {
        for (s32 i = 0; i < 8; i++) {
            if (isExistTarget(acc, i)) {
                areaModule.m_foundLr[i] = getTargetPostureLr(acc, i);
            }
        }
    } else {
        if (area <= -1 || area >= 8) {
            return;
        }
        areaModule.m_foundLr[area] = getTargetPostureLr(acc, area);
    }
}

float emAreaModuleImpl::getTargetFoundPostureLr(soModuleAccesser* acc, s32 area) {
    if (area <= -1 || area >= 8) {
        return -1.0f;
    }
    return getAreaModule(acc).m_foundLr[area];
}

void emAreaModuleImpl::setTargetFoundPostureScl(soModuleAccesser* acc, s32 area) {
    emAreaModuleImpl& areaModule = getAreaModule(acc);
    if (area == TargetAll) {
        for (s32 i = 0; i < 8; i++) {
            if (isExistTarget(acc, i)) {
                areaModule.m_foundScl[i] = getTargetPostureScl(acc, i);
            }
        }
    } else {
        if (area <= -1 || area >= 8) {
            return;
        }
        areaModule.m_foundScl[area] = getTargetPostureScl(acc, area);
    }
}

float emAreaModuleImpl::getTargetFoundPostureScl(soModuleAccesser* acc, s32 area) {
    if (area <= -1 || area >= 8) {
        return -1.0f;
    }
    return getAreaModule(acc).m_foundScl[area];
}

bool emAreaModuleImpl::isTargetFighterDownWait(soModuleAccesser* acc, u32 area) {
    if (area <= 3) {
        return fn_27_14D530(getTargetStatusKind(acc, area));
    }
    return false;
}

bool emAreaModuleImpl::isTargetFighterSmashHold(soModuleAccesser* acc, u32 area) {
    if (area <= 3) {
        return fn_27_14D550(getTargetStatusKind(acc, area));
    }
    return false;
}

bool emAreaModuleImpl::isTargetFighterFoundDownWait(soModuleAccesser* acc, u32 area) {
    if (area <= 3) {
        return fn_27_14D530(getTargetFoundStatusKind(acc, area));
    }
    return false;
}

bool emAreaModuleImpl::isTargetFighterFoundSmashHold(soModuleAccesser* acc, u32 area) {
    if (area <= 3) {
        return fn_27_14D550(getTargetFoundStatusKind(acc, area));
    }
    return false;
}

// Keeps the position, status and posture of a target (or of every existing target, for TargetAll) as its found
// information.
void emAreaModuleImpl::setTargetFoundInfo(soModuleAccesser* acc, u32 area) {
    if ((s32)area == TargetAll) {
        for (s32 i = 0; i < 8; i++) {
            if (isExistTarget(acc, i)) {
                setTargetFoundPos(acc, area);
                setTargetFoundStatusKind(acc, area);
                setTargetFoundPostureLr(acc, area);
                setTargetFoundPostureScl(acc, area);
            }
        }
    } else {
        setTargetFoundPos(acc, area);
        setTargetFoundStatusKind(acc, area);
        setTargetFoundPostureLr(acc, area);
        setTargetFoundPostureScl(acc, area);
    }
}

bool emAreaModuleImpl::isObserv(char group) {
    return group == 0x14 || group == 'i';
}

// The argument at index of an anim cmd (the acCmdArg getters stay out of line in this unit, as in the original).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

// MATCH-ONLY: statement macros, because the original expands these reads in place (as in Enemy::notifyEventAnimCmd).
// An argument is either a plain value or a variable (a value index the enemy's value accesser resolves).
#define READ_FLOAT_ARG(args, index, acc, out)                                                                          \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueFloat(acc, getCmdArg(args, index).getValueIndexData(), 0);                      \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getFloatData();                                                                   \
    }
// An integer argument that is a variable is read as a float and truncated.
#define READ_INT_ARG(args, index, acc, out)                                                                            \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = (s32)soValueAccesser::getValueFloat(acc, getCmdArg(args, index).getValueIndexData(), 0);                 \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getIntData();                                                                     \
    }

// The anim cmds of the area module (group 'i'); the other groups go to soAreaModuleImpl. A command with the wrong
// number of arguments is ignored. The ones that write a result take the work float to store it in as a variable
// argument (the work manage module slot). HYPOTHESIS: all command names.
//   0: keep the found information of a target (getTargetFoundInfo)
//   1, 2: the kinetic angle toward the found (1) or the live (2) target, from an offset
//   3, 4: the angle to a motion frame toward the found (3) or the live (4) target
//   5, 6: the distance to the live (5) or the found (6) target
//   7, 8, 9, 10: the x of the live target, the x of the found target, the y of the live target, the y of the found target
bool emAreaModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (acmd->getGroup() != 'i') {
        return soAreaModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 11) {
        return false;
    }
    switch (type) {
    case 0: {
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 area;
        acCmdArg areaArg = getCmdArg(args, 0);
        if (areaArg.getArgType() == CmdArgType_ValueIndex) {
            area = (s32)soValueAccesser::getValueFloat(moduleAccesser, areaArg.getValueIndexData(), 0);
        } else {
            area = areaArg.getIntData();
        }
        setTargetFoundInfo(moduleAccesser, area);
        return true;
    }
    case 1:
    case 2: {
        if (argNum < 5 || argNum > 5) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 area;
        READ_INT_ARG(args, 0, moduleAccesser, area)
        Vec3f offset(0.0f, 0.0f, 0.0f);
        READ_FLOAT_ARG(args, 1, moduleAccesser, offset.m_x)
        READ_FLOAT_ARG(args, 2, moduleAccesser, offset.m_y)
        if (getCmdArg(args, 3).getArgType() != CmdArgType_ValueIndex) {
            return true;
        }
        u32 workId = getCmdArg(args, 3).getValueIndexData();
        bool useLr = getCmdArg(args, 4).getBoolData();
        float result = 0.0f;
        if (type == 1) {
            result = getTargetFoundKineticAngle(moduleAccesser, area, offset, useLr);
        } else if (type == 2) {
            result = getTargetKineticAngle(moduleAccesser, area, offset, useLr);
        }
        moduleAccesser->getWorkManageModule().setFloat(result, workId);
        return true;
    }
    case 3:
    case 4: {
        if (argNum < 9 || argNum > 9) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 area;
        READ_INT_ARG(args, 0, moduleAccesser, area)
        Vec3f offset(0.0f, 0.0f, 0.0f);
        READ_FLOAT_ARG(args, 1, moduleAccesser, offset.m_x)
        READ_FLOAT_ARG(args, 2, moduleAccesser, offset.m_y)
        Vec2f dir;
        dir.m_x = 0.0f;
        dir.m_y = 0.0f;
        READ_FLOAT_ARG(args, 3, moduleAccesser, dir.m_x)
        READ_FLOAT_ARG(args, 4, moduleAccesser, dir.m_y)
        // MATCH-ONLY: a dead default (both branches of the read assign it); its third literal makes MWCC address the
        // literal pool of the function through a base register, as the original does.
        float rotSign = 1.0f;
        READ_FLOAT_ARG(args, 5, moduleAccesser, rotSign)
        float frameNum;
        READ_FLOAT_ARG(args, 6, moduleAccesser, frameNum)
        if (getCmdArg(args, 7).getArgType() != CmdArgType_ValueIndex) {
            return true;
        }
        u32 workId = getCmdArg(args, 7).getValueIndexData();
        bool useLr = getCmdArg(args, 8).getBoolData();
        float result = 0.0f;
        if (type == 3) {
            result = getTargetFoundAngleToMotionFrame(moduleAccesser, area, offset, dir, rotSign, frameNum, useLr);
        } else if (type == 4) {
            result = getTargetAngleToMotionFrame(moduleAccesser, area, offset, dir, rotSign, frameNum, useLr);
        }
        moduleAccesser->getWorkManageModule().setFloat(result, workId);
        return true;
    }
    case 5:
    case 6: {
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 area;
        READ_INT_ARG(args, 0, moduleAccesser, area)
        if (getCmdArg(args, 1).getArgType() != CmdArgType_ValueIndex) {
            return true;
        }
        u32 workId = getCmdArg(args, 1).getValueIndexData();
        float result = 3.4028235e38f;
        if (type == 5) {
            result = getTargetRealDistance(moduleAccesser, area);
        } else if (type == 6) {
            result = getTargetFoundRealDistance(moduleAccesser, area);
        }
        moduleAccesser->getWorkManageModule().setFloat(result, workId);
        return true;
    }
    case 7:
    case 8:
    case 9:
    case 10: {
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        s32 area;
        READ_INT_ARG(args, 0, moduleAccesser, area)
        if (getCmdArg(args, 1).getArgType() != CmdArgType_ValueIndex) {
            return true;
        }
        u32 workId = getCmdArg(args, 1).getValueIndexData();
        float result = 3.4028235e38f;
        if (type == 7) {
            result = getTargetPos(moduleAccesser, area, true).m_x;
        } else if (type == 8) {
            result = getTargetFoundPos(moduleAccesser, area, true).m_x;
        } else if (type == 9) {
            result = getTargetPos(moduleAccesser, area, true).m_y;
        } else if (type == 10) {
            result = getTargetFoundPos(moduleAccesser, area, true).m_y;
        }
        moduleAccesser->getWorkManageModule().setFloat(result, workId);
        return true;
    }
    }
    return false;
}

// Whether the water area (a group 13 area of this module) with the id is touching it (the general term decide module
// asks it).
bool emAreaModuleImpl::checkWaterArea(u32 areaId) {
    int logNum = getContactLogCount();
    if (logNum <= 0) {
        return false;
    }
    // HYPOTHESIS: group 13 is the water area of the enemies (called without the virtual dispatch, as in the original;
    // the result is compared as a byte, the soAreaData field).
    if ((u8)soAreaModuleImpl::getGroup(areaId) != 13) {
        return false;
    }
    for (int i = 0; i < logNum; i++) {
        if (areaId == getContactLog(i)->m_area) {
            return true;
        }
    }
    return false;
}
