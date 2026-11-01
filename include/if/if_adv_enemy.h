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

class IfAdvZako {
public:
    MuObject* m_objects[5];                 // 0x00
    nw4r::g3d::ScnMdlExpand* m_scnMdls[3];  // 0x14
    MuMsg* m_msg;                           // 0x20
    int unk24;                              // 0x24 HYPOTHESIS: the first create argument, not read in this unit
    u8 unk28[0x8];                          // 0x28
    int unk30;                              // 0x30
    u8 unk34[0x4];                          // 0x34
    void* m_msgData;                        // 0x38

    // HYPOTHESIS: parameter types from the register use of the callee and of IfAdvMngr.
    static IfAdvZako* create(int arg, void* msgData);
    IfAdvZako(int arg, void* msgData);
    ~IfAdvZako();
    void createModel(nw4r::g3d::ResFile* resFile);
    // HYPOTHESIS: untyped tables as in IfStgedit::createObjResFile.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    // BYTES: the original calls this out of line from createModel.
    void createExpMdl(const void* nodeInfo, s32 nodeCount) __attribute__((never_inline));
    static void createLocator(nw4r::g3d::ResFile* resFile);
    static void destroyLocator();
    static void startScene(nw4r::g3d::ScnGroup* group);
    void main();
    void erasesZako(int index);
    void zakoDamageOrDeath(int kind, int index);
};
static_assert(sizeof(IfAdvZako) == 0x3c, "Class is the wrong size!");
