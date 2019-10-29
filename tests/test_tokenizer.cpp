#include <iostream>
#include <cassert>
#include <vector>
#include <string_view>

#include "tokenizer.hpp"
#include "std_utils.hpp"

using namespace std::string_view_literals;

void test_tokenization_of_whitespace()
{
    const auto source = "    \r\t"sv;
    const auto result = lcl::tokenize_code(source);
 
    assert(result.size() == 0);
}

void test_tokenization_of_newline()
{
    {
        const auto source = "\n"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type               == lcl::token_type::newline);
        assert(result[0].source_code.size() == source.size());
        assert(result[0].source_code        == source);
    }

    {
        const auto source = "\n \n        "sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 2);

        assert(result[0].type               == lcl::token_type::newline);
        assert(result[0].source_code.size() == 1);
        assert(result[0].source_code        == "\n");

        assert(result[1].type               == lcl::token_type::newline);
        assert(result[1].source_code.size() == 1);
        assert(result[1].source_code        == "\n");
    }
}

void test_tokenization_of_single_line_comment()
{
    const auto source = "////Test漢語"sv;
    const auto result = lcl::tokenize_code(source);
 
    assert(result.size() == 1);

    assert(result[0].type               == lcl::token_type::comment);
    assert(result[0].source_code.size() == source.size());
    assert(result[0].source_code        == source);
}

void test_tokenization_of_multi_line_comment()
{
    {
        const auto source = "/* //Test漢語 /* inner comment */ \n\n\n */"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type               == lcl::token_type::comment);
        assert(result[0].source_code.size() == source.size());
        assert(result[0].source_code        == source);
        assert(result[0].is_multi_line_comment());
    }

    {
        const auto source = "/* //Test漢語 /* inner comment */ // // /* // */\n\n\n */"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type               == lcl::token_type::comment);
        assert(result[0].source_code.size() == source.size());
        assert(result[0].source_code        == source);
        assert(result[0].is_multi_line_comment());
    }

    {
        const auto source = "//"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type               == lcl::token_type::comment);
        assert(result[0].source_code.size() == source.size());
        assert(result[0].source_code        == source);
    }
}

void test_tokenization_of_string()
{
    const auto source = "\"Test\""sv;
    const auto result = lcl::tokenize_code(source);

    assert(result.size() == 1);
    assert(result[0].type               == lcl::token_type::string_literal);
    assert(result[0].source_code.size() == source.size());
    assert(result[0].source_code        == source);
    assert(result[0].is_string_literal());
}

void test_tokenization_of_integer()
{
    {
        const auto source = "1801"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        assert(result[0].type               == lcl::token_type::numeric_literal);
        assert(result[0].source_code.size() == source.size());
        assert(result[0].source_code        == source);
        assert(result[0].is_int_literal());
    }

    {
        const auto source = "1801 83274"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 2);
        
        assert(result[0].type               == lcl::token_type::numeric_literal);
        assert(result[0].source_code.size() == "1801"sv.size());
        assert(result[0].source_code        == "1801"sv);
        assert(result[0].is_int_literal());

        assert(result[1].type               == lcl::token_type::numeric_literal);
        assert(result[1].source_code.size() == "83274"sv.size());
        assert(result[1].source_code        == "83274"sv);
        assert(result[1].is_int_literal());
    }
}

void test_tokenization_of_single_char_tokens()
{
    //Test sources with single char
    for (const auto source : lcl::single_char_tokens)
    {
        const auto result = lcl::tokenize_code(source);

        assert(lcl::utils::ssize(result) == 1);
        
        assert(result[0].type               == lcl::get_token_type_that_represents_char(source[0]));
        assert(result[0].source_code.size() == 1);
        assert(result[0].source_code        == source);
        assert(result[0].is_single_char_token());
    }

    //Test sources with multiple chars of the same type
    for (const auto it : lcl::single_char_tokens)
    {
        //Skip this one because `//` is a comment.
        if (it == "/")
        {
            continue;
        }

        const auto source = std::string { it } + std::string { it } + std::string { it };
        const auto result = lcl::tokenize_code(source);

        assert(lcl::utils::ssize(result) == 3);
        
        for (auto i = 0; i < lcl::utils::ssize(result); ++i)
        {
            assert(result[i].type               == lcl::get_token_type_that_represents_char(source[0]));
            assert(result[i].source_code.size() == 1);
            assert(result[i].source_code        == it);
            assert(result[i].is_single_char_token());
        }
    }
}

int main()
{
    std::cout << "Running Tokenizer Tests\n";
    
    test_tokenization_of_whitespace();
    test_tokenization_of_single_line_comment();
    test_tokenization_of_multi_line_comment();
    test_tokenization_of_string();
    test_tokenization_of_single_char_tokens();
    // test_tokenization_of_integer();
    
    std::cout << "Tokenizer tests run succesfully.";
}