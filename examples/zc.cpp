#include <iostream>
#include <string>
#include "zstr.hpp"

void cat_stream(std::istream& is, std::ostream& os)
{
    const std::streamsize buff_size = 1 << 16;
    char * buff = new char [buff_size];
    while (true)
    {
        is.read(buff, buff_size);
        std::streamsize cnt = is.gcount();
        if (cnt == 0) break;
        os.write(buff, cnt);
    }
    delete [] buff;
}

int main(int argc, char * argv[])
{
    //
    // With no arguments (argc == 1), act as if "-" was given, and cat stdin
    //
    for (int i = argc > 1? 1 : 0; i < argc; ++i)
    {
        //
        // Create input stream object; "-" is used for stdin
        //
        std::istream * is_p;
        if ((i == 0) or (i > 0 and std::string("-") == argv[i]))
        {
            is_p = new zstr::istream(std::cin);
        }
        else
        {
            is_p = new zstr::ifstream(argv[i]);
        }
        //
        // Cat stream
        //
        cat_stream(*is_p, std::cout);
        //
        // Deallocate input stream object
        //
        delete is_p;
    }
}
