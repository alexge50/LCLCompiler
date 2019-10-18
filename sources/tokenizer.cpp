#include <cctype>
#include <algorithm>
#include <optional>
#include <cassert>
#include <functional>
#include <cassert>

#include "std_utils.hpp"
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

        [[nodiscard]] std::optional<token> try_tokenize_single_line_comment() const
        {
            const auto first_2_chars = maybe_substring(iterator, source_code.cend(), 2);

            if (!first_2_chars.has_value())
            {
                return std::nullopt;
            }

            if (*first_2_chars != "//")
            {
                return std::nullopt;
            }

            const auto comment_begin = iterator;
            const auto comment_end   = std::find_if(iterator, source_code.cend(), ascii::is_newline);

            return token { token_type::comment, substring(comment_begin, comment_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_multi_line_comment() const
        {
            if (substring(iterator, 2) != "/*") 
            {
                return std::nullopt;
            }

            const auto comment_begin   = iterator;
            const auto code_to_look_at = substring(comment_begin + 2, source_code.cend());
            const auto comment_end     = [&code_to_look_at] () -> std::string_view::const_iterator
            {
                auto inner_comments_count = 0;

                for (int i = 0; i < ssize(code_to_look_at) - 1; i++)
                {
                    const auto view = substring(code_to_look_at.cbegin() + i, 2);

                    if (view == "/*")
                    {
                        inner_comments_count++;
                        i++;
                        continue;
                    }

                    if (view == "*/")
                    {
                        if (inner_comments_count == 0)
                        {
                            return view.cbegin();
                        }

                        inner_comments_count--;
                        i++;
                    }
                }

                return code_to_look_at.cend();
            }();

            if (comment_end == code_to_look_at.cend()) 
            {
                //@Temporary
                assert(!"Error: Comment was not closed.");
            }

            return token { token_type::comment, substring(comment_begin, comment_end + 2) };
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

            return token { token_type::string_literal, substring(string_literal_begin, string_literal_end + 1) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_integer_literal() const
        {
            if (!ascii::is_digit(*iterator)) 
            {
                return std::nullopt;
            }

            const auto integer_literal_begin = iterator;
            const auto integer_literal_end   = std::find_if_not(iterator, source_code.cend(), ascii::is_digit);

            return token { token_type::integer_literal, substring(integer_literal_begin, integer_literal_end) };
        }

        [[nodiscard]] std::optional<token> try_tokenize_word() const
        {
            if (!is_valid_first_character_in_word(*iterator))
            {
                return std::nullopt;
            }

            const auto word_begin = iterator;
            const auto word_end   = std::find_if_not(iterator + 1, source_code.cend(), is_valid_mid_character_in_word);

            return token { token_type::word, substring(word_begin, word_end) };
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
            
            while (!end_of_source())
            {
                switch (*iterator)
                {
                    case ' ':
                    {
                        iterator = std::find_if_not(iterator, source_code.cend(), ascii::is_white_space);
                        continue;
                    }

                    case '\n':
                    {
                        const auto newlines_end   = std::find_if_not(iterator, source_code.cend(), ascii::is_newline);
                        const auto newlines_count = std::distance(iterator, newlines_end);

                        for (int i = 0; i < newlines_count; i++)
                        {
                            tokens.emplace_back(token_type::newline, substring(iterator + i, 1));
                        }

                        iterator = newlines_end;

                        continue;
                    }
                }

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