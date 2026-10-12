#pragma once

#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/GX.h>
#include <types.h>

// The color channel of a material (its color and alpha): main.dol has the functions, the shared g3d header does not.
namespace nw4r {
namespace g3d {
struct ResChanData;
class ResMatChan : public ResCommon<ResChanData> {
public:
    inline ResMatChan() : ResCommon() { }
    bool GXGetChanMatColor(GXChannelID id, GXColor* color) const;
    void GXSetChanMatColor(GXChannelID id, GXColor color);
};
}  // namespace g3d
}  // namespace nw4r
// nw4r::g3d::ScnMdl::CopiedMatAccess::GetResMatChanEx()
extern "C" nw4r::g3d::ResMatChan GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(nw4r::g3d::ScnMdl::CopiedMatAccess* access);
// nw4r::g3d::ResMdl: the number of materials, and a material by number
extern "C" u32 GetResMatNumEntries__Q34nw4r3g3d6ResMdlCFv(const nw4r::g3d::ResMdl* mdl);
extern "C" nw4r::g3d::ResMat GetResMat__Q34nw4r3g3d6ResMdlCFi(const nw4r::g3d::ResMdl* mdl, int index);
