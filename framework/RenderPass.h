#pragma once

#include "Vulkan.h"

class Device;

struct Attachment;

struct SubpassInfo
{
    std::vector<uint32_t> inputAttachments{};

    std::vector<uint32_t> outputAttachments{};

    std::vector<uint32_t> colorResolveAttachments;

    bool disableDepthStencilAttachment;

    uint32_t depthStencilResolveAttachment;

    VkResolveModeFlagBits depthStencilResolveMode;

    std::string debugName;
};


class RenderPass
{
public:
    VkRenderPass getHandle() const
    {
        return _pass;
    }

    //    RenderPass(const ptr<Device>& device,
    //               const std::vector<VkAttachmentDescription>& attachmentDescs,
    //               const std::vector<VkSubpassDependency>& subpassDependencies,
    //               const std::vector<VkSubpassDescription>& subpassDesc);

    //    static std::vector<VkClearValue> getClearValues(const std::vector<Attachment>& attachments);

    RenderPass(Device& device, VkRenderPass pass) : _pass(pass), device(device)
    {
    }

    RenderPass(Device& device,
               const std::vector<Attachment>& attachments,
               const std::vector<SubpassInfo>& subpasses);

    //    RenderPass(const VkRenderPass pass) : _pass(pass) {
    //
    //    }
    const uint32_t getColorOutputCount(uint32_t subpass_index) const;

    ~RenderPass();

protected:
    VkRenderPass _pass;
    Device& device;
    std::vector<uint32_t> colorOutputCount;
};
