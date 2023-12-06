//
// Created by pc on 2023/8/18.
//

#include "SgImage.h"
#include "Core/Images/Image.h"

#include "Core/Images/ImageView.h"

#include "Core/Device/Device.h"
#include "Common/FIleUtils.h"


// #include <ext/ktx/lib/vk_format.h>
#include <ktx.h>
#include <stb_image.h>

#include "Images/AstcImageHelper.h"
#include "Gui/Gui.h"
#include "Images/KtxFormat.h"

bool isAstc(const VkFormat format) {
    return (format == VK_FORMAT_ASTC_4x4_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_4x4_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_5x4_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_5x4_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_5x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_5x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_6x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_6x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_6x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_6x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_8x8_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x5_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x5_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x6_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x6_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x8_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x8_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_10x10_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_10x10_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_12x10_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_12x10_SRGB_BLOCK ||
            format == VK_FORMAT_ASTC_12x12_UNORM_BLOCK ||
            format == VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
}

void SgImage::createVkImage(Device &device, VkImageViewType image_view_type, VkImageCreateFlags flags) {
    assert(vkImage == nullptr && "Image has been created");
    assert(vkImageView == nullptr && "ImageView has been created");
    vkImage = std::make_unique<Image>(device, extent3D, format,
                                      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                      VMA_MEMORY_USAGE_GPU_ONLY, VK_SAMPLE_COUNT_1_BIT, toUint32(mipMaps.size()),
                                      layers, flags);
    vkImageView = std::make_unique<ImageView>(*vkImage, image_view_type);
}

SgImage::SgImage(Device &device, const std::string &path, VkImageViewType viewType) : device(device) {
    loadResources(path);
    createVkImage(device, viewType);
}

SgImage::~SgImage() {
    // if(vkImage)delete vkImage.get();
    // if(vkImageView) delete vkImageView.get();
}

SgImage::SgImage(SgImage &&other) : vkImage(std::move(other.vkImage)), vkImageView(std::move(other.vkImageView)),
                                    data(std::move(other.data)),
                                    format(other.format), extent3D(other.extent3D),
                                    mipMaps(std::move(other.mipMaps)), offsets(std::move(other.offsets)),
                                    device(other.device), layers(other.layers) {
    other.vkImage = nullptr;
    other.vkImageView = nullptr;
}

SgImage::SgImage(Device &device, const std::string &name, const VkExtent3D &extent, VkFormat format,
                 VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage, VkImageViewType viewType,
                 VkSampleCountFlagBits sample_count, uint32_t mip_levels, uint32_t array_layers,
                 VkImageCreateFlags flags) : format(format), extent3D(extent), name(name), device(device) {
    vkImage = std::make_unique<Image>(device, extent, format, image_usage, memory_usage, sample_count, mip_levels,
                                      array_layers, flags);
    vkImageView = std::make_unique<ImageView>(*vkImage, viewType);

    this->format = format;
    this->extent3D = extent;
}

uint64_t SgImage::getBufferSize() const {
    return data.size();
}

std::vector<uint8_t> &SgImage::getData() {
    return data;
}


VkExtent3D SgImage::getExtent() const {
    return extent3D;
}

VkExtent2D SgImage::getExtent2D() const {
    return {extent3D.width, extent3D.height};
}

Image &SgImage::getVkImage() const {
    return *vkImage;
}

ImageView &SgImage::getVkImageView() const {
    return *vkImageView;
}

VkFormat SgImage::getFormat() const {
    return format;
}

const std::vector<Mipmap> &SgImage::getMipMaps() const {
    return mipMaps;
}

const std::vector<std::vector<VkDeviceSize>> &SgImage::getOffsets() const {
    return offsets;
}

uint32_t SgImage::getLayers() const {
    return layers;
}

void SgImage::setLayers(uint32_t layers) {
    SgImage::layers = layers;
}

void SgImage::generateMipMap() {
    assert(mipMaps.size() == 1 && "Mipmaps already generated");

    auto extent = getExtent();
    auto next_width = std::max<uint32_t>(1u, extent.width / 2);
    auto next_height = std::max<uint32_t>(1u, extent.height / 2);
    auto channels = 4;
    auto next_size = next_width * next_height * channels;

    while (true) {
        // Make space for next mipmap
        auto old_size = toUint32(data.size());
        data.resize(old_size + next_size);

        auto &prev_mipmap = mipMaps.back();
        // Update mipmaps
        Mipmap next_mipmap{};
        next_mipmap.level = prev_mipmap.level + 1;
        next_mipmap.offset = old_size;
        next_mipmap.extent = {next_width, next_height, 1u};

        //todo

        // std::copy(data.begin()+ next_mipmap.offset,data.begin()+prev_mipmap.offset,data.begin()+pr

        // Fill next mipmap memory
        //            stbir_resize_uint8(data.data() + prev_mipmap.offset, prev_mipmap.extent.width, prev_mipmap.extent.height, 0,
        //                               data.data() + next_mipmap.offset, next_mipmap.extent.width, next_mipmap.extent.height, 0,
        //                               channels);

        mipMaps.emplace_back(std::move(next_mipmap));

        // Next mipmap values
        next_width = std::max<uint32_t>(1u, next_width / 2);
        next_height = std::max<uint32_t>(1u, next_height / 2);
        next_size = next_width * next_height * channels;

        if (next_width == 1 && next_height == 1) {
            break;
        }
    }
}


void SgImage::loadResources(const std::string &path) {
    auto ext = FileUtils::getFileExt(path);
    if (ext == "jpg" || ext == "png") {
        int w, h, comp;
        auto rawData = stbi_load(path.c_str(), &w, &h, &comp, 4);
        comp = 4;
        data = {rawData, rawData + w * h * comp};
        stbi_image_free(rawData);

        extent3D = {toUint32(w), toUint32(h), 1};
        format = VK_FORMAT_R8G8B8A8_SRGB;
    } else if (ext == "ttf") {
        assert(FileUtils::getFileExt(path) == "ttf");
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(path.c_str(), 16.f);

        unsigned char *fontData;
        int texWidth, texHeight;
        io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

        data = std::vector<uint8_t>(fontData, fontData + texHeight * texWidth * 4);
        format = VK_FORMAT_R8G8B8A8_UNORM;
        extent3D = {toUint32(texWidth), toUint32(texHeight), 1};
    } else if (ext == "ktx") {
        ktxTexture *ktxTexture;
        ktxResult result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                          &ktxTexture);
        assert(result == KTX_SUCCESS);
        this->extent3D = VkExtent3D{ktxTexture->baseWidth, ktxTexture->baseHeight, 1};

        ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
        ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

        mipMaps.resize(ktxTexture->numLevels);
        layers = ktxTexture->numLayers;

        ktx_size_t pixelsMulChannels;
        result = ktxTexture_GetImageOffset(ktxTexture, 0, 0, 0, &pixelsMulChannels);

        data = {ktxTextureData, ktxTextureData + ktxTextureSize};


        format = GetFormatFromOpenGLInternalFormat(ktxTexture->glInternalformat);

        if (layers > 1) {
            for (uint32_t layer = 0; layer < layers; layer++) {
                std::vector<VkDeviceSize> layerOffsets{};
                for (uint32_t level = 0; level < mipMaps.size(); level++) {
                    ktx_size_t offset;

                    result = ktxTexture_GetImageOffset(ktxTexture, level, layer, 0, &offset);
                    assert(result == KTX_SUCCESS);
                    layerOffsets.push_back(static_cast<VkDeviceSize>(offset));
                }
                offsets.push_back(layerOffsets);
            }
        }
    }

    if (isAstc(format)) {
        if (!device.isImageFormatSupported(format)) {
            LOGW("ASTC not supported: decoding {}", name);
            AstcImageHelper::decodeAstcImage(*this);
        }
    }
}


SgImage::SgImage(Device &device, VkImage handle, const VkExtent3D &extent, VkFormat format,
                 VkImageUsageFlags image_usage, VkSampleCountFlagBits sample_count,
                 VkImageViewType viewType) : device(device) {
    vkImage = std::make_unique<Image>(device, handle, extent, format, image_usage, sample_count);
    vkImageView = std::make_unique<ImageView>(*vkImage, viewType);
    this->format = format;
    this->extent3D = extent;
}