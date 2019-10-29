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
    [[nodiscard]] std::vector<token> tokenize_code(const std::string_view& code)
    {
        const auto code_begin = std::cbegin(code);
        const auto code_end   = std::cend(code);

        auto tokens        = std::vector<lcl::token>{};
        auto code_iterator = code_begin;

        const auto add_one_char_token = [&] (const lcl::token_type tk_type) 
        { 
            tokens.emplace_back(tk_type, lcl::utils::string_view_from_iterator(code_iterator, std::next(code_iterator))); 
            code_iterator = std::next(code_iterator);
        };

        while (code_iterator != code_end)
        {
            switch (*code_iterator)
            {
                //One character tokens
                case '`':
                case '\n':
                case '~':
                case '!':
                case '@':
                case '#':
                case '%':
                case '^':
                case '&':
                case '*':
                case '(':
                case ')':
                case '-':
                case '+':
                case '=':
                case ':':
                case '<':
                case '>':
                case ',':
                case '.':
                case '[':
                case ']':
                case '{':
                case '}':
                case '|':
                case ';':
                //case '/': We dont handle / here because it is used to produce comments
                case '\\':
                {
                    tokens.emplace_back(lcl::get_token_type_that_represents_char(*code_iterator), lcl::utils::string_view_from_iterator(code_iterator, std::next(code_iterator))); 
                    code_iterator = std::next(code_iterator);
                    
                    continue;
                }

                case ' ':
                {
                    code_iterator = std::find_if_not(code_iterator, code_end, lcl::chars::is_white_space_not_newline);
                    
                    continue;
                }

                case '/':
                {
                    const auto iterator_to_initial_forward_slash    = code_iterator;
                    const auto iterator_after_initial_forward_slash = std::next(iterator_to_initial_forward_slash);
                    const auto should_tokenize_comment              = iterator_after_initial_forward_slash != code_end && (*iterator_after_initial_forward_slash == '/' || *iterator_after_initial_forward_slash == '*');

                    if (!should_tokenize_comment)
                    {
                        tokens.emplace_back(lcl::token_type::forward_slash, lcl::utils::string_view_from_iterator(iterator_to_initial_forward_slash, 1));
                        code_iterator = std::next(code_iterator);

                        continue;
                    }
                    else switch (*iterator_after_initial_forward_slash)
                    {
                        //Regular comment
                        case '/':
                        {
                            const auto commend_begin = iterator_to_initial_forward_slash;
                            const auto comment_end   = std::find(iterator_after_initial_forward_slash, code_end, '\n');
                            
                            tokens.emplace_back(lcl::token_type::comment, lcl::utils::string_view_from_iterator(commend_begin, comment_end));
                            code_iterator = comment_end;

                            continue;
                        }

                        //Multiline comment
                        case '*':
                        {
                            const auto comment_begin = iterator_to_initial_forward_slash;

                            //A multiline comment ends with `*/`.
                            //This procedure will return an iterator to the slash at the end of the close `*/` or `code_end` in case it was not found.
                            //This procedure also takes into account nested comments. 
                            const auto iterator_to_comment_closer_slash = [&] () -> std::string_view::const_iterator 
                            {
                                //We ignore the initial `/*` so we start 2 chars ahead to look for the end `*/` of the comment.
                                const auto code_to_look_at_for_comment_closer_begin  = std::next(comment_begin, 2);
                                const auto code_to_look_at_for_comment_closer_length = std::distance(code_to_look_at_for_comment_closer_begin, code_end);  
                                
                                //We use this to count inner comment blocks. Eg: /* /* inner */ */
                                auto inner_comments_count = 0;
                                
                                //We look at 2 chars at a time, advance by one char. Eg: for "Test" we will look at the views: [ "Te", "es", "st" ]
                                for (auto i = 0; i < code_to_look_at_for_comment_closer_length - 1; ++i)
                                {
                                    const auto view = lcl::utils::string_view_from_iterator(std::next(code_to_look_at_for_comment_closer_begin, i), 2);

                                    if (view == "/*")
                                    {
                                        ++inner_comments_count;
                                        
                                        //We need to advance by 2 chars here because we dont want the `*` to be reused, 
                                        //that would make cases like `/*/` valid and we don't want that.
                                        ++i;
                                        
                                        continue;
                                    }

                                    if (view == "*/")
                                    {
                                        if (inner_comments_count == 0)
                                        {
                                            return std::next(std::cbegin(view));
                                        }

                                        --inner_comments_count;

                                        //We need to advance by 2 chars here because we dont want the `*` to be reused, 
                                        //that would make cases like `/*/` valid and we don't want that.
                                        ++i;
                                    }
                                }

                                return code_end;
                            }();

                            //Todo: Error handling
                            assert(iterator_to_comment_closer_slash != code_end);

                            const auto comment_end = std::next(iterator_to_comment_closer_slash);

                            tokens.emplace_back(lcl::token_type::comment, lcl::utils::string_view_from_iterator(comment_begin, comment_end));
                            code_iterator = comment_end;

                            continue;
                        }
                    }
                }

                case '"':
                {
                    const auto string_begin = code_iterator;
                    
                    //Used to mark if the next character should be escaped, such as `\"`
                    auto escape_next_character = false;
                    //This proc will return an iterator to the closing `"` or code_end if the string isn't closed properly.
                    //We start looking after the first `"`.
                    const auto iterator_to_string_closer = std::find_if(std::next(string_begin), code_end, [&escape_next_character] (const char& it) mutable -> bool 
                    { 
                        //Todo: Error handling
                        assert(!lcl::chars::is_newline(it) && it != 0);

                        switch (it)
                        {
                            case '\\': 
                            {
                                escape_next_character = true; 
                                return false;
                            }

                            case '"' : 
                            {
                                if (escape_next_character) 
                                { 
                                    escape_next_character = false;
                                    return false; 
                                } 
                                else 
                                { 
                                    return true; 
                                }
                            }

                            default: 
                            {
                                escape_next_character = false; 
                                return false;
                            }
                        }
                    });

                    //Todo: Error handling
                    assert(iterator_to_string_closer != code_end);

                    const auto string_end = std::next(iterator_to_string_closer);

                    tokens.emplace_back(lcl::token_type::string_literal, lcl::utils::string_view_from_iterator(string_begin, string_end));
                    code_iterator = string_end;

                    continue;
                }

                default: 
                {
                    if (lcl::chars::is_ascii_digit(*code_iterator))
                    {
                        const auto numeric_literal_begin = code_iterator;
                        const auto numeric_literal_end   = std::find_if_not(numeric_literal_begin, code_end, lcl::is_valid_mid_character_in_numeric_literal);

                        tokens.emplace_back(lcl::token_type::numeric_literal, lcl::utils::string_view_from_iterator(numeric_literal_begin, numeric_literal_end));
                        code_iterator = numeric_literal_end;
                    }
                    else if (lcl::is_valid_first_character_in_word(*code_iterator))
                    {
                        const auto word_literal_begin = code_iterator;
                        const auto word_literal_end   = std::find_if_not(word_literal_begin, code_end, lcl::is_valid_mid_character_in_word);

                        tokens.emplace_back(lcl::token_type::word, lcl::utils::string_view_from_iterator(word_literal_begin, word_literal_end));
                        code_iterator = word_literal_end;
                    }
                }
            }
        }

        return tokens;
    }
}