#include <cassert>

#include <fmt/core.h>

#include <tokenizer.hpp>
#include <std_utils.hpp>

using namespace std::string_view_literals;

void test_tokenization_of_whitespace()
{
    const auto source = "    \r\t"sv;
    const auto result = lcl::tokenize_code(source);
 
    assert(result.size() == 0);
}

void test_tokenization_of_single_line_comment()
{
    const auto source = "////Test漢語"sv;
    const auto result = lcl::tokenize_code(source);
 
    assert(result.size() == 1);

    assert(result[0].type        == lcl::token_type::comment);
    assert(result[0].code.size() == source.size());
    assert(result[0].code        == source);
}

void test_tokenization_of_multi_line_comment()
{
    {
        const auto source = "/* //Test漢語 /* inner comment */ \n\n\n */"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type        == lcl::token_type::comment);
        assert(result[0].code.size() == source.size());
        assert(result[0].code        == source);
        assert(result[0].is_multi_line_comment());
    }

    {
        const auto source = "/* //Test漢語 /* inner comment */ // // /* // */\n\n\n */"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type        == lcl::token_type::comment);
        assert(result[0].code.size() == source.size());
        assert(result[0].code        == source);
        assert(result[0].is_multi_line_comment());
    }

    {
        const auto source = "//"sv;
        const auto result = lcl::tokenize_code(source);
    
        assert(result.size() == 1);

        assert(result[0].type        == lcl::token_type::comment);
        assert(result[0].code.size() == source.size());
        assert(result[0].code        == source);
    }
}

void test_tokenization_of_string()
{
    const auto source = "\"Test\""sv;
    const auto result = lcl::tokenize_code(source);

    assert(result.size() == 1);
    assert(result[0].type        == lcl::token_type::string_literal);
    assert(result[0].code.size() == source.size());
    assert(result[0].code        == source);
    assert(result[0].is_string_literal());
}

void test_tokenization_of_numeric_literals()
{
    {
        const auto source = "1801"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        assert(result[0].type        == lcl::token_type::numeric_literal);
        assert(result[0].code.size() == source.size());
        assert(result[0].code        == source);
        assert(result[0].is_numeric_literal());
        assert(result[0].is_int_literal());
    }

    {
        const auto source = "1801 83274"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 2);
        
        assert(result[0].type        == lcl::token_type::numeric_literal);
        assert(result[0].code.size() == "1801"sv.size());
        assert(result[0].code        == "1801"sv);
        assert(result[0].is_numeric_literal());
        assert(result[0].is_int_literal());

        assert(result[1].type        == lcl::token_type::numeric_literal);
        assert(result[1].code.size() == "83274"sv.size());
        assert(result[1].code        == "83274"sv);
        assert(result[0].is_numeric_literal());
        assert(result[1].is_int_literal());
    }

    {
        const auto source = "1801_83274"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        
        assert(result[0].type        == lcl::token_type::numeric_literal);
        assert(result[0].code.size() == "1801_83274"sv.size());
        assert(result[0].code        == "1801_83274"sv);
        assert(result[0].is_numeric_literal());
        assert(result[0].is_int_literal());
    }

    {
        const auto source = "1801.83274"sv;
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        
        assert(result[0].type        == lcl::token_type::numeric_literal);
        assert(result[0].code.size() == "1801.83274"sv.size());
        assert(result[0].code        == "1801.83274"sv);
        assert(result[0].is_numeric_literal());
        assert(result[0].is_float_literal());
    }
}

void test_tokenization_of_single_char_tokens()
{
    //Test sources with single char
    for (const auto source : lcl::single_char_tokens)
    {
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        
        assert(result[0].type        == lcl::get_token_type_that_represents_char(source[0]));
        assert(result[0].code.size() == 1);
        assert(result[0].code        == source);
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

        assert(result.size() == 3);
        
        for (auto i = 0; i < lcl::utils::ssize(result); ++i)
        {
            assert(result[i].type        == lcl::get_token_type_that_represents_char(source[0]));
            assert(result[i].code.size() == 1);
            assert(result[i].code        == it);
            assert(result[i].is_single_char_token());
        }
    }
}

