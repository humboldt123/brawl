#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <mt/mt_vector.h>
#include <st/se_util.h>
#include <types.h>

class stOperatorReadyGo : public stOperator {
public:
    float unk48;
    float unk4C;
    float unk50;
    u8 unk54_b7 : 1;
    bool unk54_b6 : 1;
    u8 unk54_b5 : 1;
    u8 unk54_b4 : 1;
    u8 : 4;
    u32 unk58;
    u32 unk5C;
    u32 unk60;
    u32 unk64;
    u32 unk68;
    u32 unk6C;
    u32 unk70;
    u32 unk74;
    StSeUtil::SeSeqInstance<2, 5> m_seSeq;

    stOperatorReadyGo(bool flag);
    virtual void processBegin();
    virtual ~stOperatorReadyGo();
    virtual bool isEnd();
    virtual void startOperator();
};
static_assert(sizeof(stOperatorReadyGo) == 0x100, "Class is wrong size!");
