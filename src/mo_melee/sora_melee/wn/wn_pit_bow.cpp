#include <wn/pit/wn_pit_bow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <memory.h>

// Registers the Pit bow article class (kind 90) in the class info table during static initialization.
wnClassInfoImpl<90, wnPitBow, wnPitBowData> g_wnPitBowClassInfo;

void wnPitBow::activate(int founderTaskId, int resourceId, int texResId, bool unk1, bool unk2) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
#else
    desc.pos = pos;
#endif
    desc.lr = 0.0f;
    desc.team = -1;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unkC = texResId;
    desc.unk10 = unk1;
    desc.unk14 = unk2;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x40;
    Weapon::activate(&desc);
}
