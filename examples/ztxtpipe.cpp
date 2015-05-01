#include <iostream>
#include <string>
#include "zstr.hpp"

int main()
{
    //
    // Create zstr::istream feeding off std::cin.
    //
    zstr::istream is(std::cin);
    //
    // Turn on error reporting (otherwise, zstream exceptions are hidden).
    //
    is.exceptions(std::ios_base::badbit);
    //
    // Main loop
    //
    std::string s;
    while (getline(is, s))
    {
        std::cout << s << std::endl;
    }
}
