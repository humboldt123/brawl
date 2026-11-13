// Local BrawlHeaders shadow: ecMgr with typed trace storage and the effect-group calls.
#pragma once

#include <StaticAssert.h>
#include <ec/ec_trace_mgr.h>
#include <ef/ef_id.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <so/ground/so_ground_module_impl.h>
#include <types.h>

class ecMgr;
class ecScnObjCallback;
class efEffect;
class gfArchive;
class gfModelAnimation;
namespace nw4r { namespace g3d { class ScnGroup; class ResFile; } }
class ecScnObjCallback {
public:
    virtual ~ecScnObjCallback();
    // HYPOTHESIS: parameter type from boolean condition check in SetCallbackCondition.
    void SetCallbackCondition(bool);

    nw4r::g3d::ScnObj* unk4;
};
class ecResource {
public:
    // HYPOTHESIS: resource storage types follow word accesses and null checks.
    gfArchive* unk0;
    void* unk4;
    void* unk8;
    // HYPOTHESIS: rows of effect data begin 0x10 bytes into this table.
    struct {
        u8 unk0[0x10];
        struct {
            s16 unk0;
            s16 unk2;
            u32 unk4;
            u32 unk8;
            u32 unkC;
        } unk10[1];
    }* unkC;
    u32 unk10;
    u8 unk14;
    u8 unk15[3];

    ecResource();
    ~ecResource();
    void release(bool);
    void set_file_img(void*, int, u32);
    void regist(u32);
    // HYPOTHESIS: parameter types from the tail call in ecEffectWork::change_mdl_tex.
    void change_tex(nw4r::g3d::ScnMdl*, u32, const char*, const char*, nw4r::g3d::ResFile*, int);
};

class ecEffectWork {
public:
    // HYPOTHESIS: translation, rotation and scale stored for Matrix::setSRT.
    Vec3f m_pos;
    Vec3f m_rot;
    Vec3f m_scale;
    u32 unk24;
    s32 unk28;
    u32 unk2C;
    ecResource* unk30;
    union {
        u8 unk34[0x28];
        struct {
            efEffect** unk34_;
            u32 unk38;
            s32 unk3C;
            nw4r::g3d::ScnGroup* unk40;
            nw4r::g3d::ScnMdl* unk44;
            gfModelAnimation* unk48;
            ecScnObjCallback* unk4C;
            nw4r::g3d::ScnMdl* unk50;
            u16 unk54;
            u16 unk56;
            u8 unk58;
            u8 unk59;
            u8 unk5A;
            u8 unk5B;
        };
    };
    u32 unk5C;
    u16 unk60;
    u16 unk62;
    // HYPOTHESIS: ground line; non-null skips the free model matrix.
    u32 unk64;
    u32 unk68;
    float unk6C;
    // HYPOTHESIS: optional model-matrix callback and its argument.
    void (*unk70)(nw4r::g3d::ScnMdl*, void*);
    void* unk74;
    u32 unk78;
    u32 unk7C;
    u32 unk80;

    ecEffectWork();
    void update_odd(float frames);
    void renew_attach();
    void render();
    void release(bool);
    void pause();
    void play();
    void end(bool, bool);
    // HYPOTHESIS: parameter types from ecMgr::setParent forwarding.
    void set_parent(nw4r::g3d::ScnMdl*, const char*, bool);
    void set_parent(nw4r::g3d::ScnMdl*, u32, bool);
    void init_attach();
    void set_delay(int);
    void set_slow_rate(u32);
    void set_mdl_enable_update(bool, s8);
    void set_visible(bool);
    void set_rot(const Vec3f*);
    void set_scl(const Vec3f*);
    void set_mdl_texpat(u32);
    void set_mdl_frame(float, s8);
    void set_mdl_calc_mtx(void (*)(nw4r::g3d::ScnMdl*, void*), void*);
    void change_mdl_tex(const char*, const char*, nw4r::g3d::ResFile*, int);
};
static_assert(sizeof(ecEffectWork) == 0x84, "ecEffectWork size");

