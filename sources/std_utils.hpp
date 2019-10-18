#ifndef LCLCOMPILER_STRING_VIEW_UTILS_HPP
#define LCLCOMPILER_STRING_VIEW_UTILS_HPP

#include <string_view>
#include <optional>
#include <type_traits>

//Note: to be removed when moving over to C++20
template <class C>
constexpr auto ssize(const C& c) noexcept -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>> 
{
    using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>;
    return static_cast<R>(c.size());
}

template <class T, std::ptrdiff_t N>
constexpr std::ptrdiff_t ssize(const T (&array)[N]) noexcept
{
    return N;
}

[[nodiscard]] constexpr std::string_view substring(const std::string_view::const_iterator begin, const std::size_t length) noexcept
{
    return std::string_view { &*begin, length };
}

[[nodiscard]] constexpr std::string_view substring(const std::string_view::const_iterator begin, const std::string_view::const_iterator end) noexcept
{
    return std::string_view { &*begin, static_cast<std::size_t>(std::distance(begin, end)) };
}

//Returns a string_view of length n beginning from `from` if the length doesn't go past the `end` 
[[nodiscard]] constexpr std::optional<std::string_view> maybe_substring(const std::string_view::const_iterator from, const std::string_view::const_iterator end, const int n) noexcept
{
    if (from + n - 1 < end)
    {
        return substring(from, n);
    }

    return std::nullopt;
}

//Returns a substring of length n starting from string::cbegin if n is smaller than the len of the string
[[nodiscard]] constexpr std::optional<std::string_view> maybe_substring(const std::string_view& string, const int n) noexcept
{
    if (n < ssize(string))
    {
        return substring(string.cbegin(), n);
    }

    return std::nullopt;
}

#endif //LCLCOMPILER_STRING_VIEW_UTILS_HPP
