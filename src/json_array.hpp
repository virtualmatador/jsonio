#ifndef JSONIO_SRC_JSON_ARRAY_HPP
#define JSONIO_SRC_JSON_ARRAY_HPP

#include <iostream>
#include <memory>
#include <vector>

namespace jsonio
{

template<class json>
class json_array : public std::vector<json>
{
private:
    using ARRAY_TYPE = std::vector<json>;
    unsigned int flags_;
    std::unique_ptr<json> value_;

public:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_VALUE = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        SKIP_PREFIX = 0x0004;

public:
    json_array() noexcept
        : flags_{PHASE_START}
        , value_{std::make_unique<json>()}
    {
    }

    json_array(const unsigned int flags) noexcept
        : flags_{flags}
        , value_{std::make_unique<json>()}
    {
    }

    json_array(const json_array & source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = source;
    }

    json_array(json_array && source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = std::move(source);
    }

    json_array & operator=(const json_array & source) noexcept
    {
        if (this != &source)
        {
            ARRAY_TYPE::operator=(*(ARRAY_TYPE*)&source);
            flags_ = source.flags_;
            *value_ = *source.value_;
        }
        return *this;
    }

    json_array & operator=(json_array && source) noexcept
    {
        if (this != &source)
        {
            ARRAY_TYPE::operator=(std::move(*(ARRAY_TYPE*)&source));
            flags_ = source.flags_;
            source.flags_ = PHASE_START;
            *value_ = std::move(*source.value_);
        }
        return *this;
    }

    ~json_array() noexcept
    {
    }
    
    bool completed() const
    {
        return (flags_ & MASK_PHASE) == PHASE_COMPLETED;
    }

    json & operator[](size_t index)
    {
        return const_cast<json &>(static_cast<const json_array &>(*this).operator[](index));
    }

    const json & operator[](size_t index) const
    {
        return ARRAY_TYPE::operator[](index);
    }

    void read(std::istream & is)
    {
        if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
            flags_ = PHASE_START;
        if ((flags_ & MASK_PHASE) == PHASE_START)
        {
            value_->flags_ = json::PHASE_START;
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
                        {
                            is.setstate(std::ios::iostate::_S_badbit);
                        }
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
                size_t delimiter = value_->read(is, delimiters);
                if (is.good())
                {
                    if ((value_->flags_ & json::EMPTY_VALUE) == 0)
                    {
                        ARRAY_TYPE::push_back(std::move(*value_));
                    }
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
                {
                    break;
                }
            }
        }
    }

    const void write(std::ostream & os, int indents) const
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
                    {
                        os << ',';
                    }
                    else
                    {
                        comma = true;
                    }
                    os << std::endl;
                    sub_JsonValue.write(os, indents + 1);
                }
            }
            os << std::endl;
            for (int i = 0; i < indents; ++i)
            {
                os << '\t';
            }
            os << "]";
        }
    }

public:
    template<class> friend std::istream & operator>>(std::istream & is, json_array<json> & target);
    template<class> friend std::ostream & operator<<(std::ostream & os, const json_array<json> & source);
};

template<class json> std::istream & operator>>(std::istream & is, json_array<json> & target);
template<class json> std::ostream & operator<<(std::ostream & os, const json_array<json> & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_ARRAY_HPP
