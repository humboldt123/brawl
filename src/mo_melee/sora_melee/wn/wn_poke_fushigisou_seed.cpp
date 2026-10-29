#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

// Registers the Pokemon seed article class (kind 74) in the class info table during static initialization.
wnClassInfoImpl<74, wnPokeFushigisouSeed, wnPokeFushigisouSeedData> g_wnPokeFushigisouSeedClassInfo;

void wnPokeFushigisouSeed::onDeactivate() { }

// A hit on a target bursts the seed (status 2).
bool wnPokeFushigisouSeed::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
