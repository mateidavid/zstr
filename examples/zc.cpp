#include <iostream>
#include <string>
#include "zstr.hpp"


int main(int argc, char * argv[])
{
    // turn on error reporting
    std::cin.exceptions(std::ifstream::badbit);
    std::cout.exceptions(std::ifstream::badbit);
    // construct input zstream
    std::istream * is_p = nullptr;
    zstr::streambuf * zsbuf_p = nullptr;
    if (argc > 1 and std::string(argv[1]) == "-1")
    {
        std::clog << "construct zstr::streambuf from std::streambuf, wrap it in std::istream" << std::endl;
        zsbuf_p = new zstr::streambuf(std::cin.rdbuf());
        is_p = new std::istream(zsbuf_p);
    }
    else if (argc > 1 and std::string(argv[1]) == "-2")
    {
        std::clog << "construct zstr::istream from std::streambuf, convert it to std::istream" << std::endl;
        is_p = new zstr::istream(std::cin.rdbuf());
    }
    else
    {
        std::clog << "construct zstr::istream from std::istream, convert it to std::istream" << std::endl;
        is_p = new zstr::istream(std::cin);
    }
    assert(is_p);
    is_p->exceptions(std::ifstream::badbit);
    // main loop
    const std::streamsize buff_size = 1 << 16;
    char * buff = new char [buff_size];
    while (true)
    {
        is_p->read(buff, buff_size);
        std::streamsize cnt = is_p->gcount();
        if (cnt == 0) break;
        std::cout.write(buff, cnt);
    }
    delete [] buff;
    // deallocate std::istream and zstr::streambuf (if allocated)
    delete is_p;
    if (zsbuf_p) delete zsbuf_p;
}
