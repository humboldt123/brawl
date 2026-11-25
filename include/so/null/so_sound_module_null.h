#pragma once

#include <so/sound/so_sound_module_impl.h>

inline soSoundModule::~soSoundModule() { }

class soSoundModuleNull : public soSoundModule {
public:
    virtual ~soSoundModuleNull();
    virtual void update(Vec3f* pos);
    virtual void deactivate();
    virtual void activate(Vec3f* pos);
    virtual void setPos(Vec3f* pos, int);
    virtual int playSE(SndID sndId, bool, bool, int);
    virtual int playSENo3d(SndID sndId, bool);
    virtual int playSEPos(SndID sndId, Vec3f* pos, bool, bool, int);
    virtual int playStatusSE(SndID sndId, bool, int);
    virtual void stopStatusSE();
    virtual void setPlayHitSEFlag(bool);
    virtual bool getPlayHitSEFlag();
    virtual void playHitSE(float, void*);
    virtual void playDamageSE(u32 index1, u32 index2);
    virtual void setSEVol(int handle, float volume, int);
    virtual float getSEVol(int);
    virtual void setSEPitch(float);
    virtual void setSEPitch(u32, double);
    virtual float getSEPitch();
    virtual void stopSE(int, int);
    virtual void stopSEHandle(int, int);
    virtual void setSESpeed(int, float);
    virtual void playLandingSE();
    virtual void setSoundIdData(SoundIdData*);
    virtual SoundIdData* getSoundIdData();
    virtual void setFixGeneratorStatus();
    virtual void setCheckSoundGroup(int);
    virtual int getCheckSoundGroup();
    virtual bool isPlay(int);
};
