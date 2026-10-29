#pragma once

// The link module of the enemies (mo_enemy/sora_enemy/link/em_link_module_impl.cpp): soLinkModuleImpl that keeps a
// matrix between the node an enemy is constrained to and the node of its model, and handles the enemy's link anim cmds
// (group 'n'). Only declared with the full soLinkModuleImpl layout, which the shared header gives to FT_MODULE_BUILDER
// units; the unit also defines SO_LINK_MODULE_IMPL_INLINE_VIRTUALS for the base's inline accessors.

#include <StaticAssert.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <so/anim/so_anim_cmd_arg_list.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

class emLinkModuleImpl : public soLinkModuleImpl {
public:
    u32 unk50;                       // 0x50
    Matrix m_constraintOffsetMtx;    // 0x54 (constraint node to model node, see setConstraintOffsetMtx)

    emLinkModuleImpl(soModuleAccesser* acc, soArray<soLinkConnection>* connections);
    virtual ~emLinkModuleImpl();
    // The constraint's world matrix, followed by the offset matrix.
    virtual void getModelConstraintTargetNodeWorldMtx(Matrix* mtx, bool unk2);
    virtual Vec3f getModelConstraintTargetNodePosition();
    virtual Vec3f getModelConstraintTargetNodeRotation();
    virtual bool isObserv(char group);
    // The anim cmds of group 'n': type 0 / 1 send a link event to the nodes, 2 - 4 send an "execute anim cmd" event
    // (0x13F4 - 0x13F6), 5 / 6 set the constraint offset from a model node (by id / by name).
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);

    void initConstraintOffsetMtx();
    // HYPOTHESIS: node is the model node the enemy's constraint should keep its current place relative to.
    void setConstraintOffsetMtx(int node);
};
static_assert(sizeof(emLinkModuleImpl) == 0x84, "Class is wrong size!");

// The link event the anim cmds 'n' 0 / 1 send: the first anim cmd argument and the cmd's argument list.
// HYPOTHESIS: names (weapon articles read the event kind and the argument list).
struct wnemLinkEventArgs : public soLinkEventArgs {
    int m_param;
    soAnimCmdArgList* m_argList;
    wnemLinkEventArgs(int eventKind, int param, soAnimCmdArgList* argList)
        : soLinkEventArgs(eventKind), m_param(param), m_argList(argList) { }
};

// The link event the anim cmds 'n' 2 - 4 send (event kinds 0x13F4 - 0x13F6).
struct wnemLinkEvent_ExecAnmCmd : public soLinkEventArgs {
    int m_param;
    void* m_cmdAddress;
    wnemLinkEvent_ExecAnmCmd(int eventKind, int param, void* cmdAddress)
        : soLinkEventArgs(eventKind), m_param(param), m_cmdAddress(cmdAddress) { }
};
