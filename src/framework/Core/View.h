#pragma once
#include "Buffer.h"
// #include "RenderContext.h"
#include "Scene/Compoments/Camera.h"
#include "Scene/Compoments/RenderPrimitive.h"

class CommandBuffer;
struct GltfMaterial;
class Scene;
class Texture;

class View {
public:
    View(Device& device);
    void  setScene(const Scene* scene);
    void  setCamera(const Camera* camera);
    View& bindViewBuffer();
    View& bindViewShading();
    View& bindViewGeom(CommandBuffer& commandBuffer);

    void drawPrimitives(CommandBuffer& commandBuffer);

    using PrimitiveSelectFunc = std::function<bool(const Primitive& primitive)>;
    void drawPrimitives(CommandBuffer& commandBuffer, const PrimitiveSelectFunc& selectFunc);

    const Camera*                 getCamera() const;
    std::vector<const Primitive*> getMVisiblePrimitives() const;
    void                          setMVisiblePrimitives(const std::vector<const Primitive*>& mVisiblePrimitives);

    std::vector<const Texture*> GetMTextures() const;
    std::vector<GltfMaterial>   GetMMaterials() const;

protected:
    struct PerViewUnifom {
        glm::mat4 view_proj;
        glm::mat4 inv_view_proj;
        glm::mat4 proj;
        glm::mat4 view;

        glm::vec3 camera_pos;
        uint32_t  light_count;

        glm::ivec2 resolution;
        glm::ivec2 inv_resolution;

        bool operator!=(const PerViewUnifom& other) const {
            return memcmp(this, &other, sizeof(PerViewUnifom)) != 0;
        }
    };

    const Scene* mScene{nullptr};

protected:
    const Camera*                 mCamera{nullptr};
    std::vector<const Primitive*> mVisiblePrimitives;
    std::vector<const Texture*>   mTextures;
    std::vector<GltfMaterial>     mMaterials;
    std::unique_ptr<Buffer>       mPerViewBuffer;
    const Scene*                  scene;
    PerViewUnifom                 mPerViewUniform;
    std::unique_ptr<Buffer>       mLightBuffer;
};