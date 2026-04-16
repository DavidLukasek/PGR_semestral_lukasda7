#include <unordered_map>

#include "renderableObject.h"

namespace {
struct MaterialCacheEntry {
    Material material{glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), 0.0f};
    bool valid = false;
};

bool areMaterialsEqual(const Material& lhs, const Material& rhs) {
    return lhs.ambient.x == rhs.ambient.x &&
           lhs.ambient.y == rhs.ambient.y &&
           lhs.ambient.z == rhs.ambient.z &&
           lhs.diffuse.x == rhs.diffuse.x &&
           lhs.diffuse.y == rhs.diffuse.y &&
           lhs.diffuse.z == rhs.diffuse.z &&
           lhs.specular.x == rhs.specular.x &&
           lhs.specular.y == rhs.specular.y &&
           lhs.specular.z == rhs.specular.z &&
           lhs.shininess == rhs.shininess;
}
}

void RenderableObject::applyMaterialUniforms() const {
    if ((shaderProgram == nullptr) || !hasMaterial)
        return;

    static std::unordered_map<GLuint, MaterialCacheEntry> materialCache;
    MaterialCacheEntry& cacheEntry = materialCache[shaderProgram->program];

    if (cacheEntry.valid && areMaterialsEqual(cacheEntry.material, material))
        return;

    if (shaderProgram->locations.diffuse != -1)
        glUniform3fv(shaderProgram->locations.diffuse, 1, glm::value_ptr(material.diffuse));

    if (shaderProgram->locations.specular != -1)
        glUniform3fv(shaderProgram->locations.specular, 1, glm::value_ptr(material.specular));

    if (shaderProgram->locations.ambient != -1)
        glUniform3fv(shaderProgram->locations.ambient, 1, glm::value_ptr(material.ambient));

    if (shaderProgram->locations.shininess != -1)
        glUniform1f(shaderProgram->locations.shininess, material.shininess);

    cacheEntry.material = material;
    cacheEntry.valid = true;
}
