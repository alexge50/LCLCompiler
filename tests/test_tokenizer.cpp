#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <tokenizer.hpp>
#include <std_utils.hpp>

using namespace std::string_view_literals;

TEST_CASE("Tokenization of whitespace", "[tokenizer]")
{
    const auto code = "    \r\t"sv;
    const auto result = lcl::tokenize_code(code);
 
    REQUIRE(result.size() == 0);    
}

TEST_CASE("Tokenization of single line comment", "[tokenizer]")
{
    SECTION("Empty comment")
    {
        const auto code = "//"sv;
        const auto result = lcl::tokenize_code(code);
    
        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
    }

    SECTION("Comment with text")
    {
        const auto code = "//Test"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
    }

    SECTION("Comment with extra forward slashes")
    {
        const auto code = "////Test"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
    }

    SECTION("Comment with weird characters")
    {
        const auto code = "////Test漢語"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
    }

    SECTION("Single line comment with multi line comment opener")
    {
        const auto code = "////Test漢語 /* "sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
    }

    SECTION("Two single line comments")
    {
        const auto code                = "//Test \n //Test"sv;
        const auto first_comment_code  = "//Test "sv;
        const auto second_comment_code = "//Test"sv;

        const auto result = lcl::tokenize_code(code);


        REQUIRE(result.size() == 2);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == first_comment_code);

        REQUIRE(result[1].type == lcl::token_type::comment);
        REQUIRE(result[1].code == second_comment_code);
    }
}

TEST_CASE("Tokenization of multi line comment", "[tokenizer]")
{
    SECTION("Multi line comment with no text")
    {
        const auto code = "/* \n\n\n */"sv;
        const auto result = lcl::tokenize_code(code);
    
        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
        REQUIRE(result[0].is_multi_line_comment());
    }

    SECTION("Multi line comment with a regular nested comment")
    {
        const auto code = "/* //Test漢語 \n\n\n */"sv;
        const auto result = lcl::tokenize_code(code);
    
        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
        REQUIRE(result[0].is_multi_line_comment());
    }

    SECTION("Multi line comment with nested multi line comments")
    {
        const auto code = "/* //Test漢語 /* inner comment */ // // /* // */\n\n\n */"sv;
        const auto result = lcl::tokenize_code(code);
    
        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::comment);
        REQUIRE(result[0].code == code);
        REQUIRE(result[0].is_multi_line_comment());
    }
}

TEST_CASE("Tokenization of string", "[tokenizer]")
{
    const auto code = "\"Test\""sv;
    const auto result = lcl::tokenize_code(code);

    REQUIRE(result.size() == 1);

    REQUIRE(result[0].type == lcl::token_type::string_literal);
    REQUIRE(result[0].code == code);
    REQUIRE(result[0].is_string_literal());
}

TEST_CASE("Tokenization of numeric literals", "[tokenizer]")
{
    SECTION("Single numeric literal")
    {
        const auto code = "1801"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);

        REQUIRE(result[0].type == lcl::token_type::numeric_literal);
        REQUIRE(result[0].code == code);
        REQUIRE(result[0].is_numeric_literal());
        REQUIRE(result[0].is_int_literal());
    }

    SECTION("Two numeric literals")
    {
        const auto code = "1801 83274"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 2);
        
        REQUIRE(result[0].type == lcl::token_type::numeric_literal);
        REQUIRE(result[0].code == "1801"sv);
        REQUIRE(result[0].is_numeric_literal());
        REQUIRE(result[0].is_int_literal());

        REQUIRE(result[1].type == lcl::token_type::numeric_literal);
        REQUIRE(result[1].code == "83274"sv);
        REQUIRE(result[1].is_numeric_literal());
        REQUIRE(result[1].is_int_literal());
    }

    SECTION("Numeric literal with underscore")
    {
        const auto code = "1801_83274"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);
        
        REQUIRE(result[0].type == lcl::token_type::numeric_literal);
        REQUIRE(result[0].code == "1801_83274"sv);
        REQUIRE(result[0].is_numeric_literal());
        REQUIRE(result[0].is_int_literal());
    }

    SECTION("Floating point literal")
    {
        const auto code = "1801.83274"sv;
        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);
        
        REQUIRE(result[0].type == lcl::token_type::numeric_literal);
        REQUIRE(result[0].code == "1801.83274"sv);
        REQUIRE(result[0].is_numeric_literal());
        REQUIRE(result[0].is_float_literal());
    }
}

