#pragma once

#include <mu/mu_object.h>
#include <mu/mu_menuroot.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <mu/mu_msg.h>
#include <types.h>

// nw4r::g3d::ScnMdlExpand::PushBack(object, node name): puts a model in the group at the node with the name.
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);

// The models of the tournament menus are put in groups of the scene (the groups are ScnMdlExpand, made by the menus).
static inline void tourPushBack(nw4r::g3d::ScnGroup* group, nw4r::g3d::ScnMdl* mdl, const char* name) {
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(group, mdl, name);
}

// Takes all the models out of a group of the menu.
static inline void tourClearGroup(nw4r::g3d::ScnGroup* group) {
    while (group->sceneItemsCount != 0) {
        group->Remove(group->sceneItemsCount - 1);
    }
}

// Puts a model in the scene of the menu (at the end of the scene), or takes it out.
static inline void tourAttach(MenuRoot* root, nw4r::g3d::ScnMdl* mdl) {
    root->scene->Insert(root->scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(mdl));
}
static inline void tourDetach(MenuRoot* root, nw4r::g3d::ScnMdl* mdl) {
    root->scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(mdl));
}

// MuMsg::printData(message, data, length): shows a text of bytes that is not one of the messages of the file.
extern "C" void printData__5MuMsgFUlPCvi(MuMsg* msg, u32 msgIndex, const void* data, int length);
static inline void tourPrintData(MuMsg* msg, u32 msgIndex, const void* data, int length) {
    printData__5MuMsgFUlPCvi(msg, msgIndex, data, length);
}

// Copies the position of an object of the menu (the fields one by one).
static inline void tourGetTrans(const MuObject* obj, Vec3f* out) {
    out->m_x = obj->m_trans.m_x;
    out->m_y = obj->m_trans.m_y;
    out->m_z = obj->m_trans.m_z;
}
