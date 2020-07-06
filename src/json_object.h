#ifndef JSONIO_SRC_JSON_OBJECT_H
#define JSONIO_SRC_JSON_OBJECT_H

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "json_array.h"
#include "json_pair.h"

namespace jsonio
{

using JSON_OBJECT_PARENT = std::vector<json_pair>;

class json_object : public JSON_OBJECT_PARENT
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
    json_object(const unsigned int flags) noexcept;

public:
    json_object() noexcept;
    json_object(const json_object & source) noexcept;
    json_object(json_object && source) noexcept;
    json_object & operator=(const json_object & source) noexcept;
    json_object & operator=(json_object && source) noexcept;
    ~json_object() noexcept;
    
    bool completed() const;
    bool is_object() const;
    bool is_array() const;

    json & operator[](const std::string & key);
    const json & operator[](const std::string & key) const;

private:
    void read(std::istream & is);
    void write(std::ostream & os, int indents) const;

public:
    friend json;
    friend json_array;
    friend std::istream & operator>>(std::istream & is, json_object & target);
    friend std::ostream & operator<<(std::ostream & os, const json_object & source);
};

std::istream & operator>>(std::istream & is, json_object & target);
std::ostream & operator<<(std::ostream & os, const json_object & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_OBJECT_H
