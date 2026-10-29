#pragma once

// The fixed-capacity module arrays that more than one enemy class owns. The specializations are classes of sora_melee
// (the enemies only call their constructors and destructors); declared once here so that the enemy headers can be
// included together (em/em_all.h).

#include <ft/builder/ft_dol_instances.h>
#include <so/area/so_area_instance.h>
#include <so/camera/so_camera_subject.h>
#include <so/posture/so_posture_module_impl.h>

FT_DOL_ARRAY_VECTOR(soCameraSubject, 1);
// Hammer Bro, Robobeam.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 8);
// Kokkon, Popperam.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 9);
// Spar, Deathpod.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 10);
// Robobeam, Robodistance, Robohoming.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 54);
// Roada.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 17);
// Killer, Ghamgha base.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 19);
// Pacci.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 52);
// Jyakeel.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 9);
// Mite, Bitan.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 12);
// Flows.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 31);
// Shelly.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 64);
// Teckin, Jdus.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 33);
// Ngagog.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 37);
// Faulong.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 27);
// Cymal.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 14);
// Mite, Flows.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 13);
// Boobas.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 26);
// Siralamos.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 15);
// Bucyulus, Popperam, Cataguard.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 23);
// Dekakuribo, Kokkon.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 28);
// Ridley, Patapata, Botron.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 11);
// Bucyulus, Falconflyer, Ghamgha.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 7);
// Botron, Gyraan.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 22);
// Robodistance, Cataguard.
FT_DOL_ARRAY_VECTOR(soAreaInstance, 12);
// Tautau, Hammer Bro.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 32);
// Galfire, Ghamgha.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 24);
