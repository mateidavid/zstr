#ifndef __STRICT_FSTREAM_HPP
#define __STRICT_FSTREAM_HPP

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

namespace strict_fstream
{

/**
 * Wrapper for an std::fstream object.
 *
 * The main purpose is to perform a check on open(), and if necessary, to stop the program
 * with an informative error message.
 */
class fstream
    : public std::fstream
{
public:
    fstream() = default;
    fstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
    {
        open(filename, mode);
    }

    void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
    {
        if ((mode & std::ios_base::trunc) and not (mode & std::ios_base::out))
        {
            std::cerr << "strict_fstream::fstream::open('" << filename << "'): mode error: trunc but not out" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        else if ((mode & std::ios_base::trunc) and (mode & std::ios_base::app))
        {
            std::cerr << "strict_fstream::fstream::open('" << filename << "'): mode error: trunc and app" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::fstream::open(filename, mode);
        bool open_failed = fail();
        bool peek_failed = (mode & std::ios_base::in) and (peek(), fail());
        if (open_failed or peek_failed)
        {
            std::ostringstream oss;
            oss << "strict_fstream::fstream::open('" << filename << "'," << mode_to_string(mode) << ")";
            perror(oss.str().c_str());
            std::exit(EXIT_FAILURE);
        }
        clear();
        exceptions(std::ios_base::badbit);
    }

    static std::string mode_to_string(std::ios_base::openmode mode)
    {
        static const int n_modes = 6;
        static const std::ios_base::openmode mode_val_v[n_modes] =
            {
                std::ios_base::in,
                std::ios_base::out,
                std::ios_base::app,
                std::ios_base::ate,
                std::ios_base::trunc,
                std::ios_base::binary
            };

        static const char * mode_name_v[n_modes] =
            {
                "in",
                "out",
                "app",
                "ate",
                "trunc",
                "binary"
            };
        std::string res;
        for (size_t i = 0; i < n_modes; ++i)
        {
            if (mode & mode_val_v[i])
            {
                res += (not res.empty()? "|" : "");
                res += mode_name_v[i];
            }
        }
        return res;
    }
}; // class fstream

} // namespace strict_fstream

#endif
