#ifndef JSONIO_SRC_JSON_OBJECT_HPP
#define JSONIO_SRC_JSON_OBJECT_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "json_string.h"

namespace jsonio
{

template<class json>
using JSON_OBJECT_PARENT = std::map<json_string, json>;

template<class json>
class json_object : public JSON_OBJECT_PARENT<json>
{
private:
    using PARENT_TYPE = JSON_OBJECT_PARENT<json>;
    unsigned int flags_;
    json_string key_;
    std::unique_ptr<json> value_;

public:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_KEY_START = 0x0001,
        PHASE_KEY_TEXT = 0x0002,
        PHASE_COLON = 0x0003,
        PHASE_VALUE = 0x0004,
        PHASE_COMPLETED = 0x0005,
        MASK_PHASE = 0x0007,
        SKIP_PREFIX = 0x0008;

public:
    json_object() noexcept
        : flags_{PHASE_COMPLETED}
        , value_{std::make_unique<json>()}
    {
    }

    json_object(const unsigned int flags) noexcept
        : flags_{flags}
        , value_{std::make_unique<json>()}
    {
    }

    json_object(const json_object & source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = source;
    }

    json_object(json_object && source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = std::move(source);
    }

    json_object & operator=(const json_object & source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(*(PARENT_TYPE*)&source);
            flags_ = source.flags_;
            key_ = source.key_;
            *value_ = *source.value_;
        }
        return *this;
    }

    json_object & operator=(json_object && source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(std::move(*(PARENT_TYPE*)&source));
            flags_ = source.flags_;
            source.flags_ = PHASE_START;
            key_ = std::move(source.key_);
            *value_ = std::move(*source.value_);
        }
        return *this;
    }

    ~json_object() noexcept
    {
    }

    bool completed() const
    {
        return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
    }

    json & operator[](const std::string & key)
    {
        return const_cast<json &>(
            static_cast<const json_object &>(*this).operator[](key));
    }

    const json & operator[](const std::string & key) const
    {
        return PARENT_TYPE::find(key)->second;
    }

    void steal(const json_object& source, bool convert)
    {
        for (auto& [key, value] : *this)
        {
            auto source_node = source.find(key);
            if (source_node != source.end())
            {
                value.steal(source_node->second, convert);
            }
        }
    }

    void read(std::istream & is)
    {
        if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
            flags_ = PHASE_START;
        if ((flags_ & MASK_PHASE) == PHASE_START)
        {
            PARENT_TYPE::clear();
            if (flags_ & SKIP_PREFIX)
            {
                flags_ &= ~MASK_PHASE;
                flags_ |= PHASE_KEY_START;
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
                            flags_ |= PHASE_KEY_START;
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
        if ((flags_ & MASK_PHASE) == PHASE_KEY_START)
        {
            key_.flags_ = json_string::PHASE_START;
            char source;
            while (is >> source)
            {
                if (source == '\"')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_KEY_TEXT;
                    break;
                }
                else if (source == '}')
                {
                    if (PARENT_TYPE::size() == 0)
                    {
                        flags_ &= ~MASK_PHASE;
                        flags_ |= PHASE_COMPLETED;
                    }
                    else
                    {
                        flags_ = PHASE_START;
                        is.setstate(std::ios::iostate::_S_badbit);
                    }
                    break;
                }
                else if (!std::isspace(source))
                {
                    flags_ = PHASE_START;
                    is.setstate(std::ios::iostate::_S_badbit);
                    break;
                }
            }
        }
        if ((flags_ & MASK_PHASE) == PHASE_KEY_TEXT)
        {
            key_.read(is);
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
                        flags_ |= PHASE_VALUE;
                    }
                    else
                    {
                        is.setstate(std::ios::iostate::_S_badbit);
                    }
                    break;
                }
            }
        }
        if ((flags_ & MASK_PHASE) == PHASE_VALUE)
        {
            const std::string delimiters = ",}";
            std::size_t delimiter = value_->read(is, delimiters);
            if (is.good())
            {
                bool read_again = false;
                bool append = true;
                if (delimiters[delimiter] == ',')
                {
                    read_again = true;
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_KEY_START;
                }
                else if (delimiters[delimiter] == '}')
                {
                    flags_ &= ~MASK_PHASE;
                    flags_ |= PHASE_COMPLETED;
                }
                else
                {
                    append = false;
                    flags_ = PHASE_START;
                    is.setstate(std::ios::iostate::_S_badbit);
                }
                if (append)
                {
                    PARENT_TYPE::insert(std::make_pair(
                        std::move(key_),
                        std::move(*value_)));
                }
                if (read_again)
                {
                    read(is);
                }
            }
        }
    }

    void write(std::ostream & os, int indents) const
    {
        if (completed())
        {
            for (int i = 0; i < indents; ++i)
                os << '\t';
            os << '{';
            bool comma = false;
            for (const auto& [key, value] : *this)
            {
                if (comma)
                {
                    os << ',';
                }
                else
                {
                    comma = true;
                }
                os << std::endl;
                for (int i = 0; i < indents + 1; ++i)
                {
                    os << '\t';
                }
                os << '\"' << key << "\":";
                int sub_indents;
                if (value.index() == size_t(JsonType::J_ARRAY) ||
                    value.index() == size_t(JsonType::J_OBJECT))
                {
                    os << std::endl;
                    sub_indents = indents + 1;
                }
                else
                {
                    os << " ";
                    sub_indents = 0;
                }
                value.write(os, sub_indents);
            }
            os << std::endl;
            for (int i = 0; i < indents; ++i)
            {
                os << '\t';
            }
            os << '}';
        }
    }

public:
    template<class> friend std::istream & operator>>(
        std::istream & is, json_object<json> & target);
    template<class> friend std::ostream & operator<<(
        std::ostream & os, const json_object<json> & source);
};

template<class json> std::istream & operator>>(
    std::istream & is, json_object<json> & target);
template<class json> std::ostream & operator<<(
    std::ostream & os, const json_object<json> & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_OBJECT_HPP
