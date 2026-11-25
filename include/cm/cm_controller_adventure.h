#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <types.h>

struct cmAdventurePolygon {
    u16 m_numVertices;
    char _pad[2];
    Vec3f* m_vertices;
    Vec3f m_min;
    Vec3f m_max;
    char _pad2[0x20];
};
static_assert(sizeof(cmAdventurePolygon) == 0x40, "Class is wrong size!");

struct cmAdventurePolygonCollection {
    u32 m_count;
    cmAdventurePolygon* m_polygons;
};

class cmAdventureController : public gfCameraController {
public:
#ifdef ST_ADVENTURE2_IMPL
    union {
        struct {
            u32 unk8_mode : 8;
            u32 unk9_mode : 8;
        };
        struct {
            char _8[3];
            union {
                u8 unkB;
                struct {
                    u8 unkB_0 : 1;
                    u8 unkB_1 : 1;
                    u8 unkB_2 : 1;
                    u8 unkB_3 : 1;
                    u8 unkB_4 : 1;
                    u8 unkB_5 : 1;
                    u8 unkB_6 : 1;
                    u8 unkB_7 : 1;
                };
            };
        };
    };
#else
    char _8[3];
    union {
        u8 unkB;
        struct {
            u8 unkB_0 : 1;
            u8 unkB_1 : 1;
            u8 unkB_2 : 1;
            u8 unkB_3 : 1;
            u8 unkB_4 : 1;
            u8 unkB_5 : 1;
            u8 unkB_6 : 1;
            u8 unkB_7 : 1;
        };
    };
#endif
    u16 m_damage;
    u8 _E;
    u8 _F;
    u16 _10;
    u16 _12;
    u16 _14;
    char _16[2];
    float _18;
    float _1C;
    Vec3f _20;
    Vec3f _2C;
    Vec3f _38;
    char _44[12];
    Vec3f _50;
    Vec3f _5C;
    float _68;
    float _6C;
    float _70;
    float _74;
    float _78;
    float _7C;
    Vec3f _80;
    Vec3f _8C;
    float unk98;
    float unk9C;
    cmAdventurePolygonCollection* unkA0;
    float _A4;
    float _A8;
    float _AC;
    Vec3f _B0;
    Vec3f _BC;
    Vec3f _C8;
    float _D4;
    float _D8;
    float _DC;
    float _E0;
    cmSubject* m_subject;
    float _E8;
    float _EC;
    float _F0;
    float _F4;
    float _F8;
    float _FC;
    float _100;
    float _104;
    float _108;
    u32 _10C;
    Vec3f _110;
    Vec3f m_targetInsteadFighterPosition;
    float _128;
    float _12C;
    float _130;
    float _134;
    Vec3f _138;
    u8 unk144;
    u8 unk145;
    char _146[2];

    // HYPOTHESIS: inlined target position getter
    Vec3f getTargetPos() {
        if (m_subject != NULL) {
            return m_subject->m_pos;
        } else {
            _110 = m_targetInsteadFighterPosition;
            unkB_5 = 1;
            return _110;
        }
    }

    cmAdventureController();
    void searchSubject();
    Vec3f getTargetInsteadFighterPosition();
    void setTargetInsteadFighterPosition(Vec3f* pos);
    void updateParam();
    void correctSub(const clSegment2D& segment, Vec2f& pos, const Vec2f& halfSize);
    void calcOtherSubjectEffect();
    void setTargetWeightTable(float* table);
    void correct(Vec2f* pos);
    void correct_lockzone();
    virtual void update(float);
    virtual void draw();
    void resetTarget();
    void warpTarget();
    void setCameraDegXAngle();
    void initialize();
    void setBlownCorrectDamage();
    void setRateOfFlyTimeToDamage();
    void debugUpdate();
    void setQuakeAtOffset();
};

class cmAdventureFreeController : public cmAdventureController {
public:
    cmAdventureFreeController();
};
static_assert(sizeof(cmAdventureController) == 0x148, "Class is wrong size!");
