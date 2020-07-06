#ifndef JSONIO_SRC_JSON_STRING_H
#define JSONIO_SRC_JSON_STRING_H

#include <iostream>
#include <string>

namespace jsonio
{

class json_string : public std::string
{
private:
    unsigned int flags_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_TEXT = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        ESCAPING = 0x0004,
        ESCAPED = 0x0008;

public:
    json_string() noexcept;
    json_string(const std::string & text) noexcept;
    json_string(std::string && text) noexcept;
    json_string(const json_string & source) noexcept;
    json_string(json_string && source) noexcept;
    json_string & operator=(const std::string & text) noexcept;
    json_string & operator=(std::string && text) noexcept;
    json_string & operator=(const json_string & source) noexcept;
    json_string & operator=(json_string && source) noexcept;
    ~json_string() noexcept;
    
    bool completed() const;

private:
    void read(std::istream & is);
    void write(std::ostream & os) const;

private:
    static char Unescape(const char source);
    static char Escape(const char source);

public:
    friend class json_key;
    friend class json;
};

}

#endif //JSONIO_JSON_STREAM_H
