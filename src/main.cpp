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

        context.choose_present_mode = [](const gfx::present_modes)
        {
            return gfx::present_mode::eImmediate;
        };

        context.device_suitable = [](vk::PhysicalDevice device)
        {
            vk::PhysicalDeviceProperties properties = device.getProperties<vk::DispatchLoaderStatic>();
            vk::PhysicalDeviceFeatures features = device.getFeatures<vk::DispatchLoaderStatic>();

            std::vector<vk::ExtensionProperties> available_extensions = device.enumerateDeviceExtensionProperties();
            std::set<std::string> required_extensions(gfx::device_extensions.begin(), gfx::device_extensions.end());

            for (const auto &extension : available_extensions)
            {
                required_extensions.erase(extension.extensionName);
            }

            return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
                && features.geometryShader
                && required_extensions.empty();
        };

        gfx::framework framework(context.window);
        gfx::pipeline pipeline { context, "triangle.vert.spv",
            "triangle.frag.spv" };

        framework.run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}