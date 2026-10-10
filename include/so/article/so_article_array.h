#pragma once

#include <ft/builder/ft_dol_instances.h>

class soArticle;
// The empty-vector constructor receives an unused long selector, as in the
// fighter-side declarations. Its zero initialization is owned by sora_melee.
// Use a typedef so const element references qualify the pointer itself.
typedef soArticle* soArticlePtr;
FT_DOL_ARRAY_VECTOR(soArticlePtr, 2);
static_assert(sizeof(soArrayVector<soArticle*, 2>) == 0x14, "Article pointer array layout");
