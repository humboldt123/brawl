// Havok translation unit hkContinuousSimulation.o (main.dol 0x80325FDC-0x8032BBE8).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80325FDC   156  __ct   [map: hkContinuousSimulation____ct]
//   0x80326078    32  __dl   [map: hkReferencedObject____dl]
//   0x80326124   208  __dt   [map: hkContinuousSimulation____dt]
//   0x803261F4   736  handleAllToisTill   [map: hkContinuousSimulation__handleAllToisTill]
//   0x803264D4   600  advanceTime   [map: hkContinuousSimulation__advanceTime]
//   0x8032672C   500  reintegrateAndRecollideEntities   [map: hkContinuousSimulation__reintegrateAndRecollideEntities]
//   0x80326920   120  resetCollisionInformationForEntities   [map: hkContinuousSimulation__resetCollisionInformationForEntities]
//   0x80326998     4  assertThereIsNoCollisionInformationForEntities   [map: hkContinuousSimulation__assertThereIsNoCollisionInformationForEntities]
//   0x8032699C   196  removeCollisionInformationForAgent   [map: hkContinuousSimulation__removeCollisionInformationForAgent]
//   0x80326A60     4  assertThereIsNoCollisionInformationForAgent   [map: hkContinuousSimulation__assertThereIsNoCollisionInformationForAgent]
//   0x80326A64    60  warpTime   [map: hkContinuousSimulation__warpTime]
//   0x80326AA0  1100  collideInternal   [map: hkContinuousSimulation__collideInternal]
//   0x80326EEC  2432  collideEntitiesBroadPhaseContinuous   [map: hkContinuousSimulation__collideEntitiesBroadPhaseContinuous]
//   0x8032786C   752  collideIslandNarrowPhaseContinuous   [map: hkContinuousSimulation__collideIslandNarrowPhaseContinuous]
//   0x80327B5C    68  collideEntitiesNarrowPhaseContinuous   [map: hkContinuousSimulation__collideEntitiesNarrowPhaseContinuous]
//   0x80327BA0   316  processAgentCollideContinuous   [map: hkContinuousSimulation__processAgentCollideContinuous]
//   0x80327CDC   272  addToiEvent   [map: hkContinuousSimulation__addToiEvent]
//   0x80327DEC   500  removeToiEventsOfEntities   [map: hkContinuousSimulation__removeToiEventsOfEntities]
//   0x80327FE0   324  removeToiEventsOfEntity   [map: hkContinuousSimulation__removeToiEventsOfEntity]
//   0x80328124  2900  simulateToi   [map: hkContinuousSimulation__simulateToi]
//   0x80328E50  1228  hkLs_doSimpleCollisionResponse   [map: hkWorld__hkLs_doSimpleCollisionResponse]
//   0x8032931C   660  hkLs_updateEntityManifolds   [map: hkEntity__hkLs_updateEntityManifolds]
//   0x803295B0   108  hkLs_areVelocitiesOk   [map: hkConstraintSchemaInfo__hkLs_areVelocitiesOk]
//   0x8032961C  1076  hkLs_toiCheckValidityOfConstraints   [map: hkConstraintSolverResources__hkLs_toiCheckValidityOfConstraints]
//   0x80329A50   976  hkLs_toiActivateEntitiesAndCheckConstraints   [map: hkProcessCollisionInput__hkLs_toiActivateEntitiesAndCheckConstraints]
//   0x80329E20   296  hkLs_toiActivateConstraintsLinkingToFixedAndKeyframedEnti   [map: hkArray_22hkConstraintSchemaInfo___hkLs_toiActivateConstraintsLinkingToFixedAndKeyframedEntities]
//   0x80329F48   192  hkLs_toiActivateConstraintsLinkingActivatedEntities   [map: hkArray_22hkConstraintSchemaInfo___hkLs_toiActivateConstraintsLinkingActivatedEntities]
//   0x8032A008   228  hkLs_toiResetVelocityAccumulatorsForEntities   [map: hkConstraintSolverResources__hkLs_toiResetVelocityAccumulatorsForEntities]
//   0x8032A0EC   320  hkLs_backstepAndFreezeEntireIsland   [map: hkSimulationIsland__hkLs_backstepAndFreezeEntireIsland]
//   0x8032A22C   320  hkLs_restoreTransformOnBodiesWithUpdatedTransform   [map: hkSimulationIsland__hkLs_restoreTransformOnBodiesWithUpdatedTransform]
//   0x8032A36C   392  hkLs_toiCheckFinalValidityOfCriticalConstraints   [map: hkConstraintSolverResources__hkLs_toiCheckFinalValidityOfCriticalConstraints]
//   0x8032A4F4  4468  hkLs_localizedSolveToi   [map: hkToiResources__hkLs_localizedSolveToi]
//   0x8032B8DC   672  insertAt   [map: hkArray_P8hkEntity___insertAt]
//   0x8032BB7C    96  insertAt   [map: hkArray_P8hkEntity___insertAt1]
//   0x8032BBDC    12  __sinit_\hkContinuousSimulation_cpp   [map: hkContinuousSimulationcpp____sinit_]

