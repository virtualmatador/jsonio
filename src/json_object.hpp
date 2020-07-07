#ifndef JSONIO_SRC_JSON_OBJECT_HPP
#define JSONIO_SRC_JSON_OBJECT_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "json_pair.hpp"

namespace jsonio
{

template<class json>
using JSON_OBJECT_PARENT = std::vector<json_pair<json>>;

template<class json>
class json_object : public JSON_OBJECT_PARENT<json>
{
private:
    using PARENT_TYPE = JSON_OBJECT_PARENT<json>;
    unsigned int flags_;
    std::unique_ptr<json_pair<json>> key_value_;

public:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_PAIR = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        SKIP_PREFIX = 0x0004;

public:
    json_object() noexcept
        : flags_{PHASE_START}
        , key_value_(std::make_unique<json_pair<json>>())
    {
    }

    json_object(const unsigned int flags) noexcept
        : flags_{flags}
        , key_value_(std::make_unique<json_pair<json>>())
    {
    }

    json_object(const json_object & source) noexcept
        : key_value_(std::make_unique<json_pair<json>>())
    {
        *this = source;
    }

    json_object(json_object && source) noexcept
        : key_value_(std::make_unique<json_pair<json>>())
    {
        *this = std::move(source);
    }

    json_object & operator=(const json_object & source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(*(PARENT_TYPE*)&source);
            flags_ = source.flags_;
            *key_value_ = *source.key_value_;
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
            *key_value_ = std::move(*source.key_value_);
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
        auto it = std::find_if(PARENT_TYPE::begin(), PARENT_TYPE::end(), [&](const json_pair<json> & key_value)
        {
            return key_value.first == key;
        });
        return it->second;
    }

    void read(std::istream & is)
    {
        if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
            flags_ = PHASE_START;
        if ((flags_ & MASK_PHASE) == PHASE_START)
        {
            PARENT_TYPE empty_parent;
            PARENT_TYPE::swap(empty_parent);
            key_value_->reset_flags();
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
                            PARENT_TYPE::operator=(PARENT_TYPE());
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
                size_t delimiter = key_value_->read(is, delimiters);
                if (is.good())
                {
                    if (!key_value_->is_empty())
                    {
                        PARENT_TYPE::push_back(std::move(*key_value_));
                    }
                    else if (PARENT_TYPE::size() != 0)
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

    void write(std::ostream & os, int indents) const
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

public:
    template<class> friend std::istream & operator>>(std::istream & is, json_object<json> & target);
    template<class> friend std::ostream & operator<<(std::ostream & os, const json_object<json> & source);
};

template<class json> std::istream & operator>>(std::istream & is, json_object<json> & target);
template<class json> std::ostream & operator<<(std::ostream & os, const json_object<json> & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_OBJECT_HPP
