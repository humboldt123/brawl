// Brawl physics wrapper translation unit ph_2ndary_world.o (main.dol 0x80082438-0x8008AC5C).
// Not yet decompiled. Functions in address order with their map names:
//   0x80082438   268  __ct   [map: ph2ndaryWorld____ct]
//   0x80082544   456  __dt   [map: ph2ndaryWorld____dt]
//   0x8008270C  2256  operationExecute   [map: ph2ndaryWorld__operationExecute]
//   0x80082FDC   464  sameZelda   [map: ph2ndaryWorld__sameZelda]
//   0x800831AC   484  samePeach   [map: ph2ndaryWorld__samePeach]
//   0x80083390   548  operationAwakeExecute   [map: ph2ndaryWorld__operationAwakeExecute]
//   0x800835B4   808  constraintInertiaChild   [map: ph2ndaryWorld__constraintInertiaChild]
//   0x800838DC  3144  operationGravityWeapon   [map: ph2ndaryWorld__operationGravityWeapon]
//   0x80084524  1460  operationGravityWeaponTrunModelMatrix   [map: ph2ndaryWorld__operationGravityWeaponTrunModelMatrix]
//   0x80084AD8  1776  operationWeapon   [map: ph2ndaryWorld__operationWeapon]
//   0x800851C8  4776  operationGravity   [map: ph2ndaryWorld__operationGravity]
//   0x80086470   204  getSamusRandomAccel   [map: ph2ndaryWorld__getSamusRandomAccel]
//   0x8008653C   212  addConstraintArray   [map: ph2ndaryWorld__addConstraintArray]
//   0x80086610   132  removeConstraintArray   [map: ph2ndaryWorld__removeConstraintArray]
//   0x80086694   184  updateCollision   [map: ph2ndaryWorld__updateCollision]
//   0x8008674C   156  updateCollision2Pos   [map: ph2ndaryWorld__updateCollision2Pos]
//   0x800867E8   728  checkCollisionHit   [map: ph2ndaryWorld__checkCollisionHit]
//   0x80086AC0  2188  chCollisionGround   [map: ph2ndaryWorld__chCollisionGround]
//   0x8008734C   880  correctCollisionSlope   [map: ph2ndaryWorld__correctCollisionSlope]
//   0x800876BC   416  correctCollisionVertical   [map: ph2ndaryWorld__correctCollisionVertical]
//   0x8008785C   416  correctCollisionLevel   [map: ph2ndaryWorld__correctCollisionLevel]
//   0x800879FC  5564  chCollisionGround2   [map: ph2ndaryWorld__chCollisionGround2]
//   0x80088FB8   620  chCollisionRigidBody   [map: ph2ndaryWorld__chCollisionRigidBody]
//   0x80089224   460  getWindArray   [map: ph2ndaryWorld__getWindArray]
//   0x800893F0   452  setNodeWindArray   [map: ph2ndaryWorld__setNodeWindArray]
//   0x800895B4  1784  getNodeWindArray   [map: ph2ndaryWorld__getNodeWindArray]
//   0x80089CAC  1824  operationExecuteSleep   [map: ph2ndaryWorld__operationExecuteSleep]
//   0x8008A3CC   900  operationExecuteSleepCreateLocalSpace   [map: ph2ndaryWorld__operationExecuteSleepCreateLocalSpace]
//   0x8008A750   876  operationExecuteSleepRestructuring   [map: ph2ndaryWorld__operationExecuteSleepRestructuring]
//   0x8008AABC   416  isCollisionBetweenPoints   [map: ph2ndaryWorld__isCollisionBetweenPoints]
#include <mt/mt_prng.h>
#include <ph/ph2ndaryWorld.h>

s32 ph2ndaryWorld::addConstraintArray(void* ptr, const u8* kind) {
    for (int i = 0; i < m_constraints.m_size; i++) {
        if (((ConstraintEntry*)m_constraints.m_data)[i].ptr == ptr) {
            return -1;
        }
    }
    s32 cap = m_constraints.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK;
    u8 k = *kind;
    s32 id = m_nextId;
    if (m_constraints.m_size == cap) {
        hkArrayUtil::_reserveMore(&m_constraints, sizeof(ConstraintEntry));
    }
    ConstraintEntry* entry = &((ConstraintEntry*)m_constraints.m_data)[m_constraints.m_size];
    m_constraints.m_size++;
    entry->kind = k;
    entry->ptr = ptr;
    entry->id = id;
    m_nextId++;
    return id;
}

f32 ph2ndaryWorld::getSamusRandomAccel(f32 a, f32 b, f32 c) {
    f32 r = randf();
    if (r > 0.9f) {
        if (a < 0.0f) {
            return a - m_unk48 * (b * (randf()));
        }
        return a + m_unk48 * (c * (randf()));
    }
    return 0.0f;
}

void ph2ndaryWorld::removeConstraintArray(void* ptr) {
    for (int i = 0; i < m_constraints.m_size; i++) {
        if (((ConstraintEntry*)m_constraints.m_data)[i].ptr == ptr) {
            ((ConstraintEntry*)m_constraints.m_data)[i].ptr = 0;
            m_constraints.m_size--;
            ConstraintEntry* last = &((ConstraintEntry*)m_constraints.m_data)[m_constraints.m_size];
            ConstraintEntry* cur = &((ConstraintEntry*)m_constraints.m_data)[i];
            cur->kind = last->kind;
            cur->ptr = last->ptr;
            cur->id = last->id;
            return;
        }
    }
}
