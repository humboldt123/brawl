// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/zako/ft_zako.h>
#include <ft/zako/ft_zako_extend_param_accesser.h>
#include <ft/zako/ft_zako_impl.h>

#define FT_BC ftZakoGirlBuildConfig
#define FT_NOINLINE_SKIP_SHARED_DEFS
#include <ft/builder/ft_builder_noinline.h>

ftZakoGirlExtendParamAccesser g_ftZakoGirlExtendParamAccesser;

ftClassInfoImpl<Fighter_Zako_Girl, ftZakoGirl> g_ftClassInfoZakoGirl;

FT_ZAKO_IMPL(Girl)

FT_ZAKO_ON_ACTIVATE(Girl)
