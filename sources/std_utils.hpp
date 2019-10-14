#ifndef LCLCOMPILER_STRING_VIEW_UTILS_HPP
#define LCLCOMPILER_STRING_VIEW_UTILS_HPP

#include <string_view>

[[nodiscard]] constexpr std::string_view make_string_view(const std::string_view::const_iterator begin, const std::size_t length) noexcept
{
    return std::string_view { &*begin, length };
}

[[nodiscard]] constexpr std::string_view make_string_view(const std::string_view::const_iterator begin, const std::string_view::const_iterator end) noexcept
{
    return std::string_view { &*begin, static_cast<std::size_t>(std::distance(begin, end)) };
}

#endif //LCLCOMPILER_STRING_VIEW_UTILS_HPP
