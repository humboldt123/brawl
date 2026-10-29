#include <wn/sheik/wn_sheik_needle.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>

// Registers the Sheik needle article class (kind 68) in the class info table during static initialization.
wnClassInfoImpl<68, wnSheikNeedle, wnSheikNeedleData> g_wnSheikNeedleClassInfo;

void wnSheikNeedle::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

template class wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0>;
