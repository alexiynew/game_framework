/// @file
/// @brief Bmp image implementation.
/// @author Fedorov Alexey
/// @date 05.04.2019

// =============================================================================
// MIT License
//
// Copyright (c) 2017-2019 Fedorov Alexey
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// =============================================================================

#include <algorithm>
#include <cmath>
#include <fstream>
#include <memory>
#include <vector>

#include <common/types.hpp>
#include <image/details/bmp.hpp>

namespace
{
using framework::int16;
using framework::int32;
using framework::uint16;
using framework::uint32;
using framework::uint8;

//  | signature | file size | reserved | reserved | pixel array offset |
//  |------------------------------------------------------------------|
//  | 2         | 4         | 2        | 2        | 4                  |
struct file_header
{
    uint16 signature          = 0;
    uint32 file_size          = 0;
    uint32 pixel_array_offset = 0;

    static file_header read(std::ifstream& in);

    static constexpr uint32 size = 14;
};

struct info_header
{
    enum class type_t
    {
        undefined          = 0,
        bitmapcoreheader   = 12,
        bitmapinfoheader   = 40,
        bitmapv2infoheader = 52,
        bitmapv3infoheader = 56,
        bitmapv4header     = 108,
        bitmapv5header     = 124
    };

    enum class compression_t
    {
        bi_rgb            = 0, // none
        bi_rle8           = 1, // RLE 8-bit/pixel
        bi_rle4           = 2, // RLE 4-bit/pixel
        bi_bitfields      = 3, // OS22XBITMAPHEADER: Huffman 1D
        bi_jpeg           = 4, //
        bi_png            = 5, //
        bi_alphabitfields = 6, // RGBA bit field masks
    };

    enum class color_space_type_t
    {
        lcs_calibrated_rgb      = 0,          // Use endpoints
        lcs_srgb                = 0x73524742, // sRGB
        lcs_windows_color_space = 0x57696E20, // System color space (sRBG by default)
        profile_linked          = 0x4C494E4B, // Profile in another file
        profile_embedded        = 0x4D424544, // Profile in this file
    };

    struct ciexyz
    {
        uint32 x = 0;
        uint32 y = 0;
        uint32 z = 0;
    };

    struct ciexyztriple
    {
        ciexyz red;
        ciexyz green;
        ciexyz blue;
    };

    struct color_t
    {
        uint8 r = 0;
        uint8 g = 0;
        uint8 b = 0;
        uint8 a = 0;
    };

    using color_table_t = std::vector<color_t>;

    // BITMAPCOREHEADER
    uint32 size           = 0;
    int32 width           = 0;
    int32 height          = 0;
    uint16 planes         = 0;
    uint16 bits_per_pixel = 0;

    // BITMAPINFOHEADER
    compression_t compression = compression_t::bi_rgb;
    uint32 image_size         = 0;
    int32 x_ppm               = 0;
    int32 y_ppm               = 0;
    uint32 colors_in_table    = 0;
    uint32 important_colors   = 0;

    // BITMAPV2INFOHEADER
    uint32 red_chanel_bitmask   = 0; //  +-
    uint32 green_chanel_bitmask = 0; //  | matter only if bits_per_pixel == 16 or 32
    uint32 blue_chanel_bitmask  = 0; //  |
                                     //  |
    // BITMAPV3INFOHEADER            //  |
    uint32 alpha_chanel_bitmask = 0; //  +-

    // BITMAPV4HEADER
    color_space_type_t color_space_type = color_space_type_t::lcs_calibrated_rgb;
    ciexyztriple endpoints; //  +-
    uint32 gamma_red   = 0; //  | matter only if color_space_type == lcs_calibrated_rgb
    uint32 gamma_green = 0; //  |
    uint32 gamma_blue  = 0; //  +-

    // BITMAPV4HEADER
    uint32 intent               = 0;
    uint32 color_profile_offset = 0;
    uint32 color_profile_size   = 0;
    uint32 reserved             = 0;

