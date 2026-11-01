#pragma once

#include <so/collision/so_collision_hit_part.h>
#include <st_gw/gr_gw.h>
#include <yk/yakumono.h>

// The cooking scene: four chefs stand at four places, each with a pan (the "meal") that burns step by step and hurts fighters
// that are near. When the chef takes enough damage he flies away (the stage ends the scene). A hit object has one capsule for
// every chef and the attacks of the meals.
class grGWChefEtc : public grGW {
protected:
    struct Meal { // Name unknown: one of the four pans
        float m_timer;      // frames until the next step
        char _4[4];
        u8 m_stage;         // which of the five models of the pan is shown
        u8 m_state;         // 2 = waits, 3 = goes back, 4 = burns more, 5 = flames out
        u8 m_repeat;        // how many more times the flames blink
        u8 m_attackOn;
    };
    u8* m_sceneWork;                // 0x174 the scene that plays (from the stage)
    u8 m_sceneNo;                   // 0x178 the scene that the ground belongs to
    char _179[3];
    u32 m_nodeCooking;              // 0x17C the node of the pans
    u32 m_nodeMeal[20];             // 0x180 the nodes of the pan models (4 pans x 5 stages)
    u32 m_nodeChara;                // 0x1D0 the node of the chefs
    u32 m_nodeChef[4][3];           // 0x1D4 the nodes of the chefs (body and two hands)
    Meal m_meal[4];                 // 0x204
    float m_timerChef;              // 0x234 frames until the next step of the chef
    float m_hpChef;                 // 0x238 how much damage the chef takes before he flies away
    u8 m_posChef;                   // 0x23C the place of the chef
    u8 m_stateChef;                 // 0x23D 6 = cooks, 7 = turns the meal, 8 = flies away
    char _23e;
    u8 m_attackOnChef;              // 0x23F
    Vec3f* m_posLimitWork;          // 0x240 the camera limits (from the stage)
    Vec3f m_posEnd;                 // 0x244 where the chef flies to
    Vec3f m_posStart;               // 0x250 where he starts
    Vec3f m_posNow;                 // 0x25C where he is
    Vec3f m_rot;                    // 0x268 how he is turned (the last is the spin)
    u8 m_flying;                    // 0x274 1 = the flying chef is set up
    char _275[3];
    int m_nodeFly;                  // 0x278 the node of the chef that flies
    u8 m_played;                    // 0x27C
    u8 m_yakumonoMade;              // 0x27D
    char _27e[2];
    soCollisionHitData* m_hitData;              // 0x280 the capsules of the chefs
    soCollisionHitData::Simple* m_hitSimple;    // 0x284
    soSet<soCollisionHitData::Simple>* m_hitSet;        // 0x288
    ykDataGroup* m_dataGroup;       // 0x28C
    ykData* m_data;                 // 0x290
    int m_dangerZone[4];            // 0x294 the danger zones of the AI (one for every pan)

public:
    static grGWChefEtc* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWChefEtc(const char* taskName);
    virtual ~grGWChefEtc();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void onInflict(soCollisionLog* log, u32 unk, float damage);
    virtual void updateCallBack(float deltaFrame);
    virtual void setSceneIDWork(u8* scene);
    virtual void setSceneID(u8 scene);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateChef(float deltaFrame);
    virtual void updateMeal(float deltaFrame);
    virtual void setHit();
    virtual void setAttackMeal(u32 index);
    virtual void setAttackChef(u32 index);
    virtual void selectPosChef();
    virtual bool isTurnMeal(u32 index);
    virtual void setPosLimitWork(Vec3f* limit);
};
static_assert(sizeof(grGWChefEtc) == 0x2A4, "Class is wrong size!");