#include <havok/hkContinuousSimulation.h>
#include <havok/hkDefaultToiResourceMgr.h>
#include <havok/hkWorld.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCollisionInput.h>

hkContinuousSimulation::hkContinuousSimulation(hkWorld* world) : hkSimulation(world) {
    m_unk0C = 1;
    hkDefaultToiResourceMgr* mgr = new hkDefaultToiResourceMgr();
    m_toiResourceMgr = mgr;
    m_unk38 = 0;
}

hkContinuousSimulation::~hkContinuousSimulation() {
    delete m_toiResourceMgr;
}

// Handles the pending time-of-impact events in time order, up to minTime: each step takes the earliest event out
// of the list, moves the time to it and simulates it with the world locked. Stops with result 3 when the memory
// system ran out during an event.
// HYPOTHESIS: the meaning of result 3 (the step failed inside the TOI pass); m_unk38 counts the handled events.
int hkContinuousSimulation::handleAllToisTill(hkReal minTime) {
    while (true) {
        hkReal earliestTime = minTime;
        int earliest = -1;
        for (int i = 0; i < m_toiEvents.getSize(); i++) {
            if (m_toiEvents[i].unk00 < earliestTime) {
                earliest = i;
                earliestTime = m_toiEvents[i].unk00;
            }
        }
        if (earliest < 0) {
            return 0;
        }
        hkToiEvent event = m_toiEvents[earliest];
        m_toiEvents.removeAt(earliest);
        m_unk10 = event.unk00;
        hkStepInfo stepInfo(m_unk10, m_unk14);
        m_world->m_dynamicsStepInfo.m_stepInfo = stepInfo;
        m_world->m_collisionInput->m_stepInfo = stepInfo;
        m_world->m_lockCount++;
        simulateToi(m_world, event, m_unk18);
        m_world->unlockAndAttemptToExecutePendingOperations();
        m_unk38++;
        if (hkMemory::getInstance().unk4 == 1) {
            return 3;
        }
    }
}

// Advances the time like hkSimulation::advanceTime, after re-colliding a failed step and handling the pending
// time-of-impact events up to the time to advance to (both inside the "Simulate" timer). A failure in either
// returns its result; a failed TOI pass is also kept as the step result.
int hkContinuousSimulation::advanceTime() {
    {
        HK_TIME_CODE_BLOCK("Simulate");
        if (m_unk24 != 0) {
            int result = reCollideAfterStepFailure();
            if (result != 0) {
                return result;
            }
        }
        hkReal timeToAdvanceTo = snapSimulateTimeAndGetTimeToAdvanceTo();
        if (m_toiEvents.getSize() != 0) {
            int result = handleAllToisTill(timeToAdvanceTo);
            if (result != 0) {
                m_unk24 = result;
                return result;
            }
        }
        m_unk10 = timeToAdvanceTo;
    }
    if (m_unk10 >= m_unk1C) {
        if (m_world->m_postSimulationListeners.m_size != 0) {
            HK_TIMER_BEGIN("PostSimCb");
            hkWorldCallbackUtil::firePostSimulationCallback(m_world);
            HK_TIMER_END();
        }
    }
    m_unk24 = 0;
    return 0;
}

// Debug-only consistency checks: empty in this build.
void hkContinuousSimulation::assertThereIsNoCollisionInformationForEntities(const void* entities) {}

void hkContinuousSimulation::assertThereIsNoCollisionInformationForAgent(const void* agent) {}

// Adds dt to the time of every pending TOI event of the continuous simulation owned by the world.
void hkContinuousSimulation::warpTime(hkReal dt) {
    hkContinuousSimulation* sim = (hkContinuousSimulation*)m_world->m_simulation;
    for (int i = 0; i < sim->m_toiEvents.m_size; i++) {
        sim->m_toiEvents.begin()[i].unk00 += dt;
    }
}

