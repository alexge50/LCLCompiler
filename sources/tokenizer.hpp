#ifndef LCLCOMPILER_TOKENIZER_HPP
#define LCLCOMPILER_TOKENIZER_HPP

#include <vector>
#include <string_view>

#include <tl/expected.hpp>

#include <chars.hpp>
#include <std_utils.hpp>

namespace lcl
{
    enum class tokenizer_error_type
    {
        multi_line_comment_not_closed,
        newline_in_string_literal,
        null_character_in_string_literal,
        string_literal_not_closed_properly,
        numeric_literal_ends_with_underscore,
        numeric_literal_contains_unexpected_character,
    };

    struct tokenizer_error
    {
        const lcl::tokenizer_error_type        error_type;
        const std::string_view::const_iterator iterator_when_error_occured;

        constexpr explicit tokenizer_error(const lcl::tokenizer_error_type error_type, const std::string_view::const_iterator iterator_when_error_occured) : error_type(error_type), iterator_when_error_occured(iterator_when_error_occured) 
        {
            //Empty
        }
    };

    constexpr auto keywords = make_array<std::string_view>
    (
        "alignof",
        "and",
        "asm",
        "bool",
        "break",
        "case",
        "catch",
        "char",
        "continue",
        "default",
        "do",
        "else",
        "false",
        "for",
        "goto",
        "if",
        "import",
        "inline",
        "int",
        "mut",
        "not",
        "null",
        "operator",
        "or",
        "private",
        "public",
        "return",
        "sizeof",
        "struct",
        "switch",
        "this",
        "true",
        "try",
        "union",
        "typedef",
        "typename",
        "using",
        "void",
        "with",
        "while"
    );

    //@Cleanup: replace with constexpr std::find in C++20
    [[nodiscard]] constexpr auto is_keyword(const std::string_view& it) noexcept -> bool
    {
        for (const auto& keyword : keywords)
        {
            if (it == keyword)
            {
                return true;
            }
        }

        return false;
    }

    enum class token_type
    {
        word,                  // if, for, test 
        comment,               // //Comment
        string_literal,        // "String"
        numeric_literal,       // 123
        
        backtick,              // `
        tilde,                 // ~
        exclamation_mark,      // !
        at_symbol,             // @
        hashtag,               // #
        percent,               // %
        hat,                   // ^
        ampersand,             // &
        star,                  // *
        open_parans,           // (
        close_parans,          // )
        minus,                 // -
        plus,                  // +
        equal,                 // =
        colon,                 // :
        left_arrow,            // <
        right_arrow,           // >
        comma,                 // ,
        dot,                   // .
        open_square_bracket,   // [
        close_square_breacket, // ]
        open_curly,            // {
        close_curly,           // }
        bar,                   // |
        semicolon,             // ;
        forward_slash,         // /
        backward_slash,        // \

    };

    constexpr auto single_char_tokens_as_chars = make_array
    (
        '`',
        '~',
        '!',
        '@',
        '#',
        '%',
        '^',
        '&',
        '*',
        '(',
        ')',
        '-',
        '+',
        '=',
        ':',
        '<',
        '>',
        ',',
        '.',
        '[',
        ']',
        '{',
        '}',
        '|',
        ';',
        '/',
        '\\'
    );

    constexpr auto single_char_tokens = make_array<std::string_view>
    (
        "`",
        "~",
        "!",
        "@",
        "#",
        "%",
        "^",
        "&",
        "*",
        "(",
        ")",
        "-",
        "+",
        "=",
        ":",
        "<",
        ">",
        ",",
        ".",
        "[",
        "]",
        "{",
        "}",
        "|",
        ";",
        "/",
        "\\"
    );
 
    constexpr auto single_char_token_types = make_array
    (
        lcl::token_type::backtick,
        lcl::token_type::tilde,
        lcl::token_type::exclamation_mark,
        lcl::token_type::at_symbol,
        lcl::token_type::hashtag,
        lcl::token_type::percent,
        lcl::token_type::hat,
        lcl::token_type::ampersand,
        lcl::token_type::star,
        lcl::token_type::open_parans,
        lcl::token_type::close_parans,
        lcl::token_type::minus,
        lcl::token_type::plus,
        lcl::token_type::equal,
        lcl::token_type::colon,
        lcl::token_type::left_arrow,
        lcl::token_type::right_arrow,
        lcl::token_type::comma,
        lcl::token_type::dot,
        lcl::token_type::open_square_bracket,
        lcl::token_type::close_square_breacket,
        lcl::token_type::open_curly,
        lcl::token_type::close_curly,
        lcl::token_type::bar,
        lcl::token_type::semicolon,
        lcl::token_type::forward_slash,
        lcl::token_type::backward_slash
    );

