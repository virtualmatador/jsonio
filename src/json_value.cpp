#include <algorithm>
#include <memory>

#include "json.h"
#include "json_array.h"

#include "json_value.h"

jsonio::json_value::json_value() noexcept
    : flags_{PHASE_START}
{
}

jsonio::json_value::json_value(const jsonio::json_value & source) noexcept
{
    *this = source;
}

jsonio::json_value::json_value(jsonio::json_value && source) noexcept
{
    *this = std::move(source);
}

jsonio::json_value::json_value(const void* null_value)
{
    *this = null_value;
}

jsonio::json_value::json_value(const std::string & string_value)
{
    *this = string_value;
}

jsonio::json_value::json_value(std::string && string_value)
{
    *this = std::move(string_value);
}

jsonio::json_value::json_value(const char* string_value)
{
    *this = string_value;
}

jsonio::json_value::json_value(const long & long_value)
{
    *this = long_value;
}

jsonio::json_value::json_value(const double & double_value)
{
    *this = double_value;
}

jsonio::json_value::json_value(const bool & bool_value)
{
    *this = bool_value;
}

jsonio::json_value::json_value(const json & json_object_value)
{
    *this = json_object_value;
}

jsonio::json_value::json_value(json && json_object_value)
{
    *this = std::move(json_object_value);
}

jsonio::json_value::json_value(const json_array & json_array_value)
{
    *this = json_array_value;
}

jsonio::json_value::json_value(json_array && json_array_value)
{
    *this = std::move(json_array_value);
}

