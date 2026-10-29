#include <em/em_switch_decide_module_impl.h>
#include <em/em_ai_module_impl.h>
#include <em/em_area_module_impl.h>
#include <em/em_enemy_modules.h>
#include <em/enemy.h>
#include <ac/ac_anim_cmd_impl.h>
#include <mt/mt_matrix.h>
#include <so/model/so_model_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// The questions of the enemies, 1000 - 1055 (the others go to soSwitchDecideModuleImpl). The names come from the
// functions each question calls (module map names). HYPOTHESIS: the meaning of the X / Y / Z and Neg suffixes: the
// question rotates the unit axis of that name by the matrix of a model node and hands the result on as a direction.
// "Node" questions give the node by index, "NodeName" ones by name.
enum SwitchKind {
    SwitchKind_AIRandom = 0x3E8, // emAIModuleImpl::switchRnd

    // emKineticModuleImpl::getLrFromTransSpeed / getUdFromTransSpeed / getLrFromPathMotRot
    SwitchKind_LrFromTransSpeed = 0x3E9,
    SwitchKind_UdFromTransSpeed = 0x3EA,
    SwitchKind_LrFromPathMotRot = 0x3EB,

    // emAreaModuleImpl::getTargetAxisSide: which side of an axis of the enemy the target is on
    SwitchKind_TargetAxisSideNodeX = 0x3EC,
    SwitchKind_TargetAxisSideNodeY = 0x3ED,
    SwitchKind_TargetAxisSideNodeZ = 0x3EE,
    SwitchKind_TargetAxisSideNodeNameX = 0x3EF,
    SwitchKind_TargetAxisSideNodeNameY = 0x3F0,
    SwitchKind_TargetAxisSideNodeNameZ = 0x3F1,
    SwitchKind_TargetAxisSideSpeed = 0x3F2, // the axis is the kinetic speed of the enemy
    // emAreaModuleImpl::getTargetFoundAxisSide: the same for the target the enemy has found
    SwitchKind_TargetFoundAxisSideNodeX = 0x3F3,
    SwitchKind_TargetFoundAxisSideNodeY = 0x3F4,
    SwitchKind_TargetFoundAxisSideNodeZ = 0x3F5,
    SwitchKind_TargetFoundAxisSideNodeNameX = 0x3F6,
    SwitchKind_TargetFoundAxisSideNodeNameY = 0x3F7,
    SwitchKind_TargetFoundAxisSideNodeNameZ = 0x3F8,
    SwitchKind_TargetFoundAxisSideSpeed = 0x3F9,

    // emAreaModuleImpl::getTargetRotSpeedDirForAxisAdjust: which way to turn about an axis to face the target
    SwitchKind_TargetRotSpeedDirNodeX = 0x3FA,
    SwitchKind_TargetRotSpeedDirNodeY = 0x3FB,
    SwitchKind_TargetRotSpeedDirNodeZ = 0x3FC,
    SwitchKind_TargetRotSpeedDirNodeNameX = 0x3FD,
    SwitchKind_TargetRotSpeedDirNodeNameY = 0x3FE,
    SwitchKind_TargetRotSpeedDirNodeNameZ = 0x3FF,
    // emAreaModuleImpl::getTargetFoundRotSpeedDirForAxisAdjust: the same for the target the enemy has found
    SwitchKind_TargetFoundRotSpeedDirNodeX = 0x400,
    SwitchKind_TargetFoundRotSpeedDirNodeY = 0x401,
    SwitchKind_TargetFoundRotSpeedDirNodeZ = 0x402,
    SwitchKind_TargetFoundRotSpeedDirNodeNameX = 0x403,
    SwitchKind_TargetFoundRotSpeedDirNodeNameY = 0x404,
    SwitchKind_TargetFoundRotSpeedDirNodeNameZ = 0x405,
    // The same two families about the negative axes.
    SwitchKind_TargetRotSpeedDirNodeNegX = 0x406,
    SwitchKind_TargetRotSpeedDirNodeNegY = 0x407,
    SwitchKind_TargetRotSpeedDirNodeNegZ = 0x408,
    SwitchKind_TargetRotSpeedDirNodeNameNegX = 0x409,
    SwitchKind_TargetRotSpeedDirNodeNameNegY = 0x40A,
    SwitchKind_TargetRotSpeedDirNodeNameNegZ = 0x40B,
    SwitchKind_TargetFoundRotSpeedDirNodeNegX = 0x40C,
    SwitchKind_TargetFoundRotSpeedDirNodeNegY = 0x40D,
    SwitchKind_TargetFoundRotSpeedDirNodeNegZ = 0x40E,
    SwitchKind_TargetFoundRotSpeedDirNodeNameNegX = 0x40F,
    SwitchKind_TargetFoundRotSpeedDirNodeNameNegY = 0x410,
    SwitchKind_TargetFoundRotSpeedDirNodeNameNegZ = 0x411,

