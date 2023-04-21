#include <context.h>
#include <framework.h>
#include <pipeline.h>

int main()
{
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    try
    {
        gfx::context context(800, 600, "vx-gfx");
        context.device_suitable = [](vk::PhysicalDevice device)
        {
            vk::PhysicalDeviceProperties properties;
            device.getProperties<vk::DispatchLoaderStatic>(&properties);

            vk::PhysicalDeviceFeatures features;
            device.getFeatures<vk::DispatchLoaderStatic>(&features);

            return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.geometryShader;
        };

        gfx::framework framework(context.window);
        gfx::pipeline pipeline { "shaders/triangle.vert.spv",
            "shaders/triangle.frag.spv" };

        framework.run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}