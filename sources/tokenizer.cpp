#include <cctype>
#include <algorithm>
#include <optional>
#include <cassert>
#include <functional>
#include <cassert>

#include <tl/expected.hpp>

#include <std_utils.hpp>
#include <tokenizer.hpp>
#include <chars.hpp>

namespace lcl
{
    [[nodiscard]] auto tokenize_code(const std::string_view& code) -> tl::expected<std::vector<lcl::token>, lcl::tokenizer_error>
    {
        const auto code_begin = std::cbegin(code);
        const auto code_end   = std::cend(code);

        auto tokens        = std::vector<lcl::token>{};
        auto code_iterator = code_begin;

        const auto add_one_char_token = [&] (const lcl::token_type tk_type) 
        { 
            tokens.emplace_back(tk_type, string_view_slice(code_iterator, std::next(code_iterator))); 
            code_iterator = std::next(code_iterator);
        };

        while (code_iterator != code_end)
        {
            switch (*code_iterator)
            {
                //One character tokens
                case '`':
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
                    tokens.emplace_back(lcl::get_token_type_that_represents_char(*code_iterator), string_view_slice(code_iterator, std::next(code_iterator))); 
                    code_iterator = std::next(code_iterator);
                    
                    continue;
                }

                case '\t':
                case '\r':
                case '\n':
                case ' ' :
                {
                    code_iterator = std::find_if_not(code_iterator, code_end, chars::is_white_space);
                    
                    continue;
                }

                case '/':
                {
                    const auto iterator_to_initial_forward_slash    = code_iterator;
                    const auto iterator_after_initial_forward_slash = std::next(iterator_to_initial_forward_slash);
                    const auto should_tokenize_comment              = iterator_after_initial_forward_slash != code_end && (*iterator_after_initial_forward_slash == '/' || *iterator_after_initial_forward_slash == '*');

                    if (!should_tokenize_comment)
                    {
                        tokens.emplace_back(lcl::token_type::forward_slash, string_view_slice(iterator_to_initial_forward_slash, 1));
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
                            
                            tokens.emplace_back(lcl::token_type::comment, string_view_slice(commend_begin, comment_end));
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
                            const auto expected_iterator_to_comment_closer_slash = [&] () -> tl::expected<std::string_view::const_iterator, lcl::tokenizer_error> 
                            {
                                //We ignore the initial `/*` so we start 2 chars ahead to look for the end `*/` of the comment.
                                const auto code_to_look_at_for_comment_closer_begin  = std::next(comment_begin, 2);
                                const auto code_to_look_at_for_comment_closer_length = std::distance(code_to_look_at_for_comment_closer_begin, code_end);  
                                
                                //We use this to count inner comment blocks. Eg: /* /* inner */ */
                                auto inner_comments_count = 0;
                                
                                //We look at 2 chars at a time, advance by one char. Eg: for "Test" we will look at the views: [ "Te", "es", "st" ]
                                for (auto i = 0; i < code_to_look_at_for_comment_closer_length - 1; ++i)
                                {
                                    const auto view = string_view_slice(std::next(code_to_look_at_for_comment_closer_begin, i), 2);

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

                                return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::multi_line_comment_not_closed, comment_begin }); 
                            }();

                            if (!expected_iterator_to_comment_closer_slash)
                            {
                                return tl::unexpected(expected_iterator_to_comment_closer_slash.error());
                            }

                            const auto comment_end = std::next(*expected_iterator_to_comment_closer_slash);

                            tokens.emplace_back(lcl::token_type::comment, string_view_slice(comment_begin, comment_end));
                            code_iterator = comment_end;

                            continue;
                        }
                    }
                }

