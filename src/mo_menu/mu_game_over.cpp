#include <gf/gf_application.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_game_over.h>
#include <mu/mu_menu_controller.h>

// MuMsg::setFontWidthModeFixed(msg, width): the shared header lacks the argument
// MuMsg::setFontWidthModeFixed(msg, width) and MuMsg::printData(msg, data, size): the shared header lacks the arguments
extern "C" void setFontWidthModeFixed__5MuMsgFUlf(MuMsg* msg, u32 msgIndex, float width);

// One model of the screen: the name, the slots of the task it fills (start, end), and the draw priority.
struct muGameOverModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The status of the pad as the words that the original copies (MATCH-ONLY: a struct copy of gfPadStatus uses float moves).
struct muGameOverPadCopy {
    u32 m_words[16];
};

// The buttons of the controller of the task (the merged status of the pads) at +0xC of the controller.
#define GAME_OVER_CONTROLLER_BUTTONS(task) (*reinterpret_cast<u32*>((task)->m_controller + 0xC))

static inline int clampInt(int value, int low, int high) {
    int result = low;
    if (low < value) {
        result = value;
    }
    int result2 = high;
    if (result < high) {
        result2 = result;
    }
    return result2;
}

muGameOverTask* muGameOverTask::create(const muGameOverParam* param, gfArchive* archive) {
    muGameOverTask* task = new (Heaps::MenuInstance) muGameOverTask();
    task->m_score = param->m_score;
    task->m_scoreAfter = param->m_scoreAfter;
    task->m_coins = param->m_coins;
    task->m_cost = param->m_cost;
    task->m_noContinue = param->m_noContinue;
    task->m_unk6C = param->m_unk14;
    task->m_coins /= 10;
    task->m_cost /= 10;
    fn_800B96E8(reinterpret_cast<muMenuController*>(task->m_controller), 0xF0);
    task->m_step = Step_Loading;
    task->m_archive = archive;
    return task;
}

muGameOverTask::muGameOverTask() : gfTask("GameOver", Category_Menu, 0xF, 8, true) {
    m_step = Step_Appear;
    m_stepPrev = Step_Count;
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_msg = NULL;
    m_msgData = NULL;
    m_score = 0;
    m_scoreAfter = 0;
    m_coins = 0;
    m_cost = 0;
    m_noContinue = 0;
    m_unk6C = -1;
    fn_800B96E8(reinterpret_cast<muMenuController*>(m_controller), 0xFF);
    m_coinsAfter = 0;
    m_coinStep = 0;
    m_scoreStep = 0;
    m_isContinue = 0;
    m_endWait = 0;
    m_archive = NULL;
    unk2C_b1 = false;
    m_object = NULL;
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    memset(&m_score, 0, 0x18);
}

muGameOverTask::~muGameOverTask() {
    if (m_msg != NULL) {
        delete m_msg;
    }
    if (m_object != NULL) {
        delete m_object;
    }
}

// The init and main function of each step.
const muGameOverTask::StepFunc sStepFuncs[muGameOverTask::Step_Count] = {
    {muGameOverTask::mainStepLoadingInit, muGameOverTask::mainStepLoadingMain},
    {muGameOverTask::mainStepAppearInit, muGameOverTask::mainStepAppearMain},
    {muGameOverTask::mainStepSelectContinueInit, muGameOverTask::mainStepSelectContinueMain},
    {muGameOverTask::mainStepContinueInit, muGameOverTask::mainStepContinueMain},
    {muGameOverTask::mainStepGameOverInit, muGameOverTask::mainStepGameOverMain},
    {muGameOverTask::mainStepGameOverStartInit, muGameOverTask::mainStepGameOverStartMain},
    {muGameOverTask::mainStepGameOverButtonInit, muGameOverTask::mainStepGameOverButtonMain},
};

