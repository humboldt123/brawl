// MATCH-ONLY: the null flag of acCmdArg is a bool here (the original compares the register copy of it, in the term with six
// arguments, without a mask; as a byte it needs an extra clrlwi).
#define AC_CMD_ARG_NULL_IS_BOOL
// The unit keeps its vector constructors and assignments out of line (the weak copies live in enemy.cpp).
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <ac/ac_anim_cmd_impl.h>
#include <em/em_ai_module_impl.h>
#include <em/em_area_module_impl.h>
#include <em/em_enemy_modules.h>
#include <em/em_info.h>
#include <em/enemy.h>
#include <em/term/em_general_term_decide_module_impl.h>
#include <mt/mt_matrix.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/transition/so_transition_module_impl.h>
#include <st/stage.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: soGeneralTermDisideModuleImpl::checkEstablishSub and soGeneralTerm::getArg (sora_melee map),
// called by their REL names until they are recovered there.
bool fn_27_77A88(soGeneralTermDisideModuleImpl* module, int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
                 u32* returnWord, soGeneralTermCache* cache);
acCmdArg fn_27_778CC(soGeneralTerm* term, int index);

// Rotates the vector by the rotation part of the matrix (main binary).
void fn_8003E3BC(const Matrix* mtx, const Vec3f* in, Vec3f* out);

}

// The general terms of the enemies. HYPOTHESIS: names, from the functions the terms call; the numbers are the ones the
// anim cmds of the enemy kinds pass.
enum EmGeneralTerm {
    EmTerm_KineticEnergyEnd = 10000,
    EmTerm_KineticEnergyStop = 10001,
    EmTerm_KineticEnergyLimitMax = 10002,
    EmTerm_KineticEnergyLimitMin = 10003,
    EmTerm_KineticEnergyReflect = 10004,
    EmTerm_KineticEnergyProject = 10005,
    EmTerm_KineticOverTurnNode = 10006,
    EmTerm_KineticOverTurnNodeName = 10007,
    EmTerm_TargetDir = 10008,
    EmTerm_TargetFoundDir = 10009,
    EmTerm_CurrentAIInfoInvalid = 10010,
    EmTerm_PrevAIInfoInvalid = 10011,
    EmTerm_TargetDistance = 10012,
    EmTerm_TargetFoundDistance = 10013,
    EmTerm_TargetSide = 10014,
    EmTerm_TargetFoundSide = 10015,
    EmTerm_AttackAreaTarget = 10016,
    EmTerm_TargetFighterDownWait = 10017,
    EmTerm_TargetFighterFoundDownWait = 10018,
    EmTerm_TargetFighterSmashHold = 10019,
    EmTerm_TargetFighterFoundSmashHold = 10020,
    EmTerm_KineticEnergyKind = 10021,
    EmTerm_KineticEnergyKindWork = 10022,
    EmTerm_KineticEnergyZero = 10023,
    // 10024 - 10047: the angle of the target (or of the found target) from an axis of a node, tested against a range
    // (a distance bound and a flag come with it). Four blocks of six terms: [10024, 10029] the target / [10030, 10035]
    // the found target with the positive axes, [10036, 10041] / [10042, 10047] the same with the negative axes. Within
    // a block the first three terms name the node by its id, the last three by its name; x, y, z axis in that order.
    EmTerm_NodeAxisAngle = 10024,
    EmTerm_CurrentAIAreaTargetFighter = 10048,
    EmTerm_PrevAIAreaTargetFighter = 10049,
    EmTerm_ExistTarget = 10050,
    EmTerm_ArriveGoalPos = 10051,
    EmTerm_CanCreateEnemyFromTriggerId = 10052,
    EmTerm_InvalidParentEnemy = 10053,
    EmTerm_CanCreateEnemyFromKindAndNum = 10054,
    EmTerm_WaterArea = 10055,
    EmTerm_LrWithPostureParam = 10056,
    EmTerm_ChildrenDead = 10057,
    EmTerm_CanCreateItem = 10058,
    EmTerm_StageFlag = 10059,
};

