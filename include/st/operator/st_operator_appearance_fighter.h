#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorAppearanceFighter : public stOperator {
public:
    u8 unk48;
    float unk4C;
    Vec3f unk50;
    Vec3f unk5C;
    float unk68;
    float unk6C;
    char _70[4];
    void* unk74[7];
    void* unk90;
    u8 unk94;
    u8 unk95;
    char _96[0x188 - 0x96];
    float unk188;
    char _18C[4];
    u32 unk190;
    char _194[4];
    float unk198;
    float unk19C;
    char _1A0[4];
    u8 unk1A4;
    float unk1A8;
    u32 unk1AC;
    u32 unk1B0;
    u8 unk1B4;
    bool unk1B5;
    u8 unk1B6;
    void* unk1B8;

    stOperatorAppearanceFighter();
    virtual void processBegin();
    virtual ~stOperatorAppearanceFighter();
    virtual bool isEnd();
    virtual void startAppearance();
};
static_assert(sizeof(stOperatorAppearanceFighter) == 0x1BC, "Class is wrong size!");
