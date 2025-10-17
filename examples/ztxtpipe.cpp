#include <iostream>
#include <string>
#include "zstr.hpp"

#ifdef _WIN32
#include <io.h>     // for _setmode
#include <fcntl.h>  // for _O_BINARY
#endif


int main()
{
#ifdef _WIN32
    // Don't touch line endings
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    //
    // Create zstr::istream feeding off std::cin.
    //
    zstr::istream is(std::cin);
    //
    // Main loop
    //
    std::string s;
    while (getline(is, s))
    {
        std::cout << s << std::endl;
    }
}
