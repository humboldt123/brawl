#pragma once

#include <havok/hkArray.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintData.h>
#include <havok/hkMemory.h>

// Element of the chain array (32 bytes; the dtor deallocates capacity * 0x20 bytes).
struct hkBallSocketChainElement {
    u8 unk00[0x20];
};

// Ball-and-socket chain constraint data. The array at 0x18 is the chain: its m_size (0x1C) is the chain
// length read by getConstraintInfo/getRuntimeInfo, its capacity word sits at 0x20.
// HYPOTHESIS: the bridge atom at 0x0C is the same bridge layout as hkGenericConstraintData.
struct hkBallSocketChainData : hkConstraintData {
    hkBridgeConstraintAtom m_bridgeAtom;      // 0x0C
    hkArray<hkBallSocketChainElement> m_chain; // 0x18
    u8 m_unk24[0x1C];                         // 0x24 (not yet identified)

    HK_DECLARE_REF_ALLOCATOR(0x2A)

    hkBallSocketChainData();
    hkBallSocketChainData(hkFinishLoadedObjectFlag flag) __attribute__((never_inline));
    virtual ~hkBallSocketChainData();

    static void finishLoadedObjecthkBallSocketChainData(void* p);
    static const void* getVtablehkBallSocketChainData();

    u32 getType() const;
    void getConstraintInfo(hkConstraintInfo* info) const;
    void getRuntimeInfo(const hkBool* hasChain, hkConstraintRuntimeInfo* out) const;
    virtual void buildJacobian();
};
