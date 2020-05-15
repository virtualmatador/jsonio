#ifndef JSONIO_SRC_JSON_ARRAY_H
#define JSONIO_SRC_JSON_ARRAY_H

#include <iostream>
#include <memory>
#include <vector>

#include "json_value.h"

namespace jsonio
{

using ARRAY_TYPE = std::vector<json_value>;

class json_array : public ARRAY_TYPE
{
private:
    unsigned int flags_;
    json_value value_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_VALUE = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        SKIP_PREFIX = 0x0004;

private:
    json_array(const unsigned int flags) noexcept;

    void read(std::istream & is);
    const void write(std::ostream & os, int indents) const;

public:
    json_array() noexcept;
    json_array(const json_array & source) noexcept;
    json_array(json_array && source) noexcept;
    json_array & operator=(const json_array & source) noexcept;
    json_array & operator=(json_array && source) noexcept;
    ~json_array() noexcept;
    
    bool completed() const;
    json_value & operator[](size_t index);
    const json_value & operator[](size_t index) const;

public:
    friend class json;
    friend class json_value;
    friend std::istream & operator>>(std::istream & is, json_array & target);
    friend std::ostream & operator<<(std::ostream & os, const json_array & source);
};

std::istream & operator>>(std::istream & is, json_array & target);
std::ostream & operator<<(std::ostream & os, const json_array & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_ARRAY_H
