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
    class tokenizer
    {
        std::vector<token>               tokens;
        std::string_view                 source_code;
        std::string_view::const_iterator iterator;

        [[nodiscard]] bool end_of_source() const noexcept
        {
            return iterator == source_code.cend();
        }

        [[nodiscard]] std::string_view::const_iterator get_iterator_after_consuming_whitespace_from(const std::string_view::const_iterator consume_white_space_from) const noexcept
        {
            //Find first character that is not a whitespace
            return std::find_if_not(consume_white_space_from, source_code.cend(), ascii::is_white_space);
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

            const auto comment_begin              = iterator;
            const auto source_since_comment_begin = make_string_view(comment_begin, source_code.cend);
            const auto comment_end                = [&source_since_comment_begin]() -> std::string_view::const_iterator
            {
                auto inner_comments_count = 0;

                for (int i = 0; i < source_since_comment_begin.size() - 1; i++)
                {
                    const auto view_of_2_chars = make_string_view(source_since_comment_begin.cbegin() + i, source_since_comment_begin.cbegin() + i + 1);

                    if (view_of_2_chars == "/*")
                    {
                        inner_comments_count++;
                    }

                    if (view_of_2_chars == "*/")
                    {
                        if (inner_comments_count == 0)
                        {
                            return view_of_2_chars.cbegin() + 1;
                        }

                        inner_comments_count--;
                    }
                }

                return source_since_comment_begin.cend();
            }();

            if (comment_end == source_since_comment_begin.cend()) 
            {
                assert(!"Comment was not closed.");
            }

            return token { token_type::comment, make_string_view(comment_begin, comment_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_string_literal() const
        {
            if (*iterator != '"') 
            {
                return std::nullopt;
            }

            auto escaped_character = false;

            const auto string_literal_begin = iterator;
            const auto string_literal_end   = std::find_if(iterator + 1, source_code.cend(), [&escaped_character] (auto it) 
            {
                if (escaped_character)
                {
                    escaped_character = false;
                    return ascii::is_newline(it);
                }

                if (it == '\\')
                {
                    escaped_character = true;
                    return false;
                }

                return it == '"' || ascii::is_newline(it); 
            });

            if (string_literal_end == source_code.cend() || *string_literal_end != '"')
            {
                //Compiler error: String Literal didn't end with a '"'
                //@Temporary
                assert(!"Compiler error: String Literal didn't end with a '\"'.");
            }

            return token { token_type::string_literal, make_string_view(string_literal_begin, string_literal_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_integer_literal() const
        {
            if (!ascii::is_digit(*iterator)) 
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

        [[nodiscard]] bool try_add_token(const std::optional<token>& tk)
        {
            if (!tk) 
            {
                return false;
            }

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
                if (auto after_whitespace_iterator = get_iterator_after_consuming_whitespace_from(iterator); after_whitespace_iterator != iterator)
                {
                    iterator = after_whitespace_iterator;
                    
                    if (end_of_source()) 
                    {
                        break;
                    }
                }

                //If the token was added successfully we 'continue' in order to consume white space again
                if (try_add_token(try_tokenize_newline()))             continue;
                if (try_add_token(try_tokenize_single_line_comment())) continue;
                if (try_add_token(try_tokenize_multi_line_comment()))  continue;
                if (try_add_token(try_tokenize_string_literal()))      continue;
                if (try_add_token(try_tokenize_integer_literal()))     continue;

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