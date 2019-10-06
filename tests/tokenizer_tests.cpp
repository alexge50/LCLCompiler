#include <iostream>
#include <cassert>
#include <vector>
#include <string_view>

#include "tokenizer.hpp"

using namespace std::string_view_literals;

void test_tokenization_of_whitespace()
{
    const auto source = "    "sv;
    const auto result = lcl::get_tokens_of_source_code(source);
 
    assert(result.size() == 0);
}

void test_tokenization_of_newline()
{
    const auto source = "\n"sv;
    const auto result = lcl::get_tokens_of_source_code(source);
 
    assert(result.size() == 1);

    assert(result[0].type               == lcl::token_type::newline);
    assert(result[0].source_code.size() == source.size());
    assert(result[0].source_code        == source);
}

void test_tokenization_of_single_line_comment()
{
    const auto source = "////Test漢語"sv;
    const auto result = lcl::get_tokens_of_source_code(source);
 
    assert(result.size() == 1);

    assert(result[0].type               == lcl::token_type::comment);
    assert(result[0].source_code.size() == source.size());
    assert(result[0].source_code        == source);
}

void test_tokenization_of_multi_line_comment()
{
    const auto source = "/* //Test漢語 \n\n\n */"sv;
    const auto result = lcl::get_tokens_of_source_code(source);
 
    assert(result.size() == 1);

    assert(result[0].type               == lcl::token_type::comment);
    assert(result[0].source_code.size() == source.size());
    assert(result[0].source_code        == source);
    assert(result[0].is_multi_line_comment())
}

int main()
{
    std::cout << "Running Tokenizer Tests\n";
    
    test_tokenization_of_whitespace();
    test_tokenization_of_newline();
    test_tokenization_of_single_line_comment();
    
    std::cout << "Tokenizer tests run succesfully.";
}