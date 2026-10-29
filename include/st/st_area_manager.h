// Local BrawlHeaders shadow: stAreaManager task (names from the sora_melee map) and the area record layouts its copies use
#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <st/st_trigger_observe.h>
#include <types.h>

struct stAreaInfoData {
    int m_areaId;
};

// HYPOTHESIS: the area records are an id plus the area data as a member, not a second base:
// stAreaManager copies whole records word by word and the data part member by member.
struct stMovementData {
    u16 m_gimmickKind;
    float m_speed;
    float m_vector;
    float _0xC;
    float _0x10;
    float _0x14;
    int m_taskId;
    int _0x1c;
};
static_assert(sizeof(stMovementData) == 0x20, "Class is wrong size!");

struct stMovementAreaInfoData {
    int m_areaId;
    stMovementData m_data;
};
static_assert(sizeof(stMovementAreaInfoData) == 0x24, "Class is wrong size!");

struct stWaterData {
    u16 m_gimmickKind;
    float m_swimHeight;
    float m_currentSpeed;
    u8 m_0xc;
};
static_assert(sizeof(stWaterData) == 0x10, "Class is wrong size!");

struct stWaterAreaInfoData {
    int m_areaId;
    stWaterData m_data;
};
static_assert(sizeof(stWaterAreaInfoData) == 0x14, "Class is wrong size!");

// HYPOTHESIS: field meanings follow grGimmickHitPointEffectData.
struct stHitPointEffectData {
    u16 m_gimmickKind;
    float m_damage;
    u16 m_serialHitFrame;
};
static_assert(sizeof(stHitPointEffectData) == 0xC, "Class is wrong size!");

struct stHitPointEffectAreaInfoData {
    int m_areaId;
    stHitPointEffectData m_data;
};
static_assert(sizeof(stHitPointEffectAreaInfoData) == 0x10, "Class is wrong size!");

class stAreaManager : public gfTask {
    stMovementAreaInfoData* m_windAreas;
    stMovementAreaInfoData* m_beltAreas;
    stWaterAreaInfoData* m_waterAreas;
    stHitPointEffectAreaInfoData* m_hitPointEffectAreas;
    u32 m_size;

public:
    stAreaManager();
    virtual void processBegin();
    virtual ~stAreaManager();

    void initMng(u32 size);
    void eraseAll();
    void clearAll();
    // HYPOTHESIS: parameter types of the set/get/search family come from register use.
    template <class T> bool search(T* areas, u32 areaId, u32* index);
    u32 setWind(stMovementAreaInfoData* data);
    u32 setBelt(stMovementAreaInfoData* data);
    u32 setWater(stWaterAreaInfoData* data);
    u32 setHitPointEffect(stHitPointEffectAreaInfoData* data);
    void setWindParam(stMovementAreaInfoData* data, u32 index);
    void setBeltParam(stMovementAreaInfoData* data, u32 index);
    void setWaterParam(stWaterAreaInfoData* data, u32 index);
    void erase(u32 areaId);
    bool getMovementArea(u32 areaId, stMovementData* data);
    bool getWater(u32 areaId, stWaterData* data);
    bool getHitPointEffect(u32 areaId, stHitPointEffectData* data);
};
static_assert(sizeof(stAreaManager) == 0x54, "Class is wrong size!");
