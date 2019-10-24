#ifndef LCLCOMPILER_CHARS_HPP
#define LCLCOMPILER_CHARS_HPP

#include "external/utf8proc/utf8proc.h"

namespace lcl::chars
{
    [[nodiscard]] constexpr inline bool is_ascii_letter(const char32_t it) noexcept
    {
        return (it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z');
    }

    [[nodiscard]] constexpr inline bool is_ascii_digit(const char32_t it) noexcept
    {
        return it >= '0' && it <= '9';
    }

    [[nodiscard]] constexpr inline bool is_white_space(const char32_t it) noexcept
    {
        return it == ' ' || it == '\t' || it == '\n' || it == '\r';
    }

    [[nodiscard]] constexpr inline bool is_white_space_not_newline(const char32_t it) noexcept
    {
        return it == ' ' || it == '\t' || it == '\r';
    }

    [[nodiscard]] constexpr inline bool is_newline(const char32_t it) noexcept
    {
        return it == '\n';
    }

    [[nodiscard]] inline bool is_unicode_letter(const char32_t it) noexcept 
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

    [[nodiscard]] inline bool is_unicode_digit(const char32_t it) noexcept
    {
        if (it < 0x80)
        {
            return is_ascii_digit(it);
        }
        
        return utf8proc_category(it) == UTF8PROC_CATEGORY_ND;
    }
}

#endif LCLCOMPILER_CHARS_HPP