extern ecMgr* g_ecMgr;
class ecMgr : public gfTask {
public:
    union {
        u8 unk40[0x50];
        struct {
            u8 unk40_;
            u8 unk41;
            u8 unk42;
            u8 unk43;
            u32 unk44;
            u32 unk48;
            nw4r::g3d::ScnGroup* unk4C;
            nw4r::g3d::ScnGroup* unk50;
            nw4r::g3d::ScnGroup* unk54;
            ecResource* unk58;
            u32 unk5C;
            ecEffectWork* unk60;
            u32 unk64;
            u32 unk68;
            u32 unk6C;
            u32 unk70;
            u32 unk74;
            u32 unk78;
            u32 unk7C;
            u8 unk80;
            u8 unk81;
            u8 unk82[2];
            u32 unk84;
            u8 unk88[8];
        };
    };
    u32 unk90;
    u32 unk94;
    ecTraceMgr m_traceMgr;

    ecMgr();
    // HYPOTHESIS: counts are unsigned, as shown by allocation and bounds checks.
    ecMgr(u32, u32, u32);
    static void create(u32, u32, u32);
    void registResource(char*, void*, int, int);
    void render();
    void setHeapInstance(int);
    void setMdlPriority(u32, int);
    bool isExistEffectResource(u32);
    bool isExistEffect(u32);
    void setMdlCalcMtx(u32, void (*)(nw4r::g3d::ScnMdl*, void*), void*);
    void releaseResource(char*);
    void setDelay(u32 handleId, int frames);
    void killEffectGroup(u32 group, u32 mask, bool immediate, bool release, int effectKind);
    void setDrawPrio(int prio);
    u32 setEffect(EfID effectID);
    u32 setEffect(EfID effectID, Vec3f* pos);
    u32 setEffect(EfID effectID, const Vec3f* pos);
    u32 setEffect(EfID effectID, u32 resourceId, const Vec3f* pos, const Vec3f* rot, const Vec3f* scale);
    u32 setEffect(EfID effectID, const Vec3f* pos, const Vec3f* rot, const Vec3f* scale);
    void endEffect(u32);
    void killEffect(u32, u32, u32);
    void setParent(u32 handleId, nw4r::g3d::ScnMdl*, const char*, bool);
    void setParent(u32 handleId, nw4r::g3d::ScnMdl*, u32, bool);
    void setParent(u32 handleId1, u32 handleId2, const char*, bool);
    void pushsetCurrentGroup(u32 p1);
    // HYPOTHESIS: parameter types from register use at call sites.
    void endEffectGroup(u32, u32);
    void setFlatMode(bool);
    void popCurrentGroup();
    // HYPOTHESIS: parameter types from argument registers and effect call sites.
    void preSet2dScene(bool);
    void pauseEffectGroup(u32, u32);
    void playEffectGroup(u32, u32);
    void preSetAnmIdx(int chrIndex, int visIndex, int patIndex, int srtIndex, int clrIndex, int shpIndex);
    void setGroundAttach(u32, soGroundModule* groundModule);
    void setGroundAttach(u32, grCollisionLineWork* lineWork);
    void setVisible(u32, bool isVisible);
    // MATCH-ONLY: static. The map's name takes (u32, bool, u32, u32), but the body reads this, the flag and two masks
    // (r3..r6); Captain Falcon's Final Smash passes g_ecMgr as the first argument (no other caller in the tree).
    static void setVisibleGroup(u32 self, bool isVisible, u32, u32);
    void setPos(u32, Vec3f* pos);
    void setRot(u32, Vec3f* rot);
    void setScl(u32, Vec3f* scale);
    void setMdlTexpat(u32, u32);
    void setMdlAnmIdx(u32, u32, s8);
    void setMdlEnableUpdate(u32, bool, s8);
    void setMdlFrame(u32, float, s8);
    void setSlowRate(u32 effectHandleID, u32 slowMagnitude);
    u32 searchResourceID(char* resourceName);
    virtual ~ecMgr();

    inline static ecMgr* getInst() __attribute__((never_inline));
    inline static ecMgr* getInstance() { return g_ecMgr; }

};
static_assert(sizeof(ecMgr) == 0xa8, "Class is wrong size!");


inline ecMgr* ecMgr::getInst() { return g_ecMgr; }
