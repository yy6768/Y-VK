#pragma once

#include <string_view>
#include <RenderGraph/RenderGraphId.h>
#include <unordered_map>


class Image;
class ImageView;
class SgImage;

class RenderGraph;


class Blackboard
{
    using Container = std::unordered_map<
        std::string_view,
        RenderGraphHandle>;

public:
    Blackboard(const RenderGraph& graph) noexcept;
    ~Blackboard() noexcept;

    RenderGraphHandle& operator [](std::string_view name) noexcept;

    void put(std::string_view name, RenderGraphHandle handle) noexcept;

    // template <typename T>
    // RenderGraphId<T> get(std::string_view&& name) const noexcept
    // {
    //     return static_cast<RenderGraphId<T>>(getHandle(std::forward<std::string_view>(name)));
    // }

    RenderGraphHandle getHandle(std::string_view name) const noexcept;

    const Image& getImage(std::string_view name) const noexcept;

    const ImageView& getImageView(std::string_view name) const noexcept;

    const SgImage& getHwImage(std::string_view name) const noexcept;

    void remove(std::string_view name) noexcept;

private:
    // RenderGraphHandle getHandle(std::string_view name) const noexcept;
    Container mMap;
    const RenderGraph& graph;
};
