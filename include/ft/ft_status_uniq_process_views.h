#pragma once

// Local views of module vtables that the shared headers do not declare far enough (for the sora_melee
// status units). Each class is only used through reinterpret_cast of the module pointer. A view with
// P pad slots before a named slot places that slot at vtable index P + 2 (the leading two entries are
// the destructor pair in the module classes), so the pad counts below land on the offsets named in the comments.
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

// Link module (so_link_module_impl.h stops before 0x164). HYPOTHESIS: slot 0x164 is unlinkSlot164(u32).
class ftLinkModuleSlotView {
public:
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    // HYPOTHESIS: slot 0x18 stores a value (a work/task id) for the link index.
    virtual void slot18(int index, int value);
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void slot28(int index);
    virtual u32 slot2c(int index);
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void slot44(int index, void* data);
    virtual void pad16();
    virtual void pad17();
    virtual void pad18();
    virtual void pad19();
    virtual void pad20();
    virtual void pad21();
    virtual void pad22();
    virtual void pad23();
    virtual void pad24();
    virtual void pad25();
    virtual void pad26();
    virtual void pad27();
    virtual void pad28();
    virtual void pad29();
    virtual void pad30();
    virtual void pad31();
    virtual void pad32();
    virtual float slot8c(int index);
    virtual void pad34();
    virtual void pad35();
    virtual Vec3f slot98(int, int, int);
    virtual void pad37();
    virtual void pad38();
    virtual int slotA4(int index);
    virtual float slotA8(int index);
    // HYPOTHESIS: slot 0xAC returns the link offset (a Vec2f) for the link index.
    virtual Vec2f slotAC(int index);
    virtual void pad42();
    virtual void pad43();
    virtual void pad44();
    virtual void pad45();
    virtual void pad46();
    virtual void pad47();
    virtual u32 slotC8(int index);
    virtual int slotCC(int index);
    virtual void pad50();
    virtual void pad51();
    virtual void pad52();
    // HYPOTHESIS: slot 0xDC returns a Vec2f for the link index (the parent's position offset).
    virtual Vec2f slotDC(int index);
    virtual void pad54();
    virtual void pad55();
    virtual void pad56();
    virtual void pad57();
    virtual void pad58();
    virtual void pad59();
    virtual void pad60();
    virtual void pad61();
    virtual void pad62();
    virtual void pad63();
    virtual void pad64();
    virtual void pad65();
    virtual void pad66();
    virtual void pad67();
    virtual void pad68();
    virtual void pad69();
    virtual void pad70();
    virtual void pad71();
    virtual void pad72();
    virtual void pad73();
    virtual void pad74();
    // HYPOTHESIS: slot 0x134 takes the link index, two zero arguments and two flags (used when a link is dissolved).
    virtual void slot134(int index, int a, int b, bool c, bool d);
    virtual void pad76();
    virtual void pad77();
    virtual void pad78();
    virtual void pad79();
    virtual void pad80();
    virtual void pad81();
    virtual void pad82();
    virtual void pad83();
    virtual void pad84();
    virtual void slot15c(Vec3f* first, Vec3f* second);
    virtual void pad86();
    virtual void unlinkSlot164(u32 index);
    // HYPOTHESIS: slot 0x168 reports whether the fighter is linked to a parent (gates the 0x164 unlink).
    virtual bool slot168();
};

// Item manage module. HYPOTHESIS: slot 0xA0 takes a single u32 value.
class ftItemManageSlotView {
public:
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void pad15();
    virtual void pad16();
    virtual void pad17();
    virtual void pad18();
    virtual void pad19();
    virtual void pad20();
    virtual void pad21();
    virtual void pad22();
    virtual void pad23();
    virtual void pad24();
    virtual void pad25();
    virtual void pad26();
    virtual void pad27();
    virtual void pad28();
    virtual void pad29();
    virtual void pad30();
    virtual void pad31();
    virtual void pad32();
    virtual void pad33();
    virtual void pad34();
    virtual void pad35();
    virtual void pad36();
    virtual void pad37();
    virtual void slot40(u32 value);
};

// Capture module, stored as void* in the accesser. Slots 0x30 (u32, u32), 0x34 (s32, returns bool),
// 0x3C and 0x44 (no args), 0x4C (u32) and 0x54 (u32).
class ftCaptureModuleSlotView {
public:
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    // HYPOTHESIS: slot 0x28 returns the pull offset of the captured fighter for the given index, slot 0x2c its progress.
    virtual Vec2f slot28(int index);
    virtual float slot2c();
    virtual void slot30(u32 a, u32 b);
    virtual bool slot34(int value);
    virtual bool slot38(soModuleAccesser* moduleAccesser, void* damageInfo);
    virtual void slot3c();
    // HYPOTHESIS: slot 0x40 reports whether the capture is already active (slot 0x3c starts it).
    virtual bool slot40();
    virtual void slot44();
    virtual void pad102();
    virtual void slot4c(u32 value);
    virtual void pad103();
    virtual void slot54(u32 value);
    virtual void pad104();
    virtual void pad105();
    virtual void pad106();
    // HYPOTHESIS: slot 0x64 returns the capture level of the grab (0 when none).
    virtual int slot64();
};

// Controller module. HYPOTHESIS: the callers of startClatter (slot 0xc8, soControllerModuleImpl::startClatter) pass the fifth
// argument as a signed value (-1 = "no kind"); the declared u8 parameter would fold the literal to 0xff, so the call goes
// through this view. The leading destructor puts the first pad at vtable offset 0xc.
class ftControllerClatterView {
public:
    virtual ~ftControllerClatterView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void pad15();
    virtual void pad16();
    virtual void pad17();
    virtual void pad18();
    virtual void pad19();
    virtual void pad20();
    virtual void pad21();
    virtual void pad22();
    virtual void pad23();
    virtual void pad24();
    virtual void pad25();
    virtual void pad26();
    virtual void pad27();
    virtual void pad28();
    virtual void pad29();
    virtual void pad30();
    virtual void pad31();
    virtual void pad32();
    virtual void pad33();
    virtual void pad34();
    virtual void pad35();
    virtual void pad36();
    virtual void pad37();
    virtual void pad38();
    virtual void pad39();
    virtual void pad40();
    virtual void pad41();
    virtual void pad42();
    virtual void pad43();
    virtual void pad44();
    virtual void pad45();
    virtual void pad46();
    virtual void startClatter(float time, float p2, float p3, u8 p4, s8 p5, u32 index, bool p7);
};
