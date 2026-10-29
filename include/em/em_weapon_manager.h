#pragma once

// SHADOW of BrawlHeaders em/em_weapon_manager.h (-I include comes first): the full emWeaponManager of
// em/weapon/em_weapon_manager.cpp, with RegisterManageWeapon, which the enemies' article activators call.

#include <StaticAssert.h>
#include <em/em_shot.h>
#include <ft/builder/ft_dol_instances.h>
#include <mt/mt_vector.h>
#include <so/so_array.h>
#include <so/templates/so_array_value_wnemProcFncObj.h>
#include <types.h>
#include <ut/ut_stageobject_pool.h>
#include <wn/wn_em_simple.h>

// Classes of sora_melee: the manager only calls their constructors and destructors. These are the
// FT_DOL_ARRAY_VECTOR specializations (ft/builder/ft_dol_instances.h) plus the declarations of at (both) and isEmpty,
// which the manager calls through the vtable: without them every call would make this unit emit a weak copy of the
// inline definitions of the base templates, which the original unit does not carry.
#define EM_WEAPON_ARRAY_VECTOR(T, C)                           \
    template <>                                                \
    class soArrayVector<T, C> : public soArrayVectorAbstract<T> {   \
        s32 m_topIndex : sizeof(bit_width<C>) + 1;             \
        s32 m_lastIndex : sizeof(bit_width<C>) + 1;            \
        s32 m_size : sizeof(bit_width<C>) + 1;                 \
        u32 m_isFull : 1;                                      \
        T m_elements[C];                                       \
    public:                                                    \
        soArrayVector(s32 = 0);                                \
        soArrayVector(s32 size, s32);                          \
        soArrayVector(s32 size, const T& element, s32);        \
        virtual T& at(s32 index);                              \
        virtual const T& at(s32 index) const;                  \
        virtual bool isEmpty() const;                          \
        virtual s32 size() const;                              \
        virtual ~soArrayVector();                              \
        virtual s32 capacity() const;                          \
        virtual bool isFull() const;                           \
        virtual T& atFastAbstractSub(s32 index) const;         \
        virtual T& getArrayValueConst(s32 index);              \
        virtual s32 getTopIndex() const;                       \
        virtual s32 getLastIndex() const;                      \
        virtual void setSize(s32 size);                        \
        virtual void setTopIndex(s32 topIndex);                \
        virtual void setLastIndex(s32 lastIndex);              \
        virtual void onFull();                                 \
        virtual void offFull();                                \
    }
EM_WEAPON_ARRAY_VECTOR(wnemProcFncObj, 70);
typedef wnemSimple* wnemSimplePtr; // for the const& of at
EM_WEAPON_ARRAY_VECTOR(wnemSimplePtr, 10);
#undef EM_WEAPON_ARRAY_VECTOR

class Fighter;

// The enemies' weapon manager: the pool of the 39 wnemSimple articles, the table of the weapon procs (id to function,
// filled by EnemyWeaponRegister) and the list of the weapons that other enemies can look up by task id.
class emWeaponManager : public utStageObjectPool<wnemSimple, 39> {
    soArrayVector<wnemProcFncObj, 70> m_procFncs; // 0x28, the registered procs
    soArrayVector<wnemSimple*, 10> m_managedWeapons; // 0x264, the weapons registered by RegisterManageWeapon

public:
    emWeaponManager();
    virtual ~emWeaponManager();
    // Starts a pooled article: lr and speed, the founder and owner ids, the shot work block and the position (see
    // fn_41_3091C, wnemSimple's activation). Does nothing without a work block.
    virtual void activateObject(wnemSimple* wnem, float lr, float speed, int founderTaskId, int ownerId,
                                wnemSimple_Data* work, int p7, Vec3f* pos, int team, int p10, bool flag, int life, s16 p13);
    virtual void deactivateObject(wnemSimple* wnem);
    virtual void resetProcFncList();
    virtual void resetManageWeaponList();
    // The weapon of the owner (task id) whose link id is `linkId` (0xFFFF: any), the `index`th match.
    wnemSimple* GetManagedWeapon(u32 taskId, u32 linkId, u32 index);
    wnemSimple* GetManagedWeaponFromTaskID(u32 taskId);
    // The same search among the active pool articles.
    wnemSimple* findWeapon(int taskId, u32 linkId, u32 index);
    // Fills the first free entry of the proc table.
    void RegisterProcFnc(void* proc, s32 id);
    void* GetProcFncPtr(s32 id);
    // HYPOTHESIS: adds the article to the list of managed weapons that findWeapon / GetManagedWeapon search.
    void RegisterManageWeapon(wnemSimple* weapon);
    void UnregisterManageWeapon(u32 taskId);
    // The fighter (of the four player slots) nearest to pos within maxDistance, or NULL.
    static Fighter* SearchNearestFighterPosition(const Vec2f* pos, float maxDistance);

    static void create();
    static void remove();
    static emWeaponManager* getInstance();
};
static_assert(sizeof(emWeaponManager) == 0x298, "Class is wrong size!");

// Registers the proc functions of every enemy weapon (em/weapon/em_weapon_register.cpp).
void EnemyWeaponRegister();
