// The full soLinkModuleImpl and the constructor of its base are declared only for the module builders.
#define FT_MODULE_BUILDER
// This unit has its own weak copies of the soLinkModuleImpl accessors its vtable keeps.
#define SO_LINK_MODULE_IMPL_INLINE_VIRTUALS
// soLinkModuleImpl declares its anim cmd handler with an int index; the observer base must take the same one for the
// thunk of the observer vtable to be the module's.
#define WN_ARTICLE_ANIMCMD_INT
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
#define SO_ARRAY_CONTRACTIBLE_TABLE_COPY_EXTERN
#include <em/enemy.h>
#include <em/link/em_link_module_impl.h>
#include <so/anim/so_anim_cmd_arg_list.h>
#include <so/so_module_accesser.h>
#include <types.h>

// The argument types of acCmdArg::getArgType the link anim cmds accept.
static const s32 CmdArgType_Int = 0;
static const s32 CmdArgType_Address = 2;
static const s32 CmdArgType_String = 4;

// The event kinds of the "execute anim cmd" events (anim cmd 'n', types 2 - 4). HYPOTHESIS: names.
static const int LinkEvent_ExecAnmCmd0 = 0x13F4;
static const int LinkEvent_ExecAnmCmd1 = 0x13F5;
static const int LinkEvent_ExecAnmCmd2 = 0x13F6;

// The argument at index (the acCmdArg getters stay out of line in this unit, as in the original).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

emLinkModuleImpl::emLinkModuleImpl(soModuleAccesser* acc, soArray<soLinkConnection>* connections)
    : soLinkModuleImpl(acc, connections) { }

emLinkModuleImpl::~emLinkModuleImpl() { }

void emLinkModuleImpl::initConstraintOffsetMtx() {
    m_constraintOffsetMtx.setIdentity();
}

// The offset that keeps the node the enemy is constrained to where it is: the inverse of the model node's world matrix
// (node) followed by the constraint node's world matrix.
void emLinkModuleImpl::setConstraintOffsetMtx(int node) {
    Matrix nodeMtx;
    Matrix constraintMtx;
    Matrix inverseMtx;
    m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_modelConstraintNode, &constraintMtx, false);
    m_moduleAccesser->getModelModule().getNodeGlobalMtx(node, &nodeMtx, false);
    nodeMtx.inverse(&inverseMtx);
    m_constraintOffsetMtx.setIdentity();
    inverseMtx.mul(&constraintMtx, &m_constraintOffsetMtx);
}

void emLinkModuleImpl::getModelConstraintTargetNodeWorldMtx(Matrix* mtx, bool unk2) {
    soLinkModuleImpl::getModelConstraintTargetNodeWorldMtx(mtx, unk2);
    mtx->mul(&m_constraintOffsetMtx, mtx);
}

Vec3f emLinkModuleImpl::getModelConstraintTargetNodePosition() {
    Matrix mtx(true);
    getModelConstraintTargetNodeWorldMtx(&mtx, false);
    // HYPOTHESIS: the translation is read through the const accessor (by value); the float& one gives the same loads
    // in the opposite registers.
    const Matrix& worldMtx = mtx;
    return Vec3f(worldMtx(0, 3), worldMtx(1, 3), worldMtx(2, 3));
}

Vec3f emLinkModuleImpl::getModelConstraintTargetNodeRotation() {
    Matrix mtx(true);
    getModelConstraintTargetNodeWorldMtx(&mtx, false);
    Vec3f rot;
    mtx.getRotate(&rot);
    return rot;
}

// MATCH-ONLY: anim cmd 'n' 2 sends its event through this helper. The original created that event's temporary after the
// ones of types 3 and 4 (its frame slot is below theirs, while every other temporary of the function is laid out in
// source order); the temporary of an inlined call is created at inlining time, which reproduces it. Types 3 and 4
// construct theirs in place.
static inline void sendExecAnmCmdEvent(emLinkModuleImpl* link, int eventKind, int id, void* cmdAddress) {
    link->sendEventNodes(3, wnemLinkEvent_ExecAnmCmd(eventKind, id, cmdAddress), 0);
}

