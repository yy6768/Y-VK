#include "Image.h"
#include "Core/Device/Device.h"
#include "ImageUtil.h"
//
//Image::Image(VmaAllocator allocator, VmaMemoryUsage memoryUsage, const VkImageCreateInfo &createInfo) {
//    _allocator = allocator;
//    _format = createInfo.format;
//    _extent = createInfo.extent;
//    _imageType = createInfo.imageType;
//
//    VmaAllocationCreateInfo allocationInfo = {};
//    allocationInfo.usage = memoryUsage;
//    allocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//    ASSERT(vmaCreateImage(_allocator,
//                          &createInfo, &allocationInfo,
//                          &_image, &_allocation, nullptr) == VK_SUCCESS, "Create image");
//}

inline VkImageType find_image_type(VkExtent3D extent) {
    VkImageType result{};

    uint32_t dim_num{0};

    if (extent.width >= 1) {
        dim_num++;
    }

    if (extent.height >= 1) {
        dim_num++;
    }

    if (extent.depth > 1) {
        dim_num++;
    }

    switch (dim_num) {
        case 1:
            result = VK_IMAGE_TYPE_1D;
            break;
        case 2:
            result = VK_IMAGE_TYPE_2D;
            break;
        case 3:
            result = VK_IMAGE_TYPE_3D;
            break;
        default:
            throw std::runtime_error("No image type found.");
            break;
    }

    return result;
}


Image::Image(Device &device, const VkExtent3D &extent, VkFormat format, VkImageUsageFlags image_usage,
             VmaMemoryUsage memory_usage, VkSampleCountFlagBits sample_count, uint32_t mip_levels,
             uint32_t array_layers, VkImageCreateFlags flags) :
        device(device),
        extent{extent},
        type{find_image_type(extent)},
        format{format},
        usage{image_usage},
        sample_count{sample_count},
        array_layer_count{array_layers} {
    assert(mip_levels > 0 && "Image should have at least one level");
    assert(array_layers > 0 && "Image should have at least one layer");


    VkImageCreateInfo image_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_info.flags = flags;
    image_info.imageType = type;
    image_info.format = format;
    image_info.extent = extent;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = array_layers;
    image_info.samples = sample_count;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = image_usage;


    subresource.mipLevel = mip_levels;
    subresource.arrayLayer = array_layers;

    VmaAllocationCreateInfo memory_info{};
    memory_info.usage = memory_usage;

    if (image_usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        memory_info.preferredFlags = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
    }

    VK_CHECK_RESULT(vmaCreateImage(device.getMemoryAllocator(),
                                   &image_info, &memory_info,
                                   &image, &memory,
                                   nullptr));
}

Image::Image(Device &device, VkImage handle, const VkExtent3D &extent, VkFormat format,
             VkImageUsageFlags image_usage, VkSampleCountFlagBits sample_count) : device(device), image(handle),
                                                                                  extent(extent), format(format),
                                                                                  usage(image_usage),
                                                                                  sample_count(sample_count) {
    subresource.mipLevel = 1;
    subresource.arrayLayer = 1;
}

Image::~Image() {
    //只有memory也有意义的时候 这个image才是我们自己分配的 
    if (image != VK_NULL_HANDLE && memory != VK_NULL_HANDLE)
        vmaDestroyImage(device.getMemoryAllocator(), image, memory);
}

Image::Image(Image &&other) : subresource{other.subresource},
                              device(other.device),
                              image(other.image),
                              memory{other.memory},
                              extent{other.extent},
                              type{other.type},
                              format{other.format},
                              usage{other.usage},
                              sample_count{other.sample_count},
                              tiling{other.tiling},
                              mapped_data{other.mapped_data},
                              mapped{other.mapped},
                              views(std::exchange(other.views, {})) {
    other.image = VK_NULL_HANDLE;
}

VkImageSubresource Image::getSubresource() const {
    return subresource;
}

void Image::addView(ImageView *pView) {
}

Device &Image::getDevice() {
    return device;
}

uint32_t Image::getArrayLayerCount() const {
    return array_layer_count;
}

void Image::transitionLayout(CommandBuffer &commandBuffer, VulkanLayout newLayout,
                             const VkImageSubresourceRange & subresourceRange ) {
    auto oldLayout = getLayout(subresourceRange);
    //oldLayout = VulkanLayout::UNDEFINED;
    auto [srcAccessMask, dstAccessMask, srcStage, dstStage, vkOldLayout, vkNewLayout]
            = ImageUtil::getVkTransition(oldLayout, newLayout);


    VkImageMemoryBarrier barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = vkOldLayout,
            .newLayout = vkNewLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = subresourceRange
    };
    vkCmdPipelineBarrier(commandBuffer.getHandle(), srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    
   layouts.insert_or_assign(subresourceRange.baseArrayLayer << 16 | subresourceRange.baseMipLevel, newLayout);
}

void Image::setLayout(VulkanLayout newLayout, const VkImageSubresourceRange & subresourceRange )
{
    layouts.insert_or_assign(subresourceRange.baseArrayLayer << 16 | subresourceRange.baseMipLevel, newLayout);
}

VulkanLayout Image::getLayout(const VkImageSubresourceRange &subresourceRange) {
    uint32_t layoutKey = subresourceRange.baseArrayLayer << 16 | subresourceRange.baseMipLevel;
    return layouts.contains(layoutKey) ? layouts[layoutKey] : VulkanLayout::UNDEFINED;
}