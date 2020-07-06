#include <algorithm>

#include "json_object.h"

jsonio::json_object::json_object() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json_object::json_object(const unsigned int flags) noexcept
    : flags_{flags}
{
}

jsonio::json_object::json_object(const jsonio::json_object & source) noexcept
{
    *this = source;
}

jsonio::json_object::json_object(jsonio::json_object && source) noexcept
{
    *this = std::move(source);
}

jsonio::json_object & jsonio::json_object::operator=(const jsonio::json_object & source) noexcept
{
    if (this != &source)
    {
        JSON_OBJECT_PARENT::operator=(*(JSON_OBJECT_PARENT*)&source);
        flags_ = source.flags_;
        key_value_ = source.key_value_;
    }
    return *this;
}

jsonio::json_object & jsonio::json_object::operator=(jsonio::json_object && source) noexcept
{
    if (this != &source)
    {
        JSON_OBJECT_PARENT::operator=(std::move(*(JSON_OBJECT_PARENT*)&source));
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
        key_value_ = std::move(source.key_value_);
    }
    return *this;
}

jsonio::json_object::~json_object() noexcept
{
}

bool jsonio::json_object::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
}

jsonio::json & jsonio::json_object::operator[](const std::string & key)
{
    return const_cast<json &>(
        static_cast<const json_object &>(*this).operator[](key));
}

const jsonio::json & jsonio::json_object::operator[](const std::string & key) const
{
    auto it = std::find_if(begin(), end(), [&](const json_pair & key_value)
    {
        return key_value.first == key;
    });
    return it->second;
}

void jsonio::json_object::read(std::istream & is)
{
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        JSON_OBJECT_PARENT empty_parent;
        JSON_OBJECT_PARENT::swap(empty_parent);
        key_value_.flags_ = json_pair::PHASE_START;
        if (flags_ & SKIP_PREFIX)
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_PAIR;
        }
        else
        {
            char source;
            while (is >> source)
            {
                if (!isspace(source))
                {
                    if (source == '{')
                    {
                        flags_ &= ~MASK_PHASE;
                        flags_ |= PHASE_PAIR;
                        JSON_OBJECT_PARENT::operator=(JSON_OBJECT_PARENT());
                    }
                    else
                    {
                        flags_ = PHASE_START;
                        is.setstate(std::ios::iostate::_S_badbit);
                    }
                    break;
                }
            }
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_PAIR)
    {
        for (;;)
        {
            const std::string delimiters = ",}";
            size_t delimiter = key_value_.read(is, delimiters);
            if (is.good())
            {
                if (!(key_value_.flags_ & json_pair::EMPTY_PAIR))
                {
                    push_back(std::move(key_value_));
                }
                else if (size() != 0)
                {
                    flags_ = PHASE_START;
                    is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
                if (delimiters[delimiter] == '}')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                    break;                    
                }
                else if (delimiters[delimiter] != ',')
                {
                    flags_ = PHASE_START;
                    is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
            }
            else
            {
                if (is.bad())
                    flags_ = PHASE_START;
                break;
            }
        }
    }
}

void jsonio::json_object::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        for (int i = 0; i < indents; ++i)
            os << '\t';
        os << '{';
        bool comma = false;
        for (auto & key_value : *this)
        {
            if (key_value.completed())
            {
                if (comma)
                    os << ',';
                else
                    comma = true;
                os << std::endl;
                key_value.write(os, indents + 1);
            }
        }
        os << std::endl;
        for (int i = 0; i < indents; ++i)
            os << '\t';
        os << '}';
    }
}

std::istream & jsonio::operator>>(std::istream & is, jsonio::json_object & target)
{
    target.read(is);
    return is;
}

std::ostream & jsonio::operator<<(std::ostream & os, const jsonio::json_object & source)
{
    source.write(os, 0);
    return os;
}
