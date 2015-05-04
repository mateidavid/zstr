//---------------------------------------------------------
// Copyright 2015 Ontario Institute for Cancer Research
// Written by Matei David (matei@cs.toronto.edu)
//---------------------------------------------------------

// Reference:
// http://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c

#ifndef __ZSTR_HPP
#define __ZSTR_HPP

#include <cassert>
#include <fstream>
#include <sstream>
#include <zlib.h>
#include <sys/stat.h>

namespace zstr
{

/// Exception class thrown by failed zlib operations.
class Exception
    : public std::exception
{
public:
    Exception(z_stream * zstrm_p, int ret)
        : _msg("zlib: ")
    {
        switch (ret)
        {
        case Z_STREAM_ERROR:
            _msg += "Z_STREAM_ERROR: ";
            break;
        case Z_DATA_ERROR:
            _msg += "Z_DATA_ERROR: ";
            break;
        case Z_MEM_ERROR:
            _msg += "Z_MEM_ERROR: ";
            break;
        case Z_VERSION_ERROR:
            _msg += "Z_VERSION_ERROR: ";
            break;
        default:
            std::ostringstream oss;
            oss << ret;
            _msg += "[" + oss.str() + "]: ";
            break;
        }
        _msg += zstrm_p->msg;
    }
    const char * what() const noexcept { return _msg.c_str(); }
private:
    std::string _msg;
}; // class Exception

namespace detail
{

class z_stream_wrapper
    : public z_stream
{
public:
    z_stream_wrapper()
    {
        this->zalloc = Z_NULL;
        this->zfree = Z_NULL;
        this->opaque = Z_NULL;
        this->avail_in = 0;
        this->next_in = Z_NULL;
        int ret = inflateInit2(this, 15+32);
        if (ret != Z_OK) throw Exception(this, ret);
    }
    ~z_stream_wrapper()
    {
        inflateEnd(this);
    }
}; // class z_stream_wrapper

} // namespace detail

class streambuf
    : public std::streambuf
{
public:
    streambuf(std::streambuf * _sbuf_p,
              std::streamsize _buff_size = default_buff_size, bool _auto_detect = true)
        : sbuf_p(_sbuf_p),
          zstrm_p(nullptr),
          buff_size(_buff_size),
          auto_detect(_auto_detect),
          auto_detect_run(false),
          is_text(false)
    {
        assert(sbuf_p);
        in_buff = new char [buff_size];
        in_buff_start = in_buff;
        in_buff_end = in_buff;
        out_buff = new char [buff_size];
        setg(out_buff, out_buff, out_buff);
    }

    streambuf(const streambuf &) = delete;
    streambuf(streambuf &&) = default;
    streambuf & operator = (const streambuf &) = delete;
    streambuf & operator = (streambuf &&) = default;

    virtual ~streambuf()
    {
        delete [] in_buff;
        delete [] out_buff;
        if (zstrm_p) delete zstrm_p;
    }

    virtual std::streambuf::int_type underflow()
    {
        if (this->gptr() == this->egptr())
        {
            // pointers for free region in output buffer
            char * out_buff_free_start = out_buff;
            do
            {
                // read more input if none available
                if (in_buff_start == in_buff_end)
                {
                    // empty input buffer: refill from the start
                    in_buff_start = in_buff;
                    std::streamsize sz = sbuf_p->sgetn(in_buff, buff_size);
                    in_buff_end = in_buff + sz;
                    if (in_buff_end == in_buff_start) break; // end of input
                }
                // auto detect if the stream contains text or deflate data
                if (auto_detect and not auto_detect_run)
                {
                    auto_detect_run = true;
                    unsigned char b0 = *reinterpret_cast< unsigned char * >(in_buff_start);
                    unsigned char b1 = *reinterpret_cast< unsigned char * >(in_buff_start + 1);
                    // Ref:
                    // http://en.wikipedia.org/wiki/Gzip
                    // http://stackoverflow.com/questions/9050260/what-does-a-zlib-header-look-like
                    is_text = not (in_buff_start + 2 <= in_buff_end
                                   and ((b0 == 0x1F and b1 == 0x8B)         // gzip header
                                        or (b0 == 0x78 and (b1 == 0x01      // zlib header
                                                            or b1 == 0x9C
                                                            or b1 == 0xDA))));
                }
                if (is_text)
                {
                    // simply swap in_buff and out_buff, and adjust pointers
                    assert(in_buff_start == in_buff);
                    std::swap(in_buff, out_buff);
                    out_buff_free_start = in_buff_end;
                    in_buff_start = in_buff;
                    in_buff_end = in_buff;
                }
                else
                {
                    // run inflate() on input
                    if (not zstrm_p) zstrm_p = new detail::z_stream_wrapper();
                    zstrm_p->next_in = reinterpret_cast< decltype(zstrm_p->next_in) >(in_buff_start);
                    zstrm_p->avail_in = in_buff_end - in_buff_start;
                    zstrm_p->next_out = reinterpret_cast< decltype(zstrm_p->next_out) >(out_buff_free_start);
                    zstrm_p->avail_out = (out_buff + buff_size) - out_buff_free_start;
                    int ret = inflate(zstrm_p, Z_NO_FLUSH);
                    // process return code
                    if (ret != Z_OK and ret != Z_STREAM_END) throw Exception(zstrm_p, ret);
                    // update in&out pointers following inflate()
                    in_buff_start = reinterpret_cast< decltype(in_buff_start) >(zstrm_p->next_in);
                    in_buff_end = in_buff_start + zstrm_p->avail_in;
                    out_buff_free_start = reinterpret_cast< decltype(out_buff_free_start) >(zstrm_p->next_out);
                    assert(out_buff_free_start + zstrm_p->avail_out == out_buff + buff_size);
                    // if stream ended, deallocate inflator
                    if (ret == Z_STREAM_END)
                    {
                        delete zstrm_p;
                        zstrm_p = nullptr;
                    }
                }
            } while (out_buff_free_start == out_buff);
            // 2 exit conditions:
            // - end of input: there might or might not be output available
            // - out_buff_free_start != out_buff: output available
            this->setg(out_buff, out_buff, out_buff_free_start);
        }
        return this->gptr() == this->egptr()
            ? std::char_traits<char>::eof()
            : std::char_traits<char>::to_int_type(*this->gptr());
    }

private:
    std::streambuf * sbuf_p;
    char * in_buff;
    char * in_buff_start;
    char * in_buff_end;
    char * out_buff;
    detail::z_stream_wrapper * zstrm_p;
    std::streamsize buff_size;
    bool auto_detect;
    bool auto_detect_run;
    bool is_text;

    static const std::streamsize default_buff_size = 1 << 20;
}; // class streambuf

class istream
    : public std::istream
{
public:
    istream(std::istream & is)
        : std::istream(new streambuf(is.rdbuf()))
    {
        exceptions(std::ios_base::badbit);
    }

    explicit istream(std::streambuf * sbuf_p)
        : std::istream(new streambuf(sbuf_p))
    {
        exceptions(std::ios_base::badbit);
    }

    istream(const istream &) = delete;
    istream(istream &&) = default;
    istream & operator = (const istream &) = delete;
    istream & operator = (istream &&) = default;

    virtual ~istream()
    {
        delete rdbuf();
    }
}; // class istream

namespace detail
{

struct filebuf_holder
{
    filebuf_holder(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
    {
        struct stat st_buf;
        int status = stat(filename.c_str(), &st_buf);
        if (status != 0)
        {
            std::string msg;
            msg += "zstr::ifstream error: ";
            msg += filename;
            perror(msg.c_str());
            std::exit(EXIT_FAILURE);
        }
        else if (S_ISDIR(st_buf.st_mode))
        {
            std::cerr << "zstr::ifstream error: " << filename << ": is a directory" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        auto res = _fb.open(filename, mode);
        if (not res)
        {
            std::string msg;
            msg += "zstr::ifstream error: ";
            msg += filename;
            perror(msg.c_str());
            std::exit(EXIT_FAILURE);
        }
    }
    std::filebuf _fb;
}; // class filebuf_holder

} // namespace detail


class ifstream
    : private detail::filebuf_holder,
      public std::istream
{
public:
    explicit ifstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
        : detail::filebuf_holder(filename, mode),
          std::istream(new streambuf(&this->_fb))
    {
        exceptions(std::ios_base::badbit);
    }

    ifstream(const ifstream &) = delete;
    ifstream(ifstream &&) = default;
    ifstream & operator = (const ifstream &) = delete;
    ifstream & operator = (ifstream &&) = default;

    virtual ~ifstream()
    {
        if (rdbuf()) delete rdbuf();
    }

};

} // namespace zstr

#endif
