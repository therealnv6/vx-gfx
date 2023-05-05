# vx-gfx
A minimal voxel game (engine) written in C++ with Vulkan. Currently still following [vulkan-tutorial](https://vulkan-tutorial.com/), with adjustments to follow a friendlier structure and to use Vulkan-Hpp.

# About
### Plans

- [ ] ECS (maybe [EnTT](https://github.com/skypjack/entt), or make my own)
- [ ] Convert to [glfw.hpp](https://github.com/gnzlbg/glfw/blob/master/include/glfw/glfw.hpp)
- [ ] Support
  - [x] [Windows]()
  - [x] [Linux]()

### Not Planned
- Several graphics backends.
  - This project is focused towards [Vulkan](https://www.vulkan.org/). After considering using something like [NVRHI](https://github.com/NVIDIAGameWorks/nvrhi) or even something like [bgfx](https://github.com/bkaradzic/bgfx), I came to the conclusion I just want to work with Vulkan. Most of the rendering code will *not* be modular, and most of it *will* hook into Vulkan. 
- Uttermost support 
  - I'll try to support most mainstream platforms (not primary focus right now). Please read the "Plans" section for further information on what's planned.

# Building
### Prerequisites 
* C++ compiler compatible with C++20 or higher. 
  * Tested with GCC and MSVC
* [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
* [CMake](https://cmake.org/)
* [spdlog](https://github.com/gabime/spdlog)

### Windows
```PS
git clone --recurse-submodules https://github.com/therealnv6/vx-gfx
cd vx-gfx
mkdir build 
cd build
cmake ../
cmake --build ./ --config Release
```