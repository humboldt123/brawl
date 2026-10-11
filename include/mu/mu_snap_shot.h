#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <memory.h>
#include <string.h>
#include <mu/mu_collection_viewer.h>
#include <mu/mu_object.h>
#include <types.h>

// The scene of the snapshot browser (module sora_scene).
struct scSnapShot : scCollectionScene { };

// The decoded picture of a snapshot: the pixels and the size.
// HYPOTHESIS: the first word owns the pixels (the destructor gives them back).
struct muSnapShotJpeg;
// muRecorder::Data::reset(pixels), the function of main.dol that gives the pixels back and takes new ones
extern "C" void fn_800C2BB4(muSnapShotJpeg* jpeg, void* pixels);

struct muSnapShotJpeg {
    void* m_pixels;                 // 0x00
    u32 m_width;                    // 0x04
    u32 m_height;                   // 0x08

    muSnapShotJpeg() : m_pixels(0) { }
    ~muSnapShotJpeg() {
        if (this != 0) {
            reset(0);
        }
    }
    void reset(void* pixels) { fn_800C2BB4(this, pixels); }
};

// The snapshot browser of the collection: shows the pictures that the player took in the snapshot mode, one at a time.
class muSnapShotTask : public muCollectionViewer {
public:
    // The data of the menu. HYPOTHESIS: the picture is the one that is decoded from the file for the screen.
    class Work : public muCollectionViewer::Work {
    public:
        Work();
        virtual ~Work();

        scSnapShot* m_scene;                // 0x1D0C the scene the menu belongs to
        int m_slideResult;                  // 0x1D10 what the last slide of the picture gave
        muSnapShotJpeg m_jpeg;              // 0x1D14
    };

    muSnapShotTask(const char* name) : muCollectionViewer(name) { }
    static muSnapShotTask* create(scSnapShot* scene);

    virtual ~muSnapShotTask();
    virtual bool preLoad();
    virtual void loadResource();
    virtual void createModel();
    virtual void getMineLabel(muCollectionMsgBuf* buf);
    virtual void getNewLabel();
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual int dispThumbnail(MuObject* object, void* summary);
    virtual void dispPreviewCore(void* info);
    virtual void erasePreviewCore();
    virtual void* getScnRoot();
    virtual void setPanelStateCore(int state, int unk);
    virtual void cancelAndExit();
    virtual void onDecided();
    virtual void controlExit();
    virtual void onPreCheckFinished();
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlBrowsing();
    virtual void controlBrowsingLoading();
    virtual void controlPopUpStart();
    virtual void controlPopUp();
    virtual void controlPopUpCheckStart();
    virtual void controlPopUpChecking();
    virtual void controlNewDataStart();
    virtual void controlNewDataChecking();
    virtual void controlNewData();
    virtual void controlNewDataExit();

    Work* getWork() const { return static_cast<Work*>(m_work); }
};
static_assert(sizeof(muSnapShotTask) == 0x1B0, "Class is wrong size!");
