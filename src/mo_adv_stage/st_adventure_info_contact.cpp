// stAdventureInfoContact: the bridge between the Subspace Emissary enemy info classes (boss, zako, corps) and the HUD.
// Every send* forwards one event to the adventure HUD (g_IfAdvMngr) and, for bosses, also to the player HUD (g_IfMngr),
// both of which may be absent. With isStamina set, the boss health goes to player HUD slots 1 and 2 instead,
// created on the first send (m_isBossAppear) with sand-bag style appear info.
// All 12 functions are exact, but the unit does not link: the original's .rodata keeps a 0.5f constant that nothing
// in the unit references, and mwld strips an unreferenced constant from our object (see TIPS).
#include <if/if_adv_mngr.h>
#include <if/if_mngr.h>
#include <sora_adv_stage/st_adventure_info_contact.h>
#include <st/loader/st_loader_sand_bag.h>

class IfPlayer;

extern "C" void fn_80107848(IfAdvMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_801077D8(IfAdvMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_800DCEA8(IfMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_800E08A0(IfPlayer* player, stLoaderSandBagAppearInfo* info, u8 unk);

void stAdventureInfoContact::sendToBossInfo(const stAdventureInfoIconParam& param, bool isStamina) {
    if (g_IfAdvMngr != NULL) {
        fn_80107848(g_IfAdvMngr, param.m_disp);
        g_IfAdvMngr->setBossMoveDispFlag(true);
    }
    IfMngr* ifMngr = g_IfMngr;
    if (ifMngr != NULL) {
        if (!isStamina) {
            fn_800DCEA8(ifMngr, param.m_disp);
            g_IfMngr->setBossMoveDispFlag(true);
        } else {
            stLoaderSandBagAppearInfo info;
            info.m_state = 1;
            bool isLow = false;
            if (param.m_disp.m_hp > 0.0f && param.m_disp.m_hp < 1.0f) {
                isLow = true;
            }
            u32 hp;
            if (isLow) {
                hp = 1;
            } else {
                hp = param.m_disp.m_hp;
            }
            info.m_damage = hp;
            info.m_isStamina = true;
            if (param.m_disp.m_index == 0 && !m_isBossAppear[0]) {
                info.m_characterKind = 0x3C;
                info.unkC = 1;
                info.unk1C = 0;
                fn_800E08A0(g_IfMngr->m_ifPlayers[1], &info, g_IfMngr->unkA8);
                m_isBossAppear[0] = true;
            } else if (param.m_disp.m_index == 0 && ifMngr->m_ifPlayers[1] != NULL) {
                g_IfMngr->m_ifPlayers[g_IfMngr->unk68[1]]->updateDamageHP(info.m_damage, 0);
            }
            if (param.m_disp.m_index == 1 && !m_isBossAppear[1]) {
                info.m_characterKind = 0x3D;
                info.unkC = 2;
                info.unk1C = 0;
                fn_800E08A0(g_IfMngr->m_ifPlayers[2], &info, g_IfMngr->unkA8);
                m_isBossAppear[1] = true;
            } else if (param.m_disp.m_index == 1 && g_IfMngr->m_ifPlayers[2] != NULL) {
                g_IfMngr->m_ifPlayers[g_IfMngr->unk68[2]]->updateDamageHP(info.m_damage, 0);
            }
        }
    }
}

void stAdventureInfoContact::sendToDestroyBossInfo(int index, bool isStamina) {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->setBossDeathDisp(index);
    }
    if (g_IfMngr != NULL) {
        if (!isStamina) {
            g_IfMngr->setBossDeathDisp(index);
        } else {
            if (index == 0 && g_IfMngr->m_ifPlayers[1] != NULL) {
                g_IfMngr->m_ifPlayers[g_IfMngr->unk68[1]]->setDamageState(2);
            }
            if (index == 1 && g_IfMngr->m_ifPlayers[2] != NULL) {
                g_IfMngr->m_ifPlayers[g_IfMngr->unk68[2]]->setDamageState(2);
            }
        }
    }
}

void stAdventureInfoContact::sendToDamageBossInfo(int index, bool isStamina) {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->setBossDamageDisp(index);
    }
    if (g_IfMngr != NULL) {
        if (!isStamina) {
            g_IfMngr->setBossDamageDisp(index);
        } else {
            if (index == 0 && g_IfMngr->m_ifPlayers[1] != NULL) {
                IfPlayer* player = g_IfMngr->m_ifPlayers[g_IfMngr->unk68[1]];
                if (player->unk1C == player->unk18) {
                    player->unk264 = 1.0f;
                    player->setDamageState(1);
                }
            }
            if (index == 1 && g_IfMngr->m_ifPlayers[2] != NULL) {
                IfPlayer* player = g_IfMngr->m_ifPlayers[g_IfMngr->unk68[2]];
                if (player->unk1C == player->unk18) {
                    player->unk264 = 1.0f;
                    player->setDamageState(1);
                }
            }
        }
    }
}

void stAdventureInfoContact::sendToRemoveBossInfo() {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->eraseBoss(0);
        g_IfAdvMngr->eraseBoss(1);
    }
    if (g_IfMngr != NULL) {
        g_IfMngr->eraseBoss(0);
        g_IfMngr->eraseBoss(1);
    }
}

void stAdventureInfoContact::sendToZakoInfo(const stAdventureInfoIconParam& param) {
    if (g_IfAdvMngr != NULL && param.m_isValid) {
        fn_801077D8(g_IfAdvMngr, param.m_disp);
    }
}

void stAdventureInfoContact::sendToDestroyZakoInfo(int index) {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->unk6A[index] = true;
    }
}

void stAdventureInfoContact::sendToDamageZakoInfo(int index) {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->unk68[index] = true;
    }
}

void stAdventureInfoContact::sendToEraseZakoInfo(int index) {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->erasesZako(index);
    }
}

void stAdventureInfoContact::sendToRemoveZakoInfo() {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->erasesZako(0);
        g_IfAdvMngr->erasesZako(1);
    }
}

void stAdventureInfoContact::addZakoCorps(u32 count) {
    if (g_IfAdvMngr != NULL) {
        for (u8 i = 0; i < count; i++) {
            g_IfAdvMngr->addCorps();
        }
    }
}

void stAdventureInfoContact::removeZakoCorps() {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->removeCorps();
    }
}

void stAdventureInfoContact::sendToRemoveCarpsInfo() {
    if (g_IfAdvMngr != NULL) {
        int corps = g_IfAdvMngr->m_corps;
        for (int i = 0; i < corps; i++) {
            g_IfAdvMngr->removeCorps();
        }
    }
}
