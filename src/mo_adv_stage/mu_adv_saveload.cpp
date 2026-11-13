// muAdvSaveLoadTask with muAdvSaveTask and muAdvLoadTask: the save and load menus of the adventure mode. Not written yet.
#include <types.h>

#include <mu/adv/mu_adv_saveload.h>

inline muRecorder::Data::~Data() { reset(0); }

void muAdvSaveLoadTask::update() {
    controllProc();
    updateScroll();
    updatePanel();
}

muAdvSaveLoadTask::SaveBox::SaveBox() {}
muAdvSaveLoadTask::SaveBox::~SaveBox() {}

void muAdvSaveLoadTask::moveScrollCore(int amount) {
    int maximum = unk1BC - 3;
    if (maximum <= 0) return;
    int selection = unk1414 - unk1428;
    unk1428 += amount;
    bool wrap = !unk1442 && unk1418 < 0;
    if (unk1428 < 0) {
        unk1443 = wrap && unk1428 <= amount;
        unk1428 = unk1443 ? maximum : 0;
    } else if (unk1428 > maximum) {
        unk1443 = wrap && unk1428 >= maximum + amount;
        unk1428 = unk1443 ? 0 : maximum;
    }
    unk1414 = unk1428 + selection;
    unk1442 = true;
}

extern const float lbl_40_rodata_B04;
extern const float lbl_40_rodata_B08;

void muAdvSaveLoadTask::initMsg() {
    unk1888.reset(MuMsg::create(1, unk1AC, unk1B0));
    unk1888->allocMsgBuf(0x100, 24);
    void* messages = unk1518.m_handle.getBuffer();
    unk1888->setMsgData(messages);
    u32 message = 0;
    for (u32 box = 0; box < 4; ++box) {
        for (int line = 0; line < 6; ++line, ++message) {
            float size = line <= 3 ? lbl_40_rodata_B04 : lbl_40_rodata_B08;
            unk1888->attachScnMdlSimple(message, unk1580[box].unk0->m_sceneModel, line, size);
            MuMsg::AlignMode align;
            switch (line) {
            case 0: case 1: case 2: case 3: align = MuMsg::Align_Center; break;
            case 4: case 5: align = MuMsg::Align_Left; break;
            }
            unk1888->setAlignMode(message, align);
        }
    }
}

bool muAdvSaveLoadTask::loading(bool advance) {
    if (unk1418 < 0) return true;
    if (unk1B4->unk100 != 0) return false;
    if (unk141C >= 0) return true;
    {
        muRecorder::Data data = unk1B4->getAdvData();
        muRecorder::Data& destination = unk1348[unk1418];
        void* buffer = data.unk0;
        data.unk0 = 0;
        destination.reset(buffer);
    }
    initSlotList(unk1418 + 1);
    if (unk1B4->unk104 != 0) {
        unk141C = unk1418;
        return false;
    }
    if (advance && unk1418 >= 0 && unk141C < 0) {
        if ((u32)unk1418 < 50) {
            ++unk1418;
            unk1B4->loadAdv(unk1418);
        } else {
            unk1418 = -1;
        }
    }
    return true;
}

muRecorder::Data muRecorder::getAdvData() {
    Data data;
    void* buffer = m_advData.unk0;
    m_advData.unk0 = 0;
    data.reset(buffer);
    return data;
}

bool utility::StandardTask::preLoad() { return true; }
bool utility::StandardTask::checkLoadFinish() const { return true; }
