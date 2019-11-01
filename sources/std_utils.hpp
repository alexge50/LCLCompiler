#ifndef LCLCOMPILER_STRING_VIEW_UTILS_HPP
#define LCLCOMPILER_STRING_VIEW_UTILS_HPP

#include <string_view>
#include <optional>
#include <type_traits>
#include <array>
#include <cassert>

namespace lcl
{
    template <typename... T>
    constexpr auto make_array(T&&... values) -> std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(T)> 
    {
        return std::array<typename std::decay<typename std::common_type<T...>::type>::type, sizeof...(T)>{ std::forward<T>(values)... };
    }

    //@Cleanup: to be removed when moving over to C++20
    template <class C>
    constexpr auto ssize(const C& c) noexcept -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(std::size(c))>> 
    {
        using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(std::size(c))>>;
        
        return static_cast<R>(std::size(c));
    }

    template <class T, std::ptrdiff_t N>
    constexpr auto ssize(const T (&array)[N]) noexcept -> std::ptrdiff_t
    {
        return N;
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view::const_iterator begin, const std::size_t length) noexcept -> std::string_view
    {
        return std::string_view { &*begin, length };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view::const_iterator begin, const std::string_view::const_iterator end) noexcept -> std::string_view
    {
        assert(begin <= end);

        return std::string_view { &*begin, static_cast<std::size_t>(std::distance(begin, end)) };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view& it, const int end) noexcept -> std::string_view 
    {
        assert(end >= 0 && end < ssize(it));

        return std::string_view { &*std::cbegin(it), static_cast<size_t>(end) };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view& it, const int begin, const int end) noexcept -> std::string_view 
    {
        assert(begin >= 0 && begin < lcl::ssize(it) && end >= 0 && end < lcl::ssize(it));

        return std::string_view { &*std::cbegin(it), static_cast<size_t>(end - begin) };
    }
}

#endif //LCLCOMPILER_STRING_VIEW_UTILS_HPP
