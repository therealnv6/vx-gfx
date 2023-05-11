#include <util.h>

namespace gfx
{
    vk::ClearValue clear(std::array<float, 4> f32)
    {
        return vk::ClearValue {
            f32
        };
    }
}

namespace vma
{
    // clang-format off
    VmaMemoryUsage to_vma_memory_usage(vma::memory_usage usage)
    {
        switch (usage)
        {
            case vma::Unknown: return VMA_MEMORY_USAGE_UNKNOWN;
            case vma::GpuOnly: return VMA_MEMORY_USAGE_GPU_ONLY;
            case vma::CpuOnly: return VMA_MEMORY_USAGE_CPU_ONLY;
            case vma::CpuToGpu: return VMA_MEMORY_USAGE_CPU_TO_GPU;
            case vma::GpuToCpu: return VMA_MEMORY_USAGE_GPU_TO_CPU;
            case vma::CpuCopy: return VMA_MEMORY_USAGE_CPU_COPY;
            case vma::GpuLazilyAllocated: return VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
            case vma::Auto: return VMA_MEMORY_USAGE_UNKNOWN; // Use VMA's default
            case vma::AutoPreferDevice: return VMA_MEMORY_USAGE_GPU_ONLY; // Use VMA's default
            case vma::AutoPreferHost: return VMA_MEMORY_USAGE_CPU_ONLY; // Use VMA's default
            default: return VMA_MEMORY_USAGE_UNKNOWN;
        }
    }
    // clang-format on
}