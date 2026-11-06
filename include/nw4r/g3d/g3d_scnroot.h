#pragma once

// Local BrawlHeaders shadow: BrawlHeaders has no nw4r::g3d::ScnRoot; declares the root members the game calls.

#include <nw4r/g3d/g3d_rescommon.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

struct MEMAllocator;

namespace nw4r {
namespace g3d {
class AnmScn;
class Camera;
struct FogData;

// HYPOTHESIS: a view of nw4r::g3d::Fog (one pointer to the fog data of the scene); only Init() is known to the game code.
class Fog : public ResCommon<FogData> {
public:
    explicit Fog(FogData* data) : ResCommon(data) {}
    void Init();
};

class ScnRoot : public ScnGroup {
public:
    static ScnRoot* Construct(MEMAllocator* allocator, u32* size, u32 maxChildren, u32 maxScnObj, u32 numLightObj,
                              u32 numLightSet);

    Camera GetCamera(int index);
    // The fog of the given index; it is not valid (Fog::IsValid) past the last one. IfCoinShooter's constructor resets them all.
    Fog GetFog(int index);
    void SetCurrentCamera(int index);
    void SetAnmScn(AnmScn* anmScn);
    void UpdateFrame();
    void CalcWorld();
    void CalcMaterial();
    void CalcView();
    void GatherDrawScnObj();
    void ZSort();
    void DrawOpa();
    void DrawXlu();
};
} // namespace g3d
} // namespace nw4r
