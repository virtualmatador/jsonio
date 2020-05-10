#ifndef JSONIO_JSON_H
#define JSONIO_JSON_H

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "json_array.h"
#include "json_pair.h"

namespace jsonio
{

using OBJECT_TYPE = std::vector<json_pair>;
using JSON_PARENT = std::variant<OBJECT_TYPE, json_array>;

class json : public JSON_PARENT
{
private:
    unsigned int flags_;
    json_pair key_value_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_PAIR = 0x0001,
        PHASE_ARRAY = 0x0002,
        PHASE_COMPLETED = 0x0003,
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
    bool is_object() const;
    bool is_array() const;

    json_value & operator[](const std::string & key);
    const json_value & operator[](const std::string & key) const;
    json_value & operator[](const std::size_t & index);
    const json_value & operator[](const std::size_t & index) const;

    const OBJECT_TYPE & get_object() const;
    const json_array & get_array() const;

    const json_value* get_value(const std::string & key) const;

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
