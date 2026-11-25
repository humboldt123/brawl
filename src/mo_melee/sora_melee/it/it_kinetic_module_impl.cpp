#define SO_INSTANCE_UNIT_LOCAL_TEMPLATE
#define SO_INSTANCE_UNIT_EXTERNAL_FULL_PROPERTY_CTOR
#define SO_INSTANCE_UNIT_EXTERNAL_GET_ATTRIBUTE
#define SO_INSTANCE_UNIT_PTR_DTOR
#define SO_ARRAY_LIST_EXTERNAL_INDEX_MEMBERS
#define SO_ARRAY_LIST_UNIQ_HELPERS
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_OPERATOR_MUL_FRESH_REGS
#define IT_KINETIC_MODULE_IMPL_FULL

#include <it/item.h>
#include <it/kinetic/it_kinetic_energy.h>
#include <it/kinetic/it_kinetic_module_impl.h>
#include <new>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <types.h>

// HYPOTHESIS: the item status data keeps the kinetic type of the status in bits 11-18 of its word at +0xC.
struct itStatusData : public soStatusData {
    char unk0[0xC];
    int unkC : 11;
    int m_kineticType : 8;
    int unkC_19 : 13;
};

void itKineticModuleImpl::setSlopeType(itParam::SlopeType slopeType) {
    m_slopeType = slopeType;
}

void itKineticModuleImpl::changeKinetic(int kineticType, soModuleAccesser* moduleAccesser) {
    int prevType = m_kineticType;
    m_kineticType = kineticType;
    if (kineticType == prevType) {
        return;
    }
    switch (kineticType) {
        case 0:
            initializeNormal(moduleAccesser);
            break;
        case 1:
            initializeHave(moduleAccesser);
            break;
        case 2:
            initializeRolling(moduleAccesser);
            break;
        case 3:
            initializeNormal(moduleAccesser);
            m_energy3.disable();
            m_energy7.disable();
            m_energy6.disable();
            break;
        case -1:
            m_jostleEnergy.disable();
            break;
    }
}

void itKineticModuleImpl::clearRotation() {
    m_rotEnergy.clearRotSpeed();
}

void itKineticModuleImpl::clearSpeed(bool clearGravity, soModuleAccesser* moduleAccesser) {
    m_moveEnergy.clearSpeed();
    if (clearGravity && m_gravityEnergy.isEnable()) {
        m_gravityEnergy.clearSpeed();
    }
}

void itKineticModuleImpl::setThrowParam(Vec3f* speed, Vec3f* rot, soModuleAccesser* moduleAccesser, bool unk) {
    m_gravityEnergy.enable();
    clearSpeed(unk, moduleAccesser);
    addSpeed(speed, unk, moduleAccesser);
    BaseItem* item = static_cast<BaseItem*>(&moduleAccesser->getStageObject());
    if (m_kineticFlag.m_mask & 0x200) {
        u32 createOwner = item->m_createOwnerTaskId;
        if (createOwner == static_cast<u32>(item->getTeamOwnerTaskId())) {
            Vec3f halfRot = *rot * 0.5f;
            addRotation(&halfRot, moduleAccesser);
            goto afterRot;
        }
    }
    addRotation(rot, moduleAccesser);
afterRot:
    if (speed->m_y >= -0.0f) {
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
        moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        if (m_kineticFlag.m_mask & 0x100) {
            m_unk306 = 3;
        }
    }
}

void itKineticModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                  soModuleAccesser* moduleAccesser) {
    changeKinetic(static_cast<itStatusData*>(statusData)->m_kineticType, moduleAccesser);
}

void itKineticModuleImpl::initializeHave(soModuleAccesser* moduleAccesser) {
    m_moveEnergy.disable();
    m_gravityEnergy.disable();
    m_rotEnergy.clearRotSpeed();
    m_rotEnergy.disable();
    m_energy3.clearSpeed();
    m_energy3.disable();
    m_aiMoveEnergy.clearSpeed();
    m_aiMoveEnergy.disable();
    m_aiRotEnergy.clearRotSpeed();
    m_aiRotEnergy.disable();
    m_energy6.clearSpeed();
    m_energy7.clearSpeed();
    m_energy7.disable();
    m_groundMovementEnergy.clearSpeed();
    m_groundMovementEnergy.disable();
    m_windEnergy.clearSpeed();
    m_windEnergy.disable();
}

