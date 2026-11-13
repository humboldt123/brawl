#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local (98.3% vs 97.2%), as in the Mario fireball
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define SO_OTHER_ANIM_CTOR // BYTES: the sora_melee motion builder constructs and destroys its other-animation slot (soOtherAnim has real ctor and dtor)
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk, as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_samus_gbeam.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// HYPOTHESIS: the model node the beam attaches to (-1: none); the first data word of the unit.
static int s_attachNodeId = -1;

extern soStatusUniqProcess g_wnLassoStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnSamusGBeam::wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data) :
    wnWeaponBuilder<wnSamusGBeamModuleAccesserBuildConfig>(
        "SamusGBeam", articleId, info,
        wnModuleAccesserBuildData(0x84, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessMotionPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

inline wnSamusGBeamParamAccesser::~wnSamusGBeamParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSamusGBeamModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

// TODO(nonmatching): activate (95.3%). The original passes the last parameter to setFlag(bool, u32) unchanged and compares it
// with cmplwi, which is what a `bool unk` parameter compiles to (that spelling is exact apart from the flag byte register
// numbering, r9/r0), but the symbol of the original is spelled with an int parameter (...P5Vec3fi, from the module map) and the
// fighter shell (ft/samus/ft_samus_article_pools.h, ftSamus::activeArticle7) passes an int. Switching needs the symbol rename
// and the shell change in one step. The flag byte register allocation (two load/modify/store chains) is still open.
void wnSamusGBeam::activate(int founderTaskId, int etcResourceId, int team, float lr, Vec3f* __restrict pos, int attack) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk38 = 3;
    desc.unk3C = 0xff;
    Weapon::activate(&desc);
    soResourceModule& resource = m_moduleAccesser->getResourceModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    int nodeId = s_attachNodeId;
    int noValue = 0;
    alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, 1, 0, noValue, -1);
    m_moduleAccesser->getWorkManageModule().setFlag(attack, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(0x50, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(2, 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    wnLassoGetAltStatus(m_moduleAccesser)->unkDCFloat(posture.getScale());
    wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
    static_cast<wnSamusGBeamStatusView*>(static_cast<wnLassoViewPad8*>(wnLassoGetAltStatus(m_moduleAccesser)))->unkB8Int(1);
    if (attack == 1) {
        m_moduleAccesser->getWorkManageModule().setInt(5, 0x20000000);
        m_moduleAccesser->getWorkManageModule().setFloat(32.064f, 0x21000000);
        m_moduleAccesser->getWorkManageModule().setFloat(100.0f, 0x21000001);
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk14, 0x21000002);
    }
}

// TODO(nonmatching): the animation command sub builder constructor (97.2%): same family as the Mario fireball's and the Toad's
// (98.3%; the original keeps the zero byte whose address goes to the interpreter in a word-aligned slot of its own per unit
// builder, ours packs the five bytes into one word: a one-byte class temporary in ft/builder/ft_builder_animcmd.h's ftAddressOf
// plus a stack pointer cast reaches 99.2%, and what is left is only a callee-saved register rotation, see the S3 shared patch).
// TODO(callees): the situation, soArrayContractibleTable, soArrayVector and soInstanceUnitFullProperty<soTransitionTerm>
// references in the module builder constructors point at unnamed weak copies of other units (lbl_27_data_20950, 3C578, 3ED00,
// fn_27_D2A0C), so the status builder constructor shows 99.99% until those have names.
wnSamusGBeam::~wnSamusGBeam() { }
