#include "Shader.h"
#include <Device.h>

#include <fstream>
#include <FIleUtils.h>
#include <GlslCompiler.h>

VkShaderStageFlagBits getShaderStage(const std::string &ext) {
    if (ext == "vert") {
        return VK_SHADER_STAGE_VERTEX_BIT;
    } else if (ext == "frag") {
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if (ext == "comp") {
        return VK_SHADER_STAGE_COMPUTE_BIT;
    } else if (ext == "geom") {
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    } else if (ext == "tesc") {
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    } else if (ext == "tese") {
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    } else if (ext == "rgen") {
        return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    } else if (ext == "rahit") {
        return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    } else if (ext == "rchit") {
        return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    } else if (ext == "rmiss") {
        return VK_SHADER_STAGE_MISS_BIT_KHR;
    } else if (ext == "rint") {
        return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    } else if (ext == "rcall") {
        return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    } else if (ext == "mesh") {
        return VK_SHADER_STAGE_MESH_BIT_EXT;
    } else if (ext == "task") {
        return VK_SHADER_STAGE_TASK_BIT_EXT;
    }

    throw std::runtime_error("File extension `" + ext + "` does not have a vulkan shader stage.");
}
// static std::vector<char> readFile(const std::string &filename)
// {

//     file.close();
//     return buffer;
// }

bool Shader::initFromSpv() {
    return false;
}

bool Shader::initFromOriginShader() {
    return false;
}

Shader::Shader(Device &device, const std::string &path, SHADER_LOAD_MODE mode) {

    if (mode == SHADER_LOAD_MODE::SPV) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t)
                file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());
        createInfo.pNext = nullptr;
        VK_CHECK_RESULT(vkCreateShaderModule(device.getHandle(), &createInfo, nullptr, &shader));

    } else {
        std::vector<uint32_t> spirvCode;
        std::string shaderLog;

        auto shaderBuffer = FileUtils::readShaderBinary(path);

        if (!GlslCompiler::compileToSpirv(getShaderStage(FileUtils::getFileExt(path)), shaderBuffer, "main", spirvCode,
                                          shaderLog)) {
            LOGE("Failed to compile shader, Error: {}", shaderLog.c_str());
            exit(-1);
        }

    }

    // create shader

}

Shader::~Shader() {
}

VkPipelineShaderStageCreateInfo Shader::PipelineShaderStageCreateInfo(VkShaderStageFlagBits shaderStage) {
    VkPipelineShaderStageCreateInfo info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    info.stage = shaderStage;
    info.module = shader;
    info.pName = "main";
    return info;
}