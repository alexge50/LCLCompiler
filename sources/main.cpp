#include <iostream>
#include <cassert>

#include "fmt/core.h"
#include "fmt/format.h"
#include "magic_enum/magic_enum.hpp"

#include "tokenizer.hpp"

int main()
{
    fmt::print("Hello, {}!", "world");
    std::cout << magic_enum::enum_name(lcl::token_type::open_curly);

    return 0;
}