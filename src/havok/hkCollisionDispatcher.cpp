// Havok translation unit hkCollisionDispatcher.o (main.dol 0x802CB630-0x802CD000).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802CB630    16  create   [map: hkNullAgent3__create]
//   0x802CB640     4  destroy   [map: hkNullAgent3__destroy]
//   0x802CB644     8  process   [map: hkNullAgent3__process]
//   0x802CB64C   708  resetCreationFunctions   [map: hkCollisionDispatcher__resetCreationFunctions]
//   0x802CB910  1424  __ct   [map: hkCollisionDispatcher____ct]
//   0x802CBF2C    12  setEnableChecks   [map: hkCollisionDispatcher__setEnableChecks]
//   0x802CBF38   164  disableDebugging   [map: hkCollisionDispatcher__disableDebugging]
//   0x802CBFDC   272  __dt   [map: hkCollisionDispatcher____dt]
//   0x802CC0EC   296  registerCollisionAgent   [map: hkCollisionDispatcher__registerCollisionAgent]
//   0x802CC214   728  registerAgent3   [map: hkCollisionDispatcher__registerAgent3]
//   0x802CC4EC   576  internalRegisterCollisionAgent   [map: hkCollisionDispatcher__internalRegisterCollisionAgent]
//   0x802CC72C   148  updateHasAlternateType   [map: hkCollisionDispatcher__updateHasAlternateType]
//   0x802CC7C0   284  registerAlternateShapeType   [map: hkCollisionDispatcher__registerAlternateShapeType]
//   0x802CC8DC   288  registerContactMgrFactoryWithAll   [map: hkCollisionDispatcher__registerContactMgrFactoryWithAll]
//   0x802CC9FC   288  calcStatistics   [map: hkCollisionDispatcher__calcStatistics]
//   0x802CCB1C  1252  initCollisionQualityInfo   [map: hkCollisionDispatcher__initCollisionQualityInfo]

#include <havok/hkCollisionDispatcher.h>

// Null agent: a registration target that never produces contacts (the class is only used in this unit).
struct hkNullAgent3 {
    static void* create(void* unk0, u8* flag, void* agent);
    static void destroy(void* unk0);
    static void* process(void* unk0, void* unk1, void* agent);
};

// HYPOTHESIS: the first parameter is unused. Clears the flag byte and returns the agent.
void* hkNullAgent3::create(void* unk0, u8* flag, void* agent) {
    *flag = 0;
    return agent;
}

void hkNullAgent3::destroy(void* unk0) {}

void* hkNullAgent3::process(void* unk0, void* unk1, void* agent) {
    return agent;
}

void hkCollisionDispatcher::setEnableChecks(const hkBool& enable) {
    m_enableChecks = enable.m_bool;
}

#pragma push
#pragma auto_inline off
void hkCollisionDispatcher::disableDebugging() {
    if (m_buffers[0] != 0) {
        hkMemory::getInstance().deallocate(m_buffers[0]);
        hkMemory::getInstance().deallocate(m_buffers[1]);
        hkMemory::getInstance().deallocate(m_buffers[2]);
        hkMemory::getInstance().deallocate(m_buffers[3]);
        m_buffers[0] = 0;
        m_buffers[1] = 0;
        m_buffers[2] = 0;
        m_buffers[3] = 0;
    }
}
#pragma pop

// Releases the debug buffers and the contact manager factories; the shape inheritance array frees itself.
hkCollisionDispatcher::~hkCollisionDispatcher() {
    disableDebugging();
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (m_contactMgrFactory[i][j] != 0) {
                m_contactMgrFactory[i][j]->removeReference();
            }
        }
    }
}