    // emKineticModuleImpl::getRotSpeedDirForAxisAdjustWithTouchGround: which way to turn about an axis while the enemy
    // touches the ground
    SwitchKind_RotSpeedDirTouchGroundNodeX = 0x412,
    SwitchKind_RotSpeedDirTouchGroundNodeY = 0x413,
    SwitchKind_RotSpeedDirTouchGroundNodeZ = 0x414,
    SwitchKind_RotSpeedDirTouchGroundNodeNameX = 0x415,
    SwitchKind_RotSpeedDirTouchGroundNodeNameY = 0x416,
    SwitchKind_RotSpeedDirTouchGroundNodeNameZ = 0x417,
    SwitchKind_RotSpeedDirTouchGroundNodeNegX = 0x418,
    SwitchKind_RotSpeedDirTouchGroundNodeNegY = 0x419,
    SwitchKind_RotSpeedDirTouchGroundNodeNegZ = 0x41A,
    SwitchKind_RotSpeedDirTouchGroundNodeNameNegX = 0x41B,
    SwitchKind_RotSpeedDirTouchGroundNodeNameNegY = 0x41C,
    SwitchKind_RotSpeedDirTouchGroundNodeNameNegZ = 0x41D,

    // Enemy::getNearPostureParamFromPosition / getFarPostureParamFromPosition
    SwitchKind_NearPostureParam = 0x41E,
    SwitchKind_FarPostureParam = 0x41F,
};

// Whether kind lies in the (inclusive) range of kinds.
#define KIND_IN_RANGE(kind, first, last) (static_cast<u32>((kind) - (first)) <= static_cast<u32>((last) - (first)))

// The answers of the questions (the branch of the script's switch). HYPOTHESIS: names. 2 and 3 are the two answers of
// the side and direction questions.
static const int SwitchBranch_None = -1;
static const int SwitchBranch_Else = 3; // a question with the wrong number of arguments

// The argument types of acCmdArg::getArgType: a value index is read from the enemy's value accesser.
static const s32 CmdArgType_ValueIndex = 5;

// The most posture parameters the position questions take.
static const u32 PostureParamMax = 5;

// soSwitchDecideModuleImpl::decideSwitch (sora_melee, unnamed): the common questions.
extern "C" int fn_27_7B610(soSwitchDecideModule* module, int kind, soArrayContractibleTable<const acCmdArgConv>* args,
                           soModuleAccesser* acc);
// Rotates a vector by the rotation of a matrix (the translation is ignored).
extern "C" void fn_8003E3BC(const Matrix* mtx, const Vec3f* src, Vec3f* dst);

// The argument at index (a virtual call through the array interface, as in the Enemy unit).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

// An argument that is either a plain value or a variable of the enemy's value accesser; a parameter block is either an
// address or a variable naming an indefinite constant.
// MATCH-ONLY: statement macros, as in the Enemy unit (inline functions stay out of line there).
#define READ_FLOAT_ARG(args, index, acc, out)                                                                          \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueFloat(acc, getCmdArg(args, index).getValueIndexData(), 0);                      \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getFloatData();                                                                   \
    }
#define READ_INT_ARG(args, index, acc, out)                                                                            \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getValueInt(acc, getCmdArg(args, index).getValueIndexData(), 0);                        \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getIntData();                                                                     \
    }
// An integer argument whose variable the scripts keep as a float (the variable is truncated).
#define READ_INT_ARG_FROM_FLOAT(args, index, acc, out)                                                                 \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = static_cast<int>(soValueAccesser::getValueFloat(acc, getCmdArg(args, index).getValueIndexData(), 0));    \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getIntData();                                                                     \
    }