// Removes every TOI event whose key (0x18) matches the agent key (0x10 of the agent). Walks backwards and
// replaces a removed event with the last one. HYPOTHESIS: agent key is the word at 0x10.
void hkContinuousSimulation::removeCollisionInformationForAgent(const void* agent) {
    for (int i = m_toiEvents.m_size - 1; i >= 0; i--) {
        hkToiEvent* events = m_toiEvents.begin();
        if (events[i].unk18 == ((const u32*)agent)[4]) {
            m_toiEvents.m_size--;
            events[i] = events[m_toiEvents.m_size];
        }
    }
}

// Unnamed static flag (lbl_805A0F30); initialised through hkBool(bool) so the TU gets a __sinit.
// HYPOTHESIS: a debug/config switch of the unit; nothing in the written code reads it yet.
static hkBool s_hkContinuousSimulation_unk(false);

// Shape from https://github.com/jj-link/brawl src/havok/fn_8032BB7C.cpp.
// hkArray<hkEntity*> wrapping a single caller-owned entity pointer (map: insertAt1); the dtor frees only owned storage.
extern "C" void fn_8032B8DC(int, int, hkArray<hkEntity*>*);
// TODO: cleanup (shape from jj-link/brawl fn_8032BB7C)
extern "C" void fn_8032BB7C(int arg0, int arg1, hkEntity** entity) {
    hkArray<hkEntity*> entities(entity, 1, 1);
    fn_8032B8DC(arg0, arg1, &entities);
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_803295B0.cpp.
// hkLs_areVelocitiesOk (map name, owner hkConstraintSchemaInfo): index of the largest |value| * scale above the
// limit m_8 * dispatcher table entry, or -1.
// TODO: cleanup (shape from jj-link/brawl fn_803295B0)
extern "C" int fn_803295B0(const char* self, const float* values, int count, char* const* data, float scale) {
    int result = -1;
    float max = *(const float*)(self + 8) *
                *(const float*)(*data + *(const signed char*)(*(const char* const*)self + 0x1c) * 0x3c + 0x1a58);
    for (int i = 0; i < count; i++) {
        float value = scale * (float)__fabs(*values);
        if (value > max) {
            max = value;
            result = i;
        }
        values++;
    }
    return result;
}

// fn_80329E20: body from https://github.com/jj-link/brawl src/havok/fn_80329E20.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_80329E20 {
struct Flag { s8 value; Flag(bool b) : value(b) {} operator s8() const { return value; } };
struct Object {
    char pad[0xA8]; u8 type;
    Flag is7() const { return type == 7; }
    Flag is6() const { return type == 6; }
    bool valid() const { return is7() || is6(); }
};
struct Pair { char pad[4]; Object* a; Object* b; bool valid() const { return a->valid() || b->valid(); } };
struct Item { char pad[0x28]; Pair* pair; };
struct Entry { Item* item; int x; float y; };
struct Array { Entry* entries; int count; };
// TODO: cleanup (shape from jj-link/brawl fn_80329E20)
extern "C" void fn_80329E20(Array* self, int* out, int n) {
    Entry* e;
    for (int i = self->count - n; i < self->count; i++) {
        e = &self->entries[i];
        if (e->item->pair->valid()) {
            int index = (*out)++;
            Entry* dst = &self->entries[index];
            Entry tmp = *dst;
            *dst = *e;
            *e = tmp;
        }
    }
}
} // namespace jj_fn_80329E20

// fn_8032A36C: body from https://github.com/jj-link/brawl src/havok/fn_8032A36C.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_8032A36C {
extern "C" int fn_802A04EC(int, int, int, int, int, void*);
extern "C" int fn_803295B0(void*, void*, int, int, float);
struct Hdr { unsigned char pad[6]; unsigned short h6; };
class Object_vptr_0 {
public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual int slot3();
 unsigned char pad[0x28];
 Hdr* hdr;
};
struct Obj { int a0,a4,a8; Object_vptr_0* sub; unsigned char pad[0xC]; char type; };
struct Entry { Obj* obj; int b,c; };
struct List { Entry* data; int count; };
struct Arr { Entry* data; int count,cap; };
struct Inner { unsigned char pad[8]; float f8; };
struct Ctx { Inner* inner; int a4,a8,aC,a10,a14,a18,a1C; };
struct Result { unsigned char value; Result(int v) : value(v) {} };
// TODO: cleanup (shape from jj-link/brawl fn_8032A36C)
extern "C" Result fn_8032A36C(Ctx* arg0, List* arg1, int arg2, Arr* arg3) {
 char sp8[0x400];
 Entry* out;
 Entry* e;
 int off;
 int i;
 Result ret(1);
 int r31;
 i=0; off=0;
 for (; i<arg1->count; off+=12,i++) {
  e=(Entry*)((char*)arg1->data+off);
  if(e->obj->type==4) {
   r31=fn_803295B0(e,sp8,fn_802A04EC(arg0->a4,e->b,arg0->aC,arg0->a1C,0x100,sp8),arg2,arg0->inner->f8);
   if(r31>=0) {
    if(!arg3) return Result(0);
    ret.value=0;
    if(arg3->count==(arg3->cap&0x3FFFFFFF)) hkArrayUtil::_reserveMore((hkArrayBase*)(arg3),12);
    int n=arg3->count;
    out=arg3->data+n;
    arg3->count=n+1;
    out->obj=e->obj;
    if(e->obj->sub->slot3()==11) {
     Object_vptr_0* s=e->obj->sub;
     out->b=(((int)s->hdr+0x37)&~0xF)+(r31<<5);
     out->c=((((int)s->hdr+0x37)&~0xF)+(s->hdr->h6<<5))+r31*20;
    } else { out->b=0; out->c=0; }
   }
  }
 }
 return ret;
}
} // namespace jj_fn_8032A36C

