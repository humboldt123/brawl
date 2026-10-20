#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialSEnd : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialSEnd() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// Module rodata constants lbl_111_rodata_20 (0.0) and lbl_111_rodata_24 (1.0).
static const float kSEndVectorValue = 0.0f;
static const float kSEndScale = 1.0f;

void ftMetaknightStatusUniqProcessSpecialSEnd::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work id 0x20000002 is a remaining-count work slot (0 means the move is finished).
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000002);
    if (count <= 0) {
        ftMetaknightPostureView* posture = &ftMetaknightPosture(moduleAccesser);
        Vec3f unit(kSEndVectorValue, kSEndVectorValue, kSEndVectorValue);
        posture->setPos(&unit, 0);
        return;
    }
    float x = ftMetaknightPosture(moduleAccesser).getPos(0).m_x;
    float step = (kSEndScale * x) / (float)count;
    ftMetaknightPostureView* posture = &ftMetaknightPosture(moduleAccesser);
    Vec3f before = posture->getPos(0);
    Vec3f after = posture->getPos(0);
    Vec3f next(x - step, after.m_y, before.m_z);
    posture->setPos(&next, 0);
}

void ftMetaknightStatusUniqProcessSpecialSEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    ftMetaknightPostureView* posture = &ftMetaknightPosture(moduleAccesser);
    posture->setPos(&Vec3f(kSEndVectorValue, kSEndVectorValue, kSEndVectorValue), 0);
    ftMetaknightPosture(moduleAccesser).unk54();
}

ftMetaknightStatusUniqProcessSpecialSEnd g_ftMetaknightStatusUniqProcessSpecialSEnd;
