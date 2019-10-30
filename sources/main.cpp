#include <cassert>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <gsl/span>

#include <tokenizer.hpp>
#include <ast.hpp>

using namespace std::string_view_literals;

auto main() -> int
{
    const auto code                = "import Print: print, printf;"sv;
    const auto tokens              = lcl::tokenize_code(code).value();
    const auto ast                 = lcl::import_statement_ast { code, tokens };

    for (const auto& it : ast.module_imports())
    {
        fmt::print("{} ", it);
    }

    return 0;
}