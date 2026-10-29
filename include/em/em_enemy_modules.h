#pragma once

// The module classes and module storage the Enemy class (em/enemy.h) embeds, other than those with a header of their own.
// - The soArrayVector<T, C> instances are classes of sora_melee: the Enemy unit only calls their constructors and
//   destructors (FT_DOL_ARRAY_VECTOR declares them without a definition).
// - The em*ModuleImpl classes are the enemy versions of the so* modules; their code is in later units of sora_enemy that
//   are not reconstructed yet, so only their size (the distance to the next member of Enemy), their base class (from the
//   module map names) and the constructor the Enemy constructor calls are declared here.

#include <StaticAssert.h>
#ifdef EM_ENEMY_BUILDER
// The Enemy unit (which defines EM_ENEMY_BUILDER before any include) calls the sora_melee constructors of these arrays.
// The other units that include em/enemy.h only need the layout, which the plain template gives: an explicit
// specialization there would come after the template was already instantiated by the headers they include first.
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_animcmd.h> // the anim cmd and motion tables of sora_melee
#include <ft/builder/ft_builder_motion.h>
#include <ft/builder/ft_builder_status.h> // soArrayQueueImpl
#else
#include <so/so_array_queue_impl.h>
#endif
#include <ft/builder/ft_dol_types.h>
#include <ac/ac_cmd_interpreter.h>
#include <so/article/so_article.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/templates/so_array_value_soTargetSearchRayCheckCache.h>
#include <so/transition/so_transition_module_impl.h>
#include <types.h>
#include <wn/wn_em_simple.h> // soArrayVector<s32, 2>

class emTargetSearchUnit;

#ifdef EM_ENEMY_BUILDER
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 11);
FT_DOL_ARRAY_VECTOR(soOtherAnim, 3);
FT_DOL_ARRAY_VECTOR(soLinkConnection, 5);
FT_DOL_ARRAY_VECTOR(soCollisionAttackAbsolute, 1);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 10);
FT_DOL_ARRAY_VECTOR(soCollisionCatchPart, 2);
FT_DOL_ARRAY_VECTOR(soShakeTerm, 2);
FT_DOL_ARRAY_VECTOR(soTargetSearchRayCheckCache, 8);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 6);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 7);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 12);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 16);
FT_DOL_ARRAY_VECTOR(soArticle*, 10);
#endif

// HYPOTHESIS: the elements of this array are 0x10 bytes in the original (the array is 0xAC bytes and the mangled name
// says soArticleEventObserver), while BrawlHeaders' soArticleEventObserver is 0xC; the element layout is not used here.
template <>
class soArrayVector<soArticleEventObserver, 10> : public soArrayVectorAbstract<soArticleEventObserver> {
    s32 m_topIndex : sizeof(bit_width<10>) + 1;
    s32 m_lastIndex : sizeof(bit_width<10>) + 1;
    s32 m_size : sizeof(bit_width<10>) + 1;
    u32 m_isFull : 1;
    u8 m_elements[10][0x10];
public:
    soArrayVector(s32 size, s32);
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soArticleEventObserver& atFastAbstractSub(s32 index) const;
    virtual soArticleEventObserver& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};

