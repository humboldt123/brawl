#pragma once

#include <types.h>

#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>

// Lasso article module views. The lasso article's status processes read some modules through objects whose
// vtable layout differs from the header classes: the status-like module at soModuleEnumeration +0x9C and the
// model-like module at +0x4 have their vptr 8 bytes in, so they derive from a padding struct. The link module
// at +0x54 is read with a vtable that the header does not declare. Slots are named by their byte offset from the
// vptr (the first virtual is at +0x8). HYPOTHESIS: the module types; only slots named from call sites are known.
struct wnLassoViewPad8 {
    u32 m_pad00;
    u32 m_pad04;
};

class wnLassoAltStatusView : public wnLassoViewPad8 {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void unk18Attach(u32 archiveId, int, u32, u32, u32, int, int, int);
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void unk30Int(int);
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void unk60Int(int);
    virtual void unk64Int(int);
    virtual void unk68VecPair(Vec3f* a, Vec3f* b);
    virtual void slot6C();
    virtual int unk70Int();
    virtual float unk74Float(int);
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual int unk88Int();
    virtual void unk8CInt(int);
    virtual void slot90();
    virtual void slot94();
    virtual void slot98();
    virtual void slot9C();
    virtual void slotA0();
    virtual void unkA4Int(int);
    virtual int unkA8Int();
    virtual void slotAC();
    virtual void unkB0VecInt(Vec3f* pos, int, bool);
    virtual void unkB4Int(int);
    virtual void slotB8();
    virtual void unkBCInt(int);
    virtual void slotC0();
    virtual void slotC4();
    virtual int unkC8Int(int);
    virtual void unkCCVec(int, Vec3f* pos);
    virtual void slotD0();
    virtual int unkD4Int();
    virtual void slotD8();
    virtual void unkDCFloat(float);
    virtual int unkE0Int();
};

class wnLassoAltModelView : public wnLassoViewPad8 {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void slot18();
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void slot30();
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void slot60();
    virtual void slot64();
    virtual void slot68();
    virtual void slot6C();
    virtual void unk70FloatInt(float, int);
    virtual void slot74();
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual void slot88();
    virtual void slot8C();
    virtual void slot90();
    virtual void slot94();
    virtual Vec3f unk98VecIntBool(int, bool);
    virtual void slot9C();
    virtual void slotA0();
    virtual void slotA4();
    virtual void slotA8();
    virtual void slotAC();
    virtual void slotB0();
    virtual void slotB4();
    virtual void slotB8();
    virtual void slotBC();
    virtual void slotC0();
    virtual void slotC4();
    virtual void slotC8();
    virtual void slotCC();
    virtual void slotD0();
    virtual void slotD4();
    virtual void slotD8();
    virtual void slotDC();
    virtual void slotE0();
    virtual void unkE4Float(float);
};

class wnLassoLinkView {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void slot18();
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void slot30();
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void unk44SendEvent(int, soLinkEventArgs&);
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void slot60();
    virtual void slot64();
    virtual void slot68();
    virtual void slot6C();
    virtual void slot70();
    virtual void slot74();
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual float unk88Float();
    virtual void slot8C();
    virtual void slot90();
    virtual void slot94();
    virtual Vec3f unk98NodePos(u32 index, u32 nodeId, bool);
    virtual void slot9C();
    virtual void slotA0();
    virtual int unkA4Int(int);
    virtual float unkA8Float(int);
    virtual void slotAC();
    virtual void slotB0();
    virtual void slotB4();
    virtual void slotB8();
    virtual void slotBC();
    virtual void slotC0();
    virtual void slotC4();
    virtual void slotC8();
    virtual void slotCC();
    virtual void slotD0();
    virtual void slotD4();
    virtual void slotD8();
    virtual void slotDC();
    virtual void slotE0();
    virtual void slotE4();
    virtual void slotE8();
    virtual void slotEC();
    virtual void slotF0();
    virtual void slotF4();
    virtual void slotF8();
    virtual void slotFC();
    virtual void slot100();
    virtual void slot104();
    virtual void slot108();
    virtual void slot10C();
    virtual void slot110();
    virtual void slot114();
    virtual void slot118();
    virtual void slot11C();
    virtual void slot120();
    virtual void slot124();
    virtual void slot128();
    virtual void slot12C();
    virtual void slot130();
    virtual void slot134();
    virtual void slot138();
    virtual void slot13C();
    virtual void slot140();
    virtual void slot144();
    virtual void unk148Int(int);
    virtual void slot14C();
    virtual void slot150();
    virtual void slot154();
    virtual void slot158();
    virtual void slot15C();
    virtual void slot160();
    virtual void unk164Int(int);
    virtual void slot168();
    virtual void slot16C();
    virtual void slot170();
    virtual Vec3f unk174VecOut();
    virtual void slot178();
    virtual void slot17C();
    virtual void slot180();
    virtual void slot184();
    virtual float unk188Float();
    virtual void slot18C();
    virtual void slot190();
    virtual void unk194Int(int, int, bool);
};

inline wnLassoAltStatusView* wnLassoGetAltStatus(soModuleAccesser* moduleAccesser) {
    return (wnLassoAltStatusView*)moduleAccesser->m_enumerationStart->m_physicsModule;
}

inline wnLassoAltModelView* wnLassoGetAltModel(soModuleAccesser* moduleAccesser) {
    return (wnLassoAltModelView*)moduleAccesser->m_enumerationStart->m_modelModule;
}

inline wnLassoLinkView* wnLassoGetLink(soModuleAccesser* moduleAccesser) {
    return (wnLassoLinkView*)moduleAccesser->m_enumerationStart->m_linkModule;
}
