#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <types.h>

// HYPOTHESIS: generate-article manage module (soModuleEnumeration::m_generateArticleManageModule); only these slots are used
class ftGenerateArticleManageStub {
public:
    virtual ~ftGenerateArticleManageStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48(int, int);
};

// HYPOTHESIS: effect module (soModuleEnumeration::m_effectModule); only this slot is used
class ftEffectModuleStub {
public:
    virtual ~ftEffectModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C(int);
};

class ftDonkeyStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinal() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftDonkeyStatusUniqProcessFinal::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    if (!workManage.isFlag(0x2200001a)) {
        bool hit = false;
        for (int i = 0; i < 4; i++) {
            if (attack.isAttack(i, false)) {
                attack.clear(i);
                hit = true;
            }
        }
        workManage.setFlag(hit, 0x2200001a);
    }
}

void ftDonkeyStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    reinterpret_cast<ftEffectModuleStub*>(moduleAccesser->m_enumerationStart->m_effectModule)->unk5C(8);
    if (nextStatus != 0x133) {
        static_cast<ftGenerateArticleManageStub*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->unk48(0, 0);
    }
}

ftDonkeyStatusUniqProcessFinal g_ftDonkeyStatusUniqProcessFinal;
