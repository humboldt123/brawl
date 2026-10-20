#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/damage/so_damage_util_actor.h>
#include <types.h>

// HYPOTHESIS: item manage module (soModuleEnumeration::m_itemManageModule, +0xa8); only slots 0x40/0x84 are used
class ftItemManageModuleStub {
public:
    virtual ~ftItemManageModuleStub();
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
    virtual bool unk40(int);
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84(float, float, int);
};

// HYPOTHESIS: view of the collision/damage record passed to checkDamage; damage value at +0xC
struct ftDonkeyDamageRecord {
    u8 unk00[0x0C];
    float damage;
};

class ftDonkeyStatusUniqProcessSuperLiftCommon : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSuperLiftCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};

static inline ftItemManageModuleStub* getItemManage(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftItemManageModuleStub*>(moduleAccesser->m_enumerationStart->m_itemManageModule);
}

void ftDonkeyStatusUniqProcessSuperLiftCommon::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9a:
    case 0x9d:
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
    case 0x11b:
    case 0x11c:
    case 0x11d:
    case 0x11e:
    case 0x11f:
        break;
    default:
        if (getItemManage(moduleAccesser)->unk40(0)) {
            // Target constants verified from ft_donkey.o: lbl_92_rodata_70 = 90.0f; lbl_92_rodata_74 = 0.0f.
            getItemManage(moduleAccesser)->unk84(90.0f, 0.0f, 0);
        }
        break;
    }
}

bool ftDonkeyStatusUniqProcessSuperLiftCommon::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    float damage = soValueAccesser::getConstantFloat(moduleAccesser, 0x7d2, 0) * static_cast<ftDonkeyDamageRecord*>(data)->damage;
    if (soDamageUtilActor::getDamageLevel(moduleAccesser, damage) < 3) {
        return true;
    }
    if (getItemManage(moduleAccesser)->unk40(0)) {
        // Target constants verified from ft_donkey.o: lbl_92_rodata_70 = 90.0f; lbl_92_rodata_74 = 0.0f.
        getItemManage(moduleAccesser)->unk84(90.0f, 0.0f, 0);
    }
    return false;
}

ftDonkeyStatusUniqProcessSuperLiftCommon g_ftDonkeyStatusUniqProcessSuperLiftCommon;
