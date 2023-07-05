#ifndef JSONIO_SRC_JSON_OBJECT_HPP
#define JSONIO_SRC_JSON_OBJECT_HPP

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "json_string.h"

namespace jsonio
{

template<class json> class json_object;

template<class json>
class indexed_json : public json
{
public:
    indexed_json(const json& source, std::size_t index) noexcept
        : json{ source }
        , index_{ index }
    {
    }

    indexed_json(const json&& source, std::size_t index) noexcept
        : json{ std::move(source) }
        , index_{ index }
    {
    }

    bool operator==(const indexed_json& that) const
    {
        return json::operator==(that);
    }

    bool operator==(const json& that) const
    {
        return json::operator==(that);
    }

private:
    std::size_t index_;
    friend class json_object<json>;
};

template<class json>
using JSON_OBJECT_PARENT = std::unordered_map<json_string, indexed_json<json>>;

template<class json>
class json_object : public JSON_OBJECT_PARENT<json>
{
private:
    using PARENT_TYPE = JSON_OBJECT_PARENT<json>;
    unsigned int flags_;
    std::size_t next_index_;
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
    json_object(std::vector<std::pair<json_string, json>>&& init) noexcept
        : flags_{PHASE_COMPLETED}
        , next_index_{0}
        , value_{std::make_unique<json>()}
    {
        for (auto& [key, value] : init)
        {
            PARENT_TYPE::insert(
            {
                std::move(key),
                {
                    std::move(value),
                    next_index_++
                }
            });
        }
    }

    json_object() noexcept
        : flags_{PHASE_COMPLETED}
        , next_index_{0}
        , value_{std::make_unique<json>()}
    {
    }

    json_object(const unsigned int flags) noexcept
        : flags_{flags}
        , next_index_{0}
        , value_{std::make_unique<json>()}
    {
    }

    json_object(const json_object& source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = source;
    }

    json_object(json_object&& source) noexcept
        : value_{std::make_unique<json>()}
    {
        *this = std::move(source);
    }

    json_object& operator=(const json_object& source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(*(PARENT_TYPE*)&source);
            flags_ = source.flags_;
            next_index_ = source.next_index_;
            key_ = source.key_;
            *value_ = *source.value_;
        }
        return *this;
    }

    json_object& operator=(json_object&& source) noexcept
    {
        if (this != &source)
        {
            PARENT_TYPE::operator=(std::move(*(PARENT_TYPE*)&source));
            flags_ = source.flags_;
            source.flags_ = PHASE_START;
            next_index_ = source.next_index_;
            source.next_index_ = 0;
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

    json& operator[](const std::string& key)
    {
        auto pair = PARENT_TYPE::find(key);
        if (pair == PARENT_TYPE::end())
        {
            auto [node, inserted] = PARENT_TYPE::insert({key, {{}, next_index_}});
            if (inserted)
            {
                next_index_ += 1;
            }
            pair = node;
        }
        return pair->second;
    }

    const json& operator[](const std::string& key) const
    {
        auto pair = PARENT_TYPE::find(key);
        if (pair == PARENT_TYPE::end())
        {
            throw std::invalid_argument("jsonio::json_obj::operator[] " + key);
        }
        return pair->second;
    }

    json* at(const std::string& key)
    {
        return const_cast<json*>(
            static_cast<const json_object&>(*this).at(key));
    }

    const json* at(const std::string& key) const
    {
        if (auto it = PARENT_TYPE::find(key); it != PARENT_TYPE::end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void steal(const json_object& source, bool convert)
    {
        for (auto& [key, index_value] : *this)
        {
            auto source_node = source.find(key);
            if (source_node != source.end())
            {
                index_value.steal(source_node->second, convert);
            }
        }
    }

    void read(std::istream& is)
    {
        if ((flags_ & MASK_PHASE) == PHASE_COMPLETED)
        {
            flags_ = PHASE_START;
        }
        if ((flags_ & MASK_PHASE) == PHASE_START)
        {
            next_index_ = 0;
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
                    PARENT_TYPE::insert(
                    {
                        std::move(key_), { std::move(*value_), next_index_++ }
                    });
                }
                if (read_again)
                {
                    read(is);
                }
            }
        }
    }

    void write(std::ostream& os, int indents) const
    {
        if (completed())
        {
            if (!(os.flags() & std::ios_base::skipws))
            {
                for (int i = 0; i < indents; ++i)
                {
                    os << '\t';
                }
            }
            os << '{';
            bool comma = false;
            auto key_value_writer = [&](const json_string& key, const json& value)
            {
                if (comma)
                {
                    os << ',';
                }
                else
                {
                    comma = true;
                }
                if (!(os.flags() & std::ios_base::skipws))
                {
                    os << std::endl;
                    for (int i = 0; i < indents + 1; ++i)
                    {
                        os << '\t';
                    }
                }
                os << '\"' << key << "\":";
                if (value->index() == size_t(JsonType::J_ARRAY) ||
                    value->index() == size_t(JsonType::J_OBJECT))
                {
                    if (!(os.flags() & std::ios_base::skipws))
                    {
                        os << std::endl;
                    }
                    value.write(os, indents + 1);
                }
                else
                {
                    if (!(os.flags() & std::ios_base::skipws))
                    {
                        os << " ";
                    }
                    value.write(os, 0);
                }
            };
            if (os.flags() & std::ios_base::internal)
            {
                for (const auto& [key, index_value] : *this)
                {
                    key_value_writer(key, index_value);
                }
            }
            else
            {
                std::vector<typename PARENT_TYPE::const_iterator> nodes;
                nodes.reserve(PARENT_TYPE::size());
                for (auto it = PARENT_TYPE::begin(); it != PARENT_TYPE::end(); ++it)
                {
                    nodes.emplace_back(it);
                }
                if (os.flags() & std::ios_base::right)
                {
                    std::sort(nodes.begin(), nodes.end(), [](const auto& l, const auto& r)
                    {
                        return l->first < r->first;
                    });
                }
                else
                {
                    std::sort(nodes.begin(), nodes.end(), [](const auto& l, const auto& r)
                    {
                        return l->second.index_ < r->second.index_;
                    });
                }
                for (const auto& it : nodes)
                {
                    key_value_writer(it->first, it->second);
                }
            }
            if (!(os.flags() & std::ios_base::skipws))
            {
                os << std::endl;
                for (int i = 0; i < indents; ++i)
                {
                    os << '\t';
                }
            }
            os << '}';
        }
    }

public:
    template<class> friend std::istream& operator>>(
        std::istream& is, json_object<json>& target);
    template<class> friend std::ostream& operator<<(
        std::ostream& os, const json_object<json>& source);
};

template<class json>
std::istream& operator>>(std::istream& is, json_object<json>& target)
{
    target.read(is);
    return is;
}

template<class json>
std::ostream& operator<<(std::ostream& os, const json_object<json>& source)
{
    source.write(os, 0);
    return os;
}

} // namespace jsonio

#endif //JSONIO_SRC_JSON_OBJECT_HPP