bool emLinkModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    if (acmd->getGroup() != 'n') {
        return soLinkModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 7) {
        return false;
    }
    switch (type) {
    case 0: {
        // Send a link event to the nodes: (id, event kind[, arguments for the receiver]).
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        acCmdArg arg0;
        if (!acmd->getArg(&arg0, 0) || arg0.getArgType() != CmdArgType_Int) {
            return true;
        }
        int id = arg0.getIntData();
        acCmdArg arg1;
        if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != CmdArgType_Int) {
            return true;
        }
        soAnimCmdArgList argList(moduleAccesser, acmd->getArgList());
        sendEventNodes(3, wnemLinkEventArgs(arg1.getIntData(), id, &argList), 0);
        return true;
    }
    case 1: {
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        acCmdArg arg0;
        if (!acmd->getArg(&arg0, 0) || arg0.getArgType() != CmdArgType_Int) {
            return true;
        }
        int id = arg0.getIntData();
        acCmdArg arg1;
        if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != CmdArgType_Int) {
            return true;
        }
        soAnimCmdArgList argList(moduleAccesser, acmd->getArgList());
        sendEventNodes(3, wnemLinkEventArgs(arg1.getIntData(), id, &argList), 0);
        return true;
    }
    case 2: {
        // Have the nodes execute an anim cmd: (id, address of the cmd).
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        acCmdArg arg0;
        if (!acmd->getArg(&arg0, 0) || arg0.getArgType() != CmdArgType_Int) {
            return true;
        }
        int id = arg0.getIntData();
        acCmdArg arg1;
        if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != CmdArgType_Address) {
            return true;
        }
        sendExecAnmCmdEvent(this, LinkEvent_ExecAnmCmd0, id, arg1.getAddressData());
        return true;
    }
    case 3: {
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        acCmdArg arg0;
        if (!acmd->getArg(&arg0, 0) || arg0.getArgType() != CmdArgType_Int) {
            return true;
        }
        int id = arg0.getIntData();
        acCmdArg arg1;
        if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != CmdArgType_Address) {
            return true;
        }
        sendEventNodes(3, wnemLinkEvent_ExecAnmCmd(LinkEvent_ExecAnmCmd1, id, arg1.getAddressData()), 0);
        return true;
    }
    case 4: {
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        acCmdArg arg0;
        if (!acmd->getArg(&arg0, 0) || arg0.getArgType() != CmdArgType_Int) {
            return true;
        }
        int id = arg0.getIntData();
        acCmdArg arg1;
        if (!acmd->getArg(&arg1, 1) || arg1.getArgType() != CmdArgType_Address) {
            return true;
        }
        sendEventNodes(3, wnemLinkEvent_ExecAnmCmd(LinkEvent_ExecAnmCmd2, id, arg1.getAddressData()), 0);
        return true;
    }
    case 5:
    case 6: {
        // Set the constraint offset from a model node: 5 by node id, 6 by node name.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        // Unsigned: the original tests it against -1 with an unsigned compare (addis / cmplwi 0xffff).
        u32 nodeId = -1;
        if (type == 5) {
            if (getCmdArg(args, 0).getArgType() != CmdArgType_Int) {
                return true;
            }
            nodeId = getCmdArg(args, 0).getIntData();
        } else if (type == 6) {
            if (getCmdArg(args, 0).getArgType() != CmdArgType_String) {
                return true;
            }
            // The model module is read before the name argument.
            soModelModule& modelModule = moduleAccesser->getModelModule();
            nodeId = modelModule.getNodeId(getCmdArg(args, 0).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        setConstraintOffsetMtx(nodeId);
        return true;
    }
    }
    return false;
}

bool emLinkModuleImpl::isObserv(char group) {
    bool observ = false;
    if (group == 0xF || group == 'n') {
        observ = true;
    }
    return observ;
}