static const muGameOverModel sModels[] = {
    {"ItrSimpleOver0000", 0, 0, 0x10},
};

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muGameOverTask::createObjResFile(const muGameOverModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        const muGameOverModel* model = &models[i];
        int num = 1;
        if (model->m_start < model->m_end) {
            num = model->m_end - model->m_start;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, model->m_name, model->m_priority, NULL, Heaps::MenuInstance);
            (&m_object)[model->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, model->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

// The four texts: the points, the coins, the cost and the extra line.
void muGameOverTask::initMsg(void* msgData) {
    MuObject* object;
    MuMsg* msg = MuMsg::create(3, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg = msg;
    msg->setMsgData(msgData);
    msg->allocMsgBuf(0x100, 4);
    object = m_object;
    msg->attachScnMdlSimple(0, object->m_sceneModel, 0, 0.0625f);
    msg->setFontType(0, 1);
    msg->setFontColor(0, 0xFF, 0x76, 0x76, 0xFF);
    msg->setAlignMode(0, MuMsg::Align_Right);
    msg->getHScale(0, 1.5f);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 0, 30.0f);
    msg->attachScnMdlSimple(1, object->m_sceneModel, 1, 0.0625f);
    msg->setFontType(1, 1);
    msg->setFontColor(1, 0x9D, 0x9E, 0xA7, 0xFF);
    msg->setAlignMode(1, MuMsg::Align_Right);
    msg->getHScale(1, 1.5f);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 1, 30.0f);
    msg->attachScnMdlSimple(2, object->m_sceneModel, 2, 0.0625f);
    msg->setFontType(2, 1);
    msg->setFontColor(2, 0xFF, 0x76, 0x76, 0xFF);
    msg->setAlignMode(2, MuMsg::Align_Right);
    msg->getHScale(2, 1.5f);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 2, 30.0f);
    msg->attachScnMdlSimple(3, object->m_sceneModel, 3, 0.0625f);
    msg->setFontColor(3, 0xFF, 0xFF, 0xFF, 0xFF);
    msg->setEdgeEnable(3, true);
    msg->setEdge(3, 0, 0, 0, 3.0f);
}

// Starts the animation of an action (node, visibility and color animation; the last one plays from its end).
void muGameOverTask::setGameOverObjAction(int action) {
    // The animation (suffix of the model animation names) of each action.
    static const u8 sActionAnims[] = {0, 1, 2, 3, 4, 4, 5};
    char animName[0x1C];
    sprintf(animName, "ItrSimpleOver0000__%d", sActionAnims[action]);
    float rate;
    float frame;
    if (action == 5) {
        rate = 0.0f;
        frame = 101.0f;
    } else {
        rate = 1.0f;
        frame = 0.0f;
    }
    MuObject* object = m_object;
    object->changeNodeAnimN(animName);
    object->setFrameNode(frame);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(rate);
    object->changeVisAnimN(animName);
    object->setFrameVisible(frame);
    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(rate);
    object->changeClrAnimN(animName);
    object->setFrameMatCol(frame);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(rate);
}

