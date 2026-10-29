#include <wn/fox/wn_fox_blaster.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <memory.h>

// Registers the Fox Blaster article class (kind 30) in the class info table during static initialization.
wnClassInfoImpl<30, wnFoxBlaster, wnFoxBlasterData> g_wnFoxBlasterClassInfo;

void wnFoxBlaster::activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = mdlResId;
    desc.unk8 = mdlResId;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
#else
    desc.pos = origin;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = true;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = true;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unkC = texResId;
    desc.unk10 = unk1;
    desc.unk14 = unk2;
    Weapon::activate(&desc);
}

void wnFoxBlaster::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                   int selector) {
    switch (event->m_eventKind) {
    case 0x838:
        return;
    default:
        Weapon::notifyEventLink(event, moduleAccesser, sender, selector);
        return;
    }
}