// fn_8032A22C: body from https://github.com/jj-link/brawl src/havok/fn_8032A22C.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
#include <havok/hkRotation.h>
namespace jj_fn_8032A22C {
#pragma fp_contract on
extern "C" float lbl_805A4800;
struct Vec4 { float x,y,z,w; } __attribute__((aligned(16)));
struct Data { Vec4 a,b,c,d,e,f,g,h,i; };
struct Entry { char pad[0xb0]; Data data; };
struct Container { char pad[0x38]; Entry** entries; int count; };
struct Flags { unsigned char* values; };
// TODO: cleanup (shape from jj-link/brawl fn_8032A22C)
extern "C" void fn_8032A22C(Container* self, Flags* flags) {
    float zero = lbl_805A4800;
    int offset;
    int i = 0;
    offset = 0;
    for (; i < self->count; ++i, offset += 4) {
        if (flags->values[i] != 1 && flags->values[i] != 2) continue;
        {
            Data* p = &(*(Entry**)((char*)self->entries + offset))->data;
            ((hkRotation*)(p))->set(*(const hkQuaternion*)(&p->h));
            Vec4 v;
            float nz = p->i.z, ny = p->i.y, nx = p->i.x;
            v.x = nx * p->a.x + ny * p->b.x + nz * p->c.x;
            v.y = nx * p->a.y + ny * p->b.y + nz * p->c.y;
            v.z = nx * p->a.z + ny * p->b.z + nz * p->c.z;
            v.w = zero;
            p->d.x = p->f.x - v.x;
            p->d.y = p->f.y - v.y;
            p->d.z = p->f.z - v.z;
            p->d.w = p->f.w - v.w;
        }
    }
}
} // namespace jj_fn_8032A22C

// TODO: cleanup (shape from jj-link/brawl fn_80326920)
// fn_80326920: body from https://github.com/jj-link/brawl src/havok/fn_80326920.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkSimulation.h>
namespace jj_fn_80326920 {
extern "C" {
void fn_80327DEC(s32, s32, s32);
void fn_80327FE0(s32, s32);

void fn_80326920(s32 arg0, s32 *arg1, s32 arg2, unsigned char *arg3, const unsigned char *arg4) {
    s8 flag = *arg4;
    ((hkSimulation*)(arg0))->hkSimulation::resetCollisionInformationForEntities((hkEntity**)((s32)arg1), (int)(arg2), (hkWorld*)(arg3), *(hkBool*)(&flag));
    if (arg2 != 1) {
        fn_80327DEC(arg0, (s32)arg1, arg2);
    } else {
        fn_80327FE0(arg0, *arg1);
    }
}
}
} // namespace jj_fn_80326920

// TODO: cleanup (shape from jj-link/brawl fn_80327B5C)
// fn_80327B5C: body from https://github.com/jj-link/brawl src/havok/fn_80327B5C.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
#include <havok/hkSimulation.h>
namespace jj_fn_80327B5C {
typedef struct {
    int a;
    int b;
    int c;
} Tmp;

extern "C" Tmp lbl_80488CE8;

extern "C" void fn_80327B5C(int p1, int p2, int p3, int p4) {
    ((hkSimulation*)(p1))->processAgentsOfEntities((hkEntity**)(p2), (int)(p3), *(hkProcessCollisionInput*)(p4), *(hkSimulation::AgentEntryProcessFunction*)(&lbl_80488CE8), 0);
}
} // namespace jj_fn_80327B5C