void test_tokenization_of_words()
{
    {
        const auto source = "test";

        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        
        assert(result[0].type        == lcl::token_type::word);
        assert(result[0].code.size() == 1);
        assert(result[0].code        == source);
        assert(result[0].is_word());
        assert(result[0].is_identifier());
    }

    for (const auto source : lcl::keywords)
    {
        const auto result = lcl::tokenize_code(source);

        assert(result.size() == 1);
        
        assert(result[0].type        == lcl::token_type::word);
        assert(result[0].code.size() == 1);
        assert(result[0].code        == source);
        assert(result[0].is_word());
        assert(result[0].is_keyword());
    }
}

void test_tokenization_of_complex_example()
{
    const auto source = R"source_code(
        import Print: *;

        main :: () -> void
        {
            hello := 1;

            while (hello == 1)
            {
                print("Hello Sailor!");
            }
        }
    )source_code";

    const auto expected_token_types = lcl::utils::make_array
    (
        lcl::token_type::word, lcl::token_type::word, lcl::token_type::colon, lcl::token_type::star, lcl::token_type::semicolon,  
   
        lcl::token_type::word, lcl::token_type::colon, lcl::token_type::colon, lcl::token_type::open_parans, lcl::token_type::close_parans, lcl::token_type::minus, lcl::token_type::right_arrow, lcl::token_type::word,  
        lcl::token_type::open_curly, 
        lcl::token_type::word, lcl::token_type::colon, lcl::token_type::equal, lcl::token_type::numeric_literal, lcl::token_type::semicolon, 
        
        lcl::token_type::word, lcl::token_type::open_parans, lcl::token_type::word, lcl::token_type::equal, lcl::token_type::equal, lcl::token_type::numeric_literal, lcl::token_type::close_parans, 
        lcl::token_type::open_curly, 
        lcl::token_type::word, lcl::token_type::open_parans, lcl::token_type::string_literal, lcl::token_type::close_parans, lcl::token_type::semicolon, 
        lcl::token_type::close_curly, 
        lcl::token_type::close_curly
    );

    const auto result = lcl::tokenize_code(source);

    assert(result.size() == expected_token_types.size());

    const auto tokens_as_expected = [&] ()
    {
        for (auto i = 0; i < lcl::utils::ssize(result); ++i)
        {
            if (result[i].type != expected_token_types[i])
            {
                return false;
            }
        }

        return true;
    }();

    assert(tokens_as_expected);

    for (auto token_index = 0; token_index < lcl::utils::ssize(result); ++token_index)
    {
        const auto& token = result[token_index];

        if (token.is_word())
        {
            switch (token_index)
            {
                case  0: assert(token.is_keyword()    && token.code == "import"); break;
                case  2: assert(token.is_identifier() && token.code == "Print");  break;
                case  6: assert(token.is_identifier() && token.code == "main");   break;
                case 13: assert(token.is_keyword()    && token.code == "void");   break;
                case 15: assert(token.is_identifier() && token.code == "hello");  break;
                case 20: assert(token.is_keyword()    && token.code == "while");  break;
                case 22: assert(token.is_identifier() && token.code == "hello");  break;
                case 28: assert(token.is_identifier() && token.code == "print");  break;
            }
        }
    }
}

int main()
{
    fmt::print("Running Tokenizer Tests\n");
    
    test_tokenization_of_whitespace();
    test_tokenization_of_single_line_comment();
    test_tokenization_of_multi_line_comment();
    test_tokenization_of_string();
    test_tokenization_of_single_char_tokens();
    test_tokenization_of_numeric_literals();
    test_tokenization_of_complex_example();
    
    fmt::print("Tokenizer tests run succesfully.");
}