#include "Bitmap.h"

#include <fstream>
#include <format>

namespace OpenCL_PathTracer
{
    int Bitmap::CalculatePadding(int width)
    {
        return (4 - ((width * 3) % 4)) % 4;
    }

    Bitmap::Bitmap(int width, int height, std::vector<uint8_t> bytes)
    {
        _width = width;
        _height = height;
        _bytes = std::move(bytes);
    }

    int Bitmap::GetWidth() const
    {
        return _width;
    }

    int Bitmap::GetHeight() const
    {
        return _height;
    }

    std::vector<uint8_t> const& Bitmap::GetData() const
    {
        return _bytes;
    }

    Bitmap Bitmap::Read(std::filesystem::path path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error(std::format("File does not exist! Path: {}.", path.string()));
        }
        if (std::filesystem::file_size(path) < _headerSize)
        {
            throw std::runtime_error(std::format("A valid .bmp file contains at least {} bytes! Path: {}.", _headerSize, path.string()));
        }

        auto is = std::ifstream(path, std::ios::binary);
        uint8_t header[_headerSize]{};
        is.read(reinterpret_cast<char*>(header), _headerSize);
        if (header[0] != 'B' || header[1] != 'M')
        {
            throw std::runtime_error(std::format("A valid .bmp file header starts with BM! Actual values are: '{}' and '{}'.", static_cast<char>(header[0]), static_cast<char>(header[1])));
        }

        auto width = *reinterpret_cast<int*>(&header[18]);
        auto height = *reinterpret_cast<int*>(&header[22]);
        auto rowWidthWithoutPadding = width * 3;
        auto size = rowWidthWithoutPadding * height;
        auto padding = CalculatePadding(width);

        if (width > 2048 || height > 1024)
        {
            throw std::runtime_error(std::format("Max supported resolution is 2048x1024! Actual resolution: {}x{}.", width, height));
        }

        auto bytes = std::vector<uint8_t>(size);
        for (int i = 0; i < height; ++i)
        {
            is.read(reinterpret_cast<char*>(&bytes[rowWidthWithoutPadding * i]), rowWidthWithoutPadding);
            is.ignore(padding);
        }

        return Bitmap(width, height, bytes);
    }

    void Bitmap::Write(std::filesystem::path path) const
    {
        int padding = CalculatePadding(_width);
        uint8_t paddingBytes[3] = { 0, 0, 0 };
        auto rowWidthWithoutPadding = _width * 3;
        int size = (rowWidthWithoutPadding + padding) * _height;

        uint8_t header[_headerSize]{ 'B', 'M' };
        *reinterpret_cast<int*>(&header[2]) = _headerSize + size;   // File size in bytes.
        *reinterpret_cast<int*>(&header[10]) = _headerSize;         // Offset where the pixel array (bitmap data) can be found.
        *reinterpret_cast<int*>(&header[14]) = 40;                  // Number of bytes in the DIB header (from this point).
        *reinterpret_cast<int*>(&header[18]) = _width;              // Width of the bitmap in pixels.
        *reinterpret_cast<int*>(&header[22]) = _height;             // Height of the bitmap in pixels.
        *reinterpret_cast<short*>(&header[26]) = 1;                 // Number of color planes being used.
        *reinterpret_cast<short*>(&header[28]) = 24;                // Number of bits per pixel.
        *reinterpret_cast<int*>(&header[34]) = size;                // Size of the raw bitmap data (including padding).

        std::filesystem::create_directories(path.parent_path());
        auto os = std::ofstream(path, std::ios::binary);

        os.write(reinterpret_cast<char*>(&header[0]), _headerSize);
        for (int i = 0; i < _height; ++i)
        {
            os.write(reinterpret_cast<char const*>(&_bytes[rowWidthWithoutPadding * i]), rowWidthWithoutPadding);
            os.write(reinterpret_cast<char const*>(&paddingBytes[0]), padding);
        }
    }
}