    color_table_t color_table;

    type_t type() const;

    static info_header read(std::ifstream& in);
    static color_table_t read_color_table(std::ifstream& in, const info_header& info);
};

file_header file_header::read(std::ifstream& in)
{
    char buffer[file_header::size];

    in.read(buffer, file_header::size);

    file_header h;
    h.signature          = (buffer[0] << 8) + buffer[1];
    h.file_size          = *(reinterpret_cast<uint32*>(buffer + 2));
    h.pixel_array_offset = *(reinterpret_cast<uint32*>(buffer + 2 + 4 + 4));

    return h;
}

info_header::type_t info_header::type() const
{
    return static_cast<type_t>(size);
}

info_header info_header::read(std::ifstream& in)
{
    char size_buffer[4];
    in.read(size_buffer, sizeof(size_buffer));

    info_header h;
    h.size = *(reinterpret_cast<uint32*>(size_buffer));

    std::unique_ptr<char[]> buffer(new char[h.size]);
    in.read(buffer.get(), h.size);

    if (h.type() == type_t::bitmapcoreheader) {
        h.width          = *(reinterpret_cast<uint16*>(buffer.get()));
        h.height         = *(reinterpret_cast<uint16*>(buffer.get() + 2));
        h.planes         = *(reinterpret_cast<uint16*>(buffer.get() + 4));
        h.bits_per_pixel = *(reinterpret_cast<uint16*>(buffer.get() + 6));
    } else {
        h.width          = *(reinterpret_cast<int32*>(buffer.get()));
        h.height         = *(reinterpret_cast<int32*>(buffer.get() + 4));
        h.planes         = *(reinterpret_cast<uint16*>(buffer.get() + 8));
        h.bits_per_pixel = *(reinterpret_cast<uint16*>(buffer.get() + 10));
    }

    if (h.size >= static_cast<uint32>(info_header::type_t::bitmapinfoheader)) {
        h.compression      = static_cast<compression_t>(*(reinterpret_cast<uint32*>(buffer.get() + 12)));
        h.image_size       = *(reinterpret_cast<uint32*>(buffer.get() + 16));
        h.x_ppm            = *(reinterpret_cast<int32*>(buffer.get() + 20));
        h.y_ppm            = *(reinterpret_cast<int32*>(buffer.get() + 24));
        h.colors_in_table  = *(reinterpret_cast<uint32*>(buffer.get() + 28));
        h.important_colors = *(reinterpret_cast<uint32*>(buffer.get() + 32));
    }

    if (h.size >= static_cast<uint32>(info_header::type_t::bitmapv2infoheader)) {
        h.red_chanel_bitmask   = *(reinterpret_cast<uint32*>(buffer.get() + 36));
        h.green_chanel_bitmask = *(reinterpret_cast<uint32*>(buffer.get() + 40));
        h.blue_chanel_bitmask  = *(reinterpret_cast<uint32*>(buffer.get() + 44));
    }

    if (h.size >= static_cast<uint32>(info_header::type_t::bitmapv3infoheader)) {
        h.alpha_chanel_bitmask = *(reinterpret_cast<uint32*>(buffer.get() + 48));
    }

    if (h.size >= static_cast<uint32>(info_header::type_t::bitmapv4header)) {
        h.color_space_type = static_cast<color_space_type_t>(*(reinterpret_cast<uint32*>(buffer.get() + 52)));

        h.endpoints.red.x   = *(reinterpret_cast<uint32*>(buffer.get() + 56));
        h.endpoints.red.y   = *(reinterpret_cast<uint32*>(buffer.get() + 60));
        h.endpoints.red.z   = *(reinterpret_cast<uint32*>(buffer.get() + 64));
        h.endpoints.green.x = *(reinterpret_cast<uint32*>(buffer.get() + 68));
        h.endpoints.green.y = *(reinterpret_cast<uint32*>(buffer.get() + 72));
        h.endpoints.green.z = *(reinterpret_cast<uint32*>(buffer.get() + 76));
        h.endpoints.blue.x  = *(reinterpret_cast<uint32*>(buffer.get() + 80));
        h.endpoints.blue.y  = *(reinterpret_cast<uint32*>(buffer.get() + 84));
        h.endpoints.blue.z  = *(reinterpret_cast<uint32*>(buffer.get() + 88));

        h.gamma_red   = *(reinterpret_cast<uint32*>(buffer.get() + 92));
        h.gamma_green = *(reinterpret_cast<uint32*>(buffer.get() + 96));
        h.gamma_blue  = *(reinterpret_cast<uint32*>(buffer.get() + 100));
    }

    if (h.size >= static_cast<uint32>(info_header::type_t::bitmapv5header)) {
        h.intent               = *(reinterpret_cast<uint32*>(buffer.get() + 104));
        h.color_profile_offset = *(reinterpret_cast<uint32*>(buffer.get() + 108));
        h.color_profile_size   = *(reinterpret_cast<uint32*>(buffer.get() + 112));
        h.reserved             = *(reinterpret_cast<uint32*>(buffer.get() + 116));
    }

    if (h.bits_per_pixel <= 8) {
        h.color_table = read_color_table(in, h);
    }

    return h;
}

info_header::color_table_t info_header::read_color_table(std::ifstream& in, const info_header& info)
{
    const uint32 colors_count = [&info]() {
        if (info.bits_per_pixel >= 1 && info.bits_per_pixel <= 8 &&
            (info.type() == type_t::bitmapcoreheader || info.colors_in_table == 0)) {
            return static_cast<uint32>(std::pow(2, info.bits_per_pixel));
        }

        return info.colors_in_table;
    }();

    const uint32 cell_size = [&info]() {
        switch (info.type()) {
            case type_t::undefined: return 0;
            case type_t::bitmapcoreheader: return 3;
            case type_t::bitmapinfoheader:
            case type_t::bitmapv2infoheader:
            case type_t::bitmapv3infoheader:
            case type_t::bitmapv4header:
            case type_t::bitmapv5header: return 4;
            default: break;
        }

        return 0;
    }();

    if (colors_count == 0 || cell_size == 0) {
        return color_table_t();
    }

    std::unique_ptr<char[]> buffer(new char[colors_count * cell_size]);
    in.read(buffer.get(), colors_count * cell_size);

    if (!in) {
        return color_table_t();
    }

    color_table_t table(colors_count);
    for (uint32 i = 0; i < colors_count; ++i) {
        const uint32 offset = i * cell_size;

        table[i].r = buffer[offset + 2];
        table[i].g = buffer[offset + 1];
        table[i].b = buffer[offset + 0];
        table[i].a = 0;
    }

    return table;
}

bool check_signature(const file_header& h)
{
    switch (h.signature) {
        case 0x424D: return true; // BM
        case 0x4241: return true; // BA
        case 0x4349: return true; // CI
        case 0x4350: return true; // CP
        case 0x4943: return true; // IC
        case 0x5054: return true; // PT
        default: break;
    }

    return false;
}

bool check_compression(const info_header& h)
{
    if (h.type() == info_header::type_t::bitmapcoreheader) {
        return true;
    }

    switch (h.compression) {
        case info_header::compression_t::bi_rgb: return h.bits_per_pixel != 0;
        case info_header::compression_t::bi_rle8: return h.bits_per_pixel == 8;
        case info_header::compression_t::bi_rle4: return h.bits_per_pixel == 4;
        case info_header::compression_t::bi_bitfields: return h.bits_per_pixel == 16 || h.bits_per_pixel == 32;
        case info_header::compression_t::bi_jpeg: return false; // unsupported
        case info_header::compression_t::bi_png: return false;  // unsupported
        case info_header::compression_t::bi_alphabitfields: return h.bits_per_pixel == 16 || h.bits_per_pixel == 32;
        default: break;
    }

    return false;
}

bool check_color_space_type(const info_header& h)
{
    if (h.size < static_cast<uint32>(info_header::type_t::bitmapv4header)) {
        return true;
    }

    switch (h.color_space_type) {
        case info_header::color_space_type_t::lcs_calibrated_rgb: return true;
        case info_header::color_space_type_t::lcs_srgb: return true;
        case info_header::color_space_type_t::lcs_windows_color_space: return true;
        case info_header::color_space_type_t::profile_linked: return false;   // unsupported
        case info_header::color_space_type_t::profile_embedded: return false; // unsupported ???
        default: break;
    }

    return false;
}

void process_row_1bpp(char* buffer,
                      const info_header& info,
                      uint32 position,
                      framework::image::details::pixel_storage_interface* storage)
{
    for (int32 x = 0, byte = 0; x < info.width; ++byte) {
        for (int32 b = 7; b >= 0 && x < info.width; --b, ++x) {
            const uint32 color_index         = (buffer[byte] & (1 << b)) ? 1 : 0;
            const info_header::color_t color = info.color_table[color_index];
            storage->set_pixel(position + x, color.r, color.g, color.b, color.a);
        }
    }
}

void process_row_24bpp(char* buffer,
                       const info_header& info,
                       uint32 position,
                       framework::image::details::pixel_storage_interface* storage)
{
    for (int32 x = 0; x < info.width; ++x) {
        const uint8 b = buffer[x * 3 + 0];
        const uint8 g = buffer[x * 3 + 1];
        const uint8 r = buffer[x * 3 + 2];
        const uint8 a = 255;

        storage->set_pixel(position + x, r, g, b, a);
    }
}

bool read_data(std::ifstream& in, const info_header& info, framework::image::details::pixel_storage_interface* storage)
{
    storage->resize(info.width * info.height);

    const uint32 row_size = ((info.bits_per_pixel * info.width + 31) / 32) * 4;

    std::unique_ptr<char[]> buffer(new char[row_size]);
    for (int32 y = 0; y < info.height; ++y) {
        in.read(buffer.get(), row_size);

        const bool bottom_up  = info.type() == info_header::type_t::bitmapcoreheader ? true : info.height > 0;
        const uint32 position = bottom_up ? ((info.height - y - 1) * info.width) : (y * info.width);
        switch (info.bits_per_pixel) {
            case 1: process_row_1bpp(buffer.get(), info, position, storage); break;
            case 2: break;
            case 4:
                break;
                // rle
            case 8:
                break;
                // rle
            case 16:
                break;
                // chanel masks
                // may be alpha
            case 24: process_row_24bpp(buffer.get(), info, position, storage); break;
            case 32: {
                // chanel masks
                // may be alpha
                // char buffer[4] = {0};
                // in.read(buffer, sizeof(buffer));

                // storage->set_pixel(index, buffer[2], buffer[1], buffer[0], buffer[3]);
                //++x;
            } break;
            case 48: break;
            case 64:
                break;
                // always alpha
            default: break;
        }
    }

    return true;
}

} // namespace

namespace framework::image::details::bmp
{
bool load(const std::string& filename, pixel_storage_interface* storage)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        return false;
    }

    file_header h = file_header::read(file);

    if (!check_signature(h)) {
        return false;
    }

    info_header info = info_header::read(file);

    if (info.type() == info_header::type_t::undefined) {
        return false;
    }

    const uint32 row_size        = ((info.bits_per_pixel * info.width + 31) / 32) * 4;
    const uint32 image_data_size = row_size * std::abs(info.height);

    if (image_data_size != info.image_size) {
        return false;
    }

    if (!check_compression(info)) {
        return false;
    }

    if (!check_color_space_type(info)) {
        return false;
    }

    file.seekg(h.pixel_array_offset);

    if (!file) {
        return false;
    }

    return read_data(file, info, storage);
}

bool save(const std::string& filename)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    return false;
}

bool is_bmp(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        return false;
    }

    file_header h = file_header::read(file);

    return check_signature(h);
}

} // namespace framework::image::details::bmp