// The anim cmd control units of the enemy (one per interpreter). The constructor is expanded in the Enemy constructor
// (its list is a class of sora_melee), the destructor is a weak copy of the Enemy unit, the methods are sora_melee's.
// A partial specialization (on the capacity) rather than a full one: the unit emits the vtable of a class template whose
// methods are only declared, with the thunks of the policy bases, as the original does; a plain class with out-of-line
// virtuals would leave the vtable to the unit that defines them.
template <u32 C>
class soInstanceManagerFullPropertyUniqImpl<soAnimCmdControlUnit, C> : public soInstanceManagerFullProperty<soAnimCmdControlUnit> {
    soArrayList<soInstanceUnitFullProperty<soAnimCmdControlUnit>, C> m_arrayList; // 0x10
public:
#ifdef SO_ARRAY_LIST_CTOR_FLAG
    // The Enemy unit (which defines the flag) expands it; the list constructor is the one of sora_melee.
    soInstanceManagerFullPropertyUniqImpl() : m_arrayList(0) { }
#else
    soInstanceManagerFullPropertyUniqImpl();
#endif
    virtual soAnimCmdControlUnit& at(s32 index);
    virtual soAnimCmdControlUnit& atIndex(s32 index);
    virtual s32 getId(s32 index);
    virtual u32 size() const;
    virtual bool isContain(s32) const;
    virtual void erase(s32);
    virtual void clear();
    virtual void set(const soAnimCmdControlUnit&, s32 index);
    virtual s32 add(soAnimCmdControlUnit&, s32, soAttributeFlag, s16);
    virtual u32 capacity();
    virtual soInstanceUnitFullProperty<soAnimCmdControlUnit>& atUnitIndexFast(s32 index);
    virtual s32 getIndex(s32 index) const;
    virtual void getAttributeArray(soAttributeFlag mask, soArray<soAnimCmdControlUnit*>& arr);
    virtual soAttributeFlag getAttribute(s32) const;
    virtual void getPriorityArray(soArray<soAnimCmdControlUnit*>& arr);
};
static_assert(sizeof(soInstanceManagerFullPropertyUniqImpl<soAnimCmdControlUnit, 11>) == 0x120, "Class is wrong size!");

// The transition module of the enemy status module: the module followed by its GroupNum term groups of TermCap terms each.
// Built inline by the Enemy constructor; its destructor is a weak copy of the Enemy unit (map: soTransitionModuleEntity<3,1>).
template <int TermCap, int GroupNum>
class soTransitionModuleEntity : public soTransitionModuleImpl {
    soArrayVector<soTransitionTermGroup, GroupNum> m_groups;                       // +0x18
    soArrayVector<soInstanceUnitFullProperty<soTransitionTerm>, TermCap> m_terms[GroupNum]; // +0x38
public:
    soTransitionModuleEntity() : soTransitionModuleImpl(&m_groups), m_groups(0) {
        for (u32 i = 0; i < GroupNum; i++) {
            soTransitionTermGroup group(&m_terms[i]);
            soArray<soTransitionTermGroup>* groups = &m_groups;
            groups->push(group);
        }
    }
    virtual ~soTransitionModuleEntity() { }
};

// ---- modules of sora_melee without a header ----------------------------------------------------------------------------

// HYPOTHESIS: class name. The target search module (soModuleAccesser slot targetSearchModule), built from the enemy's
// target search units and parameters (emInfo::getTargetSearchNum / getTargetSearchParamAryPtrGetId) and the ray-check cache.
class soTargetSearchModuleImpl {
    u8 unk4[0x34 - 4];
public:
    soTargetSearchModuleImpl(soModuleAccesser* acc, emTargetSearchUnit* units, void* params, int unitNum,
                             soArray<soTargetSearchRayCheckCache>* rayCheckCache);
    virtual ~soTargetSearchModuleImpl();
};
static_assert(sizeof(soTargetSearchModuleImpl) == 0x34, "Class is wrong size!");

// HYPOTHESIS: class name. The glow module (soModuleAccesser slot glowModule) of the enemy; Enemy::resetGlowParam drives it.
class soGlowModuleImpl {
    u8 unk4[0x24 - 4];
public:
    soGlowModuleImpl(soModuleAccesser* acc);
    virtual ~soGlowModuleImpl();
};
static_assert(sizeof(soGlowModuleImpl) == 0x24, "Class is wrong size!");

// ---- enemy modules (code in later sora_enemy units) ---------------------------------------------------------------------

#include <em/ground/em_ground_module_impl.h>

// soLinkModuleImpl has its full layout only under FT_MODULE_BUILDER (see em/enemy.h).
#ifdef FT_MODULE_BUILDER
#include <em/link/em_link_module_impl.h>
#else
class emLinkModuleImpl;
#endif

#include <em/collision/em_collision_hit_module_impl.h>

#include <em/status/em_status_module_impl.h>

#include <em/territory/em_territory_module_impl.h>

// The damage module (code in damage/em_damage_module_impl.cpp).
#include <em/damage/em_damage_module_impl.h>

#include <em/turn/em_turn_module_impl.h>

// The kinetic module and its energies (code in kinetic/em_kinetic_module_impl.cpp and kinetic/em_kinetic_energy.cpp).
#include <em/kinetic/em_kinetic_module_impl.h>