jsonio::json_value & jsonio::json_value::operator=(const jsonio::json_value & source) noexcept
{
    if (this != &source)
    {
        switch ((JsonType)source.index())
        {
        case JsonType::J_NULL:
            VARIANT_TYPE::operator=(std::get<void*>(*(VARIANT_TYPE*)&source));
            break;
        case JsonType::J_STRING:
            VARIANT_TYPE::operator=(std::get<jsonio::json_string>(*(VARIANT_TYPE*)&source));
            break;
        case JsonType::J_LONG:
            VARIANT_TYPE::operator=(std::get<long>(*(VARIANT_TYPE*)&source));
            break;
        case JsonType::J_DOUBLE:
            VARIANT_TYPE::operator=(std::get<double>(*(VARIANT_TYPE*)&source));
            break;
        case JsonType::J_BOOL:
            VARIANT_TYPE::operator=(std::get<bool>(*(VARIANT_TYPE*)&source));
            break;
        case JsonType::J_OBJECT:
            if (std::get<std::unique_ptr<json>>(*(VARIANT_TYPE*)&source))
                VARIANT_TYPE::operator=(std::unique_ptr<json>(new json(
                    *std::get<std::unique_ptr<json>>(*(VARIANT_TYPE*)&source))));
            else
                VARIANT_TYPE::operator=(std::unique_ptr<json>(nullptr));
            break;
        case JsonType::J_ARRAY:
            if (std::get<std::unique_ptr<json_array>>(*(VARIANT_TYPE*)&source))
                VARIANT_TYPE::operator=(std::unique_ptr<json_array>(new json_array(
                    *std::get<std::unique_ptr<json_array>>(*(VARIANT_TYPE*)&source))));
            else
                VARIANT_TYPE::operator=(std::unique_ptr<json_array>(nullptr));
            break;
        }
        flags_ = source.flags_;
        binary_ = source.binary_;
    }
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(jsonio::json_value && source) noexcept
{
    if (this != &source)
    {
        switch ((JsonType)source.index())
        {
        case JsonType::J_NULL:
            VARIANT_TYPE::operator=(std::move(std::get<void*>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_STRING:
            VARIANT_TYPE::operator=(std::move(std::get<jsonio::json_string>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_LONG:
            VARIANT_TYPE::operator=(std::move(std::get<long>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_DOUBLE:
            VARIANT_TYPE::operator=(std::move(std::get<double>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_BOOL:
            VARIANT_TYPE::operator=(std::move(std::get<bool>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_OBJECT:
            VARIANT_TYPE::operator=(std::move(std::get<std::unique_ptr<json>>(*(VARIANT_TYPE*)&source)));
            break;
        case JsonType::J_ARRAY:
            VARIANT_TYPE::operator=(std::move(std::get<std::unique_ptr<json_array>>(*(VARIANT_TYPE*)&source)));
            break;
        }
        flags_ = source.flags_;
        source.flags_ = PHASE_START;
        binary_ = std::move(source.binary_);
    }
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const void* null_value)
{
    VARIANT_TYPE::operator=(nullptr);
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const std::string & string_value)
{
    VARIANT_TYPE::operator=(json_string(string_value));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(std::string && string_value)
{
    VARIANT_TYPE::operator=(json_string(std::move(string_value)));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const char* string_value)
{
    VARIANT_TYPE::operator=(json_string(string_value));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const long & long_value)
{
    VARIANT_TYPE::operator=(long_value);
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const double & double_value)
{
    VARIANT_TYPE::operator=(double_value);
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const bool & bool_value)
{
    VARIANT_TYPE::operator=(bool_value);
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const jsonio::json & json_object_value)
{
    VARIANT_TYPE::operator=(std::unique_ptr<json>(new json(json_object_value)));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(jsonio::json && json_object_value)
{
    VARIANT_TYPE::operator=(std::unique_ptr<json>(new json(std::move(json_object_value))));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(const jsonio::json_array & json_array_value)
{
    VARIANT_TYPE::operator=(std::unique_ptr<json_array>(new json_array(json_array_value)));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value & jsonio::json_value::operator=(jsonio::json_array && json_array_value)
{
    VARIANT_TYPE::operator=(std::unique_ptr<json_array>(new json_array(std::move(json_array_value))));
    flags_ = PHASE_COMPLETED;
    return *this;
}

jsonio::json_value::~json_value() noexcept
{
}

bool jsonio::json_value::completed() const
{
    return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
}

size_t jsonio::json_value::read(std::istream & is, const std::string & delimiters)
{
    size_t delimiter = -1;
    if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        flags_ = PHASE_START;
    if ((flags_ & MASK_PHASE) == PHASE_START)
    {
        char source;
        while (is >> source)
        {
            if (!isspace(source))
            {
                if (source == ']')
                {
                    delimiter = delimiters.find(source);
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                    flags_ |= EMPTY_VALUE;
                }
                else if (source == '[')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_ARRAY;
                    VARIANT_TYPE::operator=(std::unique_ptr<json_array>(new json_array(json_array::SKIP_PREFIX)));
                }
                else if (source == '{')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_OBJECT;
                    VARIANT_TYPE::operator=(std::unique_ptr<json>(new json(json::SKIP_PREFIX)));
                }
                else if (source == '\"')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_STRING;
                    VARIANT_TYPE::operator=(json_string());
                }
                else
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_BINARY;
                    binary_.clear();
                    binary_.append(1, source);
                }
                break;
            }
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_ARRAY)
    {
        std::get<std::unique_ptr<json_array>>(*this)->read(is);
        if (is.good())
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_DELIMITER;
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_OBJECT)
    {
        std::get<std::unique_ptr<json>>(*this)->read(is);
        if (is.good())
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_DELIMITER;
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_STRING)
    {
        std::get<json_string>(*this).read(is);
        if (is.good())
        {
            flags_ &= ~MASK_PHASE;
            flags_ |= PHASE_DELIMITER;
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_BINARY)
    {
        char source;
        while (is >> source)
        {
            delimiter = delimiters.find(source);
            if (delimiter == std::string::npos)
                binary_.append(1, source);
            else
                break;
        }
        if (is.good())
        {
            binary_.erase(std::find_if(binary_.rbegin(), binary_.rend(),
                [](const char c)
                {
                    return !std::isspace(c);
                }).base(), binary_.end());
            if (binary_ == "null")
            {
                VARIANT_TYPE::operator=(nullptr);
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_COMPLETED;
            }
            else if (binary_ == "true")
            {
                VARIANT_TYPE::operator=(true);
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_COMPLETED;
            }
            else if (binary_ == "false")
            {
                VARIANT_TYPE::operator=(false);
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_COMPLETED;
            }
            else
            {
                char* end_ptr;
                if (binary_.find('.') != std::string::npos)
                    VARIANT_TYPE::operator=(strtod(binary_.c_str(), &end_ptr));
                else
                    VARIANT_TYPE::operator=(strtol(binary_.c_str(), &end_ptr, 0));
                if (*end_ptr == '\0')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                }
                else
                    is.setstate(std::ios::iostate::_S_badbit);
            }
        }
    }
    if ((flags_ & MASK_PHASE) == PHASE_DELIMITER)
    {
        char source;
        while (is >> source)
        {
            delimiter = delimiters.find(source);
            if (delimiter != std::string::npos)
            {
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_COMPLETED;
                break;
            }
            else if (!isspace(source))
            {
                is.setstate(std::ios::iostate::_S_badbit);
                break;
            }
        }
    }
    return delimiter;
}

void jsonio::json_value::write(std::ostream & os, int indents) const
{
    if (completed())
    {
        switch ((JsonType)index())
        {
        case JsonType::J_NULL:
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << "null";
            break;
        case JsonType::J_STRING:
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << '\"';
            std::get<json_string>(*this).write(os);
            os << '\"';
            break;
        case JsonType::J_LONG:
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << get_long();
            break;
        case JsonType::J_DOUBLE:
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << get_double();
            break;
        case JsonType::J_BOOL:
            for (int i = 0; i < indents; ++i)
                os << '\t';
            if (get_bool())
                os << "true";
            else
                os << "false";
            break;
        case JsonType::J_OBJECT:
            std::get<std::unique_ptr<json>>(*this)->write(os, indents);
            break;
        case JsonType::J_ARRAY:
            std::get<std::unique_ptr<json_array>>(*this)->write(os, indents);
            break;
        }
    }
}

jsonio::JsonType jsonio::json_value::get_type() const
{
    return (JsonType)index();
}

jsonio::json_value & jsonio::json_value::operator[](const std::string & key)
{
    return const_cast<json_value &>(
        static_cast<const json_value &>(*this).operator[](key));
}

const jsonio::json_value & jsonio::json_value::operator[](const std::string & key) const
{
    return std::get<std::unique_ptr<json>>(*this)->operator[](key);
}

jsonio::json_value & jsonio::json_value::operator[](size_t index)
{
    return const_cast<json_value &>(
        static_cast<const json_value &>(*this).operator[](index));
}

const jsonio::json_value & jsonio::json_value::operator[](size_t index) const
{
    return std::get<std::unique_ptr<json_array>>(*this)->operator[](index);
}

void* & jsonio::json_value::get_null()
{
    return const_cast<void* &>(static_cast<const json_value &>(*this).get_null());
}

void* const & jsonio::json_value::get_null() const
{
    return std::get<void*>(*this);
}

jsonio::json_string & jsonio::json_value::get_string()
{
    return const_cast<json_string &>(static_cast<const json_value &>(*this).get_string());
}

const jsonio::json_string & jsonio::json_value::get_string() const
{
    return std::get<json_string>(*this);
}

long & jsonio::json_value::get_long()
{
    return const_cast<long &>(static_cast<const json_value &>(*this).get_long());
}

const long & jsonio::json_value::get_long() const
{
    return std::get<long>(*this);
}

double & jsonio::json_value::get_double()
{
    return const_cast<double &>(static_cast<const json_value &>(*this).get_double());
}

const double & jsonio::json_value::get_double() const
{
    return std::get<double>(*this);
}

bool & jsonio::json_value::get_bool()
{
    return const_cast<bool &>(static_cast<const json_value &>(*this).get_bool());
}

const bool & jsonio::json_value::get_bool() const
{
    return std::get<bool>(*this);
}

jsonio::json & jsonio::json_value::get_object()
{
    return const_cast<jsonio::json &>(static_cast<const json_value &>(*this).get_object());
}

const jsonio::json & jsonio::json_value::get_object() const
{
    return *std::get<std::unique_ptr<json>>(*this);
}

jsonio::json_array & jsonio::json_value::get_array()
{
    return const_cast<json_array &>(static_cast<const json_value &>(*this).get_array());
}

const jsonio::json_array & jsonio::json_value::get_array() const
{
    return *std::get<std::unique_ptr<json_array>>(*this);
}

std::istream & jsonio::operator>>(std::istream & is, jsonio::json_value & target)
{
    target.read(is, "\n");
    return is;
}

std::ostream & jsonio::operator<<(std::ostream & os, const jsonio::json_value & source)
{
    source.write(os, 0);
    return os;
}
