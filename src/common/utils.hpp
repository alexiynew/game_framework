/// @file
/// @brief Different helper functions
/// @author Fedorov Alexey
/// @date 03.03.2018

#ifndef FRAMEWORK_COMMON_UTILS_HPP
#define FRAMEWORK_COMMON_UTILS_HPP

namespace framework {

/// @defgroup common_utils_module Common utils
/// @{

namespace utils {

inline constexpr bool is_debug() noexcept
{
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

} // namespace utils

/// @}

} // namespace framework


#endif
