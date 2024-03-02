// Created by pc on 2023/11/29.
//

#ifndef VULKANDEMO_RENDERPRIMITIVE_H
#define VULKANDEMO_RENDERPRIMITIVE_H
#include "Core/BoundingBox.h"
#include "Core/Buffer.h"

class Material;

enum class PRIMITIVE_TYPE : uint8_t {
    E_TRIANGLE_LIST,
};

struct VertexAttribute {
    VkFormat      format = VK_FORMAT_UNDEFINED;
    std::uint32_t stride = 0;
    std::uint32_t offset = 0;
};

#define POSITION_ATTRIBUTE_NAME "position"
#define INDEX_ATTRIBUTE_NAME    "indices"
#define NORMAL_ATTRIBUTE_NAME   "normal"
#define TANGENT_ATTRIBUTE_NAME  "tangent"
#define TEXCOORD_ATTRIBUTE_NAME "texcoord_0"

struct PerPrimitiveUniform {
    glm::mat4 model;
};

class Primitive {
protected:
    //ShaderVarint varint{};

    PRIMITIVE_TYPE primitiveType{PRIMITIVE_TYPE::E_TRIANGLE_LIST};

    VkIndexType indexType{VK_INDEX_TYPE_UINT16};

    BBox dimensions;

    std::unordered_map<std::string, VertexAttribute>         vertexAttributes;
    std::unordered_map<std::string, std::unique_ptr<Buffer>> vertexBuffers;
    std::unique_ptr<Buffer>                                  indexBuffer;
    std::unique_ptr<Buffer>                                  uniformBuffer;

public:
    uint32_t  firstIndex{0};
    uint32_t  indexCount{};
    uint32_t  firstVertex{0};
    uint32_t  vertexCount{};
    glm::mat4 matrix{};
    uint32_t  materialIndex{0};

    bool          getVertexAttribute(const std::string& name, VertexAttribute& attribute) const;
    void          setVertxAttribute(const std::string& name, const VertexAttribute& attribute);
    void          setVertexBuffer(const std::string& name, std::unique_ptr<Buffer>& buffer);
    void          setUniformBuffer(std::unique_ptr<Buffer>& buffer);
    void          setIndexBuffer(std::unique_ptr<Buffer>& buffer);
    bool          valid() const;
    Buffer&       getVertexBuffer(const std::string& name) const;
    VkIndexType   getIndexType() const;
    void          setIndexType(VkIndexType indexType);
    bool          hasVertexBuffer(const std::string& name) const;
    const Buffer& getIndexBuffer() const;
    const Buffer& getUniformBuffer() const;

    void        setDimensions(glm::vec3 min, glm::vec3 max);
    const BBox& getDimensions() const;

    Primitive(uint32_t firstVertex, uint32_t firstIndex, uint32_t indexCount, uint32_t materialIndex) : firstIndex(firstIndex),
                                                                                                        indexCount(indexCount),
                                                                                                        materialIndex(materialIndex), dimensions({}) {
    }
};

#endif//VULKANDEMO_RENDERPRIMITIVE_H