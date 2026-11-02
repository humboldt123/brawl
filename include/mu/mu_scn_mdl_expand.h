#pragma once

#include <MEM/mem_allocator.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

namespace nw4r { namespace g3d {
class ScnMdl;
// Declaration prefix for the attachment group (the groups that the menus make out of a model and put other models in
// at the bones with the names pos00, pos01 ...). The storage after ScnGroup is not modeled here; this interface is used
// only through existing pointers.
class ScnMdlExpand : public ScnGroup {
public:
    // nw4r's ScnMdlExpand::Construct (g3d_scnmdlexpand.cpp): the group with room for `capacity` objects on the nodes of the model.
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int capacity, ScnMdl* model);
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}
