// BYTES: this unit has its own copy of the soItemPickTransactor destructor and expands the soItemPickTransactorImpl one.
#define SO_ITEM_PICK_TRANSACTOR_INLINE_DTORS
#define SO_NULLABLE_INLINE_CTOR
#define SO_ITEM_INFO_BYTE_ATTACH_GROUP
#include <ft/dedede/ft_dedede_waddledee_search_impl.h>
#include <it/it_manager.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/item/so_item_pick_transactor_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// sora_melee: soItemPickTransactorImpl::check and ::pickup (the map names); they stay unnamed there.
extern "C" bool fn_27_BE760(soItemPickTransactorImpl* transactor, soItemSearch* search, soItemInfo* info,
                            soModuleAccesser* moduleAccesser);
extern "C" bool fn_27_BE864(soItemPickTransactorImpl* transactor, soItemSearch* search, soModuleAccesser* moduleAccesser,
                            u32 unk, itParam::TraitMask mask, u32 unk2, u32 index);

// The item pick transactor of King Dedede's item manage module (g_ftDededeItemPickTransactor): a Waddle Dee found by the
// search counts as a pickable item, and picking it up starts status 0x129.
class ftDededeWaddledeePickTransactorImpl : public soItemPickTransactorImpl {
public:
    virtual ~ftDededeWaddledeePickTransactorImpl() { }
    virtual bool check(soItemSearch* search, soItemInfo* info, soModuleAccesser* moduleAccesser);
    virtual bool pickup(soItemSearch* search, soModuleAccesser* moduleAccesser, u8 unk, itParam::TraitMask mask, u32 unk2,
                        u32 index);
};

bool ftDededeWaddledeePickTransactorImpl::check(soItemSearch* search, soItemInfo* info, soModuleAccesser* moduleAccesser) {
    ftDededeWaddledeeSearchImpl::Result result;
    if (dynamic_cast<ftDededeWaddledeeSearchImpl&>(*search).searchWaddledee(&result, moduleAccesser)) {
        info->m_item = NULL;
        info->m_kind = (itKind)-1;
        info->m_trait.m_mask = (itParam::TraitMask)0;
        info->m_size = (itParam::SizeKind)0;
        info->m_isVisible = true;
        info->m_isHoldAnim = false;
        info->m_attachGroup = 0xfe;
        return true;
    }
    return fn_27_BE760(this, search, info, moduleAccesser);
}

// HYPOTHESIS: statuses 0x96/0x97 are the item-catch statuses, status 0x129 lifts the Waddle Dee.
bool ftDededeWaddledeePickTransactorImpl::pickup(soItemSearch* search, soModuleAccesser* moduleAccesser, u8 unk,
                                                 itParam::TraitMask mask, u32 unk2, u32 index) {
    if (moduleAccesser->getStatusModule().getStatusKind() != 0x96 &&
        moduleAccesser->getStatusModule().getStatusKind() != 0x97 &&
        !moduleAccesser->getItemManageModule().isHaveItem(0) &&
        moduleAccesser->getWorkManageModule().getInt(0x10000043) != -1) {
        moduleAccesser->getStatusModule().changeStatusForce(0x129, moduleAccesser);
        return true;
    }
    return fn_27_BE864(this, search, moduleAccesser, unk, mask, unk2, index);
}

// HYPOTHESIS: the two item generation sheet kinds that other units define the same way (no user in this unit).
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);
ftDededeWaddledeePickTransactorImpl g_ftDededeItemPickTransactor;
