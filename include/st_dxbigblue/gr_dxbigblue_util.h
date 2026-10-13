#pragma once

// Helpers shared by the grounds of Big Blue.

#include <gr/gr_calc_world_callback.h>
#include <gm/gm_global.h>
#include <gr/ground.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <mt/mt_vector.h>
#include <types.h>
#include <yk/yakumono.h>

// The grounds make a node callback for the model they follow: it takes the node with m_nodeIndex, and the model keeps it.
#define DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)                                                           \
    if ((scnMdl)->m_calcWorldCallBack == NULL) {                                                                     \
        (calcWorldCallBack)->m_index = 0;                                                                            \
        (calcWorldCallBack)->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;                                       \
        (scnMdl)->m_calcWorldCallBack = (calcWorldCallBack);                                                         \
        (scnMdl)->EnableScnMdlCallbackTiming(1);                                                                     \
        (scnMdl)->m_nodeIndex = (calcWorldCallBack)->m_nodeCallbackDatas[0].m_nodeIndex;                             \
    }

// MATCH-ONLY: the bytes of the match settings the stage reads (their names are unknown): the game mode in the upper six bits
// of byte 8 and the event in byte 0x10.
struct dxbigblueMeleeView {
    u8 _pad0[8];
    unsigned char m_mode : 6;
    unsigned char m_rest : 2;
    u8 _pad1[7];
    u8 m_event;
};

// HYPOTHESIS: mode 7 with event 0x9F is the Boss battle that fights on this stage.

// Ground::isCollisionStatusOwnerTask (sora_melee): the symbol takes an int pointer (the header declares another type)
extern "C" bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(Ground* ground, grCollStatus* collStatus, int* flags);

#include <gr/collision/gr_collision_joint.h>

// MATCH-ONLY: the original flips the same bits of the joint's flag byte as a chain (HYPOTHESIS: they track "enabled").
static inline void dxbigblueJointEnable(grCollisionJoint* joint) {
    joint->m_0x54_4 = true;
    joint->m_0x54_6 = joint->m_0x54_4;
}

static inline void dxbigblueJointDisable(grCollisionJoint* joint) {
    joint->m_0x54_7 = false;
    joint->m_0x54_4 = joint->m_0x54_7;
    joint->m_0x54_6 = joint->m_0x54_4;
}

#include <nw4r/math/math_arithmetic.h>

// The value is brought into the range (the original has it as fsel).
static inline float dxbigblueClamp(float value, float low, float high) {
    float result = nw4r::math::FSelect(value - low, value, low);
    return nw4r::math::FSelect(result - high, high, result);
}