bool itKineticModuleImpl::isObserv(char group) {
    bool result = false;
    if (soKineticModuleImpl::isObserv(group) || group == 'f') {
        result = true;
    }
    return result;
}

Vec2f itKineticModuleImpl::getSumSpeed(soKineticEnergy::AttributeFlag attr) {
    Vec2f sum(0.0f, 0.0f);
    if (m_kineticType == -1) {
        return sum;
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_EXTERN) {
        if (m_jostleEnergy.isEnable()) {
            sum += m_jostleEnergy.getSpeed();
        }
        if (m_windEnergy.isEnable()) {
            sum += m_windEnergy.getSpeed();
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_GROUND) {
        if (m_groundMovementEnergy.isEnable()) {
            sum += m_groundMovementEnergy.getSpeed();
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_MAIN) {
        if (m_moveEnergy.isEnable()) {
            sum += m_moveEnergy.getSpeed();
        }
        if (m_gravityEnergy.isEnable()) {
            sum += m_gravityEnergy.getSpeed();
        }
        if (m_energy3.isEnable()) {
            sum += m_energy3.getSpeed();
        }
        if (m_aiMoveEnergy.isEnable()) {
            sum += m_aiMoveEnergy.getSpeed();
        }
        if (m_energy6.isEnable()) {
            sum += m_energy6.getSpeed();
        }
        if (m_energy7.isEnable()) {
            sum += m_energy7.getSpeed();
        }
    }
    return sum;
}

Vec3f itKineticModuleImpl::getSumSpeed3f(soKineticEnergy::AttributeFlag attr) {
    Vec3f sum(0.0f, 0.0f, 0.0f);
    if (m_kineticType == -1) {
        return sum;
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_EXTERN) {
        if (m_jostleEnergy.isEnable()) {
            Vec3f s = m_jostleEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_windEnergy.isEnable()) {
            Vec3f s = m_windEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_GROUND) {
        if (m_groundMovementEnergy.isEnable()) {
            Vec3f s = m_groundMovementEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_MAIN) {
        if (m_moveEnergy.isEnable()) {
            Vec3f s = m_moveEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_gravityEnergy.isEnable()) {
            Vec3f s = m_gravityEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_energy3.isEnable()) {
            Vec3f s = m_energy3.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_aiMoveEnergy.isEnable()) {
            Vec3f s = m_aiMoveEnergy.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_energy6.isEnable()) {
            Vec3f s = m_energy6.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
        if (m_energy7.isEnable()) {
            Vec3f s = m_energy7.getSpeed3f();
            Vec3fAdd(&sum, &sum, &s);
        }
    }
    return sum;
}

void itKineticModuleImpl::mulSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag attr) {
    if (m_kineticType == -1) {
        return;
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_EXTERN) {
        if (m_jostleEnergy.isEnable()) {
            m_jostleEnergy.mulSpeed(speed);
        }
        if (m_windEnergy.isEnable()) {
            m_windEnergy.mulSpeed(speed);
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_GROUND) {
        if (m_groundMovementEnergy.isEnable()) {
            m_groundMovementEnergy.mulSpeed(speed);
        }
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_MAIN) {
        if (m_moveEnergy.isEnable()) {
            m_moveEnergy.mulSpeed(speed);
        }
        if (m_gravityEnergy.isEnable()) {
            m_gravityEnergy.mulSpeed(speed);
        }
        if (m_energy3.isEnable()) {
            m_energy3.mulSpeed(speed);
        }
        if (m_aiMoveEnergy.isEnable()) {
            m_aiMoveEnergy.mulSpeed(speed);
        }
        if (m_energy6.isEnable()) {
            m_energy6.mulSpeed(speed);
        }
        if (m_energy7.isEnable()) {
            m_energy7.mulSpeed(speed);
        }
    }
}

Vec3f itKineticModuleImpl::getSumRotation(soKineticEnergy::AttributeFlag attr) {
    Vec3f sum(0.0f, 0.0f, 0.0f);
    if (m_kineticType == -1) {
        return sum;
    }
    if (attr.m_mask & soKineticEnergy::ATTRIBUTE_MASK_MAIN) {
        if (m_rotEnergy.isEnable()) {
            Vec3f rot = m_rotEnergy.getRotation();
            Vec3fAdd(&sum, &sum, &rot);
        }
        if (m_aiRotEnergy.isEnable()) {
            Vec3f rot = m_aiRotEnergy.getRotation();
            Vec3fAdd(&sum, &sum, &rot);
        }
    }
    return sum;
}

s32 soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::add(soKineticEnergy*& elm, s32 id, soAttributeFlag attr, s16 prio) {
    s32 prevPos = size() - 1;
    if (prio >= 0) {
        prevPos = findInsertPos(prio);
    }
    s32 idx = m_arrayList.insertSubPublic(prevPos, id);
    if (idx >= 0) {
        m_arrayList.atArrayIndex(idx) = soInstanceUnitFullProperty<soKineticEnergy*>(elm, id, attr, prio);
    }
    return idx;
}

void soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::erase(s32 id) {
    m_arrayList.eraseSubPublic(id);
}

void soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::clear() {
    m_arrayList.clear();
}

soKineticEnergy*& soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::at(s32 id) {
    isContain(id);
    return m_arrayList.atArrayIndex(id).m_element;
}

soKineticEnergy*& soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::atIndex(s32 idx) {
    return m_arrayList.at(idx).m_element;
}

u32 soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::size() const {
    return m_arrayList.size();
}

bool soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::isContain(s32 id) const {
    return m_arrayList.isInUse(id);
}

soAttributeFlag soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::getAttribute(s32 id) const {
    if (!isContain(id)) {
        return soAttributeFlag(0);
    }
    return m_arrayList.atArrayIndex(id).getAttribute();
}

void soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::getPriorityArray(soArray<soKineticEnergy**>& arr) {
    s32 idx = m_arrayList.getTopIndex();
    while (idx >= 0) {
        if (idx >= 0)
            static_cast<void>(m_arrayList.capacity());
        arr.push(&m_arrayList.getUnits()[idx].m_element.m_element);
        if (idx >= 0)
            static_cast<void>(m_arrayList.capacity());
        idx = m_arrayList.getUnits()[idx].m_next;
    }
}

s32 soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::getId(s32 idx) {
    return m_arrayList.at(idx).m_id;
}

u32 soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::capacity() {
    return m_arrayList.capacity();
}

soInstanceUnitFullProperty<soKineticEnergy*>& soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::atUnitIndexFast(s32 idx) {
    return m_arrayList.at(idx);
}

s32 soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11>::getIndex(s32 id) const {
    return id;
}

// TODO: clean up: these calls only stand in for the module's use of its energy members (the constructor and the
// energy users are not decompiled yet); they make the unit put out the energies' inline functions.
#pragma dont_inline on
void itKineticModuleImpl_useEnergies(void* p, itKineticEnergy3f* e3f, itKineticEnergyGravity* eg, itKineticEnergy* e,
                                     const Vec2f& v2, const Vec3f& v3) {
    new (p) itKineticRotEnergy3f();
    e3f->itKineticEnergy3f::getSpeed3f();
    e3f->itKineticEnergy3f::clearSpeed();
    e3f->itKineticEnergy3f::getSpeed();
    e3f->itKineticEnergy3f::onConsiderGroundFriction();
    e3f->itKineticEnergy3f::offConsiderGroundFriction();
    eg->itKineticEnergyGravity::clearSpeed();
    eg->itKineticEnergyGravity::getSpeed();
    eg->itKineticEnergyGravity::resetEnergy(0, v2, v3, NULL);
    e->itKineticEnergy::getSpeed();
    e->itKineticEnergy::resetEnergy(0, v2, v3, NULL);
    e->itKineticEnergy::clearSpeed();
    delete e3f;
    delete eg;
    delete e;
    delete (itKineticEnergyInterface*)p;
    delete (itKineticEnergyStop*)p;
    delete (soArrayList<soInstanceUnitFullProperty<soKineticEnergy*>, 11>*)p;
}
#pragma dont_inline reset
