#ifndef JSONIO_SRC_JSON_KEY_H
#define JSONIO_SRC_JSON_KEY_H

#include <iostream>

#include "json_string.h"

namespace jsonio
{

class json_key
{
private:
    json_string text_;
    unsigned int flags_;

private:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_TEXT = 0x0001,
        PHASE_COLON = 0x0002,
        PHASE_COMPLETED = 0x0003,
        MASK_PHASE = 0x0003;

private:
    json_key(const json_string & text) noexcept;
    json_key(json_string && text) noexcept;

public:
    json_key() noexcept;
    json_key(const json_key & source) noexcept;
    json_key(json_key && source) noexcept;
    json_key & operator=(const json_key & source) noexcept;
    json_key & operator=(json_key && source) noexcept;
    ~json_key() noexcept;

    bool completed() const;
    bool operator<(const json_key & target);
    bool operator==(const json_string & text) const;

private:
    size_t read(std::istream & is, const std::string & delimiters);
    void write(std::ostream & os, int idndents) const;

public:
    template<class> friend class json_pair;
};

} // namespace jsonio

#endif //JSONIO_SRC_JSON_KEY_H