                case '"':
                {
                    const auto string_begin = code_iterator;
                    
                    //This proc will return an iterator to the closing `"` or code_end if the string isn't closed properly.
                    //We start looking after the first `"`.
                    const auto expected_iterator_to_string_closer = [&] () -> tl::expected<std::string_view::const_iterator, lcl::tokenizer_error> 
                    {
                        //Used to mark if the next character should be escaped, such as `\"`
                        auto escape_next_character = false;

                        for (auto it = std::next(string_begin); it < code_end; it = std::next(it))
                        {
                            const auto char_at_it = *it;

                            if (chars::is_newline(char_at_it))
                            {
                                return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::newline_in_string_literal, string_begin });
                            }

                            if (char_at_it  == 0)
                            {
                                return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::null_character_in_string_literal, string_begin });
                            }

                            switch (char_at_it )
                            {
                                case '\\': 
                                {
                                    escape_next_character = true; 
                                    continue;
                                }

                                case '"' : 
                                {
                                    if (escape_next_character) 
                                    { 
                                        escape_next_character = false;
                                        continue; 
                                    } 
                                    else 
                                    { 
                                        return it;
                                    }
                                }

                                default: 
                                {
                                    escape_next_character = false; 
                                    continue;
                                }
                            }
                        }

                        return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::string_literal_not_closed_properly, string_begin });
                    }();

                    if (!expected_iterator_to_string_closer)
                    {
                        return tl::unexpected(expected_iterator_to_string_closer.error());
                    }

                    const auto string_end = std::next(*expected_iterator_to_string_closer);

                    tokens.emplace_back(lcl::token_type::string_literal, string_view_slice(string_begin, string_end));
                    code_iterator = string_end;

                    continue;
                }

                default: 
                {
                    if (chars::is_ascii_digit(*code_iterator))
                    {
                        /*
                        1 - Number
                        1.0 - Number
                        1.0. - Number dot
                        1.0.0 - Number dot number
                        */
                        
                        const auto numeric_literal_begin = code_iterator;
                        
                        auto dot_encountered     = false;
                        auto prev_was_dot        = false;
                        auto prev_was_underscore = false;
                        auto it                  = numeric_literal_begin;

                        for (; it < code_end; it = std::next(it))
                        {
                            if (*it == '.')
                            {
                                //Here we take the numbers up to the previous dot as a numeric literal and advance the code_iterator up to the previous dot and keep tokenizing from there
                                //Eg: 1.. -> [numeric_literal, dot, dot]
                                if (prev_was_dot)
                                {
                                    prev_was_dot = false;

                                    const auto iterator_to_prev_dot = std::prev(it);
                                    tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, iterator_to_prev_dot));
                                    code_iterator = iterator_to_prev_dot;
                                    break;
                                }
                                
                                //We end the number here and keep tokenizing from this dot
                                //Eg: 1.0.0 -> [numeric_literal, dot, numeric_literal]
                                if (dot_encountered) 
                                {
                                    tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, it));
                                    code_iterator = it;
                                    break;
                                }

                                prev_was_dot    = true;
                                dot_encountered = true;
                            }
                            else if (*it == '_')
                            {
                                prev_was_underscore = true;
                            }
                            else if (chars::is_ascii_digit(*it))
                            {
                                prev_was_dot        = false;
                                prev_was_underscore = false;
                            }
                            else //if not valid character in number
                            {
                                //This dot should not be parsed as part of the number
                                if (prev_was_dot)
                                {
                                    const auto iterator_prev_was_dot = std::prev(it);
                                    tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, iterator_prev_was_dot));
                                    code_iterator = iterator_prev_was_dot;
                                }
                                else if (prev_was_underscore)
                                {
                                    return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::numeric_literal_ends_with_underscore, numeric_literal_begin });
                                }
                                else //if unexpected character
                                {
                                    if (!lcl::is_single_char_represented_by_token_type(*it) && !chars::is_white_space(*it))
                                    {
                                        return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::numeric_literal_contains_unexpected_character, numeric_literal_begin });
                                    }

                                    tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, it));
                                    code_iterator = it;
                                    break;
                                }
                            }
                        }

                        //Here we take the numbers up to the previous dot as a numeric literal and advance the code_iterator up to the previous dot and keep tokenizing from there
                        //Eg: 1.. -> [numeric_literal, dot, dot]
                        if (prev_was_dot)
                        {
                            const auto iterator_to_prev_dot = std::prev(it);
                            tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, iterator_to_prev_dot));
                            code_iterator = iterator_to_prev_dot;
                        }
                        else if (prev_was_underscore)
                        {
                            return tl::unexpected(lcl::tokenizer_error { lcl::tokenizer_error_type::numeric_literal_ends_with_underscore, numeric_literal_begin });
                        }
                        else if (it == code_end)
                        {
                            //If we reached the end of the code without problem
                            tokens.emplace_back(lcl::token_type::numeric_literal, string_view_slice(numeric_literal_begin, code_end));
                            code_iterator = code_end;
                        }
                    }
                    else if (lcl::is_valid_first_character_in_word(*code_iterator))
                    {
                        const auto word_literal_begin = code_iterator;
                        const auto word_literal_end   = std::find_if_not(word_literal_begin, code_end, lcl::is_valid_mid_character_in_word);

                        tokens.emplace_back(lcl::token_type::word, string_view_slice(word_literal_begin, word_literal_end));
                        code_iterator = word_literal_end;
                    }
                }
            }
        }

        return tokens;
    }
}