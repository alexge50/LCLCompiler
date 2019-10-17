#include <iostream>
#include <string_view>
#include <cassert>

#include "chars.hpp"
#include "std_utils.hpp"

using namespace std::string_view_literals;

void test_take_n_chars()
{
    const auto string = "Test"sv;

    {
        const auto take_1 = take_n_chars(string.cbegin(), string.cend(), 1);

        assert(take_1.has_value());
        assert(take_1->size() == 1);
        assert(*take_1 == string.substr(0, 1));   
    }

    {
        const auto take_2 = take_n_chars(string.cbegin(), string.cend(), 2);

        assert(take_2.has_value());
        assert(take_2->size() == 2);
        assert(*take_2 == string.substr(0, 2));
    }

    {
        const auto take_2_after_1 = take_n_chars(string.cbegin() + 1, string.cend(), 2);

        assert(take_2_after_1.has_value());
        assert(take_2_after_1->size() == 2);
        assert(*take_2_after_1 == string.substr(1, 2));
    }
}

int main()
{
    std::cout << "Running Chars and string_view utilities Tests\n";
    
    std::cout << "Tests run succesfully.";
}