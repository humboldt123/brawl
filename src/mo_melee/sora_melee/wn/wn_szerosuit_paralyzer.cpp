#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_kind.h>
#include <ft/ft_entry.h>
#include <memory.h>

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) {}

extern "C" int fn_27_8CC10(StageObject* object); // soExternalValueAccesser::getMotionKind

// Registers the Zero Suit paralyzer article class (kind 98) in the class info table during static initialization.
wnClassInfoImpl<98, wnSZerosuitParalyzer, wnSZerosuitParalyzerData> g_wnSZerosuitParalyzerClassInfo;

void wnSZerosuitParalyzer::activate(int founderTaskId, int resourceId, int team) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
#else
    desc.pos = origin;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = true;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = true;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit6 = 1;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
}

void wnSZerosuitParalyzer::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser,
                                           StageObject* sender, int selector) {
    soLinkModule& link = m_moduleAccesser->getLinkModule();
    if (sender->soGetKind() == StageObject_Fighter && sender->soGetSubKind() == Fighter_SZeroSuit &&
        link.isLink(3)) {
        u32 senderTaskId = sender->m_taskId;
        u32 parentTaskId = link.getParentTaskId(3);
        if (parentTaskId == senderTaskId) {
            switch (event->m_eventKind) {
            case 0x455: {
                int motionKind = -1;
                switch (fn_27_8CC10(sender)) {
                case 0x1d5:
                    motionKind = 6;
                    break;
                case 0x1d6:
                    motionKind = 7;
                    break;
                case 0x1dc:
                    motionKind = 8;
                    break;
                case 0x1dd:
                    motionKind = 9;
                    break;
                }
                m_moduleAccesser->getWorkManageModule().setInt(motionKind, 0x10000005);
                break;
            }
            }
        }
    }
    Weapon::notifyEventLink(event, moduleAccesser, sender, selector);
}

void wnSZerosuitParalyzer::processFixPosition() {
    Weapon::processFixPosition();
    int motionKind = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (motionKind != -1) {
        soMotionChangeParam param(motionKind, m_moduleAccesser->getMotionModule().getFrame(), 1.0f, 0, 0, 1, 0);
        m_moduleAccesser->getMotionModule().changeMotionRequest(&param);
        m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
    }
}
