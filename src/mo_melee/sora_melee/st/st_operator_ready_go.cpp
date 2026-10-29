#include <snd/snd_id.h>
#include <sr/sr_common.h>
#include <st/operator/st_operator_ready_go.h>
#include <types.h>

static StSeUtil::UnkStruct s_seqCountDown[4] = {
    {(SndID)0x1FE5, 0.0f, 0.0f, 0.0f},
    {(SndID)0x1FE6, 0.0f, 60.0f, 0.0f},
    {(SndID)0x1FE7, 0.0f, 120.0f, 0.0f},
    {(SndID)0x1FDB, 0.0f, 180.0f, 0.0f},
};
static StSeUtil::UnkStruct s_seqReadyGo[2] = {
    {(SndID)0x1FDA, 0.0f, 0.0f, 0.0f},
    {(SndID)0x1FDB, 0.0f, 60.0f, 0.0f},
};
static SndID s_seIds[5] = {(SndID)0x1FE5, (SndID)0x1FE6, (SndID)0x1FE7, (SndID)0x1FDB, (SndID)0x1FDA};

stOperatorReadyGo::stOperatorReadyGo(bool flag) :
    stOperator("OperatorGo"), unk48(0.0f), unk54_b7(0), unk54_b6(flag), unk54_b5(0), unk54_b4(0), unk74(0) {
    unk58 = 0;
    unk5C = 0;
    unk60 = 0;
    unk64 = 0;
    unk68 = 0;
    unk6C = 0;
    unk70 = 0;
    m_state = 0;
    unk4C = flag ? 182.0f : 62.0f;
    unk50 = unk4C;
    m_seSeq.registId(s_seIds, 5);
    if (flag) {
        m_seSeq.registSeq(0, s_seqCountDown, 4, Heaps::System);
    } else {
        m_seSeq.registSeq(0, s_seqReadyGo, 2, Heaps::System);
    }
}

stOperatorReadyGo::~stOperatorReadyGo() { }

bool stOperatorReadyGo::isEnd() {
    return m_state == 5;
}

void stOperatorReadyGo::startOperator() {
    m_state = 1;
}
