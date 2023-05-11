#pragma once
#include <context.h>
#include <vulkan/vulkan.hpp>

namespace gfx
{
    vk::ClearValue clear(std::array<float, 4> f32);
}

namespace vma
{
    typedef enum memory_usage
    {
        Unknown = 0,
        GpuOnly = 1,
        CpuOnly = 2,
        CpuToGpu = 3,
        GpuToCpu = 4,
        CpuCopy = 5,
        GpuLazilyAllocated = 6,
        Auto = 7,
        AutoPreferDevice = 8,
        AutoPreferHost = 9,
        Max = 0x7FFFFFFF,
    } memory_usage;

    VmaMemoryUsage to_vma_memory_usage(vma::memory_usage usage);
}