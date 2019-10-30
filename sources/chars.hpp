#ifndef LCLCOMPILER_CHARS_HPP
#define LCLCOMPILER_CHARS_HPP

#include <utf8proc/utf8proc.h>

namespace lcl::chars
{
    [[nodiscard]] constexpr auto is_ascii_letter(const char32_t it) noexcept -> bool
    {
        return (it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z');
    }

    [[nodiscard]] constexpr auto is_ascii_digit(const char32_t it) noexcept -> bool
    {
        return it >= '0' && it <= '9';
    }

    [[nodiscard]] constexpr auto is_white_space(const char32_t it) noexcept -> bool
    {
        return it == ' ' || it == '\t' || it == '\n' || it == '\r';
    }

    [[nodiscard]] constexpr auto is_newline(const char32_t it) noexcept -> bool
    {
        return it == '\n';
    }

    [[nodiscard]] inline auto is_unicode_letter(const char32_t it) noexcept -> bool 
    {
        if (it < 0x80)
        {
            return is_ascii_letter(it);
        }

        switch (utf8proc_category(it))
        {
            case UTF8PROC_CATEGORY_LU:
            case UTF8PROC_CATEGORY_LL:
            case UTF8PROC_CATEGORY_LT:
            case UTF8PROC_CATEGORY_LM:
            case UTF8PROC_CATEGORY_LO:
                return true; 
        }

        return false;
    }

    [[nodiscard]] inline auto is_unicode_digit(const char32_t it) noexcept -> bool
    {
        if (it < 0x80)
        {
            return is_ascii_digit(it);
        }
        
        return utf8proc_category(it) == UTF8PROC_CATEGORY_ND;
    }
}

#endif LCLCOMPILER_CHARS_HPP