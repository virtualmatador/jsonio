#ifndef JSONIO_JSON_H
#define JSONIO_JSON_H

#include <iostream>
#include <string>
#include <vector>

#include "json_pair.h"

namespace jsonio
{

using VECTOR_TYPE = std::vector<json_pair>;

class json : public VECTOR_TYPE
{
private:
    unsigned int flags_;
    json_pair key_value_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_PAIR = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        SKIP_PREFIX = 0x0004;

private:
    json(const unsigned int flags) noexcept;

public:
    json() noexcept;
    json(const json & source) noexcept;
    json(json && source) noexcept;
    json & operator=(const json & source) noexcept;
    json & operator=(json && source) noexcept;
    ~json() noexcept;
    
    bool completed() const;

    json_value & operator[](const std::string & key);
    const json_value & operator[](const std::string & key) const;

private:
    void read(std::istream & is);
    void write(std::ostream & os, int indents) const;

public:
    friend json_value;
    friend json_array;
    friend std::istream & operator>>(std::istream & is, json & target);
    friend std::ostream & operator<<(std::ostream & os, const json & source);
};

std::istream & operator>>(std::istream & is, json & target);
std::ostream & operator<<(std::ostream & os, const json & source);

} // namespace jsonio

#endif //JSONIO_JSON_H
