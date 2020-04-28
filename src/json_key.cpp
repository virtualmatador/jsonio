#include "json_key.h"

jsonio::json_key::json_key() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json_key::json_key(const jsonio::json_string & text) noexcept
    : text_(text)
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_key::json_key(jsonio::json_string && text) noexcept
    : text_(std::move(text))
    , flags_{PHASE_COMPLETED}
{
}

jsonio::json_key::json_key(const json_key & source) noexcept
{
    *this = source;
}

jsonio::json_key::json_key(json_key && source) noexcept
{
    *this = std::move(source);
}

jsonio::json_key & jsonio::json_key::operator=(const jsonio::json_key & source) noexcept
{
    if (this != &source)
    {
        text_ = source.text_;
        flags_ = source.flags_;
    }
    return *this;
}

jsonio::json_key & jsonio::json_key::operator=(jsonio::json_key && source) noexcept
{
    if (this != &source)
    {
        text_ = std::move(source.text_);
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
    }
    return *this;
}

jsonio::json_key::~json_key() noexcept
{
}

bool jsonio::json_key::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED &&
        text_.completed();
}

size_t jsonio::json_key::read(std::istream & is, const std::string & delimiters)
{
    size_t delimiter = -1;
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        text_.flags_ = json_string::PHASE_START;
        char source;
        while (is >> source)
        {
            if (source == '\"')
            {
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_TEXT;
                break;
            }
            else
            {
                delimiter = delimiters.find(source);
                if (delimiter != std::string::npos)
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                    break;
                }
                else if (!std::isspace(source))
                {
                    is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
            }
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_TEXT)
    {
        text_.read(is);
        if (is.good())
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_COLON;
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_COLON)
    {
        char source;
        while (is >> source)
        {
            if (!std::isspace(source))
            {
                if (source == ':')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                }
                else
                    is.setstate(std::ios::iostate::_S_badbit);
                break;
            }
        }
    }
    return delimiter;
}

void jsonio::json_key::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        for (int i = 0; i < indents; ++i)
            os << '\t';
        os << '\"';
        text_.write(os);
        os << '\"';
        os << ':';
    }
}

bool jsonio::json_key::operator<(const jsonio::json_key & target)
{
    return text_ < target.text_;
}

bool jsonio::json_key::operator==(const jsonio::json_string & text) const
{
    return text_ == text;
}
