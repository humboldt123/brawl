#include <ft/ft_manager.h>
#include <ft/ft_status_uniq_process_rebirth.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Base rebirth initStatus (sora_melee), still called by its fn_27 name.
extern "C" void fn_27_1733F8(soStatusUniqProcess* self, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: local view of the posture module; only the vtable slots 0x18 (getPos) and 0x1c (setPos) are used.
class ftPopoPostureModuleView {
public:
    virtual void unk0();
    virtual void unk4();
    virtual void unk8();
    virtual void unkC();
    virtual Vec3f getPos();
    virtual void setPos(Vec3f* pos);
};

static ftPopoPostureModuleView& getPosture(soModuleAccesser* moduleAccesser) {
    return *reinterpret_cast<ftPopoPostureModuleView*>(moduleAccesser->m_enumerationStart->m_postureModule);
}

class ftPopoStatusUniqProcessRebirth : public ftStatusUniqProcessRebirth {
public:
    virtual ~ftPopoStatusUniqProcessRebirth() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
    virtual void updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
};

void ftPopoStatusUniqProcessRebirth::initStatus(soModuleAccesser* moduleAccesser) {
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        Vec3f pos = getPosture(moduleAccesser).getPos();
        if (!moduleAccesser->getWorkManageModule().isFlag(0x1200003e)) {
            pos.m_x += 3.0f;
        } else {
            pos.m_x -= 3.0f;
        }
        getPosture(moduleAccesser).setPos(&pos);
    }
    fn_27_1733F8(this, moduleAccesser);
}

void ftPopoStatusUniqProcessRebirth::attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    Vec3f local = pos;
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        local.m_x -= 3.0f;
    }
    ftStatusUniqProcessRebirth::attachEffect(moduleAccesser, local);
}

void ftPopoStatusUniqProcessRebirth::updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    Vec3f local = pos;
    ftManager* manager = g_ftManager;
    if (manager->getSubFighter(moduleAccesser->getWorkManageModule().getInt(0x10000000)) != 0) {
        local.m_x -= 3.0f;
    }
    ftStatusUniqProcessRebirth::updateEffect(moduleAccesser, local);
}

ftPopoStatusUniqProcessRebirth g_ftPopoStatusUniqProcessRebirth;