    [[nodiscard]] constexpr auto is_token_type_representing_a_single_char(const lcl::token_type it) noexcept -> bool
    {
        //Note: Could be replaced with constexpr std::find in C++20 
        switch (it)
        {
            case lcl::token_type::backtick:
            case lcl::token_type::tilde:
            case lcl::token_type::exclamation_mark:
            case lcl::token_type::at_symbol:
            case lcl::token_type::hashtag:
            case lcl::token_type::percent:
            case lcl::token_type::hat:
            case lcl::token_type::ampersand:
            case lcl::token_type::star:
            case lcl::token_type::open_parans:
            case lcl::token_type::close_parans:
            case lcl::token_type::minus:
            case lcl::token_type::plus:
            case lcl::token_type::equal:
            case lcl::token_type::colon:
            case lcl::token_type::left_arrow:
            case lcl::token_type::right_arrow:
            case lcl::token_type::comma:
            case lcl::token_type::dot:
            case lcl::token_type::open_square_bracket:
            case lcl::token_type::close_square_breacket:
            case lcl::token_type::open_curly:
            case lcl::token_type::close_curly:
            case lcl::token_type::bar:
            case lcl::token_type::semicolon:
            case lcl::token_type::forward_slash:
            case lcl::token_type::backward_slash:
                return true; 

            default: 
                return false;
        }
    }

    [[nodiscard]] constexpr auto is_single_char_represented_by_token_type(const char it) noexcept -> bool
    {
        switch (it)
        {
            case '`' :
            case '~' :
            case '!' :
            case '@' :
            case '#' :
            case '%' :
            case '^' :
            case '&' :
            case '*' :
            case '(' :
            case ')' :
            case '-' :
            case '+' :
            case '=' :
            case ':' :
            case '<' :
            case '>' :
            case ',' :
            case '.' :
            case '[' :
            case ']' :
            case '{' :
            case '}' :
            case '|' :
            case ';' :
            case '/' :
            case '\\':
                return true;
             
            default  :
                return false;
        }
    }

    [[nodiscard]] constexpr auto get_single_char_represented_by_token_type(const lcl::token_type it) noexcept -> bool
    {
        assert(is_token_type_representing_a_single_char(it));

        switch (it)
        {
            case lcl::token_type::backtick: return '`';
            case lcl::token_type::tilde: return '~';
            case lcl::token_type::exclamation_mark: return '!';
            case lcl::token_type::at_symbol: return '@';
            case lcl::token_type::hashtag: return '#';
            case lcl::token_type::percent: return '%';
            case lcl::token_type::hat: return '^';
            case lcl::token_type::ampersand: return '&';
            case lcl::token_type::star: return '*';
            case lcl::token_type::open_parans: return '(';
            case lcl::token_type::close_parans: return ')';
            case lcl::token_type::minus: return '-';
            case lcl::token_type::plus: return '+';
            case lcl::token_type::equal: return '=';
            case lcl::token_type::colon: return ':';
            case lcl::token_type::left_arrow: return '<';
            case lcl::token_type::right_arrow: return '>';
            case lcl::token_type::comma: return ',';
            case lcl::token_type::dot: return '.';
            case lcl::token_type::open_square_bracket: return '[';
            case lcl::token_type::close_square_breacket: return ']';
            case lcl::token_type::open_curly: return '{';
            case lcl::token_type::close_curly: return '}';
            case lcl::token_type::bar: return '|';
            case lcl::token_type::semicolon: return ';';
            case lcl::token_type::forward_slash: return '/';
            case lcl::token_type::backward_slash: return '\\';
        }

        //This case should never be reached
        assert(false);
        return '\0';
    }

