#include <algorithm>

#include "json.h"
#include "json_value.h"

#include "json_array.h"

jsonio::json_array::json_array() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json_array::json_array(const unsigned int flags) noexcept
    : flags_{flags}
{
}

jsonio::json_array::json_array(const jsonio::json_array & source) noexcept
{
    *this = source;
}

jsonio::json_array::json_array(jsonio::json_array && source) noexcept
{
    *this = std::move(source);
}

jsonio::json_array & jsonio::json_array::operator=(const jsonio::json_array & source) noexcept
{
    if (this != &source)
    {
        ARRAY_TYPE::operator=(*(ARRAY_TYPE*)&source);
        flags_ = source.flags_;
        value_ = source.value_;
    }
    return *this;
}

jsonio::json_array & jsonio::json_array::operator=(jsonio::json_array && source) noexcept
{
    if (this != &source)
    {
        ARRAY_TYPE::operator=(std::move(*(ARRAY_TYPE*)&source));
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
        value_ = std::move(source.value_);
    }
    return *this;
}

jsonio::json_array::~json_array() noexcept
{
}

bool jsonio::json_array::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
}

void jsonio::json_array::read(std::istream & is)
{
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        value_.flags_ = json_value::PHASE_START;
        if (flags_ & SKIP_PREFIX)
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_VALUE;
        }
        else
        {
            char source;
            while (is >> source)
            {
                if (!isspace(source))
                {
                    if (source == '[')
                    {
                        flags_ &= ~MASK_PHASE;
                        flags_ |= PHASE_VALUE;
                    }
                    else
                        is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
            }
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_VALUE)
    {
        const std::string delimiters = ",]";
        for(;;)
        {
            size_t delimiter = value_.read(is, delimiters);
            if (is.good())
            {
                push_back(std::move(value_));
                if (delimiters[delimiter] == ']')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                    break;
                }
                else if (delimiters[delimiter] != ',')
                {
                    is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
            }
            else
                break;
        }
    }
}

const void jsonio::json_array::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        for (int i = 0; i < indents; ++i)
            os << '\t';
        os << "[";
        bool comma = false;
        for (const auto & sub_JsonValue : *this)
        {
            if (sub_JsonValue.completed())
            {
                if (comma)
                    os << ',';
                else
                    comma = true;
                os << std::endl;
                sub_JsonValue.write(os, indents + 1);
            }
        }
        os << std::endl;
        for (int i = 0; i < indents; ++i)
            os << '\t';
        os << "]";
    }
}

jsonio::json_value & jsonio::json_array::operator[](size_t index)
{
    return const_cast<json_value &>(static_cast<const json_array &>(*this).operator[](index));
}

const jsonio::json_value & jsonio::json_array::operator[](size_t index) const
{
    return ARRAY_TYPE::operator[](index);
}

std::istream & jsonio::operator>>(std::istream & is, jsonio::json_array & target)
{
    target.read(is);
    return is;
}

std::ostream & jsonio::operator<<(std::ostream & os, const jsonio::json_array & source)
{
    source.write(os, 0);
    return os;
}