void muGameOverTask::processDefault() {
    fn_800B9984(reinterpret_cast<muMenuController*>(m_controller));
    if (m_stepPrev != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_stepPrev = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
}

void muGameOverTask::mainStepLoadingInit(muGameOverTask* task) {
    muMenu::loadMenuSound();
}

// Waits for the menu sound, loads the files, shows the texts and the model and decides the next step.
int muGameOverTask::mainStepLoadingMain(muGameOverTask* task) {
    if (!muMenu::isLoadFinishMenuSound()) {
        return task->m_step;
    }
    nw4r::g3d::ResFile resFile(task->m_archive->getData(0, static_cast<ARCNodeType>(1), 0x14, 0xFFFE));
    nw4r::g3d::ResFile::Init(&resFile);
    task->m_resFile = resFile;
    task->m_msgData = task->m_archive->getData(0, static_cast<ARCNodeType>(1), 0x6E, 0xFFFE);
    task->createObjResFile(sModels, 1, &task->m_resFile);
    task->initMsg(task->m_msgData);
    g_gfApplication->unkD0.endKeepScreen();
    nw4r::g3d::ScnMdlSimple* scnMdl = task->m_object->m_sceneModel;
    scGameover* scene = dynamic_cast<scGameover*>(gfSceneManager::getInstance()->m_currentScene);
    nw4r::g3d::ScnGroup* root = scene->getScnRoot();
    root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(scnMdl));
    task->m_msg->printf(0, "%d", clampInt(task->m_score, -99999999, 99999999));
    task->m_msg->printf(1, "%d", clampInt(task->m_coins, -9999, 9999));
    task->m_msg->printf(2, "%d", clampInt(-task->m_cost, -999, 999));
    if (task->m_coins < task->m_cost || task->m_noContinue != 0) {
        return Step_GameOverStart;
    }
    return Step_Appear;
}

void muGameOverTask::mainStepAppearInit(muGameOverTask* task) {
    task->setGameOverObjAction(0);
    g_sndSystem->playBGM(0x2863, 0, true);
}

int muGameOverTask::mainStepAppearMain(muGameOverTask* task) {
    if (!task->m_object->isNodeAnimFinished()) {
        return task->m_step;
    }
    return Step_SelectContinue;
}

void muGameOverTask::mainStepSelectContinueInit(muGameOverTask* task) {
    task->setGameOverObjAction(1);
    task->m_isContinue = 1;
    g_sndSystem->playSEwithFixEffect(static_cast<SndID>(0x1FFF), 1.0f, 1.0f);
}

// The player picks "Continue" or "Give up" with the directions and decides with A, B or Start.
int muGameOverTask::mainStepSelectContinueMain(muGameOverTask* task) {
    u32 buttons = GAME_OVER_CONTROLLER_BUTTONS(task);
    if ((buttons & 2) != 0 && task->m_isContinue != 0) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        task->setGameOverObjAction(2);
        task->m_isContinue = 0;
    }
    if ((buttons & 1) != 0 && task->m_isContinue == 0) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        task->setGameOverObjAction(1);
        task->m_isContinue = 1;
    }
    muGameOverPadCopy padCopy;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    padCopy = *reinterpret_cast<muGameOverPadCopy*>(&pad);
    int next;
    if ((padCopy.m_words[3] & 0x1100) != 0) {
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        next = Step_GameOver;
        if (task->m_isContinue != 0) {
            next = Step_Continue;
        }
    } else {
        next = task->m_step;
    }
    return next;
}

// Computes the values the numbers count to and how fast they count.
void muGameOverTask::mainStepContinueInit(muGameOverTask* task) {
    task->setGameOverObjAction(3);
    task->m_coinsAfter = task->m_coins - task->m_cost;
    int coinDiff = task->m_coinsAfter - task->m_coins;
    task->m_coinStep = coinDiff / 0x78;
    // MATCH-ONLY: the original tests m_coinStep twice before the sign check (two bne to the same join); one test leaves out an instruction.
    if (task->m_coinStep == 0 && task->m_coinStep == 0) {
        if (coinDiff > 0) {
            task->m_coinStep = 1;
        } else {
            task->m_coinStep = -1;
        }
    }
    int scoreDiff = task->m_scoreAfter - task->m_score;
    task->m_scoreStep = scoreDiff / 0x78;
    if (task->m_scoreStep == 0) {
        if (scoreDiff > 0) {
            task->m_scoreStep = 1;
        } else {
            task->m_scoreStep = -1;
        }
    }
    task->m_endWait = 0;
    g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
}

