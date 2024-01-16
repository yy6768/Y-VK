#pragma once

#include "PassNode.h"
#include "VirtualResource.h"
#include "Core/RenderTarget.h"
#include "RenderGraphId.h"
#include "RenderGraphPass.h"
#include "RenderGraphTexture.h"
#include "Core/RenderPass.h"
#include "BlackBoard.h"
#include "RenderGraphBuffer.h"
/**render Graph **/
/*
 * 1.add pass 需要指明pass的输入 注册RenderGraphTexture 这一步会添加passNode 这里node会根据传入的RenderGraphPass::Descriptor创建RenderTarget
 * 2.指明pass node在execute时需要执行的东西 renderpass的创建根据node
 * 
 *
 *
 *
 */

// class RenderGraphPassBase;
// class RenderGraph;

class CommandBuffer;

class RenderGraph {
public:
    RenderGraph(Device& device);

    RenderGraph(RenderGraph& rhs) = delete;

    RenderGraph(RenderGraph&& rhs) = delete;

    class Builder {
    public:
        RenderGraphHandle readTexture(RenderGraphHandle         input,
                                      RenderGraphTexture::Usage usage =
                                          RenderGraphTexture::Usage::NONE);

        RenderGraphHandle writeTexture(RenderGraphHandle         output,
                                       RenderGraphTexture::Usage usage =
                                           RenderGraphTexture::Usage::NONE);

        RenderGraphHandle readBuffer(RenderGraphHandle        input,
                                     RenderGraphBuffer::Usage usage =
                                         RenderGraphBuffer::Usage::NONE);

        RenderGraphHandle writeBuffer(RenderGraphHandle        output,
                                      RenderGraphBuffer::Usage usage =
                                          RenderGraphBuffer::Usage::NONE);

        // void addSubpass(const RenderGraphSubpassInfo&);

        void declare(const char*                      name,
                     const RenderGraphPassDescriptor& desc);

        Builder(PassNode* node, RenderGraph& renderGraph)
            : node(node),
              renderGraph(renderGraph) {
        }

    protected:
        PassNode*    node;
        RenderGraph& renderGraph;
    };

    RenderGraphHandle createTexture(const char* name, const RenderGraphTexture::Descriptor& desc = {});
    RenderGraphHandle importTexture(const char* name, SgImage* hwTexture, bool addRef = true);

    RenderGraphHandle createBuffer(const char* name, const RenderGraphBuffer::Descriptor& desc = {});
    RenderGraphHandle importBuffer(const char* name, Buffer* hwBuffer);

    ResourceNode*       getResource(RenderGraphHandle handle) const;
    RenderGraphTexture* getTexture(RenderGraphHandle handle) const;
    RenderGraphBuffer*  getBuffer(RenderGraphHandle handle) const;

    bool isWrite(RenderGraphHandle handle, const RenderPassNode* passNode) const;
    bool isRead(RenderGraphHandle handle, const RenderPassNode* passNode) const;

    void setUp();

    void execute(CommandBuffer& commandBuffer);

    Blackboard& getBlackBoard() const;

    using GraphicSetup    = std::function<void(Builder& builder, GraphicPassSettings&)>;
    using ComputeSetUp    = std::function<void(Builder& builder, ComputePassSettings&)>;
    using RayTracingSetup = std::function<void(Builder& builder, RaytracingPassSettings&)>;

    // template<typename PassSettings>
    // using Setup = std::function<void(Builder & builder,PassSettings &)>;

    // void addGraphicPass(const char * name,const GraphicSetup &  setup,GraphicsExecute && execute)
    // {
    //     addPass<GraphicPassSettings>(name,setup,std::forward<GraphicsExecute>(execute));
    // }
    //
    // void addComputePass(const char * name,ComputeSetUp & setup,ComputeExecute && execute)
    // {
    //     addPass<ComputePassSettings>(name,setup,std::forward<ComputeExecute>(execute));
    //
    // }
    //
    // void addRayTracingPass(const char * name,RayTracingSetup & setup,RaytracingExecute && execute)
    // {
    //     addPass<RaytracingPassSettings>(name,setup,std::forward<RaytracingExecute>(execute));
    // }