// The argument types of acCmdArg::getArgType the terms accept.
static const s32 CmdArgType_Int = 0;
static const s32 CmdArgType_String = 4;
static const s32 CmdArgType_ValueIndex = 5;

// An argument of the term (the acCmdArg getters stay out of line in this unit, as in the original).
// MATCH-ONLY: a macro, because an inline function returning the argument adds one more copy of it on the stack.
#define getTermArg(term, index) fn_27_778CC(term, index)

// Read an argument that is either a plain value or a variable (a value index the enemy's value accesser resolves).
// MATCH-ONLY: statement macros, because the original expands these reads in place (see enemy.cpp).
#define READ_FLOAT_ARG(term, index, acc, out)                                                                          \
    if (getTermArg(term, index).getArgType() == CmdArgType_ValueIndex) {                                               \
        out = soValueAccesser::getValueFloat(acc, getTermArg(term, index).getValueIndexData(), 0);                     \
    } else {                                                                                                           \
        out = getTermArg(term, index).getFloatData();                                                                  \
    }
#define READ_INT_ARG(term, index, acc, out)                                                                            \
    if (getTermArg(term, index).getArgType() == CmdArgType_ValueIndex) {                                               \
        out = soValueAccesser::getValueInt(acc, getTermArg(term, index).getValueIndexData(), 0);                       \
    } else {                                                                                                           \
        out = getTermArg(term, index).getIntData();                                                                    \
    }
// HYPOTHESIS: the variable is read as a float and truncated.
#define READ_INT_ARG_FROM_FLOAT(term, index, acc, out)                                                                 \
    if (getTermArg(term, index).getArgType() == CmdArgType_ValueIndex) {                                               \
        out = soValueAccesser::getValueFloat(acc, getTermArg(term, index).getValueIndexData(), 0);                     \
    } else {                                                                                                           \
        out = getTermArg(term, index).getIntData();                                                                    \
    }
// The same for an argument that is a named copy.
#define READ_FLOAT_VAR(arg, acc, out)                                                                                  \
    if (arg.getArgType() == CmdArgType_ValueIndex) {                                                                   \
        out = soValueAccesser::getValueFloat(acc, arg.getValueIndexData(), 0);                                         \
    } else {                                                                                                           \
        out = arg.getFloatData();                                                                                      \
    }
#define READ_INT_VAR(arg, acc, out)                                                                                    \
    if (arg.getArgType() == CmdArgType_ValueIndex) {                                                                   \
        out = soValueAccesser::getValueInt(acc, arg.getValueIndexData(), 0);                                           \
    } else {                                                                                                           \
        out = arg.getIntData();                                                                                        \
    }
#define READ_INT_VAR_FROM_FLOAT(arg, acc, out)                                                                         \
    if (arg.getArgType() == CmdArgType_ValueIndex) {                                                                   \
        out = soValueAccesser::getValueFloat(acc, arg.getValueIndexData(), 0);                                         \
    } else {                                                                                                           \
        out = arg.getIntData();                                                                                        \
    }

// termId in [lo, hi] as one unsigned comparison.
#define IN_RANGE(id, lo, hi) ((u32)((id) - (lo)) <= (u32)((hi) - (lo)))

#define CHECK_ARG_NUM(min, max)                                                                                        \
    if (argNum < (min) || argNum > (max)) {                                                                            \
        return false;                                                                                                  \
    }

#define ENEMY() dynamic_cast<Enemy&>(moduleAccesser->getStageObject())
#define KINETIC() dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule())

// MATCH-ONLY: the unit's .rodata starts with this word (HYPOTHESIS: the remains of a constant of a weak function the
// linker dropped).
#pragma force_active on
extern const float sEmTermRodataLead = 360.0f;
#pragma force_active reset

static emGeneralTermDisideModuleImpl s_emGeneralTermDecideModule;

