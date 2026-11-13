// TODO(link): every function of this unit is exact. The linked .text order is solved but needs a shared-header
// change: with a unit macro in so/so_array.h that (a) declares the soArrayVector / soArrayVectorAbstract overriders
// in native order (destructor first, then getTopIndex, setTopIndex, getLastIndex, setLastIndex, getArrayValueConst,
// onFull, offFull, isFull, capacity, size, atFastAbstractSub, setSize; and at, at const, unshift, shift, push, pop,
// insert, erase, set, clear, isNull; the vtable layout comes from the bases and does not change) and (b) only
// declares the never_inline contractible destructor, this unit gets the native .text when it declares explicit
// specializations of the vector destructor, push, the abstract destructor and the contractible destructor before the
// class, declares the SpecialLw destructor in the class, and defines all five strong after initStatus in that order
// (SpecialLw dtor, vector dtor, push, abstract dtor, contractible dtor). A scratch link of ft_peach then has an
// identical .text (sizes of every section equal). Defined inline, or with the specializations before initStatus, the
// callee chain comes first instead.
// Still blocking: .data. The original emits the "soArrayVectorCalcInterface" name and RTTI first (before SpecialLw's
// vtable); this unit emits them after soArrayFixed<float>'s RTTI, and every other object is in native order.
// Ft_pikmin's side special and Kirby's inhale status have the same CalcInterface-first RTTI, and none of the 20
// units that emit that RTTI is linked. Ruled out for the RTTI position: every GC/3.0a3..3.0a5.2 and Wii/1.0..1.5
// compiler, a sizeof or static assert of the vector before the class or before the includes, the global object
// before initStatus, the destructor declared first in the class (key function), and the CalcInterface destructor
// defined out of class (inline or weak) at the top of the unit. Earlier rounds: implicit or never_inline
// destructors, -inline deferred/bottomup/nobottomup/on,noauto, -ipa file and #pragma
// defer_codegen/inline_depth/inline_bottom_up do not change the weak order.
// BYTES: the native vector teardown keeps the contractible array destructor out of line.
#define SO_ARRAY_CONTRACTIBLE_DTORS_NOINLINE
#include <ft/peach/ft_peach.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Map names: soUtilityLotService::lotSimple (sora_melee 0x8E8A4: true when a roll of randi(percent) comes up 0) and
// soUtilityLotService::lotTable (0x8E950: weighted pick of an index from a table of floats, -1 when the weights sum to
// zero). Their symbols are not named in sora_melee yet.
extern "C" u32 fn_27_8E8A4(int percent);
extern "C" int fn_27_8E950(soArray<float>* weights);

class ftPeachStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual ~ftPeachStatusUniqProcessSpecialLw() { }
};

// HYPOTHESIS: the extend parameter block continues after the six group pointers of ftData with the table of the
// vegetables (and the Toad) that Peach's down special can pull: an array of entries and their count.
struct ftPeachVegetable {
    int id;       // value stored for the status work (0x20000002)
    float weight; // relative chance of the entry
};

struct ftPeachVegetableTable {
    void* groups[6];
    ftPeachVegetable* entries;
    int count;
};

// Down special: decides whether Peach pulls an item at all, then picks which one by weight.
void ftPeachStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (fn_27_8E8A4(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0)) == 1) {
        ftPeach* peach = dynamic_cast<ftPeach*>(&moduleAccesser->getStageObject());
        if (peach) {
            ftPeachVegetableTable* table = reinterpret_cast<ftPeachVegetableTable*>(peach->getExtendParam());
            soArrayVector<float, 20> weights;
            for (int i = 0; i < table->count; i++) {
                weights.push(table->entries[i].weight);
            }
            int index = fn_27_8E950(&weights);
            if (index < 0 || index >= table->count) {
                return;
            }
            moduleAccesser->getWorkManageModule().setInt(table->entries[index].id, 0x20000002);
        }
    } else {
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    }
}

ftPeachStatusUniqProcessSpecialLw g_ftPeachStatusUniqProcessSpecialLw;
