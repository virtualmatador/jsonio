#include <algorithm>

#include "json.h"

jsonio::json::json() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json::json(const unsigned int flags) noexcept
    : flags_{flags}
{
}

jsonio::json::json(const jsonio::json & source) noexcept
{
    *this = source;
}

jsonio::json::json(jsonio::json && source) noexcept
{
    *this = std::move(source);
}

jsonio::json & jsonio::json::operator=(const jsonio::json & source) noexcept
{
    if (this != &source)
    {
        JSON_PARENT::operator=(*(JSON_PARENT*)&source);
        flags_ = source.flags_;
        key_value_ = source.key_value_;
    }
    return *this;
}

jsonio::json & jsonio::json::operator=(jsonio::json && source) noexcept
{
    if (this != &source)
    {
        JSON_PARENT::operator=(std::move(*(JSON_PARENT*)&source));
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
        key_value_ = std::move(source.key_value_);
    }
    return *this;
}

jsonio::json::~json() noexcept
{
}

bool jsonio::json::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
}

bool jsonio::json::is_object() const
{
    return completed() && JSON_PARENT::index() == 0;
}

bool jsonio::json::is_array() const
{
    return completed() && JSON_PARENT::index() == 1;
}

jsonio::json_value & jsonio::json::operator[](const std::string & key)
{
    return const_cast<json_value &>(
        static_cast<const json &>(*this).operator[](key));
}

const jsonio::json_value & jsonio::json::operator[](const std::string & key) const
{
    auto it = std::find_if(std::get<OBJECT_TYPE>(*this).begin(), std::get<OBJECT_TYPE>(*this).end(),
        [&](const json_pair & key_value)
    {
        return key_value.first == key;
    });
    return it->second;
}

jsonio::json_value & jsonio::json::operator[](const std::size_t & index)
{
    return const_cast<json_value &>(
        static_cast<const json &>(*this).operator[](index));
}

const jsonio::json_value & jsonio::json::operator[](const std::size_t & index) const
{
    return std::get<json_array>(*this)[index];
}

const jsonio::OBJECT_TYPE & jsonio::json::get_object() const
{
    return std::get<OBJECT_TYPE>(*this);
}

const jsonio::json_array & jsonio::json::get_array() const
{
    return std::get<json_array>(*this);
}

const jsonio::json_value* jsonio::json::get_value(const std::string & key) const
{
    if (is_object())
    {
        auto it = std::find_if(std::get<OBJECT_TYPE>(*this).begin(), std::get<OBJECT_TYPE>(*this).end(),
            [&](const json_pair & key_value)
        {
            return key_value.first == key;
        });
        if (it != std::get<OBJECT_TYPE>(*this).end())
        {
            return &it->second;
        }
    }
    return nullptr;
}

void jsonio::json::read(std::istream & is)
{
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        JSON_PARENT empty_parent;
        JSON_PARENT::swap(empty_parent);
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
                        JSON_PARENT::operator=(OBJECT_TYPE());
                    }
                    else if (source == '[')
                    {
                        flags_ &= ~MASK_PHASE;
                        flags_ |= PHASE_ARRAY;
                        JSON_PARENT::operator=(json_array(json_array::SKIP_PREFIX));
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
                    std::get<OBJECT_TYPE>(*this).push_back(std::move(key_value_));
                }
                else if (std::get<OBJECT_TYPE>(*this).size() != 0)
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
    if ((flags_ & MASK_PHASE) == PHASE_ARRAY)
    {
        std::get<json_array>(*this).read(is);
        if (is.bad())
            flags_ = PHASE_START;
        else
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_COMPLETED;
        }
    }
}

void jsonio::json::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        for (int i = 0; i < indents; ++i)
            os << '\t';
        if (JSON_PARENT::index() == 0)
        {
            os << '{';
            bool comma = false;
            for (auto & key_value : std::get<OBJECT_TYPE>(*this))
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
        else if (JSON_PARENT::index() == 1)
        {
            os << '[';
            bool comma = false;
            for (auto & value : std::get<json_array>(*this))
            {
                if (value.completed())
                {
                    if (comma)
                        os << ',';
                    else
                        comma = true;
                    os << std::endl;
                    os << value;
                }
            }
            os << std::endl;
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << ']';
        }
    }
}

std::istream & jsonio::operator>>(std::istream & is, jsonio::json & target)
{
    target.read(is);
    return is;
}

std::ostream & jsonio::operator<<(std::ostream & os, const jsonio::json & source)
{
    source.write(os, 0);
    return os;
}
