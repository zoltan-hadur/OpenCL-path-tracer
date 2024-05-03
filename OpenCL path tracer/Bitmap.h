#pragma once

#include <vector>
#include <filesystem>

namespace OpenCL_PathTracer
{
    // Stores BGR bitmap image.
    class Bitmap
    {
    private:
        static const int _headerSize = 54;
        int _width = 0;
        int _height = 0;
        std::vector<uint8_t> _bytes;

        static int CalculatePadding(int width);
    public:
        Bitmap(int width, int height, std::vector<uint8_t> bytes);

        int GetWidth() const;
        int GetHeight() const;
        std::vector<uint8_t> const& GetData() const;

        static Bitmap Read(std::filesystem::path path);
        void Write(std::filesystem::path path) const;
    };
}