#include "Scene.h"

#include "Compoments/Camera.h"

// Material Material::getDefaultMaterial() {
//     Material material{};
//     return material;
// }
void Scene::IteratePrimitives(PrimitiveCallBack primitiveCallBack) const {
    for (const auto& prim : primitives) {
        primitiveCallBack(*prim);
    }
}

// Scene::Scene(std::vector<std::unique_ptr<Primitive>>&& primitives, std::vector<std::unique_ptr<Texture>>&& textures, std::vector<Material>&& materials, std::vector<SgLight>&& lights, std::vector<std::shared_ptr<Camera>>&& cameras)
//     : materials(std::move(materials)), lights(std::move(lights)), primitives(std::move(primitives)), textures(std::move(textures)), cameras(std::move(cameras)) {
//     for (auto& prim : this->primitives) {
//         assert(prim->valid());
//     }
// }
//
// Scene::Scene(std::vector<std::unique_ptr<Primitive>>&& primitives, std::vector<std::unique_ptr<Texture>>&& textures, std::vector<GltfMaterial>&& materials, std::vector<SgLight>&& lights, std::vector<std::shared_ptr<Camera>>&& cameras)
//     : materials(std::move(materials)), lights(std::move(lights)), primitives(std::move(primitives)), textures(std::move(textures)), cameras(std::move(cameras)) {
//     for (auto& prim : this->primitives) {
//         assert(prim->valid());
//     }
// }

void Scene::addLight(const SgLight& light) {
    lights.emplace_back(light);
}

void Scene::addDirectionalLight(vec3 direction, vec3 color, float intensity) {
    SgLight light{};
    light.lightProperties.color     = color;
    light.lightProperties.intensity = intensity;
    light.lightProperties.direction = direction;
    light.type                      = LIGHT_TYPE::Directional;
    lights.emplace_back(light);
}

const std::vector<SgLight>& Scene::getLights() const {
    return lights;
}

const std::vector<std::unique_ptr<Primitive>>& Scene::getPrimitives() const {
    return primitives;
}

const std::vector<std::unique_ptr<Texture>>& Scene::getTextures() const {
    return textures;
}

const std::vector<GltfMaterial>& Scene::getGltfMaterials() const {
    return materials;
}
const std::vector<RTMaterial>& Scene::getRTMaterials() const {
    return rtMaterials;
}