TEST_CASE("Tokenization of single char tokens", "[tokenizer]")
{
    SECTION("Single char tokens")
    {
        for (const auto code : lcl::single_char_tokens)
        {
            const auto result = lcl::tokenize_code(code);

            REQUIRE(result.size() == 1);
            
            REQUIRE(result[0].type        == lcl::get_token_type_that_represents_char(code[0]));
            REQUIRE(result[0].code.size() == 1);
            REQUIRE(result[0].code        == code);
            REQUIRE(result[0].is_single_char_token());
        }
    }

    //Test codes with multiple chars of the same type
    SECTION("Single char tokens repeated")
    {
        for (const auto it : lcl::single_char_tokens)
        {
            //Skip this one because `//` is a comment.
            if (it == "/")
            {
                continue;
            }

            const auto code = std::string { it } + std::string { it } + std::string { it };
            const auto result = lcl::tokenize_code(code);

            REQUIRE(result.size() == 3);
            
            for (auto i = 0; i < lcl::utils::ssize(result); ++i)
            {
                REQUIRE(result[i].type == lcl::get_token_type_that_represents_char(code[0]));
                REQUIRE(result[i].code == it);
                REQUIRE(result[i].is_single_char_token());
            }
        }
    }
}

TEST_CASE("Tokenization of words", "[tokenizer]")
{
    SECTION("Single word")
    {
        const auto code = "test";

        const auto result = lcl::tokenize_code(code);

        REQUIRE(result.size() == 1);
        
        REQUIRE(result[0].type == lcl::token_type::word);
        REQUIRE(result[0].code == code);
        REQUIRE(result[0].is_word());
        REQUIRE(result[0].is_identifier());
    }

    SECTION("All keywords")
    {
        for (const auto code : lcl::keywords)
        {
            const auto result = lcl::tokenize_code(code);

            REQUIRE(result.size() == 1);
            
            REQUIRE(result[0].type == lcl::token_type::word);
            REQUIRE(result[0].code == code);
            REQUIRE(result[0].is_word());
            REQUIRE(result[0].is_keyword());
        }
    }
}

TEST_CASE("Tokenization of complex example", "[tokenizer]")
{
    const auto code = R"code_code(
        import Print: *;

        main :: () -> void
        {
            hello := 1;

            while (hello == 1)
            {
                print("Hello Sailor!");
            }
        }
    )code_code";

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

    const auto result = lcl::tokenize_code(code);

    SECTION("Tokens match")
    {
        REQUIRE(result.size() == expected_token_types.size());

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

        REQUIRE(tokens_as_expected);
    }

    SECTION("Test words match")
    {
        for (auto token_index = 0; token_index < lcl::utils::ssize(result); ++token_index)
        {
            const auto& token = result[token_index];

            if (token.is_word())
            {
                switch (token_index)
                {
                    case  0: REQUIRE(token.is_keyword());    REQUIRE(token.code == "import"); break;
                    case  2: REQUIRE(token.is_identifier()); REQUIRE(token.code == "Print");  break;
                    case  6: REQUIRE(token.is_identifier()); REQUIRE(token.code == "main");   break;
                    case 13: REQUIRE(token.is_keyword());    REQUIRE(token.code == "void");   break;
                    case 15: REQUIRE(token.is_identifier()); REQUIRE(token.code == "hello");  break;
                    case 20: REQUIRE(token.is_keyword());    REQUIRE(token.code == "while");  break;
                    case 22: REQUIRE(token.is_identifier()); REQUIRE(token.code == "hello");  break;
                    case 28: REQUIRE(token.is_identifier()); REQUIRE(token.code == "print");  break;
                }
            }
        }
    }
}