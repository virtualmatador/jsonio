#include "json_pair.h"

jsonio::json_pair::json_pair() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json_pair::json_pair(const std::string & key) noexcept
    : PAIR_TYPE{std::make_pair(json_key(key), json_value())}
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_pair::json_pair(std::string && key) noexcept
    : PAIR_TYPE{std::make_pair(json_key(std::move(key)), json_value())}
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_pair::json_pair(const std::string & key, const jsonio::json_value & value) noexcept
    : PAIR_TYPE{std::make_pair(json_key(key), value)}
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_pair::json_pair(std::string && key, jsonio::json_value && value) noexcept
    : PAIR_TYPE{std::make_pair(json_key(std::move(key)), std::move(value))}
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_pair::json_pair(const jsonio::json_pair & source) noexcept
{
    *this = source;
}

jsonio::json_pair::json_pair(jsonio::json_pair && source) noexcept
{
    *this = std::move(source);
}

jsonio::json_pair & jsonio::json_pair::operator=(const jsonio::json_pair & source) noexcept
{
    if (this != &source)
    {
        PAIR_TYPE::operator=(*(PAIR_TYPE*)&source);
        flags_ = source.flags_;
    }
    return *this;
}

jsonio::json_pair & jsonio::json_pair::operator=(jsonio::json_pair && source) noexcept
{
    if (this != &source)
    {
        PAIR_TYPE::operator=(std::move(*(PAIR_TYPE*)&source));
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
    }
    return *this;
}

jsonio::json_pair::~json_pair() noexcept
{
}

bool jsonio::json_pair::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED &&
        first.completed() && second.completed();
}

size_t jsonio::json_pair::read(std::istream & is, const std::string & delimiters)
{
    size_t delimiter = -1;
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        flags_ &= ~MASK_PHASE;
        flags_ |= PHASE_KEY;
        first.flags_ = json_key::PHASE_START;
    }
    if ((flags_ & MASK_PHASE) == PHASE_KEY)
    {
        delimiter = first.read(is, delimiters);
        if (is.good())
        {
            if (delimiter == std::string::npos)
            {
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_VALUE;
                second.flags_ = json_value::PHASE_START;
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
        delimiter = second.read(is, delimiters);
        if (is.good())
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_COMPLETED;
        }
    }
    return delimiter;
}

void jsonio::json_pair::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        first.write(os, indents);
        os << ' ';
        int sub_indents;
        if (second.index() >= size_t(JsonType::J_OBJECT))
        {
            os << std::endl;
            sub_indents = indents;
        }
        else
            sub_indents = 0;
        second.write(os, sub_indents);
    }
}

std::istream & jsonio::operator>>(std::istream & is, jsonio::json_pair & target)
{
    target.read(is, "\n");
    return is;
}

std::ostream & jsonio::operator<<(std::ostream & os, const jsonio::json_pair & source)
{
    source.write(os, 0);
    return os;
}
