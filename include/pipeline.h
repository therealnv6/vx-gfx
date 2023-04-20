#pragma once

#include <string>
#include <vector>

namespace gfx
{
    std::vector<char> read_file(const std::string &file_path);

    class pipeline
    {
    public:
        pipeline(const std::string &vertex_file_path, const std::string &fragment_file_path);

    private:
        void create_graphics_pipeline(const std::string &vertex_file_path, const std::string &fragment_file_path);
    };
}