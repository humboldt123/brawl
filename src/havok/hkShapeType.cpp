// Havok translation unit hkShapeType.o (main.dol 0x802D4FE4-0x802D51F4).
// Functions in address order:
//   0x802D4FE4   528  hkGetShapeTypeName   [map: hkShapeType__hkGetShapeTypeName]

#include <havok/hkShapeType.h>

const char* hkGetShapeTypeName(int type) {
    switch (type) {
    case HK_SHAPE_ALL:
        return "HK_SHAPE_ALL";
    case HK_SHAPE_CONVEX:
        return "HK_SHAPE_CONVEX";
    case HK_SHAPE_COLLECTION:
        return "HK_SHAPE_COLLECTION";
    case HK_SHAPE_BV_TREE:
        return "HK_SHAPE_BV_TREE";
    case HK_SHAPE_SPHERE:
        return "HK_SHAPE_SPHERE";
    case HK_SHAPE_TRIANGLE:
        return "HK_SHAPE_TRIANGLE";
    case HK_SHAPE_BOX:
        return "HK_SHAPE_BOX";
    case HK_SHAPE_CAPSULE:
        return "HK_SHAPE_CAPSULE";
    case HK_SHAPE_CYLINDER:
        return "HK_SHAPE_CYLINDER";
    case HK_SHAPE_CONVEX_VERTICES:
        return "HK_SHAPE_CONVEX_VERTICES";
    case HK_SHAPE_CONVEX_PIECE:
        return "HK_SHAPE_CONVEX_PIECE";
    case HK_SHAPE_MULTI_SPHERE:
        return "HK_SHAPE_MULTI_SPHERE";
    case HK_SHAPE_LIST:
        return "HK_SHAPE_LIST";
    case HK_SHAPE_CONVEX_LIST:
        return "HK_SHAPE_CONVEX_LIST";
    case HK_SHAPE_TRIANGLE_COLLECTION:
        return "HK_SHAPE_TRIANGLE_COLLECTION";
    case HK_SHAPE_MULTI_RAY:
        return "HK_SHAPE_MULTI_RAY";
    case HK_SHAPE_HEIGHT_FIELD:
        return "HK_SHAPE_HEIGHT_FIELD";
    case HK_SHAPE_SAMPLED_HEIGHT_FIELD:
        return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
    case HK_SHAPE_SPHERE_REP:
        return "HK_SHAPE_SPHERE_REP";
    case HK_SHAPE_TRI_PATCH:
        return "HK_SHAPE_TRI_PATCH";
    case HK_SHAPE_BV:
        return "HK_SHAPE_BV";
    case HK_SHAPE_PLANE:
        return "HK_SHAPE_PLANE";
    case HK_SHAPE_MOPP:
        return "HK_SHAPE_MOPP";
    case HK_SHAPE_TRANSFORM:
        return "HK_SHAPE_TRANSFORM";
    case HK_SHAPE_CONVEX_TRANSLATE:
        return "HK_SHAPE_CONVEX_TRANSLATE";
    case HK_SHAPE_CONVEX_TRANSFORM:
        return "HK_SHAPE_CONVEX_TRANSFORM";
    case HK_SHAPE_PHANTOM_CALLBACK:
        return "HK_SHAPE_PHANTOM_CALLBACK";
    case HK_SHAPE_USER0:
        return "HK_SHAPE_USER0";
    case HK_SHAPE_USER1:
        return "HK_SHAPE_USER1";
    case HK_SHAPE_USER2:
        return "HK_SHAPE_USER2";
    default:
        return "unknown";
    }
}
