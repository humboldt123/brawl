#pragma once

#include <snd/snd_3d_generator.h>
#include <st_gw/gr_gw.h>
#include <st_gw/gr_gw_scene_oil.h>
#include <so/collision/so_collision_hit_part.h>
#include <yk/yakumono.h>

// A guest of the oil panic scene (the man or the woman): it moves between the places of its guest data and turns angry when
// the oil spills on it. The guest data is shared between the calm guest and its angry version (grGWOilGuestAnger).
class grGWOilGuest : public grGW {
protected:
    grGWGuestData* m_dataGuest;     // 0x174 the places and state of the guest (from the stage)
    u8* m_sceneWork;                // 0x178 the scene that plays (from the stage)
    u8 m_sceneNo;                   // 0x17C the scene that the ground belongs to
    char _17d[3];
    Vec3f m_pos;                    // 0x180 where the guest is now
    u8 m_type;                      // 0x18C 0 = man, 1 = angry man, 2 = woman, 3 = angry woman
    u8 m_played;                    // 0x18D the animation of the appearing is over
    u8 m_yakumonoMade;              // 0x18E
    char _18f;
    soCollisionHitData* m_hitData;              // 0x190 the hit sphere
    soCollisionHitData::Simple* m_hitSimple;    // 0x194
    soSet<soCollisionHitData::Simple>* m_hitSet;        // 0x198
    ykDataGroup* m_dataGroup;       // 0x19C
    ykData* m_data;                 // 0x1A0
    snd3DGenerator m_snd;           // 0x1A4

public:
    static grGWOilGuest* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWOilGuest(const char* taskName);
    virtual ~grGWOilGuest();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateCallBack(float deltaFrame);
    virtual void setSceneIDWork(u8* scene);
    virtual void setSceneID(u8 scene);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setHit();
    virtual void setDataGuestWork(grGWGuestData* data);
    virtual void setType(u8 type);
};
static_assert(sizeof(grGWOilGuest) == 0x1AC, "Class is wrong size!");

// The angry guest: it has no hit spheres, only an attack.
class grGWOilGuestAnger : public grGWOilGuest {
public:
    inline grGWOilGuestAnger(const char* taskName) : grGWOilGuest(taskName) { }
    virtual ~grGWOilGuestAnger();
    virtual void setHit();
};
static_assert(sizeof(grGWOilGuestAnger) == 0x1AC, "Class is wrong size!");

class grGWOilGuestAngerM : public grGWOilGuestAnger {
protected:
    u8 m_attackOn;              // 0x1AC
    char _1ad[3];
    int m_dangerZone;           // 0x1B0 the id of the danger zone of the AI

public:
    static grGWOilGuestAngerM* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    inline grGWOilGuestAngerM(const char* taskName);
    virtual ~grGWOilGuestAngerM();
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setAttack();
    virtual float getMotionFrameMatCol();
};
static_assert(sizeof(grGWOilGuestAngerM) == 0x1B4, "Class is wrong size!");

class grGWOilGuestAngerW : public grGWOilGuestAnger {
protected:
    u8 m_attackOn;              // 0x1AC
    char _1ad[3];
    int m_dangerZone;           // 0x1B0 the id of the danger zone of the AI
    float m_timerTurn;          // 0x1B4 frames until the woman moves on to the next place

public:
    static grGWOilGuestAngerW* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    inline grGWOilGuestAngerW(const char* taskName);
    virtual ~grGWOilGuestAngerW();
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setAttack();
};
static_assert(sizeof(grGWOilGuestAngerW) == 0x1B8, "Class is wrong size!");
