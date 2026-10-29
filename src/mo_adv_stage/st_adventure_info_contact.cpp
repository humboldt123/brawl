#include <if/if_adv_mngr.h>
#include <if/if_mngr.h>
#include <sora_adv_stage/st_adventure_info_contact.h>
#include <st/loader/st_loader_sand_bag.h>

class IfPlayer;

extern "C" void fn_80107848(IfAdvMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_801077D8(IfAdvMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_800DCEA8(IfMngr* mngr, stAdventureInfoDisp disp);
extern "C" void fn_800E08A0(IfPlayer* player, stLoaderSandBagAppearInfo* info, u8 unk);
extern "C" void fn_800E14A4(IfPlayer* player, u32 value, int a);
extern "C" void fn_800E3684(IfPlayer* player, int state);

void stAdventureInfoContact::sendToBossInfo(stAdventureInfoIconParam param, bool isStamina) {
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
                fn_800E14A4(g_IfMngr->m_ifPlayers[g_IfMngr->unk68[1]], info.m_damage, 0);
            }
            if (param.m_disp.m_index == 1 && !m_isBossAppear[1]) {
                info.m_characterKind = 0x3D;
                info.unkC = 2;
                info.unk1C = 0;
                fn_800E08A0(g_IfMngr->m_ifPlayers[2], &info, g_IfMngr->unkA8);
                m_isBossAppear[1] = true;
            } else if (param.m_disp.m_index == 1 && g_IfMngr->m_ifPlayers[2] != NULL) {
                fn_800E14A4(g_IfMngr->m_ifPlayers[g_IfMngr->unk68[2]], info.m_damage, 0);
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
                fn_800E3684(g_IfMngr->m_ifPlayers[g_IfMngr->unk68[1]], 2);
            }
            if (index == 1 && g_IfMngr->m_ifPlayers[2] != NULL) {
                fn_800E3684(g_IfMngr->m_ifPlayers[g_IfMngr->unk68[2]], 2);
            }
        }
    }
}

void stAdventureInfoContact::sendToZakoInfo(stAdventureInfoIconParam param) {
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
