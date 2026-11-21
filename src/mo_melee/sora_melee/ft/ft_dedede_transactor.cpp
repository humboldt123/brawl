#define WN_DEDEDE_STARMISSILE_ACTIVATE_ONLY
#include <StaticAssert.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_manager.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <wn/dedede/wn_dedede_starmissile.h>


// TODO: notifyEventCollisionSearch (the inhale search).

ftDededeTransactor::ftDededeTransactor() { }
ftDededeTransactor::~ftDededeTransactor() { }

// Inhale statuses (0x112, 0x117): once the search caught an article (work flag 0x22000019), link it, tell it it was
// swallowed (link event 0x835) and change to the swallow status (Kirby's copy has its own).
bool ftDededeTransactor::notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if ((status == 0x112 || status == 0x117) && moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        int caughtTaskId = moduleAccesser->getWorkManageModule().getInt(0x20000006);
        moduleAccesser->getLinkModule().link(6, caughtTaskId);
        soLinkEventArgs args(0x835);
        moduleAccesser->getLinkModule().sendEventParents(6, args);
        moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x1a3, moduleAccesser);
        } else {
            moduleAccesser->getStatusModule().changeStatusRequest(0x127, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
        return true;
    }
    return false;
}

// The star missile is article 0xd (Dedede's Kirby resource id).
bool ftDededeTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDededeStarmissile* missile = dynamic_cast<wnDededeStarmissile*>(article);
        if (missile == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(missile, acc, resources.getKirbyResId(Fighter_Dedede));
    }
    return false;
}

// The fighter linked as the node parent.
static inline Fighter* getLinkOwnerFighter(soModuleAccesser* acc) {
    ftManager* manager = g_ftManager;
    return manager->getFighter(manager->getEntryIdFromTaskId(acc->getLinkModule().getNodeTaskId(0), 0), -1);
}

// The star missile starts in front of the mouth node; its scale and kind come from the attribute block of the fighter
// linked as the node parent (the copy owner when Kirby spits it). Its task id is kept in work 0x20000004.
bool ftDededeTransactor::activeArticle(wnDededeStarmissile* weapon, soModuleAccesser* acc, u32 etcResId) {
    float lr = acc->getPostureModule().getLr();
    Vec3f pos;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    float scale = acc->getPostureModule().getScale();
    offset.m_x = scale * acc->getConstantFloatKirby(0xfa5);
    offset.m_y = scale * acc->getConstantFloatKirby(0xfa6);
    pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    pos.m_x += offset.m_x * lr;
    pos.m_y += offset.m_y;
    pos.m_z = acc->getGroundModule().getZ();
    ftParam param = *g_ftCommonDataAccesser.getParam(getLinkOwnerFighter(acc)->getFtKind());
    float starScale = param.unk1A4;
    s32 starParam = param.unk1A8;
    weapon->activate(acc->getStageObject().getTaskId(), etcResId, acc->getTeamModule().getTeam()->getNo(), &pos,
        acc->getPostureModule().getLr(), starScale, starParam, 0);
    int taskId = weapon->getTaskId();
    acc->getWorkManageModule().setInt(taskId, 0x20000004);
    return false;
}

