#include <em/arman/wnem_proc_arman.h>
#include <em/bombhead/wnem_proc_bombhead.h>
#include <em/bosspackun/wnem_proc_bosspackun.h>
#include <em/botron/wnem_proc_botron.h>
#include <em/cataguard/wnem_proc_cataguard.h>
#include <em/crazyhand/wnem_proc_crazyhand.h>
#include <em/duon/wnem_proc_duon.h>
#include <em/em_weapon_manager.h>
#include <em/galleom/wnem_proc_galleom.h>
#include <em/gyraan/wnem_proc_gyraan.h>
#include <em/hammerbros/wnem_proc_hammerbros.h>
#include <em/metaridley/wnem_proc_metaridley.h>
#include <em/porky/wnem_proc_porky.h>
#include <em/prim/wnem_proc_prim.h>
#include <em/rayquaza/wnem_proc_rayquaza.h>
#include <em/taboo/wnem_proc_taboo.h>
#include <em/wnem/wnem_proc_common.h>
#include <em/wnem/wnem_proc_robo.h>
#include <revolution/OS/OSError.h>

// EnemyWeaponRegister: installs the proc function of every enemy weapon state in the manager's proc table, under the
// proc number the weapons' setProcFnc / changeProcFnc calls use. The procs are the static functions of the wnem proc
// units (one class per enemy).

// The proc that does nothing but report (proc number 0, the default of a weapon).
class wnemProc {
public:
    static void Dummy();
};

void wnemProc::Dummy() {
    OSReport("call wnemProc::Dummy  \n");
}

void EnemyWeaponRegister() {
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemProc::Dummy, 0);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPrimProc::BoomerangHave, 1);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPrimProc::BoomerangTurn, 3);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPrimProc::BoomerangFly, 2);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCataguardProc::LanceFly, 4);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCataguardProc::LanceFall, 5);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCataguardProc::LanceStick, 6);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCataguardProc::HelmIdol, 7);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCataguardProc::HelmBullet, 8);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBotronProc::IronIdol, 9);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBotronProc::IronTouchGround, 10);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemArmanProc::ArmEnter, 11);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemArmanProc::ArmExit, 12);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemArmanProc::ArmIdol, 13);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemArmanProc::ArmDead, 14);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRoboProc::MissileFly, 25);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRoboProc::MissileBomb, 26);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRoboProc::MissileErase, 27);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBosspackunProc::ArmEnter, 15);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBosspackunProc::ArmIdol, 16);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBosspackunProc::ArmDead, 17);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::BombFly, 18);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::BombExplosion, 19);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::MissileFly, 20);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::MissileExplosion, 21);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::LaserEnter, 22);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::LaserFly, 23);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemDuonProc::LaserDead, 24);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::RoboFall, 40);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::RoboRun, 41);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::RoboDown, 42);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::RoboExplosion, 43);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::SparkLock, 44);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::SparkHorming, 45);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPorkyProc::SparkErase, 46);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBombheadProc::HeadFly, 47);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemBombheadProc::HeadExplosion, 48);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCommonProc::ShellFly, 49);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCommonProc::ShellHitEtc, 53);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCommonProc::ShellErase, 50);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCommonProc::ShellHitGround, 52);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemHammerbrosProc::HammerFly, 28);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRayquazaProc::LightBallFly, 29);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRayquazaProc::LightBallLanding, 30);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemRayquazaProc::LightBallDisappear, 31);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemTabooProc::BodySplitBulletFly, 32);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemTabooProc::BodySplitBulletExplosion, 33);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemTabooProc::CometBulletFly, 34);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemTabooProc::CometBulletExplosion, 35);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemMetaridleyProc::FireballFly, 36);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemMetaridleyProc::FireballExplosion, 37);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemMetaridleyProc::FireballTouchGround, 67);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemMetaridleyProc::NormalBulletFly, 38);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemMetaridleyProc::NormalBulletDisappear, 39);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCrazyhandProc::BombFly, 55);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemCrazyhandProc::BombExplosion, 56);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGalleomProc::GrenadeFly, 57);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGalleomProc::GrenadeExplosion, 58);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGalleomProc::ShotFly, 59);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGalleomProc::ShotExplosion, 60);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemPrimProc::BoomerangErase, 61);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGyraanProc::AxeIdol, 62);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGyraanProc::AxeSwing, 63);
    emWeaponManager::getInstance()->RegisterProcFnc((void*)&wnemGyraanProc::AxeStick, 64);
}
