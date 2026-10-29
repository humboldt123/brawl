#include <ec/ec_mgr.h>
#include <yk/yk_effect_module_impl.h>

// HYPOTHESIS: local name; the effect group bits req adds to the caller's group.
static u32 g_ykEffectGroup = 0x01FFFFFF;

ykEffectModuleImpl::ykEffectModuleImpl() { }

ykEffectModuleImpl::~ykEffectModuleImpl() { }

u32 ykEffectModuleImpl::req(EfID effectID, Vec3f* pos, Vec3f* rot, float scale, u32 group, int anmIdx) {
    g_ecMgr->pushsetCurrentGroup(g_ykEffectGroup | group);
    if (anmIdx >= 0) {
        g_ecMgr->preSetAnmIdx(0, 0, 0, 0, anmIdx, 0);
    }
    Vec3f effectScale(scale, scale, scale);
    u32 id = g_ecMgr->setEffect(effectID, pos, rot, &effectScale);
    g_ecMgr->popCurrentGroup();
    g_ecMgr->preSetAnmIdx(0, 0, 0, 0, 0, 0);
    return id;
}
