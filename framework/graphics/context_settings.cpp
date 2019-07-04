/// @file
/// @brief Graphic context settings.
/// @author Fedorov Alexey
/// @date 12.11.2018

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

#include <graphics/context_settings.hpp>

namespace framework::graphics
{
#pragma region setters

context_settings& context_settings::set_api_type(graphic_api api)
{
    m_api = api;
    return *this;
}

context_settings& context_settings::set_version(utils::version version)
{
    m_version = version;
    return *this;
}

context_settings& context_settings::set_depth_bits(int32 bits)
{
    m_depth_bits = bits;
    return *this;
}

context_settings& context_settings::set_stencil_bits(int32 bits)
{
    m_stencil_bits = bits;
    return *this;
}

context_settings& context_settings::set_antialiasing_level(antialiasing level)
{
    m_antialiasing_level = level;
    return *this;
}

#pragma endregion

#pragma region getters

context_settings::graphic_api context_settings::api_type() const
{
    return m_api;
}

utils::version context_settings::version() const
{
    return m_version;
}

int32 context_settings::depth_bits() const
{
    return m_depth_bits;
}

int32 context_settings::stencil_bits() const
{
    return m_stencil_bits;
}

context_settings::antialiasing context_settings::antialiasing_level() const
{
    return m_antialiasing_level;
}

#pragma endregion

} // namespace framework::graphics