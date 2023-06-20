#ifndef JSONIO_SRC_JSON_STRING_H
#define JSONIO_SRC_JSON_STRING_H

#include <iostream>
#include <string>
#include <variant>

namespace jsonio
{

enum class JsonType : std::size_t
{
    J_STRING,
    J_LONG,
    J_DOUBLE,
    J_BOOL,
    J_ARRAY,
    J_OBJECT,
    J_NULL,
};

class json_string : public std::string
{
private:
    unsigned int flags_;

private:
    static constexpr unsigned int
        PHASE_START = 0x0000,
        PHASE_TEXT = 0x0001,
        PHASE_COMPLETED = 0x0002,
        MASK_PHASE = 0x0003,
        ESCAPING = 0x0004,
        ESCAPED = 0x0008;

public:
    json_string() noexcept;
    json_string(const std::string& text) noexcept;
    json_string(std::string&& text) noexcept;
    json_string(const char* text) noexcept;
    json_string(const json_string& source) noexcept;
    json_string(json_string&& source) noexcept;
    json_string& operator=(const std::string& text) noexcept;
    json_string& operator=(std::string&& text) noexcept;
    json_string& operator=(const char* text) noexcept;
    json_string& operator=(const json_string& source) noexcept;
    json_string& operator=(json_string&& source) noexcept;
    ~json_string() noexcept;

    bool completed() const;

private:
    void check_escape();
    void read(std::istream& is);
    void write(std::ostream& os) const;

private:
    static char Unescape(const char source);
    static char Escape(const char source);

public:
    template<class> friend class json_object;
    friend class json;
};

}

template<>
class std::equal_to<jsonio::json_string>
{
public:
    bool operator()(const jsonio::json_string& l, const jsonio::json_string& r) const
    {
        return *static_cast<const std::string*>(&l) ==
            *static_cast<const std::string*>(&r);
    }
};

template<>
class std::hash<jsonio::json_string> : public std::hash<std::string>
{
public:
    std::size_t operator()(const jsonio::json_string& key) const
    {
        return std::hash<std::string>::operator()(
            *static_cast<const std::string*>(&key));
    }
};

#endif //JSONIO_JSON_STREAM_H