void hkCollisionDispatcher::registerCollisionAgent(const hkAgentFuncs* funcs, int typeA, int typeB) {
    m_agentFuncs[m_numAgents] = *funcs;
    internalRegisterCollisionAgent(m_lookupE94, 1, typeA, typeB, typeA, typeB, m_buffers[2], 0);
    internalRegisterCollisionAgent(m_lookup190, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[0], 0);
    if (funcs->symmetricB != 0) {
        internalRegisterCollisionAgent(m_lookup1294, 1, typeA, typeB, typeA, typeB, m_buffers[3], 0);
        internalRegisterCollisionAgent(m_lookup590, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[1], 0);
    }
    m_numAgents++;
}

#pragma push
#pragma auto_inline off
void hkCollisionDispatcher::updateHasAlternateType(hkShapeType primaryType, hkShapeType alternateType, int depth) {
    m_hasAlternateType[primaryType] |= m_hasAlternateType[alternateType];
    for (int i = 0; i < m_shapeInheritance.getSize(); i++) {
        ShapeInheritance& si = m_shapeInheritance[i];
        if (si.m_alternateType == primaryType) {
            updateHasAlternateType(si.m_primaryType, si.m_alternateType, depth + 1);
        }
    }
}
#pragma pop

void hkCollisionDispatcher::registerAlternateShapeType(hkShapeType primaryType, hkShapeType alternateType) {
    for (int i = 0; i < m_shapeInheritance.getSize(); i++) {
        const ShapeInheritance& si = m_shapeInheritance[i];
        if (si.m_primaryType == primaryType && si.m_alternateType == alternateType) {
            m_shapeInheritance.removeAtAndCopy(i);
            i--;
        }
    }
    updateHasAlternateType(primaryType, alternateType, 0);
    ShapeInheritance& si = m_shapeInheritance.expandOne();
    si.m_primaryType = primaryType;
    si.m_alternateType = alternateType;
}

// fn_802CC8DC: body from https://github.com/jj-link/brawl src/havok/fn_802CC8DC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CC8DC {
class Class_80487298 {
public:
    virtual void fn_802CAD00(int);
    unsigned short m04;
    short m06;
};

// TODO: cleanup (shape from jj-link/brawl fn_802CC8DC)
extern "C" void fn_802CC8DC(char* self, Class_80487298* value, int index) {
    char* column = self + index * 4;
    char* row = self + (index << 5);
    int i = 0;
    do {
        if (value->m04 != 0) {
            value->m06++;
        }
        Class_80487298* old = *(Class_80487298**)(column + 0xC);
        if (old->m04 != 0) {
            if (--old->m06 == 0 && old != 0) {
                old->fn_802CAD00(1);
            }
        }
        *(Class_80487298**)(column + 0xC) = value;
        if (value->m04 != 0) {
            value->m06++;
        }
        old = *(Class_80487298**)(row + 0xC);
        if (old->m04 != 0) {
            if (--old->m06 == 0 && old != 0) {
                old->fn_802CAD00(1);
            }
        }
        i++;
        *(Class_80487298**)(row + 0xC) = value;
        column += 0x20;
        row += 4;
    } while (i < 8);
}
} // namespace jj_fn_802CC8DC

// fn_802CC9FC: body from https://github.com/jj-link/brawl src/havok/fn_802CC9FC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CC9FC {
class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1(int a, int b, void* c);
    virtual void slot2(char* name, int size, void* data, unsigned int count, unsigned int flags);
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
};

extern "C" char lbl_80410050[];

// TODO: cleanup (shape from jj-link/brawl fn_802CC9FC)
extern "C" void fn_802CC9FC(void* arg0, Object_vptr_0* obj) {
    obj->slot1(0, 2, arg0);
    void* data = *(void**)((char*)arg0 + 0x1C24);
    if (data != 0) {
        obj->slot2(lbl_80410050 + 0x147, 4, data, 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C28), 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C2C), 0xC00, 0);
        obj->slot2(lbl_80410050 + 0x147, 4, *(void**)((char*)arg0 + 0x1C30), 0xC00, 0);
    }
    obj->slot6();
}
} // namespace jj_fn_802CC9FC