std::vector<std::shared_ptr<Camera>>& Scene::getCameras() {
    return cameras;
}
bool Scene::hasVertexBuffer(const std::string& name) const {
    return sceneVertexBuffer.contains(name);
}
bool Scene::getVertexAttribute(const std::string& name, VertexAttribute* attribute) const {
    if (vertexAttributes.contains(name)) {
        if (attribute)
            *attribute = vertexAttributes.at(name);
        return true;
    }
    return false;
}
Buffer& Scene::getVertexBuffer(const std::string& name) const {
    return *sceneVertexBuffer.at(name);
}
VkIndexType Scene::getIndexType() const {
    return indexType;
}
Buffer& Scene::getIndexBuffer() const {
    return *sceneIndexBuffer;
}
Buffer& Scene::getUniformBuffer() const {
    return *sceneUniformBuffer;
}
Buffer& Scene::getPrimitiveIdBuffer() const {
    return *primitiveIdBuffer;
}
bool Scene::usePrimitiveIdBuffer() const {
    return usePrimitiveId;
}
VkPrimitiveTopology GetVkPrimitiveTopology(PRIMITIVE_TYPE type) {
    switch (type) {
        case PRIMITIVE_TYPE::E_TRIANGLE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PRIMITIVE_TYPE::E_POINT_LIST:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        default:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}
bool Primitive::getVertexAttribute(const std::string& name, VertexAttribute* attribute) const {
    if (vertexAttributes.contains(name)) {
        if (attribute)
            *attribute = vertexAttributes.at(name);
        return true;
    }
    return false;
}

void Primitive::setVertxAttribute(const std::string& name, const VertexAttribute& attribute) {
    vertexAttributes.emplace(name, attribute);
}

void Primitive::setVertexBuffer(const std::string& name, std::unique_ptr<Buffer>& buffer) {
    vertexBuffers.emplace(name, std::move(buffer));
}
void Primitive::setUniformBuffer(std::unique_ptr<Buffer>& buffer) {
    this->uniformBuffer = std::move(buffer);
}
void Primitive::setIndexBuffer(std::unique_ptr<Buffer>& buffer) {
    this->indexBuffer = std::move(buffer);
}
bool Primitive::valid() const {
    return uniformBuffer != nullptr && indexBuffer != nullptr && !vertexBuffers.empty();
}

Buffer& Primitive::getVertexBuffer(const std::string& name) const {
    if (vertexBuffers.contains(name))
        return *vertexBuffers.at(name);
    LOGE("Primitive has no such buffer {}", name);
}
VkIndexType Primitive::getIndexType() const {
    return indexType;
}

void Primitive::setIndexType(VkIndexType indexType) {
    this->indexType = indexType;
}
bool Primitive::hasVertexBuffer(const std::string& name) const {
    return vertexBuffers.contains(name);
}

Buffer& Primitive::getIndexBuffer() const {
    return *indexBuffer;
}
bool Primitive::hasIndexBuffer() const {
    return indexBuffer != nullptr;
}

Buffer& Primitive::getUniformBuffer() const {
    return *uniformBuffer;
}

std::unique_ptr<Scene> loadDefaultTriangleScene(Device& device) {
    GltfMaterial                            mat{};
    std::vector<std::unique_ptr<Primitive>> primitives = {};
    primitives.push_back(std::make_unique<Primitive>(0, 0, 3, 3, 0));

    const uint32_t bufferSize      = sizeof(float) * 3 * 3;
    const uint32_t indexBufferSize = sizeof(uint32_t) * 3;

    std::vector<glm::vec3> position = {{1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
    std::vector<glm::vec3> color    = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    std::vector<uint32_t>  index    = {0, 1, 2};
    PerPrimitiveUniform    uniform  = {.model = glm::mat4(1.0f)};

    auto positionBuffer = std::make_unique<Buffer>(device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    auto colorBuffer    = std::make_unique<Buffer>(device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    auto indexBuffer    = std::make_unique<Buffer>(device, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    auto uniformBuffer  = std::make_unique<Buffer>(device, sizeof(uniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    positionBuffer->uploadData(position.data());
    colorBuffer->uploadData(color.data());
    indexBuffer->uploadData(index.data());
    uniformBuffer->uploadData(&uniform);

    auto& prim = primitives[0];
    prim->setIndexBuffer(indexBuffer);
    prim->setVertexBuffer(POSITION_ATTRIBUTE_NAME, positionBuffer);
    prim->setVertexBuffer("color", colorBuffer);
    prim->setUniformBuffer(uniformBuffer);
    prim->setVertxAttribute(POSITION_ATTRIBUTE_NAME, VertexAttribute{.format = VK_FORMAT_R32G32B32_SFLOAT, .stride = sizeof(float) * 3, .offset = 0});
    prim->setVertxAttribute("color", VertexAttribute{.format = VK_FORMAT_R32G32B32_SFLOAT, .stride = sizeof(float) * 3, .offset = 0});
    prim->setIndexType(VK_INDEX_TYPE_UINT32);
    // auto scene = new Scene(std::move(primitives), {}, {mat}, {}, {});
    auto scene = new Scene();
    return std::unique_ptr<Scene>(scene);
}
GltfMaterial InitGltfMaterial() {
    GltfMaterial material{};
    material.pbrBaseColorFactor          = glm::vec4(1.0f);
    material.pbrBaseColorTexture         = -1;
    material.pbrMetallicFactor           = 0.f;
    material.pbrRoughnessFactor          = 0.f;
    material.pbrMetallicRoughnessTexture = -1;
    material.emissiveTexture             = -1;
    material.emissiveFactor              = glm::vec3(0.0f);
    material.normalTexture               = -1;
    material.occlusionTexture            = -1;
    material.alphaMode                   = 0;
    material.alphaCutoff                 = 0.5f;
    material.doubleSided                 = false;
    material.occlusionTextureStrength    = 1.0f;
    material.normalTextureScale          = 1.0f;
    return material;
}
