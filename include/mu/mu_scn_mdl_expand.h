#pragma once

#include <nw4r/g3d/g3d_scngroup.h>

namespace nw4r { namespace g3d {
// Declaration prefix for the attachment group (the groups that the menus make out of a model and put other models in
// at the bones with the names pos00, pos01 ...). The storage after ScnGroup is not modeled here; this interface is used
// only through existing pointers.
class ScnMdlExpand : public ScnGroup {
public:
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}
