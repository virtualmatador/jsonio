#ifndef JSONIO_SRC_JSON_PAIR_HPP
#define JSONIO_SRC_JSON_PAIR_HPP

#include <iostream>
#include <string>

#include "json_key.h"

namespace jsonio
{

template<class json>
using PAIR_TYPE = std::pair<json_key, json>;

template<class json>
class json_pair : public PAIR_TYPE<json>
{
private:
    using PARENT_TYPE = PAIR_TYPE<json>;
    unsigned int flags_;

public:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_KEY = 0x0001,
        PHASE_VALUE = 0x0002,
        PHASE_COMPLETED = 0x0003,
        MASK_PHASE = 0x0003,
        EMPTY_PAIR = 0x0004;

public:
    json_pair() noexcept
        : flags_{PHASE_START}
    {
    }

    json_pair(const std::string & key) noexcept
        : PARENT_TYPE{std::make_pair(json_key(key), json())}
        , flags_{PHASE_COMPLETED}
    {
    }

    json_pair(std::string && key) noexcept
        : PARENT_TYPE{std::make_pair(json_key(std::move(key)), json())}
        , flags_{PHASE_COMPLETED}
    {
    }

    json_pair(const std::string & key, const json & value) noexcept
        : PARENT_TYPE{std::make_pair(json_key(key), value)}
        , flags_{PHASE_COMPLETED}
    {
    }

    json_pair(std::string && key, json && value) noexcept
        : PARENT_TYPE{std::make_pair(json_key(std::move(key)), std::move(value))}
        , flags_{PHASE_COMPLETED}
    {
    }

    json_pair(const json_pair & source) noexcept
    {
        *this = source;
    }

    json_pair(json_pair && source) noexcept
    {
        *this = std::move(source);
    }

    json_pair & operator=(const json_pair & source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(*(PARENT_TYPE*)&source);
            flags_ = source.flags_;
        }
        return *this;
    }

    json_pair & operator=(json_pair && source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(std::move(*(PARENT_TYPE*)&source));
            flags_ = source.flags_;
            source.flags_ = PHASE_START;
        }
        return *this;
    }

    ~json_pair() noexcept
    {
    }

    void reset_flags()
    {
        flags_ = json_pair<json>::PHASE_START;
    }

    bool completed() const
    {
        return (flags_ & MASK_PHASE) == PHASE_COMPLETED &&
            PARENT_TYPE::first.completed() && PARENT_TYPE::second.completed();
    }

    bool is_empty()
    {
        return flags_ & json_pair<json>::EMPTY_PAIR;
    }

    size_t read(std::istream & is, const std::string & delimiters)
    {
        size_t delimiter = -1;
        if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
            flags_ = PHASE_START;
        if ((flags_ & MASK_PHASE) == PHASE_START)
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_KEY;
            PARENT_TYPE::first.flags_ = json_key::PHASE_START;
        }
        if ((flags_ & MASK_PHASE) == PHASE_KEY)
        {
            delimiter = PARENT_TYPE::first.read(is, delimiters);
            if (is.good())
            {
                if (delimiter == std::string::npos)
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_VALUE;
                    PARENT_TYPE::second.flags_ = json::PHASE_START;
                }
                else
                {
                    flags_ |= EMPTY_PAIR;
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_START;
                }
            }
        }
        if ((flags_ & MASK_PHASE) == PHASE_VALUE)
        {
            delimiter = PARENT_TYPE::second.read(is, delimiters);
            if (is.good())
            {
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_COMPLETED;
            }
        }
        return delimiter;
    }

    void write(std::ostream & os, int indents) const
    {
        if (completed())
        {
            PARENT_TYPE::first.write(os, indents);
            os << ' ';
            int sub_indents;
            if (PARENT_TYPE::second.index() >= size_t(JsonType::J_OBJECT))
            {
                os << std::endl;
                sub_indents = indents;
            }
            else
                sub_indents = 0;
            PARENT_TYPE::second.write(os, sub_indents);
        }
    }

public:
    template<class> friend std::istream & operator>>(std::istream & is, json_pair<json> & target);
    template<class> friend std::ostream & operator<<(std::ostream & os, const json_pair<json> & source);
};

template<class json> std::istream & operator>>(std::istream & is, json_pair<json> & target);
template<class json> std::ostream & operator<<(std::ostream & os, const json_pair<json> & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_PAIR_HPP
