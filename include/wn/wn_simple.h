#pragma once

#include <wn/wn_simple_builder.h>
#include <ft/ft_entry.h>

struct wnSimpleData;
// HYPOTHESIS: source descriptor spelling. Both constructor consumers read a
// kind/subkind pair through the first pointer and the heap module through the second.
struct wnSimpleKindInfo {
    s32 kind, subKind;
    wnSimpleKindInfo(s32 subKind) : kind(0), subKind(subKind) { }
};
struct wnSimpleConstructionInfo {
    const wnSimpleKindInfo* kindInfo;
    void* heapModule;
    wnSimpleConstructionInfo(const wnSimpleKindInfo& kind, void* heap) :
        kindInfo(&kind), heapModule(heap) { }
};
static_assert(sizeof(wnSimpleConstructionInfo) == 8, "Simple article construction descriptor");

// The shared builder owns its modules; the simple article adds its own
// command table and address pack. Construction remains imported.
class wnSimple : public wnWeaponBuilder<wnSimpleModuleAccesserBuildConfig> {
    soArrayContractibleTable<const acAnimCmdConv*> m_table;
    soAnimCmdAddressPackArraySeparate m_pack;
public:
    wnSimple(s32 articleId, const wnSimpleConstructionInfo& info, wnSimpleData* data, bool unk);
    virtual ~wnSimple();
    // Founder, resources and position feed Weapon::activate. Remaining selector
    // and flag meanings are unresolved; their integer/byte consumers are verified.
    // HYPOTHESIS: source qualifiers and parameter names beyond position.
    void activate(s32 founderTaskId, s32 resourceId, const wnSimpleData* data,
        bool unk0, bool unk1, const Vec3f& position, s32 unk2, s32 unk3,
        s32 unk4, bool unk5, bool unk6, s32 unk7, s32 unk8, bool unk9,
        s32 unk10, s32 unk11, bool unk12);
};
static_assert(sizeof(wnSimple) == 0x16c8, "Simple article layout");
