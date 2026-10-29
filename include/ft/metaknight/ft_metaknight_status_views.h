#pragma once

// Local views of the game's controller and posture classes for the Metaknight special status units. The BrawlHeaders
// copies place these virtuals at other vtable slots, so the views keep the slot prefix and the entries the statuses
// call. Vtable offset = 8 + 4 * (index below); the 8 covers the two destructor slots.
#include <so/so_module_accesser.h>
#include <so/link/so_link_event_presenter.h>
#include <mt/mt_vector.h>
#include <types.h>

class ftMetaknightControllerView {
public:
    virtual void slot00();
    virtual void slot01();
    virtual void slot02();
    virtual void slot03();
    virtual void slot04();
    virtual void slot05();
    virtual void slot06();
    virtual void slot07();
    virtual void slot08();
    virtual void slot09();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual float getStickX();   // 0x50
};

class ftMetaknightPostureView {
public:
    virtual void slot00();
    virtual void slot01();
    virtual void slot02();
    virtual void slot03();
    virtual void slot04();
    virtual void slot05();
    virtual void slot06();
    virtual void slot07();
    virtual void slot08();
    virtual void slot09();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual Vec3f getPos(int);   // 0x40
    virtual void setPos(Vec3f* pos, int);   // 0x44
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void unk54();   // 0x54, no arguments; name unknown
};

static inline ftMetaknightControllerView& ftMetaknightController(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftMetaknightControllerView&>(moduleAccesser->getControllerModule());
}
static inline ftMetaknightPostureView& ftMetaknightPosture(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftMetaknightPostureView&>(moduleAccesser->getPostureModule());
}

// Final-smash helpers shared by the Metaknight final statuses. Defined in ft_metaknight_status_uniq_process_final_hit_wait.cpp.
struct ftMetaknightFinalLinkEvent : public soLinkEventArgs {
    ftMetaknightFinalLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

class ftMetaknightStatusUniqProcessFinalUtil {
public:
    static void notifyMetaknightFinalEnd(soModuleAccesser* moduleAccesser);
    static void unLinkFinalTargetAll(soModuleAccesser* moduleAccesser);
    static void correctCameraRange(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: parameter types; the union of two camera ranges.
    static Rect2D sumRect(const Rect2D* a, const Rect2D* b);
};
