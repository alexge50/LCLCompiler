#include <iostream>

#include "tokenizer.hpp"
#include "magic_enum.hpp"
#include "fmt/core.h"
#include "fmt/format.h"

int main()
{
    fmt::print("Hello, {}!", "world");
    std::cout << magic_enum::enum_name(lcl::token_type::open_curly);

    return 0;
}