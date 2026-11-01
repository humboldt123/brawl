#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ResFile;
class ScnGroup;
class ScnMdlExpand;
} // namespace g3d
} // namespace nw4r
class MuObject;
class MuMsg;

class IfAdvBoss {
public:
    MuObject* m_objects[5];                 // 0x00
    nw4r::g3d::ScnMdlExpand* m_scnMdls[3];  // 0x14
    MuMsg* m_msg;                           // 0x20
    int unk24;                              // 0x24
    void* m_msgData;                        // 0x28
    int unk2C;                              // 0x2C
    // HYPOTHESIS: frame counters of the damage and death animations of the two bosses and of the move display.
    u8 m_damageFrame[2];                    // 0x30
    u8 m_deathFrame[2];                     // 0x32
    u8 m_moveFrame;                         // 0x34
    bool m_isDamageDisp[2];                 // 0x35
    bool m_isDeathDisp[2];                  // 0x37
    bool m_isMoveDisp;                      // 0x39
    int m_damageIndex;                      // 0x3C
    int m_deathIndex;                       // 0x40

    // HYPOTHESIS: parameter types from the register use of the callee and of IfAdvMngr.
    static IfAdvBoss* create(void* msgData);
    IfAdvBoss(void* msgData);
    ~IfAdvBoss();
    void createModel(nw4r::g3d::ResFile* resFile);
    // HYPOTHESIS: untyped tables as in IfStgedit::createObjResFile.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    // BYTES: the original calls this out of line from createModel.
    void createExpMdl(const void* nodeInfo, s32 nodeCount) __attribute__((never_inline));
    static void createLocator(nw4r::g3d::ResFile* resFile);
    static void destroyLocator();
    static void startScene(nw4r::g3d::ScnGroup* group);
};
static_assert(sizeof(IfAdvBoss) == 0x44, "Class is the wrong size!");
