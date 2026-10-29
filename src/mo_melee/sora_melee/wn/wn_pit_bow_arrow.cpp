#include <wn/pit/wn_pit_bow_arrow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <memory.h>

// Registers the Pit bow arrow article class (kind 91) in the class info table during static initialization.
wnClassInfoImpl<91, wnPitBowArrow, wnPitBowArrowData> g_wnPitBowArrowClassInfo;

void wnPitBowArrow::activate(int founderTaskId, int resourceId, int team) {
    int life = m_param->unk24;
    Vec3f pos(0.0f, 0.0f, 0.0f);
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
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
#else
    desc.pos = pos;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(4), true);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getGroundModule().setOffsetY(m_param->unk20, 0);
}

void wnPitBowArrow::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

bool wnPitBowArrow::reflect() {
    float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    angle += 180.0f;
    m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000002);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(4), false);
    return Weapon::reflect();
}

template class wnExtendParamAccesser<3999, 8, 23999, 2, 0, 0>;
