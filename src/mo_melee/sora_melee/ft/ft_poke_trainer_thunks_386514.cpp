// The vtable adjustor thunks of ftPokeTrainer and its ball article mediator sit in a separate text range of the original ft_poke_trainer unit.
// TODO: clean up - this unit only re-compiles ft_poke_trainer.cpp so these weak functions emit; the real fix is merging the text ranges into one unit.
// TODO: clean up - MATCH-ONLY (compiler workaround): MWCC hits an internal compiler error (CTemplateTools.c:256) in this
// unit depending on how many preprocessor names it has seen (see ft_poke_trainer_wandering_module.cpp); these unused names
// keep it compiling. Change them if the ICE returns.
#ifdef FT_POKETRAINER_ICE_DODGE_386514_0
#endif
#ifdef FT_POKETRAINER_ICE_DODGE_386514_1
#endif
#ifdef FT_POKETRAINER_ICE_DODGE_386514_2
#endif
#ifdef FT_POKETRAINER_ICE_DODGE_386514_3
#endif
#include "../src/mo_melee/sora_melee/ft/ft_poke_trainer.cpp"
