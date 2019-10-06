#include <cctype>
#include <algorithm>
#include <optional>
#include <cassert>
#include <functional>

#include "string_view_utils.hpp"
#include "tokenizer.hpp"
#include "chars.hpp"

namespace lcl
{
    [[nodiscard]] constexpr bool token::is_comment() const noexcept 
    {
        return type == token_type::comment;
    }

    [[nodiscard]] constexpr bool token::is_multi_line_comment() const noexcept
    {
        return is_comment() && source_code.substr(0, 2) == "/*";
    }

    [[nodiscard]] constexpr bool token::is_single_line_comment() const noexcept 
    {
        return is_comment() && source_code.substr(0, 2) == "//";
    }

    [[nodiscard]] bool is_valid_first_character_in_word(const char it) noexcept
    {
        return ascii::is_ascii_letter(it) || it == '_';
    }

    [[nodiscard]] bool is_valid_mid_character_in_word(const char it) noexcept
    {
        return ascii::is_ascii_letter(it) || ascii::is_digit(it) || it == '_';
    }

    class tokenizer
    {
        std::vector<token>               tokens;
        std::string_view                 source_code;
        std::string_view::const_iterator iterator;

        [[nodiscard]] bool end_of_source() const noexcept
        {
            return iterator == source_code.cend();
        }

        [[nodiscard]] bool consume_white_space()
        {
            if (!(*iterator)) 
            {
                return false;
            }

            //Find first character that is not a whitespace or newline
            iterator = std::find_if_not(iterator, source_code.cend(), ascii::is_white_space);

            return true;
        }

        [[nodiscard]] std::optional<token> try_tokenize_newline() const noexcept
        {
            if (!ascii::is_newline(*iterator)) 
            {
                return std::nullopt;
            }

            return token { token_type::newline, make_string_view(iterator, 1) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_single_line_comment() const
        {
            if (make_string_view(iterator, 2) != "//") 
            {
                return std::nullopt;
            }

            const auto comment_begin = iterator;
            const auto comment_end   = std::find_if(iterator, source_code.cend(), ascii::is_newline);

            return token { token_type::comment, make_string_view(comment_begin, comment_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_multi_line_comment() const
        {
            if (make_string_view(iterator, 2) != "/*") 
            {
                return std::nullopt;
            }

            const auto comment_begin = iterator;
            const auto comment_end   = make_string_view(iterator, source_code.cend()).find_first_of("*/");

            if (comment_end == std::string_view::npos) 
            {
                return std::nullopt;
            }

            return token { token_type::comment, make_string_view(comment_begin, comment_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_string_literal() const
        {
            if (*iterator != '"') 
            {
                return std::nullopt;
            }

            const auto string_literal_begin = iterator;
            const auto string_literal_end   = std::find_if(iterator + 1, source_code.cend(), [](auto it) { return it == '"' || ascii::is_newline(it); });

            if (string_literal_end == source_code.cend() || *string_literal_end != '"')
            {
                //Compiler error: String Literal didn't end with a '"'
                //@Temporary
                assert(!"compiler error: String Literal didn't end with a '\"'.");
            }

            return token { token_type::string_literal, make_string_view(string_literal_begin, string_literal_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_integer_literal() const
        {
            if (!std::isdigit(*iterator)) 
            {
                return std::nullopt;
            }

            const auto integer_literal_begin = iterator;
            const auto integer_literal_end   = std::find_if_not(iterator, source_code.cend(), std::isdigit);

            return token { token_type::integer_literal, make_string_view(integer_literal_begin, integer_literal_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_word() const
        {
            if (!is_valid_first_character_in_word(*iterator))
            {
                return std::nullopt;
            }

            const auto word_begin = iterator;
            const auto word_end   = std::find_if_not(iterator + 1, source_code.cend(), is_valid_mid_character_in_word);

            return token { token_type::word, make_string_view(word_begin, word_end) };
        }

        [[nodiscard]] bool add_token(const std::optional<token>& tk)
        {
            if (!tk) return false;

            tokens.push_back(*tk);
            iterator += tk->source_code.length();

            return true;
        }

    public:
        [[nodiscard]] std::vector<token> tokenize(const std::string_view& source_code)
        {
            this->source_code = source_code;
            this->iterator    = source_code.cbegin();
            
            tokens.reserve(1000); //Note: This number is completely arbitrary

            while (!end_of_source())
            {
                if (consume_white_space()) continue;

                //If the token was added successfully we 'continue' in order to consume white space again
                if (add_token(try_tokenize_newline()))             continue;
                if (add_token(try_tokenize_single_line_comment())) continue;
                if (add_token(try_tokenize_multi_line_comment()))  continue;
                if (add_token(try_tokenize_string_literal()))      continue;
                if (add_token(try_tokenize_integer_literal()))     continue;

                assert(!"Nothing to tokenize, AHHHHH");
            }

            return tokens;
        }
    };

    [[nodiscard]] std::vector<token> get_tokens_of_source_code(const std::string_view& source_code)
    {
        auto the_tokenizer = tokenizer{};
        return the_tokenizer.tokenize(source_code);
    }
}