#include <gf/gf_task_scheduler.h>
// TODO: clean up, give variables real names
namespace jj_3959D4 {
extern "C" {
extern unsigned char g_ftKirbyCopyAbilityIdConverter[];
extern char __RTTI__6Weapon[];
extern char __RTTI__6gfTask[];
int convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(void*, unsigned int, int, void*, unsigned int);
void* __dynamic_cast(void*, long, const void*, const void*, short);
}
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual void slot4();
 virtual void slot5(int, unsigned int); virtual void slot6(); virtual void slot7(); virtual void slot8(); virtual void slot9();
 virtual void slot10(); virtual void slot11(); virtual void slot12(); virtual void slot13(); virtual void slot14(); virtual void slot15();
 virtual int slot16(); virtual int slot17(unsigned int); virtual void slot18(unsigned int);
};
struct Modules { char pad[0x64]; Object_vptr_0* work; char pad68[8]; Object_vptr_0* status; };
struct Object { char pad[0xd8]; Modules* modules; };
struct Event { char pad[0xc]; unsigned int id; };
struct TargetBase { char pad[0x3c]; };
class Target : public TargetBase { public:
 virtual void slot0();
 virtual void slot1();
 virtual void slot2();
 virtual void slot3();
 virtual void slot4();
 virtual void slot5();
 virtual void slot6();
 virtual void slot7();
 virtual void slot8();
 virtual void slot9();
 virtual void slot10();
 virtual void slot11();
 virtual void slot12();
 virtual void slot13();
 virtual void slot14();
 virtual void slot15();
 virtual void slot16();
 virtual void slot17();
 virtual void slot18();
 virtual void slot19();
 virtual void slot20();
 virtual void slot21();
 virtual void slot22();
 virtual void slot23();
 virtual void slot24();
 virtual void slot25();
 virtual void slot26();
 virtual void slot27();
 virtual void slot28();
 virtual void slot29();
 virtual void slot30();
 virtual void slot31();
 virtual void slot32();
 virtual void slot33();
 virtual void slot34();
 virtual void slot35();
 virtual void slot36();
 virtual void slot37();
 virtual void slot38();
 virtual void slot39();
 virtual void slot40();
 virtual void slot41();
 virtual void slot42();
 virtual void slot43();
 virtual void slot44();
 virtual void slot45();
 virtual void slot46();
 virtual void slot47();
 virtual void slot48();
 virtual void slot49();
 virtual void slot50();
 virtual void slot51();
 virtual void slot52();
 virtual void slot53();
 virtual void slot54();
 virtual void slot55();
 virtual void slot56();
 virtual void slot57();
 virtual void slot58();
 virtual void slot59();
 virtual void slot60();
 virtual void slot61();
 virtual void slot62();
 virtual void slot63();
 virtual void slot64();
 virtual void slot65();
 virtual void slot66();
 virtual void slot67();
 virtual void slot68();
 virtual void slot69();
 virtual void slot70();
 virtual void slot71();
 virtual void slot72();
 virtual void slot73();
 virtual void slot74();
 virtual void slot75();
 virtual void slot76();
 virtual void slot77();
 virtual void slot78();
 virtual void slot79();
 virtual void slot80();
 virtual void slot81();
 virtual void slot82();
 virtual void slot83();
 virtual void slot84();
 virtual void slot85();
 virtual void slot86();
 virtual void slot87();
 virtual void slot88();
 virtual void slot89();
 virtual void slot90();
 virtual void slot91();
 virtual void slot92();
 virtual void slot93();
 virtual void slot94();
 virtual void slot95();
 virtual void slot96();
 virtual void slot97();
 virtual void slot98();
 virtual void slot99();
 virtual void slot100();
 virtual void slot101();
 virtual void slot102();
 virtual void slot103();
 virtual void slot104();
 virtual void slot105();
 virtual void slot106();
 virtual void slot107();
 virtual void slot108();
 virtual void slot109();
 virtual void slot110();
 virtual void slot111();
 virtual void slot112();
 virtual void slot113();
 virtual void slot114();
 virtual void slot115();
 virtual void slot116();
 virtual void slot117();
 virtual void slot118();
 virtual void slot119();
 virtual void slot120();
 virtual void slot121();
 virtual void slot122();
 virtual void slot123();
 virtual void slot124();
 virtual void slot125();
 virtual void slot126();
 virtual void slot127();
 virtual void slot128();
 virtual void slot129();
 virtual void slot130();
 virtual void slot131();
 virtual void slot132();
 virtual void slot133();
 virtual void slot134();
 virtual void slot135();
 virtual void slot136();
 virtual void slot137();
 virtual void slot138();
 virtual void slot139();
 virtual void slot140();
 virtual void slot141();
 virtual void slot142();
 virtual void slot143();
 virtual void slot144();
 virtual void slot145();
 virtual void slot146();
 virtual void slot147();
 virtual void slot148();
 virtual void slot149();
 virtual void slot150();
 virtual int check();
 char pad40[0x78]; int kind;
};
extern "C" void notifyEventCollisionSearch__18ftDededeTransactorFP14soCollisionLogP16soModuleAccesser(void*, Event* event, Object* obj) {
 int status = convCorrectToOrigId__29ftKirbyCopyAbilityIdConverterFiiP16soModuleAccesserb(g_ftKirbyCopyAbilityIdConverter, 0, obj->modules->status->slot16(), obj, 1);
 if ((status == 0x112 || status == 0x117) && !obj->modules->work->slot17(0x22000019)) {
  gfTask* task = gfTaskScheduler::getInstance()->getTask(event->id);
  if (task && task->m_taskCategory == gfTask::Category_Weapon) {
   Target* target = (Target*)__dynamic_cast(task, 0x3c, __RTTI__6Weapon, __RTTI__6gfTask, 0);
   obj->modules->work->slot5(0, 0x20000007);
   if (target->check()) {
    switch (target->kind) {
    case 0x37:
     obj->modules->work->slot5(2, 0x20000007);
    case 0x33:
     obj->modules->work->slot18(0x22000019);
     obj->modules->work->slot5(event->id, 0x20000006);
     break;
    }
   }
  }
 }
}
}

