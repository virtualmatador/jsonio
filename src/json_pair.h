#ifndef JSONIO_JSON_PAIR_H
#define JSONIO_JSON_PAIR_H

#include <iostream>
#include <string>

#include "json_key.h"
#include "json_value.h"

namespace jsonio
{

using PAIR_TYPE = std::pair<json_key, json_value>;

class json_pair : public PAIR_TYPE
{
private:
    unsigned int flags_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_KEY = 0x0001,
        PHASE_VALUE = 0x0002,
        PHASE_COMPLETED = 0x0003,
        MASK_PHASE = 0x0003,
        EMPTY_PAIR = 0x0004;

public:
    json_pair() noexcept;
    json_pair(const std::string & key) noexcept;
    json_pair(std::string && key) noexcept;
    json_pair(const std::string & key, const json_value & value) noexcept;
    json_pair(std::string && key, json_value && value) noexcept;
    json_pair(const json_pair & source) noexcept;
    json_pair(json_pair && source) noexcept;
    json_pair & operator=(const json_pair & source) noexcept;
    json_pair & operator=(json_pair && source) noexcept;
    ~json_pair() noexcept;

    bool completed() const;

private:
    size_t read(std::istream & is, const std::string & delimiters);
    void write(std::ostream & os, int indents) const;

public:
    friend class json;
    friend std::istream & operator>>(std::istream & is, json_pair & target);
    friend std::ostream & operator<<(std::ostream & os, const json_pair & source);
};

std::istream & operator>>(std::istream & is, json_pair & target);
std::ostream & operator<<(std::ostream & os, const json_pair & source);

} // namespace jsonio

#endif //JSONIO_JSON_PAIR_H
