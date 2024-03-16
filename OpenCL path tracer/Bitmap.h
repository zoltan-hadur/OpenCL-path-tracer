#pragma once

#include <vector>
#include <filesystem>

// Stores BGR bitmap image.
class Bitmap {
private:
    static const int _headerSize = 54;
    int _width = 0;
    int _height = 0;
    std::vector<uint8_t> _bytes;

    static int CalculatePadding(int width);
public:
    Bitmap(int width, int height, std::vector<uint8_t> bytes);

    int Width() const;
    int Height() const;
    std::vector<uint8_t> const& Bytes() const;

    static Bitmap Read(std::filesystem::path path);
    void Write(std::filesystem::path path) const;
};