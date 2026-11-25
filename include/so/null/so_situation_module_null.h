#pragma once

#include <so/situation/so_situation_module_impl.h>

class soSituationModuleNull : public soSituationModule {
public:
    virtual ~soSituationModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual SituationKind getKind();
    virtual SituationKind getPrevKind();
    virtual bool isSituationChanged();
    virtual void setKind(SituationKind kind, bool);
    virtual void setKeepAir(bool);
    virtual void update();
};