    [[nodiscard]] constexpr auto get_token_type_that_represents_char(const char it) noexcept -> lcl::token_type
    {
        assert(is_single_char_represented_by_token_type(it));

        switch (it)
        {
            case '`' : return lcl::token_type::backtick;
            case '~' : return lcl::token_type::tilde;
            case '!' : return lcl::token_type::exclamation_mark;
            case '@' : return lcl::token_type::at_symbol;
            case '#' : return lcl::token_type::hashtag;
            case '%' : return lcl::token_type::percent;
            case '^' : return lcl::token_type::hat;
            case '&' : return lcl::token_type::ampersand;
            case '*' : return lcl::token_type::star;
            case '(' : return lcl::token_type::open_parans;
            case ')' : return lcl::token_type::close_parans;
            case '-' : return lcl::token_type::minus;
            case '+' : return lcl::token_type::plus;
            case '=' : return lcl::token_type::equal;
            case ':' : return lcl::token_type::colon;
            case '<' : return lcl::token_type::left_arrow;
            case '>' : return lcl::token_type::right_arrow;
            case ',' : return lcl::token_type::comma;
            case '.' : return lcl::token_type::dot;
            case '[' : return lcl::token_type::open_square_bracket;
            case ']' : return lcl::token_type::close_square_breacket;
            case '{' : return lcl::token_type::open_curly;
            case '}' : return lcl::token_type::close_curly;
            case '|' : return lcl::token_type::bar;
            case ';' : return lcl::token_type::semicolon;
            case '/' : return lcl::token_type::forward_slash;
            case '\\': return lcl::token_type::backward_slash;
        }

        //Should never be reached
        assert(false);
        return static_cast<lcl::token_type>(0);
    }

    struct token
    {
        const lcl::token_type  type;
        const std::string_view code;

        constexpr explicit token(const lcl::token_type type, const std::string_view& code_of_token) : type(type), code(code_of_token)
        {
            //Empty
        }

        [[nodiscard]] constexpr auto is_multi_line_comment() const noexcept -> bool
        {
            return type == lcl::token_type::comment && string_view_slice(code, 2) == "/*";
        }

        [[nodiscard]] constexpr auto is_single_line_comment() const noexcept -> bool
        {
            return type == lcl::token_type::comment && string_view_slice(code, 2) == "//";
        }

        [[nodiscard]] constexpr auto is_int_literal() const noexcept -> bool 
        {
            if (type == lcl::token_type::numeric_literal)
            {
                for (const auto& it : code)
                {
                    if (it == '.')
                    {
                        return false;
                    }
                }

                return true;
            }

            return false;
        }

        //@Cleanup: replace loop with contsexpr std::find in C++20
        [[nodiscard]] constexpr auto is_float_literal() const noexcept -> bool 
        {
            if (type == lcl::token_type::numeric_literal)
            {
                for (const auto& it : code)
                {
                    if (it == '.')
                    {
                        return true;
                    }
                }

                return false;
            }

            return false;
        }

        [[nodiscard]] constexpr auto is_single_char_token() const noexcept -> bool 
        {
            return lcl::is_token_type_representing_a_single_char(type);
        }

        [[nodiscard]] constexpr auto is_keyword() const noexcept -> bool 
        {
            return type == lcl::token_type::word && lcl::is_keyword(code);
        }

        [[nodiscard]] constexpr auto is_identifier() const noexcept -> bool 
        {
            return type == lcl::token_type::word && !lcl::is_keyword(code);
        }
    };

    [[nodiscard]] constexpr auto is_valid_first_character_in_word(const char32_t it) noexcept -> bool
    {
        return chars::is_ascii_letter(it) || it == '_';
    }

    [[nodiscard]] constexpr auto is_valid_mid_character_in_word(const char32_t it) noexcept -> bool
    {
        return chars::is_ascii_letter(it) || chars::is_ascii_digit(it) || it == '_';
    }

    [[nodiscard]] constexpr auto is_valid_mid_character_in_numeric_literal(const char32_t it) noexcept -> bool
    {
        return chars::is_ascii_digit(it) || it == '.' || it == '_';
    }

    [[nodiscard]] tl::expected<std::vector<lcl::token>, lcl::tokenizer_error> tokenize_code(const std::string_view& code);
}

#endif //LCLCOMPILER_TOKENIZER_HPP
