#include <iostream>
#include "strict_fstream.hpp"

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        std::cerr
            << "Use: " << argv[0] << " file mode" << std::endl
            << "Synopsis: Open `file` as a strict_fstream::fstream object with given `mode`" << std::endl
            << "Modes:" << std::endl
            << "  in=" << std::ios_base::in << std::endl
            << "  out=" << std::ios_base::out << std::endl
            << "  app=" << std::ios_base::app << std::endl
            << "  ate=" << std::ios_base::ate << std::endl
            << "  trunc=" << std::ios_base::trunc << std::endl
            << "  binary=" << std::ios_base::binary << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::istringstream iss(argv[2]);
    int mode;
    iss >> mode;
    strict_fstream::fstream * fs_p = new strict_fstream::fstream(argv[1], static_cast< std::ios_base::openmode >(mode));
    delete fs_p;
}
