#include <types.h>
#include <sr/sr_common.h>
#include <OS/OSError.h>
#include <havok/hkArray.h>
#include <ph/ph_world.h>

class csElementInterface {
public:
    virtual ~csElementInterface() { }
};

class csElement : public csElementInterface {
public:
    u8 m_isActive; // 0x04

    csElement();
    csElement(int group);
    virtual ~csElement();
    virtual void beginOperation() = 0;
    virtual void afterOperation() = 0;
    virtual void debugDraw() = 0;
};

class csWorld : public csElement, public phWorld {
public:
    u32 m_unk10; // 0x10
    u32 m_unk14; // 0x14
    u8 m_unk18;  // 0x18
    u32 m_unk1C; // 0x1C
    u32 m_unk20; // 0x20
    s32 m_unk24; // 0x24
    s32 m_unk28; // 0x28
    u32 m_unk2C; // 0x2C

    csWorld() : csElement(5) {
        m_unk18 = 0;
        m_unk1C = 0;
        m_unk20 = 0;
        m_unk24 = -1;
        m_unk28 = -1;
        m_unk2C = 0;
    }
    virtual ~csWorld();
    virtual void beginOperation();
    virtual void afterOperation();
    virtual void debugDraw();
};

class csModule;

class csCoinshooter {
public:
    void setActiveModule(csModule* module);
    void removeModule(csModule* module);
};

extern csCoinshooter* lbl_20_bss_8;

struct csElementEntry {
    csElement* m_element;
    int m_group;
};

class csModuleInterface {
public:
    virtual ~csModuleInterface() { }
};

class csModule : public csModuleInterface {
public:
    hkArray<csElementEntry> m_elements; // 0x04
    csWorld* m_world;                   // 0x10

    csModule();
    virtual ~csModule();
    virtual void initLoadParam();
    virtual void removeWork();

    csElement* getElement(const int& index);
    bool entryElement(csElement* element, const int& group);
    bool removeElement(csElement* element);
};

csModule::csModule() {
    lbl_20_bss_8->setActiveModule(this);
    m_world = new (Heaps::MenuInstance) csWorld();
}

csModule::~csModule() {
    hkArray<csElement*> list;
    if (m_world != NULL) {
        delete m_world;
        m_world = NULL;
    }
    for (int i = 0; i < m_elements.getSize(); i++) {
        if (m_elements[i].m_element != NULL) {
            list.pushBack(m_elements[i].m_element);
        }
    }
    if (list.getSize() > 0) {
        for (int i = 0; i < list.getSize(); i++) {
            OSReport("[coinshooter]:element is [%x] \n", list[i]);
        }
    }
    list.clearAndDeallocate();
    m_elements.clearAndDeallocate();
    lbl_20_bss_8->removeModule(this);
    OSReport("destruct module \n");
}

csElement* csModule::getElement(const int& index) {
    if (m_elements.getSize() > index) {
        return m_elements[index].m_element;
    }
    return NULL;
}

void csModule::removeWork() {
}

void csModule::initLoadParam() {
}

bool csModule::removeElement(csElement* element) {
    for (int i = 0; i < m_elements.getSize(); i++) {
        if (m_elements[i].m_element == element) {
            m_elements.removeAt(i);
            return true;
        }
    }
    return false;
}
