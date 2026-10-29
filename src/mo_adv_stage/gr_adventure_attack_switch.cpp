#include <sora_adv_stage/gr_adventure_attack_switch.h>

void grAdventureAttackSwitch::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (!(m_coolTime > 0.0f)) {
        if (grYakumono::isEnableAttackPersonAttacked((u32)damage, (u32)attackerInfo, (u32)&m_data->unk70) &&
            damage->m_damageAdd > 0.0f) {
            setOn(true);
        }
    }
}
