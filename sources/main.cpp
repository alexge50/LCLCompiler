#include <iostream>

#include "tokenizer.hpp"
#include "magic_enum.hpp"

int main()
{
    std::cout << magic_enum::enum_name(lcl::token_type::open_curly);

    return 0;
}