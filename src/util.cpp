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