#ifndef LCLCOMPILER_STRING_VIEW_UTILS_HPP
#define LCLCOMPILER_STRING_VIEW_UTILS_HPP

#include <string_view>
#include <optional>
#include <type_traits>
#include <array>

#include <gsl/span>
#include <gsl/gsl_assert>

#ifdef _MSC_VER 
    #define UNREACHABLE_CODE __assume(false)
#endif

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

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view::const_iterator begin, const std::string_view::size_type length) noexcept -> std::string_view
    {
        return std::string_view { &*begin, length };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view::const_iterator begin, const std::string_view::const_iterator end) noexcept -> std::string_view
    {
        Expects(begin <= end);

        return std::string_view { &*begin, std::distance(begin, end) };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view& it, const const std::string_view::size_type end) noexcept -> std::string_view 
    {
        Expects(end >= 0 && end < it.size());

        return std::string_view { &*std::cbegin(it), end };
    }

    [[nodiscard]] constexpr auto string_view_slice(const std::string_view& it, const std::string_view::size_type begin, const const std::string_view::size_type end) noexcept -> std::string_view 
    {
        Expects(begin >= 0 && begin < it.size() && end >= 0 && end < it.size());

        return std::string_view { &*std::cbegin(it), end - begin };
    }
}

template<class T, class Iterator>
[[nodiscard]] constexpr auto make_span(const Iterator begin, const Iterator end) noexcept -> gsl::span<T>
{
    return gls::make_span(&*begin, std::distance(begin, end));
}

#endif //LCLCOMPILER_STRING_VIEW_UTILS_HPP
