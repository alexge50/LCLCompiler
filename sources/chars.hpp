#ifndef LCLCOMPILER_CHARS_HPP
#define LCLCOMPILER_CHARS_HPP

namespace lcl::ascii
{
    [[nodiscard]] constexpr inline bool is_ascii_letter(const char it) noexcept
    {
        return (it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z');
    }

    [[nodiscard]] constexpr inline bool is_digit(const char it) noexcept
    {
        return it >= '0' && it <= '9';
    }

    //@Incomplete
    [[nodiscard]] constexpr inline bool is_white_space(const char it) noexcept
    {
        return it == ' ' || it == '\r' || it == '\t';
    }

    //@Incomplete
    [[nodiscard]] constexpr inline bool is_newline(const char it) noexcept
    {
        return it == '\n';
    }
}

#endif LCLCOMPILER_CHARS_HPP