    // template<typename Data, typename Setup, typename Execute>
    // void addPass(const char *name, const Setup & setup, Execute &&execute) {
    //     auto pass = new RenderGraphPass<Data, Execute>(std::forward<Execute>(execute));
    //     PassNode * node;
    //     switch (pass->getData().type)
    //     {
    //         case(RENDER_GRAPH_PASS_TYPE::GRAPHICS):
    //         {
    //              node = new RenderPassNode(*this, name, pass);
    //                 break;
    //         }
    //         case(RENDER_GRAPH_PASS_TYPE::COMPUTE):
    //         {
    //             node = new ComputePassNode(*this, name, pass);
    //             break;
    //         }
    //         case(RENDER_GRAPH_PASS_TYPE::RAYTRACING):
    //         {
    //             node = new RayTracingPassNode(*this, name, pass);
    //             break;
    //         }
    //     }
    //
    //     mPassNodes.emplace_back(node);
    //     Builder builder(node, *this);
    //     setup(builder,pass->getData());
    // }

    // template< typename Setup, typename Execute>
    void addPass(const char* name, const GraphicSetup& setup, GraphicsExecute&& execute);

    void addComputePass(const char* name, const ComputeSetUp& setup, ComputeExecute&& execute);
    void addRaytracingPass(const char* name, const RayTracingSetup& setup, RaytracingExecute&& execute);
    void addImageCopyPass(RenderGraphHandle src, RenderGraphHandle dst);

    // void addPresentPass(RenderGraphHandle textureId);

    void compile();
    void clearPass();

    Device& getDevice() const;

    ~RenderGraph() {
        for (const auto& passNode : mPassNodes)
            delete passNode;
    }

private:
    RenderGraphHandle addTexture(RenderGraphTexture* texture);
    RenderGraphHandle addBuffer(RenderGraphBuffer* buffer);

    std::vector<RenderGraphNode*> getInComingNodes(RenderGraphNode* node) const;
    std::vector<RenderGraphNode*> getOutComingNodes(RenderGraphNode* node) const;

    PassNode* addPassImpl(const char* name, RenderGraphPassBase* base) {
        auto node = new RenderPassNode(*this, name, base);
        mPassNodes.emplace_back(node);
        return node;
    }

    struct ResourceSlot {
        using Version   = RenderGraphHandle::Version;
        using Index     = int16_t;
        Index   rid     = 0; // VirtualResource* index in mResources
        Index   nid     = 0; // ResourceNode* index in mResourceNodes
        Index   sid     = -1;// ResourceNode* index in mResourceNodes for reading subresource's parent
        Version version = 0;
    };

    // Using union instead of directly use two type of struct may be better;
    // Using texture type and buffer type to avoid call get type function at runtime
    // Pointer cast is not safe and ugly.

    // union
    // {
    //     RenderGraphTexture *texture{nullptr};
    //     RenderGraphBuffer *buffer{nullptr};
    // };
    // union
    // {
    //     RenderGraphTexture::Usage textureUsage;
    //     RenderGraphBuffer::Usage bufferUsage;
    // };

    // struct TextureEdge {
    //     PassNode *pass{nullptr};
    //     RenderGraphTexture *texture{nullptr};
    //     RenderGraphTexture::Usage usage{};
    //     bool read{true};
    // };

    struct Edge {
        PassNode*     pass{nullptr};
        ResourceNode* resource{nullptr};
        uint16_t      usage{};
        bool          read{true};

        //For Given Node ,whether this edge is incoming or outgoing
        bool inComing(const ResourceNode* resourceNode) const { return resource == resourceNode && !read; }
        bool outComing(const ResourceNode* resourceNode) const { return resource == resourceNode && read; }
        bool inComing(const PassNode* passNode) const { return pass == passNode && read; }
        bool outComing(const PassNode* passNode) const { return pass == passNode && !read; }
    };

    std::vector<const Edge*> getEdges(RenderGraphNode* node) const;

    std::unique_ptr<Blackboard> mBlackBoard{};
    std::vector<PassNode*>      mPassNodes{};
    std::vector<ResourceNode*>  mResources{};

    std::vector<PassNode*>::iterator mActivePassNodesEnd;

    std::vector<ResourceSlot> mResourceSlots{};
    Device&                   device;

    std::vector<Edge> edges;

    // std::vector<std::unique_ptr<Vi>>
};