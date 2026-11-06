#pragma once

#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_vc_trial.h>
#include <nw4r/g3d/g3d_resfile.h>

class MuMsg;
class tySealListDataManager;

class IfRewardListTask : public gfTask {
public:
    class Panel {
    public:
        Panel();
        ~Panel();

        muAutoPtr<MuObject> unk0;
        muAutoPtr<MuObject> unk4;
        muAutoPtr<MuObject> unk8;
        muAutoPtr<MuObject> unkC;
        muAutoPtr<MuObject> unk10;
        muG3dObjHolder<nw4r::g3d::ScnMdlExpand> unk14;
    };

    void detachDialogMsg();
    virtual void processDefault();
    void createModel();
    void initMsg();
    void controllProc();
    void updateScroll();

    u32 unk40;
    u32 unk44;
    muMenuController unk48;
    u8 unkF0[0x10];
    u8 unk100 : 1;
    u8 unk100_6 : 1; // tested by IfCoinShooter::update / onUpdate / isQuit (bit 0x40)
    u8 unk100_0 : 6;
    bool unk101;
    u8 unk102[0x46];
    nw4r::g3d::ResFile unk148;
    u8 unk14C[0x11C];
    MuObject* unk268;
    u8 unk26C[8];
    Panel unk274[8];
    MuMsg* unk334;
    MuMsg* unk338;
    MuMsg* unk33C;
    MuMsg* unk340;
    u8 unk344[0x5C];
    tySealListDataManager* unk3A0;
};
