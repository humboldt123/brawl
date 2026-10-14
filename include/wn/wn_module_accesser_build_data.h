// Local shadow of BrawlHeaders/wn/wn_module_accesser_build_data.h: the complete constructor (sora_melee) and the stored
// values that the article constructors read directly.
#pragma once

#include <StaticAssert.h>
#include <so/so_module_accesser_build_data.h>
#include <types.h>

// The build data of an article: the module accesser builder queries it through the soModuleAccesserBuildData interface
// (sora_melee implements the queries from the article data block).
// HYPOTHESIS: names of the values beyond kind, data, heaps and resource group.
class wnModuleAccesserBuildData : public soModuleAccesserBuildData {
public:
    s32 m_kind;                     // +0x4
    const void* m_data;             // +0x8: the article's static data block
    u8 m_resGroupNo;                // +0xC
    s32 unk10;
    soParamAccesser* m_paramAccesser; // +0x14
    s32 unk18;
    s32 unk1C;
    Heaps::HeapType m_instanceHeap;       // +0x20
    Heaps::HeapType m_nwModelInstanceHeap; // +0x24
    Heaps::HeapType m_nwMotionInstanceHeap; // +0x28
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;

    wnModuleAccesserBuildData(s32 kind, const void* data, Heaps::HeapType instHeap, Heaps::HeapType nwModelInstHeap,
                              Heaps::HeapType nwMotionInstHeap, u8 resGroupNo, s32 unk10, soParamAccesser* paramAccesser,
                              s32 unk18, s32 unk1C, s32 unk2C, s32 unk30, s32 unk34, s32 unk38, s32 unk3C);

    virtual u32 getMdlResId() const;
    virtual u32 getAnmResId() const;
    virtual u8 getResGroupNo() const;
    virtual s32 getTeam() const;
    virtual soMotionData* getMotionData() const;
    virtual soShakeData* getShakeData() const;
    virtual soVisibilityData* getVisibilityData() const;
    virtual soStatusData* getStatusData() const;
    virtual soGroundConditionChecker* getGroundConditionChecker() const;
    virtual void* getEffectEmitData() const;
    virtual soEffectCommonData* getEffectCommonData() const;
    virtual soParamAccesser* getParamAccesser() const;
    virtual s32 getAreaCategory() const;
    virtual void* getCameraRangeSet() const;
    virtual void* getCameraClipSphereSet() const;
    virtual void* getItemNodeData() const;
    virtual soJostleData* getJostleData() const;
    virtual Heaps::HeapType getInstanceHeap() const;
    virtual Heaps::HeapType getNWModelInstanceHeap() const;
    virtual Heaps::HeapType getNWMotionInstanceHeap() const;
    virtual void* getAnimCmdData(u32 eventThread, s32 p2) const;
};
static_assert(sizeof(wnModuleAccesserBuildData) == 0x40, "Class is wrong size!");

class soHeapModule;
// HYPOTHESIS: the Game & Watch articles build their data through one inline helper that takes the owner's heap module as a parameter
// (the module pointer is read once and the three heap getters are called on it, then the resource group is read from the data).
class wnModuleAccesserBuildDataByHeap : public wnModuleAccesserBuildData {
public:
    template <class D>
    wnModuleAccesserBuildDataByHeap(s32 kind, const D* data, soHeapModule* heap, soParamAccesser* paramAccesser) :
        wnModuleAccesserBuildData(kind, data, heap->getInstanceHeap(), heap->getNWModelInstanceHeap(), heap->getNWMotionInstanceHeap(),
                                  data->m_resGroupNo, -1, paramAccesser, 0, 0, 0, 0, 0, 0, 0) { }
};