#define READ_ADDRESS_ARG(args, index, acc, out)                                                                        \
    if (getCmdArg(args, index).getArgType() == CmdArgType_ValueIndex) {                                                \
        out = soValueAccesser::getConstantIndefinite(acc, getCmdArg(args, index).getValueIndexData(), 0);              \
    } else {                                                                                                           \
        out = getCmdArg(args, index).getAddressData();                                                                 \
    }

// The one module of all enemies (it has no state); Enemy hands it to its module accesser.
emSwitchDecideModuleImpl g_emSwitchDecideModule;

emSwitchDecideModuleImpl* emSwitchDecideModuleImpl::getInstance() {
    return &g_emSwitchDecideModule;
}

emSwitchDecideModuleImpl::emSwitchDecideModuleImpl() { }

soSwitchDecideModule::~soSwitchDecideModule() { }

emSwitchDecideModuleImpl::~emSwitchDecideModuleImpl() { }

// Answers a question of a script. The questions about an axis of a model node take 5 arguments: the node (index or
// name), the target, the offset from the target's position (two floats) and whether the offset is adjusted by the
// target's facing; the touch ground ones take only the node. A question with the wrong number of arguments gets
// SwitchBranch_Else (SwitchBranch_None for the questions that are not about a side).
int emSwitchDecideModuleImpl::decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* argList,
                                           soModuleAccesser* moduleAccesser) {
    const soArrayFixed<const acCmdArgConv>& args = *argList;
    int argNum = args.size();
    switch (kind) {
    case SwitchKind_AIRandom: {
        // The choice among the entries of a table of the AI (address, entry count).
        if (argNum < 2 || argNum > 2) {
            return SwitchBranch_None;
        }
        void* entries;
        READ_ADDRESS_ARG(args, 0, moduleAccesser, entries)
        u32 entryNum;
        if (getCmdArg(args, 1).getArgType() == CmdArgType_ValueIndex) {
            entryNum = static_cast<u32>(soValueAccesser::getValueFloat(moduleAccesser, getCmdArg(args, 1).getValueIndexData(), 0));
        } else {
            entryNum = getCmdArg(args, 1).getIntData();
        }
        Enemy& enemy = dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
        return enemy.getAIModule().switchRnd(static_cast<emAIRndEntry*>(entries), entryNum);
    }
    case SwitchKind_LrFromTransSpeed:
    case SwitchKind_UdFromTransSpeed:
    case SwitchKind_LrFromPathMotRot: {
        if (argNum < 1 || argNum > 1) {
            return SwitchBranch_None;
        }
        int energyIndex;
        READ_INT_ARG(args, 0, moduleAccesser, energyIndex)
        emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        if (kind == SwitchKind_LrFromTransSpeed) {
            return kinetic.getLrFromTransSpeed(energyIndex);
        }
        if (kind == SwitchKind_UdFromTransSpeed) {
            return kinetic.getUdFromTransSpeed(energyIndex);
        }
        if (kind == SwitchKind_LrFromPathMotRot) {
            return kinetic.getLrFromPathMotRot(energyIndex);
        }
        return SwitchBranch_None;
    }
    case SwitchKind_TargetAxisSideNodeX:
    case SwitchKind_TargetAxisSideNodeY:
    case SwitchKind_TargetAxisSideNodeZ:
    case SwitchKind_TargetAxisSideNodeNameX:
    case SwitchKind_TargetAxisSideNodeNameY:
    case SwitchKind_TargetAxisSideNodeNameZ:
    case SwitchKind_TargetFoundAxisSideNodeX:
    case SwitchKind_TargetFoundAxisSideNodeY:
    case SwitchKind_TargetFoundAxisSideNodeZ:
    case SwitchKind_TargetFoundAxisSideNodeNameX:
    case SwitchKind_TargetFoundAxisSideNodeNameY:
    case SwitchKind_TargetFoundAxisSideNodeNameZ: {
        if (argNum < 5 || argNum > 5) {
            return SwitchBranch_Else;
        }
        soModelModule& model = moduleAccesser->getModelModule();
        int node;
        if (kind == SwitchKind_TargetAxisSideNodeX || kind == SwitchKind_TargetAxisSideNodeY ||
            kind == SwitchKind_TargetAxisSideNodeZ ||
            KIND_IN_RANGE(kind, SwitchKind_TargetFoundAxisSideNodeX, SwitchKind_TargetFoundAxisSideNodeZ)) {
            READ_INT_ARG_FROM_FLOAT(args, 0, moduleAccesser, node)
        } else if (kind == SwitchKind_TargetAxisSideNodeNameX || kind == SwitchKind_TargetAxisSideNodeNameY ||
                   kind == SwitchKind_TargetAxisSideNodeNameZ ||
                   KIND_IN_RANGE(kind, SwitchKind_TargetFoundAxisSideNodeNameX, SwitchKind_TargetFoundAxisSideNodeNameZ)) {
            node = model.getNodeId(getCmdArg(args, 0).getStringData());
        } else {
            return SwitchBranch_Else;
        }
        Matrix nodeMtx(true);
        model.getNodeGlobalMtx(node, &nodeMtx, false);
        Vec3f axis(0.0f, 0.0f, 0.0f);
        if (kind == SwitchKind_TargetAxisSideNodeX || kind == SwitchKind_TargetFoundAxisSideNodeX ||
            kind == SwitchKind_TargetAxisSideNodeNameX || kind == SwitchKind_TargetFoundAxisSideNodeNameX) {
            axis = Vec3f(1.0f, 0.0f, 0.0f);
        } else if (kind == SwitchKind_TargetAxisSideNodeY || kind == SwitchKind_TargetFoundAxisSideNodeY ||
                   kind == SwitchKind_TargetAxisSideNodeNameY || kind == SwitchKind_TargetFoundAxisSideNodeNameY) {
            axis = Vec3f(0.0f, 1.0f, 0.0f);
        } else if (kind == SwitchKind_TargetAxisSideNodeZ || kind == SwitchKind_TargetFoundAxisSideNodeZ ||
                   kind == SwitchKind_TargetAxisSideNodeNameZ || kind == SwitchKind_TargetFoundAxisSideNodeNameZ) {
            axis = Vec3f(0.0f, 0.0f, 1.0f);
        }
        fn_8003E3BC(&nodeMtx, &axis, &axis);
        int target;
        READ_INT_ARG_FROM_FLOAT(args, 1, moduleAccesser, target)
        Vec2f targetOffset(0.0f, 0.0f);
        READ_FLOAT_ARG(args, 2, moduleAccesser, targetOffset.m_x)
        READ_FLOAT_ARG(args, 3, moduleAccesser, targetOffset.m_y)
        bool adjustByLr = getCmdArg(args, 4).getBoolData();
        if (KIND_IN_RANGE(kind, SwitchKind_TargetAxisSideNodeX, SwitchKind_TargetAxisSideNodeNameZ)) {
            return emAreaModuleImpl::getTargetAxisSide(moduleAccesser, target, targetOffset, Vec2f(axis.m_x, axis.m_y), adjustByLr);
        }
        if (KIND_IN_RANGE(kind, SwitchKind_TargetFoundAxisSideNodeX, SwitchKind_TargetFoundAxisSideNodeNameZ)) {
            return emAreaModuleImpl::getTargetFoundAxisSide(moduleAccesser, target, targetOffset, Vec2f(axis.m_x, axis.m_y), adjustByLr);
        }
        break;
    }
    case SwitchKind_TargetAxisSideSpeed:
    case SwitchKind_TargetFoundAxisSideSpeed: {
        // The direction is the speed of the enemy; 4 arguments: the target, the offset and the facing flag.
        if (argNum < 4 || argNum > 4) {
            return SwitchBranch_Else;
        }
        Vec2f speed(0.0f, 0.0f);
        speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        int target;
        READ_INT_ARG_FROM_FLOAT(args, 0, moduleAccesser, target)
        Vec2f targetOffset(0.0f, 0.0f);
        READ_FLOAT_ARG(args, 1, moduleAccesser, targetOffset.m_x)
        READ_FLOAT_ARG(args, 2, moduleAccesser, targetOffset.m_y)
        bool adjustByLr = getCmdArg(args, 3).getBoolData();
        if (kind == SwitchKind_TargetAxisSideSpeed) {
            return emAreaModuleImpl::getTargetAxisSide(moduleAccesser, target, targetOffset, speed, adjustByLr);
        }
        if (kind == SwitchKind_TargetFoundAxisSideSpeed) {
            return emAreaModuleImpl::getTargetFoundAxisSide(moduleAccesser, target, targetOffset, speed, adjustByLr);
        }
        break;
    }
    case SwitchKind_TargetRotSpeedDirNodeX:
    case SwitchKind_TargetRotSpeedDirNodeY:
    case SwitchKind_TargetRotSpeedDirNodeZ:
    case SwitchKind_TargetRotSpeedDirNodeNameX:
    case SwitchKind_TargetRotSpeedDirNodeNameY:
    case SwitchKind_TargetRotSpeedDirNodeNameZ:
    case SwitchKind_TargetFoundRotSpeedDirNodeX:
    case SwitchKind_TargetFoundRotSpeedDirNodeY:
    case SwitchKind_TargetFoundRotSpeedDirNodeZ:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameX:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameY:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameZ:
    case SwitchKind_TargetRotSpeedDirNodeNegX:
    case SwitchKind_TargetRotSpeedDirNodeNegY:
    case SwitchKind_TargetRotSpeedDirNodeNegZ:
    case SwitchKind_TargetRotSpeedDirNodeNameNegX:
    case SwitchKind_TargetRotSpeedDirNodeNameNegY:
    case SwitchKind_TargetRotSpeedDirNodeNameNegZ:
    case SwitchKind_TargetFoundRotSpeedDirNodeNegX:
    case SwitchKind_TargetFoundRotSpeedDirNodeNegY:
    case SwitchKind_TargetFoundRotSpeedDirNodeNegZ:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameNegX:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameNegY:
    case SwitchKind_TargetFoundRotSpeedDirNodeNameNegZ: {
        if (argNum < 5 || argNum > 5) {
            return SwitchBranch_Else;
        }
        soModelModule& model = moduleAccesser->getModelModule();
        int node;
        if (kind == SwitchKind_TargetRotSpeedDirNodeX || kind == SwitchKind_TargetRotSpeedDirNodeY ||
            kind == SwitchKind_TargetRotSpeedDirNodeZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeX ||
            kind == SwitchKind_TargetFoundRotSpeedDirNodeY || kind == SwitchKind_TargetFoundRotSpeedDirNodeZ ||
            kind == SwitchKind_TargetRotSpeedDirNodeNegX || kind == SwitchKind_TargetRotSpeedDirNodeNegY ||
            kind == SwitchKind_TargetRotSpeedDirNodeNegZ ||
            KIND_IN_RANGE(kind, SwitchKind_TargetFoundRotSpeedDirNodeNegX, SwitchKind_TargetFoundRotSpeedDirNodeNegZ)) {
            READ_INT_ARG_FROM_FLOAT(args, 0, moduleAccesser, node)
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeNameX || kind == SwitchKind_TargetRotSpeedDirNodeNameY ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeNameX ||
                   kind == SwitchKind_TargetFoundRotSpeedDirNodeNameY ||
                   kind == SwitchKind_TargetFoundRotSpeedDirNodeNameZ || kind == SwitchKind_TargetRotSpeedDirNodeNameNegX ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameNegY || kind == SwitchKind_TargetRotSpeedDirNodeNameNegZ ||
                   KIND_IN_RANGE(kind, SwitchKind_TargetFoundRotSpeedDirNodeNameNegX,
                                 SwitchKind_TargetFoundRotSpeedDirNodeNameNegZ)) {
            node = model.getNodeId(getCmdArg(args, 0).getStringData());
        } else {
            return SwitchBranch_Else;
        }
        Matrix nodeMtx(true);
        model.getNodeGlobalMtx(node, &nodeMtx, false);
        Vec3f axis(0.0f, 0.0f, 0.0f);
        if (kind == SwitchKind_TargetRotSpeedDirNodeX || kind == SwitchKind_TargetFoundRotSpeedDirNodeX ||
            kind == SwitchKind_TargetRotSpeedDirNodeNameX || kind == SwitchKind_TargetFoundRotSpeedDirNodeNameX) {
            axis = Vec3f(1.0f, 0.0f, 0.0f);
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeY || kind == SwitchKind_TargetFoundRotSpeedDirNodeY ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameY || kind == SwitchKind_TargetFoundRotSpeedDirNodeNameY) {
            axis = Vec3f(0.0f, 1.0f, 0.0f);
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeZ ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeNameZ) {
            axis = Vec3f(0.0f, 0.0f, 1.0f);
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeNegX || kind == SwitchKind_TargetFoundRotSpeedDirNodeNegX ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameNegX ||
                   kind == SwitchKind_TargetFoundRotSpeedDirNodeNameNegX) {
            axis = Vec3f(-1.0f, 0.0f, 0.0f);
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeNegY || kind == SwitchKind_TargetFoundRotSpeedDirNodeNegY ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameNegY ||
                   kind == SwitchKind_TargetFoundRotSpeedDirNodeNameNegY) {
            axis = Vec3f(0.0f, -1.0f, 0.0f);
        } else if (kind == SwitchKind_TargetRotSpeedDirNodeNegZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeNegZ ||
                   kind == SwitchKind_TargetRotSpeedDirNodeNameNegZ ||
                   kind == SwitchKind_TargetFoundRotSpeedDirNodeNameNegZ) {
            axis = Vec3f(0.0f, 0.0f, -1.0f);
        }
        fn_8003E3BC(&nodeMtx, &axis, &axis);
        int target;
        READ_INT_ARG_FROM_FLOAT(args, 1, moduleAccesser, target)
        Vec2f targetOffset(0.0f, 0.0f);
        READ_FLOAT_ARG(args, 2, moduleAccesser, targetOffset.m_x)
        READ_FLOAT_ARG(args, 3, moduleAccesser, targetOffset.m_y)
        bool adjustByLr = getCmdArg(args, 4).getBoolData();
        // MATCH-ONLY: the original tests the kinds of the first run one by one and the kinds of the last as a range.
        if (kind == SwitchKind_TargetRotSpeedDirNodeX || kind == SwitchKind_TargetRotSpeedDirNodeY ||
            kind == SwitchKind_TargetRotSpeedDirNodeZ || kind == SwitchKind_TargetRotSpeedDirNodeNameX ||
            kind == SwitchKind_TargetRotSpeedDirNodeNameY || kind == SwitchKind_TargetRotSpeedDirNodeNameZ ||
            KIND_IN_RANGE(kind, SwitchKind_TargetRotSpeedDirNodeNegX, SwitchKind_TargetRotSpeedDirNodeNameNegZ)) {
            return emAreaModuleImpl::getTargetRotSpeedDirForAxisAdjust(moduleAccesser, target, targetOffset, Vec2f(axis.m_x, axis.m_y), adjustByLr);
        }
        if (kind == SwitchKind_TargetFoundRotSpeedDirNodeNameX || kind == SwitchKind_TargetFoundRotSpeedDirNodeNameY ||
            kind == SwitchKind_TargetFoundRotSpeedDirNodeNameZ || kind == SwitchKind_TargetFoundRotSpeedDirNodeX ||
            kind == SwitchKind_TargetFoundRotSpeedDirNodeY || kind == SwitchKind_TargetFoundRotSpeedDirNodeZ ||
            KIND_IN_RANGE(kind, SwitchKind_TargetFoundRotSpeedDirNodeNegX,
                          SwitchKind_TargetFoundRotSpeedDirNodeNameNegZ)) {
            return emAreaModuleImpl::getTargetFoundRotSpeedDirForAxisAdjust(moduleAccesser, target, targetOffset, Vec2f(axis.m_x, axis.m_y), adjustByLr);
        }
        break;
    }
    case SwitchKind_RotSpeedDirTouchGroundNodeX:
    case SwitchKind_RotSpeedDirTouchGroundNodeY:
    case SwitchKind_RotSpeedDirTouchGroundNodeZ:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameX:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameY:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameZ:
    case SwitchKind_RotSpeedDirTouchGroundNodeNegX:
    case SwitchKind_RotSpeedDirTouchGroundNodeNegY:
    case SwitchKind_RotSpeedDirTouchGroundNodeNegZ:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameNegX:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameNegY:
    case SwitchKind_RotSpeedDirTouchGroundNodeNameNegZ: {
        if (argNum < 1 || argNum > 1) {
            return SwitchBranch_Else;
        }
        soModelModule& model = moduleAccesser->getModelModule();
        int node;
        if (kind == SwitchKind_RotSpeedDirTouchGroundNodeX || kind == SwitchKind_RotSpeedDirTouchGroundNodeY ||
            kind == SwitchKind_RotSpeedDirTouchGroundNodeZ ||
            KIND_IN_RANGE(kind, SwitchKind_RotSpeedDirTouchGroundNodeNegX, SwitchKind_RotSpeedDirTouchGroundNodeNegZ)) {
            READ_INT_ARG_FROM_FLOAT(args, 0, moduleAccesser, node)
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeNameX ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameY ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameZ ||
                   KIND_IN_RANGE(kind, SwitchKind_RotSpeedDirTouchGroundNodeNameNegX,
                                 SwitchKind_RotSpeedDirTouchGroundNodeNameNegZ)) {
            node = model.getNodeId(getCmdArg(args, 0).getStringData());
        } else {
            return SwitchBranch_Else;
        }
        Matrix nodeMtx(true);
        model.getNodeGlobalMtx(node, &nodeMtx, false);
        Vec3f axis(0.0f, 0.0f, 0.0f);
        if (kind == SwitchKind_RotSpeedDirTouchGroundNodeX || kind == SwitchKind_RotSpeedDirTouchGroundNodeNameX) {
            axis = Vec3f(1.0f, 0.0f, 0.0f);
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeY ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameY) {
            axis = Vec3f(0.0f, 1.0f, 0.0f);
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeZ ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameZ) {
            axis = Vec3f(0.0f, 0.0f, 1.0f);
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeNegX ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameNegX) {
            axis = Vec3f(-1.0f, 0.0f, 0.0f);
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeNegY ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameNegY) {
            axis = Vec3f(0.0f, -1.0f, 0.0f);
        } else if (kind == SwitchKind_RotSpeedDirTouchGroundNodeNegZ ||
                   kind == SwitchKind_RotSpeedDirTouchGroundNodeNameNegZ) {
            axis = Vec3f(0.0f, 0.0f, -1.0f);
        }
        fn_8003E3BC(&nodeMtx, &axis, &axis);
        // (The call takes the xy projection of the axis: Vec2f has a converting constructor from Vec3f.)
        // MATCH-ONLY: the cast makes the converted axis an unnamed temporary (a named local gets a frame slot of its own).
        return emKineticModuleImpl::getRotSpeedDirForAxisAdjustWithTouchGround(
            moduleAccesser, &static_cast<const Vec2f&>(Vec2f(axis)), 0xFF);
    }
    case SwitchKind_NearPostureParam:
    case SwitchKind_FarPostureParam: {
        // The posture parameter nearest to / farthest from a position (two floats), of the (at most five) addresses
        // that follow the position.
        if (argNum < 4 || argNum > 7) {
            return SwitchBranch_None;
        }
        Vec2f pos;
        READ_FLOAT_ARG(args, 0, moduleAccesser, pos.m_x)
        READ_FLOAT_ARG(args, 1, moduleAccesser, pos.m_y)
        void* postureParams[PostureParamMax] = {NULL, NULL, NULL, NULL, NULL};
        u32 postureNum = 0;
        for (int i = 2; i < argNum; i++) {
            READ_ADDRESS_ARG(args, i, moduleAccesser, postureParams[postureNum++])
            if (postureNum >= PostureParamMax) {
                break;
            }
        }
        Enemy& enemy = dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
        if (kind == SwitchKind_NearPostureParam) {
            return enemy.getNearPostureParamFromPosition(postureParams, postureNum, &pos);
        }
        if (kind == SwitchKind_FarPostureParam) {
            return enemy.getFarPostureParamFromPosition(postureParams, postureNum, &pos);
        }
        break;
    }
    }
    return fn_27_7B610(this, kind, argList, moduleAccesser);
}
