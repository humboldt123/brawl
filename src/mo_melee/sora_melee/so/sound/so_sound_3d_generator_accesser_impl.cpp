#define SO_SOUND_3D_GENERATOR_ACCESSER_FULL
#include <mt/mt_vector.h>
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <so/sound/so_sound_manager.h>
#include <types.h>

void soSound3dGeneratorAccesser::GeneratorInstance::initialize() {
    m_id = -1;
    m_soundGenerator = NULL;
}

void soSound3dGeneratorAccesser::allocateInstance(GeneratorInstance* inst, Vec3f* pos) {
    if (inst->m_id == -1) {
        int id = lbl_27_bss_D0.allocateGenerator();
        if (id != -1) {
            inst->m_soundGenerator = lbl_27_bss_D0.getGenerator(id);
            inst->m_id = id;
            Vec3f p = *pos;
            inst->m_soundGenerator->setPos(&p);
        }
    }
}

void soSound3dGeneratorAccesser::freeInstance(GeneratorInstance* inst) {
    if (inst->m_id != -1) {
        lbl_27_bss_D0.freeGenerator(inst->m_id);
        inst->initialize();
    }
}

soSound3dGeneratorAccesserImpl::soSound3dGeneratorAccesserImpl() {
    m_generatorInstance.initialize();
}

void soSound3dGeneratorAccesserImpl::activate(Vec3f* pos) {
    allocateInstance(&m_generatorInstance, pos);
}

void soSound3dGeneratorAccesserImpl::deactivate() {
    freeInstance(&m_generatorInstance);
}

soSound3dGeneratorAccesser::GeneratorInstance* soSound3dGeneratorAccesserImpl::getInstance(int) {
    return &m_generatorInstance;
}

soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
