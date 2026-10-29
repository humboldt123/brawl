#pragma once

// Local BrawlHeaders shadow: the ykAreaData members are cleared inline by the sora_melee constructor (0x270838).
#define YK_DATA_INLINE_CTOR
#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <so/area/so_area_module_impl.h>
#include <st/st_trigger_observe.h>
#include <types.h>
#include <yk/yakumono.h>

class grGimmickAreaCommon : public grYakumono {
protected:
    stGimmickAreaData* m_gimmickAreaData; // 0x150
    soAreaData m_areaData;                // 0x154
    soSet<soAreaData> m_areaDataSet;      // 0x174
    ykAreaData m_ykData;                  // 0x17C

public:
    grGimmickAreaCommon(const char* taskName);
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grGimmickAreaCommon();
};
static_assert(sizeof(grGimmickAreaCommon) == 0x188, "Class is wrong size!");
