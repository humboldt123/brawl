#pragma once
// Local BrawlHeaders shadow: the stActTriggerFrameExcelCB layout and destructor recovered from st_trigger_actor_common.o.

#include <StaticAssert.h>
#include <gf/gf_callback.h>
#include <gf/gf_task.h>
#include <st/st_trigger_observe.h>
#include <types.h>
#include <ut/ut_list.h>
#include <yk/yakumono.h>

class stTriggerCB;
struct stEarthBeltData;

class stTrigger : public utListNode {

    Gimmick::AreaKind m_gimmickAreaKind;
    int m_id;
    union {
        struct {
            stTriggerData::KeepKind m_keepKind : 8;
            char _spacer[7];
        };
        struct {
            char unk10[4];
            int unk14;
        };
    };
    stObsTriggerCB* m_obsTriggerCB;
    Yakumono* m_yakumono;
    union {
        char _spacer2[12];
        struct {
            // HYPOTHESIS: callback list heads at 0x20, 0x24, 0x28
            stTriggerCB* unk20;
            stTriggerCB* unk24;
            stTriggerCB* unk28;
        };
    };
    bool m_isValid;
    char _spacer3[3];

public:
    friend class stActTriggerFrameExcelCB;
    virtual ~stTrigger();

    void process();

    void setObserveYakumono(Yakumono* yakumono);
    void setValidCallback(gfCallBack* cb);
    void setValidEdgeCallback(gfCallBack* cb);
    void setUnValidEdgeCallback(gfCallBack* cb);
    stObsTriggerTriBeltConveyorCB* setBeltConveyorTrigger(stEarthBeltData* belt);
    stObsTriggerSquareBeltConveyorCB* setPressureAreaTrigger(grGimmickPressureAreaData* pressure);
    void setBeltConveyorParam(stEarthBeltData* belt);
    void setPressureParam(grGimmickPressureAreaData* pressure);
    stObsTriggerSquareBeltConveyorCB* setBeltConveyorTrigger(grGimmickBeltConveyorData* gimmickAreaData);
    stObsTriggerSquareHitPointEffectCB* setHitPointEffectTrigger(grGimmickHitPointEffectData* gimmickAreaData);
    stObsTriggerSquareWaterCB* setWaterTrigger(grGimmickWaterData* gimmickAreaData);
    stObsTriggerSquareWindCB* setWindTrigger(grGimmickWindData* gimmickAreaData);

    void setAreaSleep(bool);
    void setKeyAreaTrigger(stGimmickAreaData* areaData, Vec2f pos);
    void setTruckAreaTrigger(stSimpleAreaData* areaData);
    void setObserveFlag(bool flag);
    void setFrameTrigger(float frame);
    void setWindParam(grGimmickWindData2nd* wnd, s32 p2);
    void setBeltConveyorParam(grGimmickBeltConveyorData* belt);
    //void setBeltConveyorParam(grGimmickTriBeltConveyorData* belt);
    void setWaterParam(grGimmickWaterData* water);


};
static_assert(sizeof(stTrigger) == 52, "Class is wrong size!");

class stTriggerCB : public gfCallBack {
public:
    virtual ~stTriggerCB() {
        if (dynamic_cast<stTriggerCB*>(m_next)) {
            stTriggerCB* next = dynamic_cast<stTriggerCB*>(m_next);
            delete next;
        }
    }

    void updateBegin() { }
    void updateFixPosition() { }
    void updateEnd() { }
};

// HYPOTHESIS: unk8 is initialized by a non-virtual base, before the derived vtable store.
class stActTriggerFrameCBZero {
public:
    u32 unk8;

    stActTriggerFrameCBZero() : unk8(0) { }
};

class stActTriggerFrameExcelCB : public stTriggerCB, public stActTriggerFrameCBZero {
public:
    float unkC;
    bool unk10;
    char _spacer11[3];
    grGimmickFrameTriggerData* m_data;
    stTrigger* m_trigger;

    stActTriggerFrameExcelCB(grGimmickFrameTriggerData* data);
    virtual ~stActTriggerFrameExcelCB();
    virtual void userProc();
};
static_assert(sizeof(stActTriggerFrameExcelCB) == 0x1C, "Class is the wrong size!");

class stTriggerMng : public gfTask {
    utList m_triggerList;
    char _spacer[24];

public:
    stTriggerMng(const char* taskName);
    virtual void processBegin();
    virtual void processFix();
    virtual void processGameProc();
    virtual ~stTriggerMng();

    stTrigger* createTrigger(Gimmick::AreaKind gimmickKind, int triggerId);
    stTrigger* createTrigger(Gimmick::AreaKind gimmickKind, stTriggerData* triggerData);
    void setTriggerFlag(stTriggerData* triggerData);
    bool getTriggerFlag(stTriggerData* triggerData, bool defaultFlag);
    void setValidfromAreaID(u32 areaId);
};
static_assert(sizeof(stTriggerMng) == 100, "Class is wrong size!");

extern stTriggerMng* g_stTriggerMng;
