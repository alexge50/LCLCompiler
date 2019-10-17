#ifndef LCLCOMPILER_TOKENIZER_HPP
#define LCLCOMPILER_TOKENIZER_HPP

#include <vector>
#include <string_view>

#include "chars.hpp"

namespace lcl
{
    enum class token_type
    {
        newline,
        word,
        comment,
        string_literal,
        integer_literal,
        
        open_parans,           // (
        close_parans,          // )
        open_bracket,          // [
        close_bracket,         // ]
        open_curly,            // {
        close_curly,           // }

        equals,                // =
        equals_equals,         // ==
        less_than_or_equal,    // <=
        greater_than_or_equal, // >=
        not_equals,            // !=
        star_equals,           // *=
        minus_equals,          // -=
        plus_equals,           // +=
        slash_equals,          // /=
        mod_equals,            // %=
        shift_right_equals,    // >>=
        shift_left_equals,     // <<=
        bit_not_equals,        // ~=
        xor_equals,            // ^=
        pow_equals,            // **=

        less_than,             // <
        greater_than,          // >
        star,                  // *
        slash,                 // /
        plus,                  // +
        minus,                 // -
        tilde,                 // ~
        mod,                   // %
        hat,                   // ^
        ampersand,             // &
        pipe,                  // |

        backslash,             // \

    };

    struct token
    {
        const token_type       type;
        const std::string_view source_code;

        constexpr token(const token_type type, const std::string_view& source_code_of_token) : type(type), source_code(source_code_of_token)
        {
            //Empty
        }

        [[nodiscard]] constexpr bool is_comment() const noexcept
        {
            return type == token_type::comment;
        }

        [[nodiscard]] constexpr bool is_multi_line_comment() const noexcept
        {
            return is_comment() && source_code.substr(0, 2) == "/*";
        }

        [[nodiscard]] constexpr bool is_single_line_comment() const noexcept
        {
            return is_comment() && source_code.substr(0, 2) == "//";
        }

        [[nodiscard]] constexpr bool is_string_literal() const noexcept 
        {
            return type == token_type::string_literal;
        }

        [[nodiscard]] constexpr bool is_int() const noexcept 
        {
            return type == token_type::integer_literal;
        }
    };

    [[nodiscard]] constexpr bool is_valid_first_character_in_word(const char it) noexcept
    {
        return ascii::is_ascii_letter(it) || it == '_';
    }

    [[nodiscard]] constexpr bool is_valid_mid_character_in_word(const char it) noexcept
    {
        return ascii::is_ascii_letter(it) || ascii::is_digit(it) || it == '_';
    }

    [[nodiscard]] std::vector<token> get_tokens_of_source_code(const std::string_view& source_code);
}

#endif //LCLCOMPILER_TOKENIZER_HPP
