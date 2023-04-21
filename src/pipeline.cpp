#include <fstream>
#include <iostream>
#include <pipeline.h>


namespace gfx
{
    pipeline::pipeline(const std::string &vertex_file_path, const std::string &fragment_file_path)
    {
        this->create_graphics_pipeline(vertex_file_path, fragment_file_path);
    }

    std::vector<char> read_file(const std::string &file_path)
    {
        std::ifstream file { file_path, std::ios::ate | std::ios::binary };

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file: " + file_path);
        }

        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }

    void pipeline::create_graphics_pipeline(const std::string &vertex_file_path, const std::string &fragment_file_path)
    {
        auto vertex_code = gfx::read_file(vertex_file_path);
        auto fragment_code = gfx::read_file(fragment_file_path);

        std::cout << "Vertex Shader Code Size: " << vertex_code.size() << std::endl;
        std::cout << "Fragment Shader Code Size: " << fragment_code.size() << std::endl;
    }
}