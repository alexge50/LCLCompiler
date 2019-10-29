#ifndef LCLCOMPILER_STRING_VIEW_UTILS_HPP
#define LCLCOMPILER_STRING_VIEW_UTILS_HPP

#include <string_view>
#include <optional>
#include <type_traits>
#include <array>

namespace lcl::utils
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
    constexpr std::ptrdiff_t ssize(const T (&array)[N]) noexcept
    {
        return N;
    }

    [[nodiscard]] constexpr std::string_view string_view_from_iterator(const std::string_view::const_iterator begin, const std::size_t length) noexcept
    {
        return std::string_view { &*begin, length };
    }

    [[nodiscard]] constexpr std::string_view string_view_from_iterator(const std::string_view::const_iterator begin, const std::string_view::const_iterator end) noexcept
    {
        assert(begin <= end);
        return std::string_view { &*begin, static_cast<std::size_t>(std::distance(begin, end)) };
    }

    [[nodiscard]] constexpr std::string_view substring(const std::string_view& it, const int end) noexcept 
    {
        assert(end >= 0 && end < lcl::utils::ssize(it));

        return std::string_view { &*std::cbegin(it), static_cast<size_t>(end) };
    }

    [[nodiscard]] constexpr std::string_view substring(const std::string_view& it, const int begin, const int end) noexcept 
    {
        assert(begin >= 0 && begin < lcl::utils::ssize(it) && end >= 0 && end < lcl::utils::ssize(it));

        return std::string_view { &*std::cbegin(it), static_cast<size_t>(end - begin) };
    }

    //Returns a string_view of length n beginning from `from` if the length doesn't go past the `end` 
    [[nodiscard]] constexpr std::optional<std::string_view> string_view_from_iterator_if_not_overflow(const std::string_view::const_iterator from, const std::string_view::const_iterator end, const int n) noexcept
    {
        if (from + n - 1 < end)
        {
            return lcl::utils::string_view_from_iterator(from, n);
        }

        return std::nullopt;
    }

    //Returns a substring of length n starting from string::cbegin if n is smaller than the len of the string
    [[nodiscard]] constexpr std::optional<std::string_view> string_view_from_iterator_if_not_overflow(const std::string_view& string, const int n) noexcept
    {
        if (n < lcl::utils::ssize(string))
        {
            return lcl::utils::string_view_from_iterator(std::cbegin(string), n);
        }

        return std::nullopt;
    }
}

#endif //LCLCOMPILER_STRING_VIEW_UTILS_HPP
