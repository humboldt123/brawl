#define MT_VEC3F_ASSIGN_NOINLINE
#include <ft/robot/ft_robot_article_pools.h>
#include <ft/robot/ft_robot_link_event.h>
#include <wn/wn_activate_desc.h>
#include <mt/mt_prng.h>
#include <memory.h>

// Shared transition tables remain imported from the original data owner.
// HYPOTHESIS: table names and source grouping; the receiver indexes three
// pairs by current state, or one pair when the configured volley count is two.
extern const s32 lbl_120_rodata_A0[3][2];
extern const s32 lbl_120_rodata_B8[2];

void wnRobotGyro::activate(s32 founderTaskId, u32 resourceId, s32 team,
                           const Vec3f& position, float lr, float power) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    // HYPOTHESIS: this work float carries the founder's Gyro charge/power.
    m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnRobotGyro::onDeactivate() { }

// Keep the visual article's root synchronized with the weapon posture.
void wnRobotGyro::updateNodeSRT() {
    float size = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = size;
    scale.m_y = size;
    scale.m_x = size;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

void wnRobotGyro::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                 StageObject* parent, int index) {
    switch (event->m_eventKind) {
    case 0x839:
        if (acc->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        }
        break;
    case 0x83a:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}

void wnRobotGyroHolder::activate(s32 founderTaskId, u32 resourceId, s32 team,
                                 float lr, const Vec3f& position, SituationKind situation) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (situation == Situation_Air) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    // HYPOTHESIS: flag 8 selects the holder's model constraint policy.
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
}

void wnRobotGyroHolder::onDeactivate() { }

void wnRobotGyroHolder::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                       StageObject* parent, int index) {
    switch (event->m_eventKind) {
    case 0x839:
        if (acc->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        }
        break;
    case 0x83b:
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        break;
    case 0x83c:
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000002);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    case 0x83d:
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
        break;
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}

void wnRobotFinalBeam::activate(s32 founderTaskId, u32 resourceId, s32 team,
                                const Vec3f* position, float lr, s32 count, s32 selection) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    // This visual beam uses the default resource selection rather than the
    // caller's resource ID, unlike the Gyro and its holder.
    desc.resourceId = 0xffff;
    desc.unk8 = 0xffff;
    desc.unkC = 0xffff;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: activation copies position with integer moves.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(selection, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setInt(count, 0x20000000);
    m_moduleAccesser->getPostureModule().setSyncConstraintNode(0);
}

void wnRobotFinalBeam::processUpdate() {
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        // Forward the article's pending notification to its linked founder.
        // HYPOTHESIS: meaning of event 0x456; kind and direction are verified.
        ftRobotGyroLinkEvent event(0x456);
        m_moduleAccesser->getLinkModule().sendEventParents(1, event);
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
    Weapon::processUpdate();
}

void wnRobotFinalBeam::processFixPosition() {
    m_moduleAccesser->getModelModule().getNodeGlobalPosition(0, false);
    Weapon::processFixPosition();
}

// The founder requests new Diffusion Beam volleys through these link events.
// State 3 is terminal: later requests still reach the base link handler.
void wnRobotFinalBeam::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                      StageObject* parent, int index) {
    if (acc->getStatusModule().getStatusKind() != 3) {
        switch (event->m_eventKind) {
        case 0x838:
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, acc);
            break;
        case 0x839: {
            int current = acc->getStatusModule().getStatusKind();
            int next = 0;
            if (acc->getWorkManageModule().getInt(0x20000000) > 2) {
                s32 transitions[3][2];
                __memcpy(transitions, lbl_120_rodata_A0, sizeof(transitions));
                s32 random = randi(2);
                next = transitions[current][random];
            } else if (acc->getWorkManageModule().getInt(0x20000000) > 1) {
                s32 transitions[2];
                __memcpy(transitions, lbl_120_rodata_B8, sizeof(transitions));
                next = transitions[randi(2)];
            }
            static_cast<ftRobotFinalLinkEvent*>(event)->result = next;
            acc->getStatusModule().changeStatusRequest(next, acc);
            acc->getPostureModule().setSyncConstraintNode(0);
            break;
        }
        }
    }
    Weapon::notifyEventLink(event, acc, parent, index);
}
