// SHADOW of BrawlHeaders/so/sound/so_sound_3d_generator_accesser_impl.h: with FT_MODULE_BUILDER the GeneratorInstance layout is the
// real one (id + generator pointer, 8 bytes) and the DOL helper functions used by the fighter-side accesser are declared.
// SO_SOUND_3D_GENERATOR_ACCESSER_FULL selects the same verified layout for the sora_melee sound units.
#pragma once

#include <StaticAssert.h>
#include <snd/snd_3d_generator.h>
#include <types.h>

class soSound3dGeneratorAccesser {
#ifdef SO_SOUND_3D_GENERATOR_ACCESSER_FULL
// HYPOTHESIS: public in the original (soSoundUtil walks the instances that getInstance returns)
public:
#else
protected:
#endif
#if defined(FT_MODULE_BUILDER) || defined(SO_SOUND_3D_GENERATOR_ACCESSER_FULL)
    struct GeneratorInstance {
        int m_id;
        snd3DGenerator* m_soundGenerator;
        void initialize();
    };
    static void allocateInstance(GeneratorInstance* inst, Vec3f* pos);
    static void freeInstance(GeneratorInstance* inst);
#else
    struct GeneratorInstance {
        int m_0;
        snd3DGenerator m_soundGenerator;
    };
#endif

public:
#if defined(FT_MODULE_BUILDER) || defined(SO_SOUND_3D_GENERATOR_ACCESSER_FULL)
    virtual ~soSound3dGeneratorAccesser() { }
    virtual void activate(Vec3f* pos) = 0;
    virtual void deactivate() = 0;
#else
    virtual ~soSound3dGeneratorAccesser();
    virtual void activate(Vec3f* pos);
    virtual void deactivate();
#endif
#if defined(FT_MODULE_BUILDER) || defined(SO_SOUND_3D_GENERATOR_ACCESSER_FULL)
    virtual GeneratorInstance* getInstance(int idx) = 0;
#else
    virtual void getInstance();
#endif
};

class soSound3dGeneratorAccesserImpl : public soSound3dGeneratorAccesser {
    GeneratorInstance m_generatorInstance;

public:
    soSound3dGeneratorAccesserImpl();
#ifdef SO_BUILDER_HELPERS_WEAK
    virtual ~soSound3dGeneratorAccesserImpl() __attribute__((never_inline)) { } // MATCH-ONLY: a weak copy of the Enemy unit
#else
    virtual ~soSound3dGeneratorAccesserImpl();
#endif
    virtual void activate(Vec3f* pos);
    virtual void deactivate();
    virtual GeneratorInstance* getInstance(int);
};
#if defined(SO_SOUND_3D_GENERATOR_ACCESSER_FULL)
static_assert(sizeof(soSound3dGeneratorAccesserImpl) == 12, "Class is wrong size!");
#endif
#if !defined(FT_MODULE_BUILDER) && !defined(SO_SOUND_3D_GENERATOR_ACCESSER_FULL)
static_assert(sizeof(soSound3dGeneratorAccesserImpl) == 16, "Class is wrong size!");
#endif