// Counts the coins down and the points to their new value; when both are there and the animation is done the scene ends.
int muGameOverTask::mainStepContinueMain(muGameOverTask* task) {
    bool coinsDone = false;
    bool scoreDone = false;
    int step = task->m_coinStep;
    task->m_coins += step;
    task->m_cost += step;
    if (step < 1) {
        if (task->m_coins <= task->m_coinsAfter) {
            task->m_coins = task->m_coinsAfter;
            coinsDone = true;
            task->m_cost = 0;
        }
    } else if (task->m_coinsAfter <= task->m_coins) {
        task->m_coins = task->m_coinsAfter;
        coinsDone = true;
        task->m_cost = 0;
    }
    task->m_msg->printf(1, "%d", clampInt(task->m_coins, -9999, 9999));
    task->m_msg->printf(2, "%d", clampInt(-task->m_cost, -999, 999));
    task->m_score += task->m_scoreStep;
    if (task->m_scoreStep < 1) {
        if (task->m_score <= task->m_scoreAfter) {
            task->m_score = task->m_scoreAfter;
            scoreDone = true;
        }
    } else if (task->m_scoreAfter <= task->m_score) {
        task->m_score = task->m_scoreAfter;
        scoreDone = true;
    }
    task->m_msg->printf(0, "%d", clampInt(task->m_score, -99999999, 99999999));
    if (coinsDone && scoreDone && task->m_object->isNodeAnimFinished() && ++task->m_endWait > 1) {
        dynamic_cast<scGameover*>(gfSceneManager::getInstance()->m_currentScene);
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = 0;
        manager->processStep = 2;
    }
    return task->m_step;
}

void muGameOverTask::mainStepGameOverInit(muGameOverTask* task) {
    task->setGameOverObjAction(4);
    g_sndSystem->playSEwithFixEffect(static_cast<SndID>(0x2000), 1.0f, 1.0f);
    g_sndSystem->playBGM(0x2864, 0, true);
}

// The "Game Over" text appears; a button press skips to the end of its animation.
// Whether a button of the pads was pressed in this frame.
static inline bool isAnyButtonPressed() {
    gfPadStatus pad;
    muGameOverPadCopy padCopy;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    padCopy = *reinterpret_cast<muGameOverPadCopy*>(&pad);
    return padCopy.m_words[3] != 0;
}

int muGameOverTask::mainStepGameOverMain(muGameOverTask* task) {
    int next;
    if (task->m_object->isNodeAnimFinished() || isAnyButtonPressed()) {
        task->setGameOverObjAction(5);
        next = Step_GameOverButton;
    } else {
        next = task->m_step;
    }
    return next;
}

void muGameOverTask::mainStepGameOverStartInit(muGameOverTask* task) {
    char text[0x108];
    int size;
    task->setGameOverObjAction(6);
    if (task->m_unk6C > 0) {
        int length = sprintf(text, "%d", task->m_unk6C);
        const char* msgData = task->m_msg->getMsgData(0, &size);
        memcpy(text + length, msgData, size);
        text[length + size] = '\0';
        task->m_msg->printData(3, text, length + size + 1);
    }
    g_sndSystem->playSEwithFixEffect(static_cast<SndID>(0x2000), 1.0f, 1.0f);
    g_sndSystem->playBGM(0x2864, 0, true);
}

int muGameOverTask::mainStepGameOverStartMain(muGameOverTask* task) {
    if (!task->m_object->isNodeAnimFinished()) {
        return task->m_step;
    }
    return Step_GameOverButton;
}

void muGameOverTask::mainStepGameOverButtonInit(muGameOverTask* task) { }

// A, B or Start (or the corresponding buttons) leaves the screen.
int muGameOverTask::mainStepGameOverButtonMain(muGameOverTask* task) {
    muGameOverPadCopy padCopy;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    padCopy = *reinterpret_cast<muGameOverPadCopy*>(&pad);
    if ((padCopy.m_words[3] & 0x1300) != 0) {
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        dynamic_cast<scGameover*>(gfSceneManager::getInstance()->m_currentScene);
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = 1;
        manager->processStep = 2;
    }
    return task->m_step;
}