emGeneralTermDisideModuleImpl* emGeneralTermDisideModuleImpl::getInstance() {
    return &s_emGeneralTermDecideModule;
}

emGeneralTermDisideModuleImpl::emGeneralTermDisideModuleImpl() { }

emGeneralTermDisideModuleImpl::~emGeneralTermDisideModuleImpl() { }

bool emGeneralTermDisideModuleImpl::checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
                                                      u32* returnWord, soGeneralTermCache* cache) {
    if (termId <= 9999) {
        return fn_27_77A88(this, termId, term, moduleAccesser, returnWord, cache);
    }
    // HYPOTHESIS: the table also counts the term itself, so the number of arguments is one less.
    int argNum = term->m_animCmdTable.size() - 1;
    switch (termId) {
    case EmTerm_KineticEnergyEnd: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(1, 1);
        acCmdArg energy = getTermArg(term, 0);
        return kinetic.isEnergyEnd(energy.getIntData());
    }
    case EmTerm_KineticEnergyStop: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(1, 1);
        acCmdArg energy = getTermArg(term, 0);
        return kinetic.isEnergyStop(energy.getIntData());
    }
    case EmTerm_KineticEnergyLimitMax: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(1, 1);
        acCmdArg energy = getTermArg(term, 0);
        return kinetic.isEnergyLimitMax(energy.getIntData());
    }
    case EmTerm_KineticEnergyLimitMin: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(1, 1);
        acCmdArg energy = getTermArg(term, 0);
        return kinetic.isEnergyLimitMin(energy.getIntData());
    }
    case EmTerm_KineticEnergyZero: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(1, 1);
        acCmdArg energy = getTermArg(term, 0);
        return kinetic.isEnergyZero(energy.getIntData());
    }
    case EmTerm_KineticEnergyReflect: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(0, 0);
        return kinetic.isEnergyReflect();
    }
    case EmTerm_KineticEnergyProject: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(0, 1);
        int energy = -1;
        if (argNum == 1) {
            acCmdArg arg = getTermArg(term, 0);
            energy = arg.getIntData();
        }
        return kinetic.isEnergyProject(energy);
    }
    case EmTerm_KineticOverTurnNode:
    case EmTerm_KineticOverTurnNodeName: {
        CHECK_ARG_NUM(3, 3);
        int node;
        if (termId == EmTerm_KineticOverTurnNode) {
            if (getTermArg(term, 0).getArgType() != CmdArgType_Int) {
                return false;
            }
            node = getTermArg(term, 0).getIntData();
        } else if (termId == EmTerm_KineticOverTurnNodeName) {
            if (getTermArg(term, 0).getArgType() != CmdArgType_String) {
                return false;
            }
            soModelModule& model = moduleAccesser->getModelModule();
            node = model.getNodeId(getTermArg(term, 0).getStringData());
        } else {
            return false;
        }
        // MATCH-ONLY: the argument addresses stay in saved registers.
        acCmdArg arg1 = getTermArg(term, 1);
        acCmdArg* pArg1 = &arg1;
        int p3;
        READ_INT_VAR((*pArg1), moduleAccesser, p3);
        acCmdArg arg2 = getTermArg(term, 2);
        acCmdArg* pArg2 = &arg2;
        float p4;
        READ_FLOAT_VAR((*pArg2), moduleAccesser, p4);
        return emKineticModuleImpl::isOverTurn(moduleAccesser, node, p4, p3);
    }
    case EmTerm_TargetDir:
    case EmTerm_TargetFoundDir: {
        CHECK_ARG_NUM(2, 2);
        int p2;
        READ_INT_ARG_FROM_FLOAT(term, 0, moduleAccesser, p2);
        int p3;
        READ_INT_ARG_FROM_FLOAT(term, 1, moduleAccesser, p3);
        if (termId == EmTerm_TargetDir) {
            return emAreaModuleImpl::isTargetDir(moduleAccesser, p2, p3);
        } else if (termId == EmTerm_TargetFoundDir) {
            return emAreaModuleImpl::isTargetFoundDir(moduleAccesser, p2, p3);
        }
        return false;
    }
    case EmTerm_TargetDistance:
    case EmTerm_TargetFoundDistance: {
        CHECK_ARG_NUM(2, 2);
        int p2;
        READ_INT_ARG_FROM_FLOAT(term, 0, moduleAccesser, p2);
        int distance;
        READ_INT_ARG_FROM_FLOAT(term, 1, moduleAccesser, distance);
        if (termId == EmTerm_TargetDistance) {
            if (distance == emAreaModuleImpl::getTargetDistance(moduleAccesser, p2)) {
                return true;
            }
        } else if (termId == EmTerm_TargetFoundDistance) {
            if (distance == emAreaModuleImpl::getTargetFoundDistance(moduleAccesser, p2)) {
                return true;
            }
        }
        return false;
    }
    case EmTerm_TargetSide:
    case EmTerm_TargetFoundSide: {
        CHECK_ARG_NUM(2, 2);
        int p2;
        READ_INT_ARG_FROM_FLOAT(term, 0, moduleAccesser, p2);
        int p3;
        READ_INT_ARG_FROM_FLOAT(term, 1, moduleAccesser, p3);
        if (termId == EmTerm_TargetSide) {
            return emAreaModuleImpl::isTargetSide(moduleAccesser, p2, p3);
        } else if (termId == EmTerm_TargetFoundSide) {
            return emAreaModuleImpl::isTargetFoundSide(moduleAccesser, p2, p3);
        }
        return false;
    }
    case EmTerm_TargetFighterDownWait:
    case EmTerm_TargetFighterFoundDownWait: {
        CHECK_ARG_NUM(1, 1);
        int p2;
        READ_INT_ARG_FROM_FLOAT(term, 0, moduleAccesser, p2);
        if (termId == EmTerm_TargetFighterDownWait) {
            return emAreaModuleImpl::isTargetFighterDownWait(moduleAccesser, p2);
        } else if (termId == EmTerm_TargetFighterFoundDownWait) {
            return emAreaModuleImpl::isTargetFighterFoundDownWait(moduleAccesser, p2);
        }
        return false;
    }
    case EmTerm_TargetFighterSmashHold:
    case EmTerm_TargetFighterFoundSmashHold: {
        CHECK_ARG_NUM(1, 1);
        int p2;
        READ_INT_ARG_FROM_FLOAT(term, 0, moduleAccesser, p2);
        if (termId == EmTerm_TargetFighterSmashHold) {
            return emAreaModuleImpl::isTargetFighterSmashHold(moduleAccesser, p2);
        } else if (termId == EmTerm_TargetFighterFoundSmashHold) {
            return emAreaModuleImpl::isTargetFighterFoundSmashHold(moduleAccesser, p2);
        }
        return false;
    }
    case EmTerm_CurrentAIInfoInvalid: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().getAIModule().getCurrentAIInfo()->isInvalid(moduleAccesser) != true;
    }
    case EmTerm_PrevAIInfoInvalid: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().getAIModule().getPrevAIInfo(0)->isInvalid(moduleAccesser) != true;
    }
    case EmTerm_AttackAreaTarget: {
        CHECK_ARG_NUM(1, 1);
        emAIModuleImpl& aiModule = ENEMY().getAIModule();
        int area;
        READ_INT_ARG(term, 0, moduleAccesser, area);
        return aiModule.isAttackAreaTarget(area);
    }
    case EmTerm_KineticEnergyKind:
    case EmTerm_KineticEnergyKindWork: {
        emKineticModuleImpl& kinetic = KINETIC();
        CHECK_ARG_NUM(2, 2);
        acCmdArg arg0 = getTermArg(term, 0);
        acCmdArg arg1 = getTermArg(term, 1);
        int energy;
        READ_INT_VAR(arg0, moduleAccesser, energy);
        int kind;
        READ_INT_VAR(arg1, moduleAccesser, kind);
        int flag = 0;
        if (termId == EmTerm_KineticEnergyKind) {
            flag = 0;
        } else if (termId == EmTerm_KineticEnergyKindWork) {
            flag = 1;
        }
        return kinetic.getEnergyKind(energy, flag) == kind;
    }
    case 10024: case 10025: case 10026: case 10027: case 10028: case 10029: case 10030: case 10031:
    case 10032: case 10033: case 10034: case 10035: case 10036: case 10037: case 10038: case 10039:
    case 10040: case 10041: case 10042: case 10043: case 10044: case 10045: case 10046: case 10047: {
        CHECK_ARG_NUM(6, 6);
        acCmdArg arg0 = getTermArg(term, 0);
        acCmdArg arg1 = getTermArg(term, 1);
        acCmdArg arg2 = getTermArg(term, 2);
        acCmdArg arg3 = getTermArg(term, 3);
        acCmdArg arg4 = getTermArg(term, 4);
        acCmdArg arg5 = getTermArg(term, 5);
        soModelModule& model = moduleAccesser->getModelModule();
        int node;
        if (termId == 10024 || termId == 10025 || termId == 10026 || termId == 10030 || termId == 10031 ||
            termId == 10032 || termId == 10036 || termId == 10037 || termId == 10038 ||
            IN_RANGE(termId, 10042, 10044)) {
            node = arg0.getIntData();
        } else if (termId == 10027 || termId == 10028 || termId == 10029 || termId == 10033 || termId == 10034 ||
                   termId == 10035 || termId == 10039 || termId == 10040 || termId == 10041 ||
                   IN_RANGE(termId, 10045, 10047)) {
            node = model.getNodeId(arg0.getStringData());
        } else {
            return false;
        }
        Matrix nodeMtx(true);
        model.getNodeGlobalMtx(node, &nodeMtx, false);
        Vec3f axis(0.0f, 0.0f, 0.0f);
        if (termId == 10024 || termId == 10027 || termId == 10030 || termId == 10033) {
            axis = Vec3f(1.0f, 0.0f, 0.0f);
        } else if (termId == 10025 || termId == 10028 || termId == 10031 || termId == 10034) {
            axis = Vec3f(0.0f, 1.0f, 0.0f);
        } else if (termId == 10026 || termId == 10029 || termId == 10032 || termId == 10035) {
            axis = Vec3f(0.0f, 0.0f, 1.0f);
        } else if (termId == 10036 || termId == 10039 || termId == 10042 || termId == 10045) {
            axis = Vec3f(-1.0f, 0.0f, 0.0f);
        } else if (termId == 10037 || termId == 10040 || termId == 10043 || termId == 10046) {
            axis = Vec3f(0.0f, -1.0f, 0.0f);
        } else if (termId == 10038 || termId == 10041 || termId == 10044 || termId == 10047) {
            axis = Vec3f(0.0f, 0.0f, -1.0f);
        }
        fn_8003E3BC(&nodeMtx, &axis, &axis);
        int p2;
        READ_INT_VAR_FROM_FLOAT(arg1, moduleAccesser, p2);
        Vec2f range;
        range.m_x = 0.0f;
        range.m_y = 0.0f;
        READ_FLOAT_VAR(arg2, moduleAccesser, range.m_x);
        READ_FLOAT_VAR(arg3, moduleAccesser, range.m_y);
        float p5;
        READ_FLOAT_VAR(arg4, moduleAccesser, p5);
        bool p6 = arg5.getBoolData();
        if (termId == 10024 || termId == 10025 || termId == 10026 || termId == 10027 || termId == 10028 ||
            termId == 10029 || IN_RANGE(termId, 10036, 10041)) {
            return emAreaModuleImpl::isTargetAngleFromAxis(moduleAccesser, p2, range, Vec2f(axis.m_x, axis.m_y), p5,
                                                           p6);
        } else if (termId == 10030 || termId == 10031 || termId == 10032 || termId == 10033 || termId == 10034 ||
                   termId == 10035 || IN_RANGE(termId, 10042, 10047)) {
            return emAreaModuleImpl::isTargetFoundAngleFromAxis(moduleAccesser, p2, range, Vec2f(axis.m_x, axis.m_y),
                                                                p5, p6);
        }
        return false;
    }
    case EmTerm_CurrentAIAreaTargetFighter: {
        CHECK_ARG_NUM(0, 0);
        int area = ENEMY().getAIModule().getCurrentAIInfo()->unk4;
        return emAreaModuleImpl::isTargetFighter(moduleAccesser, area);
    }
    case EmTerm_PrevAIAreaTargetFighter: {
        CHECK_ARG_NUM(0, 0);
        int area = ENEMY().getAIModule().getPrevAIInfo(0)->unk4;
        return emAreaModuleImpl::isTargetFighter(moduleAccesser, area);
    }
    case EmTerm_ExistTarget: {
        CHECK_ARG_NUM(1, 1);
        int area;
        READ_INT_ARG(term, 0, moduleAccesser, area);
        return emAreaModuleImpl::isExistTarget(moduleAccesser, area);
    }
    case EmTerm_ArriveGoalPos: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().isArriveGoalPos();
    }
    case EmTerm_CanCreateEnemyFromTriggerId: {
        CHECK_ARG_NUM(2, 2);
        int triggerId;
        READ_INT_ARG(term, 0, moduleAccesser, triggerId);
        int index;
        READ_INT_ARG(term, 1, moduleAccesser, index);
        return ENEMY().canCreateEnemyFromTriggerId(triggerId, index);
    }
    case EmTerm_InvalidParentEnemy: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().isInvalidParentEnemy();
    }
    case EmTerm_CanCreateEnemyFromKindAndNum: {
        CHECK_ARG_NUM(3, 3);
        // HYPOTHESIS: the original reads the invalid kind here (the value is not used).
        emInfo::getInstance()->getInvalidKind();
        int kind;
        READ_INT_ARG(term, 0, moduleAccesser, kind);
        int maxNum;
        READ_INT_ARG(term, 1, moduleAccesser, maxNum);
        float rate;
        READ_FLOAT_ARG(term, 2, moduleAccesser, rate);
        return ENEMY().canCreateEnemyFromEnemyKindAndNum((EnemyKind)kind, maxNum, rate);
    }
    case EmTerm_WaterArea: {
        emAreaModuleImpl& areaModule = dynamic_cast<emAreaModuleImpl&>(moduleAccesser->getAreaModule());
        CHECK_ARG_NUM(1, 1);
        int area;
        READ_INT_ARG(term, 0, moduleAccesser, area);
        return areaModule.checkWaterArea(area);
    }
    case EmTerm_LrWithPostureParam: {
        CHECK_ARG_NUM(1, 1);
        void* postureParam;
        if (getTermArg(term, 0).getArgType() == CmdArgType_ValueIndex) {
            postureParam = soValueAccesser::getConstantIndefinite(moduleAccesser, getTermArg(term, 0).getValueIndexData(), 0);
        } else {
            postureParam = getTermArg(term, 0).getAddressData();
        }
        return ENEMY().isLrWithPostureParam(postureParam);
    }
    case EmTerm_ChildrenDead: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().isChildrenDead();
    }
    case EmTerm_CanCreateItem: {
        CHECK_ARG_NUM(0, 0);
        return ENEMY().isCanCreateItem();
    }
    case EmTerm_StageFlag: {
        CHECK_ARG_NUM(0, 0);
        return g_Stage->unk19F;
    }
    }
    return false;
}

bool emAIInfo::isInvalid(soModuleAccesser* acc) {
    if (m_unitIndex == -2) {
        return true;
    }
    if (unk4 == -1) {
        return true;
    }
    return emAreaModuleImpl::isExistTarget(acc, unk4) == false;
}
