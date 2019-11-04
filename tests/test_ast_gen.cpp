#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <tl/expected.hpp>

#include <tokenizer.hpp>
#include <std_utils.hpp>
#include <ast.hpp>

using namespace std::string_view_literals;

TEST_CASE("Import statement ast gen", "[ast]")
{
    const auto code   = "import Test"sv;
    const auto tokens = lcl::tokenize_code(code).get();
    const auto result = lcl::generate_ast(code, tokens);
}